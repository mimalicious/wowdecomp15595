#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x005330B0/0x1401854A0
static int32_t Script_SetHyperlinksEnabled(lua_State* L) {
    CSimpleHyperlinkedFrame* frame = static_cast<CSimpleHyperlinkedFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));
    bool enabled = StringToBOOL(L, 2, 1) != 0;

    if (enabled != frame->m_hyperlinksEnabled) {
        frame->m_hyperlinksEnabled = enabled;
        frame->RefreshHyperlinks();
    }

    return 0;
}

// PROVEN: 5acff0d1 IDENTICAL/NONE
// OFFSET: 0x00533120/0x140185530
static int32_t Script_GetHyperlinksEnabled(lua_State* L) {
    CSimpleHyperlinkedFrame* frame = static_cast<CSimpleHyperlinkedFrame*>(FrameScript_GetObjectThis(L, CSimpleFrame::GetObjectType()));

    if (frame->m_hyperlinksEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00CA3E04
static FrameScript_Method s_ScriptMethods[] = {
    { "SetHyperlinksEnabled", &Script_SetHyperlinksEnabled },
    { "GetHyperlinksEnabled", &Script_GetHyperlinksEnabled },
};

// PROVEN: 45946981 IDENTICAL/NONE
// OFFSET: 0x00532D00/0x140184EE0
void CSimpleHyperlinkedFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
