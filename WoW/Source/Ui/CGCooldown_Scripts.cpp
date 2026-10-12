#include "CGCooldown.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x009FF650/0x14079DC90
static int32_t Script_SetReverse(lua_State* L) {
    CGCooldown* cooldown = static_cast<CGCooldown*>(FrameScript_GetObjectThis(L, CGCooldown::GetObjectType()));
    cooldown->m_reverse = StringToBOOL(L, 2, 1);
    return 0;
}

// OFFSET: 0x009FF6A0/0x14079DCF0
static int32_t Script_GetReverse(lua_State* L) {
    CGCooldown* cooldown = static_cast<CGCooldown*>(FrameScript_GetObjectThis(L, CGCooldown::GetObjectType()));
    lua_pushboolean(L, cooldown->m_reverse);
    return 1;
}

// OFFSET: 0x009FF6E0/0x14079DD40
static int32_t Script_SetDrawEdge(lua_State* L) {
    CGCooldown* cooldown = static_cast<CGCooldown*>(FrameScript_GetObjectThis(L, CGCooldown::GetObjectType()));
    cooldown->m_drawEdge = StringToBOOL(L, 2, 1);
    return 0;
}

// OFFSET: 0x009FF730/0x14079DDA0
static int32_t Script_GetDrawEdge(lua_State* L) {
    CGCooldown* cooldown = static_cast<CGCooldown*>(FrameScript_GetObjectThis(L, CGCooldown::GetObjectType()));
    lua_pushboolean(L, cooldown->m_drawEdge);
    return 1;
}

// OFFSET: 0x009FFBD0/0x14079E1F0
static int32_t Script_SetCooldown(lua_State* L) {
    CGCooldown* cooldown = static_cast<CGCooldown*>(FrameScript_GetObjectThis(L, CGCooldown::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = cooldown->GetName();
        luaL_error(L, "Usage: %s:SetCooldown(start, duration)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t start = static_cast<uint32_t>(lua_tonumber(L, 2) * 1000.0);
    uint32_t duration = static_cast<uint32_t>(lua_tonumber(L, 3) * 1000.0);
    cooldown->SetCooldown(start, duration);
    return 0;
}

// OFFSET: 0x009FE940/0x14079CE70
void CGCooldown::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGCooldown::s_metatable);
    CGCooldown::s_metatable = LUA_NOREF;
}

// OFFSET: 0x009FF770/0x14079DDF0
void CGCooldown::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGCooldown::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGCooldown::RegisterScriptMethods);
}

// STATIC: 0x00D17524/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "SetCooldown", &Script_SetCooldown },
    { "SetReverse", &Script_SetReverse },
    { "GetReverse", &Script_GetReverse },
    { "SetDrawEdge", &Script_SetDrawEdge },
    { "GetDrawEdge", &Script_GetDrawEdge },
};

// OFFSET: 0x009FE920/0x14079CE40
void CGCooldown::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
