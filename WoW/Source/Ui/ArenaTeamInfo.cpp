#include "ArenaTeamInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Event/FrameTime.h"
#include <cstdlib>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00C04154
static const int32_t s_arenaBracketTypes[3] = { 2, 3, 5 };

// STATIC: 0x00D15300
int32_t CGArenaTeamInfo::m_showOffline = 1;
// STATIC: 0x00F4CCD0
ArenaTeam CGArenaTeamInfo::m_arenaTeams[3];

// STATIC: 0x00F4CC80
uint32_t CGArenaTeamInfo::m_nextAllowedQuery[3];

// STATIC: 0x00F4CC8C
ArenaTeamSort CGArenaTeamInfo::m_sortOrder[7];

// STATIC: 0x00F4CCC8
WOWGUID CGArenaTeamInfo::m_selectedMember;

// OFFSET: #0x005A1F00
ArenaTeam* CGArenaTeamInfo::GetArenaTeam(uint32_t index) {
    if (index >= 3) {
        return NULL;
    }

    return &CGArenaTeamInfo::m_arenaTeams[index];
}

// OFFSET: 0x009A90E0
void CGArenaTeamInfo::UpdateRoster(uint32_t index, int32_t reset) {
    if (index >= 3) {
        return;
    }

    if (reset) {
        CGArenaTeamInfo::m_nextAllowedQuery[index] = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_ROSTER_UPDATE, "%d", 1);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_ROSTER_UPDATE, NULL);
    }
}

// PROVEN: 439683be IDENTICAL/NONE
// OFFSET: 0x009A9390
ArenaTeamMemberInfo* CGArenaTeamInfo::SelectMember(uint32_t team, uint32_t index) {
    if (team >= 3 || index >= CGArenaTeamInfo::m_arenaTeams[team].m_numMembers) {
        CGArenaTeamInfo::m_selectedMember = WOWGUID();
        return NULL;
    }

    ArenaTeamMemberInfo* member = CGArenaTeamInfo::m_arenaTeams[team].m_members.m_data[index];
    CGArenaTeamInfo::m_selectedMember = member->m_guid;
    return member;
}

// OFFSET: 0x009A93E0
static int32_t QSortRoster(const void* a, const void* b) {
    const ArenaTeamMemberInfo* member1 = *static_cast<ArenaTeamMemberInfo* const*>(a);
    const ArenaTeamMemberInfo* member2 = *static_cast<ArenaTeamMemberInfo* const*>(b);

    if (!CGArenaTeamInfo::m_showOffline && (!member1->m_online || !member2->m_online) && member1->m_online != member2->m_online) {
        return member1->m_online ? -1 : 1;
    }

    for (uint32_t i = 0; i < 7; i++) {
        int32_t result;
        uint32_t value1;
        uint32_t value2;
        ArenaTeamSort* sort = i < 7 ? &CGArenaTeamInfo::m_sortOrder[i] : NULL;

        if (!sort) {
            continue;
        }

        switch (sort->m_column) {
            case 0:
                result = SStrCmp(member1->m_name, member2->m_name, STORM_MAX_STR);
                break;
            case 1: {
                ChrClassesRec* class1 = g_chrClassesDB.GetRecord(member1->m_class);
                ChrClassesRec* class2 = g_chrClassesDB.GetRecord(member2->m_class);

                if (!class1 || !class2) {
                    continue;
                }

                result = SStrSortUTF8I(class1->m_name, class2->m_name, STORM_MAX_STR);
                break;
            }
            case 2:
                value1 = member1->m_weekGames;
                value2 = member2->m_weekGames;

                if (value2 == value1) {
                    continue;
                }

                result = value2 < value1 ? -1 : 1;
                break;
            case 3:
                value1 = member1->m_weekWins;
                value2 = member2->m_weekWins;

                if (value2 == value1) {
                    continue;
                }

                result = value2 < value1 ? -1 : 1;
                break;
            case 4:
                value1 = member1->m_seasonGames;
                value2 = member2->m_seasonGames;

                if (value2 == value1) {
                    continue;
                }

                result = value2 < value1 ? -1 : 1;
                break;
            case 5:
                value1 = member1->m_seasonWins;
                value2 = member2->m_seasonWins;

                if (value2 == value1) {
                    continue;
                }

                result = value2 < value1 ? -1 : 1;
                break;
            case 6:
                value1 = member1->m_personalRating;
                value2 = member2->m_personalRating;

                if (value2 == value1) {
                    continue;
                }

                result = value2 < value1 ? -1 : 1;
                break;
            default:
                continue;
        }

        if (!result) {
            continue;
        }

        if (sort->m_reverse) {
            result = -result;
        }

        return result;
    }

    return 0;
}

// OFFSET: 0x009A9570
int32_t Packet_SMSG_ARENA_TEAM_STATS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t teamId;
    msg->Get(teamId);
    for (uint32_t index = 0; index < 3; index++) {
        if (CGArenaTeamInfo::m_arenaTeams[index].m_teamId == teamId) {
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_rating);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_weekGames);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_weekWins);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_seasonGames);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_seasonWins);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_rank);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_UPDATE, NULL);
            return 1;
        }
    }

    return 1;
}

// OFFSET: 0x009AA530
void ArenaTeam::FreeMemberArray() {
    this->m_teamId = 0;

    for (uint32_t i = 0; i < this->m_members.m_count; i++) {
        ArenaTeamMemberInfo* member = this->m_members.m_data[i];

        if (member) {
            SMemFree(member, "delete", -1, 0);
        }

        this->m_members.m_data[i] = NULL;
    }

    this->m_members.SetCount(0);
    this->m_type = 0;
    this->m_numMembers = 0;
    this->m_numOnline = 0;
    this->m_rating = 0;
    this->m_weekGames = 0;
    this->m_weekWins = 0;
    this->m_seasonGames = 0;
    this->m_seasonWins = 0;
    this->m_rank = 0;
}

// OFFSET: 0x009AA5F0
int32_t Packet_SMSG_ARENA_TEAM_ROSTER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t teamId;
    msg->Get(teamId);
    uint8_t hasRatings = 0;
    msg->Get(hasRatings);
    uint32_t index;

    for (index = 0; index < 3; index++) {
        if (CGArenaTeamInfo::m_arenaTeams[index].m_teamId == teamId) {
            break;
        }
    }

    if (index >= 3) {
        return 1;
    }

    uint32_t oldCount = CGArenaTeamInfo::m_arenaTeams[index].m_members.m_count;
    CGArenaTeamInfo::m_arenaTeams[index].m_numOnline = 0;
    msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_numMembers);
    msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_type);

    if (CGArenaTeamInfo::m_arenaTeams[index].m_numMembers > oldCount) {
        CGArenaTeamInfo::m_arenaTeams[index].m_members.SetCount(CGArenaTeamInfo::m_arenaTeams[index].m_numMembers);

        for (uint32_t i = oldCount; i < CGArenaTeamInfo::m_arenaTeams[index].m_numMembers; i++) {
            CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i] = static_cast<ArenaTeamMemberInfo*>(SMemAlloc(sizeof(ArenaTeamMemberInfo), __FILE__, __LINE__, 0));
        }
    }

    for (uint32_t i = 0; i < CGArenaTeamInfo::m_arenaTeams[index].m_numMembers; i++) {
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_guid.m_guid);
        uint8_t online;
        msg->Get(online);
        CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_online = online != 0;
        msg->GetString(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_name, sizeof(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_name));
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_rank);
        uint8_t level;
        uint8_t classId;
        msg->Get(level);
        msg->Get(classId);
        CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_level = level;
        CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_class = classId;
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_weekGames);
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_weekWins);
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_seasonGames);
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_seasonWins);
        msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_personalRating);

        if (hasRatings) {
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_matchmakerRatingBefore);
            msg->Get(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_matchmakerRatingAfter);
        } else {
            CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_matchmakerRatingBefore = 0.0f;
            CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_matchmakerRatingAfter = 0.0f;
        }

        if (CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data[i]->m_online) {
            CGArenaTeamInfo::m_arenaTeams[index].m_numOnline++;
        }
    }

    qsort(CGArenaTeamInfo::m_arenaTeams[index].m_members.m_data, CGArenaTeamInfo::m_arenaTeams[index].m_numMembers, sizeof(ArenaTeamMemberInfo*), QSortRoster);
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_ROSTER_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x009AA8C0
void CGArenaTeamInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_ROSTER, Packet_SMSG_ARENA_TEAM_ROSTER, NULL);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_STATS, Packet_SMSG_ARENA_TEAM_STATS, NULL);

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::m_arenaTeams[i].FreeMemberArray();
    }

    CGArenaTeamInfo::m_selectedMember = WOWGUID();

    for (uint32_t i = 0; i < 7; i++) {
        CGArenaTeamInfo::m_sortOrder[i].m_reverse = 0;
    }

    CGArenaTeamInfo::m_showOffline = 1;

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::m_nextAllowedQuery[i] = 0;
    }

    for (int32_t i = 0; i < 7; i++) {
        CGArenaTeamInfo::m_sortOrder[i].m_column = i;
    }
}

// OFFSET: 0x009A9F70
bool CGArenaTeamInfo::CheckPersonalAndTeamRating(uint32_t rating, int32_t bracket) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    if (player->m_player->m_playerFieldBattlegroundRating >= rating && static_cast<uint32_t>(s_arenaBracketTypes[bracket]) <= 3) {
        return true;
    }

    for (int32_t i = 0; i < 3; i++) {
        uint32_t* info = static_cast<uint32_t>(i) < 3 ? &player->m_player->m_playerFieldArenaTeamInfo11[i * 7] : NULL;

        if (!info) {
            continue;
        }

        if (static_cast<int32_t>(info[1]) < s_arenaBracketTypes[bracket] || info[6] < rating) {
            continue;
        }

        for (int32_t j = 0; j < 3; j++) {
            ArenaTeam* team = CGArenaTeamInfo::GetArenaTeam(j);

            if (team && team->m_type == info[1] && team->m_rating >= rating) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009A9630
void CGArenaTeamInfo::SetRosterShowOffline(int32_t showOffline) {
    if (showOffline == CGArenaTeamInfo::m_showOffline) {
        return;
    }

    CGArenaTeamInfo::m_showOffline = showOffline;

    for (uint32_t i = 0; i < 3; i++) {
        if (CGArenaTeamInfo::m_arenaTeams[i].m_numMembers) {
            qsort(CGArenaTeamInfo::m_arenaTeams[i].m_members.m_data, CGArenaTeamInfo::m_arenaTeams[i].m_numMembers, sizeof(ArenaTeamMemberInfo*), QSortRoster);

            if (i < 3) {
                FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_ROSTER_UPDATE, NULL);
            }
        }
    }
}

// OFFSET: 0x009A96A0
void CGArenaTeamInfo::SortRoster(int32_t column) {
    for (uint32_t i = 0; i < 7; i++) {
        if (CGArenaTeamInfo::m_sortOrder[i].m_column != column) {
            continue;
        }

        int32_t reverse = CGArenaTeamInfo::m_sortOrder[i].m_reverse;

        if (!i) {
            reverse = reverse == 0;
        }

        for (uint32_t j = i; j > 0; j--) {
            CGArenaTeamInfo::m_sortOrder[j].m_column = CGArenaTeamInfo::m_sortOrder[j - 1].m_column;
            CGArenaTeamInfo::m_sortOrder[j].m_reverse = CGArenaTeamInfo::m_sortOrder[j - 1].m_reverse;
        }

        CGArenaTeamInfo::m_sortOrder[0].m_column = column;
        CGArenaTeamInfo::m_sortOrder[0].m_reverse = reverse;
        break;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (CGArenaTeamInfo::m_arenaTeams[i].m_numMembers) {
            qsort(CGArenaTeamInfo::m_arenaTeams[i].m_members.m_data, CGArenaTeamInfo::m_arenaTeams[i].m_numMembers, sizeof(ArenaTeamMemberInfo*), QSortRoster);

            if (i < 3) {
                FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_ROSTER_UPDATE, NULL);
            }
        }
    }
}

// OFFSET: 0x009A9750
int32_t CGArenaTeamInfo::GetSelectedMemberIndex(uint32_t team) {
    if (team >= 3) {
        return -1;
    }

    if (!CGArenaTeamInfo::m_selectedMember) {
        return -1;
    }

    for (uint32_t i = 0; i < CGArenaTeamInfo::m_arenaTeams[team].m_numMembers; i++) {
        if (CGArenaTeamInfo::m_arenaTeams[team].m_members.m_data[i]->m_guid == CGArenaTeamInfo::m_selectedMember) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x009A9DB0
void CGArenaTeamInfo::QueryRoster(uint32_t index) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || index >= 3) {
        return;
    }

    uint32_t* info = &player->m_player->m_playerFieldArenaTeamInfo11[index * 7];

    if (!info || !*info) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGArenaTeamInfo::m_nextAllowedQuery[index] && static_cast<int32_t>(now - CGArenaTeamInfo::m_nextAllowedQuery[index]) < 0) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_ROSTER));
    msg.Put(*info);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGArenaTeamInfo::m_nextAllowedQuery[index] = now + 500;
}

// OFFSET: 0x009A9EB0
void CGArenaTeamInfo::UpdateTeams(int32_t signal) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        uint32_t* info = i < 3 ? &player->m_player->m_playerFieldArenaTeamInfo11[i * 7] : NULL;

        if (!info) {
            continue;
        }

        if (CGArenaTeamInfo::m_arenaTeams[i].m_teamId && !*info) {
            g_arenaTeamCache.Invalidate(CGArenaTeamInfo::m_arenaTeams[i].m_teamId);
        }

        CGArenaTeamInfo::m_arenaTeams[i].m_teamId = *info;

        if (*info) {
            CGArenaTeamInfo::QueryRoster(i);
        }
    }

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_UPDATE, NULL);
    }
}

// PROVEN: f5f35875 IDENTICAL/NONE
// OFFSET: 0x009AA590/0x1407318A0
void CGArenaTeamInfo::EnterWorld() {
    CGArenaTeamInfo::UpdateTeams(0);
}

// OFFSET: 0x009AA5A0
void CGArenaTeamInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_ROSTER);
    ClientServices::ClearMessageHandler(SMSG_ARENA_TEAM_STATS);

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::m_arenaTeams[i].FreeMemberArray();
    }
}
