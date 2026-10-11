#include "TradeFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// PROVEN: a0cc92d6 IDENTICAL/NONE
// OFFSET: 0x0096A7D0/0x1406E10B0
static int32_t Script_GetPlayerTradeMoney(lua_State* L) {
    lua_pushnumber(L, CGTradeInfo::m_tradingPlayer ? static_cast<double>(CGTradeInfo::m_playerTrade.m_money) : 0.0);
    return 1;
}

// PROVEN: 666e1b94 IDENTICAL/NONE
// OFFSET: 0x0096A840/0x1406E1100
static int32_t Script_GetTargetTradeMoney(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGTradeInfo::m_targetTrade.m_money));
    return 1;
}

// OFFSET: 0x0096A8A0/0x1406E1130
static int32_t Script_PickupTradeMoney(lua_State* L) {
    uint64_t money;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PickupTradeMoney(amount)");
        return 0;
    }

    if (!Script_GetMoney(L, 1, &money)) {
        luaL_error(L, "Usage: PickupTradeMoney(amount)");
        return 0;
    }

    if (money <= CGTradeInfo::m_playerTrade.m_money) {
        CGTradeInfo::RemoveMoney(money);
        CGGameUI::SetCursorMoney(money);
    }

    return 0;
}

// PROVEN: 91dff602 IDENTICAL/NONE
// OFFSET: 0x0096A920/0x1406E11B0
static int32_t Script_GetPlayerTradeCurrency(lua_State* L) {
    int32_t type = 0;
    int32_t quantity = 0;

    if (CGTradeInfo::m_tradingPlayer) {
        type = CGTradeInfo::m_playerTrade.m_currencyType;
        quantity = CGTradeInfo::m_playerTrade.m_currencyQuantity;
    }

    lua_pushnumber(L, type);
    lua_pushnumber(L, quantity);
    return 2;
}

// PROVEN: cbec4e46 IDENTICAL/NONE
// OFFSET: 0x0096A980/0x1406E1210
static int32_t Script_GetTargetTradeCurrency(lua_State* L) {
    int32_t quantity = CGTradeInfo::m_targetTrade.m_currencyQuantity;
    lua_pushnumber(L, CGTradeInfo::m_targetTrade.m_currencyType);
    lua_pushnumber(L, quantity);
    return 2;
}

// OFFSET: 0x0096AB40/0x1406E1420
static int32_t Script_GetTradePlayerItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradePlayerItemLink(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID guid;

    if (index > 6) {
        guid = WOWGUID();
    } else {
        guid = CGTradeInfo::m_playerTrade.m_items[index].m_item;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    return 1;
}

// PROVEN: 1c2c8da3 IDENTICAL/NONE
// OFFSET: 0x0096ABD0/0x1406E14C0
static int32_t Script_AcceptTrade(lua_State* L) {
    if (CGGameUI::CanPerformAction(7) && CGTradeInfo::m_playerAccepted != 1) {
        CGTradeInfo::m_playerAccepted = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", 1, CGTradeInfo::m_targetAccepted);
        CGTradeInfo::AcceptTrade();
    }

    return 0;
}

// PROVEN: 6b0f70a9 IDENTICAL/NONE
// OFFSET: 0x0096AC20/0x1406E1540
static int32_t Script_CancelTradeAccept(lua_State* L) {
    if (CGGameUI::CanPerformAction(7) && CGTradeInfo::m_playerAccepted) {
        CGTradeInfo::m_playerAccepted = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", 0, CGTradeInfo::m_targetAccepted);
        CGTradeInfo::UnacceptTrade();
    }

    return 0;
}

// OFFSET: 0x0096B1F0/0x1406E1C50
static int32_t Script_ClickTargetTradeButton(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickTargetTradeButton(index)");
        return 0;
    }

    if (CGGameUI::m_cursorMoney) {
        if (CGTradeInfo::AddMoney(CGGameUI::m_cursorMoney)) {
            CGGameUI::ClearCursor(1, 0);
            return 0;
        }
    } else if (static_cast<int32_t>(lua_tonumber(L, 1)) - 1 == 6) {
        CGGameUI::TargetTradeItem();
    }

    return 0;
}

// PROVEN: 3e3c8e27 IDENTICAL/NONE
// OFFSET: 0x0096B270/0x1406E1CD0
static int32_t Script_AddTradeMoney(lua_State* L) {
    if (CGGameUI::m_cursorMoney && CGTradeInfo::AddMoney(CGGameUI::m_cursorMoney)) {
        CGGameUI::ClearCursor(1, 0);
    }

    return 0;
}

// OFFSET: 0x0096B2A0/0x1406E1D00
static int32_t Script_SetTradeMoney(lua_State* L) {
    uint64_t money;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetTradeMoney(amount)");
        return 0;
    }

    if (!Script_GetMoney(L, 1, &money)) {
        luaL_error(L, "Usage: SetTradeMoney(amount)");
        return 0;
    }

    int64_t amount = money;

    if (amount < 0) {
        amount = 0;
    } else if (amount >= 10000000000LL) {
        amount = 9999999999LL;
    }

    CGTradeInfo::SetMoney(amount);
    return 0;
}

// OFFSET: 0x0096B340/0x1406E1DC0
static int32_t Script_SetTradeCurrency(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetTradeCurrency(type, amount)");
        return 0;
    }

    int32_t type = static_cast<int32_t>(lua_tonumber(L, 1));
    CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(type);

    if (rec && (rec->m_flags & 1)) {
        int32_t quantity = static_cast<int32_t>(lua_tonumber(L, 2));

        if (quantity >= 0) {
            CGTradeInfo::SetCurrency(type, quantity);
        }
    }

    return 0;
}

// OFFSET: 0x0096B8A0/0x1406E23B0
static int32_t Script_CloseTrade(lua_State* L) {
    if (!CGTradeInfo::m_supressMoneyEvent) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TRADE_MONEY, NULL);
    }

    CGTradeInfo::SetTradePartner(WOWGUID(), 0, 0);
    return 0;
}

// OFFSET: 0x0096BD30/0x1406E28C0
static int32_t Script_ClickTradeButton(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickTradeButton(index)");
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (lua_toboolean(L, 2)) {
        WOWGUID item;
        WOWGUID container;
        uint8_t containerSlot;
        CGTradeInfo::GetPlayerItem(slot, item, container, containerSlot);

        if (item) {
            CGTradeInfo::SetPlayerItem(slot, WOWGUID(), WOWGUID(), 0);
            CGGameUI::UnlockItem(item);
            CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (object) {
                SI3::PlayItemSound(1, object);
            }
        }

        return 0;
    }

    if (CGGameUI::m_cursorMoney) {
        if (CGTradeInfo::AddMoney(CGGameUI::m_cursorMoney)) {
            CGGameUI::ClearCursor(1, 0);
        }

        return 0;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));
    bool canTradeBound = item && CanTradeBoundItem(item);

    if (slot < 6 && item) {
        if (item->IsPermanentlyBoundForTrade() && !canTradeBound) {
            WOWGUID requester = WOWGUID();
            ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, NULL, NULL, false);

            if (rec && rec->m_bonding == 4) {
                CGGameUI::DisplayError(ERR_TRADE_QUEST_ITEM);
            } else if (item->IsBoundToAccount()) {
                CGGameUI::DisplayError(ERR_NOT_SAME_ACCOUNT);
            } else {
                CGGameUI::DisplayError(ERR_TRADE_BOUND_ITEM);
            }

            return 0;
        }

        if (item->IsTemporarilyBoundForTrade() && !canTradeBound) {
            CGGameUI::DisplayError(ERR_TRADE_TEMP_ENCHANT_BOUND);
            return 0;
        }

        if (item->m_obj->m_type & TYPEMASK_CONTAINER) {
            if (CGBag_C* bag = item->GetBag()) {
                if (bag->GetNumItems()) {
                    CGGameUI::DisplayError(ERR_TRADE_BAG);
                    return 0;
                }

                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                if (static_cast<uint32_t>(player->GetBag()->FindItemSlot(item) - 19) <= 3) {
                    CGGameUI::DisplayError(ERR_TRADE_EQUIPPED_BAG);
                    return 0;
                }
            }
        }
    }

    WOWGUID current;
    WOWGUID container;
    uint8_t containerSlot;
    CGTradeInfo::GetPlayerItem(slot, current, container, containerSlot);

    if (cursorItem == current) {
        CGGameUI::ClearCursor(1, 1);
        return 0;
    }

    int32_t set = CGTradeInfo::SetPlayerItem(slot, cursorItem, cursorContainer, static_cast<uint8_t>(cursorSlot));
    CGGameUI::SetCursorItem(current, container, containerSlot, set == 0, 0, 1);
    return 0;
}

// OFFSET: 0x0096C010/0x1406E2BF0
static int32_t Script_GetTradeTargetItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeTargetItemInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t itemId;
    WOWGUID requester;
    ItemRecSparse* sparse;
    ItemRec* rec;

    if (index > 6 || !(itemId = CGTradeInfo::m_targetTrade.m_items[index].m_itemID) || (requester = CGTradeInfo::m_tradingPlayer, !(sparse = g_itemSparseDB.GetRecord(itemId, requester, &TradeItemStatsCallback, NULL, false))) || !(rec = sparse->GetNonSparse())) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        return 6;
    }

    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), itemId, CGTradeInfo::GetTargetRandomPropertyID(index));
    lua_pushstring(L, name);
    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
    lua_pushstring(L, texture);
    lua_pushnumber(L, CGTradeInfo::GetTargetCount(index));
    lua_pushnumber(L, sparse->m_inventoryType ? sparse->m_quality : -1);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t error;

    if (!player || player->CanUseItem(sparse, error)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (index == 6 && CGTradeInfo::m_playerTrade.m_proposedEnchant) {
        if (SpellRec* spell = g_spellDB.GetRecord(CGTradeInfo::m_playerTrade.m_proposedEnchant)) {
            lua_pushstring(L, spell->m_name);
            return 6;
        }
    }

    lua_pushnil(L);
    return 6;
}

// OFFSET: 0x0096C240/0x1406E2EC0
static int32_t Script_GetTradeTargetItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeTargetItemLink(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index > 6) {
        return 0;
    }

    int32_t itemId = CGTradeInfo::m_targetTrade.m_items[index].m_itemID;

    if (!itemId) {
        return 0;
    }

    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

    if (rec) {
        const TradeTargetItem& item = CGTradeInfo::m_targetTrade.m_items[index];
        int32_t gems[3];

        for (uint32_t i = 0; i < 3; i++) {
            gems[i] = item.m_socketEnchants[i];
        }

        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, rec->m_quality, item.m_enchant, gems, item.m_randomPropertyID, item.m_suffixFactor, item.m_reforgeEnchant));
        return 1;
    }

    return 0;
}

// OFFSET: 0x0096C340/0x1406E3020
static int32_t Script_GetTradePlayerItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradePlayerItemInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID guid;

    if (index > 6) {
        guid = WOWGUID();
    } else {
        guid = CGTradeInfo::m_playerTrade.m_items[index].m_item;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        char name[256];
        item->GetFullItemName(name, sizeof(name));
        lua_pushstring(L, name);
        const char* path = ClientDBStringLookup(3);
        char texture[260];
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
        lua_pushstring(L, texture);
        lua_pushnumber(L, item->m_item->m_itemFieldStackCount);
        WOWGUID requester = ClntObjMgrGetActivePlayer();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, requester, &TradeItemStatsCallback, NULL, false);
        lua_pushnumber(L, rec && rec->m_inventoryType ? rec->m_quality : -1);

        if (index == 6 && CGTradeInfo::m_targetTrade.m_proposedEnchant) {
            if (SpellRec* spell = g_spellDB.GetRecord(CGTradeInfo::m_targetTrade.m_proposedEnchant)) {
                lua_pushstring(L, spell->m_name);
                return 5;
            }
        }

        lua_pushnil(L);
        return 5;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    return 5;
}

// STATIC: 0x00D13280
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTrade", &Script_CloseTrade },
    { "ClickTradeButton", &Script_ClickTradeButton },
    { "ClickTargetTradeButton", &Script_ClickTargetTradeButton },
    { "GetTradeTargetItemInfo", &Script_GetTradeTargetItemInfo },
    { "GetTradeTargetItemLink", &Script_GetTradeTargetItemLink },
    { "GetTradePlayerItemInfo", &Script_GetTradePlayerItemInfo },
    { "GetTradePlayerItemLink", &Script_GetTradePlayerItemLink },
    { "AcceptTrade", &Script_AcceptTrade },
    { "CancelTradeAccept", &Script_CancelTradeAccept },
    { "GetPlayerTradeMoney", &Script_GetPlayerTradeMoney },
    { "GetTargetTradeMoney", &Script_GetTargetTradeMoney },
    { "PickupTradeMoney", &Script_PickupTradeMoney },
    { "AddTradeMoney", &Script_AddTradeMoney },
    { "SetTradeMoney", &Script_SetTradeMoney },
    { "SetTradeCurrency", &Script_SetTradeCurrency },
    { "GetPlayerTradeCurrency", &Script_GetPlayerTradeCurrency },
    { "GetTargetTradeCurrency", &Script_GetTargetTradeCurrency },
};

// PROVEN: 21ea7c44 IDENTICAL/NONE
// OFFSET: 0x0096A9C0/0x1406E1260
void TradeInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: da9efac9 IDENTICAL/NONE
// OFFSET: 0x0096A9F0/0x1406E12B0
void TradeInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
