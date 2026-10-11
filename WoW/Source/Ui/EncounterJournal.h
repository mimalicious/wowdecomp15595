#ifndef WOW_SOURCE_UI_ENCOUNTERJOURNAL_H
#define WOW_SOURCE_UI_ENCOUNTERJOURNAL_H

#include <cstdint>
#include <list>
#include <map>
#include <utility>
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "WoW/Source/Object/WGUID.h"

class JournalEncounterCreatureRec;
class JournalEncounterItemRec;
class JournalEncounterRec;
class JournalInstanceRec;

typedef std::list<JournalEncounterRec*, Blizzard::Memory::AllocSTL_raw<JournalEncounterRec*> > EJEncounterList;

struct EJInstance {
    /* 0000 */ JournalInstanceRec* m_instance;
    /* 0004 */ int32_t m_instanceType;
    /* 0008 */ uint32_t m_difficultyMask;
    /* 000C */ EJEncounterList m_encounters;
};

struct EJSearchResult {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ const char* m_name;
};

typedef std::list<EJInstance, Blizzard::Memory::AllocSTL_raw<EJInstance> > EJInstanceList;
typedef std::list<EJSearchResult, Blizzard::Memory::AllocSTL_raw<EJSearchResult> > EJSearchResultList;
typedef std::list<JournalEncounterItemRec*, Blizzard::Memory::AllocSTL_raw<JournalEncounterItemRec*> > EJLootList;
typedef std::map<std::pair<int32_t, int32_t>, EJEncounterList> EJMapEncounters;

class CGEncounterJournal {
    public:
    static int32_t s_instanceID;
    static EJInstance* s_instance;
    static int32_t s_encounterID;
    static int32_t s_difficulty;
    static char* s_searchString;
    static bool s_searchTruncated;
    static int32_t s_classFilter;
    static bool s_lootDirty;
    static EJSearchResultList s_searchResults;
    static EJInstanceList s_instances;
    static EJLootList s_loot;
    static EJMapEncounters s_mapEncounters;

    static void ResetState();
    static const char* BuildLink(int32_t type, int32_t id, const char* name);
    static void ItemLoadedCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static JournalEncounterCreatureRec* GetCreature(int32_t encounterID, uint32_t index);
    static EJInstance* FindInstance(int32_t instanceID);
    static JournalEncounterRec* GetEncounterByIndex(int32_t instanceID, int32_t index);
    static void AddSearchResult(int32_t id, int32_t type, const char* name, const char* search);
    static void RefineSearch(const char* search);
    static void SearchSections(int32_t sectionID, const char* search);
    static int32_t GetInstanceByIndex(int32_t index, int32_t instanceType);
    static bool SelectInstance(int32_t instanceID);
    static void BuildInstances();
    static void ClearInstances();
    static JournalEncounterRec* GetMapEncounter(int32_t index);
    static int32_t GetCurrentInstance();
    static void InitializeGame();
    static void ShutdownGame();
    static int32_t CompareLoot(int32_t itemA, int32_t itemB);
    static void AddLoot(JournalEncounterItemRec* item);
    static void SetSearch(const char* search);
    static void OnLootItemLoaded(JournalEncounterItemRec* item);
    static void LootItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static bool IsLootFiltered(JournalEncounterItemRec* item, int32_t factionMask);
    static int32_t BuildLoot();
};

void EncounterJournalRegisterScriptFunctions();
void EncounterJournalUnregisterScriptFunctions();

#endif
