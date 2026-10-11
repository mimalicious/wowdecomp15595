#ifndef WOW_SOURCE_UI_GUILDINFO_H
#define WOW_SOURCE_UI_GUILDINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

enum GUILD_SORT_TYPE {
    GUILD_SORT_RANK = 0,
    GUILD_SORT_LEVEL = 1,
    GUILD_SORT_NAME = 2,
    GUILD_SORT_ZONE = 3,
    GUILD_SORT_CLASS = 4,
    GUILD_SORT_GROUP = 5,
    GUILD_SORT_ONLINE = 6,
    GUILD_SORT_NOTE = 7,
    GUILD_SORT_SKILL = 8,
    GUILD_SORT_BGRATING = 9,
    GUILD_SORT_WEEKLYXP = 10,
    GUILD_SORT_TOTALXP = 11,
    GUILD_SORT_ACHIEVEMENTPOINTS = 12,
    NUM_GUILD_SORT_TYPES = 13
};

struct SkillLine {
    /* 0000 */ int32_t m_skillLineID;
    /* 0004 */ int32_t m_skillRank;
    /* 0008 */ int32_t m_skillStep;
};

struct GuildMemberProfession {
    /* 0000 */ int32_t m_skillLineID;
    /* 0004 */ int32_t m_rank;
    /* 0008 */ int32_t m_step;
};

struct GuildMemberInfo {
    /* 0000 */ uint64_t m_guid;
    /* 0008 */ char m_name[48];
    /* 0038 */ int32_t m_level;
    /* 003C */ int32_t m_classID;
    /* 0040 */ int32_t m_gender;
    /* 0044 */ GuildMemberProfession m_professions[2];
    /* 005C */ int32_t m_areaID;
    /* 0060 */ int32_t m_guildRepToCap;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ int64_t m_weeklyXP;
    /* 0070 */ int64_t m_totalXP;
    /* 0078 */ int32_t m_weeklyXPRank;
    /* 007C */ int32_t m_totalXPRank;
    /* 0080 */ int32_t m_bgRating;
    /* 0084 */ int32_t m_rankID;
    /* 0088 */ int32_t m_online;
    /* 008C */ uint8_t m_status;
    /* 008D */ uint8_t unk_008D;
    /* 008E */ char m_note[128];
    /* 010E */ char m_officerNote[128];
    /* 018E */ uint8_t unk_018E[2];
    /* 0190 */ float m_lastSave;
    /* 0194 */ int32_t m_achievementPoints;
    /* 0198 */ int32_t m_achievementRank;
    /* 019C */ int32_t m_guildReputation;
    /* 01A0 */ bool m_authenticated;
    /* 01A1 */ bool m_sorEligible;
    /* 01A2 */ uint8_t unk_01A2[6];

    int32_t GetProfessionRank(int32_t skillLineID);
};

struct GuildRankInfo {
    /* 0000 */ uint32_t m_rights;
    /* 0004 */ int32_t m_rankID;
    /* 0008 */ int32_t m_rankOrder;
    /* 000C */ int32_t m_withdrawGoldLimit;
    /* 0010 */ uint32_t m_tabFlags[8];
    /* 0030 */ int32_t m_tabWithdrawItemLimit[8];
    /* 0050 */ char m_name[64];
};

struct GuildEventLogEntry {
    /* 0000 */ uint32_t m_type;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_playerGUID;
    /* 0010 */ uint64_t m_otherGUID;
    /* 0018 */ int32_t m_rankID;
    /* 001C */ int32_t m_time;
};

struct GuildSortCriteria {
    /* 0000 */ GUILD_SORT_TYPE m_type;
    /* 0004 */ int32_t m_reverse;
};

struct GuildRankFlag {
    /* 0000 */ uint32_t m_mask;
    /* 0004 */ int32_t m_enabled;
};

struct GuildProfessionMember {
    /* 0000 */ uint64_t m_guid;
    /* 0008 */ uint32_t m_memberIndex;
    /* 000C */ uint32_t unk_000C;
};

struct NewsHeaderDate {
    /* 0000 */ WowTime m_date;
    /* 0020 */ bool m_hasSticky;
};

struct GuildNewsEvent : public JamGuildNewsEvent {
    /* 0050 */ bool m_isHeader;
};

class CGGuildInfo {
    public:
    struct GuildProfessionHeader : public TSLinkedNode<GuildProfessionHeader> {
        /* 0008 */ uint32_t unk_0008;
        /* 000C */ int32_t m_skillLineID;
        /* 0010 */ bool m_collapsed;
        /* 0014 */ uint32_t m_numMembers;
        /* 0018 */ TSGrowableArray<GuildProfessionMember> m_members;
    };

    struct GuildRecipesKnown : public TSHashObject<GuildRecipesKnown, HASHKEY_NONE> {
        /* 0018 */ bool m_canView;
        /* 0019 */ uint8_t m_recipes[300];
    };

    struct SKILLLINESPELLBITS : public TSHashObject<SKILLLINESPELLBITS, HASHKEY_NONE> {
        /* 0018 */ int32_t m_spellID;
    };

    struct SKILLLINESPELLS : public TSHashObject<SKILLLINESPELLS, HASHKEY_NONE> {
        /* 0018 */ TSHashTable<SKILLLINESPELLBITS, HASHKEY_NONE> m_bits;
    };

    static int32_t m_numAccounts;
    static uint32_t m_numMembers;
    static uint32_t m_numOnline;
    static uint32_t m_numUsedRanks;
    static uint32_t m_guildFlags;
    static int32_t m_guildReputationWeeklyCap;
    static int32_t m_showOffline;
    static int32_t m_nextAllowedQuery;
    static uint64_t m_selectedMember;
    static int32_t m_numEventLogEntries;
    static int32_t m_pendingEventLogNames;
    static int32_t m_guildLevel;
    static int32_t m_guildRewardItemListVersion;
    static uint64_t m_guildMemberSkillsGuid;
    static int32_t m_numGuildNews;
    static int32_t m_guildRewardStatCBCount;
    static bool m_newsSortByDate;
    static bool m_saveToFile;
    static bool m_inGuildParty;
    static int64_t m_largestWeeklyXPContribution;
    static int64_t m_largestTotalXPContribution;
    static int32_t m_largestPersonalAchievementPoints;
    static int32_t m_numGuildGroupMembers;
    static int32_t m_numGuildGroupRequired;
    static float m_guildXPEarnedMult;
    static int32_t m_sortSkillLineID;
    static WowTime m_createDate;
    static TSGrowableArray<GuildMemberInfo*> m_members;
    static TSHashTable<GuildRecipesKnown, HASHKEY_NONE> m_guildRecipesKnown;
    static TSHashTable<SKILLLINESPELLS, HASHKEY_NONE> m_skillLineSpells;
    static TSGrowableArray<JamGuildRewardItem> m_guildRewardItemList;
    static TSGrowableArray<GuildNewsEvent> m_guildNews;
    static int32_t m_knownRecipeSkillLineID;
    static int32_t m_knownRecipeSpellID;
    static TSGrowableArray<uint64_t> m_knownRecipeMembers;
    static char m_professionNameFilter[32];
    static uint8_t m_guildMemberProfessionData[300];
    static SkillLine m_guildMemberSkillLine;
    static GuildRankInfo m_rankInfo[10];
    static GuildEventLogEntry m_eventLog[100];
    static char m_infoText[2004];
    static char m_motd[516];
    static GuildSortCriteria m_sortCriteria[NUM_GUILD_SORT_TYPES];
    static int32_t m_challengeGold[4];
    static int32_t m_challengeXP[4];
    static int32_t m_challengeMaxLevelGold[4];
    static int32_t m_challengeMaxCount[4];
    static int32_t m_challengeCurrentCount[4];
    static int32_t m_selectedRankOrder;
    static float m_guildMasterAbsentDays;
    static float m_guildMasterReplaceDays;
    static int32_t m_guildNewsFilter;
    static int32_t m_professionCategoryFilter;
    static TSList<GuildProfessionHeader, TSGetLink<GuildProfessionHeader> > m_professionList;

    static uint32_t GetNumMembers(int32_t includeOffline);
    static GuildMemberInfo* GetMember(uint32_t index);
    static int32_t GetMemberProfessionRank(GuildMemberInfo* member, int32_t skillLineID);
    static int32_t QSortRoster(const void* a, const void* b);
    static int32_t Script_GetGuildRosterLargestContribution(lua_State* L);
    static int32_t Script_GetGuildRosterLargestAchievements(lua_State* L);
    static int32_t Script_InGuildParty(lua_State* L);
    static void GetGuildMemberRecipes(SkillLine& skillLine, uint8_t* data);
    static void SetSortType(GUILD_SORT_TYPE type);
    static void QueryPermissions();
    static void ResetGuildState(bool a1);
    static int32_t GetRankIndexFromRankOrder(int32_t rankOrder);
    static int32_t GetRankOrderFromRankID(int32_t rankID);
    static int32_t GetRankIDFromRankOrder(int32_t rankOrder);
    static GuildRankInfo* GetRankInfoFromRankID(int32_t rankID);
    static uint32_t GetPermissionsByRankOrder(int32_t rankOrder);
    static uint32_t GetPermissionsByRankID(int32_t rankID);
    static int32_t GetWithdrawGoldLimitForSelectedRank();
    static int32_t GetWithdrawGoldLimitByRankID(uint32_t rankID);
    static void SetWithdrawGoldLimitForSelectedRank(uint32_t amount);
    static void SetTabWithdrawItemLimitForSelectedRank(uint32_t tab, int32_t amount);
    static uint32_t GetTabFlagsForSelectedRank(uint32_t tab);
    static uint32_t GetGuildBankTabPermissionsByRankID(uint32_t tab, uint32_t rankID);
    static int32_t SetTabFlagsForSelectedRank(int32_t tab, uint32_t flags);
    static int32_t GetTabWithdrawItemLimitForSelectedRank(uint32_t tab);
    static int32_t GetNumGuildBankTabItemWithdrawalsByRankID(uint32_t tab, uint32_t rankID);
    static void UpdateRoster(int32_t force);
    static void GuildRewardsItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void GuildNewsItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void GuildNewsNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static int32_t GetRewardMissingAchievement(const JamGuildRewardItem* item);
    static int32_t GetGuildReputationWeeklyCap();
    static void EventLogNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void SetGuildMemberRecipes(const uint64_t& guid, int32_t skillLineID, int32_t skillRank, int32_t skillStep, uint8_t* data, int32_t size);
    static bool GetGuildRecipeInfoMember(int32_t index, char* name, char* className, bool& online);
    static int32_t GetMemberIndex(const uint64_t& guid);
    static const char* GetGuildReputationTitleString(const uint64_t& guid);
    static int32_t GetSelectedMemberIndex();
    static void SetMOTD(const char* motd);
    static void ComputeContributionRanks();
    static void UpdateMemberInfo(GuildMemberInfo* info, const ClientGuildRosterMemberData* data);
    static int32_t GetNumMembersInRank(int32_t rankOrder);
    static const char* GetRankNameFromRankID(int32_t rankID);
    static void InvalidatePlayer(uint64_t guid);
    static GuildNewsEvent* GetGuildNewsEvent(int32_t index);
    static void SortGuildNewsByDate();
    static const char* GetMemberName(const uint64_t& guid);
    static bool GetMemberGUIDFromName(const char* name, uint64_t& guid);
    static void SetMemberMobile(uint64_t guid, bool mobile);
    static int32_t GetMemberReputationToCap(const uint64_t& guid);
    static void SaveToFile();
    static void QueryXP();
    static void QueryNews();
    static void QueryRanks();
    static void UpdateGuildLevel();
    static bool CanEditGuildEvent();
    static bool CanRemoveGuildEvent();
    static bool CanReplaceGuildMaster();
    static void InsertMemberInfoProfessionsList(uint32_t index, int32_t showOffline);
    static bool GetTradeSkillInfo(int32_t index, uint64_t& guid, int32_t& skillLineID, bool& collapsed, int32_t& numOnline, int32_t& numMembers, int32_t& numVisible);
    static void SortTradeSkillMembers(GUILD_SORT_TYPE type);
    static void QueryRosterAndRanks();
    static void NameChangedGuildCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static uint32_t GetNumGuildRewards();
    static JamGuildRewardItem* GetGuildRewardItem(uint32_t index);
    static bool GetGuildKnownRecipes(int32_t skillLineID, uint8_t* data, int32_t& size);
    static void SetRecipeKnownByMembers(int32_t skillLineID, int32_t spellID, const TSGrowableArray<uint64_t>& members);
    static int32_t GetSpellFromRecipeBit(int32_t skillLineID, int32_t bit);
    static int32_t GetRecipeBitFromSpell(int32_t skillLineID, int32_t spellID);
    static void ClearProfessionMembers();
    static void UpdateProfessions();
    static void SetMemberOnlineStatus(uint64_t guid, bool online);
    static void EnterWorld();
    static void SetProfessionHeaderCollapsed(int32_t skillLineID, bool collapsed);
    static void SetProfessionCategoryFilter(int32_t skillLineID);
    static void SetProfessionNameFilter(const char* filter);
    static void DestroySkillLineUniqueBitTables();
    static void InitializeProfessionsHeaderList();
    static void SetShowOffline(int32_t show);
    static void ClearGuildInfo();
    static void SortGuildNewsHeaders();
    static void ShutdownGame();
    static void SetGuildNewsFilter(int32_t filter);
    static void SetGuildKnownRecipes(int32_t skillLineID, uint8_t* data, int32_t size);
    static void InitializeSkillLineUniqueBitTables();
    static void InitializeGame();
    static bool ClientGuildMemberDailyResetHandler(ClientGuildMemberDailyReset* msg);
    static bool ClientGuildMoveStartingHandler(ClientGuildMoveStarting* msg);
    static bool ClientGuildMoveCompleteHandler(ClientGuildMoveComplete* msg);
    static bool ClientGuildEventLogQueryResultsHandler(ClientGuildEventLogQueryResults* msg);
    static bool ClientGuildRanksHandler(ClientGuildRanks* msg);
    static bool ClientGuildChallengeUpdateHandler(ClientGuildChallengeUpdate* msg);
    static bool ClientGuildChallengeCompletedHandler(ClientGuildChallengeCompleted* msg);
    static bool ClientGuildChangeNameResultHandler(ClientGuildChangeNameResult* msg);
    static bool ClientGuildFlaggedForRenameHandler(ClientGuildFlaggedForRename* msg);
    static bool ClientGuildPartyStateHandler(ClientGuildPartyState* msg);
    static bool ClientGuildPermissionsQueryResultsHandler(ClientGuildPermissionsQueryResults* msg);
    static bool ClientGuildXPEarnedHandler(ClientGuildXPEarned* msg);
    static bool ClientGuildMemberUpdateNoteHandler(ClientGuildMemberUpdateNote* msg);
    static bool ClientGuildInviteHandler(ClientGuildInvite* msg);
    static bool ClientGuildReputationWeeklyCapHandler(ClientGuildReputationWeeklyCap* msg);
    static bool ClientGuildReputationReactionChangedHandler(ClientGuildReputationReactionChanged* msg);
    static bool ClientGuildResetHandler(ClientGuildReset* msg);
    static bool ClientGuildNameChangedHandler(ClientGuildNameChanged* msg);
    static bool ClientGuildRosterHandler(ClientGuildRoster* msg);
    static bool ClientGuildRosterUpdateHandler(ClientGuildRosterUpdate* msg);
    static bool ClientGuildRewardListHandler(ClientGuildRewardList* msg);
    static bool ClientGuildNewsHandler(ClientGuildNews* msg);
    static bool ClientGuildNewsDeletedHandler(ClientGuildNewsDeleted* msg);
};

void GuildInfoRegisterScriptFunctions();
void GuildInfoUnregisterScriptFunctions();

#endif
