#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// OFFSET: %0x0054D400
static int32_t Script_Play(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));

    if (group) {
        group->Play(StringToBOOL(L, 2, 0) != 0);
    }

    return 0;
}

// OFFSET: %0x0054D460
static int32_t Script_Pause(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    group->Pause();
    return 0;
}

// OFFSET: %0x0054D4A0
static int32_t Script_Stop(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    group->Stop(true);
    return 0;
}

// OFFSET: %0x0054D4E0
static int32_t Script_Finish(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    group->Finish();
    return 0;
}

// OFFSET: %0x0054D520
static int32_t Script_GetProgress(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushnumber(L, group->m_elapsed / group->m_duration);
    return 1;
}

// OFFSET: %0x0054D570
static int32_t Script_IsDone(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushboolean(L, group->IsDone());
    return 1;
}

// OFFSET: %0x0054D5C0
static int32_t Script_IsPaused(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushboolean(L, (group->m_flags & 0x4) != 0);
    return 1;
}

// OFFSET: %0x0054D610
static int32_t Script_IsPendingFinish(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushboolean(L, (group->m_flags & 0x2) != 0);
    return 1;
}

// OFFSET: %0x0054D660
static int32_t Script_GetDuration(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushnumber(L, group->m_duration);
    return 1;
}

// OFFSET: %0x0054D6A0
static int32_t Script_SetLooping(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = group->GetName();
        luaL_error(L, "Usage: %s:SetLooping(\"loopType\")", name ? name : "<unnamed>");
        return 0;
    }

    SimpleAnimLoopType loopType;

    if (StringToLoopType(lua_tolstring(L, 2, NULL), loopType)) {
        group->m_loopType = loopType;
        return 0;
    }

    const char* name = group->GetName();
    luaL_error(L, "%s:SetLooping(): Unknown loop type specified", name ? name : "<unnamed>");
    return 0;
}

// OFFSET: %0x0054D770
static int32_t Script_GetLooping(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushstring(L, LoopTypeToString(static_cast<SimpleAnimLoopType>(group->m_loopType)));
    return 1;
}

// OFFSET: %0x0054D7C0
static int32_t Script_GetLoopState(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushstring(L, LoopStateToString(static_cast<SimpleAnimLoopState>(group->m_loopState)));
    return 1;
}

// OFFSET: %0x0054D810
static int32_t Script_GetMaxOrder(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    int32_t order = group->GetMaxAnimOrder() + 1;
    lua_pushnumber(L, order);
    return 1;
}

// OFFSET: %0x0054D870
static int32_t Script_SetInitialOffset(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = group->GetName();
        luaL_error(L, "Usage: %s:SetInitialOffset(x, y)", name ? name : "<unnamed>");
        return 0;
    }

    C2Vector offset;
    float x = lua_tonumber(L, 2);
    offset.x = NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f));
    float y = lua_tonumber(L, 3);
    offset.y = NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f));
    group->SetInitialOffset(offset);

    return 0;
}

// OFFSET: %0x0054D960
static int32_t Script_GetInitialOffset(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));

    float x = group->m_initialOffset.x;
    float y = group->m_initialOffset.y;
    x = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x);
    y = DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
}

// OFFSET: %0x0054D9F0
static int32_t Script_HasScript(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = group->GetName();
        luaL_error(L, "Usage: %s:HasScript(\"type\")", name ? name : "<unnamed>");
        return 0;
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    if (group->GetScriptByNameCamelCase(lua_tolstring(L, 2, NULL), data)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: %0x0054DAB0
static int32_t Script_GetScript(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    return group ? group->GetScript(L) : 0;
}

// OFFSET: %0x0054DAF0
static int32_t Script_SetScript(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    return group ? group->SetScript(L) : 0;
}

// OFFSET: %0x0054DB30
static int32_t Script_HookScript(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    return group ? group->HookScript(L) : 0;
}

// OFFSET: %0x0054DC50
static int32_t Script_IsPlaying(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    lua_pushboolean(L, group->m_loopState && !(group->m_flags & 0x4));
    return 1;
}

// OFFSET: %0x0054DCB0
static int32_t Script_GetAnimations(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    TSExplicitList<CSimpleAnim, offsetof(CSimpleAnim, m_link)>* animations = &group->m_animations;

    if (!animations) {
        return 0;
    }

    int32_t count = 0;

    for (CSimpleAnim* anim = animations->Head(); anim; anim = animations->Next(anim)) {
        count++;
    }

    if (!lua_checkstack(L, count)) {
        const char* name = group->GetName();
        luaL_error(L, "%s:GetAnimations(): Stack overflow", name ? name : "<unnamed>");
        return 0;
    }

    for (CSimpleAnim* anim = animations->Head(); anim; anim = animations->Next(anim)) {
        if (!anim->m_luaRegistered) {
            anim->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, anim->m_luaObjectRef);
    }

    return count;
}

// OFFSET: %0x0054DFC0
static int32_t Script_CreateAnimation(lua_State* L) {
    CSimpleAnimGroup* group = static_cast<CSimpleAnimGroup*>(FrameScript_GetObjectThis(L, CSimpleAnimGroup::GetObjectType()));
    const char* type = "Animation";

    if (lua_isstring(L, 2)) {
        type = lua_tolstring(L, 2, NULL);
    }

    const char* name = NULL;

    if (lua_isstring(L, 3)) {
        name = lua_tolstring(L, 3, NULL);
    }

    XMLHashNodeRef ref;

    if (lua_type(L, 4) == LUA_TSTRING) {
        const char* inherits = lua_tolstring(L, 4, NULL);

        if (!ref.Acquire(inherits)) {
            const char* groupName = group->GetName();
            luaL_error(L, "%s:CreateAnimation(): Couldn't find inherited node \"%s\"", groupName ? groupName : "<unnamed>", inherits);
            return 0;
        }

        if (ref.m_locked) {
            const char* groupName = group->GetName();
            luaL_error(L, "%s:CreateAnimation(): Recursively inherited node \"%s\"", groupName ? groupName : "<unnamed>", inherits);
            return 0;
        }
    }

    CSimpleAnim* anim;

    if (type && *type) {
        if (!SStrCmpI(type, "Animation", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimpleAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleAnim(group) : NULL;
        } else if (!SStrCmpI(type, "Translation", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimpleTranslationAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleTranslationAnim(group) : NULL;
        } else if (!SStrCmpI(type, "Rotation", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimpleRotationAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleRotationAnim(group) : NULL;
        } else if (!SStrCmpI(type, "Scale", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimpleScaleAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleScaleAnim(group) : NULL;
        } else if (!SStrCmpI(type, "Path", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimplePathAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimplePathAnim(group) : NULL;
        } else if (!SStrCmpI(type, "Alpha", STORM_MAX_STR)) {
            void* mem = SMemAlloc(sizeof(CSimpleAlphaAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleAlphaAnim(group) : NULL;
        } else {
            void* mem = SMemAlloc(sizeof(CSimpleAnim), __FILE__, __LINE__, 0);
            anim = mem ? new (mem) CSimpleAnim(group) : NULL;
        }
    } else {
        void* mem = SMemAlloc(sizeof(CSimpleAnim), __FILE__, __LINE__, 0);
        anim = mem ? new (mem) CSimpleAnim(group) : NULL;
    }

    if (!anim) {
        lua_pushnil(L);
        return 1;
    }

    if (name && *name) {
        anim->SetName(name);
    }

    if (ref.m_node) {
        CStatus status;
        anim->LoadXML(ref.m_node, &status);
        anim->PostLoadXML(ref.m_node, &status);
    }

    if (anim->m_order == -1) {
        anim->SetOrder(group->GetMaxAnimOrder(), false, true);
    }

    group->NotifyAnimAdded(anim);

    if (!anim->m_luaRegistered) {
        anim->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, anim->m_luaObjectRef);
    return 1;
}

// OFFSET: %0x0054DB70
void CSimpleAnimGroup::CreateScriptMetaTable() {
    CSimpleAnimGroup::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleAnimGroup::RegisterScriptMethods);
}

// OFFSET: %0x0054B800
void CSimpleAnimGroup::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleAnimGroup::s_metatable);
    CSimpleAnimGroup::s_metatable = LUA_NOREF;
}

// STATIC: %0x00BB9B18
static FrameScript_Method s_ScriptMethods[] = {
    { "Play", &Script_Play },
    { "Pause", &Script_Pause },
    { "Stop", &Script_Stop },
    { "Finish", &Script_Finish },
    { "GetProgress", &Script_GetProgress },
    { "IsDone", &Script_IsDone },
    { "IsPlaying", &Script_IsPlaying },
    { "IsPaused", &Script_IsPaused },
    { "IsPendingFinish", &Script_IsPendingFinish },
    { "GetDuration", &Script_GetDuration },
    { "SetLooping", &Script_SetLooping },
    { "GetLooping", &Script_GetLooping },
    { "GetLoopState", &Script_GetLoopState },
    { "GetMaxOrder", &Script_GetMaxOrder },
    { "SetInitialOffset", &Script_SetInitialOffset },
    { "GetInitialOffset", &Script_GetInitialOffset },
    { "GetAnimations", &Script_GetAnimations },
    { "CreateAnimation", &Script_CreateAnimation },
    { "HasScript", &Script_HasScript },
    { "GetScript", &Script_GetScript },
    { "SetScript", &Script_SetScript },
    { "HookScript", &Script_HookScript },
};

// OFFSET: %0x0054B7E0
void CSimpleAnimGroup::RegisterScriptMethods(lua_State* L) {
    CScriptObject::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
