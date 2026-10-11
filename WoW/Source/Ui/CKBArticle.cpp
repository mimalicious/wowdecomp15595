#include "CKBArticle.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00D173E8
CKBArticle g_KnowledgeBaseArticle;

// INLINED
CKBArticle::CKBArticle() {
}

// PROVEN: 431409c4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CKBArticle::_ComputeCachedData() {
}

// PROVEN: 52ec8010 IDENTICAL/NONE
// OFFSET: 0x009FB9B0/0x140799050
int32_t CKBArticle::BeginLoading(int32_t articleId, int32_t searchType) {
    if (this->m_state == 1 || this->m_inQueue) {
        return 0;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "sendKBArticleQuery.xml";
    url += "?articleId=";
    url += articleId;
    url += "&languageId=";
    url += s_locale;
    url += "&searchType=";
    url += searchType;
    url += "&locale=";
    url += s_localeCode;

    if (!OsURLDownload(url.m_buffer, &CKBPage::LoadingCallback, this)) {
        this->m_state = 3;
        this->_SendFailureEvent();
        return 0;
    }

    this->m_state = 1;
    this->m_loadingNext = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// OFFSET: 0x009FC940/0x14079A550
void CKBArticle::_SendSuccessEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_ARTICLE_LOAD_SUCCESS, NULL);
    }
}

// OFFSET: 0x009FC960/0x14079A570
void CKBArticle::_SendFailureEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_ARTICLE_LOAD_FAILURE, NULL);
    }
}
