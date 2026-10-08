#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// PROVEN: 4170c703 IDENTICAL/NONE
// OFFSET: 0x005384F0
void CSimpleMessageScrollFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleMessageScrollFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleMessageScrollFrame::RegisterScriptMethods);
}

// PROVEN: a22bd6bb IDENTICAL/NONE
// OFFSET: 0x00536EB0
void CSimpleMessageScrollFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleMessageScrollFrame::s_metatable);
    CSimpleMessageScrollFrame::s_metatable = LUA_NOREF;
}

// PROVEN: b05eda5d IDENTICAL/NONE
// OFFSET: 0x00537000
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: ffa2997c IDENTICAL/NONE
// OFFSET: 0x00537060
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5b9cfbad IDENTICAL/NONE
// OFFSET: 0x005370C0
static int32_t Script_SetFont(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: e9ed06f4 IDENTICAL/NONE
// OFFSET: 0x00537120
static int32_t Script_GetFont(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: b81e174f IDENTICAL/NONE
// OFFSET: 0x00537180
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5113edf9 IDENTICAL/NONE
// OFFSET: 0x005371E0
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: a8ec7652 IDENTICAL/NONE
// OFFSET: 0x00537240
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: cdad25f2 IDENTICAL/NONE
// OFFSET: 0x005372A0
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7b2ff78a IDENTICAL/NONE
// OFFSET: 0x00537300
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: 9e3f7074 IDENTICAL/NONE
// OFFSET: 0x00537360
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: 652f6a8f IDENTICAL/NONE
// OFFSET: 0x005373C0
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: cd5280c5 IDENTICAL/NONE
// OFFSET: 0x00537420
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: 58c1fa8f IDENTICAL/NONE
// OFFSET: 0x00537480
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7e945dd5 IDENTICAL/NONE
// OFFSET: 0x005374E0
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: 6eaa3428 IDENTICAL/NONE
// OFFSET: 0x00537540
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7884c3e3 IDENTICAL/NONE
// OFFSET: 0x005375A0/0x14018A4E0
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: 99e88492 IDENTICAL/NONE
// OFFSET: 0x00537600
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// PROVEN: 2b6b4297 IDENTICAL/NONE
// OFFSET: 0x00537660
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: %0x005470F0
static int32_t Script_AddMessage(lua_State* L) {
    int32_t extraData = 0;
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:AddMessage(\"text\", [r, g, b,] typeID, backFill, accessID, extraData)", name ? name : "<unnamed>");
        return 0;
    }

    const char* text = lua_tolstring(L, 2, NULL);

    if (!text || !*text) {
        return 0;
    }

    int32_t typeID = 0;
    int32_t accessID = 0;
    CImVector color;
    int32_t addToTop = 0;

    if (lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
        FrameScript_GetColorNoAlpha(L, 3, color);

        if (lua_isnumber(L, 6)) {
            typeID = static_cast<int32_t>(lua_tonumber(L, 6));
        }

        if (lua_type(L, 7) == LUA_TBOOLEAN) {
            addToTop = lua_toboolean(L, 7);
        }

        if (lua_isnumber(L, 8)) {
            accessID = static_cast<int32_t>(lua_tonumber(L, 8));
        }

        if (lua_isnumber(L, 9)) {
            extraData = static_cast<int32_t>(lua_tonumber(L, 9));
        }
    } else {
        color = frame->m_font->m_attributes.m_color;

        if (lua_isnumber(L, 3)) {
            typeID = static_cast<int32_t>(lua_tonumber(L, 3));
        }

        if (lua_type(L, 4) == LUA_TBOOLEAN) {
            addToTop = lua_toboolean(L, 4);
        }

        if (lua_isnumber(L, 5)) {
            accessID = static_cast<int32_t>(lua_tonumber(L, 5));
        }

        if (lua_isnumber(L, 6)) {
            extraData = static_cast<int32_t>(lua_tonumber(L, 6));
        }
    }

    frame->AddMessage(text, color, typeID, addToTop, accessID, extraData);
    return 0;
}

// OFFSET: %0x00547320
static int32_t Script_GetMessageInfo(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:GetMessageInfo(index[, accessID])", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    int32_t accessID = lua_tointeger(L, 3);
    CSimpleMessageScrollFrameLine* line = frame->GetMessageInfoByAccessID(index, accessID);

    if (!line) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetMessageInfo - A line with accessID %d and index %d does not exist.", name ? name : "<unnamed>", accessID, index);
    }

    lua_pushstring(L, line->m_text);
    lua_pushnumber(L, line->m_accessID);
    lua_pushnumber(L, line->m_typeID);
    lua_pushnumber(L, line->m_extraData);
    return 4;
}

// OFFSET: 0x00537A10/0x14018A9F0
static int32_t Script_RemoveMessagesByAccessID(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:RemoveMessagesByAccessID(accessID)", name ? name : "<unnamed>");
        return 0;
    }

    int32_t accessID = lua_tointeger(L, 2);
    frame->RemoveLines(&accessID, NULL);
    return 0;
}

// OFFSET: 0x00537AA0/0x14018AAA0
static int32_t Script_RemoveMessagesByExtraData(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:RemoveMessagesByExtraData(extraData)", name ? name : "<unnamed>");
    }

    int32_t extraData = lua_tointeger(L, 2);
    frame->RemoveLines(NULL, &extraData);
    return 0;
}

// OFFSET: 0x00537B30/0x14018AB50
static int32_t Script_ScrollUp(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollUp();
    return 0;
}

// OFFSET: 0x00537B70/0x14018AB90
static int32_t Script_ScrollDown(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollDown();
    return 0;
}

// OFFSET: %0x00547540
static int32_t Script_PageUp(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->PageUp();
    return 0;
}

// OFFSET: %0x00547580
static int32_t Script_PageDown(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->PageDown();
    return 0;
}

// OFFSET: 0x00537C30/0x14018AC50
static int32_t Script_ScrollToTop(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollToTop();
    return 0;
}

// OFFSET: 0x00537C70/0x14018AC90
static int32_t Script_ScrollToBottom(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollToBottom();
    return 0;
}

// PROVEN: 9ff129e2 IDENTICAL/UNPROVEN
// OFFSET: 0x00537CB0/0x14018ACD0
static int32_t Script_SetScrollOffset(lua_State* L) {
    if (lua_isnumber(L, 2)) {
        CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
        frame->SetScrollOffset(static_cast<int32_t>(lua_tonumber(L, 2)));
    }

    return 0;
}

// PROVEN: 00cabdf6 IDENTICAL/NONE
// OFFSET: 0x00537D10/0x14018AD40
static int32_t Script_AtTop(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (frame->m_atTop) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 3e6b2333 IDENTICAL/NONE
// OFFSET: 0x00537D70/0x14018ADA0
static int32_t Script_AtBottom(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (frame->m_atBottom) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x00547760
static int32_t Script_UpdateColorByID(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
        int32_t typeID = static_cast<int32_t>(lua_tonumber(L, 2));
        CImVector color;
        FrameScript_GetColorNoAlpha(L, 3, color);
        frame->UpdateColorByID(typeID, color);
    }

    return 0;
}

// OFFSET: %0x00547810
static int32_t Script_GetNumMessages(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    int32_t accessID = lua_tointeger(L, 2);
    lua_pushnumber(L, frame->GetNumMessages(accessID));
    return 1;
}

// OFFSET: %0x00547870
static int32_t Script_GetNumLinesDisplayed(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_numDisplayed);
    return 1;
}

// PROVEN: 34d572cb IDENTICAL/UNPROVEN
// OFFSET: 0x00537F30/0x14018AF90
static int32_t Script_GetCurrentScroll(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->GetCurrentScroll());
    return 1;
}

// OFFSET: %0x00547910
static int32_t Script_GetCurrentLine(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_currentLine);
    return 1;
}

// OFFSET: %0x00547960
static int32_t Script_GetMaxLines(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_maxMessages);
    return 1;
}

// OFFSET: %0x005479B0
static int32_t Script_SetMaxLines(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        int32_t maxLines = static_cast<int32_t>(lua_tonumber(L, 2));

        if (maxLines > 0) {
            frame->SetMaxLines(maxLines);
            return 0;
        }
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetMaxLines(maxLines)", name ? name : "<unnamed>");
    return 0;
}

// PROVEN: c4e3d673 IDENTICAL/NONE
// OFFSET: 0x005380B0/0x14018B130
static int32_t Script_SetFading(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->m_fading = StringToBOOL(L, 2, 1);
    return 0;
}

// PROVEN: f1d85cc2 IDENTICAL/NONE
// OFFSET: 0x00538100
static int32_t Script_GetFading(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (frame->m_fading) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00538160
static int32_t Script_SetTimeVisible(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        frame->SetTimeVisible(static_cast<float>(lua_tonumber(L, 2)));
        return 0;
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetTimeVisible(seconds)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x00547B70
static int32_t Script_GetTimeVisible(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_timeVisible);
    return 1;
}

// OFFSET: 0x00538230
static int32_t Script_SetFadeDuration(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        frame->SetFadeDuration(static_cast<float>(lua_tonumber(L, 2)));
        return 0;
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetFadeDuration(seconds)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x00547C40
static int32_t Script_GetFadeDuration(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_fadeDuration);
    return 1;
}

// OFFSET: %0x00547C90
static int32_t Script_Clear(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->Clear();
    return 0;
}

// OFFSET: %0x00547CD0
static int32_t Script_SetInsertMode(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:SetInsertMode(\"TOP\" or \"BOTTOM\")", name ? name : "<unnamed>");
        return 0;
    }

    const char* mode = lua_tolstring(L, 2, NULL);
    char lower[128];
    SStrCopy(lower, mode, sizeof(lower));
    SStrLower(lower);

    if (!SStrCmp(mode, "top", STORM_MAX_STR)) {
        frame->SetInsertMode(0);
        return 0;
    }

    if (!SStrCmp(mode, "bottom", STORM_MAX_STR)) {
        frame->SetInsertMode(1);
        return 0;
    }

    const char* name = frame->GetName();
    luaL_error(L, "Usage: %s:SetInsertMode(\"TOP\" or \"BOTTOM\")", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x00547E10
static int32_t Script_GetInsertMode(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
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

// STATIC: %0x00BB94F0
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
    { "AddMessage", &Script_AddMessage },
    { "GetMessageInfo", &Script_GetMessageInfo },
    { "RemoveMessagesByAccessID", &Script_RemoveMessagesByAccessID },
    { "RemoveMessagesByExtraData", &Script_RemoveMessagesByExtraData },
    { "ScrollUp", &Script_ScrollUp },
    { "ScrollDown", &Script_ScrollDown },
    { "PageUp", &Script_PageUp },
    { "PageDown", &Script_PageDown },
    { "ScrollToTop", &Script_ScrollToTop },
    { "ScrollToBottom", &Script_ScrollToBottom },
    { "SetScrollOffset", &Script_SetScrollOffset },
    { "AtTop", &Script_AtTop },
    { "AtBottom", &Script_AtBottom },
    { "UpdateColorByID", &Script_UpdateColorByID },
    { "GetNumMessages", &Script_GetNumMessages },
    { "GetNumLinesDisplayed", &Script_GetNumLinesDisplayed },
    { "GetCurrentScroll", &Script_GetCurrentScroll },
    { "GetCurrentLine", &Script_GetCurrentLine },
    { "GetMaxLines", &Script_GetMaxLines },
    { "SetMaxLines", &Script_SetMaxLines },
    { "SetFading", &Script_SetFading },
    { "GetFading", &Script_GetFading },
    { "SetTimeVisible", &Script_SetTimeVisible },
    { "GetTimeVisible", &Script_GetTimeVisible },
    { "SetFadeDuration", &Script_SetFadeDuration },
    { "GetFadeDuration", &Script_GetFadeDuration },
    { "Clear", &Script_Clear },
    { "SetInsertMode", &Script_SetInsertMode },
    { "GetInsertMode", &Script_GetInsertMode },
    { "SetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "GetIndentedWordWrap", &Script_GetIndentedWordWrap },
};

// OFFSET: %0x005469D0
void CSimpleMessageScrollFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleHyperlinkedFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
