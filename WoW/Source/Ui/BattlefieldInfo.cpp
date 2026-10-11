#include "BattlefieldInfo.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/PvpDifficultyRec.h"
#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"
#include "WoW/Source/DB/Rec/WorldStateUIRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F36068
int32_t CGBattlefieldInfo::m_ratedBgStats[4];

// STATIC: 0x00F36078
PersonalRatedInfo CGBattlefieldInfo::m_personalRatedInfo[4];

// STATIC: 0x00F360F0
WOWGUID CGBattlefieldInfo::m_arenaOpponentPets[5][2];

// STATIC: 0x00F36140
WOWGUID CGBattlefieldInfo::m_arenaOpponents[5];

// STATIC: 0x00F36168
WOWGUID CGBattlefieldInfo::m_vehicleGuids[40];

// STATIC: 0x00F362A8
int32_t CGBattlefieldInfo::m_teamPlayerCounts[2];

// STATIC: 0x00F362B0
char CGBattlefieldInfo::m_teamNames[2][96];

// STATIC: 0x00F36370
WOWGUID CGBattlefieldInfo::m_flagCarriers[2];

// STATIC: 0x00F36380
WOWGUID CGBattlefieldInfo::m_playerPositionGuids[40];

// STATIC: 0x00F364C0
BattlefieldScoreSortCriteria CGBattlefieldInfo::m_scoreSortCriteria[22];

// STATIC: 0x00F36570
int32_t CGBattlefieldInfo::m_worldStateStats[8];

// STATIC: 0x00F36590
BattlefieldScore* CGBattlefieldInfo::m_scores[80];

// STATIC: 0x00F366D0
WorldPvPQueueStatus CGBattlefieldInfo::m_worldPvPQueues[2];

// STATIC: 0x00F36700
uint32_t CGBattlefieldInfo::m_listUnk;

// STATIC: 0x00F36704
int32_t CGBattlefieldInfo::m_bgTypeID;

// STATIC: 0x00F36708
uint32_t CGBattlefieldInfo::m_minLevel;

// STATIC: 0x00F3670C
uint32_t CGBattlefieldInfo::m_maxLevel;

// STATIC: 0x00F36710
uint8_t CGBattlefieldInfo::m_hasHolidayWinToday;

// STATIC: 0x00F36711
uint8_t CGBattlefieldInfo::m_hasRandomWinToday;

// STATIC: 0x00F36712
uint8_t CGBattlefieldInfo::m_arenaFaction;

// STATIC: 0x00F36714
int32_t CGBattlefieldInfo::m_holidayLossHonor;

// STATIC: 0x00F36718
int32_t CGBattlefieldInfo::m_holidayWinHonor;

// STATIC: 0x00F3671C
int32_t CGBattlefieldInfo::m_holidayWinArena;

// STATIC: 0x00F36720
int32_t CGBattlefieldInfo::m_randomLossHonor;

// STATIC: 0x00F36724
int32_t CGBattlefieldInfo::m_randomWinHonor;

// STATIC: 0x00F36728
int32_t CGBattlefieldInfo::m_randomWinArena;

// STATIC: 0x00F3672C
uint32_t CGBattlefieldInfo::m_collapsedWarGameHeaders;

// STATIC: 0x00F36730
int32_t CGBattlefieldInfo::m_numWarGameTypes;

// STATIC: 0x00F36734
uint32_t CGBattlefieldInfo::m_maxBattlefieldID;

// STATIC: 0x00F36738
WOWGUID CGBattlefieldInfo::m_battlemaster;

// STATIC: 0x00F36740
uint32_t CGBattlefieldInfo::m_activeBattlefieldID;

// STATIC: 0x00F36744
uint32_t CGBattlefieldInfo::m_activeMapID;

// STATIC: 0x00F36748
int32_t CGBattlefieldInfo::m_instanceType;

// STATIC: 0x00F3674C
uint32_t CGBattlefieldInfo::m_instanceExpiration;

// STATIC: 0x00F36750
uint32_t CGBattlefieldInfo::m_instanceStartTime;

// STATIC: 0x00F36754
uint32_t CGBattlefieldInfo::m_numScoreEntries;

// STATIC: 0x00F36758
uint32_t CGBattlefieldInfo::m_numScores;

// STATIC: 0x00F3675C
uint32_t CGBattlefieldInfo::m_nextLogUpdate;

// STATIC: 0x00F36760
int32_t CGBattlefieldInfo::m_mapDone;

// STATIC: 0x00F36764
uint32_t CGBattlefieldInfo::m_winner;

// STATIC: 0x00F36768
int32_t CGBattlefieldInfo::m_nameCachePending;

// STATIC: 0x00F3676C
uint32_t CGBattlefieldInfo::m_teamRatings[2];

// STATIC: 0x00F36774
uint32_t CGBattlefieldInfo::m_newTeamRatings[2];

// STATIC: 0x00F3677C
uint32_t CGBattlefieldInfo::m_avgMatchmakingRating[2];

// STATIC: 0x00F36784
uint32_t CGBattlefieldInfo::m_numWorldStateStats;

// STATIC: 0x00F36788
uint32_t CGBattlefieldInfo::m_numPlayerPositions;

// STATIC: 0x00F3678C
uint32_t CGBattlefieldInfo::m_positionUpdateTime;

// STATIC: 0x00F36790
uint32_t CGBattlefieldInfo::m_numVehicles;

// STATIC: 0x00F36794
int32_t CGBattlefieldInfo::m_pvpRewardTotal;

// STATIC: 0x00F36798
WOWGUID CGBattlefieldInfo::m_warGameRequester;

// STATIC: 0x00F367A0
uint64_t CGBattlefieldInfo::m_warGameQueueID;

// STATIC: 0x00F367A8
int32_t CGBattlefieldInfo::m_pvpRewards[5];

// STATIC: 0x00F367BC
TSGrowableArray<int32_t> CGBattlefieldInfo::m_instances;

// STATIC: 0x00F367CC
TSGrowableArray<WarGameType*> CGBattlefieldInfo::m_warGameTypes;

// STATIC: 0x00F367E0
C2Vector CGBattlefieldInfo::m_playerPositions[40];

// STATIC: 0x00F36920
C2Vector CGBattlefieldInfo::m_flagPositions[2];

// STATIC: 0x00F36930
PartyMemberState CGBattlefieldInfo::m_arenaOpponentStats[5];

// STATIC: 0x00F36D68
TSGrowableArray<int32_t> CGBattlefieldInfo::m_battlegroundTypes;

// STATIC: 0x00D12228
int32_t CGBattlefieldInfo::m_selectedWarGameType = -1;

// STATIC: 0x00D1222C
int32_t CGBattlefieldInfo::m_logFilterFaction = -1;

// STATIC: 0x00D12230
int32_t CGBattlefieldInfo::m_allowJoin = 1;

// STATIC: 0x00D12488
TSList<BattlefieldStatusNode, TSGetLink<BattlefieldStatusNode> > CGBattlefieldInfo::m_statusList;

// STATIC: 0x00D124B0
static const char* s_flagTokens[2] = { "AllianceFlag", "HordeFlag" };

// STATIC: #0x00BEA4A8
int32_t CGBattlefieldInfo::m_arenaOpponentSeen[5];

// OFFSET: 0x0093D020
WorldPvPQueueStatus* CGBattlefieldInfo::FindWorldPvPQueue(uint64_t queueID) {
    for (uint32_t i = 0; i < 2; i++) {
        if (CGBattlefieldInfo::m_worldPvPQueues[i].m_queueID == queueID) {
            return &CGBattlefieldInfo::m_worldPvPQueues[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0093D070
WorldPvPQueueStatus* CGBattlefieldInfo::FindFreeWorldPvPQueue() {
    for (uint32_t i = 0; i < 2; i++) {
        if (!CGBattlefieldInfo::m_worldPvPQueues[i].m_areaID || !CGBattlefieldInfo::m_worldPvPQueues[i].m_status) {
            return &CGBattlefieldInfo::m_worldPvPQueues[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0093D0A0
uint32_t CGBattlefieldInfo::GetNumFlagCarriers() {
    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        return 2;
    }

    if (CGBattlefieldInfo::m_flagCarriers[0]) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x0093D0D0
WOWGUID CGBattlefieldInfo::GetFlagCarrier(uint32_t index) {
    uint32_t count;

    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        count = 2;
    } else {
        if (!CGBattlefieldInfo::m_flagCarriers[0]) {
            return WOWGUID::Make(0);
        }

        count = 1;
    }

    if (index < count) {
        return CGBattlefieldInfo::m_flagCarriers[index];
    }

    return WOWGUID::Make(0);
}

// OFFSET: 0x0093D120/0x1406A8BD0
void WorldMapNameUpdateCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_WORLD_MAP_NAME_UPDATE, NULL);
}

// OFFSET: 0x0093D130/0x1406A8BE0
void CGBattlefieldInfo::SetBattlemaster(WOWGUID battlemaster) {
    if (battlemaster) {
        CGGossipInfo::ClearGossip();
        CGGameUI::SetInteractTarget(battlemaster);
        CGBattlefieldInfo::m_battlemaster = battlemaster;
        FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELDS_SHOW, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELDS_CLOSED, NULL);
        CGGameUI::ClearInteractTarget(CGBattlefieldInfo::m_battlemaster, 0);
        CGBattlefieldInfo::m_battlemaster.m_guid = 0;
    }
}

// OFFSET: 0x0093D1A0/0x1406A8C50
void CGBattlefieldInfo::RequestScoreData() {
    if (CGBattlefieldInfo::m_nextLogUpdate && (CGBattlefieldInfo::m_mapDone || CGBattlefieldInfo::m_instanceType == 4)) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (!CGBattlefieldInfo::m_nextLogUpdate || static_cast<int32_t>(now - CGBattlefieldInfo::m_nextLogUpdate) >= 0) {
        UserClientRequestPvpLogData msg;
        ClientServices::Send(&msg);
        CGBattlefieldInfo::m_nextLogUpdate = now + 5000;
    }
}

// OFFSET: 0x0093D210
BattlefieldScore* CGBattlefieldInfo::FindScore(const WOWGUID& guid) {
    for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
        if (CGBattlefieldInfo::m_scores[i]->m_guid == guid) {
            return CGBattlefieldInfo::m_scores[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0093D260/0x1406A8CC0
void CGBattlefieldInfo::RemoveVehicle(const WOWGUID& guid) {
    uint32_t index;

    for (index = 0; CGBattlefieldInfo::m_vehicleGuids[index] != guid; index++) {
        if (index + 1 >= 40) {
            return;
        }
    }

    CGBattlefieldInfo::m_numVehicles--;

    if (index != 39) {
        memcpy(&CGBattlefieldInfo::m_vehicleGuids[index], &CGBattlefieldInfo::m_vehicleGuids[index + 1], sizeof(WOWGUID) * (39 - index));
    }

    CGBattlefieldInfo::m_vehicleGuids[39].m_guid = 0;
}

// OFFSET: 0x0093D2E0/0x1406A8D30
PartyMemberState* CGBattlefieldInfo::GetRemoteStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 5; i++) {
            if (CGBattlefieldInfo::m_arenaOpponents[i] == guid) {
                return &CGBattlefieldInfo::m_arenaOpponentStats[i];
            }
        }
    }

    return NULL;
}

// OFFSET: 0x0093D330/0x1406A8D70
PartyMemberState* CGBattlefieldInfo::GetRemoteStatsFromPet(const WOWGUID& petGuid) {
    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] && CGBattlefieldInfo::m_arenaOpponentStats[i].m_pet.m_guid == petGuid) {
            return &CGBattlefieldInfo::m_arenaOpponentStats[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0093D380/0x1406A8DC0
PartyMemberPetState* CGBattlefieldInfo::GetRemotePetStats(const WOWGUID& guid) {
    if (guid) {
        for (uint32_t i = 0; i < 5; i++) {
            if (CGBattlefieldInfo::m_arenaOpponents[i] && CGBattlefieldInfo::m_arenaOpponentStats[i].m_pet.m_guid == guid) {
                return &CGBattlefieldInfo::m_arenaOpponentStats[i].m_pet;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x0093D3E0/0x1406A8E10
int32_t CGBattlefieldInfo::PushPVPRewards(lua_State* L) {
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewardTotal);
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewards[0]);
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewards[1]);
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewards[2]);
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewards[3]);
    lua_pushinteger(L, CGBattlefieldInfo::m_pvpRewards[4]);
    return 6;
}

// OFFSET: 0x0093DC90
bool OnWargameRequestSent(ClientWargameRequestSent* msg) {
    CGGameUI::DisplayError(ERR_WARGAME_REQUEST_SENT);
    return true;
}

// OFFSET: 0x0093DE30
int32_t CGBattlefieldInfo::GetPlayerPosition(uint32_t index, C3Vector* position) {
    if (index >= CGBattlefieldInfo::m_numPlayerPositions) {
        return 0;
    }

    position->x = CGBattlefieldInfo::m_playerPositions[index].x;
    position->y = CGBattlefieldInfo::m_playerPositions[index].y;
    position->z = 0.0f;
    return 1;
}

// OFFSET: 0x0093DE90
int32_t CGBattlefieldInfo::GetFlagPosition(uint32_t index, C3Vector* position) {
    uint32_t count;

    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        count = 2;
    } else {
        if (!CGBattlefieldInfo::m_flagCarriers[0]) {
            return 0;
        }

        count = 1;
    }

    if (index < count) {
        position->x = CGBattlefieldInfo::m_flagPositions[index].x;
        position->y = CGBattlefieldInfo::m_flagPositions[index].y;
        position->z = 0.0f;
        return 1;
    }

    return 0;
}

// OFFSET: 0x0093DF10
int32_t CGBattlefieldInfo::GetBattlegroundType(int32_t index) {
    if (index < static_cast<int32_t>(CGBattlefieldInfo::m_battlegroundTypes.Count())) {
        return CGBattlefieldInfo::m_battlegroundTypes[index];
    }

    return 0;
}

// OFFSET: 0x0093DF30
void BattlegroundPlayerJoinedNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        char fullName[308];
        name->FullName(fullName, 305);
        CGGameUI::DisplayError(ERR_BG_PLAYER_JOINED_SS, fullName, fullName);
    }
}

// OFFSET: 0x0093DFA0
void BattlegroundPlayerLeftNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        char fullName[308];
        name->FullName(fullName, 305);
        CGGameUI::DisplayError(ERR_BG_PLAYER_LEFT_S, fullName);
    }
}

// OFFSET: 0x0093E010/0x1406A99F0
int32_t QSortBattlefields(const void* a, const void* b) {
    int32_t idA = *static_cast<const int32_t*>(a);
    int32_t idB = *static_cast<const int32_t*>(b);

    if (idA == 32) {
        return -1;
    }

    if (idB == 32) {
        return 1;
    }

    BattlemasterListRec* recA = g_battlemasterListDB.GetRecord(idA);
    BattlemasterListRec* recB = g_battlemasterListDB.GetRecord(idB);

    if (GetWorldState(recA->m_holidayWorldState)) {
        return -1;
    }

    if (GetWorldState(recB->m_holidayWorldState)) {
        return 1;
    }

    if (recA->m_minLevel != recB->m_minLevel) {
        return recA->m_minLevel - recB->m_minLevel;
    }

    return idA - idB;
}

// OFFSET: 0x0093E0C0/0x1406A9AC0
void CGBattlefieldInfo::JoinBattlefield(int32_t asGroup) {
    if (!CGBattlefieldInfo::m_allowJoin) {
        return;
    }

    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_bgTypeID);

    if (rec && rec->m_instanceType == 4) {
        return;
    }

    if (asGroup && rec && (rec->m_maxGroupSize < CGPartyInfo::NumMembers() || rec->m_maxGroupSize < static_cast<int32_t>(CGRaidInfo::m_numMembers))) {
        CGGameUI::DisplayError(ERR_GROUP_JOIN_BATTLEGROUND_TOO_MANY);
        return;
    }

    if (IsCurrentMapArena()) {
        CGGameUI::DisplayError(ERR_BATTLEGROUND_NOT_IN_BATTLEGROUND);
        return;
    }

    UserClientBattlemasterJoin msg;
    *reinterpret_cast<uint64_t*>(msg.unk_0018) = static_cast<uint16_t>(CGBattlefieldInfo::m_bgTypeID) | 0x1F10000000000000ull;
    msg.unk_0014 = asGroup != 0;
    ClientServices::Send(&msg);
}

// OFFSET: 0x0093E1C0
bool IsCurrentMapArena() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    if (map) {
        return map->m_instanceType == 3;
    }

    return false;
}

static int32_t GetScoreSortFaction(const NameCache* name) {
    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(g_chrRacesDB.GetRecord(name->m_race)->m_factionID);

    if (faction->m_factionGroup & 0x4) {
        return 0;
    }

    return (g_factionTemplateDB.GetRecord(g_chrRacesDB.GetRecord(name->m_race)->m_factionID)->m_factionGroup & 0x2) ? 1 : -1;
}

// OFFSET: 0x0093E200
int32_t QSortScoreData(const void* a, const void* b) {
    const BattlefieldScore* scoreA = *static_cast<BattlefieldScore* const*>(a);
    const BattlefieldScore* scoreB = *static_cast<BattlefieldScore* const*>(b);
    WOWGUID none = WOWGUID();
    NameCache* nameA = g_nameDBCache.GetRecord(scoreA->m_guid.m_guid, none, NULL, NULL, false);
    WOWGUID none2 = WOWGUID();
    NameCache* nameB = g_nameDBCache.GetRecord(scoreB->m_guid.m_guid, none2, NULL, NULL, false);

    if (!nameA || !nameB) {
        return 0;
    }

    if (CGBattlefieldInfo::m_logFilterFaction != -1) {
        int32_t factionA = GetScoreSortFaction(nameA);
        int32_t factionB = GetScoreSortFaction(nameB);

        if (factionA != factionB) {
            return 2 * (factionA != CGBattlefieldInfo::m_logFilterFaction) - 1;
        }
    }

    for (uint32_t i = 0; i < 22; i++) {
        const BattlefieldScoreSortCriteria& criteria = CGBattlefieldInfo::m_scoreSortCriteria[i];
        int32_t result;

        switch (criteria.m_column) {
            case 0:
                if (scoreB->m_killingBlows == scoreA->m_killingBlows) {
                    continue;
                }

                result = scoreB->m_killingBlows < scoreA->m_killingBlows ? -1 : 1;
                break;

            case 1:
                if (scoreB->m_deaths == scoreA->m_deaths) {
                    continue;
                }

                result = scoreB->m_deaths < scoreA->m_deaths ? -1 : 1;
                break;

            case 2:
                if (scoreB->m_bonusHonor == scoreA->m_bonusHonor) {
                    continue;
                }

                result = scoreB->m_bonusHonor < scoreA->m_bonusHonor ? -1 : 1;
                break;

            case 3:
                result = strcmp(nameA->m_name, nameB->m_name);
                break;

            case 4:
                result = nameB->m_class < nameA->m_class ? -1 : 1;
                break;

            case 5:
                if (scoreB->m_honorableKills == scoreA->m_honorableKills) {
                    continue;
                }

                result = scoreB->m_honorableKills < scoreA->m_honorableKills ? -1 : 1;
                break;

            case 6:
                if (scoreB->m_damageDone == scoreA->m_damageDone) {
                    continue;
                }

                result = scoreB->m_damageDone < scoreA->m_damageDone ? -1 : 1;
                break;

            case 7:
                if (scoreB->m_healingDone == scoreA->m_healingDone) {
                    continue;
                }

                result = scoreB->m_healingDone < scoreA->m_healingDone ? -1 : 1;
                break;

            case 8:
                if (scoreA->m_faction == scoreB->m_faction) {
                    continue;
                }

                result = 2 * (scoreA->m_faction >= scoreB->m_faction) - 1;
                break;

            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16: {
                uint32_t stat = criteria.m_column - 9;

                if (stat >= CGBattlefieldInfo::m_numWorldStateStats) {
                    continue;
                }

                result = 2 * (scoreA->m_stats[stat] <= scoreB->m_stats[stat]) - 1;
                break;
            }

            case 17:
                if (scoreA->m_rating == scoreB->m_rating) {
                    continue;
                }

                result = scoreA->m_rating < scoreB->m_rating ? -1 : 1;
                break;

            case 18:
                if (scoreA->m_ratingChange == scoreB->m_ratingChange) {
                    continue;
                }

                result = 2 * (scoreA->m_ratingChange >= scoreB->m_ratingChange) - 1;
                break;

            case 19:
                if (scoreA->m_preMatchMMR == scoreB->m_preMatchMMR) {
                    continue;
                }

                result = scoreA->m_preMatchMMR < scoreB->m_preMatchMMR ? -1 : 1;
                break;

            case 20:
                result = scoreA->m_preMatchMMR + scoreA->m_mmrChange - scoreB->m_mmrChange - scoreB->m_preMatchMMR;
                break;

            case 21:
                if (scoreA->m_mmrChange == scoreB->m_mmrChange) {
                    continue;
                }

                result = 2 * (scoreA->m_mmrChange >= scoreB->m_mmrChange) - 1;
                break;

            default:
                continue;
        }

        if (!result) {
            continue;
        }

        if (criteria.m_descending) {
            return -result;
        }

        return result;
    }

    return 0;
}

// OFFSET: 0x0093E5D0/0x1406AA0D0
void BattlefieldNameCacheCallback() {
    if (CGBattlefieldInfo::m_nameCachePending) {
        return;
    }

    CGBattlefieldInfo::m_numScores = 0;

    for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
        if (CGBattlefieldInfo::m_logFilterFaction == -1 || CGBattlefieldInfo::m_scores[i]->m_faction == CGBattlefieldInfo::m_logFilterFaction) {
            CGBattlefieldInfo::m_numScores++;
        }
    }

    qsort(CGBattlefieldInfo::m_scores, CGBattlefieldInfo::m_numScoreEntries, sizeof(BattlefieldScore*), QSortScoreData);
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_SCORE, NULL);
}

// OFFSET: 0x0093E640/0x1406AA150
void CGBattlefieldInfo::AddVehicle(const WOWGUID& guid) {
    if (guid && CGBattlefieldInfo::m_numVehicles < 40) {
        CGBattlefieldInfo::RemoveVehicle(guid);
        CGBattlefieldInfo::m_vehicleGuids[CGBattlefieldInfo::m_numVehicles] = guid;
        CGBattlefieldInfo::m_numVehicles++;
    }
}

// OFFSET: 0x0093E690/0x1406AA1A0
void CGBattlefieldInfo::OnArenaUnitOutOfRange(const WOWGUID& guid) {
    char token[32];

    for (uint32_t i = 0; i < 5; i++) {
        WOWGUID opponent = CGBattlefieldInfo::m_arenaOpponents[i];

        if (opponent == guid) {
            SStrPrintf(token, sizeof(token), "arena%d", i + 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "unseen");
            return;
        }

        if (opponent) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(opponent, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

                if (pet == guid) {
                    SStrPrintf(token, sizeof(token), "arenapet%d", i + 1);
                    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "unseen");
                    return;
                }
            }
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponentPets[i][0] == guid) {
            SStrPrintf(token, sizeof(token), "arenapet%d", i + 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "unseen");
            return;
        }
    }
}

// OFFSET: 0x0093E790/0x1406AA2A0
void CGBattlefieldInfo::OnArenaUnitDestroyed(const WOWGUID& guid) {
    char token[32];

    for (uint32_t i = 0; i < 5; i++) {
        WOWGUID opponent = CGBattlefieldInfo::m_arenaOpponents[i];

        if (opponent == guid) {
            SStrPrintf(token, sizeof(token), "arena%d", i + 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "destroyed");
            return;
        }

        if (opponent) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(opponent, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

                if (pet == guid) {
                    SStrPrintf(token, sizeof(token), "arenapet%d", i + 1);
                    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "destroyed");
                    return;
                }
            }
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponentPets[i][0] == guid) {
            CGBattlefieldInfo::m_arenaOpponentPets[i][0].m_guid = 0;
            CGBattlefieldInfo::m_arenaOpponentPets[i][1].m_guid = 0;
            SStrPrintf(token, sizeof(token), "arenapet%d", i + 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "destroyed");
            return;
        }
    }
}

// OFFSET: 0x0093E8B0/0x1406AA3C0
WOWGUID CGBattlefieldInfo::GetArenaOpponentPet(uint32_t index) {
    if (index >= 5) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[index], TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        return unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    }

    return CGBattlefieldInfo::m_arenaOpponentPets[index][0];
}

// OFFSET: 0x0093E920
bool OnWargameCheckEntry(ClientWargameCheckEntry* msg) {
    WOWGUID requester = *reinterpret_cast<const WOWGUID*>(&msg->unk_0018);
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(requester, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (player) {
        uint64_t queueID = *reinterpret_cast<const uint64_t*>(&msg->unk_0010);
        CGBattlefieldInfo::m_warGameRequester = requester;
        CGBattlefieldInfo::m_warGameQueueID = queueID;
        BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(static_cast<uint16_t>(queueID));

        if (rec && rec->m_name) {
            FrameScript_SignalEvent(SCRIPT_EVENT_WARGAME_REQUESTED, "%s%s%d", player->GetObjectName(), static_cast<const char*>(rec->m_name), msg->unk_0020);
        }
    }

    return true;
}

// OFFSET: 0x0093E9D0
bool OnBattlefieldPlayerPositions(ClientBattlefieldPlayerPositions* msg) {
    uint32_t count = msg->unk_0010.Count();
    CGBattlefieldInfo::m_numPlayerPositions = count;

    for (uint32_t i = 0; i < count; i++) {
        const JamBattlefieldPlayerPosition& position = msg->unk_0010[i];
        CGBattlefieldInfo::m_playerPositionGuids[i] = *reinterpret_cast<const WOWGUID*>(position.unk_0000);
        CGBattlefieldInfo::m_playerPositions[i].x = position.unk_0008;
        CGBattlefieldInfo::m_playerPositions[i].y = position.unk_000C;
    }

    int32_t flags = msg->unk_0020.Count();

    if (flags <= 0) {
        CGBattlefieldInfo::m_flagCarriers[0] = WOWGUID();
        CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
        CGBattlefieldInfo::m_flagPositions[0].x = 0.0f;
        CGBattlefieldInfo::m_flagPositions[0].y = 0.0f;
        CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
        CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
        return true;
    }

    if (flags < 2) {
        CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
        CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
        CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
    }

    do {
        flags--;
        const JamBattlefieldPlayerPosition& position = msg->unk_0020[flags];
        CGBattlefieldInfo::m_flagCarriers[flags] = *reinterpret_cast<const WOWGUID*>(position.unk_0000);
        CGBattlefieldInfo::m_flagPositions[flags].x = position.unk_0008;
        CGBattlefieldInfo::m_flagPositions[flags].y = position.unk_000C;
    } while (flags > 0);

    return true;
}

// OFFSET: 0x0093EAF0
bool OnBattlegroundPlayerJoined(ClientBattlegroundPlayerJoined* msg) {
    const WOWGUID& guid = *reinterpret_cast<const WOWGUID*>(&msg->unk_0010);
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, BattlegroundPlayerJoinedNameCallback, NULL, false);

    if (name) {
        char fullName[308];
        name->FullName(fullName, 305);
        CGGameUI::DisplayError(ERR_BG_PLAYER_JOINED_SS, fullName, fullName);
    }

    return true;
}

// OFFSET: 0x0093EB50
bool OnBattlegroundPlayerLeft(ClientBattlegroundPlayerLeft* msg) {
    const WOWGUID& guid = *reinterpret_cast<const WOWGUID*>(&msg->unk_0010);
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, BattlegroundPlayerLeftNameCallback, NULL, false);

    if (name) {
        char fullName[308];
        name->FullName(fullName, 305);
        CGGameUI::DisplayError(ERR_BG_PLAYER_LEFT_S, fullName);
    }

    return true;
}

// OFFSET: 0x0093EBB0
int32_t Packet_SMSG_ARENA_OPPONENT_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    if (CGBattlefieldInfo::m_instanceType == 4) {
        CGBattlefieldInfo::OnArenaUnitDestroyed(guid);
    }

    return 1;
}

// OFFSET: 0x0093EBE0
bool OnBattlefieldMgrEntered(ClientBattlefieldMgrEntered* msg) {
    if (msg->unk_0019) {
        const char* text = FrameScript_GetText("CLEARED_AFK", -1, GENDER_NOT_APPLICABLE);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
    }

    uint64_t queueID = *reinterpret_cast<const uint64_t*>(msg->unk_0010);
    WorldPvPQueueStatus* queue = CGBattlefieldInfo::FindWorldPvPQueue(queueID);

    if (queue || (queue = CGBattlefieldInfo::FindFreeWorldPvPQueue())) {
        queue->m_queueID = queueID;
        queue->m_status = 0;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_ENTERED, "%d%b%b", static_cast<uint16_t>(queueID), msg->unk_001A, msg->unk_0018);
    return true;
}

// OFFSET: 0x0093EC70
bool OnPvpOptionsEnabled(ClientPvpOptionsEnabled* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PVP_TYPES_ENABLED, "%b%b%b", msg->unk_0011, msg->unk_0012, msg->unk_0014);
    return true;
}

// OFFSET: 0x0093ECA0
bool OnBattlefieldMgrStateChange(ClientBattlefieldMgrStateChange* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_STATE_CHANGE, "%d%d", static_cast<uint16_t>(*reinterpret_cast<const uint64_t*>(&msg->unk_0010)), msg->unk_0018);
    return true;
}

// OFFSET: 0x0093ECD0
void CGBattlefieldInfo::AddArenaOpponentPetToSlot(const WOWGUID& petGuid, const WOWGUID& ownerGuid, int32_t slot, int32_t registerNew) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(petGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (registerNew && CGBattlefieldInfo::m_arenaOpponentPets[slot][0] != petGuid && unit) {
        unit->RegisterScript(0x4000);
    }

    CGBattlefieldInfo::m_arenaOpponentPets[slot][0] = petGuid;
    CGBattlefieldInfo::m_arenaOpponentPets[slot][1] = ownerGuid;
    char token[32];
    SStrPrintf(token, sizeof(token), "arenapet%d", slot + 1);
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "seen");

    if (!unit) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "unseen");
    }
}

// OFFSET: 0x0093EDB0/0x1406AAA10
int32_t QSortWarGameTypes(const void* a, const void* b) {
    const WarGameType* typeA = *static_cast<WarGameType* const*>(a);
    const WarGameType* typeB = *static_cast<WarGameType* const*>(b);

    if (!typeA || !typeB) {
        return 0;
    }

    if (typeA->m_collapsed != typeB->m_collapsed) {
        return 2 * (typeB->m_collapsed == 0) - 1;
    }

    if (typeA->m_instanceType != typeB->m_instanceType) {
        return 2 * (typeA->m_instanceType >= typeB->m_instanceType) - 1;
    }

    int32_t idA = typeA->m_bgTypeID;

    if (idA == -1 || typeB->m_bgTypeID == -1) {
        return 2 * (idA != -1) - 1;
    }

    int32_t idB = typeB->m_bgTypeID;

    if (idA == 32 || idB == 32) {
        return 2 * (idA == 32) - 1;
    }

    if (idA == 6 || idB == 6) {
        return 2 * (idA == 6) - 1;
    }

    BattlemasterListRec* recA = g_battlemasterListDB.GetRecord(idA);
    BattlemasterListRec* recB = g_battlemasterListDB.GetRecord(idB);

    if (recA->m_minPlayers == recB->m_minPlayers) {
        return 2 * (recA->m_id >= recB->m_id) - 1;
    }

    return 2 * (recA->m_minPlayers >= recB->m_minPlayers) - 1;
}

// OFFSET: 0x0093EEA0/0x1406AAB80
bool CGBattlefieldInfo::IsWarGameHeaderCollapsed(uint32_t index) {
    if (index < CGBattlefieldInfo::m_warGameTypes.Count()) {
        return (CGBattlefieldInfo::m_collapsedWarGameHeaders & (1 << CGBattlefieldInfo::m_warGameTypes[index]->m_instanceType)) != 0;
    }

    return false;
}

// OFFSET: 0x0093EEE0/0x1406AABB0
bool CGBattlefieldInfo::GetBracketForLevel(uint32_t level, const BattlemasterListRec* rec, int32_t* bracket) {
    if (!rec || !bracket) {
        return false;
    }

    *bracket = 0;
    bool foundMap = false;

    for (int32_t i = 0; i < g_pvpDifficultyDB.m_numRecords; i++) {
        PvpDifficultyRec* difficulty = g_pvpDifficultyDB.GetRecordByIndex(i);

        if (!difficulty || difficulty->m_mapID != rec->m_mapID[0]) {
            if (foundMap) {
                return false;
            }

            continue;
        }

        if (level >= static_cast<uint32_t>(difficulty->m_minLevel) && level <= static_cast<uint32_t>(difficulty->m_maxLevel)) {
            *bracket = difficulty->m_rangeIndex;
            return true;
        }

        foundMap = true;
    }

    return static_cast<int32_t>(level) >= rec->m_minLevel && static_cast<int32_t>(level) <= rec->m_maxLevel;
}

// OFFSET: 0x0093EF90/0x1406AACA0
void CGBattlefieldInfo::UpdateWarGameTypeList() {
    CGBattlefieldInfo::m_numWarGameTypes = 0;

    for (uint32_t i = 0; i < CGBattlefieldInfo::m_warGameTypes.Count(); i++) {
        WarGameType* type = CGBattlefieldInfo::m_warGameTypes[i];

        if (type->m_bgTypeID != -1) {
            type->m_collapsed = ((1 << type->m_instanceType) & CGBattlefieldInfo::m_collapsedWarGameHeaders) != 0;
        }

        if (!CGBattlefieldInfo::m_warGameTypes[i]->m_collapsed) {
            CGBattlefieldInfo::m_numWarGameTypes++;
        }
    }

    qsort(CGBattlefieldInfo::m_warGameTypes.m_data, CGBattlefieldInfo::m_warGameTypes.Count(), sizeof(WarGameType*), QSortWarGameTypes);
}

// OFFSET: 0x0093F010
const char* CGBattlefieldInfo::GetBattlefieldMgrZoneName(uint64_t queueID) {
    World_PVP_AreaRec* rec = g_world_PVP_AreaDB.GetRecord(static_cast<int32_t>(queueID & 0xFFFF));

    if (rec) {
        AreaTableRec* area = g_areaTableDB.GetRecord(rec->m_areaID);

        if (area) {
            return area->m_areaName;
        }
    }

    return NULL;
}

// OFFSET: 0x0093F070
bool OnRatedBgStats(ClientRatedBgStats* msg) {
    CGBattlefieldInfo::m_ratedBgStats[0] = msg->unk_0034 + msg->unk_0038 + msg->unk_003C;
    CGBattlefieldInfo::m_ratedBgStats[1] = msg->unk_0010 + msg->unk_0014 + msg->unk_0018;
    CGBattlefieldInfo::m_ratedBgStats[2] = msg->unk_0040 + msg->unk_0044 + msg->unk_0048;
    CGBattlefieldInfo::m_ratedBgStats[3] = msg->unk_001C + msg->unk_0020 + msg->unk_0024;
    FrameScript_SignalEvent(SCRIPT_EVENT_PVP_RATED_STATS_UPDATE, NULL);
    return true;
}

// OFFSET: 0x0093F0D0
bool OnRequestPvpRewardsResponse(ClientRequestPvpRewardsResponse* msg) {
    CGBattlefieldInfo::m_pvpRewardTotal = msg->unk_0024;
    CGBattlefieldInfo::m_pvpRewards[0] = msg->unk_0020;
    CGBattlefieldInfo::m_pvpRewards[1] = msg->unk_0014;
    CGBattlefieldInfo::m_pvpRewards[2] = msg->unk_0018;
    CGBattlefieldInfo::m_pvpRewards[3] = msg->unk_001C;
    CGBattlefieldInfo::m_pvpRewards[4] = msg->unk_0010;
    FrameScript_SignalEvent(SCRIPT_EVENT_PVP_REWARDS_UPDATE, NULL);
    return true;
}

// OFFSET: 0x0093F120
bool OnBattlefieldRatedInfo(ClientBattlefieldRatedInfo* msg) {
    uint8_t index = msg->unk_002C;

    if (index <= 3) {
        PersonalRatedInfo* info = &CGBattlefieldInfo::m_personalRatedInfo[static_cast<int8_t>(index)];
        info->m_seasonWon = msg->unk_0018;
        info->m_seasonPlayed = msg->unk_001C;
        info->m_weeklyPlayed = msg->unk_0028;
        info->m_weeklyWon = msg->unk_0010;
        info->m_weeklyCap = msg->unk_0024;
        info->m_rating = msg->unk_0014;
        FrameScript_SignalEvent(SCRIPT_EVENT_PVP_RATED_STATS_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x00940390/0x1406AC3F0
void ScoreNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGBattlefieldInfo::m_nameCachePending || (--CGBattlefieldInfo::m_nameCachePending, !CGBattlefieldInfo::m_nameCachePending)) {
        BattlefieldNameCacheCallback();
    }
}

// OFFSET: 0x009403B0
void CGBattlefieldInfo::SetScoreSortColumn(int32_t column) {
    if (CGBattlefieldInfo::m_nameCachePending) {
        return;
    }

    uint32_t index;

    for (index = 0; CGBattlefieldInfo::m_scoreSortCriteria[index].m_column != column; index++) {
        if (index + 1 >= 22) {
            BattlefieldNameCacheCallback();
            return;
        }
    }

    int32_t descending = CGBattlefieldInfo::m_scoreSortCriteria[index].m_descending;

    if (!index) {
        descending = descending == 0;
    }

    for (uint32_t i = index; i; i--) {
        CGBattlefieldInfo::m_scoreSortCriteria[i] = CGBattlefieldInfo::m_scoreSortCriteria[i - 1];
    }

    CGBattlefieldInfo::m_scoreSortCriteria[0].m_column = column;
    CGBattlefieldInfo::m_scoreSortCriteria[0].m_descending = descending;
    BattlefieldNameCacheCallback();
}

// OFFSET: 0x00940430/0x1406AC520
const char* CGBattlefieldInfo::GetFlagToken(uint32_t index) {
    uint32_t count;

    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        count = 2;
    } else if (CGBattlefieldInfo::m_flagCarriers[0]) {
        count = 1;
    } else {
        return NULL;
    }

    if (index >= count) {
        return NULL;
    }

    BattlefieldScore* score = CGBattlefieldInfo::FindScore(CGBattlefieldInfo::m_flagCarriers[index]);
    int32_t faction;

    if (score) {
        faction = score->m_faction;
    } else {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (CGRaidInfo::FindMemberByGuid(CGBattlefieldInfo::m_flagCarriers[index])) {
            faction = player->GetPVPFactionIndex();
        } else {
            faction = player->GetPVPFactionIndex() == 0;
        }
    }

    if (CGBattlefieldInfo::m_activeMapID == 566 || CGBattlefieldInfo::m_activeMapID == 968) {
        return s_flagTokens[faction == 0];
    }

    return s_flagTokens[faction];
}

// OFFSET: 0x00940500
void CGBattlefieldInfo::AddArenaOpponentPet(const WOWGUID& petGuid, const WOWGUID& ownerGuid) {
    WOWGUID owner = ownerGuid;

    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] == owner) {
            CGBattlefieldInfo::AddArenaOpponentPetToSlot(petGuid, ownerGuid, i, 0);
            return;
        }

        if (CGBattlefieldInfo::m_arenaOpponentPets[i][1] == owner) {
            CGBattlefieldInfo::AddArenaOpponentPetToSlot(petGuid, ownerGuid, i, 1);
            return;
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (!CGBattlefieldInfo::m_arenaOpponents[i] && !CGBattlefieldInfo::m_arenaOpponentPets[i][0]) {
            CGBattlefieldInfo::AddArenaOpponentPetToSlot(petGuid, ownerGuid, i, 1);
            return;
        }
    }
}

// OFFSET: 0x009405C0
bool OnPvpLogData(ClientPvpLogData* msg) {
    if (CGBattlefieldInfo::m_nameCachePending) {
        for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
            g_nameDBCache.CancelCallback(CGBattlefieldInfo::m_scores[i]->m_guid.m_guid, ScoreNameQueryCallback, NULL);
        }
    }

    if (!msg->m_hasRatings) {
        CGBattlefieldInfo::m_teamRatings[0] = 0;
        CGBattlefieldInfo::m_newTeamRatings[0] = 0;
        CGBattlefieldInfo::m_avgMatchmakingRating[0] = 0;
        CGBattlefieldInfo::m_teamRatings[1] = 0;
        CGBattlefieldInfo::m_newTeamRatings[1] = 0;
        CGBattlefieldInfo::m_avgMatchmakingRating[1] = 0;
    } else {
        CGBattlefieldInfo::m_teamRatings[0] = msg->m_prematchRating[0];
        CGBattlefieldInfo::m_newTeamRatings[0] = msg->m_newRating[0];
        CGBattlefieldInfo::m_avgMatchmakingRating[0] = msg->m_matchmakingRating[0];
        CGBattlefieldInfo::m_teamRatings[1] = msg->m_prematchRating[1];
        CGBattlefieldInfo::m_newTeamRatings[1] = msg->m_newRating[1];
        CGBattlefieldInfo::m_avgMatchmakingRating[1] = msg->m_matchmakingRating[1];
    }

    if (!msg->m_hasTeamNames) {
        for (uint32_t i = 0; i < 2; i++) {
            CGBattlefieldInfo::m_teamNames[i][0] = '\0';
        }
    } else {
        for (uint32_t i = 0; i < 2; i++) {
            SStrCopy(CGBattlefieldInfo::m_teamNames[i], msg->m_teamNames[i], sizeof(CGBattlefieldInfo::m_teamNames[i]));
        }
    }

    CGBattlefieldInfo::m_teamPlayerCounts[0] = msg->m_playerCount[0];
    CGBattlefieldInfo::m_teamPlayerCounts[1] = msg->m_playerCount[1];
    CGBattlefieldInfo::m_mapDone = msg->m_hasWinner;

    if (CGBattlefieldInfo::m_mapDone) {
        CGBattlefieldInfo::m_winner = msg->m_winner;
    }

    CGBattlefieldInfo::m_nameCachePending = 0;
    CGBattlefieldInfo::m_numScoreEntries = msg->m_players.Count();

    for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
        JamPvpLogDataPlayer& player = msg->m_players[i];
        BattlefieldScore* score = CGBattlefieldInfo::m_scores[i];
        score->m_guid = *reinterpret_cast<const WOWGUID*>(player.m_playerGUID);
        score->m_killingBlows = player.m_kills;
        score->m_faction = player.m_faction;

        if (!player.m_hasHonor) {
            score->m_honorableKills = 0;
            score->m_deaths = 0;
            score->m_bonusHonor = 0;
        } else {
            score->m_honorableKills = player.m_honorKills;
            score->m_deaths = player.m_deaths;
            score->m_bonusHonor = player.m_contributionPoints;
        }

        score->m_damageDone = player.m_damageDone;
        score->m_healingDone = player.m_healingDone;
        score->m_rating = player.m_hasRating ? player.m_rating : 0;
        score->m_ratingChange = player.m_hasRatingChange ? player.m_ratingChange : 0;
        score->m_preMatchMMR = player.m_hasPreMatchMMR ? player.m_preMatchMMR : 0;
        score->m_mmrChange = player.m_hasMmrChange ? player.m_mmrChange : 0;

        for (uint32_t j = 0; j < player.m_stats.Count() && j < 8; j++) {
            score->m_stats[j] = player.m_stats[j];
        }

        if (!g_nameDBCache.GetRecord(score->m_guid.m_guid, score->m_guid, ScoreNameQueryCallback, NULL, false)) {
            CGBattlefieldInfo::m_nameCachePending++;
        }

        score->m_talentSpec = player.m_primaryTalentTree;
    }

    if (!CGBattlefieldInfo::m_nameCachePending) {
        BattlefieldNameCacheCallback();
    }

    return true;
}

// OFFSET: 0x009408A0
bool OnBattlefieldPortDenied(ClientBattlefieldPortDenied* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGGameUI::DisplayError(ERR_PLAYER_DEAD);
    }

    return true;
}

// OFFSET: 0x009408D0
bool OnBattlefieldMgrEntryInvite(ClientBattlefieldMgrEntryInvite* msg) {
    uint64_t queueID = *reinterpret_cast<const uint64_t*>(msg->unk_0018);
    uint32_t expireTime = msg->unk_0010;
    int32_t areaID = msg->unk_0020;
    WorldPvPQueueStatus* queue = CGBattlefieldInfo::FindWorldPvPQueue(queueID);

    if (queue || (queue = CGBattlefieldInfo::FindFreeWorldPvPQueue())) {
        if (areaID) {
            queue->m_areaID = areaID;
        }

        if (expireTime) {
            queue->m_expireTime = expireTime;
        }

        queue->m_queueID = queueID;
        queue->m_status = 2;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_ENTRY_INVITE, "%d%s", static_cast<uint16_t>(queueID), CGBattlefieldInfo::GetBattlefieldMgrZoneName(queueID));
    return true;
}

// OFFSET: 0x00940960/0x1406ACC60
void CGBattlefieldInfo::BattlefieldMgrEntryInviteResponse(uint16_t battleID, int32_t a2, int32_t accept) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    UserClientBFMgrEntryInviteResponse msg;
    *reinterpret_cast<uint64_t*>(msg.unk_0010) = battleID | 0x1F10000000020000ull;
    msg.unk_0018 = accept != 0;
    ClientServices::Send(&msg);
}

// OFFSET: 0x009409D0
bool OnBattlefieldMgrQueueRequestResponse(ClientBattlefieldMgrQueueRequestResponse* msg) {
    int32_t inZone = 0;

    if (CGGameUI::m_areaID == msg->unk_0020) {
        inZone = 1;
    } else {
        AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

        if (area && area->m_parentAreaID == msg->unk_0020) {
            inZone = 1;
        }
    }

    uint64_t queueID = *reinterpret_cast<const uint64_t*>(msg->unk_0028);
    bool loggingIn = msg->unk_0012 != 0;
    WorldPvPQueueStatus* queue = CGBattlefieldInfo::FindWorldPvPQueue(queueID);

    if (queue || (queue = CGBattlefieldInfo::FindFreeWorldPvPQueue())) {
        if (msg->unk_0020) {
            queue->m_areaID = msg->unk_0020;
        }

        queue->m_queueID = queueID;
        queue->m_status = loggingIn;
    }

    const char* zoneName = CGBattlefieldInfo::GetBattlefieldMgrZoneName(queueID);
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, "%d%b%b%b%b%s", static_cast<uint16_t>(queueID), static_cast<int8_t>(msg->unk_0012), msg->unk_0010 == 1, inZone, msg->unk_0011, zoneName);
    return true;
}

// OFFSET: 0x00940AB0
void CGBattlefieldInfo::BattlefieldMgrQueueRequest(uint16_t battleID) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    UserClientBFMgrQueueRequest msg(battleID | 0x1F10000000020000ull);
    ClientServices::Send(&msg);
}

// OFFSET: 0x00940B10
bool OnBattlefieldMgrEjectPending(ClientBattlefieldMgrEjectPending* msg) {
    uint64_t queueID = *reinterpret_cast<const uint64_t*>(&msg->unk_0010);
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_EJECT_PENDING, "%d%b%s", static_cast<uint16_t>(queueID), msg->unk_0018, CGBattlefieldInfo::GetBattlefieldMgrZoneName(queueID));
    return true;
}

// OFFSET: 0x00940B50
bool OnBattlefieldMgrEjected(ClientBattlefieldMgrEjected* msg) {
    uint64_t queueID = *reinterpret_cast<const uint64_t*>(&msg->unk_0018);

    int8_t reason = msg->unk_0021;

    switch (reason) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11: {
            WorldPvPQueueStatus* queue = CGBattlefieldInfo::FindWorldPvPQueue(queueID);

            if (queue || (queue = CGBattlefieldInfo::FindFreeWorldPvPQueue())) {
                queue->m_queueID = queueID;
                queue->m_status = 0;
            }

            break;
        }
    }

    const char* zoneName = CGBattlefieldInfo::GetBattlefieldMgrZoneName(queueID);
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_EJECTED, "%d%b%b%b%b%s", static_cast<uint16_t>(queueID), reason == 8, msg->unk_0010, msg->unk_0020 == 2, reason == 10, zoneName);
    return true;
}

// OFFSET: 0x00940C10
bool OnBattlefieldMgrQueueInvite(ClientBattlefieldMgrQueueInvite* msg) {
    uint64_t queueID = *reinterpret_cast<const uint64_t*>(msg->unk_0010);
    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_MGR_QUEUE_INVITE, "%d%b%s", static_cast<uint16_t>(queueID), msg->unk_0024 == 1, CGBattlefieldInfo::GetBattlefieldMgrZoneName(queueID));
    return true;
}

// OFFSET: 0x00940C50/0x1406AD040
void CGBattlefieldInfo::BattlefieldMgrQueueInviteResponse(uint16_t battleID, int32_t a2, int32_t accept) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    UserClientBFMgrQueueInviteResponse msg(battleID | 0x1F10000000020000ull, accept != 0);
    ClientServices::Send(&msg);
}

// OFFSET: 0x00940CC0
void CGBattlefieldInfo::BattlefieldMgrExitRequest(uint16_t battleID) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    UserClientBFMgrQueueExitRequest msg(battleID | 0x1F10000000020000ull);
    ClientServices::Send(&msg);
}

// OFFSET: 0x00941EC0
BattlefieldStatusNode* CGBattlefieldInfo::FindStatus(const JamClientRideTicket& ticket) {
    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (*reinterpret_cast<const uint64_t*>(ticket.m_requesterGuid) == *reinterpret_cast<const uint64_t*>(node->m_ticket.m_requesterGuid) && ticket.m_id == node->m_ticket.m_id && ticket.m_type == node->m_ticket.m_type && ticket.m_time == node->m_ticket.m_time) {
            return node;
        }
    }

    return NULL;
}

// OFFSET: 0x00941F20
BattlefieldStatusNode* CGBattlefieldInfo::FindStatus(uint32_t id) {
    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == id) {
            return node;
        }
    }

    return NULL;
}

// OFFSET: 0x00941F50
void CGBattlefieldInfo::RequestBattlegroundInstanceInfo(int32_t index) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    if (index < 0 || index >= static_cast<int32_t>(CGBattlefieldInfo::m_battlegroundTypes.Count())) {
        return;
    }

    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_battlegroundTypes[index]);

    if (rec) {
        UserClientBattlefieldList msg(rec->m_id);
        ClientServices::Send(&msg);
        CGBattlefieldInfo::m_instances.Clear();
        CGBattlefieldInfo::m_bgTypeID = rec->m_id;
    }
}

// OFFSET: 0x00942000/0x1406AE660
void CGBattlefieldInfo::ClearWarGameTypes() {
    CGBattlefieldInfo::m_numWarGameTypes = 0;

    for (int32_t i = CGBattlefieldInfo::m_warGameTypes.Count() - 1; i >= 0; i--) {
        WarGameType* type = CGBattlefieldInfo::m_warGameTypes[i];

        if (type) {
            SMemFree(type, "delete", -1, 0);
        }
    }

    CGBattlefieldInfo::m_warGameTypes.Clear();
}

// OFFSET: 0x00942070/0x1406AE700
void CGBattlefieldInfo::BuildWarGameTypeList() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    uint32_t level = player->m_unit->m_unitFieldLevel;
    bool battlegrounds = false;
    bool arenas = false;
    BattlemasterListRec* arenaList = g_battlemasterListDB.GetRecord(6);
    CGBattlefieldInfo::ClearWarGameTypes();

    for (int32_t i = 0; i < g_battlemasterListDB.m_numRecords; i++) {
        BattlemasterListRec* rec = g_battlemasterListDB.GetRecordByIndex(i);
        int32_t bracket;

        if (!rec || !CGBattlefieldInfo::GetBracketForLevel(level, rec, &bracket)) {
            continue;
        }

        if (rec->m_instanceType != 3 || (rec->m_flags & 0x2)) {
            if (rec->m_instanceType != 4) {
                continue;
            }

            uint32_t map;

            for (map = 0; arenaList->m_mapID[map] != rec->m_mapID[0]; map++) {
                if (map + 1 >= 8) {
                    break;
                }
            }

            if (map + 1 >= 8 && arenaList->m_mapID[map] != rec->m_mapID[0]) {
                continue;
            }

            WarGameType* type = static_cast<WarGameType*>(SMemAlloc(sizeof(WarGameType), __FILE__, __LINE__, 0));
            type->m_bgTypeID = rec->m_id;
            type->m_instanceType = 4;
            type->m_collapsed = false;
            CGBattlefieldInfo::m_warGameTypes.Add(1, &type);
            arenas = true;
        } else {
            WarGameType* type = static_cast<WarGameType*>(SMemAlloc(sizeof(WarGameType), __FILE__, __LINE__, 0));
            type->m_bgTypeID = rec->m_id;
            type->m_instanceType = 3;
            type->m_collapsed = false;
            CGBattlefieldInfo::m_warGameTypes.Add(1, &type);
            battlegrounds = true;
        }
    }

    if (battlegrounds) {
        WarGameType* type = static_cast<WarGameType*>(SMemAlloc(sizeof(WarGameType), __FILE__, __LINE__, 0));
        type->m_bgTypeID = -1;
        type->m_instanceType = 3;
        type->m_collapsed = false;
        CGBattlefieldInfo::m_warGameTypes.Add(1, &type);
    }

    if (arenas) {
        WarGameType* type = static_cast<WarGameType*>(SMemAlloc(sizeof(WarGameType), __FILE__, __LINE__, 0));
        type->m_bgTypeID = -1;
        type->m_instanceType = 4;
        type->m_collapsed = false;
        CGBattlefieldInfo::m_warGameTypes.Add(1, &type);
    }

    CGBattlefieldInfo::m_numWarGameTypes = CGBattlefieldInfo::m_warGameTypes.Count();
}

// OFFSET: 0x00942A80
BattlefieldStatusNode::BattlefieldStatusNode() {
    this->m_id = 0;
    this->m_queueID = 0;
    this->m_mapID = 0;
    this->m_status = 0;
    this->m_minLevel = 0;
    this->m_maxLevel = 0;
    this->m_instanceID = 0;
    this->m_portExpiration = 0;
    this->m_estimatedWaitTime = 0;
    this->m_queueStartTime = 0;
    this->m_teamSize = 0;
    this->m_registeredMatch = 0;
    this->m_eligibleInQueue = 0;
    this->m_waitingOnOtherActivity = 0;
    this->m_waitTeamReady[0] = 0;
    this->m_waitTeamReady[1] = 0;
    this->m_waitTeamSize[0] = 0;
    this->m_waitTeamSize[1] = 0;
}

// OFFSET: 0x00942AE0
uint64_t CGBattlefieldInfo::GetActiveQueueID() {
    for (BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head(); node; node = CGBattlefieldInfo::m_statusList.Next(node)) {
        if (node->m_id == CGBattlefieldInfo::m_activeBattlefieldID) {
            return node->m_queueID;
        }
    }

    return 0;
}

// OFFSET: 0x00942B20/0x1406AF400
void CGBattlefieldInfo::RequestPlayerPositions() {
    if (!CGBattlefieldInfo::GetActiveBattlefieldTypeID()) {
        CGBattlefieldInfo::m_numPlayerPositions = 0;
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (!CGBattlefieldInfo::m_positionUpdateTime || static_cast<int32_t>(now - CGBattlefieldInfo::m_positionUpdateTime) >= 0) {
        UserClientRequestBattlefieldPositions msg;
        ClientServices::Send(&msg);
        CGBattlefieldInfo::m_positionUpdateTime = now + 5000;
    }
}

// OFFSET: 0x00942B80
bool OnBattlefieldList(ClientBattlefieldList* msg) {
    int32_t bgTypeID = msg->unk_0018;
    CGBattlefieldInfo::m_bgTypeID = bgTypeID;
    CGBattlefieldInfo::m_minLevel = msg->unk_002A;
    CGBattlefieldInfo::m_maxLevel = msg->unk_0014;
    CGBattlefieldInfo::m_hasHolidayWinToday = msg->unk_0029;
    CGBattlefieldInfo::m_holidayWinHonor = msg->unk_0010;
    CGBattlefieldInfo::m_holidayWinArena = msg->unk_0044;
    CGBattlefieldInfo::m_holidayLossHonor = msg->unk_0050;
    CGBattlefieldInfo::m_hasRandomWinToday = msg->unk_0054;
    CGBattlefieldInfo::m_randomWinHonor = msg->unk_004C;
    CGBattlefieldInfo::m_randomWinArena = msg->unk_002C;
    CGBattlefieldInfo::m_randomLossHonor = msg->unk_0048;
    uint32_t count = msg->unk_0030.Count();

    CGBattlefieldInfo::m_instances.SetCount(count);

    for (uint32_t i = 0; i < CGBattlefieldInfo::m_instances.Count(); i++) {
        CGBattlefieldInfo::m_instances[i] = msg->unk_0030[i];
    }

    CGBattlefieldInfo::m_allowJoin = 1;

    if (msg->unk_0040) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PVPQUEUE_ANYWHERE_SHOW, NULL);
        return true;
    }

    CGBattlefieldInfo::m_battlemaster = *reinterpret_cast<const WOWGUID*>(msg->unk_0020);
    int32_t arena = 0;
    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_bgTypeID);

    if (rec && rec->m_instanceType == 4) {
        arena = 1;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELDS_SHOW, "%b%d", arena, CGBattlefieldInfo::m_bgTypeID);
    return true;
}

// OFFSET: 0x00942CF0
BattlefieldStatusNode* CGBattlefieldInfo::UpdateStatus(const JamClientBattlefieldStatusHeader& header) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::FindStatus(header.m_ticket);

    if (!node) {
        uint32_t id = 1;

        while (CGBattlefieldInfo::FindStatus(id)) {
            id++;
        }

        node = new (__FILE__, __LINE__) BattlefieldStatusNode;
        node->m_id = id;
        node->m_ticket = header.m_ticket;

        if (id > CGBattlefieldInfo::m_maxBattlefieldID) {
            CGBattlefieldInfo::m_maxBattlefieldID = id;
        }

        CGBattlefieldInfo::m_statusList.LinkToTail(node);
    }

    node->m_queueID = *reinterpret_cast<const uint64_t*>(header.m_queueID);
    node->m_teamSize = header.m_teamSize;
    node->m_minLevel = header.m_rangeMin;
    node->m_maxLevel = header.m_rangeMax;
    node->m_instanceID = header.m_instanceID;
    node->m_registeredMatch = header.m_registeredMatch;
    node->m_portExpiration = 0;
    CGBattlefieldInfo::m_instanceExpiration = 0;
    CGBattlefieldInfo::m_instanceStartTime = 0;

    if (CGBattlefieldInfo::m_activeBattlefieldID == node->m_id) {
        CGBattlefieldInfo::m_activeBattlefieldID = 0;
    }

    node->m_estimatedWaitTime = 0;
    node->m_queueStartTime = 0;
    return node;
}

// OFFSET: 0x00942DF0
bool OnBattlefieldStatusNeedConfirmation(ClientBattlefieldStatusNeedconfirmation* msg) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::UpdateStatus(msg->m_header);

    if (node) {
        node->m_status = 2;
        node->m_mapID = msg->unk_0010;
        CGTutorial::TriggerTutorial(48);
        node->m_portExpiration = msg->unk_0014 ? msg->unk_0014 + FrameTime::GetCurTimeMs() : 0;
        CGLCD::OnBattlefieldReady(node);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_STATUS, "%d", node->m_id);
    }

    return true;
}

// OFFSET: 0x00942E60
bool OnBattlefieldStatusActive(ClientBattlefieldStatusActive* msg) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::UpdateStatus(msg->m_header);

    if (!node) {
        return true;
    }

    node->m_status = 4;
    CGBattlefieldInfo::m_activeMapID = msg->unk_0050;
    MapRec* map = g_mapDB.GetRecord(CGBattlefieldInfo::m_activeMapID);

    if (map) {
        CGBattlefieldInfo::m_instanceType = map->m_instanceType;
    }

    CGBattlefieldInfo::m_instanceExpiration = msg->unk_0010 ? msg->unk_0010 + FrameTime::GetCurTimeMs() : 0;
    CGBattlefieldInfo::m_instanceStartTime = msg->unk_0048 ? FrameTime::GetCurTimeMs() - msg->unk_0048 : 0;
    CGBattlefieldInfo::m_arenaFaction = msg->unk_004C;
    uint32_t numStats = 0;
    uint32_t first = 0;
    CGBattlefieldInfo::m_activeBattlefieldID = node->m_id;
    CGBattlefieldInfo::m_numWorldStateStats = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_worldStateUIDB.m_numRecords); i++) {
        WorldStateUIRec* rec = g_worldStateUIDB.GetRecordByIndex(i);

        if ((rec->m_mapID == static_cast<int32_t>(CGBattlefieldInfo::m_activeMapID) || rec->m_mapID == -1) && rec->m_type == 2) {
            if (!numStats) {
                first = i;
            }

            CGBattlefieldInfo::m_numWorldStateStats = ++numStats;
        } else if (numStats) {
            break;
        }
    }

    for (uint32_t i = 0; i < numStats; i++) {
        CGBattlefieldInfo::m_worldStateStats[i] = g_worldStateUIDB.GetRecordByIndex(first + i)->m_id;
    }

    CGBattlefieldInfo::m_logFilterFaction = -1;
    BattlefieldNameCacheCallback();
    CGBarberShop::DisableBarberShop();
    CGLCD::OnBattlefieldActive();
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_STATUS, "%d", node->m_id);
    return true;
}

// OFFSET: 0x00942FF0
bool OnBattlefieldStatusQueued(ClientBattlefieldStatusQueued* msg) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::UpdateStatus(msg->m_header);

    if (node) {
        if (!node->m_status) {
            BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(static_cast<uint16_t>(node->m_queueID));

            if (rec) {
                if (msg->unk_0014) {
                    CGGameUI::DisplayError(ERR_GROUP_JOIN_BATTLEGROUND_S, static_cast<const char*>(rec->m_name));
                }
            } else {
                CGGameUI::DisplayError(ERR_GROUP_JOIN_BATTLEGROUND_FAIL);
            }
        }

        node->m_status = 1;
        CGTutorial::TriggerTutorial(47);
        node->m_estimatedWaitTime = msg->unk_0018;
        node->m_queueStartTime = FrameTime::GetCurTimeMs() - msg->unk_0010;
        node->m_eligibleInQueue = msg->unk_0015;
        node->m_waitingOnOtherActivity = msg->unk_001C;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_STATUS, "%d", node->m_id);
    }

    return true;
}

// OFFSET: 0x009430B0
bool OnBattlefieldStatusWaitForGroups(ClientBattlefieldStatusWaitforgroups* msg) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::UpdateStatus(msg->m_header);

    if (node) {
        node->m_status = 3;
        node->m_waitTeamReady[0] = msg->unk_004A;
        node->m_waitTeamSize[0] = msg->unk_0048;
        node->m_waitTeamReady[1] = msg->unk_004B;
        node->m_waitTeamSize[1] = msg->unk_0049;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_STATUS, "%d", node->m_id);
    }

    return true;
}

// OFFSET: 0x00943290/0x1406AFDA0
bool CGBattlefieldInfo::IsRatedBattleground() {
    uint64_t queueID = CGBattlefieldInfo::GetActiveQueueID();

    if (!queueID) {
        return false;
    }

    return !(queueID & 0xF0000) && (queueID & 0x100000);
}

// OFFSET: 0x00943350
static void DeleteBattlefieldStatus(BattlefieldStatusNode* node) {
    delete node;
}

// OFFSET: 0x009433B0
void CGBattlefieldInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_ARENA_OPPONENT_UPDATE);
    ClientBattlefieldList::s_handler = NULL;
    ClientBattlefieldStatusActive::s_handler = NULL;
    ClientBattlefieldStatusNeedconfirmation::s_handler = NULL;
    ClientBattlefieldStatusQueued::s_handler = NULL;
    ClientBattlefieldStatusFailed::s_handler = NULL;
    ClientBattlefieldStatusWaitforgroups::s_handler = NULL;
    ClientBattlefieldPlayerPositions::s_handler = NULL;
    ClientRatedBgStats::s_handler = NULL;
    ClientBattlegroundPlayerJoined::s_handler = NULL;
    ClientBattlegroundPlayerLeft::s_handler = NULL;
    ClientBattlefieldPortDenied::s_handler = NULL;
    ClientBattlefieldMgrEntered::s_handler = NULL;
    ClientBattlefieldMgrQueueRequestResponse::s_handler = NULL;
    ClientBattlefieldMgrEjectPending::s_handler = NULL;
    ClientBattlefieldMgrEjected::s_handler = NULL;
    ClientBattlefieldMgrQueueInvite::s_handler = NULL;
    ClientBattlefieldMgrStateChange::s_handler = NULL;
    ClientBattlefieldMgrEntryInvite::s_handler = NULL;
    ClientWargameCheckEntry::s_handler = NULL;
    ClientBattlefieldRatedInfo::s_handler = NULL;
    ClientWargameRequestSent::s_handler = NULL;
    ClientPvpLogData::s_handler = NULL;
    ClientPvpOptionsEnabled::s_handler = NULL;
    ClientRequestPvpRewardsResponse::s_handler = NULL;

    for (uint32_t i = 0; i < 80; i++) {
        if (CGBattlefieldInfo::m_scores[i]) {
            SMemFree(CGBattlefieldInfo::m_scores[i], "delete", -1, 0);
        }
    }

    CGBattlefieldInfo::m_activeBattlefieldID = 0;
    BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head();

    while (node) {
        BattlefieldStatusNode* next = CGBattlefieldInfo::m_statusList.Next(node);
        DeleteBattlefieldStatus(node);
        node = next;
    }

    CGBattlefieldInfo::m_maxBattlefieldID = 0;
    CGBattlefieldInfo::m_battlegroundTypes.Clear();
    CGBattlefieldInfo::ClearWarGameTypes();
}

// OFFSET: 0x00943540/0x1406B0160
void CGBattlefieldInfo::EnterWorld() {
    CGBattlefieldInfo::m_listUnk = 0;
    CGBattlefieldInfo::m_bgTypeID = 0;
    CGBattlefieldInfo::m_minLevel = 0;
    CGBattlefieldInfo::m_maxLevel = 0;
    CGBattlefieldInfo::m_battlemaster = WOWGUID();
    CGBattlefieldInfo::m_nextLogUpdate = 0;
    CGBattlefieldInfo::m_mapDone = 0;
    CGBattlefieldInfo::m_winner = 0;
    CGBattlefieldInfo::m_logFilterFaction = -1;
    CGBattlefieldInfo::m_allowJoin = 1;
    CGBattlefieldInfo::m_warGameRequester = WOWGUID();
    CGBattlefieldInfo::m_warGameQueueID = 0;

    if (CGBattlefieldInfo::m_nameCachePending) {
        for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
            g_nameDBCache.CancelCallback(CGBattlefieldInfo::m_scores[i]->m_guid.m_guid, ScoreNameQueryCallback, NULL);
        }
    }

    CGBattlefieldInfo::m_numScoreEntries = 0;
    CGBattlefieldInfo::m_numScores = 0;
    CGBattlefieldInfo::m_nameCachePending = 0;
    CGBattlefieldInfo::m_numWorldStateStats = 0;
    CGBattlefieldInfo::m_numPlayerPositions = 0;
    CGBattlefieldInfo::m_flagCarriers[0] = WOWGUID();
    CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
    CGBattlefieldInfo::m_flagPositions[0].x = 0.0f;
    CGBattlefieldInfo::m_flagPositions[0].y = 0.0f;
    CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
    CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
    CGBattlefieldInfo::m_positionUpdateTime = 0;
    CGBattlefieldInfo::m_teamPlayerCounts[1] = 0;
    CGBattlefieldInfo::m_teamPlayerCounts[0] = 0;

    for (uint32_t i = 0; i < 2; i++) {
        CGBattlefieldInfo::m_teamNames[i][0] = '\0';
    }

    CGBattlefieldInfo::m_activeBattlefieldID = 0;
    BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head();

    while (node) {
        BattlefieldStatusNode* next = CGBattlefieldInfo::m_statusList.Next(node);
        DeleteBattlefieldStatus(node);
        node = next;
    }

    if (!CGGameUI::m_currentlyReloadingUI) {
        memset(CGBattlefieldInfo::m_arenaOpponentPets, 0, sizeof(CGBattlefieldInfo::m_arenaOpponentPets));

        for (uint32_t i = 0; i < 5; i++) {
            CGBattlefieldInfo::m_arenaOpponents[i] = WOWGUID();
        }

        char token[32];

        for (uint32_t i = 1; i <= 5; i++) {
            SStrPrintf(token, sizeof(token), "arena%d", i);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "cleared");
            SStrPrintf(token, sizeof(token), "arenapet%d", i);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "cleared");
        }
    }

    UserClientRequestBattlefieldStatus statusMsg;
    ClientServices::Send(&statusMsg);
    UserClientQueryBattlefieldState stateMsg;
    ClientServices::Send(&stateMsg);
}

// OFFSET: 0x009437E0/0x1406B0440
void CGBattlefieldInfo::LeaveWorld() {
    if (!CGGameUI::m_currentlyReloadingUI) {
        memset(CGBattlefieldInfo::m_vehicleGuids, 0, sizeof(CGBattlefieldInfo::m_vehicleGuids));
        CGBattlefieldInfo::m_numVehicles = 0;
    }

    CGBattlefieldInfo::m_instances.Clear();
    CGBattlefieldInfo::m_activeBattlefieldID = 0;
    BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head();

    while (node) {
        BattlefieldStatusNode* next = CGBattlefieldInfo::m_statusList.Next(node);
        DeleteBattlefieldStatus(node);
        node = next;
    }

    CGBattlefieldInfo::m_instanceType = 0;
}

// OFFSET: 0x009438B0
void CGBattlefieldInfo::RemoveStatus(const JamClientRideTicket& ticket) {
    BattlefieldStatusNode* node = CGBattlefieldInfo::FindStatus(ticket);

    if (!node) {
        return;
    }

    uint32_t id = node->m_id;
    DeleteBattlefieldStatus(node);

    if (id == CGBattlefieldInfo::m_activeBattlefieldID) {
        CGBattlefieldInfo::m_numPlayerPositions = 0;
        CGBattlefieldInfo::m_instanceExpiration = 0;
        CGBattlefieldInfo::m_instanceStartTime = 0;
        CGBattlefieldInfo::m_numWorldStateStats = 0;
        CGBattlefieldInfo::m_activeMapID = 0;
        CGBattlefieldInfo::m_instanceType = 0;
        CGBattlefieldInfo::m_activeBattlefieldID = 0;
        CGLCD::ShowBattlefieldScreen();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BATTLEFIELD_STATUS, "%d", id);
}

// OFFSET: 0x00943930
bool OnBattlefieldStatus(ClientBattlefieldStatus* msg) {
    CGBattlefieldInfo::RemoveStatus(msg->m_ticket);
    return true;
}

// OFFSET: 0x00943950
bool OnBattlefieldStatusFailed(ClientBattlefieldStatusFailed* msg) {
    switch (msg->unk_0018) {
        case 2:
            CGGameUI::DisplayError(ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_ARENA_TEAM_PARTY_SIZE);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_TOO_MANY_QUEUES);
            break;

        case 5:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_CANNOT_QUEUE_FOR_RATED);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_BATTLEDGROUND_QUEUED_FOR_RATED);
            break;

        case 7:
        case 13:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_TEAM_LEFT_QUEUE);
            break;

        case 8: {
            WOWGUID none = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(*reinterpret_cast<const uint64_t*>(&msg->unk_0020), none, NULL, NULL, false);

            if (name) {
                char fullName[308];
                name->FullName(fullName, 305);
                CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_TIMED_OUT, fullName);
                FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_QUEUE_TIMEOUT, NULL);
                CGBattlefieldInfo::RemoveStatus(msg->m_ticket);
                return true;
            }

            CGGameUI::DisplayError(ERR_BATTLEGROUND_NOT_IN_BATTLEGROUND);
            break;
        }

        case 9:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_XP_GAIN);
            break;

        case 10:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_RANGE_INDEX);
            break;

        case 11:
        case 12: {
            WOWGUID none = WOWGUID();
            NameCache* name = g_nameDBCache.GetRecord(*reinterpret_cast<const uint64_t*>(&msg->unk_0020), none, NULL, NULL, false);

            if (name) {
                char fullName[308];
                name->FullName(fullName, 305);
                CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_TIMED_OUT, fullName);
                FrameScript_SignalEvent(SCRIPT_EVENT_BATTLEFIELD_QUEUE_TIMEOUT, NULL);
                CGBattlefieldInfo::RemoveStatus(msg->m_ticket);
                return true;
            }

            CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_FAILED);
            break;
        }

        case 14:
            CGGameUI::DisplayError(ERR_LFG_CANT_USE_BATTLEGROUND);
            break;

        case 15:
            CGGameUI::DisplayError(ERR_IN_RANDOM_BG);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_IN_NON_RANDOM_BG);
            break;

        case 17:
            CGGameUI::DisplayError(ERR_BG_DEVELOPER_ONLY);
            break;

        case 18:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_INVITATION_DECLINED);
            break;

        case 19:
            CGGameUI::DisplayError(ERR_MEETING_STONE_NOT_FOUND);
            break;

        case 20:
            CGGameUI::DisplayError(ERR_WARGAME_REQUEST_FAILURE);
            break;

        case 22:
            CGGameUI::DisplayError(ERR_BATTLEFIELD_TEAM_PARTY_SIZE);
            break;

        case 23:
            CGGameUI::DisplayError(ERR_NOT_ON_TOURNAMENT_REALM);
            break;

        case 24:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_PLAYERS_FROM_DIFFERENT_REALMS);
            break;

        case 33:
            CGGameUI::DisplayError(ERR_REMOVE_FROM_PVP_QUEUE_GRANT_LEVEL);
            break;

        case 34:
            CGGameUI::DisplayError(ERR_REMOVE_FROM_PVP_QUEUE_FACTION_CHANGE);
            break;

        case 35:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_JOIN_FAILED);
            break;

        case 43:
            CGGameUI::DisplayError(ERR_BATTLEGROUND_DUPE_QUEUE);
            break;
    }

    CGBattlefieldInfo::RemoveStatus(msg->m_ticket);
    return true;
}

// OFFSET: 0x00943B70/0x1406B08D0
void CGBattlefieldInfo::AddArenaOpponentToSlot(const WOWGUID& guid, int32_t slot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (static_cast<uint8_t>(player->m_unit->m_unitFieldAurastate >> 19) & 1)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (CGBattlefieldInfo::m_arenaOpponents[slot] != guid) {
        CGBattlefieldInfo::m_arenaOpponents[slot] = guid;
        CGBattlefieldInfo::m_arenaOpponentStats[slot].Reset();

        if (unit) {
            unit->RegisterScript(0x4000);
        }
    }

    char token[32];
    SStrPrintf(token, sizeof(token), "arena%d", slot + 1);
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "seen");

    if (!unit) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", token, "unseen");
    }
}

// OFFSET: 0x00943C80/0x1406B09F0
void CGBattlefieldInfo::InitializeGame() {
    ClientBattlefieldList::s_handler = OnBattlefieldList;
    ClientBattlefieldStatus::s_handler = OnBattlefieldStatus;
    ClientBattlefieldStatusNeedconfirmation::s_handler = OnBattlefieldStatusNeedConfirmation;
    ClientBattlefieldStatusActive::s_handler = OnBattlefieldStatusActive;
    ClientBattlefieldStatusQueued::s_handler = OnBattlefieldStatusQueued;
    ClientBattlefieldStatusFailed::s_handler = OnBattlefieldStatusFailed;
    ClientBattlefieldStatusWaitforgroups::s_handler = OnBattlefieldStatusWaitForGroups;
    ClientBattlefieldPlayerPositions::s_handler = OnBattlefieldPlayerPositions;
    ClientBattlegroundPlayerLeft::s_handler = OnBattlegroundPlayerLeft;
    ClientBattlegroundPlayerJoined::s_handler = OnBattlegroundPlayerJoined;
    ClientWargameRequestSent::s_handler = OnWargameRequestSent;
    ClientBattlefieldMgrEntered::s_handler = OnBattlefieldMgrEntered;
    ClientBattlefieldPortDenied::s_handler = OnBattlefieldPortDenied;
    ClientBattlefieldMgrQueueRequestResponse::s_handler = OnBattlefieldMgrQueueRequestResponse;
    ClientBattlefieldMgrEjected::s_handler = OnBattlefieldMgrEjected;
    ClientBattlefieldMgrEjectPending::s_handler = OnBattlefieldMgrEjectPending;
    ClientBattlefieldMgrQueueInvite::s_handler = OnBattlefieldMgrQueueInvite;
    ClientBattlefieldMgrEntryInvite::s_handler = OnBattlefieldMgrEntryInvite;
    ClientBattlefieldMgrStateChange::s_handler = OnBattlefieldMgrStateChange;
    ClientWargameCheckEntry::s_handler = OnWargameCheckEntry;
    ClientPvpLogData::s_handler = OnPvpLogData;
    ClientBattlefieldRatedInfo::s_handler = OnBattlefieldRatedInfo;
    ClientRatedBgStats::s_handler = OnRatedBgStats;
    ClientRequestPvpRewardsResponse::s_handler = OnRequestPvpRewardsResponse;
    ClientPvpOptionsEnabled::s_handler = OnPvpOptionsEnabled;
    ClientServices::SetMessageHandler(SMSG_ARENA_OPPONENT_UPDATE, Packet_SMSG_ARENA_OPPONENT_UPDATE, NULL);
    CGBattlefieldInfo::m_activeBattlefieldID = 0;
    CGBattlefieldInfo::m_activeMapID = 0;
    CGBattlefieldInfo::m_battlemaster.m_guid = 0;
    CGBattlefieldInfo::m_instanceExpiration = 0;
    CGBattlefieldInfo::m_instanceStartTime = 0;
    CGBattlefieldInfo::m_nameCachePending = 0;
    CGBattlefieldInfo::m_numScoreEntries = 0;
    CGBattlefieldInfo::m_numScores = 0;
    CGBattlefieldInfo::m_logFilterFaction = -1;
    CGBattlefieldInfo::m_numVehicles = 0;
    CGBattlefieldInfo::m_allowJoin = 1;
    BattlefieldStatusNode* node = CGBattlefieldInfo::m_statusList.Head();

    while (node) {
        BattlefieldStatusNode* next = CGBattlefieldInfo::m_statusList.Next(node);
        DeleteBattlefieldStatus(node);
        node = next;
    }

    CGBattlefieldInfo::m_maxBattlefieldID = 0;

    for (uint32_t i = 0; i < 2; i++) {
        CGBattlefieldInfo::m_worldPvPQueues[i].m_status = 0;
        CGBattlefieldInfo::m_worldPvPQueues[i].m_areaID = 0;
        CGBattlefieldInfo::m_worldPvPQueues[i].m_queueID = 0;
    }

    for (uint32_t i = 0; i < 80; i++) {
        CGBattlefieldInfo::m_scores[i] = static_cast<BattlefieldScore*>(SMemAlloc(sizeof(BattlefieldScore), __FILE__, __LINE__, 0));
    }

    for (uint32_t i = 0; i < 22; i++) {
        CGBattlefieldInfo::m_scoreSortCriteria[i].m_column = i;
        CGBattlefieldInfo::m_scoreSortCriteria[i].m_descending = 0;
    }

    for (int32_t i = 0; i < g_battlemasterListDB.m_numRecords; i++) {
        BattlemasterListRec* rec = g_battlemasterListDB.GetRecordByIndex(i);

        if (rec && rec->m_instanceType == 3 && !(rec->m_flags & 0x2)) {
            *CGBattlefieldInfo::m_battlegroundTypes.New() = rec->m_id;
        }
    }

    qsort(CGBattlefieldInfo::m_battlegroundTypes.m_data, CGBattlefieldInfo::m_battlegroundTypes.Count(), sizeof(int32_t), QSortBattlefields);
    CGBattlefieldInfo::m_collapsedWarGameHeaders = 0;
    CGBattlefieldInfo::m_numWarGameTypes = 0;
    CGBattlefieldInfo::m_selectedWarGameType = -1;
    CGBattlefieldInfo::m_pvpRewardTotal = 0;
    CGBattlefieldInfo::m_pvpRewards[0] = 0;
    CGBattlefieldInfo::m_pvpRewards[1] = 0;
    CGBattlefieldInfo::m_pvpRewards[2] = 0;
    CGBattlefieldInfo::m_pvpRewards[3] = 0;
    CGBattlefieldInfo::m_pvpRewards[4] = 0;
    memset(CGBattlefieldInfo::m_arenaOpponentPets, 0, sizeof(CGBattlefieldInfo::m_arenaOpponentPets));
}

// OFFSET: 0x00944050
void CGBattlefieldInfo::AddArenaOpponent(const WOWGUID& guid) {
    for (uint32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponentPets[i][1] == guid) {
            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponentPets[i][0], TYPEMASK_UNIT, __FILE__, __LINE__));

            if (pet) {
                pet->UnregisterScript(0x4000);
            }

            CGBattlefieldInfo::AddArenaOpponentToSlot(guid, i);
            return;
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (!CGBattlefieldInfo::m_arenaOpponentPets[i][0]) {
            WOWGUID opponent = CGBattlefieldInfo::m_arenaOpponents[i];

            if (opponent == guid || !opponent) {
                CGBattlefieldInfo::AddArenaOpponentToSlot(guid, i);
                return;
            }
        }
    }
}

// OFFSET: 0x0044BC10/0x140059270
uint32_t GetArenaPetGUIDCount() {
    uint32_t count = 0;

    do {
        if (!CGBattlefieldInfo::m_arenaOpponents[count] && !CGBattlefieldInfo::m_arenaOpponentPets[count][0]) {
            break;
        }

        count++;
    } while (count < 5);

    return count;
}

// OFFSET: 0x00554BE0
bool IsOnTransport(CGObject_C* object) {
    return object->GetTransportGUID() != 0;
}

// OFFSET: 0x008C1430
uint16_t CGBattlefieldInfo::GetActiveBattlefieldTypeID() {
    BattlefieldStatusNode* node = CGBattlefieldInfo::FindStatus(CGBattlefieldInfo::m_activeBattlefieldID);

    if (node) {
        return static_cast<uint16_t>(node->m_queueID);
    }

    return 0;
}

// OFFSET: 0x0044BBE0
WOWGUID CGBattlefieldInfo::GetArenaOpponent(uint32_t index) {
    if (index < 5) {
        return CGBattlefieldInfo::m_arenaOpponents[index];
    }

    return WOWGUID::Make(0);
}

// OFFSET: #0x006CF670
int32_t CGBattlefieldInfo::GetArenaOpponentIndex(const WOWGUID& guid, int32_t* index) {
    for (int32_t i = 0; i < 5; i++) {
        if (CGBattlefieldInfo::m_arenaOpponents[i] == guid) {
            *index = i;
            return 1;
        }
    }

    return 0;
}
