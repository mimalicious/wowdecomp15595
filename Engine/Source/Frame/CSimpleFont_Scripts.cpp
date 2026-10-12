#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFont.h"
#include <cmath>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x005337D0/0x140185D70
static int32_t CSimpleFont_CopyFontObject(const char* name, CSimpleFont* font, lua_State* L) {
    CSimpleFont* source;

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        source = static_cast<CSimpleFont*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!source) {
            luaL_error(L, "%s:CopyFontObject(): Couldn't find 'this' in font object", name);
            return 0;
        }

        if (!source->IsA(CSimpleFont::GetObjectType())) {
            luaL_error(L, "%s:CopyFontObject(): Wrong object type, expected font", name);
            return 0;
        }
    } else {
        if (lua_type(L, 2) != LUA_TSTRING) {
            luaL_error(L, "Usage: %s:CopyFontObject(font or \"font\")", name);
            return 0;
        }

        source = CSimpleFont::GetFont(lua_tolstring(L, 2, NULL), 0);

        if (!source) {
            luaL_error(L, "%s:CopyFontObject(): Couldn't find font named %s", name, lua_tolstring(L, 2, NULL));
            return 0;
        }
    }

    *font = *source;

    return 0;
}

// OFFSET: 0x00533A50/0x140186080
static int32_t CSimpleFont_SetAlpha(const char* name, CSimpleFont* font, lua_State* L) {
    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: %s:SetAlpha(alpha)", name);
        return 0;
    }

    uint8_t alpha = static_cast<uint8_t>(lua_tonumber(L, 2) * 255.0);

    if (alpha != font->m_attributes.m_color.a || !(font->m_attributes.m_flags & 0x400)) {
        font->m_attributes.m_flags |= 0x404;
        font->m_attributes.m_color.a = alpha;
    }

    font->m_fontableFlags &= ~0x4u;
    font->UpdateObjects();

    return 0;
}

// INLINED
static int32_t CSimpleFont_GetAlpha(const char* name, CSimpleFont* font, lua_State* L) {
    lua_pushnumber(L, font->m_attributes.m_color.a * 0.00392156f);

    return 1;
}

// OFFSET: 0x00533630/0x140185B90
int32_t CSimpleFont_SetFontObject(const char* name, CSimpleFont* font, lua_State* L) {
    CSimpleFont* parent = NULL;

    if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        parent = static_cast<CSimpleFont*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!parent) {
            luaL_error(L, "%s:SetFontObject(): Couldn't find 'this' in font object", name);
            return 0;
        }

        if (!parent->IsA(CSimpleFont::GetObjectType())) {
            luaL_error(L, "%s:SetFontObject(): Wrong object type, expected font", name);
            return 0;
        }
    } else if (lua_type(L, 2) == LUA_TSTRING) {
        parent = CSimpleFont::GetFont(lua_tolstring(L, 2, NULL), 0);

        if (!parent) {
            luaL_error(L, "%s:SetFontObject(): Couldn't find font named %s", name, lua_tolstring(L, 2, NULL));
            return 0;
        }
    } else if (lua_type(L, 2) != LUA_TNIL) {
        luaL_error(L, "Usage: %s:SetFontObject(font or \"font\" or nil)", name);
        return 0;
    }

    for (CSimpleFont* ancestor = parent; ancestor; ancestor = ancestor->m_fontObject) {
        if (ancestor == font) {
            luaL_error(L, "%s:SetFontObject(): Can't create a font object loop", name);
            return 0;
        }
    }

    font->SetFontObject(parent);

    return 0;
}

// OFFSET: 0x00533780/0x140185D10
int32_t CSimpleFont_GetFontObject(const char* name, CSimpleFont* font, lua_State* L) {
    CSimpleFont* parent = font->m_fontObject;

    if (!parent) {
        lua_pushnil(L);
        return 1;
    }

    if (!parent->m_luaRegistered) {
        parent->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);

    return 1;
}

// OFFSET: 0x005338E0/0x140185EA0
int32_t CSimpleFont_SetFont(const char* name, CSimpleFont* font, lua_State* L) {
    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: %s:SetFont(\"font\", fontHeight [, flags])", name);
        return 0;
    }

    const char* fontName = lua_tolstring(L, 2, NULL);
    float height = static_cast<float>(lua_tonumber(L, 3));
    float fontHeight = NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f));
    uint32_t fontFlags = 0;

    if (lua_isstring(L, 4)) {
        fontFlags = StringToFontFlags(lua_tolstring(L, 4, NULL));
    }

    if (font && font->m_attributes.SetFont(fontName, fontHeight, fontFlags)) {
        font->m_fontableFlags &= ~0x1u;
        font->UpdateObjects();
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x005339F0/0x140186000
int32_t CSimpleFont_GetFont(const char* name, CSimpleFont* font, lua_State* L) {
    lua_pushstring(L, font->m_attributes.m_font.m_str);
    float fontHeight = font->m_attributes.m_fontHeight;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * fontHeight));
    lua_pushstring(L, FontFlagsToString(font->m_attributes.m_fontFlags));

    return 3;
}

// OFFSET: 0x00533AE0/0x140186120
int32_t CSimpleFont_SetTextColor(const char* name, CSimpleFont* font, lua_State* L) {
    CImVector color;
    FrameScript_GetColor(L, 2, color);

    if (*reinterpret_cast<uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&font->m_attributes.m_color) || !(font->m_attributes.m_flags & 0x400)) {
        font->m_attributes.m_color = color;
        font->m_attributes.m_flags |= 0x404;
    }

    font->m_fontableFlags &= ~0x4u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x00533B30/0x140186180
int32_t CSimpleFont_GetTextColor(const char* name, CSimpleFont* font, lua_State* L) {
    CImVector color = font->m_attributes.m_color;
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: 0x00533BD0/0x140186240
int32_t CSimpleFont_SetShadowColor(const char* name, CSimpleFont* font, lua_State* L) {
    CImVector color;
    FrameScript_GetColor(L, 2, color);

    if (*reinterpret_cast<uint32_t*>(&color) != *reinterpret_cast<uint32_t*>(&font->m_attributes.m_shadowColor) || !(font->m_attributes.m_flags & 0x800)) {
        font->m_attributes.m_shadowColor = color;
        font->m_attributes.m_flags |= 0x808;
    }

    font->m_fontableFlags &= ~0x8u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x00533C50/0x1401862C0
int32_t CSimpleFont_GetShadowColor(const char* name, CSimpleFont* font, lua_State* L) {
    CImVector color = font->m_attributes.m_shadowColor;
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: 0x00533CF0/0x140186380
int32_t CSimpleFont_SetShadowOffset(const char* name, CSimpleFont* font, lua_State* L) {
    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: %s:SetShadowOffset(x, y)", name);
        return 0;
    }

    float x = static_cast<float>(lua_tonumber(L, 2));
    float y = static_cast<float>(lua_tonumber(L, 3));

    C2Vector offset;
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    font->m_attributes.SetShadowOffset(offset);

    font->m_fontableFlags &= ~0x8u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x00533DB0/0x1401864C0
int32_t CSimpleFont_GetShadowOffset(const char* name, CSimpleFont* font, lua_State* L) {
    float x = font->m_attributes.m_shadowOffset.x;
    float y = font->m_attributes.m_shadowOffset.y;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y));

    return 2;
}

// OFFSET: 0x00533E20/0x140186550
int32_t CSimpleFont_SetSpacing(const char* name, CSimpleFont* font, lua_State* L) {
    if (lua_isnumber(L, 2)) {
        float value = static_cast<float>(lua_tonumber(L, 2));
        float spacing = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));

        if (!(fabsf(spacing - font->m_attributes.m_spacing) < 0.00000023841858f) || !(font->m_attributes.m_flags & 0x1000)) {
            font->m_attributes.m_flags |= 0x1010;
            font->m_attributes.m_spacing = spacing;
        }

        font->m_fontableFlags &= ~0x10u;
        font->UpdateObjects();
        return 0;
    }

    luaL_error(L, "Usage: %s:SetSpacing(spacing)", name);
    return 0;
}

// OFFSET: 0x00533EC0/0x140186620
int32_t CSimpleFont_GetSpacing(const char* name, CSimpleFont* font, lua_State* L) {
    float spacing = font->m_attributes.m_spacing;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * spacing));

    return 1;
}

// OFFSET: 0x00533F00/0x140186670
int32_t CSimpleFont_GetJustifyH(const char* name, CSimpleFont* font, lua_State* L) {
    lua_pushstring(L, JustifyToString(font->m_attributes.m_styleFlags & 0x7));

    return 1;
}

// OFFSET: 0x00533F30/0x1401866A0
int32_t CSimpleFont_GetJustifyV(const char* name, CSimpleFont* font, lua_State* L) {
    lua_pushstring(L, JustifyToString(font->m_attributes.m_styleFlags & 0x38));

    return 1;
}

// OFFSET: 0x00533F60/0x1401866D0
int32_t CSimpleFont_GetIndentedWordWrap(const char* name, CSimpleFont* font, lua_State* L) {
    if (font->m_attributes.m_styleFlags & 0x20000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x005347B0/0x140187070
int32_t CSimpleFont_SetJustifyH(const char* name, CSimpleFont* font, lua_State* L) {
    uint32_t justify;

    if (!lua_isstring(L, 2) || !StringToJustify(lua_tolstring(L, 2, NULL), justify)) {
        luaL_error(L, "Usage: %s:SetJustifyH(\"justify\")", name);
        return 0;
    }

    uint32_t styleFlags = (font->m_attributes.m_styleFlags & ~0x7u) | (justify & 0x7);

    if (styleFlags != font->m_attributes.m_styleFlags || !(font->m_attributes.m_flags & 0x200)) {
        font->m_attributes.m_flags |= 0x202;
        font->m_attributes.m_styleFlags = styleFlags;
    }

    font->m_fontableFlags &= ~0x2u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x00534840/0x140187120
int32_t CSimpleFont_SetJustifyV(const char* name, CSimpleFont* font, lua_State* L) {
    uint32_t justify;

    if (!lua_isstring(L, 2) || !StringToJustify(lua_tolstring(L, 2, NULL), justify)) {
        luaL_error(L, "Usage: %s:SetJustifyV(\"justify\")", name);
        return 0;
    }

    uint32_t styleFlags = (font->m_attributes.m_styleFlags & ~0x38u) | (justify & 0x38);

    if (styleFlags != font->m_attributes.m_styleFlags || !(font->m_attributes.m_flags & 0x200)) {
        font->m_attributes.m_flags |= 0x202;
        font->m_attributes.m_styleFlags = styleFlags;
    }

    font->m_fontableFlags &= ~0x2u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x005348D0/0x1401871D0
int32_t CSimpleFont_SetIndentedWordWrap(const char* name, CSimpleFont* font, lua_State* L) {
    int32_t indented = StringToBOOL(L, 2, 1);
    uint32_t styleFlags = indented ? font->m_attributes.m_styleFlags | 0x20000 : font->m_attributes.m_styleFlags & ~0x20000u;

    if (styleFlags != font->m_attributes.m_styleFlags || !(font->m_attributes.m_flags & 0x200)) {
        font->m_attributes.m_flags |= 0x202;
        font->m_attributes.m_styleFlags = styleFlags;
    }

    font->m_fontableFlags &= ~0x2u;
    font->UpdateObjects();

    return 0;
}

// OFFSET: 0x00534790/0x140187040
void CSimpleFont::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleFont::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleFont::RegisterScriptMethods);
}

// OFFSET: 0x005335B0/0x140185B60
void CSimpleFont::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleFont::s_metatable);
    CSimpleFont::s_metatable = LUA_NOREF;
}

// OFFSET: 0x00533FA0/0x140186710
static int32_t Script_GetObjectType(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    lua_pushstring(L, font->GetObjectTypeName());

    return 1;
}

// OFFSET: 0x00534000/0x140186760
static int32_t Script_IsObjectType(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = font->GetName();
        luaL_error(L, "Usage: %s:IsObjectType(\"TYPE\")", name ? name : "<unnamed>");
        return 0;
    }

    if (font->IsA(lua_tolstring(L, 2, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x005340B0/0x140186840
static int32_t Script_GetName(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    if (name && *name) {
        lua_pushstring(L, name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00534110/0x1401868B0
static int32_t Script_SetFontObject(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534160/0x140186910
static int32_t Script_GetFontObject(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005341E0/0x1401869A0
static int32_t Script_CopyFontObject(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_CopyFontObject(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534230/0x140186A00
static int32_t Script_SetFont(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetFont(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534280/0x140186A60
static int32_t Script_GetFont(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetFont(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005342D0/0x140186AC0
static int32_t Script_SetAlpha(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetAlpha(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534320/0x140186B20
static int32_t Script_GetAlpha(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetAlpha(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534380/0x140186B90
static int32_t Script_SetTextColor(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534410/0x140186C20
static int32_t Script_GetTextColor(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetTextColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534460/0x140186C80
static int32_t Script_SetShadowColor(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005344B0/0x140186CE0
static int32_t Script_GetShadowColor(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetShadowColor(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534500/0x140186D40
static int32_t Script_SetShadowOffset(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534550/0x140186DA0
static int32_t Script_GetShadowOffset(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetShadowOffset(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005345A0/0x140186E00
static int32_t Script_SetSpacing(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005345F0/0x140186E60
static int32_t Script_GetSpacing(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetSpacing(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534930/0x140187230
static int32_t Script_SetJustifyH(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534660/0x140186EE0
static int32_t Script_GetJustifyH(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetJustifyH(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534980/0x140187290
static int32_t Script_SetJustifyV(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005346C0/0x140186F50
static int32_t Script_GetJustifyV(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetJustifyV(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x005349D0/0x1401872F0
static int32_t Script_SetIndentedWordWrap(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_SetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// OFFSET: 0x00534720/0x140186FC0
static int32_t Script_GetIndentedWordWrap(lua_State* L) {
    CSimpleFont* font = reinterpret_cast<CSimpleFont*>(FrameScript_GetObjectThis(L, CSimpleFont::GetObjectType()));
    const char* name = font->GetName();

    return CSimpleFont_GetIndentedWordWrap(name ? name : "<unnamed>", font, L);
}

// STATIC: 0x00CA3E40/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "GetObjectType", &Script_GetObjectType },
    { "IsObjectType", &Script_IsObjectType },
    { "GetName", &Script_GetName },
    { "SetFontObject", &Script_SetFontObject },
    { "GetFontObject", &Script_GetFontObject },
    { "CopyFontObject", &Script_CopyFontObject },
    { "SetFont", &Script_SetFont },
    { "GetFont", &Script_GetFont },
    { "SetAlpha", &Script_SetAlpha },
    { "GetAlpha", &Script_GetAlpha },
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
};

// OFFSET: 0x00533590/0x140185B40
void CSimpleFont::RegisterScriptMethods(lua_State* L) {
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
