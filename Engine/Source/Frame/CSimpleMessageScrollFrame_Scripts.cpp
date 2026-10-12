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

// PROVEN: 4170c703 IDENTICAL/IDENTICAL
// OFFSET: 0x005384F0/0x14018B5E0
void CSimpleMessageScrollFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleMessageScrollFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleMessageScrollFrame::RegisterScriptMethods);
}

// PROVEN: a22bd6bb IDENTICAL/IDENTICAL
// OFFSET: 0x00536EB0/0x140189E20
void CSimpleMessageScrollFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleMessageScrollFrame::s_metatable);
    CSimpleMessageScrollFrame::s_metatable = LUA_NOREF;
}

// PROVEN: b05eda5d IDENTICAL/IDENTICAL
// OFFSET: 0x00537000/0x140189E50
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: ffa2997c IDENTICAL/IDENTICAL
// OFFSET: 0x00537060/0x140189EC0
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5b9cfbad IDENTICAL/IDENTICAL
// OFFSET: 0x005370C0/0x140189F30
static int32_t Script_SetFont(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: e9ed06f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00537120/0x140189FA0
static int32_t Script_GetFont(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// PROVEN: b81e174f IDENTICAL/IDENTICAL
// OFFSET: 0x00537180/0x14018A010
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 5113edf9 IDENTICAL/UNPROVEN
// OFFSET: 0x005371E0/0x14018A080
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: a8ec7652 IDENTICAL/IDENTICAL
// OFFSET: 0x00537240/0x14018A0F0
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: cdad25f2 IDENTICAL/UNPROVEN
// OFFSET: 0x005372A0/0x14018A160
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7b2ff78a IDENTICAL/IDENTICAL
// OFFSET: 0x00537300/0x14018A1D0
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: 9e3f7074 IDENTICAL/IDENTICAL
// OFFSET: 0x00537360/0x14018A240
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// PROVEN: 652f6a8f IDENTICAL/IDENTICAL
// OFFSET: 0x005373C0/0x14018A2B0
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: cd5280c5 IDENTICAL/IDENTICAL
// OFFSET: 0x00537420/0x14018A320
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// PROVEN: 58c1fa8f IDENTICAL/IDENTICAL
// OFFSET: 0x00537480/0x14018A390
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7e945dd5 IDENTICAL/IDENTICAL
// OFFSET: 0x005374E0/0x14018A400
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// PROVEN: 6eaa3428 IDENTICAL/IDENTICAL
// OFFSET: 0x00537540/0x14018A470
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: 7884c3e3 IDENTICAL/IDENTICAL
// OFFSET: 0x005375A0/0x14018A4E0
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// PROVEN: 99e88492 IDENTICAL/IDENTICAL
// OFFSET: 0x00537600/0x14018A550
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// PROVEN: 2b6b4297 IDENTICAL/IDENTICAL
// OFFSET: 0x00537660/0x14018A5C0
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    CSimpleFont* font = frame->m_font;
    const char* name = frame->GetName();
    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005376C0/0x14018A630
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

// OFFSET: 0x005378F0/0x14018A8A0
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

// PROVEN: e07c19a8 DIFFERENT/IDENTICAL
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

// PROVEN: 6f70b0fc DIFFERENT/IDENTICAL
// OFFSET: 0x00537AA0/0x14018AAA0
static int32_t Script_RemoveMessagesByExtraData(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:RemoveMessagesByExtraData(extraData)", name ? name : "<unnamed>");
        return 0;
    }

    int32_t extraData = lua_tointeger(L, 2);
    frame->RemoveLines(NULL, &extraData);
    return 0;
}

// PROVEN: 1c283e2b DIFFERENT/IDENTICAL
// OFFSET: 0x00537B30/0x14018AB50
static int32_t Script_ScrollUp(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollUp();
    return 0;
}

// PROVEN: 8ce16af6 DIFFERENT/IDENTICAL
// OFFSET: 0x00537B70/0x14018AB90
static int32_t Script_ScrollDown(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollDown();
    return 0;
}

// PROVEN: d9372160 DIFFERENT/IDENTICAL
// OFFSET: 0x00537BB0/0x14018ABD0
static int32_t Script_PageUp(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->PageUp();
    return 0;
}

// PROVEN: f7adf94c DIFFERENT/IDENTICAL
// OFFSET: 0x00537BF0/0x14018AC10
static int32_t Script_PageDown(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->PageDown();
    return 0;
}

// PROVEN: f90ff101 DIFFERENT/IDENTICAL
// OFFSET: 0x00537C30/0x14018AC50
static int32_t Script_ScrollToTop(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollToTop();
    return 0;
}

// PROVEN: 87ee5a49 DIFFERENT/IDENTICAL
// OFFSET: 0x00537C70/0x14018AC90
static int32_t Script_ScrollToBottom(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->ScrollToBottom();
    return 0;
}

// PROVEN: 9ff129e2 IDENTICAL/IDENTICAL
// OFFSET: 0x00537CB0/0x14018ACD0
static int32_t Script_SetScrollOffset(lua_State* L) {
    if (lua_isnumber(L, 2)) {
        CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
        frame->SetScrollOffset(static_cast<int32_t>(lua_tonumber(L, 2)));
    }

    return 0;
}

// PROVEN: 00cabdf6 IDENTICAL/IDENTICAL
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

// PROVEN: 3e6b2333 IDENTICAL/IDENTICAL
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

// PROVEN: cce42069 DIFFERENT/IDENTICAL
// OFFSET: 0x00537DD0/0x14018AE00
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

// PROVEN: ab79f5e2 IDENTICAL/IDENTICAL
// OFFSET: 0x00537E80/0x14018AED0
static int32_t Script_GetNumMessages(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    int32_t accessID = lua_tointeger(L, 2);
    lua_pushnumber(L, frame->GetNumMessages(accessID));
    return 1;
}

// PROVEN: aadf7626 DIFFERENT/IDENTICAL
// OFFSET: 0x00537EE0/0x14018AF40
static int32_t Script_GetNumLinesDisplayed(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_numDisplayed);
    return 1;
}

// PROVEN: 34d572cb IDENTICAL/IDENTICAL
// OFFSET: 0x00537F30/0x14018AF90
static int32_t Script_GetCurrentScroll(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->GetCurrentScroll());
    return 1;
}

// PROVEN: 8582ebd7 DIFFERENT/IDENTICAL
// OFFSET: 0x00537F80/0x14018AFE0
static int32_t Script_GetCurrentLine(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_currentLine);
    return 1;
}

// PROVEN: f61c8e2d DIFFERENT/IDENTICAL
// OFFSET: 0x00537FD0/0x14018B030
static int32_t Script_GetMaxLines(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_maxMessages);
    return 1;
}

// PROVEN: 0ff423af IDENTICAL/DIFFERENT
// OFFSET: 0x00538020/0x14018B080
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

// PROVEN: c4e3d673 IDENTICAL/IDENTICAL
// OFFSET: 0x005380B0/0x14018B130
static int32_t Script_SetFading(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->m_fading = StringToBOOL(L, 2, 1);
    return 0;
}

// PROVEN: f1d85cc2 IDENTICAL/IDENTICAL
// OFFSET: 0x00538100/0x14018B190
static int32_t Script_GetFading(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));

    if (frame->m_fading) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00538160/0x14018B1F0
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

// PROVEN: 862216cc DIFFERENT/IDENTICAL
// OFFSET: 0x005381E0/0x14018B290
static int32_t Script_GetTimeVisible(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_timeVisible);
    return 1;
}

// OFFSET: 0x00538230/0x14018B2E0
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

// PROVEN: 1a9cfaf3 DIFFERENT/IDENTICAL
// OFFSET: 0x005382B0/0x14018B380
static int32_t Script_GetFadeDuration(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    lua_pushnumber(L, frame->m_fadeDuration);
    return 1;
}

// PROVEN: 3e3add17 DIFFERENT/IDENTICAL
// OFFSET: 0x00538300/0x14018B3D0
static int32_t Script_Clear(lua_State* L) {
    CSimpleMessageScrollFrame* frame = static_cast<CSimpleMessageScrollFrame*>(FrameScript_GetObjectThis(L, CSimpleMessageScrollFrame::GetObjectType()));
    frame->Clear();
    return 0;
}

// OFFSET: 0x00538340/0x14018B410
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

// OFFSET: 0x00538480/0x14018B560
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

// STATIC: 0x00CA41E0/0x140B328F0
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

// PROVEN: aa9137c4 IDENTICAL/UNPROVEN
// OFFSET: 0x00536E90/0x140189DF0
void CSimpleMessageScrollFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleHyperlinkedFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
