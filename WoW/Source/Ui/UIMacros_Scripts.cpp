#include "UIMacros.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/FrameXML/LoadXML.h"

// OFFSET: 0x009C3D10/0x1407510D0
static int32_t Script_StopMacro(lua_State* L) {
    if (CGGameUI::CanPerformAction(3)) {
        CGUIMacros::m_macroButton = NULL;
        CGUIMacros::m_macroRunning = NULL;
    }

    return 0;
}

// OFFSET: 0x009C3DE0/0x1407511A0
static int32_t Script_GetRunningMacroButton(lua_State* L) {
    lua_pushstring(L, CGUIMacros::m_macroRunning);
    return 1;
}

// OFFSET: 0x009C3D30/0x140751100
static int32_t Script_GetNumMacros(lua_State* L) {
    lua_pushnumber(L, CGUIMacros::m_numMacros[0]);
    lua_pushnumber(L, CGUIMacros::m_numMacros[1]);
    return 2;
}

// OFFSET: 0x009C3D90/0x140751140
static int32_t Script_GetRunningMacro(lua_State* L) {
    uint32_t id;

    if (CGUIMacros::m_macroButton && (id = CGUIMacros::m_macroButton->m_id)) {
        lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(id) + 1);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009C5570/0x140753250
static MACRONODE* GetMacroFromLua(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < 72) {
            return CGUIMacros::GetMacroByID(CGUIMacros::m_sortedMacros[index]);
        }
    } else if (lua_isstring(L, 1)) {
        return CGUIMacros::FindMacro(lua_tolstring(L, 1, NULL));
    }

    return NULL;
}

// OFFSET: 0x009C5880/0x140753690
static int32_t Script_SetMacroSpell(lua_State* L) {
    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetMacroSpell(macro, spell [,target])");
        return 0;
    }

    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        int32_t spellId = CGSpellBook::FindSpellByName(lua_tolstring(L, 2, NULL), macro->m_spellFlags);

        if (!spellId) {
            spellId = -1;
        }

        WOWGUID target = { 0 };
        Script_GetGUIDFromToken(lua_tolstring(L, 3, NULL), target, false, false);

        if (macro->m_itemID > 0 || spellId != macro->m_spellID || target != macro->m_target) {
            macro->m_target = target;
            macro->m_itemID = 0;
            macro->m_spellID = spellId;
            CGActionBar::UpdateMacro(macro->m_id);
        }

        if (macro->m_dynamic) {
            macro->m_dynamic = 0;
            CGUIMacros::RemoveDynamicMacro(macro);
        }

        if (spellId > 0) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    return 0;
}

// OFFSET: 0x009C59B0/0x1407537E0
static int32_t Script_GetMacroSpell(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        const SpellRec* spell = g_spellDB.GetRecord(macro->m_spellID);

        if (spell) {
            lua_pushstring(L, spell->m_name);
            lua_pushstring(L, spell->m_nameSubtext);
            lua_pushnumber(L, static_cast<double>(macro->m_spellID));
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
        }

        return 3;
    }

    return 0;
}

// OFFSET: 0x009C3C60/0x140751000
static int32_t Script_SecureCmdOptionParse(lua_State* L) {
    const char* options = lua_tolstring(L, 1, NULL);

    if (!options) {
        luaL_error(L, "Usage: SecureCmdOptionParse(\"options\")");
        return 0;
    }

    MacroSubstring result = { NULL, NULL };
    MacroSubstring target = { NULL, NULL };

    if (!EvaluateMacroOptions(options, options + SStrLen(options), &result, &target)) {
        return 0;
    }

    lua_pushlstring(L, result.m_start, result.m_end - result.m_start);

    size_t length = target.m_end - target.m_start;

    if (length) {
        lua_pushlstring(L, target.m_start, length);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x009C5630/0x140753350
static int32_t Script_RunMacroText(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RunMacroText(\"text\" [, \"button\"])");
        return 0;
    }

    MACRONODE macro;
    SStrCopy(macro.m_body, lua_tolstring(L, 1, NULL), sizeof(macro.m_body));
    CGUIMacros::RunMacro(&macro, lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x009C5A40/0x140753930
static int32_t Script_GetMacroIcons(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetMacroIcons([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    int32_t first = lua_objlen(L, -1) + 1;

    if (!s_macroIcons.m_count) {
        CGUIMacros::LoadIcons();
    }

    int32_t count = s_macroIcons.m_count;

    for (int32_t i = 0; i < count; i++) {
        lua_pushnumber(L, static_cast<double>(i + first));
        lua_pushstring(L, static_cast<uint32_t>(i) < s_macroIcons.m_count ? s_macroIcons.m_data[i] : NULL);
        lua_settable(L, -3);
    }

    return 1;
}

// OFFSET: 0x009C5B20/0x140753A50
static int32_t Script_GetMacroItemIcons(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetMacroItemIcons([table])");
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    int32_t first = lua_objlen(L, -1) + 1;

    if (!s_macroItemIcons.m_count) {
        CGUIMacros::LoadIcons();
    }

    int32_t count = s_macroItemIcons.m_count;

    for (int32_t i = 0; i < count; i++) {
        lua_pushnumber(L, static_cast<double>(i + first));
        lua_pushstring(L, static_cast<uint32_t>(i) < s_macroItemIcons.m_count ? s_macroItemIcons.m_data[i] : NULL);
        lua_settable(L, -3);
    }

    return 1;
}

// OFFSET: 0x009C5C30/0x140753BB0
static int32_t Script_GetMacroIndexByName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetMacroIndexByName(name)");
        return 0;
    }

    lua_pushnumber(L, CGUIMacros::GetSortedIndexByName(lua_tolstring(L, 1, NULL)) + 1);
    return 1;
}

// OFFSET: 0x009C5600/0x1407532C0
static int32_t Script_RunMacro(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        CGUIMacros::RunMacro(macro, lua_tolstring(L, 2, NULL));
    }

    return 0;
}

// OFFSET: 0x009C6460/0x140754750
static int32_t Script_GetMacroInfo(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        lua_pushstring(L, macro->m_name);
        char texture[260];
        CGUIMacros::GetFullIconPath(macro->m_id, texture, sizeof(texture));

        if (texture[0]) {
            lua_pushstring(L, texture);
        } else {
            lua_pushnil(L);
        }

        lua_pushstring(L, macro->m_body);
        return 3;
    }

    return 0;
}

// OFFSET: 0x009C56E0/0x140753490
static int32_t Script_GetMacroBody(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        lua_pushstring(L, macro->m_body);
        return 1;
    }

    return 0;
}

// OFFSET: 0x009C5710/0x140753510
static int32_t Script_DeleteMacro(lua_State* L) {
    if (CGGameUI::CanPerformAction(12)) {
        MACRONODE* macro = GetMacroFromLua(L);

        if (macro) {
            CGUIMacros::DeleteMacro(macro->m_id, 0);
        }
    }

    return 0;
}

// OFFSET: 0x009C5F70/0x140754060
static int32_t Script_EditMacro(lua_State* L) {
    if (!CGGameUI::CanPerformAction(12)) {
        return 0;
    }

    uint32_t id = 0;
    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        id = macro->m_id;

        if (id) {
            const char* name = NULL;
            const char* icon = NULL;
            const char* body = NULL;

            if (lua_isstring(L, 2)) {
                name = lua_tolstring(L, 2, NULL);
            }

            if (lua_isstring(L, 3)) {
                icon = lua_tolstring(L, 3, NULL);
            }

            if (lua_isstring(L, 4)) {
                body = lua_tolstring(L, 4, NULL);
            }

            CGUIMacros::EditMacro(id, name, icon, body);
        }
    }

    int32_t index;

    for (index = 0; CGUIMacros::m_sortedMacros[index] != id; ) {
        if (++index >= 72) {
            index = -1;
            break;
        }
    }

    lua_pushnumber(L, static_cast<double>(index + 1));
    return 1;
}

// OFFSET: 0x009C5750/0x140753550
static int32_t Script_SetMacroItem(lua_State* L) {
    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetMacroItem(macro, item [,target])");
        return 0;
    }

    MACRONODE* macro = GetMacroFromLua(L);

    if (macro) {
        int32_t itemId = GetItemIDByName(lua_tolstring(L, 2, NULL));
        WOWGUID target = { 0 };
        Script_GetGUIDFromToken(lua_tolstring(L, 3, NULL), target, false, false);

        if (macro->m_spellID > 0 || itemId != macro->m_itemID || target != macro->m_target) {
            macro->m_target = target;
            macro->m_itemID = itemId;
            macro->m_spellID = (itemId != 0) - 1;
            CGActionBar::UpdateMacro(macro->m_id);
        }

        if (macro->m_dynamic) {
            macro->m_dynamic = 0;
            CGUIMacros::RemoveDynamicMacro(macro);
        }

        if (itemId > 0) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    return 0;
}

// OFFSET: 0x009C64F0/0x140754830
static int32_t Script_GetMacroItem(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (!macro) {
        return 0;
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(macro->m_itemID, WOWGUID(), NULL, NULL, false);

    if (rec) {
        char name[1024];
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGItem_C* item = player->m_bag.FindItemOfType(macro->m_itemID, 0);

            if (item) {
                lua_pushstring(L, item->GetFullItemName(name, sizeof(name)));
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
                return 2;
            }
        }

        lua_pushstring(L, CGItem_C::BuildItemName(name, sizeof(name), macro->m_itemID, 0));
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(macro->m_itemID, rec->m_quality, 0, 0, 0, 0));
        return 2;
    }

    return 0;
}

// OFFSET: 0x009C5C00/0x140753B70
static int32_t Script_PickupMacro(lua_State* L) {
    if (CGGameUI::CanPerformAction(14)) {
        MACRONODE* macro = GetMacroFromLua(L);

        if (macro) {
            CGGameUI::SetCursorMacro(macro->m_id);
        }
    }

    return 0;
}

// OFFSET: 0x009C7220/0x140755B00
static int32_t Script_CreateMacro(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: CreateMacro(name, iconFileName, body, perCharacter)");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(12)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    const char* icon = lua_tolstring(L, 2, NULL);
    const char* body = NULL;

    if (!name || !*name) {
        luaL_error(L, "CreateMacro() failed, no name specified");
        return 0;
    }

    if (!icon || !*icon) {
        luaL_error(L, "CreateMacro() failed, no icon specified");
        return 0;
    }

    if (lua_isstring(L, 3)) {
        body = lua_tolstring(L, 3, NULL);
    }

    uint32_t id = CGUIMacros::CreateMacro(name, icon, body, StringToBOOL(L, 4, 0), 0);

    if (!id) {
        luaL_error(L, "CreateMacro() failed, already have %d macros", 36);
        return 0;
    }

    lua_pushnumber(L, static_cast<double>(CGUIMacros::GetMacroIndexByID(id) + 1));
    return 1;
}

// STATIC: 0x00D15D98/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "SecureCmdOptionParse", &Script_SecureCmdOptionParse },
    { "RunMacro", &Script_RunMacro },
    { "RunMacroText", &Script_RunMacroText },
    { "StopMacro", &Script_StopMacro },
    { "CreateMacro", &Script_CreateMacro },
    { "GetNumMacros", &Script_GetNumMacros },
    { "GetMacroInfo", &Script_GetMacroInfo },
    { "GetMacroBody", &Script_GetMacroBody },
    { "DeleteMacro", &Script_DeleteMacro },
    { "EditMacro", &Script_EditMacro },
    { "SetMacroItem", &Script_SetMacroItem },
    { "GetMacroItem", &Script_GetMacroItem },
    { "SetMacroSpell", &Script_SetMacroSpell },
    { "GetMacroSpell", &Script_GetMacroSpell },
    { "GetMacroIcons", &Script_GetMacroIcons },
    { "GetMacroItemIcons", &Script_GetMacroItemIcons },
    { "PickupMacro", &Script_PickupMacro },
    { "GetMacroIndexByName", &Script_GetMacroIndexByName },
    { "GetRunningMacro", &Script_GetRunningMacro },
    { "GetRunningMacroButton", &Script_GetRunningMacroButton },
};

// OFFSET: 0x009C3E00/0x1407511C0
void UIMacrosRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009C3E30/0x140751210
void UIMacrosUnegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
