#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x0053A380/0x14018DB80
static HTML_TEXT_TYPE ReadHtmlTextType(lua_State* L) {
    if (lua_type(L, 2) != LUA_TSTRING) {
        return HTML_TEXT_NORMAL;
    }

    const char* style = lua_tolstring(L, 2, NULL);

    if (!SStrCmpI(style, "P", STORM_MAX_STR)) {
        lua_remove(L, 2);
        return HTML_TEXT_NORMAL;
    }

    if (!SStrCmpI(style, "H1", STORM_MAX_STR)) {
        lua_remove(L, 2);
        return HTML_TEXT_HEADER1;
    }

    if (!SStrCmpI(style, "H2", STORM_MAX_STR)) {
        lua_remove(L, 2);
        return HTML_TEXT_HEADER2;
    }

    if (!SStrCmpI(style, "H3", STORM_MAX_STR)) {
        lua_remove(L, 2);
        return HTML_TEXT_HEADER3;
    }

    return HTML_TEXT_NORMAL;
}

// PROVEN: 6e8ba53c IDENTICAL/NONE
// OFFSET: 0x0053A490
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: d579a133 IDENTICAL/NONE
// OFFSET: 0x0053A4F0
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: 65ee27d2 IDENTICAL/NONE
// OFFSET: 0x0053A550
static int32_t Script_SetFont(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: 990b9763 IDENTICAL/NONE
// OFFSET: 0x0053A5B0
static int32_t Script_GetFont(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: ed0ba36e IDENTICAL/NONE
// OFFSET: 0x0053A610
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: c5ebbd44 IDENTICAL/NONE
// OFFSET: 0x0053A670
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5af69eff IDENTICAL/NONE
// OFFSET: 0x0053A6D0
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 43ac330e IDENTICAL/NONE
// OFFSET: 0x0053A730
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: c4b2841c IDENTICAL/NONE
// OFFSET: 0x0053A790
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: d5fb03b6 IDENTICAL/NONE
// OFFSET: 0x0053A7F0
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: c50f99b8 IDENTICAL/NONE
// OFFSET: 0x0053A850
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7a9d3901 IDENTICAL/NONE
// OFFSET: 0x0053A8B0
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: 76a068eb IDENTICAL/NONE
// OFFSET: 0x0053A910
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: e94083cc IDENTICAL/NONE
// OFFSET: 0x0053A970
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5ebe3f26 IDENTICAL/NONE
// OFFSET: 0x0053A9D0
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: c751c502 IDENTICAL/NONE
// OFFSET: 0x0053AA30
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: 9fd2d92c IDENTICAL/NONE
// OFFSET: 0x0053AA90
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// PROVEN: b5f87a5d IDENTICAL/NONE
// OFFSET: 0x0053AAF0/0x14018E550
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));
    CSimpleFont* font = html->m_fonts[ReadHtmlTextType(L)];
    const char* name = html->GetName();
    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x0053AB50/0x14018E5D0
static int32_t Script_SetText(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));

    html->SetText(lua_tolstring(L, 2, NULL), NULL);
    return 0;
}

// OFFSET: 0x0053ABB0/0x14018E650
static int32_t Script_SetHyperlinkFormat(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));

    if (lua_isstring(L, 2)) {
        html->SetHyperlinkFormat(lua_tolstring(L, 2, NULL));
        return 0;
    }

    const char* name = html->GetName();
    luaL_error(L, "Usage: %s:SetHyperlinkFormat(\"format\")", name ? name : "<unnamed>");
    return 0;
}

// PROVEN: 5e4e33fd IDENTICAL/NONE
// OFFSET: 0x0053AC30/0x14018E6F0
static int32_t Script_GetHyperlinkFormat(lua_State* L) {
    CSimpleHTML* html = static_cast<CSimpleHTML*>(FrameScript_GetObjectThis(L, CSimpleHTML::GetObjectType()));

    lua_pushstring(L, html->m_hyperlinkFormat);
    return 1;
}

// PROVEN: be1ff7a8 IDENTICAL/NONE
// OFFSET: 0x0053A460
void CSimpleHTML::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleHTML::s_metatable);
    CSimpleHTML::s_metatable = LUA_NOREF;
}

// PROVEN: c97c98c3 IDENTICAL/NONE
// OFFSET: 0x0053AC70
void CSimpleHTML::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleHTML::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleHTML::RegisterScriptMethods);
}

// STATIC: %0x00BB9800
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
    { "SetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "GetIndentedWordWrap", &Script_GetIndentedWordWrap },
    { "SetText", &Script_SetText },
    { "SetHyperlinkFormat", &Script_SetHyperlinkFormat },
    { "GetHyperlinkFormat", &Script_GetHyperlinkFormat },
};

// OFFSET: 0x0053A440
void CSimpleHTML::RegisterScriptMethods(lua_State* L) {
    CSimpleHyperlinkedFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
