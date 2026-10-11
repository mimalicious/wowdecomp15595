#include "BattlefieldInfo.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/DB/Rec/WorldStateUIRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Client.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BFAC68
static int32_t s_arenaTeamSizes[3] = { 2, 3, 5 };

// STATIC: #0x00AD6B14
static const char* s_vehicleTypes[5] = { "Drive", "Fly", "Idle", "Airship Horde", "Airship Alliance" };

// OFFSET: 0x0093D440/0x1406A8E80
static int32_t Script_JoinArena(lua_State* L) {
    for (int32_t i = 0; i < 3; i++) {
        if (s_arenaTeamSizes[i] == static_cast<int32_t>(CGPartyInfo::m_realNumMembers + 1)) {
            UserClientBattlemasterJoinArena msg(i);
            ClientServices::Send(&msg);
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x0093D490/0x1406A8EE0
static int32_t Script_JoinRatedBattlefield(lua_State* L) {
    UserClientBattlemasterJoinRated msg;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x0093D4C0/0x1406A8F10
static int32_t Script_GetMaxBattlefieldID(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_maxBattlefieldID);
    return 1;
}

// OFFSET: 0x0093D500/0x1406A8F00
static int32_t Script_GetBattlefieldInstanceExpiration(lua_State* L) {
    if (!CGBattlefieldInfo::m_instanceExpiration) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (static_cast<int32_t>(now - CGBattlefieldInfo::m_instanceExpiration) < 0) {
        lua_pushnumber(L, CGBattlefieldInfo::m_instanceExpiration - now);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x0093D590/0x1406A8F90
static int32_t Script_GetBattlefieldInstanceRunTime(lua_State* L) {
    uint32_t start = CGBattlefieldInfo::m_instanceStartTime;

    if (start) {
        lua_pushnumber(L, FrameTime::GetCurTimeMs() - start);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x0093D5F0/0x1406A8FF0
static int32_t Script_RequestBattlefieldScoreData(lua_State* L) {
    CGBattlefieldInfo::RequestScoreData();
    return 0;
}

// OFFSET: 0x0093D600/0x1406A9000
static int32_t Script_GetNumBattlefieldScores(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numScores);
    return 1;
}

// OFFSET: 0x0093D640/0x1406A9030
static int32_t Script_GetBattlefieldWinner(lua_State* L) {
    if (CGBattlefieldInfo::m_mapDone) {
        lua_pushnumber(L, CGBattlefieldInfo::m_winner);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0093D6A0/0x1406A9090
static int32_t Script_GetNumBattlefieldStats(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numWorldStateStats);
    return 1;
}

// OFFSET: 0x0093D6E0/0x1406A90D0
static int32_t Script_GetBattlefieldStatData(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetBattlefieldStatData(playerIndex, statIndex)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldScore* score;
    double value;
    uint32_t stat;

    if (index < CGBattlefieldInfo::m_numScoreEntries && (score = CGBattlefieldInfo::m_scores[index]) && (stat = static_cast<int32_t>(lua_tonumber(L, 2)) - 1, stat <= 8)) {
        value = score->m_stats[stat];
    } else {
        value = 0.0;
    }

    lua_pushnumber(L, value);
    return 1;
}

// OFFSET: 0x0093D7B0/0x1406A91A0
static int32_t Script_GetNumBattlefieldPositions(lua_State* L) {
    uint32_t count = CGBattlefieldInfo::m_numPlayerPositions;
    uint32_t visible = count;

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid = i < CGBattlefieldInfo::m_numPlayerPositions ? CGBattlefieldInfo::m_playerPositionGuids[i] : WOWGUID();

        if (CGPartyInfo::IsMember(guid) || IsGuidInMyRaid(guid)) {
            visible--;
        }
    }

    lua_pushnumber(L, visible);
    return 1;
}

// OFFSET: 0x0093D840/0x1406A9240
static int32_t Script_GetNumBattlefieldFlagPositions(lua_State* L) {
    uint32_t count;

    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        count = 2;
    } else if (CGBattlefieldInfo::m_flagCarriers[0]) {
        count = 1;
    } else {
        count = 0;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0093D8A0/0x1406A9290
static int32_t Script_GetNumBattlefieldVehicles(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numVehicles);
    return 1;
}

// OFFSET: 0x0093D8E0/0x1406A92D0
static int32_t Script_GetBattlefieldTeamInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldTeamInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    lua_pushstring(L, index < 2 ? CGBattlefieldInfo::m_teamNames[index] : NULL);
    lua_pushnumber(L, index < 2 ? CGBattlefieldInfo::m_teamRatings[index] : 0);
    lua_pushnumber(L, index < 2 ? CGBattlefieldInfo::m_newTeamRatings[index] : 0);
    lua_pushnumber(L, index < 2 ? CGBattlefieldInfo::m_avgMatchmakingRating[index] : 0);
    lua_pushnumber(L, index < 2 ? CGBattlefieldInfo::m_teamPlayerCounts[index] : 0);
    return 5;
}

// PROVEN: 1e4f6a4d IDENTICAL/NONE
// OFFSET: 0x0093DA00/0x1406A93F0
static int32_t Script_GetBattlefieldArenaFaction(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_arenaFaction);
    return 1;
}

// OFFSET: 0x0093DA30/0x1406A9420
static int32_t Script_GetNumArenaOpponents(lua_State* L) {
    lua_pushnumber(L, GetArenaPetGUIDCount());
    return 1;
}

// OFFSET: 0x0093DA70/0x1406A9460
static int32_t Script_GetHolidayBGHonorCurrencyBonuses(lua_State* L) {
    lua_pushboolean(L, CGBattlefieldInfo::m_hasHolidayWinToday);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayWinHonor);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayWinArena);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayLossHonor);
    lua_pushnumber(L, 0.0);
    return 5;
}

// OFFSET: 0x0093DAE0/0x1406A94D0
static int32_t Script_GetRandomBGHonorCurrencyBonuses(lua_State* L) {
    lua_pushboolean(L, CGBattlefieldInfo::m_hasRandomWinToday);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomWinHonor);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomWinArena);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomLossHonor);
    lua_pushnumber(L, 0.0);
    return 5;
}

// OFFSET: 0x0093DB50/0x1406A9500
static int32_t Script_GetPersonalRatedBGInfo(lua_State* L) {
    PersonalRatedInfo* info = &CGBattlefieldInfo::m_personalRatedInfo[3];
    lua_pushinteger(L, info->m_rating);
    lua_pushinteger(L, info->m_seasonPlayed);
    lua_pushinteger(L, info->m_seasonWon);
    lua_pushinteger(L, info->m_weeklyPlayed);
    lua_pushinteger(L, info->m_weeklyWon);
    lua_pushinteger(L, info->m_weeklyCap);
    lua_pushinteger(L, CGBattlefieldInfo::m_ratedBgStats[1]);
    lua_pushinteger(L, CGBattlefieldInfo::m_ratedBgStats[0]);
    lua_pushinteger(L, CGBattlefieldInfo::m_ratedBgStats[3]);
    lua_pushinteger(L, CGBattlefieldInfo::m_ratedBgStats[2]);
    return 10;
}

// OFFSET: 0x0093DBF0/0x1406A95A0
static int32_t Script_GetPersonalRatedArenaInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPersonalRatedArenaInfo(accept)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index > 2) {
        return 0;
    }

    PersonalRatedInfo* info = &CGBattlefieldInfo::m_personalRatedInfo[index];

    if (!info) {
        return 0;
    }

    lua_pushinteger(L, info->m_rating);
    lua_pushinteger(L, info->m_seasonPlayed);
    lua_pushinteger(L, info->m_seasonWon);
    lua_pushinteger(L, info->m_weeklyPlayed);
    lua_pushinteger(L, info->m_weeklyWon);
    lua_pushinteger(L, info->m_weeklyCap);
    return 6;
}

// OFFSET: 0x0093DCA0/0x1406A9680
static int32_t Script_GetNumWarGameTypes(lua_State* L) {
    lua_pushinteger(L, CGBattlefieldInfo::m_numWarGameTypes);
    return 1;
}

// OFFSET: 0x0093DCC0/0x1406A96A0
static int32_t Script_RequestPVPRewards(lua_State* L) {
    UserClientRequestPvpRewards msg;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x0093DCF0/0x1406A96D0
static int32_t Script_GetPVPRewards(lua_State* L) {
    return CGBattlefieldInfo::PushPVPRewards(L);
}

// OFFSET: 0x0093DD00/0x1406A96E0
static int32_t Script_ClearBattlemaster(lua_State* L) {
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELDS_CLOSED, NULL);
    CGGameUI::ClearInteractTarget(CGBattlefieldInfo::m_battlemaster, 0);
    CGBattlefieldInfo::m_battlemaster.m_guid = 0;
    return 0;
}

// OFFSET: 0x0093F180/0x1406AAED0
static int32_t Script_GetNumWorldPVPAreas(lua_State* L) {
    lua_pushnumber(L, g_world_PVP_AreaDB.m_numRecords);
    return 1;
}

// OFFSET: 0x0093F1B0/0x1406AAF80
static int32_t Script_JoinBattlefield(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: JoinBattlefield(id, joinAsGroup)");
        return 0;
    }

    CGBattlefieldInfo::JoinBattlefield(StringToBOOL(L, 2, 0));
    return 0;
}

// OFFSET: 0x0093F200/0x1406AAFD0
static int32_t Script_GetBattlefieldScore(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldScore(index)");
        return 0;
    }

    lua_checkstack(L, 12);
    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldScore* score;
    NameCache* name;
    WOWGUID none = WOWGUID();

    if (index >= CGBattlefieldInfo::m_numScoreEntries || !(score = CGBattlefieldInfo::m_scores[index]) || !(name = g_nameDBCache.GetRecord(score->m_guid.m_guid, none, NULL, NULL, false))) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 16;
    }

    char fullName[308];
    lua_pushstring(L, name->FullName(fullName, 305));
    lua_pushnumber(L, score->m_killingBlows);
    lua_pushnumber(L, score->m_honorableKills);
    lua_pushnumber(L, score->m_deaths);
    lua_pushnumber(L, score->m_bonusHonor);
    lua_pushnumber(L, score->m_faction);
    lua_pushstring(L, CGUnit_C::GetDisplayRaceName(NULL, name));
    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(name->m_class);
    const char* className = CGUnit_C::GetDisplayClassName(NULL, name);

    if (chrClass && className) {
        lua_pushstring(L, className);
        lua_pushstring(L, chrClass->m_filename);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    lua_pushnumber(L, score->m_damageDone);
    lua_pushnumber(L, score->m_healingDone);
    lua_pushnumber(L, score->m_rating);
    lua_pushnumber(L, score->m_ratingChange);
    lua_pushnumber(L, score->m_preMatchMMR);
    lua_pushnumber(L, score->m_mmrChange);
    TalentTabRec* talentTab = g_talentTabDB.GetRecord(score->m_talentSpec);

    if (talentTab) {
        lua_pushstring(L, talentTab->m_name);
    } else {
        lua_pushnil(L);
    }

    return 16;
}

// PROVEN: b5812a08 IDENTICAL/NONE
// OFFSET: 0x0093F560/0x1406AB340
static int32_t Script_SetBattlefieldScoreFaction(lua_State* L) {
    int32_t faction = -1;

    if (!lua_isnumber(L, 1) || (faction = static_cast<int32_t>(lua_tonumber(L, 1)), faction == -1) || static_cast<uint32_t>(faction) < 2) {
        CGBattlefieldInfo::m_logFilterFaction = faction;
        BattlefieldNameCacheCallback();
    }

    return 0;
}

// OFFSET: 0x0093F5B0/0x1406AB390
static int32_t Script_GetBattlefieldStatInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldStatInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t id = index >= CGBattlefieldInfo::m_numWorldStateStats ? 0 : CGBattlefieldInfo::m_worldStateStats[index];
    WorldStateUIRec* rec = g_worldStateUIDB.GetRecord(id);

    if (rec) {
        lua_pushstring(L, rec->m_string);
        lua_pushstring(L, rec->m_icon);
        lua_pushstring(L, rec->m_tooltip);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x0093F690/0x1406AB4C0
static int32_t Script_GetBattlefieldPosition(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldPosition(index)");
        return 0;
    }

    double value = lua_tonumber(L, 1);
    uint32_t count = CGBattlefieldInfo::m_numPlayerPositions;
    uint32_t index = 0;
    int32_t skip = static_cast<int32_t>(value) - 1;
    WOWGUID guid;
    float x;
    float y;

    for (; index < count; index++) {
        guid = index < CGBattlefieldInfo::m_numPlayerPositions ? CGBattlefieldInfo::m_playerPositionGuids[index] : WOWGUID();

        if (CGPartyInfo::IsMember(guid) || IsGuidInMyRaid(guid)) {
            continue;
        }

        if (!skip) {
            break;
        }

        skip--;
    }

    if (index >= count) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 3;
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        position = player->GetPosition();
    } else if (!CGBattlefieldInfo::GetPlayerPosition(index, &position)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 3;
    }

    CGWorldMap::GetWorldPosition(position, CGBattlefieldInfo::m_activeMapID, &x, &y, NULL, false, 0);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, WorldMapNameUpdateCallback, NULL, false);

    if (name) {
        char fullName[308];
        lua_pushstring(L, name->FullName(fullName, 305));
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x0093F890/0x1406AB6C0
static int32_t Script_GetBattlefieldVehicleInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldVehicleInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID guid = index < CGBattlefieldInfo::m_numVehicles ? CGBattlefieldInfo::m_vehicleGuids[index] : WOWGUID();
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    int32_t mapID = CGGameUI::m_iCurrentMapID;
    float x = 0.0f;
    float y = 0.0f;

    if (!CGWorldMap::GetWorldPosition(unit->GetPosition(), mapID, &x, &y, NULL, false, 0) || !fnotequal(x, 0.0f) || !fnotequal(y, 0.0f)) {
        return 0;
    }

    VehicleRec* vehicle = unit->GetVehicleRec();
    bool hasType = vehicle && (vehicle->m_flags & 0x10000000);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushstring(L, unit->GetUnitName(NULL, 1));
    lua_pushboolean(L, unit->m_unit->m_unitFieldBytes2[3] & 0x1);

    if (hasType) {
        lua_pushstring(L, s_vehicleTypes[vehicle->m_uiLocomotionType]);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, unit->GetFacing());
    lua_pushboolean(L, unit->IsLocalClientControlled());
    lua_pushboolean(L, unit->IsDeadOrGhost() == 0);
    return 8;
}

// PROVEN: 26b88a9b IDENTICAL/NONE
// OFFSET: 0x0093FAA0/0x1406AB8D0
static int32_t Script_CanJoinBattlefieldAsGroup(lua_State* L) {
    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_bgTypeID);

    if (rec && rec->m_groupsAllowed) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 3db2a8c6 IDENTICAL/NONE
// OFFSET: 0x0093FB00/0x1406AB930
static int32_t Script_GetBattlefieldMapIconScale(lua_State* L) {
    MapRec* map = g_mapDB.GetRecord(CGBattlefieldInfo::m_activeMapID);

    if (map) {
        lua_pushnumber(L, map->m_minimapIconScale);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: 0x0093FB60/0x1406AB990
static int32_t Script_HearthAndResurrectFromArea(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_HEARTH_AND_RESURRECT));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x0093FC30/0x1406ABA60
static int32_t Script_GetNumBattlegroundTypes(lua_State* L) {
    lua_pushnumber(L, static_cast<int32_t>(CGBattlefieldInfo::m_battlegroundTypes.Count()));
    return 1;
}

// OFFSET: 0x0093FC60/0x1406ABA90
static int32_t Script_GetWorldPVPQueueStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetWorldPVPQueueStatus(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 2) {
        return 0;
    }

    WorldPvPQueueStatus* queue = &CGBattlefieldInfo::m_worldPvPQueues[index];

    if (!queue) {
        return 0;
    }

    switch (queue->m_status) {
        case 0:
            lua_pushstring(L, "none");
            break;

        case 1:
            lua_pushstring(L, "queued");
            break;

        case 2:
            lua_pushstring(L, "confirm");
            break;

        case 4:
            lua_pushstring(L, "active");
            break;

        default:
            lua_pushstring(L, "error");
            break;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(queue->m_areaID);

    if (area) {
        lua_pushstring(L, area->m_areaName);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<uint16_t>(queue->m_queueID));
    lua_pushnumber(L, queue->m_status == 2 ? static_cast<double>(queue->m_expireTime - CGWorldStateInfo::GetUITime()) : 0.0);
    return 4;
}

// OFFSET: 0x0093FDC0/0x1406ABBF0
static int32_t Script_SortBGList(lua_State* L) {
    lua_pushboolean(L, 1);
    qsort(CGBattlefieldInfo::m_battlegroundTypes.m_data, CGBattlefieldInfo::m_battlegroundTypes.Count(), sizeof(int32_t), QSortBattlefields);
    return 1;
}

// OFFSET: 0x0093FE00/0x1406ABDC0
static int32_t Script_RequestRatedBattlegroundInfo(lua_State* L) {
    // STATIC: 0x00F36D78
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        UserClientRequestRatedInfo infoMsg;
        infoMsg.unk_0010 = 3;
        ClientServices::Send(&infoMsg);
        UserClientRequestRatedBgInfo bgMsg;
        ClientServices::Send(&bgMsg);
    }

    return 0;
}

// OFFSET: 0x0093FE90/0x1406ABE60
static int32_t Script_RequestRatedArenaInfo(lua_State* L) {
    // STATIC: 0x00F36D8C
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: RequestRatedArenaInfo(index)");
            return 0;
        }

        uint32_t index = lua_tointeger(L, 1) - 1;

        if (index <= 2) {
            UserClientRequestRatedInfo msg;
            msg.unk_0010 = index;
            ClientServices::Send(&msg);
        }
    }

    return 0;
}

// OFFSET: 0x0093FF40/0x1406ABF20
static int32_t Script_GetRatedBattleGroundInfo(lua_State* L) {
    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(GetWorldState(5508) + 99);

    if (rec) {
        lua_pushstring(L, rec->m_name);
        lua_pushinteger(L, rec->m_ratedPlayers);
        return 2;
    }

    return 0;
}

// OFFSET: 0x0093FFA0/0x1406ABFB0
static int32_t Script_CommentatorStartWargame(lua_State* L) {
    UserClientCommentatorStartWargame msg;
    *reinterpret_cast<uint64_t*>(msg.unk_0010) = ((lua_tointeger(L, 2) & 0x3F) << 24) | static_cast<uint16_t>(lua_tointeger(L, 1)) | 0x1F10000000030000ull;

    for (int32_t i = 0; i < 2; i++) {
        if (!lua_isstring(L, i + 3)) {
            luaL_error(L, "arg %d is not a string", i + 3);
            return 0;
        }

        const char* name = lua_tolstring(L, i + 3, NULL);

        if (!name) {
            luaL_error(L, "arg %d is not a string", i + 3);
            return 0;
        }

        if (strlen(name) > 48) {
            luaL_error(L, "Name too long");
            return 0;
        }

        SStrCopy(i ? msg.unk_0048 : msg.unk_0018, name, 48);
    }

    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x00940100/0x1406AC100
static int32_t Script_RequestPVPOptionsEnabled(lua_State* L) {
    // STATIC: 0x00F36DA0
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        UserClientGetPVPOptionsEnabled msg;
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x00940170/0x1406AC180
static int32_t Script_GetWarGameTypeInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetWarGameInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= static_cast<uint32_t>(CGBattlefieldInfo::m_numWarGameTypes) || index >= CGBattlefieldInfo::m_warGameTypes.Count()) {
        return 0;
    }

    WarGameType* type = CGBattlefieldInfo::m_warGameTypes[index];

    if (!type) {
        return 0;
    }

    if (type->m_bgTypeID == -1) {
        lua_pushstring(L, "header");
        lua_pushinteger(L, type->m_instanceType);
        lua_pushboolean(L, CGBattlefieldInfo::IsWarGameHeaderCollapsed(index));
        return 3;
    }

    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(type->m_bgTypeID);

    if (!rec) {
        return 0;
    }

    lua_pushstring(L, rec->m_name);
    lua_pushinteger(L, rec->m_instanceType);
    lua_pushboolean(L, 0);
    lua_pushinteger(L, rec->m_id);
    lua_pushinteger(L, rec->m_minPlayers);
    lua_pushinteger(L, rec->m_maxPlayers);
    lua_pushboolean(L, rec->m_id == 32 || rec->m_id == 6);
    return 7;
}

// OFFSET: 0x009402B0/0x1406AC320
static int32_t Script_SetSelectedWarGameType(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedWarGameType(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGBattlefieldInfo::m_warGameTypes.Count()) {
        WarGameType* type = CGBattlefieldInfo::m_warGameTypes[index];

        if (type->m_bgTypeID != -1) {
            CGBattlefieldInfo::m_selectedWarGameType = type->m_bgTypeID;
        }
    }

    return 0;
}

// OFFSET: 0x00940330/0x1406AC390
static int32_t Script_GetSelectedWarGameType(lua_State* L) {
    int32_t index;

    if (CGBattlefieldInfo::m_selectedWarGameType < 0 || CGBattlefieldInfo::m_numWarGameTypes <= 0) {
        index = -1;
    } else {
        for (index = 0; CGBattlefieldInfo::m_warGameTypes[index]->m_bgTypeID != CGBattlefieldInfo::m_selectedWarGameType; index++) {
            if (index + 1 >= CGBattlefieldInfo::m_numWarGameTypes) {
                index = -1;
                break;
            }
        }
    }

    lua_pushnumber(L, index + 1);
    return 1;
}

// OFFSET: 0x00940D20/0x1406AD130
static int32_t Script_GetWorldPVPAreaInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetWorldPVPAreaInfo(index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= g_world_PVP_AreaDB.m_numRecords) {
        luaL_error(L, "invalid index");
        return 0;
    }

    World_PVP_AreaRec* rec;

    if (index < 0 || !(rec = g_world_PVP_AreaDB.GetRecordByIndex(index))) {
        luaL_error(L, "Area not found");
        return 0;
    }

    lua_pushnumber(L, rec->m_id);
    AreaTableRec* area = g_areaTableDB.GetRecord(rec->m_areaID);

    if (area) {
        lua_pushstring(L, area->m_areaName);
    } else {
        lua_pushstring(L, "");
    }

    int32_t nextTime = GetWorldState(rec->m_nextTimeWorldState);
    bool active = GetWorldState(rec->m_gameTimeWorldState) > 0;
    lua_pushboolean(L, active);
    bool canQueue = true;

    if (!active) {
        canQueue = static_cast<int32_t>(CGWorldStateInfo::GetUITime()) > nextTime - rec->m_battlePopulateTime;
    }

    lua_pushboolean(L, canQueue);
    int32_t remaining = nextTime - CGWorldStateInfo::GetUITime();

    if (remaining < 0) {
        remaining = 0;
    }

    lua_pushnumber(L, remaining);
    bool canEnter = false;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t level = player->m_unit->m_unitFieldLevel;

        if (level >= rec->m_minLevel && (!rec->m_maxLevel || level <= rec->m_maxLevel)) {
            canEnter = true;
        }
    }

    lua_pushboolean(L, canEnter);
    return 6;
}

// OFFSET: 0x00940EE0/0x1406ACF10
static int32_t Script_GetBattlefieldFlagPosition(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldFlagPosition(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID carrier = CGBattlefieldInfo::GetFlagCarrier(index);
    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;

    if (!carrier || (CGWorldMap::GetPlayerPosition(carrier, &position.x, &position.y), position.x == 0.0f && position.y == 0.0f)) {
        if (CGBattlefieldInfo::GetFlagPosition(index, &position)) {
            float x;
            float y;
            CGWorldMap::GetWorldPosition(position, CGBattlefieldInfo::m_activeMapID, &x, &y, NULL, false, 0);
            position.x = x;
            position.y = y;
            position.z = 0.0f;
        }
    }

    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);
    const char* token = CGBattlefieldInfo::GetFlagToken(index);

    if (token) {
        lua_pushstring(L, token);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x00941030/0x1406AD060
static int32_t Script_SortBattlefieldScoreData(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SortBattlefieldScoreData(\"type\")");
        return 0;
    }

    int32_t column = 0;
    const char* type = lua_tolstring(L, 1, NULL);

    if (!_stricmp(type, "kills")) {
        CGBattlefieldInfo::SetScoreSortColumn(0);
        return 0;
    }

    if (!_stricmp(type, "deaths")) {
        CGBattlefieldInfo::SetScoreSortColumn(1);
        return 0;
    }

    if (!_stricmp(type, "cp")) {
        CGBattlefieldInfo::SetScoreSortColumn(2);
        return 0;
    }

    if (!_stricmp(type, "name")) {
        CGBattlefieldInfo::SetScoreSortColumn(3);
        return 0;
    }

    if (!_stricmp(type, "class")) {
        CGBattlefieldInfo::SetScoreSortColumn(4);
        return 0;
    }

    if (!_stricmp(type, "hk")) {
        CGBattlefieldInfo::SetScoreSortColumn(5);
        return 0;
    }

    if (!_stricmp(type, "damage")) {
        CGBattlefieldInfo::SetScoreSortColumn(6);
        return 0;
    }

    if (!_stricmp(type, "healing")) {
        CGBattlefieldInfo::SetScoreSortColumn(7);
        return 0;
    }

    if (!_stricmp(type, "team")) {
        CGBattlefieldInfo::SetScoreSortColumn(8);
        return 0;
    }

    if (!_stricmp(type, "bgrating")) {
        CGBattlefieldInfo::SetScoreSortColumn(17);
        return 0;
    }

    if (!_stricmp(type, "bgratingChange")) {
        CGBattlefieldInfo::SetScoreSortColumn(18);
        return 0;
    }

    if (!_stricmp(type, "preMatchMMR")) {
        CGBattlefieldInfo::SetScoreSortColumn(19);
        return 0;
    }

    if (!_stricmp(type, "mmr")) {
        CGBattlefieldInfo::SetScoreSortColumn(20);
        return 0;
    }

    if (!_stricmp(type, "mmrChange")) {
        CGBattlefieldInfo::SetScoreSortColumn(21);
        return 0;
    }

    if (!_strnicmp(type, "stat", 4)) {
        type += 4;
        uint32_t stat = SStrToUnsigned(&type) + 8;

        if (stat >= 22) {
            return 0;
        }

        column = stat;
    }

    CGBattlefieldInfo::SetScoreSortColumn(column);
    return 0;
}

// OFFSET: 0x00941300/0x1406AD340
static int32_t Script_GetBattlegroundInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlegroundInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || index < 0 || index >= static_cast<int32_t>(CGBattlefieldInfo::m_battlegroundTypes.Count())) {
        return 0;
    }

    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::GetBattlegroundType(index));

    if (!rec) {
        return 0;
    }

    lua_pushstring(L, rec->m_name);
    int32_t bracket;

    if (CGBattlefieldInfo::GetBracketForLevel(player->m_unit->m_unitFieldLevel, rec, &bracket)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (GetWorldState(rec->m_holidayWorldState)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (rec->m_id == 32) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, rec->m_id);
    MapRec* map = g_mapDB.GetRecord(rec->m_mapID[0]);

    if (!map) {
        return 5;
    }

    int32_t faction = player->GetPVPFactionIndex();

    switch (faction) {
        case 0:
            lua_pushstring(L, map->m_mapDescription0);
            break;
        case 1:
            lua_pushstring(L, map->m_mapDescription1);
            break;
    }

    return 6;
}

// OFFSET: 0x009414B0/0x1406AD4E0
static int32_t Script_BattlefieldMgrEntryInviteResponse(lua_State* L) {
    int64_t queueID = static_cast<int32_t>(lua_tonumber(L, 1));
    CGBattlefieldInfo::BattlefieldMgrEntryInviteResponse(static_cast<uint16_t>(queueID), static_cast<int32_t>(queueID >> 32), StringToBOOL(L, 2, 0));
    return 0;
}

// OFFSET: 0x009414F0/0x1406AD510
static int32_t Script_BattlefieldMgrQueueRequest(lua_State* L) {
    // STATIC: 0x00F36DB4
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        CGBattlefieldInfo::BattlefieldMgrQueueRequest(static_cast<uint16_t>(static_cast<int64_t>(lua_tonumber(L, 1))));
    }

    return 0;
}

// OFFSET: 0x00941560/0x1406AD590
static int32_t Script_BattlefieldMgrQueueInviteResponse(lua_State* L) {
    int64_t queueID = static_cast<int32_t>(lua_tonumber(L, 1));
    CGBattlefieldInfo::BattlefieldMgrQueueInviteResponse(static_cast<uint16_t>(queueID), static_cast<int32_t>(queueID >> 32), StringToBOOL(L, 2, 0));
    return 0;
}

// OFFSET: 0x009415A0/0x1406AD5C0
static int32_t Script_BattlefieldMgrExitRequest(lua_State* L) {
    // STATIC: 0x00F36DC8
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        CGBattlefieldInfo::BattlefieldMgrExitRequest(static_cast<uint16_t>(static_cast<int64_t>(lua_tonumber(L, 1))));
    }

    return 0;
}

// OFFSET: 0x00941610/0x1406ADB40
static int32_t Script_StartWarGame(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (lua_isstring(L, 1)) {
        const char* targetName = lua_tolstring(L, 1, NULL);

        if (targetName) {
            if (strlen(targetName) > 48) {
                luaL_error(L, "Name too long");
                return 0;
            }

            NameCache* target = g_nameDBCache.GetRecordByName(targetName);

            if (target) {
                WOWGUID guid = target->m_guid;

                if (!(guid.m_guid & 0xF000000000000000ull) && (guid.m_guid & 0xF07FFFFFFFFFFFFFull)) {
                    int32_t bgTypeID = -1;
                    int32_t teamSize = 0;

                    if (lua_isstring(L, 2)) {
                        char search[1024];
                        SStrCopy(search, lua_tolstring(L, 2, NULL), sizeof(search));
                        CaseAccentInsensitiveParse(search, sizeof(search), s_locale);

                        for (int32_t i = 0; i < g_battlemasterListDB.m_numRecords; i++) {
                            BattlemasterListRec* rec = g_battlemasterListDB.GetRecordByIndex(i);

                            if (!rec || (rec->m_mapID[1] != -1 && rec->m_id != 32 && rec->m_id != 6)) {
                                continue;
                            }

                            char name[1024];
                            SStrCopy(name, rec->m_name, sizeof(name));
                            CaseAccentInsensitiveParse(name, sizeof(name), s_locale);

                            if (strstr(name, search)) {
                                bgTypeID = rec->m_id;

                                if (CGRaidInfo::m_numMembers) {
                                    teamSize = 0;
                                } else {
                                    teamSize = CGPartyInfo::NumMembers() + 1;
                                }

                                break;
                            }
                        }
                    } else if (CGRaidInfo::m_numMembers > 4) {
                        bgTypeID = 32;
                        teamSize = 0;
                    } else if (CGPartyInfo::NumMembers()) {
                        bgTypeID = 6;
                        teamSize = CGPartyInfo::NumMembers() + 1;
                    }

                    UserClientStartWarGame msg;
                    *reinterpret_cast<uint64_t*>(msg.unk_0010) = (static_cast<uint16_t>(bgTypeID) | ((teamSize & 0x3F) << 24) | 0x1F10000000030000ull);
                    *reinterpret_cast<uint64_t*>(msg.unk_0018) = guid.m_guid;
                    ClientServices::Send(&msg);
                    return 0;
                }
            }
        }
    }

    CGGameUI::DisplayError(ERR_WARGAME_REQUEST_FAILURE);
    return 0;
}

// OFFSET: 0x009418A0/0x1406ADE70
static int32_t Script_CanInitiateWarGame(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    BattlemasterListRec* rec;
    int32_t index;

    if (lua_isnumber(L, 1) && (index = lua_tointeger(L, 1) - 1, index >= 0) && index < static_cast<int32_t>(CGBattlefieldInfo::m_battlegroundTypes.Count()) && (rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::GetBattlegroundType(index)))) {
        bool canInitiate = false;

        if (player->m_player->m_playerFlags & 0x1) {
            int32_t members = CGRaidInfo::m_numMembers;

            if (members <= CGPartyInfo::NumMembers() + 1) {
                members = CGPartyInfo::NumMembers() + 1;
            }

            if (members >= rec->m_minPlayers && members <= rec->m_maxPlayers) {
                canInitiate = true;
            }
        }

        lua_pushboolean(L, canInitiate);
        lua_pushinteger(L, rec->m_minPlayers);
        lua_pushinteger(L, rec->m_maxPlayers);
    } else {
        lua_pushboolean(L, 0);
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }

    return 3;
}

// OFFSET: 0x009419A0/0x1406ADFE0
static int32_t Script_StartUnratedArena(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    const char* targetName;
    NameCache* target;

    if (!lua_isstring(L, 1) || !(targetName = lua_tolstring(L, 1, NULL))) {
        CGGameUI::DisplayError(ERR_WARGAME_REQUEST_FAILURE);
        return 0;
    }

    if (strlen(targetName) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    if (!(target = g_nameDBCache.GetRecordByName(targetName))) {
        CGGameUI::DisplayError(ERR_WARGAME_REQUEST_FAILURE);
        return 0;
    }

    WOWGUID guid = target->m_guid;
    int32_t bgTypeID = -1;

    if (lua_isnumber(L, 2)) {
        bgTypeID = lua_tointeger(L, 2);
    }

    if ((guid.m_guid & 0xF000000000000000ull) || !(guid.m_guid & 0xF07FFFFFFFFFFFFFull)) {
        CGGameUI::DisplayError(ERR_WARGAME_REQUEST_FAILURE);
        return 0;
    }

    if (bgTypeID == -1) {
        bgTypeID = 6;
    }

    uint32_t queue = static_cast<uint16_t>(bgTypeID) | (((CGPartyInfo::NumMembers() + 1) << 24) & 0x3F000000) | 0x30000;
    UserClientStartWarGame msg;
    *reinterpret_cast<uint64_t*>(msg.unk_0010) = queue | 0x1F10000000000000ull;
    *reinterpret_cast<uint64_t*>(msg.unk_0018) = guid.m_guid;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x00941B10/0x1406AE160
static int32_t Script_WarGameRespond(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: WarGameRespond(accept)");
        return 0;
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) && CGBattlefieldInfo::m_warGameRequester) {
        bool accept = lua_toboolean(L, 1) != 0;
        UserClientAcceptWargameInvite msg;
        *reinterpret_cast<uint64_t*>(msg.m_opposingPartyMemberGUID) = CGBattlefieldInfo::m_warGameRequester.m_guid;
        msg.m_accept = accept;
        *reinterpret_cast<uint64_t*>(msg.m_queueID) = CGBattlefieldInfo::m_warGameQueueID;
        ClientServices::Send(&msg);
        CGBattlefieldInfo::m_warGameRequester = WOWGUID();
        CGBattlefieldInfo::m_warGameQueueID = 0;
    }

    return 0;
}

// OFFSET: 0x00941BE0/0x1406AE230
static int32_t Script_IsInActiveWorldPVP(lua_State* L) {
    CGPlayer_C* player;

    if (lua_isstring(L, 1)) {
        WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 1, NULL));
        player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    } else {
        player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    }

    if (!player) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int32_t worldMapAreaID;

    if (CGWorldMap::m_currentContinent < 0) {
        worldMapAreaID = CGWorldMap::m_currentOrphanZone;
    } else if (CGWorldMap::m_currentZone < 0) {
        worldMapAreaID = CGWorldMap::m_continents[CGWorldMap::m_currentContinent].m_areaID;
    } else {
        worldMapAreaID = CGWorldMap::m_continents[CGWorldMap::m_currentContinent].m_zones[CGWorldMap::m_currentZone];
    }

    WorldMapAreaRec* worldMapArea = g_worldMapAreaDB.GetRecord(worldMapAreaID);
    int32_t index;

    if (!worldMapArea || (index = g_world_PVP_AreaDB.m_numRecords - 1, index < 0)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    for (; index >= 0; index--) {
        World_PVP_AreaRec* rec = index < g_world_PVP_AreaDB.m_numRecords ? g_world_PVP_AreaDB.GetRecordByIndex(index) : NULL;

        if (rec && rec->m_areaID == worldMapArea->m_areaID) {
            lua_pushboolean(L, GetWorldState(rec->m_gameTimeWorldState) > 0);
            return 1;
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x00941D20/0x1406AE370
static int32_t Script_CollapseWarGameHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseWarGameHeader(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < static_cast<uint32_t>(CGBattlefieldInfo::m_numWarGameTypes)) {
        WarGameType* type = CGBattlefieldInfo::m_warGameTypes[index];

        if (type->m_bgTypeID == -1) {
            CGBattlefieldInfo::m_collapsedWarGameHeaders |= 1 << type->m_instanceType;
            CGBattlefieldInfo::UpdateWarGameTypeList();
        }
    }

    return 0;
}

// OFFSET: 0x00941DB0/0x1406AE3F0
static int32_t Script_ExpandWarGameHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandWarGameHeader(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < static_cast<uint32_t>(CGBattlefieldInfo::m_numWarGameTypes)) {
        WarGameType* type = CGBattlefieldInfo::m_warGameTypes[index];

        if (type->m_bgTypeID == -1) {
            CGBattlefieldInfo::m_collapsedWarGameHeaders &= ~(1 << type->m_instanceType);
            CGBattlefieldInfo::UpdateWarGameTypeList();
        }
    }

    return 0;
}

// OFFSET: 0x00942280/0x1406AEB80
static int32_t Script_AcceptBattlefieldPort(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: AcceptBattlefieldPort(id, accept)");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(20)) {
        return 0;
    }

    BattlefieldStatusNode* status = CGBattlefieldInfo::FindStatus(static_cast<uint32_t>(lua_tonumber(L, 1)));

    if (!status) {
        lua_pushnil(L);
        return 1;
    }

    bool accept = StringToBOOL(L, 2, 0) != 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && accept && !IsCurrentMapArena()) {
        if (player->m_unit->m_unitFieldFlags & 0x800) {
            CGGameUI::DisplayError(ERR_NOT_IN_COMBAT);
            lua_pushnil(L);
            return 1;
        }

        if (player->IsDeadOrGhost()) {
            CGGameUI::DisplayError(ERR_PLAYER_DEAD);
            lua_pushnil(L);
            return 1;
        }

        if (player->m_movement->m_flags & 0x800) {
            CGGameUI::DisplayError(ERR_NOT_WHILE_FALLING);
            lua_pushnil(L);
            return 1;
        }

        if (IsOnTransport(player)) {
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            lua_pushnil(L);
            return 1;
        }

        MirrorTimerInfo* timer = CGGameUI::MirrorTimerGetInfo(0);

        if (timer && timer->m_scale < 0) {
            CGGameUI::DisplayError(ERR_NOT_WHILE_FATIGUED);
            lua_pushnil(L);
            return 1;
        }
    }

    if (status->m_queueID) {
        UserClientBattlefieldPort msg;
        msg.m_ticket = status->m_ticket;
        msg.m_accept = accept;
        ClientServices::Send(&msg);
    }

    if (!accept) {
        CGLCD::HideBattlefieldAlert();
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x009424B0/0x1406AEDB0
static int32_t Script_GetBattlefieldStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldStatus(id)");
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    BattlefieldStatusNode* status = NULL;

    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == id) {
            status = node;
            break;
        }
    }

    if (!status) {
        lua_pushstring(L, "none");
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 9;
    }

    switch (status->m_status) {
        case 0:
            lua_pushstring(L, "none");
            break;

        case 1:
            lua_pushstring(L, "queued");
            break;

        case 2:
            lua_pushstring(L, "confirm");
            break;

        case 4:
            lua_pushstring(L, "active");
            break;

        default:
            lua_pushstring(L, "error");
            break;
    }

    MapRec* map = NULL;
    bool useMap = false;

    if (status->m_status == 2) {
        uint32_t queue = static_cast<uint32_t>(status->m_queueID);

        if (!(queue & 0x100000) && ((queue >> 16) & 0xF) != 1 && static_cast<uint16_t>(queue) != 32) {
            map = g_mapDB.GetRecord(status->m_mapID);
            useMap = true;
        }
    } else if (status->m_status == 4) {
        map = g_mapDB.GetRecord(CGBattlefieldInfo::m_activeMapID);
        useMap = true;
    }

    if (useMap && map) {
        lua_pushstring(L, map->m_mapName);
    } else {
        BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(static_cast<uint16_t>(status->m_queueID));

        if (rec) {
            lua_pushstring(L, rec->m_name);
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushnumber(L, status->m_instanceID);
    lua_pushnumber(L, status->m_minLevel);
    lua_pushnumber(L, status->m_maxLevel);
    lua_pushnumber(L, status->m_teamSize);

    if (status->m_registeredMatch) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (status->m_eligibleInQueue) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (status->m_waitingOnOtherActivity) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 9;
}

// OFFSET: 0x009427A0/0x1406AF0A0
static int32_t Script_GetBattlefieldPortExpiration(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldPortExpiration(id)");
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    double remaining = 0.0;

    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id != id) {
            continue;
        }

        if (node->m_portExpiration) {
            uint32_t now = FrameTime::GetCurTimeMs();

            if (static_cast<int32_t>(now - node->m_portExpiration) < 0) {
                remaining = (node->m_portExpiration - now) / 1000;
            }
        }

        break;
    }

    lua_pushnumber(L, remaining);
    return 1;
}

// OFFSET: 0x00942870/0x1406AF170
static int32_t Script_GetBattlefieldEstimatedWaitTime(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldEstimatedWaitTime(id)");
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    double wait = 0.0;

    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == id) {
            wait = node->m_estimatedWaitTime;
            break;
        }
    }

    lua_pushnumber(L, wait);
    return 1;
}

// OFFSET: 0x00942920/0x1406AF220
static int32_t Script_GetBattlefieldTimeWaited(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldTimeWaited(id)");
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));
    double waited = 0.0;

    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id != id) {
            continue;
        }

        uint32_t start = node->m_queueStartTime;

        if (start) {
            waited = FrameTime::GetCurTimeMs() - start;
        }

        break;
    }

    lua_pushnumber(L, waited);
    return 1;
}

// OFFSET: 0x009429E0/0x1406AF2E0
int32_t Script_RequestBattlegroundInstanceInfo(lua_State* L) {
    // STATIC: 0x00F36DDC
    static ThrottleObject s_throttle(5, 2);

    if (s_throttle.IncrementAndCheck()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: RequestBattlegroundInstanceInfo(index)");
            return 0;
        }

        CGBattlefieldInfo::RequestBattlegroundInstanceInfo(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    }

    return 0;
}

// OFFSET: 0x00942A70/0x1406AF370
static int32_t Script_UpdateWarGamesList(lua_State* L) {
    CGBattlefieldInfo::BuildWarGameTypeList();
    CGBattlefieldInfo::UpdateWarGameTypeList();
    return 0;
}

// OFFSET: 0x00943110/0x1406AF910
static int32_t Script_IsActiveBattlefieldArena(lua_State* L) {
    if (CGBattlefieldInfo::m_instanceType == 4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == CGBattlefieldInfo::m_activeBattlefieldID) {
            if (node->m_registeredMatch) {
                lua_pushnumber(L, 1.0);
                return 2;
            }

            break;
        }
    }

    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x009431A0/0x1406AF9A0
static int32_t Script_LeaveBattlefield(lua_State* L) {
    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == CGBattlefieldInfo::m_activeBattlefieldID) {
            UserClientBattlefieldLeave msg;
            ClientServices::Send(&msg);
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x00943200/0x1406AFA00
static int32_t Script_RequestBattlefieldPositions(lua_State* L) {
    CGBattlefieldInfo::RequestPlayerPositions();
    return 0;
}

// OFFSET: 0x00943210/0x1406AFD10
static int32_t Script_GetWarGameQueueStatus(lua_State* L) {
    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == CGBattlefieldInfo::m_activeBattlefieldID) {
            lua_pushinteger(L, node->m_waitTeamSize[0]);
            lua_pushinteger(L, node->m_waitTeamReady[0]);
            lua_pushinteger(L, node->m_waitTeamSize[1]);
            lua_pushinteger(L, node->m_waitTeamReady[1]);
            return 4;
        }
    }

    return 0;
}

// OFFSET: 0x009432C0/0x1406AFE10
static int32_t Script_IsRatedBattleground(lua_State* L) {
    uint64_t queueID = CGBattlefieldInfo::GetActiveQueueID();
    lua_pushboolean(L, queueID && !(queueID & 0xF0000) && (queueID & 0x100000));
    return 1;
}

// OFFSET: 0x00943310/0x1406AFE90
static int32_t Script_IsRatedMap(lua_State* L) {
    uint64_t queueID = CGBattlefieldInfo::GetActiveQueueID();
    bool rated = queueID && (queueID & 0x100000);
    lua_pushboolean(L, rated);
    return 1;
}

// STATIC: 0x00D12238
static FrameScript_Method s_scriptFunctions[74] = {
    { "IsActiveBattlefieldArena", &Script_IsActiveBattlefieldArena },
    { "JoinBattlefield", &Script_JoinBattlefield },
    { "JoinRatedBattlefield", &Script_JoinRatedBattlefield },
    { "JoinArena", &Script_JoinArena },
    { "AcceptBattlefieldPort", &Script_AcceptBattlefieldPort },
    { "GetBattlefieldStatus", &Script_GetBattlefieldStatus },
    { "GetBattlefieldPortExpiration", &Script_GetBattlefieldPortExpiration },
    { "GetBattlefieldInstanceExpiration", &Script_GetBattlefieldInstanceExpiration },
    { "GetBattlefieldInstanceRunTime", &Script_GetBattlefieldInstanceRunTime },
    { "GetBattlefieldEstimatedWaitTime", &Script_GetBattlefieldEstimatedWaitTime },
    { "GetBattlefieldTimeWaited", &Script_GetBattlefieldTimeWaited },
    { "RequestBattlefieldScoreData", &Script_RequestBattlefieldScoreData },
    { "GetNumBattlefieldScores", &Script_GetNumBattlefieldScores },
    { "GetBattlefieldScore", &Script_GetBattlefieldScore },
    { "GetBattlefieldWinner", &Script_GetBattlefieldWinner },
    { "SetBattlefieldScoreFaction", &Script_SetBattlefieldScoreFaction },
    { "LeaveBattlefield", &Script_LeaveBattlefield },
    { "GetNumBattlefieldStats", &Script_GetNumBattlefieldStats },
    { "GetBattlefieldStatInfo", &Script_GetBattlefieldStatInfo },
    { "GetBattlefieldStatData", &Script_GetBattlefieldStatData },
    { "RequestBattlefieldPositions", &Script_RequestBattlefieldPositions },
    { "GetNumBattlefieldPositions", &Script_GetNumBattlefieldPositions },
    { "GetBattlefieldPosition", &Script_GetBattlefieldPosition },
    { "GetNumBattlefieldFlagPositions", &Script_GetNumBattlefieldFlagPositions },
    { "GetBattlefieldFlagPosition", &Script_GetBattlefieldFlagPosition },
    { "GetNumBattlefieldVehicles", &Script_GetNumBattlefieldVehicles },
    { "GetBattlefieldVehicleInfo", &Script_GetBattlefieldVehicleInfo },
    { "CanJoinBattlefieldAsGroup", &Script_CanJoinBattlefieldAsGroup },
    { "GetBattlefieldMapIconScale", &Script_GetBattlefieldMapIconScale },
    { "GetBattlefieldTeamInfo", &Script_GetBattlefieldTeamInfo },
    { "GetBattlefieldArenaFaction", &Script_GetBattlefieldArenaFaction },
    { "SortBattlefieldScoreData", &Script_SortBattlefieldScoreData },
    { "HearthAndResurrectFromArea", &Script_HearthAndResurrectFromArea },
    { "CanHearthAndResurrectFromArea", &Script_CanHearthAndResurrectFromArea },
    { "GetNumBattlegroundTypes", &Script_GetNumBattlegroundTypes },
    { "GetBattlegroundInfo", &Script_GetBattlegroundInfo },
    { "RequestBattlegroundInstanceInfo", &Script_RequestBattlegroundInstanceInfo },
    { "GetNumArenaOpponents", &Script_GetNumArenaOpponents },
    { "BattlefieldMgrEntryInviteResponse", &Script_BattlefieldMgrEntryInviteResponse },
    { "BattlefieldMgrQueueRequest", &Script_BattlefieldMgrQueueRequest },
    { "BattlefieldMgrQueueInviteResponse", &Script_BattlefieldMgrQueueInviteResponse },
    { "BattlefieldMgrExitRequest", &Script_BattlefieldMgrExitRequest },
    { "GetWorldPVPQueueStatus", &Script_GetWorldPVPQueueStatus },
    { "GetHolidayBGHonorCurrencyBonuses", &Script_GetHolidayBGHonorCurrencyBonuses },
    { "GetRandomBGHonorCurrencyBonuses", &Script_GetRandomBGHonorCurrencyBonuses },
    { "SortBGList", &Script_SortBGList },
    { "StartWarGame", &Script_StartWarGame },
    { "CanInitiateWarGame", &Script_CanInitiateWarGame },
    { "StartUnratedArena", &Script_StartUnratedArena },
    { "WarGameRespond", &Script_WarGameRespond },
    { "GetWarGameQueueStatus", &Script_GetWarGameQueueStatus },
    { "RequestRatedBattlegroundInfo", &Script_RequestRatedBattlegroundInfo },
    { "RequestRatedArenaInfo", &Script_RequestRatedArenaInfo },
    { "GetRatedBattleGroundInfo", &Script_GetRatedBattleGroundInfo },
    { "GetNumWorldPVPAreas", &Script_GetNumWorldPVPAreas },
    { "GetWorldPVPAreaInfo", &Script_GetWorldPVPAreaInfo },
    { "GetPersonalRatedBGInfo", &Script_GetPersonalRatedBGInfo },
    { "GetPersonalRatedArenaInfo", &Script_GetPersonalRatedArenaInfo },
    { "IsRatedBattleground", &Script_IsRatedBattleground },
    { "IsRatedMap", &Script_IsRatedMap },
    { "RequestPVPOptionsEnabled", &Script_RequestPVPOptionsEnabled },
    { "CommentatorStartWargame", &Script_CommentatorStartWargame },
    { "IsInActiveWorldPVP", &Script_IsInActiveWorldPVP },
    { "GetNumWarGameTypes", &Script_GetNumWarGameTypes },
    { "GetWarGameTypeInfo", &Script_GetWarGameTypeInfo },
    { "CollapseWarGameHeader", &Script_CollapseWarGameHeader },
    { "ExpandWarGameHeader", &Script_ExpandWarGameHeader },
    { "UpdateWarGamesList", &Script_UpdateWarGamesList },
    { "SetSelectedWarGameType", &Script_SetSelectedWarGameType },
    { "GetSelectedWarGameType", &Script_GetSelectedWarGameType },
    { "RequestPVPRewards", &Script_RequestPVPRewards },
    { "GetPVPRewards", &Script_GetPVPRewards },
    { "GetMaxBattlefieldID", &Script_GetMaxBattlefieldID },
    { "ClearBattlemaster", &Script_ClearBattlemaster }
};

// OFFSET: 0x0093DD40
void BattlefieldInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < 74; i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0093DD70
void BattlefieldInfoUnregisterScriptFunctions() {
    for (int32_t i = 0; i < 74; i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
