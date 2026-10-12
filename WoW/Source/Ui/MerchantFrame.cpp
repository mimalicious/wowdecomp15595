#include "MerchantFrame.h"
#include <cstddef>
#include <cstdlib>
#include "WoW/Source/Object/Player.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include <cstring>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Ui/Tutorial.h"

// STATIC: 0x00F469C0/0x140E3AA90
uint32_t CGMerchantInfo::m_buybackSlots[12];

// STATIC: 0x00F469F0/0x140E3AAC0
VendorItem CGMerchantInfo::m_items[150];

// STATIC: 0x00F48160/0x140E3C230
WOWGUID CGMerchantInfo::m_merchant;

// STATIC: 0x00F48168/0x140E3C238
int32_t CGMerchantInfo::m_numMerchantItems;

// STATIC: 0x00F4816C/0x140E3C23C
uint32_t CGMerchantInfo::m_numBuybackItems;

// STATIC: 0x00F48170/0x140E3C240
uint32_t CGMerchantInfo::m_pendingItemStats;

// OFFSET: 0x00464680/0x140634FC0
VendorItem* CGMerchantInfo::GetItem(int32_t index) {
    if (index >= 0 && index < CGMerchantInfo::m_numMerchantItems && CGMerchantInfo::m_merchant) {
        return &CGMerchantInfo::m_items[index];
    }

    return NULL;
}

// OFFSET: 0x00967C70/0x1406DE1C0
void CGMerchantInfo::RefreshList() {
    if (CGMerchantInfo::m_merchant) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
    }
}

// OFFSET: 0x00967C90/0x1406DE1E0
void CGMerchantInfo::UpdateItemQuantity(WOWGUID merchant, int32_t slot, int32_t quantity) {
    if (merchant != CGMerchantInfo::m_merchant) {
        return;
    }

    for (int32_t i = 0; i < 150; i++) {
        if (CGMerchantInfo::m_items[i].m_slot == slot) {
            CGMerchantInfo::m_items[i].m_maxCount = quantity;
            break;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
}

// OFFSET: 0x00967D00/0x1406DE240
VendorItem* CGMerchantInfo::FindItem(int32_t type, int32_t itemId) {
    if (!CGMerchantInfo::m_merchant || !CGMerchantInfo::m_numMerchantItems) {
        return NULL;
    }

    for (int32_t i = 0; i < 150; i++) {
        if (CGMerchantInfo::m_items[i].m_type == type && CGMerchantInfo::m_items[i].m_itemID == itemId) {
            return &CGMerchantInfo::m_items[i];
        }
    }

    return NULL;
}

// OFFSET: 0x00967D50/0x1406DE290
static int32_t QSortBuybackTimestamps(const void* a, const void* b) {
    uint32_t timeA = static_cast<const BuybackEntry*>(a)->m_timestamp;
    uint32_t timeB = static_cast<const BuybackEntry*>(b)->m_timestamp;

    if (timeB > timeA) {
        return -1;
    }

    return timeB < timeA;
}

// OFFSET: 0x00967EC0/0x1406DE410
static void MerchantItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (CGMerchantInfo::m_pendingItemStats > 0 && --CGMerchantInfo::m_pendingItemStats) {
        return;
    }

    if (CGMerchantInfo::m_merchant) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
    }
}

// OFFSET: 0x00967EF0/0x1406DE450
int32_t CGMerchantInfo::CanRepair() {
    CGUnit_C* merchant = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGMerchantInfo::m_merchant, TYPEMASK_UNIT, __FILE__, __LINE__));
    return merchant && (merchant->m_unit->m_unitNpcFlags & 0x1000);
}

// OFFSET: 0x00967F30/0x1406DE4A0
void CGMerchantInfo::CloseMerchant() {
    if (!CGMerchantInfo::m_merchant) {
        return;
    }

    CGGameUI::ClearInteractTarget(CGMerchantInfo::m_merchant, 0);
    CGMerchantInfo::m_merchant = WOWGUID::Make(0);
    CGMerchantInfo::m_numMerchantItems = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_CLOSED, NULL);

    if (CGGameUI::m_cursorItemType == 5 && CGGameUI::GetCursorVirtualItem()) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (CursorGetResetMode() == 17) {
        CursorSetResetMode(1);
        CursorSetMode(1);
    }
}

// OFFSET: 0x009680B0/0x1406DE610
int32_t CGMerchantInfo::GetRepairCost(CGItem_C* item) {
    float multiplier = 1.0f;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* merchant = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGMerchantInfo::m_merchant, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (player && merchant) {
        multiplier = 1.0f - merchant->GetPlayerDiscount(player);
    }

    return CMath::fint_(static_cast<float>(static_cast<uint32_t>(item->GetRepairCost())) * multiplier);
}

// OFFSET: 0x00968160/0x1406DE6C0
void CGMerchantInfo::SortBuybackItems() {
    memset(CGMerchantInfo::m_buybackSlots, 0, sizeof(CGMerchantInfo::m_buybackSlots));
    CGMerchantInfo::m_numBuybackItems = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    BuybackEntry entries[12];

    for (uint32_t slot = 74; slot - 74 < 12; slot++) {
        if (slot < static_cast<uint32_t>(player->m_bag.m_numSlots) && player->m_bag.m_slots[slot] && slot - 74 < 12 && player->m_player->m_playerFieldBuybackPrice1[slot - 74]) {
            entries[CGMerchantInfo::m_numBuybackItems].m_timestamp = player->m_player->m_playerFieldBuybackTimestamp1[slot - 74];
            entries[CGMerchantInfo::m_numBuybackItems].m_slot = slot;
            CGMerchantInfo::m_numBuybackItems++;
        }
    }

    qsort(entries, CGMerchantInfo::m_numBuybackItems, sizeof(BuybackEntry), QSortBuybackTimestamps);

    for (uint32_t i = 0; i < CGMerchantInfo::m_numBuybackItems; i++) {
        CGMerchantInfo::m_buybackSlots[i] = entries[i].m_slot;
    }
}

// OFFSET: 0x00968340/0x1406DE7F0
bool CGMerchantInfo::IsBuybackItem(WOWGUID item) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    for (uint32_t i = 0; i < 12; i++) {
        WOWGUID guid;

        if (i + 74 >= static_cast<uint32_t>(player->m_bag.m_numSlots)) {
            guid = WOWGUID::Make(0);
        } else {
            guid = player->m_bag.m_slots[i + 74];
        }

        if (guid == item) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00968EF0/0x1406DF580
static int32_t BuybackUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGMerchantInfo::SortBuybackItems();
    FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x00968F10/0x1406DF5A0
void CGMerchantInfo::LeaveWorld() {
    CGMerchantInfo::CloseMerchant();
    ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldBuybackPrice1), BuybackUpdateHandler, NULL);
    ClientVendorInventory::s_handler = NULL;
}

// OFFSET: 0x00968F40/0x1406DF5F0
ItemRecSparse* CGMerchantInfo::GetItemStats(int32_t itemId) {
    if (!itemId) {
        return NULL;
    }

    WOWGUID merchant = CGMerchantInfo::m_merchant;
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, merchant, MerchantItemStatsCallback, NULL, false);

    if (!stats) {
        CGMerchantInfo::m_pendingItemStats++;
    }

    return stats;
}

// OFFSET: 0x00969FB0/0x1406E0930
static bool OnVendorInventory(ClientVendorInventory* msg) {
    uint8_t reason = msg->m_reason;

    if (!msg->m_items.m_count && reason != 1) {
        return true;
    }

    int32_t count = CGMerchantInfo::m_numMerchantItems;

    if (count) {
        for (int32_t i = 0; i < count; i++) {
            if (CGMerchantInfo::m_items[i].m_type == VENDOR_ITEM_TYPE_ITEM && CGMerchantInfo::m_items[i].m_itemID) {
                g_itemSparseDB.CancelCallback(CGMerchantInfo::m_items[i].m_itemID, MerchantItemStatsCallback, NULL);
                count = CGMerchantInfo::m_numMerchantItems;
            }
        }
    }

    memset(CGMerchantInfo::m_items, 0, sizeof(CGMerchantInfo::m_items));

    if (count > 150) {
        return true;
    }

    CGMerchantInfo::m_merchant = *reinterpret_cast<const WOWGUID*>(msg->m_vendor);
    CGMerchantInfo::m_numMerchantItems = msg->m_items.m_count;

    for (int32_t i = 0; i < CGMerchantInfo::m_numMerchantItems && i < 150; i++) {
        const JamVendorItem& source = msg->m_items.m_data[i];
        VendorItem& item = CGMerchantInfo::m_items[i];
        item.m_slot = source.m_muid;
        item.m_type = source.m_type;
        item.m_itemID = source.m_item;
        item.m_displayID = source.m_displayID;
        item.m_maxCount = source.m_quantity;
        item.m_price = source.m_price;
        item.m_durability = source.m_durability;
        item.m_buyCount = source.m_stackCount;

        if (item.m_type == VENDOR_ITEM_TYPE_CURRENCY) {
            CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item.m_itemID);

            if (currency && (currency->m_flags & 0x8)) {
                item.m_buyCount = source.m_stackCount / 100;
            }
        }

        item.m_extendedCostID = source.m_extendedCostID;
        item.m_playerConditionID = source.m_playerConditionID;
    }

    if (CGMerchantInfo::m_numMerchantItems > 0 || reason == 1) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            player->RequestAllItemPurchaseData();

            if (player->m_unit->m_unitFieldLevel >= 5) {
                CGTutorial::TriggerTutorial(0x13);
            }
        }

        CGMerchantInfo::m_pendingItemStats = 0;
        CGGossipInfo::ClearGossip();
        CGGameUI::SetInteractTarget(CGMerchantInfo::m_merchant);
        FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_SHOW, NULL);
    }

    return true;
}

// OFFSET: 0x0096A1B0/0x1406E0C10
void CGMerchantInfo::EnterWorld() {
    CGMerchantInfo::m_merchant = WOWGUID::Make(0);
    CGMerchantInfo::m_numMerchantItems = 0;
    memset(CGMerchantInfo::m_items, 0, sizeof(CGMerchantInfo::m_items));
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerFieldBuybackPrice1), sizeof(((CGPlayerLocalData*)0)->m_playerFieldBuybackPrice1) + sizeof(((CGPlayerLocalData*)0)->m_playerFieldBuybackTimestamp1), BuybackUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClientVendorInventory::s_handler = JamHandlerEncode(OnVendorInventory, g_jamHandlerKey_CFE1B8);
    CGMerchantInfo::SortBuybackItems();
}
