#include "CGQuestPOIFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/QuestLog.h"

// OFFSET: 0x00A02940/0x1407A19B0
static int32_t Script_UpdateMouseOverTooltip(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    float x = static_cast<float>(lua_tonumber(L, 2));
    float y = static_cast<float>(lua_tonumber(L, 3));
    frame->UpdateMouseOverPOI(x, y);

    if (!frame->m_mouseOverQuestId) {
        return 0;
    }

    lua_pushinteger(L, CGQuestLog::GetQuestIndex(frame->m_mouseOverQuestId));
    lua_pushinteger(L, frame->m_mouseOverCount);
    return 2;
}

// OFFSET: 0x00A029D0/0x1407A1A70
static int32_t Script_GetTooltipIndex(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    uint32_t index = lua_tointeger(L, 2) - 1;

    if (index > 3) {
        lua_pushinteger(L, 0);
    } else {
        lua_pushinteger(L, frame->m_mouseOverObjectives[index]);
    }

    return 1;
}

// PROVEN: d85cd4f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02A40/0x1407A1AE0
static int32_t Script_GetNumTooltips(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    lua_pushinteger(L, frame->m_mouseOverCount);
    return 1;
}

// STATIC: 0x00D17664/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "UpdateMouseOverTooltip", &Script_UpdateMouseOverTooltip },
    { "GetTooltipIndex", &Script_GetTooltipIndex },
    { "GetNumTooltips", &Script_GetNumTooltips },
};

// PROVEN: 27f49d90 IDENTICAL/UNPROVEN
// OFFSET: 0x00A022F0/0x1407A1220
void CGQuestPOIFrame::RegisterScriptMethods(lua_State* L) {
    CGBlobFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// PROVEN: 4a834d12 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02310/0x1407A1250
void CGQuestPOIFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGQuestPOIFrame::s_metatable);
    CGQuestPOIFrame::s_metatable = LUA_NOREF;
}

// PROVEN: a802a15f IDENTICAL/IDENTICAL
// OFFSET: 0x00A02A80/0x1407A1B30
void CGQuestPOIFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGQuestPOIFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGQuestPOIFrame::RegisterScriptMethods);
}
