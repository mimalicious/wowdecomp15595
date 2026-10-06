#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x00549210
static int32_t Script_SetScrollChild(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    if (lua_type(L, 2) == LUA_TNIL) {
        scrollFrame->SetScrollChild(NULL);
        return 0;
    }

    CSimpleFrame* child = NULL;

    if (lua_isstring(L, 2)) {
        child = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 2, NULL), CSimpleFrame::GetObjectType()));
    } else if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        child = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!child) {
            const char* name = scrollFrame->GetName();
            luaL_error(L, "%s:SetScrollChild(): Couldn't find 'this' in child object", name ? name : "<unnamed>");
            return 0;
        }

        if (!child->IsA(CSimpleFrame::GetObjectType())) {
            luaL_error(L, "%s:SetScrollChild(): Wrong child object type, expected frame", scrollFrame->GetDisplayName());
            return 0;
        }
    }

    if (!child) {
        const char* name = scrollFrame->GetName();
        luaL_error(L, "%s:SetScrollChild(): Couldn't find frame named '%s'", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    if (!child->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(scrollFrame);
        }

        return 0;
    }

    for (CSimpleFrame* frame = scrollFrame; frame; frame = frame->m_parent) {
        if (frame == child) {
            const char* childName = child->GetName();
            const char* name = scrollFrame->GetName();
            luaL_error(L, "%s:SetScrollChild(): Would create a loop adding child %s", name ? name : "<unnamed>", childName ? childName : "<unnamed>");
            return 0;
        }
    }

    scrollFrame->SetScrollChild(child);
    return 0;
}

// OFFSET: %0x00549410
static int32_t Script_GetScrollChild(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));
    CSimpleFrame* child = scrollFrame->m_scrollChild;

    if (!child) {
        lua_pushnil(L);
        return 1;
    }

    if (!child->m_luaRegistered) {
        child->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, child->m_luaObjectRef);
    return 1;
}

// OFFSET: %0x00549480
static int32_t Script_SetHorizontalScroll(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    if (!scrollFrame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(scrollFrame);
        }

        return 0;
    }

    if (lua_isnumber(L, 2)) {
        float offset = static_cast<float>(lua_tonumber(L, 2));
        scrollFrame->SetHorizontalScroll(NDCToDDCWidth(offset / (CoordinateGetAspectCompensation() * 1024.0f)));
        return 0;
    }

    const char* name = scrollFrame->GetName();
    luaL_error(L, "Usage: %s:SetHorizontalScroll(offset)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x00549550
static int32_t Script_SetVerticalScroll(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    if (!scrollFrame->ProtectedFunctionsAllowed()) {
        if (CSimpleTop::s_instance->m_actionBlockedCallback) {
            CSimpleTop::s_instance->m_actionBlockedCallback(scrollFrame);
        }

        return 0;
    }

    if (lua_isnumber(L, 2)) {
        float offset = static_cast<float>(lua_tonumber(L, 2));
        scrollFrame->SetVerticalScroll(NDCToDDCWidth(offset / (CoordinateGetAspectCompensation() * 1024.0f)));
        return 0;
    }

    const char* name = scrollFrame->GetName();
    luaL_error(L, "Usage: %s:SetVerticalScroll(offset)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x00549620
static int32_t Script_GetHorizontalScroll(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scrollFrame->m_scrollOffset.x));
    return 1;
}

// OFFSET: %0x00549680
static int32_t Script_GetVerticalScroll(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scrollFrame->m_scrollOffset.y));
    return 1;
}

// OFFSET: %0x005496E0
static int32_t Script_GetHorizontalScrollRange(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scrollFrame->m_scrollRange.x));
    return 1;
}

// OFFSET: %0x00549740
static int32_t Script_GetVerticalScrollRange(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * scrollFrame->m_scrollRange.y));
    return 1;
}

// OFFSET: %0x005497A0
static int32_t Script_UpdateScrollChildRect(lua_State* L) {
    CSimpleScrollFrame* scrollFrame = static_cast<CSimpleScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleScrollFrame::GetObjectType()));

    scrollFrame->m_scrollChildUpdate = 1;
    return 0;
}

// OFFSET: %0x005491E0
void CSimpleScrollFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleScrollFrame::s_metatable);
    CSimpleScrollFrame::s_metatable = LUA_NOREF;
}

// OFFSET: %0x005497E0
void CSimpleScrollFrame::CreateScriptMetaTable() {
    CSimpleScrollFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleScrollFrame::RegisterScriptMethods);
}

// STATIC: %0x00BB9740
static FrameScript_Method s_ScriptMethods[] = {
    { "SetScrollChild", &Script_SetScrollChild },
    { "GetScrollChild", &Script_GetScrollChild },
    { "SetHorizontalScroll", &Script_SetHorizontalScroll },
    { "SetVerticalScroll", &Script_SetVerticalScroll },
    { "GetHorizontalScroll", &Script_GetHorizontalScroll },
    { "GetVerticalScroll", &Script_GetVerticalScroll },
    { "GetHorizontalScrollRange", &Script_GetHorizontalScrollRange },
    { "GetVerticalScrollRange", &Script_GetVerticalScrollRange },
    { "UpdateScrollChildRect", &Script_UpdateScrollChildRect },
};

// PROVEN: 23c95563 IDENTICAL/NONE
// OFFSET: 0x005395D0/0x14018CB00
void CSimpleScrollFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
