#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTitleRegion.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: %0x00BB8348
int32_t CSimpleTitleRegion::s_metatable = LUA_NOREF;

// OFFSET: %0x00512970
int32_t CSimpleTitleRegion::GetScriptMetaTable() {
    return CSimpleTitleRegion::s_metatable;
}
