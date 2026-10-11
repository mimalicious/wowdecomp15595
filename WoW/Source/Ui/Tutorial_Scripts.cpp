#include "Tutorial.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x00901D00/0x14065A290
static int32_t Script_FlagTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: FlagTutorial(\"tutorial\")");
        return 0;
    }

    int32_t tutorial = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(tutorial) <= 64) {
        CGTutorial::FlagTutorial(tutorial);
    }

    return 0;
}

// OFFSET: 0x00901F20/0x14065A540
static int32_t Script_TriggerTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Trigger(\"tutorial\")");
        return 0;
    }

    int32_t tutorial = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(tutorial) <= 64) {
        CGTutorial::TriggerTutorial(tutorial);
    }

    return 0;
}

// OFFSET: 0x00901D50/0x14065A2E0
static int32_t Script_IsTutorialFlagged(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Trigger(\"tutorial\")");
        return 0;
    }

    int32_t tutorial = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(tutorial) > 64) {
        return 0;
    }

    if (IsTutorialFlagged(tutorial)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 63a167dd IDENTICAL/NONE
// OFFSET: 0x00901CB0/0x14065A230
static int32_t Script_CanResetTutorials(lua_State* L) {
    for (uint32_t i = CGTutorial::m_tutorialShown.m_bits.m_count; i; ) {
        i--;

        if (CGTutorial::m_tutorialShown.m_bits.m_data[i]) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00901DD0/0x14065A380
static int32_t Script_ClearTutorials(lua_State* L) {
    CGTutorial::ClearTutorials();
    return 0;
}

// OFFSET: 0x00901DE0/0x14065A390
static int32_t Script_ResetTutorials(lua_State* L) {
    CGTutorial::ResetTutorials();
    return 0;
}

// OFFSET: 0x00901750/0x140659B90
static int32_t Script_GetNextCompleatedTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNextCompleatedTutorial(\"tutorial\")");
        return 0;
    }

    int32_t tutorial = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t next = CGTutorial::GetNextCompletedTutorial(tutorial - 1);

    if (next == 65) {
        return 0;
    }

    lua_pushnumber(L, next);
    return 1;
}

// OFFSET: 0x009017C0/0x140659C00
static int32_t Script_GetPrevCompleatedTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPrevCompleatedTutorial(\"tutorial\")");
        return 0;
    }

    int32_t tutorial = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t prev = CGTutorial::GetPrevCompletedTutorial(tutorial - 1);

    if (!prev) {
        return 0;
    }

    lua_pushnumber(L, prev);
    return 1;
}

// STATIC: 0x00D109A0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CanResetTutorials", &Script_CanResetTutorials },
    { "FlagTutorial", &Script_FlagTutorial },
    { "IsTutorialFlagged", &Script_IsTutorialFlagged },
    { "TriggerTutorial", &Script_TriggerTutorial },
    { "ClearTutorials", &Script_ClearTutorials },
    { "ResetTutorials", &Script_ResetTutorials },
    { "GetNextCompleatedTutorial", &Script_GetNextCompleatedTutorial },
    { "GetPrevCompleatedTutorial", &Script_GetPrevCompleatedTutorial },
};

// PROVEN: bf9c0cbc IDENTICAL/NONE
// OFFSET: 0x00901830/0x140659CB0
void TutorialRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: d5d9091c IDENTICAL/NONE
// OFFSET: 0x00901860/0x140659D00
void TutorialUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
