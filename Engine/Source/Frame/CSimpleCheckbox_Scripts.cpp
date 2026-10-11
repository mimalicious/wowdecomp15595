#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// PROVEN: bf9a51c2 IDENTICAL/IDENTICAL
// OFFSET: 0x00532690/0x1401847C0
static int32_t Script_SetChecked(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));

    checkbox->SetChecked(StringToBOOL(L, 2, 1), 0);
    return 0;
}

// PROVEN: 2dc76d4c IDENTICAL/NONE
// OFFSET: 0x005326E0/0x140184820
static int32_t Script_GetChecked(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));

    if (checkbox->m_checked) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00532740/0x140184880
static int32_t Script_GetCheckedTexture(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));
    CSimpleTexture* texture = checkbox->m_checkedTexture;

    if (!texture) {
        lua_pushnil(L);
        return 1;
    }

    if (!texture->m_luaRegistered) {
        texture->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, texture->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x005327B0/0x140184910
static int32_t Script_SetCheckedTexture(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = checkbox->GetName();
            luaL_error(L, "%s:SetCheckedTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = checkbox->GetName();
            luaL_error(L, "%s:SetCheckedTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        checkbox->SetCheckedTexture(texture);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        checkbox->SetCheckedTexture(lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = checkbox->GetName();
        luaL_error(L, "Usage: %s:SetCheckedTexture(texture or \"texture\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    checkbox->SetCheckedTexture(static_cast<CSimpleTexture*>(NULL));
    return 0;
}

// OFFSET: 0x00532910/0x140184AA0
static int32_t Script_GetDisabledCheckedTexture(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));
    CSimpleTexture* texture = checkbox->m_disabledCheckedTexture;

    if (!texture) {
        lua_pushnil(L);
        return 1;
    }

    if (!texture->m_luaRegistered) {
        texture->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, texture->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x00532980/0x140184B30
static int32_t Script_SetDisabledCheckedTexture(lua_State* L) {
    CSimpleCheckbox* checkbox = static_cast<CSimpleCheckbox*>(FrameScript_GetObjectThis(L, CSimpleCheckbox::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = checkbox->GetName();
            luaL_error(L, "%s:SetDisabledCheckedTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = checkbox->GetName();
            luaL_error(L, "%s:SetDisabledCheckedTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        checkbox->SetDisabledCheckedTexture(texture);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        checkbox->SetDisabledCheckedTexture(lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = checkbox->GetName();
        luaL_error(L, "Usage: %s:SetDisabledCheckedTexture(texture or \"texture\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    checkbox->SetDisabledCheckedTexture(static_cast<CSimpleTexture*>(NULL));
    return 0;
}

// PROVEN: f113b968 IDENTICAL/NONE
// OFFSET: 0x00532640
void CSimpleCheckbox::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleCheckbox::s_metatable);
    CSimpleCheckbox::s_metatable = LUA_NOREF;
}

// PROVEN: 81972cd5 IDENTICAL/NONE
// OFFSET: 0x00532AE0
void CSimpleCheckbox::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleCheckbox::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleCheckbox::RegisterScriptMethods);
}

// STATIC: 0x00CA3DCC
static FrameScript_Method s_ScriptMethods[] = {
    { "SetChecked", &Script_SetChecked },
    { "GetChecked", &Script_GetChecked },
    { "GetCheckedTexture", &Script_GetCheckedTexture },
    { "SetCheckedTexture", &Script_SetCheckedTexture },
    { "GetDisabledCheckedTexture", &Script_GetDisabledCheckedTexture },
    { "SetDisabledCheckedTexture", &Script_SetDisabledCheckedTexture },
};

// PROVEN: 95e82c38 IDENTICAL/UNPROVEN
// OFFSET: 0x00532620/0x140184760
void CSimpleCheckbox::RegisterScriptMethods(lua_State* L) {
    CSimpleButton::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
