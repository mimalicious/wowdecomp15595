#include "ItemSocketInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x0097A230/0x1406F37A0
static int32_t Script_GetSocketTypes(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSocketType(index)");
        return 0;
    }

    lua_checkstack(L, 4);
    ItemStats_C* stats = CGItemSocketInfo::m_socketItemStats;
    int32_t count = 0;
    double index = lua_tonumber(L, 1);

    if (!stats) {
        return 0;
    }

    uint32_t color = stats->m_socketColor[static_cast<int32_t>(index - 1)];

    for (uint32_t i = 0; i < 4; i++) {
        if ((1 << i) & color) {
            lua_pushstring(L, s_socketColorNames[i]);
            count++;
        }
    }

    if (!count) {
        lua_pushstring(L, "Socket");
        return 1;
    }

    return count;
}

// OFFSET: 0x0097AC10/0x1406F4180
static int32_t Script_CloseSocketInfo(lua_State* L) {
    CGItemSocketInfo::SetItem(WOWGUID());
    return 0;
}

// OFFSET: 0x0097A3C0/0x1406F3920
static int32_t Script_GetSocketItemInfo(lua_State* L) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));
    ItemStats_C* stats = CGItemSocketInfo::m_socketItemStats;

    if (!item || !stats) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    char name[256];
    item->GetFullItemName(name, sizeof(name));
    lua_pushstring(L, name);
    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(stats->m_displayId));
    lua_pushstring(L, texture);
    lua_pushnumber(L, stats->m_inventoryType ? static_cast<int32_t>(stats->m_quality) : -1);
    return 3;
}

// OFFSET: 0x0097A500/0x1406F3A70
static int32_t Script_GetNumSockets(lua_State* L) {
    int32_t count = 0;
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        count = item->GetNumSockets();
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0097AF40/0x1406F4500
static int32_t Script_GetNewSocketInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNewSocketInfo(index)");
        return 0;
    }

    double number = lua_tonumber(L, 1);
    uint32_t index = static_cast<int32_t>(number) - 1;
    WOWGUID guid = index >= 3 ? WOWGUID() : CGItemSocketInfo::m_gemItems[index];
    CGItem_C* gem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
    ItemStats_C* stats = NULL;

    if (gem) {
        WOWGUID none = WOWGUID();
        stats = g_itemDBCache.GetRecord(gem->m_obj->m_entryID, none, NULL, NULL, false);
    }

    if (!stats) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), gem->m_obj->m_entryID, 0);
    lua_pushstring(L, name);
    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(stats->m_displayId));
    lua_pushstring(L, texture);

    if (CGItemSocketInfo::GemMatchesSocket(CGItemSocketInfo::m_socketItemStats, stats, static_cast<int32_t>(number - 1))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x0097A570/0x1406F3AD0
static int32_t Script_GetNewSocketLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNewSocketLink(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID guid = index >= 3 ? WOWGUID() : CGItemSocketInfo::m_gemItems[index];
    CGItem_C* gem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!gem) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(gem));
    return 1;
}

// OFFSET: 0x0097B100/0x1406F46F0
static int32_t Script_ClickSocketButton(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ClickSocketButton(index)");
        return 0;
    }

    WOWGUID cursorItem;
    WOWGUID cursorContainer;
    uint32_t cursorSlot;
    CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
    double number = lua_tonumber(L, 1);
    WOWGUID gem;
    WOWGUID container;
    uint8_t containerSlot;
    CGItemSocketInfo::GetGemItemForSlot(static_cast<int32_t>(number - 1), gem, container, containerSlot);

    if (CGItemSocketInfo::SetGem(static_cast<int32_t>(number - 1), cursorItem, cursorContainer, static_cast<uint8_t>(cursorSlot))) {
        CGGameUI::ClearCursor(0, 1);

        if (gem) {
            CGGameUI::SetCursorItem(gem, container, containerSlot, 1, 0, 1);
        }
    }

    return 0;
}

// OFFSET: 0x0097A620
static int32_t Script_GetSocketItemBoundTradeable(lua_State* L) {
    if (CGItemSocketInfo::IsSocketItemBoundTradeable()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0097AC20/0x1406F41A0
static int32_t Script_GetExistingSocketInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetExistingSocketInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));
    SpellItemEnchantmentRec* enchant;
    ItemStats_C* stats;

    if (!item || index >= 3 || !(enchant = g_spellItemEnchantmentDB.GetRecord(item->GetEnchantmentId(index + 2))) || !enchant->m_srcItemID) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    WOWGUID none = WOWGUID();
    stats = g_itemDBCache.GetRecord(enchant->m_srcItemID, none, &EnchantItemCacheCallback, NULL, false);

    if (!stats) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), enchant->m_srcItemID, 0);
    lua_pushstring(L, name);
    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(stats->m_displayId));
    lua_pushstring(L, texture);

    if (CGItemSocketInfo::GemMatchesSocket(CGItemSocketInfo::m_socketItemStats, stats, index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x0097AE20/0x1406F43C0
static int32_t Script_GetExistingSocketLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetExistingSocketLink(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || index >= 3) {
        return 0;
    }

    SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(item->GetEnchantmentId(index + 2));

    if (!enchant || !enchant->m_srcItemID) {
        return 0;
    }

    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(enchant->m_srcItemID, none, &EnchantItemCacheCallback, NULL, false);

    if (!stats) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(enchant->m_srcItemID, stats->m_quality, 0, 0, 0, 0));
    return 1;
}

// OFFSET: 0x0097A820/0x1406F3DD0
static int32_t Script_AcceptSockets(lua_State* L) {
    CGItemSocketInfo::Accept();
    return 0;
}

// OFFSET: 0x0097A830/0x1406F3DE0
static int32_t Script_GetSocketItemRefundable(lua_State* L) {
    if (CGItemSocketInfo::IsSocketItemRefundable()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: #0x00AD0568
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseSocketInfo", &Script_CloseSocketInfo },
    { "GetSocketItemInfo", &Script_GetSocketItemInfo },
    { "GetNumSockets", &Script_GetNumSockets },
    { "GetExistingSocketInfo", &Script_GetExistingSocketInfo },
    { "GetExistingSocketLink", &Script_GetExistingSocketLink },
    { "GetNewSocketInfo", &Script_GetNewSocketInfo },
    { "GetNewSocketLink", &Script_GetNewSocketLink },
    { "ClickSocketButton", &Script_ClickSocketButton },
    { "AcceptSockets", &Script_AcceptSockets },
    { "GetSocketTypes", &Script_GetSocketTypes },
    { "GetSocketItemRefundable", &Script_GetSocketItemRefundable },
    { "GetSocketItemBoundTradeable", &Script_GetSocketItemBoundTradeable },
};

// PROVEN: 5a0d4f7c IDENTICAL/NONE
// OFFSET: 0x0097A2C0/0x1406F3830
void ItemSocketInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: f1a30822 IDENTICAL/NONE
// OFFSET: 0x0097A2F0/0x1406F3880
void ItemSocketInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
