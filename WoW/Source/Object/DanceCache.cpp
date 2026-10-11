#include "DanceCache.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
DanceCache::DanceCache() {
    this->m_id = 0;
    this->m_guid = WOWGUID();
    this->m_name[0] = '\0';
    this->m_data = NULL;
    this->m_dataCount = 0;
    this->unk_0098 = 0;
}

// INLINED
DanceCache::~DanceCache() {
    delete[] this->m_data;
}

// OFFSET: 0x0049BD60/0x1400C7C30
DanceCache& DanceCache::operator=(const DanceCache& source) {
    this->m_id = source.m_id;
    this->m_guid = source.m_guid;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));

    if (this->m_dataCount < source.m_dataCount) {
        if (this->m_data) {
            SMemReAlloc(this->m_data, source.m_dataCount, __FILE__, __LINE__, 0);
        } else {
            this->m_data = new (__FILE__, __LINE__) DANCEMOVE[source.m_dataCount];
        }
    }

    memcpy(this->m_data, source.m_data, 4 * source.m_dataCount);
    this->m_dataCount = source.m_dataCount;
    this->unk_0098 = source.unk_0098;

    return *this;
}

// PROVEN: f5394072 IDENTICAL/NONE
// OFFSET: 0x00A286D0
void DanceCache::Pack(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(this->m_id));
    *msg << static_cast<const SmartGUID&>(this->m_guid);
    msg->PutString(this->m_name);
    msg->Put(this->m_dataCount);
    msg->PutData(this->m_data, 4 * this->m_dataCount);
    msg->Put(this->unk_0098);
}

// OFFSET: 0x00A28740/0x1407D0240
void DanceCache::Unpack(CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(this->m_id));

    if (this->m_id < 0) {
        return;
    }

    *msg >> static_cast<SmartGUID&>(this->m_guid);
    msg->GetString(this->m_name, sizeof(this->m_name));
    msg->Get(this->m_dataCount);
    this->m_data = new (__FILE__, __LINE__) DANCEMOVE[this->m_dataCount];
    msg->GetData(this->m_data, 4 * this->m_dataCount);
    msg->Get(this->unk_0098);
}
