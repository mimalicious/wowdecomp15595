#ifndef WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H
#define WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct lua_State;

class CGAutoCompleteName {
    public:
    struct AUTOCOMPLETE_NAMENODE : public TSLinkedNode<AUTOCOMPLETE_NAMENODE> {
        /* 0008 */ char m_name[308];
        /* 013C */ uint32_t m_flags;
        /* 0140 */ uint32_t m_lastUsed;
        /* 0144 */ uint8_t m_presenceId;
        /* 0148 */ WOWGUID m_guid;
    };

    struct AUTOCOMPLETE_CHARACTER_HASHNODE : public TSHashObject<AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID> {
        /* 0020 */ AUTOCOMPLETE_NAMENODE* m_node;
    };

    struct AUTOCOMPLETE_BATTLENET_HASHNODE : public TSHashObject<AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE> {
        /* 0018 */ AUTOCOMPLETE_NAMENODE* m_node;
    };

    static TSList<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>> m_list;
    static TSHashTable<AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID> m_characterHash;
    static TSHashTable<AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE> m_battlenetHash;
    static uint32_t s_numNodes;
    static bool s_resortNamesOnRecency;
    static bool s_batchUpdate;
    static bool s_resortPending;
    static bool s_whenEditingFromCenter;
    static bool s_useContext;
    static int32_t RemoveFlagsFromAllNodes(uint32_t flags);
    static int32_t RemoveFlagsByName(const char* name, uint32_t flags, bool characters, bool battlenet);
    static int32_t CompareNodes(const void* a, const void* b);
    static void PutNameInCorrectSpot(AUTOCOMPLETE_NAMENODE* node);
    static int32_t AddFlagsToNode(AUTOCOMPLETE_NAMENODE* node, uint32_t flags, int32_t add, const char* name);
    static AUTOCOMPLETE_NAMENODE* RemoveNode(AUTOCOMPLETE_NAMENODE* node);
    static void RemoveAllBattlenet();
    static void ResortAll();
    static const char* GetDisplayName(const char* name);
    static int32_t GetPresenceID(const char* name, uint32_t* presenceId);
    static int32_t AddFlags(const WOWGUID& guid, uint32_t flags, int32_t add, const char* name);
    static int32_t AddFlags(const uint32_t& presenceId, uint32_t flags, int32_t add, const char* name);
    static int32_t RemoveFlags(const WOWGUID& guid, uint32_t flags);
    static int32_t RemoveFlags(const uint32_t& presenceId, uint32_t flags);
    static int32_t SetName(const uint32_t& presenceId, const char* name, bool overwrite);
    static AUTOCOMPLETE_NAMENODE* FindNodeByName(const char* name, bool characters, bool battlenet);
    static AUTOCOMPLETE_NAMENODE* AddPlayerToList(const WOWGUID& guid, uint32_t flags, uint32_t lastUsed, const char* name);
    static AUTOCOMPLETE_NAMENODE* AddPlayerToList(const uint32_t& presenceId, uint32_t flags, uint32_t lastUsed, const char* name);
    static void RefreshName(const WOWGUID& guid);
    static int32_t SetName(const WOWGUID& guid, const char* name, bool overwrite);
    static int32_t NodeMatchesParams(const AUTOCOMPLETE_NAMENODE* node, const char* text, uint32_t include, uint32_t exclude, int32_t cursor, bool allowFullMatch);
    static int32_t GetMatchingResults(lua_State* L, const char* text, uint32_t include, uint32_t exclude, int32_t numReturns, int32_t cursor, bool allowFullMatch);
};

typedef CGAutoCompleteName::AUTOCOMPLETE_NAMENODE AUTOCOMPLETE_NAMENODE;

void AutoCompleteRegisterScriptFunctions();
void AutoCompleteUnregisterScriptFunctions();

#endif
