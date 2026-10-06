#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0054B320
void CSimpleColorSelect::CreateScriptMetaTable() {
    CSimpleColorSelect::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleColorSelect::RegisterScriptMethods);
}

// OFFSET: %0x0054A9C0
void CSimpleColorSelect::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleColorSelect::s_metatable);
    CSimpleColorSelect::s_metatable = LUA_NOREF;
}

// OFFSET: %0x0054A9F0
static int32_t Script_GetColorWheelTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    CSimpleTexture* texture = colorSelect->m_wheelTexture;

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

// OFFSET: %0x0054AA60
static int32_t Script_SetColorWheelTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorWheelTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorWheelTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        colorSelect->SetColorWheelTexture(texture);
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = colorSelect->GetName();
        luaL_error(L, "Usage: %s:SetColorWheelTexture(texture or nil)", name ? name : "<unnamed>");
        return 0;
    }

    colorSelect->SetColorWheelTexture(NULL);
    return 0;
}

// OFFSET: %0x0054AB90
static int32_t Script_GetColorWheelThumbTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    CSimpleTexture* texture = colorSelect->m_wheelThumbTexture;

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

// OFFSET: %0x0054AC00
static int32_t Script_SetColorWheelThumbTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorWheelThumbTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorWheelThumbTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        colorSelect->SetColorWheelThumbTexture(texture);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        colorSelect->SetColorWheelThumbTexture(lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = colorSelect->GetName();
        luaL_error(L, "Usage: %s:SetColorWheelThumbTexture(texture or \"texture\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    colorSelect->SetColorWheelThumbTexture(static_cast<CSimpleTexture*>(NULL));
    return 0;
}

// OFFSET: %0x0054AD60
static int32_t Script_GetColorValueTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    CSimpleTexture* texture = colorSelect->m_valueTexture;

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

// OFFSET: %0x0054ADD0
static int32_t Script_SetColorValueTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorValueTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorValueTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        colorSelect->SetColorValueTexture(texture);
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = colorSelect->GetName();
        luaL_error(L, "Usage: %s:SetColorValueTexture(texture or nil)", name ? name : "<unnamed>");
        return 0;
    }

    colorSelect->SetColorValueTexture(NULL);
    return 0;
}

// OFFSET: %0x0054AF00
static int32_t Script_GetColorValueThumbTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    CSimpleTexture* texture = colorSelect->m_valueThumbTexture;

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

// OFFSET: %0x0054AF70
static int32_t Script_SetColorValueThumbTexture(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorValueThumbTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = colorSelect->GetName();
            luaL_error(L, "%s:SetColorValueThumbTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        colorSelect->SetColorValueThumbTexture(texture);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        colorSelect->SetColorValueThumbTexture(lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = colorSelect->GetName();
        luaL_error(L, "Usage: %s:SetColorValueThumbTexture(texture or \"texture\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    colorSelect->SetColorValueThumbTexture(static_cast<CSimpleTexture*>(NULL));
    return 0;
}

// OFFSET: %0x0054B0D0
static int32_t Script_SetColorHSV(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    float h = lua_tonumber(L, 2);
    float s = lua_tonumber(L, 3);
    float v = lua_tonumber(L, 4);

    colorSelect->m_hsv.x = h;
    colorSelect->m_hsv.y = s;
    colorSelect->m_hsv.z = v;
    colorSelect->UpdateColor();
    return 0;
}

// OFFSET: %0x0054B150
static int32_t Script_GetColorHSV(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    C3Vector hsv = colorSelect->m_hsv;

    lua_pushnumber(L, hsv.x);
    lua_pushnumber(L, hsv.y);
    lua_pushnumber(L, hsv.z);
    return 3;
}

// OFFSET: %0x0054B1D0
static int32_t Script_SetColorRGB(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    CImVector color;

    FrameScript_GetColor(L, 2, color);
    RGBtoHSV(C3Vector(color), colorSelect->m_hsv);
    colorSelect->UpdateColor();
    return 0;
}

// OFFSET: %0x0054B250
static int32_t Script_GetColorRGB(lua_State* L) {
    CSimpleColorSelect* colorSelect = static_cast<CSimpleColorSelect*>(FrameScript_GetObjectThis(L, CSimpleColorSelect::GetObjectType()));
    C3Vector rgb;
    CImVector color;

    HSVtoRGB(colorSelect->m_hsv, rgb);
    color = rgb;
    lua_pushnumber(L, color.r * (1.0 / 255.0));
    lua_pushnumber(L, color.g * (1.0 / 255.0));
    lua_pushnumber(L, color.b * (1.0 / 255.0));
    return 3;
}

// STATIC: %0x00BB98C0
static FrameScript_Method s_ScriptMethods[] = {
    { "GetColorWheelTexture", &Script_GetColorWheelTexture },
    { "SetColorWheelTexture", &Script_SetColorWheelTexture },
    { "GetColorWheelThumbTexture", &Script_GetColorWheelThumbTexture },
    { "SetColorWheelThumbTexture", &Script_SetColorWheelThumbTexture },
    { "GetColorValueTexture", &Script_GetColorValueTexture },
    { "SetColorValueTexture", &Script_SetColorValueTexture },
    { "GetColorValueThumbTexture", &Script_GetColorValueThumbTexture },
    { "SetColorValueThumbTexture", &Script_SetColorValueThumbTexture },
    { "SetColorHSV", &Script_SetColorHSV },
    { "GetColorHSV", &Script_GetColorHSV },
    { "SetColorRGB", &Script_SetColorRGB },
    { "GetColorRGB", &Script_GetColorRGB },
};

// OFFSET: %0x0054A9A0
void CSimpleColorSelect::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
