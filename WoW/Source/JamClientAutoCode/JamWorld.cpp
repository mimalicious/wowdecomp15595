#include "WoW/Source/JamClientAutoCode/JamWorld.h"
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: a5273747 IDENTICAL/DIFFERENT
// OFFSET: 0x00673870/0x14031BD10
WorldRequestGuildRewardsList::WorldRequestGuildRewardsList(int32_t currentVersion) {
    this->m_currentVersion = currentVersion;
}

// PROVEN: e9dbc23e IDENTICAL/IDENTICAL
// OFFSET: 0x00670EF0/0x140319290
WorldRequestGuildRewardsList::~WorldRequestGuildRewardsList() {
}

// PROVEN: 79a5c22c IDENTICAL/IDENTICAL
// OFFSET: 0x00673B70/0x14031C0A0
void WorldRequestGuildRewardsList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3012));
    msg->Put(static_cast<uint32_t>(this->m_currentVersion));
}
