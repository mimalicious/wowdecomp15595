#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0053E5F0
void CSimpleFontString::CreateScriptMetaTable() {
    CSimpleFontString::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleFontString::RegisterScriptMethods);
}

// OFFSET: %0x0053C190
void CSimpleFontString::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleFontString::s_metatable);
    CSimpleFontString::s_metatable = LUA_NOREF;
}

// OFFSET: %0x0053D160
static int32_t Script_GetObjectType(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    lua_pushstring(L, string->GetObjectTypeName());

    return 1;
}

// OFFSET: %0x0053D1B0
static int32_t Script_IsObjectType(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:IsObjectType(\"TYPE\")", name ? name : "<unnamed>");
        return 0;
    }

    if (string->IsA(lua_tolstring(L, 2, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053D260
static int32_t Script_GetDrawLayer(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    lua_pushstring(L, DrawLayerToString(string->m_drawLayer));

    return 1;
}

// OFFSET: %0x0053D2B0
static int32_t Script_SetDrawLayer(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    uint32_t layer;

    if (!lua_isstring(L, 2) || (layer = string->m_drawLayer, !StringToDrawLayer(lua_tolstring(L, 2, NULL), layer))) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetDrawLayer(\"layer\")", name ? name : "<unnamed>");
        return 0;
    }

    string->SetFrame(string->m_parent, layer, string->m_subLevel, string->m_shown);

    return 0;
}

// OFFSET: %0x0053D380
static int32_t Script_SetVertexColor(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector current;
    string->GetVertexColor(current);

    CImVector color;
    FrameScript_GetColor(L, 2, color);

    if (!lua_isnumber(L, 5)) {
        color.a = current.a;
    }

    string->SetVertexColor(color);
    string->m_fontableFlags &= ~0x4u;

    return 0;
}

// OFFSET: %0x0053D410
static int32_t Script_GetAlpha(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector color;
    string->GetVertexColor(color);
    lua_pushnumber(L, color.a * 0.0039215689f);

    return 1;
}

// OFFSET: %0x0053D480
static int32_t Script_SetAlpha(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetAlpha(alpha)", name ? name : "<unnamed>");
        return 0;
    }

    uint8_t alpha = static_cast<uint8_t>(lua_tonumber(L, 2) * 255.0);

    CImVector color;
    string->GetVertexColor(color);
    color.a = alpha;
    string->SetVertexColor(color);

    return 0;
}

// OFFSET: %0x0053D540
static int32_t Script_SetAlphaGradient(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetAlphaGradient(start, length)", name ? name : "<unnamed>");
        return 0;
    }

    int32_t start = static_cast<int32_t>(lua_tonumber(L, 2));
    int32_t length = static_cast<int32_t>(lua_tonumber(L, 3));

    if (string->SetAlphaGradient(start, length)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053D610
static int32_t Script_Show(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    string->m_shown = 1;
    string->ShowThis(false);

    return 0;
}

// OFFSET: %0x0053D660
static int32_t Script_Hide(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    string->m_shown = 0;
    string->HideThis();

    return 0;
}

// OFFSET: %0x0053D6A0
static int32_t Script_IsVisible(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (string->m_visible) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053D700
static int32_t Script_IsShown(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (string->m_shown) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053D760
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    CSimpleFont* font = string->m_fontObject;

    if (!font) {
        lua_pushnil(L);
        return 1;
    }

    if (!font->m_luaRegistered) {
        font->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, font->m_luaObjectRef);

    return 1;
}

// OFFSET: %0x0053D7D0
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    CSimpleFont* font = NULL;

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        font = static_cast<CSimpleFont*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!font) {
            const char* name = string->GetName();
            luaL_error(L, "%s:SetFontObject(): Couldn't find 'this' in font object", name ? name : "<unnamed>");
            return 0;
        }

        if (!font->IsA(CSimpleFont::GetObjectType())) {
            const char* name = string->GetName();
            luaL_error(L, "%s:SetFontObject(): Wrong object type, expected font", name ? name : "<unnamed>");
            return 0;
        }
    } else if (lua_type(L, 2) == LUA_TSTRING) {
        font = CSimpleFont::GetFont(lua_tolstring(L, 2, NULL), 0);

        if (!font) {
            const char* name = string->GetName();
            luaL_error(L, "%s:SetFontObject(): Couldn't find font named %s", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
            return 0;
        }
    } else if (lua_type(L, 2) != LUA_TNIL) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetFontObject(font or \"font\" or nil)", name ? name : "<unnamed>");
        return 0;
    }

    string->SetFontObject(font);

    return 0;
}

// OFFSET: %0x0053D960
static int32_t Script_GetFont(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    lua_pushstring(L, string->GetFontName());
    float fontHeight = string->GetFontHeight(false);
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * fontHeight));
    lua_pushstring(L, FontFlagsToString(string->GetFontFlags()));

    return 3;
}

// OFFSET: %0x0053D9F0
static int32_t Script_SetFont(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetFont(\"font\", fontHeight [, flags])", name ? name : "<unnamed>");
        return 0;
    }

    const char* fontName = lua_tolstring(L, 2, NULL);
    float height = static_cast<float>(lua_tonumber(L, 3));
    float fontHeight = NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f));

    if (fontHeight <= 0.00000011920929f) {
        const char* name = string->GetName();
        luaL_error(L, "ERROR: %s:SetFont(): invalid fontHeight: %f, height must be > 0", name ? name : "<unnamed>", fontHeight);
        return 0;
    }

    if (!*fontName) {
        return 0;
    }

    uint32_t fontFlags = 0;

    if (lua_isstring(L, 4)) {
        fontFlags = StringToFontFlags(lua_tolstring(L, 4, NULL));
    }

    if (string->SetFont(fontName, fontHeight, fontFlags, false)) {
        string->m_fontableFlags &= ~0x1u;
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053DB70
static int32_t Script_GetText(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    const char* text = string->m_text && *string->m_text ? string->m_text : NULL;
    lua_pushstring(L, text);

    return 1;
}

// OFFSET: %0x0053C140
static int32_t Script_GetFieldSize(lua_State* L) {
    lua_pushnumber(L, 8191.0);

    return 1;
}

// OFFSET: %0x0053DBC0
static int32_t Script_SetText(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!string->m_fontHandle) {
        const char* name = string->GetName();
        luaL_error(L, "%s:SetText(): Font not set", name ? name : "<unnamed>");
    }

    string->SetText(lua_tolstring(L, 2, NULL), 1);

    return 0;
}

// OFFSET: %0x0053DC40
static int32_t Script_SetFormattedText(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!string->m_fontHandle) {
        const char* name = string->GetName();
        luaL_error(L, "%s:SetFormattedText(): Font not set", name ? name : "<unnamed>");
    }

    char buffer[4096];
    string->SetText(FrameScript_Sprintf(L, 2, buffer, sizeof(buffer)), 1);

    return 0;
}

// OFFSET: %0x0053DCD0
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector color;
    string->GetVertexColor(color);
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: %0x0053DDB0
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector color;
    FrameScript_GetColor(L, 2, color);
    string->SetVertexColor(color);
    string->m_fontableFlags &= ~0x4u;

    return 0;
}

// OFFSET: %0x0053DE10
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector color = string->m_shadowColor;
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: %0x0053DED0
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    CImVector color;
    FrameScript_GetColor(L, 2, color);
    string->SetShadowColor(color);
    string->m_fontableFlags &= ~0x8u;

    return 0;
}

// OFFSET: %0x0053DF30
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    C2Vector offset = string->m_shadowOffset;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * offset.x));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * offset.y));

    return 2;
}

// OFFSET: %0x0053DFC0
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetShadowOffset(x, y)", name ? name : "<unnamed>");
        return 0;
    }

    float x = static_cast<float>(lua_tonumber(L, 2));
    float y = static_cast<float>(lua_tonumber(L, 3));

    C2Vector offset;
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    string->SetShadowOffset(offset);
    string->m_fontableFlags &= ~0x8u;

    return 0;
}

// OFFSET: %0x0053E0B0
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    float spacing = string->m_spacing;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * spacing));

    return 1;
}

// OFFSET: %0x0053E110
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        float spacing = static_cast<float>(lua_tonumber(L, 2));
        string->SetSpacing(NDCToDDCWidth(spacing / (CoordinateGetAspectCompensation() * 1024.0f)));
        string->m_fontableFlags &= ~0x10u;
        return 0;
    }

    const char* name = string->GetName();
    luaL_error(L, "Usage: %s:SetSpacing(spacing)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x0053E1B0
static int32_t Script_SetTextHeight(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        float height = static_cast<float>(lua_tonumber(L, 2));

        if (!(height > 0.00000011920929f)) {
            const char* name = string->GetName();
            luaL_error(L, "%s:SetTextHeight(): invalid texHeight: %f, height must be > 0", name ? name : "<unnamed>", height);
            return 0;
        }

        string->SetTextHeight(NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f)));
        return 0;
    }

    const char* name = string->GetName();
    luaL_error(L, "Usage: %s:SetTextHeight(pixelHeight)", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x0053E290
static int32_t Script_GetStringWidth(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    float width = string->GetStringWidth();
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * width));

    return 1;
}

// OFFSET: %0x0053E300
static int32_t Script_GetWrappedWidth(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    float width = string->GetWrappedWidth();
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * width));

    return 1;
}

// OFFSET: %0x0053E370
static int32_t Script_IsTruncated(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    lua_pushboolean(L, string->IsTruncated());

    return 1;
}

// OFFSET: %0x0053E3C0
static int32_t Script_GetStringHeight(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    float height = string->GetStringHeight();
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * height));

    return 1;
}

// OFFSET: %0x0053E430
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    lua_pushstring(L, JustifyToString(string->m_styleFlags & 0x7));

    return 1;
}

// OFFSET: %0x0053E800
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    uint32_t justify;

    if (!lua_isstring(L, 2) || !StringToJustify(lua_tolstring(L, 2, NULL), justify)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetJustifyH(\"justify\")", name ? name : "<unnamed>");
        return 0;
    }

    string->m_settableStyleFlags &= ~0x7u;
    uint32_t styleFlags = (string->m_styleFlags & ~0x7u) | (justify & 0x7);

    if (styleFlags != string->m_styleFlags) {
        string->m_styleFlags = styleFlags;

        if (string->m_string) {
            string->UpdateString();
        }
    }

    string->m_fontableFlags &= ~0x2u;

    return 0;
}

// OFFSET: %0x0053E480
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    lua_pushstring(L, JustifyToString(string->m_styleFlags & 0x38));

    return 1;
}

// OFFSET: %0x0053E8C0
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    uint32_t justify;

    if (!lua_isstring(L, 2) || !StringToJustify(lua_tolstring(L, 2, NULL), justify)) {
        const char* name = string->GetName();
        luaL_error(L, "Usage: %s:SetJustifyV(\"justify\")", name ? name : "<unnamed>");
        return 0;
    }

    string->m_settableStyleFlags &= ~0x38u;
    uint32_t styleFlags = (string->m_styleFlags & ~0x38u) | (justify & 0x38);

    if (styleFlags != string->m_styleFlags) {
        string->m_styleFlags = styleFlags;

        if (string->m_string) {
            string->UpdateString();
        }
    }

    string->m_fontableFlags &= ~0x2u;

    return 0;
}

// OFFSET: %0x0053E4D0
static int32_t Script_CanNonSpaceWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (string->m_styleFlags & 0x1000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053E980
static int32_t Script_SetNonSpaceWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    int32_t wrap = StringToBOOL(L, 2, 1);

    string->m_settableStyleFlags &= ~0x1000u;
    uint32_t styleFlags = wrap ? string->m_styleFlags | 0x1000 : string->m_styleFlags & ~0x1000u;

    if (styleFlags != string->m_styleFlags) {
        string->m_styleFlags = styleFlags;

        if (string->m_string) {
            string->UpdateString();
        }
    }

    return 0;
}

// OFFSET: %0x0053E530
static int32_t Script_CanWordWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (string->m_styleFlags & 0x40) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: %0x0053EA00
static int32_t Script_SetWordWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    int32_t wrap = StringToBOOL(L, 2, 1);

    string->m_settableStyleFlags &= ~0x40u;
    uint32_t styleFlags = wrap ? string->m_styleFlags & ~0x40u : string->m_styleFlags | 0x40;

    if (styleFlags != string->m_styleFlags) {
        string->m_styleFlags = styleFlags;

        if (string->m_string) {
            string->UpdateString();
        }
    }

    return 0;
}

// OFFSET: %0x0053E590
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));

    if (string->m_styleFlags & 0x20000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0053EA80
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleFontString* string = static_cast<CSimpleFontString*>(FrameScript_GetObjectThis(L, CSimpleFontString::GetObjectType()));
    int32_t indented = StringToBOOL(L, 2, 1);

    string->m_settableStyleFlags &= ~0x20000u;
    uint32_t styleFlags = indented ? string->m_styleFlags | 0x20000 : string->m_styleFlags & ~0x20000u;

    if (styleFlags != string->m_styleFlags) {
        string->m_styleFlags = styleFlags;

        if (string->m_string) {
            string->UpdateString();
        }
    }

    return 0;
}

// STATIC: %0x00BB8D90
static FrameScript_Method s_ScriptMethods[] = {
    { "IsObjectType", &Script_IsObjectType },
    { "GetObjectType", &Script_GetObjectType },
    { "GetDrawLayer", &Script_GetDrawLayer },
    { "SetDrawLayer", &Script_SetDrawLayer },
    { "SetVertexColor", &Script_SetVertexColor },
    { "GetAlpha", &Script_GetAlpha },
    { "SetAlpha", &Script_SetAlpha },
    { "SetAlphaGradient", &Script_SetAlphaGradient },
    { "Show", &Script_Show },
    { "Hide", &Script_Hide },
    { "IsVisible", &Script_IsVisible },
    { "IsShown", &Script_IsShown },
    { "GetFontObject", &Script_GetFontObject },
    { "SetFontObject", &Script_SetFontObject },
    { "GetFont", &Script_GetFont },
    { "SetFont", &Script_SetFont },
    { "GetText", &Script_GetText },
    { "GetFieldSize", &Script_GetFieldSize },
    { "SetText", &Script_SetText },
    { "SetFormattedText", &Script_SetFormattedText },
    { "GetTextColor", &Script_GetTextColor },
    { "SetTextColor", &Script_SetTextColor },
    { "GetShadowColor", &Script_GetShadowColor },
    { "SetShadowColor", &Script_SetShadowColor },
    { "GetShadowOffset", &Script_GetShadowOffset },
    { "SetShadowOffset", &Script_SetShadowOffset },
    { "GetSpacing", &Script_GetSpacing },
    { "SetSpacing", &Script_SetSpacing },
    { "SetTextHeight", &Script_SetTextHeight },
    { "GetStringWidth", &Script_GetStringWidth },
    { "GetWrappedWidth", &Script_GetWrappedWidth },
    { "IsTruncated", &Script_IsTruncated },
    { "GetStringHeight", &Script_GetStringHeight },
    { "GetJustifyH", &Script_GetJustifyH },
    { "SetJustifyH", &Script_SetJustifyH },
    { "GetJustifyV", &Script_GetJustifyV },
    { "SetJustifyV", &Script_SetJustifyV },
    { "CanNonSpaceWrap", &Script_CanNonSpaceWrap },
    { "SetNonSpaceWrap", &Script_SetNonSpaceWrap },
    { "CanWordWrap", &Script_CanWordWrap },
    { "SetWordWrap", &Script_SetWordWrap },
    { "GetIndentedWordWrap", &Script_SetIndentedWordWrap },
    { "SetIndentedWordWrap", &Script_GetIndentedWordWrap },
};

// OFFSET: %0x0053C170
void CSimpleFontString::RegisterScriptMethods(lua_State* L) {
    CScriptRegion::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
