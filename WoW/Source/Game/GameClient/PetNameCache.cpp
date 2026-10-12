#include "PetNameCache.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0049BB50/0x1400C79B0
PetNameCache& PetNameCache::operator=(const PetNameCache& source) {
    this->m_petNumber = source.m_petNumber;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));

    if (source.m_declinedNames) {
        if (!this->m_declinedNames) {
            this->m_declinedNames = static_cast<PetNameCache::DeclinedNames*>(SMemAlloc(sizeof(PetNameCache::DeclinedNames), __FILE__, __LINE__, 0));
        }

        for (uint32_t i = 0; i < 5; i++) {
            SStrCopy(this->m_declinedNames->m_names[i], source.m_declinedNames->m_names[i], sizeof(this->m_declinedNames->m_names[i]));
        }
    }

    this->m_timestamp = source.m_timestamp;

    return *this;
}

// OFFSET: 0x004EB900/0x14012EA50
void PetNameCache::Pack(CDataStore* msg) {
    msg->Put(this->m_petNumber);
    msg->PutString(this->m_name);

    if (this->m_declinedNames) {
        msg->Put(static_cast<uint8_t>(1));

        for (uint32_t i = 0; i < 5; i++) {
            msg->PutString(this->m_declinedNames->m_names[i]);
        }
    } else {
        msg->Put(static_cast<uint8_t>(0));
    }

    msg->Put(static_cast<uint32_t>(this->m_timestamp));
}

// OFFSET: 0x004EB980/0x14012EAF0
void PetNameCache::Unpack(CDataStore* msg) {
    uint8_t hasDeclinedNames;
    msg->Get(this->m_petNumber);
    msg->GetString(this->m_name, sizeof(this->m_name));
    msg->Get(hasDeclinedNames);

    if (hasDeclinedNames) {
        if (!this->m_declinedNames) {
            this->m_declinedNames = static_cast<PetNameCache::DeclinedNames*>(SMemAlloc(sizeof(PetNameCache::DeclinedNames), __FILE__, __LINE__, 0));
        }

        for (uint32_t i = 0; i < 5; i++) {
            msg->GetString(this->m_declinedNames->m_names[i], sizeof(this->m_declinedNames->m_names[i]));
        }
    }

    uint32_t timestamp = 0;
    msg->Get(timestamp);
    this->m_timestamp = timestamp;
}
