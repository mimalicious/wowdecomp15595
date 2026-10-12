#include "WoW/Source/Ui/CUFProfiles.h"
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F60E38/0x140E55F40
char CGCUFProfiles::s_savedName[128];

// STATIC: 0x00F60EB8/0x140E55FC0
CUFAUTOACTIVATE CGCUFProfiles::s_savedAutoActivate;

// STATIC: 0x00F60EBA/0x140E55FC2
bool CGCUFProfiles::s_dirty;

// STATIC: 0x00F60EBB/0x140E55FC3
bool CGCUFProfiles::s_loaded;

// STATIC: 0x00F60EBC/0x140E55FC8
CUFFRAMEOPTIONS CGCUFProfiles::s_savedOptions;

// STATIC: 0x00F60EC4/0x140E55FD4
TSGrowableArray<CUFProfileInfo*> CGCUFProfiles::s_profiles;

// STATIC: 0x00F60ED4/0x140BCFE30
TSHashTable<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I> CGCUFProfiles::s_profileHash;

// STATIC: 0x00F60F00/0x140E55FF0
TSHashTable<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR> CGCUFProfiles::s_options[4];

// PROVEN: 409a9fdb IDENTICAL/NONE
// OFFSET: 0x009C7360
void CUFProfileInfo::CopySettings(const CUFProfileInfo* source) {
    this->m_options = source->m_options;
    this->m_position = source->m_position;
    this->m_state = source->m_state;
    this->m_autoActivate = source->m_autoActivate;
}

// PROVEN: 1e5740c6 IDENTICAL/NONE
// OFFSET: 0x009C73C0
void CUFProfileInfo::SetDefaults() {
    this->m_options.m_frameHeight = 36;
    this->m_options.m_frameWidth = 72;
    this->m_options.m_sortBy = 0;
    this->m_options.m_healthText = 0;
    this->m_options.m_keepGroupsTogether = 0;
    this->m_options.m_displayPets = 0;
    this->m_options.m_displayMainTankAndAssist = 1;
    this->m_options.m_displayHealPrediction = 1;
    this->m_options.m_displayAggroHighlight = 1;
    this->m_options.m_displayOnlyDispellableDebuffs = 0;
    this->m_options.m_displayPowerBar = 0;
    this->m_options.m_displayBorder = 1;
    this->m_options.m_useClassColors = 0;
    this->m_options.m_displayNonBossDebuffs = 1;
    this->m_options.m_horizontalGroups = 0;
    memset(&this->m_position, 0, sizeof(this->m_position));
    this->m_position.m_dynamic = 1;
    this->m_state.m_locked = 1;
    this->m_state.m_shown = 1;
    memset(&this->m_autoActivate, 0, sizeof(this->m_autoActivate));
}

// PROVEN: bde81a30 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7DA0/0x140756590
static void GetOptionFrameHeight(CUFProfileInfo* profile, lua_State* L) {
    lua_pushnumber(L, profile->m_options.m_frameHeight);
}

// PROVEN: aafaa4e3 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7DD0/0x1407565B0
static void SetOptionFrameHeight(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    if (lua_isnumber(L, index)) {
        profile->m_options.m_frameHeight = static_cast<int32_t>(lua_tonumber(L, index));
    }
}

// PROVEN: 28523cab IDENTICAL/IDENTICAL
// OFFSET: 0x009C7E30/0x140756600
static void GetOptionFrameWidth(CUFProfileInfo* profile, lua_State* L) {
    lua_pushnumber(L, profile->m_options.m_frameWidth);
}

// PROVEN: 18c66d92 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7E60/0x140756620
static void SetOptionFrameWidth(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    if (lua_isnumber(L, index)) {
        profile->m_options.m_frameWidth = static_cast<int32_t>(lua_tonumber(L, index));
    }
}

// PROVEN: c3f124bc IDENTICAL/IDENTICAL
// OFFSET: 0x009C7EC0/0x140756670
static void GetOptionSortBy(CUFProfileInfo* profile, lua_State* L) {
    switch (profile->m_options.m_sortBy) {
        case 0:
            lua_pushstring(L, "role");
            break;
        case 1:
            lua_pushstring(L, "group");
            break;
        case 2:
            lua_pushstring(L, "alphabetical");
            break;
    }
}

// PROVEN: 9bcad95d IDENTICAL/DIFFERENT
// OFFSET: 0x009C7F20/0x1407566D0
static void SetOptionSortBy(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    if (lua_isstring(L, index)) {
        const char* type = lua_tostring(L, index);

        if (!SStrCmpI(type, "role", STORM_MAX_STR)) {
            profile->m_options.m_sortBy = 0;
            return;
        }

        if (!SStrCmpI(type, "group", STORM_MAX_STR)) {
            profile->m_options.m_sortBy = 1;
            return;
        }

        if (!SStrCmpI(type, "alphabetical", STORM_MAX_STR)) {
            profile->m_options.m_sortBy = 2;
            return;
        }
    }

    luaL_error(L, "Invalid sort type. Valid types: \"role\", \"group\", \"alphabetical\"");
    return;
}

// PROVEN: ed4817e3 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7FF0/0x1407567D0
static void GetOptionHealthText(CUFProfileInfo* profile, lua_State* L) {
    switch (profile->m_options.m_healthText) {
        case 0:
            lua_pushstring(L, "none");
            break;
        case 1:
            lua_pushstring(L, "health");
            break;
        case 2:
            lua_pushstring(L, "losthealth");
            break;
        case 3:
            lua_pushstring(L, "perc");
            break;
    }
}

// PROVEN: 0c8056f7 IDENTICAL/DIFFERENT
// OFFSET: 0x009C8070/0x140756850
static void SetOptionHealthText(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    if (lua_isstring(L, index)) {
        const char* type = lua_tostring(L, index);

        if (!SStrCmpI(type, "none", STORM_MAX_STR)) {
            profile->m_options.m_healthText = 0;
            return;
        }

        if (!SStrCmpI(type, "health", STORM_MAX_STR)) {
            profile->m_options.m_healthText = 1;
            return;
        }

        if (!SStrCmpI(type, "losthealth", STORM_MAX_STR)) {
            profile->m_options.m_healthText = 2;
            return;
        }

        if (!SStrCmpI(type, "perc", STORM_MAX_STR)) {
            profile->m_options.m_healthText = 3;
            return;
        }
    }

    luaL_error(L, "Invalid sort type. Valid types: \"none\", \"health\", \"losthealth\", \"perc\"");
    return;
}

// PROVEN: ec162bca IDENTICAL/IDENTICAL
// OFFSET: 0x009C7430/0x140755C90
static void GetOptionKeepGroupsTogether(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_keepGroupsTogether);
}

// PROVEN: ac8351c1 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7450/0x140755CB0
static void SetOptionKeepGroupsTogether(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_keepGroupsTogether = lua_toboolean(L, index) != 0;
}

// PROVEN: 8325af76 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7490/0x140755CF0
static void GetOptionDisplayPets(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayPets);
}

// PROVEN: df2b0b84 IDENTICAL/IDENTICAL
// OFFSET: 0x009C74C0/0x140755D10
static void SetOptionDisplayPets(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayPets = lua_toboolean(L, index) != 0;
}

// PROVEN: 5935cdd9 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7500/0x140755D50
static void GetOptionDisplayMainTankAndAssist(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayMainTankAndAssist);
}

// PROVEN: 6fcc5590 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7530/0x140755D70
static void SetOptionDisplayMainTankAndAssist(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayMainTankAndAssist = lua_toboolean(L, index) != 0;
}

// PROVEN: b9deaf9e IDENTICAL/IDENTICAL
// OFFSET: 0x009C7570/0x140755DB0
static void GetOptionDisplayHealPrediction(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayHealPrediction);
}

// PROVEN: 41acd9fe IDENTICAL/IDENTICAL
// OFFSET: 0x009C75A0/0x140755DD0
static void SetOptionDisplayHealPrediction(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayHealPrediction = lua_toboolean(L, index) != 0;
}

// PROVEN: eab02afe IDENTICAL/IDENTICAL
// OFFSET: 0x009C75E0/0x140755E10
static void GetOptionDisplayAggroHighlight(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayAggroHighlight);
}

// PROVEN: 5eac86d3 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7610/0x140755E30
static void SetOptionDisplayAggroHighlight(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayAggroHighlight = lua_toboolean(L, index) != 0;
}

// PROVEN: 0f8adb97 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7650/0x140755E70
static void GetOptionDisplayOnlyDispellableDebuffs(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayOnlyDispellableDebuffs);
}

// PROVEN: fc03569e IDENTICAL/IDENTICAL
// OFFSET: 0x009C7680/0x140755E90
static void SetOptionDisplayOnlyDispellableDebuffs(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayOnlyDispellableDebuffs = lua_toboolean(L, index) != 0;
}

// PROVEN: 8df370bc IDENTICAL/IDENTICAL
// OFFSET: 0x009C76C0/0x140755ED0
static void GetOptionDisplayPowerBar(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayPowerBar);
}

// PROVEN: 007a8635 IDENTICAL/IDENTICAL
// OFFSET: 0x009C76F0/0x140755EF0
static void SetOptionDisplayPowerBar(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayPowerBar = lua_toboolean(L, index) != 0;
}

// PROVEN: 017dee1e IDENTICAL/IDENTICAL
// OFFSET: 0x009C7730/0x140755F30
static void GetOptionDisplayBorder(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayBorder);
}

// PROVEN: f132b5a9 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7750/0x140755F50
static void SetOptionDisplayBorder(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayBorder = lua_toboolean(L, index) != 0;
}

// PROVEN: 76b6d65f IDENTICAL/IDENTICAL
// OFFSET: 0x009C7790/0x140755F90
static void GetOptionUseClassColors(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_useClassColors);
}

// PROVEN: 098c3f27 IDENTICAL/IDENTICAL
// OFFSET: 0x009C77B0/0x140755FB0
static void SetOptionUseClassColors(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_useClassColors = lua_toboolean(L, index) != 0;
}

// PROVEN: 570cfc95 IDENTICAL/IDENTICAL
// OFFSET: 0x009C77F0/0x140755FF0
static void GetOptionDisplayNonBossDebuffs(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_displayNonBossDebuffs);
}

// PROVEN: 5168ccce IDENTICAL/IDENTICAL
// OFFSET: 0x009C7820/0x140756010
static void SetOptionDisplayNonBossDebuffs(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_displayNonBossDebuffs = lua_toboolean(L, index) != 0;
}

// PROVEN: a8adf523 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7860/0x140756050
static void GetOptionHorizontalGroups(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_options.m_horizontalGroups);
}

// PROVEN: 1dfbf263 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7890/0x140756070
static void SetOptionHorizontalGroups(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_options.m_horizontalGroups = lua_toboolean(L, index) != 0;
}

// PROVEN: ab53d0bf IDENTICAL/IDENTICAL
// OFFSET: 0x009C78D0/0x1407560B0
static void GetOptionLocked(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_state.m_locked);
}

// PROVEN: 8ec40215 IDENTICAL/IDENTICAL
// OFFSET: 0x009C78F0/0x1407560D0
static void SetOptionLocked(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_state.m_locked = lua_toboolean(L, index) != 0;
}

// PROVEN: 36fa39db IDENTICAL/IDENTICAL
// OFFSET: 0x009C7930/0x140756110
static void GetOptionShown(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_state.m_shown);
}

// PROVEN: 4162e5e9 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7960/0x140756130
static void SetOptionShown(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_state.m_shown = lua_toboolean(L, index) != 0;
}

// PROVEN: 783bedda IDENTICAL/IDENTICAL
// OFFSET: 0x009C79A0/0x140756170
static void GetOptionAutoActivate2Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_2Players);
}

// PROVEN: 75ee9e9d IDENTICAL/IDENTICAL
// OFFSET: 0x009C79C0/0x140756190
static void SetOptionAutoActivate2Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_2Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 735ea729 IDENTICAL/IDENTICAL
// OFFSET: 0x009C79F0/0x1407561D0
static void GetOptionAutoActivate3Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_3Players);
}

// PROVEN: 52da1853 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7A20/0x1407561F0
static void SetOptionAutoActivate3Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_3Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 979a927b IDENTICAL/IDENTICAL
// OFFSET: 0x009C7A50/0x140756230
static void GetOptionAutoActivate5Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_5Players);
}

// PROVEN: 48d6703b IDENTICAL/IDENTICAL
// OFFSET: 0x009C7A80/0x140756250
static void SetOptionAutoActivate5Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_5Players = lua_toboolean(L, index) != 0;
}

// PROVEN: ab520d60 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7AB0/0x140756290
static void GetOptionAutoActivate10Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_10Players);
}

// PROVEN: 04b5bc5b IDENTICAL/IDENTICAL
// OFFSET: 0x009C7AE0/0x1407562B0
static void SetOptionAutoActivate10Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_10Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 59b1f4ae IDENTICAL/IDENTICAL
// OFFSET: 0x009C7B20/0x1407562F0
static void GetOptionAutoActivate15Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_15Players);
}

// PROVEN: 31ec9928 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7B50/0x140756310
static void SetOptionAutoActivate15Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_15Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 1837ef1c IDENTICAL/IDENTICAL
// OFFSET: 0x009C7B80/0x140756350
static void GetOptionAutoActivate25Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_25Players);
}

// PROVEN: 672a6253 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7BB0/0x140756370
static void SetOptionAutoActivate25Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_25Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 9c1f8461 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7BE0/0x1407563B0
static void GetOptionAutoActivate40Players(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_40Players);
}

// PROVEN: 4584e82d IDENTICAL/IDENTICAL
// OFFSET: 0x009C7C10/0x1407563D0
static void SetOptionAutoActivate40Players(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_40Players = lua_toboolean(L, index) != 0;
}

// PROVEN: 298cb2aa IDENTICAL/IDENTICAL
// OFFSET: 0x009C7C40/0x140756410
static void GetOptionAutoActivateSpec1(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_spec1);
}

// PROVEN: 090348d1 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7C60/0x140756430
static void SetOptionAutoActivateSpec1(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_spec1 = lua_toboolean(L, index) != 0;
}

// PROVEN: 92515ab9 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7C90/0x140756470
static void GetOptionAutoActivateSpec2(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_spec2);
}

// PROVEN: 7ca3d2bb IDENTICAL/IDENTICAL
// OFFSET: 0x009C7CB0/0x140756490
static void SetOptionAutoActivateSpec2(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_spec2 = lua_toboolean(L, index) != 0;
}

// PROVEN: 4acbdab7 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7CE0/0x1407564D0
static void GetOptionAutoActivatePvP(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_pvp);
}

// PROVEN: 7b6277ca IDENTICAL/IDENTICAL
// OFFSET: 0x009C7D10/0x1407564F0
static void SetOptionAutoActivatePvP(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_pvp = lua_toboolean(L, index) != 0;
}

// PROVEN: 780b49f7 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7D40/0x140756530
static void GetOptionAutoActivatePvE(CUFProfileInfo* profile, lua_State* L) {
    lua_pushboolean(L, profile->m_autoActivate.m_pve);
}

// PROVEN: 69869e93 IDENTICAL/IDENTICAL
// OFFSET: 0x009C7D70/0x140756550
static void SetOptionAutoActivatePvE(CUFProfileInfo* profile, lua_State* L, int32_t index) {
    profile->m_autoActivate.m_pve = lua_toboolean(L, index) != 0;
}

// PROVEN: 24a6857a IDENTICAL/IDENTICAL
// OFFSET: 0x009C8170/0x140756980
void CGCUFProfiles::EnterWorld(bool reload) {
    if (reload) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMPACT_UNIT_FRAME_PROFILES_LOADED, NULL);
    }
}

// OFFSET: 0x009C8190/0x1407569A0
void CGCUFProfiles::GetProfilesPath(char* path, uint32_t size) {
    SStrPrintf(path, size, "WTF\\Account\\%s\\%s\\%s", ClientServices::GetAccountName(), ClientServices::GetSelectedRealmName(), ClientServices::GetCharacterName());

    if (!OsDirectoryExists(path)) {
        OsCreateDirectory(path, 1);
    }

    SStrPack(path, "\\CUFProfiles.txt", size);
}

// OFFSET: 0x009C8390/0x140756C30
CUFProfileInfo::CUFProfileInfo(const char* name) {
    this->m_version = 3;
    this->SetDefaults();
    SStrCopy(this->m_name, name, sizeof(this->m_name));
}

// OFFSET: 0x009C83E0/0x140756CE0
void CGCUFProfiles::SaveProfiles() {
    char path[260];
    char backup[260];
    bool created = false;

    CGCUFProfiles::GetProfilesPath(path, sizeof(path));

    if (OsFileExists(path)) {
        SStrCopy(backup, path, sizeof(backup));
        SStrPack(backup, ".bak", sizeof(backup));
        OsMoveFile(path, backup);
    } else {
        created = true;
    }

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return;
    }

    uint32_t count = CGCUFProfiles::s_profiles.Count();
    uint32_t written;
    OsWriteFile(file, &count, sizeof(count), &written);

    for (uint32_t i = 0; i < count; i++) {
        CUFProfileInfo* profile = i < CGCUFProfiles::s_profiles.Count() ? CGCUFProfiles::s_profiles[i] : NULL;
        OsWriteFile(file, profile, sizeof(CUFProfileInfo), &written);
    }

    OsCloseFile(file);

    if (created) {
        SFile::RebuildHash();
    }
}

// PROVEN: 2a85d4a5 IDENTICAL/IDENTICAL
// OFFSET: 0x009C8530/0x140756EA0
void CUFProfileInfo::FromJam(const JamCliCUFProfile& jam) {
    SStrCopy(this->m_name, jam.m_name, sizeof(this->m_name));
    this->m_options.m_frameHeight = jam.m_frameHeight;
    this->m_options.m_frameWidth = jam.m_frameWidth;
    this->m_options.m_sortBy = jam.m_sortBy;
    this->m_options.m_healthText = jam.m_healthText;
    this->m_options.m_keepGroupsTogether = jam.m_keepGroupsTogether;
    this->m_options.m_displayPets = jam.m_displayPets;
    this->m_options.m_displayMainTankAndAssist = jam.m_displayMainTankAndAssist;
    this->m_options.m_displayHealPrediction = jam.m_displayHealPrediction;
    this->m_options.m_displayAggroHighlight = jam.m_displayAggroHighlight;
    this->m_options.m_displayOnlyDispellableDebuffs = jam.m_displayOnlyDispellableDebuffs;
    this->m_options.m_displayPowerBar = jam.m_displayPowerBar;
    this->m_options.m_displayBorder = jam.m_displayBorder;
    this->m_options.m_useClassColors = jam.m_useClassColors;
    this->m_options.m_horizontalGroups = jam.m_horizontalGroups;
    this->m_options.m_displayNonBossDebuffs = jam.m_displayNonBossDebuffs;
    this->m_position.m_dynamic = jam.m_dynamicPosition;
    this->m_position.m_topPoint = jam.m_topPoint;
    this->m_position.m_bottomPoint = jam.m_bottomPoint;
    this->m_position.m_leftPoint = jam.m_leftPoint;
    this->m_position.m_topOffset = jam.m_topOffset;
    this->m_position.m_bottomOffset = jam.m_bottomOffset;
    this->m_position.m_leftOffset = jam.m_leftOffset;
    this->m_state.m_locked = jam.m_locked;
    this->m_state.m_shown = jam.m_shown;
    this->m_autoActivate.m_2Players = jam.m_autoActivate2Players;
    this->m_autoActivate.m_3Players = jam.m_autoActivate3Players;
    this->m_autoActivate.m_5Players = jam.m_autoActivate5Players;
    this->m_autoActivate.m_10Players = jam.m_autoActivate10Players;
    this->m_autoActivate.m_15Players = jam.m_autoActivate15Players;
    this->m_autoActivate.m_25Players = jam.m_autoActivate25Players;
    this->m_autoActivate.m_40Players = jam.m_autoActivate40Players;
    this->m_autoActivate.m_spec1 = jam.m_autoActivateSpec1;
    this->m_autoActivate.m_spec2 = jam.m_autoActivateSpec2;
    this->m_autoActivate.m_pve = jam.m_autoActivatePvE;
    this->m_autoActivate.m_pvp = jam.m_autoActivatePvP;
}

// OFFSET: 0x009C89A0/0x140757370
JamCliCUFProfile* CUFProfileInfo::ToJam(JamCliCUFProfile* jam) {
    jam->m_fieldMask |= 0x1;
    SStrCopy(jam->m_name, this->m_name, sizeof(jam->m_name));
    jam->m_fieldMask |= 0x2;
    jam->m_frameHeight = this->m_options.m_frameHeight;
    jam->m_fieldMask |= 0x4;
    jam->m_frameWidth = this->m_options.m_frameWidth;
    jam->m_fieldMask |= 0x8;
    jam->m_sortBy = this->m_options.m_sortBy;
    jam->m_fieldMask |= 0x10;
    jam->m_healthText = this->m_options.m_healthText;
    jam->m_fieldMask |= 0x20;
    jam->m_keepGroupsTogether = this->m_options.m_keepGroupsTogether;
    jam->m_fieldMask |= 0x40;
    jam->m_displayPets = this->m_options.m_displayPets != 0;
    jam->m_fieldMask |= 0x80;
    jam->m_displayMainTankAndAssist = this->m_options.m_displayMainTankAndAssist != 0;
    jam->m_fieldMask |= 0x100;
    jam->m_displayHealPrediction = this->m_options.m_displayHealPrediction != 0;
    jam->m_fieldMask |= 0x200;
    jam->m_displayAggroHighlight = this->m_options.m_displayAggroHighlight != 0;
    jam->m_fieldMask |= 0x400;
    jam->m_displayOnlyDispellableDebuffs = this->m_options.m_displayOnlyDispellableDebuffs != 0;
    jam->m_fieldMask |= 0x800;
    jam->m_displayPowerBar = this->m_options.m_displayPowerBar != 0;
    jam->m_fieldMask |= 0x1000;
    jam->m_displayBorder = this->m_options.m_displayBorder != 0;
    jam->m_fieldMask |= 0x2000;
    jam->m_useClassColors = this->m_options.m_useClassColors;
    jam->m_fieldMask |= 0x4000;
    jam->m_horizontalGroups = this->m_options.m_horizontalGroups != 0;
    jam->m_fieldMask |= 0x8000;
    jam->m_displayNonBossDebuffs = this->m_options.m_displayNonBossDebuffs != 0;
    jam->m_fieldMask |= 0x10000;
    jam->m_dynamicPosition = this->m_position.m_dynamic;
    jam->m_fieldMask |= 0x20000;
    jam->m_topPoint = this->m_position.m_topPoint != 0;
    jam->m_fieldMask |= 0x40000;
    jam->m_bottomPoint = this->m_position.m_bottomPoint != 0;
    jam->m_fieldMask |= 0x80000;
    jam->m_leftPoint = this->m_position.m_leftPoint;
    jam->m_fieldMask |= 0x100000;
    jam->m_topOffset = this->m_position.m_topOffset;
    jam->m_fieldMask |= 0x200000;
    jam->m_bottomOffset = this->m_position.m_bottomOffset;
    jam->m_fieldMask |= 0x400000;
    jam->m_leftOffset = this->m_position.m_leftOffset;
    jam->m_fieldMask |= 0x800000;
    jam->m_locked = this->m_state.m_locked;
    jam->m_fieldMask |= 0x1000000;
    jam->m_shown = this->m_state.m_shown != 0;
    jam->m_fieldMask |= 0x2000000;
    jam->m_autoActivate2Players = this->m_autoActivate.m_2Players;
    jam->m_fieldMask |= 0x4000000;
    jam->m_autoActivate3Players = this->m_autoActivate.m_3Players != 0;
    jam->m_fieldMask |= 0x8000000;
    jam->m_autoActivate5Players = this->m_autoActivate.m_5Players != 0;
    jam->m_fieldMask |= 0x10000000;
    jam->m_autoActivate10Players = this->m_autoActivate.m_10Players != 0;
    jam->m_fieldMask |= 0x20000000;
    jam->m_autoActivate15Players = this->m_autoActivate.m_15Players != 0;
    jam->m_fieldMask |= 0x40000000;
    jam->m_autoActivate25Players = this->m_autoActivate.m_25Players != 0;
    jam->m_fieldMask |= 0x80000000;
    jam->m_autoActivate40Players = this->m_autoActivate.m_40Players != 0;
    jam->m_fieldMask2 |= 0x1;
    jam->m_autoActivateSpec1 = this->m_autoActivate.m_spec1 != 0;
    jam->m_fieldMask2 |= 0x2;
    jam->m_autoActivateSpec2 = this->m_autoActivate.m_spec2;
    jam->m_fieldMask2 |= 0x4;
    jam->m_autoActivatePvP = this->m_autoActivate.m_pvp != 0;
    jam->m_fieldMask2 |= 0x8;
    jam->m_autoActivatePvE = this->m_autoActivate.m_pve != 0;

    return jam;
}

// PROVEN: e6e040a0 IDENTICAL/IDENTICAL
// OFFSET: 0x009C8DC0/0x140757920
CUFProfileInfo* CGCUFProfiles::GetProfile(const char* name) {
    PROFILENODE* node = CGCUFProfiles::s_profileHash.Ptr(name);

    return node ? node->m_profile : NULL;
}

// OFFSET: 0x009C8DE0/0x140757950
bool CGCUFProfiles::HasUnsavedChanges() {
    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(CGCUFProfiles::s_savedName);

    if (profile) {
        return memcmp(&profile->m_options, &CGCUFProfiles::s_savedOptions, sizeof(CUFFRAMEOPTIONS)) != 0 || memcmp(&CGCUFProfiles::s_savedAutoActivate, &profile->m_autoActivate, sizeof(CUFAUTOACTIVATE)) != 0;
    }

    return false;
}

// OFFSET: 0x009C8E50/0x1407579B0
void CGCUFProfiles::SendProfiles(bool logout) {
    static ThrottleObject s_throttle(2, 10);
    static ThrottleObject s_logoutThrottle(2, 20);

    ThrottleObject* throttle = logout ? &s_logoutThrottle : &s_throttle;

    if (!throttle->IncrementAndCheck()) {
        return;
    }

    UserClientSaveCufProfiles msg;

    for (uint32_t i = 0; i < CGCUFProfiles::s_profiles.Count() && i < 5; i++) {
        CGCUFProfiles::s_profiles[i]->ToJam(msg.m_profiles.New());
    }

    ClientServices::Send(&msg);
    CGCUFProfiles::s_dirty = false;
}

// OFFSET: 0x009C8F80/0x140757B60
void CGCUFProfiles::PushOptions(CUFProfileInfo* profile, lua_State* L, int32_t table, int32_t index) {
    for (OPTIONENTRY* entry = CGCUFProfiles::s_options[table].Head(); entry; entry = CGCUFProfiles::s_options[table].Next(entry)) {
        lua_pushstring(L, entry->m_key.m_str);
        entry->m_get(profile, L);
        lua_settable(L, index - 2);
    }
}

// OFFSET: 0x009C9930/0x140758780
void CGCUFProfiles::DeleteProfile(CUFProfileInfo* profile) {
    for (uint32_t i = 0; i < CGCUFProfiles::s_profiles.Count(); i++) {
        if (CGCUFProfiles::s_profiles[i] == profile) {
            if (i != CGCUFProfiles::s_profiles.Count() - 1) {
                CGCUFProfiles::s_profiles[i] = CGCUFProfiles::s_profiles[CGCUFProfiles::s_profiles.Count() - 1];
            }

            CGCUFProfiles::s_profiles.m_count--;
            break;
        }
    }

    PROFILENODE* node = CGCUFProfiles::s_profileHash.Ptr(profile->m_name);

    if (node) {
        CGCUFProfiles::s_profileHash.Delete(node);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }

    delete profile;
    CGCUFProfiles::s_dirty = true;
}

// OFFSET: 0x009C99D0/0x1407588E0
void CGCUFProfiles::SaveProfileCopy(CUFProfileInfo* profile) {
    CGCUFProfiles::s_savedOptions = profile->m_options;
    CGCUFProfiles::s_savedAutoActivate = profile->m_autoActivate;
    SStrCopy(CGCUFProfiles::s_savedName, profile->m_name, sizeof(CGCUFProfiles::s_savedName));

    if (CGCUFProfiles::s_dirty) {
        CGCUFProfiles::SendProfiles(false);
    }
}

// PROVEN: 7210c3e3 IDENTICAL/DIFFERENT
// OFFSET: 0x009C9A40/0x140758BC0
void CGCUFProfiles::RestoreProfileCopy() {
    CUFProfileInfo* profile = CGCUFProfiles::GetProfile(CGCUFProfiles::s_savedName);

    if (profile && CGCUFProfiles::HasUnsavedChanges()) {
        profile->m_options = CGCUFProfiles::s_savedOptions;
        profile->m_autoActivate = CGCUFProfiles::s_savedAutoActivate;
        CGCUFProfiles::s_dirty = true;
    }
}

// OFFSET: 0x009C9AA0/0x140758960
void CGCUFProfiles::ShutdownGame() {
    CGCUFProfiles::SaveProfiles();
    CGCUFProfiles::s_profileHash.Clear();

    for (uint32_t i = 0; i < CGCUFProfiles::s_profiles.Count(); i++) {
        delete CGCUFProfiles::s_profiles[i];
    }

    CGCUFProfiles::s_profiles.Clear();

    for (uint32_t i = 0; i < 4; i++) {
        CGCUFProfiles::s_options[i].Clear();
    }

    ClientLoadCufProfiles::s_handler = NULL;
}

// OFFSET: 0x009CA980/0x14075A060
void CGCUFProfiles::AddProfile(CUFProfileInfo* profile) {
    *CGCUFProfiles::s_profiles.New() = profile;

    uint32_t hashval = SStrHashHT(profile->m_name);
    PROFILENODE* node = CGCUFProfiles::s_profileHash.InternalNewNode(hashval, 0, 0);
    node->m_hashval = hashval;
    node->m_profile = profile;
    node->m_key.m_str = profile->m_name;
}

// OFFSET: 0x009CAA60
bool CGCUFProfiles::LoadProfilesHandler(ClientLoadCufProfiles* msg) {
    CGCUFProfiles::s_loaded = true;

    if (static_cast<int32_t>(CGCUFProfiles::s_profiles.Count()) <= 0) {
        for (uint32_t i = 0; i < msg->m_profiles.m_count; i++) {
            CUFProfileInfo* profile = new (__FILE__, __LINE__) CUFProfileInfo();
            profile->FromJam(msg->m_profiles.m_data[i]);
            CGCUFProfiles::AddProfile(profile);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_COMPACT_UNIT_FRAME_PROFILES_LOADED, NULL);
    }

    return true;
}

// OFFSET: 0x009CAAE0/0x14075A300
void CGCUFProfiles::CreateProfile(const char* name, CUFProfileInfo* base) {
    CUFProfileInfo* profile = new (__FILE__, __LINE__) CUFProfileInfo(name);

    if (base) {
        profile->CopySettings(base);
    }

    CGCUFProfiles::AddProfile(profile);
    CGCUFProfiles::s_dirty = true;
}

// OFFSET: 0x009CAB40/0x14075A3A0
void CGCUFProfiles::InitializeGame() {
    CGCUFProfiles::s_savedName[0] = 0;
    CGCUFProfiles::s_dirty = false;
    CGCUFProfiles::s_loaded = false;

    {
        uint32_t hashval = SStrHashHT("frameHeight");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "frameHeight";
        entry->m_get = &GetOptionFrameHeight;
        entry->m_set = &SetOptionFrameHeight;
    }

    {
        uint32_t hashval = SStrHashHT("frameWidth");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "frameWidth";
        entry->m_get = &GetOptionFrameWidth;
        entry->m_set = &SetOptionFrameWidth;
    }

    {
        uint32_t hashval = SStrHashHT("sortBy");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "sortBy";
        entry->m_get = &GetOptionSortBy;
        entry->m_set = &SetOptionSortBy;
    }

    {
        uint32_t hashval = SStrHashHT("healthText");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "healthText";
        entry->m_get = &GetOptionHealthText;
        entry->m_set = &SetOptionHealthText;
    }

    {
        uint32_t hashval = SStrHashHT("keepGroupsTogether");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "keepGroupsTogether";
        entry->m_get = &GetOptionKeepGroupsTogether;
        entry->m_set = &SetOptionKeepGroupsTogether;
    }

    {
        uint32_t hashval = SStrHashHT("displayPets");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayPets";
        entry->m_get = &GetOptionDisplayPets;
        entry->m_set = &SetOptionDisplayPets;
    }

    {
        uint32_t hashval = SStrHashHT("displayMainTankAndAssist");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayMainTankAndAssist";
        entry->m_get = &GetOptionDisplayMainTankAndAssist;
        entry->m_set = &SetOptionDisplayMainTankAndAssist;
    }

    {
        uint32_t hashval = SStrHashHT("displayHealPrediction");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayHealPrediction";
        entry->m_get = &GetOptionDisplayHealPrediction;
        entry->m_set = &SetOptionDisplayHealPrediction;
    }

    {
        uint32_t hashval = SStrHashHT("displayAggroHighlight");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayAggroHighlight";
        entry->m_get = &GetOptionDisplayAggroHighlight;
        entry->m_set = &SetOptionDisplayAggroHighlight;
    }

    {
        uint32_t hashval = SStrHashHT("displayOnlyDispellableDebuffs");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayOnlyDispellableDebuffs";
        entry->m_get = &GetOptionDisplayOnlyDispellableDebuffs;
        entry->m_set = &SetOptionDisplayOnlyDispellableDebuffs;
    }

    {
        uint32_t hashval = SStrHashHT("displayPowerBar");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayPowerBar";
        entry->m_get = &GetOptionDisplayPowerBar;
        entry->m_set = &SetOptionDisplayPowerBar;
    }

    {
        uint32_t hashval = SStrHashHT("displayBorder");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayBorder";
        entry->m_get = &GetOptionDisplayBorder;
        entry->m_set = &SetOptionDisplayBorder;
    }

    {
        uint32_t hashval = SStrHashHT("useClassColors");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "useClassColors";
        entry->m_get = &GetOptionUseClassColors;
        entry->m_set = &SetOptionUseClassColors;
    }

    {
        uint32_t hashval = SStrHashHT("displayNonBossDebuffs");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "displayNonBossDebuffs";
        entry->m_get = &GetOptionDisplayNonBossDebuffs;
        entry->m_set = &SetOptionDisplayNonBossDebuffs;
    }

    {
        uint32_t hashval = SStrHashHT("horizontalGroups");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[0].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "horizontalGroups";
        entry->m_get = &GetOptionHorizontalGroups;
        entry->m_set = &SetOptionHorizontalGroups;
    }

    {
        uint32_t hashval = SStrHashHT("locked");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[2].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "locked";
        entry->m_get = &GetOptionLocked;
        entry->m_set = &SetOptionLocked;
    }

    {
        uint32_t hashval = SStrHashHT("shown");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[2].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "shown";
        entry->m_get = &GetOptionShown;
        entry->m_set = &SetOptionShown;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate2Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate2Players";
        entry->m_get = &GetOptionAutoActivate2Players;
        entry->m_set = &SetOptionAutoActivate2Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate3Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate3Players";
        entry->m_get = &GetOptionAutoActivate3Players;
        entry->m_set = &SetOptionAutoActivate3Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate5Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate5Players";
        entry->m_get = &GetOptionAutoActivate5Players;
        entry->m_set = &SetOptionAutoActivate5Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate10Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate10Players";
        entry->m_get = &GetOptionAutoActivate10Players;
        entry->m_set = &SetOptionAutoActivate10Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate15Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate15Players";
        entry->m_get = &GetOptionAutoActivate15Players;
        entry->m_set = &SetOptionAutoActivate15Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate25Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate25Players";
        entry->m_get = &GetOptionAutoActivate25Players;
        entry->m_set = &SetOptionAutoActivate25Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivate40Players");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivate40Players";
        entry->m_get = &GetOptionAutoActivate40Players;
        entry->m_set = &SetOptionAutoActivate40Players;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivateSpec1");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivateSpec1";
        entry->m_get = &GetOptionAutoActivateSpec1;
        entry->m_set = &SetOptionAutoActivateSpec1;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivateSpec2");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivateSpec2";
        entry->m_get = &GetOptionAutoActivateSpec2;
        entry->m_set = &SetOptionAutoActivateSpec2;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivatePvP");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivatePvP";
        entry->m_get = &GetOptionAutoActivatePvP;
        entry->m_set = &SetOptionAutoActivatePvP;
    }

    {
        uint32_t hashval = SStrHashHT("autoActivatePvE");
        CGCUFProfiles::OPTIONENTRY* entry = CGCUFProfiles::s_options[3].InternalNewNode(hashval, 0, 0);
        entry->m_hashval = hashval;
        entry->m_key.m_str = "autoActivatePvE";
        entry->m_get = &GetOptionAutoActivatePvE;
        entry->m_set = &SetOptionAutoActivatePvE;
    }
    ClientLoadCufProfiles::s_handler = JamHandlerEncode(&CGCUFProfiles::LoadProfilesHandler, g_jamHandlerKey_CFCF68);
}
