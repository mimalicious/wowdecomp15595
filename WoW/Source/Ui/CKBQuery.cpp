#include "CKBQuery.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00D17398
CKBQuery g_KnowledgeBaseQuery;

// STATIC: 0x00D17258
static char s_hexDigits[] = "0123456789ABCDEF";

// INLINED
CKBQuery::CKBQuery() {
    this->m_numArticleHeaders = 0;
    this->m_articleHeaders.m_node = NULL;
    this->m_articleHeaders.m_index = 0;
}

// OFFSET: 0x009FB820
XMLNode* CKBQuery::GetArticleHeader(int32_t index) {
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

// OFFSET: 0x009FB8D0
void CKBQuery::_ComputeCachedData() {
    XMLNode* node = XMLTree_GetRoot(this->m_tree);

    if (node) {
        node = node->GetChildByName("ArticleHeaders");

        if (node) {
            node = node->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numArticleHeaders = 0; node; node = node->m_next) {
        this->m_numArticleHeaders++;
    }
}

// OFFSET: 0x009FB920
void CKBQuery::_AppendMangledSearchQuery(CDynamicString& url, const char* query) {
    for (uint8_t c = *query; c; c = *++query) {
        if (IsAlphaEnglish(c) || IsNumeric(c)) {
            url += *query;
        } else {
            url += '%';
            url += s_hexDigits[c >> 4];
            url += s_hexDigits[c & 0xF];
        }
    }
}

// OFFSET: 0x009FC740
int32_t CKBQuery::BeginLoading(const char* query, int32_t categoryId, int32_t numArticles, int32_t pageNumber) {
    if (this->m_state == 1 || this->m_inQueue) {
        return 0;
    }

    if (!*query) {
        query = NULL;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "sendKBQuery.xml";
    url += '?';

    if (query) {
        url += "searchQuery=";
        CKBQuery::_AppendMangledSearchQuery(url, query);

        if (categoryId != -1) {
            url += '&';
            url += "categoryId=";
            url += categoryId;
        }

        url += '&';
    } else if (categoryId != -1) {
        url += "categoryId=";
        url += categoryId;
        url += '&';
    }

    url += "languageId=";
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

    this->m_articleHeaders.m_node = NULL;
    this->m_state = 1;
    this->m_loadingNext = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// OFFSET: 0x009FC900
void CKBQuery::_SendSuccessEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_QUERY_LOAD_SUCCESS, NULL);
    }
}

// OFFSET: 0x009FC920
void CKBQuery::_SendFailureEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_QUERY_LOAD_FAILURE, NULL);
    }
}
