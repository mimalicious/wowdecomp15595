#include "UIBindings.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/Types.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/MODIFIEDCLICK.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include <malloc.h>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00F33EEC/0x140E26EBC
static int32_t s_setBindingClickDepth;

// PROVEN: 26a33c50 IDENTICAL/DIFFERENT
// OFFSET: 0x00922A50/0x140686260
static int32_t Script_GetBindingKey(lua_State* L) {
    if (lua_isstring(L, 1)) {
        BINDING_MODE mode = BINDING_MODE_ACTIVE;

        if (lua_isnumber(L, 2)) {
            int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

            if (index >= 0 && index <= BINDING_MODE_3) {
                mode = static_cast<BINDING_MODE>(index);
            }
        }

        CGUIBindings* bindings = CGUIBindings::s_bindings;
        int32_t count = 0;
        const char* command = lua_tolstring(L, 1, NULL);

        for (const char* key = bindings->GetCommandKey(mode, command, 0); key; key = bindings->GetCommandKey(mode, command, count)) {
            count++;
            lua_pushstring(L, key);
        }

        return count;
    }

    luaL_error(L, "Usage: GetBindingKey(\"COMMAND\"[, mode])");
    return 0;
}

// PROVEN: 4d74a8db IDENTICAL/IDENTICAL
// OFFSET: 0x009213D0/0x140683BC0
static int32_t Script_GetNumBindings(lua_State* L) {
    lua_pushnumber(L, CGUIBindings::s_bindings->m_numCommands);
    return 1;
}

// PROVEN: a882d465 IDENTICAL/IDENTICAL
// OFFSET: 0x00921400/0x140683BF0
static int32_t Script_GetCurrentBindingSet(lua_State* L) {
    lua_pushnumber(L, CGUIBindings::s_bindings->m_saveSet);
    return 1;
}

// PROVEN: cc5694e1 IDENTICAL/IDENTICAL
// OFFSET: 0x00921430/0x140683C20
static int32_t Script_GetNumModifiedClickActions(lua_State* L) {
    lua_pushnumber(L, CGUIBindings::s_bindings->m_numModifiedClicks);
    return 1;
}

// PROVEN: 7edc48f3 IDENTICAL/IDENTICAL
// OFFSET: 0x009278A0/0x14068D280
static int32_t Script_LoadBindings(lua_State* L) {
    int32_t set = static_cast<int32_t>(lua_tonumber(L, 1));

    if (set >= 3) {
        luaL_error(L, "Usage: LoadBindings(0||1||2)");
        return 0;
    }

    CGUIBindings::s_bindings->CopyBindings(static_cast<BINDING_SET>(set), BINDING_SET_CURRENT);
    return 0;
}

// PROVEN: 1a86a601 IDENTICAL/DIFFERENT
// OFFSET: 0x00922D40/0x1406866E0
static int32_t Script_GetModifiedClickAction(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGUIBindings* bindings = CGUIBindings::s_bindings;
        lua_pushstring(L, bindings->GetModifiedClick(static_cast<int32_t>(lua_tonumber(L, 1)) - 1));
        return 1;
    }

    luaL_error(L, "Usage: GetModifiedClickAction(index)");
    return 0;
}

// OFFSET: 0x00922ED0/0x140686940
static int32_t Script_IsModifiedClick(lua_State* L) {
    if (CGUIBindings::s_bindings->IsModifiedClick(lua_tolstring(L, 1, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 088accb5 IDENTICAL/IDENTICAL
// OFFSET: 0x009278F0/0x14068D2D0
static int32_t Script_SaveBindings(lua_State* L) {
    int32_t set = static_cast<int32_t>(lua_tonumber(L, 1));

    if (set != 1 && set != 2) {
        luaL_error(L, "Usage: SaveBindings(1||2)");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    bindings->m_saveSet = set;
    bindings->CopyBindings(BINDING_SET_CURRENT, static_cast<BINDING_SET>(set));

    if (set == 1) {
        bindings->CopyBindings(BINDING_SET_ACCOUNT, BINDING_SET_CHARACTER);
    }

    return 0;
}

// OFFSET: 0x00922DA0/0x1406867A0
static int32_t Script_SetModifiedClick(lua_State* L) {
    if (lua_isstring(L, 1)) {
        CGUIBindings* bindings = CGUIBindings::s_bindings;
        const char* action = lua_tolstring(L, 1, NULL);
        const char* binding = lua_tolstring(L, 2, NULL);
        MODIFIEDCLICK* click;

        if (!action || !(click = bindings->m_modifiedClicks.Ptr(action)) || !click->SetBinding(BINDING_SET_CURRENT, binding)) {
            luaL_error(L, "SetModifiedClick(): Unknown action (%s) or binding (%s)", action, binding);
            return 0;
        }

        return 0;
    }

    luaL_error(L, "Usage: SetModifiedClick(\"action\", \"binding\")");
    return 0;
}

// PROVEN: 1410cb72 IDENTICAL/DIFFERENT
// OFFSET: 0x00922E30/0x140686870
static int32_t Script_GetModifiedClick(lua_State* L) {
    if (lua_isstring(L, 1)) {
        CGUIBindings* bindings = CGUIBindings::s_bindings;
        const char* action = lua_tolstring(L, 1, NULL);
        MODIFIEDCLICK* click;

        if (action && (click = bindings->m_modifiedClicks.Ptr(action))) {
            char binding[128];
            click->GetBinding(BINDING_SET_CURRENT, binding, sizeof(binding), NULL);
            lua_pushstring(L, binding);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    luaL_error(L, "Usage: GetModifiedClick(\"action\")");
    return 0;
}

// OFFSET: 0x00922B10/0x140686350
static int32_t Script_GetBindingAction(lua_State* L) {
    if (lua_isstring(L, 1)) {
        BINDING_MODE mode = BINDING_MODE_ACTIVE;

        if (lua_isnumber(L, 3)) {
            int32_t value = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

            if (value >= 0 && value <= 3) {
                mode = static_cast<BINDING_MODE>(value);
            }
        }

        CGUIBindings* bindings = CGUIBindings::s_bindings;
        int32_t checkOverride = StringToBOOL(L, 2, 0);
        const char* action = bindings->GetCommandAction(mode, lua_tolstring(L, 1, NULL), checkOverride);

        if (!action) {
            action = "";
        }

        lua_pushstring(L, action);
        return 1;
    }

    luaL_error(L, "Usage: GetBindingAction(\"KEY\"[, checkOverride][, mode])");
    return 0;
}

// PROVEN: 88ea25da IDENTICAL/DIFFERENT
// OFFSET: 0x00923AE0/0x140687A60
static int32_t Script_ClearOverrideBindings(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType())) {
        luaL_error(L, "Usage: ClearOverrideBindings(owner)");
        return 0;
    }

    CGUIBindings::s_bindings->ClearOverrideBindings(owner, true);
    return 0;
}

// OFFSET: 0x00922BC0/0x140686430
static int32_t Script_GetBindingByKey(lua_State* L) {
    if (lua_isstring(L, 1)) {
        BINDING_MODE mode = BINDING_MODE_ACTIVE;

        if (lua_isnumber(L, 2)) {
            int32_t value = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

            if (value >= 0 && value <= 3) {
                mode = static_cast<BINDING_MODE>(value);
            }
        }

        CGUIBindings* bindings = CGUIBindings::s_bindings;
        char key[128];
        KEYBINDING* binding = bindings->GetReducedKeyBinding(mode, lua_tolstring(L, 1, NULL), key, sizeof(key));

        if (!binding) {
            return 0;
        }

        lua_pushstring(L, bindings->GetBindingCommand(binding, mode));
        return 1;
    }

    luaL_error(L, "Usage: GetBindingByKey(\"action\"[, mode])");
    return 0;
}

// OFFSET: 0x00922C70/0x140686570
static int32_t Script_RunBinding(lua_State* L) {
    if (lua_isstring(L, 1)) {
        CGUIBindings* bindings = CGUIBindings::s_bindings;
        int32_t isDown = 1;
        float pressure;

        if (!lua_isstring(L, 2) || SStrCmpI(lua_tolstring(L, 2, NULL), "up", STORM_MAX_STR)) {
            pressure = 1.0f;
        } else {
            pressure = 0.0f;
            isDown = 0;
        }

        const char* taint = lua_tainted;
        bindings->ExecCommand(lua_tolstring(L, 1, NULL), isDown, pressure, 1, 0, 0, -1.0f, 0.0f, taint);
        return 0;
    }

    luaL_error(L, "Usage: RunBinding(\"COMMAND\")");
    return 0;
}

// OFFSET: 0x00926CE0/0x14068C450
static int32_t Script_SetBinding(lua_State* L) {
    if (lua_isstring(L, 1)) {
        BINDING_MODE mode = BINDING_MODE_0;

        if (lua_isnumber(L, 3)) {
            int32_t value = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

            if (value >= 0 && value <= 3) {
                mode = static_cast<BINDING_MODE>(value);
            }
        }

        CGUIBindings* bindings = CGUIBindings::s_bindings;
        const char* command = lua_tolstring(L, 2, NULL);

        if (bindings->Bind(BINDING_SET_CURRENT, mode, lua_tolstring(L, 1, NULL), command)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BINDINGS, NULL);
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    luaL_error(L, "Usage: SetBinding(\"KEY\"[, \"COMMAND\"][, mode])");
    return 0;
}

// PROVEN: 04d72169 IDENTICAL/DIFFERENT
// OFFSET: 0x00922970/0x140686120
static int32_t Script_GetBinding(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGUIBindings* bindings = CGUIBindings::s_bindings;
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t count = 0;
        BINDING_MODE mode = BINDING_MODE_0;

        if (lua_isnumber(L, 2)) {
            int32_t value = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

            if (value >= 0 && value <= 3) {
                mode = static_cast<BINDING_MODE>(value);
            }
        }

        const char* command = "";
        bindings->GetCommand(index, &command);
        lua_pushstring(L, command);

        for (const char* key = bindings->GetCommandKey(mode, command, 0); key; key = bindings->GetCommandKey(mode, command, count)) {
            count++;
            lua_pushstring(L, key);
        }

        return count + 1;
    }

    luaL_error(L, "Usage: GetBinding(index[, mode])");
    return 0;
}

// PROVEN: 6339e57a IDENTICAL/DIFFERENT
// OFFSET: 0x009272C0/0x14068CAF0
static int32_t Script_SetOverrideBinding(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType()) || !lua_isstring(L, 3)) {
        luaL_error(L, "Usage: SetOverrideBinding(owner, isPriority, \"KEY\"[, \"COMMAND\"])");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* command = NULL;

    if (lua_isstring(L, 4)) {
        command = lua_tolstring(L, 4, NULL);
    }

    const char* key = lua_tolstring(L, 3, NULL);
    bindings->SetOverrideBinding(owner, StringToBOOL(L, 2, 0), key, command, true);
    return 0;
}

// PROVEN: ff698ba7 IDENTICAL/DIFFERENT
// OFFSET: 0x00926DB0/0x14068C550
static int32_t Script_SetBindingSpell(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetBindingSpell(\"KEY\", \"spellname\"[, mode])");
        return 0;
    }

    BINDING_MODE mode = BINDING_MODE_0;

    if (lua_isnumber(L, 3)) {
        int32_t value = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

        if (value >= 0 && value <= 3) {
            mode = static_cast<BINDING_MODE>(value);
        }
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 2, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "SPELL %s", argument);

    const char* key = lua_tolstring(L, 1, NULL);

    if (bindings->Bind(BINDING_SET_CURRENT, mode, key, command)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BINDINGS, NULL);
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 11cdf391 IDENTICAL/DIFFERENT
// OFFSET: 0x00926EE0/0x14068C6A0
static int32_t Script_SetBindingItem(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetBindingItem(\"KEY\", \"itemname\"[, mode])");
        return 0;
    }

    BINDING_MODE mode = BINDING_MODE_0;

    if (lua_isnumber(L, 3)) {
        int32_t value = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

        if (value >= 0 && value <= 3) {
            mode = static_cast<BINDING_MODE>(value);
        }
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 2, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "ITEM %s", argument);

    const char* key = lua_tolstring(L, 1, NULL);

    if (bindings->Bind(BINDING_SET_CURRENT, mode, key, command)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BINDINGS, NULL);
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 545669fc IDENTICAL/DIFFERENT
// OFFSET: 0x00927010/0x14068C7F0
static int32_t Script_SetBindingMacro(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetBindingMacro(\"KEY\", \"macroname\"|macroid[, mode])");
        return 0;
    }

    BINDING_MODE mode = BINDING_MODE_0;

    if (lua_isnumber(L, 3)) {
        int32_t value = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

        if (value >= 0 && value <= 3) {
            mode = static_cast<BINDING_MODE>(value);
        }
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 2, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "MACRO %s", argument);

    const char* key = lua_tolstring(L, 1, NULL);

    if (bindings->Bind(BINDING_SET_CURRENT, mode, key, command)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BINDINGS, NULL);
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: d2e9620f IDENTICAL/DIFFERENT
// OFFSET: 0x00927140/0x14068C940
static int32_t Script_SetBindingClick(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetBindingClick(\"KEY\", \"buttonName\"[, \"mouseButton\"][, mode])");
        return 0;
    }

    BINDING_MODE mode = BINDING_MODE_0;

    if (lua_isnumber(L, 4)) {
        int32_t value = static_cast<int32_t>(lua_tonumber(L, 4)) - 1;

        if (value >= 0 && value <= 3) {
            mode = static_cast<BINDING_MODE>(value);
        }
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* button = lua_tolstring(L, 2, NULL);
    const char* mouseButton = lua_tolstring(L, 3, NULL);

    if (!mouseButton) {
        mouseButton = "LeftButton";
    }

    size_t length = SStrLen(button) + SStrLen(mouseButton) + 8;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "CLICK %s:%s", button, mouseButton);

    const char* key = lua_tolstring(L, 1, NULL);

    if (bindings->Bind(BINDING_SET_CURRENT, mode, key, command)) {
        if (++s_setBindingClickDepth < 10) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BINDINGS, NULL);
        }

        s_setBindingClickDepth--;
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: eec0225a IDENTICAL/DIFFERENT
// OFFSET: 0x009273B0/0x14068CC40
static int32_t Script_SetOverrideBindingSpell(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType()) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
        luaL_error(L, "Usage: SetOverrideBindingSpell(owner, isPriority, \"KEY\", \"spellname\")");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 4, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "SPELL %s", argument);
    const char* key = lua_tolstring(L, 3, NULL);
    bindings->SetOverrideBinding(owner, StringToBOOL(L, 2, 0), key, command, true);
    return 0;
}

// PROVEN: dfcde78f IDENTICAL/DIFFERENT
// OFFSET: 0x009274E0/0x14068CDC0
static int32_t Script_SetOverrideBindingItem(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType()) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
        luaL_error(L, "Usage: SetOverrideBindingItem(owner, isPriority, \"KEY\", \"itemname\")");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 4, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "ITEM %s", argument);
    const char* key = lua_tolstring(L, 3, NULL);
    bindings->SetOverrideBinding(owner, StringToBOOL(L, 2, 0), key, command, true);
    return 0;
}

// PROVEN: a86ec422 IDENTICAL/DIFFERENT
// OFFSET: 0x00927610/0x14068CF40
static int32_t Script_SetOverrideBindingMacro(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType()) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
        luaL_error(L, "Usage: SetOverrideBindingMacro(owner, isPriority, \"KEY\", \"macroname\"|macroid)");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* argument = lua_tolstring(L, 4, NULL);
    size_t length = SStrLen(argument) + 7;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "MACRO %s", argument);
    const char* key = lua_tolstring(L, 3, NULL);
    bindings->SetOverrideBinding(owner, StringToBOOL(L, 2, 0), key, command, true);
    return 0;
}

// PROVEN: 372fccdb IDENTICAL/DIFFERENT
// OFFSET: 0x00927740/0x14068D0C0
static int32_t Script_SetOverrideBindingClick(lua_State* L) {
    CScriptObject* owner = NULL;

    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        owner = static_cast<CScriptObject*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!owner || !owner->IsA(CSimpleFrame::GetObjectType()) || !lua_isstring(L, 3) || !lua_isstring(L, 4)) {
        luaL_error(L, "Usage: SetOverrideBindingClick(owner, isPriority, \"KEY\", \"buttonName\"[, \"mouseButton\"])");
        return 0;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    const char* button = lua_tolstring(L, 4, NULL);
    const char* mouseButton = lua_tolstring(L, 5, NULL);

    if (!mouseButton) {
        mouseButton = "LeftButton";
    }

    size_t length = SStrLen(button) + SStrLen(mouseButton) + 8;
    char* command = static_cast<char*>(alloca(length));
    SStrPrintf(command, length, "CLICK %s:%s", button, mouseButton);
    const char* key = lua_tolstring(L, 3, NULL);
    bindings->SetOverrideBinding(owner, StringToBOOL(L, 2, 0), key, command, true);
    return 0;
}

// OFFSET: 0x00927960/0x14068D350
static int32_t Script_GetClickFrame(lua_State* L) {
    if (lua_isstring(L, 1)) {
        const char* name = lua_tolstring(L, 1, NULL);
        CSimpleFrame* frame = GetClickFrame(L, name, SStrLen(name));

        if (frame) {
            if (!frame->m_luaRegistered) {
                frame->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, frame->m_luaObjectRef);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    luaL_error(L, "Usage: GetClickFrame(\"name\")");
    return 0;
}

// STATIC: 0x00D114D0/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumBindings", &Script_GetNumBindings },
    { "GetBinding", &Script_GetBinding },
    { "SetBinding", &Script_SetBinding },
    { "SetBindingSpell", &Script_SetBindingSpell },
    { "SetBindingItem", &Script_SetBindingItem },
    { "SetBindingMacro", &Script_SetBindingMacro },
    { "SetBindingClick", &Script_SetBindingClick },
    { "SetOverrideBinding", &Script_SetOverrideBinding },
    { "SetOverrideBindingSpell", &Script_SetOverrideBindingSpell },
    { "SetOverrideBindingItem", &Script_SetOverrideBindingItem },
    { "SetOverrideBindingMacro", &Script_SetOverrideBindingMacro },
    { "SetOverrideBindingClick", &Script_SetOverrideBindingClick },
    { "ClearOverrideBindings", &Script_ClearOverrideBindings },
    { "GetBindingKey", &Script_GetBindingKey },
    { "GetBindingAction", &Script_GetBindingAction },
    { "GetBindingByKey", &Script_GetBindingByKey },
    { "RunBinding", &Script_RunBinding },
    { "GetCurrentBindingSet", &Script_GetCurrentBindingSet },
    { "LoadBindings", &Script_LoadBindings },
    { "SaveBindings", &Script_SaveBindings },
    { "GetNumModifiedClickActions", &Script_GetNumModifiedClickActions },
    { "GetModifiedClickAction", &Script_GetModifiedClickAction },
    { "SetModifiedClick", &Script_SetModifiedClick },
    { "GetModifiedClick", &Script_GetModifiedClick },
    { "IsModifiedClick", &Script_IsModifiedClick },
    { "GetClickFrame", &Script_GetClickFrame },
};

// PROVEN: 4be9aead IDENTICAL/UNPROVEN
// OFFSET: 0x00921460/0x140683C50
void UIBindingsRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: e5537092 IDENTICAL/UNPROVEN
// OFFSET: 0x00921490/0x140683CA0
void UIBindingsUnegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
