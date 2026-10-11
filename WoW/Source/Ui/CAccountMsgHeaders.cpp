#include "CAccountMsgHeaders.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F76AB8
CAccountMsgHeaders g_accountMsgHeaders;

// PROVEN: 8cf57339 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
static bool AccountMsgReadCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    return false;
}

// PROVEN: 73c0db07 IDENTICAL/NONE
// OFFSET: 0x009FA090/0x140797550
void CAccountMsgHeaders::_SendFailureEvent() {
    this->m_iterator.m_node = NULL;
    this->m_count = 0;
}

// PROVEN: 44c5b11c IDENTICAL/NONE
// OFFSET: 0x009FA0A0
const char* CAccountMsgHeaders::GetHeaderSubject(int32_t index) {
    if (this->m_state == 1 || index < 0 || index >= this->m_count) {
        return NULL;
    }

    this->m_iterator.GoTo(index);
    CKBIterator subject = this->m_iterator;
    subject.GoToChild("subject");
    return this->_GetBodyString(subject.m_node);
}

// OFFSET: 0x009FA100/0x140798B10
int32_t CAccountMsgHeaders::GetHeaderPriority(int32_t index) {
    if (this->m_state == 1 || index < 0 || index >= this->m_count) {
        return 0;
    }

    this->m_iterator.GoTo(index);
    return this->_GetInt(this->m_iterator.m_node, "priority");
}

// OFFSET: 0x009FA140/0x140797560
int32_t CAccountMsgHeaders::GetMessageId(int32_t index) {
    if (this->m_state == 1) {
        return 0;
    }

    if (index < 0 || index >= this->m_count) {
        return 0;
    }

    if (!this->m_iterator.m_node) {
        this->_GetRoot(this->m_iterator);
        this->m_iterator.GoToFirstChild();
    }

    this->m_iterator.GoTo(index);
    return this->_GetInt(this->m_iterator.m_node, "id");
}

// OFFSET: 0x009FA410/0x140797910
int32_t CAccountMsgHeaders::GetNumUnreadUrgentMsgs() {
    int32_t count = 0;

    for (int32_t i = 0; i < this->m_count; i++) {
        if (!this->m_opened.m_data[i] && this->GetHeaderPriority(i) >= 3) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x009FA480/0x1407979B0
int32_t CAccountMsgHeaders::GetIndexHighestPriorityUnreadMsg() {
    if (this->m_state == 1) {
        return 0;
    }

    int32_t index = 0;

    for (int32_t i = 0; i < this->m_count; i++) {
        bool read = i >= 0 && i < this->m_count && this->m_opened.m_data[i];

        if (!read && this->GetHeaderPriority(i) >= 3) {
            index = i;
        }
    }

    return index;
}

// OFFSET: 0x009FA500
int32_t CAccountMsgHeaders::GetIndexNextUnreadMsg(int32_t index) {
    if (this->m_state == 1) {
        return -1;
    }

    for (int32_t i = 0; i < this->m_count; i++) {
        if (i > index) {
            bool read = i >= 0 && i < this->m_count && this->m_opened.m_data[i];

            if (!read) {
                return i;
            }
        }
    }

    return -1;
}

// OFFSET: 0x009FA6D0/0x140797C60
int32_t CAccountMsgHeaders::SetMsgRead(int32_t index) {
    int32_t messageId = this->GetMessageId(index);

    char accountName[1280];
    accountName[0] = '\0';
    SStrCopy(accountName, ClientServices::GetAccountName(), sizeof(accountName));

    uint8_t* sessionKey = ClientServices::Connection()->GetSessionKey();
    SHA1_CONTEXT context;
    uint8_t digest[20];
    SHA1_Init(&context);
    SHA1_Update(&context, sessionKey, 40);
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>("\x0Bz"), 2);
    SHA1_Final(digest, &context);

    CDynamicString url;
    url.SetCapacity(1024);
    url = FrameScript_GetText("ACCOUNT_MESSAGE_READ_URL", -1, GENDER_NOT_APPLICABLE);
    url += "?accountName=";
    url += accountName;
    url += "&sessionKeyHash=";

    for (int32_t i = 0; i < 20; i++) {
        url += static_cast<int32_t>(digest[i]);
    }

    url += "&messageId=";
    url += messageId;

    if (!OsURLDownload(url.m_buffer, &AccountMsgReadCallback, NULL)) {
        return 0;
    }

    if (index >= 0 && index < this->m_count) {
        this->m_opened.m_data[index] = 1;
    }

    return 1;
}

// INLINED
CAccountMsgHeaders::~CAccountMsgHeaders() {
}

// OFFSET: 0x009FAC20/0x1407982F0
int32_t CAccountMsgHeaders::LoadHeaders() {
    if (this->m_state == 1 || this->m_inQueue) {
        return 0;
    }

    char accountName[1280];
    accountName[0] = '\0';
    SStrCopy(accountName, ClientServices::GetAccountName(), sizeof(accountName));

    uint8_t* sessionKey = ClientServices::Connection()->GetSessionKey();
    SHA1_CONTEXT context;
    uint8_t digest[20];
    SHA1_Init(&context);
    SHA1_Update(&context, sessionKey, 40);
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>("\x0Bz"), 2);
    SHA1_Final(digest, &context);

    CDynamicString url;
    url.SetCapacity(1024);
    url = FrameScript_GetText("ACCOUNT_MESSAGE_HEADERS_URL", -1, GENDER_NOT_APPLICABLE);
    url += "?accountName=";
    url += accountName;
    url += "&sessionKeyHash=";

    for (int32_t i = 0; i < 20; i++) {
        url += static_cast<int32_t>(digest[i]);
    }

    if (!OsURLDownload(url.m_buffer, &CKBPage::LoadingCallback, this)) {
        this->m_state = 3;
        return 0;
    }

    this->m_iterator.m_node = NULL;
    this->m_count = 0;
    this->m_opened.Clear();
    this->_BeginLoading();
    return 1;
}

// OFFSET: 0x009FADD0/0x140798530
void CAccountMsgHeaders::_SendSuccessEvent() {
    if (!this->m_iterator.m_node) {
        this->_GetRoot(this->m_iterator);
        this->m_iterator.GoToFirstChild();
    }

    CKBIterator iterator = this->m_iterator;
    this->m_count = 0;
    iterator.GoToFirst();

    while (iterator.m_node) {
        this->m_count++;
        iterator.GoToNext();
    }

    this->m_opened.SetCount(this->m_count);

    iterator = this->m_iterator;
    iterator.GoToFirst();

    for (int32_t i = 0; i < this->m_count; i++) {
        iterator.GoTo(i);
        this->m_opened.m_data[i] = this->_GetInt(iterator.m_node, "opened");
    }

    if (CGlueMgr::AreScriptFunctionsLoaded()) {
        FrameScript_SignalEvent(GLUE_EVENT_ACCOUNT_MESSAGES_HEADERS_LOADED, NULL);
    }
}
