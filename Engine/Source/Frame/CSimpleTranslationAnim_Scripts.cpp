#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x0053C7A0/0x1401907F0
static int32_t Script_SetOffset(lua_State* L) {
    CSimpleTranslationAnim* anim = static_cast<CSimpleTranslationAnim*>(FrameScript_GetObjectThis(L, CSimpleTranslationAnim::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetOffset(x, y)", name ? name : "<unnamed>");
        return 0;
    }

    C2Vector offset;
    float x = lua_tonumber(L, 2);
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    float y = lua_tonumber(L, 3);
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    anim->SetOffset(offset);

    return 0;
}

// OFFSET: 0x0053C890/0x140190920
static int32_t Script_GetOffset(lua_State* L) {
    CSimpleTranslationAnim* anim = static_cast<CSimpleTranslationAnim*>(FrameScript_GetObjectThis(L, CSimpleTranslationAnim::GetObjectType()));

    float x = anim->m_offset.x;
    float y = anim->m_offset.y;
    x = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x);
    y = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}


// PROVEN: 4f1dd3c5 IDENTICAL/NONE
// OFFSET: 0x0053C930
void CSimpleTranslationAnim::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleTranslationAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleTranslationAnim::RegisterScriptMethods);
}

// PROVEN: b3cb05b8 IDENTICAL/NONE
// OFFSET: 0x0053B8D0
void CSimpleTranslationAnim::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleTranslationAnim::s_metatable);
    CSimpleTranslationAnim::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB9A3C
static FrameScript_Method s_ScriptMethods[] = {
    { "SetOffset", &Script_SetOffset },
    { "GetOffset", &Script_GetOffset },
};

// OFFSET: 0x0053B8A0
void CSimpleTranslationAnim::RegisterScriptMethods(lua_State* L) {
    CSimpleAnim::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
