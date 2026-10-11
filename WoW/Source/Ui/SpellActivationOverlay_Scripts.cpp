#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x009F2460/0x14078CBC0
static int32_t Script_IsSpellOverlayed(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsSpellOverlayed(spellID)");
    }

    lua_pushboolean(L, CGSpellActivationOverlay::IsSpellOverlayed(lua_tointeger(L, 1)));

    return 1;
}

// STATIC: 0x00D16E3C
static FrameScript_Method s_ScriptFunctions[] = {
    { "IsSpellOverlayed", &Script_IsSpellOverlayed },
};

// OFFSET: 0x009F2090
void SpellActivationOverlayRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009F20B0
void SpellActivationOverlayUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
