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

class PlayerCliCancelTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerCliCancelTrade();
    ~PlayerCliCancelTrade();
};

class PlayerCliBeginTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerCliBeginTrade();
    ~PlayerCliBeginTrade();
};

class PlayerCliUnacceptTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerCliUnacceptTrade();
    ~PlayerCliUnacceptTrade();
};

class PlayerCliAcceptTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_stateIndex;

    void Put(CDataStore* msg) override;

    PlayerCliAcceptTrade();
    ~PlayerCliAcceptTrade();
};

class PlayerCliIgnoreTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerCliIgnoreTrade();
    ~PlayerCliIgnoreTrade();
};

class PlayerCliBusyTrade : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerCliBusyTrade();
    ~PlayerCliBusyTrade();
};

class PlayerInitiateTrade : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guid[8];

    void Put(CDataStore* msg) override;

    PlayerInitiateTrade();
    ~PlayerInitiateTrade();
};

class PlayerSetTradeGold : public JamClientMessage {
    public:
    /* 0010 */ uint64_t m_coinage;

    void Put(CDataStore* msg) override;

    PlayerSetTradeGold();
    ~PlayerSetTradeGold();
};

class PlayerSetTradeCurrency : public JamClientMessage {
    public:
    /* 0010 */ int32_t m_type;
    /* 0014 */ int32_t m_quantity;

    void Put(CDataStore* msg) override;

    PlayerSetTradeCurrency();
    ~PlayerSetTradeCurrency();
};

class PlayerSetTradeItem : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_packSlot;
    /* 0011 */ uint8_t m_itemSlotInPack;
    /* 0012 */ uint8_t m_tradeSlot;

    void Put(CDataStore* msg) override;

    PlayerSetTradeItem();
    ~PlayerSetTradeItem();
};

class PlayerClearTradeItem : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_tradeSlot;

    void Put(CDataStore* msg) override;

    PlayerClearTradeItem();
    ~PlayerClearTradeItem();
};

class PlayerRequestGuildPartyState : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guildGUID[8];

    void Put(CDataStore* msg) override;

    PlayerRequestGuildPartyState(uint64_t guildGUID);
    ~PlayerRequestGuildPartyState();
};

#endif
