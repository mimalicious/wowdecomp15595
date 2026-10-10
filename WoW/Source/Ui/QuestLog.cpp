#include "QuestLog.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <ctime>
#include <cstdlib>
#include <malloc.h>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/QuestInfoRec.h"
#include "WoW/Source/DB/Rec/QuestSortRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/QuestXPRec.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/QuestLogLeaderBoardParams.h"
#include "WoW/Source/Ui/WorldMap.h"
#include <cmath>
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/NTempest.h"
#include <typeinfo>

// STATIC: 0x00F457A8
QuestLogEntry CGQuestLog::m_quests[100];

// STATIC: 0x00F45358
int32_t CGQuestLog::m_sortTypes[50];

// STATIC: 0x00F45420
AutoQuestPopUp CGQuestLog::m_autoQuestPopUps[50];

// STATIC: 0x00F45678
QuestWatch CGQuestLog::m_questWatches[25];

// STATIC: 0x00F45DE8
uint32_t CGQuestLog::m_numQuest;

// STATIC: 0x00F45DEC
uint32_t CGQuestLog::m_numSortTypes;

// STATIC: 0x00F45DF0
int32_t CGQuestLog::m_selectedQuest;

// STATIC: 0x00F45DF4
int32_t CGQuestLog::m_superTrackedQuest;

// STATIC: 0x00F45DF8
int32_t CGQuestLog::m_abandonQuest;

// STATIC: 0x00F45DFC
uint32_t CGQuestLog::m_numAutoQuestPopUps;

// STATIC: 0x00F45E00
uint32_t CGQuestLog::m_numQuestWatches;

// STATIC: 0x00F45E04
uint32_t CGQuestLog::m_numShownQuests;

// STATIC: 0x00F45E08
int32_t CGQuestLog::m_timeOffset;

// STATIC: 0x00F45E0C
int32_t CGQuestLog::m_questResetTime;

// STATIC: 0x00F45E10
uint8_t CGQuestLog::m_trackedQuestsLoaded;

// STATIC: 0x00F45E14
static uint32_t s_questCallbackCount;

// STATIC: 0x00F45E18
int32_t CGQuestLog::m_nextQuestUpdate;

// STATIC: 0x00F45E1C
static CVar* s_cvPOIShiftComplete;

// STATIC: 0x00F45E20
QuestPOIInfo CGQuestLog::m_questPOIs[50];

// STATIC: 0x00F46780
TSHashTable<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE> CGQuestLog::s_questCompletionNPCs;

// STATIC: 0x00D12E80
float CGQuestLog::m_questIconOverlapDist = 12.0f;

// STATIC: 0x00D12E84
float CGQuestLog::m_questIconOverlapPushDist = 18.0f;

// STATIC: 0x00D13184
static int32_t s_poiOverlapIterations = 20;

// STATIC: 0x00BFD938
static const int32_t s_questGreenRange[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

// PROVEN: 11c5bb76 IDENTICAL/NONE
// OFFSET: 0x0095BF90
int32_t CGQuestLog::GetQuestLogSlot(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return -1;
    }

    return CGQuestLog::m_quests[index].m_logSlot;
}

// OFFSET: 0x0095BFC0
static void QuestSelectQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        AddQuestPointDisplay();
    } else {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
    }
}

// OFFSET: 0x0095BFE0/0x1406CED30
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(id, none, NULL, NULL, false);

    if (quest) {
        CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
    }
}

// OFFSET: 0x0095C040/0x1406CED90
void QuestCreatureQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    } else {
        ConsoleWrite("Invalid creature in quest", DEFAULT_COLOR);
    }
}

// OFFSET: 0x0095C070/0x1406CEDB0
void ObjectQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    } else {
        ConsoleWrite("Invalid object in quest", DEFAULT_COLOR);
    }
}

// PROVEN: 7ce68c36 IDENTICAL/NONE
// OFFSET: 0x0095C0A0/0x1406CEDD0
void ItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    }
}

// OFFSET: 0x0095C0C0/0x1406CEDF0
void CGQuestLog::RemoveQuestWatch(int32_t questId) {
    uint32_t count = CGQuestLog::m_numQuestWatches;
    uint32_t index = 0;

    for (; index < count; index++) {
        if (CGQuestLog::m_questWatches[index].m_questId == questId) {
            break;
        }
    }

    if (index >= count) {
        return;
    }

    for (; index < count - 1; index++) {
        CGQuestLog::m_questWatches[index].m_questId = CGQuestLog::m_questWatches[index + 1].m_questId;
        CGQuestLog::m_questWatches[index].m_expireTime = CGQuestLog::m_questWatches[index + 1].m_expireTime;
    }

    CGQuestLog::m_questWatches[count - 1].m_questId = 0;
    CGQuestLog::m_numQuestWatches = count - 1;
    CGQuestLog::m_questWatches[count - 1].m_expireTime = 0;
}

// PROVEN: 6b6ba060 IDENTICAL/NONE
// OFFSET: 0x0095C140
AutoQuestPopUp* CGQuestLog::FindAutoQuestPopUp(int32_t questId) {
    for (uint32_t i = 0; i < CGQuestLog::m_numAutoQuestPopUps; i++) {
        if (CGQuestLog::m_autoQuestPopUps[i].m_questId == questId) {
            return &CGQuestLog::m_autoQuestPopUps[i];
        }
    }

    return NULL;
}

// OFFSET: 0x0095C180/0x1406CEEB0
int32_t GetQuestSortId(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest) {
        return -1;
    }

    int32_t sortId = CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::m_quests[index].m_isHeader) {
        WOWGUID none = WOWGUID::Make(0);
        QuestCache* quest = g_questDBCache.GetRecord(sortId, none, NULL, NULL, false);

        if (!quest) {
            return -1;
        }

        sortId = quest->m_areaIdOrSortId;
    }

    for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
        if (CGQuestLog::m_sortTypes[i] == sortId) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x0095C200
void AddQuestPointDisplay() {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, QuestSelectQueryCallback, NULL, false);

    if (quest) {
        DisplayQuestPoint(quest->m_pointX, quest->m_pointY, quest->m_pointOptional, quest->m_name);
    }
}

// PROVEN: 3f2dd7d0 IDENTICAL/NONE
// OFFSET: 0x0095C270
int32_t CGQuestLog::GetSelectedLogEntry() {
    uint32_t index = 0;

    for (; index < CGQuestLog::m_numQuest; index++) {
        if (!CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_questId == CGQuestLog::m_selectedQuest) {
            break;
        }
    }

    if (index == CGQuestLog::m_numQuest) {
        return -1;
    }

    return index;
}

// PROVEN: b1c7e4d0 IDENTICAL/NONE
// OFFSET: 0x0095C2B0
void CGQuestLog::ClearQuest(int32_t questId) {
    if (questId && questId == CGQuestLog::m_selectedQuest) {
        DisplayQuestPoint(0.0f, 0.0f, 0, "");
    }
}

// PROVEN: 9df14ef6 IDENTICAL/NONE
// OFFSET: 0x0095C2E0/0x1406CF030
int32_t CGQuestLog::GetQuestID(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    return CGQuestLog::m_quests[index].m_questId;
}

// OFFSET: 0x0095C310/0x1406CF060
int32_t CGQuestLog::GetQuestItemID(const char* type, int32_t index) {
    if (!type || !*type || index < 0) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (!quest) {
        return 0;
    }

    if (!_stricmp(type, "reward")) {
        if (static_cast<uint32_t>(index) < 4) {
            return quest->m_rewardItem[index];
        }

        return 0;
    }

    if (_stricmp(type, "choice") || static_cast<uint32_t>(index) >= 6) {
        return 0;
    }

    return quest->m_rewardChoiceItem[index];
}

// PROVEN: a6fdd2f6 IDENTICAL/NONE
// OFFSET: 0x0095C3B0/0x1406CF140
int32_t CGQuestLog::IsQuestFailed(uint32_t index) {
    return index < CGQuestLog::m_numQuest && !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired;
}

// OFFSET: 0x0095C3E0
int32_t CGQuestLog::IsQuestExpiredByID(int32_t questId) {
    uint32_t index = 0;

    for (; index < CGQuestLog::m_numQuest; index++) {
        if (CGQuestLog::m_quests[index].m_questId == questId) {
            break;
        }
    }

    if (index >= CGQuestLog::m_numQuest) {
        return 0;
    }

    return !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired;
}

// OFFSET: 0x0095C430/0x1406CF1C0
uint32_t IsQuestStartEvent(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, NULL, NULL, false);
    return quest && (quest->m_flags & 0x2);
}

// OFFSET: 0x0095C490/0x1406CF230
int32_t CGQuestLog::GetQuestIndex(int32_t questId) {
    for (int32_t i = 0; i < 100; i++) {
        if (!CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId == questId) {
            return i + 1;
        }
    }

    return 0;
}

// OFFSET: 0x0095C4C0
bool CGQuestLog::GetQuestPOILoc(int32_t questId, bool& complete, int32_t& mapId, int32_t& floor, int32_t& x, int32_t& y, int32_t& objectiveIndex) {
    uint32_t index = 0;

    for (; index < 50; index++) {
        if (CGQuestLog::m_questPOIs[index].m_questId == questId) {
            break;
        }
    }

    if (index >= 50) {
        return false;
    }

    QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];
    complete = poi->m_objectiveMask == 0xFFFFFFFF;
    mapId = poi->m_iconMapId;
    floor = poi->m_iconFloor;
    x = poi->m_iconX;
    y = poi->m_iconY;
    objectiveIndex = poi->m_iconObjectiveIndex;
    return true;
}

// OFFSET: 0x0095C540
bool CGQuestLog::IsPOIVisibleOnWorldMap(const BLOBPOI& point, int32_t mapId, int32_t currentMapId) {
    if (mapId != currentMapId) {
        return false;
    }

    C3Vector position(static_cast<float>(point.m_x), static_cast<float>(point.m_y), 0.0f);
    float x;
    float y;
    bool outland;
    if (!CGWorldMap::GetWorldPosition(position, currentMapId, &x, &y, &outland, false, CGWorldMap::GetCurrentDungeonMap())) {
        return false;
    }

    return !outland && fabs(fabs(x * y)) >= 0.001;
}

// PROVEN: 84c84734 IDENTICAL/NONE
// OFFSET: 0x0095C5D0/0x1406CF390
void CGQuestLog::ShiftQuestWatches(uint32_t from, uint32_t to) {
    int32_t step;

    if (to > from && to < CGQuestLog::m_numQuestWatches) {
        step = 1;
    } else if (to < from && from < CGQuestLog::m_numQuestWatches) {
        step = -1;
    } else {
        return;
    }

    QuestWatch moved = CGQuestLog::m_questWatches[from];

    for (uint32_t i = from; i != to; i += step) {
        CGQuestLog::m_questWatches[i] = CGQuestLog::m_questWatches[i + step];
    }

    CGQuestLog::m_questWatches[to] = moved;
}

// OFFSET: 0x0095D7F0
static int32_t QSortQuestSortTypes(const void* a, const void* b) {
    int32_t sortA = *static_cast<const int32_t*>(a);
    int32_t sortB = *static_cast<const int32_t*>(b);

    if (sortA == sortB) {
        return 0;
    }

    if (!sortA) {
        return -1;
    }

    if (!sortB) {
        return 1;
    }

    char nameA[256];
    char nameB[256];
    memset(nameA, 0, sizeof(nameA));
    memset(nameB, 0, sizeof(nameB));

    if (sortA >= 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(sortA);

        if (area) {
            SStrCopy(nameA, area->m_areaName, 256);
        }
    } else {
        QuestSortRec* sort = g_questSortDB.GetRecord(-sortA);

        if (sort) {
            SStrCopy(nameA, sort->m_sortName, 256);
        }
    }

    if (sortB >= 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(sortB);

        if (area) {
            SStrCopy(nameB, area->m_areaName, 256);
        }
    } else {
        QuestSortRec* sort = g_questSortDB.GetRecord(-sortB);

        if (sort) {
            SStrCopy(nameB, sort->m_sortName, 256);
        }
    }

    return SStrSortUTF8I(nameA, nameB, 0x7FFFFFFF);
}

// PROVEN: 6154cc11 IDENTICAL/NONE
// OFFSET: 0x0095D960
void CGQuestLog::UpdateWatchTimers() {
    for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
        if (CGQuestLog::m_questWatches[i].m_expireTime > 0 && _time32(NULL) >= CGQuestLog::m_questWatches[i].m_expireTime) {
            CGQuestLog::RemoveQuestWatch(CGQuestLog::m_questWatches[i].m_questId);
            Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
        }
    }
}

// OFFSET: 0x0095D9B0
void CGQuestLog::SetServerTime(int32_t serverTime, int32_t resetTime) {
    int32_t now = _time32(NULL);
    CGQuestLog::m_timeOffset = now - serverTime;

    if (now == serverTime) {
        CGQuestLog::m_timeOffset = 1;
    }

    CGQuestLog::m_questResetTime = now + resetTime;
    CGQuestLog::m_nextQuestUpdate = now + 3600;

    if (now + 3600 >= now + resetTime) {
        CGQuestLog::m_nextQuestUpdate = now + resetTime;
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
}

// PROVEN: 602bed41 IDENTICAL/NONE
// OFFSET: 0x0095DA10
void CGQuestLog::SetSelectedQuest(int32_t index) {
    if (CGQuestLog::m_selectedQuest) {
        DisplayQuestPoint(0.0f, 0.0f, 0, "");
    }

    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest)) {
        CGQuestLog::m_selectedQuest = 0;
    } else if (!CGQuestLog::m_quests[index].m_isHeader) {
        CGQuestLog::m_selectedQuest = CGQuestLog::m_quests[index].m_questId;
        AddQuestPointDisplay();
    }
}

// PROVEN: a27e2510 IDENTICAL/NONE
// OFFSET: 0x0095DA70
int32_t CGQuestLog::IsSelectedQuestExpired() {
    uint32_t index = CGQuestLog::GetSelectedLogEntry();
    return index < CGQuestLog::m_numQuest && !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired;
}

// PROVEN: 878a66d1 IDENTICAL/NONE
// OFFSET: 0x0095DAA0
void CGQuestLog::GetBlobBounds(const TSGrowableArray<BLOBPOI>& points, CRect& bounds) {
    bounds.minX = 3.402823466e38f;
    bounds.maxX = -3.402823466e38f;
    bounds.minY = 3.402823466e38f;
    bounds.maxY = -3.402823466e38f;

    for (uint32_t i = 0; i < points.m_count; i++) {
        float x = static_cast<float>(points.m_data[i].m_x);
        bounds.minX = x > bounds.minX ? bounds.minX : x;
        float xMax = static_cast<float>(points.m_data[i].m_x);
        bounds.maxX = bounds.maxX > xMax ? bounds.maxX : xMax;
        float y = static_cast<float>(points.m_data[i].m_y);
        bounds.minY = y > bounds.minY ? bounds.minY : y;
        float yMax = static_cast<float>(points.m_data[i].m_y);
        bounds.maxY = bounds.maxY > yMax ? bounds.maxY : yMax;
    }
}

// OFFSET: 0x0095DB60
bool CGQuestLog::IsPOIVisibleOnDungeonMap(const BLOBPOI& point, int32_t mapId, int32_t floor, int32_t dungeonMapId) {
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(dungeonMapId);

    if (!dungeonMap || mapId != dungeonMap->m_mapID || floor != dungeonMap->m_floorIndex) {
        return false;
    }

    C3Vector position(static_cast<float>(point.m_x), static_cast<float>(point.m_y), 0.0f);
    float x;
    float y;
    bool outland;

    if (!CGWorldMap::GetWorldPosition(position, mapId, &x, &y, &outland, false, dungeonMapId) || outland) {
        return false;
    }

    return !fequalz(fabs(x * y), 0.0f, 0.001f);
}

// OFFSET: 0x0095DE40/0x1406D0FE0
int32_t CalcQuestExp(int32_t questId, int32_t level) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (!quest) {
        return 0;
    }

    int32_t questLevel = quest->m_level != static_cast<uint32_t>(-1) ? static_cast<int32_t>(quest->m_level) : level;
    QuestXPRec* xp = g_questXPDB.GetRecord(questLevel);

    if (!xp) {
        return 0;
    }

    int32_t multiplier;
    int32_t diff = 2 * (questLevel - level) + 20;

    if (diff > 10 || diff >= 1) {
        multiplier = diff <= 10 ? diff : 10;
    } else {
        multiplier = 1;
    }

    int32_t experience = multiplier * xp->m_difficulty[quest->m_xPId] / 10;

    if (experience <= 100) {
        return 5 * ((experience + 2) / 5);
    }

    if (experience <= 500) {
        return 10 * ((experience + 5) / 10);
    }

    if (experience <= 1000) {
        return 25 * ((experience + 12) / 25);
    }

    return 50 * ((experience + 25) / 50);
}

// OFFSET: 0x0095E550/0x1406D1840
static void QuestLogQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    bool update = s_questCallbackCount == 0;

    if (s_questCallbackCount > 0) {
        update = --s_questCallbackCount == 0;
    }

    if (update) {
        CGQuestLog::Update(1);
    }

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::CheckForQuestAutoComplete(id, 0, 0);
    }
}

// OFFSET: 0x0095E5C0/0x1406D18C0
void QuestQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::CheckForQuestAutoComplete(id, 0, 0);
    }
}

// PROVEN: 199e966e IDENTICAL/NONE
// OFFSET: 0x0095E620
int32_t Packet_SMSG_QUERY_TIME_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t serverTime = 0;
    uint32_t resetTime;
    msg->Get(serverTime);
    msg->Get(resetTime);
    CGQuestLog::SetServerTime(serverTime, resetTime);
    return 1;
}

// OFFSET: 0x0095E670
static int32_t QSortQuestLogEntries(const void* a, const void* b) {
    const QuestLogEntry* first = static_cast<const QuestLogEntry*>(a);
    const QuestLogEntry* second = static_cast<const QuestLogEntry*>(b);
    int32_t firstId = first->m_questId;
    int32_t secondId = second->m_questId;
    int32_t firstHeader = first->m_isHeader;
    int32_t secondHeader = second->m_isHeader;
    QuestCache* firstQuest = NULL;
    QuestCache* secondQuest = NULL;
    uint32_t firstSort = 0;

    if (!firstHeader) {
        WOWGUID none = WOWGUID::Make(0);
        firstQuest = g_questDBCache.GetRecord(firstId, none, NULL, NULL, false);
    }

    if (!secondHeader) {
        WOWGUID none = WOWGUID::Make(0);
        secondQuest = g_questDBCache.GetRecord(secondId, none, NULL, NULL, false);
    }

    uint32_t i = 0;

    if (firstHeader) {
        for (; i < 50; i++) {
            if (CGQuestLog::m_sortTypes[i] == firstId) {
                firstSort = i;
                break;
            }
        }
    } else {
        if (!firstQuest) {
            return 0;
        }

        for (; i < 50; i++) {
            if (CGQuestLog::m_sortTypes[i] == static_cast<int32_t>(firstQuest->m_areaIdOrSortId)) {
                firstSort = i;
                break;
            }
        }
    }

    uint32_t secondSort = 0;

    if (secondHeader) {
        for (i = 0; i < 50; i++) {
            if (CGQuestLog::m_sortTypes[i] == secondId) {
                secondSort = i;
                break;
            }
        }
    } else {
        if (!secondQuest) {
            return 0;
        }

        for (i = 0; i < 50; i++) {
            if (CGQuestLog::m_sortTypes[i] == static_cast<int32_t>(secondQuest->m_areaIdOrSortId)) {
                secondSort = i;
                break;
            }
        }
    }

    int32_t firstCollapsed = !firstHeader && ((1 << firstSort) & g_questLogCollapseHeaderFilterCVar->m_intValue) != 0;
    int32_t secondCollapsed = !secondHeader && ((1 << secondSort) & g_questLogCollapseHeaderFilterCVar->m_intValue) != 0;

    if (firstCollapsed) {
        if (secondCollapsed) {
            return 0;
        }

        return 2 * firstCollapsed - 1;
    }

    if (secondCollapsed) {
        return 2 * firstCollapsed - 1;
    }

    if (firstSort != secondSort) {
        return firstSort < secondSort ? -1 : 1;
    }

    if (firstHeader) {
        return -1;
    }

    if (secondHeader) {
        return 1;
    }

    int32_t firstLevel = firstQuest->m_level;
    int32_t secondLevel = secondQuest->m_level;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        if (firstLevel == -1) {
            firstLevel = player->m_unit->m_unitFieldLevel;
        }

        if (secondLevel == -1) {
            secondLevel = player->m_unit->m_unitFieldLevel;
        }
    }

    if (firstLevel == secondLevel) {
        return SStrSortUTF8I(firstQuest->m_name, secondQuest->m_name, 0x7FFFFFFF);
    }

    return 2 * (firstLevel >= secondLevel) - 1;
}

// OFFSET: 0x0095E8E0
void CGQuestLog::FilterAndSortQuests() {
    qsort(CGQuestLog::m_quests, CGQuestLog::m_numQuest, sizeof(QuestLogEntry), QSortQuestLogEntries);
    CGQuestLog::m_numShownQuests = CGQuestLog::m_numQuest;

    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        if (CGQuestLog::m_quests[i].m_isHeader) {
            continue;
        }

        uint32_t sortId = GetQuestSortId(i);

        if (sortId < 50 && ((1 << sortId) & g_questLogCollapseHeaderFilterCVar->m_intValue)) {
            CGQuestLog::m_numShownQuests--;
        }
    }
}

// OFFSET: 0x0095E950
void CGQuestLog::CollapseHeader(uint32_t index, int32_t collapse) {
    if (index >= CGQuestLog::m_numQuest || !CGQuestLog::m_quests[index].m_isHeader) {
        g_questLogCollapseHeaderFilterCVar->Set(-(collapse != 0), true, false, false, true);
        CGQuestLog::FilterAndSortQuests();
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
        return;
    }

    uint32_t i = 0;

    for (; i < CGQuestLog::m_numSortTypes; i++) {
        if (CGQuestLog::m_sortTypes[i] == CGQuestLog::m_quests[index].m_questId) {
            break;
        }
    }

    if (i == CGQuestLog::m_numSortTypes) {
        return;
    }

    int32_t filter = g_questLogCollapseHeaderFilterCVar->m_intValue;

    if (collapse) {
        filter |= 1 << i;
    } else {
        filter &= ~(1 << i);
    }

    g_questLogCollapseHeaderFilterCVar->Set(filter, true, false, false, true);
    CGQuestLog::FilterAndSortQuests();
    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
}

// OFFSET: 0x0095EA10/0x1406D1D10
void CGQuestLog::LoadAutoQuestPopUps() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("autoQuestPopUps");
    list.m_pos = 2;
    list.m_state = 0;
    CGQuestLog::m_numAutoQuestPopUps = 0;
    list.GetNextValue();
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    for (uint32_t questId = list.GetNextValue(); questId; questId = list.GetNextValue()) {
        if (CGQuestLog::m_numAutoQuestPopUps >= 50 || !player || !questId) {
            continue;
        }

        uint32_t slot = 0;

        for (; slot < 50; slot++) {
            if (player->m_player->m_playerQuestLog[slot].m_questId == questId) {
                break;
            }
        }

        if (slot >= 50) {
            continue;
        }

        uint32_t type = list.GetNextValue();

        if (type < 2) {
            AutoQuestPopUp* popUp = &CGQuestLog::m_autoQuestPopUps[CGQuestLog::m_numAutoQuestPopUps];
            popUp->m_questId = questId;
            popUp->m_type = type;
            popUp->m_shown = 0;
            CGQuestLog::m_numAutoQuestPopUps++;
        }
    }

    if (list.m_state != 1) {
        char value[256];
        value[0] = 'v';
        value[1] = 1;
        value[list.m_pos] = '\0';
        list.m_cvar->Set(value, true, false, false, true);
    }
}

// OFFSET: 0x0095EB40/0x1406D1E70
void CGQuestLog::SaveAutoQuestPopUps() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("autoQuestPopUps");
    list.m_pos = 2;
    list.m_state = 0;
    list.AddNextValue(1);

    for (uint32_t i = 0; i < CGQuestLog::m_numAutoQuestPopUps; i++) {
        list.AddNextValue(CGQuestLog::m_autoQuestPopUps[i].m_questId);
        list.AddNextValue(CGQuestLog::m_autoQuestPopUps[i].m_type);
    }

    char value[256];
    value[0] = 'v';
    value[1] = 1;
    value[2] = '\0';
    list.m_cvar->Set(value, true, false, false, true);
}

// OFFSET: 0x0095EC00/0x1406D1F50
void CGQuestLog::AbandonSelectedQuest() {
    if (!CGQuestLog::m_abandonQuest || !ClntObjMgrGetActivePlayerObj()) {
        return;
    }

    for (uint32_t i = 0; i < 100; i++) {
        if (!CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId == CGQuestLog::m_abandonQuest) {
            CGPlayer_C::QuestLogRemoveQuest(CGQuestLog::m_quests[i].m_logSlot);
            return;
        }
    }
}

// OFFSET: 0x0095EC60
const char* CGQuestLog::GetQuestNameByID(int32_t questId) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCallback, NULL, false);
    return quest ? quest->m_name : NULL;
}

// OFFSET: 0x0095ECB0/0x1406D2020
const char* GetQuestTag(int32_t index) {
    QuestCache* quest;
    QuestInfoRec* info;

    if (index >= 0 && index < static_cast<int32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader) {
        WOWGUID none = WOWGUID::Make(0);

        if ((quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false)) != NULL && (info = g_questInfoDB.GetRecord(quest->m_infoId)) != NULL) {
            return info->m_infoName;
        }
    }

    return NULL;
}

// OFFSET: 0x0095ED20/0x1406D20D0
uint32_t IsDailyQuest(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false);

    if (!quest) {
        return 0;
    }

    return quest->m_flags & 0x1000;
}

// OFFSET: 0x0095ED90/0x1406D2140
int32_t GetQuestSuggestedPlayers(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false);

    if (!quest) {
        return 0;
    }

    return quest->m_suggestedPlayers;
}

// OFFSET: 0x0095EDF0/0x1406D21B0
int32_t GetQuestLevel(int32_t questId) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCallback, NULL, false);

    if (!quest) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && quest->m_level == static_cast<uint32_t>(-1)) {
        return player->m_unit->m_unitFieldLevel;
    }

    return quest->m_level;
}

// OFFSET: 0x0095EE70/0x1406D2240
CGItem_C* CGQuestLog::GetQuestSpecialItem(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return NULL;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false);
    CGPlayer_C* player;

    if (!quest || (!quest->m_startingItemId && !(quest->m_flags & 0x20000)) || (player = ClntObjMgrGetActivePlayerObj()) == NULL) {
        return NULL;
    }

    CGItem_C* item = player->m_bag.FindItemOfType(quest->m_startingItemId, 0);

    if (item && item->CanBeUsed()) {
        return item;
    }

    if (!(quest->m_flags & 0x20000)) {
        return NULL;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!quest->m_intermediateItemId[i]) {
            continue;
        }

        CGItem_C* source = player->m_bag.FindItemOfType(quest->m_intermediateItemId[i], 0);

        if (source && source->CanBeUsed()) {
            return source;
        }
    }

    return NULL;
}

// OFFSET: 0x0095EF70/0x1406D2370
int32_t CGQuestLog::IsQuestCompleteEx(const CGPlayer_C* player, uint32_t index, bool requireObjectives, int32_t itemId, int32_t itemCount) {
    if (index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader || !player) {
        return 0;
    }

    int32_t questId = CGQuestLog::m_quests[index].m_questId;
    PlayerQuestLogEntry* entry = player->GetQuestLogEntryByID(questId);
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCallback, NULL, false);

    if (!quest || !entry || (entry->m_state & 0x2) || CGQuestLog::IsQuestFailed(index)) {
        return 0;
    }

    if (!(quest->m_flags & 0x6) && !quest->m_friendlyFactionId && !quest->m_hostileFactionId && quest->m_rewardMoney >= 0) {
        int32_t hasObjectives = 0;

        for (uint32_t i = 0; i < 4; i++) {
            if (quest->m_objectiveId[i]) {
                hasObjectives = 1;
                break;
            }
        }

        for (uint32_t i = 0; i < 6; i++) {
            if (quest->m_collectItemId[i]) {
                hasObjectives = 1;
                break;
            }
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (quest->m_requiredCurrencyId[i]) {
                hasObjectives = 1;
                break;
            }
        }

        if (requireObjectives && !hasObjectives && !quest->m_requiredPlayersKilled) {
            return 0;
        }
    }

    if ((quest->m_flags & 0x6) && !(entry->m_state & 0x1)) {
        return 0;
    }

    if (quest->m_friendlyFactionId && GetRepListRepValue(quest->m_friendlyFactionId) < static_cast<int32_t>(quest->m_friendlyFactionAmount)) {
        return 0;
    }

    if (quest->m_hostileFactionId && GetRepListRepValue(quest->m_hostileFactionId) > static_cast<int32_t>(quest->m_hostileFactionAmount)) {
        return 0;
    }

    if (quest->m_rewardMoney < 0 && player->GetCoinage() < static_cast<uint32_t>(-quest->m_rewardMoney)) {
        return 0;
    }

    const uint16_t* count = entry->m_counts;

    for (uint32_t i = 0; i < 4; i++) {
        if (!quest->m_objectiveId[i]) {
            continue;
        }

        if (*count < quest->m_objectiveRequiredCount[i]) {
            return 0;
        }

        count++;
    }

    for (uint32_t i = 0; i < 6; i++) {
        int32_t collectId = quest->m_collectItemId[i];

        if (!collectId) {
            continue;
        }

        if (collectId == itemId && itemId && itemCount ? itemCount < static_cast<int32_t>(quest->m_collectItemCount[i]) : player->m_bag.GetItemTypeCount(collectId, 8) < static_cast<int32_t>(quest->m_collectItemCount[i])) {
            return 0;
        }
    }

    if (entry->m_counts[0] < static_cast<int32_t>(quest->m_requiredPlayersKilled)) {
        return 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (static_cast<int32_t>(quest->m_requiredCurrencyId[i]) > 0 && !CGCurrencyTypes::HasCurrency(quest->m_requiredCurrencyId[i], quest->m_requiredCurrencyCount[i])) {
            return 0;
        }
    }

    return !quest->m_requiredSpell || player->IsSpellKnown(quest->m_requiredSpell);
}

// OFFSET: 0x0095F1F0/0x1406D2670
bool CGQuestLog::HasNoObjectives(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return false;
    }

    uint32_t slot = CGQuestLog::m_quests[index].m_logSlot;
    PlayerQuestLogEntry* entry = slot > 50 ? NULL : &player->m_player->m_playerQuestLog[slot];
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false);

    if (!quest || !entry || (entry->m_state & 0x2) || CGQuestLog::IsQuestFailed(index)) {
        return false;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (quest->m_objectiveId[i]) {
            return false;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (quest->m_collectItemId[i] && (quest->m_collectItemId[i] != quest->m_startingItemId || static_cast<int32_t>(quest->m_collectItemCount[i]) > 1)) {
            return false;
        }
    }

    if (quest->m_requiredPlayersKilled || (quest->m_flags & 0x6) || quest->m_friendlyFactionId || quest->m_hostileFactionId) {
        return false;
    }

    if (quest->m_rewardMoney < 0) {
        return false;
    }

    return quest->m_requiredSpell == 0;
}

// OFFSET: 0x0095F320/0x1406D27E0
bool CGQuestLog::HasAutoCompleteBeenTried(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return false;
    }

    uint32_t slot = CGQuestLog::m_quests[index].m_logSlot;
    PlayerQuestLogEntry* entry = slot > 50 ? NULL : &player->m_player->m_playerQuestLog[slot];
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, NULL, false);
    return quest && entry && !(entry->m_state & 0x2) && !CGQuestLog::IsQuestFailed(index) && (quest->m_flags & 0x10000) && (entry->m_state & 0x4);
}

// OFFSET: 0x0095F400/0x1406D28F0
int32_t CGQuestLog::IsOnQuest(const CGPlayer_C* player, int32_t questId) {
    if (!player || (player->m_modelFlags & 0x10000)) {
        return 0;
    }

    for (uint32_t i = 0; i < 50; i++) {
        if (i <= 50 && &player->m_player->m_playerQuestLog[i]) {
            int32_t current = player->m_player->m_playerQuestLog[i].m_questId;

            if (current && current == questId) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0095F470
void CGQuestLog::UpdateQuestItem(int32_t itemId) {
    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        WOWGUID none = WOWGUID::Make(0);
        QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[i].m_questId, none, NULL, NULL, false);

        if (!quest) {
            continue;
        }

        uint32_t j = 0;

        for (; j < 6; j++) {
            if (static_cast<int32_t>(quest->m_collectItemId[j]) == itemId) {
                break;
            }
        }

        if (j < 6) {
            Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::QueryQuestGiverStatusMultiple();
            }

            return;
        }

        if (quest->m_flags & 0x20000) {
            for (uint32_t k = 0; k < 4; k++) {
                if (static_cast<int32_t>(quest->m_intermediateItemId[k]) == itemId) {
                    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
                }
            }
        }
    }
}

// OFFSET: 0x0095F550
void CGQuestLog::UpdateQuestSpell(int32_t spellId) {
    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        WOWGUID none = WOWGUID::Make(0);
        QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[i].m_questId, none, NULL, NULL, false);

        if (quest && static_cast<int32_t>(quest->m_requiredSpell) == spellId) {
            Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::QueryQuestGiverStatusMultiple();
            }

            return;
        }
    }
}

// OFFSET: 0x0095F5F0
void CGQuestLog::UpdateQuestCurrency(int32_t currencyId) {
    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        WOWGUID none = WOWGUID::Make(0);
        QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[i].m_questId, none, NULL, NULL, false);

        if (!quest) {
            continue;
        }

        for (uint32_t j = 0; j < 6; j++) {
            if (static_cast<int32_t>(quest->m_requiredCurrencyId[j]) == currencyId) {
                Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

                if (ClntObjMgrGetActivePlayerObj()) {
                    CGPlayer_C::QueryQuestGiverStatusMultiple();
                }

                return;
            }
        }
    }
}

// OFFSET: 0x0095F690
int32_t CGQuestLog::GetQuestCriteriaSpell(int32_t questId) {
    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    int32_t id = questId ? questId : CGQuestLog::m_selectedQuest;

    if (!id) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(id, none, QuestQueryCallback, NULL, false);

    if (!quest) {
        return 0;
    }

    return quest->m_requiredSpell;
}

// OFFSET: 0x0095F700/0x1406D2C70
int32_t CountQuestItemDropsNeeded(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    int32_t count = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, callback, param, false);

    if (quest) {
        for (uint32_t i = 0; i < 4; i++) {
            int32_t itemId = quest->m_intermediateItemId[i];

            if (itemId > 0 && (itemId != static_cast<int32_t>(quest->m_startingItemId) || static_cast<int32_t>(quest->m_intermediateItemCount[i]) > 1) && !player->m_bag.GetItemTypeCount(itemId, 8)) {
                count++;
            }
        }
    }

    return count;
}

// OFFSET: 0x0095F7B0/0x1406D2D50
int32_t CGQuestLog::GetQuestLeaderBoardForKill(const CGPlayer_C* player, int32_t questId, int32_t objectId, QuestLogLeaderBoardParams* params) {
    if (!params || !player) {
        return 0;
    }

    char* text = params->m_text;
    params->m_text[0] = '\0';
    char* type = params->m_type;
    params->m_type[0] = '\0';
    params->m_complete = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, params->m_questCallback, params->m_param, false);
    PlayerQuestLogEntry* entry = questId ? player->GetQuestLogEntryByID(questId) : NULL;

    if (!quest || !entry) {
        return 0;
    }

    uint32_t i = 0;

    for (; static_cast<int32_t>(quest->m_objectiveId[i]) != objectId; i++) {
        if (i + 1 >= 4) {
            return 0;
        }
    }

    int32_t count = entry->m_counts[i];
    int32_t objective = quest->m_objectiveId[i];
    char buffer[4096];

    if (objective >= 0) {
        const char* objectiveText = quest->m_objectiveText[i];

        if (*objectiveText) {
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_OBJECTS_FOUND", -1, GENDER_NOT_APPLICABLE), objectiveText, count, quest->m_objectiveRequiredCount[i]);
            params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
        } else {
            CreatureStats_C* creature = g_creatureDBCache.GetRecord(objective, none, params->m_creatureCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(quest->m_objectiveRequiredCount[i]);
            const char* format = FrameScript_GetText("QUEST_MONSTERS_KILLED", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(buffer, sizeof(buffer), format, creature ? creature->GetName(plural) : " ", count, quest->m_objectiveRequiredCount[i]);
            params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
        }
    } else {
        const char* objectiveText = quest->m_objectiveText[i];

        if (*objectiveText) {
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_OBJECTS_FOUND", -1, GENDER_NOT_APPLICABLE), objectiveText, count, quest->m_objectiveRequiredCount[i]);
            params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
        } else {
            GameObjectStats_C* object = g_gameObjectDBCache.GetRecord(objective & 0x7FFFFFFF, none, params->m_objectCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(quest->m_objectiveRequiredCount[i]);
            const char* format = FrameScript_GetText("QUEST_OBJECTS_FOUND", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(buffer, sizeof(buffer), format, object ? object->GetName(plural) : " ", count, quest->m_objectiveRequiredCount[i]);
            params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
        }
    }

    SStrCopy(text, buffer, 4096);
    SStrCopy(type, static_cast<int32_t>(quest->m_objectiveId[i]) >= 0 ? "monster" : "object", 64);
    return 1;
}

// OFFSET: 0x0095FAD0/0x1406D3160
int32_t CGQuestLog::GetNumQuestItems(const CGPlayer_C* player, int32_t questId, int32_t itemId) {
    if (!player || !questId || !itemId) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);
    PlayerQuestLogEntry* entry = player->GetQuestLogEntryByID(questId);

    if (!quest || !entry) {
        return 0;
    }

    int32_t slot = quest->m_objectiveId[0] != 0;

    if (quest->m_objectiveId[1]) {
        slot++;
    }

    if (quest->m_objectiveId[2]) {
        slot++;
    }

    if (quest->m_objectiveId[3]) {
        slot++;
    }

    for (uint32_t i = 0; static_cast<int32_t>(quest->m_collectItemId[i]) != itemId;) {
        if (quest->m_collectItemId[i]) {
            slot++;
        }

        if (++i >= 6) {
            return 0;
        }
    }

    return entry->m_counts[slot];
}

// OFFSET: 0x0095FBA0/0x1406D3260
int32_t CGQuestLog::GetQuestGreenRange() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t index = player->m_unit->m_unitFieldLevel / 5;

    if (index >= 20) {
        index = 19;
    }

    return s_questGreenRange[index];
}

// OFFSET: 0x0095FBF0
bool CGQuestLog::HasQuestPOI(int32_t questId) {
    if (!ClntObjMgrGetActivePlayerObj() || questId <= 0) {
        return false;
    }

    for (uint32_t i = 0; i < 50; i++) {
        if (CGQuestLog::m_questPOIs[i].m_questId == questId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0095FC40
int32_t CGQuestLog::GetQuestPOIIndex(int32_t questId) {
    if (!ClntObjMgrGetActivePlayerObj() || questId <= 0) {
        return -1;
    }

    for (int32_t i = 0; i < 50; i++) {
        if (CGQuestLog::m_questPOIs[i].m_questId == questId) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x0095FC90
uint32_t CGQuestLog::ComputeBlobQuestObjectives(int32_t questId) {
    uint32_t result = 0;
    int32_t index = CGQuestLog::GetQuestIndex(questId) - 1;

    if (index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader || index < 0) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    PlayerQuestLogEntry* entry = player->GetQuestLogEntry(CGQuestLog::m_quests[index].m_logSlot);
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, NULL, NULL, false);

    if (!quest || !entry || (entry->m_state & 0x2) || CGQuestLog::IsQuestFailed(index)) {
        return 0;
    }

    if (CGQuestLog::IsQuestCompleteEx(player, index, false, 0, 0)) {
        return 0xFFFFFFFF;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (quest->m_objectiveId[i] && entry->m_counts[i] < quest->m_objectiveRequiredCount[i]) {
            result |= 1 << i;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (quest->m_collectItemId[i] && player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8) < static_cast<int32_t>(quest->m_collectItemCount[i])) {
            result |= 1 << (i + 4);
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t itemId = quest->m_intermediateItemId[i];

        if (itemId && itemId != quest->m_startingItemId && player->m_bag.GetItemTypeCount(itemId, 8) < static_cast<int32_t>(quest->m_intermediateItemCount[i])) {
            result |= 1 << (i + 10);
        }
    }

    if ((quest->m_flags & 0x6) && !(entry->m_state & 0x1)) {
        return result | 0x10000;
    }

    return result;
}

// OFFSET: 0x0095FEC0
int32_t CGQuestLog::GetQuestWorldMapAreaID(int32_t questId, int32_t& floor) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || questId <= 0) {
        return 0;
    }

    CGWorldMap::SetMapToCurrentZone(0);
    WorldMapAreaRec* current = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    int32_t currentFloor = dungeonMap ? dungeonMap->m_floorIndex : 0;

    if (current) {
        uint32_t index = 0;

        for (; index < 50; index++) {
            if (CGQuestLog::m_questPOIs[index].m_questId == questId) {
                break;
            }
        }

        if (index < 50) {
            QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];
            poi->m_objectiveMask = CGQuestLog::ComputeBlobQuestObjectives(poi->m_questId);
            int32_t bestArea = 0;
            int32_t bestFloor = 0;
            float bestDistance = 3.402823466e38f;
            int32_t bestFloorDelta = 0x7FFFFFFF;

            for (uint32_t i = 0; static_cast<int32_t>(i) < static_cast<int32_t>(poi->m_blobs.m_count); i++) {
                BLOB* blob = &poi->m_blobs.m_data[i];

                if (poi->m_objectiveMask == 0xFFFFFFFF) {
                    if (blob->m_objectiveIndex != -1) {
                        continue;
                    }
                } else if (blob->m_objectiveIndex == -1 || !((1 << blob->m_objectiveIndex) & poi->m_objectiveMask)) {
                    continue;
                }

                if (current->m_id == blob->m_worldMapAreaID && currentFloor == blob->m_floor) {
                    floor = poi->m_blobs.m_data[i].m_floor;
                    return poi->m_blobs.m_data[i].m_worldMapAreaID;
                }

                WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(blob->m_worldMapAreaID);

                if (!area) {
                    continue;
                }

                if (area->m_mapID == current->m_mapID) {
                    C3Vector position = player->GetPosition();
                    position.x -= static_cast<float>(blob->m_center.m_x);
                    position.y -= static_cast<float>(blob->m_center.m_y);
                    int32_t blobFloor = blob->m_floor;
                    int32_t floorDelta = abs(currentFloor - blobFloor);

                    if (floorDelta < bestFloorDelta || (floorDelta == bestFloorDelta && bestDistance > position.y * position.y + position.x * position.x)) {
                        bestDistance = position.y * position.y + position.x * position.x;
                        bestFloorDelta = floorDelta;
                        bestArea = blob->m_worldMapAreaID;
                        bestFloor = blobFloor;
                    }
                } else if (!bestArea) {
                    bestArea = blob->m_worldMapAreaID;
                    bestFloor = blob->m_floor;
                }
            }

            if (bestArea) {
                floor = bestFloor;
                return bestArea;
            }
        }
    }

    uint32_t header = 0;
    uint32_t i = 0;

    for (;; i++) {
        if (CGQuestLog::m_quests[i].m_isHeader) {
            header = i;
        } else if (CGQuestLog::m_quests[i].m_questId == questId) {
            break;
        }

        if (i + 1 >= 100) {
            return 0;
        }
    }

    int32_t areaId = CGQuestLog::m_quests[header].m_questId;

    if (areaId <= 0) {
        return 0;
    }

    while (true) {
        AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

        if (!area) {
            return 0;
        }

        if (!(area->m_flags & 0x40000000)) {
            for (int32_t j = 0; j < g_worldMapAreaDB.m_numRecords; j++) {
                WorldMapAreaRec* map = g_worldMapAreaDB.GetRecordByIndex(j);

                if (map && map->m_areaID == areaId) {
                    return map->m_id;
                }
            }

            return 0;
        }

        areaId = area->m_parentAreaID;

        if (!areaId) {
            return 0;
        }
    }
}

// OFFSET: 0x00960220
bool CGQuestLog::QuestPOIBlobIntersectsRect(int32_t questId, int32_t blobIndex, const CRect& rect) {
    if (!ClntObjMgrGetActivePlayerObj() || blobIndex < 0) {
        return false;
    }

    uint32_t index = 0;

    for (; index < 50; index++) {
        if (CGQuestLog::m_questPOIs[index].m_questId == questId) {
            break;
        }
    }

    if (index >= 50) {
        return false;
    }

    QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];

    if (blobIndex >= static_cast<int32_t>(poi->m_blobs.m_count)) {
        return false;
    }

    BLOB* blob = &poi->m_blobs.m_data[blobIndex];
    C3Vector quad[4];
    quad[0] = C3Vector(rect.minY, rect.minX, 0.0f);
    quad[1] = C3Vector(rect.minY, rect.maxX, 0.0f);
    quad[2] = C3Vector(rect.maxY, rect.maxX, 0.0f);
    quad[3] = C3Vector(rect.maxY, rect.minX, 0.0f);
    uint32_t count = blob->m_points.m_count;

    if (!count) {
        return false;
    }

    for (uint32_t i = 0; i < count; i++) {
        C3Vector point(static_cast<float>(blob->m_points.m_data[i].m_x), static_cast<float>(blob->m_points.m_data[i].m_y), 0.0f);

        if (NTempest::Intersect(point, quad, 4, C3Vector::AXIS_Z)) {
            return true;
        }

        uint32_t next = (i + 1) % count;
        C2Vector nextPoint(static_cast<float>(blob->m_points.m_data[next].m_x), static_cast<float>(blob->m_points.m_data[next].m_y));

        for (uint32_t edge = 1; edge < 4; edge++) {
            if (NTempest::Intersect2dSegments(C2Vector(point), nextPoint, C2Vector(quad[edge]), C2Vector(quad[edge == 1 ? 3 : edge - 1]))) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009603E0
QuestPOIInfo* CGQuestLog::GetQuestPOIData(int32_t questId) {
    uint32_t index = CGQuestLog::GetQuestPOIIndex(questId);

    if (index > 49) {
        return NULL;
    }

    return &CGQuestLog::m_questPOIs[index];
}

// OFFSET: 0x00960410
BLOB* CGQuestLog::GetQuestPOIBlob(int32_t questId, uint32_t blobIndex) {
    uint32_t index = CGQuestLog::GetQuestPOIIndex(questId);
    QuestPOIInfo* poi;

    if (index <= 49 && (poi = &CGQuestLog::m_questPOIs[index]) != NULL && blobIndex < poi->m_blobs.m_count) {
        return &poi->m_blobs.m_data[blobIndex];
    }

    return NULL;
}

// OFFSET: 0x00960450/0x1406D3CF0
static int32_t QSortQuestWatches(const void* a, const void* b) {
    const QuestWatch* watchA = static_cast<const QuestWatch*>(a);
    const QuestWatch* watchB = static_cast<const QuestWatch*>(b);
    int32_t questA = watchA->m_questId;
    int32_t questB = watchB->m_questId;
    int32_t sort = g_trackerSortingCVar->m_intValue;

    if (sort == 1) {
        return watchA->m_distance <= watchB->m_distance ? -1 : 1;
    }

    if (sort <= 1 || sort > 3) {
        return questA - questB;
    }

    int32_t levelA = GetQuestLevel(questA);
    int32_t levelB = GetQuestLevel(questB);

    if (levelA == levelB) {
        return questA - questB;
    }

    return sort == 3 ? levelA - levelB : levelB - levelA;
}

// OFFSET: 0x009604F0/0x1406D3DC0
float CGQuestLog::GetQuestDistSqr(int32_t questId, C2Vector& position) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0.0f;
    }

    int32_t index = CGQuestLog::GetQuestPOIIndex(questId);

    if (index < 0) {
        return 3.402823466e38f;
    }

    float bestDistance = 3.402823466e38f;
    int32_t bestFloorDelta = 0x7FFFFFFF;
    int32_t continent = CGPlayer_C::GetNewContinentID();
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    int32_t currentFloor = dungeonMap ? dungeonMap->m_floorIndex : 0;
    QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];
    poi->m_objectiveMask = CGQuestLog::ComputeBlobQuestObjectives(poi->m_questId);

    for (int32_t i = 0; i < static_cast<int32_t>(poi->m_blobs.m_count); i++) {
        BLOB* blob = &poi->m_blobs.m_data[i];

        if (poi->m_objectiveMask == 0xFFFFFFFF) {
            if (blob->m_objectiveIndex != -1) {
                continue;
            }
        } else if (blob->m_objectiveIndex == -1 || !((1 << blob->m_objectiveIndex) & poi->m_objectiveMask)) {
            continue;
        }

        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(blob->m_worldMapAreaID);

        if (!area || area->m_mapID != continent) {
            continue;
        }

        C3Vector playerPosition = player->GetPosition();
        playerPosition.x -= static_cast<float>(poi->m_blobs.m_data[i].m_center.m_x);
        playerPosition.y -= static_cast<float>(poi->m_blobs.m_data[i].m_center.m_y);
        int32_t floorDelta = abs(currentFloor - poi->m_blobs.m_data[i].m_floor);

        if (floorDelta < bestFloorDelta || (floorDelta == bestFloorDelta && bestDistance > playerPosition.y * playerPosition.y + playerPosition.x * playerPosition.x)) {
            bestDistance = playerPosition.y * playerPosition.y + playerPosition.x * playerPosition.x;
            bestFloorDelta = floorDelta;
            position.x = static_cast<float>(poi->m_blobs.m_data[i].m_center.m_x);
            position.y = static_cast<float>(poi->m_blobs.m_data[i].m_center.m_y);
        }
    }

    return bestDistance;
}

// PROVEN: 2691a13d IDENTICAL/NONE
// OFFSET: 0x00960700/0x1406D0C50
void CGQuestLog::SaveTrackedQuests() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedQuests");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
        if (!CGQuestLog::m_questWatches[i].m_expireTime) {
            list.AddNextValue(CGQuestLog::m_questWatches[i].m_questId);
        }
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x009607B0/0x1406D4080
void CGQuestLog::GetAllOfGroupsQuests(int32_t (&quests)[250]) {
    memset(quests, 0, sizeof(quests));
    int32_t count = 0;

    for (int32_t i = 0; i < static_cast<int32_t>(CGQuestLog::m_numQuest); i++) {
        if (i >= 0 && i < static_cast<int32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId) {
            quests[count++] = CGQuestLog::m_quests[i].m_questId;
        }
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        CGPlayer_C* member = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGPartyInfo::GetMember(i), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!member) {
            continue;
        }

        for (uint32_t slot = 0; slot < 50; slot++) {
            PlayerQuestLogEntry* entry = slot > 50 ? NULL : &member->m_player->m_playerQuestLog[slot];
            int32_t questId = entry->m_questId;

            if (!questId) {
                continue;
            }

            int32_t j = 0;

            for (; j < count; j++) {
                if (quests[j] == questId) {
                    break;
                }
            }

            if (j >= count) {
                quests[count++] = questId;
            }
        }
    }
}

// OFFSET: 0x00961A70/0x1406D57C0
bool CGQuestLog::AddAutoQuestPopUp(int32_t questId, int32_t type) {
    uint32_t index = 0;

    for (; index < 100; index++) {
        if (!CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_questId == questId) {
            break;
        }
    }

    if (index >= 100 && CGQuestInfo::m_currentQuest != questId) {
        return false;
    }

    uint32_t count = CGQuestLog::m_numAutoQuestPopUps;

    for (uint32_t i = 0; i < count; i++) {
        if (CGQuestLog::m_autoQuestPopUps[i].m_questId == questId) {
            if (CGQuestLog::m_autoQuestPopUps[i].m_type == type) {
                return false;
            }

            CGQuestLog::m_autoQuestPopUps[i].m_type = type;
            CGQuestLog::SaveAutoQuestPopUps();
            return true;
        }
    }

    if (count < 50) {
        AutoQuestPopUp* popUp = &CGQuestLog::m_autoQuestPopUps[count];
        popUp->m_questId = questId;
        popUp->m_type = type;
        popUp->m_shown = CGQuestLog::GetQuestIndex(questId) == 0;
        CGQuestLog::m_numAutoQuestPopUps = count + 1;
    }

    CGQuestLog::SaveAutoQuestPopUps();
    return true;
}

// PROVEN: e68df7f6 IDENTICAL/NONE
// OFFSET: 0x00961B50
void CGQuestLog::RemoveAutoQuestPopUp(int32_t questId) {
    uint32_t count = CGQuestLog::m_numAutoQuestPopUps;
    uint32_t kept = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (CGQuestLog::m_autoQuestPopUps[i].m_questId == questId) {
            CGQuestLog::m_numAutoQuestPopUps--;
            continue;
        }

        if (kept != i) {
            CGQuestLog::m_autoQuestPopUps[kept] = CGQuestLog::m_autoQuestPopUps[i];
        }

        kept++;
    }

    CGQuestLog::SaveAutoQuestPopUps();
}

// OFFSET: 0x00961BB0/0x1406D5920
const char* CGQuestLog::GetQuestName(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest)) {
        return NULL;
    }

    int32_t id = CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::m_quests[index].m_isHeader) {
        return CGQuestLog::GetQuestNameByID(id);
    }

    if (id > 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(id);

        if (area) {
            return area->m_areaName;
        }

        return NULL;
    }

    if (id >= 0) {
        return "Missing header! (quest designers)";
    }

    QuestSortRec* sort = g_questSortDB.GetRecord(-id);

    if (sort) {
        return sort->m_sortName;
    }

    return NULL;
}

// OFFSET: 0x00961C20/0x1406D59E0
int32_t CGQuestLog::IsQuestComplete(uint32_t index) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    return CGQuestLog::IsQuestCompleteEx(player, index, true, 0, 0);
}

// PROVEN: 23fcc8e2 IDENTICAL/NONE
// OFFSET: 0x00961C50/0x1406D5A30
int32_t CGQuestLog::IsQuestComplete(const CGPlayer_C* player, uint32_t index) {
    return CGQuestLog::IsQuestCompleteEx(player, index, true, 0, 0);
}

// OFFSET: 0x00961C70
int32_t CGQuestLog::IsOnQuest(int32_t questId) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    return CGQuestLog::IsOnQuest(player, questId);
}

// OFFSET: 0x00961CA0/0x1406D5AE0
int32_t CGQuestLog::GetNumQuestLeaderBoards(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    int32_t count = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, callback, param, false);

    if (quest) {
        if (quest->m_endText[0]) {
            count = 1;
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (quest->m_objectiveId[i]) {
                count++;
            }
        }

        for (uint32_t i = 0; i < 6; i++) {
            int32_t itemId = quest->m_collectItemId[i];

            if (itemId > 0 && (itemId != static_cast<int32_t>(quest->m_startingItemId) || static_cast<int32_t>(quest->m_collectItemCount[i]) > 1)) {
                count++;
            }
        }

        if (quest->m_friendlyFactionId) {
            count++;
        }

        if (quest->m_requiredPlayersKilled) {
            count++;
        }

        if (quest->m_requiredSpell) {
            count++;
        }

        if (count) {
            return count;
        }
    }

    uint32_t index = CGQuestLog::GetQuestIndex(questId);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!CGQuestLog::IsQuestCompleteEx(player, index - 1, true, 0, 0)) {
        return count;
    }

    return 1;
}

// OFFSET: 0x00961E30
int32_t CGQuestLog::GetQuestLeaderBoardForKill(int32_t questId, int32_t objectId, QuestLogLeaderBoardParams* params) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    return CGQuestLog::GetQuestLeaderBoardForKill(player, questId, objectId, params);
}

// OFFSET: 0x00961E70/0x1406D5C90
int32_t CGQuestLog::UpdateAllQuestPOI() {
    int32_t visible = 0;

    if (!g_questPOICVar->m_intValue || !ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    WorldMapAreaRec* area = NULL;
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());

    if (!dungeonMap) {
        area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

        if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
            return 0;
        }
    }

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo& poi = CGQuestLog::m_questPOIs[i];
        poi.m_objectiveMask = CGQuestLog::ComputeBlobQuestObjectives(poi.m_questId);
        poi.m_visible = 0;

        for (int32_t j = 0; j < static_cast<int32_t>(poi.m_blobs.m_count); j++) {
            BLOB& blob = poi.m_blobs.m_data[j];

            if (poi.m_objectiveMask == 0xFFFFFFFF) {
                if (blob.m_objectiveIndex != -1) {
                    continue;
                }
            } else if (blob.m_objectiveIndex == -1 || !((1 << blob.m_objectiveIndex) & poi.m_objectiveMask)) {
                continue;
            }

            bool onMap;

            if (dungeonMap) {
                onMap = CGQuestLog::IsPOIVisibleOnDungeonMap(blob.m_center, blob.m_mapId, blob.m_floor, dungeonMap->m_id);
            } else {
                if (!area || ((blob.m_flags & 0x4) && blob.m_worldMapAreaID != area->m_id)) {
                    continue;
                }

                onMap = CGQuestLog::IsPOIVisibleOnWorldMap(blob.m_center, blob.m_mapId, area->m_mapID);
            }

            if (onMap) {
                visible++;
                poi.m_visible = 1;
                break;
            }
        }
    }

    return visible;
}

// OFFSET: 0x00962030
void CGQuestLog::UpdatePOIIcons() {
    if (!g_questPOICVar->m_intValue) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    WorldMapAreaRec* area = NULL;
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());

    if (!dungeonMap) {
        area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

        if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
            return;
        }
    }

    C3Vector position = player->GetPosition();
    int32_t numActive = 0;
    int32_t numComplete = 0;
    int32_t chain[50];
    int32_t completeList[50];
    memset(chain, 0, sizeof(chain));
    uint8_t movable[50];
    memset(movable, 0, sizeof(movable));
    int32_t activeList[50];

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];
        float bestDistance = 3.402823466e38f;
        int32_t best = -1;
        int32_t bestPriority = 10;

        for (int32_t j = 0; j < static_cast<int32_t>(poi->m_blobs.m_count); j++) {
            BLOB* blob = &poi->m_blobs.m_data[j];
            int32_t objective = blob->m_objectiveIndex;

            if (!((objective == -1 && poi->m_objectiveMask == 0xFFFFFFFF) || (poi->m_objectiveMask != 0xFFFFFFFF && ((1 << objective) & poi->m_objectiveMask)))) {
                continue;
            }

            bool visible;

            if (dungeonMap) {
                visible = CGQuestLog::IsPOIVisibleOnDungeonMap(blob->m_center, blob->m_mapId, blob->m_floor, dungeonMap->m_id);
            } else {
                if (!area || ((blob->m_flags & 0x4) && blob->m_worldMapAreaID != area->m_id)) {
                    continue;
                }

                visible = CGQuestLog::IsPOIVisibleOnWorldMap(blob->m_center, blob->m_mapId, area->m_mapID);
            }

            if (!visible) {
                continue;
            }

            int32_t priority = blob->m_priority;
            float dy = fabs(position.y - blob->m_center.m_y);
            float dx = fabs(position.x - blob->m_center.m_x);
            float distance = dx * dx + dy * dy;

            if (priority < bestPriority || (bestDistance > distance && priority == bestPriority)) {
                bestDistance = distance;
                best = j;
                bestPriority = priority;
            }
        }

        if (best == -1) {
            poi->m_iconMapId = 0;
            poi->m_iconObjectiveIndex = 0;
            poi->m_iconFloor = 0;
            poi->m_iconX = 999999;
            poi->m_iconY = 999999;
            continue;
        }

        BLOB* blob = &poi->m_blobs.m_data[best];
        poi->m_iconMapId = blob->m_mapId;
        poi->m_iconObjectiveIndex = blob->m_objectiveIndex;
        poi->m_iconFloor = blob->m_floor;
        poi->m_iconX = blob->m_center.m_x;
        poi->m_iconY = blob->m_center.m_y;
        CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();

        if (CGQuestLog::IsQuestCompleteEx(activePlayer, CGQuestLog::GetQuestIndex(poi->m_questId) - 1, true, 0, 0)) {
            completeList[numComplete++] = i;
        } else {
            movable[i] = blob->m_points.m_count != 1;
            activeList[numActive++] = i;
        }
    }

    if (!area) {
        return;
    }

    int32_t zoneId;

    if (CGWorldMap::m_currentContinent < 0) {
        zoneId = CGWorldMap::m_currentOrphanZone;
    } else {
        WorldMapContinentInfo* continent = &CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent];
        zoneId = CGWorldMap::m_currentZone < 0 ? continent->m_areaID : continent->m_zones.m_data[CGWorldMap::m_currentZone];
    }

    if (zoneId == -1) {
        return;
    }

    int32_t nudges[3][2] = { { 0, 2 }, { 1, -2 }, { -2, 1 } };
    int32_t width = static_cast<int32_t>(fabs(area->m_locLeft - area->m_locRight));
    int32_t height = static_cast<int32_t>(fabs(area->m_locTop - area->m_locBottom));
    float radius = static_cast<float>(width) * (CGQuestLog::m_questIconOverlapDist * 2.0f) * 0.0009765625f;
    float verticalRadius = static_cast<float>(height) * (CGQuestLog::m_questIconOverlapDist * 2.0f) * 0.0009765625f;
    float maxStep = radius * 0.050000001f;
    float radiusSq = radius * radius;
    uint32_t nudge = 0;

    for (int32_t k = 0; k < numActive; k++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[activeList[k]];
        poi->m_iconY = static_cast<int32_t>(static_cast<float>(poi->m_iconY) * 0.75001878f);
    }

    for (int32_t iteration = 0; iteration < s_poiOverlapIterations; iteration++) {
        int32_t forces[50][2];
        memset(forces, 0, sizeof(forces));

        for (int32_t a = 0; a < numActive; a++) {
            int32_t indexA = activeList[a];
            int32_t ax = CGQuestLog::m_questPOIs[indexA].m_iconX;
            int32_t ay = CGQuestLog::m_questPOIs[indexA].m_iconY;

            for (int32_t b = a + 1; b < numActive; b++) {
                int32_t indexB = activeList[b];
                int32_t dx = ax - CGQuestLog::m_questPOIs[indexB].m_iconX;
                int32_t dy = ay - CGQuestLog::m_questPOIs[indexB].m_iconY;
                int32_t distanceSq = dx * dx + dy * dy;

                if (radiusSq <= static_cast<float>(distanceSq)) {
                    continue;
                }

                if (!distanceSq) {
                    dx = nudges[nudge][0];
                    dy = nudges[nudge][1];
                    nudge = (nudge + 1) % 3;
                }

                float fdy = static_cast<float>(dy);
                float fdx = static_cast<float>(dx);
                float scale = radius / sqrt(fdy * fdy + fdx * fdx);
                int32_t fx = static_cast<int32_t>(static_cast<float>(dx) * scale);
                int32_t fy = static_cast<int32_t>(static_cast<float>(dy) * scale);

                if (movable[indexA]) {
                    forces[indexA][0] += fx;
                    forces[indexA][1] += fy;
                }

                if (distanceSq && movable[indexB]) {
                    forces[indexB][0] -= fx;
                    forces[indexB][1] -= fy;
                }
            }
        }

        float maxForce = -9999999.0f;

        for (int32_t k = 0; k < numActive; k++) {
            float fx = static_cast<float>(fabs(static_cast<double>(forces[activeList[k]][0])));
            float fy = static_cast<float>(fabs(static_cast<double>(forces[activeList[k]][1])));

            if (fx > maxForce) {
                maxForce = fx;
            }

            if (fy > maxForce) {
                maxForce = fy;
            }
        }

        if (maxForce == 0.0f) {
            break;
        }

        if (maxForce > maxStep) {
            float scale = maxStep / maxForce;

            for (int32_t k = 0; k < numActive; k++) {
                int32_t index = activeList[k];
                forces[index][0] = static_cast<int32_t>(static_cast<float>(forces[index][0]) * scale);
                forces[index][1] = static_cast<int32_t>(static_cast<float>(forces[index][1]) * scale);
            }
        }

        for (int32_t k = 0; k < numActive; k++) {
            int32_t index = activeList[k];
            CGQuestLog::m_questPOIs[index].m_iconX += forces[index][0];
            CGQuestLog::m_questPOIs[index].m_iconY += forces[index][1];
        }
    }

    if (s_cvPOIShiftComplete && s_cvPOIShiftComplete->m_floatValue > 0.001) {
        for (int32_t c = 0; c < numComplete - 1; c++) {
            int32_t indexC = completeList[c + 1];
            int32_t cx = CGQuestLog::m_questPOIs[indexC].m_iconX;
            int32_t cy = CGQuestLog::m_questPOIs[indexC].m_iconY;

            for (int32_t d = c; d >= 0; d--) {
                QuestPOIInfo* other = &CGQuestLog::m_questPOIs[completeList[d]];

                if (radius * 2.0f > static_cast<float>((cx - other->m_iconX) * (cx - other->m_iconX) + (cy - other->m_iconY) * (cy - other->m_iconY))) {
                    chain[indexC] = chain[completeList[d]] + 1;
                    break;
                }
            }
        }

        int32_t offsets[4][2];
        offsets[0][0] = static_cast<int32_t>(radius);
        offsets[0][1] = static_cast<int32_t>(verticalRadius * 2.0f);
        offsets[1][0] = static_cast<int32_t>(verticalRadius * -2.0f);
        offsets[1][1] = static_cast<int32_t>(radius * 2.0f);
        offsets[2][0] = static_cast<int32_t>(-radius);
        offsets[2][1] = offsets[0][1];
        offsets[3][0] = offsets[1][0];
        offsets[3][1] = static_cast<int32_t>(radius * 0.2f);
        int32_t offset = 0;

        for (int32_t k = 1; k < numComplete; k++) {
            int32_t index = completeList[k];

            if (chain[index]) {
                float shift = s_cvPOIShiftComplete->m_floatValue;
                float oy = static_cast<float>(offsets[offset][1]);
                CGQuestLog::m_questPOIs[index].m_iconX += static_cast<int32_t>(static_cast<float>(offsets[offset][0]) * shift);
                offset = (offset + 1) & 3;
                CGQuestLog::m_questPOIs[index].m_iconY += static_cast<int32_t>(oy * shift);
            }
        }
    }

    for (int32_t k = 0; k < numActive; k++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[activeList[k]];
        poi->m_iconY = static_cast<int32_t>(static_cast<float>(poi->m_iconY) * 1.3333f);
    }
}

// OFFSET: 0x00962AE0
int32_t CGQuestLog::GetPOIQuestIDByVisibleIndex(int32_t index) {
    if (static_cast<uint32_t>(index) > 49) {
        return -1;
    }

    uint32_t found = 0;

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];

        if (!poi->m_visible) {
            continue;
        }

        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (CGQuestLog::IsQuestCompleteEx(player, CGQuestLog::GetQuestIndex(poi->m_questId) - 1, true, 0, 0) || CGQuestLog::HasNoObjectives(CGQuestLog::GetQuestIndex(poi->m_questId) - 1)) {
            if (static_cast<uint32_t>(index) == found++) {
                return CGQuestLog::m_questPOIs[i].m_questId;
            }
        }
    }

    for (int32_t i = 0; i < static_cast<int32_t>(CGQuestLog::m_numQuestWatches); i++) {
        QuestWatch* watch = &CGQuestLog::m_questWatches[i];
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (CGQuestLog::IsQuestCompleteEx(player, CGQuestLog::GetQuestIndex(watch->m_questId) - 1, true, 0, 0) || CGQuestLog::HasNoObjectives(CGQuestLog::GetQuestIndex(watch->m_questId) - 1)) {
            continue;
        }

        int32_t poiIndex = CGQuestLog::GetQuestPOIIndex(watch->m_questId);

        if (poiIndex >= 0 && CGQuestLog::m_questPOIs[poiIndex].m_visible && static_cast<uint32_t>(index) == found++) {
            return CGQuestLog::m_questPOIs[poiIndex].m_questId;
        }
    }

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];

        if (!poi->m_visible) {
            continue;
        }

        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (CGQuestLog::IsQuestCompleteEx(player, CGQuestLog::GetQuestIndex(poi->m_questId) - 1, true, 0, 0) || CGQuestLog::HasNoObjectives(CGQuestLog::GetQuestIndex(poi->m_questId) - 1)) {
            continue;
        }

        bool watched = false;

        for (uint32_t j = 0; j < CGQuestLog::m_numQuestWatches; j++) {
            if (CGQuestLog::m_questWatches[j].m_questId == poi->m_questId) {
                watched = true;
                break;
            }
        }

        if (!watched && static_cast<uint32_t>(index) == found++) {
            return CGQuestLog::m_questPOIs[i].m_questId;
        }
    }

    return -1;
}

// OFFSET: 0x00962DA0/0x1406D7920
bool CGQuestLog::SortQuestWatches() {
    int32_t sort = g_trackerSortingCVar->m_intValue;

    if (!sort || CGQuestLog::m_numQuestWatches <= 1) {
        return false;
    }

    if (sort == 1) {
        float previous = 0.0f;
        bool sorted = true;

        for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
            C2Vector position;
            float distance = CGQuestLog::GetQuestDistSqr(CGQuestLog::m_questWatches[i].m_questId, position);
            CGQuestLog::m_questWatches[i].m_distance = distance;

            if (sorted) {
                if (previous <= distance) {
                    previous = distance;
                } else {
                    sorted = false;
                }
            }
        }

        if (sorted) {
            return false;
        }
    }

    qsort(CGQuestLog::m_questWatches, CGQuestLog::m_numQuestWatches, sizeof(QuestWatch), QSortQuestWatches);
    return true;
}

// OFFSET: 0x009635C0/0x1406D8240
bool CGQuestLog::AddQuestWatch(int32_t questId, uint32_t duration) {
    int32_t now = _time32(NULL);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || !questId) {
        return false;
    }

    uint32_t slot = 0;

    for (; slot < 50; slot++) {
        if (static_cast<int32_t>(player->m_player->m_playerQuestLog[slot].m_questId) == questId) {
            break;
        }
    }

    if (slot >= 50) {
        return false;
    }

    uint32_t count = CGQuestLog::m_numQuestWatches;

    for (uint32_t i = 0; i < count; i++) {
        if (CGQuestLog::m_questWatches[i].m_questId == questId) {
            if (CGQuestLog::m_questWatches[i].m_expireTime > 0) {
                CGQuestLog::m_questWatches[i].m_expireTime = duration ? duration + now : 0;
            }

            return false;
        }
    }

    if (count >= 25) {
        return false;
    }

    CGQuestLog::m_questWatches[count].m_questId = questId;
    CGQuestLog::m_questWatches[count].m_expireTime = duration ? duration + now : 0;
    CGQuestLog::m_numQuestWatches = count + 1;
    return CGQuestLog::SortQuestWatches();
}

// OFFSET: 0x009636B0/0x1406D8360
int32_t CGQuestLog::SetQuestExpired(uint32_t index) {
    if (index >= CGQuestLog::m_numQuest) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (CGQuestLog::IsQuestCompleteEx(player, index, true, 0, 0)) {
        return 0;
    }

    CGQuestLog::m_quests[index].m_expired = 1;

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    return 1;
}

// OFFSET: 0x00963730
int32_t CGQuestLog::SetQuestExpiredByID(int32_t questId) {
    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        if (CGQuestLog::m_quests[i].m_questId == questId) {
            return CGQuestLog::SetQuestExpired(i);
        }
    }

    return 0;
}

// OFFSET: 0x00963770
int32_t CGQuestLog::GetQuestNumCompletionNPC(int32_t questId) {
    WOWGUID none = WOWGUID::Make(0);
    CGQuestLog::QUESTCOMPLETIONNPC* npcs;

    if (g_questDBCache.GetRecord(questId, none, NULL, NULL, false) && (npcs = CGQuestLog::s_questCompletionNPCs.Ptr(questId, HASHKEY_NONE())) != NULL) {
        return npcs->m_npcs.m_count;
    }

    return 0;
}

// OFFSET: 0x009637D0
int32_t CGQuestLog::GetQuestCompletionNPC(int32_t questId, int32_t index) {
    WOWGUID none = WOWGUID::Make(0);
    CGQuestLog::QUESTCOMPLETIONNPC* npcs;

    if (g_questDBCache.GetRecord(questId, none, NULL, NULL, false) && index >= 0 && (npcs = CGQuestLog::s_questCompletionNPCs.Ptr(questId, HASHKEY_NONE())) != NULL && index < static_cast<int32_t>(npcs->m_npcs.m_count)) {
        return npcs->m_npcs.m_data[index];
    }

    return 0;
}

// OFFSET: 0x00963840
uint32_t CGQuestLog::GetQuestPOIBlobsOnContinent(int32_t questId, TSGrowableArray<uint32_t>& blobs) {
    uint32_t index = 0;

    if (blobs.m_data) {
        SMemFree(blobs.m_data, typeid(uint32_t).raw_name(), SLOG_OBJECT, 0);
    }

    blobs.m_alloc = 0;
    blobs.m_count = 0;
    blobs.m_data = NULL;

    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    int32_t continent = CGPlayer_C::GetNewContinentID();

    for (; index < 50; index++) {
        if (CGQuestLog::m_questPOIs[index].m_questId != questId) {
            continue;
        }

        CGQuestLog::m_questPOIs[index].m_objectiveMask = CGQuestLog::ComputeBlobQuestObjectives(CGQuestLog::m_questPOIs[index].m_questId);

        if (CGQuestLog::m_questPOIs[index].m_objectiveMask != 0xFFFFFFFF) {
            break;
        }
    }

    if (index >= 50) {
        return blobs.m_count;
    }

    QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];

    for (uint32_t i = 0; i < poi->m_blobs.m_count; i++) {
        BLOB* blob = &poi->m_blobs.m_data[i];

        if (((1 << blob->m_objectiveIndex) & poi->m_objectiveMask) && continent == blob->m_mapId) {
            uint32_t value = i;
            blobs.Add(1, &value);
        }
    }

    return blobs.m_count;
}

// OFFSET: 0x00963940
static uint32_t AddUniqueBlobIndex(uint32_t blobIndex, TSGrowableArray<uint32_t>& indices) {
    for (uint32_t i = 0; i < indices.m_count; i++) {
        if (indices.m_data[i] == blobIndex) {
            return i;
        }
    }

    uint32_t* entry = indices.New();
    *entry = blobIndex;
    return indices.m_count;
}

// PROVEN: 605690e6 IDENTICAL/NONE
// OFFSET: 0x009639B0
void CGQuestLog::LoadTrackedQuests(bool force) {
    if (force && !CGQuestLog::m_trackedQuestsLoaded) {
        return;
    }

    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedQuests");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t questId = list.GetNextValue(); questId; questId = list.GetNextValue()) {
        CGQuestLog::AddQuestWatch(questId, 0);
    }

    CGQuestLog::m_trackedQuestsLoaded = 1;

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x00963F00/0x1406D8D40
void CGQuestLog::LeaveWorld() {
    for (CGQuestLog::QUESTCOMPLETIONNPC* entry = CGQuestLog::s_questCompletionNPCs.Head(); entry; entry = CGQuestLog::s_questCompletionNPCs.Next(entry)) {
        entry->m_npcs.Clear();
    }

    CGQuestLog::s_questCompletionNPCs.Clear();
}

// OFFSET: 0x00963FC0/0x1406D8E40
void CGQuestLog::Update(int32_t rebuild) {
    CGResearchFrame::s_enteredWorld = 1;

    if (!rebuild) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
        return;
    }

    if (s_questCallbackCount) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    int32_t filter = g_questLogCollapseHeaderFilterCVar->m_intValue;
    int32_t* oldSortTypes = static_cast<int32_t*>(alloca(CGQuestLog::m_numSortTypes * sizeof(int32_t)));
    uint32_t numOldSortTypes = CGQuestLog::m_numSortTypes;

    for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
        oldSortTypes[i] = CGQuestLog::m_sortTypes[i];

        if ((1 << i) & filter) {
            oldSortTypes[i] = -oldSortTypes[i];
        }
    }

    memset(CGQuestLog::m_quests, 0, sizeof(CGQuestLog::m_quests));
    memset(CGQuestLog::m_sortTypes, 0, sizeof(CGQuestLog::m_sortTypes));
    CGQuestLog::m_numQuest = 0;
    CGQuestLog::m_numSortTypes = 0;
    s_questCallbackCount = 0;
    int32_t numUnknownPOI = 0;
    int32_t unknownPOI[50];
    memset(unknownPOI, 0, sizeof(unknownPOI));
    int32_t numUnknownNPC = 0;
    int32_t unknownNPC[50];
    memset(unknownNPC, 0, sizeof(unknownNPC));
    int32_t timeOffset = CGQuestLog::m_timeOffset;

    for (uint32_t slot = 0; slot < 50; slot++) {
        PlayerQuestLogEntry* entry = slot > 50 ? NULL : &player->m_player->m_playerQuestLog[slot];
        int32_t questId = entry->m_questId;

        if (questId <= 0) {
            continue;
        }

        WOWGUID none = WOWGUID::Make(0);
        QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestLogQueryCallback, NULL, false);

        if (!quest) {
            s_questCallbackCount++;
            continue;
        }

        if (!CGQuestLog::HasQuestPOI(questId)) {
            numUnknownPOI++;
            unknownPOI[slot] = questId;
        }

        CGQuestLog::QUESTCOMPLETIONNPC* npcs = CGQuestLog::s_questCompletionNPCs.Ptr(questId, HASHKEY_NONE());

        if (!npcs || !npcs->m_npcs.m_count) {
            unknownNPC[numUnknownNPC++] = questId;
        }

        CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_questId = questId;
        CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_logSlot = slot;
        CGQuestLog::m_numQuest++;

        if (entry->m_time && !(entry->m_state & 0x2) && timeOffset + static_cast<int32_t>(entry->m_time) - _time32(NULL) - 1 < 0) {
            CGQuestLog::SetQuestExpired(CGQuestLog::m_numQuest - 1);
        }

        uint32_t i;

        for (i = 0; i < CGQuestLog::m_numSortTypes; i++) {
            if (CGQuestLog::m_sortTypes[i] == quest->m_areaIdOrSortId) {
                break;
            }
        }

        if (i == CGQuestLog::m_numSortTypes) {
            CGQuestLog::m_sortTypes[i] = quest->m_areaIdOrSortId;
            CGQuestLog::m_numSortTypes++;
            CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_questId = quest->m_areaIdOrSortId;
            CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_isHeader = 1;
            CGQuestLog::m_numQuest++;
        }
    }

    qsort(CGQuestLog::m_sortTypes, CGQuestLog::m_numSortTypes, sizeof(int32_t), QSortQuestSortTypes);

    if (numOldSortTypes) {
        filter = 0;

        for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
            for (uint32_t j = 0; j < numOldSortTypes; j++) {
                if (oldSortTypes[j] == CGQuestLog::m_sortTypes[i]) {
                    break;
                }

                if (oldSortTypes[j] == -CGQuestLog::m_sortTypes[i]) {
                    filter |= 1 << i;
                    break;
                }
            }
        }
    }

    g_questLogCollapseHeaderFilterCVar->Set(filter, true, false, false, true);
    CGQuestLog::FilterAndSortQuests();

    if (!CGQuestLog::m_trackedQuestsLoaded) {
        CGQuestLog::LoadTrackedQuests(false);
        CGQuestLog::LoadAutoQuestPopUps();
    }

    for (int32_t i = CGQuestLog::m_numQuestWatches - 1; i >= 0; i--) {
        int32_t questId = CGQuestLog::m_questWatches[i].m_questId;
        uint32_t slot = 0;

        if (questId) {
            for (; slot < 50; slot++) {
                if (static_cast<int32_t>(player->m_player->m_playerQuestLog[slot].m_questId) == questId) {
                    break;
                }
            }
        }

        if (!questId || slot >= 50) {
            CGQuestLog::RemoveQuestWatch(questId);
        }
    }

    for (int32_t i = CGQuestLog::m_numAutoQuestPopUps - 1; i >= 0; i--) {
        AutoQuestPopUp* popUp = &CGQuestLog::m_autoQuestPopUps[i];
        PlayerQuestLogEntry* entry = NULL;

        if (popUp->m_questId) {
            for (uint32_t slot = 0; slot < 50; slot++) {
                if (static_cast<int32_t>(player->m_player->m_playerQuestLog[slot].m_questId) == popUp->m_questId) {
                    entry = &player->m_player->m_playerQuestLog[slot];
                    break;
                }
            }
        }

        if (entry) {
            if (popUp->m_shown) {
                popUp->m_shown = 0;
            }
        } else if (!popUp->m_shown) {
            CGQuestLog::RemoveAutoQuestPopUp(popUp->m_questId);
        }
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

    if (numUnknownPOI) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_QUEST_POI_QUERY));
        msg.Put(static_cast<uint32_t>(numUnknownPOI));

        for (uint32_t i = 0; i < 50; i++) {
            if (unknownPOI[i]) {
                msg.Put(static_cast<uint32_t>(unknownPOI[i]));
            }
        }

        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (numUnknownNPC) {
        PlayerQueryQuestCompletionNPCs query;

        for (int32_t i = 0; i < numUnknownNPC; i++) {
            query.m_questIds.Add(1, &unknownNPC[i]);
        }

        ClientServices::Send(&query);
    }

    if (CGQuestLog::m_nextQuestUpdate && _time32(NULL) >= CGQuestLog::m_nextQuestUpdate) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_QUERY_TIME));
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGQuestLog::m_nextQuestUpdate = 0;
    }
}

// OFFSET: 0x009644F0
void CGQuestLog::MergeOverlappingBlobs(int32_t questId, TSGrowableArray<uint32_t>& blobs, TSGrowableArray<uint32_t>& merged) {
    int32_t index = 0;

    if (merged.m_data) {
        SMemFree(merged.m_data, typeid(uint32_t).raw_name(), SLOG_OBJECT, 0);
    }

    merged.m_alloc = 0;
    merged.m_count = 0;
    merged.m_data = NULL;

    if (blobs.m_count < 2) {
        merged.SetCount(blobs.m_count);

        for (uint32_t i = 0; i < blobs.m_count; i++) {
            merged.m_data[i] = blobs.m_data[i];
        }

        return;
    }

    for (; static_cast<uint32_t>(index) < 50; index++) {
        if (CGQuestLog::m_questPOIs[index].m_questId == questId) {
            break;
        }
    }

    if (index >= 50) {
        return;
    }

    for (uint32_t i = 0; i < blobs.m_count - 1; i++) {
        for (uint32_t j = i + 1; j < blobs.m_count; j++) {
            uint32_t a = blobs.m_data[i];

            if (a == 0xFFFFFFFF) {
                continue;
            }

            uint32_t b = blobs.m_data[j];

            if (b == 0xFFFFFFFF) {
                continue;
            }

            CRect boundsA;
            CRect boundsB;
            QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[index];
            CGQuestLog::GetBlobBounds(poi->m_blobs.m_data[a].m_points, boundsA);
            CGQuestLog::GetBlobBounds(poi->m_blobs.m_data[b].m_points, boundsB);
            float maxX = boundsB.maxX <= boundsA.maxX ? boundsB.maxX : boundsA.maxX;
            float maxY = boundsB.maxY <= boundsA.maxY ? boundsB.maxY : boundsA.maxY;
            float minX = boundsA.minX <= boundsB.minX ? boundsB.minX : boundsA.minX;
            float minY = boundsA.minY <= boundsB.minY ? boundsB.minY : boundsA.minY;

            if (maxY <= minY || maxX <= minX) {
                continue;
            }

            uint32_t small;
            uint32_t large;

            if ((boundsB.maxY - boundsB.minY) * (boundsB.maxX - boundsB.minX) <= (boundsA.maxX - boundsA.minX) * (boundsA.maxY - boundsA.minY)) {
                small = blobs.m_data[j];
                large = blobs.m_data[i];
            } else {
                small = blobs.m_data[i];
                large = blobs.m_data[j];
            }

            uint32_t inside = 0;
            TSGrowableArray<C3Vector> polygon;
            polygon.SetCount(poi->m_blobs.m_data[large].m_points.m_count);

            for (uint32_t k = 0; k < poi->m_blobs.m_data[large].m_points.m_count; k++) {
                polygon.m_data[k].x = static_cast<float>(poi->m_blobs.m_data[large].m_points.m_data[k].m_x);
                polygon.m_data[k].y = static_cast<float>(poi->m_blobs.m_data[large].m_points.m_data[k].m_y);
                polygon.m_data[k].z = 0.0f;
            }

            for (uint32_t k = 0; k < poi->m_blobs.m_data[small].m_points.m_count; k++) {
                C3Vector point(static_cast<float>(poi->m_blobs.m_data[small].m_points.m_data[k].m_x), static_cast<float>(poi->m_blobs.m_data[small].m_points.m_data[k].m_y), 0.0f);

                if (NTempest::Intersect(point, polygon.m_data, polygon.m_count, C3Vector::AXIS_Z)) {
                    inside++;
                }
            }

            if (static_cast<double>(inside) / static_cast<double>(poi->m_blobs.m_data[small].m_points.m_count) > 0.25) {
                for (uint32_t k = 0; k < blobs.m_count; k++) {
                    if (blobs.m_data[k] == small) {
                        blobs.m_data[k] = 0xFFFFFFFF;
                        break;
                    }
                }
            }
        }
    }

    for (uint32_t i = 0; i < blobs.m_count; i++) {
        if (blobs.m_data[i] != 0xFFFFFFFF) {
            AddUniqueBlobIndex(blobs.m_data[i], merged);
        }
    }
}

// OFFSET: 0x00964AA0/0x1406DA140
void CGQuestLog::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_QUERY_TIME_RESPONSE);

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];
        poi->m_questId = 0;
        poi->m_blobs.Clear();
    }

    CGQuestLog::m_trackedQuestsLoaded = 0;
    ConsoleCommandUnregister("UpdatePOI");
}

// OFFSET: 0x00964B00/0x1406DA1D0
void CGQuestLog::EnterWorld() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_TIME));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_questCallbackCount = 0;
    CGQuestLog::Update(1);
}

// OFFSET: 0x00964B80
int32_t CGQuestLog::FindEmptyQuestPOISlot(int32_t questId) {
    if (!ClntObjMgrGetActivePlayerObj() || questId <= 0) {
        return -1;
    }

    for (int32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];

        if (poi->m_questId == questId) {
            return i;
        }

        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!CGQuestLog::IsOnQuest(player, poi->m_questId)) {
            poi->m_questId = 0;
            poi->m_blobs.Clear();
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x00964C40/0x1406DA330
void CGQuestLog::UpdateQuestPOIFromUserServer() {
    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];

        for (uint32_t j = 0; j < poi->m_blobs.m_count; j++) {
            poi->m_blobs.m_data[j].m_points.Clear();
        }

        poi->m_blobs.Clear();
        poi->m_questId = 0;
        poi->m_iconMapId = 0;
        poi->m_iconObjectiveIndex = 0;
        poi->m_iconFloor = 0;
        poi->m_iconX = 0;
        poi->m_iconY = 0;
        poi->m_hasIcon = 0;
        poi->m_visible = 0;
        poi->m_objectiveMask = 0;
    }

    CGQuestLog::Update(1);
}

// OFFSET: 0x00964E30/0x1406DA600
void CGQuestLog::InitializeGame() {
    CGQuestLog::m_selectedQuest = 0;
    CGQuestLog::m_superTrackedQuest = 0;
    CGQuestLog::m_abandonQuest = 0;
    CGQuestLog::m_numQuest = 0;
    CGQuestLog::m_numShownQuests = 0;
    memset(CGQuestLog::m_quests, 0, sizeof(CGQuestLog::m_quests));
    CGQuestLog::m_numQuestWatches = 0;
    memset(CGQuestLog::m_questWatches, 0, sizeof(CGQuestLog::m_questWatches));
    CGQuestLog::m_timeOffset = 0;
    CGQuestLog::m_nextQuestUpdate = 0;

    for (uint32_t i = 0; i < 50; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];
        poi->m_questId = 0;
        poi->m_iconMapId = 0;
        poi->m_iconObjectiveIndex = 0;
        poi->m_iconFloor = 0;
        poi->m_iconX = 0;
        poi->m_iconY = 0;
        poi->m_hasIcon = 0;
        poi->m_visible = 0;
        poi->m_objectiveMask = 0;
    }

    CGxTexFlags flags;
    flags.m_renderTarget = 1;
    ClientServices::SetMessageHandler(SMSG_QUERY_TIME_RESPONSE, Packet_SMSG_QUERY_TIME_RESPONSE, NULL);
    s_cvPOIShiftComplete = CVar::Register("POIShiftComplete", NULL, 0, "0.3", NULL, 0, false, NULL, false);
    ConsoleCommandRegister("UpdatePOI", UpdateQuestPOIHandler, CATEGORY_GAME, "Updates Quest POI from User Server", NULL);
}

// OFFSET: 0x00964F30/0x1406DA720
int32_t CGQuestLog::GetQuestLeaderBoardByID(int32_t questId, QuestLogLeaderBoardParams* params) {
    if (!params) {
        return 0;
    }

    char* text = params->m_text;
    params->m_text[0] = '\0';
    char* type = params->m_type;
    params->m_type[0] = '\0';
    params->m_complete = 0;
    int32_t showProgress = params->m_showProgress;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (!questId) {
        questId = CGQuestLog::m_selectedQuest;
    }

    int32_t requireLog = params->m_requireLogEntry;
    int32_t slot = -1;

    if (requireLog) {
        int32_t index = CGQuestLog::GetQuestIndex(questId) - 1;

        if (index < 0) {
            int32_t selected = CGQuestLog::GetSelectedLogEntry();
            slot = selected < 0 ? 0 : CGQuestLog::m_quests[selected].m_logSlot;
        } else {
            slot = CGQuestLog::GetQuestLogSlot(index);
        }
    }

    if (questId <= 0 || (requireLog && slot < 0)) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, params->m_questCallback, params->m_param, false);
    PlayerQuestLogEntry* entry = static_cast<uint32_t>(slot) > 50 ? NULL : &player->m_player->m_playerQuestLog[slot];

    if (!quest || (params->m_requireLogEntry && !entry)) {
        return 0;
    }

    int32_t found = -1;

    if (quest->m_endText[0]) {
        found = 0;

        if (!params->m_index) {
            SStrCopy(text, quest->m_endText, 4096);
            SStrCopy(type, "event", 64);

            if (entry) {
                params->m_complete = entry->m_state & 1;
            }

            return 1;
        }
    }

    char buffer[4096];
    int32_t counter = -1;

    for (uint32_t i = 0; found < params->m_index && i < 4; i++) {
        if (!quest->m_objectiveId[i]) {
            if (params->m_countEmpty) {
                found++;
            }

            continue;
        }

        counter++;

        if (++found != params->m_index) {
            continue;
        }

        int32_t count = entry ? entry->m_counts[counter] : 0;
        int32_t objective = quest->m_objectiveId[i];
        const char* objectiveText = quest->m_objectiveText[i];

        if (objective >= 0 && !*objectiveText) {
            WOWGUID creatureGuid = WOWGUID::Make(0);
            CreatureStats_C* creature = g_creatureDBCache.GetRecord(objective, creatureGuid, params->m_creatureCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(quest->m_objectiveRequiredCount[i]);

            if (showProgress) {
                const char* format = FrameScript_GetText("QUEST_MONSTERS_KILLED", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, creature ? creature->GetName(plural) : " ", count, quest->m_objectiveRequiredCount[i]);
                params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
            } else {
                const char* format = FrameScript_GetText("QUEST_MONSTERS_KILLED_NOPROGRESS", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, creature ? creature->GetName(plural) : " ", quest->m_objectiveRequiredCount[i]);
            }
        } else if (*objectiveText) {
            if (showProgress) {
                SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_OBJECTS_FOUND", -1, GENDER_NOT_APPLICABLE), objectiveText, count, quest->m_objectiveRequiredCount[i]);
                params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
            } else {
                SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_OBJECTS_FOUND_NOPROGRESS", -1, GENDER_NOT_APPLICABLE), objectiveText, quest->m_objectiveRequiredCount[i]);
            }
        } else {
            WOWGUID objectGuid = WOWGUID::Make(0);
            GameObjectStats_C* object = g_gameObjectDBCache.GetRecord(objective & 0x7FFFFFFF, objectGuid, params->m_objectCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(quest->m_objectiveRequiredCount[i]);

            if (showProgress) {
                const char* format = FrameScript_GetText("QUEST_OBJECTS_FOUND", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, object ? object->GetName(plural) : " ", count, quest->m_objectiveRequiredCount[i]);
                params->m_complete = count >= static_cast<int32_t>(quest->m_objectiveRequiredCount[i]);
            } else {
                const char* format = FrameScript_GetText("QUEST_OBJECTS_FOUND_NOPROGRESS", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, object ? object->GetName(plural) : " ", quest->m_objectiveRequiredCount[i]);
            }
        }

        SStrCopy(text, buffer, 4096);
        SStrCopy(type, static_cast<int32_t>(quest->m_objectiveId[i]) >= 0 ? "monster" : "object", 64);
        return 1;
    }

    for (uint32_t i = 0; i < 6 && found < params->m_index; i++) {
        int32_t itemId = quest->m_collectItemId[i];

        if (itemId <= 0 || (static_cast<uint32_t>(itemId) == quest->m_startingItemId && static_cast<int32_t>(quest->m_collectItemCount[i]) <= 1)) {
            if (params->m_countEmpty) {
                found++;
            }

            continue;
        }

        if (++found != params->m_index) {
            continue;
        }

        WOWGUID itemGuid = WOWGUID::Make(0);
        ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_collectItemId[i], itemGuid, params->m_itemCallback, params->m_param, false);
        uint32_t plural = FrameScript_GetPluralIndex(quest->m_collectItemCount[i]);

        if (showProgress) {
            int32_t required = quest->m_collectItemCount[i];
            int32_t have = player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8) >= required ? required : player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8);
            const char* format = FrameScript_GetText("QUEST_ITEMS_NEEDED", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", have, quest->m_collectItemCount[i]);
            params->m_complete = have >= static_cast<int32_t>(quest->m_collectItemCount[i]);
        } else {
            const char* format = FrameScript_GetText("QUEST_ITEMS_NEEDED_NOPROGRESS", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", quest->m_collectItemCount[i]);
        }

        SStrCopy(text, buffer, 4096);
        SStrCopy(type, "item", 64);
        return 1;
    }

    if (!CGQuestLog::IsQuestCompleteEx(ClntObjMgrGetActivePlayerObj(), CGQuestLog::GetQuestIndex(questId) - 1, true, 0, 0)) {
        for (uint32_t i = 0; found < params->m_index && i < 4; i++) {
            if (static_cast<int32_t>(quest->m_intermediateItemId[i]) <= 0) {
                if (params->m_countEmpty) {
                    found++;
                }

                continue;
            }

            if (++found != params->m_index) {
                continue;
            }

            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_intermediateItemId[i], itemGuid, params->m_itemCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(1);
            int32_t have = player->m_bag.GetItemTypeCount(quest->m_intermediateItemId[i], 8);
            const char* format = FrameScript_GetText("QUEST_INTERMEDIATE_ITEMS_NEEDED", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", have);
            params->m_complete = have >= 1;
            SStrCopy(text, buffer, 4096);
            SStrCopy(type, "item", 64);
            return 1;
        }
    }

    if (quest->m_requiredPlayersKilled) {
        int32_t count = entry ? entry->m_counts[0] : 0;

        if (showProgress) {
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_PLAYERS_KILLED", -1, GENDER_NOT_APPLICABLE), count, quest->m_requiredPlayersKilled);
            params->m_complete = count >= static_cast<int32_t>(quest->m_requiredPlayersKilled);
        } else {
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_PLAYERS_KILLED_NOPROGRESS", -1, GENDER_NOT_APPLICABLE), quest->m_requiredPlayersKilled);
        }

        SStrCopy(text, buffer, 4096);
        SStrCopy(type, "player", 64);
        return 1;
    }

    if (quest->m_requiredSpell) {
        const char* format = FrameScript_GetText("QUEST_SPELL_NEEDED", -1, GENDER_NOT_APPLICABLE);
        SpellRec* spell = g_spellDB.GetRecord(quest->m_requiredSpell);

        if (spell && spell->m_name) {
            SStrPrintf(buffer, sizeof(buffer), format, static_cast<const char*>(spell->m_name));
            SStrCopy(text, buffer, 4096);
            SStrCopy(type, "spell", 64);
            uint32_t spellId = quest->m_requiredSpell;
            params->m_complete = spellId <= static_cast<uint32_t>(g_spellDB.m_maxID) && ((1 << (spellId & 0x1F)) & s_knownSpellBits.m_bits.m_data[spellId >> 5]);
        }

        return 1;
    }

    FactionRec* faction;

    if (quest->m_friendlyFactionId && (faction = g_factionDB.GetRecord(quest->m_friendlyFactionId)) != NULL) {
        int32_t amount = quest->m_friendlyFactionAmount;
        int32_t rank;

        if (amount >= 42000) {
            rank = 7;
        } else if (amount >= 21000) {
            rank = 6;
        } else if (amount >= 9000) {
            rank = 5;
        } else if (amount >= 3000) {
            rank = 4;
        } else if (amount >= 0) {
            rank = 3;
        } else if (amount >= -3000) {
            rank = 2;
        } else {
            rank = amount >= -6000;
        }

        char needed[64];
        char current[64];
        SStrPrintf(buffer, sizeof(buffer), "FACTION_STANDING_LABEL%d", rank + 1);
        SStrCopy(needed, player->GetFrameScriptText(buffer, -1), sizeof(needed));

        if (showProgress) {
            SStrPrintf(buffer, sizeof(buffer), "FACTION_STANDING_LABEL%d", CGReputationInfo::GetFactionStandingReaction(quest->m_friendlyFactionId) + 1);
            SStrCopy(current, player->GetFrameScriptText(buffer, -1), sizeof(current));
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_FACTION_NEEDED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(faction->m_name), current, needed);
            params->m_complete = GetRepListRepValue(quest->m_friendlyFactionId) >= static_cast<int32_t>(quest->m_friendlyFactionAmount);
        } else {
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("QUEST_FACTION_NEEDED_NOPROGRESS", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(faction->m_name), needed);
        }

        SStrCopy(text, buffer, 4096);
        SStrCopy(type, "reputation", 64);
        return 1;
    }

    if (!CGQuestLog::IsQuestCompleteEx(ClntObjMgrGetActivePlayerObj(), CGQuestLog::GetQuestIndex(questId) - 1, true, 0, 0)) {
        return 0;
    }

    params->m_complete = 1;
    char objectives[3000];
    WOWGUID playerGuid = ClntObjMgrGetActivePlayer();
    QuestParserParseText(quest->m_objText, objectives, sizeof(objectives), playerGuid, NULL, 0);
    SStrCopy(text, objectives, 4096);
    SStrCopy(type, "log", 64);
    return 1;
}

// OFFSET: 0x00965CE0/0x1406DB900
int32_t CGQuestLog::GetQuestLogItemDropByID(int32_t questId, QuestLogLeaderBoardParams* params) {
    if (!params) {
        return 0;
    }

    char* text = params->m_text;
    params->m_text[0] = '\0';
    char* type = params->m_type;
    params->m_type[0] = '\0';
    params->m_complete = 0;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (!questId) {
        questId = CGQuestLog::m_selectedQuest;
    }

    int32_t requireLog = params->m_requireLogEntry;
    int32_t slot = -1;

    if (requireLog) {
        int32_t index = CGQuestLog::GetQuestIndex(questId) - 1;

        if (index < 0) {
            int32_t selected = CGQuestLog::GetSelectedLogEntry();
            slot = selected < 0 ? 0 : CGQuestLog::m_quests[selected].m_logSlot;
        } else {
            slot = CGQuestLog::GetQuestLogSlot(index);
        }
    }

    if (questId <= 0 || (requireLog && slot < 0)) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, params->m_questCallback, params->m_param, false);
    PlayerQuestLogEntry* entry = static_cast<uint32_t>(slot) > 50 ? NULL : &player->m_player->m_playerQuestLog[slot];

    if (!quest || (params->m_requireLogEntry && !entry)) {
        return 0;
    }

    int32_t found = -1;

    for (uint32_t i = 0; found < params->m_index; i++) {
        if (static_cast<int32_t>(quest->m_intermediateItemId[i]) <= 0) {
            if (params->m_countEmpty) {
                found++;
            }
        } else if (++found == params->m_index) {
            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_intermediateItemId[i], itemGuid, params->m_itemCallback, params->m_param, false);
            uint32_t plural = FrameScript_GetPluralIndex(1);
            char buffer[4096];

            if (params->m_showProgress) {
                int32_t required = quest->m_intermediateItemCount[i];
                int32_t have = required;

                if (player->m_bag.GetItemTypeCount(quest->m_intermediateItemId[i], 8) < required) {
                    have = player->m_bag.GetItemTypeCount(quest->m_intermediateItemId[i], 8);
                }

                const char* format = FrameScript_GetText("QUEST_ITEMS_NEEDED", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", have, quest->m_intermediateItemCount[i]);
                params->m_complete = have >= static_cast<int32_t>(quest->m_intermediateItemCount[i]);
            } else {
                const char* format = FrameScript_GetText("QUEST_ITEMS_NEEDED_NOPROGRESS", -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", quest->m_intermediateItemCount[i]);
            }

            SStrCopy(text, buffer, 4096);
            SStrCopy(type, "item", 64);
            return 1;
        }

        if (i + 1 >= 4) {
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x00965FE0/0x1406DBCE0
int32_t CGQuestLog::GetQuestLeaderBoardForItem(const CGPlayer_C* player, int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params) {
    if (!params || !player) {
        return 0;
    }

    params->m_text[0] = '\0';
    char* text = params->m_text;
    params->m_type[0] = '\0';
    char* type = params->m_type;
    params->m_complete = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, params->m_questCallback, params->m_param, false);
    PlayerQuestLogEntry* entry = questId ? player->GetQuestLogEntryByID(questId) : NULL;

    if (!quest || !entry) {
        return 0;
    }

    uint32_t i = 0;

    for (; static_cast<int32_t>(quest->m_collectItemId[i]) != itemId || (quest->m_collectItemId[i] == quest->m_startingItemId && static_cast<int32_t>(quest->m_collectItemCount[i]) <= 1);) {
        if (++i >= 6) {
            return 0;
        }
    }

    WOWGUID itemGuid = WOWGUID::Make(0);
    ItemRecSparse* item = g_itemSparseDB.GetRecord(quest->m_collectItemId[i], itemGuid, params->m_itemCallback, params->m_param, false);
    uint32_t plural = FrameScript_GetPluralIndex(quest->m_collectItemCount[i]);
    int32_t required = quest->m_collectItemCount[i];
    int32_t have;

    if (player == ClntObjMgrGetActivePlayerObj()) {
        have = player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8) < required ? player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8) : required;
    } else {
        have = CGQuestLog::GetNumQuestItems(player, questId, itemId) < required ? CGQuestLog::GetNumQuestItems(player, questId, itemId) : required;
    }

    char buffer[4096];
    const char* format = FrameScript_GetText("QUEST_ITEMS_NEEDED", -1, GENDER_NOT_APPLICABLE);
    SStrPrintf(buffer, sizeof(buffer), format, item ? item->GetName(plural) : " ", have, quest->m_collectItemCount[i]);
    params->m_complete = have >= static_cast<int32_t>(quest->m_collectItemCount[i]);
    SStrCopy(text, buffer, 4096);
    SStrCopy(type, "item", 64);
    return 1;
}

// OFFSET: 0x00967200
void CGQuestLog::OnQuestPOIResponse(CDataStore* msg) {
    if (!msg) {
        return;
    }

    uint32_t count;
    msg->Get(count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        int32_t questId;
        int32_t numBlobs;
        msg->Get(questId);
        msg->Get(numBlobs);
        bool known = CGQuestLog::HasQuestPOI(questId);
        uint32_t slot = CGQuestLog::FindEmptyQuestPOISlot(questId);
        QuestPOIInfo* poi = NULL;

        if (!known && slot <= 49) {
            poi = &CGQuestLog::m_questPOIs[slot];
            poi->m_questId = questId;
            poi->m_blobs.SetCount(numBlobs);
            poi->m_iconMapId = 0;
            poi->m_iconObjectiveIndex = 0;
            poi->m_iconFloor = 0;
            poi->m_iconX = 0;
            poi->m_iconY = 0;
            poi->m_hasIcon = 0;
            poi->m_visible = 0;
            poi->m_objectiveMask = 0;
        }

        for (int32_t j = 0; j < numBlobs; j++) {
            int32_t id;
            int32_t objectiveIndex;
            int32_t mapId;
            int32_t worldMapAreaID;
            int32_t floor;
            int32_t priority;
            int32_t flags;
            int32_t numPoints;
            msg->Get(id);
            msg->Get(objectiveIndex);
            msg->Get(mapId);
            msg->Get(worldMapAreaID);
            msg->Get(floor);
            msg->Get(priority);
            msg->Get(flags);
            msg->Get(numPoints);

            if (poi) {
                BLOB* blob = &poi->m_blobs.m_data[j];
                blob->m_points.SetCount(numPoints);
                blob->m_id = id;
                blob->m_objectiveIndex = objectiveIndex;
                blob->m_mapId = mapId;
                blob->m_worldMapAreaID = worldMapAreaID;
                blob->m_floor = floor;
                blob->m_flags = flags;
                blob->m_priority = priority;
                blob->m_center.m_x = 0;
                blob->m_center.m_y = 0;
            }

            for (int32_t k = 0; k < numPoints; k++) {
                int32_t x;
                int32_t y;
                msg->Get(x);
                msg->Get(y);

                if (poi) {
                    BLOB* blob = &poi->m_blobs.m_data[j];
                    blob->m_points.m_data[k].m_x = x;
                    blob->m_points.m_data[k].m_y = y;
                    blob->m_center.m_x += x;
                    blob->m_center.m_y += y;
                }
            }

            if (numPoints > 1 && poi) {
                BLOB* blob = &poi->m_blobs.m_data[j];
                blob->m_center.m_x /= numPoints;
                blob->m_center.m_y /= numPoints;
            }
        }
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_POI_UPDATE);
}

// OFFSET: 0x00967840/0x1406DDC10
int32_t CGQuestLog::GetQuestPOILeaderBoardByID(int32_t questId, QuestLogLeaderBoardParams* params) {
    if (!params) {
        return 0;
    }

    params->m_text[0] = '\0';
    params->m_type[0] = '\0';
    params->m_complete = 0;

    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    if (!questId) {
        questId = CGQuestLog::m_selectedQuest;
    }

    if (questId <= 0) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, params->m_questCallback, params->m_param, false);

    if (!quest) {
        return 0;
    }

    int32_t index = params->m_index;

    if (index < 4) {
        if (index >= 0) {
            return CGQuestLog::GetQuestLeaderBoardForKill(questId, quest->m_objectiveId[index], params);
        }
    } else if (index < 10) {
        return CGQuestLog::GetQuestLeaderBoardForItem(questId, quest->m_collectItemId[index - 4], params);
    } else if (index < 16) {
        return CGQuestLog::GetQuestLeaderBoardForItem(questId, quest->m_intermediateItemId[index - 10], params);
    }

    if (index != 16 && index != 17) {
        return 0;
    }

    int32_t logIndex = CGQuestLog::GetQuestIndex(questId) - 1;

    if (logIndex < 0) {
        return 0;
    }

    PlayerQuestLogEntry* entry = ClntObjMgrGetActivePlayerObj()->GetQuestLogEntry(CGQuestLog::GetQuestLogSlot(logIndex));

    if (!entry || !quest->m_endText[0]) {
        return 0;
    }

    SStrCopy(params->m_text, quest->m_endText, 4096);
    SStrCopy(params->m_type, "event", 64);
    params->m_complete = entry->m_state & 1;
    return 1;
}

// OFFSET: 0x00967B70
void CGQuestLog::AddQuestCompletionNPC(int32_t questId, int32_t npcId) {
    CGQuestLog::QUESTCOMPLETIONNPC* entry = CGQuestLog::s_questCompletionNPCs.Ptr(questId, HASHKEY_NONE());

    if (!entry) {
        entry = CGQuestLog::s_questCompletionNPCs.New(questId, HASHKEY_NONE(), 0, 0);
    }

    for (uint32_t i = 0; i < entry->m_npcs.m_count; i++) {
        if (entry->m_npcs.m_data[i] == npcId) {
            return;
        }
    }

    *entry->m_npcs.New() = npcId;
}
