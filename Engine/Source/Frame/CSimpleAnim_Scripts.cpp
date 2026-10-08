#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0054B4E0
void ReadOrigin(lua_State* L, int32_t index, FRAMEPOINT& point, C2Vector& offset) {
    if (!StringToFramePoint(lua_tolstring(L, index, NULL), point)) {
        point = FRAMEPOINT_CENTER;
    }

    float x = lua_tonumber(L, index + 1);
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));

    float y = lua_tonumber(L, index + 2);
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
}

// OFFSET: 0x0053BB60
static int32_t Script_Play(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    anim->Play();
    return 0;
}

// OFFSET: 0x0053BBA0
static int32_t Script_Pause(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    anim->Pause();
    return 0;
}

// OFFSET: %0x0054B8F0
static int32_t Script_Stop(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    anim->Stop(true);
    return 0;
}

// OFFSET: %0x0054B930
static int32_t Script_IsDone(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushboolean(L, anim->m_progressWithDelay >= 1.0f);
    return 1;
}

// PROVEN: 29f807f1 IDENTICAL/NONE
// OFFSET: 0x0053BC80
static int32_t Script_IsPlaying(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushboolean(L, anim->m_playState == 1);
    return 1;
}

// PROVEN: 5b096820 IDENTICAL/DIFFERENT
// OFFSET: 0x0053BCD0/0x14018FAC0
static int32_t Script_IsPaused(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushboolean(L, anim->m_playState == 2);
    return 1;
}

// PROVEN: db88d1fa IDENTICAL/DIFFERENT
// OFFSET: 0x0053BD20/0x14018FB10
static int32_t Script_IsStopped(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushboolean(L, anim->m_playState == 0);
    return 1;
}

// PROVEN: 95fcd629 IDENTICAL/NONE
// OFFSET: 0x0053BD70/0x14018FB60
static int32_t Script_IsDelaying(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushboolean(L, anim->IsDelaying());
    return 1;
}

// OFFSET: %0x0054BAC0
static int32_t Script_SetStartDelay(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetStartDelay(delaySec)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetStartDelay(lua_tonumber(L, 2), true);
    return 0;
}

// OFFSET: %0x0054BB40
static int32_t Script_GetStartDelay(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_startDelay);
    return 1;
}

// OFFSET: %0x0054BB90
static int32_t Script_SetEndDelay(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetEndDelay(delaySec)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetEndDelay(lua_tonumber(L, 2), true);
    return 0;
}

// OFFSET: %0x0054BC10
static int32_t Script_GetEndDelay(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_endDelay);
    return 1;
}

// OFFSET: %0x0054BC60
static int32_t Script_SetDuration(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetDuration(durationSec)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetDuration(lua_tonumber(L, 2), true);
    return 0;
}

// OFFSET: %0x0054BCE0
static int32_t Script_GetDuration(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_duration);
    return 1;
}

// OFFSET: %0x0054BD30
static int32_t Script_GetSmoothProgress(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_smoothProgress);
    return 1;
}

// OFFSET: %0x0054BD80
static int32_t Script_SetSmoothProgress(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetSmoothProgress(smoothProgress)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetSmoothProgress(lua_tonumber(L, 2));
    return 0;
}

// OFFSET: %0x0054BE00
static int32_t Script_GetProgress(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_progress);
    return 1;
}

// OFFSET: %0x0054BE50
static int32_t Script_GetProgressWithDelay(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_progressWithDelay);
    return 1;
}

// OFFSET: %0x0054BF70
static int32_t Script_GetElapsed(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushnumber(L, anim->m_elapsed);
    return 1;
}

// OFFSET: 0x0053C180/0x140190060
static int32_t Script_SetOrder(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetOrder(order)", name ? name : "<unnamed>");
        return 0;
    }

    anim->SetOrder(lua_tointeger(L, 2) - 1, true, false);
    return 0;
}

// PROVEN: 3c3cc62a IDENTICAL/NONE
// OFFSET: 0x0053C200/0x140190110
static int32_t Script_GetOrder(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    lua_pushinteger(L, anim->m_order + 1);
    return 1;
}

// OFFSET: %0x0054C080
static int32_t Script_SetSmoothing(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:SetSmoothing(\"smoothingType\")", name ? name : "<unnamed>");
        return 0;
    }

    float in;
    float out;

    if (StringToSmoothWeights(lua_tolstring(L, 2, NULL), in, out)) {
        anim->SetSmoothing(in, out);
        return 0;
    }

    const char* name = anim->GetName();
    luaL_error(L, "%s:SetSmoothing(): Unknown smoothing type specified", name ? name : "<unnamed>");
    return 0;
}

// PROVEN: 9033697f IDENTICAL/UNPROVEN
// OFFSET: 0x0053C310/0x140190230
static int32_t Script_GetSmoothing(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    float in;
    float out;
    anim->GetSmoothing(in, out);

    lua_pushstring(L, SmoothWeightsToString(in, out));
    return 1;
}

// OFFSET: %0x0054C1C0
static int32_t Script_SetParent(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (lua_type(L, 2) == LUA_TNIL) {
        const char* name = anim->GetName();
        luaL_error(L, "%s:SetParent(): Cannot set a 'nil' parent for animations", name ? name : "<unnamed>");
        return 0;
    }

    CSimpleAnimGroup* group = NULL;

    if (lua_isstring(L, 2)) {
        group = static_cast<CSimpleAnimGroup*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 2, NULL), CSimpleAnimGroup::GetObjectType()));
    } else if (lua_type(L, 2) == LUA_TTABLE) {
        lua_rawgeti(L, 2, 0);
        group = static_cast<CSimpleAnimGroup*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!group) {
            const char* name = anim->GetName();
            luaL_error(L, "%s:SetParent(): Couldn't find 'this' in parent object", name ? name : "<unnamed>");
            return 0;
        }

        if (!group->IsA(CSimpleAnimGroup::GetObjectType())) {
            luaL_error(L, "%s:SetParent(): Wrong parent object type, expected AnimationGroup", anim->GetDisplayName());
            return 0;
        }
    }

    if (!group) {
        const char* name = anim->GetName();
        luaL_error(L, "%s:SetParent(): Couldn't find animation group named '%s'", name ? name : "<unnamed>", lua_tolstring(L, 2, NULL));
        return 0;
    }

    int32_t oldOrder = anim->m_order;

    if (lua_isnumber(L, 3)) {
        anim->SetOrder(lua_tointeger(L, 3) - 1, false, false);
    } else {
        int32_t order = anim->m_order;

        if (order == -1) {
            order = group->GetMaxAnimOrder() + 1;
        }

        anim->SetOrder(order, false, true);
    }

    if (!anim->ChangeParent(group) && oldOrder != anim->m_order) {
        group->NotifyOrderChanged(anim, oldOrder);
    }

    return 0;
}

// OFFSET: %0x0054C3B0
static int32_t Script_GetRegionParent(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    CScriptRegion* region = anim->GetRegionParent();

    if (region) {
        if (!region->m_luaRegistered) {
            region->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, region->m_luaObjectRef);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0053C600/0x1401905E0
static int32_t Script_HasScript(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = anim->GetName();
        luaL_error(L, "Usage: %s:HasScript(\"type\")", name ? name : "<unnamed>");
        return 0;
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    if (anim->GetScriptByNameCamelCase(lua_tolstring(L, 2, NULL), data)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0054C4F0
static int32_t Script_GetScript(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    return anim ? anim->GetScript(L) : 0;
}

// OFFSET: %0x0054C530
static int32_t Script_SetScript(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    return anim ? anim->SetScript(L) : 0;
}

// OFFSET: %0x0054C570
static int32_t Script_HookScript(lua_State* L) {
    CSimpleAnim* anim = static_cast<CSimpleAnim*>(FrameScript_GetObjectThis(L, CSimpleAnim::GetObjectType()));
    return anim ? anim->HookScript(L) : 0;
}

// PROVEN: c9389b1c IDENTICAL/NONE
// OFFSET: 0x0053C780
void CSimpleAnim::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CSimpleAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CSimpleAnim::RegisterScriptMethods);
}

// PROVEN: fa82111e IDENTICAL/NONE
// OFFSET: 0x0053B870
void CSimpleAnim::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CSimpleAnim::s_metatable);
    CSimpleAnim::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB9940
static FrameScript_Method s_ScriptMethods[] = {
    { "Play", &Script_Play },
    { "Pause", &Script_Pause },
    { "Stop", &Script_Stop },
    { "IsDone", &Script_IsDone },
    { "IsPlaying", &Script_IsPlaying },
    { "IsPaused", &Script_IsPaused },
    { "IsStopped", &Script_IsStopped },
    { "IsDelaying", &Script_IsDelaying },
    { "GetElapsed", &Script_GetElapsed },
    { "SetStartDelay", &Script_SetStartDelay },
    { "GetStartDelay", &Script_GetStartDelay },
    { "SetEndDelay", &Script_SetEndDelay },
    { "GetEndDelay", &Script_GetEndDelay },
    { "SetDuration", &Script_SetDuration },
    { "GetDuration", &Script_GetDuration },
    { "GetSmoothProgress", &Script_GetSmoothProgress },
    { "SetSmoothProgress", &Script_SetSmoothProgress },
    { "GetProgress", &Script_GetProgress },
    { "GetProgressWithDelay", &Script_GetProgressWithDelay },
    { "SetOrder", &Script_SetOrder },
    { "GetOrder", &Script_GetOrder },
    { "SetSmoothing", &Script_SetSmoothing },
    { "GetSmoothing", &Script_GetSmoothing },
    { "SetParent", &Script_SetParent },
    { "GetRegionParent", &Script_GetRegionParent },
    { "HasScript", &Script_HasScript },
    { "GetScript", &Script_GetScript },
    { "SetScript", &Script_SetScript },
    { "HookScript", &Script_HookScript },
};

// OFFSET: %0x0054B560
void CSimpleAnim::RegisterScriptMethods(lua_State* L) {
    CScriptObject::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
