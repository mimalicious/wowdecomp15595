#include "NamePlateFrame.h"
#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"

// PROVEN: 12a315af IDENTICAL/IDENTICAL
// OFFSET: 0x009E74F0/0x14077FF50
static int32_t Script_GetNumNamePlateMotionTypes(lua_State* L) {
    lua_pushnumber(L, 3.0);
    return 1;
}

// PROVEN: 4bdc7450 IDENTICAL/IDENTICAL
// OFFSET: 0x009E80C0/0x140780D90
static int32_t Script_GetNamePlateMotionType(lua_State* L) {
    lua_pushnumber(L, CGNamePlateFrame::s_cvNamePlateMotion->m_intValue + 1);
    return 1;
}

// PROVEN: 44048fec DIFFERENT/IDENTICAL
// OFFSET: 0x009EA5A0/0x140783740
static int32_t Script_SetNamePlateMotionType(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetNamePlateMotionType(type)");
        return 0;
    }

    CGNamePlateFrame::s_cvNamePlateMotion->Set(static_cast<int32_t>(lua_tointeger(L, 1)) - 1, true, false, false, true);

    for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Head(); !CGNamePlateFrame::s_namePlates.IsEnd(frame); frame = CGNamePlateFrame::s_namePlates.RawNext(frame)) {
        frame->m_offset.x = 0.0f;
        frame->m_offset.y = 0.0f;
        frame->m_offset.z = 0.0f;
    }

    return 0;
}

// STATIC: 0x00D16AB0/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNamePlateMotionType", &Script_GetNamePlateMotionType },
    { "SetNamePlateMotionType", &Script_SetNamePlateMotionType },
    { "GetNumNamePlateMotionTypes", &Script_GetNumNamePlateMotionTypes },
};

// PROVEN: e507afbe IDENTICAL/UNPROVEN
// OFFSET: 0x009E7520/0x14077FF70
void NamePlateRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 3b4a6f82 IDENTICAL/UNPROVEN
// OFFSET: 0x009E7550/0x14077FFC0
void NamePlateUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
