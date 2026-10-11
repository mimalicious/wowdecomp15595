#ifndef WOW_SOURCE_UI_CGQUESTOBJECTIVECACHE_H
#define WOW_SOURCE_UI_CGQUESTOBJECTIVECACHE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CGGameObject_C;
class CGUnit_C;

class CGQuestObjectiveCache {
    public:
    struct REQUIREDUNITNODE : public TSHashObject<REQUIREDUNITNODE, HASHKEY_NONE> {
        /* 0018 */ uint8_t m_required : 1;
        /* 0018 */ uint8_t m_minimapMark : 1;
    };

    struct REQUIREDITEMNODE : public TSHashObject<REQUIREDITEMNODE, HASHKEY_NONE> {
        /* 0018 */ uint8_t m_required : 1;
        /* 0018 */ uint8_t m_minimapMark : 1;
    };

    static uint8_t s_dirty;
    static TSHashTable<REQUIREDUNITNODE, HASHKEY_NONE> s_requiredUnits;
    static TSHashTable<REQUIREDITEMNODE, HASHKEY_NONE> s_requiredItems;

    static void SetUnitRequired(int32_t id, bool required, bool minimapMark);
    static void SetItemRequired(int32_t id, bool required, bool minimapMark);
    static void UpdateLists();
    static bool CreatureTrackedInQuests(const CGUnit_C* unit, bool minimapMark);
    static bool ObjectTrackedInQuests(const CGGameObject_C* object, bool minimapMark);
};

#endif
