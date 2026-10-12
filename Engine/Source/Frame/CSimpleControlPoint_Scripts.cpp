#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x0053CEF0/0x140191190
static int32_t Script_SetParent(lua_State* L) {
    CSimpleControlPoint* point = static_cast<CSimpleControlPoint*>(FrameScript_GetObjectThis(L, CSimpleControlPoint::GetObjectType()));

    if (lua_type(L, 2) == LUA_TNIL) {
        const char* name = point->GetName();
        luaL_error(L, "%s:SetParent(): Cannot set a 'nil' parent for control points", name ? name : "<unnamed>");
        return 0;
    }

    CSimplePathAnim* path = NULL;

    if (lua_isstring(L, 2)) {
        path = static_cast<CSimplePathAnim*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 2, NULL), CSimplePathAnim::GetObjectType()));
    } else if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        path = static_cast<CSimplePathAnim*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!path) {
            const char* name = point->GetName();
            luaL_error(L, "%s:SetParent(): Couldn't find 'this' in parent object", name ? name : "<unnamed>");
            return 0;
        }

        if (!path->IsA(CSimplePathAnim::GetObjectType())) {
            luaL_error(L, "%s:SetParent(): Wrong parent object type, expected Path", point->GetDisplayName());
            return 0;
        }
    }

    if (!path) {
        const char* name = point->GetName();
        luaL_error(L, "%s:SetParent(): Couldn't find Path named '%s'", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    int32_t oldOrder = point->m_order;

    if (lua_isnumber(L, 3)) {
        point->SetOrder(lua_tointeger(L, 3) - 1, false, false);
    } else {
        int32_t order = point->m_order;

        if (order == -1) {
            order = path->GetMaxControlPointOrder() + 1;
        }

        point->SetOrder(order, false, true);
    }

    if (!point->ChangeParent(path) && oldOrder != point->m_order) {
        path->NotifyOrderChanged(point);
    }

    return 0;
}

// OFFSET: 0x0053D0E0/0x140191440
static int32_t Script_SetOffset(lua_State* L) {
    CSimpleControlPoint* point = static_cast<CSimpleControlPoint*>(FrameScript_GetObjectThis(L, CSimpleControlPoint::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = point->GetName();
        luaL_error(L, "Usage: %s:SetOffset(x, y)", name ? name : "<unnamed>");
        return 0;
    }

    C2Vector offset;
    float x = lua_tonumber(L, 2);
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    float y = lua_tonumber(L, 3);
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    point->SetOffset(offset);

    return 0;
}

// OFFSET: 0x0053D1D0/0x140191570
static int32_t Script_GetOffset(lua_State* L) {
    CSimpleControlPoint* point = static_cast<CSimpleControlPoint*>(FrameScript_GetObjectThis(L, CSimpleControlPoint::GetObjectType()));

    float x = point->m_offset.x;
    float y = point->m_offset.y;
    x = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x);
    y = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

// OFFSET: 0x0053D260/0x140191620
static int32_t Script_SetOrder(lua_State* L) {
    CSimpleControlPoint* point = static_cast<CSimpleControlPoint*>(FrameScript_GetObjectThis(L, CSimpleControlPoint::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = point->GetName();
        luaL_error(L, "Usage: %s:SetOrder(order)", name ? name : "<unnamed>");
        return 0;
    }

    point->SetOrder(lua_tointeger(L, 2) - 1, true, false);
    return 0;
}

// OFFSET: 0x0053D2E0/0x1401916D0
static int32_t Script_GetOrder(lua_State* L) {
    CSimpleControlPoint* point = static_cast<CSimpleControlPoint*>(FrameScript_GetObjectThis(L, CSimpleControlPoint::GetObjectType()));
    lua_pushinteger(L, point->m_order + 1);
    return 1;
}

// OFFSET: 0x0053D320/0x140191720
void CSimpleControlPoint::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleControlPoint::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleControlPoint::RegisterScriptMethods);
}

// OFFSET: 0x0053B9E0/0x14018F7E0
void CSimpleControlPoint::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleControlPoint::s_metatable);
    CSimpleControlPoint::s_metatable = LUA_NOREF;
}

// STATIC: 0x00CA4770/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "SetParent", &Script_SetParent },
    { "SetOffset", &Script_SetOffset },
    { "GetOffset", &Script_GetOffset },
    { "SetOrder", &Script_SetOrder },
    { "GetOrder", &Script_GetOrder },
};

// OFFSET: 0x0053B9C0/0x14018F7B0
void CSimpleControlPoint::RegisterScriptMethods(lua_State* L) {
    CScriptObject::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
