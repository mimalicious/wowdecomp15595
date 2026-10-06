#include "ArenaTeamCache.h"
#include "Engine/Source/Base/CDataStore.h"

// OFFSET: %0x00964530
void ArenaTeamCache::Pack(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(this->m_id));
    msg->PutString(this->m_name);
    msg->Put(this->m_type);
    msg->Put(this->m_backgroundColor);
    msg->Put(this->m_emblemStyle);
    msg->Put(this->m_emblemColor);
    msg->Put(this->m_borderStyle);
    msg->Put(this->m_borderColor);
}

// OFFSET: %0x009645A0
void ArenaTeamCache::Unpack(CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(this->m_id));
    msg->GetString(this->m_name, sizeof(this->m_name));
    msg->Get(this->m_type);
    msg->Get(this->m_backgroundColor);
    msg->Get(this->m_emblemStyle);
    msg->Get(this->m_emblemColor);
    msg->Get(this->m_borderStyle);
    msg->Get(this->m_borderColor);
}
