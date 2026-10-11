#include "MerchantFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D131B8
static uint32_t s_costCurrencyFlags[5] = { 0x2, 0x4, 0x8, 0x10, 0x20 };

// OFFSET: 0x00967C60/0x1406DE1B0
static void MerchantUpdateCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_MERCHANT_UPDATE, NULL);
}

// OFFSET: 0x00967D80/0x1406DE2B0
static int32_t Script_GetMerchantNumItems(lua_State* L) {
    lua_pushnumber(L, CGMerchantInfo::m_numMerchantItems);
    return 1;
}

// PROVEN: 5551c8c7 IDENTICAL/NONE
// OFFSET: 0x00967DB0/0x1406DE2D0
static int32_t Script_HideRepairCursor(lua_State* L) {
    if (CursorGetResetMode() == 17) {
        CursorSetResetMode(1);
        CursorSetMode(1);
    }

    return 0;
}

// PROVEN: b8396488 IDENTICAL/NONE
// OFFSET: 0x00967DD0/0x1406DE300
static int32_t Script_InRepairMode(lua_State* L) {
    if (CursorGetResetMode() == 17) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00967E10/0x1406DE340
static int32_t Script_GetNumBuybackItems(lua_State* L) {
    if (!CGMerchantInfo::m_merchant) {
        lua_pushnumber(L, 0.0);
    } else {
        lua_pushnumber(L, CGMerchantInfo::m_numBuybackItems);
    }

    return 1;
}

// OFFSET: 0x00967FB0/0x1406DE520
static int32_t Script_CloseMerchant(lua_State* L) {
    CGMerchantInfo::CloseMerchant();
    return 0;
}

// OFFSET: 0x00967FC0/0x1406DE530
static int32_t Script_CanMerchantRepair(lua_State* L) {
    if (CGMerchantInfo::CanRepair()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00968030/0x1406DE5A0
static int32_t Script_ShowRepairCursor(lua_State* L) {
    if (Spell_C_IsTargeting()) {
        return 0;
    }

    if (CGMerchantInfo::CanRepair()) {
        CGGameUI::ClearCursor(1, 1);
        CursorSetResetMode(17);
        CursorSetMode(17);
    }

    return 0;
}

// OFFSET: 0x009683C0/0x1406DE870
static int32_t Script_GetBuybackItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBuybackItemLink(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    uint32_t slot = index < CGMerchantInfo::m_numBuybackItems ? CGMerchantInfo::m_buybackSlots[index] : 0;

    if (!player || !slot || slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[slot]) {
        return 0;
    }

    CGItem_C* item = player->m_bag.GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    return 1;
}

// OFFSET: 0x009684A0/0x1406DE950
static int32_t Script_PickupMerchantItem(lua_State* L) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return 0;
    }

    WOWGUID cursorItem = CGGameUI::GetCursorItem();

    if (cursorItem) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item || !CGGameUI::m_stackSplit || item->m_item->m_itemFieldStackCount == CGGameUI::m_stackSplit) {
            CGPlayer_C::SellItem(CGMerchantInfo::m_merchant, cursorItem, CGGameUI::m_stackSplit);
            CGGameUI::ClearCursor(0, 1);
        }

        return 0;
    }

    int32_t index;
    VendorItem* item;

    if (!lua_isnumber(L, 1) || (item = CGMerchantInfo::GetItem(index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) == NULL || !item->m_slot || item->m_type != VENDOR_ITEM_TYPE_ITEM) {
        CGGameUI::ClearCursor(1, 1);
        return 0;
    }

    int32_t itemId = item->m_itemID;

    if (CGGameUI::GetCursorVirtualItem() != itemId) {
        CGGameUI::SetCursorVirtualItem(itemId, item->m_displayID, index, 5, 0);
    } else {
        CGGameUI::ClearCursor(1, 1);
    }

    return 0;
}

// OFFSET: 0x009685C0/0x1406DEAE0
static int32_t Script_BuyMerchantItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BuyMerchantItem(index [,quantity])");
        return 0;
    }

    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return 0;
    }

    CGGameUI::ClearCursor(1, 1);
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant) {
        return 0;
    }

    VendorItem* item = &CGMerchantInfo::m_items[index];

    if (!item || !item->m_slot) {
        return 0;
    }

    int32_t count = item->m_buyCount;

    if (!lua_isnumber(L, 2)) {
        if (count >= 255) {
            count = 255;
        }
    } else if (static_cast<int32_t>(lua_tonumber(L, 2)) < 1) {
        count = 1;
    } else {
        count = static_cast<int32_t>(lua_tonumber(L, 2));

        if (count >= 255) {
            count = 255;
        }
    }

    if (item->m_type == VENDOR_ITEM_TYPE_CURRENCY) {
        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_itemID);

        if (currency && (currency->m_flags & 0x8)) {
            count *= 100;
        }
    }

    CGPlayer_C::BuyItemInSlot(CGMerchantInfo::m_merchant, item, count, WOWGUID(), 255);
    return 0;
}

// OFFSET: 0x00968700/0x1406DEC60
static int32_t Script_BuybackItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BuybackItem(slot)");
        return 0;
    }

    WOWGUID merchant = CGMerchantInfo::m_merchant;

    if (!merchant) {
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1)));
    uint32_t slot = static_cast<uint32_t>(index - 1) < CGMerchantInfo::m_numBuybackItems ? CGMerchantInfo::m_buybackSlots[index - 1] : 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && slot && player->m_bag.GetItemGuid(slot)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_BUY_BACK_ITEM));
        msg.Put(merchant.m_guid);
        msg.Put(slot);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x00968800/0x1406DEDC0
static int32_t Script_ShowBuybackSellCursor(lua_State* L) {
    if (Spell_C_IsTargeting() || CursorGetResetMode() != 1) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ShowBuybackSellCursor(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    uint32_t slot = index < CGMerchantInfo::m_numBuybackItems ? CGMerchantInfo::m_buybackSlots[index] : 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && slot && player->m_bag.GetItemGuid(slot)) {
        uint32_t price = player->GetBuybackPrice(slot - 74);

        if (player->GetCoinage() >= price) {
            CursorSetMode(3);
            return 0;
        }

        CursorSetMode(29);
    }

    return 0;
}

// OFFSET: 0x00968910/0x1406DEED0
static int32_t Script_GetRepairAllCost(lua_State* L) {
    CGPlayer_C* player;

    if (!CGMerchantInfo::CanRepair() || (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) == NULL) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    CGBag_C* bag = &player->m_bag;
    uint32_t cost = 0;
    int32_t needsRepair = 0;

    for (uint32_t i = 0; i <= 18; i++) {
        CGItem_C* item = bag->GetItemPointer(i);

        if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
            cost += CGMerchantInfo::GetRepairCost(item);
            uint32_t durability = (item->m_item->m_itemFieldFlags & 0x8) ? 0 : item->m_item->m_itemFieldDurability;

            if (!(item->m_item->m_itemFieldFlags & 0x8) && durability < item->m_item->m_itemFieldMaxdurability) {
                needsRepair = 1;
            }
        }
    }

    for (uint32_t i = 23; i <= 38; i++) {
        CGItem_C* item = bag->GetItemPointer(i);

        if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
            cost += CGMerchantInfo::GetRepairCost(item);
            uint32_t durability = (item->m_item->m_itemFieldFlags & 0x8) ? 0 : item->m_item->m_itemFieldDurability;

            if (!(item->m_item->m_itemFieldFlags & 0x8) && durability < item->m_item->m_itemFieldMaxdurability) {
                needsRepair = 1;
            }
        }
    }

    for (uint32_t i = 19; i <= 22; i++) {
        CGItem_C* container = bag->GetItemPointer(i);

        if (!container || !container->GetBag()) {
            continue;
        }

        CGBag_C* containerBag = container->GetBag();

        for (uint32_t j = 0; j < static_cast<uint32_t>(containerBag->m_numSlots); j++) {
            CGItem_C* item = containerBag->GetItemPointer(j);

            if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
                cost += CGMerchantInfo::GetRepairCost(item);
                uint32_t durability = (item->m_item->m_itemFieldFlags & 0x8) ? 0 : item->m_item->m_itemFieldDurability;

                if (!(item->m_item->m_itemFieldFlags & 0x8) && durability < item->m_item->m_itemFieldMaxdurability) {
                    needsRepair = 1;
                }
            }
        }
    }

    lua_pushnumber(L, cost);

    if (needsRepair) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00968C10/0x1406DF200
static int32_t Script_RepairAllItems(lua_State* L) {
    if (!CGMerchantInfo::CanRepair()) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t cost = 0;
    int32_t guildBank = StringToBOOL(L, 1, 0);
    CGBag_C* bag = &player->m_bag;

    for (uint32_t i = 0; i <= 18; i++) {
        CGItem_C* item = bag->GetItemPointer(i);

        if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
            cost += CGMerchantInfo::GetRepairCost(item);
        }
    }

    for (uint32_t i = 23; i <= 38; i++) {
        CGItem_C* item = bag->GetItemPointer(i);

        if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
            cost += CGMerchantInfo::GetRepairCost(item);
        }
    }

    for (uint32_t i = 19; i <= 22; i++) {
        CGItem_C* container = bag->GetItemPointer(i);

        if (!container || !container->GetBag()) {
            continue;
        }

        CGBag_C* containerBag = container->GetBag();

        for (uint32_t j = 0; j < static_cast<uint32_t>(containerBag->m_numSlots); j++) {
            CGItem_C* item = containerBag->GetItemPointer(j);

            if (item && (item->m_obj->m_type & TYPEMASK_ITEM) && !(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldMaxdurability) {
                cost += CGMerchantInfo::GetRepairCost(item);
            }
        }
    }

    if (!guildBank && cost > player->GetCoinage()) {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        return 0;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REPAIR_ITEM));
    msg.Put(CGMerchantInfo::m_merchant.m_guid);
    msg.Put(static_cast<uint64_t>(0));
    msg.Put(static_cast<uint8_t>(guildBank != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00968FC0/0x1406DF650
static int32_t Script_GetMerchantItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    VendorItem* item;

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant || (item = &CGMerchantInfo::m_items[index]) == NULL || !item->m_itemID) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        return 7;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    ItemRecSparse* stats = NULL;
    int32_t usable = 1;
    char texture[260];

    if (item->m_type == VENDOR_ITEM_TYPE_ITEM) {
        stats = CGMerchantInfo::GetItemStats(item->m_itemID);

        if (stats) {
            lua_pushstring(L, stats->GetName(0));

            if (stats->m_requiredLevel && player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) < stats->m_requiredLevel) {
                usable = 0;
            }
        } else {
            lua_pushnil(L);
        }

        const char* path = ClientDBStringLookup(3);
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayID));
        lua_pushstring(L, texture);
    } else {
        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_itemID);

        if (!currency) {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 1.0);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 1.0);
            lua_pushnil(L);
            return 7;
        }

        lua_pushstring(L, currency->m_name);
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
        lua_pushstring(L, texture);
    }

    lua_pushnumber(L, item->m_price);
    lua_pushnumber(L, item->m_buyCount);
    lua_pushnumber(L, item->m_maxCount);
    WOWGUID none = WOWGUID();
    ItemExtendedCostRec* cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, none, NULL, NULL, false);

    if (cost) {
        if (cost->m_requiredArenaRating && static_cast<uint32_t>(cost->m_arenaBracket) <= 2 && !CGArenaTeamInfo::CheckPersonalAndTeamRating(cost->m_requiredArenaRating, cost->m_arenaBracket)) {
            usable = 0;
        }

        if (cost->m_requiredFactionID && GetRepListRepValue(cost->m_requiredFactionID) < CGReputationInfo::m_reactionThresholds[cost->m_requiredFactionStanding]) {
            usable = 0;
        }

        if ((cost->m_flags & 0x1) && !player->GetGuildGUID()) {
            usable = 0;
        }

        if (static_cast<int32_t>(player->m_player->m_playerGuildlevel) < cost->m_requiredGuildLevel && cost->m_requiredGuildLevel > 0) {
            usable = 0;
        }

        if (cost->m_requiredAchievement && !CGAchievementInfo::GetAchievement(cost->m_requiredAchievement)) {
            usable = 0;
        }
    }

    if (item->m_playerConditionID) {
        usable = 0;
    }

    int32_t error;

    if (item->m_type == VENDOR_ITEM_TYPE_ITEM ? (!player || !stats || player->CanUseItem(stats, error)) && usable : usable) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (cost) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 7;
}

// OFFSET: 0x009693E0/0x1406DFB40
static int32_t Script_GetMerchantItemCostInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemCostInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    VendorItem* item;
    ItemExtendedCostRec* cost;
    WOWGUID none;

    if (index >= 0 && index < CGMerchantInfo::m_numMerchantItems && CGMerchantInfo::m_merchant && (item = &CGMerchantInfo::m_items[index]) != NULL && item->m_itemID && (none = WOWGUID::Make(0), (cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, none, NULL, NULL, false)) != NULL)) {
        uint32_t count = 0;

        for (uint32_t i = 0; i < 5; i++) {
            if (cost->m_itemID[i]) {
                count++;
            }
        }

        for (uint32_t i = 0; i < 5; i++) {
            if (cost->m_currencyID[i] && !(s_costCurrencyFlags[i] & cost->m_flags)) {
                count++;
            }
        }

        lua_pushnumber(L, count);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00969540/0x1406DFC70
static int32_t Script_GetMerchantItemCostItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetMerchantItemCostItem(index, itemIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t costIndex = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    VendorItem* item;
    ItemExtendedCostRec* cost;
    WOWGUID none;

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant || (item = &CGMerchantInfo::m_items[index]) == NULL || !item->m_itemID || (none = WOWGUID::Make(0), (cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, none, NULL, NULL, false)) == NULL)) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    int32_t found = 0;

    for (uint32_t i = 0; i < 5; i++) {
        if (!cost->m_itemID[i]) {
            continue;
        }

        WOWGUID itemGuid = WOWGUID::Make(0);
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(cost->m_itemID[i], itemGuid, MerchantUpdateCallback, NULL, true);
        ItemRec* rec;

        if (!stats || (rec = stats->GetNonSparse()) == NULL) {
            lua_pushnil(L);
            lua_pushnumber(L, 0.0);
            return 2;
        }

        if (found == costIndex) {
            const char* path = ClientDBStringLookup(3);
            char texture[260];
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
            lua_pushstring(L, texture);
            lua_pushnumber(L, cost->m_itemCount[i]);
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(cost->m_itemID[i], stats->m_quality, 0, 0, 0, 0));
            return 3;
        }

        found++;
    }

    for (uint32_t i = 0; i < 5; i++) {
        int32_t currencyId = cost->m_currencyID[i];

        if (!currencyId) {
            continue;
        }

        CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId);

        if (s_costCurrencyFlags[i] & cost->m_flags) {
            continue;
        }

        if (found == costIndex) {
            const char* path = ClientDBStringLookup(3);
            char texture[260];
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
            lua_pushstring(L, texture);
            int32_t count = cost->m_currencyCount[i];

            if (currency->m_flags & 0x8) {
                count /= 100;
            }

            lua_pushnumber(L, count);
            lua_pushnil(L);
            lua_pushstring(L, currency->m_name);
            return 4;
        }

        found++;
    }

    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x00969860/0x1406E0050
static int32_t Script_GetMerchantCurrencies(lua_State* L) {
    int32_t count = 0;
    int32_t currencies[6];

    if (CGMerchantInfo::m_numMerchantItems <= 0 || !CGMerchantInfo::m_merchant) {
        return 0;
    }

    VendorItem* item = CGMerchantInfo::m_items;

    for (int32_t i = 0; item && item->m_itemID && count < 6; i++) {
        if (item->m_type == VENDOR_ITEM_TYPE_CURRENCY) {
            int32_t currencyId = item->m_itemID;
            int32_t j = 0;

            for (; j < count && currencies[j] != currencyId; j++) {
            }

            if (j >= count) {
                currencies[count++] = currencyId;
                lua_pushnumber(L, currencyId);

                if (count >= 6) {
                    return count;
                }
            }
        }

        WOWGUID none = WOWGUID();
        ItemExtendedCostRec* cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, none, NULL, NULL, false);

        if (cost) {
            for (uint32_t k = 0; k < 5; k++) {
                int32_t currencyId = cost->m_currencyID[k];

                if (!currencyId) {
                    continue;
                }

                int32_t j = 0;

                for (; j < count && currencies[j] != currencyId; j++) {
                }

                if (j >= count) {
                    currencies[count++] = currencyId;
                    lua_pushnumber(L, currencyId);

                    if (count >= 6) {
                        break;
                    }
                }
            }
        }

        item++;

        if (item < CGMerchantInfo::m_items || i + 1 >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant) {
            break;
        }
    }

    return count;
}

// OFFSET: 0x009699A0/0x1406E01E0
static int32_t Script_GetBuybackItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBuybackItemInfo(slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    uint32_t slot = index < CGMerchantInfo::m_numBuybackItems ? CGMerchantInfo::m_buybackSlots[index] : 0;
    CGItem_C* item;
    ItemRecSparse* stats;
    WOWGUID none;

    if (player && slot && slot < static_cast<uint32_t>(player->m_bag.m_numSlots) && player->m_bag.m_slots[slot] && (item = player->m_bag.GetItemPointer(slot)) != NULL && (none = WOWGUID::Make(0), (stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, none, MerchantUpdateCallback, NULL, false)) != NULL)) {
        char buffer[260];
        item->GetFullItemName(buffer, sizeof(buffer));
        lua_pushstring(L, buffer);
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
        lua_pushstring(L, buffer);
        lua_pushnumber(L, player->GetBuybackPrice(slot - 74));
        lua_pushnumber(L, item->m_item->m_itemFieldStackCount);
        lua_pushnumber(L, 0.0);
        int32_t error;

        if (!player->CanUseItem(stats, error)) {
            lua_pushnil(L);
            return 6;
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 0.0);
    }

    lua_pushnumber(L, 1.0);
    return 6;
}

// OFFSET: 0x00969BF0/0x1406E0490
static int32_t Script_GetMerchantItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemLink(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant) {
        return 0;
    }

    VendorItem* item = &CGMerchantInfo::m_items[index];

    if (!item || item->m_type != VENDOR_ITEM_TYPE_ITEM || !item->m_itemID) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_itemID, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item->m_itemID, stats->m_quality, 0, 0, 0, 0));
    return 1;
}

// OFFSET: 0x00969CD0/0x1406E05A0
static int32_t Script_GetMerchantItemMaxStack(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemMaxStack(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    VendorItem* item;
    ItemRecSparse* stats;
    WOWGUID none;
    double maxStack;

    if (index >= 0 && index < CGMerchantInfo::m_numMerchantItems && CGMerchantInfo::m_merchant && (item = &CGMerchantInfo::m_items[index]) != NULL && item->m_type == VENDOR_ITEM_TYPE_ITEM && item->m_itemID && (none = WOWGUID::Make(0), (stats = g_itemSparseDB.GetRecord(item->m_itemID, none, NULL, NULL, false)) != NULL)) {
        maxStack = stats->m_stackable;
    } else {
        maxStack = 1.0;
    }

    lua_pushnumber(L, maxStack);
    return 1;
}

// OFFSET: 0x00969D90/0x1406E0680
static int32_t Script_ShowMerchantSellCursor(lua_State* L) {
    // STATIC: 0x00BFE2EC
    static const uint32_t s_currencyFlags[5] = { 0x2, 0x4, 0x8, 0x10, 0x20 };

    if (Spell_C_IsTargeting() || CursorGetResetMode() != 1) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ShowMerchantSellCursor(index)");
        return 0;
    }

    VendorItem* item = CGMerchantInfo::GetItem(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (!item || !item->m_itemID) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (!player->GetCoinage() < static_cast<uint32_t>(item->m_price)) {
        if (item->m_type == VENDOR_ITEM_TYPE_ITEM) {
            WOWGUID none = WOWGUID::Make(0);
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_itemID, none, NULL, NULL, false);

            if (!stats) {
                CursorSetMode(3);
                return 0;
            }

            if (stats->m_requiredReputationFaction && GetRepListRepValue(stats->m_requiredReputationFaction) < CGReputationInfo::m_reactionThresholds[stats->m_requiredReputationRank]) {
                CursorSetMode(29);
                return 0;
            }
        }

        WOWGUID none = WOWGUID::Make(0);
        ItemExtendedCostRec* cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, none, NULL, NULL, false);

        if (!cost) {
            CursorSetMode(3);
            return 0;
        }

        int32_t missing = 0;

        for (uint32_t i = 0; i < 5; i++) {
            int32_t itemId = cost->m_itemID[i];

            if (!itemId || cost->m_itemCount[i] <= 0) {
                continue;
            }

            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRec* rec = g_itemDB.GetRecord(itemId, itemGuid, NULL, NULL, false);

            if (rec && rec->m_classID != 10 && player->m_bag.GetItemTypeCount(cost->m_itemID[i], 0) < cost->m_itemCount[i]) {
                missing = 1;
                break;
            }
        }

        for (uint32_t i = 0; i < 5; i++) {
            int32_t currencyId = cost->m_currencyID[i];

            if (!currencyId) {
                continue;
            }

            if (!((s_currencyFlags[i] & cost->m_flags) ? CGCurrencyTypes::HasEarnedCurrency(currencyId, cost->m_currencyCount[i]) : CGCurrencyTypes::HasCurrency(currencyId, cost->m_currencyCount[i]))) {
                missing = 1;
                break;
            }
        }

        if (cost->m_requiredArenaRating && static_cast<uint32_t>(cost->m_arenaBracket) <= 2 && !CGArenaTeamInfo::CheckPersonalAndTeamRating(cost->m_requiredArenaRating, cost->m_arenaBracket)) {
            missing = 1;
        }

        CursorSetMode(missing ? 29 : 3);
        return 0;
    }

    CursorSetMode(29);
    return 0;
}

// STATIC: 0x00D131D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseMerchant", &Script_CloseMerchant },
    { "GetMerchantNumItems", &Script_GetMerchantNumItems },
    { "GetMerchantItemInfo", &Script_GetMerchantItemInfo },
    { "GetMerchantItemCostInfo", &Script_GetMerchantItemCostInfo },
    { "GetMerchantItemCostItem", &Script_GetMerchantItemCostItem },
    { "GetMerchantCurrencies", &Script_GetMerchantCurrencies },
    { "GetBuybackItemInfo", &Script_GetBuybackItemInfo },
    { "GetBuybackItemLink", &Script_GetBuybackItemLink },
    { "GetMerchantItemLink", &Script_GetMerchantItemLink },
    { "GetMerchantItemMaxStack", &Script_GetMerchantItemMaxStack },
    { "PickupMerchantItem", &Script_PickupMerchantItem },
    { "BuyMerchantItem", &Script_BuyMerchantItem },
    { "BuybackItem", &Script_BuybackItem },
    { "CanMerchantRepair", &Script_CanMerchantRepair },
    { "ShowMerchantSellCursor", &Script_ShowMerchantSellCursor },
    { "ShowBuybackSellCursor", &Script_ShowBuybackSellCursor },
    { "ShowRepairCursor", &Script_ShowRepairCursor },
    { "HideRepairCursor", &Script_HideRepairCursor },
    { "InRepairMode", &Script_InRepairMode },
    { "GetRepairAllCost", &Script_GetRepairAllCost },
    { "RepairAllItems", &Script_RepairAllItems },
    { "GetNumBuybackItems", &Script_GetNumBuybackItems },
};

// OFFSET: 0x00967E70/0x1406DE380
void MerchantRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00967EA0/0x1406DE3D0
void MerchantUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
