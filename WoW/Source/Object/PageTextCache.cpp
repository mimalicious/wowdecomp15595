#include "PageTextCache.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: #0x00668550
PageTextCache::~PageTextCache() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x00964F50
void PageTextCache::Pack(CDataStore* msg) {
    if (this->m_text) {
        msg->PutString(this->m_text);
    } else {
        msg->PutString("");
    }

    msg->Put(this->m_nextPage);
}

// OFFSET: %0x00964F90
void PageTextCache_C::Unpack(CDataStore* msg) {
    char buffer[4000];

    msg->GetString(buffer, sizeof(buffer));
    this->m_text = SStrDupA(buffer, __FILE__, __LINE__);
    msg->Get(this->m_nextPage);
}
