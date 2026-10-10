#ifndef WOW_SOURCE_UI_QUESTLOG_H
#define WOW_SOURCE_UI_QUESTLOG_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CGItem_C;
class CGPlayer_C;
class CDataStore;
class CRect;
class C2Vector;

struct lua_State;

class QuestLogLeaderBoardParams;

struct QuestLogEntry {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ int32_t m_logSlot;
    /* 0008 */ int32_t m_isHeader;
    /* 000C */ int32_t m_expired;
};

struct BLOBPOI {
    /* 0000 */ int32_t m_x;
    /* 0004 */ int32_t m_y;
};

struct BLOB {
    /* 0000 */ TSGrowableArray<BLOBPOI> m_points;
    /* 0010 */ BLOBPOI m_center;
    /* 0018 */ int32_t m_id;
    /* 001C */ int32_t m_objectiveIndex;
    /* 0020 */ int32_t m_mapId;
    /* 0024 */ int32_t m_worldMapAreaID;
    /* 0028 */ int32_t m_floor;
    /* 002C */ int32_t m_flags;
    /* 0030 */ int32_t m_priority;
};

struct QuestPOIInfo {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ TSGrowableArray<BLOB> m_blobs;
    /* 0014 */ int32_t m_iconMapId;
    /* 0018 */ int32_t m_iconObjectiveIndex;
    /* 001C */ int32_t m_iconFloor;
    /* 0020 */ int32_t m_iconX;
    /* 0024 */ int32_t m_iconY;
    /* 0028 */ uint8_t m_visible;
    /* 0029 */ uint8_t m_hasIcon;
    /* 002C */ uint32_t m_objectiveMask;
};

struct QuestWatch {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ int32_t m_expireTime;
    /* 0008 */ float m_distance;
};

struct AutoQuestPopUp {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ int32_t m_type;
    /* 0008 */ uint8_t m_shown;
};

void QuestQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void ItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void ObjectQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void QuestCreatureQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success);
void AddQuestPointDisplay();
int32_t GetQuestSuggestedPlayers(int32_t index);
int32_t GetQuestSortId(uint32_t index);
const char* GetQuestTag(int32_t index);
int32_t GetQuestLevel(int32_t questId);
uint32_t IsDailyQuest(int32_t index);
uint32_t IsQuestStartEvent(uint32_t index);
int32_t CalcQuestExp(int32_t questId, int32_t level);
int32_t CountQuestItemDropsNeeded(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);

class CGQuestLog {
    public:
    struct QUESTCOMPLETIONNPC : public TSHashObject<QUESTCOMPLETIONNPC, HASHKEY_NONE> {
        /* 0018 */ TSGrowableArray<int32_t> m_npcs;
    };

    static QuestLogEntry m_quests[100];
    static int32_t m_sortTypes[50];
    static AutoQuestPopUp m_autoQuestPopUps[50];
    static QuestWatch m_questWatches[25];
    static uint32_t m_numQuest;
    static uint32_t m_numSortTypes;
    static int32_t m_selectedQuest;
    static int32_t m_superTrackedQuest;
    static int32_t m_abandonQuest;
    static uint32_t m_numAutoQuestPopUps;
    static uint32_t m_numQuestWatches;
    static uint32_t m_numShownQuests;
    static int32_t m_timeOffset;
    static int32_t m_questResetTime;
    static uint8_t m_trackedQuestsLoaded;
    static int32_t m_nextQuestUpdate;
    static QuestPOIInfo m_questPOIs[50];
    static float m_questIconOverlapDist;
    static float m_questIconOverlapPushDist;
    static TSHashTable<QUESTCOMPLETIONNPC, HASHKEY_NONE> s_questCompletionNPCs;

    static void InitializeGame();
    static void ShutdownGame();
    static void EnterWorld();
    static void LeaveWorld();
    static void Update(int32_t rebuild);
    static void UpdateWatchTimers();
    static void SetServerTime(int32_t serverTime, int32_t resetTime);
    static void SetSelectedQuest(int32_t index);
    static int32_t GetSelectedLogEntry();
    static int32_t IsSelectedQuestExpired();
    static void FilterAndSortQuests();
    static void CollapseHeader(uint32_t index, int32_t collapse);
    static void AbandonSelectedQuest();
    static void ClearQuest(int32_t questId);
    static int32_t GetQuestLogSlot(int32_t index);
    static int32_t GetQuestID(int32_t index);
    static int32_t GetQuestIndex(int32_t questId);
    static int32_t GetQuestItemID(const char* type, int32_t index);
    static const char* GetQuestName(int32_t index);
    static const char* GetQuestNameByID(int32_t questId);
    static CGItem_C* GetQuestSpecialItem(uint32_t index);
    static int32_t GetQuestCriteriaSpell(int32_t questId);
    static int32_t GetQuestGreenRange();
    static int32_t IsOnQuest(int32_t questId);
    static int32_t IsOnQuest(const CGPlayer_C* player, int32_t questId);
    static int32_t IsQuestComplete(uint32_t index);
    static int32_t IsQuestComplete(const CGPlayer_C* player, uint32_t index);
    static int32_t IsQuestCompleteEx(const CGPlayer_C* player, uint32_t index, bool requireObjectives, int32_t itemId, int32_t itemCount);
    static int32_t IsQuestFailed(uint32_t index);
    static int32_t IsQuestExpiredByID(int32_t questId);
    static int32_t SetQuestExpired(uint32_t index);
    static int32_t SetQuestExpiredByID(int32_t questId);
    static bool HasNoObjectives(uint32_t index);
    static bool HasAutoCompleteBeenTried(uint32_t index);
    static void UpdateQuestItem(int32_t itemId);
    static void UpdateQuestSpell(int32_t spellId);
    static void UpdateQuestCurrency(int32_t currencyId);
    static int32_t GetNumQuestItems(const CGPlayer_C* player, int32_t questId, int32_t itemId);
    static int32_t GetNumQuestLeaderBoards(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);
    static int32_t GetQuestLeaderBoardByID(int32_t questId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLogItemDropByID(int32_t questId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestPOILeaderBoardByID(int32_t questId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLeaderBoardForKill(int32_t questId, int32_t objectId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLeaderBoardForKill(const CGPlayer_C* player, int32_t questId, int32_t objectId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLeaderBoardForItem(int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLeaderBoardForItem(const CGPlayer_C* player, int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params);
    static void GetAllOfGroupsQuests(int32_t (&quests)[250]);
    static void LoadTrackedQuests(bool force);
    static void SaveTrackedQuests();
    static bool AddQuestWatch(int32_t questId, uint32_t duration);
    static void RemoveQuestWatch(int32_t questId);
    static void ShiftQuestWatches(uint32_t from, uint32_t to);
    static bool SortQuestWatches();
    static float GetQuestDistSqr(int32_t questId, C2Vector& position);
    static void LoadAutoQuestPopUps();
    static void SaveAutoQuestPopUps();
    static AutoQuestPopUp* FindAutoQuestPopUp(int32_t questId);
    static bool AddAutoQuestPopUp(int32_t questId, int32_t type);
    static void RemoveAutoQuestPopUp(int32_t questId);
    static void AddQuestCompletionNPC(int32_t questId, int32_t npcId);
    static int32_t GetQuestNumCompletionNPC(int32_t questId);
    static int32_t GetQuestCompletionNPC(int32_t questId, int32_t index);
    static void OnQuestPOIResponse(CDataStore* msg);
    static void UpdateQuestPOIFromUserServer();
    static bool HasQuestPOI(int32_t questId);
    static int32_t GetQuestPOIIndex(int32_t questId);
    static int32_t FindEmptyQuestPOISlot(int32_t questId);
    static QuestPOIInfo* GetQuestPOIData(int32_t questId);
    static BLOB* GetQuestPOIBlob(int32_t questId, uint32_t blobIndex);
    static bool GetQuestPOILoc(int32_t questId, bool& complete, int32_t& mapId, int32_t& floor, int32_t& x, int32_t& y, int32_t& objectiveIndex);
    static uint32_t ComputeBlobQuestObjectives(int32_t questId);
    static int32_t GetQuestWorldMapAreaID(int32_t questId, int32_t& floor);
    static bool IsPOIVisibleOnWorldMap(const BLOBPOI& point, int32_t mapId, int32_t currentMapId);
    static bool IsPOIVisibleOnDungeonMap(const BLOBPOI& point, int32_t mapId, int32_t floor, int32_t dungeonMapId);
    static void GetBlobBounds(const TSGrowableArray<BLOBPOI>& points, CRect& bounds);
    static bool QuestPOIBlobIntersectsRect(int32_t questId, int32_t blobIndex, const CRect& rect);
    static uint32_t GetQuestPOIBlobsOnContinent(int32_t questId, TSGrowableArray<uint32_t>& blobs);
    static void MergeOverlappingBlobs(int32_t questId, TSGrowableArray<uint32_t>& blobs, TSGrowableArray<uint32_t>& merged);
    static int32_t UpdateAllQuestPOI();
    static void UpdatePOIIcons();
    static int32_t GetPOIQuestIDByVisibleIndex(int32_t index);
};

void QuestLogRegisterScriptFunctions();
void QuestLogUnregisterScriptFunctions();
int32_t Packet_SMSG_QUERY_TIME_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t UpdateQuestPOIHandler(const char* command, const char* arguments);

#endif
