#include "WoW/Source/JamClientAutoCode/JamWorld.h"
#include "Engine/Source/Base/CDataStore.h"

// OFFSET: 0x00673870/0x14031BD10
WorldRequestGuildRewardsList::WorldRequestGuildRewardsList(int32_t currentVersion) {
    this->m_currentVersion = currentVersion;
}

// OFFSET: 0x00670EF0/0x140319290
WorldRequestGuildRewardsList::~WorldRequestGuildRewardsList() {
}

// OFFSET: 0x00673B70/0x14031C0A0
void WorldRequestGuildRewardsList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3012));
    msg->Put(static_cast<uint32_t>(this->m_currentVersion));
}
