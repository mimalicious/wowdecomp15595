#include "KnowledgeBase.h"
#include <cstdlib>
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "WoW/Source/Ui/CKBQuery.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/FrameScript/FrameScript.h"

// STATIC: 0x00D17320/0x140BD2000
CKBSetup g_KnowledgeBaseSetup;

// STATIC: 0x00F76B60/0x140E6CC78
CKBPage* CKBPage::s_pLoadingQueue_Head;

// OFFSET: 0x009FB070/0x140798850
CKBPage::CKBPage() {
    this->m_state = 0;
    this->m_data = NULL;
    this->m_tree = NULL;
    this->m_loadingNext = NULL;
    this->m_inQueue = 0;
}

// OFFSET: 0x009FB110/0x140798930
bool CKBPage::LoadingCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    CKBPage* page = static_cast<CKBPage*>(param);

    if (!page) {
        return false;
    }

    page->m_lock.Enter();
    bool loading = page->m_state == 1;

    if (!done) {
        if (!data) {
            loading = false;
        }

        if (!size) {
            page->m_lock.Leave();
            return true;
        }
    }

    if (error || !loading) {
        if (error == 2) {
            page->m_state = 5;
        } else if (error == 3) {
            page->m_state = 6;
        } else {
            page->m_state = 4;
        }

        page->m_lock.Leave();
        return false;
    }

    if (!page->m_data) {
        page->m_data = new (SMemAlloc(sizeof(CDynamicString), __FILE__, __LINE__, 0)) CDynamicString();
        page->m_data->SetCapacity(0x8000);
    }

    if (!page->m_data) {
        page->m_state = 7;
        page->m_lock.Leave();
        return false;
    }

    page->m_data->Append(data, size);

    if (!done) {
        page->m_lock.Leave();
        return true;
    }

    if (page->m_tree) {
        XMLTree_Free(page->m_tree);
    }

    page->m_tree = XMLTree_Load(page->m_data->m_buffer, page->m_data->m_length);
    CDynamicString* buffer = page->m_data;

    if (buffer) {
        buffer->~CDynamicString();
        SMemFree(buffer, "delete", SLOG_FUNCTION, 0);
    }

    page->m_data = NULL;

    if (!page->m_tree) {
        page->m_state = 8;
        page->m_lock.Leave();
        return false;
    }

    page->_ComputeCachedData();
    page->m_state = 2;
    page->m_lock.Leave();
    return true;
}

// OFFSET: 0x009FB390/0x140798B70
int32_t CKBSetup::BeginLoading(int32_t numArticles, int32_t pageNumber) {
    if (this->m_state == 1 || this->m_inQueue) {
        return 0;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "getKBSetup.xml";
    url += "?languageId=";
    url += s_locale;
    url += "&numArticles=";
    url += numArticles;
    url += "&pageNumber=";
    url += pageNumber;
    url += "&locale=";
    url += s_localeCode;

    if (!OsURLDownload(url.m_buffer, &CKBPage::LoadingCallback, this)) {
        this->m_state = 3;
        this->_SendFailureEvent();
        return 0;
    }

    this->m_currentCategory = -1;
    this->m_languages.m_node = NULL;
    this->m_categories.m_node = NULL;
    this->m_subCategories.m_node = NULL;
    this->m_articleHeaders.m_node = NULL;
    this->m_state = 1;
    this->m_loadingNext = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// OFFSET: 0x009FC640/0x14079A180
CKBPage::~CKBPage() {
    if (this->m_tree) {
        XMLTree_Free(this->m_tree);
        XMLTree_Cleanup();
    }

    if (this->m_data) {
        CDynamicString* data = this->m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CKBPage::_ComputeCachedData() {
}

// OFFSET: 0x009FB0F0/0x140798900
int32_t CKBPage::_BeginLoading() {
    this->m_state = 1;
    this->m_loadingNext = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// OFFSET: 0x009FB2A0/0x140798AE0
XMLNode* CKBPage::_GetRoot(CKBIterator& iterator) {
    XMLNode* root = XMLTree_GetRoot(this->m_tree);

    if (!root) {
        return NULL;
    }

    iterator.m_node = root;
    iterator.m_index = 0;
    return root;
}

// OFFSET: [0x009FB2D0/0x1400120D0, 0x009FB590]
int32_t CKBPage::_GetInt(XMLNode* node, const char* attribute) const {
    if (this->m_state != 2 || !this->m_tree || !node) {
        return 0;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value ? atol(value) : 0;
}

// OFFSET: 0x009FB310/0x140798B50
const char* CKBPage::_GetBodyString(XMLNode* node) const {
    if (this->m_state != 2 || !this->m_tree) {
        return NULL;
    }

    return node->m_body;
}

// OFFSET: 0x009FB330
bool CKBPage::_GetBodyBool(const char* element, const char* child) const {
    const char* body = this->_GetBodyString(element, child);

    if (body) {
        return SStrCmpI(body, "true", STORM_MAX_STR) == 0;
    }

    return false;
}

// OFFSET: [0x009FB630, 0x009FB880]
bool CKBPage::_GetBool(XMLNode* node, const char* attribute) const {
    if (this->m_state != 2 || !this->m_tree || !node) {
        return false;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value && !SStrCmpI(value, "true", STORM_MAX_STR);
}

// OFFSET: 0x009FB680
int32_t CKBPage::_GetInt(const char* element, const char* attribute) const {
    if (this->m_state != 2 || !this->m_tree) {
        return 0;
    }

    XMLNode* root = XMLTree_GetRoot(this->m_tree);

    if (!root) {
        return 0;
    }

    XMLNode* node = root->GetChildByName(element);

    if (!node) {
        return 0;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value ? atol(value) : 0;
}

// INLINED
const char* CKBPage::_GetBodyString(const char* element, const char* child) const {
    if (this->m_state != 2 || !this->m_tree) {
        return NULL;
    }

    XMLNode* root = XMLTree_GetRoot(this->m_tree);

    if (!root) {
        return NULL;
    }

    XMLNode* node = root->GetChildByName(element);

    if (!node) {
        return NULL;
    }

    node = node->GetChildByName(child);
    return node ? node->m_body : NULL;
}

// OFFSET: [0x009FBAD0, 0x009FBB20]
int32_t CKBPage::_GetBodyInt(const char* element, const char* child) const {
    const char* body = this->_GetBodyString(element, child);
    return body ? atol(body) : 0;
}

// OFFSET: 0x009FB4D0
XMLNode* CKBSetup::GetLanguage(int32_t index) {
    if (this->m_state != 2 || !this->m_tree) {
        return NULL;
    }

    if (!this->m_languages.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_tree);

        if (root) {
            XMLNode* languages = root->GetChildByName("Languages");

            if (languages) {
                this->m_languages.m_node = languages->m_child;
                this->m_languages.m_index = 0;
            }
        }
    }

    this->m_languages.GoTo(index);
    return this->m_languages.m_node;
}

// OFFSET: 0x009FB530/0x140798D00
XMLNode* CKBSetup::GetCategoryNode(int32_t index) {
    if (this->m_state != 2 || !this->m_tree) {
        return NULL;
    }

    if (!this->m_categories.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_tree);

        if (root) {
            XMLNode* categories = root->GetChildByName("Categories");

            if (categories) {
                this->m_categories.m_node = categories->m_child;
                this->m_categories.m_index = 0;
            }
        }
    }

    this->m_categories.GoTo(index);
    return this->m_categories.m_node;
}

// OFFSET: 0x009FB5D0
XMLNode* CKBSetup::GetArticleHeader(int32_t index) {
    if (this->m_state != 2 || !this->m_tree) {
        return NULL;
    }

    if (!this->m_articleHeaders.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_tree);

        if (root) {
            XMLNode* headers = root->GetChildByName("ArticleHeaders");

            if (headers) {
                this->m_articleHeaders.m_node = headers->m_child;
                this->m_articleHeaders.m_index = 0;
            }
        }
    }

    this->m_articleHeaders.GoTo(index);
    return this->m_articleHeaders.m_node;
}

// OFFSET: 0x009FB6D0/0x140798D80
void CKBSetup::_ComputeCachedData() {
    XMLNode* node = NULL;
    XMLNode* root = XMLTree_GetRoot(this->m_tree);

    if (root) {
        XMLNode* languages = root->GetChildByName("Languages");

        if (languages) {
            node = languages->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numLanguages = 0; node; node = node->m_next) {
        this->m_numLanguages++;
    }

    root = XMLTree_GetRoot(this->m_tree);

    if (root) {
        XMLNode* categories = root->GetChildByName("Categories");

        if (categories) {
            node = categories->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numCategories = 0; node; node = node->m_next) {
        this->m_numCategories++;
    }

    this->m_numSubCategories = 0;
    root = XMLTree_GetRoot(this->m_tree);

    if (root) {
        XMLNode* headers = root->GetChildByName("ArticleHeaders");

        if (headers) {
            node = headers->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numArticleHeaders = 0; node; node = node->m_next) {
        this->m_numArticleHeaders++;
    }
}

// OFFSET: 0x009FB7B0/0x140798EB0
void CKBSetup::GetCategory(int32_t category) {
    this->m_subCategories.m_node = NULL;
    this->m_subCategories.m_index = 0;
    this->m_numSubCategories = 0;
    this->m_currentCategory = -1;

    XMLNode* node = this->GetCategoryNode(category);

    if (!node) {
        return;
    }

    XMLNode* subCategories = node->GetChildByName("SubCategories");

    if (!subCategories) {
        return;
    }

    XMLNode* child = subCategories->m_child;

    if (!child) {
        return;
    }

    this->m_currentCategory = category;
    this->m_subCategories.m_node = child;

    do {
        this->m_numSubCategories++;
        child = child->m_next;
    } while (child);
}

// OFFSET: 0x009FC6B0/0x14079A210
XMLNode* CKBSetup::GetSubCategory(int32_t category, int32_t subCategory) {
    if (this->m_state != 2 || !this->m_tree || category < 0) {
        return NULL;
    }

    if (this->m_currentCategory != category || !this->m_subCategories.m_node) {
        this->GetCategory(category);
    }

    this->m_subCategories.GoTo(subCategory);
    return this->m_subCategories.m_node;
}

// OFFSET: 0x009FC700/0x14079A270
void CKBSetup::_SendSuccessEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SETUP_LOAD_SUCCESS, NULL);
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CKBPage::_SendSuccessEvent() {
}

// OFFSET: 0x00401000/0x1407D7E80
void CKBPage::_SendFailureEvent() {
}

// INLINED
CKBSetup::CKBSetup() {
    this->m_numLanguages = 0;
    this->m_numCategories = 0;
    this->m_numSubCategories = 0;
    this->m_numArticleHeaders = 0;
    this->m_currentCategory = -1;
    this->m_languages.m_node = NULL;
    this->m_languages.m_index = 0;
    this->m_categories.m_node = NULL;
    this->m_categories.m_index = 0;
    this->m_subCategories.m_node = NULL;
    this->m_subCategories.m_index = 0;
    this->m_articleHeaders.m_node = NULL;
    this->m_articleHeaders.m_index = 0;
}

// OFFSET: 0x00401000/0x1407D7E80
void CKBPage::Initialize() {
}

// OFFSET: 0x009FAED0/0x140798640
const char* KnowledgeBaseGetURLForLocale() {
    switch (LocaleNameToIndex(s_localeCode)) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 11:
            return "http://support.wow-europe.com/kb/";

        case 5:
            return "http://support.worldofwarcraft.co.kr/kb/";

        case 6:
        case 8:
            return "http://cn.kbase.blizzard.com/kb/wow/";

        case 7:
        case 9:
            return "http://support.wowtaiwan.com.tw/kb/";

        default:
            return "http://support.worldofwarcraft.com/kb/";
    }
}

// OFFSET: 0x009FB010/0x1407987D0
void CKBPage::UpdateLoadingQueue() {
    CKBPage* prev = NULL;
    CKBPage* page = CKBPage::s_pLoadingQueue_Head;

    while (page) {
        if (page->m_state == 1) {
            prev = page;
            page = page->m_loadingNext;
            continue;
        }

        if (prev) {
            prev->m_loadingNext = page->m_loadingNext;
        } else {
            CKBPage::s_pLoadingQueue_Head = page->m_loadingNext;
        }

        if (page->m_state == 2) {
            page->_SendSuccessEvent();
        } else {
            page->_SendFailureEvent();
        }

        CKBPage* next = page->m_loadingNext;
        page->m_inQueue = 0;
        page->m_loadingNext = NULL;
        page = next;
    }
}

// OFFSET: 0x009FCD10/0x14079AA00
void CKBPage::Shutdown() {
    while (CKBPage::s_pLoadingQueue_Head) {
        OsSleep(1);
        CKBPage::UpdateLoadingQueue();
    }

    if (g_KnowledgeBaseSetup.m_tree) {
        XMLTree_Free(g_KnowledgeBaseSetup.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseSetup.m_tree = NULL;
    }

    if (g_KnowledgeBaseSetup.m_data) {
        CDynamicString* data = g_KnowledgeBaseSetup.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseSetup.m_data = NULL;
    }

    g_KnowledgeBaseSetup.m_state = 0;

    if (g_KnowledgeBaseQuery.m_tree) {
        XMLTree_Free(g_KnowledgeBaseQuery.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseQuery.m_tree = NULL;
    }

    if (g_KnowledgeBaseQuery.m_data) {
        CDynamicString* data = g_KnowledgeBaseQuery.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseQuery.m_data = NULL;
    }

    g_KnowledgeBaseQuery.m_state = 0;

    if (g_KnowledgeBaseArticle.m_tree) {
        XMLTree_Free(g_KnowledgeBaseArticle.m_tree);
        XMLTree_Cleanup();
        g_KnowledgeBaseArticle.m_tree = NULL;
    }

    if (g_KnowledgeBaseArticle.m_data) {
        CDynamicString* data = g_KnowledgeBaseArticle.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseArticle.m_data = NULL;
    }

    g_KnowledgeBaseArticle.m_state = 0;
}

// OFFSET: 0x009FC720/0x14079A290
void CKBSetup::_SendFailureEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SETUP_LOAD_FAILURE, NULL);
    }
}
