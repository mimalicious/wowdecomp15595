#include "CreatureStats.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 6c18e375 IDENTICAL/NONE
// OFFSET: 0x00464860
const char* CreatureStats::GetName(int32_t index) const {
    const char* name = this->m_name[index];

    if (!name || !*name) {
        return this->m_name[0];
    }

    return name;
}

// PROVEN: 32694511 IDENTICAL/NONE
// OFFSET: 0x00A287E0
void CreatureStats::Pack(CDataStore* msg) const {
    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            msg->PutString(this->m_name[i]);
        } else {
            msg->PutString("");
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_nameAlt[i]) {
            msg->PutString(this->m_nameAlt[i]);
        } else {
            msg->PutString("");
        }
    }

    msg->PutString(this->m_subName);
    msg->PutString(this->m_iconName);
    for (uint32_t i = 0; i < 2; i++) {
        msg->Put(this->m_typeFlags[i]);
    }
    msg->Put(this->m_type);
    msg->Put(this->m_family);
    msg->Put(this->m_rank);
    msg->Put(this->m_killCredit[0]);
    msg->Put(this->m_killCredit[1]);
    msg->Put(this->m_displayId[0]);
    msg->Put(this->m_displayId[1]);
    msg->Put(this->m_displayId[2]);
    msg->Put(this->m_displayId[3]);
    msg->Put(this->m_hpModifier);
    msg->Put(this->m_mpModifier);
    msg->Put(static_cast<uint8_t>(this->m_racialLeader != 0));

    for (uint32_t i = 0; i < 6; i++) {
        msg->Put(this->m_questItem[i]);
    }

    msg->Put(this->m_movementId);
    msg->Put(this->m_expansion);
}

// PROVEN: 03835d8d IDENTICAL/NONE
// OFFSET: 0x00A28950/0x1407D04C0
void CreatureStats::Unpack(CDataStore* msg) {
    char buffer[1024];

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            SMemFree(this->m_name[i], __FILE__, __LINE__, 0);
        }

        msg->GetString(buffer, sizeof(buffer));

        if (buffer[0]) {
            this->m_name[i] = SStrDupA(buffer, __FILE__, __LINE__);
        } else {
            this->m_name[i] = NULL;
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_nameAlt[i]) {
            SMemFree(this->m_nameAlt[i], __FILE__, __LINE__, 0);
        }

        msg->GetString(buffer, sizeof(buffer));

        if (buffer[0]) {
            this->m_nameAlt[i] = SStrDupA(buffer, __FILE__, __LINE__);
        } else {
            this->m_nameAlt[i] = NULL;
        }
    }

    if (this->m_subName) {
        SMemFree(this->m_subName, __FILE__, __LINE__, 0);
    }

    msg->GetString(buffer, sizeof(buffer));
    this->m_subName = SStrDupA(buffer, __FILE__, __LINE__);

    if (this->m_iconName) {
        SMemFree(this->m_iconName, __FILE__, __LINE__, 0);
    }

    msg->GetString(buffer, sizeof(buffer));
    this->m_iconName = SStrDupA(buffer, __FILE__, __LINE__);

    for (uint32_t i = 0; i < 2; i++) {
        msg->Get(this->m_typeFlags[i]);
    }
    msg->Get(this->m_type);
    msg->Get(this->m_family);
    msg->Get(this->m_rank);
    msg->Get(this->m_killCredit[0]);
    msg->Get(this->m_killCredit[1]);
    msg->Get(this->m_displayId[0]);
    msg->Get(this->m_displayId[1]);
    msg->Get(this->m_displayId[2]);
    msg->Get(this->m_displayId[3]);
    msg->Get(this->m_hpModifier);
    msg->Get(this->m_mpModifier);

    uint8_t racialLeader;
    msg->Get(racialLeader);
    this->m_racialLeader = racialLeader != 0;

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(this->m_questItem[i]);
    }

    msg->Get(this->m_movementId);
    msg->Get(this->m_expansion);
}
