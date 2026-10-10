#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYER_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
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

class PlayerSetCurrencyFlags : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_type;
    /* 0014 */ uint32_t m_flags;

    void Put(CDataStore* msg) override;

    PlayerSetCurrencyFlags();
    ~PlayerSetCurrencyFlags();
};

class PlayerLootCurrency : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_lootListID;

    void Put(CDataStore* msg) override;

    PlayerLootCurrency();
    ~PlayerLootCurrency();
};

class PlayerSetPetSlot : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_stableMaster[8];
    /* 0018 */ uint32_t m_petNumber;
    /* 001C */ uint8_t m_destSlot;

    void Put(CDataStore* msg) override;

    PlayerSetPetSlot();
    ~PlayerSetPetSlot();
};

class PlayerQueryQuestCompletionNPCs : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<int32_t> m_questIds;

    void Put(CDataStore* msg) override;

    PlayerQueryQuestCompletionNPCs();
    ~PlayerQueryQuestCompletionNPCs();
};

#endif
