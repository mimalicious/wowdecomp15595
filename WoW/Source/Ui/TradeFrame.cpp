#include "TradeFrame.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"

// STATIC: 0x00F48178
static uint32_t s_clientStateIndex;

// STATIC: 0x00F48180
TradeTargetData CGTradeInfo::m_targetTrade;

// STATIC: 0x00F483C8
TradePlayerData CGTradeInfo::m_playerTrade;

// STATIC: 0x00F48488
WOWGUID CGTradeInfo::m_tradingPlayer;

// STATIC: 0x00F48490
int32_t CGTradeInfo::m_playerAccepted;

// STATIC: 0x00F48494
int32_t CGTradeInfo::m_targetAccepted;

// STATIC: 0x00F48498
int32_t CGTradeInfo::m_supressMoneyEvent;

// STATIC: 0x00F4849C
int32_t CGTradeInfo::m_useCursorItem;

// STATIC: 0x00F484A0
WOWGUID CGTradeInfo::m_enchantingItem;

// STATIC: 0x00F484A8
int32_t CGTradeInfo::m_initiator;

// STATIC: 0x00F484AC
uint32_t CGTradeInfo::m_tradeID;

// STATIC: 0x00F484B0
int32_t CGTradeInfo::m_partnerIsSameBnetAccount;

// STATIC: 0x00F484B4
static uint32_t s_nextTradeRequestTime;

// OFFSET: 0x004646B0
int32_t CGTradeInfo::GetTargetCount(uint32_t slot) {
    if (slot <= 6) {
        return CGTradeInfo::m_targetTrade.m_items[slot].m_count;
    }

    return 0;
}

// OFFSET: 0x004646D0
int32_t CGTradeInfo::GetTargetEnchant(uint32_t slot) {
    if (slot <= 6) {
        return CGTradeInfo::m_targetTrade.m_items[slot].m_enchant;
    }

    return 0;
}

// OFFSET: 0x004646F0
int32_t CGTradeInfo::GetTargetRandomPropertyID(uint32_t slot) {
    if (slot <= 6) {
        return CGTradeInfo::m_targetTrade.m_items[slot].m_randomPropertyID;
    }

    return 0;
}

// OFFSET: 0x0096A220
void CGTradeInfo::GetPlayerItem(uint32_t slot, WOWGUID& item, WOWGUID& container, uint8_t& containerSlot) {
    if (slot > 6) {
        item = WOWGUID();
    } else {
        item = CGTradeInfo::m_playerTrade.m_items[slot].m_item;
    }

    if (item) {
        container = CGTradeInfo::m_playerTrade.m_items[slot].m_container;
        containerSlot = CGTradeInfo::m_playerTrade.m_items[slot].m_containerSlot;
    } else {
        container = WOWGUID();
        containerSlot = 0;
    }
}

// PROVEN: c6b5775e IDENTICAL/NONE
// OFFSET: 0x0096A2A0/0x1406E0C90
void TradeItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_UPDATE, NULL);
    }
}

// OFFSET: 0x0096A2C0/0x1406E0CB0
void CGTradeInfo::EnterWorld() {
    CGTradeInfo::m_tradingPlayer = WOWGUID();
    CGTradeInfo::m_playerAccepted = 0;
    CGTradeInfo::m_targetAccepted = 0;
    memset(&CGTradeInfo::m_playerTrade, 0, sizeof(CGTradeInfo::m_playerTrade));
    memset(&CGTradeInfo::m_targetTrade, 0, sizeof(CGTradeInfo::m_targetTrade));
}

// OFFSET: 0x0096A300/0x1406E0D00
static void TradeNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        CGGameUI::DisplayError(ERR_INITIATE_TRADE_S, name->m_name);
    }
}

// OFFSET: 0x0096A350/0x1406E0D50
void CGTradeInfo::InitiateTrade(WOWGUID guid, int32_t useCursorItem) {
    if (CGTradeInfo::m_tradingPlayer) {
        CGGameUI::DisplayError(ERR_ALREADY_TRADING);
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (s_nextTradeRequestTime && static_cast<int32_t>(now - s_nextTradeRequestTime) < 0) {
        return;
    }

    s_nextTradeRequestTime = now + 1000;
    PlayerInitiateTrade msg;
    *reinterpret_cast<WOWGUID*>(msg.m_guid) = guid;
    ClientServices::Send(&msg);
    CGTradeInfo::m_useCursorItem = useCursorItem;
    CGTradeInfo::m_tradingPlayer = guid;
    CGTradeInfo::m_initiator = 1;
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, &TradeNameCallback, NULL, false);

    if (name) {
        CGGameUI::DisplayError(ERR_INITIATE_TRADE_S, name->m_name);
    }
}

// OFFSET: 0x0096A420
static void SetTargetAccepted(int32_t accepted) {
    if (CGTradeInfo::m_targetAccepted != accepted) {
        CGTradeInfo::m_targetAccepted = accepted;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", CGTradeInfo::m_playerAccepted, accepted);
    }
}

// PROVEN: e76cd299 IDENTICAL/NONE
// OFFSET: 0x0096A450/0x1406E0E60
void CGTradeInfo::RemoveMoney(int64_t amount) {
    if (!amount || static_cast<uint64_t>(amount) > CGTradeInfo::m_playerTrade.m_money) {
        return;
    }

    uint64_t money = CGTradeInfo::m_playerTrade.m_money - amount;
    PlayerSetTradeGold msg;
    msg.m_coinage = money;
    ClientServices::Send(&msg);
    s_clientStateIndex++;
    CGTradeInfo::m_playerTrade.m_money = money;
}

// PROVEN: a56a29b3 IDENTICAL/NONE
// OFFSET: 0x0096A4C0
void CGTradeInfo::UpdatePlayerItem(WOWGUID guid) {
    if (!guid) {
        return;
    }

    for (int32_t i = 0; i < 7; i++) {
        if (CGTradeInfo::m_playerTrade.m_items[i].m_item == guid) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_PLAYER_ITEM_CHANGED, "%d", i + 1);
            return;
        }
    }
}

// OFFSET: 0x0096A510
void CGTradeInfo::UnlockTradeItems() {
    for (uint32_t i = 0; i < 7; i++) {
        if (i <= 6 && CGTradeInfo::m_playerTrade.m_items[i].m_item) {
            CGGameUI::UnlockItem(CGTradeInfo::m_playerTrade.m_items[i].m_item);
        }
    }
}

// OFFSET: 0x0096A550/0x1406E0F60
bool CGTradeInfo::IsItemBeingTraded(WOWGUID item) {
    if (!CGTradeInfo::m_tradingPlayer) {
        return false;
    }

    for (uint32_t i = 0; i < 7; i++) {
        WOWGUID current;

        if (i > 6) {
            current = WOWGUID();
        } else {
            current = CGTradeInfo::m_playerTrade.m_items[i].m_item;
        }

        if (current == item) {
            return true;
        }
    }

    return false;
}

// PROVEN: b8cac5cb IDENTICAL/NONE
// OFFSET: 0x0096A5B0/0x1402190F0
bool CanTradeBoundItem(CGItem_C* item) {
    return item->IsBoundToBattlenetAccount() && CGTradeInfo::m_partnerIsSameBnetAccount;
}

// PROVEN: 7ef05467 IDENTICAL/NONE
// OFFSET: 0x0096A5E0
int32_t CGTradeInfo::GetGameError(int32_t error, int32_t target) {
    switch (error) {
        case 4:
            return target ? ERR_TRADE_BAG_FULL : ERR_TRADE_TARGET_BAG_FULL;

        case 17:
            return target ? ERR_TRADE_TARGET_MAX_COUNT_EXCEEDED : ERR_TRADE_MAX_COUNT_EXCEEDED;

        case 28:
            return target ? ERR_MAX_VALUE : ERR_SPELL_FAILED_REAGENTS_GENERIC;

        case 84:
            return target ? ERR_TRADE_TARGET_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS : ERR_ITEM_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS;

        default:
            return GetDisplayErrorByInventoryError(error);
    }
}

// PROVEN: 72caeb14 IDENTICAL/NONE
// OFFSET: 0x0096A6B0/0x1406E0FF0
void CGTradeInfo::BeginTrade() {
    PlayerCliBeginTrade msg;
    ClientServices::Send(&msg);
}

// PROVEN: 2961daaf IDENTICAL/NONE
// OFFSET: 0x0096A6E0/0x1406E1020
void CGTradeInfo::CancelTrade() {
    PlayerCliCancelTrade msg;
    ClientServices::Send(&msg);
}

// PROVEN: ba0eb945 IDENTICAL/NONE
// OFFSET: 0x0096A710
void CGTradeInfo::UnacceptTrade() {
    PlayerCliUnacceptTrade msg;
    ClientServices::Send(&msg);
}

// PROVEN: d1bc0cb8 IDENTICAL/NONE
// OFFSET: 0x0096A740
void CGTradeInfo::AcceptTrade() {
    PlayerCliAcceptTrade msg;
    msg.m_stateIndex = CGTradeInfo::m_targetTrade.m_stateIndex;
    ClientServices::Send(&msg);
}

// PROVEN: 89cfa6a7 IDENTICAL/NONE
// OFFSET: 0x0096A770
void CGTradeInfo::IgnoreTrade() {
    PlayerCliIgnoreTrade msg;
    ClientServices::Send(&msg);
}

// PROVEN: 68f1c970 IDENTICAL/NONE
// OFFSET: 0x0096A7A0
void CGTradeInfo::BusyTrade() {
    PlayerCliBusyTrade msg;
    ClientServices::Send(&msg);
}

// PROVEN: 904274db IDENTICAL/NONE
// OFFSET: 0x0096AA10
void CGTradeInfo::ShutdownGame() {
    ClientTradeStatus::s_handler = NULL;
    ClientTradeUpdated::s_handler = NULL;
}

// OFFSET: 0x0096AA20
static void ConvertTradeItem(TradeTargetItem* dst, const JamTradeItem* src) {
    dst->m_itemID = src->m_itemID;
    dst->m_count = src->m_stackCount;
    dst->m_wrapped = !src->m_hasUnwrapped;
    dst->m_giftCreator = *reinterpret_cast<const WOWGUID*>(src->m_giftCreator);
    const ClientTradeItemUnwrapped* unwrapped = src->m_hasUnwrapped ? &src->m_unwrapped : NULL;

    if (unwrapped) {
        dst->m_enchant = unwrapped->m_enchantID;
        dst->m_reforgeEnchant = unwrapped->m_reforgeID;
        dst->m_engineeringEnchant = unwrapped->m_onUseEnchantmentID;
        dst->m_socketEnchants[0] = unwrapped->m_socketEnchant[0];
        dst->m_socketEnchants[1] = unwrapped->m_socketEnchant[1];
        dst->m_socketEnchants[2] = unwrapped->m_socketEnchant[2];
        dst->m_creator = *reinterpret_cast<const WOWGUID*>(unwrapped->m_creator);
        dst->m_spellCharges = unwrapped->m_charges;
        dst->m_suffixFactor = unwrapped->m_randomPropertiesSeed;
        dst->m_randomPropertyID = unwrapped->m_randomPropertiesID;
        dst->m_locked = unwrapped->m_lock;
        dst->m_maxDurability = unwrapped->m_maxDurability;
        dst->m_durability = unwrapped->m_durability;
        return;
    }

    memset(&dst->m_enchant, 0, sizeof(TradeTargetItem) - offsetof(TradeTargetItem, m_enchant));
}

// OFFSET: 0x0096AAC0/0x1406E13B0
void CGTradeInfo::SetEnchantingItem(CGItem_C* item) {
    if (CGTradeInfo::m_enchantingItem) {
        CGGameUI::UnlockItem(CGTradeInfo::m_enchantingItem);
        CGTradeInfo::m_enchantingItem = WOWGUID();
    }

    if (item) {
        WOWGUID none;
        none.m_guid = 0;
        item->UseItem(&none, 0);
        CGGameUI::LockItem(item->m_obj->m_guid);
        CGTradeInfo::m_enchantingItem = item->m_obj->m_guid;
    }
}

// OFFSET: 0x0096AC70
void CGTradeInfo::PlayerAccept() {
    if (CGGameUI::CanPerformAction(7) && CGTradeInfo::m_playerAccepted) {
        CGTradeInfo::m_playerAccepted = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", 0, CGTradeInfo::m_targetAccepted);
        CGTradeInfo::UnacceptTrade();
    }

    if (CGTradeInfo::m_targetAccepted) {
        CGTradeInfo::m_targetAccepted = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", CGTradeInfo::m_playerAccepted, 0);
    }
}

// PROVEN: 2ff66143 IDENTICAL/NONE
// OFFSET: 0x0096ACE0/0x1406E1650
bool CGTradeInfo::AddMoney(int64_t amount) {
    if (!amount) {
        return true;
    }

    uint64_t total = amount + CGTradeInfo::m_playerTrade.m_money;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->GetCoinage() < total) {
        return false;
    }

    PlayerSetTradeGold msg;
    msg.m_coinage = total;
    ClientServices::Send(&msg);
    s_clientStateIndex++;
    CGTradeInfo::m_playerTrade.m_money = total;
    return true;
}

// OFFSET: 0x0096AD80
void CGTradeInfo::SetMoney(int64_t amount) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->GetCoinage() < static_cast<uint64_t>(amount) || amount >= 10000000000LL) {
        return;
    }

    PlayerSetTradeGold msg;
    msg.m_coinage = amount;
    ClientServices::Send(&msg);
    s_clientStateIndex++;
    CGTradeInfo::m_playerTrade.m_money = amount;
}

// PROVEN: a2a2c604 IDENTICAL/NONE
// OFFSET: 0x0096AE50
void CGTradeInfo::SetCurrency(int32_t type, int32_t quantity) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || !CGCurrencyTypes::HasCurrency(type, quantity)) {
        return;
    }

    if (type && type == CGTradeInfo::m_targetTrade.m_currencyType) {
        return;
    }

    PlayerSetTradeCurrency msg;
    msg.m_type = type;
    msg.m_quantity = quantity;
    ClientServices::Send(&msg);
    s_clientStateIndex++;
    CGTradeInfo::m_playerTrade.m_currencyType = type;
    CGTradeInfo::m_playerTrade.m_currencyQuantity = quantity;
}

// OFFSET: 0x0096AEE0/0x1406E1880
int32_t CGTradeInfo::SetPlayerItem(uint32_t slot, WOWGUID item, WOWGUID container, uint8_t containerSlot) {
    if (slot <= 6) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            return 0;
        }

        if (item) {
            if (slot == 6) {
                CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));
                bool bound = object && object->IsBound() && !object->IsBoundForTrade();
                FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_POTENTIAL_BIND_ENCHANT, "%b", bound);
            }
        } else if (slot == 6) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_POTENTIAL_BIND_ENCHANT, "%b", 0);
        }

        CGTradeInfo::m_playerTrade.m_items[slot].m_item = item;
        CGTradeInfo::m_playerTrade.m_items[slot].m_container = container;
        CGTradeInfo::m_playerTrade.m_items[slot].m_containerSlot = containerSlot;

        if (item) {
            PlayerSetTradeItem msg;
            msg.m_tradeSlot = static_cast<uint8_t>(slot);
            msg.m_packSlot = player->FindSlotIndex(container);
            msg.m_itemSlotInPack = containerSlot;
            ClientServices::Send(&msg);
        } else {
            PlayerClearTradeItem msg;
            msg.m_tradeSlot = static_cast<uint8_t>(slot);
            ClientServices::Send(&msg);
        }

        s_clientStateIndex++;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_PLAYER_ITEM_CHANGED, "%d", slot + 1);
        return 1;
    }

    return 0;
}

// OFFSET: 0x0096B040
void CGTradeInfo::RemovePlayerItem(WOWGUID item) {
    if (!item) {
        return;
    }

    for (int32_t i = 0; i < 7; i++) {
        if (CGTradeInfo::m_playerTrade.m_items[i].m_item == item) {
            CGTradeInfo::SetPlayerItem(i, WOWGUID::Make(0), WOWGUID::Make(0), 0);
            return;
        }
    }
}

// OFFSET: 0x0096B090
void CGTradeInfo::DenyPlayerItem(uint32_t slot) {
    if (slot == 255) {
        CGTradeInfo::m_playerTrade.m_money = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_MONEY, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, NULL);
        return;
    }

    CGGameUI::UnlockItem(slot <= 6 ? CGTradeInfo::m_playerTrade.m_items[slot].m_item : WOWGUID());
    CGTradeInfo::SetPlayerItem(slot, WOWGUID(), WOWGUID(), 0);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_PLAYER_ITEM_CHANGED, "%d", slot + 1);
}

// OFFSET: 0x0096B120/0x1406E1BA0
void CGTradeInfo::AddCursorItem(int32_t bound) {
    WOWGUID item;
    WOWGUID container;
    uint32_t slot;
    CGGameUI::GetCursorItem(item, container, slot);

    if (!item) {
        return;
    }

    uint32_t index = -1;

    if (bound) {
        if (CGTradeInfo::m_playerTrade.m_items[6].m_item) {
            return;
        }

        index = 6;
    } else {
        for (uint32_t i = 0; i < 6; i++) {
            WOWGUID current;

            if (i > 6) {
                current = WOWGUID();
            } else {
                current = CGTradeInfo::m_playerTrade.m_items[i].m_item;
            }

            if (current == item) {
                return;
            }

            if (index == -1 && (i > 6 || !CGTradeInfo::m_playerTrade.m_items[i].m_item)) {
                index = i;
            }
        }

        if (index == -1) {
            return;
        }
    }

    if (CGTradeInfo::SetPlayerItem(index, item, container, static_cast<uint8_t>(slot))) {
        CGGameUI::ClearCursor(0, 1);
    }
}

// OFFSET: 0x0096B3E0/0x1406E1E80
void CGTradeInfo::SetTradePartner(WOWGUID partner, int32_t supressMoneyEvent, int32_t sameBnetAccount) {
    CGTradeInfo::UnlockTradeItems();

    if (!partner) {
        if (CGTradeInfo::m_tradingPlayer) {
            if (CGTradeInfo::m_enchantingItem) {
                CGGameUI::UnlockItem(CGTradeInfo::m_enchantingItem);
                CGTradeInfo::m_enchantingItem = WOWGUID();
            }

            CGTradeInfo::m_supressMoneyEvent = supressMoneyEvent;
            CGGameUI::ClearInteractTarget(CGTradeInfo::m_tradingPlayer, 0);
            CGTradeInfo::m_tradingPlayer = WOWGUID();
            CGTradeInfo::m_initiator = 0;
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_CLOSED, NULL);
        }

        CGTradeInfo::CancelTrade();
        return;
    }

    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(partner);
    CGTradeInfo::m_tradingPlayer = partner;
    CGTradeInfo::m_partnerIsSameBnetAccount = sameBnetAccount;
    CGTradeInfo::m_playerAccepted = 0;
    CGTradeInfo::m_targetAccepted = 0;
    memset(&CGTradeInfo::m_playerTrade, 0, sizeof(CGTradeInfo::m_playerTrade));
    memset(&CGTradeInfo::m_targetTrade, 0, sizeof(CGTradeInfo::m_targetTrade));

    if (CGTradeInfo::m_useCursorItem) {
        if (CGGameUI::m_cursorMoney) {
            if (CGTradeInfo::AddMoney(CGGameUI::m_cursorMoney)) {
                CGGameUI::ClearCursor(1, 0);
                FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_MONEY, NULL);
                FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, NULL);
            }
        } else {
            WOWGUID item;
            WOWGUID container;
            uint32_t slot;
            CGGameUI::GetCursorItem(item, container, slot);

            if (item && container) {
                uint32_t index = 0;
                CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (object && object->IsBound() && !CanTradeBoundItem(object)) {
                    index = 6;
                    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_POTENTIAL_BIND_ENCHANT, "%b", !object->IsBoundForTrade());
                }

                CGGameUI::ClearCursor(0, 1);
                CGTradeInfo::SetPlayerItem(index, item, container, static_cast<uint8_t>(slot));
            }
        }
    }

    CGTradeInfo::m_playerTrade.m_stateIndex = 1;
    CGTradeInfo::m_targetTrade.m_stateIndex = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SHOW, NULL);
}

// OFFSET: 0x0096B610
bool CGTradeInfo::OnTradeUpdated(ClientTradeUpdated* msg) {
    uint8_t whichPlayer = msg->m_whichPlayer;

    if (msg->m_id != CGTradeInfo::m_tradeID) {
        CGTradeInfo::CancelTrade();
        return true;
    }

    if (!whichPlayer) {
        if (msg->m_clientStateIndex < s_clientStateIndex) {
            return true;
        }

        if (msg->m_clientStateIndex != s_clientStateIndex) {
            CGTradeInfo::CancelTrade();
            return true;
        }
    }

    CGTradeInfo::PlayerAccept();
    TradeData* data = whichPlayer ? static_cast<TradeData*>(&CGTradeInfo::m_targetTrade) : static_cast<TradeData*>(&CGTradeInfo::m_playerTrade);
    data->m_stateIndex = msg->m_stateIndex;

    if (data->m_money != msg->m_gold) {
        data->m_money = msg->m_gold;

        if (!whichPlayer) {
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_MONEY, NULL);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_MONEY_CHANGED, NULL);
    }

    if (data->m_proposedEnchant != msg->m_proposedEnchantment) {
        data->m_proposedEnchant = msg->m_proposedEnchantment;
        FrameScript_SignalEvent(whichPlayer ? SCRIPT_EVENT_TRADE_PLAYER_ITEM_CHANGED : SCRIPT_EVENT_TRADE_TARGET_ITEM_CHANGED, "%d", 7);
    }

    if (data->m_currencyType != msg->m_currencyType || data->m_currencyQuantity != msg->m_currencyQuantity) {
        data->m_currencyType = msg->m_currencyType;
        data->m_currencyQuantity = msg->m_currencyQuantity;

        if (!whichPlayer) {
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_CURRENCY, NULL);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_CURRENCY_CHANGED, NULL);
    }

    TradeTargetItem items[7];
    bool present[7] = {};
    memset(items, 0, sizeof(items));

    for (uint32_t i = 0; i < msg->m_items.Count(); i++) {
        JamTradeItem* item = &msg->m_items[i];
        uint8_t slot = item->m_slot;
        ConvertTradeItem(&items[slot], item);
        present[slot] = true;
    }

    for (int32_t i = 0; i < 7; i++) {
        if (!whichPlayer) {
            if (CGTradeInfo::m_playerTrade.m_items[i].m_item && (!items[i].m_itemID || !present[i])) {
                CGTradeInfo::DenyPlayerItem(i);
            }
        } else if (memcmp(&CGTradeInfo::m_targetTrade.m_items[i], &items[i], sizeof(TradeTargetItem))) {
            CGTradeInfo::m_targetTrade.m_items[i] = items[i];
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_TARGET_ITEM_CHANGED, "%d", i + 1);

            if (i == 6 && CGTradeInfo::m_enchantingItem) {
                CGGameUI::UnlockItem(CGTradeInfo::m_enchantingItem);
                CGTradeInfo::m_enchantingItem = WOWGUID();
            }
        }
    }

    return true;
}

// OFFSET: 0x0096B8D0/0x1406E23E0
void CGTradeInfo::LeaveWorld() {
    CGTradeInfo::UnlockTradeItems();
    CGTradeInfo::SetTradePartner(WOWGUID(), 0, 0);
    CGTradeInfo::CancelTrade();
}

// OFFSET: 0x0096B910
void CGTradeInfo::HandleTradeMessage(int32_t status, int32_t bagResult, int32_t target, int32_t itemId) {
    switch (status) {
        case 3:
            CGGameUI::DisplayError(ERR_LOGGING_OUT);
            break;

        case 4:
            if (CGTradeInfo::m_initiator) {
                if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGTradeInfo::m_tradingPlayer, TYPEMASK_PLAYER, __FILE__, __LINE__))) {
                    CGGameUI::DisplayError(ERR_IGNORING_YOU_S, unit->GetUnitName(NULL, 1));
                }
            }

            break;

        case 5:
            CGGameUI::DisplayError(ERR_TRADE_TARGET_DEAD);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_TARGET_LOGGING_OUT);
            break;

        case 10:
            CGGameUI::DisplayError(ERR_RESTRICTED_ACCOUNT);
            break;

        case 13:
            CGGameUI::DisplayError(ERR_PLAYER_DEAD);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_TRADE_TOO_FAR);
            break;

        case 17:
            CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
            break;

        case 18:
        case 21:
            if (CGTradeInfo::m_initiator) {
                if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGTradeInfo::m_tradingPlayer, TYPEMASK_PLAYER, __FILE__, __LINE__))) {
                    CGGameUI::DisplayError(ERR_PLAYER_BUSY_S, unit->GetUnitName(NULL, 1));
                }
            }

            break;

        case 20:
            CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
            break;

        case 27:
            CGGameUI::DisplayError(ERR_GENERIC_STUNNED);
            break;

        case 29:
            CGGameUI::DisplayError(ERR_TARGET_STUNNED);
            break;

        case 31:
            CGTradeInfo::UnlockTradeItems();

            if (bagResult == 84 && itemId) {
                WOWGUID requester = WOWGUID();
                ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

                if (!rec) {
                    break;
                }

                ItemLimitCategoryRec* category = g_itemLimitCategoryDB.GetRecord(rec->m_itemLimitCategory);

                if (!category) {
                    break;
                }

                CGGameUI::DisplayError(static_cast<UIErrors>(CGTradeInfo::GetGameError(84, target)), category->m_quantity, category->m_name);
            } else {
                CGGameUI::DisplayError(static_cast<UIErrors>(CGTradeInfo::GetGameError(bagResult, target)));
            }

            break;
    }

    CGTradeInfo::SetTradePartner(WOWGUID(), 0, 0);
}

// OFFSET: 0x0096BB70
bool CGTradeInfo::AddPlayerItem(WOWGUID item, WOWGUID container, int32_t slot) {
    CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!object) {
        return false;
    }

    uint32_t index;

    if (!object->IsBound() || (object->IsBoundToBattlenetAccount() && CGTradeInfo::m_partnerIsSameBnetAccount)) {
        for (index = 0; index < 6; index++) {
            if (!CGTradeInfo::m_playerTrade.m_items[index].m_item) {
                break;
            }
        }

        if (index >= 6) {
            return false;
        }
    } else {
        WOWGUID requester = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, requester, NULL, NULL, false);

        if (rec && rec->m_bonding == 4) {
            CGGameUI::DisplayError(ERR_TRADE_QUEST_ITEM);
            return false;
        }

        if (CGTradeInfo::m_playerTrade.m_items[6].m_item) {
            return false;
        }

        index = 6;
    }

    if (object->m_obj->m_type & TYPEMASK_CONTAINER) {
        if (const CGBag_C* bag = object->GetBag()) {
            if (bag->GetNumItems()) {
                CGGameUI::DisplayError(ERR_TRADE_BAG);
                return false;
            }

            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (static_cast<uint32_t>(player->GetBag()->FindItemSlot(object) - 19) <= 3) {
                CGGameUI::DisplayError(ERR_TRADE_EQUIPPED_BAG);
                return false;
            }
        }
    }

    CGTradeInfo::SetPlayerItem(index, item, container, static_cast<uint8_t>(slot));
    CGGameUI::LockItem(item);
    SI3::PlayItemSound(1, object);
    return true;
}

// OFFSET: 0x0096C530
bool CGTradeInfo::OnTradeStatus(ClientTradeStatus* msg) {
    uint32_t status = msg->m_status;

    switch (status) {
        case 0:
            CGTradeInfo::m_tradeID = msg->m_id;
            s_clientStateIndex = 1;
            CGTradeInfo::SetTradePartner(CGTradeInfo::m_tradingPlayer, 0, msg->m_partnerIsSameBnetAccount);
            break;

        case 1:
        case 23:
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, NULL);

            if (status == 23) {
                FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_REQUEST_CANCEL, NULL);
                CGTradeInfo::SetTradePartner(WOWGUID(), 0, 0);
                CGGameUI::DisplayError(ERR_TRADE_CANCELLED);
            }

            break;

        case 2:
        case 26: {
            uint8_t slot = msg->m_tradeSlot;

            if (slot == 255) {
                CGTradeInfo::m_playerTrade.m_money = 0;
            }

            if (status == 26) {
                CGGameUI::DisplayError(ERR_TRADE_WRONG_REALM);
            } else if (status == 2) {
                CGGameUI::DisplayError(ERR_TRADE_NOT_ON_TAPLIST);
            }

            CGTradeInfo::DenyPlayerItem(slot);
            break;
        }

        case 3:
        case 4:
        case 5:
        case 7:
        case 10:
        case 13:
        case 16:
        case 17:
        case 18:
        case 20:
        case 21:
        case 27:
        case 29:
        case 31:
            CGTradeInfo::HandleTradeMessage(status, msg->m_bagResult, msg->m_failureForYou, msg->m_itemID);
            break;

        case 6:
            SetTargetAccepted(1);
            break;

        case 9:
            CGTradeInfo::UnlockTradeItems();
            CGTradeInfo::SetTradePartner(WOWGUID(), 1, 0);
            CGGameUI::DisplayError(ERR_TRADE_COMPLETE);
            break;

        case 12: {
            if (CGTradeInfo::m_initiator) {
                CGTradeInfo::BusyTrade();
            } else {
                CGTradeInfo::m_tradingPlayer = *reinterpret_cast<WOWGUID*>(msg->m_partner);
            }

            CGTradeInfo::m_partnerIsSameBnetAccount = msg->m_partnerIsSameBnetAccount;

            if (CGTradeInfo::m_initiator) {
                break;
            }

            if (FriendList::s_friendList->IsIgnored(CGTradeInfo::m_tradingPlayer, true)) {
                CGTradeInfo::IgnoreTrade();
                break;
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGTradeInfo::m_tradingPlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (!unit) {
                break;
            }

            if (unit->IsDeadOrGhost() || CGGameUI::m_inCinematic || !CGGameUI::m_hasControl || CGAuctionHouse::m_auctioneer) {
                CGTradeInfo::BusyTrade();
            } else if (CVar::Lookup("BlockTrades")->m_intValue) {
                CGTradeInfo::BusyTrade();
                CGGameUI::DisplayError(ERR_TRADE_BLOCKED_S, unit->GetUnitName(NULL, 1));
            } else {
                CGTradeInfo::BeginTrade();
            }

            break;
        }

        case 19:
        case 24:
            if (status == 19) {
                CGGameUI::DisplayError(ERR_CURRENCY_NOT_TRADABLE);
            }

            CGTradeInfo::m_playerTrade.m_currencyType = msg->m_currencyType;
            CGTradeInfo::m_playerTrade.m_currencyQuantity = msg->m_currencyQuantity;
            FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_CURRENCY, NULL);
            break;

        case 22:
            SetTargetAccepted(0);
            break;

        case 25:
            CGTradeInfo::PlayerAccept();
            break;
    }

    return true;
}

// OFFSET: 0x0096C820/0x1406E35D0
void CGTradeInfo::InitializeGame() {
    ClientTradeStatus::s_handler = &CGTradeInfo::OnTradeStatus;
    ClientTradeUpdated::s_handler = &CGTradeInfo::OnTradeUpdated;
}
