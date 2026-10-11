#include "DanceStudio.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x009B8380/0x140742B30
static int32_t Script_PlayDance(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PlayDance(\"dance name\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (name) {
        PlayDance(name);
    }

    return 0;
}

// STATIC: 0x00D159C4
static FrameScript_Method s_ScriptFunctions[] = {
    { "PlayDance", &Script_PlayDance },
};

// PROVEN: 574e2ad8 IDENTICAL/NONE
// OFFSET: 0x009B7CF0/0x140742260
void CGDanceStudio::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: fd7a5340 IDENTICAL/NONE
// OFFSET: 0x009B7D10/0x140742280
void CGDanceStudio::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
