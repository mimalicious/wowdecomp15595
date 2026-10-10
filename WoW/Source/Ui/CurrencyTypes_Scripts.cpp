#include "CurrencyTypes.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/UIUtil/Tooltip.h"

// OFFSET: 0x0099F7B0/0x140723780
static int32_t Script_GetCurrencyLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage:GetCurrencyLink(currencyType)");
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetCurrencyHyperlinkString(static_cast<int32_t>(lua_tonumber(L, 1))));
    return 1;
}

// OFFSET: 0x0099FCD0/0x140723F30
static int32_t Script_GetCurrencyListSize(lua_State* L) {
    lua_pushnumber(L, CGCurrencyTypes::s_displayList.Count());
    return 1;
}

// OFFSET: 0x0099FD10/0x140723F60
static int32_t Script_GetCurrencyListLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage:GetCurrencyListLink(index)");
        return 0;
    }

    CGCurrencyTypes::CurrencyListInfo* info = CGCurrencyTypes::GetDisplayListInfo(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (info) {
        lua_pushstring(L, CGTooltip::GetCurrencyHyperlinkString(info->m_currencyID));
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009A0270/0x140724570
static int32_t Script_ExpandCurrencyList(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ExpandCurrencyList(index,expand)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    CGCurrencyTypes::SetCategoryExpand(index, static_cast<int32_t>(lua_tonumber(L, 2)));
    CGCurrencyTypes::BuildDisplayList();
    return 0;
}

// OFFSET: 0x009A0300/0x140724600
static int32_t Script_PickupCurrency(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage:PickupCurrency(currencyType)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (CGCurrencyTypes::GetCurrency(id)) {
        CGGameUI::SetCursorCurrency(id);
    }

    return 0;
}

// OFFSET: 0x009A0360/0x140724680
static int32_t Script_GetCurrencyInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage:GetCurrencyInfo(currencyType)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(id);

    if (currency) {
        lua_pushstring(L, currency->m_rec->m_name);
        lua_pushnumber(L, currency->m_quantity);
        lua_pushstring(L, CGCurrencyTypes::GetCurrencyIcon(currency->m_rec));
        lua_pushnumber(L, currency->m_weeklyQuantity);

        if (currency->m_rec->m_flags & 0x4) {
            lua_pushnumber(L, currency->m_weeklyCap);
        } else {
            lua_pushnumber(L, currency->m_rec->m_weeklyCap);
        }

        lua_pushnumber(L, currency->m_rec->m_totalCap);
        lua_pushboolean(L, 1);
        return 7;
    }

    CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(id);

    if (rec) {
        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, CGCurrencyTypes::GetCurrencyIcon(rec));
    } else {
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushboolean(L, 0);
    return 7;
}

// OFFSET: 0x009A0790/0x140724C60
static int32_t Script_GetCurrencyListInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCurrencyListInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    CGCurrencyTypes::CurrencyListInfo* info;

    if (index >= static_cast<int32_t>(CGCurrencyTypes::s_displayList.Count()) || (info = &CGCurrencyTypes::s_displayList[index]) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 11;
    }

    if (info->m_currencyID) {
        CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(info->m_currencyID);

        if (rec) {
            lua_pushstring(L, rec->m_name);
        } else {
            lua_pushstring(L, "");
        }
    } else {
        CurrencyCategoryRec* category;
        const char* name;

        if (info->m_isHeader && (category = g_currencyCategoryDB.GetRecord(info->m_categoryID)) != NULL && (name = category->m_name) != NULL) {
            lua_pushstring(L, name);
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushboolean(L, info->m_isHeader);
    lua_pushboolean(L, info->m_isExpanded);
    lua_pushboolean(L, CGCurrencyTypes::IsCurrencyUnused(index));
    lua_pushboolean(L, info->m_isWatched);
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(info->m_currencyID);

    if (!currency) {
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 11;
    }

    lua_pushnumber(L, currency->m_quantity);
    char texture[260];
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency->m_rec));
    lua_pushstring(L, texture);
    lua_pushinteger(L, currency->m_rec->m_totalCap);

    if (currency->m_rec->m_weeklyCap <= 0) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    lua_pushinteger(L, currency->m_weeklyQuantity);

    if (currency->m_rec->m_flags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 11;
}

// OFFSET: 0x009A0A30/0x140724F90
static int32_t Script_SetCurrencyUnused(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetCurrencyUnused(index,unused)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    CGCurrencyTypes::SetUnused(index, static_cast<int32_t>(lua_tonumber(L, 2)));
    CGCurrencyTypes::BuildDisplayList();
    return 0;
}

// OFFSET: 0x009A0AC0/0x140725020
static int32_t Script_SetCurrencyBackpack(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetCurrencyBackpack(index,backpack)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    CGCurrencyTypes::SetBackpack(index, static_cast<int32_t>(lua_tonumber(L, 2)));
    CGCurrencyTypes::BuildDisplayList();
    return 0;
}

// OFFSET: 0x009A0B50/0x1407250B0
static int32_t Script_GetBackpackCurrencyInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBackpackCurrencyInfo(index)");
        return 0;
    }

    const CurrencyTypesRec* rec = CGCurrencyTypes::GetBackpackCurrencyInfo(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (!rec) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    if (rec->m_name) {
        lua_pushstring(L, rec->m_name);
    } else {
        lua_pushnil(L);
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(rec->m_id);
        lua_pushnumber(L, currency ? currency->m_quantity : 0);
    } else {
        lua_pushnumber(L, 0.0);
    }

    char texture[260];
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(rec));
    lua_pushstring(L, texture);
    lua_pushnumber(L, rec->m_id);
    return 4;
}

// STATIC: 0x00D14EF0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetCurrencyListSize", &Script_GetCurrencyListSize },
    { "GetCurrencyListInfo", &Script_GetCurrencyListInfo },
    { "ExpandCurrencyList", &Script_ExpandCurrencyList },
    { "SetCurrencyUnused", &Script_SetCurrencyUnused },
    { "SetCurrencyBackpack", &Script_SetCurrencyBackpack },
    { "GetBackpackCurrencyInfo", &Script_GetBackpackCurrencyInfo },
    { "PickupCurrency", &Script_PickupCurrency },
    { "GetCurrencyInfo", &Script_GetCurrencyInfo },
    { "GetCurrencyLink", &Script_GetCurrencyLink },
    { "GetCurrencyListLink", &Script_GetCurrencyListLink },
};

// OFFSET: 0x0099F800
void CurrencyTypesRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0099F830
void CurrencyTypesUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
