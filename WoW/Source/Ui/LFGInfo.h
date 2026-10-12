#ifndef WOW_SOURCE_UI_LFGINFO_H
#define WOW_SOURCE_UI_LFGINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Ui/LFGLIST.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"

class CDataStore;
class LfgDungeonExpansionRec;

struct lua_State;

struct LFG_SLOT_INFO_LOOT {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_displayID;
    /* 0008 */ uint32_t m_quantity;
    /* 000C */ int32_t m_isCurrency;
};

struct LFGShortageReward {
    /* 0000 */ uint32_t m_roleMask;
    /* 0004 */ int32_t m_money;
    /* 0008 */ int32_t m_xp;
    /* 000C */ TSGrowableArray<LFG_SLOT_INFO_LOOT> m_rewards;

    LFGShortageReward();
};

struct LFGDungeonEntry : public TSLinkedNode<LFGDungeonEntry> {
    /* 0008 */ uint32_t m_slot;
    /* 000C */ uint8_t m_rewardDone;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ int32_t m_currencyID;
    /* 001C */ int32_t unk_001C[8];
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t m_completedEncounters;
    /* 0044 */ uint8_t m_locked;
    /* 0048 */ int32_t m_lockReason;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t m_baseMoney;
    /* 0058 */ int32_t m_baseXP;
    /* 005C */ TSGrowableArray<LFG_SLOT_INFO_LOOT> m_rewards;
    /* 006C */ uint8_t unk_006C;
    /* 0070 */ LFGShortageReward m_shortages[3];

    LFGDungeonEntry();
    ~LFGDungeonEntry();
};

typedef LFG_SLOT_INFO_LOOT LFGReward;
typedef LFGDungeonEntry LFGDungeonRewardInfo;

struct LFGInfo {
    /* 0000 */ JamClientRideTicket m_ticket;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t m_joined;
    /* 0019 */ uint8_t unk_0001;
    /* 001A */ uint8_t m_queued;
    /* 001B */ uint8_t unk_0003;
    /* 001C */ uint8_t unk_0004;
    /* 001D */ uint8_t unk_0005;
    /* 001E */ uint8_t m_roles[3];
    /* 0021 */ uint8_t unk_0009[3];
    /* 0024 */ TSList<LFGDungeonEntry, TSGetLink<LFGDungeonEntry>> m_dungeons;
    /* 0030 */ uint32_t m_numDungeons;
    /* 0034 */ char m_comment[256];
    /* 0134 */ uint32_t unk_011C;

    LFGInfo();
    ~LFGInfo();
    void Reset();
    void AddDungeon(uint32_t slot);
    LFGDungeonEntry* FindDungeon(uint32_t slot) const;
    bool Equals(const LFGInfo& other) const;
    LFGInfo& CopyFrom(const LFGInfo& other);
};

struct LFGLockInfo : public TSLinkedNode<LFGLockInfo> {
    /* 0008 */ uint8_t m_valid;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ TSList<LFGDungeonEntry, TSGetLink<LFGDungeonEntry>> m_dungeons;

    LFGLockInfo();
    ~LFGLockInfo();
    void ClearDungeons();
    LFGDungeonEntry* AddDungeon(uint32_t slot);
};

struct LFGPartyLockInfo {
    /* 0000 */ uint8_t m_valid;
    /* 0001 */ uint8_t unk_0001[3];
    /* 0004 */ TSList<LFGLockInfo, TSGetLink<LFGLockInfo>> m_members;

    void Clear();
    LFGLockInfo* FindMember(WOWGUID guid);
    LFGLockInfo* AddMember(WOWGUID guid);
};

struct LFGRewardInfo {
    /* 0000 */ uint32_t m_randomSlot;
    /* 0004 */ uint32_t m_actualSlot;
    /* 0008 */ int32_t m_baseMoney;
    /* 000C */ int32_t m_baseXP;
};

struct LFGProposalMember {
    /* 0000 */ uint32_t m_roles;
    /* 0004 */ uint32_t m_isLocal;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t m_accepted;
    /* 0014 */ uint32_t unk_0014;
};

struct LFGProposal {
    /* 0000 */ JamClientRideTicket m_ticket;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t m_id;
    /* 0024 */ uint8_t m_state;
    /* 0028 */ uint32_t unk_0008;
    /* 002C */ uint32_t m_completedMask;
    /* 0030 */ uint32_t unk_0010;
    /* 0034 */ uint32_t m_numMembers;
    /* 0038 */ LFGProposalMember m_members[25];

    void Clear();
};

struct LFGQueueStatus {
    /* 0000 */ int32_t m_lfgID;
    /* 0004 */ int32_t m_averageWait;
    /* 0008 */ int32_t m_tankWait;
    /* 000C */ int32_t m_healerWait;
    /* 0010 */ int32_t m_damageWait;
    /* 0014 */ int32_t m_myWait;
    /* 0018 */ uint8_t m_tanksNeeded;
    /* 0019 */ uint8_t m_healersNeeded;
    /* 001A */ uint8_t m_damageNeeded;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint32_t m_statusTime;
    /* 0020 */ int32_t m_queuedTime;

    void Clear();
};

struct LFGBootProposal {
    /* 0000 */ uint32_t m_inProgress;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_didVote;
    /* 000C */ uint32_t m_myVote;
    /* 0010 */ WOWGUID m_target;
    /* 0018 */ uint32_t m_totalVotes;
    /* 001C */ uint32_t m_bootVotes;
    /* 0020 */ uint32_t m_timeLeft;
    /* 0024 */ uint32_t m_votesNeeded;
    /* 0028 */ char m_reason[256];
};

struct LFGJoinResultError {
    /* 0000 */ uint32_t m_result;
    /* 0004 */ UIErrors m_error;
};

struct LFGRandomSlotDungeons {
    /* 0000 */ uint32_t m_count;
    /* 0004 */ int32_t m_dungeons[32];
};

struct LFGRandomSlot {
    /* 0000 */ int32_t m_randomID;
    /* 0004 */ LFGRandomSlotDungeons m_list;
};

struct LFG_ROLE_CHECK_MEMBER_INFO {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ int32_t m_confirmed;
    /* 000C */ uint32_t m_roles;
    /* 0010 */ uint8_t m_level;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t unk_0014;
};

typedef LFG_ROLE_CHECK_MEMBER_INFO LFGRoleCheckMember;

class LfgDungeonsRec;
class CGLookingForGroup {
    public:
    struct LFGEntryInfo {
        /* 0000 */ int32_t m_id;
        /* 0004 */ int32_t m_order;
    };

    static int32_t m_searchJoinedID;
    static uint32_t m_lastProposalID;
    static uint32_t m_randomDungeonBestChoice;
    static TSGrowableArray<uint32_t> m_randomDungeons;
    static TSGrowableArray<CGLookingForGroup::LFGEntryInfo> m_lfdEntries;
    static TSGrowableArray<CGLookingForGroup::LFGEntryInfo> m_lfrEntries;
    static TSGrowableArray<CGLookingForGroup::LFGEntryInfo> m_rfEntries;
    static LFGLIST m_searchResults;
    static LFGLIST m_prevSearchResults;
    static LFGProposal m_proposal;
    static CGLookingForGroup::LFGEntryInfo m_unkBEA820[7];
    static LFGBootProposal m_bootProposal;
    static uint8_t m_unkBEAA10;
    static uint8_t m_unkACD660;
    static LFGInfo m_localInfo;
    static LFGInfo m_playerInfo;
    static LFGInfo m_partyInfo;
    static LFGLockInfo m_playerLockInfo;
    static LFGPartyLockInfo m_partyLockInfo;
    static LFGRewardInfo m_reward;
    static LFGQueueStatus m_queueStatus;
    static int32_t m_roleCheckState;
    static TSGrowableArray<uint32_t> m_roleCheckSlots;
    static TSGrowableArray<LFG_ROLE_CHECK_MEMBER_INFO> m_roleCheckMembers;
    static uint32_t m_randomSlotCount;
    static LFGRandomSlot m_randomSlots[16];
    static int32_t m_randomSlotMinLevel;
    static int32_t m_randomSlotMaxLevel;
    static void EnterWorld();
    static void InitializeGame();
    static void ShutdownGame();
    static int32_t GetLFGRoleUpdate(lua_State* L);
    static int32_t GetLFGRoleUpdateSlot(lua_State* L);
    static void RequestLFGStatus();
    static void JoiningParty();
    static void ClearDungeonList();
    static void SetLFGDungeon(uint32_t slot);
    static void JoinLFG();
    static TSGrowableArray<LFG_SLOT_INFO_LOOT> m_completionRewards;
    static LFGDungeonEntry* GetDungeonRewardInfo(uint32_t dungeonKey);
    static LFG_SLOT_INFO_LOOT* GetDungeonReward(uint32_t dungeonKey, uint32_t index);
    static void RequestLFGPlayerInfo();
    static void ClearPartyInfo();
    static uint32_t GetClassRoles(uint32_t classID);
    static bool HasRaidDungeon(LFGInfo* info);
    static bool HasRFDungeon(LFGInfo* info);
    static void UpdateLFDEntries();
    static void UpdateLFREntries();
    static void UpdateRFEntries();
    static void ShowProposal(int32_t silent);
    static void ParsePartyInfo(CDataStore* msg);
    static void ParseBlackList(CDataStore* msg, LFGLockInfo* lockInfo);
    static void UpdateRandomDungeons();
    static bool IsRandomDungeonUnlocked(const LfgDungeonsRec* rec);
    static bool UpdateRandomDungeonBestChoice();
    static void UpdateRandomSlots(int32_t minLevel, int32_t maxLevel);
    static LFGRandomSlotDungeons* GetRandomSlot(int32_t dungeonID);
    static int32_t GetBestRFChoice();
    static bool IsSlotValidForPlayer(uint32_t slot);
    static bool IsSlotValidForAll(uint32_t slot);
    static void RequestLFGPartyInfo();
    static void SetLFGComment(const char* comment);
    static void LeaveLFGSearch();
    static void LeaveWorld();
    static void LeaveLFG();
    static int32_t GetLFGProposal(lua_State* L);
    static int32_t GetLFGProposalMember(lua_State* L);
    static int32_t GetLFGProposalEncounter(lua_State* L);
    static void ProposalResponse(bool accept);
    static int32_t GetLFGRoleUpdateMember(lua_State* L);
    static int32_t SetLFGBootVote(lua_State* L);
    static int32_t GetLFGBootProposal(lua_State* L);
    static bool SetHeaderCollapsed(uint32_t header, bool collapsed);
    static bool SetDungeonEnabled(uint32_t dungeon, bool enabled);
    static void JoinLFGSearch(int32_t lfgID);
    static void ClearLFGDungeon(uint32_t slot);
    static int32_t GetQueueStatusIndex();
    static void ResortLFGLists();
    static bool GetLFGListEntryDisplayed(uint32_t index, const LFGListParty*& party, const LFGListPlayer*& player);
};

extern const char* s_roleTokens[3];

LfgDungeonExpansionRec* GetLfgDungeonExpansionRec(int32_t dungeonID, int32_t expansionLevel);
int32_t Packet_SMSG_LFG_OPEN_FROM_GOSSIP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

void LookingForGroupRegisterScriptFunctions();
void LookingForGroupUnregisterScriptFunctions();

void DungeonEncounterCountMatchingRows(int32_t mapId, int32_t difficulty, uint32_t completedMask, uint32_t* total, uint32_t* completed);
int32_t PushDungeonEncounterInfo(lua_State* L, int32_t mapId, int32_t difficulty, int32_t index, uint32_t completedMask, uint32_t otherMask);
static int32_t GetLFGDungeonSubtype(const LfgDungeonsRec* rec);
const char* GetRoleTokenFromFlags(uint32_t flags);
uint32_t GetRoleFlagFromToken(const char* token);

#include "WoW/Source/Ui/LFGInfo.inl"

#endif
