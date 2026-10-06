#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0054D300
static int32_t Script_SetChange(lua_State* L) {
    CSimpleAlphaAnim* anim = static_cast<CSimpleAlphaAnim*>(FrameScript_GetObjectThis(L, CSimpleAlphaAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetChange(change)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetChange(lua_tonumber(L, 2));
    return 0;
}

// OFFSET: %0x0054D380
static int32_t Script_GetChange(lua_State* L) {
    CSimpleAlphaAnim* anim = static_cast<CSimpleAlphaAnim*>(FrameScript_GetObjectThis(L, CSimpleAlphaAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_change * (1.0f / 255.0f));
    return 1;
}

// OFFSET: %0x0054D3E0
void CSimpleAlphaAnim::CreateScriptMetaTable() {
    CSimpleAlphaAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleAlphaAnim::RegisterScriptMethods);
}

// OFFSET: %0x0054B7B0
void CSimpleAlphaAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleAlphaAnim::s_metatable);
    CSimpleAlphaAnim::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB9B00
static FrameScript_Method s_ScriptMethods[] = {
    { "SetChange", &Script_SetChange },
    { "GetChange", &Script_GetChange },
};

// OFFSET: %0x0054B780
void CSimpleAlphaAnim::RegisterScriptMethods(lua_State* L) {
    CSimpleAnim::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
