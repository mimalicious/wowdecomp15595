#include "ContainerFrame.h"
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/Reforge.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/Transmogrify.h"
#include "WoW/Source/Ui/VoidStorageInfo.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x009BB330/0x140746B60
static int32_t Script_ContainerIDToInventoryID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ContainerIDToInventoryID(containerID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(id) > 10) {
        luaL_error(L, "ContainerIDToInventoryID(): invalid container ID");
        return 0;
    }

    lua_pushnumber(L, id >= 4 ? id + 64 : id + 20);
    return 1;
}

// OFFSET: 0x009BB3C0/0x140746BF0
static int32_t Script_GetMaxArenaCurrency(lua_State* L) {
    lua_pushnumber(L, 10000.0);
    return 1;
}

// OFFSET: 0x009BB3F0/0x140746C10
static int32_t Script_SetItemSearch(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetItemSearch(search)");
        return 0;
    }

    CGContainerInfo::SetItemSearch(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x009BB590/0x140746DF0
static int32_t Script_SetBagPortraitTexture(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetBagPortraitTexture(texture, slot)");
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (slot < 0) {
        return 0;
    }

    if (slot >= 11) {
        luaL_error(L, "Invalid slot in SetBagPortraitTexture");
        return 0;
    }

    texture->SetTexture(static_cast<CTexture*>(NULL));
    CGItem_C* bag = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(slot), TYPEMASK_CONTAINER, __FILE__, __LINE__));

    if (!bag) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);
    char fileName[260];
    SStrPrintf(fileName, sizeof(fileName), "%s%s%s", path, *path ? "\\" : "", bag->GetInventoryArt());
    SetPortraitTexture(texture, fileName);
    return 0;
}

// OFFSET: 0x009BB820/0x1407470E0
static int32_t Script_GetContainerNumSlots(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetContainerNumSlots(index)");
        return 0;
    }

    double slots;

    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        slots = 0.0;
    } else {
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

        if (!index) {
            slots = 16.0;
        } else if (index == -1) {
            slots = 28.0;
        } else {
            CGContainer_C* container;

            if (static_cast<uint32_t>(index - 1) <= 10 && (container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__))) != NULL) {
                slots = static_cast<uint32_t>(container->GetBag()->m_numSlots);
            } else {
                slots = 0.0;
            }
        }
    }

    lua_pushnumber(L, slots);
    return 1;
}

// OFFSET: 0x009BB900/0x1407471F0
static int32_t Script_GetContainerNumFreeSlots(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetContainerFreeSlots(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!index) {
        int32_t free = 0;

        for (uint32_t slot = 23; slot <= 38; slot++) {
            if (slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[slot]) {
                free++;
            }
        }

        lua_pushnumber(L, free);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    if (index == -1) {
        if (CGBankInfo::m_banker) {
            int32_t free = 0;

            for (uint32_t slot = 39; slot <= 66; slot++) {
                if (slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[slot]) {
                    free++;
                }
            }

            lua_pushnumber(L, free);
            lua_pushnumber(L, 0.0);
            return 2;
        }
    } else if (static_cast<uint32_t>(index - 1) <= 10) {
        if (CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__))) {
            lua_pushnumber(L, container->GetFreeSlots());
            lua_pushnumber(L, container->GetBagFamily());
            return 2;
        }
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x009BBB90/0x1407473B0
static int32_t Script_GetContainerFreeSlots(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetContainerFreeSlots(index [, returnTable])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (lua_type(L, 2) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    uint32_t count = 1;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!index) {
        for (int32_t i = 23; i <= 38; i++) {
            if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[i]) {
                lua_checkstack(L, 2);
                lua_pushnumber(L, count);
                lua_pushnumber(L, i - 22);
                lua_settable(L, -3);
                count++;
            }
        }

        return 1;
    }

    if (index == -1) {
        if (!CGBankInfo::m_banker) {
            return 0;
        }

        for (int32_t i = 39; i <= 66; i++) {
            if (static_cast<uint32_t>(i) >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[i]) {
                lua_checkstack(L, 2);
                lua_pushnumber(L, count);
                lua_pushnumber(L, i - 38);
                lua_settable(L, -3);
                count++;
            }
        }

        return 1;
    }

    if (static_cast<uint32_t>(index - 1) > 10) {
        return 0;
    }

    CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__));

    if (!container) {
        return 0;
    }

    CGBag_C* bag = container->GetBag();

    if (!bag) {
        return 0;
    }

    for (uint32_t slot = 0; slot < static_cast<uint32_t>(bag->m_numSlots); slot++) {
        if (!bag->GetItemGuid(slot)) {
            lua_checkstack(L, 2);
            lua_pushnumber(L, count);
            lua_pushnumber(L, slot + 1);
            lua_settable(L, -3);
            count++;
        }
    }

    return 1;
}

// OFFSET: 0x009BBDF0/0x140747650
static int32_t Script_GetContainerItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemLink(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    return 1;
}

// OFFSET: 0x009BBE70/0x1407476E0
static int32_t Script_GetContainerItemID(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemID(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
    return 1;
}

// OFFSET: 0x009BBF00/0x140747770
static int32_t Script_GetContainerItemCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemCooldown(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;
    CGItem_C* item;

    if (CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank) && (item = bag->GetItemPointer(slot)) != NULL && (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges() || (item->GetUsableSpellCharges() < 0 && !item->GetCurrentSpellCharges()))) {
        uint32_t duration = 0;
        uint32_t start = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        lua_pushnumber(L, start * 0.001);
        lua_pushnumber(L, duration * 0.001);
        lua_pushnumber(L, enable);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// OFFSET: 0x009BC060/0x140747900
static int32_t Script_GetContainerItemDurability(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemDurability(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    uint32_t maxDurability = (item->m_item->m_itemFieldFlags & 0x8) ? 0 : item->m_item->m_itemFieldMaxdurability;
    uint32_t durability = (item->m_item->m_itemFieldFlags & 0x8) ? 0 : item->m_item->m_itemFieldDurability;

    if (!maxDurability) {
        return 0;
    }

    lua_pushnumber(L, durability);
    lua_pushnumber(L, maxDurability);
    return 2;
}

// OFFSET: 0x009BC160/0x140747A00
static int32_t Script_SplitContainerItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: SplitContainerItem(index, slot, amount)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!player || (!CGGameUI::m_hasControl && !(static_cast<uint8_t>(player->m_unit->m_unitFieldFlags >> 20) & 1)) || !CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);
    WOWGUID itemGuid = item ? item->m_obj->m_guid : WOWGUID();

    if (!item || (item->m_itemFlags & 0x1)) {
        return 0;
    }

    int32_t amount = static_cast<int32_t>(lua_tonumber(L, 3));

    if (amount < 1 || amount > item->m_item->m_itemFieldStackCount) {
        return 0;
    }

    CGGameUI::ClearCursor(1, 1);
    Spell_C_StopTargeting();

    if (amount == item->m_item->m_itemFieldStackCount) {
        CGGameUI::SetCursorItem(itemGuid, bag->m_guid, slot, 1, 0, 1);
    } else {
        CGGameUI::SetCursorItem(itemGuid, bag->m_guid, slot, 1, amount, 1);
    }

    CGGameUI::LockItem(itemGuid);
    return 0;
}

// OFFSET: 0x009BC2C0/0x140747BA0
static int32_t Script_SocketContainerItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SocketContainerItem(index, slot)");
        return 0;
    }

    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item || (item->m_itemFlags & 0x1) || !item->GetNumSockets()) {
        return 0;
    }

    CGItemSocketInfo::SetItem(item->m_obj->m_guid);
    return 0;
}

// OFFSET: 0x009BC380/0x140747C70
static int32_t Script_ShowContainerSellCursor(lua_State* L) {
    if (Spell_C_IsTargeting() || CursorGetResetMode() != 1) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ShowContainerSellCursor(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item || (item->m_itemFlags & 0x1)) {
        return 0;
    }

    CursorSetMode(3);
    return 0;
}

// OFFSET: 0x009BC420/0x140747D10
static int32_t Script_GetContainerItemPurchaseInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemPurchaseInfo(index, slot[, isEquipped])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGItem_C* item;

    if (static_cast<int32_t>(lua_tonumber(L, 3))) {
        CGBag_C* bag = &static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_bag;
        int32_t slot = static_cast<int32_t>(lua_tonumber(L, 2));

        if (!bag || static_cast<uint32_t>(slot - 1) > 18) {
            return 0;
        }

        item = bag->GetItemPointer(slot - 1);
    } else {
        CGBag_C* bag;
        int32_t slot;
        int32_t bank;

        if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
            return 0;
        }

        item = bag->GetItemPointer(slot);
    }

    if (!item || CGLootInfo::m_lootObj == item->m_obj->m_guid) {
        return 0;
    }

    CGItem_C::ItemPurchaseData* data = item->m_itemPurchaseData;

    if (!data) {
        return 0;
    }

    int32_t timeLeft = data->m_refundTimeLeft - player->GetPlayedTime() + 7200;

    if (timeLeft <= 0) {
        return 0;
    }

    int32_t itemCount = static_cast<int32_t>(data->m_itemCount[0]) > 0;

    for (uint32_t i = 1; i < 5; i++) {
        if (static_cast<int32_t>(data->m_itemCount[i]) > 0) {
            itemCount++;
        }
    }

    lua_pushnumber(L, data->m_money);
    lua_pushnumber(L, itemCount);
    lua_pushnumber(L, timeLeft);
    int32_t currencyCount = static_cast<int32_t>(data->m_currencyCount[0]) > 0;

    for (uint32_t i = 1; i < 5; i++) {
        if (static_cast<int32_t>(data->m_currencyCount[i]) > 0) {
            currencyCount++;
        }
    }

    lua_pushnumber(L, currencyCount);
    lua_pushboolean(L, item->HasEnchantments());
    return 5;
}

// OFFSET: 0x009BC630/0x140747F50
static int32_t Script_GetContainerItemPurchaseCurrency(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: Script_GetContainerItemPurchaseCurrency(bag, slot, itemIndex[, isEquipped])");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t slot;
    CGBag_C* bag = NULL;

    if (static_cast<int32_t>(lua_tonumber(L, 4))) {
        bag = &ClntObjMgrGetActivePlayerObj()->m_bag;
        slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

        if (!bag || slot > 18) {
            return 0;
        }
    } else {
        int32_t bagSlot;
        int32_t bank;

        if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, bagSlot, bank)) {
            return 0;
        }

        slot = bagSlot;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item || CGLootInfo::m_lootObj == item->m_obj->m_guid) {
        return 0;
    }

    CGItem_C::ItemPurchaseData* data = item->m_itemPurchaseData;

    if (!data || static_cast<int32_t>(data->m_refundTimeLeft - player->GetPlayedTime() + 7200) <= 0) {
        return 0;
    }

    int32_t wanted = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;
    int32_t found = 0;

    for (uint32_t i = 0; i < 5; i++) {
        if (static_cast<int32_t>(data->m_currencyId[i]) <= 0) {
            continue;
        }

        if (found == wanted) {
            CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(data->m_currencyId[i]);
            lua_pushstring(L, CGCurrencyTypes::GetCurrencyIcon(currency));
            lua_pushnumber(L, static_cast<int32_t>(data->m_currencyCount[i]));
            lua_pushstring(L, currency->m_name);
            return 3;
        }

        found++;
    }

    return 0;
}

// OFFSET: 0x009BC7E0/0x1407481A0
static int32_t Script_ContainerRefundItemPurchase(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ContainerRefundItemPurchase(index, slot[, isEquipped])");
        return 0;
    }

    CGBag_C* bag = NULL;
    uint32_t slot;

    if (static_cast<int32_t>(lua_tonumber(L, 3))) {
        bag = &static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_bag;
        slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

        if (!bag || slot > 18) {
            CGGameUI::DisplayError(ERR_INTERNAL_BAG_ERROR);
            return 0;
        }
    } else {
        int32_t bagSlot;
        int32_t bank;

        if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, bagSlot, bank)) {
            CGGameUI::DisplayError(ERR_INTERNAL_BAG_ERROR);
            return 0;
        }

        slot = bagSlot;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (item && item->m_itemPurchaseData && CGMerchantInfo::m_merchant) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_ITEM_PURCHASE_REFUND));
        msg.Put(item->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return 0;
    }

    CGGameUI::DisplayError(ERR_INTERNAL_BAG_ERROR);
    return 0;
}

// OFFSET: 0x009BC930/0x1407483C0
static int32_t Script_GetContainerItemGems(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemGems(bag, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        if (CGItem_C* item = bag->GetItemPointer(slot)) {
            for (int32_t i = 0; i < 3; i++) {
                int32_t enchantId = (item->GetItemStaticFlags(0) & 0x2000) ? 0 : item->m_item->m_itemFieldEnchantment[2 + i].m_id;
                SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);
                int32_t gemId;

                if (enchant && (gemId = enchant->m_srcItemID) != 0) {
                    lua_pushnumber(L, gemId);
                } else {
                    lua_pushnil(L);
                }
            }
        }
    }

    return 3;
}

// OFFSET: 0x009BCA30/0x1407484E0
static int32_t Script_GetContainerItemEquipmentSetInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemEquipmentSetInfo(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;
    CGItem_C* item;
    char list[260];

    if (CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank) && (item = bag->GetItemPointer(slot)) != NULL && FrameScript_GetText("EQUIPMENT_SETS", -1, GENDER_NOT_APPLICABLE) && CGEquipmentManager::GetSetList(list, sizeof(list), item->m_obj->m_guid)) {
        lua_pushboolean(L, 1);
        lua_pushstring(L, list);
        return 2;
    }

    lua_pushboolean(L, 0);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x009BCB30/0x140748620
static int32_t Script_PlayerHasHearthstone(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item;

    if (player && (item = player->m_bag.FindItemWithSpellCategory(1176, 0)) != NULL) {
        lua_pushnumber(L, static_cast<int32_t>(item->m_obj->m_entryID));
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009BCBA0/0x1407486A0
static int32_t Script_UseHearthstone(lua_State* L) {
    if (!CGGameUI::CanPerformAction(2)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGItem_C* item = player->m_bag.FindItemWithSpellCategory(1176, 0);

    if (!item) {
        return 0;
    }

    WOWGUID target = WOWGUID::Make(0);
    lua_pushboolean(L, item->UseItem(&target, 0));
    return 1;
}

// OFFSET: 0x009BCC30/0x140748730
static int32_t Script_GetItemCooldown(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetItemCooldown(itemID)");
        return 0;
    }

    int32_t itemId = static_cast<int32_t>(lua_tonumber(L, 1));
    CGItem_C* item;

    if (player && (item = player->m_bag.FindItemOfType(itemId, 0)) != NULL && (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges() || (item->GetUsableSpellCharges() < 0 && !item->GetCurrentSpellCharges()))) {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        lua_pushnumber(L, start * 0.001);
        lua_pushnumber(L, duration * 0.001);
        lua_pushnumber(L, enable);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// OFFSET: 0x009BD5A0/0x140749200
static int32_t Script_PickupContainerItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: PickupContainerItem(index, slot)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!player || (!CGGameUI::m_hasControl && !(static_cast<uint8_t>(player->m_unit->m_unitFieldFlags >> 20) & 1)) || !CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    if (bag->m_hasBank && ((slot >= 39 && slot <= 66) || static_cast<uint32_t>(slot - 67) <= 6) && !CGBankInfo::m_banker) {
        return 0;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
    int32_t virtualId;
    int32_t virtualSlot;
    CGGameUI::GetCursorVirtualItem(virtualId, virtualSlot);
    CGItem_C* item = bag->GetItemPointer(slot);
    WOWGUID itemGuid = item ? item->m_obj->m_guid : WOWGUID();

    if (cursorItem) {
        if (itemGuid != cursorItem) {
            if (CGGameUI::m_stackSplit) {
                player->SplitItem(cursorItem, cursorContainer, cursorSlot, bag->m_guid, slot, CGGameUI::m_stackSplit);
                CGGameUI::ClearCursor(0, 1);
            } else if ((!itemGuid || cursorContainer) && (!item || !(item->m_itemFlags & 0x1))) {
                CGGameUI::LockItem(itemGuid);
                player->SwapItems(cursorItem, cursorContainer, cursorSlot, bag->m_guid, slot, 0);
                return 0;
            }

            return 0;
        }

        CGGameUI::ClearCursor(1, 1);
        return 0;
    }

    if (virtualId) {
        if (CGGameUI::m_cursorItemType == 5) {
            WOWGUID merchant = CGMerchantInfo::m_merchant;

            if (merchant) {
                if (VendorItem* vendorItem = CGMerchantInfo::GetItem(virtualSlot)) {
                    CGPlayer_C::BuyItemInSlot(merchant, vendorItem, vendorItem->m_buyCount, bag->m_guid, slot);
                    CGGameUI::ClearCursor(1, 1);
                    return 0;
                }
            }

            return 0;
        }

        if (CGGameUI::m_cursorItemType == 10) {
            if (!CGGuildBankInfo::m_banker) {
                return 0;
            }

            int32_t bankItemId = 0;
            int32_t bankSlot = 0;
            CGGameUI::GetCursorVirtualItem(bankItemId, bankSlot);

            if (!virtualId) {
                return 0;
            }

            uint8_t bagIndex = player->FindSlotIndex(bag->m_guid);
            uint32_t count = 0;
            WOWGUID requester = WOWGUID();
            ItemRecSparse* rec = g_itemSparseDB.GetRecord(bankItemId, requester, NULL, NULL, false);
            int32_t itemId = item ? item->m_obj->m_entryID : 0;
            int32_t maxStack;

            if (rec && (maxStack = rec->m_stackable) != 0 && (!itemId || bankItemId == itemId)) {
                if (!itemId) {
                    count = CGGameUI::m_stackSplit;
                } else {
                    int32_t room = maxStack - item->m_item->m_itemFieldStackCount;

                    if (CGGameUI::m_stackSplit) {
                        count = CGGameUI::m_stackSplit < room ? CGGameUI::m_stackSplit : room;
                    } else if (CGGameUI::GetVirtualItemCount(bankItemId, bankSlot, 0) < room) {
                        count = CGGameUI::GetVirtualItemCount(bankItemId, bankSlot, 0);
                    } else {
                        count = maxStack - item->m_item->m_itemFieldStackCount;
                    }
                }
            } else if (virtualId == itemId) {
                CGGameUI::ClearCursor(1, 1);
                CGGameUI::UnlockVirtualItem(bankItemId, bankSlot, 0);
                return 0;
            }

            player->SwapGuildBankItem(bankItemId, bankSlot, bagIndex, slot, count);
        }

        CGGameUI::ClearCursor(1, 1);
        return 0;
    }

    if (!item) {
        return 0;
    }

    if (Spell_C_IsTargeting()) {
        Spell_C_StopTargeting();
    }

    if (CursorGetResetMode() != 17 || !CGMerchantInfo::CanRepair()) {
        if (CGPlayer_C::IsGiftWrapping()) {
            if (CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
                activePlayer->GiftWrap(item);
                return 0;
            }
        } else if (!(item->m_itemFlags & 0x1)) {
            CGGameUI::SetCursorItem(itemGuid, bag->m_guid, slot, 1, 0, 1);
            CGGameUI::LockItem(itemGuid);
            return 0;
        }

        return 0;
    }

    uint32_t cost = CGMerchantInfo::GetRepairCost(item);

    if (cost <= player->GetCoinage()) {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("ITEM_REPAIR", NULL, &properties, NULL, __FILE__, __LINE__, "Script_PickupContainerItem()");
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REPAIR_ITEM));
        msg.Put(CGMerchantInfo::m_merchant.m_guid);
        msg.Put(itemGuid.m_guid);
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    } else {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
    }

    return 0;
}

// OFFSET: 0x009BDA90/0x1407498A0
static int32_t Script_UseContainerItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UseContainerItem(index, slot[, target])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    WOWGUID target;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 3, NULL), target, true, false)) {
        return 0;
    }

    CGGameUI::ClearCursor(1, 1);
    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item || (item->m_itemFlags & 0x1)) {
        return 0;
    }

    WOWGUID guid = item->m_obj->m_guid;
    int32_t itemId = item->m_obj->m_entryID;

    if (Spell_C_IsTargeting() && !Spell_C_CanTargetItems()) {
        Spell_C_StopTargeting();
    }

    if (Spell_C_IsTargeting()) {
        CGGameUI::CastItemSpell(guid, 0);
        return 0;
    }

    if (CursorGetResetMode() == 17 && CGMerchantInfo::CanRepair()) {
        uint32_t cost = CGMerchantInfo::GetRepairCost(item);

        if (cost <= player->GetCoinage()) {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("ITEM_REPAIR", NULL, &properties, NULL, __FILE__, __LINE__, "Script_UseContainerItem");
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_REPAIR_ITEM));
            msg.Put(CGMerchantInfo::m_merchant.m_guid);
            msg.Put(guid.m_guid);
            msg.Put(static_cast<uint8_t>(0));
            msg.Finalize();
            ClientServices::Send2(&msg);
        } else {
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        }

        return 0;
    }

    if (bank) {
        player->AutoStoreBankItem(bag->m_guid, slot);
        CGGameUI::SetPendingItemSound(guid, itemId);
        CGGameUI::LockItem(guid);
        return 0;
    }

    if (CGBankInfo::m_banker) {
        player->AutoBankItem(bag->m_guid, slot);
        CGGameUI::SetPendingItemSound(guid, itemId);
        CGGameUI::LockItem(guid);
        return 0;
    }

    if (CGMerchantInfo::m_merchant) {
        CGPlayer_C::SellItem(CGMerchantInfo::m_merchant, guid, 0);
        CGGameUI::LockItem(guid);
        return 0;
    }

    if (CGReforge::m_forgeMaster) {
        CGReforge::SetItem(guid);
        return 0;
    }

    if (CGTransmogrify::m_transmogrifier) {
        CGTransmogrify::AutoAddItem(item);
        return 0;
    }

    if (CGVoidStorageInfo::m_npc) {
        CGVoidStorageInfo::AutoDepositItem(item);
        return 0;
    }

    if (CGMailInfo::m_sendMailShowing && CGMailInfo::m_object) {
        CGMailInfo::AddItemToOutgoingMail(-1, guid, bag->m_guid, slot);
        return 0;
    }

    if (CGAuctionHouse::m_auctionsTabShowing && CGAuctionHouse::m_auctioneer) {
        if (!CGAuctionHouse::IsSellItemValid(item, 1)) {
            return 0;
        }

        WOWGUID sellItem;
        WOWGUID sellContainer;
        uint8_t sellSlot;
        CGAuctionHouse::GetSellItem(&sellItem, &sellContainer, &sellSlot);

        if (sellItem) {
            CGGameUI::UnlockItem(sellItem);
        }

        CGGameUI::LockItem(guid);
        CGAuctionHouse::SetSellItem(guid, bag->m_guid, slot);
        FrameScript_SignalEvent(SCRIPT_EVENT_NEW_AUCTION_UPDATE, NULL);
        return 0;
    }

    if (CGTradeInfo::m_tradingPlayer) {
        int32_t enchant;

        if (item->GetEnchantmentSpell(53) || item->GetEnchantmentSpell(54) || item->GetEnchantmentSpell(156) || (enchant = item->GetEnchantmentSpell(33)) != 0) {
            enchant = 1;
        }

        if (CGTradeInfo::m_enchantingItem && enchant) {
            CGTradeInfo::SetEnchantingItem(item);
        } else {
            CGTradeInfo::AddPlayerItem(guid, bag->m_guid, slot);
        }

        return 0;
    }

    if (CGGuildBankInfo::m_banker) {
        CGGuildBankInfo::AutoStoreItem(guid.m_guid, bag->m_guid.m_guid, slot);
        CGGameUI::SetPendingItemSound(guid, itemId);
        return 0;
    }

    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);

    if (!item->GetInventoryType() || (rec && rec->m_startQuest)) {
        item->UseItem(&target, 0);
        return 0;
    }

    if (CGGameUI::m_hasControl || (static_cast<uint8_t>(player->m_unit->m_unitFieldFlags >> 20) & 1)) {
        player->AutoEquipItem(bag->m_guid, slot, 0);
        return 0;
    }

    return 0;
}

// OFFSET: 0x009BDFC0/0x140749E90
static int32_t Script_GetBagName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBagName(index)");
        return 0;
    }

    if (static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

        if (!index) {
            lua_pushstring(L, FrameScript_GetText("BACKPACK_TOOLTIP", -1, GENDER_NOT_APPLICABLE));
            return 1;
        }

        CGItem_C* container;
        ItemRecSparse* rec;
        WOWGUID requester = WOWGUID::Make(0);

        if (static_cast<uint32_t>(index - 1) <= 10 && (container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__))) != NULL && (rec = g_itemSparseDB.GetRecord(container->m_obj->m_entryID, requester, NULL, NULL, false)) != NULL) {
            lua_pushstring(L, rec->GetName(0));
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009BE0D0/0x140749FC0
static bool BuildItemIconPath(int32_t itemId, char* buffer, uint32_t size) {
    if (!itemId) {
        return false;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRec* rec = g_itemDB.GetRecord(itemId, requester, NULL, NULL, false);

    if (!rec) {
        return false;
    }

    const char* path = ClientDBStringLookup(3);
    SStrPrintf(buffer, size, "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
    return true;
}

// OFFSET: 0x009BE150/0x14074A090
static int32_t Script_GetContainerItemPurchaseItem(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: ContainerItemPurchaseItem(bag, slot, itemIndex[, isEquipped])");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGBag_C* bag = NULL;
    uint32_t slot;

    if (static_cast<int32_t>(lua_tonumber(L, 4))) {
        bag = &ClntObjMgrGetActivePlayerObj()->m_bag;
        slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

        if (!bag || slot > 18) {
            return 0;
        }
    } else {
        int32_t bagSlot;
        int32_t bank;

        if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, bagSlot, bank)) {
            return 0;
        }

        slot = bagSlot;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item || CGLootInfo::m_lootObj == item->m_obj->m_guid) {
        return 0;
    }

    CGItem_C::ItemPurchaseData* data = item->m_itemPurchaseData;

    if (!data || static_cast<int32_t>(data->m_refundTimeLeft - player->GetPlayedTime() + 7200) <= 0) {
        return 0;
    }

    int32_t wanted = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;
    int32_t found = 0;
    uint32_t i;

    for (i = 0; ; i++) {
        if (static_cast<int32_t>(data->m_itemId[i]) > 0) {
            if (found == wanted) {
                break;
            }

            found++;
        }

        if (i + 1 >= 5) {
            return 0;
        }
    }

    char texture[260];

    if (BuildItemIconPath(data->m_itemId[i], texture, sizeof(texture))) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<int32_t>(data->m_itemCount[i]));
    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(data->m_itemId[i], requester, NULL, NULL, true);

    if (rec) {
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(data->m_itemId[i], rec->m_quality, 0, 0, 0, 0));
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x009BE370/0x14074A350
static int32_t Script_GetContainerItemQuestInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemQuestInfo(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;
    CGItem_C* item;
    ItemRecSparse* rec;
    WOWGUID requester = WOWGUID::Make(0);

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank) || (item = bag->GetItemPointer(slot)) == NULL || (rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false)) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    if (rec->m_bonding == 4) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushnil(L);
    }

    if (int32_t questId = rec->m_startQuest) {
        lua_pushnumber(L, questId);
        lua_pushboolean(L, CGQuestLog::IsOnQuest(rec->m_startQuest));
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x009BE8D0/0x14074A9F0
static int32_t Script_IsContainerFiltered(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsContainerFiltered(index)");
        return 0;
    }

    CGPlayer_C* player;

    if (!CGContainerInfo::m_searchText || (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) == NULL) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!index) {
        for (int32_t slot = 23; slot <= 38; slot++) {
            CGItem_C* item = player->m_bag.GetItemPointer(slot);

            if (item && !CGContainerInfo::IsItemFiltered(item)) {
                lua_pushboolean(L, 0);
                return 1;
            }
        }
    } else if (index == -1) {
        if (CGBankInfo::m_banker) {
            for (int32_t slot = 39; slot <= 66; slot++) {
                CGItem_C* item = player->m_bag.GetItemPointer(slot);

                if (item && !CGContainerInfo::IsItemFiltered(item)) {
                    lua_pushboolean(L, 0);
                    return 1;
                }
            }
        }
    } else if (static_cast<uint32_t>(index - 1) <= 10) {
        if (CGItem_C* container = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(index - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__))) {
            if (CGBag_C* bag = container->GetBag()) {
                for (uint32_t slot = 0; slot < static_cast<uint32_t>(bag->m_numSlots); slot++) {
                    CGItem_C* item = bag->GetItemPointer(slot);

                    if (item && !CGContainerInfo::IsItemFiltered(item)) {
                        lua_pushboolean(L, 0);
                        return 1;
                    }
                }
            }
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x009BEA70/0x14074ABB0
static int32_t Script_GetContainerItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemInfo(index, slot)");
        return 0;
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, bag, slot, bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    lua_pushstring(L, texture);
    lua_pushnumber(L, item->m_item->m_itemFieldStackCount);

    if (item->m_itemFlags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);

    if (rec && rec->m_inventoryType) {
        lua_pushnumber(L, rec->m_quality);
    } else {
        lua_pushnumber(L, -1.0);
    }

    if (item->GetPageTextID(NULL) || (static_cast<uint8_t>(item->m_item->m_itemFieldFlags >> 9) & 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (item->GetItemStaticFlags(0) & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    lua_pushboolean(L, CGContainerInfo::IsItemFiltered(item));
    return 8;
}

// STATIC: 0x00D15B08/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "ContainerIDToInventoryID", &Script_ContainerIDToInventoryID },
    { "GetContainerNumSlots", &Script_GetContainerNumSlots },
    { "GetContainerItemInfo", &Script_GetContainerItemInfo },
    { "GetContainerItemID", &Script_GetContainerItemID },
    { "GetContainerItemLink", &Script_GetContainerItemLink },
    { "GetContainerItemCooldown", &Script_GetContainerItemCooldown },
    { "PickupContainerItem", &Script_PickupContainerItem },
    { "SplitContainerItem", &Script_SplitContainerItem },
    { "UseContainerItem", &Script_UseContainerItem },
    { "SocketContainerItem", &Script_SocketContainerItem },
    { "ShowContainerSellCursor", &Script_ShowContainerSellCursor },
    { "SetBagPortraitTexture", &Script_SetBagPortraitTexture },
    { "GetBagName", &Script_GetBagName },
    { "GetContainerItemDurability", &Script_GetContainerItemDurability },
    { "GetContainerNumFreeSlots", &Script_GetContainerNumFreeSlots },
    { "GetContainerFreeSlots", &Script_GetContainerFreeSlots },
    { "IsContainerFiltered", &Script_IsContainerFiltered },
    { "GetContainerItemPurchaseInfo", &Script_GetContainerItemPurchaseInfo },
    { "GetContainerItemPurchaseItem", &Script_GetContainerItemPurchaseItem },
    { "GetContainerItemPurchaseCurrency", &Script_GetContainerItemPurchaseCurrency },
    { "ContainerRefundItemPurchase", &Script_ContainerRefundItemPurchase },
    { "GetMaxArenaCurrency", &Script_GetMaxArenaCurrency },
    { "GetContainerItemGems", &Script_GetContainerItemGems },
    { "GetContainerItemQuestInfo", &Script_GetContainerItemQuestInfo },
    { "GetContainerItemEquipmentSetInfo", &Script_GetContainerItemEquipmentSetInfo },
    { "PlayerHasHearthstone", &Script_PlayerHasHearthstone },
    { "UseHearthstone", &Script_UseHearthstone },
    { "GetItemCooldown", &Script_GetItemCooldown },
    { "SetItemSearch", &Script_SetItemSearch },
};

// OFFSET: 0x009BB440/0x140746C60
void ContainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009BB470/0x140746CB0
void ContainerUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
