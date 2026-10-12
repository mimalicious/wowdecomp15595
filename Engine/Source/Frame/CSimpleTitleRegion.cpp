#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00CA2FA0/0x140B3FDF0
int32_t CSimpleTitleRegion::s_metatable = LUA_NOREF;

// PROVEN: b7bf3d42 IDENTICAL/IDENTICAL
// OFFSET: 0x00502820/0x1401486A0
int32_t CSimpleTitleRegion::GetScriptMetaTable() {
    return CSimpleTitleRegion::s_metatable;
}
