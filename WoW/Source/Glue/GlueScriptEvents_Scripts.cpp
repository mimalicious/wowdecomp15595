#include "GlueMgr.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Console/ConsoleClient.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Event/Event.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/Ui/SharedScripts.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Scene.h"

int32_t Script_GetLocale(lua_State* L);
int32_t Script_PlayCreditsMusic(lua_State* L);
int32_t Script_PatchDownloadProgress(lua_State* L);
int32_t Script_GetAddOnEnableState(lua_State* L);
int32_t Script_ResetAddOns(lua_State* L);
int32_t Script_GetBillingTimeRemaining(lua_State* L);
int32_t Script_GetBillingPlan(lua_State* L);
int32_t Script_GetBillingTimeRested(lua_State* L);
int32_t Script_IsLinuxClient(lua_State* L);
int32_t Script_RestoreVideoResolutionDefaults(lua_State* L);
int32_t Script_RestoreVideoEffectsDefaults(lua_State* L);

// PROVEN: 4ad51880 DIFFERENT/IDENTICAL
// OFFSET: 0x00807380/0x14051CB90
static int32_t Script_GetBuildInfo(lua_State* L) {
    lua_pushstring(L, FrameScript_GetText("VERSION", -1, GENDER_NOT_APPLICABLE));

    const char* release = FrameScript_GetText("RELEASE_BUILD", -1, GENDER_NOT_APPLICABLE);
    const char* platform = FrameScript_GetText(sizeof(void*) == 8 ? "X64_BUILD" : "X86_BUILD", -1, GENDER_NOT_APPLICABLE);
    size_t size = strlen(platform) + strlen(release) + 2;
    char* build = static_cast<char*>(_alloca(size));
    SStrPrintf(build, size, "%s%s%s", release, *platform ? " " : "", platform);
    lua_pushstring(L, build);

    lua_pushstring(L, "4.3.4");
    lua_pushstring(L, "15595");
    lua_pushstring(L, "Apr 10 2012");
    return 5;
}

// PROVEN: 738cfcb2 IDENTICAL/IDENTICAL
// OFFSET: 0x00805B50/0x14051B100
static int32_t Script_SetSavedAccountName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetSavedAccountName(\"accountName\")");
        return 0;
    }

    g_accountNameVar->Set(lua_tolstring(L, 1, NULL), true, false, false, true);
    return 0;
}

// PROVEN: 2bb9c52f IDENTICAL/IDENTICAL
// OFFSET: 0x00805BA0/0x14051B170
static int32_t Script_GetUsesToken(lua_State* L) {
    lua_pushboolean(L, g_accountUsesTokenVar->m_intValue);
    return 1;
}

// PROVEN: 55697f40 IDENTICAL/IDENTICAL
// OFFSET: 0x00805BC0/0x14051B190
static int32_t Script_SetUsesToken(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetUsesToken( 0 | 1 )");
        return 0;
    }

    g_accountUsesTokenVar->Set(lua_toboolean(L, 1), true, false, false, true);
    return 0;
}

// PROVEN: 59b6afbb IDENTICAL/IDENTICAL
// OFFSET: 0x00805C30/0x14051B210
static int32_t Script_SetSavedAccountList(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetSavedAccountList(\"accountList\")");
        return 0;
    }

    g_accountListVar->Set(lua_tolstring(L, 1, NULL), true, false, false, true);
    return 0;
}

// PROVEN: d92c203f IDENTICAL/IDENTICAL
// OFFSET: 0x00805C80/0x14051B2B0
static int32_t Script_SetCurrentScreen(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCurrentScreen(\"screen\")");
        return 0;
    }

    CGlueMgr::UpdateCurrentScreen(lua_tolstring(L, 1, NULL));

    if (!SFile::IsStreamingMode()) {
        AsyncFileReadWaitAll();
    }

    return 0;
}

// PROVEN: c2ae06c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00805CD0/0x14051B310
static int32_t Script_QuitGame(lua_State* L) {
    CGlueMgr::QuitGame();
    return 0;
}

// PROVEN: 5e4941cd IDENTICAL/IDENTICAL
// OFFSET: 0x00805CF0/0x14051B330
static int32_t Script_PlayGlueMusic(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PlayGlueMusic(\"filename\")");
        return 0;
    }

    SI3::StartGlueMusic(lua_tolstring(L, 1, NULL), -1);
    return 0;
}

// OFFSET: 0x00805D40/0x14051B380
static int32_t Script_PlayGlueAmbience(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PlayGlueAmbience(\"sound name\", \"(optional)fade in time\")");
        return 0;
    }

    SI3::StartGlueAmbience(lua_tolstring(L, 1, NULL), lua_isnumber(L, 2) ? static_cast<int32_t>(static_cast<float>(lua_tonumber(L, 2)) * 1000.0f) : -1);
    return 0;
}

// PROVEN: 7c232824 IDENTICAL/IDENTICAL
// OFFSET: 0x00805DB0/0x14051B430
static int32_t Script_StopGlueAmbience(lua_State* L) {
    SI3::StopGlueAmbience(1000);
    return 0;
}

// PROVEN: bf2aaa45 IDENTICAL/IDENTICAL
// OFFSET: 0x00805E10/0x14051B4A0
static int32_t Script_StopGlueMusic(lua_State* L) {
    SI3::StopCreditsMusic(-1);
    SI3::StopGlueMusic(-1);
    return 0;
}

// PROVEN: daa1212f IDENTICAL/IDENTICAL
// OFFSET: 0x00805E80/0x14051B510
static int32_t Script_AcceptTOS(lua_State* L) {
    CGlueMgr::AcceptTOS();
    return 0;
}

// PROVEN: c327884b IDENTICAL/IDENTICAL
// OFFSET: 0x00805E90/0x14051B520
static int32_t Script_AcceptEULA(lua_State* L) {
    CGlueMgr::AcceptEULA();
    return 0;
}

// PROVEN: b29b1181 IDENTICAL/IDENTICAL
// OFFSET: 0x00805EA0/0x14051B530
static int32_t Script_AcceptTerminationWithoutNotice(lua_State* L) {
    CGlueMgr::AcceptTerminationWithoutNotice();
    return 0;
}

// PROVEN: e8c67eed IDENTICAL/IDENTICAL
// OFFSET: 0x00805EB0/0x14051B540
static int32_t Script_AcceptScanning(lua_State* L) {
    CGlueMgr::AcceptScanning();
    return 0;
}

// PROVEN: 1a1e96e0 IDENTICAL/IDENTICAL
// OFFSET: 0x00805EC0/0x14051B550
static int32_t Script_AcceptContest(lua_State* L) {
    CGlueMgr::AcceptContest();
    return 0;
}

// PROVEN: e0fedecb IDENTICAL/DIFFERENT
// OFFSET: 0x00805ED0/0x14051B560
static int32_t Script_DefaultServerLogin(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: DefaultServerLogin(\"accountName\", \"password\")");
        return 0;
    }

    CGlueMgr::LoginServerLogin(lua_tolstring(L, 1, NULL), const_cast<char*>(lua_tolstring(L, 2, NULL)));
    return 0;
}

// PROVEN: 37edd67f IDENTICAL/DIFFERENT
// OFFSET: 0x00806160/0x14051B7F0
static int32_t Script_StatusDialogClick(lua_State* L) {
    CGlueMgr::StatusDialogClick();
    return 0;
}

// PROVEN: 27a12112 IDENTICAL/IDENTICAL
// OFFSET: 0x008066C0/0x14051BDB0
static int32_t Script_DisableAllAddOns(lua_State* L) {
    const char* character = lua_tolstring(L, 1, NULL);

    for (uint32_t i = 0; i < GetNumAddOns(); i++) {
        SetAddOnEnableState(GetAddOnByIndex(i), character, 0);
    }

    return 0;
}

// OFFSET: 0x008067B0/0x140631ED0
static int32_t Script_GetCursorPosition(lua_State* L) {
    float x = 0.0f;
    float y = 0.0f;
    NDCToDDC(CSimpleTop::s_instance->m_mousePosition.m_x, CSimpleTop::s_instance->m_mousePosition.m_y, &x, &y);

    lua_pushnumber(L, DDCToNDCWidth(x * (CoordinateGetAspectCompensation() * 1024.0f)));
    lua_pushnumber(L, DDCToNDCWidth(y * (CoordinateGetAspectCompensation() * 1024.0f)));
    return 2;
}

// PROVEN: fd7f6624 IDENTICAL/IDENTICAL
// OFFSET: 0x00806840/0x14051BEC0
static int32_t Script_GetScreenWidth(lua_State* L) {
    float width = NDCToDDCWidth(1.0f);
    lua_pushnumber(L, DDCToNDCWidth(width * (CoordinateGetAspectCompensation() * 1024.0f)));
    return 1;
}

// PROVEN: abbd9751 IDENTICAL/IDENTICAL
// OFFSET: 0x00806890/0x14051BF20
static int32_t Script_GetScreenHeight(lua_State* L) {
    float height = NDCToDDCHeight(1.0f);
    lua_pushnumber(L, DDCToNDCWidth(height * (CoordinateGetAspectCompensation() * 1024.0f)));
    return 1;
}

// PROVEN: 7d1bd4eb IDENTICAL/DIFFERENT
// OFFSET: 0x00806910/0x14051BFB0
static int32_t Script_ScanDLLStart(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ScanDLLStart(\"VersionURL\", \"DLLURL\")");
        return 0;
    }

    CGlueMgr::ScanDLLStart(lua_tolstring(L, 1, NULL), lua_tolstring(L, 2, NULL));
    return 0;
}

// PROVEN: de59281a IDENTICAL/IDENTICAL
// OFFSET: 0x00806A20/0x14051C0F0
static int32_t Script_ScanDLLContinueAnyway(lua_State* L) {
    CGlueMgr::ScanDLLContinueAnyway();
    return 0;
}

// PROVEN: 3b6cdf48 IDENTICAL/IDENTICAL
// OFFSET: 0x00806A30/0x14051C100
static int32_t Script_IsScanDLLFinished(lua_State* L) {
    if (CGlueMgr::IsScanDLLFinished()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: f98ae376 IDENTICAL/IDENTICAL
// OFFSET: 0x00806A80/0x14051C150
static int32_t Script_SetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCVar(\"cvar\", value [, \"scriptCvar\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    CVar* var = CVar::LookupRegistered(name);

    if (!var) {
        luaL_error(L, "Couldn't find CVar named '%s'", name);
        return 0;
    }

    if (static_cast<uint8_t>(var->m_flags >> 2) & 1) {
        luaL_error(L, "\"%s\" is read-only", name);
        return 0;
    }

    const char* value = lua_tolstring(L, 2, NULL);

    if (!value) {
        value = "0";
    }

    var->Set(value, true, false, false, true);
    return 0;
}

// PROVEN: ed8058ad IDENTICAL/IDENTICAL
// OFFSET: 0x00806CD0/0x14051C420
static int32_t Script_ShowChangedOptionWarnings(lua_State* L) {
    if (CGlueMgr::ShowChangedOptionWarnings()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 5f824f86 IDENTICAL/IDENTICAL
// OFFSET: 0x00807110/0x14051C8D0
static int32_t Script_StopAllSFX(lua_State* L) {
    SE3::StopAllSounds(static_cast<SE3BUS>(0), static_cast<int32_t>(static_cast<float>(lua_tonumber(L, 1)) * 1000.0f));
    return 0;
}

// PROVEN: 88ecb7a9 IDENTICAL/IDENTICAL
// OFFSET: 0x00807190/0x140632170
int32_t Script_RestartGx(lua_State* L) {
    ConsoleCommandExecute("gxRestart", 1);
    return 0;
}

// PROVEN: 8fcd73d1 IDENTICAL/IDENTICAL
// OFFSET: 0x008071C0/0x14051C950
static int32_t Script_IsStreamingTrial(lua_State* L) {
    return SFile::IsTrial();
}

// PROVEN: feee0aac IDENTICAL/IDENTICAL
// OFFSET: 0x00807250/0x14051CA30
static int32_t Script_IsConsoleActive(lua_State* L) {
    if (ConsoleGetActive()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 73f01f04 IDENTICAL/IDENTICAL
// OFFSET: 0x00807290/0x14051CA70
static int32_t Script_RunScript(lua_State* L) {
    if (lua_isstring(L, 1)) {
        const char* script = lua_tolstring(L, 1, NULL);

        if (script && *script) {
            FrameScript_Execute(script, script, NULL);
        }
    }

    return 0;
}

// PROVEN: 279f1667 IDENTICAL/IDENTICAL
// OFFSET: 0x008072D0/0x14051CAC0
static int32_t Script_IsSystemSupported(lua_State* L) {
    if (OsGetProcessorFeatures() & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 3927c850 IDENTICAL/IDENTICAL
// OFFSET: 0x00805B30/0x14051B0E0
static int32_t Script_GetSavedAccountName(lua_State* L) {
    lua_pushstring(L, g_accountNameVar->m_stringValue.m_str);
    return 1;
}

// PROVEN: 7d2dc8ef IDENTICAL/IDENTICAL
// OFFSET: 0x00805C10/0x14051B1F0
static int32_t Script_GetSavedAccountList(lua_State* L) {
    lua_pushstring(L, g_accountListVar->m_stringValue.m_str);
    return 1;
}

// PROVEN: 79e4c70b IDENTICAL/IDENTICAL
// OFFSET: 0x00807460/0x14051CCC0
static int32_t Script_ShowTOSNotice(lua_State* L) {
    if (CGlueMgr::m_rereadTOS) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 6aca59f0 IDENTICAL/IDENTICAL
// OFFSET: 0x008074A0/0x14051CD00
static int32_t Script_TOSAccepted(lua_State* L) {
    if (CGlueMgr::m_acceptedTOS) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 93680450 IDENTICAL/IDENTICAL
// OFFSET: 0x008074E0/0x14051CD40
static int32_t Script_ShowEULANotice(lua_State* L) {
    if (CGlueMgr::m_rereadEULA) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 17304d36 IDENTICAL/IDENTICAL
// OFFSET: 0x00807520/0x14051CD80
static int32_t Script_EULAAccepted(lua_State* L) {
    if (CGlueMgr::m_acceptedEULA) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 14d5c2b6 IDENTICAL/IDENTICAL
// OFFSET: 0x00807560/0x14051CDC0
static int32_t Script_ShowTerminationWithoutNoticeNotice(lua_State* L) {
    if (CGlueMgr::m_rereadTerminationWithoutNotice) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 770ab687 IDENTICAL/IDENTICAL
// OFFSET: 0x008075A0/0x14051CE00
static int32_t Script_TerminationWithoutNoticeAccepted(lua_State* L) {
    if (CGlueMgr::m_acceptedTerminationWithoutNotice) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 90a47247 IDENTICAL/IDENTICAL
// OFFSET: 0x008075E0/0x14051CE40
static int32_t Script_ShowScanningNotice(lua_State* L) {
    if (CGlueMgr::m_rereadScanning) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 33b7528e IDENTICAL/IDENTICAL
// OFFSET: 0x00807620/0x14051CE80
static int32_t Script_ScanningAccepted(lua_State* L) {
    if (CGlueMgr::m_acceptedScanning) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 435b32b0 IDENTICAL/IDENTICAL
// OFFSET: 0x00807660/0x14051CEC0
static int32_t Script_ShowContestNotice(lua_State* L) {
    if (CGlueMgr::m_rereadContest) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 452c9251 IDENTICAL/IDENTICAL
// OFFSET: 0x008076A0/0x14051CF00
static int32_t Script_ContestAccepted(lua_State* L) {
    if (CGlueMgr::m_acceptedContest) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 6c717d2f IDENTICAL/UNPROVEN
// OFFSET: 0x008076E0/0x14051CF40
static int32_t Script_GetServerName(lua_State* L) {
    lua_pushstring(L, ClientServices::GetSelectedRealmName());

    int32_t pvp = 0;
    int32_t rp = 0;
    int32_t down = 1;
    REALM_INFO* realm = ClientServices::GetSelectedRealm();

    if (realm) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(g_cfg_ConfigsDB.m_numRecords); i++) {
            Cfg_ConfigsRec* config = g_cfg_ConfigsDB.GetRecordByIndex(i);

            if (config->m_realmType == realm->m_type) {
                pvp = config->m_playerKillingAllowed != 0;
                rp = config->m_roleplaying != 0;
                down = realm->m_flags & 0x2;
                break;
            }
        }
    }

    if (pvp) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (rp) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// PROVEN: 1f1893b0 IDENTICAL/IDENTICAL
// OFFSET: 0x00806B30/0x14051C250
static int32_t Script_GetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVar(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, NULL));
        return 0;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    return 1;
}

// PROVEN: bcb00696 IDENTICAL/IDENTICAL
// OFFSET: 0x00806C30/0x14051C370
static int32_t Script_GetCVarDefault(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarDefault(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, NULL));
        return 0;
    }

    if (var->m_defaultValue.m_str) {
        lua_pushstring(L, var->m_defaultValue.m_str);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 6086afa6 IDENTICAL/UNPROVEN
// OFFSET: 0x00807AF0/0x14051D360
static int32_t Script_GetChangedOptionWarnings(lua_State* L) {
    uint32_t count = CGlueMgr::m_numChangedOptionWarnings;

    if (count) {
        lua_checkstack(L, count);

        for (uint32_t i = 0; i < count; i++) {
            const char* warning = CGlueMgr::m_changedOptionWarnings[i];
            lua_pushlstring(L, warning, SStrLen(warning));
        }

        return count;
    }

    return 0;
}

// OFFSET: 0x008C3780/0x140633280
int32_t Script_GetMovieResolution(lua_State* L) {
    CVar* resolution = CVar::Lookup("gxResolution");

    if (!resolution || !resolution->m_stringValue.m_str) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    int32_t width;
    int32_t height;
    sscanf(resolution->m_stringValue.m_str, "%dx%d", &width, &height);
    lua_pushnumber(L, width);

    return 1;
}

// PROVEN: 53b4c8e1 IDENTICAL/IDENTICAL
// OFFSET: 0x00807970/0x14051D1F0
static int32_t Script_ShowCursor(lua_State* L) {
    g_theGxDevicePtr->CursorSetVisible(1);
    return 0;
}

// PROVEN: fa9ddfd8 IDENTICAL/IDENTICAL
// OFFSET: 0x00807990/0x14051D210
static int32_t Script_HideCursor(lua_State* L) {
    g_theGxDevicePtr->CursorSetVisible(0);

    return 0;
}

// PROVEN: 6467a37d IDENTICAL/DIFFERENT
// OFFSET: 0x00806BB0/0x14051C2E0
static int32_t Script_GetCVarBool(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarBool(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (var && StringToBOOL(var->m_stringValue.m_str)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: b87f7ab2 IDENTICAL/IDENTICAL
// OFFSET: 0x008077F0/0x14051D040
static int32_t Script_DisconnectFromServer(lua_State* L) {
    if (ClientServices::Connection()->unk_2F3C) {
        CGlueMgr::m_disconnectPending = 1;
        ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(14));
    }

    ClientServices::LoginConnection()->Logoff();
    return 0;
}

// PROVEN: 5e8b8225 IDENTICAL/IDENTICAL
// OFFSET: 0x00806170/0x14051B800
static int32_t Script_EnterWorld(lua_State* L) {
    CGlueMgr::EnterWorld();
    return 0;
}

// PROVEN: 45c188eb IDENTICAL/IDENTICAL
// OFFSET: 0x00807830/0x14051D090
static int32_t Script_IsConnectedToServer(lua_State* L) {
    if (ClientServices::Connection()->unk_2F3C) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00807D00/0x14051D640
static int32_t Script_RequestRealmSplitInfo(lua_State* L) {
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REALM_SPLIT));
    msg.Put(static_cast<uint32_t>(0xFFFFFFFF));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00807DB0/0x14051D720
static int32_t Script_ReadyForAccountDataTimes(lua_State* L) {
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_READY_FOR_ACCOUNT_DATA_TIMES));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// PROVEN: 03b2b9ea IDENTICAL/IDENTICAL
// OFFSET: 0x008C2A90/0x140632510
int32_t Script_GetNumAddOns(lua_State* L) {
    lua_pushnumber(L, GetNumAddOns());
    return 1;
}

// PROVEN: 70f2dae9 IDENTICAL/IDENTICAL
// OFFSET: 0x00806720/0x14051BE20
static int32_t Script_ResetAddOns(lua_State* L) {
    LoadAddOnEnableState(NULL);
    return 0;
}

// PROVEN: d4513791 IDENTICAL/IDENTICAL
// OFFSET: 0x008C2770/0x14051B280
int32_t Script_GetLocale(lua_State* L) {
    lua_pushstring(L, s_iniLocaleNames[s_locale]);
    return 1;
}

// PROVEN: f6b50669 IDENTICAL/IDENTICAL
// OFFSET: 0x00807870/0x14051D0E0
static int32_t Script_PatchDownloadProgress(lua_State* L) {
    lua_pushnumber(L, CGlueMgr::m_patchDownloadProgress);
    return 1;
}

// PROVEN: 436f7454 IDENTICAL/IDENTICAL
// OFFSET: 0x00805DC0/0x14051B450
static int32_t Script_PlayCreditsMusic(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PlayCreditsMusic( \"Sound kit name\" )");
        return 0;
    }

    SI3::StartCreditsMusic(lua_tolstring(L, 1, NULL));
    return 0;
}

// PROVEN: 021fddfd IDENTICAL/IDENTICAL
// OFFSET: 0x008079B0/0x14051D230
static int32_t Script_GetBillingTimeRemaining(lua_State* L) {
    uint32_t remaining = ClientServices::Connection()->m_billingTimeRemaining;

    if (ClientServices::Connection()->m_billingFlags & 0x2) {
        remaining = 0;
    }

    lua_pushnumber(L, remaining);
    return 1;
}

// PROVEN: 84b9b5b4 IDENTICAL/IDENTICAL
// OFFSET: 0x00807A00/0x14051D280
static int32_t Script_GetBillingPlan(lua_State* L) {
    uint8_t flags = ClientServices::Connection()->m_billingFlags;
    uint32_t plan = flags & 0x16;
    int32_t type = 0;

    if (plan == 0x2) {
        type = 1;
    } else if (plan == 0x4) {
        type = 2;
    } else if (!plan) {
        type = 3;
    } else if (plan == 0x10) {
        type = 4;
    }

    lua_pushnumber(L, type);
    lua_pushnumber(L, static_cast<uint8_t>(static_cast<uint8_t>(flags >> 5) & 1));
    lua_pushnumber(L, static_cast<uint8_t>(static_cast<uint8_t>(flags >> 3) & 1));
    return 3;
}

// PROVEN: 1d07beb1 IDENTICAL/IDENTICAL
// OFFSET: 0x00806180/0x14051B810
static int32_t Script_Screenshot(lua_State* L) {
    ScrnScreenshot(&CGlueMgr::HandleScreenshot, reinterpret_cast<uint8_t*>(ClientServices::GetClientStamp()), 88, NULL, NULL);
    return 0;
}

// PROVEN: 56e540c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00806420/0x14051BAD0
static int32_t Script_GetAddOnEnableState(lua_State* L) {
    if ((!lua_isstring(L, 1) && lua_type(L, 1)) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetAddOnEnableState(\"character\", index)");
        return 0;
    }

    const char* character = lua_tolstring(L, 1, NULL);
    const char* name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 2)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    lua_pushnumber(L, GetAddOnEnableState(name, character, true));
    return 1;
}

// PROVEN: 0eea43ea IDENTICAL/IDENTICAL
// OFFSET: 0x00805E30/0x14051B4C0
static int32_t Script_LaunchURL(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: LaunchURL(\"URL\")");
        return 0;
    }

    ClientServices::LaunchURL(lua_tolstring(L, 1, NULL));
    return 0;
}

// PROVEN: 26c5ccd2 IDENTICAL/IDENTICAL
// OFFSET: 0x00806980/0x14051C030
static int32_t Script_GetCreditsText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_GetCreditText(versionIndex)");
        return 0;
    }

    int32_t version = static_cast<int32_t>(lua_tonumber(L, 1));
    const char* filename;

    if (version == 1) {
        filename = "credits.html";
    } else if (version == 2) {
        filename = "credits_BC.html";
    } else if (version == 3) {
        filename = "credits_LK.html";
    } else {
        filename = "credits_CT.html";
    }

    void* text;

    if (SFile::ReadFile(NULL, filename, &text, NULL, 1, 1)) {
        lua_pushstring(L, static_cast<const char*>(text));
        SFile::Unload(text);
    }

    return 1;
}

// PROVEN: 6606612c IDENTICAL/IDENTICAL
// OFFSET: 0x00806130/0x14051B7B0
static int32_t Script_TokenEntered(lua_State* L) {
    if (lua_isstring(L, 1)) {
        CGlueMgr::LoginEnterToken(lua_tolstring(L, 1, NULL));
    }

    return 0;
}

// PROVEN: 687a8f91 DIFFERENT/IDENTICAL
// OFFSET: 0x008061C0/0x14051B860
static int32_t Script_GetAddOnInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAddOnInfo(index)");
        return 0;
    }

    const char* name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    lua_pushstring(L, name);
    const char* title = GetAddOnTitle(name);

    if (title) {
        lua_pushstring(L, title);
    } else {
        lua_pushnil(L);
    }

    const char* notes = GetAddOnNotes(name);

    if (notes) {
        lua_pushstring(L, notes);
    } else {
        lua_pushnil(L);
    }

    const char* url = GetAddOnURL(name);

    if (url) {
        lua_pushstring(L, url);
    } else {
        lua_pushnil(L);
    }

    UIADDON_LOADABLE_REASON reason = UIADDON_LOADABLE;
    UIADDON_LOADABLE_REASON dependencyReason = UIADDON_LOADABLE;

    if (IsAddOnLoadable(name, false, reason, dependencyReason, NULL)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (reason || dependencyReason) {
        char buffer[128];
        lua_pushstring(L, AddOnUnloadableToken(reason, dependencyReason, buffer, sizeof(buffer)));
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, AddOnSecurityToken(GetAddOnSecurity(name)));

    if (IsAddOnUpdateAvailable(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 8;
}

// PROVEN: a84327cd IDENTICAL/IDENTICAL
// OFFSET: 0x00806710/0x14051BE10
static int32_t Script_SaveAddOns(lua_State* L) {
    SaveAddOnEnableState();
    return 0;
}

// PROVEN: b356e5fc IDENTICAL/IDENTICAL
// OFFSET: 0x00806730/0x14051BE40
static int32_t Script_IsAddonVersionCheckEnabled(lua_State* L) {
    if (IsAddonVersionCheckEnabled()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00807C70/0x14051D510
static int32_t Script_SetRealmSplitState(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetRealmSplitState(1 or 2)");
        return 0;
    }

    int32_t state = static_cast<int32_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(state) <= 2) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REALM_SPLIT));
        msg.Put(static_cast<uint32_t>(state));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// PROVEN: f8c01e54 IDENTICAL/IDENTICAL
// OFFSET: 0x00805AE0/0x14051B090
static int32_t Script_IsShiftKeyDown(lua_State* L) {
    if (EventIsKeyDown(KEY_LSHIFT) || EventIsKeyDown(KEY_RSHIFT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 3198c8ae IDENTICAL/DIFFERENT
// OFFSET: 0x00806770/0x14051BE80
static int32_t Script_SetAddonVersionCheck(lua_State* L) {
    if (StringToBOOL(L, 1, 1)) {
        SetAddonVersionCheck(1);
    } else {
        SetAddonVersionCheck(0);
    }

    return 0;
}

// PROVEN: 1d33ec71 IDENTICAL/IDENTICAL
// OFFSET: 0x00806D10/0x14051C460
static int32_t Script_GetNumDeclensionSets(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetNumDeclensionSets(\"name\", gender)");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    int32_t sex = 2;

    if (lua_isnumber(L, 2)) {
        int32_t value = lua_tointeger(L, 2);

        for (uint32_t i = 0; i < 3; i++) {
            if (g_scriptSexIndex[i] == value) {
                sex = i;
                break;
            }
        }
    }

    lua_pushnumber(L, GetNumNameDeclensionSets(s_locale, name, sex));
    return 1;
}

// PROVEN: 754bc365 IDENTICAL/DIFFERENT
// OFFSET: 0x00806DC0/0x14051C520
static int32_t Script_DeclineName(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: DeclineName(\"name\", gender, declensionSet)");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    int32_t sex = 2;

    if (lua_isnumber(L, 2)) {
        int32_t value = lua_tointeger(L, 2);

        for (uint32_t i = 0; i < 3; i++) {
            if (g_scriptSexIndex[i] == value) {
                sex = i;
                break;
            }
        }
    }

    char declensions[5][1024];
    int32_t set = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

    if (GetNameDeclensions(s_locale, declensions[0], name, sex, set)) {
        for (uint32_t i = 0; i < 5; i++) {
            lua_pushstring(L, declensions[i]);
        }

        return 5;
    }

    for (uint32_t i = 0; i < 5; i++) {
        lua_pushnil(L);
    }

    return 5;
}

// PROVEN: 53e55f37 IDENTICAL/IDENTICAL
// OFFSET: 0x00806ED0/0x14051C660
static int32_t Script_GetNumGameAccounts(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login) {
        luaL_error(L, "No login available");
        return 0;
    }

    if (login->GetLoginServerType() != 1) {
        luaL_error(L, "Wrong login type");
        return 0;
    }

    lua_pushnumber(L, static_cast<BattlenetLogin*>(login)->GetNumGameAccounts());
    return 1;
}

// PROVEN: d000b8bf IDENTICAL/IDENTICAL
// OFFSET: 0x00806F60/0x14051C6F0
static int32_t Script_GetGameAccountInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGameAccountInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    Login* login = ClientServices::LoginConnection();

    if (!login) {
        luaL_error(L, "No login available");
        return 0;
    }

    if (login->GetLoginServerType() != 1) {
        luaL_error(L, "Wrong login type");
        return 0;
    }

    BattlenetLogin* battlenet = static_cast<BattlenetLogin*>(login);
    const char* name = battlenet->GetGameAccountName(index);

    if (!name) {
        return 0;
    }

    lua_pushstring(L, name);
    lua_pushnumber(L, battlenet->GetGameAccountRegion(index));
    return 2;
}

// OFFSET: 0x00807050/0x14051C7F0
static int32_t Script_SetGameAccount(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGameAccount(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    Login* login = ClientServices::LoginConnection();

    if (!login) {
        luaL_error(L, "No login available");
        return 0;
    }

    if (login->GetLoginServerType() != 1) {
        luaL_error(L, "Wrong login type");
        return 0;
    }

    lua_pushboolean(L, static_cast<BattlenetLogin*>(login)->SetGameAccount(index));
    return 1;
}

// OFFSET: 0x00807140/0x14051C900
static int32_t Script_SetClearConfigData(lua_State* L) {
    if (lua_type(L, 1) == LUA_TBOOLEAN) {
        SetClearConfigData(lua_toboolean(L, 1));
    } else {
        SetClearConfigData(1);
    }

    return 0;
}

// PROVEN: 8306d1be DIFFERENT/IDENTICAL
// OFFSET: 0x008078A0/0x14051D100
static int32_t Script_GetAddOnDependencies(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAddOnDependencies(index)");
        return 0;
    }

    const char* name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    TSGrowableArray<char*>* dependencies = GetAddonDeps(name);

    if (!dependencies) {
        return 0;
    }

    lua_checkstack(L, dependencies->m_count);

    for (uint32_t i = 0; i < dependencies->m_count; i++) {
        lua_pushstring(L, dependencies->m_data[i]);
    }

    return dependencies->m_count;
}

// PROVEN: ec513584 IDENTICAL/IDENTICAL
// OFFSET: 0x00805CE0/0x14051B320
static int32_t Script_QuitGameAndRunLauncher(lua_State* L) {
    CGlueMgr::QuitGameAndRunLauncher();
    return 0;
}

// OFFSET: 0x00805F40/0x14051B5E0
static int32_t Script_PINEntered(lua_State* L) {
    uint8_t digits[10];
    int32_t count = 0;

    for (int32_t i = 1; i < 11; i++) {
        if (lua_type(L, i) <= LUA_TNIL) {
            break;
        }

        if (!lua_isnumber(L, i)) {
            luaL_error(L, "Usage: PINEntered(4-10 key positions) - non-numeric arg #%d", count + 1);
            return 0;
        }

        digits[i - 1] = static_cast<uint8_t>(lua_tonumber(L, i));
        count++;
    }

    if (static_cast<uint32_t>(count - 4) > 6) {
        luaL_error(L, "Usage: PINEntered(4-10 key positions) - bad number of args %d", count);
        return 0;
    }

    CGlueMgr::LoginEnterPIN(count, digits);
    return 0;
}

// PROVEN: 7bc159df IDENTICAL/IDENTICAL
// OFFSET: 0x00806000/0x14051B690
static int32_t Script_MatrixEntered(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: MatrixEntered(number)");
        return 0;
    }

    uint32_t value = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (value > 9) {
        luaL_error(L, "MatrixEntered: number must be 0 to 9");
        return 0;
    }

    CGlueMgr::LoginEnterMatrix(value);
    return 0;
}

// PROVEN: b1ce844c IDENTICAL/IDENTICAL
// OFFSET: 0x00806080/0x14051B700
static int32_t Script_MatrixRevert(lua_State* L) {
    CGlueMgr::LoginRevertMatrix();
    return 0;
}

// PROVEN: 41b704bd IDENTICAL/IDENTICAL
// OFFSET: 0x00806090/0x14051B710
static int32_t Script_MatrixCommit(lua_State* L) {
    CGlueMgr::LoginCommitMatrix();
    return 0;
}

// PROVEN: 0822bf67 IDENTICAL/IDENTICAL
// OFFSET: 0x008060A0/0x14051B720
static int32_t Script_GetMatrixCoordinates(lua_State* L) {
    uint32_t x;
    uint32_t y;

    if (CGlueMgr::LoginGetMatrixCoordinates(&x, &y)) {
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, x);
        lua_pushnumber(L, y);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3;
}

// PROVEN: 8431dc4c IDENTICAL/IDENTICAL
// OFFSET: 0x008061A0/0x14051B840
static int32_t Script_PatchDownloadCancel(lua_State* L) {
    CGlueMgr::PatchDownloadCancel();
    return 0;
}

// PROVEN: a454d1fd IDENTICAL/IDENTICAL
// OFFSET: 0x008061B0/0x14051B850
static int32_t Script_PatchDownloadApply(lua_State* L) {
    CGlueMgr::PatchDownloadApply();
    return 0;
}

// PROVEN: 99ca6d72 IDENTICAL/IDENTICAL
// OFFSET: 0x00806380/0x14051BA40
static int32_t Script_LaunchAddOnURL(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LaunchAddOnURL(index)");
        return 0;
    }

    const char* name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    const char* url = GetAddOnURL(name);

    if (url) {
        OsLaunchURL(url);
    }

    return 0;
}

// PROVEN: f875af74 IDENTICAL/IDENTICAL
// OFFSET: 0x008064F0/0x14051BBB0
static int32_t Script_EnableAddOn(lua_State* L) {
    if ((!lua_isstring(L, 1) && lua_type(L, 1)) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: EnableAddOn(\"character\", index)");
        return 0;
    }

    const char* character = lua_tolstring(L, 1, NULL);
    const char* name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 2)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    SetAddOnEnableState(name, character, 2);
    return 0;
}

// PROVEN: 8fb4a34f IDENTICAL/IDENTICAL
// OFFSET: 0x008065B0/0x14051BC80
static int32_t Script_EnableAllAddOns(lua_State* L) {
    const char* character = lua_tolstring(L, 1, NULL);

    for (uint32_t i = 0; i < GetNumAddOns(); i++) {
        SetAddOnEnableState(GetAddOnByIndex(i), character, 2);
    }

    return 0;
}

// PROVEN: afe64031 IDENTICAL/IDENTICAL
// OFFSET: 0x00806600/0x14051BCE0
static int32_t Script_DisableAddOn(lua_State* L) {
    if ((!lua_isstring(L, 1) && lua_type(L, 1)) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: DisableAddOn(\"character\", index)");
        return 0;
    }

    const char* character = lua_tolstring(L, 1, NULL);
    const char* name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 2)) - 1);

    if (!name) {
        luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        return 0;
    }

    SetAddOnEnableState(name, character, 0);
    return 0;
}

// PROVEN: bf0c565f IDENTICAL/IDENTICAL
// OFFSET: 0x008068E0/0x14051BF80
static int32_t Script_SurveyNotificationDone(lua_State* L) {
    if (StringToBOOL(L, 1, 0)) {
        CGlueMgr::SurveySendResults();
    }

    CGlueMgr::SurveyDownloadEnd();
    return 1;
}

// PROVEN: 4254be6f IDENTICAL/IDENTICAL
// OFFSET: 0x00806A70/0x14051C140
static int32_t Script_CancelLogin(lua_State* L) {
    CGlueMgr::CancelLogin();
    return 0;
}

// PROVEN: 75fdb48f IDENTICAL/IDENTICAL
// OFFSET: 0x00806CC0/0x14051C410
static int32_t Script_AcceptChangedOptionWarnings(lua_State* L) {
    CGlueMgr::AcceptChangedOptionWarnings();
    return 0;
}

// PROVEN: cc3199ba IDENTICAL/IDENTICAL
// OFFSET: 0x008071B0/0x14051C940
static int32_t Script_IsStreamingMode(lua_State* L) {
    return SFile::IsStreamingMode();
}

// PROVEN: b20ab011 IDENTICAL/DIFFERENT
// OFFSET: 0x008071D0/0x14051C960
static int32_t Script_GetLoginScreenBackground(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: GetLoginScreenBackground(\"highresbg\", \"lowresbg\")");
        return 0;
    }

    const char* highRes = lua_tolstring(L, 1, NULL);
    const char* lowRes = lua_tolstring(L, 2, NULL);

    if (World::GetScene()->PrefetchModel(highRes, 5)) {
        lua_pushstring(L, highRes);
    } else {
        lua_pushstring(L, lowRes);
    }

    return 1;
}

// PROVEN: ce2003e3 IDENTICAL/IDENTICAL
// OFFSET: 0x00807310/0x140631100
static int32_t Script_IsGMClient(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

// PROVEN: 923b490f IDENTICAL/UNPROVEN
// OFFSET: 0x00807B50/0x14051D400
static int32_t Script_LoadURLIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LoadURLIndex(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));
    static ThrottleObject s_throttle(3, 5);

    if (s_throttle.IncrementAndCheck()) {
        char url[1024] = { 0 };
        FormatVisitableURL(index, url, sizeof(url));

        if (url[0]) {
            ClientServices::LaunchURL(url);
        }
    }

    return 0;
}

// PROVEN: d4033d80 IDENTICAL/IDENTICAL
// OFFSET: 0x00807C40/0x14051D4E0
static int32_t Script_HasShownTrialPopUp(lua_State* L) {
    lua_pushboolean(L, CGlueMgr::m_shownTrialPopUp);
    return 1;
}

// PROVEN: 0a0fa419 IDENTICAL/IDENTICAL
// OFFSET: 0x00807C60/0x14051D500
static int32_t Script_SetTrialPopUp(lua_State* L) {
    CGlueMgr::m_shownTrialPopUp = 1;
    return 0;
}

// STATIC: 0x00D07600/0x140BD1C10
static FrameScript_Method s_scriptFunctions[] = {
    { "IsShiftKeyDown", &Script_IsShiftKeyDown },
    { "GetBuildInfo", &Script_GetBuildInfo },
    { "GetLocale", &Script_GetLocale },
    { "GetSavedAccountName", &Script_GetSavedAccountName },
    { "SetSavedAccountName", &Script_SetSavedAccountName },
    { "GetUsesToken", &Script_GetUsesToken },
    { "SetUsesToken", &Script_SetUsesToken },
    { "GetSavedAccountList", &Script_GetSavedAccountList },
    { "SetSavedAccountList", &Script_SetSavedAccountList },
    { "SetCurrentScreen", &Script_SetCurrentScreen },
    { "QuitGame", &Script_QuitGame },
    { "QuitGameAndRunLauncher", &Script_QuitGameAndRunLauncher },
    { "PlayGlueMusic", &Script_PlayGlueMusic },
    { "PlayCreditsMusic", &Script_PlayCreditsMusic },
    { "StopGlueMusic", &Script_StopGlueMusic },
    { "GetMovieResolution", &Script_GetMovieResolution },
    { "GetScreenWidth", &Script_GetScreenWidth },
    { "GetScreenHeight", &Script_GetScreenHeight },
    { "LaunchURL", &Script_LaunchURL },
    { "ShowTOSNotice", &Script_ShowTOSNotice },
    { "TOSAccepted", &Script_TOSAccepted },
    { "AcceptTOS", &Script_AcceptTOS },
    { "ShowEULANotice", &Script_ShowEULANotice },
    { "EULAAccepted", &Script_EULAAccepted },
    { "AcceptEULA", &Script_AcceptEULA },
    { "ShowTerminationWithoutNoticeNotice", &Script_ShowTerminationWithoutNoticeNotice },
    { "TerminationWithoutNoticeAccepted", &Script_TerminationWithoutNoticeAccepted },
    { "AcceptTerminationWithoutNotice", &Script_AcceptTerminationWithoutNotice },
    { "ShowScanningNotice", &Script_ShowScanningNotice },
    { "ScanningAccepted", &Script_ScanningAccepted },
    { "AcceptScanning", &Script_AcceptScanning },
    { "ShowContestNotice", &Script_ShowContestNotice },
    { "ContestAccepted", &Script_ContestAccepted },
    { "AcceptContest", &Script_AcceptContest },
    { "DefaultServerLogin", &Script_DefaultServerLogin },
    { "StatusDialogClick", &Script_StatusDialogClick },
    { "GetServerName", &Script_GetServerName },
    { "DisconnectFromServer", &Script_DisconnectFromServer },
    { "IsConnectedToServer", &Script_IsConnectedToServer },
    { "EnterWorld", &Script_EnterWorld },
    { "Screenshot", &Script_Screenshot },
    { "PatchDownloadProgress", &Script_PatchDownloadProgress },
    { "PatchDownloadCancel", &Script_PatchDownloadCancel },
    { "PatchDownloadApply", &Script_PatchDownloadApply },
    { "GetNumAddOns", &Script_GetNumAddOns },
    { "GetAddOnInfo", &Script_GetAddOnInfo },
    { "LaunchAddOnURL", &Script_LaunchAddOnURL },
    { "GetAddOnDependencies", &Script_GetAddOnDependencies },
    { "GetAddOnEnableState", &Script_GetAddOnEnableState },
    { "EnableAddOn", &Script_EnableAddOn },
    { "EnableAllAddOns", &Script_EnableAllAddOns },
    { "DisableAddOn", &Script_DisableAddOn },
    { "DisableAllAddOns", &Script_DisableAllAddOns },
    { "SaveAddOns", &Script_SaveAddOns },
    { "ResetAddOns", &Script_ResetAddOns },
    { "IsAddonVersionCheckEnabled", &Script_IsAddonVersionCheckEnabled },
    { "SetAddonVersionCheck", &Script_SetAddonVersionCheck },
    { "GetCursorPosition", &Script_GetCursorPosition },
    { "ShowCursor", &Script_ShowCursor },
    { "HideCursor", &Script_HideCursor },
    { "GetBillingTimeRemaining", &Script_GetBillingTimeRemaining },
    { "GetBillingPlan", &Script_GetBillingPlan },
    { "GetBillingTimeRested", &Script_GetBillingTimeRested },
    { "SurveyNotificationDone", &Script_SurveyNotificationDone },
    { "PINEntered", &Script_PINEntered },
    { "PlayGlueAmbience", &Script_PlayGlueAmbience },
    { "StopGlueAmbience", &Script_StopGlueAmbience },
    { "GetCreditsText", &Script_GetCreditsText },
    { "MatrixEntered", &Script_MatrixEntered },
    { "MatrixRevert", &Script_MatrixRevert },
    { "MatrixCommit", &Script_MatrixCommit },
    { "GetMatrixCoordinates", &Script_GetMatrixCoordinates },
    { "ScanDLLStart", &Script_ScanDLLStart },
    { "ScanDLLContinueAnyway", &Script_ScanDLLContinueAnyway },
    { "IsScanDLLFinished", &Script_IsScanDLLFinished },
    { "SetRealmSplitState", &Script_SetRealmSplitState },
    { "RequestRealmSplitInfo", &Script_RequestRealmSplitInfo },
    { "CancelLogin", &Script_CancelLogin },
    { "GetCVar", &Script_GetCVar },
    { "GetCVarBool", &Script_GetCVarBool },
    { "SetCVar", &Script_SetCVar },
    { "GetCVarDefault", &Script_GetCVarDefault },
    { "GetChangedOptionWarnings", &Script_GetChangedOptionWarnings },
    { "AcceptChangedOptionWarnings", &Script_AcceptChangedOptionWarnings },
    { "ShowChangedOptionWarnings", &Script_ShowChangedOptionWarnings },
    { "TokenEntered", &Script_TokenEntered },
    { "GetNumDeclensionSets", &Script_GetNumDeclensionSets },
    { "DeclineName", &Script_DeclineName },
    { "GetNumGameAccounts", &Script_GetNumGameAccounts },
    { "GetGameAccountInfo", &Script_GetGameAccountInfo },
    { "SetGameAccount", &Script_SetGameAccount },
    { "StopAllSFX", &Script_StopAllSFX },
    { "SetClearConfigData", &Script_SetClearConfigData },
    { "RestartGx", &Script_RestartGx },
    { "IsStreamingMode", &Script_IsStreamingMode },
    { "IsStreamingTrial", &Script_IsStreamingTrial },
    { "GetLoginScreenBackground", &Script_GetLoginScreenBackground },
    { "IsConsoleActive", &Script_IsConsoleActive },
    { "RunScript", &Script_RunScript },
    { "ReadyForAccountDataTimes", &Script_ReadyForAccountDataTimes },
    { "IsSystemSupported", &Script_IsSystemSupported },
    { "IsGMClient", &Script_IsGMClient },
    { "LoadURLIndex", &Script_LoadURLIndex },
    { "HasShownTrialPopUp", &Script_HasShownTrialPopUp },
    { "SetTrialPopUp", &Script_SetTrialPopUp },
};

// PROVEN: bc0c7bfd IDENTICAL/UNPROVEN
// OFFSET: 0x00807330/0x14051CB00
void GlueScriptEventsRegisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// PROVEN: 554db767 IDENTICAL/UNPROVEN
// OFFSET: 0x00807360/0x14051CB50
void GlueScriptEventsUnregisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
