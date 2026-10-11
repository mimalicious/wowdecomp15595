#include "GuildInfo.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameScript/Types.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/DungeonEncounterRec.h"
#include "WoW/Source/DB/Rec/GuildPerkSpellsRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/JamClientAutoCode/JamWorld.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D11C08
static GuildRankFlag s_rankFlags[20] = {
    { 0x1, 0 },
    { 0x2, 0 },
    { 0x4, 0 },
    { 0x8, 0 },
    { 0x80, 0 },
    { 0x100, 0 },
    { 0x10, 0 },
    { 0x20, 0 },
    { 0x1000, 0 },
    { 0x2000, 0 },
    { 0x4000, 0 },
    { 0x8000, 0 },
    { 0x10000, 0 },
    { 0x20000, 0 },
    { 0x40000, 0 },
    { 0x80000, 0 },
    { 0x100000, 0 },
    { 0x200000, 0 },
    { 0x400000, 0 },
    { 0x800000, 0 },
};

// STATIC: 0x00D11CA8
static const uint32_t s_tabFlags[3] = { 0x1, 0x2, 0x4 };

// PROVEN: 7f202da4 IDENTICAL/NONE
// OFFSET: 0x00931AA0/0x140699DC0
static int32_t Script_GetNumGuildMembers(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_numMembers);
    lua_pushnumber(L, CGGuildInfo::m_numOnline);
    return 2;
}

// PROVEN: cf8ec614 IDENTICAL/NONE
// OFFSET: 0x00931B00/0x140699E00
static int32_t Script_GetGuildRosterMOTD(lua_State* L) {
    if (!g_profanityFilterCVar || g_profanityFilterCVar->m_intValue) {
        ProfanityFilter(CGGuildInfo::m_motd, false, false);
    }

    lua_pushstring(L, CGGuildInfo::m_motd);
    return 1;
}

// OFFSET: 0x00935A50/0x14069E800
static int32_t Script_GetGuildRosterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildRosterInfo(index)");
        return 0;
    }

    GuildMemberInfo* member;
    int32_t rankOrder;

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < CGGuildInfo::m_numMembers && (member = CGGuildInfo::m_members[index]) && (rankOrder = CGGuildInfo::GetRankOrderFromRankID(member->m_rankID)) >= 0) {
            lua_pushstring(L, member->m_name);

            const char* rankName = CGGuildInfo::GetRankNameFromRankID(member->m_rankID);

            if (rankName) {
                lua_pushstring(L, rankName);
            } else {
                lua_pushstring(L, "");
            }

            lua_pushnumber(L, rankOrder);
            lua_pushnumber(L, member->m_level);

            ChrClassesRec* classRec = g_chrClassesDB.GetRecord(member->m_classID);
            const char* className = CGUnit_C::GetDisplayClassNameFromRecord(classRec, member->m_gender, NULL);

            if (className) {
                lua_pushstring(L, className);
            } else {
                lua_pushnil(L);
            }

            AreaTableRec* area = g_areaTableDB.GetRecord(member->m_areaID);

            if (area) {
                lua_pushstring(L, area->m_areaName);
            } else {
                lua_pushnil(L);
            }

            if (!g_profanityFilterCVar || g_profanityFilterCVar->m_intValue) {
                char note[128];
                SStrCopy(note, member->m_note, sizeof(note));
                ProfanityFilter(note, true, false);
                lua_pushstring(L, note);
                SStrCopy(note, member->m_officerNote, sizeof(note));
                ProfanityFilter(note, true, false);
                lua_pushstring(L, note);
            } else {
                lua_pushstring(L, member->m_note);
                lua_pushstring(L, member->m_officerNote);
            }

            if (member->m_online) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnil(L);
            }

            uint8_t status = member->m_status;

            if (status & 0x4) {
                lua_pushnumber(L, 2.0);
            } else if (status & 0x2) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnumber(L, 0.0);
            }

            if (classRec) {
                lua_pushstring(L, classRec->m_filename);
            } else {
                lua_pushnil(L);
            }

            lua_pushnumber(L, member->m_achievementPoints);
            lua_pushnumber(L, member->m_achievementRank + 1);
            lua_pushboolean(L, member->m_status & 0x10);

            if (CGGameUI::s_canSendSoR && member->m_sorEligible) {
                lua_pushboolean(L, 1);
            } else {
                lua_pushboolean(L, 0);
            }

            return 15;
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushstring(L, "");
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushboolean(L, 0);
    lua_pushboolean(L, 0);
    return 15;
}

// OFFSET: 0x00933B30/0x14069C200
static int32_t Script_GuildRosterSendSoR(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildRosterSendSoR(index[, \"comment\"])");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    GuildMemberInfo* member;

    if (index >= CGGuildInfo::m_numMembers || !(member = CGGuildInfo::m_members[index])) {
        lua_pushboolean(L, 0);
        return 1;
    }

    uint64_t guid = member->m_guid;
    const char* comment = "";

    if (lua_isstring(L, 2)) {
        comment = lua_tolstring(L, 2, NULL);
    }

    if (!CGGameUI::s_canSendSoR || !member->m_sorEligible) {
        lua_pushboolean(L, 0);
        return 1;
    }

    GlobalGuildSendSoRRequest request;
    memcpy(request.m_targetGUID, &guid, sizeof(request.m_targetGUID));

    if (!ValidateStringIsUTF8(comment) || !ScrubString(comment, request.m_text, 255, sizeof(request.m_text) - 4, 6)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    char* src = request.m_text;
    char* dst = request.m_text;

    do {
        if (*src == '|' && src[1] == '|') {
            *dst++ = '|';
            src += 2;
        } else {
            *dst++ = *src++;
        }
    } while (*src);

    ClientServices::Send(&request);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x00933CE0/0x14069C3C0
static int32_t Script_GetGuildRosterLastOnline(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildRosterLastOnline(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    GuildMemberInfo* member;

    if (index >= CGGuildInfo::m_numMembers || !(member = CGGuildInfo::m_members[index]) || member->m_online) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    float days = member->m_lastSave;
    int32_t years = static_cast<int32_t>(days * (1.0f / 365.0f));
    days -= static_cast<float>(years) * 365.0f;
    int32_t months = static_cast<int32_t>(days * (1.0f / 30.0f));
    days -= static_cast<float>(months) * 30.0f;
    lua_pushnumber(L, years);
    lua_pushnumber(L, months);
    lua_pushnumber(L, static_cast<int32_t>(days));
    lua_pushnumber(L, static_cast<int32_t>((days - static_cast<float>(static_cast<int32_t>(days))) * 24.0f));
    return 4;
}

// OFFSET: 0x00933E50/0x14069C530
static int32_t Script_GetGuildRosterContribution(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildRosterContributionInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGGuildInfo::m_numMembers) {
        return 0;
    }

    GuildMemberInfo* member = CGGuildInfo::m_members[index];

    if (!member) {
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(member->m_weeklyXP));
    lua_pushnumber(L, static_cast<int32_t>(member->m_totalXP));
    lua_pushnumber(L, member->m_weeklyXPRank + 1);
    lua_pushnumber(L, member->m_totalXPRank + 1);
    lua_pushnumber(L, member->m_guildRepToCap);
    return 5;
}

// PROVEN: b159a4bc IDENTICAL/NONE
// OFFSET: 0x00931B40/0x140699E50
int32_t CGGuildInfo::Script_GetGuildRosterLargestContribution(lua_State* L) {
    lua_pushnumber(L, static_cast<int32_t>(CGGuildInfo::m_largestWeeklyXPContribution));
    lua_pushnumber(L, static_cast<int32_t>(CGGuildInfo::m_largestTotalXPContribution));
    return 2;
}

// PROVEN: a7ca725d IDENTICAL/NONE
// OFFSET: 0x00931B80/0x140699E90
int32_t CGGuildInfo::Script_GetGuildRosterLargestAchievements(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_largestPersonalAchievementPoints);
    return 1;
}

// OFFSET: 0x00935E10/0x14069EC80
static int32_t Script_GuildRosterSetPublicNote(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: GuildRosterSetPublicNote(index, note)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return 0;
    }

    const char* note = lua_tolstring(L, 2, NULL);
    char scrubbed[128];
    ScrubString(note, scrubbed, 32, sizeof(scrubbed), 1);

    GuildMemberInfo* member = CGGuildInfo::GetMember(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

    if (member && note && strcmp(member->m_note, note)) {
        JamGuildMemberSetNoteData noteData(member->m_guid, scrubbed, true);
        GlobalGuildSetMemberNote request(noteData);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00935F90/0x14069EE10
static int32_t Script_GuildRosterSetOfficerNote(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: GuildRosterSetOfficerNote(index, note)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return 0;
    }

    const char* note = lua_tolstring(L, 2, NULL);
    char scrubbed[128];
    ScrubString(note, scrubbed, 32, sizeof(scrubbed), 1);

    GuildMemberInfo* member = CGGuildInfo::GetMember(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

    if (member && note && strcmp(member->m_officerNote, note)) {
        JamGuildMemberSetNoteData noteData(member->m_guid, scrubbed, false);
        GlobalGuildSetMemberNote request(noteData);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00933F30/0x14069C620
static int32_t Script_SetGuildRosterSelection(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGuildRosterSelection(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGGuildInfo::m_numMembers) {
        CGGuildInfo::m_selectedMember = CGGuildInfo::m_members[index]->m_guid;
        return 0;
    }

    CGGuildInfo::m_selectedMember = 0;
    return 0;
}

// PROVEN: d3e64912 IDENTICAL/NONE
// OFFSET: 0x00933FC0/0x14069C6A0
static int32_t Script_GetGuildRosterSelection(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::GetSelectedMemberIndex() + 1);
    return 1;
}

// PROVEN: 3b00f054 IDENTICAL/NONE
// OFFSET: 0x00936110/0x14069EFA0
static int32_t Script_CanGuildPromote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x80) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: ab504469 IDENTICAL/NONE
// OFFSET: 0x009361A0/0x14069F070
static int32_t Script_CanGuildDemote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x100) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 16f207ce IDENTICAL/NONE
// OFFSET: 0x00936230/0x14069F140
static int32_t Script_CanGuildInvite(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x10) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: d1a97ffe IDENTICAL/NONE
// OFFSET: 0x009362C0/0x14069F210
static int32_t Script_CanGuildRemove(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 919e3e0b IDENTICAL/NONE
// OFFSET: 0x00936350/0x14069F2E0
static int32_t Script_CanEditMOTD(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x1000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: c30b752b IDENTICAL/NONE
// OFFSET: 0x009363E0/0x14069F3B0
static int32_t Script_CanEditPublicNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x2000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 91708185 IDENTICAL/NONE
// OFFSET: 0x00936470/0x14069F480
static int32_t Script_CanEditOfficerNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x8000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: f808adbe IDENTICAL/NONE
// OFFSET: 0x00936500/0x14069F550
static int32_t Script_CanViewOfficerNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x4000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 079d8ebb IDENTICAL/NONE
// OFFSET: 0x00936590/0x14069F620
static int32_t Script_CanEditGuildInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x10000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: afaed309 IDENTICAL/NONE
// OFFSET: 0x00936620/0x14069F6F0
static int32_t Script_CanGuildBankRepair(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x40000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 57805227 IDENTICAL/NONE
// OFFSET: 0x009366B0/0x14069F7C0
static int32_t Script_IsInAuthenticatedRank(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        lua_pushnil(L);
        return 1;
    }

    if (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x200000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 91459882 IDENTICAL/NONE
// OFFSET: 0x00933FF0/0x14069C700
static int32_t Script_GetNumMembersInRank(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNumMembersInRank(index)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 1));
    lua_pushnumber(L, CGGuildInfo::GetNumMembersInRank(rankOrder - 1));
    return 1;
}

// OFFSET: 0x00936740/0x14069F890
static int32_t Script_CanEditGuildTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanEditGuildTabInfo(tab)");
        return 0;
    }

    uint8_t tab = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (CGGuildInfo::GetGuildBankTabPermissionsByRankID(tab, player->m_player->m_playerGuildrank) & 0x4)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// PROVEN: e48ec7ce IDENTICAL/NONE
// OFFSET: 0x00936800/0x14069F990
static int32_t Script_CanEditGuildEvent(lua_State* L) {
    if (CGGuildInfo::CanEditGuildEvent()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: b582434b IDENTICAL/NONE
// OFFSET: 0x00939620/0x1406A3860
static int32_t Script_SortGuildRoster(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SortGuildRoster(\"type\")");
        return 0;
    }

    GUILD_SORT_TYPE type = GUILD_SORT_NAME;
    const char* name = lua_tolstring(L, 1, NULL);

    if (_stricmp(name, "name")) {
        if (!_stricmp(name, "level")) {
            type = GUILD_SORT_LEVEL;
        } else if (!_stricmp(name, "class")) {
            type = GUILD_SORT_CLASS;
        } else if (!_stricmp(name, "group")) {
            type = GUILD_SORT_GROUP;
        } else if (!_stricmp(name, "rank")) {
            type = GUILD_SORT_RANK;
        } else if (!_stricmp(name, "zone")) {
            type = GUILD_SORT_ZONE;
        } else if (!_stricmp(name, "online")) {
            type = GUILD_SORT_ONLINE;
        } else if (!_stricmp(name, "note")) {
            type = GUILD_SORT_NOTE;
        } else if (!_stricmp(name, "bgrating")) {
            type = GUILD_SORT_BGRATING;
        } else if (!_stricmp(name, "weeklyxp")) {
            type = GUILD_SORT_WEEKLYXP;
        } else if (!_stricmp(name, "totalxp")) {
            type = GUILD_SORT_TOTALXP;
        } else if (!_stricmp(name, "achievementpoints")) {
            type = GUILD_SORT_ACHIEVEMENTPOINTS;
        }
    }

    CGGuildInfo::SetSortType(type);
    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), CGGuildInfo::QSortRoster);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    CGGuildInfo::UpdateProfessions();
    return 0;
}

// PROVEN: e2c92a9f IDENTICAL/NONE
// OFFSET: 0x00931BB0/0x140699EB0
static int32_t Script_SetGuildRosterShowOffline(lua_State* L) {
    g_guildShowOfflineCVar->Set(StringToBOOL(L, 1, 1), true, false, false, true);
    return 0;
}

// PROVEN: f90c2fcd IDENTICAL/NONE
// OFFSET: 0x00931BE0/0x140699EF0
static int32_t Script_GetGuildRosterShowOffline(lua_State* L) {
    if (g_guildShowOfflineCVar->m_intValue) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 9796cab4 IDENTICAL/NONE
// OFFSET: 0x00931C20/0x140699F30
static int32_t Script_GuildControlGetNumRanks(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_numUsedRanks);
    return 1;
}

// OFFSET: 0x00936840/0x14069F9D0
static int32_t Script_GuildControlGetRankName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlGetRankName(index)");
        return 0;
    }

    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);
    char name[64] = { 0 };
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (player) {
        int32_t rankID = CGGuildInfo::GetRankIDFromRankOrder(index - 1);

        if (rankID >= 0) {
            const char* rankName = CGGuildInfo::GetRankNameFromRankID(rankID);

            if (rankName) {
                SStrCopy(name, rankName, sizeof(name));
                TruncateNewLines(name);
            }
        }
    }

    lua_pushstring(L, name);
    return 1;
}

// OFFSET: 0x00931C60/0x140699F60
static int32_t Script_GuildControlSetRank(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlSetRank(rankOrder)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (rankOrder < 0 || rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
    }

    CGGuildInfo::m_selectedRankOrder = rankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    if (rankOrder >= 0) {
        uint32_t rights;
        int32_t index;

        if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) || (index = CGGuildInfo::GetRankIndexFromRankOrder(rankOrder)) < 0) {
            rights = 0;
        } else {
            rights = CGGuildInfo::m_rankInfo[index].m_rights;
        }

        for (uint32_t i = 0; i < 20; i++) {
            s_rankFlags[i].m_enabled = rights & s_rankFlags[i].m_mask;
        }
    }

    return 0;
}

// OFFSET: 0x00936930/0x14069FB20
static int32_t Script_GuildControlGetRankFlags(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    uint32_t rights = 0;

    if (rankOrder >= 0) {
        int32_t index;

        if (rankOrder < static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) && (index = CGGuildInfo::GetRankIndexFromRankOrder(rankOrder)) >= 0) {
            rights = CGGuildInfo::m_rankInfo[index].m_rights;
        }
    } else {
        lua_pushnil(L);
    }

    for (uint32_t i = 0; i < 20; i++) {
        if (player && player->GetGuildGUID() && (rights & s_rankFlags[i].m_mask)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    }

    return 20;
}

// OFFSET: 0x00934050/0x14069C760
static int32_t Script_GuildControlSetRankFlag(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlSetRankFlag(index, enabled)");
        return 0;
    }

    uint32_t flag = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (flag >= 20) {
        return 0;
    }

    int32_t enabled = StringToBOOL(L, 2, 0);
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;
    s_rankFlags[flag].m_enabled = enabled;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    if (rankOrder < 0 || rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        luaL_error(L, "Invalid rank order.");
        return 0;
    }

    if (s_rankFlags[flag].m_mask == 0x200000 && enabled && CGGuildInfo::GetNumMembersInRank(rankOrder) > 0) {
        luaL_error(L, "Can't enable authentication while rank has members.");
        return 0;
    }

    int32_t rankID = CGGuildInfo::GetRankIDFromRankOrder(rankOrder);

    if (rankID < 0) {
        luaL_error(L, "Invalid rankID.");
        return 0;
    }

    const char* rankName = CGGuildInfo::GetRankNameFromRankID(rankID);

    if (!rankName) {
        luaL_error(L, "Invalid rank name.");
        return 0;
    }

    uint32_t oldFlags = CGGuildInfo::GetPermissionsByRankOrder(rankOrder);
    uint32_t newFlags = oldFlags;

    for (uint32_t i = 0; i < 20; i++) {
        if (s_rankFlags[i].m_enabled) {
            newFlags |= s_rankFlags[i].m_mask;
        } else {
            newFlags &= ~s_rankFlags[i].m_mask;
        }
    }

    GlobalGuildSetRankPermissions request;
    request.m_rankID = rankID;
    request.m_rankOrder = rankOrder;
    SStrCopy(request.m_rankName, rankName, sizeof(request.m_rankName));
    request.m_newFlags = newFlags;
    request.m_oldFlags = oldFlags;
    request.m_withdrawGoldLimit = CGGuildInfo::GetWithdrawGoldLimitForSelectedRank();

    for (uint32_t i = 0; i < 8; i++) {
        request.m_tabFlags[i] = CGGuildInfo::GetTabFlagsForSelectedRank(static_cast<uint8_t>(i));
        request.m_tabWithdrawItemLimit[i] = CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(static_cast<uint8_t>(i));
    }

    ClientServices::Send(&request);
    return 0;
}

// OFFSET: 0x009342E0/0x14069C9C0
static int32_t Script_GuildControlSaveRank(lua_State* L) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    if (rankOrder < 0) {
        luaL_error(L, "no rank selected");
        return 0;
    }

    uint32_t oldFlags;
    int32_t index;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) || (index = CGGuildInfo::GetRankIndexFromRankOrder(rankOrder)) < 0) {
        oldFlags = 0;
    } else {
        oldFlags = CGGuildInfo::m_rankInfo[index].m_rights;
    }

    uint32_t newFlags = oldFlags;

    for (uint32_t i = 0; i < 20; i++) {
        if (s_rankFlags[i].m_enabled) {
            newFlags |= s_rankFlags[i].m_mask;
        } else {
            newFlags &= ~s_rankFlags[i].m_mask;
        }
    }

    GlobalGuildSetRankPermissions request;
    request.m_rankID = CGGuildInfo::GetRankIDFromRankOrder(rankOrder);

    int32_t selectedRankOrder = CGGuildInfo::m_selectedRankOrder;

    if (selectedRankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        selectedRankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    request.m_rankOrder = selectedRankOrder;

    const char* name = lua_tolstring(L, 1, NULL);

    if (name && *name) {
        char scrubbed[64];
        ScrubString(name, scrubbed, 16, sizeof(scrubbed), 1);
        SStrCopy(request.m_rankName, scrubbed, sizeof(request.m_rankName));
    } else {
        SStrCopy(request.m_rankName, CGGuildInfo::GetRankNameFromRankID(request.m_rankID), sizeof(request.m_rankName));
    }

    request.m_newFlags = newFlags;
    request.m_oldFlags = oldFlags;
    request.m_withdrawGoldLimit = CGGuildInfo::GetWithdrawGoldLimitForSelectedRank();

    for (uint32_t i = 0; i < 8; i++) {
        request.m_tabFlags[i] = CGGuildInfo::GetTabFlagsForSelectedRank(static_cast<uint8_t>(i));
        request.m_tabWithdrawItemLimit[i] = CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(static_cast<uint8_t>(i));
    }

    ClientServices::Send(&request);
    return 0;
}

// PROVEN: 970cca3f IDENTICAL/NONE
// OFFSET: 0x00931D10/0x14069A040
static int32_t Script_GuildControlAddRank(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GuildControlAddRank(name)");
        return 0;
    }

    if (CGGuildInfo::m_numUsedRanks < 10) {
        const char* name = lua_tolstring(L, 1, NULL);

        if (name && *name) {
            char scrubbed[64];
            ScrubString(name, scrubbed, 16, sizeof(scrubbed), 1);

            GlobalGuildAddRank request(scrubbed, CGGuildInfo::m_numUsedRanks);
            ClientServices::Send(&request);
        }
    }

    return 0;
}

// OFFSET: 0x00934560/0x14069CDB0
static int32_t Script_GuildControlDelRank(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlDelRank(rankOrder)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t numRanks;

    if (rankOrder <= 0 || (numRanks = CGGuildInfo::m_numUsedRanks, rankOrder >= static_cast<int32_t>(numRanks))) {
        luaL_error(L, "invalid rank order");
        return 0;
    }

    if (CGGuildInfo::GetNumMembersInRank(rankOrder) > 0) {
        luaL_error(L, "Can't delete rank while rank has members.");
        return 0;
    }

    if (rankOrder == static_cast<int32_t>(numRanks) - 1 && (CGGuildInfo::GetPermissionsByRankID(CGGuildInfo::GetRankIDFromRankOrder(rankOrder - 1)) & 0x200000)) {
        luaL_error(L, "Lowest rank can't require authenticator.");
        return 0;
    }

    GlobalGuildDeleteRank request(rankOrder);
    ClientServices::Send(&request);
    return 0;
}

// OFFSET: 0x00931DB0/0x14069A0F0
static int32_t Script_GuildControlShiftRankUp(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlShiftRankUp(rankOrder)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t numRanks = CGGuildInfo::m_numUsedRanks;

    if (rankOrder <= 0 || rankOrder >= numRanks) {
        luaL_error(L, "invalid rank order");
        return 0;
    }

    if (rankOrder == numRanks - 1 && (CGGuildInfo::GetPermissionsByRankOrder(rankOrder - 1) & 0x200000)) {
        luaL_error(L, "Lowest rank can't require authenticator");
        return 0;
    }

    GlobalGuildShiftRank request(rankOrder, true);
    ClientServices::Send(&request);
    return 0;
}

// OFFSET: 0x00931E70/0x14069A1B0
static int32_t Script_GuildControlShiftRankDown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlShiftRankDown(rankOrder)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t numRanks = CGGuildInfo::m_numUsedRanks;

    if (rankOrder <= 0 || rankOrder >= numRanks) {
        luaL_error(L, "invalid rank order");
        return 0;
    }

    if (rankOrder + 1 == numRanks - 1 && (CGGuildInfo::GetPermissionsByRankOrder(rankOrder) & 0x200000)) {
        luaL_error(L, "Lowest rank can't require authenticator");
        return 0;
    }

    GlobalGuildShiftRank request(rankOrder, false);
    ClientServices::Send(&request);
    return 0;
}

// OFFSET: 0x00931F30/0x14069A280
static int32_t Script_GuildControlGetAllowedShifts(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlGetAllowedShifts(rankOrder)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t numRanks = CGGuildInfo::m_numUsedRanks;

    if (index <= 0 || index >= numRanks) {
        luaL_error(L, "invalid rank order");
        return 0;
    }

    if (index <= 1) {
        lua_pushboolean(L, 0);
    } else if (index == numRanks - 1 && (CGGuildInfo::GetPermissionsByRankOrder(index - 1) & 0x200000)) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    }

    if (index >= numRanks - 1) {
        lua_pushboolean(L, 0);
        return 2;
    }

    if (index + 1 == numRanks - 1 && (CGGuildInfo::GetPermissionsByRankOrder(index) & 0x200000)) {
        lua_pushboolean(L, 0);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 2;
}

// OFFSET: 0x00934650/0x14069CE90
static int32_t Script_GetPromotionRank(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGetPromotionRank(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGGuildInfo::m_numMembers) {
        GuildMemberInfo* member = CGGuildInfo::m_members[index];

        if (member) {
            int32_t rankOrder = CGGuildInfo::GetRankOrderFromRankID(member->m_rankID);

            if (rankOrder > 1) {
                int32_t promotion = rankOrder - 1;

                if (member->m_authenticated) {
                    lua_pushnumber(L, promotion);
                    return 1;
                }

                for (; promotion >= 1; promotion--) {
                    uint32_t rights;
                    int32_t rankIndex;

                    if (promotion >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) || (rankIndex = CGGuildInfo::GetRankIndexFromRankOrder(promotion)) < 0) {
                        rights = 0;
                    } else {
                        rights = CGGuildInfo::m_rankInfo[rankIndex].m_rights;
                    }

                    if (!(rights & 0x200000)) {
                        lua_pushnumber(L, promotion);
                        return 1;
                    }
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00934760/0x14069CFD0
static int32_t Script_GetDemotionRank(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGetDemotionRank(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGGuildInfo::m_numMembers) {
        GuildMemberInfo* member = CGGuildInfo::m_members[index];

        if (member) {
            int32_t rankOrder = CGGuildInfo::GetRankOrderFromRankID(member->m_rankID);
            int32_t lowestRank = CGGuildInfo::m_numUsedRanks - 1;

            if (rankOrder < lowestRank) {
                int32_t demotion = rankOrder + 1;

                if (member->m_authenticated) {
                    lua_pushnumber(L, demotion);
                    return 1;
                }

                for (; demotion <= lowestRank; demotion++) {
                    uint32_t rights;
                    int32_t rankIndex;

                    if (demotion < 0 || demotion >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) || (rankIndex = CGGuildInfo::GetRankIndexFromRankOrder(demotion)) < 0) {
                        rights = 0;
                    } else {
                        rights = CGGuildInfo::m_rankInfo[rankIndex].m_rights;
                    }

                    if (!(rights & 0x200000)) {
                        lua_pushnumber(L, demotion);
                        return 1;
                    }
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00934890/0x14069D140
static int32_t Script_IsGuildRankAssignmentAllowed(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: IsGuildRankAssignmentAllowed(player index, rank index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    GuildMemberInfo* member;

    if (index < CGGuildInfo::m_numMembers && (member = CGGuildInfo::m_members[index]) && (CGGuildInfo::GetPermissionsByRankOrder(static_cast<int32_t>(lua_tonumber(L, 2)) - 1) & 0x200000) && !member->m_authenticated) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "authenticator");
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x00936A00/0x14069FC50
static int32_t Script_SetGuildMemberRank(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildMemberRank(player index, rank index)");
        return 0;
    }

    int32_t rankOrder = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (rankOrder > 0 && static_cast<uint32_t>(rankOrder) <= 10) {
        GuildMemberInfo* member = CGGuildInfo::GetMember(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
        GlobalGuildAssignMemberRank request(player->m_obj->m_guid.m_guid, member->m_guid, rankOrder);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00934970/0x14069D230
static int32_t Script_SetGuildBankTabPermissions(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildTabPermissions(tab, index, enabled)");
        return 0;
    }

    int32_t tab = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t flag = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    int32_t enabled = StringToBOOL(L, 3, 0);
    uint32_t flags = CGGuildInfo::GetTabFlagsForSelectedRank(tab);
    uint32_t mask = s_tabFlags[flag];

    if (enabled) {
        flags |= mask;
    } else {
        flags &= ~mask;
    }

    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    if (rankOrder >= 0) {
        CGGuildInfo::SetTabFlagsForSelectedRank(tab, flags);

        GlobalGuildSetRankPermissions request;
        int32_t rankID = CGGuildInfo::GetRankIDFromRankOrder(rankOrder);
        request.m_rankOrder = rankOrder;
        request.m_rankID = rankID;
        SStrCopy(request.m_rankName, CGGuildInfo::GetRankNameFromRankID(rankID), sizeof(request.m_rankName));
        request.m_newFlags = CGGuildInfo::GetPermissionsByRankID(request.m_rankID);
        request.m_oldFlags = request.m_newFlags;
        request.m_withdrawGoldLimit = CGGuildInfo::GetWithdrawGoldLimitForSelectedRank();

        for (uint32_t i = 0; i < 8; i++) {
            request.m_tabFlags[i] = CGGuildInfo::GetTabFlagsForSelectedRank(static_cast<uint8_t>(i));
            request.m_tabWithdrawItemLimit[i] = CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(static_cast<uint8_t>(i));
        }

        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00932010/0x14069A3C0
static int32_t Script_GetGuildBankTabPermissions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildBankTabPermissions(tab)");
        return 0;
    }

    int32_t tab = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t flags = CGGuildInfo::GetTabFlagsForSelectedRank(tab);

    if (flags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (flags & 0x2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (flags & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(tab));
    return 4;
}

// PROVEN: 3b3b08b7 IDENTICAL/NONE
// OFFSET: 0x009320F0/0x14069A4A0
static int32_t Script_SetGuildBankWithdrawLimit(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_SetGuildBankWithdrawLimit(amount)");
        return 0;
    }

    int32_t amount = static_cast<int32_t>(lua_tonumber(L, 1));
    CGGuildInfo::SetWithdrawGoldLimitForSelectedRank(amount);
    return 0;
}

// PROVEN: 7bbe961d IDENTICAL/NONE
// OFFSET: 0x00932140
static int32_t Script_GetGuildBankWithdrawLimit(lua_State* L) {
    lua_pushinteger(L, CGGuildInfo::GetWithdrawGoldLimitForSelectedRank());
    return 1;
}

// OFFSET: 0x00934AE0/0x14069D3E0
static int32_t Script_SetGuildBankTabItemWithdraw(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildBankTabItemWithdraw(tab, amount)");
        return 0;
    }

    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    if (rankOrder < 0) {
        return 0;
    }

    int32_t tab = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(tab) < 8) {
        int32_t amount = static_cast<int32_t>(lua_tonumber(L, 2));

        if (amount >= 0) {
            CGGuildInfo::SetTabWithdrawItemLimitForSelectedRank(tab, amount);

            GlobalGuildSetRankPermissions request;
            int32_t rankID = CGGuildInfo::GetRankIDFromRankOrder(rankOrder);
            request.m_rankOrder = rankOrder;
            request.m_rankID = rankID;
            SStrCopy(request.m_rankName, CGGuildInfo::GetRankNameFromRankID(rankID), sizeof(request.m_rankName));
            request.m_newFlags = CGGuildInfo::GetPermissionsByRankID(request.m_rankID);
            request.m_oldFlags = request.m_newFlags;
            request.m_withdrawGoldLimit = CGGuildInfo::GetWithdrawGoldLimitForSelectedRank();

            for (uint32_t i = 0; i < 8; i++) {
                request.m_tabFlags[i] = CGGuildInfo::GetTabFlagsForSelectedRank(static_cast<uint8_t>(i));
                request.m_tabWithdrawItemLimit[i] = CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(static_cast<uint8_t>(i));
            }

            ClientServices::Send(&request);
        }
    }

    return 0;
}

// PROVEN: c693fadb IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_CloseGuildRoster(lua_State* L) {
    return 0;
}

// PROVEN: 9fb15c2e IDENTICAL/NONE
// OFFSET: 0x00937EA0/0x1406A1490
static int32_t Script_GuildRoster(lua_State* L) {
    CGGuildInfo::QueryRosterAndRanks();
    return 0;
}

// PROVEN: a70e8790 IDENTICAL/NONE
// OFFSET: 0x00932160/0x14069A570
static int32_t Script_GetGuildInfoText(lua_State* L) {
    if (!g_profanityFilterCVar || g_profanityFilterCVar->m_intValue) {
        ProfanityFilter(CGGuildInfo::m_infoText, false, false);
    }

    lua_pushstring(L, CGGuildInfo::m_infoText);
    return 1;
}

// PROVEN: 136471a4 IDENTICAL/NONE
// OFFSET: 0x00936AF0/0x14069FD60
static int32_t Script_SetGuildInfoText(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetGuildInfoText(text)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player || (player->GetGuildGUID())) {
        const char* text = lua_tolstring(L, 1, NULL);
        char scrubbed[2004];
        scrubbed[0] = 0;

        if (text && *text) {
            ScrubString(text, scrubbed, 501, sizeof(scrubbed), 0);
        }

        GlobalGuildUpdateInfoText request(scrubbed);
        ClientServices::Send(&request);
    }

    return 0;
}

// PROVEN: 46d9d647 IDENTICAL/NONE
// OFFSET: 0x00936BD0/0x14069FE50
static int32_t Script_QueryGuildEventLog(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID()) {
        GlobalGuildEventLogQuery query;
        ClientServices::Send(&query);
    }

    return 0;
}

// PROVEN: 93da4263 IDENTICAL/NONE
// OFFSET: 0x009321A0/0x14069A5C0
static int32_t Script_GetNumGuildEvents(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_numEventLogEntries);
    return 1;
}

// OFFSET: 0x00936C30/0x14069FEB0
static int32_t Script_GetGuildEventInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildEventInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    GuildEventLogEntry* entry;

    if (index >= static_cast<uint32_t>(CGGuildInfo::m_numEventLogEntries) || !(entry = &CGGuildInfo::m_eventLog[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 8;
    }

    char type[16];

    switch (entry->m_type) {
        case 1: {
            SStrCopy(type, "invite", sizeof(type));
            break;
        }

        case 2: {
            SStrCopy(type, "join", sizeof(type));
            break;
        }

        case 3: {
            SStrCopy(type, "promote", sizeof(type));
            break;
        }

        case 4: {
            SStrCopy(type, "demote", sizeof(type));
            break;
        }

        case 5: {
            SStrCopy(type, "remove", sizeof(type));
            break;
        }

        case 6: {
            SStrCopy(type, "quit", sizeof(type));
            break;
        }

        default: {
            SStrCopy(type, "none", sizeof(type));
            break;
        }
    }

    lua_pushstring(L, type);

    WOWGUID requester = WOWGUID::Make(0);
    NameCache* player = g_nameDBCache.GetRecord(entry->m_playerGUID, requester, NULL, NULL, false);

    if (player) {
        lua_pushstring(L, player->m_name);
    } else {
        lua_pushnil(L);
    }

    uint64_t other = entry->m_otherGUID;
    NameCache* otherName;

    if (!(other & 0xF000000000000000ull) && (other & 0xF07FFFFFFFFFFFFFull) && (requester = WOWGUID::Make(0), otherName = g_nameDBCache.GetRecord(other, requester, NULL, NULL, false))) {
        lua_pushstring(L, otherName->m_name);
    } else {
        lua_pushnil(L);
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        const char* rankName = CGGuildInfo::GetRankNameFromRankID(entry->m_rankID);

        if (rankName) {
            lua_pushstring(L, rankName);
        } else {
            lua_pushstring(L, "");
        }
    }

    int32_t elapsed = _time32(NULL) - entry->m_time;
    int32_t years = elapsed / 3600 / 8760;
    int32_t hours = elapsed / 3600 % 8760;
    int32_t months = hours / 720;
    hours %= 720;
    int32_t days = hours / 24;
    lua_pushnumber(L, years);
    lua_pushnumber(L, months);
    lua_pushnumber(L, days);
    lua_pushnumber(L, hours % 24);
    return 8;
}

// PROVEN: 2ce73ac9 IDENTICAL/NONE
// OFFSET: 0x00936FF0/0x1406A03B0
static int32_t Script_QueryGuildXP(lua_State* L) {
    CGGuildInfo::QueryXP();
    return 0;
}

// PROVEN: b35b82bc IDENTICAL/NONE
// OFFSET: 0x009321D0/0x14069A5E0
static int32_t Script_GetGuildLevel(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_guildLevel);
    return 1;
}

// OFFSET: 0x00937000/0x1406A03C0
static int32_t Script_GetGuildFactionGroup(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->GetPVPFactionIndex());
        return 1;
    }

    return 0;
}

// PROVEN: b1e1f9cf IDENTICAL/NONE
// OFFSET: 0x00934C40/0x14069D580
static int32_t Script_GetNumGuildPerks(lua_State* L) {
    lua_pushnumber(L, g_guildPerkSpellsDB.m_numRecords);
    return 1;
}

// OFFSET: 0x00934C70/0x14069D5A0
static int32_t Script_GetGuildPerkInfo(lua_State* L) {
    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= g_guildPerkSpellsDB.m_numRecords) {
        return 0;
    }

    GuildPerkSpellsRec* perk = reinterpret_cast<GuildPerkSpellsRec*>(g_guildPerkSpellsDB.m_records + index * sizeof(GuildPerkSpellsRec));

    if (!perk) {
        return 0;
    }

    if (perk->m_spellID < g_spellDB.m_minID || perk->m_spellID > g_spellDB.m_maxID) {
        return 0;
    }

    SpellRec* spell = reinterpret_cast<SpellRec*>(g_spellDB.m_recordsById[perk->m_spellID - g_spellDB.m_minID]);

    if (!spell) {
        return 0;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);
    lua_pushstring(L, spell->m_name);
    lua_pushnumber(L, perk->m_spellID);
    lua_pushstring(L, icon ? icon->m_textureFilename : "");
    lua_pushnumber(L, perk->m_guildLevel);
    return 4;
}

// OFFSET: 0x00937060/0x1406A0420
static int32_t Script_GetGuildMemberRecipes(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildMemberRecipes(name, skillLineID)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (!name) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (strlen(name) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    int32_t skillLineID = static_cast<int32_t>(lua_tonumber(L, 2));

    for (uint32_t i = 0; i < CGGuildInfo::GetNumMembers(1); i++) {
        GuildMemberInfo* member = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (member && !SStrCmpUTF8I(member->m_name, name, STORM_MAX_STR)) {
            GlobalGuildQueryMemberRecipes request(member->m_guid, player->GetGuildGUID().m_guid, skillLineID);
            ClientServices::Send(&request);
            return 0;
        }
    }

    return 0;
}

// PROVEN: c6877242 IDENTICAL/NONE
// OFFSET: 0x009371C0/0x1406A0600
static int32_t Script_QueryGuildRecipes(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        GlobalGuildQueryRecipes request(player->GetGuildGUID().m_guid);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x009387A0/0x1406A22A0
static int32_t Script_SearchGuildRecipes(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SearchGuildRecipes(recipe name, skillLineID (-1 for any))");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    const char* search = lua_tolstring(L, 1, NULL);

    if (!search) {
        return 0;
    }

    if (strlen(search) > 400) {
        luaL_error(L, "Recipe name too long");
        return 0;
    }

    int32_t skillLineID = static_cast<int32_t>(lua_tonumber(L, 2));
    uint8_t recipes[300];
    int32_t size = sizeof(recipes);

    if (!CGGuildInfo::GetGuildKnownRecipes(skillLineID, recipes, size)) {
        return 0;
    }

    for (int32_t i = 0; i < size; i++) {
        for (int32_t bit = 0; bit < 8; bit++) {
            if (!(recipes[i] & static_cast<uint8_t>(1 << bit))) {
                continue;
            }

            int32_t spellID = CGGuildInfo::GetSpellFromRecipeBit(skillLineID, bit + 8 * i);

            if (spellID < g_spellDB.m_minID || spellID > g_spellDB.m_maxID) {
                continue;
            }

            SpellRec* spell = reinterpret_cast<SpellRec*>(g_spellDB.m_recordsById[spellID - g_spellDB.m_minID]);

            if (spell && strstr(spell->m_name, search)) {
                lua_pushnumber(L, spellID);
            }
        }
    }

    return 0;
}

// OFFSET: 0x00938930/0x1406A2520
static int32_t Script_ViewGuildRecipes(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ViewGuildRecipes(skillLineID)");
        return 0;
    }

    int32_t skillLineID = static_cast<int32_t>(lua_tonumber(L, 1));
    HASHKEY_NONE key;
    CGGuildInfo::GuildRecipesKnown* recipes = CGGuildInfo::m_guildRecipesKnown.Ptr(skillLineID, key);

    if (recipes && recipes->m_canView) {
        CGTradeSkillInfo::OpenGuildTradeSkill(skillLineID);
    }

    return 0;
}

// OFFSET: 0x009389A0/0x1406A2580
static int32_t Script_CanViewGuildRecipes(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanViewGuildRecipes(skillLineID)");
        return 0;
    }

    HASHKEY_NONE key;
    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    CGGuildInfo::GuildRecipesKnown* recipes = CGGuildInfo::m_guildRecipesKnown.Ptr(slot, key);
    lua_pushboolean(L, recipes && recipes->m_canView);
    return 1;
}

// OFFSET: 0x00938A20/0x1406A25E0
static int32_t Script_QueryGuildMembersForRecipe(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t skillLineID;
    int32_t spellID;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        skillLineID = lua_tointeger(L, 1);
        spellID = lua_tointeger(L, 2);
    } else {
        uint32_t selection = CGTradeSkillInfo::GetSelectionIndex();

        if (selection >= CGTradeSkillInfo::m_displayList.Count()) {
            return 0;
        }

        TradeSkillInfo* skill = CGTradeSkillInfo::m_displayList[selection];

        if (!skill) {
            return 0;
        }

        spellID = skill->m_spellId;

        if (!spellID) {
            return 0;
        }

        skillLineID = CGTradeSkillInfo::m_skillLineID;
    }

    int32_t uniqueBit = CGGuildInfo::GetRecipeBitFromSpell(skillLineID, spellID);

    if (uniqueBit) {
        GlobalGuildQueryMembersForRecipe request(player->GetGuildGUID().m_guid, skillLineID, spellID, uniqueBit);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00934D60/0x14069D6F0
static int32_t Script_GetGuildRecipeInfoPostQuery(lua_State* L) {
    int32_t spellID = CGGuildInfo::m_knownRecipeSpellID;
    uint32_t count = CGGuildInfo::m_knownRecipeMembers.Count();
    lua_pushinteger(L, CGGuildInfo::m_knownRecipeSkillLineID);
    lua_pushinteger(L, spellID);
    lua_pushinteger(L, count);
    return 3;
}

// OFFSET: 0x00934DA0/0x14069D750
static int32_t Script_GetGuildRecipeMember(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildRecipeMember(index)");
        return 0;
    }

    char className[128];
    char name[48];
    bool online;
    int32_t index = lua_tointeger(L, 1) - 1;

    if (!CGGuildInfo::GetGuildRecipeInfoMember(index, name, className, online)) {
        return 0;
    }

    lua_pushstring(L, name);
    lua_pushstring(L, className);
    lua_pushboolean(L, online);
    return 3;
}

// PROVEN: 0a24ad96 IDENTICAL/NONE
// OFFSET: 0x00937EB0/0x1406A14A0
static int32_t Script_GetNumGuildTradeSkill(lua_State* L) {
    uint32_t count = 0;

    for (CGGuildInfo::GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
        CGGuildInfo::GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

        count++;

        if (!header->m_collapsed) {
            count += header->m_members.Count();
        }

        header = next;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00937F10/0x1406A1500
static int32_t Script_GetGuildTradeSkillInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildTradeSkillInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t skillLineID = 0;
    int32_t numOnline = 0;
    int32_t numMembers = 0;
    int32_t numVisible = 0;
    uint64_t guid;
    bool collapsed;

    if (!CGGuildInfo::GetTradeSkillInfo(index - 1, guid, skillLineID, collapsed, numOnline, numMembers, numVisible)) {
        return 0;
    }

    if (guid) {
        int32_t memberIndex = CGGuildInfo::GetMemberIndex(guid);

        if (memberIndex < 0) {
            return 0;
        }

        GuildMemberInfo* member = CGGuildInfo::GetMember(memberIndex);

        if (!member) {
            return 0;
        }

        lua_pushinteger(L, skillLineID);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushstring(L, member->m_name);

        ChrClassesRec* classRec = g_chrClassesDB.GetRecord(member->m_classID);
        const char* className = CGUnit_C::GetDisplayClassNameFromRecord(classRec, member->m_gender, NULL);

        if (className) {
            lua_pushstring(L, className);
        } else {
            lua_pushnil(L);
        }

        if (member->m_online) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        AreaTableRec* area = g_areaTableDB.GetRecord(member->m_areaID);

        if (area) {
            lua_pushstring(L, area->m_areaName);
        } else {
            lua_pushnil(L);
        }

        lua_pushinteger(L, CGGuildInfo::GetMemberProfessionRank(member, skillLineID));

        if (classRec) {
            lua_pushstring(L, classRec->m_filename);
        } else {
            lua_pushnil(L);
        }

        lua_pushboolean(L, member->m_status & 0x10);

        uint8_t status = member->m_status;

        if (status & 0x4) {
            lua_pushnumber(L, 2.0);
        } else if (status & 0x2) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnumber(L, 0.0);
        }

        return 15;
    }

    if (skillLineID < g_skillLineDB.m_minID || skillLineID > g_skillLineDB.m_maxID) {
        return 0;
    }

    SkillLineRec* skillLine = reinterpret_cast<SkillLineRec*>(g_skillLineDB.m_recordsById[skillLineID - g_skillLineDB.m_minID]);

    if (!skillLine) {
        return 0;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(skillLine->m_spellIconID);
    lua_pushinteger(L, skillLineID);
    lua_pushboolean(L, collapsed != 0);
    lua_pushstring(L, icon ? icon->m_textureFilename : "");
    lua_pushstring(L, skillLine->m_displayName);
    lua_pushinteger(L, numOnline);
    lua_pushinteger(L, numMembers);
    lua_pushinteger(L, numVisible);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushboolean(L, 0);
    lua_pushnumber(L, 0.0);
    return 15;
}

// OFFSET: 0x009381D0/0x1406A19E0
static int32_t Script_SortGuildTradeSkill(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SortGuildTradeSkillInfo(\"type\")");
        return 0;
    }

    GUILD_SORT_TYPE type = GUILD_SORT_NAME;
    const char* name = lua_tolstring(L, 1, NULL);

    if (!_stricmp(name, "name")) {
        type = GUILD_SORT_NAME;
    } else if (!_stricmp(name, "zone")) {
        type = GUILD_SORT_ZONE;
    } else if (!_stricmp(name, "skill")) {
        type = GUILD_SORT_SKILL;
    }

    CGGuildInfo::SortTradeSkillMembers(type);
    return 0;
}

// PROVEN: df2ca5a3 IDENTICAL/NONE
// OFFSET: 0x009397C0/0x1406A39F0
static int32_t Script_SetGuildTradeSkillItemNameFilter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetGuildTradeSkillItemNameFilter(item name)");
        return 0;
    }

    const char* filter = lua_tolstring(L, 2, NULL);

    if (filter && *filter) {
        CGGuildInfo::SetProfessionNameFilter(filter);
    }

    return 0;
}

// PROVEN: 8d7c24b3 IDENTICAL/NONE
// OFFSET: 0x00939810/0x1406A3A50
static int32_t Script_SetGuildTradeSkillCategoryFilter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGuildTradeSkillCategoryFilter(trade skill ID)");
        return 0;
    }

    CGGuildInfo::SetProfessionCategoryFilter(lua_tointeger(L, 1));
    return 0;
}

// OFFSET: 0x00939850/0x1406A3AA0
static int32_t Script_CollapseGuildTradeSkillHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseGuildTradeSkillHeader(trade skill ID)");
        return 0;
    }

    int32_t skillLineID = lua_tointeger(L, 1);
    CGGuildInfo::SetProfessionHeaderCollapsed(skillLineID, true);
    return 0;
}

// OFFSET: 0x009398A0/0x1406A3AF0
static int32_t Script_ExpandGuildTradeSkillHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandGuildTradeSkillHeader(trade skill ID)");
        return 0;
    }

    int32_t skillLineID = lua_tointeger(L, 1);
    CGGuildInfo::SetProfessionHeaderCollapsed(skillLineID, false);
    return 0;
}

// PROVEN: 7ad08d8e IDENTICAL/NONE
// OFFSET: 0x00807310/0x140631100
static int32_t Script_GuildUIEnabled(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

// PROVEN: 1e714d47 IDENTICAL/NONE
// OFFSET: 0x00932200/0x14069A600
static int32_t Script_GetNumGuildNews(lua_State* L) {
    lua_pushinteger(L, CGGuildInfo::m_numGuildNews);
    return 1;
}

// OFFSET: 0x0093A6B0/0x1406A4F60
static int32_t Script_GetGuildNewsInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildNewsInfo(index)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    GuildNewsEvent* news = index < 0 || index >= CGGuildInfo::m_numGuildNews ? NULL : &CGGuildInfo::m_guildNews[index];
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!news || !player) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(news->m_memberGUID, requester, NULL, NULL, false);
    WOWGUID playerGUID = player->m_obj->m_guid;
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(player->GetGuildGUID().m_guid, playerGUID, NULL, NULL, false);

    int32_t data = news->m_data[1];
    char buffer[128];
    const char* newsName = NULL;
    const char* newsText = NULL;
    int32_t extra = 0;

    if (!news->m_isHeader) {
        switch (news->m_type) {
            case 0:
            case 2:
            case 6:
            case 7: {
                newsName = stats ? stats->m_name : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 1:
            case 3:
            case 4:
            case 5: {
                newsName = name ? name->m_name : CGGuildInfo::GetMemberName(news->m_memberGUID);
                break;
            }

            default: {
                return 0;
            }
        }

        switch (news->m_type) {
            case 0:
            case 1: {
                AchievementRec* achievement = g_achievementDB.GetRecord(news->m_data[0]);

                if (achievement) {
                    newsText = achievement->m_title;
                }

                break;
            }

            case 2: {
                DungeonEncounterRec* encounter = g_dungeonEncounterDB.GetRecord(news->m_data[0]);

                if (encounter) {
                    data = encounter->m_mapID;
                    newsText = encounter->m_name;
                    extra = encounter->unk_0018;
                }

                break;
            }

            case 3:
            case 4:
            case 5: {
                WOWGUID itemRequester = WOWGUID::Make(0);
                ItemRecSparse* item = g_itemSparseDB.GetRecord(news->m_data[0], itemRequester, NULL, NULL, false);

                if (item) {
                    newsText = item->GetName(0);
                }

                break;
            }

            case 6: {
                SStrPrintf(buffer, "%d", news->m_data[0]);
                newsText = buffer;
                break;
            }

            default: {
                break;
            }
        }
    }

    lua_pushboolean(L, news->m_flags & 0x1);
    lua_pushboolean(L, news->m_isHeader);
    lua_pushinteger(L, news->m_type);
    lua_pushstring(L, newsName);
    lua_pushstring(L, newsText);
    lua_pushinteger(L, news->m_data[0]);
    lua_pushinteger(L, data);
    lua_pushinteger(L, extra);
    lua_pushinteger(L, news->m_date.m_weekDay);
    lua_pushinteger(L, news->m_date.m_monthDay);
    lua_pushinteger(L, news->m_date.m_month);
    lua_pushinteger(L, news->m_date.m_year);
    lua_pushinteger(L, news->m_memberList.Count());
    return 13;
}

// OFFSET: 0x00934E30/0x14069D7F0
static int32_t Script_GetGuildNewsMemberName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildNewsMemberName(index, nameIndex)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;

    if (index < 0 || index >= CGGuildInfo::m_numGuildNews) {
        return 0;
    }

    GuildNewsEvent* news = &CGGuildInfo::m_guildNews[index];

    if (!news) {
        return 0;
    }

    int32_t nameIndex = lua_tointeger(L, 2) - 1;

    if (nameIndex < 0 || nameIndex > static_cast<int32_t>(news->m_memberList.Count())) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    NameCache* name = g_nameDBCache.GetRecord(news->m_memberList[nameIndex], requester, NULL, NULL, false);

    if (name) {
        lua_pushstring(L, name->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 34074a5a IDENTICAL/NONE
// OFFSET: 0x00937220/0x1406A0660
static int32_t Script_QueryGuildNews(lua_State* L) {
    CGGuildInfo::QueryNews();
    return 0;
}

// OFFSET: 0x00937230/0x1406A0670
static int32_t Script_GuildNewsSetSticky(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GuildNewsSetSticky(index, bool)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID() || !(CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) & 0x1000)) {
        return 0;
    }

    GuildNewsEvent* news = CGGuildInfo::GetGuildNewsEvent(lua_tointeger(L, 1) - 1);

    if (!news || news->m_isHeader) {
        return 0;
    }

    if (lua_tointeger(L, 2)) {
        if (!(news->m_flags & 0x1)) {
            GlobalGuildNewsUpdateSticky request(player->GetGuildGUID().m_guid, news->m_id, true);
            ClientServices::Send(&request);
        }
    } else if (news->m_flags & 0x1) {
        GlobalGuildNewsUpdateSticky request(player->GetGuildGUID().m_guid, news->m_id, false);
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x0093A960/0x1406A5370
static int32_t Script_SetGuildNewsFilter(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildNewsFilter(index, bool)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    int32_t value = lua_tointeger(L, 2);
    int32_t current = CGGuildInfo::m_guildNewsFilter;
    int32_t filter;

    if (value) {
        filter = current | (1 << index);
    } else {
        filter = current & ~(1 << index);
    }

    if (filter != current) {
        CGGuildInfo::SetGuildNewsFilter(filter);
    }

    return 0;
}

// PROVEN: 89c07d1d IDENTICAL/NONE
// OFFSET: 0x00932220/0x14069A620
static int32_t Script_GetGuildNewsFilters(lua_State* L) {
    int32_t filter = CGGuildInfo::m_guildNewsFilter;
    uint32_t mask = 1;

    for (uint32_t i = 8; i; i--) {
        lua_pushboolean(L, filter & mask);
        mask = (mask << 1) | (mask >> 31);
    }

    return 8;
}

// OFFSET: 0x0093A9E0/0x1406A5410
static int32_t Script_GuildNewsSort(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildNewsSort(byDate)");
        return 0;
    }

    bool byDate = lua_tointeger(L, 1) != 0;
    CGGuildInfo::m_newsSortByDate = byDate;

    if (byDate) {
        CGGuildInfo::SortGuildNewsByDate();
    } else {
        CGGuildInfo::SortGuildNewsHeaders();
    }

    return 0;
}

// OFFSET: 0x009373C0/0x1406A0880
static int32_t Script_RequestGuildPartyState(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID()) {
        PlayerRequestGuildPartyState request(player->GetGuildGUID().m_guid);
        ClientServices::Send(&request);
    }

    return 0;
}

// PROVEN: 2e72e5db IDENTICAL/NONE
// OFFSET: 0x00932260/0x14069A680
int32_t CGGuildInfo::Script_InGuildParty(lua_State* L) {
    lua_pushboolean(L, CGGuildInfo::m_inGuildParty);
    lua_pushnumber(L, CGGuildInfo::m_numGuildGroupMembers);
    lua_pushnumber(L, CGGuildInfo::m_numGuildGroupRequired);
    lua_pushnumber(L, CGGuildInfo::m_guildXPEarnedMult);
    return 4;
}

// PROVEN: 0bdffcc8 IDENTICAL/NONE
// OFFSET: 0x00937370/0x1406A0820
static int32_t Script_RequestGuildRewards(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        WorldRequestGuildRewardsList request(CGGuildInfo::m_guildRewardItemListVersion);
        ClientServices::Send(&request);
    }

    return 0;
}

// PROVEN: b0edf6c6 IDENTICAL/NONE
// OFFSET: 0x00938270/0x1406A1A90
static int32_t Script_GetNumGuildRewards(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::GetNumGuildRewards());
    return 1;
}

// OFFSET: 0x0093AA40/0x1406A5480
static int32_t Script_GetGuildRewardInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildRewardInfo(index)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index >= CGGuildInfo::GetNumGuildRewards()) {
        return 0;
    }

    JamGuildRewardItem* reward = CGGuildInfo::GetGuildRewardItem(index);

    if (!reward) {
        return 0;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(reward->m_itemID, requester, NULL, NULL, false);

    if (!sparse) {
        return 0;
    }

    ItemRec* item = sparse->GetNonSparse();

    if (!item) {
        return 0;
    }

    lua_pushinteger(L, CGGuildInfo::GetRewardMissingAchievement(reward));
    lua_pushinteger(L, reward->m_itemID);
    lua_pushstring(L, sparse->GetName(0));

    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, texture);
    lua_pushnumber(L, reward->m_minGuildRep + 1);
    lua_pushnumber(L, static_cast<double>(reward->m_cost));
    return 6;
}

// PROVEN: 0ff2553e IDENTICAL/NONE
// OFFSET: 0x00932450/0x14069A8F0
static int32_t Script_GetGuildNewsSort(lua_State* L) {
    lua_pushboolean(L, CGGuildInfo::m_newsSortByDate);
    return 1;
}

// PROVEN: 8e260315 IDENTICAL/NONE
// OFFSET: 0x009374E0/0x1406A09C0
static int32_t Script_GetGuildLevelEnabled(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushboolean(L, (player->m_player->m_playerFlags >> 28) & 1);
    return 1;
}

// PROVEN: c91d379b IDENTICAL/NONE
// OFFSET: 0x009322C0/0x14069A6E0
static int32_t Script_RequestGuildChallengeInfo(lua_State* L) {
    GlobalGuildChallengeUpdateRequest request;
    ClientServices::Send(&request);
    return 0;
}

// PROVEN: 961909e9 IDENTICAL/NONE
// OFFSET: 0x009322F0/0x14069A710
static int32_t Script_GetNumGuildChallenges(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (CGGuildInfo::m_challengeMaxCount[i] > 0) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00932350/0x14069A750
static int32_t Script_GetGuildChallengeInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildChallengeInfo(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index > 3) {
        return 0;
    }

    uint32_t current = 0;

    for (int32_t i = 0; i < 4; i++) {
        if (CGGuildInfo::m_challengeMaxCount[i] <= 0) {
            continue;
        }

        if (current != index) {
            current++;
            continue;
        }

        lua_pushnumber(L, i);
        lua_pushnumber(L, CGGuildInfo::m_challengeCurrentCount[i]);
        lua_pushnumber(L, CGGuildInfo::m_challengeMaxCount[i]);
        lua_pushnumber(L, CGGuildInfo::m_challengeGold[i]);
        lua_pushnumber(L, CGGuildInfo::m_challengeMaxLevelGold[i]);
        lua_pushnumber(L, CGGuildInfo::m_challengeXP[i]);
        return 6;
    }

    return 0;
}

// PROVEN: b8760cc8 IDENTICAL/NONE
// OFFSET: 0x009382B0/0x1406A1AC0
static int32_t Script_ReplaceGuildMaster(lua_State* L) {
    if (CGGuildInfo::CanReplaceGuildMaster()) {
        GlobalGuildReplaceGuildMaster request;
        ClientServices::Send(&request);
    }

    return 0;
}

// PROVEN: 162fac4c IDENTICAL/NONE
// OFFSET: 0x00937640/0x1406A0C00
static int32_t Script_CanReplaceGuildMaster(lua_State* L) {
    lua_pushboolean(L, CGGuildInfo::CanReplaceGuildMaster());
    return 1;
}

// OFFSET: 0x00934F70/0x14069D900
static int32_t Script_GuildMasterAbsent(lua_State* L) {
    float lastSave = 0.0f;

    for (uint32_t i = 0; i < CGGuildInfo::m_members.Count(); i++) {
        int32_t index = -1;

        for (uint32_t j = 0; j < CGGuildInfo::m_numUsedRanks; j++) {
            if (CGGuildInfo::m_rankInfo[j].m_rankID == CGGuildInfo::m_members[i]->m_rankID) {
                index = j;
                break;
            }
        }

        if (index >= 0 && index < static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) && !CGGuildInfo::m_rankInfo[index].m_rankOrder) {
            lastSave = CGGuildInfo::m_members[i]->m_lastSave;
            break;
        }
    }

    bool absent = lastSave > CGGuildInfo::m_guildMasterAbsentDays;
    lua_pushboolean(L, absent);
    return 1;
}

// PROVEN: 8b6bf0a0 IDENTICAL/NONE
// OFFSET: 0x00932430/0x14069A880
static int32_t Script_GetGuildRenameRequired(lua_State* L) {
    lua_pushboolean(L, CGGuildInfo::m_guildFlags & 0x1);
    return 1;
}

// OFFSET: 0x00937430/0x1406A08F0
static int32_t Script_SubmitRequiredGuildRename(lua_State* L) {
    if (!(CGGuildInfo::m_guildFlags & 0x1)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->IsGuildLeader()) {
        return 0;
    }

    if (lua_isstring(L, 1)) {
        GlobalGuildChangeNameRequest request;
        SStrCopy(request.m_newName, lua_tolstring(L, 1, NULL), sizeof(request.m_newName));
        ClientServices::Send(&request);
    }

    return 0;
}

// OFFSET: 0x00937660/0x1406A0C30
static int32_t Script_GuildInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    WOWGUID requester = player->m_obj->m_guid;
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(player->GetGuildGUID().m_guid, requester, NULL, NULL, false);

    if (stats) {
        char text[3000];
        SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_NAME_TEMPLATE", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(stats->m_name));
        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_INFO_TEMPLATE", -1, GENDER_NOT_APPLICABLE), CGGuildInfo::m_createDate.m_month + 1, CGGuildInfo::m_createDate.m_monthDay + 1, CGGuildInfo::m_createDate.m_year + 2000, CGGuildInfo::m_numMembers, CGGuildInfo::m_numAccounts);
        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    }

    return 0;
}

// STATIC: 0x00D11CB8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumGuildMembers", &Script_GetNumGuildMembers },
    { "GetGuildRosterMOTD", &Script_GetGuildRosterMOTD },
    { "GetGuildRosterInfo", &Script_GetGuildRosterInfo },
    { "GuildRosterSendSoR", &Script_GuildRosterSendSoR },
    { "GetGuildRosterLastOnline", &Script_GetGuildRosterLastOnline },
    { "GetGuildRosterContribution", &Script_GetGuildRosterContribution },
    { "GetGuildRosterLargestContribution", &CGGuildInfo::Script_GetGuildRosterLargestContribution },
    { "GetGuildRosterLargestAchievementPoints", &CGGuildInfo::Script_GetGuildRosterLargestAchievements },
    { "GuildRosterSetPublicNote", &Script_GuildRosterSetPublicNote },
    { "GuildRosterSetOfficerNote", &Script_GuildRosterSetOfficerNote },
    { "SetGuildRosterSelection", &Script_SetGuildRosterSelection },
    { "GetGuildRosterSelection", &Script_GetGuildRosterSelection },
    { "CanGuildPromote", &Script_CanGuildPromote },
    { "CanGuildDemote", &Script_CanGuildDemote },
    { "CanGuildInvite", &Script_CanGuildInvite },
    { "CanGuildRemove", &Script_CanGuildRemove },
    { "CanEditMOTD", &Script_CanEditMOTD },
    { "CanEditPublicNote", &Script_CanEditPublicNote },
    { "CanEditOfficerNote", &Script_CanEditOfficerNote },
    { "CanViewOfficerNote", &Script_CanViewOfficerNote },
    { "CanEditGuildInfo", &Script_CanEditGuildInfo },
    { "CanGuildBankRepair", &Script_CanGuildBankRepair },
    { "IsInAuthenticatedRank", &Script_IsInAuthenticatedRank },
    { "GetNumMembersInRank", &Script_GetNumMembersInRank },
    { "CanEditGuildTabInfo", &Script_CanEditGuildTabInfo },
    { "CanEditGuildEvent", &Script_CanEditGuildEvent },
    { "SortGuildRoster", &Script_SortGuildRoster },
    { "SetGuildRosterShowOffline", &Script_SetGuildRosterShowOffline },
    { "GetGuildRosterShowOffline", &Script_GetGuildRosterShowOffline },
    { "GuildControlGetNumRanks", &Script_GuildControlGetNumRanks },
    { "GuildControlGetRankName", &Script_GuildControlGetRankName },
    { "GuildControlSetRank", &Script_GuildControlSetRank },
    { "GuildControlGetRankFlags", &Script_GuildControlGetRankFlags },
    { "GuildControlSetRankFlag", &Script_GuildControlSetRankFlag },
    { "GuildControlSaveRank", &Script_GuildControlSaveRank },
    { "GuildControlAddRank", &Script_GuildControlAddRank },
    { "GuildControlDelRank", &Script_GuildControlDelRank },
    { "GuildControlShiftRankUp", &Script_GuildControlShiftRankUp },
    { "GuildControlShiftRankDown", &Script_GuildControlShiftRankDown },
    { "GuildControlGetAllowedShifts", &Script_GuildControlGetAllowedShifts },
    { "GetPromotionRank", &Script_GetPromotionRank },
    { "GetDemotionRank", &Script_GetDemotionRank },
    { "IsGuildRankAssignmentAllowed", &Script_IsGuildRankAssignmentAllowed },
    { "SetGuildMemberRank", &Script_SetGuildMemberRank },
    { "SetGuildBankTabPermissions", &Script_SetGuildBankTabPermissions },
    { "GetGuildBankTabPermissions", &Script_GetGuildBankTabPermissions },
    { "SetGuildBankWithdrawGoldLimit", &Script_SetGuildBankWithdrawLimit },
    { "GetGuildBankWithdrawGoldLimit", &Script_GetGuildBankWithdrawLimit },
    { "SetGuildBankTabItemWithdraw", &Script_SetGuildBankTabItemWithdraw },
    { "CloseGuildRoster", &Script_CloseGuildRoster },
    { "GuildRoster", &Script_GuildRoster },
    { "GetGuildInfoText", &Script_GetGuildInfoText },
    { "SetGuildInfoText", &Script_SetGuildInfoText },
    { "QueryGuildEventLog", &Script_QueryGuildEventLog },
    { "GetNumGuildEvents", &Script_GetNumGuildEvents },
    { "GetGuildEventInfo", &Script_GetGuildEventInfo },
    { "QueryGuildXP", &Script_QueryGuildXP },
    { "GetGuildLevel", &Script_GetGuildLevel },
    { "GetGuildFactionGroup", &Script_GetGuildFactionGroup },
    { "GetNumGuildPerks", &Script_GetNumGuildPerks },
    { "GetGuildPerkInfo", &Script_GetGuildPerkInfo },
    { "GetGuildMemberRecipes", &Script_GetGuildMemberRecipes },
    { "QueryGuildRecipes", &Script_QueryGuildRecipes },
    { "SearchGuildRecipes", &Script_SearchGuildRecipes },
    { "ViewGuildRecipes", &Script_ViewGuildRecipes },
    { "CanViewGuildRecipes", &Script_CanViewGuildRecipes },
    { "QueryGuildMembersForRecipe", &Script_QueryGuildMembersForRecipe },
    { "GetGuildRecipeInfoPostQuery", &Script_GetGuildRecipeInfoPostQuery },
    { "GetGuildRecipeMember", &Script_GetGuildRecipeMember },
    { "GetNumGuildTradeSkill", &Script_GetNumGuildTradeSkill },
    { "GetGuildTradeSkillInfo", &Script_GetGuildTradeSkillInfo },
    { "SortGuildTradeSkill", &Script_SortGuildTradeSkill },
    { "SetGuildTradeSkillItemNameFilter", &Script_SetGuildTradeSkillItemNameFilter },
    { "SetGuildTradeSkillCategoryFilter", &Script_SetGuildTradeSkillCategoryFilter },
    { "CollapseGuildTradeSkillHeader", &Script_CollapseGuildTradeSkillHeader },
    { "ExpandGuildTradeSkillHeader", &Script_ExpandGuildTradeSkillHeader },
    { "GuildUIEnabled", &Script_GuildUIEnabled },
    { "GetNumGuildNews", &Script_GetNumGuildNews },
    { "GetGuildNewsInfo", &Script_GetGuildNewsInfo },
    { "GetGuildNewsMemberName", &Script_GetGuildNewsMemberName },
    { "QueryGuildNews", &Script_QueryGuildNews },
    { "GuildNewsSetSticky", &Script_GuildNewsSetSticky },
    { "SetGuildNewsFilter", &Script_SetGuildNewsFilter },
    { "GetGuildNewsFilters", &Script_GetGuildNewsFilters },
    { "GuildNewsSort", &Script_GuildNewsSort },
    { "RequestGuildPartyState", &Script_RequestGuildPartyState },
    { "InGuildParty", &CGGuildInfo::Script_InGuildParty },
    { "RequestGuildRewards", &Script_RequestGuildRewards },
    { "GetNumGuildRewards", &Script_GetNumGuildRewards },
    { "GetGuildRewardInfo", &Script_GetGuildRewardInfo },
    { "GetGuildNewsSort", &Script_GetGuildNewsSort },
    { "GetGuildLevelEnabled", &Script_GetGuildLevelEnabled },
    { "RequestGuildChallengeInfo", &Script_RequestGuildChallengeInfo },
    { "GetNumGuildChallenges", &Script_GetNumGuildChallenges },
    { "GetGuildChallengeInfo", &Script_GetGuildChallengeInfo },
    { "ReplaceGuildMaster", &Script_ReplaceGuildMaster },
    { "CanReplaceGuildMaster", &Script_CanReplaceGuildMaster },
    { "GuildMasterAbsent", &Script_GuildMasterAbsent },
    { "GetGuildRenameRequired", &Script_GetGuildRenameRequired },
    { "SubmitRequiredGuildRename", &Script_SubmitRequiredGuildRename },
    { "GuildInfo", &Script_GuildInfo },
};

// PROVEN: 6ff30541 IDENTICAL/NONE
// OFFSET: 0x00932470/0x14069A910
void GuildInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 5ad6003a IDENTICAL/NONE
// OFFSET: 0x009324A0/0x14069A960
void GuildInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
