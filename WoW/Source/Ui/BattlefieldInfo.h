#ifndef WOW_SOURCE_UI_BATTLEFIELDINFO_H
#define WOW_SOURCE_UI_BATTLEFIELDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"

class BattlemasterListRec;
class PartyMemberState;
class PartyMemberPetState;
class CGObject_C;
class CDataStore;
struct lua_State;

struct BattlefieldScore {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_killingBlows;
    /* 000C */ uint32_t m_honorableKills;
    /* 0010 */ uint32_t m_deaths;
    /* 0014 */ uint32_t m_bonusHonor;
    /* 0018 */ uint32_t m_damageDone;
    /* 001C */ uint32_t m_healingDone;
    /* 0020 */ int32_t m_faction;
    /* 0024 */ uint32_t m_rating;
    /* 0028 */ int32_t m_ratingChange;
    /* 002C */ uint32_t m_preMatchMMR;
    /* 0030 */ int32_t m_mmrChange;
    /* 0034 */ int32_t m_stats[8];
    /* 0054 */ uint32_t m_talentSpec;
};

struct BattlefieldScoreSortCriteria {
    /* 0000 */ int32_t m_column;
    /* 0004 */ int32_t m_descending;
};

struct WorldPvPQueueStatus {
    /* 0000 */ uint64_t m_queueID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_status;
    /* 0010 */ int32_t m_expireTime;
    /* 0014 */ int32_t unk_0014;
};

struct PersonalRatedInfo {
    /* 0000 */ uint32_t m_seasonWon;
    /* 0004 */ uint32_t m_seasonPlayed;
    /* 0008 */ uint32_t m_weeklyPlayed;
    /* 000C */ uint32_t m_weeklyWon;
    /* 0010 */ uint32_t m_weeklyCap;
    /* 0014 */ uint32_t m_rating;
};

struct WarGameType {
    /* 0000 */ int32_t m_bgTypeID;
    /* 0004 */ int32_t m_instanceType;
    /* 0008 */ bool m_collapsed;
};

struct BattlefieldStatusNode : public TSLinkedNode<BattlefieldStatusNode> {
    /* 0008 */ uint32_t m_id;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ JamClientRideTicket m_ticket;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint64_t m_queueID;
    /* 0030 */ uint32_t m_mapID;
    /* 0034 */ uint32_t m_status;
    /* 0038 */ uint32_t m_minLevel;
    /* 003C */ uint32_t m_maxLevel;
    /* 0040 */ uint32_t m_instanceID;
    /* 0044 */ uint32_t m_portExpiration;
    /* 0048 */ uint32_t m_estimatedWaitTime;
    /* 004C */ uint32_t m_queueStartTime;
    /* 0050 */ uint32_t m_teamSize;
    /* 0054 */ int32_t m_registeredMatch;
    /* 0058 */ int32_t m_eligibleInQueue;
    /* 005C */ int32_t m_waitingOnOtherActivity;
    /* 0060 */ uint32_t m_waitTeamReady[2];
    /* 0068 */ uint32_t m_waitTeamSize[2];

    BattlefieldStatusNode();
};

int32_t Packet_SMSG_ARENA_OPPONENT_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void WorldMapNameUpdateCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void BattlegroundPlayerJoinedNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void BattlegroundPlayerLeftNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t QSortBattlefields(const void* a, const void* b);
int32_t QSortScoreData(const void* a, const void* b);
int32_t QSortWarGameTypes(const void* a, const void* b);
void BattlefieldNameCacheCallback();
void ScoreNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool IsOnTransport(CGObject_C* object);
bool IsCurrentMapArena();
uint32_t GetArenaPetGUIDCount();
bool OnBattlefieldList(ClientBattlefieldList* msg);
bool OnBattlefieldStatus(ClientBattlefieldStatus* msg);
bool OnBattlefieldStatusNeedConfirmation(ClientBattlefieldStatusNeedconfirmation* msg);
bool OnBattlefieldStatusActive(ClientBattlefieldStatusActive* msg);
bool OnBattlefieldStatusQueued(ClientBattlefieldStatusQueued* msg);
bool OnBattlefieldStatusFailed(ClientBattlefieldStatusFailed* msg);
bool OnBattlefieldStatusWaitForGroups(ClientBattlefieldStatusWaitforgroups* msg);
bool OnBattlefieldPlayerPositions(ClientBattlefieldPlayerPositions* msg);
bool OnBattlegroundPlayerLeft(ClientBattlegroundPlayerLeft* msg);
bool OnBattlegroundPlayerJoined(ClientBattlegroundPlayerJoined* msg);
bool OnWargameRequestSent(ClientWargameRequestSent* msg);
bool OnBattlefieldMgrEntered(ClientBattlefieldMgrEntered* msg);
bool OnBattlefieldPortDenied(ClientBattlefieldPortDenied* msg);
bool OnBattlefieldMgrQueueRequestResponse(ClientBattlefieldMgrQueueRequestResponse* msg);
bool OnBattlefieldMgrEjected(ClientBattlefieldMgrEjected* msg);
bool OnBattlefieldMgrEjectPending(ClientBattlefieldMgrEjectPending* msg);
bool OnBattlefieldMgrQueueInvite(ClientBattlefieldMgrQueueInvite* msg);
bool OnBattlefieldMgrEntryInvite(ClientBattlefieldMgrEntryInvite* msg);
bool OnBattlefieldMgrStateChange(ClientBattlefieldMgrStateChange* msg);
bool OnWargameCheckEntry(ClientWargameCheckEntry* msg);
bool OnPvpLogData(ClientPvpLogData* msg);
bool OnBattlefieldRatedInfo(ClientBattlefieldRatedInfo* msg);
bool OnRatedBgStats(ClientRatedBgStats* msg);
bool OnRequestPvpRewardsResponse(ClientRequestPvpRewardsResponse* msg);
bool OnPvpOptionsEnabled(ClientPvpOptionsEnabled* msg);

class CGBattlefieldInfo {
    public:
    static int32_t m_ratedBgStats[4];
    static PersonalRatedInfo m_personalRatedInfo[4];
    static WOWGUID m_arenaOpponentPets[5][2];
    static WOWGUID m_arenaOpponents[5];
    static WOWGUID m_vehicleGuids[40];
    static int32_t m_teamPlayerCounts[2];
    static char m_teamNames[2][96];
    static WOWGUID m_flagCarriers[2];
    static WOWGUID m_playerPositionGuids[40];
    static BattlefieldScoreSortCriteria m_scoreSortCriteria[22];
    static int32_t m_worldStateStats[8];
    static BattlefieldScore* m_scores[80];
    static WorldPvPQueueStatus m_worldPvPQueues[2];
    static uint32_t m_listUnk;
    static int32_t m_bgTypeID;
    static uint32_t m_minLevel;
    static uint32_t m_maxLevel;
    static uint8_t m_hasHolidayWinToday;
    static uint8_t m_hasRandomWinToday;
    static uint8_t m_arenaFaction;
    static int32_t m_holidayLossHonor;
    static int32_t m_holidayWinHonor;
    static int32_t m_holidayWinArena;
    static int32_t m_randomLossHonor;
    static int32_t m_randomWinHonor;
    static int32_t m_randomWinArena;
    static uint32_t m_collapsedWarGameHeaders;
    static int32_t m_numWarGameTypes;
    static uint32_t m_maxBattlefieldID;
    static WOWGUID m_battlemaster;
    static uint32_t m_activeBattlefieldID;
    static uint32_t m_activeMapID;
    static int32_t m_instanceType;
    static uint32_t m_instanceExpiration;
    static uint32_t m_instanceStartTime;
    static uint32_t m_numScoreEntries;
    static uint32_t m_numScores;
    static uint32_t m_nextLogUpdate;
    static int32_t m_mapDone;
    static uint32_t m_winner;
    static int32_t m_nameCachePending;
    static uint32_t m_teamRatings[2];
    static uint32_t m_newTeamRatings[2];
    static uint32_t m_avgMatchmakingRating[2];
    static uint32_t m_numWorldStateStats;
    static uint32_t m_numPlayerPositions;
    static uint32_t m_positionUpdateTime;
    static uint32_t m_numVehicles;
    static int32_t m_pvpRewardTotal;
    static WOWGUID m_warGameRequester;
    static uint64_t m_warGameQueueID;
    static int32_t m_pvpRewards[5];
    static TSGrowableArray<int32_t> m_instances;
    static TSGrowableArray<WarGameType*> m_warGameTypes;
    static C2Vector m_playerPositions[40];
    static C2Vector m_flagPositions[2];
    static PartyMemberState m_arenaOpponentStats[5];
    static TSGrowableArray<int32_t> m_battlegroundTypes;
    static int32_t m_selectedWarGameType;
    static int32_t m_logFilterFaction;
    static int32_t m_allowJoin;
    static TSList<BattlefieldStatusNode, TSGetLink<BattlefieldStatusNode> > m_statusList;
    static int32_t m_arenaOpponentSeen[5];

    static WorldPvPQueueStatus* FindWorldPvPQueue(uint64_t queueID);
    static WorldPvPQueueStatus* FindFreeWorldPvPQueue();
    static uint32_t GetNumFlagCarriers();
    static WOWGUID GetFlagCarrier(uint32_t index);
    static void SetBattlemaster(WOWGUID battlemaster);
    static void RequestScoreData();
    static BattlefieldScore* FindScore(const WOWGUID& guid);
    static void RemoveVehicle(const WOWGUID& guid);
    static PartyMemberState* GetRemoteStats(const WOWGUID& guid);
    static PartyMemberState* GetRemoteStatsFromPet(const WOWGUID& petGuid);
    static PartyMemberPetState* GetRemotePetStats(const WOWGUID& guid);
    static int32_t PushPVPRewards(lua_State* L);
    static int32_t GetPlayerPosition(uint32_t index, C3Vector* position);
    static int32_t GetFlagPosition(uint32_t index, C3Vector* position);
    static int32_t GetBattlegroundType(int32_t index);
    static void JoinBattlefield(int32_t asGroup);
    static void AddVehicle(const WOWGUID& guid);
    static void OnArenaUnitOutOfRange(const WOWGUID& guid);
    static void OnArenaUnitDestroyed(const WOWGUID& guid);
    static WOWGUID GetArenaOpponentPet(uint32_t index);
    static void AddArenaOpponentPetToSlot(const WOWGUID& petGuid, const WOWGUID& ownerGuid, int32_t slot, int32_t registerNew);
    static bool IsWarGameHeaderCollapsed(uint32_t index);
    static bool GetBracketForLevel(uint32_t level, const BattlemasterListRec* rec, int32_t* bracket);
    static void UpdateWarGameTypeList();
    static const char* GetBattlefieldMgrZoneName(uint64_t queueID);
    static void SetScoreSortColumn(int32_t column);
    static const char* GetFlagToken(uint32_t index);
    static void AddArenaOpponentPet(const WOWGUID& petGuid, const WOWGUID& ownerGuid);
    static void BattlefieldMgrEntryInviteResponse(uint16_t battleID, int32_t a2, int32_t accept);
    static void BattlefieldMgrQueueRequest(uint16_t battleID);
    static void BattlefieldMgrQueueInviteResponse(uint16_t battleID, int32_t a2, int32_t accept);
    static void BattlefieldMgrExitRequest(uint16_t battleID);
    static BattlefieldStatusNode* FindStatus(const JamClientRideTicket& ticket);
    static BattlefieldStatusNode* FindStatus(uint32_t id);
    static void RequestBattlegroundInstanceInfo(int32_t index);
    static void ClearWarGameTypes();
    static void BuildWarGameTypeList();
    static uint64_t GetActiveQueueID();
    static void RequestPlayerPositions();
    static BattlefieldStatusNode* UpdateStatus(const JamClientBattlefieldStatusHeader& header);
    static bool IsRatedBattleground();
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static void RemoveStatus(const JamClientRideTicket& ticket);
    static void AddArenaOpponentToSlot(const WOWGUID& guid, int32_t slot);
    static void InitializeGame();
    static void AddArenaOpponent(const WOWGUID& guid);
    static uint16_t GetActiveBattlefieldTypeID();
    static WOWGUID GetArenaOpponent(uint32_t index);
    static int32_t GetArenaOpponentIndex(const WOWGUID& guid, int32_t* index);
};

int32_t Script_CanHearthAndResurrectFromArea(lua_State* L);
int32_t Script_RequestBattlegroundInstanceInfo(lua_State* L);
void BattlefieldInfoRegisterScriptFunctions();
void BattlefieldInfoUnregisterScriptFunctions();

#endif
