#include "WoW/Source/Ui/IncomingResurrection.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x009FD000/0x14079ADA0
static int32_t Script_UnitHasIncomingResurrection(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHasIncomingResurrection(\"unit\")");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tostring(L, 1));
    lua_pushboolean(L, CGIncomingResurrection::HasIncomingResurrection(guid));

    return 1;
}

// STATIC: 0x00D1744C/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "UnitHasIncomingResurrection", &Script_UnitHasIncomingResurrection },
};

// OFFSET: 0x009FCE30/0x14079ABB0
void IncomingResurrectionRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009FCE50/0x14079ABD0
void IncomingResurrectionUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
