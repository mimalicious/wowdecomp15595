#include "GameObjectStats.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"

// OFFSET: 0x0049B170/0x1400C6F50
GameObjectStats::~GameObjectStats() {
    for (int32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            SMemFree(this->m_name[i], __FILE__, __LINE__, 0);
        }
    }

    if (this->m_iconName) {
        SMemFree(this->m_iconName, __FILE__, __LINE__, 0);
    }

    if (this->m_castBarCaption) {
        SMemFree(this->m_castBarCaption, __FILE__, __LINE__, 0);
    }

    if (this->m_closeText) {
        SMemFree(this->m_closeText, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x00554D80
const char* GameObjectStats::GetName(int32_t index) {
    const char* name = this->m_name[index];

    if (!name || !*name) {
        return this->m_name[0];
    }

    return name;
}

// OFFSET: 0x00A28BA0/0x1407D0780
void GameObjectStats::Pack(CDataStore* msg) {
    msg->Put(this->m_typeID);
    msg->Put(this->m_displayID);

    for (int32_t i = 0; i < 4; i++) {
        if (this->m_name[i]) {
            msg->PutString(this->m_name[i]);
        } else {
            msg->PutString("");
        }
    }

    msg->PutString(this->m_iconName);
    msg->PutString(this->m_castBarCaption);
    msg->PutString(this->m_closeText);

    for (int32_t i = 0; i < 32; i++) {
        msg->Put(this->m_propValue[i]);
    }

    msg->Put(this->m_scale);

    for (int32_t i = 0; i < 6; i++) {
        msg->Put(this->m_questItems[i]);
    }

    msg->Put(this->m_requiredLevel);
}

// OFFSET: 0x00A28C80/0x1407D0880
void GameObjectStats_C::Unpack(CDataStore* msg) {
    char buffer[1024];

    msg->Get(this->m_typeID);
    msg->Get(this->m_displayID);

    for (int32_t i = 0; i < 4; i++) {
        msg->GetString(buffer, sizeof(buffer));

        if (buffer[0]) {
            this->m_name[i] = SStrDupA(buffer, __FILE__, __LINE__);
        } else {
            this->m_name[i] = NULL;
        }
    }

    msg->GetString(buffer, sizeof(buffer));
    this->m_iconName = SStrDupA(buffer, __FILE__, __LINE__);
    msg->GetString(buffer, sizeof(buffer));
    this->m_castBarCaption = SStrDupA(buffer, __FILE__, __LINE__);
    msg->GetString(buffer, sizeof(buffer));
    this->m_closeText = SStrDupA(buffer, __FILE__, __LINE__);

    for (int32_t i = 0; i < 32; i++) {
        msg->Get(this->m_propValue[i]);
    }

    msg->Get(this->m_scale);

    for (int32_t i = 0; i < 6; i++) {
        msg->Get(this->m_questItems[i]);
    }

    msg->Get(this->m_requiredLevel);
}
