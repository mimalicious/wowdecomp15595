#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMWORLD_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMWORLD_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

class WorldRequestGuildRewardsList : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_currentVersion;

    void Put(CDataStore* msg) override;

    WorldRequestGuildRewardsList(int32_t currentVersion);
    ~WorldRequestGuildRewardsList();
};

#endif
