#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0054D170
static int32_t Script_SetCurve(lua_State* L) {
    CSimplePathAnim* path = static_cast<CSimplePathAnim*>(FrameScript_GetObjectThis(L, CSimplePathAnim::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = path->GetName();
        luaL_error(L, "Usage: %s:SetCurve(\"curveType\")", name ? name : "<unnamed>");
        return 0;
    }

    SimpleAnimCurveType curve;

    if (StringToCurveType(lua_tolstring(L, 2, NULL), curve)) {
        path->m_curve = curve;
        return 0;
    }

    const char* name = path->GetName();
    luaL_error(L, "%s:SetCurve(): Unknown curve type specified", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x0054D230
static int32_t Script_GetCurve(lua_State* L) {
    CSimplePathAnim* path = static_cast<CSimplePathAnim*>(FrameScript_GetObjectThis(L, CSimplePathAnim::GetObjectType()));
    lua_pushstring(L, CurveTypeToString(static_cast<SimpleAnimCurveType>(path->m_curve)));
    return 1;
}

// OFFSET: %0x0054D280
static int32_t Script_GetMaxOrder(lua_State* L) {
    CSimplePathAnim* path = static_cast<CSimplePathAnim*>(FrameScript_GetObjectThis(L, CSimplePathAnim::GetObjectType()));
    int32_t order = path->GetMaxControlPointOrder() + 1;
    lua_pushnumber(L, order);
    return 1;
}

// OFFSET: %0x0054DB90
static int32_t Script_GetControlPoints(lua_State* L) {
    CSimplePathAnim* path = static_cast<CSimplePathAnim*>(FrameScript_GetObjectThis(L, CSimplePathAnim::GetObjectType()));
    uint32_t count = path->m_sortedPoints.Count();

    if (!count) {
        return 0;
    }

    if (!lua_checkstack(L, count)) {
        const char* name = path->GetName();
        luaL_error(L, "%s:GetControlPoints(): Stack overflow", name ? name : "<unnamed>");
        return 0;
    }

    for (uint32_t i = 0; i < count; i++) {
        CSimpleControlPoint* point = path->GetControlPoint(i);

        if (!point->m_luaRegistered) {
            point->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, point->m_luaObjectRef);
    }

    return count;
}

// OFFSET: %0x0054DD90
static int32_t Script_CreateControlPoint(lua_State* L) {
    CSimplePathAnim* path = static_cast<CSimplePathAnim*>(FrameScript_GetObjectThis(L, CSimplePathAnim::GetObjectType()));
    XMLHashNodeRef ref;
    const char* name = NULL;

    if (lua_isstring(L, 2)) {
        name = lua_tolstring(L, 2, NULL);
    }

    if (lua_type(L, 3) == LUA_TSTRING) {
        const char* inherits = lua_tolstring(L, 3, NULL);

        if (!ref.Acquire(inherits)) {
            const char* pathName = path->GetName();
            luaL_error(L, "%s:CreateControlPoint(): Couldn't find inherited node \"%s\"", pathName ? pathName : "<unnamed>", inherits);
            return 0;
        }

        if (ref.m_locked) {
            const char* pathName = path->GetName();
            luaL_error(L, "%s:CreateControlPoint(): Recursively inherited node \"%s\"", pathName ? pathName : "<unnamed>", inherits);
            return 0;
        }
    }

    void* mem = SMemAlloc(sizeof(CSimpleControlPoint), __FILE__, __LINE__, 0);
    CSimpleControlPoint* point = mem ? new (mem) CSimpleControlPoint(path) : NULL;

    if (!point) {
        lua_pushnil(L);
        return 1;
    }

    if (name && *name) {
        point->SetName(name);
    }

    if (ref.m_node) {
        CStatus status;
        point->LoadXML(ref.m_node, &status);
    }

    if (lua_isnumber(L, 4)) {
        point->SetOrder(lua_tointeger(L, 4) - 1, false, false);
    }

    if (point->m_order == -1) {
        point->SetOrder(path->GetMaxControlPointOrder(), false, true);
    }

    path->NotifyControlPointAdded(point);

    if (!point->m_luaRegistered) {
        point->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, point->m_luaObjectRef);
    return 1;
}

// PROVEN: 9943457d IDENTICAL/NONE
// OFFSET: 0x0053D4B0
void CSimplePathAnim::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimplePathAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimplePathAnim::RegisterScriptMethods);
}

// PROVEN: 213e571e IDENTICAL/NONE
// OFFSET: 0x0053BA40
void CSimplePathAnim::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimplePathAnim::s_metatable);
    CSimplePathAnim::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB9AD4
static FrameScript_Method s_ScriptMethods[] = {
    { "SetCurve", &Script_SetCurve },
    { "GetCurve", &Script_GetCurve },
    { "GetControlPoints", &Script_GetControlPoints },
    { "CreateControlPoint", &Script_CreateControlPoint },
    { "GetMaxOrder", &Script_GetMaxOrder },
};

// OFFSET: %0x0054B720
void CSimplePathAnim::RegisterScriptMethods(lua_State* L) {
    CSimpleAnim::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
