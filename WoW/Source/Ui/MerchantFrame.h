#ifndef WOW_SOURCE_UI_MERCHANTFRAME_H
#define WOW_SOURCE_UI_MERCHANTFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class ItemRecSparse;
class CGItem_C;
class ClientVendorInventory;

struct lua_State;

enum VENDOR_ITEM_TYPE {
    VENDOR_ITEM_TYPE_NONE = 0,
    VENDOR_ITEM_TYPE_ITEM = 1,
    VENDOR_ITEM_TYPE_CURRENCY = 2
};

struct VendorItem {
    /* 0000 */ int32_t m_slot;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_itemID;
    /* 000C */ int32_t m_displayID;
    /* 0010 */ int32_t m_maxCount;
    /* 0014 */ int32_t m_price;
    /* 0018 */ int32_t m_durability;
    /* 001C */ int32_t m_buyCount;
    /* 0020 */ int32_t m_extendedCostID;
    /* 0024 */ int32_t m_playerConditionID;
};

struct BuybackEntry {
    /* 0000 */ uint32_t m_timestamp;
    /* 0004 */ uint32_t m_slot;
};

class CGMerchantInfo {
    public:
    static uint32_t m_buybackSlots[12];
    static VendorItem m_items[150];
    static WOWGUID m_merchant;
    static int32_t m_numMerchantItems;
    static uint32_t m_numBuybackItems;
    static uint32_t m_pendingItemStats;

    static void EnterWorld();
    static void LeaveWorld();
    static void RefreshList();
    static void UpdateItemQuantity(WOWGUID merchant, int32_t slot, int32_t quantity);
    static VendorItem* FindItem(int32_t type, int32_t itemId);
    static VendorItem* GetItem(int32_t index);
    static ItemRecSparse* GetItemStats(int32_t itemId);
    static int32_t CanRepair();
    static void CloseMerchant();
    static int32_t GetRepairCost(CGItem_C* item);
    static void SortBuybackItems();
    static bool IsBuybackItem(WOWGUID item);
};

void MerchantRegisterScriptFunctions();
void MerchantUnregisterScriptFunctions();

#endif
