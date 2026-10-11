#include "LFGInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include <ctime>
#include "WoW/Source/Game/ValidateName.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/Ui/LFGHandler.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

// STATIC: 0x00F4A8C8
TSGrowableArray<LFG_SLOT_INFO_LOOT> CGLookingForGroup::m_completionRewards;

// STATIC: 0x00F4A4D8
int32_t CGLookingForGroup::m_searchJoinedID;

// STATIC: 0x00F4A4DC
uint32_t CGLookingForGroup::m_lastProposalID;

// STATIC: 0x00F4A4E8
uint32_t CGLookingForGroup::m_randomDungeonBestChoice;

// STATIC: 0x00F4AB8C
TSGrowableArray<uint32_t> CGLookingForGroup::m_randomDungeons;

// STATIC: 0x00F4AB9C
TSGrowableArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfdEntries;

// STATIC: 0x00F4ABAC
TSGrowableArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_lfrEntries;

// STATIC: 0x00F4ABBC
TSGrowableArray<CGLookingForGroup::LFGEntryInfo> CGLookingForGroup::m_rfEntries;

// STATIC: 0x00F4A368
CGLookingForGroup::LFGEntryInfo CGLookingForGroup::m_unkBEA820[7];

// STATIC: 0x00F4A3A8
LFGBootProposal CGLookingForGroup::m_bootProposal;

// STATIC: 0x00F4A8D8
LFGProposal CGLookingForGroup::m_proposal;

// STATIC: 0x00F4A4D4
uint8_t CGLookingForGroup::m_unkBEAA10;

// STATIC: 0x00F4ABD0
LFGLIST CGLookingForGroup::m_prevSearchResults;

// STATIC: 0x00D137C4
uint8_t CGLookingForGroup::m_unkACD660;

// STATIC: 0x00F4AC60
LFGLIST CGLookingForGroup::m_searchResults;

// STATIC: 0x00F4A510
LFGInfo CGLookingForGroup::m_localInfo;

// STATIC: 0x00F4A648
LFGInfo CGLookingForGroup::m_playerInfo;

// STATIC: 0x00F4A780
LFGInfo CGLookingForGroup::m_partyInfo;

// STATIC: 0x00D13A38
LFGLockInfo CGLookingForGroup::m_playerLockInfo;

// STATIC: 0x00D13A60
LFGPartyLockInfo CGLookingForGroup::m_partyLockInfo;

// STATIC: 0x00F4A8B8
LFGRewardInfo CGLookingForGroup::m_reward;

// STATIC: 0x00F4A4EC
LFGQueueStatus CGLookingForGroup::m_queueStatus;

// STATIC: 0x00F4AB68
int32_t CGLookingForGroup::m_roleCheckState;

// STATIC: 0x00F4AB6C
TSGrowableArray<uint32_t> CGLookingForGroup::m_roleCheckSlots;

// STATIC: 0x00F4AB7C
TSGrowableArray<LFG_ROLE_CHECK_MEMBER_INFO> CGLookingForGroup::m_roleCheckMembers;

// STATIC: 0x00D14460
static const LFGJoinResultError s_joinResultErrors[16] = {
    { 27, ERR_LFG_ROLE_CHECK_FAILED },
    { 28, ERR_LFG_GROUP_FULL },
    { 30, ERR_LFG_NO_LFG_OBJECT },
    { 31, ERR_LFG_NO_SLOTS_PLAYER },
    { 32, ERR_LFG_MISMATCHED_SLOTS },
    { 33, ERR_LFG_PARTY_PLAYERS_FROM_DIFFERENT_REALMS },
    { 34, ERR_LFG_MEMBERS_NOT_PRESENT },
    { 35, ERR_LFG_GET_INFO_TIMEOUT },
    { 36, ERR_LFG_INVALID_SLOT },
    { 37, ERR_LFG_DESERTER_PLAYER },
    { 38, ERR_LFG_DESERTER_PARTY },
    { 39, ERR_LFG_RANDOM_COOLDOWN_PLAYER },
    { 40, ERR_LFG_RANDOM_COOLDOWN_PARTY },
    { 41, ERR_LFG_TOO_MANY_MEMBERS },
    { 42, ERR_LFG_CANT_USE_DUNGEONS },
    { 43, ERR_LFG_ROLE_CHECK_FAILED },
};

// STATIC: 0x00D13A70
uint32_t CGLookingForGroup::m_randomSlotCount;

// STATIC: 0x00D13A74
LFGRandomSlot CGLookingForGroup::m_randomSlots[16];

// STATIC: 0x00F4A3A4
int32_t CGLookingForGroup::m_randomSlotMinLevel;

// STATIC: 0x00F4A3A0
int32_t CGLookingForGroup::m_randomSlotMaxLevel;

// STATIC: 0x00F4A4E0
static uint32_t s_searchType;

// STATIC: 0x00F4A4E4
static uint32_t s_searchID;

// OFFSET: 0x00985040/0x1407001D0
bool CGLookingForGroup::HasRaidDungeon(LFGInfo* info) {
    if (info->m_joined) {
        for (LFGDungeonEntry* entry = info->m_dungeons.Head(); !info->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(entry->m_slot & 0xFFFFFF);

            if (rec && rec->m_typeID == 2) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009850B0/0x140700240
bool CGLookingForGroup::HasRFDungeon(LFGInfo* info) {
    if (info->m_joined) {
        for (LFGDungeonEntry* entry = info->m_dungeons.Head(); !info->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(entry->m_slot & 0xFFFFFF);

            if (rec) {
                int32_t subtype = rec->m_typeID == 1 ? (rec->IsRaid() ? 3 : (rec->IsHeroic() ? 2 : 1)) : 0;

                if (subtype == 3 && (rec->m_flags & 0x2) && !(rec->m_flags & 0x4)) {
                    return true;
                }
            }
        }
    }

    return false;
}

// OFFSET: 0x009807A0/0x1406FAAC0
static int QSortLFGDungeons(const void* a, const void* b) {
    const CGLookingForGroup::LFGEntryInfo* left = static_cast<const CGLookingForGroup::LFGEntryInfo*>(a);
    const CGLookingForGroup::LFGEntryInfo* right = static_cast<const CGLookingForGroup::LFGEntryInfo*>(b);
    int32_t leftID = left->m_id;
    int32_t rightID = right->m_id;
    bool leftIsGroup = leftID < 0;
    int32_t leftGroupID = leftIsGroup ? -leftID : left->m_order;
    int32_t rightGroupID = rightID < 0 ? -rightID : right->m_order;

    if (leftGroupID != rightGroupID) {
        int32_t leftOrder = -1;
        int32_t rightOrder = -1;

        if (leftGroupID) {
            LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(leftGroupID);
            leftOrder = group ? group->m_orderIndex : 0;
        }

        if (rightGroupID) {
            LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(rightGroupID);
            rightOrder = group ? group->m_orderIndex : 0;
        }

        if (leftOrder != rightOrder) {
            return leftOrder - rightOrder;
        }

        return leftGroupID - rightGroupID;
    }

    if (leftIsGroup != (rightID < 0)) {
        return 2 * !leftIsGroup - 1;
    }

    LfgDungeonsRec* leftRec = g_lfgDungeonsDB.GetRecord(leftID);
    LfgDungeonsRec* rightRec = g_lfgDungeonsDB.GetRecord(rightID);

    if (leftRec->m_orderIndex != rightRec->m_orderIndex) {
        return leftRec->m_orderIndex - rightRec->m_orderIndex;
    }

    LfgDungeonExpansionRec* leftExpansion = GetLfgDungeonExpansionRec(leftRec->m_id, ClientServices::Connection()->m_accountExpansion);
    LfgDungeonExpansionRec* rightExpansion = GetLfgDungeonExpansionRec(rightRec->m_id, ClientServices::Connection()->m_accountExpansion);
    int32_t leftLevel = leftExpansion ? leftExpansion->m_hardLevelMin + leftExpansion->m_hardLevelMax : leftRec->m_minLevel + leftRec->m_maxLevel;
    int32_t rightLevel = rightExpansion ? rightExpansion->m_hardLevelMin + rightExpansion->m_hardLevelMax : rightRec->m_minLevel + rightRec->m_maxLevel;

    if (leftLevel != rightLevel) {
        return leftLevel - rightLevel;
    }

    return SStrCmpUTF8I(leftRec->m_name, rightRec->m_name, 0x7FFFFFFF);
}

// OFFSET: 0x00980910
static int QSortRFDungeons(const void* a, const void* b) {
    LfgDungeonsRec* leftRec = g_lfgDungeonsDB.GetRecord(static_cast<const CGLookingForGroup::LFGEntryInfo*>(a)->m_id);
    LfgDungeonsRec* rightRec = g_lfgDungeonsDB.GetRecord(static_cast<const CGLookingForGroup::LFGEntryInfo*>(b)->m_id);

    if (leftRec->m_orderIndex != rightRec->m_orderIndex) {
        return leftRec->m_orderIndex - rightRec->m_orderIndex;
    }

    return SStrCmpUTF8I(leftRec->m_name, rightRec->m_name, 0x7FFFFFFF);
}

// OFFSET: 0x00984B30
void CGLookingForGroup::UpdateLFDEntries() {
    CGLookingForGroup::m_lfdEntries.m_chunk = 32;

    uint32_t numDungeons = g_lfgDungeonsDB.m_numRecords;

    for (uint32_t i = 0; i < numDungeons; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec) {
            int32_t subtype = rec->m_typeID == 1 ? (rec->IsRaid() ? 3 : (rec->IsHeroic() ? 2 : 1)) : 0;

            if ((rec->m_flags & 0x2) && rec->m_typeID == 1 && !(rec->m_flags & 0x4) && (subtype == 1 || subtype == 2)) {
                CGLookingForGroup::LFGEntryInfo* entry = CGLookingForGroup::m_lfdEntries.New();
                entry->m_id = rec->m_id;
                entry->m_order = rec->GetTopGroupID();
            }
        }
    }

    uint32_t numGroups = g_lfgDungeonGroupDB.m_numRecords;

    for (uint32_t i = 0; i < numGroups; i++) {
        LfgDungeonGroupRec* rec = g_lfgDungeonGroupDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID == 1) {
            CGLookingForGroup::LFGEntryInfo* entry = CGLookingForGroup::m_lfdEntries.New();
            entry->m_id = -rec->m_id;
            entry->m_order = rec->m_parentGroupID;
        }
    }

    qsort(CGLookingForGroup::m_lfdEntries.m_data, CGLookingForGroup::m_lfdEntries.m_count, sizeof(CGLookingForGroup::LFGEntryInfo), QSortLFGDungeons);
}

// OFFSET: 0x00984D50
void CGLookingForGroup::UpdateRFEntries() {
    CGLookingForGroup::m_rfEntries.m_chunk = 8;

    uint32_t numDungeons = g_lfgDungeonsDB.m_numRecords;

    for (uint32_t i = 0; i < numDungeons; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec) {
            int32_t subtype = rec->m_typeID == 1 ? (rec->IsRaid() ? 3 : (rec->IsHeroic() ? 2 : 1)) : 0;

            if (subtype == 3 && (rec->m_flags & 0x2) && !(rec->m_flags & 0x4)) {
                CGLookingForGroup::LFGEntryInfo* entry = CGLookingForGroup::m_rfEntries.New();
                entry->m_id = rec->m_id;
                entry->m_order = rec->GetTopGroupID();
            }
        }
    }

    qsort(CGLookingForGroup::m_rfEntries.m_data, CGLookingForGroup::m_rfEntries.m_count, sizeof(CGLookingForGroup::LFGEntryInfo), QSortRFDungeons);
}

// OFFSET: 0x00984E70
void CGLookingForGroup::UpdateLFREntries() {
    CGLookingForGroup::m_lfrEntries.m_chunk = 8;

    uint32_t numDungeons = g_lfgDungeonsDB.m_numRecords;

    for (uint32_t i = 0; i < numDungeons; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (rec && rec->m_typeID == 2) {
            CGLookingForGroup::LFGEntryInfo* entry = CGLookingForGroup::m_lfrEntries.New();
            entry->m_id = rec->m_id;
            entry->m_order = rec->GetTopGroupID();
        }
    }

    uint32_t numGroups = g_lfgDungeonGroupDB.m_numRecords;

    for (uint32_t i = 0; i < numGroups; i++) {
        LfgDungeonGroupRec* rec = g_lfgDungeonGroupDB.GetRecordByIndex(i);

        if (rec) {
            CGLookingForGroup::LFGEntryInfo* entry = CGLookingForGroup::m_lfrEntries.New();
            entry->m_id = -rec->m_id;
            entry->m_order = rec->m_parentGroupID;
        }
    }

    qsort(CGLookingForGroup::m_lfrEntries.m_data, CGLookingForGroup::m_lfrEntries.m_count, sizeof(CGLookingForGroup::LFGEntryInfo), QSortLFGDungeons);
}

// OFFSET: 0x00987380
void LFGInfo::Reset() {
    *reinterpret_cast<uint64_t*>(this->m_ticket.m_requesterGuid) = 0;
    this->m_ticket.m_id = 0;
    this->m_ticket.m_type = 0;
    this->m_ticket.m_time = 0;
    this->m_joined = 0;
    this->unk_0001 = 0;
    this->m_queued = 0;
    this->unk_0003 = 0;
    this->unk_0004 = 0;
    this->unk_0005 = 0;
    memset(this->m_roles, 0, sizeof(this->m_roles));

    for (LFGDungeonEntry* dungeon = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();
        dungeon->~LFGDungeonEntry();
        SMemFree(dungeon, "delete", -1, 0);
        dungeon = next;
    }

    this->m_comment[0] = '\0';
    this->m_numDungeons = 0;
}

// OFFSET: 0x009877F0
void CGLookingForGroup::JoiningParty() {
    CGLookingForGroup::m_playerInfo.Reset();
    CGLookingForGroup::m_partyInfo.Reset();

    {
        UserClientOpcode2581 request;
        ClientServices::Send(&request);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, NULL);
}

// OFFSET: 0x00987840/0x140703580
void CGLookingForGroup::ClearDungeonList() {
    for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();
        dungeon->~LFGDungeonEntry();
        SMemFree(dungeon, "delete", -1, 0);
        dungeon = next;
    }

    CGLookingForGroup::m_localInfo.unk_0003 = 1;
    CGLookingForGroup::m_localInfo.m_numDungeons = 0;
}

// OFFSET: 0x00987D80
void LFGLockInfo::ClearDungeons() {
    for (LFGDungeonEntry* dungeon = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();
        dungeon->~LFGDungeonEntry();
        SMemFree(dungeon, "delete", -1, 0);
        dungeon = next;
    }
}

// INLINED
LFGLockInfo::LFGLockInfo() {
    this->m_valid = 0;
    this->m_guid.m_guid = 0;
    this->ClearDungeons();
}

// OFFSET: 0x00989BE0
LFGLockInfo::~LFGLockInfo() {
    this->m_valid = 0;
    this->m_guid.m_guid = 0;
    this->ClearDungeons();
}

// OFFSET: 0x00989EC0
void LFGPartyLockInfo::Clear() {
    this->m_valid = 0;

    for (LFGLockInfo* member = this->m_members.Head(); !this->m_members.IsEnd(member); ) {
        LFGLockInfo* next = member->RawNext();
        member->~LFGLockInfo();
        SMemFree(member, "delete", -1, 0);
        member = next;
    }
}

// OFFSET: 0x004952D0/0x1400BACE0
LfgDungeonExpansionRec* GetLfgDungeonExpansionRec(int32_t dungeonID, int32_t expansionLevel) {
    for (int32_t i = 0; i < g_lfgDungeonExpansionDB.m_numRecords; i++) {
        if (g_lfgDungeonExpansionDB.GetRecordByIndex(i)->m_lfgID != dungeonID) {
            continue;
        }

        for (int32_t j = i; j < g_lfgDungeonExpansionDB.m_numRecords; j++) {
            LfgDungeonExpansionRec* rec = g_lfgDungeonExpansionDB.GetRecordByIndex(j);

            if (rec->m_lfgID != dungeonID) {
                return NULL;
            }

            if (rec->m_expansionLevel == expansionLevel) {
                return rec;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x0097EF40
void LFGProposal::Clear() {
    *reinterpret_cast<uint64_t*>(this->m_ticket.m_requesterGuid) = 0;
    this->m_ticket.m_id = 0;
    this->m_ticket.m_type = 0;
    this->m_ticket.m_time = 0;
    this->unk_0018 = 0;
    this->unk_001C = 0;
    this->m_id = 0;
    this->m_state = 0;
    this->unk_0008 = 0;
    this->m_completedMask = 0;
    this->unk_0010 = 0;
    memset(&this->m_numMembers, 0, sizeof(this->m_numMembers) + sizeof(this->m_members));
}

// INLINED
void LFGQueueStatus::Clear() {
    this->m_lfgID = 0;
    this->m_averageWait = 0;
    this->m_tankWait = 0;
    this->m_healerWait = 0;
    this->m_damageWait = 0;
    this->m_myWait = 0;
    this->m_tanksNeeded = 0;
    this->m_healersNeeded = 0;
    this->m_damageNeeded = 0;
    this->m_statusTime = 0;
    this->m_queuedTime = 0;
}

// INLINED
void CGLookingForGroup::ShowProposal(int32_t silent) {
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_UPDATE, NULL);

    if (CGLookingForGroup::m_lastProposalID != CGLookingForGroup::m_proposal.unk_0008) {
        CGLookingForGroup::m_lastProposalID = CGLookingForGroup::m_proposal.unk_0008;

        if (!silent) {
            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_SHOW, NULL);
        }
    }
}

// OFFSET: 0x00984490
LFGDungeonEntry* LFGInfo::FindDungeon(uint32_t slot) const {
    for (LFGDungeonEntry* dungeon = const_cast<LFGInfo*>(this)->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
        if (dungeon->m_slot == slot) {
            return dungeon;
        }
    }

    return NULL;
}

// OFFSET: 0x009844C0
bool LFGInfo::Equals(const LFGInfo& other) const {
    if (this->m_ticket.m_id != other.m_ticket.m_id || *reinterpret_cast<const uint64_t*>(this->m_ticket.m_requesterGuid) != *reinterpret_cast<const uint64_t*>(other.m_ticket.m_requesterGuid) || this->m_ticket.m_type != other.m_ticket.m_type || this->m_ticket.m_time != other.m_ticket.m_time) {
        return false;
    }

    if ((this->m_joined != 0) != (other.m_joined != 0) || (this->unk_0001 != 0) != (other.unk_0001 != 0) || (this->m_queued != 0) != (other.m_queued != 0) || (this->unk_0004 != 0) != (other.unk_0004 != 0) || (this->unk_0005 != 0) != (other.unk_0005 != 0) || this->m_numDungeons != other.m_numDungeons) {
        return false;
    }

    if (other.m_roles[0] != this->m_roles[0] || other.m_roles[1] != this->m_roles[1] || other.m_roles[2] != this->m_roles[2] || SStrCmp(this->m_comment, other.m_comment, 0x7FFFFFFF)) {
        return false;
    }

    LFGInfo* self = const_cast<LFGInfo*>(this);

    for (LFGDungeonEntry* dungeon = self->m_dungeons.Head(); !self->m_dungeons.IsEnd(dungeon); dungeon = self->m_dungeons.Next(dungeon)) {
        if (!other.FindDungeon(dungeon->m_slot)) {
            return false;
        }
    }

    return true;
}

// INLINED
LFGShortageReward::LFGShortageReward() {
    this->m_roleMask = 0;
    this->m_money = 0;
    this->m_xp = 0;
}

// OFFSET: 0x00986790/0x140701D10
LFGDungeonEntry::LFGDungeonEntry() {
    this->m_slot = 0;
    this->m_rewardDone = 0;
    this->unk_0010 = 0;
    this->unk_0014 = 0;
    this->m_currencyID = 0;
    this->unk_001C[0] = 0;
    this->unk_001C[1] = 0;
    this->unk_001C[2] = 0;
    this->unk_001C[3] = 0;
    this->unk_001C[4] = 0;
    this->unk_001C[5] = 0;
    this->unk_001C[6] = 0;
    this->unk_001C[7] = 0;
    this->unk_003C = 0;
    this->m_completedEncounters = 0;
    this->m_locked = 0;
    this->m_lockReason = 0;
    this->unk_004C = 0;
    this->unk_0050 = 0;
    this->m_baseMoney = 0;
    this->m_baseXP = 0;
    this->unk_006C = 0;
}

// OFFSET: 0x009868D0/0x140701DF0
LFGDungeonEntry::~LFGDungeonEntry() {
}

// OFFSET: 0x00986860
LFGDungeonEntry* LFGLockInfo::AddDungeon(uint32_t slot) {
    for (LFGDungeonEntry* entry = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(entry); ) {
        LFGDungeonEntry* next = entry->RawNext();

        if (entry->m_slot == slot) {
            return entry;
        }

        entry = next;
    }

    LFGDungeonEntry* dungeon = new (__FILE__, __LINE__) LFGDungeonEntry();
    dungeon->m_slot = slot;
    this->m_dungeons.LinkToTail(dungeon);
    return dungeon;
}

// INLINED
LFGInfo::LFGInfo() {
    this->Reset();
}

// OFFSET: 0x00987DC0
LFGInfo::~LFGInfo() {
    this->Reset();
}

// OFFSET: 0x00986960
void LFGInfo::AddDungeon(uint32_t slot) {
    for (LFGDungeonEntry* entry = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(entry); ) {
        LFGDungeonEntry* next = entry->RawNext();

        if (entry->m_slot == slot) {
            return;
        }

        entry = next;
    }

    LFGDungeonEntry* dungeon = new (__FILE__, __LINE__) LFGDungeonEntry();
    dungeon->m_slot = slot;
    this->m_dungeons.LinkToTail(dungeon);
    this->m_numDungeons++;
}

// OFFSET: 0x00987400
LFGInfo& LFGInfo::CopyFrom(const LFGInfo& other) {
    this->m_ticket = other.m_ticket;
    this->m_joined = other.m_joined;
    this->unk_0001 = other.unk_0001;
    this->m_queued = other.m_queued;
    this->unk_0004 = other.unk_0004;
    this->unk_0005 = other.unk_0005;
    memcpy(this->m_roles, other.m_roles, sizeof(this->m_roles));
    SStrCopy(this->m_comment, other.m_comment, sizeof(this->m_comment));

    for (LFGDungeonEntry* dungeon = this->m_dungeons.Head(); !this->m_dungeons.IsEnd(dungeon); ) {
        LFGDungeonEntry* next = dungeon->RawNext();

        if (!other.FindDungeon(dungeon->m_slot)) {
            dungeon->~LFGDungeonEntry();
        SMemFree(dungeon, "delete", -1, 0);
            this->m_numDungeons--;
        }

        dungeon = next;
    }

    LFGInfo& source = const_cast<LFGInfo&>(other);

    for (LFGDungeonEntry* dungeon = source.m_dungeons.Head(); !source.m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
        if (!this->FindDungeon(dungeon->m_slot)) {
            this->AddDungeon(dungeon->m_slot);
        }
    }

    return *this;
}

// INLINED
static void ClearSearchResults() {
    CGLookingForGroup::m_searchResults.Clear();
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
}

// OFFSET: 0x00989D00
void CGLookingForGroup::ParsePartyInfo(CDataStore* msg) {
    uint8_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid;
        msg->Get(guid.m_guid);
        LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.AddMember(guid);
        member->ClearDungeons();
        member->m_valid = 1;
        CGLookingForGroup::ParseBlackList(msg, member);
    }
}

// OFFSET: 0x009802C0
static int32_t Packet_SMSG_LFG_ROLE_CHOSEN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t guid;
    msg->Get(guid);
    uint8_t accepted = 0;
    msg->Get(accepted);
    uint32_t roles;
    msg->Get(roles);
    NameCache* name = g_nameDBCache.GetRecord(guid, WOWGUID(), NULL, NULL, false);

    if (name) {
        if (accepted) {
            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_ROLE_CHOSEN, "%s%b%b%b", static_cast<const char*>(name->m_name), (roles >> 1) & 1, (roles >> 2) & 1, (roles >> 3) & 1);
            return 1;
        }

        CGGameUI::DisplayError(ERR_LFG_PLAYER_DECLINED_ROLE_CHECK, name->m_name);
    }

    return 1;
}

// OFFSET: 0x00980440
static int32_t Packet_SMSG_LFG_BOOT_PROPOSAL_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    LFGBootProposal& boot = CGLookingForGroup::m_bootProposal;
    uint8_t value = 0;
    msg->Get(value);
    boot.m_inProgress = value;
    value = 0;
    msg->Get(value);
    boot.unk_0004 = value;
    value = 0;
    msg->Get(value);
    boot.m_didVote = value;
    value = 0;
    msg->Get(value);
    boot.m_myVote = value;
    msg->Get(boot.m_target.m_guid);
    msg->Get(boot.m_totalVotes);
    msg->Get(boot.m_bootVotes);
    msg->Get(boot.m_timeLeft);
    msg->Get(boot.m_votesNeeded);
    msg->GetString(boot.m_reason, sizeof(boot.m_reason));
    DebugLog("LFGMessage: LFG_BOOT_PLAYER - In Progress: %d, I Voted: %d, My Vote: %d, Target: %016llX, Total Votes: %u, Boot Votes: %u, Time Left: %d, Votes Needed: %u, Comment: %s", boot.m_inProgress, boot.m_didVote, boot.m_myVote, boot.m_target.m_guid, boot.m_totalVotes, boot.m_bootVotes, boot.m_timeLeft, boot.m_votesNeeded, boot.m_reason);

    if (!boot.m_inProgress) {
        NameCache* name = g_nameDBCache.GetRecord(boot.m_target.m_guid, WOWGUID(), NULL, NULL, false);

        if (name) {
            CGGameUI::DisplayError(boot.unk_0004 ? ERR_PARTY_LFG_BOOT_VOTE_SUCCEEDED : ERR_PARTY_LFG_BOOT_VOTE_FAILED, name->m_name);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_BOOT_PROPOSAL_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x009805B0
static int32_t Packet_SMSG_LFG_TELEPORT_DENIED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t value;
    msg->Get(value);
    uint32_t reason = value;
    DebugLog("LFGMessage: LFG_TELEPORT_DENIED - Reason: %u", reason);

    switch (reason) {
        case 0:
        case 5:
        case 7:
            break;

        case 1:
            CGGameUI::DisplayError(ERR_PLAYER_DEAD);
            break;

        case 2:
            CGGameUI::DisplayError(ERR_NOT_WHILE_FALLING);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_NOT_WHILE_FATIGUED);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_INVALID_TELEPORT_LOCATION);
            break;

        default:
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            break;
    }

    return 1;
}

// OFFSET: 0x00980680
static int32_t Packet_SMSG_LFG_OFFER_CONTINUE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t value;
    msg->Get(value);
    uint32_t slot = value;
    DebugLog("LFGMessage: LFG_OFFER_CONTINUE - Slot: %u", slot);
    LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(slot & 0xFFFFFF);

    if (dungeon) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_OFFER_CONTINUE, "%s%d%d", static_cast<const char*>(dungeon->m_name), dungeon->m_id, dungeon->m_typeID);
    }

    return 1;
}

// OFFSET: 0x00984620
static int32_t Packet_SMSG_LFG_ROLE_CHECK_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(CGLookingForGroup::m_roleCheckState));
    uint8_t beginning = 0;
    msg->Get(beginning);
    uint8_t slotCount;
    msg->Get(slotCount);
    CGLookingForGroup::m_roleCheckSlots.SetCount(slotCount);

    for (uint32_t i = 0; i < slotCount; i++) {
        msg->Get(CGLookingForGroup::m_roleCheckSlots.m_data[i]);
    }

    uint8_t memberCount;
    msg->Get(memberCount);
    CGLookingForGroup::m_roleCheckMembers.SetCount(memberCount);
    uint32_t localIndex = 0;

    for (uint32_t i = 0; i < memberCount; i++) {
        msg->Get(CGLookingForGroup::m_roleCheckMembers.m_data[i].m_guid.m_guid);
        uint8_t confirmed = 0;
        msg->Get(confirmed);
        CGLookingForGroup::m_roleCheckMembers.m_data[i].m_confirmed = confirmed;
        msg->Get(CGLookingForGroup::m_roleCheckMembers.m_data[i].m_roles);
        msg->Get(CGLookingForGroup::m_roleCheckMembers.m_data[i].m_level);

        if (ClntObjMgrGetActivePlayer() == CGLookingForGroup::m_roleCheckMembers.m_data[i].m_guid) {
            localIndex = i;
        }
    }

    DebugLog("LFGMessage: LFG_ROLE_CHECK_UPDATE - Status: %u, Is Beginning: %d, Count: %d", CGLookingForGroup::m_roleCheckState, beginning, memberCount);

    if (beginning) {
        CGGameUI::DisplayError(ERR_LFG_ROLE_CHECK_INITIATED);
        SI3::PlayUISound(17318, NULL, true);

        for (uint32_t i = 0; i < memberCount; i++) {
            if (!CGLookingForGroup::m_roleCheckMembers.m_data[i].m_confirmed) {
                continue;
            }

            NameCache* name = g_nameDBCache.GetRecord(CGLookingForGroup::m_roleCheckMembers.m_data[i].m_guid.m_guid, WOWGUID(), NULL, NULL, false);

            if (name) {
                FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_ROLE_CHOSEN, "%s%b%b%b", static_cast<const char*>(name->m_name), (CGLookingForGroup::m_roleCheckMembers.m_data[i].m_roles >> 1) & 1, (CGLookingForGroup::m_roleCheckMembers.m_data[i].m_roles >> 2) & 1, (CGLookingForGroup::m_roleCheckMembers.m_data[i].m_roles >> 3) & 1);
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_UPDATE, NULL);

    if (!CGLookingForGroup::m_roleCheckMembers.m_data[localIndex].m_confirmed && CGLookingForGroup::m_roleCheckState == 2) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_SHOW, NULL);
        return 1;
    }

    if (CGLookingForGroup::m_roleCheckState == 5) {
        SI3::PlayUISound(17341, NULL, true);
        CGGameUI::DisplayError(ERR_LFG_ROLE_CHECK_ABORTED);
    } else if (CGLookingForGroup::m_roleCheckState == 6) {
        SI3::PlayUISound(17341, NULL, true);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_CHECK_HIDE, NULL);
    return 1;
}

// OFFSET: 0x00984920
static int32_t Packet_SMSG_LFG_PLAYER_REWARD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGLookingForGroup::m_reward.m_randomSlot = 0;
    CGLookingForGroup::m_reward.m_actualSlot = 0;
    CGLookingForGroup::m_reward.m_baseMoney = 0;
    CGLookingForGroup::m_reward.m_baseXP = 0;
    CGLookingForGroup::m_completionRewards.Clear();
    msg->Get(CGLookingForGroup::m_reward.m_randomSlot);
    msg->Get(CGLookingForGroup::m_reward.m_actualSlot);
    msg->Get(reinterpret_cast<uint32_t&>(CGLookingForGroup::m_reward.m_baseMoney));
    msg->Get(reinterpret_cast<uint32_t&>(CGLookingForGroup::m_reward.m_baseXP));
    DebugLog("LFGMessage: LFG_PLAYER_REWARD - Queued Slot: %u, Actual Slot: %u, Base Money: %d, Base XP: %d", CGLookingForGroup::m_reward.m_randomSlot, CGLookingForGroup::m_reward.m_actualSlot, CGLookingForGroup::m_reward.m_baseMoney, CGLookingForGroup::m_reward.m_baseXP);
    uint8_t itemCount;
    msg->Get(itemCount);
    CGLookingForGroup::m_completionRewards.SetCount(itemCount);

    for (uint32_t i = 0; i < itemCount; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(CGLookingForGroup::m_completionRewards.m_data[i].m_id));
        msg->Get(reinterpret_cast<uint32_t&>(CGLookingForGroup::m_completionRewards.m_data[i].m_displayID));
        msg->Get(CGLookingForGroup::m_completionRewards.m_data[i].m_quantity);
        int32_t& isCurrency = CGLookingForGroup::m_completionRewards.m_data[i].m_isCurrency;
        uint8_t value = 0;
        msg->Get(value);
        isCurrency = value;
        DebugLog("LFG_PLAYER_REWARD: Receiving Item %u, Display %u, Quantity: %u", CGLookingForGroup::m_completionRewards.m_data[i].m_id, CGLookingForGroup::m_completionRewards.m_data[i].m_displayID, CGLookingForGroup::m_completionRewards.m_data[i].m_quantity);
    }

    if (CGLookingForGroup::m_reward.m_baseXP > 0) {
        CGGameUI::DisplayError(ERR_QUEST_REWARD_EXP_I, CGLookingForGroup::m_reward.m_baseXP);
    }

    if (CGLookingForGroup::m_reward.m_baseMoney > 0) {
        char money[1024];
        GetCurrencyText(CGLookingForGroup::m_reward.m_baseMoney, money, sizeof(money), ", ");
        CGGameUI::DisplayError(ERR_QUEST_REWARD_MONEY_S, money);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_COMPLETION_REWARD, NULL);
    return 1;
}

// OFFSET: 0x00988700
static int32_t Packet_SMSG_LFG_PLAYER_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGLookingForGroup::m_playerLockInfo.m_valid = 0;
    CGLookingForGroup::m_playerLockInfo.m_guid.m_guid = 0;
    CGLookingForGroup::m_playerLockInfo.ClearDungeons();
    uint8_t dungeonCount;
    msg->Get(dungeonCount);
    DebugLog("LFGMessage: MSG_LFG_PLAYER_INFO");

    for (uint32_t i = 0; i < dungeonCount; i++) {
        uint32_t slot;
        msg->Get(slot);
        LFGDungeonEntry* dungeon = CGLookingForGroup::m_playerLockInfo.AddDungeon(slot);
        uint8_t flag = 0;
        msg->Get(flag);
        dungeon->m_rewardDone = flag != 0;
        msg->Get(dungeon->unk_0010);
        msg->Get(dungeon->unk_0014);
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->m_currencyID));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[0]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[1]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[2]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[3]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[4]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[5]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[6]));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->unk_001C[7]));
        msg->Get(dungeon->unk_003C);
        msg->Get(dungeon->m_completedEncounters);
        flag = 0;
        msg->Get(flag);
        dungeon->unk_006C = flag != 0;

        for (uint32_t j = 0; j < 3; j++) {
            LFGShortageReward& shortage = dungeon->m_shortages[j];
            uint32_t roleMask = 0;
            msg->Get(roleMask);
            shortage.m_roleMask = roleMask;

            if (roleMask) {
                msg->Get(reinterpret_cast<uint32_t&>(shortage.m_money));
                msg->Get(reinterpret_cast<uint32_t&>(shortage.m_xp));
                uint8_t count = 0;
                msg->Get(count);
                shortage.m_rewards.SetCount(count);

                for (uint32_t k = 0; k < count; k++) {
                    msg->Get(reinterpret_cast<uint32_t&>(shortage.m_rewards.m_data[k].m_id));
                    msg->Get(reinterpret_cast<uint32_t&>(shortage.m_rewards.m_data[k].m_displayID));
                    msg->Get(shortage.m_rewards.m_data[k].m_quantity);
                    int32_t& isCurrency = shortage.m_rewards.m_data[k].m_isCurrency;
                    uint8_t value = 0;
                    msg->Get(value);
                    isCurrency = value;
                }
            }
        }

        msg->Get(reinterpret_cast<uint32_t&>(dungeon->m_baseMoney));
        msg->Get(reinterpret_cast<uint32_t&>(dungeon->m_baseXP));
        uint8_t itemCount;
        msg->Get(itemCount);
        dungeon->m_rewards.SetCount(itemCount);

        for (uint32_t j = 0; j < itemCount; j++) {
            msg->Get(reinterpret_cast<uint32_t&>(dungeon->m_rewards.m_data[j].m_id));
            msg->Get(reinterpret_cast<uint32_t&>(dungeon->m_rewards.m_data[j].m_displayID));
            msg->Get(dungeon->m_rewards.m_data[j].m_quantity);
            int32_t& isCurrency = dungeon->m_rewards.m_data[j].m_isCurrency;
            uint8_t value = 0;
            msg->Get(value);
            isCurrency = value;
        }
    }

    CGLookingForGroup::ParseBlackList(msg, &CGLookingForGroup::m_playerLockInfo);
    CGLookingForGroup::m_playerLockInfo.m_valid = 1;
    CGLookingForGroup::UpdateRandomDungeons();
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_LOCK_INFO_RECEIVED, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE_RANDOM_INFO, NULL);
    return 1;
}

// OFFSET: 0x0098A0D0
static int32_t Packet_SMSG_LFG_DISABLED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGLookingForGroup::m_localInfo.Reset();
    CGLookingForGroup::m_playerInfo.Reset();
    CGLookingForGroup::m_partyInfo.Reset();
    CGLookingForGroup::m_playerLockInfo.m_valid = 0;
    CGLookingForGroup::m_playerLockInfo.m_guid.m_guid = 0;
    CGLookingForGroup::m_playerLockInfo.ClearDungeons();
    CGLookingForGroup::m_partyLockInfo.Clear();
    CGLookingForGroup::m_queueStatus.Clear();
    CGLookingForGroup::m_reward.m_randomSlot = 0;
    CGLookingForGroup::m_reward.m_actualSlot = 0;
    CGLookingForGroup::m_reward.m_baseMoney = 0;
    CGLookingForGroup::m_reward.m_baseXP = 0;
    CGLookingForGroup::m_completionRewards.Clear();
    DebugLog("LFGMessage: LFG_DISABLED");
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, NULL);
    CGGameUI::DisplayError(ERR_SYSTEM_DISABLED);
    return 1;
}

// OFFSET: 0x0098A370
static int32_t Packet_SMSG_LFG_PARTY_INFO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    DebugLog("LFGMessage: LFG_PARTY_INFO");
    CGLookingForGroup::m_partyLockInfo.Clear();
    CGLookingForGroup::ParsePartyInfo(msg);
    CGLookingForGroup::m_partyLockInfo.m_valid = 1;
    CGLookingForGroup::UpdateRandomDungeons();
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_LOCK_INFO_RECEIVED, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE_RANDOM_INFO, NULL);
    return 1;
}

// OFFSET: 0x00980380
static bool OnLfgQueueStatus(ClientLfgQueueStatus* msg) {
    CGLookingForGroup::m_queueStatus.m_lfgID = msg->unk_0038 & 0xFFFFFF;
    CGLookingForGroup::m_queueStatus.m_averageWait = msg->unk_0010;
    CGLookingForGroup::m_queueStatus.m_tankWait = msg->unk_0018;
    CGLookingForGroup::m_queueStatus.m_healerWait = msg->unk_003C[0];
    CGLookingForGroup::m_queueStatus.m_damageWait = msg->unk_003C[1];
    CGLookingForGroup::m_queueStatus.m_myWait = msg->unk_003C[2];
    CGLookingForGroup::m_queueStatus.m_tanksNeeded = msg->unk_0048[0];
    CGLookingForGroup::m_queueStatus.m_healersNeeded = msg->unk_0048[1];
    CGLookingForGroup::m_queueStatus.m_damageNeeded = msg->unk_0048[2];
    CGLookingForGroup::m_queueStatus.m_queuedTime = FrameTime::GetCurTimeMs() - 1000 * msg->unk_0014;

    if (!CGLookingForGroup::m_queueStatus.m_tanksNeeded && !CGLookingForGroup::m_queueStatus.m_healersNeeded && !CGLookingForGroup::m_queueStatus.m_damageNeeded) {
        CGLookingForGroup::m_queueStatus.m_damageNeeded = 1;
    }

    CGLookingForGroup::m_queueStatus.m_statusTime = static_cast<uint32_t>(::time(NULL));
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_QUEUE_STATUS_UPDATE, NULL);
    return true;
}

// OFFSET: 0x009800F0
static bool OnLfgProposalUpdate(ClientLfgProposalUpdate* msg) {
    LFGProposal& proposal = CGLookingForGroup::m_proposal;
    proposal.m_ticket = *reinterpret_cast<JamClientRideTicket*>(msg->unk_0018);
    proposal.unk_0018 = *reinterpret_cast<uint32_t*>(&msg->unk_0030[0]);
    proposal.unk_001C = *reinterpret_cast<uint32_t*>(&msg->unk_0030[4]);
    proposal.m_id = msg->unk_0054;
    proposal.m_state = msg->unk_0010;
    proposal.unk_0008 = msg->unk_004C;
    proposal.m_completedMask = msg->unk_0048;
    proposal.unk_0010 = msg->unk_0050;
    proposal.m_numMembers = 0;
    bool declinedSelf = false;
    bool declinedParty = false;

    for (uint32_t i = 0; i < msg->unk_0038.Count(); i++) {
        LFGProposalMember& member = proposal.m_members[proposal.m_numMembers++];
        member.m_roles = msg->unk_0038[i].unk_0000;
        member.m_isLocal = msg->unk_0038[i].unk_0004;
        member.unk_0008 = msg->unk_0038[i].unk_0005;
        member.unk_000C = msg->unk_0038[i].unk_0006;
        member.m_accepted = msg->unk_0038[i].unk_0007;
        member.unk_0014 = msg->unk_0038[i].unk_0008;

        if (member.m_accepted && !member.unk_0014) {
            if (member.m_isLocal) {
                declinedSelf = true;
            } else if (member.unk_000C) {
                declinedParty = true;
            }
        }
    }

    CGLookingForGroup::ShowProposal(proposal.unk_0010);

    if (declinedParty) {
        CGGameUI::DisplayError(ERR_LFG_PROPOSAL_DECLINED_PARTY);
    } else if (declinedSelf) {
        CGGameUI::DisplayError(ERR_LFG_PROPOSAL_DECLINED_SELF);
    }

    if (msg->unk_0010 == 1) {
        SI3::PlayUISound(17341, NULL, true);
        proposal.Clear();
        FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_FAILED, NULL);
    }

    return true;
}

// OFFSET: 0x00980770
static bool OnUpdateDungeonEncounterForLoot(ClientUpdateDungeonEncounterForLoot* msg) {
    UserClientLfgLockInfoRequest request;
    request.unk_0010 = true;
    ClientServices::Send(&request);
    return true;
}

// OFFSET: 0x0098A1D0
static bool OnLfgJoinResult(ClientLfgJoinResult* msg) {
    uint8_t result = msg->unk_0010;
    DebugLog("LFGMessage: LFG_JOIN_RESULT - Result: %u", result);

    if (result == 31) {
        CGLookingForGroup::m_partyLockInfo.Clear();

        for (uint32_t i = 0; i < msg->unk_0014.Count(); i++) {
            LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.AddMember(*reinterpret_cast<WOWGUID*>(msg->unk_0014[i].unk_0000));
            member->ClearDungeons();
            member->m_valid = 1;

            for (uint32_t j = 0; j < msg->unk_0014[i].unk_0008.Count(); j++) {
                LFGDungeonEntry* entry = member->AddDungeon(msg->unk_0014[i].unk_0008[j].unk_0000);
                entry->m_locked = 1;
                entry->m_lockReason = msg->unk_0014[i].unk_0008[j].unk_0004;
                entry->unk_004C = msg->unk_0014[i].unk_0008[j].unk_0008;
                entry->unk_0050 = msg->unk_0014[i].unk_0008[j].unk_000C;
            }
        }

        CGLookingForGroup::m_partyLockInfo.m_valid = 1;
    }

    if (msg->unk_0010 == 27) {
        if (msg->unk_0011 == 3) {
            CGGameUI::DisplayError(ERR_LFG_ROLE_CHECK_FAILED_TIMEOUT);
            return true;
        }

        if (msg->unk_0011 == 4) {
            CGGameUI::DisplayError(ERR_LFG_ROLE_CHECK_FAILED_NOT_VIABLE);
            return true;
        }
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (s_joinResultErrors[i].m_result == msg->unk_0010) {
            CGGameUI::DisplayError(s_joinResultErrors[i].m_error);
            break;
        }
    }

    return true;
}

// OFFSET: 0x00987F00
static bool OnLfgUpdateStatus(ClientLfgUpdateStatus* msg) {
    LFGInfo info;

    if (*reinterpret_cast<uint32_t*>(&msg->unk_0034) == 5) {
        if (CGLookingForGroup::m_searchJoinedID && (!msg->unk_0014.Count() || CGLookingForGroup::m_searchJoinedID == static_cast<int32_t>(msg->unk_0014[0])) && !msg->unk_0012) {
            CGLookingForGroup::m_prevSearchResults.Clear();
            CGLookingForGroup::m_searchJoinedID = 0;
            CGLookingForGroup::m_searchResults.Clear();
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
        }

        return true;
    }

    info.m_joined = msg->unk_0012 != 0;

    if (msg->unk_0012) {
        info.m_ticket = *reinterpret_cast<JamClientRideTicket*>(msg->unk_0028);
        info.unk_0001 = msg->unk_0011 != 0;
        info.m_queued = msg->unk_0010 != 0;
        info.unk_0004 = 0;
        info.unk_0005 = 0;
        info.m_roles[0] = msg->unk_0142[0];
        info.m_roles[1] = msg->unk_0142[1];
        info.m_roles[2] = msg->unk_0142[2];
        info.m_numDungeons = msg->unk_0014.Count();

        for (uint32_t i = 0; i < msg->unk_0014.Count(); i++) {
            info.AddDungeon(msg->unk_0014[i]);
        }

        SStrCopy(info.m_comment, msg->unk_0042, sizeof(info.m_comment));
    }

    if (CGLookingForGroup::m_proposal.m_id && (!msg->unk_0012 || msg->unk_0010) && (CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers)) {
        CGLookingForGroup::m_proposal.Clear();

        if (msg->unk_0012) {
            CGGameUI::DisplayError(ERR_LFG_PROPOSAL_FAILED);
        }

        if (msg->unk_0040 == 11 || msg->unk_0040 == 1 || msg->unk_0040 == 17) {
            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_SUCCEEDED, NULL);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_PROPOSAL_FAILED, NULL);
        }
    }

    LFGInfo* target = msg->unk_0041 ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

    if (!target->Equals(info)) {
        bool left;

        if (info.unk_0001) {
            left = false;

            if (!target->unk_0001 && info.m_numDungeons) {
                SI3::PlayUISound(8462, NULL, true);
                CGGameUI::DisplayError(CGLookingForGroup::HasRaidDungeon(&info) ? ERR_LFG_JOINED_LIST : ERR_LFG_JOINED_QUEUE);
            } else if (!info.m_queued && target->m_queued) {
                left = true;
            }
        } else {
            left = target->unk_0001 || (!info.m_queued && target->m_queued);
        }

        if (left) {
            switch (msg->unk_0040) {
                case 8:
                case 16:
                    SI3::PlayUISound(17341, NULL, true);
                    CGGameUI::DisplayError(CGLookingForGroup::HasRaidDungeon(target) ? ERR_LFG_LEFT_LIST : ERR_LFG_LEFT_QUEUE);
                    break;

                case 43:
                    CGGameUI::DisplayError(ERR_PARTY_ROLE_NOT_AVAILABLE);
                    break;

                case 45:
                    CGGameUI::DisplayError(ERR_JOIN_LFG_OBJECT_FAILED);
                    break;

                default:
                    break;
            }
        }

        target->CopyFrom(info);
        bool inParty = CGPartyInfo::m_members[0] || CGPartyInfo::m_onePersonParty || CGRaidInfo::m_numMembers;

        if (inParty == (msg->unk_0041 != 0)) {
            if (!info.m_queued) {
                CGLookingForGroup::m_queueStatus.Clear();
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_LFG_UPDATE, NULL);
        }
    }

    return true;
}

// OFFSET: 0x0098AA40
static bool OnLfgSearchResult(ClientLfgSearchResult* msg) {
    bool changed = false;
    s_searchType = msg->unk_006C;
    s_searchID = msg->unk_0024;
    int32_t lfgID = (s_searchID & 0xFFFFFF) | (s_searchType << 24);

    if (lfgID != CGLookingForGroup::m_searchJoinedID) {
        return true;
    }

    LFGLIST* list = &CGLookingForGroup::m_prevSearchResults;

    if (static_cast<uint32_t>(lfgID) != list->m_lfgID) {
        changed = true;
    }

    if (!msg->unk_0028) {
        list->Clear();
    } else {
        for (uint32_t i = 0; i < msg->m_removed.Count(); i++) {
            uint64_t guid = *reinterpret_cast<uint64_t*>(msg->m_removed.m_data[i].m_guid);

            if ((guid & 0xF000000000000000ull) || !(guid & 0xF07FFFFFFFFFFFFFull)) {
                LFGListParty* party = list->m_parties.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

                if (party) {
                    list->DeleteParty(party);
                }
            } else {
                LFGListPlayer* player = list->m_players.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

                if (player) {
                    list->RemovePlayer(player);
                    player->~LFGListPlayer();
                    SMemFree(player, "delete", -1, 0);
                }
            }
        }
    }

    list->m_lfgID = lfgID;
    list->m_totalParties = msg->unk_0058;

    for (uint32_t i = 0; i < msg->m_parties.Count(); i++) {
        JamLfgSearchResultParty* source = &msg->m_parties.m_data[i];
        uint64_t guid = *reinterpret_cast<uint64_t*>(source->m_guid);
        LFGListParty* party = list->m_parties.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (!party) {
            party = new (__FILE__, __LINE__) LFGListParty();
            list->m_parties.Insert(party, static_cast<uint32_t>(guid), CHashKeyGUID(guid));
        }

        LFGHandler::ReadGroupInfo(source, party);

        if (party->m_index < 0) {
            uint32_t index = list->m_partyList.Count();
            list->m_partyList.SetCount(index + 1);
            list->m_partyList.m_data[index] = party;
            party->m_index = index;
        }
    }

    list->m_totalPlayers = msg->unk_0010;

    for (uint32_t i = 0; i < msg->m_players.Count(); i++) {
        JamLfgSearchResultPlayer* source = &msg->m_players.m_data[i];
        uint64_t guid = *reinterpret_cast<uint64_t*>(source->m_guid);
        LFGListPlayer* player = list->m_players.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

        if (!player) {
            player = new (__FILE__, __LINE__) LFGListPlayer();
            list->m_players.Insert(player, static_cast<uint32_t>(guid), CHashKeyGUID(guid));

            if (!g_nameDBCache.GetRecord(*reinterpret_cast<uint64_t*>(source->m_guid), *reinterpret_cast<WOWGUID*>(source->m_guid), LFGNameCacheCallback, NULL, false)) {
                list->m_pendingNames++;
            }
        }

        LFGHandler::ReadPlayerInfo(source, list, player);
        list->AddPlayer(player);
    }

    if (changed) {
        CGLookingForGroup::m_searchResults.Copy(*list);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
    }

    return true;
}

// OFFSET: 0x0098B200/0x140708F30
void CGLookingForGroup::InitializeGame() {
    CGLookingForGroup::m_prevSearchResults.Clear();
    CGLookingForGroup::m_searchResults.Clear();
    CGLookingForGroup::m_localInfo.Reset();
    CGLookingForGroup::m_playerInfo.Reset();
    CGLookingForGroup::m_partyInfo.Reset();
    CGLookingForGroup::m_playerLockInfo.m_valid = 0;
    CGLookingForGroup::m_playerLockInfo.m_guid.m_guid = 0;
    CGLookingForGroup::m_playerLockInfo.ClearDungeons();
    CGLookingForGroup::m_partyLockInfo.Clear();
    CGLookingForGroup::m_queueStatus.m_lfgID = 0;
    CGLookingForGroup::m_queueStatus.m_averageWait = 0;
    CGLookingForGroup::m_queueStatus.m_tankWait = 0;
    CGLookingForGroup::m_queueStatus.m_healerWait = 0;
    CGLookingForGroup::m_queueStatus.m_damageWait = 0;
    CGLookingForGroup::m_queueStatus.m_myWait = 0;
    CGLookingForGroup::m_queueStatus.m_tanksNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_healersNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_damageNeeded = 0;
    CGLookingForGroup::m_queueStatus.m_statusTime = 0;
    CGLookingForGroup::m_queueStatus.m_queuedTime = 0;
    CGLookingForGroup::m_reward.m_randomSlot = 0;
    CGLookingForGroup::m_reward.m_actualSlot = 0;
    CGLookingForGroup::m_reward.m_baseMoney = 0;
    CGLookingForGroup::m_reward.m_baseXP = 0;
    CGLookingForGroup::m_completionRewards.Clear();
    CGLookingForGroup::m_unkBEAA10 = 0;
    CGLookingForGroup::m_searchJoinedID = 0;
    CGLookingForGroup::m_randomDungeonBestChoice = 0;
    CGLookingForGroup::m_randomDungeons.Clear();
    CGLookingForGroup::m_lfdEntries.m_count = 0;
    CGLookingForGroup::m_lfrEntries.m_count = 0;
    CGLookingForGroup::m_rfEntries.m_count = 0;
    CGLookingForGroup::UpdateLFDEntries();
    CGLookingForGroup::UpdateLFREntries();
    CGLookingForGroup::UpdateRFEntries();
    CGLookingForGroup::m_proposal.Clear();
    CGLookingForGroup::m_roleCheckState = 0;
    CGLookingForGroup::m_roleCheckSlots.m_count = 0;
    CGLookingForGroup::m_roleCheckMembers.m_count = 0;

    for (uint32_t i = 0; i < 7; i++) {
        CGLookingForGroup::m_unkBEA820[i].m_id = i;
        CGLookingForGroup::m_unkBEA820[i].m_order = 0;
    }

    ClientServices::SetMessageHandler(SMSG_LFG_DISABLED, Packet_SMSG_LFG_DISABLED, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_ROLE_CHECK_UPDATE, Packet_SMSG_LFG_ROLE_CHECK_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_ROLE_CHOSEN, Packet_SMSG_LFG_ROLE_CHOSEN, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_BOOT_PROPOSAL_UPDATE, Packet_SMSG_LFG_BOOT_PROPOSAL_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_PLAYER_INFO, Packet_SMSG_LFG_PLAYER_INFO, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_PARTY_INFO, Packet_SMSG_LFG_PARTY_INFO, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_PLAYER_REWARD, Packet_SMSG_LFG_PLAYER_REWARD, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_TELEPORT_DENIED, Packet_SMSG_LFG_TELEPORT_DENIED, NULL);
    ClientServices::SetMessageHandler(SMSG_LFG_OFFER_CONTINUE, Packet_SMSG_LFG_OFFER_CONTINUE, NULL);
    ClientServices::SetMessageHandler(SMSG_OPEN_LFG_DUNGEON_FINDER, Packet_SMSG_LFG_OPEN_FROM_GOSSIP, NULL);
    ClientLfgUpdateStatus::s_handler = OnLfgUpdateStatus;
    ClientLfgJoinResult::s_handler = OnLfgJoinResult;
    ClientLfgProposalUpdate::s_handler = OnLfgProposalUpdate;
    ClientLfgQueueStatus::s_handler = OnLfgQueueStatus;
    CGLookingForGroup::m_randomSlotCount = 0;
    CGLookingForGroup::m_unkACD660 = 1;
    ClientLfgSearchResult::s_handler = OnLfgSearchResult;
    ClientUpdateDungeonEncounterForLoot::s_handler = OnUpdateDungeonEncounterForLoot;
    CGLookingForGroup::m_randomSlotMinLevel = -1;
    CGLookingForGroup::m_randomSlotMaxLevel = -1;
}

// OFFSET: 0x00984450
LFGLockInfo* LFGPartyLockInfo::FindMember(WOWGUID guid) {
    for (LFGLockInfo* member = this->m_members.Head(); !this->m_members.IsEnd(member); ) {
        LFGLockInfo* next = member->RawNext();

        if (member->m_guid == guid) {
            return member;
        }

        member = next;
    }

    return NULL;
}

// OFFSET: 0x00989C80
LFGLockInfo* LFGPartyLockInfo::AddMember(WOWGUID guid) {
    LFGLockInfo* member = this->FindMember(guid);

    if (member) {
        return member;
    }

    member = new (__FILE__, __LINE__) LFGLockInfo();
    member->m_guid = guid;
    this->m_members.LinkToTail(member);
    return member;
}

// OFFSET: 0x00989F00
void CGLookingForGroup::ShutdownGame() {
    CGLookingForGroup::m_prevSearchResults.Clear();
    CGLookingForGroup::m_searchResults.Clear();
    CGLookingForGroup::m_localInfo.Reset();
    CGLookingForGroup::m_playerInfo.Reset();
    CGLookingForGroup::m_partyInfo.Reset();
    CGLookingForGroup::m_playerLockInfo.m_valid = 0;
    CGLookingForGroup::m_playerLockInfo.m_guid.m_guid = 0;
    CGLookingForGroup::m_playerLockInfo.ClearDungeons();
    CGLookingForGroup::m_partyLockInfo.Clear();
    CGLookingForGroup::m_queueStatus.Clear();
    CGLookingForGroup::m_reward.m_randomSlot = 0;
    CGLookingForGroup::m_reward.m_actualSlot = 0;
    CGLookingForGroup::m_reward.m_baseMoney = 0;
    CGLookingForGroup::m_reward.m_baseXP = 0;
    CGLookingForGroup::m_completionRewards.Clear();
    ClientServices::ClearMessageHandler(SMSG_LFG_DISABLED);
    ClientServices::ClearMessageHandler(SMSG_LFG_ROLE_CHECK_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_LFG_ROLE_CHOSEN);
    ClientServices::ClearMessageHandler(SMSG_LFG_BOOT_PROPOSAL_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_LFG_PLAYER_INFO);
    ClientServices::ClearMessageHandler(SMSG_LFG_PARTY_INFO);
    ClientServices::ClearMessageHandler(SMSG_LFG_PLAYER_REWARD);
    ClientServices::ClearMessageHandler(SMSG_LFG_TELEPORT_DENIED);
    ClientServices::ClearMessageHandler(SMSG_LFG_OFFER_CONTINUE);
    ClientServices::ClearMessageHandler(SMSG_OPEN_LFG_DUNGEON_FINDER);
    ClientLfgProposalUpdate::s_handler = NULL;
    ClientLfgJoinResult::s_handler = NULL;
    ClientLfgUpdateStatus::s_handler = NULL;
    ClientLfgSearchResult::s_handler = NULL;
    ClientLfgQueueStatus::s_handler = NULL;
    ClientUpdateDungeonEncounterForLoot::s_handler = NULL;
}

// OFFSET: 0x00986C40
void CGLookingForGroup::ParseBlackList(CDataStore* msg, LFGLockInfo* lockInfo) {
    uint32_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t slot;
        msg->Get(slot);
        LFGDungeonEntry* entry = lockInfo->AddDungeon(slot);
        entry->m_locked = 1;
        uint32_t reason;
        msg->Get(reason);
        msg->Get(reinterpret_cast<uint32_t&>(entry->unk_004C));
        msg->Get(reinterpret_cast<uint32_t&>(entry->unk_0050));
        entry->m_lockReason = reason;
    }
}

// OFFSET: 0x009853E0
bool CGLookingForGroup::IsRandomDungeonUnlocked(const LfgDungeonsRec* rec) {
    if (!(rec->m_flags & 0x4)) {
        return false;
    }

    uint32_t slot = (rec->m_typeID << 24) | (rec->m_id & 0xFFFFFF);

    for (LFGDungeonEntry* entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
        if (entry->m_slot == slot) {
            return !entry->m_locked;
        }
    }

    return false;
}

// OFFSET: 0x009879C0
void CGLookingForGroup::UpdateRandomDungeons() {
    int32_t numRecords = g_lfgDungeonsDB.m_numRecords;
    CGLookingForGroup::m_randomDungeons.SetCount(0);

    for (int32_t i = 0; i < numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);
        uint32_t slot = (rec->m_typeID << 24) | (rec->m_id & 0xFFFFFF);

        if (rec->m_typeID == 6 || CGLookingForGroup::IsRandomDungeonUnlocked(rec)) {
            CGLookingForGroup::m_randomDungeons.Add(1, &slot);
        }
    }

    CGLookingForGroup::UpdateRandomDungeonBestChoice();
}

// OFFSET: 0x009878A0/0x1407035F0
void CGLookingForGroup::SetLFGDungeon(uint32_t slot) {
    if ((slot >> 24) == 1) {
        for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
            if (static_cast<uint8_t>(dungeon->m_slot >> 24) != 1) {
                CGLookingForGroup::ClearDungeonList();
                break;
            }
        }
    } else if ((slot >> 24) != 2 || CGPartyInfo::NumMembers()) {
        CGLookingForGroup::ClearDungeonList();
    } else {
        for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
            if (static_cast<uint8_t>(dungeon->m_slot >> 24) != 2) {
                CGLookingForGroup::ClearDungeonList();
                break;
            }
        }
    }

    for (LFGDungeonEntry* dungeon = CGLookingForGroup::m_localInfo.m_dungeons.Head(); !CGLookingForGroup::m_localInfo.m_dungeons.IsEnd(dungeon); dungeon = dungeon->RawNext()) {
        if (dungeon->m_slot == slot) {
            return;
        }
    }

    CGLookingForGroup::m_localInfo.AddDungeon(slot);
    CGLookingForGroup::m_localInfo.unk_0003 = 1;
}

// OFFSET: 0x00986CC0/0x140702300
bool CGLookingForGroup::IsSlotValidForAll(uint32_t slot) {
    if (!CGLookingForGroup::IsSlotValidForPlayer(slot)) {
        return false;
    }

    if ((slot & 0xFF000000) != 0x2000000 && CGLookingForGroup::m_partyLockInfo.m_valid) {
        for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); !CGLookingForGroup::m_partyLockInfo.m_members.IsEnd(member); member = member->RawNext()) {
            for (LFGDungeonEntry* entry = member->m_dungeons.Head(); !member->m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
                if (entry->m_slot == slot) {
                    if (entry->m_locked) {
                        return false;
                    }

                    break;
                }
            }
        }
    }

    return true;
}

// OFFSET: 0x00985300
bool CGLookingForGroup::IsSlotValidForPlayer(uint32_t slot) {
    if (CGLookingForGroup::m_playerLockInfo.m_valid) {
        for (LFGDungeonEntry* entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); !CGLookingForGroup::m_playerLockInfo.m_dungeons.IsEnd(entry); entry = entry->RawNext()) {
            if (entry->m_slot == slot) {
                if (entry->m_locked) {
                    return false;
                }

                break;
            }
        }
    }

    return true;
}

// OFFSET: 0x00986D50/0x1407023C0
static bool IsBetterRandomDungeon(const LfgDungeonsRec* candidate, const LfgDungeonsRec* best) {
    if (!candidate || candidate->m_typeID != 6 || !CGLookingForGroup::IsSlotValidForAll(candidate->m_id & 0xFFFFFF | 0x6000000)) {
        return false;
    }

    if (!best) {
        return true;
    }

    if (candidate->m_expansionLevel != best->m_expansionLevel) {
        return candidate->m_expansionLevel > best->m_expansionLevel;
    }

    LfgDungeonExpansionRec* bestExpansion = GetLfgDungeonExpansionRec(best->m_id, ClientServices::Connection()->m_accountExpansion);
    LfgDungeonExpansionRec* candidateExpansion = GetLfgDungeonExpansionRec(candidate->m_id, ClientServices::Connection()->m_accountExpansion);
    int32_t bestLevel = (bestExpansion ? bestExpansion->m_targetLevelMin + bestExpansion->m_targetLevelMax : best->m_targetLevelMin + best->m_targetLevelMax) / 2;
    int32_t candidateLevel = (candidateExpansion ? candidateExpansion->m_targetLevelMin + candidateExpansion->m_targetLevelMax : candidate->m_targetLevelMin + candidate->m_targetLevelMax) / 2;

    if (candidateLevel != bestLevel) {
        return candidateLevel > bestLevel;
    }

    int32_t bestMin = bestExpansion ? bestExpansion->m_hardLevelMin : best->m_minLevel;
    int32_t candidateMin = candidateExpansion ? candidateExpansion->m_hardLevelMin : candidate->m_minLevel;

    if (candidateMin != bestMin) {
        return candidateMin > bestMin;
    }

    return candidate->m_orderIndex > best->m_orderIndex;
}

// OFFSET: 0x00986E50
bool CGLookingForGroup::UpdateRandomDungeonBestChoice() {
    LfgDungeonsRec* best = NULL;
    uint32_t count = CGLookingForGroup::m_randomDungeons.m_count;

    for (uint32_t i = 0; i < count; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_randomDungeons.m_data[i] & 0xFFFFFF);

        if (IsBetterRandomDungeon(rec, best)) {
            best = rec;
        }
    }

    uint32_t previous = CGLookingForGroup::m_randomDungeonBestChoice;
    if (best) {
        CGLookingForGroup::m_randomDungeonBestChoice = best->m_id & 0xFFFFFF | (best->m_typeID << 24);
        return previous != CGLookingForGroup::m_randomDungeonBestChoice;
    }

    CGLookingForGroup::m_randomDungeonBestChoice = 0;
    return previous != 0;
}

// OFFSET: 0x00980980/0x1406FAD80
void CGLookingForGroup::UpdateRandomSlots(int32_t minLevel, int32_t maxLevel) {
    if (!CGLookingForGroup::m_randomSlotCount) {
        int32_t numRecords = g_lfgDungeonsDB.m_numRecords;
        uint32_t count = 0;

        for (int32_t i = 0; i < g_lfgDungeonsDB.m_numRecords; i++) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

            if (rec->m_typeID == 6 && !(rec->m_flags & 0x4)) {
                count++;
            }
        }

        CGLookingForGroup::m_randomSlotCount = count;
        LFGRandomSlot* slot = CGLookingForGroup::m_randomSlots;

        for (int32_t i = 0; i < numRecords; i++) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

            if (rec->m_typeID == 6 && !(rec->m_flags & 0x4)) {
                slot->m_list.m_count = 0;
                slot->m_randomID = rec->m_id;
                slot++;
            }
        }
    } else {
        for (uint32_t i = 0; i < CGLookingForGroup::m_randomSlotCount; i++) {
            CGLookingForGroup::m_randomSlots[i].m_list.m_count = 0;
        }
    }

    int32_t expansion = ClientServices::Connection()->m_accountExpansion;

    for (uint32_t i = 0; i < CGLookingForGroup::m_randomSlotCount; i++) {
        LfgDungeonsRec* random = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_randomSlots[i].m_randomID);

        for (int32_t j = 0; j < g_lfgDungeonsDB.m_numRecords; j++) {
            LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(j);

            if (!rec->IsInRandomGroup(random->m_id, expansion)) {
                continue;
            }

            int32_t subtype = rec->m_typeID == 1 ? (rec->IsRaid() ? 3 : (rec->IsHeroic() ? 2 : 1)) : 0;

            if (!(rec->m_flags & 0x2) || rec->m_typeID != 1 || (rec->m_flags & 0x4) || (subtype != 1 && subtype != 2)) {
                continue;
            }

            LfgDungeonExpansionRec* levels = GetLfgDungeonExpansionRec(rec->m_id, expansion);

            if (levels) {
                if (levels->m_targetLevelMin && minLevel < levels->m_targetLevelMin) {
                    continue;
                }

                if (levels->m_targetLevelMax && minLevel > levels->m_targetLevelMax) {
                    continue;
                }

                if (levels->m_hardLevelMax && maxLevel > levels->m_hardLevelMax) {
                    continue;
                }
            } else {
                if (rec->m_targetLevelMin && minLevel < rec->m_targetLevelMin) {
                    continue;
                }

                if (rec->m_targetLevelMax && minLevel > rec->m_targetLevelMax) {
                    continue;
                }

                if (rec->m_maxLevel && maxLevel > rec->m_maxLevel) {
                    continue;
                }
            }

            LFGRandomSlotDungeons* list = &CGLookingForGroup::m_randomSlots[i].m_list;
            list->m_dungeons[list->m_count++] = rec->m_id;
        }
    }

    CGLookingForGroup::m_randomSlotMinLevel = minLevel;
    CGLookingForGroup::m_randomSlotMaxLevel = maxLevel;
}

// OFFSET: 0x00983680/0x1406FE420
LFGRandomSlotDungeons* CGLookingForGroup::GetRandomSlot(int32_t dungeonID) {
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(dungeonID);

    CGPlayer_C* player = rec && rec->m_typeID == 6 ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) : NULL;

    if (!player) {
        return NULL;
    }

    int32_t minLevel = player->m_unit->m_unitFieldLevel;
    int32_t maxLevel = minLevel;

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        WOWGUID guid = CGPartyInfo::GetMember(i);

        if (!guid) {
            continue;
        }

        int32_t level;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            level = unit->m_unit->m_unitFieldLevel;
        } else {
            PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);

            if (!state) {
                continue;
            }

            level = state->m_level;
        }

        if (level != -1) {
            if (minLevel >= level) {
                minLevel = level;
            }

            if (maxLevel <= level) {
                maxLevel = level;
            }
        }
    }

    if (minLevel != CGLookingForGroup::m_randomSlotMinLevel || maxLevel != CGLookingForGroup::m_randomSlotMaxLevel) {
        CGLookingForGroup::UpdateRandomSlots(minLevel, maxLevel);
    }

    for (uint32_t i = 0; i < CGLookingForGroup::m_randomSlotCount; i++) {
        if (CGLookingForGroup::m_randomSlots[i].m_randomID == rec->m_id) {
            return &CGLookingForGroup::m_randomSlots[i].m_list;
        }
    }

    return NULL;
}

// OFFSET: 0x00987670/0x140703320
static bool IsBetterRFDungeon(const LfgDungeonsRec* candidate, const LfgDungeonsRec* best) {
    if (!candidate || !CGLookingForGroup::IsSlotValidForAll(candidate->m_id & 0xFFFFFF | (candidate->m_typeID << 24))) {
        return false;
    }

    if (!best) {
        return true;
    }

    if (candidate->m_expansionLevel != best->m_expansionLevel) {
        return candidate->m_expansionLevel > best->m_expansionLevel;
    }

    LfgDungeonExpansionRec* bestExpansion = GetLfgDungeonExpansionRec(best->m_id, ClientServices::Connection()->m_accountExpansion);
    LfgDungeonExpansionRec* candidateExpansion = GetLfgDungeonExpansionRec(candidate->m_id, ClientServices::Connection()->m_accountExpansion);
    int32_t bestLevel = (bestExpansion ? bestExpansion->m_targetLevelMin + bestExpansion->m_targetLevelMax : best->m_targetLevelMin + best->m_targetLevelMax) / 2;
    int32_t candidateLevel = (candidateExpansion ? candidateExpansion->m_targetLevelMin + candidateExpansion->m_targetLevelMax : candidate->m_targetLevelMin + candidate->m_targetLevelMax) / 2;

    if (candidateLevel != bestLevel) {
        return candidateLevel > bestLevel;
    }

    int32_t bestMin = bestExpansion ? bestExpansion->m_hardLevelMin : best->m_minLevel;
    int32_t candidateMin = candidateExpansion ? candidateExpansion->m_hardLevelMin : candidate->m_minLevel;

    if (candidateMin != bestMin) {
        return candidateMin > bestMin;
    }

    return candidate->m_orderIndex > best->m_orderIndex;
}

// OFFSET: 0x00987770/0x140703480
int32_t CGLookingForGroup::GetBestRFChoice() {
    LfgDungeonsRec* best = NULL;

    for (uint32_t i = 0; i < CGLookingForGroup::m_rfEntries.m_count; i++) {
        CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_rfEntries.m_data[i];
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(entry.m_id);

        if (IsBetterRFDungeon(rec, best)) {
            best = rec;
        }
    }

    if (best) {
        return best->m_id;
    }

    return 0;
}

// INLINED
void CGLookingForGroup::RequestLFGPartyInfo() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LFG_PARTY_LOCK_INFO_REQUEST));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00980D90/0x1406FB240
void CGLookingForGroup::SetLFGComment(const char* comment) {
    char scrubbed[256];
    ScrubString(comment, scrubbed, 64, sizeof(scrubbed), 1);
    SStrCopy(CGLookingForGroup::m_localInfo.m_comment, scrubbed, sizeof(CGLookingForGroup::m_localInfo.m_comment));

    if (!CGLookingForGroup::m_localInfo.m_joined) {
        CGLookingForGroup::m_localInfo.unk_0003 = 1;
        return;
    }

    static ThrottleObject s_throttle(2, 2);

    if (s_throttle.IncrementAndCheck()) {
        UserClientLfgSetComment request;
        SStrCopy(request.unk_0010, CGLookingForGroup::m_localInfo.m_comment, sizeof(request.unk_0010));
        ClientServices::Send(&request);
    }
}

// OFFSET: 0x009886E0/0x140704C90
void CGLookingForGroup::LeaveWorld() {
    ClientSetDfFastLaunchResult::s_handler = NULL;
    ClientLfgSlotInvalid::s_handler = NULL;
    CGLookingForGroup::LeaveLFGSearch();
}

// OFFSET: 0x00987E90/0x140703E50
void CGLookingForGroup::LeaveLFGSearch() {
    if (CGLookingForGroup::m_searchJoinedID) {
        UserClientLfgLfrLeave request;
        request.unk_0010 = CGLookingForGroup::m_searchJoinedID;
        ClientServices::Send(&request);
        CGLookingForGroup::m_prevSearchResults.Clear();
        CGLookingForGroup::m_prevSearchResults.m_time = FrameTime::GetCurTimeMs();
        CGLookingForGroup::m_searchJoinedID = 0;
        CGLookingForGroup::m_searchResults.Clear();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
    }
}

// OFFSET: 0x0097EFB0
void LFGNameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGLookingForGroup::m_prevSearchResults.m_pendingNames || !--CGLookingForGroup::m_prevSearchResults.m_pendingNames) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_LFG_LIST, NULL);
    }
}

// OFFSET: 0x0097EFE0
int32_t Packet_SMSG_LFG_OPEN_FROM_GOSSIP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t slot;
    msg->Get(slot);
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_OPEN_FROM_GOSSIP, "%d", slot);
    return 1;
}
