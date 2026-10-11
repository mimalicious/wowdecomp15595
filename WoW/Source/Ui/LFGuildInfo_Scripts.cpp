#include "WoW/Source/Ui/LFGuildInfo.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x009EB9F0
static int32_t Script_RequestGuildRecruitmentSettings(lua_State* L) {
    GlobalLFGuildGetGuildPost msg;
    ClientServices::Send(&msg);

    return 0;
}

// OFFSET: 0x009EBA20/0x140784A30
static int32_t Script_GetGuildRecruitmentSettings(lua_State* L) {
    lua_pushboolean(L, CGLFGuildInfo::s_settings.m_interests & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_interests >> 1) & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_interests >> 2) & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_interests >> 3) & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_interests >> 4) & 1);
    lua_pushboolean(L, CGLFGuildInfo::s_settings.m_availability & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_availability >> 1) & 1);
    lua_pushboolean(L, CGLFGuildInfo::s_settings.m_classRoles & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_classRoles >> 1) & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_classRoles >> 2) & 1);
    lua_pushboolean(L, CGLFGuildInfo::s_settings.m_level & 1);
    lua_pushboolean(L, (CGLFGuildInfo::s_settings.m_level >> 1) & 1);
    lua_pushboolean(L, CGLFGuildInfo::s_listed);

    return 13;
}

// OFFSET: 0x009EBB20/0x140784B30
static int32_t Script_GetGuildRecruitmentComment(lua_State* L) {
    lua_pushstring(L, CGLFGuildInfo::s_settings.m_comment);

    return 1;
}

// OFFSET: 0x009EBB40/0x140784B50
static int32_t Script_RequestGuildApplicantsList(lua_State* L) {
    GlobalLFGuildGetRecruits msg(CGLFGuildInfo::s_recruitsLastUpdate);
    ClientServices::Send(&msg);

    return 0;
}

// OFFSET: 0x009EBB70/0x140784B80
static int32_t Script_SetLookingForGuildComment(lua_State* L) {
    CVar* var = CVar::Lookup("lfGuildComment");

    if (var) {
        var->Set(lua_tostring(L, 1), true, false, false, true);
    }

    return 0;
}

// OFFSET: 0x009EBBB0/0x140784BE0
static int32_t Script_RequestGuildMembershipList(lua_State* L) {
    GlobalLFGuildGetApplications msg;
    ClientServices::Send(&msg);

    return 0;
}

// OFFSET: 0x009EBE30
static int32_t Script_SetGuildRecruitmentSettings(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGuildRecruitmentSettings(index, true/false)");
        return 0;
    }

    uint32_t flag = 1 << (static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    int32_t value = lua_toboolean(L, 2);

    switch (flag) {
        case 0x1: {
            if (value) {
                CGLFGuildInfo::s_settings.m_interests |= 1;
            } else {
                CGLFGuildInfo::s_settings.m_interests &= ~1;
            }

            break;
        }

        case 0x2: {
            if (value) {
                CGLFGuildInfo::s_settings.m_interests |= 2;
            } else {
                CGLFGuildInfo::s_settings.m_interests &= ~2;
            }

            break;
        }

        case 0x4: {
            if (value) {
                CGLFGuildInfo::s_settings.m_interests |= 4;
            } else {
                CGLFGuildInfo::s_settings.m_interests &= ~4;
            }

            break;
        }

        case 0x8: {
            if (value) {
                CGLFGuildInfo::s_settings.m_interests |= 8;
            } else {
                CGLFGuildInfo::s_settings.m_interests &= ~8;
            }

            break;
        }

        case 0x10: {
            if (value) {
                CGLFGuildInfo::s_settings.m_interests |= 0x10;
            } else {
                CGLFGuildInfo::s_settings.m_interests &= ~0x10;
            }

            break;
        }

        case 0x20: {
            if (value) {
                CGLFGuildInfo::s_settings.m_availability |= 1;
            } else {
                CGLFGuildInfo::s_settings.m_availability &= ~1;
            }

            break;
        }

        case 0x40: {
            if (value) {
                CGLFGuildInfo::s_settings.m_availability |= 2;
            } else {
                CGLFGuildInfo::s_settings.m_availability &= ~2;
            }

            break;
        }

        case 0x80: {
            if (value) {
                CGLFGuildInfo::s_settings.m_classRoles |= 1;
            } else {
                CGLFGuildInfo::s_settings.m_classRoles &= ~1;
            }

            break;
        }

        case 0x100: {
            if (value) {
                CGLFGuildInfo::s_settings.m_classRoles |= 2;
            } else {
                CGLFGuildInfo::s_settings.m_classRoles &= ~2;
            }

            break;
        }

        case 0x200: {
            if (value) {
                CGLFGuildInfo::s_settings.m_classRoles |= 4;
            } else {
                CGLFGuildInfo::s_settings.m_classRoles &= ~4;
            }

            break;
        }

        case 0x400: {
            if (value) {
                CGLFGuildInfo::s_settings.m_level = 1;
            }

            break;
        }

        case 0x800: {
            if (value) {
                CGLFGuildInfo::s_settings.m_level = 2;
            }

            break;
        }

        case 0x1000: {
            CGLFGuildInfo::s_listed = value != 0;
            break;
        }

        default: {
            return 0;
        }
    }

    CGLFGuildInfo::SendPost();

    return 0;
}

// OFFSET: 0x009EC160/0x1407850B0
static int32_t Script_SetGuildRecruitmentComment(lua_State* L) {
    SStrCopy(CGLFGuildInfo::s_settings.m_comment, lua_tostring(L, 1), sizeof(CGLFGuildInfo::s_settings.m_comment));
    CGLFGuildInfo::SendPost();

    return 0;
}

// OFFSET: 0x009EC1B0/0x140785100
static int32_t Script_GetNumGuildApplicants(lua_State* L) {
    lua_pushnumber(L, CGLFGuildInfo::s_recruits.m_count);

    return 1;
}

// OFFSET: 0x009EC1F0/0x140785130
static int32_t Script_GetGuildApplicantInfo(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_recruits.m_count)) {
        return 0;
    }

    LFGuildRecruitData* recruit = &CGLFGuildInfo::s_recruits.m_data[index];
    lua_pushstring(L, recruit->m_name);
    lua_pushnumber(L, recruit->m_level);
    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(recruit->m_classID);

    if (chrClass) {
        lua_pushstring(L, chrClass->m_filename);
    } else {
        lua_pushnil(L);
    }

    lua_pushboolean(L, recruit->m_interests & 1);
    lua_pushboolean(L, (recruit->m_interests >> 1) & 1);
    lua_pushboolean(L, (recruit->m_interests >> 2) & 1);
    lua_pushboolean(L, (recruit->m_interests >> 3) & 1);
    lua_pushboolean(L, (recruit->m_interests >> 4) & 1);
    lua_pushboolean(L, recruit->m_availability & 1);
    lua_pushboolean(L, (recruit->m_availability >> 1) & 1);
    lua_pushboolean(L, recruit->m_classRoles & 1);
    lua_pushboolean(L, (recruit->m_classRoles >> 1) & 1);
    lua_pushboolean(L, (recruit->m_classRoles >> 2) & 1);

    if (CGChat::IsProfanityFilterEnabled()) {
        char comment[1024];
        SStrCopy(comment, recruit->m_comment, sizeof(comment));
        ProfanityFilter(comment, false, false);
        lua_pushstring(L, comment);
    } else {
        lua_pushstring(L, recruit->m_comment);
    }

    lua_pushnumber(L, recruit->m_timeSince);
    lua_pushnumber(L, recruit->m_timeLeft);

    return 16;
}

// OFFSET: 0x009EC3F0/0x140785380
static int32_t Script_SetGuildApplicantSelection(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_recruits.m_count)) {
        CGLFGuildInfo::s_selectedApplicant = 0;
        return 0;
    }

    CGLFGuildInfo::s_selectedApplicant = *reinterpret_cast<uint64_t*>(CGLFGuildInfo::s_recruits.m_data[index].m_recruitGUID);

    return 0;
}

// OFFSET: 0x009EC450/0x1407853E0
static int32_t Script_GetGuildApplicantSelection(lua_State* L) {
    if (CGLFGuildInfo::s_selectedApplicant) {
        for (uint32_t i = 0; i < CGLFGuildInfo::s_recruits.m_count; i++) {
            if (*reinterpret_cast<uint64_t*>(CGLFGuildInfo::s_recruits.m_data[i].m_recruitGUID) == CGLFGuildInfo::s_selectedApplicant) {
                lua_pushnumber(L, i + 1);
                return 1;
            }
        }
    }

    lua_pushnumber(L, 0.0);

    return 1;
}

// OFFSET: 0x009EC510/0x140785450
static int32_t Script_DeclineGuildApplicant(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 0 && index < static_cast<int32_t>(CGLFGuildInfo::s_recruits.m_count)) {
        GlobalLFGuildDeclineRecruit msg(*reinterpret_cast<uint64_t*>(CGLFGuildInfo::s_recruits.m_data[index].m_recruitGUID));
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x009EC570/0x1407854B0
static int32_t Script_GetLookingForGuildSettings(lua_State* L) {
    CVar* var = CVar::Lookup("lfGuildSettings");

    if (var) {
        uint32_t value = var->m_intValue;
        lua_pushboolean(L, value & 1);
        lua_pushboolean(L, (value >> 1) & 1);
        lua_pushboolean(L, (value >> 2) & 1);
        lua_pushboolean(L, (value >> 3) & 1);
        lua_pushboolean(L, (value >> 4) & 1);
        lua_pushboolean(L, (value >> 5) & 1);
        lua_pushboolean(L, (value >> 6) & 1);
        lua_pushboolean(L, (value >> 7) & 1);
        lua_pushboolean(L, (value >> 8) & 1);
        lua_pushboolean(L, (value >> 9) & 1);
        return 10;
    }

    return 0;
}

// OFFSET: 0x009EC640/0x140785590
static int32_t Script_GetLookingForGuildComment(lua_State* L) {
    CVar* var = CVar::Lookup("lfGuildComment");

    if (var) {
        lua_pushstring(L, var->m_stringValue.m_str);
        return 1;
    }

    return 0;
}

// OFFSET: 0x009EC670/0x1407855D0
static int32_t Script_GetNumRecruitingGuilds(lua_State* L) {
    lua_pushnumber(L, CGLFGuildInfo::s_browseResults.m_count);

    return 1;
}

// OFFSET: 0x009EC6B0/0x140785600
static int32_t Script_GetRecruitingGuildInfo(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count)) {
        return 0;
    }

    LFGuildBrowseData* guild = &CGLFGuildInfo::s_browseResults.m_data[index];
    lua_pushstring(L, guild->m_name);
    lua_pushnumber(L, guild->m_guildLevel);
    lua_pushnumber(L, guild->m_numMembers);
    lua_pushnumber(L, guild->m_achievementPoints);

    if (CGChat::IsProfanityFilterEnabled()) {
        char comment[1024];
        SStrCopy(comment, guild->m_comment, sizeof(comment));
        ProfanityFilter(comment, false, false);
        lua_pushstring(L, comment);
    } else {
        lua_pushstring(L, guild->m_comment);
    }

    lua_pushboolean(L, guild->m_cached != 0);
    lua_pushboolean(L, guild->m_membershipRequested != 0);

    return 7;
}

// OFFSET: 0x009EC7D0/0x140785750
static int32_t Script_GetRecruitingGuildSettings(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count)) {
        return 0;
    }

    LFGuildBrowseData* guild = &CGLFGuildInfo::s_browseResults.m_data[index];
    lua_pushboolean(L, guild->m_interests & 1);
    lua_pushboolean(L, (guild->m_interests >> 1) & 1);
    lua_pushboolean(L, (guild->m_interests >> 2) & 1);
    lua_pushboolean(L, (guild->m_interests >> 3) & 1);
    lua_pushboolean(L, (guild->m_interests >> 4) & 1);
    lua_pushboolean(L, guild->m_availability & 1);
    lua_pushboolean(L, (guild->m_availability >> 1) & 1);
    lua_pushboolean(L, guild->m_classRoles & 1);
    lua_pushboolean(L, (guild->m_classRoles >> 1) & 1);
    lua_pushboolean(L, (guild->m_classRoles >> 2) & 1);

    return 10;
}

// OFFSET: 0x009EC8C0/0x140785850
static int32_t Script_GetRecruitingGuildTabardInfo(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count)) {
        return 0;
    }

    CImVector background = 0;
    CImVector border = 0;
    CImVector emblem = 0;
    char fileName[260];
    LFGuildBrowseData* guild = &CGLFGuildInfo::s_browseResults.m_data[index];

    if (!CGObject_C::GetGuildLogoInfo(guild->m_background, guild->m_borderColor, guild->m_emblemColor, guild->m_emblemStyle, background, border, emblem, fileName, 260)) {
        return 0;
    }

    lua_pushnumber(L, background.r);
    lua_pushnumber(L, background.g);
    lua_pushnumber(L, background.b);
    lua_pushnumber(L, border.r);
    lua_pushnumber(L, border.g);
    lua_pushnumber(L, border.b);
    lua_pushnumber(L, emblem.r);
    lua_pushnumber(L, emblem.g);
    lua_pushnumber(L, emblem.b);
    lua_pushstring(L, fileName);

    return 10;
}

// OFFSET: 0x009ECA40/0x140785A20
static int32_t Script_SetRecruitingGuildSelection(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count)) {
        CGLFGuildInfo::s_selectedGuild = 0;
        return 0;
    }

    CGLFGuildInfo::s_selectedGuild = *reinterpret_cast<uint64_t*>(CGLFGuildInfo::s_browseResults.m_data[index].m_guildGUID);

    return 0;
}

// OFFSET: 0x009ECAA0/0x140785A80
static int32_t Script_GetRecruitingGuildSelection(lua_State* L) {
    for (int32_t i = 0; i < static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count); i++) {
        if (*reinterpret_cast<uint64_t*>(CGLFGuildInfo::s_browseResults.m_data[i].m_guildGUID) == CGLFGuildInfo::s_selectedGuild) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x009ECB10/0x140785AE0
static int32_t Script_GetNumGuildMembershipRequests(lua_State* L) {
    lua_pushnumber(L, CGLFGuildInfo::s_applications.m_count);
    lua_pushnumber(L, CGLFGuildInfo::s_numApplicationsRemaining);

    return 2;
}

// OFFSET: 0x009ECB60/0x140785B20
static int32_t Script_GetGuildMembershipRequestInfo(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_applications.m_count)) {
        return 0;
    }

    LFGuildApplicationData* application = &CGLFGuildInfo::s_applications.m_data[index];
    lua_pushstring(L, application->m_guildName);
    lua_pushnumber(L, application->m_timeSince);
    lua_pushnumber(L, application->m_timeLeft);

    return 3;
}

// OFFSET: 0x009ECBD0/0x140785BB0
static int32_t Script_GetGuildMembershipRequestSettings(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGLFGuildInfo::s_applications.m_count)) {
        return 0;
    }

    LFGuildApplicationData* application = &CGLFGuildInfo::s_applications.m_data[index];
    lua_pushboolean(L, application->m_interests & 1);
    lua_pushboolean(L, (application->m_interests >> 1) & 1);
    lua_pushboolean(L, (application->m_interests >> 2) & 1);
    lua_pushboolean(L, (application->m_interests >> 3) & 1);
    lua_pushboolean(L, (application->m_interests >> 4) & 1);
    lua_pushboolean(L, application->m_availability & 1);
    lua_pushboolean(L, (application->m_availability >> 1) & 1);
    lua_pushboolean(L, application->m_classRoles & 1);
    lua_pushboolean(L, (application->m_classRoles >> 1) & 1);
    lua_pushboolean(L, (application->m_classRoles >> 2) & 1);

    return 10;
}

// OFFSET: 0x009ECD50/0x140785D90
static int32_t Script_SetLookingForGuildSettings(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetLookingForGuildSettings(index, true/false)");
        return 0;
    }

    CVar* var = CVar::Lookup("lfGuildSettings");

    if (!var) {
        return 0;
    }

    uint32_t settings = var->m_intValue;
    uint32_t flag = 1 << (static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    int32_t value = lua_toboolean(L, 2);

    switch (flag) {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
        case 0x10:
        case 0x20:
        case 0x40:
        case 0x80:
        case 0x100:
        case 0x200: {
            var->Set(static_cast<int32_t>(value ? flag | settings : ~flag & settings), true, false, false, true);
            CGLFGuildInfo::SendBrowse();
            break;
        }
    }

    return 0;
}

// OFFSET: 0x009ECE60/0x140785EA0
static int32_t Script_RequestRecruitingGuildsList(lua_State* L) {
    CGLFGuildInfo::SendBrowse();

    return 0;
}

// OFFSET: 0x009ECE70/0x140785EB0
static int32_t Script_RequestGuildMembership(lua_State* L) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !lua_isstring(L, 1)) {
        return 0;
    }

    const char* name = lua_tostring(L, 1);
    int32_t index;

    for (index = 0; index < static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count); index++) {
        if (!_stricmp(CGLFGuildInfo::s_browseResults.m_data[index].m_name, name)) {
            break;
        }
    }

    if (index >= static_cast<int32_t>(CGLFGuildInfo::s_browseResults.m_count) || index < 0) {
        return 0;
    }

    LFGuildBrowseData* guild = &CGLFGuildInfo::s_browseResults.m_data[index];
    guild->m_membershipRequested = true;
    uint32_t interests;
    uint32_t availability;
    uint32_t classRoles;
    CGLFGuildInfo::GetLookingForGuildSettings(interests, availability, classRoles);
    UserLFGuildAddRecruit msg;
    memcpy(msg.m_guildGUID, guild->m_guildGUID, sizeof(msg.m_guildGUID));
    msg.m_interests = interests;
    msg.m_availability = availability;
    msg.m_classRoles = classRoles;
    SStrCopy(msg.m_name, player->GetUnitName(NULL, 1), sizeof(msg.m_name));

    if (lua_isstring(L, 2)) {
        SStrCopy(msg.m_comment, lua_tostring(L, 2), sizeof(msg.m_comment));
    } else {
        msg.m_comment[0] = '\0';
    }

    ClientServices::Send(&msg);

    return 0;
}

// OFFSET: 0x009ED020/0x1407860F0
static int32_t Script_CancelGuildMembershipRequest(lua_State* L) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !lua_isnumber(L, 1)) {
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 0 && index < static_cast<int32_t>(CGLFGuildInfo::s_applications.m_count)) {
        LFGuildApplicationData* application = &CGLFGuildInfo::s_applications.m_data[index];
        GlobalLFGuildRemoveRecruit msg;
        memcpy(msg.m_guildGUID, application->m_guildGUID, sizeof(msg.m_guildGUID));
        ClientServices::Send(&msg);
    }

    return 0;
}

// STATIC: 0x00D16BB0
static FrameScript_Method s_ScriptFunctions[] = {
    { "RequestGuildRecruitmentSettings", &Script_RequestGuildRecruitmentSettings },
    { "GetGuildRecruitmentSettings", &Script_GetGuildRecruitmentSettings },
    { "SetGuildRecruitmentSettings", &Script_SetGuildRecruitmentSettings },
    { "GetGuildRecruitmentComment", &Script_GetGuildRecruitmentComment },
    { "SetGuildRecruitmentComment", &Script_SetGuildRecruitmentComment },
    { "RequestGuildApplicantsList", &Script_RequestGuildApplicantsList },
    { "GetNumGuildApplicants", &Script_GetNumGuildApplicants },
    { "GetGuildApplicantInfo", &Script_GetGuildApplicantInfo },
    { "SetGuildApplicantSelection", &Script_SetGuildApplicantSelection },
    { "GetGuildApplicantSelection", &Script_GetGuildApplicantSelection },
    { "DeclineGuildApplicant", &Script_DeclineGuildApplicant },
    { "GetLookingForGuildSettings", &Script_GetLookingForGuildSettings },
    { "SetLookingForGuildSettings", &Script_SetLookingForGuildSettings },
    { "GetLookingForGuildComment", &Script_GetLookingForGuildComment },
    { "SetLookingForGuildComment", &Script_SetLookingForGuildComment },
    { "RequestRecruitingGuildsList", &Script_RequestRecruitingGuildsList },
    { "GetNumRecruitingGuilds", &Script_GetNumRecruitingGuilds },
    { "GetRecruitingGuildInfo", &Script_GetRecruitingGuildInfo },
    { "GetRecruitingGuildSettings", &Script_GetRecruitingGuildSettings },
    { "GetRecruitingGuildTabardInfo", &Script_GetRecruitingGuildTabardInfo },
    { "SetRecruitingGuildSelection", &Script_SetRecruitingGuildSelection },
    { "GetRecruitingGuildSelection", &Script_GetRecruitingGuildSelection },
    { "RequestGuildMembership", &Script_RequestGuildMembership },
    { "RequestGuildMembershipList", &Script_RequestGuildMembershipList },
    { "GetNumGuildMembershipRequests", &Script_GetNumGuildMembershipRequests },
    { "GetGuildMembershipRequestInfo", &Script_GetGuildMembershipRequestInfo },
    { "CancelGuildMembershipRequest", &Script_CancelGuildMembershipRequest },
    { "GetGuildMembershipRequestSettings", &Script_GetGuildMembershipRequestSettings },
};

// OFFSET: 0x009EBBE0/0x140784C10
void LFGuildRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
