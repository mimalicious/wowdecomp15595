#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x0050AFA0/0x1401533E0
void CSimpleTitleRegion::RegisterScriptMethods(lua_State* L) {
    CScriptRegion::RegisterScriptMethods(L);
}

// OFFSET: 0x0050B030/0x140153480
void CSimpleTitleRegion::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleTitleRegion::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleTitleRegion::RegisterScriptMethods);
}

// OFFSET: 0x0050AFB0/0x1401533F0
void CSimpleTitleRegion::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleTitleRegion::s_metatable);
    CSimpleTitleRegion::s_metatable = LUA_NOREF;
}
