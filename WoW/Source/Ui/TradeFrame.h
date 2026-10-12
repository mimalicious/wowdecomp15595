#ifndef WOW_SOURCE_UI_TRADEFRAME_H
#define WOW_SOURCE_UI_TRADEFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGItem_C;
class ClientTradeStatus;
class ClientTradeUpdated;
struct JamTradeItem;

struct lua_State;

struct TradePlayerItem {
    /* 0000 */ WOWGUID m_item;
    /* 0008 */ WOWGUID m_container;
    /* 0010 */ uint8_t m_containerSlot;
    /* 0011 */ uint8_t unk_0011[7];
};

struct TradeTargetItem {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t m_count;
    /* 0008 */ uint8_t m_wrapped;
    /* 0009 */ uint8_t unk_0009[7];
    /* 0010 */ WOWGUID m_giftCreator;
    /* 0018 */ int32_t m_enchant;
    /* 001C */ int32_t m_socketEnchants[3];
    /* 0028 */ int32_t m_reforgeEnchant;
    /* 002C */ int32_t m_engineeringEnchant;
    /* 0030 */ WOWGUID m_creator;
    /* 0038 */ uint8_t m_locked;
    /* 003C */ int32_t m_spellCharges;
    /* 0040 */ int32_t m_suffixFactor;
    /* 0044 */ int32_t m_randomPropertyID;
    /* 0048 */ int32_t m_maxDurability;
    /* 004C */ int32_t m_durability;
};

struct TradeData {
    /* 0000 */ uint32_t m_stateIndex;
    /* 0004 */ int32_t m_proposedEnchant;
    /* 0008 */ uint64_t m_money;
    /* 0010 */ int32_t m_currencyType;
    /* 0014 */ int32_t m_currencyQuantity;
};

struct TradePlayerData : public TradeData {
    /* 0018 */ TradePlayerItem m_items[7];
};

struct TradeTargetData : public TradeData {
    /* 0018 */ TradeTargetItem m_items[7];
};

class CGTradeInfo {
    public:
    static TradeTargetData m_targetTrade;
    static TradePlayerData m_playerTrade;
    static WOWGUID m_tradingPlayer;
    static int32_t m_playerAccepted;
    static int32_t m_targetAccepted;
    static int32_t m_supressMoneyEvent;
    static int32_t m_useCursorItem;
    static WOWGUID m_enchantingItem;
    static int32_t m_initiator;
    static uint32_t m_tradeID;
    static int32_t m_partnerIsSameBnetAccount;

    static void EnterWorld();
    static void InitiateTrade(WOWGUID guid, int32_t useCursorItem);
    static void RemoveMoney(int64_t amount);
    static void UpdatePlayerItem(WOWGUID guid);
    static void UnlockTradeItems();
    static bool IsItemBeingTraded(WOWGUID item);
    static int32_t GetGameError(int32_t error, int32_t target);
    static void BeginTrade();
    static void CancelTrade();
    static void UnacceptTrade();
    static void AcceptTrade();
    static void IgnoreTrade();
    static void BusyTrade();
    static void ShutdownGame();
    static void SetEnchantingItem(CGItem_C* item);
    static void PlayerAccept();
    static bool AddMoney(int64_t amount);
    static void SetMoney(int64_t amount);
    static void SetCurrency(int32_t type, int32_t quantity);
    static int32_t SetPlayerItem(uint32_t slot, WOWGUID item, WOWGUID container, uint8_t containerSlot);
    static void RemovePlayerItem(WOWGUID item);
    static void DenyPlayerItem(uint32_t slot);
    static void AddCursorItem(int32_t bound);
    static void SetTradePartner(WOWGUID partner, int32_t supressMoneyEvent, int32_t sameBnetAccount);
    static bool OnTradeUpdated(ClientTradeUpdated* msg);
    static void LeaveWorld();
    static void HandleTradeMessage(int32_t status, int32_t bagResult, int32_t target, int32_t itemId);
    static bool AddPlayerItem(WOWGUID item, WOWGUID container, int32_t slot);
    static bool OnTradeStatus(ClientTradeStatus* msg);
    static void InitializeGame();
    static int32_t GetTargetCount(uint32_t slot);
    static int32_t GetTargetEnchant(uint32_t slot);
    static int32_t GetTargetRandomPropertyID(uint32_t slot);
    static void GetPlayerItem(uint32_t slot, WOWGUID& item, WOWGUID& container, uint8_t& containerSlot);
};

void TradeInfoRegisterScriptFunctions();
void TradeInfoUnregisterScriptFunctions();

void TradeItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool CanTradeBoundItem(CGItem_C* item);

#endif
