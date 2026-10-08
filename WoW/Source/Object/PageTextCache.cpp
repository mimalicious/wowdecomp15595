#include "PageTextCache.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0049B320/0x1400C7000
PageTextCache::~PageTextCache() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }
}

// PROVEN: 15ea4fd7 IDENTICAL/NONE
// OFFSET: 0x00A290C0/0x1407D0D80
void PageTextCache::Pack(CDataStore* msg) {
    if (this->m_text) {
        msg->PutString(this->m_text);
    } else {
        msg->PutString("");
    }

    msg->Put(this->m_nextPage);
}

// PROVEN: cb0bc2da IDENTICAL/NONE
// OFFSET: 0x00A29100/0x1407D0DC0
void PageTextCache_C::Unpack(CDataStore* msg) {
    char buffer[4000];

    msg->GetString(buffer, sizeof(buffer));
    this->m_text = SStrDupA(buffer, __FILE__, __LINE__);
    msg->Get(this->m_nextPage);
}
