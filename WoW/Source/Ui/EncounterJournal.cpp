#include "WoW/Source/Ui/EncounterJournal.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/JournalEncounterCreatureRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterItemRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterSectionRec.h"
#include "WoW/Source/DB/Rec/JournalInstanceRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/ItemTransmogrify.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Transmogrify.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldMap.h"

// STATIC: 0x00C0E590/0x140A087C0
static const int32_t s_inventoryTypeOrder[29] = { 0, 10, 11, 12, 0, 14, 17, 18, 19, 15, 16, 20, 21, 4, 6, 5, 13, 1, 0, 0, 14, 2, 3, 7, 0, 8, 5, 0, 9 };

// STATIC: 0x00D17804/0x140BD2828
static const char* s_journalLinkColor = "ff66bbff";

// STATIC: 0x00D1782C/0x140BD2878
int32_t CGEncounterJournal::s_classFilter = -1;

// STATIC: 0x00D17830/0x140BD287C
bool CGEncounterJournal::s_lootDirty = true;

// STATIC: 0x00D179D8
EJMapEncounters CGEncounterJournal::s_mapEncounters;

// STATIC: 0x00F76CE0/0x140E6CE70
static char s_linkBuffer[1024];

// STATIC: 0x00F770E0/0x140E6D270
static int32_t s_tierID;

// STATIC: 0x00F770E4/0x140E6D274
int32_t CGEncounterJournal::s_instanceID;

// STATIC: 0x00F770E8/0x140E6D278
EJInstance* CGEncounterJournal::s_instance;

// STATIC: 0x00F770EC/0x140E6D280
int32_t CGEncounterJournal::s_encounterID;

// STATIC: 0x00F770F0/0x140E6D284
int32_t CGEncounterJournal::s_difficulty;

// STATIC: 0x00F770F4/0x140E6D288
char* CGEncounterJournal::s_searchString;

// STATIC: 0x00F770F8/0x140E6D290
bool CGEncounterJournal::s_searchTruncated;

// STATIC: 0x00F77144/0x140E6D2C0
EJSearchResultList CGEncounterJournal::s_searchResults;

// STATIC: 0x00F77150
EJInstanceList CGEncounterJournal::s_instances;

// STATIC: 0x00F7715C/0x140E6D2F0
EJLootList CGEncounterJournal::s_loot;

// OFFSET: 0x00A06CA0/0x1407A6B70
void CGEncounterJournal::ResetState() {
    s_tierID = 0;
    CGEncounterJournal::s_instanceID = 0;
    CGEncounterJournal::s_instance = NULL;
    CGEncounterJournal::s_encounterID = 0;
    CGEncounterJournal::s_classFilter = -1;
    CGEncounterJournal::s_difficulty = 0;
    CGEncounterJournal::s_lootDirty = true;

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = NULL;
}

// OFFSET: 0x00A06D00/0x1407A6BE0
const char* CGEncounterJournal::BuildLink(int32_t type, int32_t id, const char* name) {
    SStrPrintf(s_linkBuffer, sizeof(s_linkBuffer), "|c%s|Hjournal:%d:%d:%d|h[%s]|h%s", s_journalLinkColor, type, id, CGEncounterJournal::s_difficulty + 1, name, *s_journalLinkColor ? "|r" : "");
    return s_linkBuffer;
}

// OFFSET: 0x00A06DC0/0x1407A6C50
void CGEncounterJournal::ItemLoadedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_EJ_LOOT_DATA_RECIEVED, "%d", id);
    }
}

// OFFSET: 0x00A06F80/0x1407A6E20
JournalEncounterCreatureRec* CGEncounterJournal::GetCreature(int32_t encounterID, uint32_t index) {
    if (index > 6) {
        return NULL;
    }

    int32_t low = 0;
    int32_t high = g_journalEncounterCreatureDB.m_numRecords - 1;

    while (low <= high) {
        int32_t mid = low + (high - low) / 2;
        JournalEncounterCreatureRec* rec = g_journalEncounterCreatureDB.GetRecordByIndex(mid);

        if (encounterID < rec->m_journalEncounterID) {
            high = mid - 1;
        } else if (encounterID > rec->m_journalEncounterID) {
            low = mid + 1;
        } else {
            if (index != rec->m_orderIndex) {
                rec = g_journalEncounterCreatureDB.GetRecordByIndex(index + mid - rec->m_orderIndex);
            }

            if (!rec || rec->m_journalEncounterID != encounterID) {
                return NULL;
            }

            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x00A07BA0
EJInstance* CGEncounterJournal::FindInstance(int32_t instanceID) {
    if (instanceID == CGEncounterJournal::s_instanceID && CGEncounterJournal::s_instance) {
        return CGEncounterJournal::s_instance;
    }

    for (EJInstanceList::iterator it = CGEncounterJournal::s_instances.begin(); it != CGEncounterJournal::s_instances.end(); ++it) {
        if (it->m_instance->m_id == instanceID) {
            return &*it;
        }
    }

    return NULL;
}

// OFFSET: 0x00A07BE0/0x1407A7D30
JournalEncounterRec* CGEncounterJournal::GetEncounterByIndex(int32_t instanceID, int32_t index) {
    if (index < 0) {
        return NULL;
    }

    int32_t current = 0;
    int32_t mask = 1 << CGEncounterJournal::s_difficulty;
    EJInstance* instance = CGEncounterJournal::FindInstance(instanceID);

    if (!instance) {
        return NULL;
    }

    for (EJEncounterList::iterator it = instance->m_encounters.begin(); it != instance->m_encounters.end(); ++it) {
        if (mask & (*it)->m_difficultyMask) {
            if (current == index) {
                return *it;
            }

            current++;
        }
    }

    return NULL;
}

// OFFSET: 0x00A07C50/0x1407A7DC0
void CGEncounterJournal::AddSearchResult(int32_t id, int32_t type, const char* name, const char* search) {
    if (!SStrStrUTF8I(name, search)) {
        return;
    }

    if (CGEncounterJournal::s_searchResults.size() >= 500) {
        CGEncounterJournal::s_searchTruncated = true;
        return;
    }

    EJSearchResultList::iterator it = CGEncounterJournal::s_searchResults.begin();

    while (it != CGEncounterJournal::s_searchResults.end() && it->m_type <= type) {
        ++it;
    }

    EJSearchResult result;
    result.m_id = id;
    result.m_type = type;
    result.m_name = name;
    CGEncounterJournal::s_searchResults.insert(it, result);
}

// OFFSET: 0x00A07D10/0x1407A7ED0
void CGEncounterJournal::RefineSearch(const char* search) {
    for (EJSearchResultList::iterator it = CGEncounterJournal::s_searchResults.begin(); it != CGEncounterJournal::s_searchResults.end(); ++it) {
        if (!it->m_name || !SStrStrUTF8I(it->m_name, search)) {
            it = CGEncounterJournal::s_searchResults.erase(it);
            --it;
        }
    }

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = SStrDupA(search, __FILE__, __LINE__);
}

// OFFSET: 0x00A07DB0/0x1407A7FA0
void CGEncounterJournal::SearchSections(int32_t sectionID, const char* search) {
    for (JournalEncounterSectionRec* section = g_journalEncounterSectionDB.GetRecord(sectionID); section; section = g_journalEncounterSectionDB.GetRecord(section->m_firstChildSectionID)) {
        if (section->m_type) {
            CGEncounterJournal::AddSearchResult(section->m_id, 3, section->m_title, search);
        }

        CGEncounterJournal::SearchSections(section->m_nextSiblingSectionID, search);
    }
}

// OFFSET: 0x00A07E10
int32_t CGEncounterJournal::GetInstanceByIndex(int32_t index, int32_t instanceType) {
    for (EJInstanceList::iterator it = CGEncounterJournal::s_instances.begin(); it != CGEncounterJournal::s_instances.end(); ++it) {
        if (it->m_instanceType == instanceType) {
            if (!index) {
                return it->m_instance->m_id;
            }

            index--;
        }
    }

    return 0;
}

// OFFSET: 0x00A08490/0x1407A89E0
bool CGEncounterJournal::SelectInstance(int32_t instanceID) {
    CGEncounterJournal::s_lootDirty = true;
    CGEncounterJournal::s_encounterID = 0;

    if (instanceID != CGEncounterJournal::s_instanceID) {
        bool wasRaid = false;

        if (CGEncounterJournal::s_instanceID) {
            EJInstance* instance = CGEncounterJournal::FindInstance(CGEncounterJournal::s_instanceID);

            if (instance) {
                wasRaid = instance->m_instanceType == 2;
            }
        }

        CGEncounterJournal::s_instance = CGEncounterJournal::FindInstance(instanceID);

        if (!CGEncounterJournal::s_instance) {
            CGEncounterJournal::s_instanceID = 0;
            return false;
        }

        bool isRaid = false;
        CGEncounterJournal::s_instanceID = instanceID;

        if (instanceID) {
            EJInstance* instance = CGEncounterJournal::FindInstance(instanceID);

            if (instance) {
                isRaid = instance->m_instanceType == 2;
            }
        }

        if (wasRaid != isRaid) {
            CGEncounterJournal::s_difficulty = 0;
            CGEncounterJournal::s_lootDirty = true;
            FrameScript_SignalEvent(SCRIPT_EVENT_EJ_DIFFICULTY_UPDATE, "%d", 1);
        }
    }

    return true;
}

// OFFSET: 0x00A088C0/0x1407A9000
void CGEncounterJournal::BuildInstances() {
    for (int32_t i = 0; i < g_journalInstanceDB.m_numRecords; i++) {
        JournalInstanceRec* rec = g_journalInstanceDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        EJInstance instance;
        instance.m_instance = rec;
        instance.m_instanceType = 0;
        instance.m_difficultyMask = 0;
        MapRec* map = g_mapDB.GetRecord(rec->m_mapID);

        if (map) {
            instance.m_instanceType = map->m_instanceType;
        }

        EJInstanceList::iterator it = CGEncounterJournal::s_instances.begin();

        while (it != CGEncounterJournal::s_instances.end() && it->m_instance->m_orderIndex < rec->m_orderIndex) {
            ++it;
        }

        CGEncounterJournal::s_instances.insert(it, instance);
    }

    EJInstance* instance = NULL;

    for (int32_t i = 0; i < g_journalEncounterDB.m_numRecords; i++) {
        JournalEncounterRec* encounter = g_journalEncounterDB.GetRecordByIndex(i);

        if (!encounter) {
            continue;
        }

        if (!instance || instance->m_instance->m_id != encounter->m_journalInstanceID) {
            instance = CGEncounterJournal::FindInstance(encounter->m_journalInstanceID);
        }

        if (instance) {
            instance->m_difficultyMask |= encounter->m_difficultyMask;
            EJEncounterList::iterator it = instance->m_encounters.begin();

            while (it != instance->m_encounters.end() && encounter->m_orderIndex >= (*it)->m_orderIndex) {
                ++it;
            }

            instance->m_encounters.insert(it, encounter);
        }

        std::pair<int32_t, int32_t> key(encounter->m_dungeonMapID ? encounter->m_dungeonMapID : -1, encounter->m_worldMapAreaID);
        CGEncounterJournal::s_mapEncounters[key].push_back(encounter);
    }
}

// OFFSET: 0x00A08BF0/0x1407A9480
void CGEncounterJournal::ClearInstances() {
    for (EJInstanceList::iterator it = CGEncounterJournal::s_instances.begin(); it != CGEncounterJournal::s_instances.end(); ++it) {
        it->m_encounters.clear();
    }

    CGEncounterJournal::s_instances.clear();

    for (EJMapEncounters::iterator it = CGEncounterJournal::s_mapEncounters.begin(); it != CGEncounterJournal::s_mapEncounters.end(); ++it) {
        it->second.clear();
    }
}

// OFFSET: 0x00A08CE0/0x1407A95C0
JournalEncounterRec* CGEncounterJournal::GetMapEncounter(int32_t index) {
    std::pair<int32_t, int32_t> key(CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap, CGWorldMap::GetCurrentAreaID());

    if (CGEncounterJournal::s_mapEncounters.find(key) == CGEncounterJournal::s_mapEncounters.end()) {
        return NULL;
    }

    EJEncounterList& encounters = CGEncounterJournal::s_mapEncounters[key];
    int32_t current = 0;

    for (EJEncounterList::iterator it = encounters.begin(); it != encounters.end(); ++it) {
        if (current == index) {
            return *it;
        }

        current++;
    }

    return NULL;
}

// OFFSET: 0x00A08DA0/0x1407A96C0
int32_t CGEncounterJournal::GetCurrentInstance() {
    int32_t area = CGWorldMap::GetCurrentAreaID();
    std::pair<int32_t, int32_t> key(CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap, area);

    if (CGEncounterJournal::s_mapEncounters.find(key) != CGEncounterJournal::s_mapEncounters.end()) {
        return CGEncounterJournal::s_mapEncounters[key].front()->m_journalInstanceID;
    }

    for (EJInstanceList::iterator it = CGEncounterJournal::s_instances.begin(); it != CGEncounterJournal::s_instances.end(); ++it) {
        JournalEncounterRec* encounter = CGEncounterJournal::GetEncounterByIndex(it->m_instance->m_id, 0);

        if (encounter && encounter->m_worldMapAreaID == area) {
            return it->m_instance->m_id;
        }
    }

    return 0;
}

// OFFSET: 0x00A08FB0/0x1407A9A10
void CGEncounterJournal::InitializeGame() {
    CGEncounterJournal::ResetState();
    CGEncounterJournal::BuildInstances();
}

// OFFSET: 0x00A08FC0/0x1407A9A30
void CGEncounterJournal::ShutdownGame() {
    CGEncounterJournal::ClearInstances();

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = NULL;
}

// OFFSET: 0x00A08FF0/0x1407A9A70
int32_t CGEncounterJournal::CompareLoot(int32_t itemA, int32_t itemB) {
    ItemRecSparse* a = g_itemSparseDB.GetRecord(itemA, WOWGUID(), NULL, NULL, false);
    ItemRecSparse* b = g_itemSparseDB.GetRecord(itemB, WOWGUID(), NULL, NULL, false);

    if (!a || !b) {
        return 0;
    }

    int32_t orderA = s_inventoryTypeOrder[a->m_inventoryType];
    int32_t orderB = s_inventoryTypeOrder[b->m_inventoryType];

    if (orderA != orderB) {
        return orderA >= orderB ? 1 : -1;
    }

    return SStrCmpUTF8I(a->GetName(0), b->GetName(0), STORM_MAX_STR);
}

// OFFSET: 0x00A09090
void CGEncounterJournal::AddLoot(JournalEncounterItemRec* item) {
    EJLootList::iterator it = CGEncounterJournal::s_loot.begin();

    while (it != CGEncounterJournal::s_loot.end() && CGEncounterJournal::CompareLoot(item->m_itemID, (*it)->m_itemID) > 0) {
        ++it;
    }

    CGEncounterJournal::s_loot.insert(it, item);
}

// OFFSET: 0x00A09100/0x1407A9C20
void CGEncounterJournal::SetSearch(const char* search) {
    if (CGEncounterJournal::s_searchString && SStrStrUTF8I(search, CGEncounterJournal::s_searchString) && !CGEncounterJournal::s_searchTruncated) {
        CGEncounterJournal::RefineSearch(search);
        return;
    }

    CGEncounterJournal::s_searchResults.clear();

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = NULL;
    CGEncounterJournal::s_searchTruncated = false;

    for (int32_t i = g_journalInstanceDB.m_numRecords - 1; i >= 0; i--) {
        JournalInstanceRec* instance = g_journalInstanceDB.GetRecordByIndex(i);
        CGEncounterJournal::AddSearchResult(instance->m_id, 4, instance->m_name, search);
    }

    for (int32_t i = g_journalEncounterDB.m_numRecords - 1; i >= 0; i--) {
        JournalEncounterRec* encounter = g_journalEncounterDB.GetRecordByIndex(i);
        CGEncounterJournal::AddSearchResult(encounter->m_id, 1, encounter->m_name, search);
        CGEncounterJournal::SearchSections(encounter->m_firstSectionID, search);
    }

    for (int32_t i = g_journalEncounterCreatureDB.m_numRecords - 1; i >= 0; i--) {
        JournalEncounterCreatureRec* creature = g_journalEncounterCreatureDB.GetRecordByIndex(i);
        JournalEncounterRec* encounter = g_journalEncounterDB.GetRecord(creature->m_journalEncounterID);

        if (encounter && SStrCmpUTF8I(creature->m_name, encounter->m_name, STORM_MAX_STR)) {
            CGEncounterJournal::AddSearchResult(creature->m_id, 2, creature->m_name, search);
        }
    }

    WOWGUID requester = WOWGUID::Make(0);

    for (int32_t i = g_journalEncounterItemDB.m_numRecords - 1; i >= 0; i--) {
        JournalEncounterItemRec* item = g_journalEncounterItemDB.GetRecordByIndex(i);
        ItemRecSparse* sparse = g_itemSparseDB.GetRecord(item->m_itemID, requester, NULL, NULL, false);

        if (sparse) {
            CGEncounterJournal::AddSearchResult(item->m_id, 0, sparse->m_name, search);
        }
    }

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = SStrDupA(search, __FILE__, __LINE__);
}

// OFFSET: 0x00A099E0/0x1407AA850
void CGEncounterJournal::OnLootItemLoaded(JournalEncounterItemRec* item) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !item || (CGEncounterJournal::s_encounterID && CGEncounterJournal::s_encounterID != item->m_journalEncounterID)) {
        return;
    }

    JournalEncounterRec* encounter = g_journalEncounterDB.GetRecord(item->m_journalEncounterID);

    if (!encounter || CGEncounterJournal::s_instanceID != encounter->m_journalInstanceID) {
        return;
    }

    if (!CGEncounterJournal::IsLootFiltered(item, 1 << player->GetPVPFactionIndex())) {
        CGEncounterJournal::AddLoot(item);
        FrameScript_SignalEvent(SCRIPT_EVENT_EJ_LOOT_DATA_RECIEVED, NULL);
    }
}

// OFFSET: 0x00A09A80/0x1407AA900
void CGEncounterJournal::LootItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGEncounterJournal::OnLootItemLoaded(static_cast<JournalEncounterItemRec*>(param));
    }
}

// OFFSET: 0x00A09AA0/0x1407AA920
bool CGEncounterJournal::IsLootFiltered(JournalEncounterItemRec* item, int32_t factionMask) {
    if (!(factionMask & item->m_factionMask) || !((1 << CGEncounterJournal::s_difficulty) & item->m_difficultyMask)) {
        return true;
    }

    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(item->m_itemID, WOWGUID(), CGEncounterJournal::LootItemCallback, item, true);

    if (!sparse) {
        return true;
    }

    if (CGEncounterJournal::s_classFilter <= 0) {
        return false;
    }

    ClientItemRec rec(sparse);

    if (IsItemUsefulForClass(CGEncounterJournal::s_classFilter, &rec, 0, false, sparse->m_randomSuffix != 0)) {
        return false;
    }

    return true;
}

// OFFSET: 0x00A09B50/0x1407AA9F0
int32_t CGEncounterJournal::BuildLoot() {
    if (CGEncounterJournal::s_lootDirty) {
        CGEncounterJournal::s_loot.clear();
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            return 0;
        }

        int32_t factionMask = 1 << player->GetPVPFactionIndex();
        EJInstance* instance = CGEncounterJournal::FindInstance(CGEncounterJournal::s_instanceID);

        if (instance) {
            for (EJEncounterList::iterator it = instance->m_encounters.begin(); it != instance->m_encounters.end(); ++it) {
                int32_t encounterID = (*it)->m_id;

                if (CGEncounterJournal::s_encounterID && CGEncounterJournal::s_encounterID != encounterID) {
                    continue;
                }

                int32_t high = g_journalEncounterItemDB.m_numRecords - 1;
                int32_t low = 0;
                bool found = false;

                while (low < high) {
                    int32_t mid = low + (high - low) / 2;
                    JournalEncounterItemRec* rec = g_journalEncounterItemDB.GetRecordByIndex(mid);

                    if (encounterID < rec->m_journalEncounterID) {
                        high = mid - 1;
                    } else if (encounterID > rec->m_journalEncounterID) {
                        low = mid + 1;
                    } else {
                        low = mid;
                        found = true;
                        break;
                    }
                }

                if (!found && low != high) {
                    continue;
                }

                for (int32_t i = low; i >= 0 && i < g_journalEncounterItemDB.m_numRecords; i++) {
                    JournalEncounterItemRec* rec = g_journalEncounterItemDB.GetRecordByIndex(i);

                    if (!rec || rec->m_journalEncounterID != encounterID) {
                        break;
                    }

                    if (!CGEncounterJournal::IsLootFiltered(rec, factionMask)) {
                        CGEncounterJournal::AddLoot(rec);
                    }
                }

                for (int32_t i = low - 1; i >= 0 && i < g_journalEncounterItemDB.m_numRecords; i--) {
                    JournalEncounterItemRec* rec = g_journalEncounterItemDB.GetRecordByIndex(i);

                    if (!rec || rec->m_journalEncounterID != encounterID) {
                        break;
                    }

                    if (!CGEncounterJournal::IsLootFiltered(rec, factionMask)) {
                        CGEncounterJournal::AddLoot(rec);
                    }
                }
            }
        }
    }

    CGEncounterJournal::s_lootDirty = false;
    return static_cast<int32_t>(CGEncounterJournal::s_loot.size());
}
