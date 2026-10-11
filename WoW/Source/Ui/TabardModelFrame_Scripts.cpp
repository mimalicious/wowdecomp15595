#include "TabardModelFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"


// OFFSET: 0x00A05F20
void CGTabardModelFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGTabardModelFrame::s_metatable);
    CGTabardModelFrame::s_metatable = LUA_NOREF;
}

// OFFSET: 0x00A06220
void CGTabardModelFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGTabardModelFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGTabardModelFrame::RegisterScriptMethods);
}

// OFFSET: 0x00A05EC0/0x1407A5A50
static int32_t Script_CanSaveTabardNow(lua_State* L) {
    if (CGPlayer_C::CanSaveTabardNow()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00A06060/0x1407A5C00
static int32_t Script_GetUpperBackgroundFileName(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    char fileName[260];
    CCharacterComponent::GetTabardBackgroundFileName(1, SECTION_TORSO_UPPER, frame->m_variations[4], fileName, sizeof(fileName));
    lua_pushstring(L, fileName);
    return 1;
}

// OFFSET: 0x00A060D0/0x1407A5C70
static int32_t Script_GetLowerBackgroundFileName(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    char fileName[260];
    CCharacterComponent::GetTabardBackgroundFileName(1, SECTION_TORSO_LOWER, frame->m_variations[4], fileName, sizeof(fileName));
    lua_pushstring(L, fileName);
    return 1;
}

// OFFSET: 0x00A06140/0x1407A5CE0
static int32_t Script_GetUpperEmblemFileName(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    char fileName[260];
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_UPPER, frame->m_variations[0], frame->m_variations[1], fileName, sizeof(fileName));
    lua_pushstring(L, fileName);
    return 1;
}

// OFFSET: 0x00A061B0/0x1407A5D50
static int32_t Script_GetLowerEmblemFileName(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    char fileName[260];
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_LOWER, frame->m_variations[0], frame->m_variations[1], fileName, sizeof(fileName));
    lua_pushstring(L, fileName);
    return 1;
}

// OFFSET: 0x00A06340/0x1407A5F20
static int32_t Script_Save(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    frame->Save();
    return 0;
}

// OFFSET: 0x00A06380/0x1407A5F60
static int32_t Script_CycleVariation(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CycleVariation(variationIndex, delta)");
        return 0;
    }

    uint32_t variation = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (variation >= 5) {
        luaL_error(L, "Invalid variationIndex in CycleVariation");
        return 0;
    }

    frame->CycleVariation(variation, static_cast<int32_t>(lua_tonumber(L, 3)));
    return 0;
}

// OFFSET: 0x00A06690/0x1407A62F0
static int32_t Script_InitializeTabardColors(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        frame->InitializeTabardColors(player);
    }

    return 0;
}

// OFFSET: 0x00A066F0/0x1407A6360
static int32_t Script_GetUpperEmblemTexture(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:GetUpperEmblemTexture(texture)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!texture) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetUpperEmblemTexture(): Couldn't find 'this' in texture object", name ? name : "<unnamed>");
        return 0;
    }

    if (!texture->IsA(CSimpleTexture::GetObjectType())) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetUpperEmblemTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
        return 0;
    }

    // STATIC: 0x00F76CC0
    static TSGrowableArray<CImVector> s_texels;

    if (!s_texels.m_count) {
        s_texels.SetCount(0x2000);
    }

    char fileName[260];
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_UPPER, frame->m_variations[0], frame->m_variations[1], fileName, sizeof(fileName));
    SStrPack(fileName, ".BLP", sizeof(fileName));
    CBLPFile blp;

    if (blp.Open(fileName, 0)) {
        uint8_t* data;
        uint32_t stride;

        if (blp.Lock(PIXEL_ARGB8888, 0, data, stride)) {
            for (uint32_t i = 0; i < s_texels.m_count; i++) {
                CImVector& texel = s_texels.m_data[i];
                texel.a = data[4 * i + 3];
                texel.r = 0xFF;
                texel.g = 0xFF;
                texel.b = 0xFF;
            }
        }

        blp.Unlock(0);
        blp.Close();
    }

    CTexture* emblem = TextureCreate(0x80, 0x40, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &s_texels, EmblemTextureUpdate, "TabardModelFrameUpper", 0);
    texture->SetTexture(emblem);
    TextureRelease(emblem);
    blp.Close();
    return 0;
}

// OFFSET: 0x00A06990/0x1407A6780
static int32_t Script_GetLowerEmblemTexture(lua_State* L) {
    CGTabardModelFrame* frame = static_cast<CGTabardModelFrame*>(FrameScript_GetObjectThis(L, CGTabardModelFrame::GetObjectType()));

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = frame->GetName();
        luaL_error(L, "Usage: %s:GetLowerEmblemTexture(texture)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleTexture* texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!texture) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetLowerEmblemTexture(): Couldn't find 'this' in texture object", name ? name : "<unnamed>");
        return 0;
    }

    if (!texture->IsA(CSimpleTexture::GetObjectType())) {
        const char* name = frame->GetName();
        luaL_error(L, "%s:GetLowerEmblemTexture(): Wrong object type, expected texture", name ? name : "<unnamed>");
    }

    // STATIC: 0x00F76CD0
    static TSGrowableArray<CImVector> s_texels;

    if (!s_texels.m_count) {
        s_texels.SetCount(0x1000);
    }

    char fileName[260];
    CCharacterComponent::GetTabardEmblemFileName(SECTION_TORSO_LOWER, frame->m_variations[0], frame->m_variations[1], fileName, sizeof(fileName));
    SStrPack(fileName, ".BLP", sizeof(fileName));
    CBLPFile blp;

    if (blp.Open(fileName, 0)) {
        uint8_t* data;
        uint32_t stride;

        if (blp.Lock(PIXEL_ARGB8888, 0, data, stride)) {
            for (uint32_t i = 0; i < s_texels.m_count; i++) {
                CImVector& texel = s_texels.m_data[i];
                texel.a = data[4 * i + 3];
                texel.r = 0xFF;
                texel.g = 0xFF;
                texel.b = 0xFF;
            }
        }

        blp.Unlock(0);
        blp.Close();
    }

    CTexture* emblem = TextureCreate(0x80, 0x20, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &s_texels, EmblemTextureUpdate, "TabardModelFrameLower", 0);
    texture->SetTexture(emblem);
    TextureRelease(emblem);
    blp.Close();
    return 0;
}

// STATIC: 0x00D17718
static FrameScript_Method s_ScriptMethods[] = {
    { "InitializeTabardColors", &Script_InitializeTabardColors },
    { "Save", &Script_Save },
    { "CycleVariation", &Script_CycleVariation },
    { "GetUpperBackgroundFileName", &Script_GetUpperBackgroundFileName },
    { "GetLowerBackgroundFileName", &Script_GetLowerBackgroundFileName },
    { "GetUpperEmblemFileName", &Script_GetUpperEmblemFileName },
    { "GetLowerEmblemFileName", &Script_GetLowerEmblemFileName },
    { "GetUpperEmblemTexture", &Script_GetUpperEmblemTexture },
    { "GetLowerEmblemTexture", &Script_GetLowerEmblemTexture },
    { "CanSaveTabardNow", &Script_CanSaveTabardNow },
};

// PROVEN: 3e29c68b IDENTICAL/NONE
// OFFSET: 0x00A05F00/0x1407A5A90
void CGTabardModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
