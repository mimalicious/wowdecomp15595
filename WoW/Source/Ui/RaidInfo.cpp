#include "RaidInfo.h"
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F33EF8
WOWGUID CGRaidInfo::m_raidTargets[28];

// STATIC: 0x00F33FD8
CGRaidMember* CGRaidInfo::m_members[40];

// STATIC: 0x00F34078
uint32_t CGRaidInfo::m_numMembers;

// STATIC: 0x00F3407C
uint32_t CGRaidInfo::m_realNumMembers;

// STATIC: 0x00F34080
WOWGUID CGRaidInfo::m_selectedMember;

// STATIC: 0x00F34088
uint32_t CGRaidInfo::m_raidMarkers;

// STATIC: 0x00F3408C
static int32_t s_pendingNameQueries;

// STATIC: 0x00F34090
uint32_t CGRaidInfo::m_readyCheckTime;

// STATIC: 0x00F34094
uint32_t CGRaidInfo::m_readyCheckTimer;

// STATIC: 0x00F34098
WOWGUID CGRaidInfo::m_readyCheckInitiator;

// STATIC: 0x00F340A0
uint8_t CGRaidInfo::m_isInFakeRaid;

// STATIC: 0x00F340A1
uint8_t CGRaidInfo::m_everyoneIsAssistant;

// STATIC: 0x00F340A4
TSGrowableArray<RaidInstanceInfo> s_savedInstances;

// STATIC: 0x00D11AB0
uint8_t CGRaidInfo::m_rosterCounter = 0xFF;

// OFFSET: 0x0092CB10
static void RaidMemberRemovedNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false)) {
        CGGameUI::DisplayError(ERR_RAID_MEMBER_REMOVED_S, name->m_name);
    }
}

// OFFSET: 0x0092CB60
static void SignalReadyCheckConfirmEvents(const WOWGUID& guid, int32_t ready) {
    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_READY_CHECK_CONFIRM, "%s%b", tokens[i], ready);
    }
}

// PROVEN: e4cec15d IDENTICAL/NONE
// OFFSET: 0x0092CBB0/0x140693B60
void CGRaidInfo::UpdateRealStats(uint32_t count, WOWGUID leader) {
    if (count <= 40) {
        CGRaidInfo::m_realNumMembers = count;
    }

    CGPartyInfo::m_realLeader = leader;
}

// OFFSET: 0x0092CBE0
int32_t Packet_MSG_RAID_READY_CHECK_FINISHED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGRaidInfo::m_readyCheckTime = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_READY_CHECK_FINISHED, "%b", 0);
    CGRaidInfo::m_readyCheckInitiator = WOWGUID();
    return 1;
}

// OFFSET: 0x0092CC20
int32_t Packet_SMSG_READY_CHECK_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_READY_CHECK_THROTTLED);
    return 1;
}

// OFFSET: 0x0092CC40
int32_t Packet_MSG_NOTIFY_PARTY_SQUELCH(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t squelched;
    msg->Get(squelched);

    if (!squelched) {
        CGGameUI::DisplayError(ERR_PLAYER_UNSILENCED);
    } else {
        CGGameUI::DisplayError(ERR_PLAYER_SILENCED);
    }

    return 1;
}

// OFFSET: 0x0092CC90
int32_t Packet_SMSG_ECHO_PARTY_SQUELCH(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t squelched;
    msg->Get(guid.m_guid);
    msg->Get(squelched);
    WOWGUID requester = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

    if (!name) {
        return 1;
    }

    if (!squelched) {
        CGGameUI::DisplayError(ERR_PLAYER_UNSILENCED_ECHO, name->m_name);
        return 1;
    }

    CGGameUI::DisplayError(ERR_PLAYER_SILENCED_ECHO, name->m_name);
    return 1;
}

// OFFSET: 0x0092CD10
int32_t CGRaidInfo::GetSelectedMemberIndex() {
    if (!CGRaidInfo::m_selectedMember) {
        return -1;
    }

    uint32_t i = 0;

    if (CGRaidInfo::m_numMembers <= 0) {
        return -1;
    }

    while (CGRaidInfo::m_members[i]->m_guid != CGRaidInfo::m_selectedMember) {
        if (++i >= CGRaidInfo::m_numMembers) {
            return -1;
        }
    }

    return i;
}

// PROVEN: b44723de IDENTICAL/NONE
// OFFSET: 0x0092CD60/0x140693CA0
int32_t IsGuidInMyRaid(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: a66e6444 IDENTICAL/NONE
// OFFSET: 0x0092CDB0/0x140693CE0
int32_t CGRaidInfo::IsPlayerRaidOfficer(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return CGRaidInfo::m_members[i]->m_rank > 0 || CGPartyInfo::m_leader == guid;
        }
    }

    return 0;
}

// OFFSET: 0x0092CE20/0x140693D40
PartyMemberState* CGRaidInfo::GetRemoteStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 40; i++) {
            if (CGRaidInfo::m_members[i]->m_guid == guid) {
                return &CGRaidInfo::m_members[i]->m_stats;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x0092CE70/0x140693D80
PartyMemberState* CGRaidInfo::GetRemoteStatsFromPet(const WOWGUID& petGuid) {
    for (uint32_t i = 0; i < 40; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (member->m_guid && member->m_stats.m_pet.m_guid == petGuid) {
            return &CGRaidInfo::m_members[i]->m_stats;
        }
    }

    return NULL;
}

// OFFSET: 0x0092CEC0/0x140693DC0
PartyMemberPetState* CGRaidInfo::GetRemotePetStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 40; i++) {
            CGRaidMember* member = CGRaidInfo::m_members[i];

            if (member->m_guid && (member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid) {
                return &CGRaidInfo::m_members[i]->m_stats.m_pet;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x0092CF20/0x140693E10
uint32_t GetRaidTargetIndexFromGuid(WOWGUID guid) {
    uint32_t index = 0;

    while (CGRaidInfo::m_raidTargets[index] != guid) {
        if (++index >= 28) {
            return 28;
        }
    }

    return index;
}

// OFFSET: 0x0092CF60
uint8_t CGRaidInfo::GetPartyAssignments(const WOWGUID& guid) {
    uint32_t i = 0;

    if (CGRaidInfo::m_numMembers) {
        while (CGRaidInfo::m_members[i]->m_guid != guid) {
            if (++i >= CGRaidInfo::m_numMembers) {
                return 0;
            }
        }

        return CGRaidInfo::m_members[i]->m_assignments;
    }

    return 0;
}

// OFFSET: 0x0092CFB0/0x140693E80
int32_t CGRaidInfo::GetPartyAssignments(lua_State* L, int32_t assignment) {
    uint8_t flag = 0;

    if (assignment == 0) {
        flag = 2;
    } else if (assignment == 1) {
        flag = 4;
    }

    if (lua_isstring(L, 2)) {
        const char* name = lua_tolstring(L, 2, NULL);
        WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 6, StringToBOOL(L, 3, 0), 3.402823466e38f, 0);

        if (!guid) {
            CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
            return 0;
        }

        if (CGRaidInfo::GetPartyAssignments(guid) & flag) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    int32_t count = 0;

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (flag & CGRaidInfo::m_members[i]->m_assignments) {
            lua_checkstack(L, ++count);
            lua_pushnumber(L, i + 1);
        }
    }

    return count;
}

// OFFSET: 0x0092D0F0
void CGRaidInfo::ClearRecruitAFriend(WOWGUID guid) {
    if (PartyMemberState* stats = CGRaidInfo::GetRemoteStats(guid)) {
        stats->m_flags &= ~0x100;
    }
}

// OFFSET: 0x0092D520
void RaidRosterNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!s_pendingNameQueries || !--s_pendingNameQueries) {
        FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, NULL);
        Script_SendUniqueSignal(SCRIPT_EVENT_PARTY_MEMBERS_CHANGED);
    }
}

// OFFSET: 0x0092D550
static void RaidMemberAddedNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false)) {
        CGGameUI::DisplayError(ERR_RAID_MEMBER_ADDED_S, name->m_name);
        char fullName[256];
        CGAutoCompleteName::SetName(guid, name->FullName(fullName, sizeof(fullName)), false);
    }
}

// OFFSET: 0x0092D5D0/0x140694670
void CGRaidInfo::EnterWorld() {
    uint32_t i = 0;

    if (CGRaidInfo::m_numMembers <= 0) {
        return;
    }

    while (CGRaidInfo::m_members[i]->m_guid) {
        if (++i >= CGRaidInfo::m_numMembers) {
            return;
        }
    }

    CGRaidInfo::m_members[i]->m_guid = ClntObjMgrGetActivePlayer();
    CGRaidMember* member = CGRaidInfo::m_members[i];

    if (g_nameDBCache.GetRecord(member->m_guid.m_guid, member->m_guid, &RaidRosterNameCallback, NULL, false)) {
        if (!s_pendingNameQueries) {
            FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, NULL);
        }
    } else {
        s_pendingNameQueries++;
    }
}

// PROVEN: b8e75bb9 IDENTICAL/NONE
// OFFSET: 0x0092D650/0x140694720
void CGRaidInfo::SetRaidTarget(uint32_t index, const WOWGUID& guid) {
    CGUnit_C* previous = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGRaidInfo::m_raidTargets[index], TYPEMASK_UNIT, __FILE__, __LINE__));
    CGRaidInfo::m_raidTargets[index] = guid;

    if (previous) {
        previous->NamePlateUpdateRaidTarget();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->NamePlateUpdateRaidTarget();
    }
}

// OFFSET: 0x0092D6D0/0x1406947B0
int32_t CGRaidInfo::IsPVPRaid() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && map->m_instanceType == 3) {
        return 1;
    }

    return map->m_instanceType == 4;
}

// OFFSET: 0x0092D710
void CGRaidInfo::ClearRoster() {
    if (CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_RAID_YOU_LEFT);
        CGRaidInfo::m_numMembers = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, NULL);

        for (uint32_t i = 0; i < 40; i++) {
            CGRaidInfo::m_members[i]->m_guid = WOWGUID();
        }
    }

    WOWGUID targets[28];
    memcpy(targets, CGRaidInfo::m_raidTargets, sizeof(targets));
    memset(CGRaidInfo::m_raidTargets, 0, sizeof(CGRaidInfo::m_raidTargets));

    for (uint32_t i = 0; i < 28; i++) {
        if (!targets[i]) {
            continue;
        }

        if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targets[i], TYPEMASK_UNIT, __FILE__, __LINE__))) {
            unit->NamePlateUpdateRaidTarget();
        }
    }

    CGAutoCompleteName::RemoveFlagsFromAllNodes(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_RAID_TARGET_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ROLES_ASSIGNED, NULL);
}

// OFFSET: 0x0092D810/0x140694900
void CGRaidInfo::EnableMember(CGPlayer_C* player, int32_t enable) {
    WOWGUID guid = player->m_obj->m_guid;
    uint32_t index;

    for (index = 0; index < CGRaidInfo::m_numMembers; index++) {
        if (CGRaidInfo::m_members[index]->m_guid == guid) {
            break;
        }
    }

    if (index == CGRaidInfo::m_numMembers || enable) {
        return;
    }

    player->FillPartyMemberState(&CGRaidInfo::m_members[index]->m_stats);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
    msg.Put(player->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0092D8F0/0x140694A00
int32_t CGRaidInfo::IsPetInRaid(const WOWGUID& guid, int32_t* index) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (!member) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
        bool match;

        if (player) {
            const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
            match = pet == guid;
        } else {
            match = (member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid;
        }

        if (match) {
            if (index) {
                *index = i;
            }

            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0092D9A0/0x140694AF0
int32_t CGRaidInfo::IsPlayerOrPetInRaid(const WOWGUID& guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (!member) {
            continue;
        }

        if (member->m_guid == guid) {
            return 1;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            CGUnitData* unit = player->m_unit;
            const WOWGUID& pet = unit->m_unitFieldCharm ? unit->m_unitFieldCharm : unit->m_unitFieldSummon;

            if (pet == guid) {
                return 1;
            }
        } else if ((member->m_stats.m_flags & 1) && member->m_stats.m_pet.m_guid == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0092DA50/0x140694BC0
void CGRaidInfo::SendReadyCheck() {
    WOWGUID player = ClntObjMgrGetActivePlayer();

    if (!player) {
        return;
    }

    if (CGRaidInfo::m_numMembers) {
        if (!CGRaidInfo::IsPlayerRaidOfficer(player)) {
            return;
        }
    } else if (player != CGPartyInfo::m_leader) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0 && CGRaidInfo::m_readyCheckTime != now) {
        CGGameUI::DisplayError(ERR_READY_CHECK_IN_PROGRESS);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_RAID_READY_CHECK));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x0092DB30/0x140694CF0
void CGRaidInfo::CheckReadyCheckTime() {
    if (!CGRaidInfo::m_readyCheckTime || !CGRaidInfo::m_readyCheckInitiator || CGRaidInfo::m_readyCheckInitiator != ClntObjMgrGetActivePlayer() || static_cast<int32_t>(FrameTime::GetCurTimeMs() - CGRaidInfo::m_readyCheckTime) < 0) {
        return;
    }

    if (CGRaidInfo::m_readyCheckTimer) {
        ClientKillTimer(CGRaidInfo::m_readyCheckTimer, CGRaidInfo::ProcessReadyCheckTimer, "ProcessReadyCheckTimer");
        CGRaidInfo::m_readyCheckTimer = 0;
    }

    CGRaidInfo::m_readyCheckTime = 0;
    const char* format = FrameScript_GetText("RAID_MEMBERS_AFK", -1, GENDER_NOT_APPLICABLE);
    int32_t first = 1;
    int32_t allReady = 1;
    char names[512];

    if (CGRaidInfo::m_numMembers) {
        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            CGRaidMember* member = CGRaidInfo::m_members[i];

            if (!member->m_readyCheckReady) {
                allReady = 0;
            }

            if (!member->m_readyCheckPending) {
                continue;
            }

            WOWGUID requester = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(member->m_guid.m_guid, requester, NULL, NULL, false);

            if (!name) {
                continue;
            }

            if (first) {
                SStrCopy(names, name->m_name, sizeof(names));
                first = 0;
            } else {
                SStrPack(names, ", ", sizeof(names));
                SStrPack(names, name->m_name, sizeof(names));
            }
        }
    } else {
        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            if (i >= 4 || !CGPartyInfo::m_readyCheckReady[i]) {
                allReady = 0;
            }

            if (i >= 4 || !CGPartyInfo::m_readyCheckPending[i]) {
                continue;
            }

            WOWGUID requester = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(CGPartyInfo::GetMember(i).m_guid, requester, NULL, NULL, false);

            if (!name) {
                continue;
            }

            if (first) {
                SStrCopy(names, name->m_name, sizeof(names));
                first = 0;
            } else {
                SStrPack(names, ", ", sizeof(names));
                SStrPack(names, name->m_name, sizeof(names));
            }
        }
    }

    if (!first) {
        char text[512];
        SStrPrintf(text, sizeof(text), format, names);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    } else if (!allReady) {
        CGChat::AddChatMessage(FrameScript_GetText("READY_CHECK_FINISHED", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
    } else {
        CGChat::AddChatMessage(FrameScript_GetText("READY_CHECK_ALL_READY", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_RAID_READY_CHECK_FINISHED));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// PROVEN: 3c2c0e63 IDENTICAL/NONE
// OFFSET: 0x0092DED0/0x1406951E0
int32_t CGRaidInfo::ProcessReadyCheckTimer(const void* data, void* param) {
    CGRaidInfo::m_readyCheckTimer = 0;
    CGRaidInfo::CheckReadyCheckTime();
    return 1;
}

// OFFSET: 0x0092ED90/0x1406963B0
static bool GetSavedInstanceCompletedMask(int32_t mapId, uint32_t& mask, int32_t difficulty) {
    bool matchDifficulty = true;
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (map && !(map->m_flags & 0x8000)) {
        matchDifficulty = false;
    }

    for (uint32_t i = 0; i < s_savedInstances.Count(); i++) {
        RaidInstanceInfo& info = s_savedInstances.m_data[i];

        if (info.m_mapID == static_cast<uint32_t>(mapId) && info.m_locked && static_cast<int32_t>(info.m_resetTime) > 0 && (matchDifficulty || static_cast<int32_t>(info.m_difficulty) == difficulty)) {
            mask = info.m_completedMask;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0092EE20/0x140696450
void CGRaidInfo::GetMapNameWithDifficulty(char* buffer, uint32_t size, int32_t mapId, int32_t difficulty, int32_t heroic) {
    *buffer = 0;
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map) {
        return;
    }

    const char* difficultyName;

    if (map->m_flags & 0x8000) {
        if (heroic <= 0) {
            SStrCopy(buffer, map->m_mapName, size);
            return;
        }

        difficultyName = "PLAYER_DIFFICULTY2";
    } else {
        MapDifficultyRec* entry = GetMapDifficultyRec(mapId, difficulty, NULL);

        if (!entry || !entry->m_difficultystring || !*entry->m_difficultystring) {
            SStrCopy(buffer, map->m_mapName, size);
            return;
        }

        difficultyName = entry->m_difficultystring;
    }

    const char* name = map->m_mapName;
    const char* difficultyText = FrameScript_GetText(difficultyName, -1, GENDER_NOT_APPLICABLE);
    SStrPrintf(buffer, size, FrameScript_GetText("DUNGEON_NAME_WITH_DIFFICULTY", -1, GENDER_NOT_APPLICABLE), name, difficultyText);
}

// OFFSET: 0x0092EF40
uint32_t CGRaidInfo::GetIneligibleEncounterMask(int32_t mapId, uint32_t completedMask, uint32_t difficulty) {
    uint32_t mask;

    if (GetSavedInstanceCompletedMask(mapId, mask, difficulty)) {
        return mask & ~completedMask;
    }

    return 0;
}

// OFFSET: 0x0092F310
int32_t Packet_MSG_RAID_TARGET_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID setter;
    setter.m_guid = 0;
    uint8_t full;
    msg->Get(full);

    if (full) {
        WOWGUID previous[28];
        memcpy(previous, CGRaidInfo::m_raidTargets, sizeof(previous));
        memset(CGRaidInfo::m_raidTargets, 0, sizeof(CGRaidInfo::m_raidTargets));

        for (uint32_t i = 0; i < 28; i++) {
            if (previous[i]) {
                if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(previous[i], TYPEMASK_UNIT, __FILE__, __LINE__))) {
                    unit->NamePlateUpdateRaidTarget();
                }
            }
        }
    } else {
        msg->Get(setter.m_guid);
    }

    uint8_t index = 0;
    WOWGUID target;
    target.m_guid = 0;

    while (!msg->IsRead()) {
        msg->Get(index);
        msg->Get(target.m_guid);

        if (index < 28) {
            CGRaidInfo::SetRaidTarget(index, target);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_RAID_TARGET_UPDATE, NULL);

    if (!full && setter && target) {
        CGChat::InformTargetIcon(setter, target, index);
    }

    return 1;
}

// OFFSET: 0x0092F450
int32_t Packet_MSG_RAID_READY_CHECK_CONFIRM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    guid.m_guid = 0;
    msg->Get(guid.m_guid);
    uint8_t ready = 0;

    if (!msg->IsRead()) {
        msg->Get(ready);
    }

    bool pending = false;
    bool found = false;
    char buffer[256];

    if (CGRaidInfo::m_numMembers) {
        bool initiator = CGRaidInfo::m_readyCheckInitiator == ClntObjMgrGetActivePlayer();

        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            CGRaidMember* member = CGRaidInfo::m_members[i];

            if (member->m_guid == guid) {
                member->m_readyCheckPending = 0;
                CGRaidInfo::m_members[i]->m_readyCheckReady = ready;
                found = true;

                if (!ready) {
                    WOWGUID requester;
                    requester.m_guid = 0;
                    NameCache* name = g_nameDBCache.GetRecord(CGRaidInfo::m_members[i]->m_guid.m_guid, requester, NULL, NULL, false);

                    if (name && initiator) {
                        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("RAID_MEMBER_NOT_READY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name));
                        CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
                    }
                }

                SignalReadyCheckConfirmEvents(guid, ready);
            }

            member = CGRaidInfo::m_members[i];

            if (pending || ((member->m_stats.m_flags & 0x1) && member->m_readyCheckPending)) {
                pending = true;

                if (found) {
                    return 1;
                }
            } else {
                pending = false;
            }
        }

        if (pending) {
            return 1;
        }
    } else {
        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            PartyReadyCheckInfo info;
            CGPartyInfo::GetReadyCheckInfo(&info, i);

            if (info.m_guid == guid) {
                info.m_pending = 0;
                info.m_ready = ready;
                CGPartyInfo::SetReadyCheckInfo(&info, i);
                found = true;

                if (!ready) {
                    WOWGUID requester;
                    requester.m_guid = 0;
                    NameCache* name = g_nameDBCache.GetRecord(info.m_guid.m_guid, requester, NULL, NULL, false);

                    if (name) {
                        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("RAID_MEMBER_NOT_READY", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name));
                        CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
                    }
                }

                SignalReadyCheckConfirmEvents(guid, ready);
            }

            PartyMemberState* state = CGPartyInfo::m_members[i] ? &s_partyMemberStates[i] : NULL;

            if (pending || (state && (state->m_flags & 0x1) && i < 4 && CGPartyInfo::m_readyCheckPending[i])) {
                pending = true;

                if (found) {
                    return 1;
                }
            } else {
                pending = false;
            }
        }

        if (pending) {
            return 1;
        }
    }

    CGRaidInfo::m_readyCheckTime = FrameTime::GetCurTimeMs() - 1;
    CGRaidInfo::CheckReadyCheckTime();
    return 1;
}

// OFFSET: 0x0092F720/0x140697190
bool CGRaidInfo::CanBeRaidTarget(WOWGUID guid) {
    if (!guid) {
        return false;
    }

    if (CGRaidInfo::IsPlayerOrPetInRaid(guid) || CGPartyInfo::IsPlayerOrPetInParty(guid)) {
        return true;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return unit && player && (unit == player || !(unit->m_obj->m_type & TYPEMASK_PLAYER) || unit->CanCooperate(player));
}

// OFFSET: 0x0092F7C0
void CGRaidInfo::SetRaidTarget(WOWGUID guid, uint32_t index) {
    if (guid && !CGRaidInfo::IsPlayerOrPetInRaid(guid) && !CGPartyInfo::IsPlayerOrPetInParty(guid)) {
        if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
            CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
            return;
        }

        if (!CGRaidInfo::CanBeRaidTarget(guid)) {
            CGGameUI::DisplayError(ERR_INVALID_RAID_TARGET);
            return;
        }
    }

    if (index >= 28) {
        return;
    }

    if (CGRaidInfo::m_numMembers || CGPartyInfo::m_members[0]) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_RAID_TARGET_UPDATE));
        msg.Put(static_cast<uint8_t>(index));
        msg.Put(guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return;
    }

    uint32_t current = GetRaidTargetIndexFromGuid(guid);

    if (current == index || current < 28) {
        CGRaidInfo::SetRaidTarget(current, WOWGUID());
    }

    if (current != index) {
        CGRaidInfo::SetRaidTarget(index, guid);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_RAID_TARGET_UPDATE, NULL);
}

// OFFSET: 0x0092F940/0x140697410
void CGRaidInfo::InitiateReadyCheck(const char* name) {
    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t remaining;

    if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0) {
        remaining = CGRaidInfo::m_readyCheckTime - now;
    } else {
        remaining = 0;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_READY_CHECK, "%s%d", name, remaining / 1000);

    if (CGRaidInfo::m_readyCheckInitiator != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_RAID_LEADER_READY_CHECK_START_S, name);
        return;
    }

    if (CGRaidInfo::m_readyCheckTimer > 0) {
        ClientKillTimer(CGRaidInfo::m_readyCheckTimer, CGRaidInfo::ProcessReadyCheckTimer, "ProcessReadyCheckTimer");
        CGRaidInfo::m_readyCheckTimer = 0;
    }

    CGRaidInfo::m_readyCheckTimer = ClientSetTimer(30000, CGRaidInfo::ProcessReadyCheckTimer, NULL);
}

// OFFSET: 0x0092F9F0/0x1406974C0
void CGRaidInfo::RosterUpdateRefreshReadyCheck(uint32_t removed, WOWGUID leader) {
    if (!CGRaidInfo::m_numMembers || !CGPartyInfo::m_members[0] || leader != CGPartyInfo::m_leader) {
        CGRaidInfo::m_readyCheckTime = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_READY_CHECK_FINISHED, "%b", 1);
        CGRaidInfo::m_readyCheckInitiator = WOWGUID();

        if (CGRaidInfo::m_readyCheckTimer) {
            ClientKillTimer(CGRaidInfo::m_readyCheckTimer, CGRaidInfo::ProcessReadyCheckTimer, "ProcessReadyCheckTimer");
            CGRaidInfo::m_readyCheckTimer = 0;
        }

        return;
    }

    WOWGUID player = ClntObjMgrGetActivePlayer();
    uint32_t count = CGRaidInfo::m_numMembers;
    bool done;

    if (count) {
        if (!CGRaidInfo::m_readyCheckTime || !removed || !player || !CGRaidInfo::IsPlayerRaidOfficer(player)) {
            return;
        }

        uint32_t i = 0;

        while (i < count && !(CGRaidInfo::m_members[i]->m_readyCheckPending && (CGRaidInfo::m_members[i]->m_stats.m_flags & 0x1))) {
            i++;
        }

        done = i == count;
    } else {
        if (!CGRaidInfo::m_readyCheckTime || !removed || !player || player != CGPartyInfo::m_leader) {
            return;
        }

        uint32_t i = 0;

        for (; i < CGPartyInfo::NumMembers(); i++) {
            PartyMemberState* state = CGPartyInfo::GetMemberState(i);

            if (state && (state->m_flags & 0x1) && CGRaidInfo::IsPartyMemberReadyCheckPending(i)) {
                break;
            }
        }

        done = i == CGPartyInfo::NumMembers();
    }

    if (done) {
        CGRaidInfo::m_readyCheckTime = FrameTime::GetCurTimeMs() - 1;
        CGRaidInfo::CheckReadyCheckTime();
    }
}

// OFFSET: 0x00930260
static void ReadyCheckNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false)) {
        CGRaidInfo::InitiateReadyCheck(name->m_name);
    }
}

// OFFSET: 0x009302B0
int32_t Packet_SMSG_RAID_INSTANCE_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    if (count) {
        if (count >= s_savedInstances.Count()) {
            s_savedInstances.SetCount(count);
        }

        for (uint32_t i = 0; i < count; i++) {
            RaidInstanceInfo& info = s_savedInstances.m_data[i];
            msg->Get(info.m_mapID);
            msg->Get(info.m_difficulty);
            msg->Get(info.m_heroic);
            msg->Get(info.m_instanceID);
            uint8_t value = 0;
            msg->Get(value);
            info.m_locked = value;
            value = 0;
            msg->Get(value);
            info.m_extended = value;
            msg->Get(info.m_resetTime);
            msg->Get(info.m_completedMask);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_INSTANCE_INFO, NULL);
    return 1;
}

// OFFSET: 0x009303D0
int32_t Packet_MSG_RAID_READY_CHECK(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID initiator;
    msg->Get(initiator.m_guid);
    CGRaidInfo::m_readyCheckInitiator = initiator;
    CGRaidInfo::m_readyCheckTime = FrameTime::GetCurTimeMs() + 30000;

    if (initiator == ClntObjMgrGetActivePlayer()) {
        CGChat::AddChatMessage(FrameScript_GetText("READY_CHECK_START", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
    }

    if (CGRaidInfo::m_numMembers) {
        CGRaidMember* self = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

        if (self && self->m_rank > 0) {
            for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
                CGRaidMember* member = CGRaidInfo::m_members[i];

                if (member->m_guid) {
                    if (member->m_guid == initiator) {
                        member->m_readyCheckPending = 0;
                        CGRaidInfo::m_members[i]->m_readyCheckReady = 1;
                    } else {
                        member->m_readyCheckPending = 1;
                    }
                }
            }
        }
    } else if (CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            if (CGPartyInfo::GetMember(i)) {
                PartyReadyCheckInfo info;
                CGPartyInfo::GetReadyCheckInfo(&info, i);

                if (info.m_guid == initiator) {
                    info.m_pending = 0;
                    info.m_ready = 1;
                } else {
                    info.m_pending = 1;
                }

                CGPartyInfo::SetReadyCheckInfo(&info, i);
            }
        }
    }

    if (NameCache* name = g_nameDBCache.GetRecord(initiator.m_guid, initiator, &ReadyCheckNameCallback, NULL, false)) {
        CGRaidInfo::InitiateReadyCheck(name->m_name);
    }

    return 1;
}

// OFFSET: 0x00930570/0x140698360
void CGRaidInfo::UpdateRoster(const WOWGUID* guids, const uint32_t* subgroups, const uint8_t* flags, const uint32_t* roles, const uint8_t* online, uint32_t count, uint8_t battleground, uint8_t everyoneIsAssistant) {
    if (count) {
        CGTutorial::TriggerTutorial(45);
    }

    uint32_t oldCount = CGRaidInfo::m_numMembers;
    int32_t joining = oldCount == 0;

    if (s_pendingNameQueries) {
        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            g_nameDBCache.CancelCallback(CGRaidInfo::m_members[i]->m_guid.m_guid, &RaidRosterNameCallback, NULL);
            oldCount = CGRaidInfo::m_numMembers;
        }
    }

    uint32_t addedCount = 0;
    WOWGUID* added = static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * count));
    uint32_t removedCount = 0;
    WOWGUID* removed = static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * oldCount));
    PartyMemberState* stats = new (__FILE__, __LINE__) PartyMemberState[count];

    for (uint32_t i = 0; i < count; i++) {
        uint32_t j = 0;

        for (; j < CGRaidInfo::m_numMembers; j++) {
            if (guids[i] == CGRaidInfo::m_members[j]->m_guid) {
                stats[i].Copy(&CGRaidInfo::m_members[j]->m_stats);
                break;
            }
        }

        if (j != CGRaidInfo::m_numMembers) {
            continue;
        }

        stats[i].Reset();
        stats[i].m_health = 1;
        stats[i].m_maxHealth = 1;
        stats[i].m_power = 1;
        stats[i].m_maxPower = 1;

        if (CGRaidInfo::m_numMembers) {
            const char* fullNamePtr = NULL;
            char fullName[256];

            if (NameCache* name = g_nameDBCache.GetRecord(guids[i].m_guid, guids[i], &RaidMemberAddedNameCallback, NULL, false)) {
                CGGameUI::DisplayError(ERR_RAID_MEMBER_ADDED_S, name->m_name);

                if (name->m_realmName[0]) {
                    SStrPrintf(fullName, sizeof(fullName), "%s-%s", static_cast<const char*>(name->m_name), name->m_realmName);
                } else {
                    SStrCopy(fullName, name->m_name, sizeof(fullName));
                }

                fullNamePtr = fullName;
            }

            CGAutoCompleteName::AddFlags(guids[i], 1, 0, fullNamePtr);
        }

        added[addedCount++] = guids[i];
    }

    if (CGRaidInfo::m_rosterCounter == battleground) {
        for (uint32_t j = 0; j < CGRaidInfo::m_numMembers; j++) {
            uint32_t i = 0;

            while (i < count && CGRaidInfo::m_members[j]->m_guid != guids[i]) {
                i++;
            }

            if (i != count) {
                continue;
            }

            NameCache* name = g_nameDBCache.GetRecord(CGRaidInfo::m_members[j]->m_guid.m_guid, CGRaidInfo::m_members[j]->m_guid, &RaidMemberRemovedNameCallback, NULL, false);

            if (name && ClntObjMgrGetActivePlayer()) {
                CGGameUI::DisplayError(ERR_RAID_MEMBER_REMOVED_S, name->m_name);
                CGAutoCompleteName::RemoveFlags(CGRaidInfo::m_members[j]->m_guid, 1);
            }

            removed[removedCount++] = CGRaidInfo::m_members[j]->m_guid;
        }
    }

    CGRaidInfo::m_rosterCounter = battleground;

    if (!CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_RAID_YOU_JOINED);
    }

    uint32_t previousCount = CGRaidInfo::m_numMembers;
    PartyReadyCheckInfo* previous = static_cast<PartyReadyCheckInfo*>(alloca(sizeof(PartyReadyCheckInfo) * previousCount));
    WOWGUID leader = WOWGUID();

    for (uint32_t i = 0; i < previousCount; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];
        previous[i].m_guid = member->m_guid;
        previous[i].m_pending = member->m_readyCheckPending;
        previous[i].m_ready = member->m_readyCheckReady;

        if (member->m_rank == 2) {
            leader = member->m_guid;
        }
    }

    s_pendingNameQueries = 0;
    CGRaidInfo::m_numMembers = count;

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];
        member->m_guid = guids[i];
        member->m_subgroup = subgroups[i];
        member->m_assignments = flags[i];
        member->m_groupRole = roles[i];

        if (member->m_guid == CGPartyInfo::m_leader) {
            member->m_rank = 2;
        } else {
            member->m_rank = everyoneIsAssistant || (flags[i] & 0x1);
        }

        member->m_stats.Copy(&stats[i]);

        if (online[i]) {
            member->m_stats.m_flags |= 0x1;
        } else {
            member->m_stats.m_flags &= ~0x1;
        }

        uint32_t j = 0;

        while (j < previousCount && member->m_guid != previous[j].m_guid) {
            j++;
        }

        if (j < previousCount) {
            member->m_readyCheckPending = previous[j].m_pending;
            member->m_readyCheckReady = previous[j].m_ready;
        } else {
            member->m_readyCheckPending = 0;
            member->m_readyCheckReady = 0;
        }

        if (member->m_guid && !g_nameDBCache.GetRecord(member->m_guid.m_guid, member->m_guid, &RaidRosterNameCallback, NULL, false)) {
            s_pendingNameQueries++;
        }
    }

    for (uint32_t i = 0; i < removedCount; i++) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(removed[i], TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!unit) {
            CGGameUI::ClearTarget(removed[i], 1);
            continue;
        }

        const WOWGUID& petGuid = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

        if (petGuid) {
            if (CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon, TYPEMASK_UNIT, __FILE__, __LINE__))) {
                pet->UnregisterScript(512);
            } else {
                CGGameUI::ClearTarget(unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon, 1);
            }
        }

        unit->UnregisterScript(256);
    }

    for (uint32_t i = 0; i < addedCount; i++) {
        if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(added[i], TYPEMASK_PLAYER, __FILE__, __LINE__))) {
            unit->RegisterScript(256);
            const WOWGUID& petGuid = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

            if (ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
                unit->RegisterScript(512);
            }
        } else {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PARTY_MEMBER_STATS));
            msg.Put(added[i].m_guid);
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    if (!s_pendingNameQueries) {
        FrameScript_SignalEvent(SCRIPT_EVENT_RAID_ROSTER_UPDATE, NULL);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ROLES_ASSIGNED, NULL);

    if (joining) {
        UserClientRequestPartyJoinUpdates msg;
        ClientServices::Send(&msg);
    }

    CGRaidInfo::RosterUpdateRefreshReadyCheck(removedCount, leader);
    delete[] stats;
}

// OFFSET: 0x00930DE0
void CGRaidInfo::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_RAID_TARGET_UPDATE, Packet_MSG_RAID_TARGET_UPDATE, NULL);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK, Packet_MSG_RAID_READY_CHECK, NULL);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK_CONFIRM, Packet_MSG_RAID_READY_CHECK_CONFIRM, NULL);
    ClientServices::SetMessageHandler(MSG_RAID_READY_CHECK_FINISHED, Packet_MSG_RAID_READY_CHECK_FINISHED, NULL);
    ClientServices::SetMessageHandler(SMSG_READY_CHECK_ERROR, Packet_SMSG_READY_CHECK_ERROR, NULL);
    ClientServices::SetMessageHandler(MSG_NOTIFY_PARTY_SQUELCH, Packet_MSG_NOTIFY_PARTY_SQUELCH, NULL);
    ClientServices::SetMessageHandler(SMSG_ECHO_PARTY_SQUELCH, Packet_SMSG_ECHO_PARTY_SQUELCH, NULL);
    ClientServices::SetMessageHandler(SMSG_RAID_INSTANCE_INFO, Packet_SMSG_RAID_INSTANCE_INFO, NULL);
    CGRaidInfo::m_numMembers = 0;
    CGRaidInfo::m_realNumMembers = 0;
    CGRaidInfo::m_selectedMember = WOWGUID();
    s_pendingNameQueries = 0;
    CGRaidInfo::m_rosterCounter = 0xFF;
    memset(CGRaidInfo::m_raidTargets, 0, sizeof(CGRaidInfo::m_raidTargets));
    CGRaidInfo::m_raidMarkers = 0;

    for (uint32_t i = 0; i < 40; i++) {
        CGRaidInfo::m_members[i] = new (__FILE__, __LINE__) CGRaidMember;
        CGRaidInfo::m_members[i]->m_guid = WOWGUID();
        CGRaidInfo::m_members[i]->m_subgroup = 0;
        CGRaidInfo::m_members[i]->m_rank = 0;
        CGRaidInfo::m_members[i]->m_readyCheckPending = 0;
        CGRaidInfo::m_members[i]->m_readyCheckReady = 0;
        CGRaidInfo::m_members[i]->m_assignments = 0;
        CGRaidInfo::m_members[i]->m_groupRole = 0;
        CGRaidInfo::m_members[i]->m_stats.Reset();
    }

    CGRaidInfo::m_readyCheckTime = 0;
    CGRaidInfo::m_readyCheckInitiator = WOWGUID();
    CGRaidInfo::m_readyCheckTimer = 0;
    CGRaidInfo::m_isInFakeRaid = 0;
    CGRaidInfo::m_everyoneIsAssistant = 0;
}

// OFFSET: 0x00930FE0/0x140699000
void CGRaidInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(MSG_RAID_TARGET_UPDATE);
    ClientServices::ClearMessageHandler(MSG_RAID_READY_CHECK);
    ClientServices::ClearMessageHandler(MSG_RAID_READY_CHECK_CONFIRM);
    ClientServices::ClearMessageHandler(MSG_RAID_READY_CHECK_FINISHED);
    ClientServices::ClearMessageHandler(SMSG_READY_CHECK_ERROR);
    ClientServices::ClearMessageHandler(MSG_NOTIFY_PARTY_SQUELCH);
    ClientServices::ClearMessageHandler(SMSG_ECHO_PARTY_SQUELCH);
    ClientServices::ClearMessageHandler(SMSG_RAID_INSTANCE_INFO);

    for (uint32_t i = 0; i < 40; i++) {
        delete CGRaidInfo::m_members[i];
        CGRaidInfo::m_members[i] = NULL;
    }

    if (CGRaidInfo::m_readyCheckTimer) {
        ClientKillTimer(CGRaidInfo::m_readyCheckTimer, CGRaidInfo::ProcessReadyCheckTimer, "ProcessReadyCheckTimer");
    }

    s_savedInstances.Clear();
}

// PROVEN: b809d5f5 IDENTICAL/NONE
// OFFSET: 0x0044BB40
int32_t CGRaidInfo::GetMemberIndexByGuid(WOWGUID guid) {
    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return i;
        }
    }

    return -1;
}

// PROVEN: 72f38f33 IDENTICAL/NONE
// OFFSET: 0x004E8290
CGRaidMember* CGRaidInfo::GetMember(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers) {
        return NULL;
    }

    return CGRaidInfo::m_members[index];
}

// PROVEN: f108baec IDENTICAL/NONE
// OFFSET: 0x0044BAF0/0x140059210
CGRaidMember* CGRaidInfo::FindMemberByGuid(WOWGUID guid) {
    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (CGRaidInfo::m_members[i]->m_guid == guid) {
            return CGRaidInfo::m_members[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0092CA90
int32_t CGRaidInfo::GetPartyMemberIndex(WOWGUID guid) {
    for (uint32_t i = 0; i < 4; i++) {
        if (CGPartyInfo::m_members[i] == guid) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x0092CAD0
int32_t CGRaidInfo::IsPartyMemberReadyCheckPending(uint32_t index) {
    return index >= 4 ? 0 : CGPartyInfo::m_readyCheckPending[index];
}

// OFFSET: 0x0092CAF0
int32_t CGRaidInfo::IsPartyMemberReadyCheckReady(uint32_t index) {
    return index >= 4 ? 0 : CGPartyInfo::m_readyCheckReady[index];
}

// PROVEN: c54805fa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGRaidInfo::LeaveWorld() {
}
