#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x0053C950
static int32_t Script_SetOrigin(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetOrigin(point, offsetX, offsetY)", name ? name : "<unnamed>");
        return 0;
    }

    FRAMEPOINT point;
    C2Vector offset;
    ReadOrigin(L, 2, point, offset);

    anim->m_origin = point;
    anim->m_originOffset = offset;

    return 0;
}

// OFFSET: 0x0053CA20/0x140190B70
static int32_t Script_GetOrigin(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));

    float x = anim->m_originOffset.x;
    float y = anim->m_originOffset.y;

    lua_pushstring(L, FramePointToString(anim->m_origin));
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 3;
}

// OFFSET: 0x0053CAA0/0x140190BF0
static int32_t Script_SetDegrees(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetDegrees(degrees)", name ? name : "<unnamed>");
        return 0;
    }

    anim->m_radians = lua_tonumber(L, 2) * 0.017453292f;
    return 0;
}

// OFFSET: 0x0053CB20/0x140190CA0
static int32_t Script_GetDegrees(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_radians * 57.29578f);
    return 1;
}

// OFFSET: 0x0053CB70/0x140190D00
static int32_t Script_SetRadians(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetRadians(radians)", name ? name : "<unnamed>");
        return 0;
    }

    anim->m_radians = lua_tonumber(L, 2);
    return 0;
}

// OFFSET: 0x0053CBF0
static int32_t Script_GetRadians(lua_State* L) {
    CSimpleRotationAnim* anim = static_cast<CSimpleRotationAnim*>(FrameScript_GetObjectThis(L, CSimpleRotationAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_radians);
    return 1;
}

// PROVEN: 6091772f IDENTICAL/NONE
// OFFSET: 0x0053CC40
void CSimpleRotationAnim::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleRotationAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleRotationAnim::RegisterScriptMethods);
}

// PROVEN: e3adba13 IDENTICAL/NONE
// OFFSET: 0x0053B930
void CSimpleRotationAnim::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleRotationAnim::s_metatable);
    CSimpleRotationAnim::s_metatable = LUA_NOREF;
}

// STATIC: 0x00CA4718
static FrameScript_Method s_ScriptMethods[] = {
    { "SetOrigin", &Script_SetOrigin },
    { "GetOrigin", &Script_GetOrigin },
    { "SetDegrees", &Script_SetDegrees },
    { "GetDegrees", &Script_GetDegrees },
    { "SetRadians", &Script_SetRadians },
    { "GetRadians", &Script_GetRadians },
};

// OFFSET: 0x0053B900
void CSimpleRotationAnim::RegisterScriptMethods(lua_State* L) {
    CSimpleAnim::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
