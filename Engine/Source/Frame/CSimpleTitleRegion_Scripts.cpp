#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// PROVEN: 972105ca IDENTICAL/NONE
// OFFSET: 0x0050AFA0
void CSimpleTitleRegion::RegisterScriptMethods(lua_State* L) {
    CScriptRegion::RegisterScriptMethods(L);
}

// OFFSET: %0x0051B3C0
void CSimpleTitleRegion::CreateScriptMetaTable() {
    CSimpleTitleRegion::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleTitleRegion::RegisterScriptMethods);
}

// OFFSET: %0x0051B030
void CSimpleTitleRegion::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleTitleRegion::s_metatable);
    CSimpleTitleRegion::s_metatable = LUA_NOREF;
}
