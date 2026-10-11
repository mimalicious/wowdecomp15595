#include "GuildInfo.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/JamClientAutoCode/JamWorld.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/LFGuildInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F35EC8
int32_t CGGuildInfo::m_numAccounts;

// STATIC: 0x00F35ECC
uint32_t CGGuildInfo::m_numMembers;

// STATIC: 0x00F35ED0
uint32_t CGGuildInfo::m_numOnline;

// STATIC: 0x00F35ED4
uint32_t CGGuildInfo::m_numUsedRanks;

// STATIC: 0x00F35ED8
uint32_t CGGuildInfo::m_guildFlags;

// STATIC: 0x00F35EDC
int32_t CGGuildInfo::m_guildReputationWeeklyCap;

// STATIC: 0x00F35EE0
int32_t CGGuildInfo::m_showOffline;

// STATIC: 0x00F35EE4
int32_t CGGuildInfo::m_nextAllowedQuery;

// STATIC: 0x00F35EE8
uint64_t CGGuildInfo::m_selectedMember;

// STATIC: 0x00F35EF0
int32_t CGGuildInfo::m_numEventLogEntries;

// STATIC: 0x00F35EF4
int32_t CGGuildInfo::m_pendingEventLogNames;

// STATIC: 0x00F35EF8
int32_t CGGuildInfo::m_guildLevel;

// STATIC: 0x00F35EFC
int32_t CGGuildInfo::m_guildRewardItemListVersion;

// STATIC: 0x00F35F00
uint64_t CGGuildInfo::m_guildMemberSkillsGuid;

// STATIC: 0x00F35F08
int32_t CGGuildInfo::m_numGuildNews;

// STATIC: 0x00F35F0C
int32_t CGGuildInfo::m_guildRewardStatCBCount;

// STATIC: 0x00F35F10
bool CGGuildInfo::m_newsSortByDate;

// STATIC: 0x00F35F11
bool CGGuildInfo::m_saveToFile;

// STATIC: 0x00F35F12
bool CGGuildInfo::m_inGuildParty;

// STATIC: 0x00F35F18
int64_t CGGuildInfo::m_largestWeeklyXPContribution;

// STATIC: 0x00F35F20
int64_t CGGuildInfo::m_largestTotalXPContribution;

// STATIC: 0x00F35F28
int32_t CGGuildInfo::m_largestPersonalAchievementPoints;

// STATIC: 0x00F35F2C
int32_t CGGuildInfo::m_numGuildGroupMembers;

// STATIC: 0x00F35F30
int32_t CGGuildInfo::m_numGuildGroupRequired;

// STATIC: 0x00F35F34
float CGGuildInfo::m_guildXPEarnedMult;

// STATIC: 0x00F35F38
int32_t CGGuildInfo::m_sortSkillLineID;

// STATIC: 0x00F35F3C
WowTime CGGuildInfo::m_createDate;

// STATIC: 0x00F35F5C
TSGrowableArray<GuildMemberInfo*> CGGuildInfo::m_members;

// STATIC: 0x00F35F6C
TSHashTable<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE> CGGuildInfo::m_guildRecipesKnown;

// STATIC: 0x00F35F98
TSHashTable<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE> CGGuildInfo::m_skillLineSpells;

// STATIC: 0x00F35FC4
TSGrowableArray<JamGuildRewardItem> CGGuildInfo::m_guildRewardItemList;

// STATIC: 0x00F35FD4
TSGrowableArray<GuildNewsEvent> CGGuildInfo::m_guildNews;

// STATIC: 0x00F35FE4
int32_t CGGuildInfo::m_knownRecipeSkillLineID;

// STATIC: 0x00F35FE8
int32_t CGGuildInfo::m_knownRecipeSpellID;

// STATIC: 0x00F35FEC
TSGrowableArray<uint64_t> CGGuildInfo::m_knownRecipeMembers;

// STATIC: 0x00F34108
char CGGuildInfo::m_professionNameFilter[32];

// STATIC: 0x00F34128
uint8_t CGGuildInfo::m_guildMemberProfessionData[300];

// STATIC: 0x00F34254
SkillLine CGGuildInfo::m_guildMemberSkillLine;

// STATIC: 0x00F34260
GuildRankInfo CGGuildInfo::m_rankInfo[10];

// STATIC: 0x00F34800
GuildEventLogEntry CGGuildInfo::m_eventLog[100];

// STATIC: 0x00F35480
char CGGuildInfo::m_infoText[2004];

// STATIC: 0x00F35C58
char CGGuildInfo::m_motd[516];

// STATIC: 0x00F35E60
GuildSortCriteria CGGuildInfo::m_sortCriteria[NUM_GUILD_SORT_TYPES];

// STATIC: 0x00F340B8
int32_t CGGuildInfo::m_challengeGold[4];

// STATIC: 0x00F340C8
int32_t CGGuildInfo::m_challengeXP[4];

// STATIC: 0x00F340D8
int32_t CGGuildInfo::m_challengeMaxLevelGold[4];

// STATIC: 0x00F340E8
int32_t CGGuildInfo::m_challengeMaxCount[4];

// STATIC: 0x00F340F8
int32_t CGGuildInfo::m_challengeCurrentCount[4];

// STATIC: 0x00D11BF0
static const char* s_rosterFileName = "Logs\\GuildRoster.txt";

// STATIC: 0x00D11BF4
int32_t CGGuildInfo::m_selectedRankOrder = -1;

// STATIC: 0x00D11BF8
float CGGuildInfo::m_guildMasterAbsentDays = 90.0f;

// STATIC: 0x00D11BFC
float CGGuildInfo::m_guildMasterReplaceDays = 7.0f;

// STATIC: 0x00D11C00
int32_t CGGuildInfo::m_guildNewsFilter = -1;

// STATIC: 0x00D11C04
int32_t CGGuildInfo::m_professionCategoryFilter = -1;

// STATIC: 0x00D11FEC
TSList<CGGuildInfo::GuildProfessionHeader, TSGetLink<CGGuildInfo::GuildProfessionHeader> > CGGuildInfo::m_professionList;

// OFFSET: 0x0044BB90/0x140059250
uint32_t CGGuildInfo::GetNumMembers(int32_t includeOffline) {
    if (CGGuildInfo::m_showOffline) {
        return CGGuildInfo::m_numMembers;
    }

    uint32_t count = CGGuildInfo::m_numOnline;

    if (includeOffline) {
        count = CGGuildInfo::m_numMembers;
    }

    return count;
}

// PROVEN: 9fb711da IDENTICAL/NONE
// OFFSET: 0x0044E470
GuildMemberInfo* CGGuildInfo::GetMember(uint32_t index) {
    if (index >= CGGuildInfo::m_numMembers) {
        return NULL;
    }

    return CGGuildInfo::m_members[index];
}

// PROVEN: 22599f55 IDENTICAL/NONE
// OFFSET: 0x00931100
int32_t GuildMemberInfo::GetProfessionRank(int32_t skillLineID) {
    for (uint32_t i = 0; i < 2; i++) {
        if (this->m_professions[i].m_skillLineID == skillLineID) {
            return this->m_professions[i].m_rank;
        }
    }

    return 0;
}

// PROVEN: a0df2800 IDENTICAL/NONE
// OFFSET: 0x00931130
void CGGuildInfo::GetGuildMemberRecipes(SkillLine& skillLine, uint8_t* data) {
    skillLine = CGGuildInfo::m_guildMemberSkillLine;

    if (data) {
        memcpy(data, CGGuildInfo::m_guildMemberProfessionData, sizeof(CGGuildInfo::m_guildMemberProfessionData));
    }
}

// PROVEN: 12b6dcc8 IDENTICAL/NONE
// OFFSET: 0x00931170/0x1406991B0
int32_t CGGuildInfo::GetMemberProfessionRank(GuildMemberInfo* member, int32_t skillLineID) {
    if (!member) {
        return 0;
    }

    for (int32_t i = 0; i < 2; i++) {
        if (member->m_professions[i].m_skillLineID == skillLineID) {
            return member->m_professions[i].m_rank;
        }
    }

    return 0;
}

// OFFSET: 0x009311E0/0x140699230
void CGGuildInfo::SetSortType(GUILD_SORT_TYPE type) {
    for (uint32_t index = 0; index < NUM_GUILD_SORT_TYPES; index++) {
        if (CGGuildInfo::m_sortCriteria[index].m_type == type) {
            int32_t reverse = CGGuildInfo::m_sortCriteria[index].m_reverse;

            if (!index) {
                reverse = reverse == 0;
            }

            for (uint32_t i = index; i > 0; i--) {
                CGGuildInfo::m_sortCriteria[i] = CGGuildInfo::m_sortCriteria[i - 1];
            }

            CGGuildInfo::m_sortCriteria[0].m_type = type;
            CGGuildInfo::m_sortCriteria[0].m_reverse = reverse;
            return;
        }
    }
}

// PROVEN: fd4f309a IDENTICAL/NONE
// OFFSET: 0x00931250
static int32_t QSortWeeklyXP(const void* a, const void* b) {
    int64_t xpA = (*static_cast<GuildMemberInfo* const*>(a))->m_weeklyXP;
    int64_t xpB = (*static_cast<GuildMemberInfo* const*>(b))->m_weeklyXP;

    if (xpA == xpB) {
        return 0;
    }

    if (xpA < xpB) {
        return 1;
    }

    return -1;
}

// PROVEN: 2a441d5a IDENTICAL/NONE
// OFFSET: 0x00931290
static int32_t QSortTotalXP(const void* a, const void* b) {
    int64_t xpA = (*static_cast<GuildMemberInfo* const*>(a))->m_totalXP;
    int64_t xpB = (*static_cast<GuildMemberInfo* const*>(b))->m_totalXP;

    if (xpA == xpB) {
        return 0;
    }

    if (xpA < xpB) {
        return 1;
    }

    return -1;
}

// OFFSET: 0x009312D0
static int32_t QSortAchievementPoints(const void* a, const void* b) {
    GuildMemberInfo* memberA = *static_cast<GuildMemberInfo* const*>(a);
    GuildMemberInfo* memberB = *static_cast<GuildMemberInfo* const*>(b);

    if (memberA->m_achievementPoints == memberB->m_achievementPoints) {
        return 0;
    }

    return 2 * (memberA->m_achievementPoints < memberB->m_achievementPoints) - 1;
}

// PROVEN: 1460262f IDENTICAL/NONE
// OFFSET: 0x00931300
void CGGuildInfo::QueryPermissions() {
    GlobalGuildPermissionsQuery query;
    ClientServices::Send(&query);

    GlobalGuildBankRemainingWithdrawMoneyQuery moneyQuery;
    ClientServices::Send(&moneyQuery);
}

// PROVEN: 316cfdb9 IDENTICAL/NONE
// OFFSET: 0x00931350
int32_t CGGuildInfo::GetRankIndexFromRankOrder(int32_t rankOrder) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            return i;
        }
    }

    return -1;
}

// PROVEN: 6bc6cbb9 IDENTICAL/NONE
// OFFSET: 0x00931380/0x140699390
int32_t CGGuildInfo::GetRankOrderFromRankID(int32_t rankID) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == rankID) {
            return static_cast<int32_t>(i) >= 0 && static_cast<int32_t>(i) < static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) ? CGGuildInfo::m_rankInfo[i].m_rankOrder : -1;
        }
    }

    return -1;
}

// PROVEN: 16fc5e62 IDENTICAL/NONE
// OFFSET: 0x009313D0/0x1406993E0
int32_t CGGuildInfo::GetRankIDFromRankOrder(int32_t rankOrder) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            return CGGuildInfo::m_rankInfo[i].m_rankID;
        }
    }

    return -1;
}

// PROVEN: 1f2e4280 IDENTICAL/NONE
// OFFSET: 0x00931410
GuildRankInfo* CGGuildInfo::GetRankInfoFromRankID(int32_t rankID) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == rankID) {
            return static_cast<int32_t>(i) >= 0 && static_cast<int32_t>(i) < static_cast<int32_t>(CGGuildInfo::m_numUsedRanks) ? &CGGuildInfo::m_rankInfo[i] : NULL;
        }
    }

    return NULL;
}

// PROVEN: 63702c1a IDENTICAL/NONE
// OFFSET: 0x00931460/0x140699420
uint32_t CGGuildInfo::GetPermissionsByRankOrder(int32_t rankOrder) {
    if (rankOrder < 0 || rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        return 0;
    }

    int32_t index = CGGuildInfo::GetRankIndexFromRankOrder(rankOrder);

    if (index < 0) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_rights;
}

// PROVEN: 0a04c28a IDENTICAL/NONE
// OFFSET: 0x009314A0/0x140699470
uint32_t CGGuildInfo::GetPermissionsByRankID(int32_t rankID) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == rankID) {
            return static_cast<int32_t>(i) >= 0 ? CGGuildInfo::m_rankInfo[i].m_rights : 0;
        }
    }

    return 0;
}

// PROVEN: f1b8bc49 IDENTICAL/NONE
// OFFSET: 0x009314E0/0x1406994C0
int32_t CGGuildInfo::GetWithdrawGoldLimitForSelectedRank() {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_withdrawGoldLimit;
}

// PROVEN: aa5fa092 IDENTICAL/NONE
// OFFSET: 0x00931530/0x140699520
int32_t CGGuildInfo::GetWithdrawGoldLimitByRankID(uint32_t rankID) {
    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == static_cast<int32_t>(rankID)) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_withdrawGoldLimit;
}

// OFFSET: 0x00931580
void CGGuildInfo::SetWithdrawGoldLimitForSelectedRank(uint32_t amount) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        CGGuildInfo::m_rankInfo[index].m_withdrawGoldLimit = amount > 100000 ? 100000 : amount;
    }
}

// PROVEN: 501e4870 IDENTICAL/NONE
// OFFSET: 0x009315F0
void CGGuildInfo::SetTabWithdrawItemLimitForSelectedRank(uint32_t tab, int32_t amount) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            if (static_cast<int32_t>(i) >= 0 && tab < 8) {
                CGGuildInfo::m_rankInfo[i].m_tabWithdrawItemLimit[tab] = amount > 100000 ? 100000 : amount;
            }

            return;
        }
    }
}

// OFFSET: 0x00931660/0x140699660
uint32_t CGGuildInfo::GetTabFlagsForSelectedRank(uint32_t tab) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    if (tab >= 8) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_tabFlags[tab];
}

// OFFSET: 0x009316C0/0x1406996D0
uint32_t CGGuildInfo::GetGuildBankTabPermissionsByRankID(uint32_t tab, uint32_t rankID) {
    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == static_cast<int32_t>(rankID)) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_tabFlags[tab];
}

// PROVEN: 9503acd9 IDENTICAL/NONE
// OFFSET: 0x00931710
int32_t CGGuildInfo::SetTabFlagsForSelectedRank(int32_t tab, uint32_t flags) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            if (static_cast<int32_t>(i) >= 0) {
                CGGuildInfo::m_rankInfo[i].m_tabFlags[tab] = flags;
            }

            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x00931770/0x140699790
int32_t CGGuildInfo::GetTabWithdrawItemLimitForSelectedRank(uint32_t tab) {
    int32_t rankOrder = CGGuildInfo::m_selectedRankOrder;

    if (rankOrder >= static_cast<int32_t>(CGGuildInfo::m_numUsedRanks)) {
        rankOrder = -1;
        CGGuildInfo::m_selectedRankOrder = -1;
    }

    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankOrder == rankOrder) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    if (tab >= 8) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_tabWithdrawItemLimit[tab];
}

// OFFSET: 0x009317D0/0x140699800
int32_t CGGuildInfo::GetNumGuildBankTabItemWithdrawalsByRankID(uint32_t tab, uint32_t rankID) {
    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == static_cast<int32_t>(rankID)) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return 0;
    }

    if (tab >= 8) {
        return 0;
    }

    return CGGuildInfo::m_rankInfo[index].m_tabWithdrawItemLimit[tab];
}

// PROVEN: b056c625 IDENTICAL/NONE
// OFFSET: 0x00931820/0x140699850
void CGGuildInfo::UpdateRoster(int32_t force) {
    if (force) {
        CGGuildInfo::m_nextAllowedQuery = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, "%d", 1);
        return;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
}

// PROVEN: 2199a9e9 IDENTICAL/NONE
// OFFSET: 0x00931860
void CGGuildInfo::GuildRewardsItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (CGGuildInfo::m_guildRewardStatCBCount > 0) {
        CGGuildInfo::m_guildRewardStatCBCount--;
    }

    if (!CGGuildInfo::m_guildRewardStatCBCount) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REWARDS_LIST, NULL);
    }
}

// OFFSET: 0x00931890/0x1406998B0
static int32_t QSortGuildNews(const void* a, const void* b) {
    const GuildNewsEvent* newsA = static_cast<const GuildNewsEvent*>(a);
    const GuildNewsEvent* newsB = static_cast<const GuildNewsEvent*>(b);

    bool hiddenA = !((1 << newsA->m_type) & CGGuildInfo::m_guildNewsFilter) && !newsA->m_isHeader;
    bool hiddenB = !((1 << newsB->m_type) & CGGuildInfo::m_guildNewsFilter) && !newsB->m_isHeader;

    if (hiddenA) {
        if (!hiddenB) {
            return 1;
        }
    } else if (hiddenB) {
        return -1;
    }

    if (newsA->m_flags & 0x1) {
        if (!newsB->m_flags & 0x1) {
            return -1;
        }

        return newsA->m_date.Compare(newsB->m_date);
    }

    if (newsB->m_flags & 0x1) {
        return 1;
    }

    if (newsA->m_isHeader) {
        if (newsA->m_date.IsSameDay(newsB->m_date)) {
            return -1;
        }
    } else if (newsB->m_isHeader && newsA->m_date.IsSameDay(newsB->m_date)) {
        return 1;
    }

    int32_t result = newsA->m_date.Compare(newsB->m_date);

    if (!result) {
        return 2 * (newsA->m_id < newsB->m_id) - 1;
    }

    return result;
}

// OFFSET: 0x00931980/0x1406999A0
static int32_t QSortGuildNewsByDate(const void* a, const void* b) {
    const GuildNewsEvent* newsA = static_cast<const GuildNewsEvent*>(a);
    const GuildNewsEvent* newsB = static_cast<const GuildNewsEvent*>(b);

    if (newsA->m_flags & 0x1) {
        if (!newsB->m_flags & 0x1) {
            return -1;
        }

        return newsA->m_date.Compare(newsB->m_date);
    }

    if ((newsB->m_flags & 0x1) || newsA->m_isHeader) {
        return 1;
    }

    if (newsB->m_isHeader) {
        return -1;
    }

    int32_t result = newsA->m_date.Compare(newsB->m_date);

    if (!result) {
        return 2 * (newsA->m_id < newsB->m_id) - 1;
    }

    return result;
}

// PROVEN: cc509590 IDENTICAL/NONE
// OFFSET: 0x009319F0
void CGGuildInfo::GuildNewsItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_NEWS_UPDATE, NULL);
}

// PROVEN: 14ed0abe IDENTICAL/NONE
// OFFSET: 0x009319F0
void CGGuildInfo::GuildNewsNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_NEWS_UPDATE, NULL);
}

// PROVEN: 03ec115e IDENTICAL/NONE
// OFFSET: 0x00931A00
bool CGGuildInfo::ClientGuildMoveStartingHandler(ClientGuildMoveStarting* msg) {
    CGChat::AddChatMessage(FrameScript_GetText("GUILD_MOVE_STARTING", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    return true;
}

// PROVEN: b973e57b IDENTICAL/NONE
// OFFSET: 0x00931A30
bool CGGuildInfo::ClientGuildMoveCompleteHandler(ClientGuildMoveComplete* msg) {
    CGChat::AddChatMessage(FrameScript_GetText("GUILD_MOVE_COMPLETE", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    return true;
}

// PROVEN: 8b8853b2 IDENTICAL/NONE
// OFFSET: 0x00931A60/0x140699D80
int32_t CGGuildInfo::GetRewardMissingAchievement(const JamGuildRewardItem* item) {
    if (item && item->m_achievementRequired && !CGAchievementInfo::GetAchievement(item->m_achievementRequired)) {
        return item->m_achievementRequired;
    }

    return 0;
}

// PROVEN: e2c06ed3 IDENTICAL/NONE
// OFFSET: 0x00931A90/0x140699DB0
int32_t CGGuildInfo::GetGuildReputationWeeklyCap() {
    return CGGuildInfo::m_guildReputationWeeklyCap;
}

// OFFSET: 0x009311A0
bool CGGuildInfo::ClientGuildMemberDailyResetHandler(ClientGuildMemberDailyReset* msg) {
    CGGuildBankInfo::QueryGuildBankTab(CGGuildBankInfo::m_currentTab);

    GlobalGuildBankRemainingWithdrawMoneyQuery query;
    ClientServices::Send(&query);
    return true;
}

// PROVEN: 5d958491 IDENTICAL/NONE
// OFFSET: 0x009326E0
void CGGuildInfo::EventLogNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGGuildInfo::m_pendingEventLogNames || !--CGGuildInfo::m_pendingEventLogNames) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_EVENT_LOG_UPDATE, NULL);
    }
}

// OFFSET: 0x00932710
int32_t CGGuildInfo::QSortRoster(const void* a, const void* b) {
    GuildMemberInfo* memberA = *static_cast<GuildMemberInfo* const*>(a);
    GuildMemberInfo* memberB = *static_cast<GuildMemberInfo* const*>(b);

    if (!CGGuildInfo::m_showOffline && (!memberA->m_online || !memberB->m_online) && memberA->m_online != memberB->m_online) {
        return 2 * (memberA->m_online == 0) - 1;
    }

    for (uint32_t i = 0; i < NUM_GUILD_SORT_TYPES; i++) {
        GuildSortCriteria* criteria = &CGGuildInfo::m_sortCriteria[i];

        if (!criteria) {
            continue;
        }

        int32_t result;

        switch (criteria->m_type) {
            case GUILD_SORT_RANK: {
                int32_t orderB = CGGuildInfo::GetRankOrderFromRankID(memberB->m_rankID);
                int32_t orderA = CGGuildInfo::GetRankOrderFromRankID(memberA->m_rankID);

                if (orderA == orderB) {
                    continue;
                }

                result = 2 * (orderA <= orderB) - 1;
                break;
            }

            case GUILD_SORT_LEVEL: {
                if (memberA->m_level == memberB->m_level) {
                    continue;
                }

                result = 2 * (memberA->m_level >= memberB->m_level) - 1;
                break;
            }

            case GUILD_SORT_NAME: {
                result = SStrCmpUTF8I(memberA->m_name, memberB->m_name, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_ZONE: {
                uint8_t mobileA = memberA->m_status & 0x10;

                if (mobileA) {
                    if (mobileA == (memberB->m_status & 0x10)) {
                        continue;
                    }

                    result = 2 * (mobileA != 0) - 1;
                    break;
                }

                if (memberB->m_status & 0x10) {
                    result = 2 * (mobileA != 0) - 1;
                    break;
                }

                AreaTableRec* areaA = g_areaTableDB.GetRecord(memberA->m_areaID);
                AreaTableRec* areaB = g_areaTableDB.GetRecord(memberB->m_areaID);

                if (!areaA || !areaB) {
                    continue;
                }

                result = SStrSortUTF8I(areaA->m_areaName, areaB->m_areaName, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_CLASS: {
                ChrClassesRec* classA = memberA->m_classID < g_chrClassesDB.m_minID || memberA->m_classID > g_chrClassesDB.m_maxID ? NULL : reinterpret_cast<ChrClassesRec*>(g_chrClassesDB.m_recordsById[memberA->m_classID - g_chrClassesDB.m_minID]);
                ChrClassesRec* classB = memberB->m_classID < g_chrClassesDB.m_minID || memberB->m_classID > g_chrClassesDB.m_maxID ? NULL : reinterpret_cast<ChrClassesRec*>(g_chrClassesDB.m_recordsById[memberB->m_classID - g_chrClassesDB.m_minID]);

                if (!classA || !classB) {
                    continue;
                }

                result = SStrSortUTF8I(classA->m_name, classB->m_name, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_ONLINE: {
                if (memberA->m_online && memberB->m_online) {
                    continue;
                }

                if (memberA->m_online != memberB->m_online) {
                    result = 2 * (memberA->m_online == 0) - 1;
                    break;
                }

                result = memberB->m_lastSave <= memberA->m_lastSave ? 1 : -1;
                return criteria->m_reverse ? -result : result;
            }

            case GUILD_SORT_NOTE: {
                result = SStrSortUTF8I(memberA->m_note, memberB->m_note, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_BGRATING: {
                if (memberA->m_bgRating == memberB->m_bgRating) {
                    continue;
                }

                result = 2 * (memberA->m_bgRating <= memberB->m_bgRating) - 1;
                break;
            }

            case GUILD_SORT_WEEKLYXP: {
                if (memberA->m_weeklyXPRank == memberB->m_weeklyXPRank) {
                    continue;
                }

                result = 2 * (memberA->m_weeklyXPRank <= memberB->m_weeklyXPRank) - 1;
                break;
            }

            case GUILD_SORT_TOTALXP: {
                if (memberA->m_totalXPRank == memberB->m_totalXPRank) {
                    continue;
                }

                result = 2 * (memberA->m_totalXPRank <= memberB->m_totalXPRank) - 1;
                break;
            }

            case GUILD_SORT_ACHIEVEMENTPOINTS: {
                if (memberA->m_achievementRank == memberB->m_achievementRank) {
                    continue;
                }

                result = 2 * (memberA->m_achievementRank <= memberB->m_achievementRank) - 1;
                break;
            }

            default: {
                continue;
            }
        }

        if (result) {
            return criteria->m_reverse ? -result : result;
        }
    }

    return 0;
}

// OFFSET: 0x009329F0
void CGGuildInfo::SetGuildMemberRecipes(const uint64_t& guid, int32_t skillLineID, int32_t skillRank, int32_t skillStep, uint8_t* data, int32_t size) {
    if (!data || size <= 0) {
        return;
    }

    uint32_t numMembers = CGGuildInfo::GetNumMembers(1);

    for (uint32_t i = 0; i < numMembers; i++) {
        GuildMemberInfo* member = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (member && member->m_guid == guid) {
            CGGuildInfo::m_guildMemberSkillsGuid = member->m_guid;
            CGGuildInfo::m_guildMemberSkillLine.m_skillLineID = skillLineID;
            CGGuildInfo::m_guildMemberSkillLine.m_skillRank = skillRank;
            CGGuildInfo::m_guildMemberSkillLine.m_skillStep = skillStep;
            memset(CGGuildInfo::m_guildMemberProfessionData, 0, sizeof(CGGuildInfo::m_guildMemberProfessionData));
            memcpy(CGGuildInfo::m_guildMemberProfessionData, data, static_cast<uint32_t>(size) > 300 ? 300 : size);
            CGTradeSkillInfo::OpenGuildMemberTradeSkill(CGGuildInfo::m_guildMemberSkillsGuid, CGGuildInfo::m_guildMemberSkillLine);
            return;
        }
    }
}

// OFFSET: 0x00932AD0/0x14069B1E0
bool CGGuildInfo::GetGuildRecipeInfoMember(int32_t index, char* name, char* className, bool& online) {
    if (index < 0 || index > static_cast<int32_t>(CGGuildInfo::m_knownRecipeMembers.Count())) {
        return false;
    }

    uint32_t numMembers = CGGuildInfo::GetNumMembers(1);
    GuildMemberInfo* member = NULL;

    for (uint32_t i = 0; i < numMembers; i++) {
        GuildMemberInfo* info = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (info && info->m_guid == CGGuildInfo::m_knownRecipeMembers[index]) {
            member = info;
            break;
        }
    }

    if (!member) {
        return false;
    }

    SStrCopy(name, member->m_name, 48);

    ChrClassesRec* classRec = member->m_classID < g_chrClassesDB.m_minID || member->m_classID > g_chrClassesDB.m_maxID ? NULL : reinterpret_cast<ChrClassesRec*>(g_chrClassesDB.m_recordsById[member->m_classID - g_chrClassesDB.m_minID]);

    if (classRec) {
        SStrCopy(className, classRec->m_filename, 128);
    }

    online = member->m_online != 0;
    return true;
}

// OFFSET: 0x00932BD0
bool CGGuildInfo::ClientGuildEventLogQueryResultsHandler(ClientGuildEventLogQueryResults* msg) {
    int32_t now = _time32(NULL);

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGGuildInfo::m_numEventLogEntries); i++) {
        g_nameDBCache.CancelCallback(CGGuildInfo::m_eventLog[i].m_playerGUID, &CGGuildInfo::EventLogNameCacheCallback, NULL);

        if (CGGuildInfo::m_eventLog[i].m_otherGUID) {
            g_nameDBCache.CancelCallback(CGGuildInfo::m_eventLog[i].m_otherGUID, &CGGuildInfo::EventLogNameCacheCallback, NULL);
        }
    }

    CGGuildInfo::m_pendingEventLogNames = 0;
    CGGuildInfo::m_numEventLogEntries = msg->m_entry.Count() > 100 ? 100 : msg->m_entry.Count();

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGGuildInfo::m_numEventLogEntries); i++) {
        JamGuildEventLogEntry& entry = msg->m_entry[i];
        GuildEventLogEntry& log = CGGuildInfo::m_eventLog[i];

        log.m_type = entry.m_type;
        log.m_playerGUID = *reinterpret_cast<uint64_t*>(entry.m_playerGUID);

        WOWGUID requester = WOWGUID::Make(0);

        if (!g_nameDBCache.GetRecord(log.m_playerGUID, requester, &CGGuildInfo::EventLogNameCacheCallback, NULL, false)) {
            CGGuildInfo::m_pendingEventLogNames++;
        }

        log.m_otherGUID = *reinterpret_cast<uint64_t*>(entry.m_otherGUID);

        if (log.m_otherGUID && !(log.m_otherGUID & 0xF000000000000000ull) && (log.m_otherGUID & 0xF07FFFFFFFFFFFFFull)) {
            WOWGUID otherRequester = WOWGUID::Make(0);

            if (!g_nameDBCache.GetRecord(log.m_otherGUID, otherRequester, &CGGuildInfo::EventLogNameCacheCallback, NULL, false)) {
                CGGuildInfo::m_pendingEventLogNames++;
            }
        }

        log.m_rankID = entry.m_rankID;
        log.m_time = now - entry.m_timeOffset;
    }

    if (!CGGuildInfo::m_pendingEventLogNames) {
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_EVENT_LOG_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x00932D60/0x14069B500
int32_t CGGuildInfo::GetMemberIndex(const uint64_t& guid) {
    for (uint32_t i = 0; ; i++) {
        uint32_t count = CGGuildInfo::m_numOnline;

        if (CGGuildInfo::m_showOffline) {
            count = CGGuildInfo::m_numMembers;
        }

        if (i >= count) {
            break;
        }

        GuildMemberInfo* member = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (member && member->m_guid == guid) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x00932DC0/0x14069B570
const char* CGGuildInfo::GetGuildReputationTitleString(const uint64_t& guid) {
    uint32_t index = 0;

    if (!CGGuildInfo::m_numMembers) {
        return NULL;
    }

    while (CGGuildInfo::m_members[index]->m_guid != guid) {
        if (++index >= CGGuildInfo::m_numMembers) {
            return NULL;
        }
    }

    int32_t gender = 2;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(CGGuildInfo::m_members[index]->m_guid), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit && unit->m_unit->m_unitFieldBytes0[2] == 1) {
        gender = 3;
    }

    int32_t reputation = CGGuildInfo::m_members[index]->m_guildReputation;

    if (reputation >= 42000) {
        return FrameScript_GetText("GUILD_FACTION_TITLE_EXALTED", -1, static_cast<FRAMESCRIPT_GENDER>(gender));
    }

    if (reputation >= 21000) {
        return FrameScript_GetText("GUILD_FACTION_TITLE_REVERED", -1, static_cast<FRAMESCRIPT_GENDER>(gender));
    }

    if (reputation >= 9000) {
        return FrameScript_GetText("GUILD_FACTION_TITLE_HONORED", -1, static_cast<FRAMESCRIPT_GENDER>(gender));
    }

    if (reputation < 3000) {
        return NULL;
    }

    return FrameScript_GetText("GUILD_FACTION_TITLE_FRIENDLY", -1, static_cast<FRAMESCRIPT_GENDER>(gender));
}

// PROVEN: fe18de79 IDENTICAL/NONE
// OFFSET: 0x00932ED0
int32_t CGGuildInfo::GetSelectedMemberIndex() {
    if (!CGGuildInfo::m_selectedMember) {
        return -1;
    }

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        if (CGGuildInfo::m_members[i]->m_guid == CGGuildInfo::m_selectedMember) {
            return i;
        }
    }

    return -1;
}

// PROVEN: 90d73578 IDENTICAL/NONE
// OFFSET: 0x00932F20
void CGGuildInfo::SetMOTD(const char* motd) {
    SStrCopy(CGGuildInfo::m_motd, motd, sizeof(CGGuildInfo::m_motd));
}

// OFFSET: 0x00932F60
static int32_t QSortTradeSkillMembers(const void* a, const void* b) {
    if (!a || !b) {
        return 0;
    }

    uint32_t indexA = static_cast<const GuildProfessionMember*>(a)->m_memberIndex;
    GuildMemberInfo* memberA = indexA >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[indexA];
    uint32_t indexB = static_cast<const GuildProfessionMember*>(b)->m_memberIndex;
    GuildMemberInfo* memberB = indexB >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[indexB];

    if (!memberA || !memberB) {
        return 0;
    }

    for (uint32_t i = 0; i < NUM_GUILD_SORT_TYPES; i++) {
        GuildSortCriteria* criteria = &CGGuildInfo::m_sortCriteria[i];

        if (!criteria) {
            continue;
        }

        int32_t result;

        switch (criteria->m_type) {
            case GUILD_SORT_NAME: {
                result = SStrCmpUTF8I(memberA->m_name, memberB->m_name, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_ZONE: {
                AreaTableRec* areaA = g_areaTableDB.GetRecord(memberA->m_areaID);
                AreaTableRec* areaB = g_areaTableDB.GetRecord(memberB->m_areaID);

                if (!areaA || !areaB) {
                    continue;
                }

                result = SStrSortUTF8I(areaA->m_areaName, areaB->m_areaName, STORM_MAX_STR);
                break;
            }

            case GUILD_SORT_SKILL: {
                int32_t rankA = memberA->GetProfessionRank(CGGuildInfo::m_sortSkillLineID);
                int32_t rankB = memberB->GetProfessionRank(CGGuildInfo::m_sortSkillLineID);

                if (rankA == rankB) {
                    continue;
                }

                result = 2 * (rankA <= rankB) - 1;
                break;
            }

            default: {
                continue;
            }
        }

        if (result) {
            return criteria->m_reverse ? -result : result;
        }
    }

    return 0;
}

// PROVEN: 95b45d15 IDENTICAL/NONE
// OFFSET: 0x009330A0
void CGGuildInfo::ComputeContributionRanks() {
    CGGuildInfo::m_largestWeeklyXPContribution = 0;
    CGGuildInfo::m_largestTotalXPContribution = 0;
    CGGuildInfo::m_largestPersonalAchievementPoints = 0;

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        if (CGGuildInfo::m_members[i]->m_weeklyXP > CGGuildInfo::m_largestWeeklyXPContribution) {
            CGGuildInfo::m_largestWeeklyXPContribution = CGGuildInfo::m_members[i]->m_weeklyXP;
        }

        if (CGGuildInfo::m_members[i]->m_totalXP > CGGuildInfo::m_largestTotalXPContribution) {
            CGGuildInfo::m_largestTotalXPContribution = CGGuildInfo::m_members[i]->m_totalXP;
        }

        if (CGGuildInfo::m_members[i]->m_achievementPoints > CGGuildInfo::m_largestPersonalAchievementPoints) {
            CGGuildInfo::m_largestPersonalAchievementPoints = CGGuildInfo::m_members[i]->m_achievementPoints;
        }
    }

    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortWeeklyXP);

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        CGGuildInfo::m_members[i]->m_weeklyXPRank = i;
    }

    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortTotalXP);

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        CGGuildInfo::m_members[i]->m_totalXPRank = i;
    }

    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortAchievementPoints);

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        CGGuildInfo::m_members[i]->m_achievementRank = i;
    }
}

// OFFSET: 0x00933200/0x14069BAC0
void CGGuildInfo::UpdateMemberInfo(GuildMemberInfo* info, const ClientGuildRosterMemberData* data) {
    if (info->m_name[0]) {
        CGAutoCompleteName::RemoveFlags(WOWGUID::Make(info->m_guid), 0x22);
    }

    info->m_online = data->m_status & 0x1;
    info->m_guid = *reinterpret_cast<const uint64_t*>(data->m_guid);
    info->m_status = data->m_status;
    info->m_rankID = data->m_rankID;
    info->m_level = data->m_level;
    info->m_classID = data->m_classID;
    info->m_gender = data->m_gender;
    info->m_areaID = data->m_areaID;
    info->m_weeklyXP = static_cast<int32_t>(data->m_weeklyXP);
    info->m_totalXP = static_cast<int32_t>(data->m_totalXP);
    info->m_weeklyXPRank = 0;
    info->m_totalXPRank = 0;
    info->m_achievementRank = 0;
    info->m_bgRating = data->m_bgRating;
    info->m_achievementPoints = data->m_personalAchievementPoints;
    info->m_guildReputation = data->m_guildReputation;
    info->m_guildRepToCap = data->m_guildRepToCap;
    info->m_authenticated = data->m_authenticated;
    info->m_sorEligible = data->m_sorEligible;
    SStrCopy(info->m_name, data->m_name, sizeof(info->m_name));

    for (uint32_t i = 0; i < 2; i++) {
        info->m_professions[i].m_skillLineID = data->m_profession[i].m_dbID;
        info->m_professions[i].m_rank = data->m_profession[i].m_rank;
        info->m_professions[i].m_step = data->m_profession[i].m_step;
    }

    uint32_t flags = 0x2;

    if (info->m_online) {
        info->m_lastSave = 0.0f;
        flags = 0x22;
    } else {
        info->m_lastSave = data->m_lastSave;
    }

    CGAutoCompleteName::AddFlags(WOWGUID::Make(info->m_guid), flags, 0, info->m_name);
    ScrubString(data->m_note, info->m_note, 32, sizeof(info->m_note), 1);
    ScrubString(data->m_officerNote, info->m_officerNote, 32, sizeof(info->m_officerNote), 1);

    if (info->m_online && PlayerNameShouldShowGuildTitle()) {
        CGObject_C* object = ClntObjMgrObjectPtr(WOWGUID::Make(info->m_guid), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (object) {
            PlayerNameTriggerNameRegenerate(object->m_nameDesc);
        }
    }
}

// OFFSET: 0x009333F0
bool CGGuildInfo::ClientGuildRanksHandler(ClientGuildRanks* msg) {
    CGGuildInfo::m_numUsedRanks = msg->m_ranks.Count();

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        JamCliGuildRankData& rank = msg->m_ranks[i];
        GuildRankInfo& info = CGGuildInfo::m_rankInfo[i];

        SStrCopy(info.m_name, rank.m_rankName, sizeof(info.m_name));
        info.m_rankID = rank.m_rankID;
        info.m_rankOrder = rank.m_rankOrder;
        info.m_rights = rank.m_flags;
        info.m_withdrawGoldLimit = rank.m_withdrawGoldLimit;

        for (uint32_t j = 0; j < 8; j++) {
            info.m_tabFlags[j] = rank.m_tabFlags[j];
            info.m_tabWithdrawItemLimit[j] = rank.m_tabWithdrawItemLimit[j];
        }
    }

    for (uint32_t i = CGGuildInfo::m_numUsedRanks; i < 10; i++) {
        GuildRankInfo& info = CGGuildInfo::m_rankInfo[i];

        info.m_rankID = 0;
        info.m_rankOrder = 0;
        info.m_name[0] = 0;
        info.m_rights = 0;
        info.m_withdrawGoldLimit = 0;

        for (uint32_t j = 0; j < 8; j++) {
            info.m_tabFlags[j] = 0;
            info.m_tabWithdrawItemLimit[j] = 0;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_RANKS_UPDATE, NULL);
    return true;
}

// OFFSET: 0x00933560/0x14069BEA0
int32_t CGGuildInfo::GetNumMembersInRank(int32_t rankOrder) {
    int32_t rankID = CGGuildInfo::GetRankIDFromRankOrder(rankOrder);
    int32_t count = 0;

    if (rankID >= 0) {
        for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
            if (CGGuildInfo::m_members[i]->m_rankID == rankID) {
                count++;
            }
        }
    }

    return count;
}

// PROVEN: 545c495b IDENTICAL/NONE
// OFFSET: 0x009335B0/0x14069BF20
const char* CGGuildInfo::GetRankNameFromRankID(int32_t rankID) {
    int32_t index = -1;

    for (uint32_t i = 0; i < CGGuildInfo::m_numUsedRanks; i++) {
        if (CGGuildInfo::m_rankInfo[i].m_rankID == rankID) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return NULL;
    }

    return CGGuildInfo::m_rankInfo[index].m_name;
}

// PROVEN: f85641a9 IDENTICAL/NONE
// OFFSET: 0x00933600
void CGGuildInfo::InvalidatePlayer(uint64_t guid) {
    int32_t now = FrameTime::GetCurTimeMs();

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        if (CGGuildInfo::m_members[i]->m_guid == guid) {
            GlobalGuildGetRoster roster;
            ClientServices::Send(&roster);
            CGGuildInfo::m_nextAllowedQuery = now + 10000;
            return;
        }
    }
}

// PROVEN: 4e1899bb IDENTICAL/NONE
// OFFSET: 0x00933680
bool CGGuildInfo::ClientGuildChallengeUpdateHandler(ClientGuildChallengeUpdate* msg) {
    for (uint32_t i = 0; i < 4; i++) {
        CGGuildInfo::m_challengeCurrentCount[i] = msg->m_challengeCurrentCount[i];
        CGGuildInfo::m_challengeMaxCount[i] = msg->m_challengeMaxCount[i];
        CGGuildInfo::m_challengeGold[i] = msg->m_challengeGoldReward[i];
        CGGuildInfo::m_challengeMaxLevelGold[i] = msg->m_challengeMaxLevelGoldReward[i];
        CGGuildInfo::m_challengeXP[i] = msg->m_challengeXPReward[i];
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_CHALLENGE_UPDATED, NULL);
    return true;
}

// OFFSET: 0x00933750
bool CGGuildInfo::ClientGuildChallengeCompletedHandler(ClientGuildChallengeCompleted* msg) {
    if (static_cast<uint32_t>(msg->m_challengeType) > 3) {
        return true;
    }

    CGGuildInfo::m_challengeCurrentCount[msg->m_challengeType] = msg->m_currentCount;

    const char* label;

    switch (msg->m_challengeType) {
        case 0:
        case 1: {
            label = FrameScript_GetText("GUILD_CHALLENGE_LABEL1", -1, GENDER_NOT_APPLICABLE);
            break;
        }

        case 2: {
            label = FrameScript_GetText("GUILD_CHALLENGE_LABEL2", -1, GENDER_NOT_APPLICABLE);
            break;
        }

        case 3: {
            label = FrameScript_GetText("GUILD_CHALLENGE_LABEL3", -1, GENDER_NOT_APPLICABLE);
            break;
        }

        default: {
            label = NULL;
            break;
        }
    }

    if (label) {
        char text[3000];

        if (msg->m_xpAwarded <= 0) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_CHALLENGE_COMPLETE_GOLD", -1, GENDER_NOT_APPLICABLE), label);
        } else if (msg->m_goldAwarded) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_CHALLENGE_COMPLETE_XP_GOLD", -1, GENDER_NOT_APPLICABLE), label);
        } else {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_CHALLENGE_COMPLETE_XP", -1, GENDER_NOT_APPLICABLE), label);
        }

        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_CHALLENGE_COMPLETED, "%d%d%d%d%d", msg->m_challengeType, msg->m_currentCount, msg->m_maxCount, msg->m_xpAwarded, msg->m_goldAwarded);
    return true;
}

// PROVEN: b1d990b8 IDENTICAL/NONE
// OFFSET: 0x009338A0
bool CGGuildInfo::ClientGuildChangeNameResultHandler(ClientGuildChangeNameResult* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_REQUIRED_GUILD_RENAME_RESULT, "%b", msg->m_success);
    return true;
}

// PROVEN: dc24a7ce IDENTICAL/NONE
// OFFSET: 0x009338D0
bool CGGuildInfo::ClientGuildFlaggedForRenameHandler(ClientGuildFlaggedForRename* msg) {
    if (msg->m_flagSet) {
        CGGuildInfo::m_guildFlags |= 0x1;
    } else {
        CGGuildInfo::m_guildFlags &= ~0x1;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_RENAME_REQUIRED, "%b", msg->m_flagSet);
    return true;
}

// PROVEN: fcd787ad IDENTICAL/NONE
// OFFSET: 0x00933910
GuildNewsEvent* CGGuildInfo::GetGuildNewsEvent(int32_t index) {
    if (index < 0 || index >= CGGuildInfo::m_numGuildNews) {
        return NULL;
    }

    return &CGGuildInfo::m_guildNews[index];
}

// PROVEN: 538f911d IDENTICAL/NONE
// OFFSET: 0x00933940/0x14069C000
void CGGuildInfo::SortGuildNewsByDate() {
    qsort(CGGuildInfo::m_guildNews.m_data, CGGuildInfo::m_guildNews.Count(), sizeof(GuildNewsEvent), QSortGuildNewsByDate);
    CGGuildInfo::m_numGuildNews = 0;

    for (uint32_t i = 0; i < CGGuildInfo::m_guildNews.Count() && !CGGuildInfo::m_guildNews[i].m_isHeader; i++) {
        CGGuildInfo::m_numGuildNews++;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_NEWS_UPDATE, NULL);
}

// PROVEN: 8b5388ef IDENTICAL/NONE
// OFFSET: 0x009339B0
const char* CGGuildInfo::GetMemberName(const uint64_t& guid) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        GuildMemberInfo* member = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (member && member->m_guid == guid) {
            return member->m_name;
        }
    }

    return FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
}

// PROVEN: e553c288 IDENTICAL/NONE
// OFFSET: 0x00933A10/0x14069C0C0
bool CGGuildInfo::GetMemberGUIDFromName(const char* name, uint64_t& guid) {
    if (!name) {
        return false;
    }

    for (uint32_t i = 0; i < CGGuildInfo::GetNumMembers(1); i++) {
        GuildMemberInfo* member = i >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[i];

        if (member && !SStrCmpUTF8I(member->m_name, name, STORM_MAX_STR)) {
            guid = member->m_guid;
            return true;
        }
    }

    return false;
}

// PROVEN: 4ce72d57 IDENTICAL/NONE
// OFFSET: 0x00933A90
void CGGuildInfo::SetMemberMobile(uint64_t guid, bool mobile) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        if (CGGuildInfo::m_members[i] && CGGuildInfo::m_members[i]->m_guid == guid) {
            if (mobile) {
                CGGuildInfo::m_members[i]->m_status |= 0x10;
            } else {
                CGGuildInfo::m_members[i]->m_status &= ~0x10;
            }
        }
    }
}

// PROVEN: a3bef9e0 IDENTICAL/NONE
// OFFSET: 0x00933AF0/0x14069C1C0
int32_t CGGuildInfo::GetMemberReputationToCap(const uint64_t& guid) {
    int32_t index = CGGuildInfo::GetMemberIndex(guid);
    GuildMemberInfo* member;

    if (index >= 0 && static_cast<uint32_t>(index) < CGGuildInfo::m_numMembers && (member = CGGuildInfo::m_members[index])) {
        return member->m_guildRepToCap;
    }

    return 0;
}

// PROVEN: 2ff85acc IDENTICAL/NONE
// OFFSET: 0x00934F20
bool CGGuildInfo::ClientGuildPartyStateHandler(ClientGuildPartyState* msg) {
    CGGuildInfo::m_inGuildParty = msg->m_inGuildParty;
    CGGuildInfo::m_numGuildGroupMembers = msg->m_numGuildGroupMembers;
    CGGuildInfo::m_numGuildGroupRequired = msg->m_numGuildGroupRequired;
    CGGuildInfo::m_guildXPEarnedMult = msg->m_guildXPEarnedMult;
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_PARTY_STATE_UPDATED, "%b", CGGuildInfo::m_inGuildParty);
    return true;
}

// OFFSET: 0x00935060
static int32_t QSortRecipeMembers(const void* a, const void* b) {
    uint64_t guidA = *static_cast<const uint64_t*>(a);
    uint64_t guidB = *static_cast<const uint64_t*>(b);

    uint32_t indexA = CGGuildInfo::GetMemberIndex(guidA);
    GuildMemberInfo* memberA = indexA >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[indexA];
    uint32_t indexB = CGGuildInfo::GetMemberIndex(guidB);
    GuildMemberInfo* memberB = indexB >= CGGuildInfo::m_numMembers ? NULL : CGGuildInfo::m_members[indexB];

    if (memberA && memberB) {
        if (memberA->m_online != memberB->m_online) {
            return 2 * (memberA->m_online == 0) - 1;
        }

        return SStrCmpUTF8I(memberA->m_name, memberB->m_name, STORM_MAX_STR);
    }

    return guidA < guidB ? -1 : 1;
}

// PROVEN: ad83d57c IDENTICAL/NONE
// OFFSET: 0x00935130
bool CGGuildInfo::ClientGuildPermissionsQueryResultsHandler(ClientGuildPermissionsQueryResults* msg) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGGuildBankInfo::m_numTabs = msg->m_numTabs;

        GuildRankInfo* rank = CGGuildInfo::GetRankInfoFromRankID(msg->m_rankID);

        if (rank) {
            rank->m_rights = msg->m_flags;
            rank->m_withdrawGoldLimit = msg->m_withdrawGoldLimit;

            for (uint32_t i = 0; i < msg->m_tabPermissions.Count(); i++) {
                rank->m_tabFlags[i] = msg->m_tabPermissions[i].m_flags;
                rank->m_tabWithdrawItemLimit[i] = msg->m_tabPermissions[i].m_withdrawItemLimit;
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    }

    return true;
}

// OFFSET: 0x009351C0/0x14069DC20
void CGGuildInfo::SaveToFile() {
    OsCreateDirectory("Logs", 0);
    HOSFILE file = OsCreateFile(s_rosterFileName, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file != OS_INVALID_FILE && ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
            GuildMemberInfo* member = CGGuildInfo::m_members[i];
            ChrClassesRec* classRec = member->m_classID < g_chrClassesDB.m_minID || member->m_classID > g_chrClassesDB.m_maxID ? NULL : reinterpret_cast<ChrClassesRec*>(g_chrClassesDB.m_recordsById[member->m_classID - g_chrClassesDB.m_minID]);
            AreaTableRec* area = member->m_areaID < g_areaTableDB.m_minID || member->m_areaID > g_areaTableDB.m_maxID ? NULL : reinterpret_cast<AreaTableRec*>(g_areaTableDB.m_recordsById[member->m_areaID - g_areaTableDB.m_minID]);
            const char* rankName = CGGuildInfo::GetRankNameFromRankID(member->m_rankID);

            if (!rankName) {
                rankName = "";
            }

            char line[512];
            SStrPrintf(line, sizeof(line), "%s\t%i\t%s\t%s\t%s\t%s\t%s\t%i\r\n", static_cast<const char*>(CGGuildInfo::m_members[i]->m_name), CGGuildInfo::m_members[i]->m_level, classRec ? static_cast<const char*>(classRec->m_name) : "", area ? static_cast<const char*>(area->m_areaName) : "", rankName, CGGuildInfo::m_members[i]->m_note, CGGuildInfo::m_members[i]->m_officerNote, static_cast<uint32_t>(CGGuildInfo::m_members[i]->m_lastSave));

            uint32_t written;
            OsWriteFile(file, line, strlen(line), &written);
        }

        OsCloseFile(file);
    }

    CGGuildInfo::m_saveToFile = false;
}

// OFFSET: 0x00935370/0x14069DED0
void CGGuildInfo::QueryXP() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return;
    }

    if (!player->GetGuildMaxDailyXP()) {
        GlobalGuildRequestMaxDailyXP request(player->GetGuildGUID().m_guid);
        ClientServices::Send(&request);
    }

    GlobalGuildRequestGuildXP request(player->GetGuildGUID().m_guid);
    ClientServices::Send(&request);
}

// OFFSET: 0x00935430/0x14069DFB0
void CGGuildInfo::QueryNews() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return;
    }

    GlobalGuildQueryNews query(player->GetGuildGUID().m_guid);
    ClientServices::Send(&query);
}

// OFFSET: 0x009354A0/0x14069E020
void CGGuildInfo::QueryRanks() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return;
    }

    GlobalGuildGetRanks query(player->GetGuildGUID().m_guid);
    ClientServices::Send(&query);
}

// PROVEN: 1e3951ce IDENTICAL/NONE
// OFFSET: 0x00935510
void CGGuildInfo::UpdateGuildLevel() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        CGGuildInfo::m_guildLevel = player->m_player->m_playerGuildlevel;
    }
}

// PROVEN: b1fcdeac IDENTICAL/NONE
// OFFSET: 0x00935540/0x14069E0D0
bool CGGuildInfo::CanEditGuildEvent() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return false;
    }

    return (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) >> 20) & 1;
}

// PROVEN: fe637d28 IDENTICAL/NONE
// OFFSET: 0x00935590
bool CGGuildInfo::CanRemoveGuildEvent() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return false;
    }

    return (CGGuildInfo::GetPermissionsByRankID(player->m_player->m_playerGuildrank) >> 23) & 1;
}

// OFFSET: 0x009355E0
bool CGGuildInfo::ClientGuildXPEarnedHandler(ClientGuildXPEarned* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID() && static_cast<int64_t>(msg->m_xp) > 0) {
        char text[3000];
        SStrPrintf(text, sizeof(text), FrameScript_GetText("COMBATLOG_GUILD_XPGAIN", -1, GENDER_NOT_APPLICABLE), static_cast<uint32_t>(msg->m_xp));
        CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_GUILD_XP_GAIN, NULL);
    }

    return true;
}

// OFFSET: 0x00935670
bool CGGuildInfo::ClientGuildMemberUpdateNoteHandler(ClientGuildMemberUpdateNote* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return true;
    }

    uint32_t index = CGGuildInfo::GetMemberIndex(*reinterpret_cast<uint64_t*>(msg->m_memberGUID));

    GuildMemberInfo* member;

    if (index < CGGuildInfo::m_numMembers && (member = CGGuildInfo::m_members[index]) != NULL) {
        if (msg->m_isPublic) {
            SStrCopy(member->m_note, msg->m_note, sizeof(member->m_note));
        } else {
            SStrCopy(member->m_officerNote, msg->m_note, sizeof(member->m_officerNote));
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    return true;
}

// OFFSET: 0x00935730
bool CGGuildInfo::ClientGuildInviteHandler(ClientGuildInvite* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    if (player->m_player->m_playerFlags & 0x8000000) {
        GlobalGuildAutoDeclineInvitation decline;
        ClientServices::Send(&decline);
        return true;
    }

    int32_t guildLevel = msg->m_guildLevel;
    WOWGUID requester = WOWGUID::Make(0);
    GuildStats_C* stats = g_guildInfoCache.GetRecord2(*reinterpret_cast<uint64_t*>(msg->m_oldGuildGUID), requester, NULL, NULL, false);
    const char* oldGuildName = stats ? stats->m_name : msg->m_oldGuildName;

    CImVector background(0);
    CImVector border(0);
    CImVector emblem(0);
    char emblemFile[260];
    const char* inviterName;
    const char* guildName;

    if (CGObject_C::GetGuildLogoInfo(msg->m_background, msg->m_borderColor, msg->m_emblemColor, msg->m_emblemStyle, background, border, emblem, emblemFile, sizeof(emblemFile))) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, g_scriptEvents[SCRIPT_EVENT_GUILD_INVITE_REQUEST]);
        inviterName = msg->m_inviterName;
        lua_pushstring(L, msg->m_inviterName);
        guildName = msg->m_guildName;
        lua_pushstring(L, msg->m_guildName);
        lua_pushnumber(L, guildLevel);
        lua_pushstring(L, oldGuildName);
        lua_pushboolean(L, *reinterpret_cast<uint64_t*>(msg->m_oldGuildGUID) != *reinterpret_cast<uint64_t*>(msg->m_guildGUID));
        lua_pushnumber(L, background.r);
        lua_pushnumber(L, background.g);
        lua_pushnumber(L, background.b);
        lua_pushnumber(L, border.r);
        lua_pushnumber(L, border.g);
        lua_pushnumber(L, border.b);
        lua_pushnumber(L, emblem.r);
        lua_pushnumber(L, emblem.g);
        lua_pushnumber(L, emblem.b);
        lua_pushstring(L, emblemFile);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_INVITE_REQUEST, L, 16);
        lua_settop(L, -17);
    } else {
        guildName = msg->m_guildName;
        inviterName = msg->m_inviterName;
        CGGameUI::OpenGuildInvite(msg->m_inviterName, msg->m_guildName, guildLevel, oldGuildName);
    }

    CGGameUI::DisplayError(ERR_INVITED_TO_GUILD_SSS, inviterName, inviterName, guildName);
    return true;
}

// OFFSET: 0x009359C0
bool CGGuildInfo::ClientGuildReputationWeeklyCapHandler(ClientGuildReputationWeeklyCap* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return true;
    }

    uint64_t guid = player->m_obj->m_guid.m_guid;
    uint32_t index = CGGuildInfo::GetMemberIndex(guid);

    GuildMemberInfo* member;

    if (index < CGGuildInfo::m_numMembers && (member = CGGuildInfo::m_members[index]) != NULL) {
        member->m_guildRepToCap = msg->m_repToCap;
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REP_UPDATED, NULL);
    }

    return true;
}

// OFFSET: 0x00937530/0x1406A0A20
bool CGGuildInfo::CanReplaceGuildMaster() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    uint64_t guid = player->m_obj->m_guid.m_guid;
    int32_t index = CGGuildInfo::GetMemberIndex(guid);

    if (index < 0 || index >= static_cast<int32_t>(CGGuildInfo::m_numMembers) || !CGGuildInfo::m_members[index]) {
        return false;
    }

    int32_t rankOrder = CGGuildInfo::GetRankOrderFromRankID(CGGuildInfo::m_members[index]->m_rankID);

    if (!rankOrder) {
        return false;
    }

    float guildMasterAbsent = 0.0f;
    int32_t lowestActiveRank = 0;

    for (uint32_t i = 0; i < CGGuildInfo::m_members.Count(); i++) {
        GuildMemberInfo* member = CGGuildInfo::m_members[i];
        int32_t order = CGGuildInfo::GetRankOrderFromRankID(member->m_rankID);

        if (order) {
            if (CGGuildInfo::m_guildMasterReplaceDays >= member->m_lastSave && (order < lowestActiveRank || !lowestActiveRank)) {
                lowestActiveRank = order;
            }
        } else {
            guildMasterAbsent = member->m_lastSave;

            if (CGGuildInfo::m_guildMasterAbsentDays >= guildMasterAbsent) {
                return false;
            }
        }
    }

    return guildMasterAbsent > CGGuildInfo::m_guildMasterAbsentDays && (rankOrder == 1 || rankOrder == lowestActiveRank);
}

// OFFSET: 0x00937870
void CGGuildInfo::InsertMemberInfoProfessionsList(uint32_t index, int32_t showOffline) {
    if (index >= CGGuildInfo::m_numMembers) {
        return;
    }

    GuildMemberInfo* member = CGGuildInfo::m_members[index];

    if (!member) {
        return;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (!member->m_professions[i].m_skillLineID) {
            continue;
        }

        for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
            GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

            if (header->m_skillLineID == member->m_professions[i].m_skillLineID) {
                if (member->m_online || showOffline) {
                    GuildProfessionMember* entry = header->m_members.New();
                    entry->m_guid = member->m_guid;
                    entry->m_memberIndex = index;
                }

                header->m_numMembers++;
                break;
            }

            header = next;
        }
    }
}

// OFFSET: 0x00937970
bool CGGuildInfo::GetTradeSkillInfo(int32_t index, uint64_t& guid, int32_t& skillLineID, bool& collapsed, int32_t& numOnline, int32_t& numMembers, int32_t& numVisible) {
    int32_t current = 0;

    for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
        GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

        if (current++ == index) {
            guid = 0;
            skillLineID = header->m_skillLineID;
            collapsed = header->m_collapsed;
            numOnline = 0;
            numMembers = 0;
            numVisible = header->m_numMembers;

            for (uint32_t i = 0; i < header->m_members.Count(); i++) {
                uint32_t memberIndex = header->m_members[i].m_memberIndex;

                if (memberIndex < CGGuildInfo::m_numMembers && CGGuildInfo::m_members[memberIndex]) {
                    numMembers++;

                    if (CGGuildInfo::m_members[memberIndex]->m_online) {
                        numOnline++;
                    }
                }
            }

            return true;
        }

        if (!header->m_collapsed) {
            for (uint32_t i = 0; i < header->m_members.Count(); i++) {
                if (current++ == index) {
                    guid = header->m_members[i].m_guid;
                    skillLineID = header->m_skillLineID;
                    collapsed = false;
                    numOnline = 0;
                    return true;
                }
            }
        }

        header = next;
    }

    return false;
}

// PROVEN: 5a71d151 IDENTICAL/NONE
// OFFSET: 0x00937A90/0x1406A0F90
void CGGuildInfo::SortTradeSkillMembers(GUILD_SORT_TYPE type) {
    if (type < GUILD_SORT_NAME || (type > GUILD_SORT_ZONE && type != GUILD_SORT_SKILL)) {
        type = GUILD_SORT_SKILL;
    }

    CGGuildInfo::SetSortType(type);

    for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
        GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

        if (!header->m_collapsed && header->m_members.Count() > 1) {
            CGGuildInfo::m_sortSkillLineID = header->m_skillLineID;
            qsort(header->m_members.m_data, header->m_members.Count(), sizeof(GuildProfessionMember), QSortTradeSkillMembers);
        }

        header = next;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_TRADESKILL_UPDATE, NULL);
}

// PROVEN: 81564120 IDENTICAL/NONE
// OFFSET: 0x00937B20/0x1406A1030
void CGGuildInfo::QueryRosterAndRanks() {
    int32_t now = FrameTime::GetCurTimeMs();

    if (CGGuildInfo::m_nextAllowedQuery && now - CGGuildInfo::m_nextAllowedQuery < 0) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return;
    }

    GlobalGuildGetRoster roster;
    ClientServices::Send(&roster);
    CGGuildInfo::QueryRanks();
    CGGuildInfo::m_nextAllowedQuery = now + 10000;
}

// PROVEN: 46802d5c IDENTICAL/NONE
// OFFSET: 0x00937BA0
bool CGGuildInfo::ClientGuildReputationReactionChangedHandler(ClientGuildReputationReactionChanged* msg) {
    CGGuildInfo::m_nextAllowedQuery = 0;
    CGGuildInfo::QueryRosterAndRanks();
    return true;
}

// OFFSET: 0x00937BC0
bool CGGuildInfo::ClientGuildResetHandler(ClientGuildReset* msg) {
    CGGuildInfo::m_guildLevel = 1;
    CGAchievementInfo::ClearGuildInfo();
    CGGuildInfo::m_guildNews.Clear();
    CGGuildInfo::m_numGuildNews = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_NEWS_UPDATE, NULL);

    for (uint32_t i = 0; i < 4; i++) {
        CGGuildInfo::m_challengeCurrentCount[i] = 0;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_CHALLENGE_UPDATED, NULL);
    CGLFGuildInfo::ClearRecruitmentSettings(true);
    return true;
}

// PROVEN: 90696506 IDENTICAL/NONE
// OFFSET: 0x00937C30
void CGGuildInfo::NameChangedGuildCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGGuildInfo::m_nextAllowedQuery = 0;
        CGGuildInfo::QueryRosterAndRanks();
        Script_SendUnitSignal(guid, SCRIPT_EVENT_PLAYER_GUILD_UPDATE);
    }
}

// OFFSET: 0x00937C60
bool CGGuildInfo::ClientGuildNameChangedHandler(ClientGuildNameChanged* msg) {
    g_guildInfoCache.Invalidate(*reinterpret_cast<uint64_t*>(msg->m_guildGUID));

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID requester = player->m_obj->m_guid;
        g_guildInfoCache.GetRecord(player->GetGuildGUID().m_guid, requester, &CGGuildInfo::NameChangedGuildCallback, NULL, false);
    }

    CGChat::AddChatMessage(FrameScript_GetText("GUILD_RENAMED", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    return true;
}

// PROVEN: 30ec61ed IDENTICAL/NONE
// OFFSET: 0x00937D00/0x1406A1280
uint32_t CGGuildInfo::GetNumGuildRewards() {
    uint32_t count = 0;
    bool showAll = !g_guildRewardsUsableCVar || !g_guildRewardsUsableCVar->m_intValue;

    for (uint32_t i = 0; i < CGGuildInfo::m_guildRewardItemList.Count(); i++) {
        JamGuildRewardItem* item = &CGGuildInfo::m_guildRewardItemList[i];

        if (!showAll && (!item || (item->m_achievementRequired && !CGAchievementInfo::GetAchievement(item->m_achievementRequired) && item->m_achievementRequired))) {
            continue;
        }

        if (!item) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && (!item->m_raceMask || ((1 << (player->m_unit->m_unitFieldBytes0[0] - 1)) & item->m_raceMask))) {
            count++;
        }
    }

    return count;
}

// PROVEN: aedc1f10 IDENTICAL/NONE
// OFFSET: 0x00937DC0/0x1406A1380
JamGuildRewardItem* CGGuildInfo::GetGuildRewardItem(uint32_t index) {
    if (index >= CGGuildInfo::m_guildRewardItemList.Count()) {
        return NULL;
    }

    uint32_t count = 0;
    bool showAll = !g_guildRewardsUsableCVar || !g_guildRewardsUsableCVar->m_intValue;

    for (uint32_t i = 0; i < CGGuildInfo::m_guildRewardItemList.Count(); i++) {
        JamGuildRewardItem* item = &CGGuildInfo::m_guildRewardItemList[i];

        if (!showAll && (!item || (item->m_achievementRequired && !CGAchievementInfo::GetAchievement(item->m_achievementRequired) && item->m_achievementRequired))) {
            continue;
        }

        if (!item) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            continue;
        }

        if (item->m_raceMask && !((1 << (player->m_unit->m_unitFieldBytes0[0] - 1)) & item->m_raceMask)) {
            continue;
        }

        if (count == index) {
            return item;
        }

        count++;
    }

    return NULL;
}

// OFFSET: 0x00938400
bool CGGuildInfo::GetGuildKnownRecipes(int32_t skillLineID, uint8_t* data, int32_t& size) {
    if (!data || size <= 0) {
        return false;
    }

    uint32_t bytes = size;
    HASHKEY_NONE key;
    GuildRecipesKnown* recipes = CGGuildInfo::m_guildRecipesKnown.Ptr(skillLineID, key);

    if (!recipes) {
        return false;
    }

    memcpy(data, recipes->m_recipes, bytes < 300 ? bytes : 300);
    size = static_cast<uint32_t>(size) >= 300 ? 300 : size;
    return true;
}

// OFFSET: 0x00938470
void CGGuildInfo::SetRecipeKnownByMembers(int32_t skillLineID, int32_t spellID, const TSGrowableArray<uint64_t>& members) {
    CGGuildInfo::m_knownRecipeSkillLineID = 0;
    CGGuildInfo::m_knownRecipeSpellID = 0;
    CGGuildInfo::m_knownRecipeMembers.Clear();
    CGGuildInfo::m_knownRecipeSkillLineID = skillLineID;
    CGGuildInfo::m_knownRecipeSpellID = spellID;
    CGGuildInfo::m_knownRecipeMembers.SetCount(members.m_count);

    for (uint32_t i = 0; i < members.m_count; i++) {
        CGGuildInfo::m_knownRecipeMembers[i] = members.m_data[i];
    }

    qsort(CGGuildInfo::m_knownRecipeMembers.m_data, CGGuildInfo::m_knownRecipeMembers.Count(), sizeof(uint64_t), QSortRecipeMembers);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_RECIPE_KNOWN_BY_MEMBERS, NULL);
}

// OFFSET: 0x00938570
int32_t CGGuildInfo::GetSpellFromRecipeBit(int32_t skillLineID, int32_t bit) {
    HASHKEY_NONE key;
    SKILLLINESPELLS* spells = CGGuildInfo::m_skillLineSpells.Ptr(skillLineID, key);
    SKILLLINESPELLBITS* spellBit;

    if (spells && (spellBit = spells->m_bits.Ptr(bit, key))) {
        return spellBit->m_spellID;
    }

    return 0;
}

// OFFSET: 0x009385B0/0x1406A1F90
int32_t CGGuildInfo::GetRecipeBitFromSpell(int32_t skillLineID, int32_t spellID) {
    HASHKEY_NONE key;
    SKILLLINESPELLS* spells = CGGuildInfo::m_skillLineSpells.Ptr(skillLineID, key);

    if (!spells) {
        return 0;
    }

    for (SKILLLINESPELLBITS* spellBit = spells->m_bits.Head(); spellBit; spellBit = spells->m_bits.Next(spellBit)) {
        if (spellBit->m_spellID == spellID) {
            return spellBit->m_hashval;
        }
    }

    return 0;
}

// PROVEN: 13fa98e3 IDENTICAL/NONE
// OFFSET: 0x00938620
void CGGuildInfo::ClearProfessionMembers() {
    for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
        GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

        header->m_members.Clear();
        header->m_numMembers = 0;

        header = next;
    }
}

// PROVEN: 2b0ef0e1 IDENTICAL/NONE
// OFFSET: 0x00938680
void CGGuildInfo::UpdateProfessions() {
    CGGuildInfo::ClearProfessionMembers();

    int32_t showOffline = CGGuildInfo::m_showOffline;

    for (uint32_t i = 0; i < CGGuildInfo::GetNumMembers(1); i++) {
        CGGuildInfo::InsertMemberInfoProfessionsList(i, showOffline);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_TRADESKILL_UPDATE, NULL);
}

// OFFSET: 0x009386D0
void CGGuildInfo::SetMemberOnlineStatus(uint64_t guid, bool online) {
    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        if (!CGGuildInfo::m_members[i] || CGGuildInfo::m_members[i]->m_guid != guid) {
            continue;
        }

        GuildMemberInfo* member = CGGuildInfo::m_members[i];

        if (online) {
            if (!member->m_online && ++CGGuildInfo::m_numOnline >= CGGuildInfo::m_numMembers) {
                CGGuildInfo::m_numOnline = CGGuildInfo::m_numMembers;
            }
        } else if (member->m_online) {
            uint32_t count = CGGuildInfo::m_numOnline - 1;
            CGGuildInfo::m_numOnline = count > 0 ? count : 0;
        }

        CGGuildInfo::m_members[i]->m_online = online;
        CGGuildInfo::ComputeContributionRanks();
        qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), CGGuildInfo::QSortRoster);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
        CGGuildInfo::UpdateProfessions();
        return;
    }
}

// OFFSET: 0x00938F50/0x1406A2C70
void CGGuildInfo::EnterWorld() {
    CGGuildInfo::m_infoText[0] = 0;
    CGGuildInfo::m_motd[0] = 0;
    CGGuildInfo::m_guildMemberSkillsGuid = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillLineID = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillRank = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillStep = 0;
    memset(CGGuildInfo::m_guildMemberProfessionData, 0, sizeof(CGGuildInfo::m_guildMemberProfessionData));
    CGGuildInfo::m_guildRecipesKnown.InternalClear(0);
    CGGuildInfo::m_knownRecipeSkillLineID = 0;
    CGGuildInfo::m_knownRecipeSpellID = 0;
    CGGuildInfo::m_knownRecipeMembers.Clear();

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID()) {
        GlobalGuildPermissionsQuery query;
        ClientServices::Send(&query);

        CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (activePlayer) {
            CGGuildInfo::m_guildLevel = activePlayer->m_player->m_playerGuildlevel;
        }
    }

    CGGuildInfo::m_guildNews.Clear();

    CVar* filter = CVar::Lookup("guildNewsFilter");

    if (filter) {
        CGGuildInfo::m_guildNewsFilter = ~filter->m_intValue;
    } else {
        CGGuildInfo::m_guildNewsFilter = -1;
    }

    CGGuildInfo::m_numGuildNews = 0;
    CGGuildInfo::m_guildRewardStatCBCount = 0;
    CGGuildInfo::m_guildRewardItemListVersion = 0;
    CGGuildInfo::m_newsSortByDate = false;
    CGGuildInfo::m_inGuildParty = false;
}

// PROVEN: 795e8803 IDENTICAL/NONE
// OFFSET: 0x009390C0
void CGGuildInfo::SetProfessionHeaderCollapsed(int32_t skillLineID, bool collapsed) {
    bool found = false;

    for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); ) {
        GuildProfessionHeader* next = CGGuildInfo::m_professionList.RawNext(header);

        if (skillLineID == -1 || header->m_skillLineID == skillLineID) {
            found = true;
            header->m_collapsed = collapsed;
        }

        header = next;
    }

    if (found) {
        CGGuildInfo::UpdateProfessions();
    }
}

// OFFSET: 0x00939110
void CGGuildInfo::SetProfessionCategoryFilter(int32_t skillLineID) {
    if (skillLineID == -1) {
        CGGuildInfo::m_professionCategoryFilter = -1;
        CGGuildInfo::UpdateProfessions();
        return;
    }

    GuildProfessionHeader* next;

    for (GuildProfessionHeader* header = CGGuildInfo::m_professionList.Head(); !CGGuildInfo::m_professionList.IsEnd(header); header = next) {
        next = CGGuildInfo::m_professionList.RawNext(header);

        if (header->m_skillLineID == skillLineID) {
            CGGuildInfo::m_professionCategoryFilter = skillLineID;
            CGGuildInfo::UpdateProfessions();
            return;
        }
    }
}

// PROVEN: 6836855e IDENTICAL/NONE
// OFFSET: 0x00939160
void CGGuildInfo::SetProfessionNameFilter(const char* filter) {
    if (filter) {
        SStrCopy(CGGuildInfo::m_professionNameFilter, filter, sizeof(CGGuildInfo::m_professionNameFilter));
        CGGuildInfo::UpdateProfessions();
    }
}

// OFFSET: 0x009391A0
void CGGuildInfo::DestroySkillLineUniqueBitTables() {
    for (SKILLLINESPELLS* spells = CGGuildInfo::m_skillLineSpells.Head(); spells; spells = CGGuildInfo::m_skillLineSpells.Next(spells)) {
        spells->m_bits.Clear();
    }

    CGGuildInfo::m_skillLineSpells.Clear();
}

// OFFSET: 0x00939280/0x1406A3200
void CGGuildInfo::InitializeProfessionsHeaderList() {
    for (int32_t i = 0; i < g_skillLineDB.m_numRecords; i++) {
        SkillLineRec* skillLine = g_skillLineDB.GetRecordByIndex(i);

        if (!skillLine || skillLine->m_categoryID != 11) {
            continue;
        }

        GuildProfessionHeader* header = static_cast<GuildProfessionHeader*>(SMemAlloc(sizeof(GuildProfessionHeader), __FILE__, __LINE__, 0x8));

        if (header) {
            new (header) GuildProfessionHeader();
        }

        header->m_skillLineID = skillLine->m_id;
        header->m_collapsed = true;
        header->m_numMembers = 0;

        for (GuildProfessionHeader* existing = CGGuildInfo::m_professionList.Head(); existing; existing = CGGuildInfo::m_professionList.Next(existing)) {
            SkillLineRec* existingSkill = g_skillLineDB.GetRecord(existing->m_skillLineID);

            if (SStrCmpUTF8I(skillLine->m_displayName, existingSkill->m_displayName, STORM_MAX_STR) < 0) {
                CGGuildInfo::m_professionList.LinkNode(header, 1, existing);
                break;
            }
        }

        if (!header->m_link.IsLinked()) {
            CGGuildInfo::m_professionList.LinkToTail(header);
        }
    }
}

// PROVEN: 878c5035 IDENTICAL/NONE
// OFFSET: 0x00939430
void CGGuildInfo::SetShowOffline(int32_t show) {
    if (show == CGGuildInfo::m_showOffline) {
        return;
    }

    CGGuildInfo::m_showOffline = show;

    if (CGGuildInfo::m_numMembers) {
        qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), CGGuildInfo::QSortRoster);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
        CGGuildInfo::UpdateProfessions();
    }
}

// OFFSET: 0x00939480/0x1406A35D0
void CGGuildInfo::ClearGuildInfo() {
    CGGuildInfo::m_infoText[0] = 0;
    CGGuildInfo::m_motd[0] = 0;
    CGGuildInfo::m_guildMemberSkillsGuid = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillLineID = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillRank = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillStep = 0;
    memset(CGGuildInfo::m_guildMemberProfessionData, 0, sizeof(CGGuildInfo::m_guildMemberProfessionData));
    CGGuildInfo::m_guildRecipesKnown.InternalClear(0);
    CGGuildInfo::m_knownRecipeSkillLineID = 0;
    CGGuildInfo::m_knownRecipeSpellID = 0;
    CGGuildInfo::m_knownRecipeMembers.Clear();
    CGGuildInfo::m_guildNews.Clear();
    CGGuildInfo::m_numGuildNews = 0;
    CGGuildInfo::m_guildRewardStatCBCount = 0;
    CGGuildInfo::m_guildRewardItemListVersion = 0;
    CGGuildInfo::m_newsSortByDate = false;
    CGGuildInfo::m_inGuildParty = false;

    for (uint32_t i = 0; i < CGGuildInfo::m_members.Count(); i++) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(CGGuildInfo::m_members[i]->m_guid), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (unit) {
            unit->UpdateUnitNameText();
        }

        delete CGGuildInfo::m_members[i];
        CGGuildInfo::m_members[i] = NULL;
    }

    CGGuildInfo::m_members.m_count = 0;
    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::m_guildRewardItemList.Clear();
    CGGuildInfo::m_guildRewardItemListVersion = 0;
}

// OFFSET: 0x00939B30
bool CGGuildInfo::ClientGuildRosterHandler(ClientGuildRoster* msg) {
    if (!msg || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return true;
    }

    CGGuildInfo::m_numOnline = 0;
    CGGuildInfo::m_numMembers = msg->m_memberData.Count();
    CGGuildInfo::m_guildFlags = msg->m_guildFlags;
    CGGuildInfo::m_numAccounts = msg->m_numAccounts;
    CGGuildInfo::m_createDate = msg->m_createDate;

    uint32_t oldCount = CGGuildInfo::m_members.Count();
    CGGuildInfo::m_guildReputationWeeklyCap = msg->m_weeklyRepCap;

    if (CGGuildInfo::m_numMembers > oldCount) {
        CGGuildInfo::m_members.SetCount(CGGuildInfo::m_numMembers);

        for (uint32_t i = oldCount; i < CGGuildInfo::m_numMembers; i++) {
            CGGuildInfo::m_members[i] = static_cast<GuildMemberInfo*>(SMemAlloc(sizeof(GuildMemberInfo), __FILE__, __LINE__, 0));
            CGGuildInfo::m_members[i]->m_name[0] = 0;
        }
    }

    CGGuildInfo::SetMOTD(msg->m_motd);
    ScrubString(msg->m_infoText, CGGuildInfo::m_infoText, 501, sizeof(CGGuildInfo::m_infoText), 0);
    CGAutoCompleteName::s_batchUpdate = true;

    for (uint32_t i = 0; i < CGGuildInfo::m_numMembers; i++) {
        GuildMemberInfo* member = CGGuildInfo::m_members[i];
        CGGuildInfo::UpdateMemberInfo(member, &msg->m_memberData[i]);

        if (member->m_online) {
            CGGuildInfo::m_numOnline++;
        }
    }

    CGAutoCompleteName::s_batchUpdate = false;

    if (CGAutoCompleteName::s_resortPending) {
        CGAutoCompleteName::ResortAll();
    }

    CGGuildInfo::ComputeContributionRanks();
    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), CGGuildInfo::QSortRoster);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    CGGuildInfo::UpdateProfessions();

    if (CGGuildInfo::m_saveToFile) {
        CGGuildInfo::SaveToFile();
    }

    return true;
}

// OFFSET: 0x00939CD0
bool CGGuildInfo::ClientGuildRosterUpdateHandler(ClientGuildRosterUpdate* msg) {
    if (!msg || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return true;
    }

    bool batch = msg->m_memberData.Count() > 3;

    if (batch) {
        CGAutoCompleteName::s_batchUpdate = true;
    }

    for (uint32_t i = 0; i < msg->m_memberData.Count(); i++) {
        for (uint32_t j = 0; j < CGGuildInfo::m_numMembers; j++) {
            if (CGGuildInfo::m_members[j]->m_guid == *reinterpret_cast<uint64_t*>(msg->m_memberData[i].m_guid)) {
                CGGuildInfo::UpdateMemberInfo(CGGuildInfo::m_members[j], &msg->m_memberData[i]);
            }
        }
    }

    if (batch) {
        CGAutoCompleteName::s_batchUpdate = false;

        if (CGAutoCompleteName::s_resortPending) {
            CGAutoCompleteName::ResortAll();
        }
    }

    CGGuildInfo::ComputeContributionRanks();
    qsort(CGGuildInfo::m_members.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), CGGuildInfo::QSortRoster);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, NULL);
    CGGuildInfo::UpdateProfessions();

    if (CGGuildInfo::m_saveToFile) {
        CGGuildInfo::SaveToFile();
    }

    return true;
}

// OFFSET: 0x00939DE0/0x1406A40C0
void CGGuildInfo::SortGuildNewsHeaders() {
    TSGrowableArray<NewsHeaderDate> headers;

    for (uint32_t i = 0; i < CGGuildInfo::m_guildNews.Count(); i++) {
        uint32_t j = 0;

        for (; j < headers.Count(); j++) {
            if (headers[j].m_date.IsSameDay(CGGuildInfo::m_guildNews[i].m_date)) {
                break;
            }
        }

        if (j < headers.Count()) {
            if (!headers[j].m_hasSticky && CGGuildInfo::m_guildNews[i].m_isHeader) {
                headers[j].m_hasSticky = true;
            }

            continue;
        }

        bool sticky = CGGuildInfo::m_guildNews[i].m_isHeader;
        NewsHeaderDate header;
        header.m_date = CGGuildInfo::m_guildNews[i].m_date;
        header.m_hasSticky = sticky;
        header.m_date.SetHourAndMinutes(0);
        headers.Add(1, &header);
    }

    for (uint32_t i = 0; i < headers.Count(); i++) {
        if (headers[i].m_hasSticky) {
            continue;
        }

        JamGuildNewsEvent event;
        event.m_id = 0;
        event.m_date = headers[i].m_date;
        event.m_type = 0;
        event.m_flags = 0;
        event.m_memberGUID = 0;
        event.m_data[0] = 0;
        event.m_data[1] = 0;

        GuildNewsEvent news;
        static_cast<JamGuildNewsEvent&>(news) = event;
        news.m_isHeader = true;
        CGGuildInfo::m_guildNews.New(news);
    }

    qsort(CGGuildInfo::m_guildNews.m_data, CGGuildInfo::m_guildNews.Count(), sizeof(GuildNewsEvent), QSortGuildNews);
    CGGuildInfo::m_numGuildNews = 0;

    for (uint32_t i = 0; i < CGGuildInfo::m_guildNews.Count(); i++) {
        if (CGGuildInfo::m_guildNews[i].m_isHeader || ((1 << CGGuildInfo::m_guildNews[i].m_type) & CGGuildInfo::m_guildNewsFilter)) {
            CGGuildInfo::m_numGuildNews++;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_NEWS_UPDATE, NULL);
}

// OFFSET: 0x0093A340
void CGGuildInfo::ShutdownGame() {
    uint32_t count = CGGuildInfo::m_members.Count();

    ClientGuildRoster::s_handler = NULL;
    ClientGuildRosterUpdate::s_handler = NULL;
    ClientGuildNews::s_handler = NULL;
    ClientGuildNewsDeleted::s_handler = NULL;
    ClientGuildRanks::s_handler = NULL;
    ClientGuildMemberUpdateNote::s_handler = NULL;
    ClientGuildInvite::s_handler = NULL;
    ClientGuildPartyState::s_handler = NULL;
    ClientGuildReputationWeeklyCap::s_handler = NULL;
    ClientGuildReputationReactionChanged::s_handler = NULL;
    ClientGuildChallengeUpdate::s_handler = NULL;
    ClientGuildChallengeCompleted::s_handler = NULL;
    ClientGuildReset::s_handler = NULL;
    ClientGuildMoveComplete::s_handler = NULL;
    ClientGuildNameChanged::s_handler = NULL;
    ClientGuildMoveStarting::s_handler = NULL;
    ClientGuildFlaggedForRename::s_handler = NULL;
    ClientGuildPermissionsQueryResults::s_handler = NULL;
    ClientGuildEventLogQueryResults::s_handler = NULL;
    ClientGuildMemberDailyReset::s_handler = NULL;

    for (uint32_t i = 0; i < count; i++) {
        delete CGGuildInfo::m_members[i];
        CGGuildInfo::m_members[i] = NULL;
    }

    CGGuildInfo::m_members.m_count = 0;
    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::DestroySkillLineUniqueBitTables();
    CGGuildInfo::ClearProfessionMembers();
    CGGuildInfo::m_professionList.Clear();
    ClientGuildRewardList::s_handler = NULL;
    CGGuildInfo::m_guildRewardItemList.Clear();
    CGGuildInfo::m_guildRewardItemListVersion = 0;
    ClientGuildNews::s_handler = NULL;
    ClientGuildXPEarned::s_handler = NULL;
    CGGuildInfo::m_guildRecipesKnown.Clear();
    CGGuildInfo::m_knownRecipeSkillLineID = 0;
    CGGuildInfo::m_knownRecipeSpellID = 0;
    CGGuildInfo::m_knownRecipeMembers.Clear();
    CGGuildInfo::m_guildNews.Clear();
}

// OFFSET: 0x0093A590
bool CGGuildInfo::ClientGuildRewardListHandler(ClientGuildRewardList* msg) {
    if (msg->m_version != CGGuildInfo::m_guildRewardItemListVersion) {
        CGGuildInfo::m_guildRewardItemListVersion = msg->m_version;
        CGGuildInfo::m_guildRewardItemList.Clear();
        CGGuildInfo::m_guildRewardItemList.SetCount(msg->m_rewardItems.Count());
        CGGuildInfo::m_guildRewardStatCBCount = 0;

        for (uint32_t i = 0; i < msg->m_rewardItems.Count(); i++) {
            WOWGUID requester = WOWGUID::Make(0);
            CGGuildInfo::m_guildRewardItemList[i] = msg->m_rewardItems[i];

            if (!g_itemSparseDB.GetRecord(CGGuildInfo::m_guildRewardItemList[i].m_itemID, requester, &CGGuildInfo::GuildRewardsItemStatsCallback, NULL, true)) {
                CGGuildInfo::m_guildRewardStatCBCount++;
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_REWARDS_LIST, NULL);
    return true;
}

// PROVEN: a7dac5e5 IDENTICAL/NONE
// OFFSET: 0x0093A660/0x1406A4F00
void CGGuildInfo::SetGuildNewsFilter(int32_t filter) {
    CGGuildInfo::m_guildNewsFilter = filter;

    if (CVar* cvar = CVar::Lookup("guildNewsFilter")) {
        cvar->Set(~CGGuildInfo::m_guildNewsFilter, true, false, false, true);
    }

    if (CGGuildInfo::m_newsSortByDate) {
        CGGuildInfo::SortGuildNewsByDate();
    } else {
        CGGuildInfo::SortGuildNewsHeaders();
    }
}

// OFFSET: 0x0093B910
bool CGGuildInfo::ClientGuildNewsHandler(ClientGuildNews* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    for (uint32_t i = 0; i < msg->m_newsEvents.Count(); i++) {
        JamGuildNewsEvent& event = msg->m_newsEvents[i];
        uint32_t j = 0;

        for (; j < CGGuildInfo::m_guildNews.Count(); j++) {
            if (event.m_id == CGGuildInfo::m_guildNews[j].m_id) {
                break;
            }
        }

        if (j < CGGuildInfo::m_guildNews.Count()) {
            CGGuildInfo::m_guildNews[j].m_flags = event.m_flags;
            continue;
        }

        GuildNewsEvent news;
        static_cast<JamGuildNewsEvent&>(news) = event;
        news.m_isHeader = false;
        CGGuildInfo::m_guildNews.New(news);

        WOWGUID requester = player->m_obj->m_guid;
        g_nameDBCache.GetRecord(news.m_memberGUID, requester, &CGGuildInfo::GuildNewsNameCacheCallback, NULL, true);

        if (static_cast<uint32_t>(msg->m_newsEvents[i].m_type - 3) <= 2) {
            WOWGUID itemRequester = player->m_obj->m_guid;
            g_itemSparseDB.GetRecord(msg->m_newsEvents[i].m_data[0], itemRequester, &CGGuildInfo::GuildNewsItemStatsCallback, NULL, true);
        }
    }

    if (CGGuildInfo::m_newsSortByDate) {
        CGGuildInfo::SortGuildNewsByDate();
        return true;
    }

    CGGuildInfo::SortGuildNewsHeaders();
    return true;
}

// OFFSET: 0x0093BA60
bool CGGuildInfo::ClientGuildNewsDeletedHandler(ClientGuildNewsDeleted* msg) {
    for (uint32_t i = 0; i < CGGuildInfo::m_guildNews.Count(); i++) {
        if (CGGuildInfo::m_guildNews[i].m_id != msg->m_newsID) {
            continue;
        }

        if (i != CGGuildInfo::m_guildNews.Count() - 1) {
            GuildNewsEvent& last = CGGuildInfo::m_guildNews[CGGuildInfo::m_guildNews.Count() - 1];
            static_cast<JamGuildNewsEvent&>(CGGuildInfo::m_guildNews[i]) = last;
            CGGuildInfo::m_guildNews[i].m_isHeader = last.m_isHeader;
        }

        CGGuildInfo::m_guildNews[CGGuildInfo::m_guildNews.Count() - 1].~GuildNewsEvent();
        CGGuildInfo::m_guildNews.m_count--;
        break;
    }

    if (CGGuildInfo::m_newsSortByDate) {
        CGGuildInfo::SortGuildNewsByDate();
    } else {
        CGGuildInfo::SortGuildNewsHeaders();
    }

    return true;
}

// OFFSET: 0x0093BDA0
void CGGuildInfo::SetGuildKnownRecipes(int32_t skillLineID, uint8_t* data, int32_t size) {
    if (!data || size <= 0) {
        return;
    }

    HASHKEY_NONE key;
    GuildRecipesKnown* recipes = CGGuildInfo::m_guildRecipesKnown.Ptr(skillLineID, key);

    if (!recipes) {
        recipes = CGGuildInfo::m_guildRecipesKnown.New(skillLineID, key, 0, 0);
    }

    memset(recipes->m_recipes, 0, sizeof(recipes->m_recipes));
    memcpy(recipes->m_recipes, data, static_cast<uint32_t>(size) >= 300 ? 300 : size);
    recipes->m_canView = false;

    for (uint32_t i = 0; i < (static_cast<uint32_t>(size) >= 300 ? 300u : static_cast<uint32_t>(size)); i++) {
        if (!recipes->m_recipes[i]) {
            continue;
        }

        for (int32_t bit = 0; bit < 8; bit++) {
            SpellRec* spell = g_spellDB.GetRecord(CGGuildInfo::GetSpellFromRecipeBit(skillLineID, bit + 8 * i));

            if (spell && (spell->m_attributes & 0x20)) {
                recipes->m_canView = true;
                break;
            }
        }
    }
}

// OFFSET: 0x0093BF70/0x1406A7420
void CGGuildInfo::InitializeSkillLineUniqueBitTables() {
    for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
        SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

        if (!ability || !ability->m_uniqueBit || !ability->m_skillLine || !ability->m_spell) {
            continue;
        }

        HASHKEY_NONE key;
        SKILLLINESPELLS* spells = CGGuildInfo::m_skillLineSpells.Ptr(ability->m_skillLine, key);

        if (!spells) {
            spells = CGGuildInfo::m_skillLineSpells.New(ability->m_skillLine, key, 0, 0);
        }

        int32_t bit = ability->m_uniqueBit;

        if (!spells->m_bits.Ptr(bit, key)) {
            SKILLLINESPELLBITS* spellBit = spells->m_bits.New(bit, key, 0, 0);
            spellBit->m_spellID = ability->m_spell;
        }
    }
}

// OFFSET: 0x0093C0A0/0x1406A7680
void CGGuildInfo::InitializeGame() {
    CGGuildInfo::m_selectedRankOrder = -1;
    CGGuildInfo::m_professionCategoryFilter = -1;
    CGGuildInfo::m_numAccounts = 0;
    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::m_numOnline = 0;
    CGGuildInfo::m_numUsedRanks = 0;
    CGGuildInfo::m_guildFlags = 0;
    CGGuildInfo::m_guildReputationWeeklyCap = 0;
    CGGuildInfo::m_showOffline = 0;
    CGGuildInfo::m_selectedMember = 0;
    CGGuildInfo::m_nextAllowedQuery = 0;
    CGGuildInfo::m_numEventLogEntries = 0;
    CGGuildInfo::m_pendingEventLogNames = 0;
    CGGuildInfo::m_infoText[0] = 0;
    CGGuildInfo::m_motd[0] = 0;
    CGGuildInfo::m_guildMemberSkillsGuid = 0;
    memset(CGGuildInfo::m_professionNameFilter, 0, sizeof(CGGuildInfo::m_professionNameFilter));
    CGGuildInfo::m_guildMemberSkillLine.m_skillLineID = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillRank = 0;
    CGGuildInfo::m_guildMemberSkillLine.m_skillStep = 0;
    memset(CGGuildInfo::m_guildMemberProfessionData, 0, sizeof(CGGuildInfo::m_guildMemberProfessionData));
    CGGuildInfo::m_knownRecipeSkillLineID = 0;
    CGGuildInfo::m_knownRecipeSpellID = 0;
    CGGuildInfo::m_knownRecipeMembers.Clear();
    CGGuildInfo::m_guildRecipesKnown.InternalClear(0);
    CGGuildInfo::m_guildMasterAbsentDays = 90.0f;

    for (uint32_t i = 0; i < NUM_GUILD_SORT_TYPES; i++) {
        CGGuildInfo::m_sortCriteria[i].m_type = static_cast<GUILD_SORT_TYPE>(i);
        CGGuildInfo::m_sortCriteria[i].m_reverse = 0;
    }

    CGGuildInfo::m_largestWeeklyXPContribution = 0;
    CGGuildInfo::m_largestTotalXPContribution = 0;
    CGGuildInfo::m_largestPersonalAchievementPoints = 0;

    for (uint32_t i = 0; i < 4; i++) {
        CGGuildInfo::m_challengeCurrentCount[i] = 0;
        CGGuildInfo::m_challengeMaxCount[i] = 0;
        CGGuildInfo::m_challengeGold[i] = 0;
        CGGuildInfo::m_challengeMaxLevelGold[i] = 0;
        CGGuildInfo::m_challengeXP[i] = 0;
    }

    CGGuildInfo::InitializeSkillLineUniqueBitTables();
    CGGuildInfo::InitializeProfessionsHeaderList();
    ClientGuildRoster::s_handler = &CGGuildInfo::ClientGuildRosterHandler;
    ClientGuildRosterUpdate::s_handler = &CGGuildInfo::ClientGuildRosterUpdateHandler;
    ClientGuildRewardList::s_handler = &CGGuildInfo::ClientGuildRewardListHandler;
    ClientGuildNews::s_handler = &CGGuildInfo::ClientGuildNewsHandler;
    ClientGuildNewsDeleted::s_handler = &CGGuildInfo::ClientGuildNewsDeletedHandler;
    ClientGuildRanks::s_handler = &CGGuildInfo::ClientGuildRanksHandler;
    ClientGuildXPEarned::s_handler = &CGGuildInfo::ClientGuildXPEarnedHandler;
    ClientGuildMemberUpdateNote::s_handler = &CGGuildInfo::ClientGuildMemberUpdateNoteHandler;
    ClientGuildInvite::s_handler = &CGGuildInfo::ClientGuildInviteHandler;
    ClientGuildPartyState::s_handler = &CGGuildInfo::ClientGuildPartyStateHandler;
    ClientGuildReputationWeeklyCap::s_handler = &CGGuildInfo::ClientGuildReputationWeeklyCapHandler;
    ClientGuildReputationReactionChanged::s_handler = &CGGuildInfo::ClientGuildReputationReactionChangedHandler;
    ClientGuildChallengeUpdate::s_handler = &CGGuildInfo::ClientGuildChallengeUpdateHandler;
    ClientGuildChallengeCompleted::s_handler = &CGGuildInfo::ClientGuildChallengeCompletedHandler;
    ClientGuildReset::s_handler = &CGGuildInfo::ClientGuildResetHandler;
    ClientGuildMoveComplete::s_handler = &CGGuildInfo::ClientGuildMoveCompleteHandler;
    ClientGuildNameChanged::s_handler = &CGGuildInfo::ClientGuildNameChangedHandler;
    ClientGuildMoveStarting::s_handler = &CGGuildInfo::ClientGuildMoveStartingHandler;
    ClientGuildChangeNameResult::s_handler = &CGGuildInfo::ClientGuildChangeNameResultHandler;
    ClientGuildFlaggedForRename::s_handler = &CGGuildInfo::ClientGuildFlaggedForRenameHandler;
    ClientGuildPermissionsQueryResults::s_handler = &CGGuildInfo::ClientGuildPermissionsQueryResultsHandler;
    ClientGuildEventLogQueryResults::s_handler = &CGGuildInfo::ClientGuildEventLogQueryResultsHandler;
    ClientGuildMemberDailyReset::s_handler = &CGGuildInfo::ClientGuildMemberDailyResetHandler;
}
