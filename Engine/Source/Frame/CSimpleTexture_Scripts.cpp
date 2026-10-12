#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA3A00/0x140B40DC0
int32_t CSimpleTexture::s_metatable = LUA_NOREF;

// OFFSET: 0x0052C070/0x14017C7F0
static int32_t Script_GetObjectType(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    lua_pushstring(L, texture->GetObjectTypeName());
    return 1;
}

// OFFSET: 0x0052C0C0/0x14017C840
static int32_t Script_IsObjectType(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:IsObjectType(\"TYPE\")", name ? name : "<unnamed>");
        return 0;
    }

    if (texture->IsA(lua_tolstring(L, 2, NULL))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052C170/0x14017C920
static int32_t Script_GetDrawLayer(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    lua_pushstring(L, DrawLayerToString(texture->m_drawLayer));
    return 1;
}

// OFFSET: 0x0052C1E0/0x14017C9A0
static int32_t Script_SetDrawLayer(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    uint32_t layer;

    if (!lua_isstring(L, 2) || (layer = texture->m_drawLayer, !StringToDrawLayer(lua_tolstring(L, 2, NULL), layer))) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetDrawLayer(\"layer\", [sublevel])", name ? name : "<unnamed>");
        return 0;
    }

    int8_t subLevel = 0;

    if (lua_isnumber(L, 3)) {
        int32_t level = lua_tointeger(L, 3);
        subLevel = static_cast<int8_t>(level);

        if (level > 7 || level < -8) {
            luaL_error(L, "SetDrawLayer: sublevel must be between %d and %d!", -8, 7);
            return 0;
        }
    }

    texture->SetFrame(texture->m_parent, layer, subLevel, texture->m_shown);
    return 0;
}

// OFFSET: 0x0052C2E0/0x14017CAE0
static int32_t Script_GetBlendMode(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    lua_pushstring(L, BlendModeToString(texture->m_alphaMode));
    return 1;
}

// OFFSET: 0x0052C330/0x14017CB30
static int32_t Script_SetBlendMode(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (lua_isstring(L, 2)) {
        EGxBlend blend = texture->m_alphaMode;

        if (StringToBlendMode(lua_tolstring(L, 2, NULL), blend)) {
            texture->SetBlendMode(blend);
            return 0;
        }
    }

    const char* name = texture->GetName();
    luaL_error(L, "Usage: %s:SetBlendMode(\"mode\")", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: 0x0052C3D0/0x14017CBF0
static int32_t Script_GetVertexColor(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    CImVector color;

    texture->GetVertexColor(color);
    lua_pushnumber(L, color.r * 0.00392156f);
    lua_pushnumber(L, color.g * 0.00392156f);
    lua_pushnumber(L, color.b * 0.00392156f);
    lua_pushnumber(L, color.a * 0.00392156f);

    return 4;
}

// OFFSET: 0x0052C4B0/0x14017CD00
static int32_t Script_SetVertexColor(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    CImVector current;
    texture->GetVertexColor(current);

    CImVector color;
    FrameScript_GetColor(L, 2, color);

    if (!lua_isnumber(L, 5)) {
        color.a = current.a;
    }

    texture->SetVertexColor(color);
    return 0;
}

// OFFSET: 0x0052C530/0x14017CDA0
static int32_t Script_SetGradient(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    ORIENTATION orientation;

    if (!lua_isstring(L, 2) || !StringToOrientation(lua_tolstring(L, 2, NULL), orientation)) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetGradient(\"orientation\", minR, minG, minB, maxR, maxG, maxB)", name ? name : "<unnamed>");
        return 0;
    }

    CImVector minColor;
    CImVector maxColor;
    FrameScript_GetColorNoAlpha(L, 3, minColor);
    FrameScript_GetColorNoAlpha(L, 6, maxColor);
    texture->SetVertexGradient(orientation, minColor, maxColor);

    return 0;
}

// OFFSET: 0x0052C600/0x14017CE90
static int32_t Script_SetGradientAlpha(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    ORIENTATION orientation;

    if (!lua_isstring(L, 2) || !StringToOrientation(lua_tolstring(L, 2, NULL), orientation)) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetGradientAlpha(\"orientation\", minR, minG, minB, minA, maxR, maxG, maxB, maxA)", name ? name : "<unnamed>");
        return 0;
    }

    CImVector minColor;
    CImVector maxColor;
    FrameScript_GetColor(L, 3, minColor);
    FrameScript_GetColor(L, 7, maxColor);
    texture->SetVertexGradient(orientation, minColor, maxColor);

    return 0;
}

// OFFSET: 0x0052C6D0/0x14017CF80
static int32_t Script_SetAlpha(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetAlpha(alpha)", name ? name : "<unnamed>");
        return 0;
    }

    float alpha = lua_tonumber(L, 2);
    alpha = alpha < 0.0f ? 0.0f : (alpha >= 1.0f ? 1.0f : alpha);

    CImVector color;
    texture->GetVertexColor(color);
    color.a = static_cast<uint8_t>(alpha * 255.0f);
    texture->SetVertexColor(color);

    return 0;
}

// OFFSET: 0x0052C7B0/0x14017D070
static int32_t Script_GetAlpha(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    CImVector color;

    texture->GetVertexColor(color);
    lua_pushnumber(L, color.a * 0.0039215689f);

    return 1;
}

// OFFSET: 0x0052C820/0x14017D0E0
static int32_t Script_Show(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    texture->m_shown = 1;
    texture->ShowThis(false);

    return 0;
}

// OFFSET: 0x0052C870/0x14017D130
static int32_t Script_Hide(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    texture->m_shown = 0;
    texture->HideThis();

    return 0;
}

// OFFSET: 0x0052C8B0/0x14017D180
static int32_t Script_IsVisible(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture->m_visible) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052C910/0x14017D1F0
static int32_t Script_IsShown(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture->m_shown) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052C970/0x14017D260
static int32_t Script_GetTexture(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    char buffer[260];

    if (texture->GetTexture(buffer, sizeof(buffer))) {
        lua_pushstring(L, buffer);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052C9F0/0x14017D2E0
static int32_t Script_SetTexture(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        CImVector color;
        FrameScript_GetColor(L, 2, color);
        texture->SetTexture(color);
    } else if (lua_isstring(L, 2)) {
        int32_t wrapV = lua_toboolean(L, 3);
        int32_t wrapU = lua_toboolean(L, 3);

        if (!texture->SetTexture(lua_tolstring(L, 2, NULL), wrapU, wrapV, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        texture->SetTexture(static_cast<const char*>(NULL), 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x0052CAE0/0x14017D430
static int32_t Script_GetTexCoord(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    for (uint32_t i = 0; i < 4; i++) {
        lua_pushnumber(L, texture->m_texCoords[i].x);
        lua_pushnumber(L, texture->m_texCoords[i].y);
    }

    return 8;
}

// OFFSET: 0x0052CB50/0x14017D4C0
static int32_t Script_SetRotation(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetRotation(angle, [cx, cy])", name ? name : "<unnamed>");
    }

    float angle = static_cast<float>(lua_tonumber(L, 2) - 0.78539819f);
    float centerX = 0.5f;
    float centerY = 0.5f;

    if (lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
        centerX = lua_tonumber(L, 3);
        centerY = lua_tonumber(L, 4);
    }

    float s = sin(angle);
    float c = cos(angle);

    C2Vector texCoords[4];
    texCoords[0].x = s + centerX;
    texCoords[0].y = centerY - c;
    texCoords[1].x = centerX - c;
    texCoords[1].y = centerY - s;
    texCoords[2].x = c + centerX;
    texCoords[2].y = s + centerY;
    texCoords[3].x = centerX - s;
    texCoords[3].y = centerY + c;
    texture->SetTexCoord(texCoords);

    return 0;
}

// OFFSET: 0x0052CCE0/0x14017D6B0
static int32_t Script_SetDesaturated(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    texture->SetDesaturated(StringToBOOL(L, 2, 1) != 0);
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x0052CD40/0x14017D730
static int32_t Script_IsDesaturated(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture && (texture->m_textureFlags & 0x600) == 0x200) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052CDB0/0x14017D7B0
static int32_t Script_SetNonBlocking(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    uint32_t nonBlocking = StringToBOOL(L, 2, 1) != 0;

    texture->m_textureFlags ^= (texture->m_textureFlags ^ nonBlocking) & 0x1;
    return 0;
}

// OFFSET: 0x0052CE10/0x14017D820
static int32_t Script_GetNonBlocking(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture->m_textureFlags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052CE70/0x14017D880
static int32_t Script_SetHorizTile(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    texture->m_textureFlags ^= (texture->m_textureFlags ^ ((StringToBOOL(L, 2, 1) != 0) << 2)) & 0x4;
    return 0;
}

// OFFSET: 0x0052CED0/0x14017D8F0
static int32_t Script_GetHorizTile(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture->m_textureFlags & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052CF30/0x14017D960
static int32_t Script_SetVertTile(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    texture->m_textureFlags ^= (texture->m_textureFlags ^ ((StringToBOOL(L, 2, 1) != 0) << 3)) & 0x8;
    return 0;
}

// OFFSET: 0x0052CF90/0x14017D9D0
static int32_t Script_GetVertTile(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));

    if (texture->m_textureFlags & 0x8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0052E5E0/0x14017F410
static int32_t Script_SetTexCoord(lua_State* L) {
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(FrameScript_GetObjectThis(L, CSimpleTexture::GetObjectType()));
    int32_t count = lua_gettop(L) - 1;

    if (count == 4) {
        CRect texRect(0.0f, 0.0f, 0.0f, 0.0f);
        texRect.minX = lua_tonumber(L, 2);
        texRect.maxX = lua_tonumber(L, 3);
        texRect.minY = lua_tonumber(L, 4);
        texRect.maxY = lua_tonumber(L, 5);

        if (texRect.minX < -10000.0f || texRect.minX > 10000.0f || texRect.maxX < -10000.0f || texRect.maxX > 10000.0f || texRect.minY < -10000.0f || texRect.minY > 10000.0f || texRect.maxY < -10000.0f || texRect.maxY > 10000.0f) {
            luaL_error(L, "TexCoord out of range");
            return 0;
        }

        texture->SetTexCoord(texRect);
        return 0;
    }

    if (count != 8) {
        const char* name = texture->GetName();
        luaL_error(L, "Usage: %s:SetTexCoord(minX, maxX, minY, maxY) or SetTexCoord(ULx, ULy, LLx, LLy, URx, URy, LRx, LRy)", name ? name : "<unnamed>");
        return 0;
    }

    C2Vector texCoords[4];
    int32_t index = 2;

    for (uint32_t i = 0; i < 4; i++) {
        texCoords[i].x = lua_tonumber(L, index++);

        if (texCoords[i].x < -10000.0f || texCoords[i].x > 10000.0f) {
            luaL_error(L, "TexCoord out of range");
            return 0;
        }

        texCoords[i].y = lua_tonumber(L, index++);

        if (texCoords[i].y < -10000.0f || texCoords[i].y > 10000.0f) {
            luaL_error(L, "TexCoord out of range");
            return 0;
        }
    }

    texture->SetTexCoord(texCoords);
    return 0;
}

// OFFSET: 0x0052CFF0/0x14017DA40
void CSimpleTexture::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleTexture::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleTexture::RegisterScriptMethods);
}

// OFFSET: 0x0052BE30/0x14017C740
void CSimpleTexture::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleTexture::s_metatable);
    CSimpleTexture::s_metatable = LUA_NOREF;
}

// STATIC: 0x00CA3918/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "IsObjectType", &Script_IsObjectType },
    { "GetObjectType", &Script_GetObjectType },
    { "GetDrawLayer", &Script_GetDrawLayer },
    { "SetDrawLayer", &Script_SetDrawLayer },
    { "GetBlendMode", &Script_GetBlendMode },
    { "SetBlendMode", &Script_SetBlendMode },
    { "GetVertexColor", &Script_GetVertexColor },
    { "SetVertexColor", &Script_SetVertexColor },
    { "SetGradient", &Script_SetGradient },
    { "SetGradientAlpha", &Script_SetGradientAlpha },
    { "SetAlpha", &Script_SetAlpha },
    { "GetAlpha", &Script_GetAlpha },
    { "Show", &Script_Show },
    { "Hide", &Script_Hide },
    { "IsVisible", &Script_IsVisible },
    { "IsShown", &Script_IsShown },
    { "GetTexture", &Script_GetTexture },
    { "SetTexture", &Script_SetTexture },
    { "GetTexCoord", &Script_GetTexCoord },
    { "SetTexCoord", &Script_SetTexCoord },
    { "SetRotation", &Script_SetRotation },
    { "SetDesaturated", &Script_SetDesaturated },
    { "IsDesaturated", &Script_IsDesaturated },
    { "SetNonBlocking", &Script_SetNonBlocking },
    { "GetNonBlocking", &Script_GetNonBlocking },
    { "SetHorizTile", &Script_SetHorizTile },
    { "GetHorizTile", &Script_GetHorizTile },
    { "SetVertTile", &Script_SetVertTile },
    { "GetVertTile", &Script_GetVertTile },
};

// OFFSET: 0x0052BE10/0x14017C710
void CSimpleTexture::RegisterScriptMethods(lua_State* L) {
    CScriptRegion::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
