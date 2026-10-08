#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// PROVEN: c0968a67 IDENTICAL/NONE
// OFFSET: 0x00531270
void CSimpleStatusBar::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleStatusBar::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleStatusBar::RegisterScriptMethods);
}

// PROVEN: 80f61908 IDENTICAL/NONE
// OFFSET: 0x005309C0
void CSimpleStatusBar::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleStatusBar::s_metatable);
    CSimpleStatusBar::s_metatable = LUA_NOREF;
}

// OFFSET: 0x005309F0
void CSimpleStatusBar::GetStatusBarColor(CImVector& color) {
    if (this->m_barTexture) {
        this->m_barTexture->GetVertexColor(color);
    } else {
        color.Set(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// PROVEN: df625eff IDENTICAL/NONE
// OFFSET: 0x00530A10/0x140181E30
static int32_t Script_GetOrientation(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    lua_pushstring(L, OrientationToString(static_cast<ORIENTATION>(statusBar->m_orientation)));
    return 1;
}

// OFFSET: 0x00530A60
static int32_t Script_SetOrientation(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    if (lua_isstring(L, 2)) {
        ORIENTATION orientation;

        if (StringToOrientation(lua_tolstring(L, 2, NULL), orientation)) {
            statusBar->SetOrientation(orientation);
            return 0;
        }

        const char* name = statusBar->GetName();
        luaL_error(L, "%s:SetOrientation(): Unknown orientation: %s", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    const char* name = statusBar->GetName();
    luaL_error(L, "Usage: %s:SetOrientation(\"orientation\")", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x00530B30
static int32_t Script_GetMinMaxValues(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    lua_pushnumber(L, statusBar->m_minValue);
    lua_pushnumber(L, statusBar->m_maxValue);
    return 2;
}

// OFFSET: 0x00530B90/0x140182000
static int32_t Script_SetMinMaxValues(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = statusBar->GetName();
        luaL_error(L, "Usage: %s:SetMinMaxValues(min, max)", name ? name : "<unnamed>");
        return 0;
    }

    float minValue = lua_tonumber(L, 2);
    float maxValue = lua_tonumber(L, 3);

    if (minValue < -1.0e12 || minValue > 1.0e12 || maxValue < -1.0e12 || maxValue > 1.0e12) {
        luaL_error(L, "Min or Max out of range");
        return 0;
    }

    if (maxValue - minValue > 1.0e12) {
        luaL_error(L, "Min and Max too far apart");
        return 0;
    }

    statusBar->SetMinMaxValues(minValue, maxValue);
    return 0;
}

// OFFSET: 0x00530CB0
static int32_t Script_GetValue(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    lua_pushnumber(L, statusBar->m_value);
    return 1;
}

// OFFSET: 0x00530D00/0x1401821C0
static int32_t Script_SetValue(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        statusBar->SetValue(lua_tonumber(L, 2));
        return 0;
    }

    const char* name = statusBar->GetName();
    luaL_error(L, "Usage: %s:SetValue(value)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x00530D80/0x140182270
static int32_t Script_GetStatusBarTexture(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));
    CSimpleTexture* texture = statusBar->m_barTexture;

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

// OFFSET: 0x00530DF0/0x140182300
static int32_t Script_SetStatusBarTexture(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));
    uint32_t drawLayer = 2;
    int8_t subLevel = 0;

    if (lua_isstring(L, 3)) {
        StringToDrawLayer(lua_tolstring(L, 3, NULL), drawLayer);
    }

    if (lua_isnumber(L, 4)) {
        subLevel = static_cast<int8_t>(lua_tointeger(L, 4));
    }

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = statusBar->GetName();
            luaL_error(L, "%s:SetCheckedTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = statusBar->GetName();
            luaL_error(L, "%s:SetCheckedTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        statusBar->SetBarTexture(texture, drawLayer, subLevel);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        statusBar->SetBarTexture(lua_tolstring(L, 2, NULL), drawLayer, subLevel);
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = statusBar->GetName();
        luaL_error(L, "Usage: %s:SetStatusBarTexture(texture or \"texture\" or nil [, \"layer\"[, subLayer]])", name ? name : "<unnamed>");
        return 0;
    }

    statusBar->SetBarTexture(static_cast<CSimpleTexture*>(NULL), 2, 0);
    return 0;
}

// OFFSET: 0x00530FC0/0x1401824E0
static int32_t Script_GetStatusBarColor(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));
    CImVector color;

    statusBar->GetStatusBarColor(color);
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);
    return 4;
}

// OFFSET: 0x005310B0/0x140182600
static int32_t Script_SetStatusBarColor(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));
    CImVector color;

    FrameScript_GetColor(L, 2, color);
    statusBar->SetStatusBarColor(color);
    return 0;
}

// PROVEN: a8b4e84f IDENTICAL/NONE
// OFFSET: 0x00531110/0x140182670
static int32_t Script_GetRotatesTexture(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    if (statusBar->m_rotatesTexture) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 38cb25dc IDENTICAL/NONE
// OFFSET: 0x00531170/0x1401826D0
static int32_t Script_SetRotatesTexture(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    statusBar->SetRotatesTexture(StringToBOOL(L, 2, 0));
    return 0;
}

// PROVEN: fa7f0c5a IDENTICAL/NONE
// OFFSET: 0x005311C0/0x140182730
static int32_t Script_GetReverseFill(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    if (statusBar->m_reverseFill) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: cd8c3534 IDENTICAL/NONE
// OFFSET: 0x00531220/0x140182790
static int32_t Script_SetReverseFill(lua_State* L) {
    CSimpleStatusBar* statusBar = static_cast<CSimpleStatusBar*>(FrameScript_GetObjectThis(L, CSimpleStatusBar::GetObjectType()));

    statusBar->SetReverseFill(StringToBOOL(L, 2, 0));
    return 0;
}

// STATIC: %0x00BB8FB8
static FrameScript_Method s_ScriptMethods[] = {
    { "GetOrientation", &Script_GetOrientation },
    { "SetOrientation", &Script_SetOrientation },
    { "GetMinMaxValues", &Script_GetMinMaxValues },
    { "SetMinMaxValues", &Script_SetMinMaxValues },
    { "GetValue", &Script_GetValue },
    { "SetValue", &Script_SetValue },
    { "GetStatusBarTexture", &Script_GetStatusBarTexture },
    { "SetStatusBarTexture", &Script_SetStatusBarTexture },
    { "GetStatusBarColor", &Script_GetStatusBarColor },
    { "SetStatusBarColor", &Script_SetStatusBarColor },
    { "GetRotatesTexture", &Script_GetRotatesTexture },
    { "SetRotatesTexture", &Script_SetRotatesTexture },
    { "GetReverseFill", &Script_GetReverseFill },
    { "SetReverseFill", &Script_SetReverseFill },
};

// OFFSET: 0x005309A0
void CSimpleStatusBar::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
