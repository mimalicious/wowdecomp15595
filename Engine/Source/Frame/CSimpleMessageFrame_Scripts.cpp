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

// OFFSET: %0x005461E0
void CSimpleMessageFrame::CreateScriptMetaTable() {
    CSimpleMessageFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleMessageFrame::RegisterScriptMethods);
}

// OFFSET: %0x00545670
void CSimpleMessageFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleMessageFrame::s_metatable);
    CSimpleMessageFrame::s_metatable = LUA_NOREF;
}

// OFFSET: %0x005456A0
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545700
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545760
static int32_t Script_SetFont(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (!frame || !frame->m_font) {
        return 0;
    }

    const char* name = frame->GetName();
    return CSimpleFont_SetFont(name ? name : "<unnamed>", frame->m_font, L);
}

// OFFSET: %0x005457C0
static int32_t Script_GetFont(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545820
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545880
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x005458E0
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545940
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x005459A0
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545A00
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545A60
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545AC0
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545B20
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545B80
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545BE0
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545C40
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545CA0
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

// OFFSET: %0x00545D50
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

// OFFSET: %0x00545DC0
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545E20
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x00545E80
static int32_t Script_SetFading(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    frame->m_fading = StringToBOOL(L, 2, 1);
    return 0;
}

// OFFSET: %0x00545ED0
static int32_t Script_GetFading(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (frame->m_fading) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x00545F30
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

// OFFSET: %0x00545FB0
static int32_t Script_GetTimeVisible(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_timeVisible);
    return 1;
}

// OFFSET: %0x00546000
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

// OFFSET: %0x00546080
static int32_t Script_GetFadeDuration(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_fadeDuration);
    return 1;
}

// OFFSET: %0x005460D0
static int32_t Script_AddMessage(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));

    if (lua_isstring(L, 2)) {
        const char* text = lua_tolstring(L, 2, NULL);

        if (text && *text) {
            CImVector color;
            color.b = 0xFF;
            color.g = 0xFF;
            color.r = 0xFF;
            color.a = 0xFF;

            if (lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
                FrameScript_GetColor(L, 3, color);
            }

            frame->AddMessage(text, color);
        }
    }

    return 0;
}

// OFFSET: %0x005461A0
static int32_t Script_Clear(lua_State* L) {
    CSimpleMessageFrame* frame = static_cast<CSimpleMessageFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageFrame::GetObjectType()));
    frame->Clear();
    return 0;
}

// STATIC: %0x00BB9408
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

// PROVEN: b6166b97 IDENTICAL/UNPROVEN
// OFFSET: 0x005362E0/0x140189020
void CSimpleMessageFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
