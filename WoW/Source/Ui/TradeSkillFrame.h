#ifndef WOW_SOURCE_UI_TRADESKILLFRAME_H
#define WOW_SOURCE_UI_TRADESKILLFRAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
struct SkillLine;
class SkillLineRec;

struct TradeSkillInfo {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ int32_t m_difficulty;
    /* 0008 */ int32_t m_itemClass;
    /* 000C */ int32_t m_itemSubClass;
    /* 0010 */ uint32_t m_invTypeMask;
    /* 0014 */ int32_t m_itemLevel;
    /* 0018 */ int32_t m_numAvailable;
    /* 001C */ int32_t m_skillLineRank;
    /* 0020 */ int32_t m_numSkillUps;
    /* 0024 */ int32_t m_visible;
};

struct TradeSkillSubClassInfo {
    /* 0000 */ int32_t m_itemClass;
    /* 0004 */ int32_t m_itemSubClass;
    /* 0008 */ int32_t m_shown;
    /* 000C */ int32_t m_collapsed;
    /* 0010 */ uint32_t m_invTypeMask;
};

class CGTradeSkillInfo {
    public:
    static WOWGUID m_playerGuid;
    static int32_t m_spellID;
    static int32_t m_skillLineID;
    static int32_t m_selectedSkill;
    static uint32_t m_itemsPending;
    static uint32_t m_numDisplayEntries;
    static uint32_t m_numSubClasses;
    static int32_t m_filteredSkills;
    static char* m_itemNameFilter;
    static int32_t m_itemMinLevelFilter;
    static int32_t m_itemMaxLevelFilter;
    static int32_t m_showOnlyMakeable;
    static int32_t m_showOnlySkillUps;
    static uint32_t m_invSlots;
    static int32_t m_recastSpellID;
    static uint32_t m_recastTimes;
    static int32_t m_pendingRecastSpellID;
    static uint32_t m_pendingRecastTimes;
    static void* m_linkData;
    static int32_t m_skillRank;
    static int32_t m_skillMaxRank;
    static bool m_guildMemberSkill;
    static bool m_guildSkill;
    static int32_t m_lastSkillLineID;
    static TSGrowableArray<TradeSkillInfo*> m_displayList;
    static TSGrowableArray<TradeSkillSubClassInfo*> m_subClassList;
    static uint32_t m_subClassFilter;
    static uint32_t m_subClassFilterIndex;
    static uint32_t m_invTypeFilter;
    static uint32_t m_collapseFilter;

    static void DoTradeSkill(int32_t spellId, uint32_t times);
    static void StopRecast();
    static void StopRecast(int32_t spellId);
    static void DoRecast(int32_t spellId);
    static void ActivatePendingRecast(int32_t spellId);
    static void Close();
    static SkillLineRec* GetSkillLineRec(int32_t spellId, const uint64_t& guid);
    static int32_t GetSelectionIndex();
    static int32_t GetSubClassIndexFromSkill(uint32_t index);
    static int32_t IsCollapsedHeader(uint32_t index);
    static uint32_t GetInvSlotFilter();
    static int32_t GetSkillMaxRank();
    static int32_t GetSkillModifier();
    static bool GetTradeSkillListLink(int32_t spellId, char* buffer, uint32_t bufferSize);
    static void ShutdownGame();
    static void ClearItemCallbacks();
    static void FilterAndSortSkills(int32_t resetSubClasses);
    static void SetSubClassFilter(uint32_t filter, uint32_t index);
    static void SetInvSlotFilter(uint32_t filter);
    static void SetItemNameFilter(const char* filter);
    static void SetItemLevelFilter(int32_t minLevel, int32_t maxLevel);
    static void LeaveWorld();
    static void RefreshList(int32_t full);
    static void SetTradeSkill(int32_t spellId, const uint64_t& guid, const char* link, int32_t skillRank, int32_t skillMaxRank);
    static void OpenGuildMemberTradeSkill(const uint64_t& guid, const SkillLine& skillLine);
    static void OpenGuildTradeSkill(int32_t skillLineID);
    static void SetLink(const char* link);
};

extern const char* s_invSlotTokens[27];
extern const char* s_tradeLinkColor;

const char* GetInventorySlotToken(int32_t slot);
void TradeSkillNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
bool Profession_FilterSpell(int32_t spellId, int32_t minLevel, int32_t maxLevel, const char* name);
void TradeSkillRegisterScriptFunctions();
void TradeSkillUnregisterScriptFunctions();

#endif
