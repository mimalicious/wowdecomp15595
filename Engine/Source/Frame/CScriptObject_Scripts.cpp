#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// PROVEN: c9761d84 DIFFERENT/IDENTICAL
// OFFSET: 0x0052EE80/0x140184F10
CScriptObject* FrameScript_GetObjectThis(lua_State* L, int32_t type) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Attempt to find 'this' in non-table object (used '.' instead of ':' ?)");
        return NULL;
    }

    lua_rawgeti(L, 1, 0);
    CScriptObject* object = static_cast<CScriptObject*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!object) {
        luaL_error(L, "Attempt to find 'this' in non-framescript object");
        return NULL;
    }

    if (!object->IsA(type)) {
        luaL_error(L, "Wrong object type for member function");
        return NULL;
    }

    return object;
}

// PROVEN: 6cb01ba8 IDENTICAL/IDENTICAL
// OFFSET: 0x0053E5D0/0x140192D70
static int32_t Script_GetObjectType(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());

    lua_pushstring(L, object->GetObjectTypeName());
    return 1;
}

// PROVEN: 338bf564 DIFFERENT/IDENTICAL
// OFFSET: 0x0053E620/0x140192DC0
static int32_t Script_IsObjectType(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());

    if (!lua_isstring(L, 2)) {
        const char* name = object->GetName();
        luaL_error(L, "Usage: %s:IsObjectType(\"type\")", name ? name : "<unnamed>");
        return 0;
    }

    if (object->IsA(lua_tolstring(L, 2, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 5a9b9a7d IDENTICAL/IDENTICAL
// OFFSET: 0x0053E6D0/0x140192EA0
static int32_t Script_GetName(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());
    const char* name = object->GetName();

    if (name && *name) {
        lua_pushstring(L, name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 924320fd DIFFERENT/IDENTICAL
// OFFSET: 0x0053E730/0x140192F10
static int32_t Script_GetParent(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());
    CScriptObject* parent = object->GetScriptObjectParent();

    if (parent) {
        if (!parent->m_luaRegistered) {
            parent->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00CA48A0/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "GetObjectType", &Script_GetObjectType },
    { "IsObjectType", &Script_IsObjectType },
    { "GetName", &Script_GetName },
    { "GetParent", &Script_GetParent },
};

// PROVEN: a252f575 IDENTICAL/UNPROVEN
// OFFSET: 0x0053E5B0/0x140192D50
void CScriptObject::RegisterScriptMethods(lua_State* L) {
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
