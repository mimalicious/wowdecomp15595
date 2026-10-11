#include "RaidInfo.h"
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameScript/Types.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BF8CE8
static const int32_t s_raidMarkerSpells[5] = { 84996, 84997, 84998, 84999, 85000 };

// OFFSET: 0x0092D110/0x140694080
static int32_t Script_GetRaidTargetIndex(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetRaidTargetIndex(unit)");
        return 0;
    }

    uint32_t index = GetRaidTargetIndexFromGuid(Script_GetGUIDFromName(lua_tolstring(L, 1, NULL)));

    if (index == 28) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, index + 1);
    }

    return 1;
}

// PROVEN: 85456542 IDENTICAL/NONE
// OFFSET: 0x0092D1A0/0x140694130
static int32_t Script_GetNumRaidMembers(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::m_numMembers);
    return 1;
}

// PROVEN: 24feec04 IDENTICAL/NONE
// OFFSET: 0x0092D1E0/0x140694160
static int32_t Script_GetRealNumRaidMembers(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::m_realNumMembers);
    return 1;
}

// OFFSET: 0x0092D220/0x140694190
static int32_t Script_SetRaidRosterSelection(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetRaidRosterSelection(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (!(index >= CGRaidInfo::m_numMembers)) {
        CGRaidInfo::m_selectedMember = CGRaidInfo::m_members[index]->m_guid;
        return 0;
    }

    CGRaidInfo::m_selectedMember = WOWGUID();
    return 0;
}

// PROVEN: 006e1ac5 IDENTICAL/NONE
// OFFSET: 0x0092D2B0/0x140694210
static int32_t Script_GetRaidRosterSelection(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::GetSelectedMemberIndex() + 1);
    return 1;
}

// OFFSET: 0x0092D2E0/0x140694270
static int32_t Script_GetReadyCheckTimeLeft(lua_State* L) {
    if (CGRaidInfo::m_numMembers || CGPartyInfo::m_members[0]) {
        uint32_t now = FrameTime::GetCurTimeMs();
        uint32_t remaining;

        if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0) {
            remaining = CGRaidInfo::m_readyCheckTime - now;
        } else {
            remaining = 0;
        }

        lua_pushnumber(L, remaining / 1000);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x0092D370/0x1406942F0
static int32_t Script_IsRaidMarkerActive(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsRaidMarkerActive(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tointeger(L, 1)) - 1;

    if (index > 5) {
        return 0;
    }

    lua_pushboolean(L, index < 5 && ((1 << index) & CGRaidInfo::m_raidMarkers));
    return 1;
}

// OFFSET: 0x0092D3E0/0x140694360
static int32_t Script_IsInFakeRaid(lua_State* L) {
    lua_pushboolean(L, CGRaidInfo::m_isInFakeRaid);
    return 1;
}

// OFFSET: 0x0092D400/0x140694380
static int32_t Script_IsEveryoneAssistant(lua_State* L) {
    lua_pushboolean(L, CGRaidInfo::m_everyoneIsAssistant);
    return 1;
}

// OFFSET: 0x0092DEF0/0x140695200
static int32_t Script_GetRaidRosterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRaidRosterInfo(index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGRaidMember* member;
    NameCache* name;
    WOWGUID requester = WOWGUID();

    if (index >= static_cast<int32_t>(CGRaidInfo::m_numMembers) || !(member = CGRaidInfo::m_members[index]) || !(name = g_nameDBCache.GetRecord(member->m_guid.m_guid, requester, NULL, NULL, false))) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushstring(L, GetRoleTokenFromFlags(0));
        return 12;
    }

    char fullName[308];
    lua_pushstring(L, name->FullName(fullName, 305));
    lua_pushnumber(L, member->m_rank);
    lua_pushnumber(L, member->m_subgroup + 1);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        lua_pushnumber(L, static_cast<int32_t>(unit->m_unit->m_unitFieldLevel));
    } else if (member->m_stats.m_flags & 1) {
        lua_pushnumber(L, member->m_stats.m_level);
    } else {
        lua_pushnumber(L, 0.0);
    }

    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(name->m_class);
    const char* className = CGUnit_C::GetDisplayClassName(NULL, name);

    if (chrClass && className) {
        lua_pushstring(L, className);
        lua_pushstring(L, chrClass->m_filename);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    bool dead;

    if (unit || (member->m_stats.m_flags & 1)) {
        int32_t zoneID = unit ? CGGameUI::m_areaID : member->m_stats.m_zoneID;

        if (AreaTableRec* area = g_areaTableDB.GetRecord(zoneID)) {
            lua_pushstring(L, area->m_areaName);
        } else {
            lua_pushnil(L);
        }

        lua_pushnumber(L, 1.0);

        if (unit) {
            dead = unit->IsDeadOrGhost();
        } else {
            dead = (member->m_stats.m_flags & 0xC) && (member->m_stats.m_flags & 1);
        }
    } else {
        lua_pushstring(L, FrameScript_GetText("PLAYER_OFFLINE", -1, GENDER_NOT_APPLICABLE));
        lua_pushnil(L);
        dead = (member->m_stats.m_flags & 0xC) && (member->m_stats.m_flags & 1);
    }

    if (!dead) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    if (member->m_assignments & 2) {
        lua_pushstring(L, "MAINTANK");
    } else if (member->m_assignments & 4) {
        lua_pushstring(L, "MAINASSIST");
    } else {
        lua_pushnil(L);
    }

    if (CGPartyInfo::m_lootMethod == 2 && CGPartyInfo::m_lootMaster == member->m_guid) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, GetRoleTokenFromFlags(member->m_groupRole));
    return 12;
}

// PROVEN: e85ed437 IDENTICAL/NONE
// OFFSET: 0x0092E260/0x140695680
static int32_t Script_IsRaidLeader(lua_State* L) {
    if (ClntObjMgrGetActivePlayer() == CGPartyInfo::m_leader) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 6bb5c14d IDENTICAL/NONE
// OFFSET: 0x0092E2B0/0x1406956D0
static int32_t Script_IsRealRaidLeader(lua_State* L) {
    if (ClntObjMgrGetActivePlayer() == CGPartyInfo::m_realLeader) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// PROVEN: 68a52d7b IDENTICAL/NONE
// OFFSET: 0x0092E300/0x140695720
static int32_t Script_IsRaidOfficer(lua_State* L) {
    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

    if (member && member->m_rank > 0) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0092E350/0x1406957A0
static int32_t Script_PromoteToLeader(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PromoteToLeader(name)");
        return 0;
    }

    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(lua_tolstring(L, 1, NULL), TYPEMASK_PLAYER, 6, exact, 3.402823466e38f, 0);

    if (guid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_GROUP_SET_LEADER));
        msg.Put(guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x0092E430/0x1406958B0
static int32_t Script_PromoteToAssistant(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: PromoteToAssistant(name)");
        return 0;
    }

    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(lua_tolstring(L, 1, NULL), TYPEMASK_PLAYER, 6, exact, 3.402823466e38f, 0);

    if (guid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_ASSISTANT_LEADER));
        msg.Put(guid.m_guid);
        msg.Put(static_cast<uint8_t>(1));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x0092E510/0x1406959D0
static int32_t Script_DemoteAssistant(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: DemoteAssistant(name)");
        return 0;
    }

    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(lua_tolstring(L, 1, NULL), TYPEMASK_PLAYER, 6, exact, 3.402823466e38f, 0);

    if (guid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_ASSISTANT_LEADER));
        msg.Put(guid.m_guid);
        msg.Put(static_cast<uint8_t>(0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// PROVEN: 229f059b IDENTICAL/NONE
// OFFSET: 0x0092E5F0/0x140695AF0
static int32_t Script_DoReadyCheck(lua_State* L) {
    CGRaidInfo::SendReadyCheck();
    return 0;
}

// OFFSET: 0x0092E600/0x140695B00
static int32_t Script_ConfirmReadyCheck(lua_State* L) {
    if (!CGRaidInfo::m_numMembers && !CGPartyInfo::m_members[0]) {
        return 0;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGRaidInfo::m_readyCheckTime && static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) < 0 && CGRaidInfo::m_readyCheckTime != now) {
        uint8_t ready = static_cast<uint8_t>(StringToBOOL(L, 1, 0));
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_RAID_READY_CHECK));
        msg.Put(ready);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x0092E6C0/0x140695BF0
static int32_t Script_GetReadyCheckStatus(lua_State* L) {
    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));
    const char* status = NULL;

    if (CGRaidInfo::m_numMembers) {
        CGRaidMember* self = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

        if (!self || self->m_rank <= 0) {
            lua_pushstring(L, status);
            return 1;
        }

        CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);
        uint32_t now = FrameTime::GetCurTimeMs();

        if (!CGRaidInfo::m_readyCheckTime || static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) >= 0 || CGRaidInfo::m_readyCheckTime == now || !member) {
            lua_pushstring(L, status);
            return 1;
        }

        if (guid == CGRaidInfo::m_readyCheckInitiator) {
            lua_pushstring(L, "ready");
            return 1;
        }

        if (member->m_readyCheckPending) {
            lua_pushstring(L, "waiting");
            return 1;
        }

        if (member->m_readyCheckReady) {
            lua_pushstring(L, "ready");
            return 1;
        }

        lua_pushstring(L, "notready");
        return 1;
    }

    if (!CGPartyInfo::m_members[0] || CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        lua_pushstring(L, status);
        return 1;
    }

    int32_t index = CGRaidInfo::GetPartyMemberIndex(guid);
    uint32_t now = FrameTime::GetCurTimeMs();

    if (!CGRaidInfo::m_readyCheckTime || static_cast<int32_t>(now - CGRaidInfo::m_readyCheckTime) >= 0 || CGRaidInfo::m_readyCheckTime == now) {
        lua_pushstring(L, status);
        return 1;
    }

    if (index == -1) {
        if (guid == CGRaidInfo::m_readyCheckInitiator) {
            status = "ready";
        }

        lua_pushstring(L, status);
        return 1;
    }

    if (CGRaidInfo::IsPartyMemberReadyCheckPending(index)) {
        lua_pushstring(L, "waiting");
        return 1;
    }

    if (CGRaidInfo::IsPartyMemberReadyCheckReady(index)) {
        lua_pushstring(L, "ready");
        return 1;
    }

    lua_pushstring(L, "notready");
    return 1;
}

// OFFSET: 0x0092E850/0x140695DE0
static int32_t Script_RequestRaidInfo(lua_State* L) {
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_RAID_INFO));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x0092E8F0/0x140695EB0
static int32_t Script_GetNumSavedInstances(lua_State* L) {
    lua_pushnumber(L, s_savedInstances.Count());
    return 1;
}

// OFFSET: 0x0092E930/0x140695EE0
static int32_t Script_GetSavedInstanceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSavedInstanceInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MapRec* map;

    if (index >= s_savedInstances.Count() || !(map = g_mapDB.GetRecord(s_savedInstances.m_data[index].m_mapID))) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, 0);
        lua_pushboolean(L, 0);
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, 0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 12;
    }

    RaidInstanceInfo* info = &s_savedInstances.m_data[index];
    int32_t mapId = info->m_mapID;
    int32_t difficulty = info->m_difficulty;
    int32_t heroic = info->m_heroic;
    int32_t difficultyIndex = difficulty + 2 * heroic;
    lua_pushstring(L, map->m_mapName);
    lua_pushnumber(L, static_cast<uint32_t>(info->m_instanceID));
    lua_pushnumber(L, info->m_resetTime);
    lua_pushnumber(L, difficultyIndex + 1);
    lua_pushboolean(L, info->m_locked);
    lua_pushboolean(L, info->m_extended);
    lua_pushnumber(L, static_cast<uint32_t>(info->m_instanceID >> 32));
    lua_pushboolean(L, map->m_instanceType == 2);
    lua_pushnumber(L, GetMaxPlayersForMap(map, difficulty));

    if (!(map->m_flags & 0x8000) || heroic) {
        if (MapDifficultyRec* rec = GetMapDifficultyRec(mapId, difficultyIndex, NULL)) {
            lua_pushstring(L, FrameScript_GetText(rec->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
        } else {
            lua_pushstring(L, "");
        }
    } else {
        lua_pushstring(L, FrameScript_GetText("PLAYER_DIFFICULTY1", -1, GENDER_NOT_APPLICABLE));
    }

    uint32_t total;
    uint32_t completed;
    DungeonEncounterCountMatchingRows(mapId, difficulty, info->m_completedMask, &total, &completed);
    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return 12;
}

// OFFSET: 0x0092EBE0/0x140696210
static int32_t Script_GetSavedInstanceEncounterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetSavedInstanceEncounterInfo(instanceIndex, encounterIndex)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    double encounter = lua_tonumber(L, 2);

    if (index < s_savedInstances.Count() && g_mapDB.GetRecord(s_savedInstances.m_data[index].m_mapID)) {
        RaidInstanceInfo* info = &s_savedInstances.m_data[index];
        return PushDungeonEncounterInfo(L, info->m_mapID, info->m_difficulty, static_cast<int32_t>(encounter) - 1, info->m_completedMask, 0);
    }

    return PushDungeonEncounterInfo(L, 0, 0, 0, 0, 0);
}

// OFFSET: 0x0092ECE0/0x140696310
static int32_t Script_GetSavedInstanceChatLink(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (index < s_savedInstances.Count()) {
            RaidInstanceInfo* info = &s_savedInstances.m_data[index];
            lua_pushstring(L, CGTooltip::GetInstanceLockHyperlink(info->m_mapID, info->m_difficulty + 2 * info->m_heroic, info->m_completedMask));
        } else {
            lua_pushstring(L, "");
        }

        return 1;
    }

    luaL_error(L, "Usage: GetSavedInstanceInfo(index)");
    return 0;
}

// OFFSET: 0x0092EF70/0x1406966E0
static int32_t Script_SetSavedInstanceExtend(lua_State* L) {
    if (!lua_isnumber(L, 1) || lua_type(L, 2) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetSavedInstanceExtend(index, extend)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t extend = lua_toboolean(L, 2);

    if (static_cast<uint32_t>(index) >= s_savedInstances.Count()) {
        return 0;
    }

    RaidInstanceInfo* info = &s_savedInstances.m_data[index];
    info->m_extended = extend;
    char mapName[1024];
    CGRaidInfo::GetMapNameWithDifficulty(mapName, sizeof(mapName), info->m_mapID, info->m_difficulty, info->m_heroic);
    const char* format = extend ? FrameScript_GetText("RAID_INSTANCE_LOCK_EXTENDED", -1, GENDER_NOT_APPLICABLE) : FrameScript_GetText("RAID_INSTANCE_LOCK_NOT_EXTENDED", -1, GENDER_NOT_APPLICABLE);
    char text[1024];
    SStrPrintf(text, sizeof(text), format, mapName);
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_SAVED_INSTANCE_EXTEND));
    msg.Put(info->m_mapID);
    msg.Put(info->m_difficulty);
    msg.Put(static_cast<uint8_t>(extend != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x0092F120/0x1406968E0
static int32_t Script_PlaceRaidMarker(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PlaceRaidMarker(index)");
        return 0;
    }

    if (CGRaidInfo::m_numMembers) {
        CGRaidMember* member = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

        if (!member || !member->m_rank) {
            return 0;
        }
    } else if (!CGPartyInfo::m_members[0]) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 5) {
        Spell_C_CastSpell(s_raidMarkerSpells[index], NULL, WOWGUID(), false, false, 0, 0);
    }

    return 0;
}

// OFFSET: 0x0092F1E0/0x1406969D0
static int32_t Script_ClearRaidMarker(lua_State* L) {
    if (CGRaidInfo::m_numMembers) {
        CGRaidMember* member = CGRaidInfo::FindMemberByGuid(ClntObjMgrGetActivePlayer());

        if (!member || !member->m_rank) {
            return 0;
        }
    } else if (!CGPartyInfo::m_members[0]) {
        return 0;
    }

    uint8_t marker = 5;

    if (lua_isnumber(L, 1)) {
        marker = static_cast<uint8_t>(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    }

    UserClientClearRaidMarker msg;
    msg.m_markerId = marker;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x0092F280/0x140696AA0
static int32_t Script_SetEveryoneIsAssistant(lua_State* L) {
    ThrottleObject throttle(3, 10);

    if (throttle.IncrementAndCheck()) {
        UserClientSetEveryoneIsAssistant msg;
        msg.m_everyoneIsAssistant = lua_toboolean(L, 1) != 0;
        ClientServices::Send(&msg);
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x0092FB70/0x140697680
static int32_t Script_SetRaidSubgroup(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetRaidSubgroup(index, subgroup)");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(11)) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t subgroup = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (subgroup >= 8) {
        luaL_error(L, "Usage: SetRaidSubgroup(index, subgroup)");
        return 0;
    }

    CGRaidMember* member = CGRaidInfo::GetMember(index);

    if (!member || member->m_subgroup == subgroup) {
        return 0;
    }

    WOWGUID requester = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(member->m_guid.m_guid, requester, NULL, NULL, false);

    if (!name) {
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GROUP_CHANGE_SUB_GROUP));
    char fullName[308];
    msg.PutString(name->FullName(fullName, 305));
    msg.Put(static_cast<uint8_t>(subgroup));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x0092FCD0/0x1406978B0
static int32_t Script_SwapRaidSubgroup(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SwapRaidSubgroup(index1, index2)");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(11)) {
        return 0;
    }

    uint32_t index1 = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t index2 = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    CGRaidMember* member1 = index1 >= CGRaidInfo::m_numMembers ? NULL : CGRaidInfo::m_members[index1];
    CGRaidMember* member2 = index2 < CGRaidInfo::m_numMembers ? CGRaidInfo::m_members[index2] : NULL;

    if (!member1 || !member2 || member1->m_subgroup == member2->m_subgroup) {
        return 0;
    }

    WOWGUID requester1 = WOWGUID();
    NameCache* name1 = g_nameDBCache.GetRecord(member1->m_guid.m_guid, requester1, NULL, NULL, false);
    WOWGUID requester2 = WOWGUID();
    NameCache* name2 = g_nameDBCache.GetRecord(member2->m_guid.m_guid, requester2, NULL, NULL, false);

    if (name1 && name2) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_GROUP_SWAP_SUB_GROUP));
        char fullName1[308];
        msg.PutString(name1->FullName(fullName1, 305));
        char fullName2[308];
        msg.PutString(name2->FullName(fullName2, 305));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x0092FE70/0x140697B90
static int32_t Script_ConvertToRaid(lua_State* L) {
    if ((CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) && CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) && CGPartyInfo::m_members[0]) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_GROUP_RAID_CONVERT));
            msg.Put(static_cast<uint8_t>(1));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    return 0;
}

// OFFSET: 0x0092FF50/0x140697C80
static int32_t Script_ConvertToParty(lua_State* L) {
    if ((CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) && CGPartyInfo::m_leader == ClntObjMgrGetActivePlayer()) {
        if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) && CGRaidInfo::m_numMembers) {
            CDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_GROUP_RAID_CONVERT));
            msg.Put(static_cast<uint8_t>(0));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }
    }

    return 0;
}

// OFFSET: 0x00930030/0x140697D70
static int32_t Script_CanBeRaidTarget(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CanBeRaidTarget(\"unit\")");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));
    lua_pushboolean(L, CGRaidInfo::CanBeRaidTarget(guid));
    return 1;
}

// OFFSET: 0x00930090/0x140697DD0
static int32_t Script_SetRaidTarget(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetRaidTarget(unit, index)");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));

    if (!guid) {
        return 0;
    }

    uint32_t index = static_cast<uint8_t>(static_cast<int32_t>(lua_tonumber(L, 2))) - 1;

    if (index >= 28) {
        CGRaidInfo::SetRaidTarget(WOWGUID(), GetRaidTargetIndexFromGuid(guid));
        return 0;
    }

    if (index < 18) {
        CGRaidInfo::SetRaidTarget(guid, index);
    }

    return 0;
}

// OFFSET: 0x00930160/0x140697E90
static int32_t Script_SetRaidTargetProtected(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetRaidTargetProtected(unit, index)");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));

    if (!guid) {
        return 0;
    }

    uint32_t index = static_cast<uint8_t>(static_cast<int32_t>(lua_tonumber(L, 2))) - 1;

    if (index >= 28) {
        index = GetRaidTargetIndexFromGuid(guid);
        guid = WOWGUID();
    }

    CGRaidInfo::SetRaidTarget(guid, index);
    return 0;
}

// STATIC: 0x00D11AB8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumRaidMembers", &Script_GetNumRaidMembers },
    { "GetRealNumRaidMembers", &Script_GetRealNumRaidMembers },
    { "GetRaidRosterInfo", &Script_GetRaidRosterInfo },
    { "SetRaidRosterSelection", &Script_SetRaidRosterSelection },
    { "GetRaidRosterSelection", &Script_GetRaidRosterSelection },
    { "IsRaidLeader", &Script_IsRaidLeader },
    { "IsRealRaidLeader", &Script_IsRealRaidLeader },
    { "IsRaidOfficer", &Script_IsRaidOfficer },
    { "SetRaidSubgroup", &Script_SetRaidSubgroup },
    { "SwapRaidSubgroup", &Script_SwapRaidSubgroup },
    { "ConvertToRaid", &Script_ConvertToRaid },
    { "ConvertToParty", &Script_ConvertToParty },
    { "PromoteToLeader", &Script_PromoteToLeader },
    { "PromoteToAssistant", &Script_PromoteToAssistant },
    { "DemoteAssistant", &Script_DemoteAssistant },
    { "CanBeRaidTarget", &Script_CanBeRaidTarget },
    { "SetRaidTarget", &Script_SetRaidTarget },
    { "SetRaidTargetProtected", &Script_SetRaidTargetProtected },
    { "GetRaidTargetIndex", &Script_GetRaidTargetIndex },
    { "DoReadyCheck", &Script_DoReadyCheck },
    { "ConfirmReadyCheck", &Script_ConfirmReadyCheck },
    { "GetReadyCheckTimeLeft", &Script_GetReadyCheckTimeLeft },
    { "GetReadyCheckStatus", &Script_GetReadyCheckStatus },
    { "RequestRaidInfo", &Script_RequestRaidInfo },
    { "GetNumSavedInstances", &Script_GetNumSavedInstances },
    { "GetSavedInstanceInfo", &Script_GetSavedInstanceInfo },
    { "GetSavedInstanceEncounterInfo", &Script_GetSavedInstanceEncounterInfo },
    { "GetSavedInstanceChatLink", &Script_GetSavedInstanceChatLink },
    { "SetSavedInstanceExtend", &Script_SetSavedInstanceExtend },
    { "PlaceRaidMarker", &Script_PlaceRaidMarker },
    { "ClearRaidMarker", &Script_ClearRaidMarker },
    { "IsRaidMarkerActive", &Script_IsRaidMarkerActive },
    { "IsInFakeRaid", &Script_IsInFakeRaid },
    { "SetEveryoneIsAssistant", &Script_SetEveryoneIsAssistant },
    { "IsEveryoneAssistant", &Script_IsEveryoneAssistant },
};

// OFFSET: 0x0092D420/0x1406943A0
void RaidInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0092D450/0x1406943F0
void RaidInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
