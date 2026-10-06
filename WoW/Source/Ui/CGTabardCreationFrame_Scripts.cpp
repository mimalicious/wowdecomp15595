#include "CGTabardCreationFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// OFFSET: %0x0092A5D0
static int32_t Script_GetTabardCreationCost(lua_State* L) {
    lua_pushnumber(L, GuildGetTabardCost());
    return 1;
}

// OFFSET: %0x0092A730
static int32_t Script_CloseTabardCreation(lua_State* L) {
    CGTabardCreationFrame::Close();
    return 0;
}

// STATIC: #0x00ACF56C
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTabardCreation", &Script_CloseTabardCreation },
    { "GetTabardCreationCost", &Script_GetTabardCreationCost },
};

// PROVEN: dad72d6f IDENTICAL/NONE
// OFFSET: 0x009DFFC0/0x140776B10
void TabardCreationRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: f34a593d IDENTICAL/NONE
// OFFSET: 0x009DFFF0/0x140776B60
void TabardCreationUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
