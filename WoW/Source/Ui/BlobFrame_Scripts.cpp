#include "BlobFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: 0x00A002C0/0x14079E8D0
static int32_t Script_SetFillAlpha(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    double value = lua_tonumber(L, 2);
    float alpha = value;

    if (value > 255.0) {
        alpha = 255.0f;
    } else if (value < 0.0) {
        alpha = 0.0f;
    }

    frame->m_fillAlpha = static_cast<uint8_t>(alpha);
    return 0;
}

// OFFSET: 0x00A00360/0x14079E960
static int32_t Script_SetBorderAlpha(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    double value = lua_tonumber(L, 2);
    float alpha = value;

    if (value > 255.0) {
        alpha = 255.0f;
    } else if (value < 0.0) {
        alpha = 0.0f;
    }

    frame->m_borderAlpha = static_cast<uint8_t>(alpha);
    return 0;
}

// OFFSET: 0x00A00400/0x14079E9F0
static int32_t Script_SetBorderScalar(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    double scalar = lua_tonumber(L, 2);

    if (scalar > 10.0) {
        scalar = 10.0;
    } else if (scalar < 0.0) {
        scalar = 0.0;
    }

    frame->m_borderScalar = scalar;
    return 0;
}

// OFFSET: 0x00A00490/0x14079EA90
static int32_t Script_DrawBlob(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    int32_t id = static_cast<int32_t>(lua_tointeger(L, 2));
    bool show = lua_toboolean(L, 3) != 0;
    frame->DrawBlob(id, show);
    return 0;
}

// OFFSET: 0x00A004F0/0x14079EB10
static int32_t Script_DrawNone(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    frame->DrawNone();
    return 0;
}

// OFFSET: 0x00A00530/0x14079EB50
static int32_t Script_EnableSmoothing(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    frame->m_smoothing = lua_toboolean(L, 2) != 0;
    return 0;
}

// OFFSET: 0x00A00580/0x14079EBB0
static int32_t Script_EnableMerging(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    frame->m_merging = lua_toboolean(L, 2) != 0;
    return 0;
}

// OFFSET: 0x00A005D0/0x14079EC10
static int32_t Script_SetMergeThreshold(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    double threshold = lua_tonumber(L, 2);

    if (threshold < 0.1f) {
        threshold = 0.1f;
    } else if (threshold > 0.5f) {
        threshold = 0.5f;
    }

    frame->m_mergeThreshold = threshold;
    return 0;
}

// OFFSET: 0x00A00660/0x14079ECC0
static int32_t Script_SetNumSplinePoints(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    int32_t points = lua_tointeger(L, 2);

    if (points >= 8) {
        if (points > 30) {
            points = 30;
        }
    } else {
        points = 8;
    }

    frame->m_numSplinePoints = points;
    return 0;
}

// OFFSET: 0x00A016B0/0x1407A0180
static int32_t Script_SetFillTexture(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    frame->SetFillTexture(lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x00A01700/0x1407A01E0
static int32_t Script_SetBorderTexture(lua_State* L) {
    CGBlobFrame* frame = static_cast<CGBlobFrame*>(FrameScript_GetObjectThis(L, CGBlobFrame::GetObjectType()));
    frame->SetBorderTexture(lua_tolstring(L, 2, NULL));
    return 0;
}

// STATIC: 0x00D175D8/0x140B328F0
static FrameScript_Method s_ScriptMethods[] = {
    { "SetFillTexture", &Script_SetFillTexture },
    { "SetFillAlpha", &Script_SetFillAlpha },
    { "SetBorderTexture", &Script_SetBorderTexture },
    { "SetBorderAlpha", &Script_SetBorderAlpha },
    { "SetBorderScalar", &Script_SetBorderScalar },
    { "DrawBlob", &Script_DrawBlob },
    { "DrawNone", &Script_DrawNone },
    { "EnableSmoothing", &Script_EnableSmoothing },
    { "EnableMerging", &Script_EnableMerging },
    { "SetMergeThreshold", &Script_SetMergeThreshold },
    { "SetNumSplinePoints", &Script_SetNumSplinePoints },
};

// OFFSET: 0x009FFCD0/0x14079E540
void CGBlobFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// OFFSET: 0x009FFCF0/0x14079E570
void CGBlobFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGBlobFrame::s_metatable);
    CGBlobFrame::s_metatable = LUA_NOREF;
}

// OFFSET: 0x00A006C0/0x14079ED30
void CGBlobFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGBlobFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGBlobFrame::RegisterScriptMethods);
}
