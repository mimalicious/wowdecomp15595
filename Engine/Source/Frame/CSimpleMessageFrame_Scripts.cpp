#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageFrame.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x00536E70/0x140189DC0
void CSimpleMessageFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleMessageFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleMessageFrame::RegisterScriptMethods);
}

// OFFSET: 0x00536300/0x140189050
void CSimpleMessageFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleMessageFrame::s_metatable);
    CSimpleMessageFrame::s_metatable = LUA_NOREF;
}

// OFFSET: 0x00536330/0x140189080
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536390/0x1401890F0
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005363F0/0x140189160
static int32_t Script_SetFont(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (!frame || !frame->m_font) {
        return 0;
    }

    const char* name = frame->GetName();
    return CSimpleFont_SetFont(name ? name : "<unnamed>", frame->m_font, L);
}

// OFFSET: 0x00536450/0x1401891E0
static int32_t Script_GetFont(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005364B0/0x140189250
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536510/0x1401892C0
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536570/0x140189330
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005365D0/0x1401893A0
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536630/0x140189410
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536690/0x140189480
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005366F0/0x1401894F0
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536750/0x140189560
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005367B0/0x1401895D0
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536810/0x140189640
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536870/0x1401896B0
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005368D0/0x140189720
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536930/0x140189790
static int32_t Script_SetInsertMode(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (lua_isstring(L, 2)) {
        const char* mode = lua_tolstring(L, 2, NULL);

        if (!SStrCmpI(mode, "TOP", STORM_MAX_STR)) {
            frame->SetInsertMode(0);
            return 0;
        }

        if (!SStrCmpI(mode, "BOTTOM", STORM_MAX_STR)) {
            frame->SetInsertMode(1);
            return 0;
        }
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetInsertMode(\"TOP\" or \"BOTTOM\")", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x005369E0/0x140189890
static int32_t Script_GetInsertMode(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    const char* mode = NULL;

    switch (frame->m_insertMode) {
        case 0:
            mode = "TOP";
            break;
        case 1:
            lua_pushstring(L, "BOTTOM");
            return 1;
    }

    lua_pushstring(L, mode);
    return 1;
}

// OFFSET: 0x00536A50/0x140189910
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536AB0/0x140189980
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00536B10/0x1401899F0
static int32_t Script_SetFading(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    frame->m_fading = StringToBOOL(L, 2, 1);
    return 0;
}

// OFFSET: 0x00536B60/0x140189A50
static int32_t Script_GetFading(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (frame->m_fading) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00536BC0/0x140189AB0
static int32_t Script_SetTimeVisible(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        frame->SetTimeVisible(static_cast<float>(lua_tonumber(L, 2)));
        return 0;
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetTimeVisible(seconds)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x00536C40/0x140189B50
static int32_t Script_GetTimeVisible(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_timeVisible);
    return 1;
}

// OFFSET: 0x00536C90/0x140189BA0
static int32_t Script_SetFadeDuration(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        frame->SetFadeDuration(static_cast<float>(lua_tonumber(L, 2)));
        return 0;
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetFadeDuration(seconds)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x00536D10/0x140189C40
static int32_t Script_GetFadeDuration(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_fadeDuration);
    return 1;
}

// OFFSET: 0x00536D60/0x140189C90
static int32_t Script_AddMessage(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (lua_isstring(L, 2)) {
        const char* text = lua_tolstring(L, 2, NULL);

        if (text && *text) {
            CImVector color(0xFF, 0xFF, 0xFF, 0xFF);

            if (lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
                FrameScript_GetColor(L, 3, color);
            }

            frame->AddMessage(text, color);
        }
    }

    return 0;
}

// OFFSET: 0x00536E30/0x140189D80
static int32_t Script_Clear(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    frame->Clear();
    return 0;
}

// STATIC: 0x00CA40F8/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "SetFontObject", &Script_SetFontObject },
    { "GetFontObject", &Script_GetFontObject },
    { "SetFont", &Script_SetFont },
    { "GetFont", &Script_GetFont },
    { "SetTextColor", &Script_SetTextColor },
    { "GetTextColor", &Script_GetTextColor },
    { "SetShadowColor", &Script_SetShadowColor },
    { "GetShadowColor", &Script_GetShadowColor },
    { "SetShadowOffset", &Script_SetShadowOffset },
    { "GetShadowOffset", &Script_GetShadowOffset },
    { "SetSpacing", &Script_SetSpacing },
    { "GetSpacing", &Script_GetSpacing },
    { "SetJustifyH", &Script_SetJustifyH },
    { "GetJustifyH", &Script_GetJustifyH },
    { "SetJustifyV", &Script_SetJustifyV },
    { "GetJustifyV", &Script_GetJustifyV },
    { "SetInsertMode", &Script_SetInsertMode },
    { "GetInsertMode", &Script_GetInsertMode },
    { "SetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "GetIndentedWordWrap", &Script_GetIndentedWordWrap },
    { "SetFading", &Script_SetFading },
    { "GetFading", &Script_GetFading },
    { "SetTimeVisible", &Script_SetTimeVisible },
    { "GetTimeVisible", &Script_GetTimeVisible },
    { "SetFadeDuration", &Script_SetFadeDuration },
    { "GetFadeDuration", &Script_GetFadeDuration },
    { "AddMessage", &Script_AddMessage },
    { "Clear", &Script_Clear },
};

// OFFSET: 0x005362E0/0x140189020
void CSimpleMessageFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
