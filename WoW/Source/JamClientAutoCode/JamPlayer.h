#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYER_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYER_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

class PlayerObjectUpdateFailed : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guid[8];

    void Put(CDataStore* msg) override;

    PlayerObjectUpdateFailed();
    ~PlayerObjectUpdateFailed();
};

class PlayerObjectUpdateRescued : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guid[8];

    void Put(CDataStore* msg) override;

    PlayerObjectUpdateRescued();
    ~PlayerObjectUpdateRescued();
};

#endif
