#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x00539C60/0x14018D320
static int32_t Script_GetThumbTexture(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));
    CSimpleTexture* texture = slider->m_thumbTexture;

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

// OFFSET: 0x00539CD0/0x14018D3B0
static int32_t Script_SetThumbTexture(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            const char* name = slider->GetName();
            luaL_error(L, "%s:SetThumbTexture(): Couldn't find 'this' in texture", name ? name : "<unnamed>");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            const char* name = slider->GetName();
            luaL_error(L, "%s:SetThumbTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
            return 0;
        }

        slider->SetThumbTexture(texture, 3);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        slider->SetThumbTexture(lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = slider->GetName();
        luaL_error(L, "Usage: %s:SetThumbTexture(texture or \"texture\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    slider->SetThumbTexture(NULL, 3);
    return 0;
}

// OFFSET: 0x00539E30/0x14018D550
static int32_t Script_GetOrientation(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    lua_pushstring(L, OrientationToString(slider->m_orientation));
    return 1;
}

// OFFSET: 0x00539E80/0x14018D5A0
static int32_t Script_SetOrientation(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    if (lua_isstring(L, 2)) {
        ORIENTATION orientation;

        if (StringToOrientation(lua_tolstring(L, 2, NULL), orientation)) {
            slider->SetOrientation(orientation);
            return 0;
        }

        const char* name = slider->GetName();
        luaL_error(L, "%s:SetOrientation(): Unknown orientation: %s", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    const char* name = slider->GetName();
    luaL_error(L, "Usage: %s:SetOrientation(\"orientation\")", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x00539F50/0x14018D6A0
static int32_t Script_GetMinMaxValues(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    lua_pushnumber(L, slider->m_baseValue);
    lua_pushnumber(L, slider->m_range + slider->m_baseValue);
    return 2;
}

// OFFSET: 0x00539FB0/0x14018D720
static int32_t Script_SetMinMaxValues(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));
    float minValue;
    double maxValue;

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3) || (minValue = static_cast<float>(lua_tonumber(L, 2)), maxValue = lua_tonumber(L, 3), minValue > maxValue)) {
        const char* name = slider->GetName();
        luaL_error(L, "Usage: %s:SetMinMaxValues(min, max)", name ? name : "<unnamed>");
        return 0;
    }

    slider->SetMinMaxValues(minValue, static_cast<float>(maxValue));
    return 0;
}

// OFFSET: 0x0053A060/0x14018D800
static int32_t Script_GetValue(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    lua_pushnumber(L, slider->m_value);
    return 1;
}

// OFFSET: 0x0053A0B0/0x14018D850
static int32_t Script_SetValue(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        slider->SetValue(static_cast<float>(lua_tonumber(L, 2)), false);
        return 0;
    }

    const char* name = slider->GetName();
    luaL_error(L, "Usage: %s:SetValue(value)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x0053A130/0x14018D8F0
static int32_t Script_GetValueStep(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    lua_pushnumber(L, slider->m_valueStep);
    return 1;
}

// OFFSET: 0x0053A180/0x14018D940
static int32_t Script_SetValueStep(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        slider->SetValueStep(static_cast<float>(lua_tonumber(L, 2)));
        return 0;
    }

    const char* name = slider->GetName();
    luaL_error(L, "Usage: %s:SetValueStep(value)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x0053A200/0x14018D9E0
static int32_t Script_IsEnabled(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    if (slider->m_frameFlags & 0x400) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: 0x0053A280/0x14018DA80
static int32_t Script_Enable(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    slider->SetFrameFlag(0x400, 0);

    if (slider->HasScript(slider->m_onEnable) && !slider->m_isLoading) {
        slider->RunScript(slider->m_onEnable, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0053A300/0x14018DB00
static int32_t Script_Disable(lua_State* L) {
    CSimpleSlider* slider = static_cast<CSimpleSlider*>(FrameScript_GetObjectThis(L, CSimpleSlider::GetObjectType()));

    slider->SetFrameFlag(0x400, 1);

    if (slider->HasScript(slider->m_onDisable) && !slider->m_isLoading) {
        slider->RunScript(slider->m_onDisable, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x00539C30/0x14018D2F0
void CSimpleSlider::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleSlider::s_metatable);
    CSimpleSlider::s_metatable = LUA_NOREF;
}

// OFFSET: 0x0053A260/0x14018DA50
void CSimpleSlider::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleSlider::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleSlider::RegisterScriptMethods);
}

// STATIC: 0x00CA4478/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "GetThumbTexture", &Script_GetThumbTexture },
    { "SetThumbTexture", &Script_SetThumbTexture },
    { "GetOrientation", &Script_GetOrientation },
    { "SetOrientation", &Script_SetOrientation },
    { "GetMinMaxValues", &Script_GetMinMaxValues },
    { "SetMinMaxValues", &Script_SetMinMaxValues },
    { "GetValue", &Script_GetValue },
    { "SetValue", &Script_SetValue },
    { "GetValueStep", &Script_GetValueStep },
    { "SetValueStep", &Script_SetValueStep },
    { "Enable", &Script_Enable },
    { "Disable", &Script_Disable },
    { "IsEnabled", &Script_IsEnabled },
};

// OFFSET: 0x00539C10/0x14018D2C0
void CSimpleSlider::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
