#include "CGMinigameInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Minigame/Minigame_c.h"

// STATIC: 0x00D179FC/0x140BD2B40
static const char* s_minigameNames[] = { "TicTacToe", NULL };

// OFFSET: 0x00A09FD0/0x1407AAFF0
static int32_t Script_GetMinigameType(lua_State* L) {
    CMinigame_C* minigame = CGMinigameInfo::m_minigame;

    if (minigame && minigame->m_type - 1 <= 0) {
        lua_pushstring(L, s_minigameNames[minigame->m_type]);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00A0A020/0x1407AB040
static int32_t Script_MakeMinigameMove(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: MakeMinigameMove(moveType, param)");
        return 0;
    }

    uint8_t moveType = static_cast<int32_t>(lua_tonumber(L, 1) - 1.0);
    int32_t param = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (CGMinigameInfo::m_minigame) {
        CGMinigameInfo::m_minigame->MakeMove(moveType, param);
    }

    return 0;
}

// OFFSET: 0x00A0A0C0/0x1407AB0E0
static int32_t Script_GetMinigameState(lua_State* L) {
    return CGMinigameInfo::m_minigame ? CGMinigameInfo::m_minigame->PushStateToLua(L) : 0;
}

// STATIC: 0x00D17A04/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetMinigameType", &Script_GetMinigameType },
    { "MakeMinigameMove", &Script_MakeMinigameMove },
    { "GetMinigameState", &Script_GetMinigameState },
};

// OFFSET: 0x00A0A0E0/0x1407AB100
void MinigameInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00A0A110/0x1407AB150
void MinigameInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
