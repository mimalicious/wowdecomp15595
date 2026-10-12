#include "WoW/Source/Ui/CUFProfiles.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"

// PROVEN: 957e9877 IDENTICAL/IDENTICAL
// OFFSET: 0x009C8210/0x140756A70
static int32_t Script_GetMaxNumCUFProfiles(lua_State* L) {
    lua_pushnumber(L, 5.0);
    return 1;
}

// PROVEN: bb92d69c IDENTICAL/IDENTICAL
// OFFSET: 0x009C8240/0x140756A90
static int32_t Script_HasLoadedCUFProfiles(lua_State* L) {
    lua_pushboolean(L, CGCUFProfiles::s_loaded);
    return 1;
}

// PROVEN: 109073a8 IDENTICAL/IDENTICAL
// OFFSET: 0x009C8920/0x1407572E0
static int32_t Script_GetNumRaidProfiles(lua_State* L) {
    lua_pushinteger(L, CGCUFProfiles::s_profiles.Count());
    return 1;
}

// PROVEN: f5e29cb2 IDENTICAL/DIFFERENT
// OFFSET: 0x009C8940/0x140757300
static int32_t Script_GetRaidProfileName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRaidProfileName(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index >= CGCUFProfiles::s_profiles.Count()) {
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::s_profiles[index];

    if (!profile) {
        return 0;
    }

    lua_pushstring(L, profile->m_name);
    return 1;
}

// OFFSET: 0x009C8FF0/0x140757BF0
static int32_t Script_GetRaidProfileFlattenedOptions(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetRaidProfileFlattened(\"profile\")");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (!profile) {
        return 0;
    }

    lua_createtable(L, 0, 0);
    CGCUFProfiles::PushOptions(profile, L, 0, -1);
    CGCUFProfiles::PushOptions(profile, L, 2, -1);
    CGCUFProfiles::PushOptions(profile, L, 3, -1);
    return 1;
}

// OFFSET: 0x009C9080/0x140757CC0
static int32_t Script_GetRaidProfileOption(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: GetRaidProfileOption(\"profile\", \"optionName\")");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (!profile) {
        return 0;
    }

    const char* option = lua_tostring(L, 2);
    TSHashTable<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>* table = CGCUFProfiles::s_options;
    CGCUFProfiles::OPTIONENTRY* entry;

    while (!(entry = table->Ptr(option))) {
        table++;

        if (table >= &CGCUFProfiles::s_options[4]) {
            luaL_error(L, "Unknown option: %s", option);
            return 0;
        }
    }

    entry->m_get(profile, L);
    return 1;
}

// OFFSET: 0x009C9150/0x140757DD0
static int32_t Script_SetRaidProfileOption(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetRaidProfileOption(\"profile\", \"optionName\", value)");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (!profile) {
        return 0;
    }

    const char* option = lua_tostring(L, 2);
    TSHashTable<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>* table = CGCUFProfiles::s_options;
    CGCUFProfiles::OPTIONENTRY* entry;

    while (!(entry = table->Ptr(option))) {
        table++;

        if (table >= &CGCUFProfiles::s_options[4]) {
            luaL_error(L, "Unknown option: %s", option);
            return 0;
        }
    }

    entry->m_set(profile, L, 3);
    CGCUFProfiles::s_dirty = true;
    return 0;
}

// OFFSET: 0x009C9230/0x140757EE0
static int32_t Script_GetRaidProfileSavedPosition(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetRaidProfileSavedPosition(\"profile\")");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (!profile) {
        return 0;
    }

    lua_pushboolean(L, profile->m_position.m_dynamic);
    lua_pushstring(L, profile->m_position.m_topPoint ? "BOTTOM" : "TOP");
    lua_pushnumber(L, profile->m_position.m_topOffset);
    lua_pushstring(L, profile->m_position.m_bottomPoint ? "BOTTOM" : "TOP");
    lua_pushnumber(L, profile->m_position.m_bottomOffset);

    switch (profile->m_position.m_leftPoint) {
        case 0:
            lua_pushstring(L, "ATTACHED");
            break;
        case 1:
            lua_pushstring(L, "LEFT");
            break;
        case 2:
            lua_pushstring(L, "RIGHT");
            break;
    }

    lua_pushnumber(L, profile->m_position.m_leftOffset);
    return 7;
}

// OFFSET: 0x009C9370/0x140758060
static int32_t Script_SetRaidProfileSavedPosition(lua_State* L) {
    if (!lua_isstring(L, 1) || (!lua_toboolean(L, 2) && (!lua_isstring(L, 3) || !lua_isnumber(L, 4) || !lua_isstring(L, 5) || !lua_isnumber(L, 6) || !lua_isstring(L, 7) || !lua_isnumber(L, 8)))) {
        luaL_error(L, "Usage: SetRaidProfileSavedPosition(\"profile\", isDynamic, \"topPoint\", topOffset, \"bottomPoint\", bottomOffset, \"leftPoint\", leftOffset)");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (!profile) {
        return 0;
    }

    const char* topPoint = lua_tostring(L, 3);
    const char* bottomPoint = lua_tostring(L, 5);
    const char* leftPoint = lua_tostring(L, 7);
    profile->m_position.m_dynamic = lua_toboolean(L, 2);

    if (!profile->m_position.m_dynamic) {
        profile->m_position.m_topPoint = SStrCmpI(topPoint, "BOTTOM", STORM_MAX_STR) == 0;
        profile->m_position.m_topOffset = lua_tointeger(L, 4);
        profile->m_position.m_bottomPoint = SStrCmpI(bottomPoint, "BOTTOM", STORM_MAX_STR) == 0;
        profile->m_position.m_bottomOffset = lua_tointeger(L, 6);

        if (!SStrCmpI(leftPoint, "LEFT", STORM_MAX_STR)) {
            profile->m_position.m_leftPoint = 1;
        } else if (!SStrCmp(leftPoint, "RIGHT", STORM_MAX_STR)) {
            profile->m_position.m_leftPoint = 2;
        } else {
            profile->m_position.m_leftPoint = 0;
        }

        profile->m_position.m_leftOffset = lua_tointeger(L, 8);
    }

    CGCUFProfiles::s_dirty = true;
    return 0;
}

// PROVEN: 2f92b9a3 IDENTICAL/IDENTICAL
// OFFSET: 0x009C95C0/0x140660F20
static int32_t Script_RaidProfileHasUnsavedChanges(lua_State* L) {
    lua_pushboolean(L, CGCUFProfiles::HasUnsavedChanges());
    return 1;
}

// OFFSET: 0x009C95E0/0x140758360
static int32_t Script_RaidProfileExists(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RaidProfileExists(\"profile\")");
        return 0;
    }

    lua_pushboolean(L, CGCUFProfiles::GetProfile(lua_tostring(L, 1)) != NULL);
    return 1;
}

// OFFSET: 0x009C9B80/0x140758B50
static int32_t Script_SaveRaidProfileCopy(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SaveRaidProfileCopy(\"profile\")");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (profile) {
        CGCUFProfiles::SaveProfileCopy(profile);
    }

    return 0;
}

// PROVEN: 87d2508f IDENTICAL/DIFFERENT
// OFFSET: 0x009C9BE0/0x140758BC0
static int32_t Script_RestoreRaidProfileFromCopy(lua_State* L) {
    CGCUFProfiles::RestoreProfileCopy();
    return 0;
}

// OFFSET: 0x009C9BF0/0x140758C30
static int32_t Script_DeleteRaidProfile(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: DeleteRaidProfile(\"profile\")");
        return 0;
    }

    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(lua_tostring(L, 1));

    if (profile) {
        CGCUFProfiles::DeleteProfile(profile);
    }

    return 0;
}

// OFFSET: 0x009CB130/0x14075AB00
static int32_t Script_CreateNewRaidProfile(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CreateNewRaidProfile(\"name\"[, \"baseOnProfile\"])");
        return 0;
    }

    const char* name = lua_tostring(L, 1);

    if (CGCUFProfiles::GetProfile(name)) {
        luaL_error(L, "That profile already exists.");
        return 0;
    }

    if (SStrLenUTF8(name) >= 32) {
        luaL_error(L, "Name too long.");
        return 0;
    }

    if (CGCUFProfiles::s_profiles.Count() >= 5) {
        luaL_error(L, "Already at maximum number of profiles.");
        return 0;
    }

    CUFProfileInfo* base = NULL;

    if (lua_isstring(L, 2)) {
        base = CGCUFProfiles::GetProfile(lua_tostring(L, 2));
    }

    CGCUFProfiles::CreateProfile(name, base);
    return 0;
}

// STATIC: 0x00D15F08/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumRaidProfiles", &Script_GetNumRaidProfiles },
    { "GetRaidProfileName", &Script_GetRaidProfileName },
    { "GetRaidProfileFlattenedOptions", &Script_GetRaidProfileFlattenedOptions },
    { "GetRaidProfileOption", &Script_GetRaidProfileOption },
    { "SetRaidProfileOption", &Script_SetRaidProfileOption },
    { "GetRaidProfileSavedPosition", &Script_GetRaidProfileSavedPosition },
    { "SetRaidProfileSavedPosition", &Script_SetRaidProfileSavedPosition },
    { "SaveRaidProfileCopy", &Script_SaveRaidProfileCopy },
    { "RestoreRaidProfileFromCopy", &Script_RestoreRaidProfileFromCopy },
    { "CreateNewRaidProfile", &Script_CreateNewRaidProfile },
    { "DeleteRaidProfile", &Script_DeleteRaidProfile },
    { "RaidProfileHasUnsavedChanges", &Script_RaidProfileHasUnsavedChanges },
    { "GetRaidProfileSavedPosition", &Script_GetRaidProfileSavedPosition },
    { "RaidProfileExists", &Script_RaidProfileExists },
    { "GetMaxNumCUFProfiles", &Script_GetMaxNumCUFProfiles },
    { "HasLoadedCUFProfiles", &Script_HasLoadedCUFProfiles },
};

// PROVEN: b0f6b848 IDENTICAL/UNPROVEN
// OFFSET: 0x009C8260/0x140756AB0
void CompactUnitFrameProfilesRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 8e8651c2 IDENTICAL/UNPROVEN
// OFFSET: 0x009C8290/0x140756B00
void CompactUnitFrameProfilesUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
