#include "CGQuestObjectiveCache.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/QuestLog.h"

// STATIC: 0x00C09818
static const uint32_t s_objectiveMarkFlags[4] = { 0x1, 0x2, 0x4, 0x8 };

// STATIC: 0x00C09828
static const uint32_t s_collectItemMarkFlags[6] = { 0x10, 0x20, 0x40, 0x80, 0x100, 0x200 };

// STATIC: 0x00F71010
uint8_t CGQuestObjectiveCache::s_dirty;

// STATIC: 0x00F71014
TSHashTable<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE> CGQuestObjectiveCache::s_requiredUnits;

// STATIC: 0x00F71040
TSHashTable<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE> CGQuestObjectiveCache::s_requiredItems;

// OFFSET: 0x009DF8D0
void CGQuestObjectiveCache::SetUnitRequired(int32_t id, bool required, bool minimapMark) {
    REQUIREDUNITNODE* node = CGQuestObjectiveCache::s_requiredUnits.Ptr(id, HASHKEY_NONE());

    if (node) {
        node->m_required = node->m_required || required;
        node->m_minimapMark = node->m_minimapMark || minimapMark;
        return;
    }

    node = CGQuestObjectiveCache::s_requiredUnits.New(id, HASHKEY_NONE(), 0, 0);
    node->m_required = required;
    node->m_minimapMark = minimapMark;
}

// OFFSET: 0x009DF9D0
void CGQuestObjectiveCache::SetItemRequired(int32_t id, bool required, bool minimapMark) {
    REQUIREDITEMNODE* node = CGQuestObjectiveCache::s_requiredItems.Ptr(id, HASHKEY_NONE());

    if (node) {
        node->m_required = node->m_required || required;
        node->m_minimapMark = node->m_minimapMark || minimapMark;
        return;
    }

    node = CGQuestObjectiveCache::s_requiredItems.New(id, HASHKEY_NONE(), 0, 0);
    node->m_required = required;
    node->m_minimapMark = minimapMark;
}

// OFFSET: 0x009DFAD0
void CGQuestObjectiveCache::UpdateLists() {
    CGQuestObjectiveCache::s_requiredUnits.Clear();
    CGQuestObjectiveCache::s_requiredItems.Clear();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(CGQuestLog::m_numQuest); i++) {
        if (CGQuestLog::IsQuestComplete(i)) {
            continue;
        }

        WOWGUID requester = WOWGUID();
        QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::GetQuestID(i), requester, NULL, NULL, false);

        if (!quest) {
            continue;
        }

        if (i < 0 || i >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[i].m_isHeader) {
            continue;
        }

        int32_t logSlot = CGQuestLog::m_quests[i].m_logSlot;

        if (logSlot < 0 || logSlot > 50) {
            continue;
        }

        PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[logSlot];

        if (!entry) {
            continue;
        }

        for (uint32_t j = 0; j < 4; j++) {
            if (quest->m_objectiveId[j] && entry->m_counts[j] < quest->m_objectiveRequiredCount[j]) {
                CGQuestObjectiveCache::SetUnitRequired(quest->m_objectiveId[j], true, (s_objectiveMarkFlags[j] & quest->m_minimapTargetMark) != 0);
            }
        }

        for (uint32_t j = 0; j < 6; j++) {
            if (quest->m_collectItemId[j] && player->m_bag.GetItemTypeCount(quest->m_collectItemId[j], 8) < static_cast<int32_t>(quest->m_collectItemCount[j])) {
                CGQuestObjectiveCache::SetItemRequired(quest->m_collectItemId[j], true, (s_collectItemMarkFlags[j] & quest->m_minimapTargetMark) != 0);
            }
        }

        for (uint32_t j = 0; j < 4; j++) {
            if (quest->m_intermediateItemId[j]) {
                CGQuestObjectiveCache::SetItemRequired(quest->m_intermediateItemId[j], true, false);
            }
        }
    }

    CGQuestObjectiveCache::s_dirty = 0;
}

// OFFSET: 0x009DFCC0/0x1407766C0
bool CGQuestObjectiveCache::CreatureTrackedInQuests(const CGUnit_C* unit, bool minimapMark) {
    if (CGQuestObjectiveCache::s_dirty) {
        CGQuestObjectiveCache::UpdateLists();
    }

    int32_t id = unit->m_obj->m_entryID;
    REQUIREDUNITNODE* node = CGQuestObjectiveCache::s_requiredUnits.Ptr(id, HASHKEY_NONE());

    if (node) {
        if (!node->m_required) {
            return false;
        }

        if (minimapMark) {
            return node->m_minimapMark != 0;
        }

        return true;
    }

    WOWGUID requester = WOWGUID();
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(id, requester, NULL, NULL, false);

    if (!stats) {
        return false;
    }

    bool required = false;
    bool mark = false;

    for (uint32_t i = 0; i < 2; i++) {
        REQUIREDUNITNODE* credit = CGQuestObjectiveCache::s_requiredUnits.Ptr(stats->m_killCredit[i], HASHKEY_NONE());

        if (credit) {
            required = required || credit->m_required;
            mark = mark || credit->m_minimapMark;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        REQUIREDITEMNODE* item = CGQuestObjectiveCache::s_requiredItems.Ptr(stats->m_questItem[i], HASHKEY_NONE());

        if (item) {
            required = required || item->m_required;
            mark = mark || item->m_minimapMark;
        }
    }

    CGQuestObjectiveCache::SetUnitRequired(id, required, mark);

    if (!required) {
        return false;
    }

    if (!minimapMark) {
        return true;
    }

    return mark;
}

// OFFSET: 0x009DFE70
bool CGQuestObjectiveCache::ObjectTrackedInQuests(const CGGameObject_C* object, bool minimapMark) {
    if (CGQuestObjectiveCache::s_dirty) {
        CGQuestObjectiveCache::UpdateLists();
    }

    int32_t id = object->m_obj->m_entryID | 0x80000000;
    REQUIREDUNITNODE* node = CGQuestObjectiveCache::s_requiredUnits.Ptr(id, HASHKEY_NONE());

    if (node) {
        if (!node->m_required) {
            return false;
        }

        if (minimapMark) {
            return node->m_minimapMark != 0;
        }

        return true;
    }

    GameObjectStats* stats = object->m_stats;

    if (!stats) {
        return false;
    }

    bool required = false;
    bool mark = false;

    for (uint32_t i = 0; i < 6; i++) {
        if (!stats->m_questItems[i]) {
            continue;
        }

        REQUIREDITEMNODE* item = CGQuestObjectiveCache::s_requiredItems.Ptr(stats->m_questItems[i], HASHKEY_NONE());

        if (item) {
            required = required || item->m_required;
            mark = mark || item->m_minimapMark;
        }
    }

    CGQuestObjectiveCache::SetUnitRequired(id, required, mark);

    if (!required) {
        return false;
    }

    if (!minimapMark) {
        return true;
    }

    return mark;
}
