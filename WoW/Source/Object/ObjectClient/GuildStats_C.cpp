#include "GuildStats_C.h"
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: a7ac1d60 IDENTICAL/IDENTICAL
// OFFSET: 0x00A28FE0/0x1407D0C70
void GuildStats_C::Pack(CDataStore* msg) {
    msg->Put(this->m_guildGuid);
    msg->PutString(this->m_name);

    for (uint32_t i = 0; i < 10; i++) {
        msg->PutString(this->m_rankNames[i]);
    }

    for (uint32_t i = 0; i < 10; i++) {
        msg->Put(static_cast<uint32_t>(this->m_rankIds[i]));
    }

    for (uint32_t i = 0; i < 10; i++) {
        msg->Put(static_cast<uint32_t>(this->m_rankOrders[i]));
    }

    msg->Put(static_cast<uint32_t>(this->m_emblemStyle));
    msg->Put(static_cast<uint32_t>(this->m_emblemColor));
    msg->Put(static_cast<uint32_t>(this->m_borderStyle));
    msg->Put(static_cast<uint32_t>(this->m_borderColor));
    msg->Put(static_cast<uint32_t>(this->m_background));
    msg->Put(static_cast<uint32_t>(this->m_numRanks));
}

// PROVEN: a6dd9b72 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAD50/0x14027FF50
int32_t GuildStats_C::GetRankIndex(int32_t rankId) {
    for (int32_t i = 0; i < this->m_numRanks; i++) {
        if (this->m_rankIds[i] == rankId) {
            return i;
        }
    }

    return -1;
}

// PROVEN: 9bd6c1da IDENTICAL/IDENTICAL
// OFFSET: 0x005FAD90/0x14027FF80
void GuildStats_C::Unpack(CDataStore* msg) {
    msg->Get(this->m_guildGuid);
    msg->GetString(this->m_name, sizeof(this->m_name));

    for (uint32_t i = 0; i < 10; i++) {
        msg->GetString(this->m_rankNames[i], sizeof(this->m_rankNames[i]));
    }

    for (uint32_t i = 0; i < 10; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(this->m_rankIds[i]));
    }

    for (uint32_t i = 0; i < 10; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(this->m_rankOrders[i]));
    }

    msg->Get(reinterpret_cast<uint32_t&>(this->m_emblemStyle));
    msg->Get(reinterpret_cast<uint32_t&>(this->m_emblemColor));
    msg->Get(reinterpret_cast<uint32_t&>(this->m_borderStyle));
    msg->Get(reinterpret_cast<uint32_t&>(this->m_borderColor));
    msg->Get(reinterpret_cast<uint32_t&>(this->m_background));
    msg->Get(reinterpret_cast<uint32_t&>(this->m_numRanks));
}
