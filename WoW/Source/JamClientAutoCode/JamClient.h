#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/WowServices/WDataStore.h"

struct JamClientCharacterListItem {
    /* 0000 */ uint32_t m_displayID;
    /* 0004 */ uint32_t m_auraVisualID;
    /* 0008 */ uint8_t m_inventoryType;
    /* 0009 */ uint8_t unk_0009[3];
};

struct JamClientCharacterListEntry {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ char m_name[48];
    /* 0038 */ uint8_t m_listOrder;
    /* 0039 */ uint8_t m_raceID;
    /* 003A */ uint8_t m_classID;
    /* 003B */ uint8_t m_sexID;
    /* 003C */ uint8_t m_skinColorID;
    /* 003D */ uint8_t m_faceID;
    /* 003E */ uint8_t m_hairStyleID;
    /* 003F */ uint8_t m_hairColorID;
    /* 0040 */ uint8_t m_facialHairStyleID;
    /* 0041 */ uint8_t m_experienceLevel;
    /* 0042 */ uint8_t unk_0042[2];
    /* 0044 */ uint32_t m_zoneID;
    /* 0048 */ uint32_t m_mapID;
    /* 004C */ C3Vector m_position;
    /* 0058 */ uint8_t m_guildGUID[8];
    /* 0060 */ uint32_t m_flags;
    /* 0064 */ uint32_t m_customizeFlags;
    /* 0068 */ bool m_firstLogin;
    /* 0069 */ uint8_t unk_0069[3];
    /* 006C */ uint32_t m_petDisplayInfoID;
    /* 0070 */ uint32_t m_petExperienceLevel;
    /* 0074 */ uint32_t m_petCreatureFamilyID;
    /* 0078 */ JamClientCharacterListItem m_items[23];
    /* 018C */ uint8_t unk_018C[4];
};

struct JamClientRestrictedFactionChangeRule {
    /* 0000 */ uint32_t m_mask;
    /* 0004 */ uint8_t m_raceID;
    /* 0005 */ uint8_t unk_0005[3];
};

struct JamGuildMemberProfessionData {
    /* 0000 */ int32_t m_dbID;
    /* 0004 */ int32_t m_rank;
    /* 0008 */ int32_t m_step;
};

struct ClientGuildRosterMemberData {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint64_t m_weeklyXP;
    /* 0010 */ uint64_t m_totalXP;
    /* 0018 */ int32_t m_rankID;
    /* 001C */ int32_t m_areaID;
    /* 0020 */ int32_t m_personalAchievementPoints;
    /* 0024 */ int32_t m_guildReputation;
    /* 0028 */ int32_t m_guildRepToCap;
    /* 002C */ int32_t m_bgRating;
    /* 0030 */ float m_lastSave;
    /* 0034 */ char m_name[48];
    /* 0064 */ char m_note[128];
    /* 00E4 */ char m_officerNote[128];
    /* 0164 */ uint8_t m_status;
    /* 0165 */ uint8_t m_level;
    /* 0166 */ uint8_t m_classID;
    /* 0167 */ uint8_t m_gender;
    /* 0168 */ bool m_authenticated;
    /* 0169 */ bool m_sorEligible;
    /* 016A */ uint8_t unk_016A[2];
    /* 016C */ JamGuildMemberProfessionData m_profession[2];
    /* 0184 */ uint32_t unk_0184;
};

struct JamGuildRewardItem {
    /* 0000 */ uint64_t m_cost;
    /* 0008 */ int32_t m_itemID;
    /* 000C */ int32_t m_achievementRequired;
    /* 0010 */ uint32_t m_raceMask;
    /* 0014 */ int32_t m_minGuildLevel;
    /* 0018 */ int32_t m_minGuildRep;
    /* 001C */ uint32_t unk_001C;

    ~JamGuildRewardItem();
    JamGuildRewardItem& operator=(const JamGuildRewardItem& other);
};

struct JamGuildNewsEvent {
    /* 0000 */ int32_t m_id;
    /* 0004 */ WowTime m_date;
    /* 0024 */ int32_t m_type;
    /* 0028 */ uint32_t m_flags;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint64_t m_memberGUID;
    /* 0038 */ TSGrowableArray<uint64_t> m_memberList;
    /* 0048 */ int32_t m_data[2];

    JamGuildNewsEvent();
    JamGuildNewsEvent(const JamGuildNewsEvent& other);
    ~JamGuildNewsEvent();
    JamGuildNewsEvent& operator=(const JamGuildNewsEvent& other);
};

struct JamCliGuildRankData {
    /* 0000 */ int32_t m_rankID;
    /* 0004 */ int32_t m_rankOrder;
    /* 0008 */ char m_rankName[64];
    /* 0048 */ uint32_t m_flags;
    /* 004C */ int32_t m_withdrawGoldLimit;
    /* 0050 */ uint32_t m_tabFlags[8];
    /* 0070 */ int32_t m_tabWithdrawItemLimit[8];
};

struct JamGuildRankTabPermissions {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ int32_t m_withdrawItemLimit;
};

struct JamGuildEventLogEntry {
    /* 0000 */ uint8_t m_playerGUID[8];
    /* 0008 */ uint8_t m_otherGUID[8];
    /* 0010 */ uint32_t m_timeOffset;
    /* 0014 */ uint8_t m_type;
    /* 0015 */ uint8_t m_rankID;
    /* 0016 */ uint8_t unk_0016[2];
};

struct JamGuildBankSocketEnchant {
    /* 0000 */ int32_t m_socketIndex;
    /* 0004 */ int32_t m_socketEnchantID;
};

struct JamGuildBankItemInfo {
    /* 0000 */ int32_t m_slot;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_randomPropertiesSeed;
    /* 000C */ int32_t m_randomPropertiesID;
    /* 0010 */ int32_t m_count;
    /* 0014 */ int32_t m_enchantmentID;
    /* 0018 */ int32_t m_charges;
    /* 001C */ TSGrowableArray<JamGuildBankSocketEnchant> m_socketEnchant;
    /* 002C */ int32_t m_reforgeID;
    /* 0030 */ int32_t m_onUseEnchantmentID;
    /* 0034 */ bool m_locked;
    /* 0038 */ uint32_t m_flags;
};

struct JamGuildBankTabInfo {
    /* 0000 */ int32_t m_tabIndex;
    /* 0004 */ char m_name[64];
    /* 0044 */ char m_icon[256];
};

struct JamGuildBankLogEntry {
    /* 0000 */ uint8_t m_playerGUID[8];
    /* 0008 */ uint32_t m_timeOffset;
    /* 000C */ int8_t m_entryType;
    /* 0010 */ bool m_hasMoney;
    /* 0018 */ uint64_t m_money;
    /* 0020 */ bool m_hasItemID;
    /* 0024 */ int32_t m_itemID;
    /* 0028 */ bool m_hasCount;
    /* 002C */ int32_t m_count;
    /* 0030 */ bool m_hasOtherTab;
    /* 0031 */ int8_t m_otherTab;
};

class ClientAuthResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAuthResponse* msg);

    /* 0010 */ bool m_hasSuccessInfo;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t m_billingTimeRemaining;
    /* 0018 */ uint32_t m_billingTimeRested;
    /* 001C */ uint32_t m_timeRemaining;
    /* 0020 */ uint8_t m_billingFlags;
    /* 0021 */ uint8_t m_accountExpansion;
    /* 0022 */ uint8_t m_activeExpansion;
    /* 0023 */ uint8_t unk_0023;
    /* 0024 */ bool m_hasWaitInfo;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ uint32_t m_waitCount;
    /* 002C */ bool m_freeCharacterMigration;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ uint8_t m_result;

    ClientAuthResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAuthResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWaitQueueUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWaitQueueUpdate* msg);

    /* 0010 */ uint32_t m_waitCount;
    /* 0014 */ bool m_freeCharacterMigration;

    ClientWaitQueueUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWaitQueueUpdate();
    bool Handle();
};

class ClientWaitQueueFinish : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWaitQueueFinish* msg);

    ClientWaitQueueFinish(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWaitQueueFinish();
    bool Handle();
};

class ClientEnumCharactersResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientEnumCharactersResult* msg);

    /* 0010 */ bool m_success;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ TSGrowableArray<JamClientCharacterListEntry> m_characters;
    /* 0024 */ TSGrowableArray<JamClientRestrictedFactionChangeRule> m_factionChangeRestrictions;

    ClientEnumCharactersResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientEnumCharactersResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGenerateRandomCharacterNameResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGenerateRandomCharacterNameResult* msg);

    /* 0010 */ bool m_success;
    /* 0011 */ char m_name[48];

    ClientGenerateRandomCharacterNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGenerateRandomCharacterNameResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

struct JamSummonRaidMemberValidateFailed {
    /* 0000 */ uint8_t m_memberGUID[8];
    /* 0008 */ int32_t m_reasonCode;
    /* 000C */ uint32_t unk_000C;
};

struct JamQuestCompletionNPC {
    /* 0000 */ int32_t m_questID;
    /* 0004 */ TSGrowableArray<int32_t> m_npcs;
};

struct JamGuildKnownRecipes {
    /* 0000 */ int32_t m_skillLineID;
    /* 0004 */ uint8_t m_skillLineBitArray[300];
};

struct JamItemRefundEntry {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint32_t m_count;
};

struct JamItemPurchaseRefundCost {
    /* 0000 */ uint32_t m_money;
    /* 0004 */ JamItemRefundEntry m_items[5];
    /* 002C */ JamItemRefundEntry m_currencies[5];
};

struct JamItemRefundInfo {
    /* 0000 */ uint8_t m_itemGUID[8];
    /* 0008 */ JamItemPurchaseRefundCost m_cost;
    /* 005C */ uint32_t m_flags;
    /* 0060 */ uint32_t m_refundTimeLeft;
};

struct JamItemRefundResult {
    /* 0000 */ bool m_hasCost;
    /* 0004 */ JamItemPurchaseRefundCost m_cost;
    /* 0058 */ uint8_t m_itemGUID[8];
    /* 0060 */ uint8_t m_result;
};

struct JamQuestGiverQuestComplete {
    /* 0000 */ int32_t m_questID;
    /* 0004 */ bool m_useQuestReward;
    /* 0008 */ int32_t m_talentReward;
    /* 000C */ int32_t m_moneyReward;
    /* 0010 */ int32_t m_xpReward;
    /* 0014 */ bool m_launchGossip;
    /* 0018 */ int32_t m_skillLineIDReward;
    /* 001C */ int32_t m_numSkillUpsReward;
};

struct JamThrottleState {
    /* 0000 */ uint32_t m_maxTries;
    /* 0004 */ uint32_t m_perMilliseconds;
    /* 0008 */ uint32_t m_tryCount;
    /* 000C */ uint32_t m_lastResetTimeBeforeNow;
};

struct JamCategoryCooldown {
    /* 0000 */ int32_t m_category;
    /* 0004 */ int32_t m_modCooldown;
};

struct JamVoidItem {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint8_t m_creator[8];
    /* 0010 */ int32_t m_itemID;
    /* 0014 */ uint32_t m_propertySeed;
    /* 0018 */ int32_t m_randomPropertiesId;
    /* 001C */ uint32_t m_slot;
};

struct JamServerPerfString {
    /* 0000 */ char m_text[128];
};

struct JamServerInfoField {
    /* 0000 */ char m_name[32];
    /* 0020 */ char m_value[128];
};

struct JamResearchHistory {
    /* 0000 */ int32_t m_projectID;
    /* 0004 */ int32_t m_firstCompleted;
    /* 0008 */ int32_t m_completionCount;

    JamResearchHistory();
    JamResearchHistory(const JamResearchHistory& source);
    ~JamResearchHistory();
};

struct JamCliCUFProfile {
    /* 0000 */ char m_name[128];
    /* 0080 */ uint16_t m_frameHeight;
    /* 0082 */ uint16_t m_frameWidth;
    /* 0084 */ uint8_t m_sortBy;
    /* 0085 */ uint8_t m_healthText;
    /* 0086 */ bool m_keepGroupsTogether;
    /* 0087 */ bool m_displayPets;
    /* 0088 */ bool m_displayMainTankAndAssist;
    /* 0089 */ bool m_displayHealPrediction;
    /* 008A */ bool m_displayAggroHighlight;
    /* 008B */ bool m_displayOnlyDispellableDebuffs;
    /* 008C */ bool m_displayPowerBar;
    /* 008D */ bool m_displayBorder;
    /* 008E */ bool m_useClassColors;
    /* 008F */ bool m_horizontalGroups;
    /* 0090 */ bool m_displayNonBossDebuffs;
    /* 0091 */ bool m_dynamicPosition;
    /* 0092 */ uint8_t m_topPoint;
    /* 0093 */ uint8_t m_bottomPoint;
    /* 0094 */ uint8_t m_leftPoint;
    /* 0095 */ uint8_t unk_0095;
    /* 0096 */ uint16_t m_topOffset;
    /* 0098 */ uint16_t m_bottomOffset;
    /* 009A */ uint16_t m_leftOffset;
    /* 009C */ bool m_locked;
    /* 009D */ bool m_shown;
    /* 009E */ bool m_autoActivate2Players;
    /* 009F */ bool m_autoActivate3Players;
    /* 00A0 */ bool m_autoActivate5Players;
    /* 00A1 */ bool m_autoActivate10Players;
    /* 00A2 */ bool m_autoActivate15Players;
    /* 00A3 */ bool m_autoActivate25Players;
    /* 00A4 */ bool m_autoActivate40Players;
    /* 00A5 */ bool m_autoActivateSpec1;
    /* 00A6 */ bool m_autoActivateSpec2;
    /* 00A7 */ bool m_autoActivatePvP;
    /* 00A8 */ bool m_autoActivatePvE;
    /* 00A9 */ uint8_t unk_00A9[3];
    /* 00AC */ uint32_t m_fieldMask;
    /* 00B0 */ uint32_t m_fieldMask2;

    JamCliCUFProfile();
};

struct JamCliStructAddonPrefix {
    /* 0000 */ char m_prefix[17];

    JamCliStructAddonPrefix();
    ~JamCliStructAddonPrefix();
};

struct JamCliSupportTicketChatLine {
    /* 0000 */ uint32_t m_timestamp;
    /* 0004 */ char m_text[3000];

    JamCliSupportTicketChatLine& operator=(const JamCliSupportTicketChatLine& source);
};

struct JamCliSupportTicketChatLog {
    /* 0000 */ TSGrowableArray<JamCliSupportTicketChatLine> m_lines;
    /* 0010 */ bool m_hasReportLineIndex;
    /* 0014 */ uint32_t m_reportLineIndex;

    JamCliSupportTicketChatLog();
    ~JamCliSupportTicketChatLog();
    void Put(CDataStore* msg) const;
};

struct JamCliSupportTicketHeader {
    /* 0000 */ uint32_t m_mapID;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ float m_facing;

    JamCliSupportTicketHeader();
    ~JamCliSupportTicketHeader();
    void Put(CDataStore* msg) const;
};

struct JamAccountInfoData {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
};

struct JamRideTicketString {
    /* 0000 */ char m_text[512];
};

struct JamGuildCriteriaProgress {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
};

struct JamEarnedAchievement {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ WowTime unk_0004;
};

struct JamCriteriaProgress {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ WowTime unk_001C;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
};

struct JamGuildAchievementMember {
    /* 0000 */ uint8_t unk_0000[8];
};

struct JamClientCommentatorPartyMember {
    /* 0000 */ char unk_0000[48];
    /* 0030 */ uint8_t unk_0030[8];
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ bool unk_0048;
    /* 0049 */ bool unk_0049;
};

struct JamLfgJoinResultLock {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
};

struct JamLfgJoinResultSlot {
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ TSGrowableArray<JamLfgJoinResultLock> unk_0008;
};

struct JamPvpLogDataPlayer {
    /* 0000 */ uint8_t m_playerGUID[8];
    /* 0008 */ uint32_t m_kills;
    /* 000C */ uint8_t m_faction;
    /* 000D */ bool unk_000D;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ bool m_hasHonor;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t m_honorKills;
    /* 0018 */ uint32_t m_deaths;
    /* 001C */ uint32_t m_contributionPoints;
    /* 0020 */ uint32_t m_damageDone;
    /* 0024 */ uint32_t m_healingDone;
    /* 0028 */ bool m_hasRating;
    /* 0029 */ uint8_t unk_0029[3];
    /* 002C */ uint32_t m_rating;
    /* 0030 */ bool m_hasRatingChange;
    /* 0031 */ uint8_t unk_0031[3];
    /* 0034 */ int32_t m_ratingChange;
    /* 0038 */ bool m_hasPreMatchMMR;
    /* 0039 */ uint8_t unk_0039[3];
    /* 003C */ uint32_t m_preMatchMMR;
    /* 0040 */ bool m_hasMmrChange;
    /* 0041 */ uint8_t unk_0041[3];
    /* 0044 */ int32_t m_mmrChange;
    /* 0048 */ TSGrowableArray<uint32_t> m_stats;
    /* 0058 */ uint32_t m_primaryTalentTree;
    /* 005C */ uint8_t unk_005C[4];
};

struct JamClientRideTicket {
    /* 0000 */ uint8_t m_requesterGuid[8];
    /* 0008 */ uint32_t m_id;
    /* 000C */ uint32_t m_type;
    /* 0010 */ uint32_t m_time;

    JamClientRideTicket();
    ~JamClientRideTicket();
    JamClientRideTicket& operator=(const JamClientRideTicket& source);
};

struct JamClientBattlefieldStatusHeader {
    /* 0000 */ JamClientRideTicket m_ticket;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t m_queueID[8];
    /* 0020 */ uint8_t m_rangeMin;
    /* 0021 */ uint8_t m_rangeMax;
    /* 0022 */ uint8_t m_teamSize;
    /* 0024 */ uint32_t m_instanceID;
    /* 0028 */ bool m_registeredMatch;
    /* 0029 */ uint8_t unk_0029[7];
};

struct JamBattlefieldPlayerPosition {
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ float unk_0008;
    /* 000C */ float unk_000C;
};

struct JamLfgProposalMember {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ bool unk_0004;
    /* 0005 */ bool unk_0005;
    /* 0006 */ bool unk_0006;
    /* 0007 */ bool unk_0007;
    /* 0008 */ bool unk_0008;
};

struct JamLFGuildSettings {
    /* 0000 */ uint32_t m_interests;
    /* 0004 */ uint32_t m_availability;
    /* 0008 */ uint32_t m_classRoles;
    /* 000C */ uint32_t m_level;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ char m_comment[1024];

    JamLFGuildSettings& operator=(const JamLFGuildSettings& source);
};

struct JamLFGuildPost {
    /* 0000 */ bool m_listed;
    /* 0004 */ JamLFGuildSettings m_settings;
};

struct LFGuildBrowseData {
    /* 0000 */ char m_name[96];
    /* 0060 */ uint8_t m_guildGUID[8];
    /* 0068 */ int32_t m_guildLevel;
    /* 006C */ int32_t m_numMembers;
    /* 0070 */ int32_t m_achievementPoints;
    /* 0074 */ uint32_t m_interests;
    /* 0078 */ uint32_t m_availability;
    /* 007C */ uint32_t m_classRoles;
    /* 0080 */ int32_t unk_0080;
    /* 0084 */ int32_t m_emblemStyle;
    /* 0088 */ int32_t m_emblemColor;
    /* 008C */ int32_t unk_008C;
    /* 0090 */ int32_t m_borderColor;
    /* 0094 */ int32_t m_background;
    /* 0098 */ char m_comment[1024];
    /* 0498 */ bool m_cached;
    /* 0499 */ bool m_membershipRequested;
    /* 049A */ uint8_t unk_049A[6];

    LFGuildBrowseData& operator=(const LFGuildBrowseData& source);
};

struct LFGuildRecruitData {
    /* 0000 */ uint8_t m_recruitGUID[8];
    /* 0008 */ char m_name[48];
    /* 0038 */ char m_comment[1024];
    /* 0438 */ int32_t m_classID;
    /* 043C */ int32_t unk_043C;
    /* 0440 */ int32_t m_level;
    /* 0444 */ uint32_t m_classRoles;
    /* 0448 */ uint32_t m_interests;
    /* 044C */ uint32_t m_availability;
    /* 0450 */ int32_t m_timeSince;
    /* 0454 */ int32_t m_timeLeft;

    LFGuildRecruitData& operator=(const LFGuildRecruitData& source);
};

struct LFGuildApplicationData {
    /* 0000 */ uint8_t m_guildGUID[8];
    /* 0008 */ char m_guildName[96];
    /* 0068 */ uint32_t m_classRoles;
    /* 006C */ uint32_t m_interests;
    /* 0070 */ uint32_t m_availability;
    /* 0074 */ int32_t m_timeSince;
    /* 0078 */ int32_t m_timeLeft;
    /* 007C */ char m_comment[1024];
    /* 047C */ uint8_t unk_047C[4];

    LFGuildApplicationData& operator=(const LFGuildApplicationData& source);
};

struct JamWeeklySpellUse {
    /* 0000 */ int32_t m_category;
    /* 0004 */ uint8_t m_uses;
    /* 0005 */ uint8_t unk_0005[3];

    JamWeeklySpellUse();
    JamWeeklySpellUse& operator=(const JamWeeklySpellUse& source);
};

struct HotfixNotify {
    /* 0000 */ uint32_t m_tableHash;
    /* 0004 */ int32_t m_recordID;
    /* 0008 */ uint32_t m_timestamp;
};

class ClientDBReply : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDBReply* msg);

    /* 0010 */ uint32_t m_tableHash;
    /* 0014 */ int32_t m_recordID;
    /* 0018 */ uint32_t m_timestamp;
    /* 001C */ WDataStore m_data;

    ClientDBReply(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDBReply();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientHotfixNotify : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientHotfixNotify* msg);

    /* 0010 */ TSGrowableArray<HotfixNotify> m_hotfixes;

    ClientHotfixNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientHotfixNotify();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientHotfixNotifyBlob : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientHotfixNotifyBlob* msg);

    /* 0010 */ uint32_t m_tableHash;
    /* 0014 */ int32_t m_recordID;
    /* 0018 */ uint32_t m_timestamp;

    ClientHotfixNotifyBlob(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientHotfixNotifyBlob();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGameObjectActivateAnimKit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGameObjectActivateAnimKit* msg);

    /* 0010 */ uint8_t m_objectGUID[8];
    /* 0018 */ uint32_t m_animKitID;

    ClientGameObjectActivateAnimKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGameObjectActivateAnimKit();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMapObjEvents : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMapObjEvents* msg);

    /* 0010 */ uint32_t m_uniqueID;
    /* 0014 */ WDataStore m_data;

    ClientMapObjEvents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMapObjEvents();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRaidMarkersChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRaidMarkersChanged* msg);

    /* 0010 */ uint32_t m_raidMarkers;

    ClientRaidMarkersChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRaidMarkersChanged();
    bool Handle();
};

class ClientLfGuildApplicantListUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildApplicantListUpdated* msg);

    ClientLfGuildApplicantListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildApplicantListUpdated();
    bool Handle();
};

class ClientGuildMemberDailyReset : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMemberDailyReset* msg);

    ClientGuildMemberDailyReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMemberDailyReset();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientAccountInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAccountInfoResponse* msg);

    /* 0010 */ char unk_0010[1280];
    /* 0510 */ TSGrowableArray<JamAccountInfoData> unk_0510;
    /* 0520 */ uint32_t unk_0520;
    /* 0524 */ uint32_t unk_0524;
    /* 0528 */ uint32_t unk_0528;
    /* 052C */ char unk_052C[1280];
    /* 0A2C */ uint32_t unk_0A2C;
    /* 0A30 */ uint32_t unk_0A30;
    /* 0A34 */ TSGrowableArray<JamAccountInfoData> unk_0A34;

    ClientAccountInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAccountInfoResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildEventLogQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildEventLogQueryResults* msg);

    /* 0010 */ TSGrowableArray<JamGuildEventLogEntry> m_entry;

    ClientGuildEventLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildEventLogQueryResults();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildKnownRecipes : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildKnownRecipes* msg);

    /* 0010 */ TSGrowableArray<JamGuildKnownRecipes> m_recipes;

    ClientGuildKnownRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildKnownRecipes();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSetupResearchHistory : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetupResearchHistory* msg);

    /* 0010 */ TSGrowableArray<JamResearchHistory> m_history;

    ClientSetupResearchHistory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetupResearchHistory();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDumpRideTicketsResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDumpRideTicketsResponse* msg);

    /* 0010 */ TSGrowableArray<JamRideTicketString> unk_0010;

    ClientDumpRideTicketsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDumpRideTicketsResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

struct JamLootItem {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t m_displayID;
    /* 0008 */ int32_t m_quantity;
    /* 000C */ int32_t m_randomSuffix;
    /* 0010 */ int32_t m_randomPropertiesID;
};

class ClientLootContents : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLootContents* msg);

    /* 0010 */ TSGrowableArray<JamLootItem> m_items;

    ClientLootContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLootContents();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildMoveComplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMoveComplete* msg);

    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ char m_guildName[512];

    ClientGuildMoveComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMoveComplete();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetActiveMover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetActiveMover* msg);

    /* 0010 */ uint8_t m_moverGUID[8];

    ClientMoveSetActiveMover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetActiveMover();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientShowRatings : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientShowRatings* msg);

    /* 0010 */ uint32_t unk_0010[4];
    /* 0020 */ float unk_0020[4];
    /* 0030 */ uint32_t unk_0030[4];
    /* 0040 */ float unk_0040[4];

    ClientShowRatings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientShowRatings();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWeeklySpellUsageUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWeeklySpellUsageUpdate* msg);

    /* 0010 */ JamWeeklySpellUse m_usage;

    ClientWeeklySpellUsageUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWeeklySpellUsageUpdate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
};

class ClientNotification : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientNotification* msg);

    /* 0010 */ char m_text[8192];

    ClientNotification(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientNotification();
    bool Handle();
};

class ClientGuildXPEarned : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildXPEarned* msg);

    /* 0010 */ uint64_t m_xp;

    ClientGuildXPEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildXPEarned();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildInvite* msg);

    /* 0010 */ char m_guildName[96];
    /* 0070 */ int32_t m_background;
    /* 0074 */ int32_t m_achievementPoints;
    /* 0078 */ int32_t m_emblemStyle;
    /* 007C */ int32_t m_emblemColor;
    /* 0080 */ int32_t m_guildLevel;
    /* 0084 */ int32_t m_borderColor;
    /* 0088 */ uint8_t m_guildGUID[8];
    /* 0090 */ char m_inviterName[48];
    /* 00C0 */ char m_oldGuildName[96];
    /* 0120 */ uint8_t m_oldGuildGUID[8];

    ClientGuildInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildInvite();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSuspendToken : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSuspendToken* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ bool unk_0014;

    ClientSuspendToken(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSuspendToken();
    bool Handle();
};

class ClientGuildCriteriaData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildCriteriaData* msg);

    /* 0010 */ TSGrowableArray<JamGuildCriteriaProgress> unk_0010;

    ClientGuildCriteriaData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCriteriaData();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientNewWorldAbort : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientNewWorldAbort* msg);

    ClientNewWorldAbort(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientNewWorldAbort();
    bool Handle();
};

class ClientRatedBgRating : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRatedBgRating* msg);

    /* 0010 */ int32_t unk_0010;

    ClientRatedBgRating(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRatedBgRating();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemRefundInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemRefundInfoResponse* msg);

    /* 0010 */ JamItemRefundInfo m_info;

    ClientItemRefundInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRefundInfoResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientChatIgnoredAccountMuted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientChatIgnoredAccountMuted* msg);

    ClientChatIgnoredAccountMuted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientChatIgnoredAccountMuted();
    bool Handle();
};

struct JamClientCurrencyData {
    /* 0000 */ int32_t m_type;
    /* 0004 */ int32_t m_quantity;
    /* 0008 */ bool m_hasWeeklyQuantity;
    /* 000C */ int32_t m_weeklyQuantity;
    /* 0010 */ bool m_hasMaxWeeklyQuantity;
    /* 0014 */ int32_t m_maxWeeklyQuantity;
    /* 0018 */ bool m_hasTrackedQuantity;
    /* 001C */ int32_t m_trackedQuantity;
    /* 0020 */ uint8_t m_flags;
};

class ClientSetupCurrency : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetupCurrency* msg);

    /* 0010 */ TSGrowableArray<JamClientCurrencyData> m_data;

    ClientSetupCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetupCurrency();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldMgrQueueInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrQueueInvite* msg);

    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ int32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ bool unk_0030;

    ClientBattlefieldMgrQueueInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrQueueInvite();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRespondInspectAchievements : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRespondInspectAchievements* msg);

    /* 0010 */ TSGrowableArray<JamEarnedAchievement> unk_0010;
    /* 0020 */ TSGrowableArray<JamCriteriaProgress> unk_0020;
    /* 0030 */ uint8_t unk_0030[8];

    ClientRespondInspectAchievements(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRespondInspectAchievements();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDifferentInstanceFromParty : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDifferentInstanceFromParty* msg);

    ClientDifferentInstanceFromParty(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDifferentInstanceFromParty();
    bool Handle();
};

class ClientStreamingMoviesUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientStreamingMoviesUpdate* msg);

    /* 0010 */ TSGrowableArray<uint16_t> unk_0010;

    ClientStreamingMoviesUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientStreamingMoviesUpdate();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientTransferPending : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTransferPending* msg);

    /* 0010 */ uint8_t m_hasTransferSpellID;
    /* 0014 */ uint32_t m_transferSpellID;
    /* 0018 */ uint32_t m_mapID;
    /* 001C */ uint8_t m_hasShip;
    /* 0020 */ uint32_t m_shipID;
    /* 0024 */ uint32_t m_shipOriginMapID;

    ClientTransferPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTransferPending();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidStorageFailed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageFailed* msg);

    /* 0010 */ uint8_t m_reason;

    ClientVoidStorageFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageFailed();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildRosterUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRosterUpdate* msg);

    /* 0010 */ TSGrowableArray<ClientGuildRosterMemberData> m_memberData;

    ClientGuildRosterUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRosterUpdate();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildChallengeUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChallengeUpdate* msg);

    /* 0010 */ int32_t m_challengeMaxCount[4];
    /* 0020 */ int32_t m_challengeGoldReward[4];
    /* 0030 */ int32_t m_challengeCurrentCount[4];
    /* 0040 */ int32_t m_challengeXPReward[4];
    /* 0050 */ int32_t m_challengeMaxLevelGoldReward[4];

    ClientGuildChallengeUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChallengeUpdate();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSummonRaidMemberValidateFailed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSummonRaidMemberValidateFailed* msg);

    /* 0010 */ TSGrowableArray<JamSummonRaidMemberValidateFailed> m_members;

    ClientSummonRaidMemberValidateFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSummonRaidMemberValidateFailed();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientClearBossEmotes : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientClearBossEmotes* msg);

    ClientClearBossEmotes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientClearBossEmotes();
    bool Handle();
};

class ClientFailedPlayerCondition : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientFailedPlayerCondition* msg);

    /* 0010 */ int32_t unk_0010;

    ClientFailedPlayerCondition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientFailedPlayerCondition();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientInspectRatedBgStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientInspectRatedBgStats* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;

    ClientInspectRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientInspectRatedBgStats();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemExpirePurchaseRefund : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemExpirePurchaseRefund* msg);

    /* 0010 */ uint8_t m_itemGUID[8];

    ClientItemExpirePurchaseRefund(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemExpirePurchaseRefund();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildMembershipListUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildMembershipListUpdated* msg);

    /* 0010 */ TSGrowableArray<LFGuildApplicationData> m_applications;
    /* 0020 */ int32_t m_numRemaining;

    ClientLfGuildMembershipListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildMembershipListUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildMemberRecipes : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMemberRecipes* msg);

    /* 0010 */ uint8_t m_memberGUID[8];
    /* 0018 */ int32_t m_skillStep;
    /* 001C */ int32_t m_skillRank;
    /* 0020 */ int32_t m_skillLineID;
    /* 0024 */ uint8_t m_skillLineBitArray[300];

    ClientGuildMemberRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMemberRecipes();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildRecruitListUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildRecruitListUpdated* msg);

    /* 0010 */ TSGrowableArray<LFGuildRecruitData> m_recruits;
    /* 0020 */ int32_t m_updateTime;

    ClientLfGuildRecruitListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildRecruitListUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildReset : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReset* msg);

    /* 0010 */ uint8_t m_guildGUID[8];

    ClientGuildReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReset();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildMembersWithRecipe : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMembersWithRecipe* msg);

    /* 0010 */ TSGrowableArray<uint64_t> m_members;
    /* 0020 */ int32_t m_spellID;
    /* 0024 */ int32_t m_skillLineID;

    ClientGuildMembersWithRecipe(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMembersWithRecipe();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildBrowseUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildBrowseUpdated* msg);

    /* 0010 */ TSGrowableArray<LFGuildBrowseData> m_guilds;

    ClientLfGuildBrowseUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildBrowseUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidTransferResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidTransferResult* msg);

    /* 0010 */ uint32_t m_result;

    ClientVoidTransferResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidTransferResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientSpellinterruptlog : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSpellinterruptlog* msg);

    /* 0010 */ uint8_t m_casterGUID[8];
    /* 0018 */ uint8_t m_victimGUID[8];
    /* 0020 */ int32_t m_interruptingSpellID;
    /* 0024 */ int32_t m_interruptedSpellID;

    ClientSpellinterruptlog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSpellinterruptlog();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildRewardList : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRewardList* msg);

    /* 0010 */ TSGrowableArray<JamGuildRewardItem> m_rewardItems;
    /* 0020 */ uint32_t m_version;

    ClientGuildRewardList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRewardList();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientCurrencyLootRemoved : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCurrencyLootRemoved* msg);

    /* 0010 */ uint8_t m_lootListID;

    ClientCurrencyLootRemoved(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCurrencyLootRemoved();
    bool Handle();
};

class ClientCustomLoadScreen : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCustomLoadScreen* msg);

    /* 0010 */ int32_t unk_0010;

    ClientCustomLoadScreen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCustomLoadScreen();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientCurrencyLootRestored : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCurrencyLootRestored* msg);

    /* 0010 */ int32_t m_currencyID;
    /* 0014 */ int32_t m_quantity;
    /* 0018 */ uint8_t m_lootListID;

    ClientCurrencyLootRestored(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCurrencyLootRestored();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMessageBox : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMessageBox* msg);

    ClientMessageBox(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMessageBox();
    bool Handle();
};

class ClientRequestCemeteryListResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRequestCemeteryListResponse* msg);

    /* 0010 */ bool m_isGossipTriggered;
    /* 0014 */ TSGrowableArray<uint32_t> m_cemeteryIDs;

    ClientRequestCemeteryListResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRequestCemeteryListResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildBankLogQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankLogQueryResults* msg);

    /* 0010 */ bool m_hasWeeklyBonusMoney;
    /* 0018 */ uint64_t m_weeklyBonusMoney;
    /* 0020 */ uint32_t m_tab;
    /* 0024 */ TSGrowableArray<JamGuildBankLogEntry> m_entry;

    ClientGuildBankLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankLogQueryResults();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildRanks : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRanks* msg);

    /* 0010 */ TSGrowableArray<JamCliGuildRankData> m_ranks;

    ClientGuildRanks(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRanks();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildFlaggedForRename : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildFlaggedForRename* msg);

    /* 0010 */ uint8_t m_flagSet;

    ClientGuildFlaggedForRename(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildFlaggedForRename();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildReputationWeeklyCap : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReputationWeeklyCap* msg);

    /* 0010 */ int32_t m_repToCap;

    ClientGuildReputationWeeklyCap(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReputationWeeklyCap();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientWardenData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWardenData* msg);

    ClientWardenData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWardenData();
    bool Handle();
};

class ClientWorldServerInfo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWorldServerInfo* msg);

    /* 0010 */ uint32_t m_weeklyReset;
    /* 0014 */ bool m_hasRestrictedMoney;
    /* 0018 */ uint32_t m_restrictedMoney;
    /* 001C */ uint8_t m_isTournamentRealm;
    /* 001D */ uint8_t unk_001D[3];
    /* 0020 */ bool m_hasRestrictedLevel;
    /* 0024 */ uint32_t m_restrictedLevel;
    /* 0028 */ uint32_t m_difficultyID;
    /* 002C */ bool m_hasIneligibleForLootMask;
    /* 0030 */ uint32_t m_ineligibleForLootMask;

    ClientWorldServerInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWorldServerInfo();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfgUpdateStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgUpdateStatus* msg);

    /* 0010 */ bool unk_0010;
    /* 0011 */ bool unk_0011;
    /* 0012 */ bool unk_0012;
    /* 0014 */ TSGrowableArray<uint32_t> unk_0014;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint8_t unk_0028[8];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint8_t unk_0040;
    /* 0041 */ bool unk_0041;
    /* 0042 */ char unk_0042[256];
    /* 0142 */ uint8_t unk_0142[3];

    ClientLfgUpdateStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgUpdateStatus();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDisplayGameError : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDisplayGameError* msg);

    /* 0010 */ bool unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ bool unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;

    ClientDisplayGameError(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDisplayGameError();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGameEventDebugLog : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGameEventDebugLog* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ char unk_0020[64];
    /* 0060 */ char unk_0060[128];
    /* 00E0 */ uint32_t unk_00E0;
    /* 00E4 */ char unk_00E4[256];

    ClientGameEventDebugLog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGameEventDebugLog();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPartyInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPartyInvite* msg);

    /* 0010 */ uint32_t m_serial;
    /* 0014 */ TSGrowableArray<uint32_t> m_lfgSlots;
    /* 0024 */ uint32_t m_lfgCompletedMask;
    /* 0028 */ char m_inviterRealmName[256];
    /* 0128 */ uint32_t m_inviterCfgRealmID;
    /* 012C */ uint32_t unk_012C;
    /* 0130 */ uint8_t m_inviterGUID[8];
    /* 0138 */ bool m_isXRealm;
    /* 0139 */ char m_inviterName[48];
    /* 0169 */ bool m_canAccept;
    /* 016C */ uint32_t m_proposedRoles;
    /* 0170 */ bool m_mustBeBNetFriend;

    ClientPartyInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPartyInvite();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDebugRuneRegen : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDebugRuneRegen* msg);

    /* 0010 */ float m_regen[4];
    /* 0020 */ uint32_t m_runeCount[6];
    /* 0038 */ uint32_t m_runeCooldown[6];
    /* 0050 */ uint32_t m_runeState;
    /* 0054 */ uint32_t m_runeMask;

    ClientDebugRuneRegen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDebugRuneRegen();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRatedBgStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRatedBgStats* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;

    ClientRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRatedBgStats();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldMgrEjectPending : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEjectPending* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint8_t unk_0018;

    ClientBattlefieldMgrEjectPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEjectPending();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildPermissionsQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildPermissionsQueryResults* msg);

    /* 0010 */ int32_t m_withdrawGoldLimit;
    /* 0014 */ int32_t m_numTabs;
    /* 0018 */ int32_t m_rankID;
    /* 001C */ uint32_t m_flags;
    /* 0020 */ TSGrowableArray<JamGuildRankTabPermissions> m_tabPermissions;

    ClientGuildPermissionsQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildPermissionsQueryResults();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlegroundInfoThrottled : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlegroundInfoThrottled* msg);

    ClientBattlegroundInfoThrottled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlegroundInfoThrottled();
    bool Handle();
};

class ClientBattlefieldMgrEntryInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEntryInvite* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[4];
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint8_t unk_001E;
    /* 001F */ uint8_t unk_001F;
    /* 0020 */ uint32_t unk_0020;

    ClientBattlefieldMgrEntryInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEntryInvite();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildAchievementDeleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementDeleted* msg);

    /* 0010 */ WowTime unk_0010;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint8_t unk_0038[8];

    ClientGuildAchievementDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementDeleted();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldStatusQueued : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusQueued* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C;
    /* 0020 */ JamClientBattlefieldStatusHeader m_header;

    ClientBattlefieldStatusQueued(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusQueued();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldPortDenied : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldPortDenied* msg);

    ClientBattlefieldPortDenied(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldPortDenied();
    bool Handle();
};

class ClientResearchComplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientResearchComplete* msg);

    /* 0010 */ JamResearchHistory m_research;

    ClientResearchComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientResearchComplete();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildNews : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNews* msg);

    /* 0010 */ TSGrowableArray<JamGuildNewsEvent> m_newsEvents;

    ClientGuildNews(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNews();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSupercededSpells : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSupercededSpells* msg);

    /* 0010 */ int32_t m_spellID;
    /* 0014 */ int32_t m_supercededSpellID;

    ClientSupercededSpells(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSupercededSpells();
    bool Handle();
};

class ClientBattlefieldMgrStateChange : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrStateChange* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint32_t unk_0018;

    ClientBattlefieldMgrStateChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrStateChange();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSetDfFastLaunchResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetDfFastLaunchResult* msg);

    /* 0010 */ uint8_t unk_0010;

    ClientSetDfFastLaunchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetDfFastLaunchResult();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildPostUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildPostUpdated* msg);

    /* 0010 */ bool m_hasPost;
    /* 0014 */ JamLFGuildPost m_post;

    ClientLfGuildPostUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildPostUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDontAutoPushSpellsToActionBar : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDontAutoPushSpellsToActionBar* msg);

    ClientDontAutoPushSpellsToActionBar(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDontAutoPushSpellsToActionBar();
    bool Handle();
};

class ClientGuildAchievementMembers : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementMembers* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ TSGrowableArray<JamGuildAchievementMember> unk_0020;

    ClientGuildAchievementMembers(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementMembers();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientCommentatorPartyInfo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCommentatorPartyInfo* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ TSGrowableArray<JamClientCommentatorPartyMember> unk_0014;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ bool unk_0028;

    ClientCommentatorPartyInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCommentatorPartyInfo();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientUpdateActionButtons : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateActionButtons* msg);

    /* 0010 */ int32_t m_actionButtons[144];
    /* 0250 */ uint8_t m_reason;

    ClientUpdateActionButtons(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateActionButtons();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfgJoinResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgJoinResult* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0014 */ TSGrowableArray<JamLfgJoinResultSlot> unk_0014;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint8_t unk_0028[8];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;

    ClientLfgJoinResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgJoinResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemRemovePassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemRemovePassive* msg);

    /* 0010 */ int32_t m_spellID;

    ClientItemRemovePassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRemovePassive();
    bool Handle();
};

class ClientGuildChallengeCompleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChallengeCompleted* msg);

    /* 0010 */ int32_t m_goldAwarded;
    /* 0014 */ int32_t m_maxCount;
    /* 0018 */ int32_t m_challengeType;
    /* 001C */ int32_t m_xpAwarded;
    /* 0020 */ int32_t m_currentCount;

    ClientGuildChallengeCompleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChallengeCompleted();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRoleChangedInform : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRoleChangedInform* msg);

    /* 0010 */ uint8_t m_changedByGUID[8];
    /* 0018 */ uint8_t m_targetGUID[8];
    /* 0020 */ uint32_t m_oldRole;
    /* 0024 */ uint32_t m_newRole;

    ClientRoleChangedInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRoleChangedInform();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientArenaTeamCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientArenaTeamCommandResult* msg);

    /* 0010 */ uint32_t m_action;
    /* 0014 */ char m_name[48];
    /* 0044 */ char m_teamName[96];
    /* 00A4 */ uint32_t m_result;

    ClientArenaTeamCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientArenaTeamCommandResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWeeklySpellUsage : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWeeklySpellUsage* msg);

    /* 0010 */ TSGrowableArray<JamWeeklySpellUse> m_usage;

    ClientWeeklySpellUsage(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWeeklySpellUsage();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWeeklyResetCurrency : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWeeklyResetCurrency* msg);

    ClientWeeklyResetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWeeklyResetCurrency();
    bool Handle();
};

class ClientTimeSyncReq : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTimeSyncReq* msg);

    /* 0010 */ uint32_t m_sequenceIndex;

    ClientTimeSyncReq(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTimeSyncReq();
    bool Handle();
};

class ClientPetAdded : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPetAdded* msg);

    /* 0010 */ uint8_t m_flags;
    /* 0014 */ uint32_t m_level;
    /* 0018 */ char m_name[80];
    /* 0068 */ int32_t m_slot;
    /* 006C */ uint32_t m_petNumber;
    /* 0070 */ int32_t m_creatureID;

    ClientPetAdded(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPetAdded();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildChangeNameResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChangeNameResult* msg);

    /* 0010 */ uint8_t m_success;

    ClientGuildChangeNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChangeNameResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientUpdateDungeonEncounterForLoot : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateDungeonEncounterForLoot* msg);

    ClientUpdateDungeonEncounterForLoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateDungeonEncounterForLoot();
    bool Handle();
};

class ClientGuildRoster : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRoster* msg);

    /* 0010 */ WowTime m_createDate;
    /* 0030 */ TSGrowableArray<ClientGuildRosterMemberData> m_memberData;
    /* 0040 */ uint32_t m_guildFlags;
    /* 0044 */ uint32_t m_numAccounts;
    /* 0048 */ uint32_t m_weeklyRepCap;
    /* 004C */ char m_motd[516];
    /* 0250 */ char m_infoText[2048];

    ClientGuildRoster(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRoster();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWargameCheckEntry : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWargameCheckEntry* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint8_t unk_001E;
    /* 001F */ uint8_t unk_001F;
    /* 0020 */ uint32_t unk_0020;

    ClientWargameCheckEntry(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWargameCheckEntry();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildSendGuildXP : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildSendGuildXP* msg);

    /* 0010 */ uint64_t m_guildRemainingXP;
    /* 0018 */ uint64_t m_memberTotalXP;
    /* 0020 */ uint64_t m_guildCurrentXP;
    /* 0028 */ uint64_t m_memberWeeklyXP;
    /* 0030 */ uint64_t m_guildTodayXP;

    ClientGuildSendGuildXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildSendGuildXP();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientAreaTriggerMovementUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAreaTriggerMovementUpdate* msg);

    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ float unk_0018;
    /* 001C */ float unk_001C;
    /* 0020 */ float unk_0020;
    /* 0024 */ float unk_0024;

    ClientAreaTriggerMovementUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAreaTriggerMovementUpdate();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMissileCancel : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMissileCancel* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015[3];
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint8_t unk_001E;
    /* 001F */ uint8_t unk_001F;

    ClientMissileCancel(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMissileCancel();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientFeatureSystemStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientFeatureSystemStatus* msg);

    /* 0010 */ bool m_sessionAlertEnabled;
    /* 0014 */ int32_t m_sessionAlertDelay;
    /* 0018 */ int32_t m_sessionAlertPeriod;
    /* 001C */ int32_t m_sessionAlertDisplayTime;
    /* 0020 */ uint8_t m_complaintStatus;
    /* 0024 */ uint32_t m_cfgRealmID;
    /* 0028 */ uint32_t m_cfgRealmRecID;
    /* 002C */ bool m_scrollOfResurrectionEnabled;
    /* 0030 */ uint32_t m_scrollOfResurrectionRequestsRemaining;
    /* 0034 */ uint32_t m_scrollOfResurrectionMaxRequestsPerDay;
    /* 0038 */ bool m_itemRestorationButtonEnabled;
    /* 0039 */ bool m_voiceEnabled;
    /* 003A */ bool m_travelPassEnabled;
    /* 003B */ uint8_t unk_003B[1];
    /* 003C */ bool m_europaTicketSystemEnabled;
    /* 0040 */ JamThrottleState m_europaTicketThrottle;

    ClientFeatureSystemStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientFeatureSystemStatus();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPvpOptionsEnabled : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPvpOptionsEnabled* msg);

    /* 0010 */ bool unk_0010;
    /* 0011 */ bool unk_0011;
    /* 0012 */ bool unk_0012;
    /* 0013 */ bool unk_0013;
    /* 0014 */ bool unk_0014;

    ClientPvpOptionsEnabled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPvpOptionsEnabled();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWeeklyLastReset : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWeeklyLastReset* msg);

    /* 0010 */ int32_t unk_0010;

    ClientWeeklyLastReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWeeklyLastReset();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildPartyState : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildPartyState* msg);

    /* 0010 */ float m_guildXPEarnedMult;
    /* 0014 */ uint32_t m_numGuildGroupRequired;
    /* 0018 */ bool m_inGuildParty;
    /* 001C */ uint32_t m_numGuildGroupMembers;

    ClientGuildPartyState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildPartyState();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlegroundPlayerJoined : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlegroundPlayerJoined* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;

    ClientBattlegroundPlayerJoined(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlegroundPlayerJoined();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLoadCufProfiles : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLoadCufProfiles* msg);

    /* 0010 */ TSGrowableArray<JamCliCUFProfile> m_profiles;

    ClientLoadCufProfiles(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLoadCufProfiles();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientXpGainAborted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientXpGainAborted* msg);

    /* 0010 */ uint8_t m_victimGUID[8];
    /* 0018 */ uint32_t m_xp;
    /* 001C */ uint32_t m_reason;
    /* 0020 */ uint32_t m_xpBonus;

    ClientXpGainAborted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientXpGainAborted();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildAchievementEarned : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementEarned* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ WowTime unk_0020;

    ClientGuildAchievementEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementEarned();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPetSlotUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPetSlotUpdated* msg);

    /* 0010 */ int32_t m_prevSlot;
    /* 0014 */ uint32_t m_petNumber;
    /* 0018 */ uint32_t m_prevPetNumber;
    /* 001C */ int32_t m_newSlot;

    ClientPetSlotUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPetSlotUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidStorageTransferChanges : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageTransferChanges* msg);

    /* 0010 */ TSGrowableArray<JamVoidItem> m_added;
    /* 0020 */ TSGrowableArray<uint64_t> m_removed;

    ClientVoidStorageTransferChanges(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageTransferChanges();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientAccountRestrictedWarning : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAccountRestrictedWarning* msg);

    ClientAccountRestrictedWarning(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAccountRestrictedWarning();
    bool Handle();
};

class ClientBattlefieldMgrExitRequest : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrExitRequest* msg);

    ClientBattlefieldMgrExitRequest(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrExitRequest();
    bool Handle();
};

struct JamLfgSearchResultGUID {
    /* 0000 */ uint8_t m_guid[8];
};

struct JamLfgSearchResultParty {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ char m_comment[260];
    /* 0110 */ uint8_t m_leaderGUID[8];
    /* 0118 */ uint32_t unk_0118;
    /* 011C */ uint8_t unk_011C[3];
    /* 011F */ uint8_t unk_011F;
};

struct JamLfgSearchResultPlayer {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ float unk_0044;
    /* 0048 */ float unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ char m_comment[256];
    /* 0164 */ bool unk_0164;
    /* 0165 */ uint8_t unk_0165[3];
    /* 0168 */ uint8_t m_partyGUID[8];
    /* 0170 */ uint8_t unk_0170;
    /* 0171 */ uint8_t unk_0171[3];
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint8_t unk_0178;
    /* 0179 */ uint8_t unk_0179[7];
    /* 0180 */ uint8_t m_guildGUID[8];
    /* 0188 */ uint32_t unk_0188;
    /* 018C */ uint8_t unk_018C;
    /* 018D */ uint8_t unk_018D;
    /* 018E */ uint8_t unk_018E;
    /* 018F */ uint8_t unk_018F;
};

class ClientLfgSearchResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgSearchResult* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ TSGrowableArray<JamLfgSearchResultGUID> m_removed;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ bool unk_0028;
    /* 0029 */ uint8_t unk_0029[7];
    /* 0030 */ uint8_t m_guid[8];
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ TSGrowableArray<JamLfgSearchResultParty> m_parties;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ TSGrowableArray<JamLfgSearchResultPlayer> m_players;
    /* 006C */ uint32_t unk_006C;

    ClientLfgSearchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgSearchResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldRatedInfo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldRatedInfo* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint8_t unk_002C;

    ClientBattlefieldRatedInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldRatedInfo();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildCommandResult* msg);

    /* 0010 */ uint32_t m_command;
    /* 0014 */ uint8_t m_result;

    ClientLfGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildCommandResult();
    bool Handle();
};

class ClientLfgSlotInvalid : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgSlotInvalid* msg);

    /* 0010 */ JamResearchHistory unk_0010;

    ClientLfgSlotInvalid(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgSlotInvalid();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildAchievementData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementData* msg);

    /* 0010 */ TSGrowableArray<JamEarnedAchievement> unk_0010;

    ClientGuildAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementData();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientDisenchantCredit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDisenchantCredit* msg);

    /* 0010 */ uint8_t m_lootedGUID[8];
    /* 0018 */ int32_t m_randomPropertiesID;
    /* 001C */ int32_t m_itemID;
    /* 0020 */ int32_t m_randomPropertiesSeed;

    ClientDisenchantCredit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDisenchantCredit();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientQuestGiverQuestComplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientQuestGiverQuestComplete* msg);

    /* 0010 */ JamQuestGiverQuestComplete m_complete;

    ClientQuestGiverQuestComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientQuestGiverQuestComplete();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPlaySpellVisualKit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPlaySpellVisualKit* msg);

    /* 0010 */ uint8_t m_unitGUID[8];
    /* 0018 */ uint32_t m_timeOffset;
    /* 001C */ int32_t m_kitRecID;
    /* 0020 */ uint32_t m_kitType;

    ClientPlaySpellVisualKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPlaySpellVisualKit();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildCriteriaDeleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildCriteriaDeleted* msg);

    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ uint32_t unk_0018;

    ClientGuildCriteriaDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCriteriaDeleted();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoneyNotify : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoneyNotify* msg);

    /* 0010 */ uint64_t unk_0010;

    ClientMoneyNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoneyNotify();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientLearnedSpell : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLearnedSpell* msg);

    /* 0010 */ int32_t m_spellID;
    /* 0014 */ uint32_t m_suppressMessaging;

    ClientLearnedSpell(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLearnedSpell();
    bool Handle();
};

class ClientReforgeResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientReforgeResult* msg);

    /* 0010 */ uint8_t m_success;

    ClientReforgeResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientReforgeResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientAllAchievementData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAllAchievementData* msg);

    /* 0010 */ TSGrowableArray<JamEarnedAchievement> unk_0010;
    /* 0020 */ TSGrowableArray<JamCriteriaProgress> unk_0020;

    ClientAllAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAllAchievementData();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldPlayerPositions : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldPlayerPositions* msg);

    /* 0010 */ TSGrowableArray<JamBattlefieldPlayerPosition> unk_0010;
    /* 0020 */ TSGrowableArray<JamBattlefieldPlayerPosition> unk_0020;

    ClientBattlefieldPlayerPositions(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldPlayerPositions();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldStatusNeedconfirmation : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusNeedconfirmation* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ JamClientBattlefieldStatusHeader m_header;

    ClientBattlefieldStatusNeedconfirmation(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusNeedconfirmation();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientStartTimer : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientStartTimer* msg);

    /* 0010 */ uint32_t m_timeLeft;
    /* 0014 */ uint32_t m_totalTime;
    /* 0018 */ uint32_t m_type;

    ClientStartTimer(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientStartTimer();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlegroundPlayerLeft : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlegroundPlayerLeft* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;

    ClientBattlegroundPlayerLeft(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlegroundPlayerLeft();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSetCurrency : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetCurrency* msg);

    /* 0010 */ int32_t m_quantity;
    /* 0014 */ int32_t m_type;
    /* 0018 */ bool m_suppressChatLog;
    /* 0019 */ uint8_t unk_0019[3];
    /* 001C */ bool m_hasWeeklyQuantity;
    /* 0020 */ int32_t m_weeklyQuantity;
    /* 0024 */ bool m_hasTrackedQuantity;
    /* 0028 */ int32_t m_trackedQuantity;

    ClientSetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetCurrency();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientWargameRequestSent : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWargameRequestSent* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;

    ClientWargameRequestSent(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWargameRequestSent();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientClearCooldowns : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientClearCooldowns* msg);

    /* 0010 */ uint8_t m_guid[8];
    /* 0018 */ TSGrowableArray<int32_t> m_spellID;

    ClientClearCooldowns(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientClearCooldowns();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldMgrEntered : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEntered* msg);

    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ bool unk_0018;
    /* 0019 */ bool unk_0019;
    /* 001A */ bool unk_001A;

    ClientBattlefieldMgrEntered(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEntered();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientTradeStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTradeStatus* msg);

    /* 0010 */ bool m_partnerIsSameBnetAccount;
    /* 0014 */ uint32_t m_status;
    /* 0018 */ int32_t m_currencyType;
    /* 001C */ int32_t m_currencyQuantity;
    /* 0020 */ uint8_t m_tradeSlot;
    /* 0024 */ uint32_t m_id;
    /* 0028 */ uint8_t m_partner[8];
    /* 0030 */ int32_t m_itemID;
    /* 0034 */ int32_t m_bagResult;
    /* 0038 */ bool m_failureForYou;

    ClientTradeStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTradeStatus();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPvpLogData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPvpLogData* msg);

    /* 0010 */ TSGrowableArray<JamPvpLogDataPlayer> m_players;
    /* 0020 */ bool m_hasTeamNames;
    /* 0021 */ char m_teamNames[2][96];
    /* 00E1 */ bool m_hasWinner;
    /* 00E2 */ uint8_t m_winner;
    /* 00E3 */ uint8_t unk_00E3;
    /* 00E4 */ bool m_hasRatings;
    /* 00E5 */ uint8_t unk_00E5[3];
    /* 00E8 */ uint32_t m_prematchRating[2];
    /* 00F0 */ uint32_t m_newRating[2];
    /* 00F8 */ uint32_t m_matchmakingRating[2];
    /* 0100 */ int8_t m_playerCount[2];

    ClientPvpLogData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPvpLogData();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildSendRankChange : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildSendRankChange* msg);

    /* 0010 */ uint32_t m_rankID;
    /* 0014 */ uint8_t unk_0014[4];
    /* 0018 */ uint8_t m_officerGUID[8];
    /* 0020 */ uint8_t m_otherGUID[8];
    /* 0028 */ bool m_promote;

    ClientGuildSendRankChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildSendRankChange();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildReputationReactionChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReputationReactionChanged* msg);

    /* 0010 */ uint8_t m_guid[8];

    ClientGuildReputationReactionChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReputationReactionChanged();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPetitionAlreadySigned : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPetitionAlreadySigned* msg);

    /* 0010 */ uint8_t m_signerGUID[8];

    ClientPetitionAlreadySigned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPetitionAlreadySigned();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRequestPvpRewardsResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRequestPvpRewardsResponse* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;

    ClientRequestPvpRewardsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRequestPvpRewardsResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientAverageItemLevelInform : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAverageItemLevelInform* msg);

    ClientAverageItemLevelInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAverageItemLevelInform();
    bool Handle();
};

class ClientItemRefundResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemRefundResult* msg);

    /* 0010 */ JamItemRefundResult m_result;

    ClientItemRefundResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRefundResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildBankMoneyWithdrawn : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankMoneyWithdrawn* msg);

    /* 0010 */ uint64_t m_remainingWithdrawMoney;

    ClientGuildBankMoneyWithdrawn(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankMoneyWithdrawn();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

struct JamPhaseShiftChange {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t m_guid[8];
    /* 0010 */ WDataStore m_preloadMapIDs;
    /* 0034 */ WDataStore m_visibleMapIDs;
    /* 0058 */ WDataStore m_phaseIDs;
    /* 007C */ WDataStore m_uiWorldMapAreaIDSwaps;

    JamPhaseShiftChange();
    ~JamPhaseShiftChange();
};

class ClientPhaseShiftChange : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPhaseShiftChange* msg);

    /* 0010 */ JamPhaseShiftChange m_data;

    ClientPhaseShiftChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPhaseShiftChange();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

struct ClientTradeItemUnwrapped {
    /* 0000 */ int32_t m_enchantID;
    /* 0004 */ int32_t m_reforgeID;
    /* 0008 */ int32_t m_onUseEnchantmentID;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t m_creator[8];
    /* 0018 */ int32_t m_charges;
    /* 001C */ int32_t m_randomPropertiesSeed;
    /* 0020 */ int32_t m_randomPropertiesID;
    /* 0024 */ bool m_lock;
    /* 0028 */ int32_t m_maxDurability;
    /* 002C */ int32_t m_durability;
    /* 0030 */ int32_t m_socketEnchant[3];
    /* 003C */ uint32_t unk_003C;
};

struct ClientTradeItem {
    /* 0000 */ uint8_t m_slot;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ uint32_t m_stackCount;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t m_giftCreator[8];
    /* 0018 */ bool m_hasUnwrapped;
    /* 0019 */ uint8_t unk_0019[7];
    /* 0020 */ ClientTradeItemUnwrapped m_unwrapped;

    ClientTradeItem();
};

class ClientTradeUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTradeUpdated* msg);

    /* 0010 */ uint32_t m_stateIndex;
    /* 0014 */ uint8_t m_whichPlayer;
    /* 0018 */ uint32_t m_clientStateIndex;
    /* 001C */ int32_t m_currencyQuantity;
    /* 0020 */ uint64_t m_gold;
    /* 0028 */ int32_t m_currencyType;
    /* 002C */ int32_t m_proposedEnchantment;
    /* 0030 */ TSGrowableArray<ClientTradeItem> m_items;
    /* 0040 */ uint32_t m_id;

    ClientTradeUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTradeUpdated();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildMoveStarting : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMoveStarting* msg);

    /* 0010 */ uint8_t m_guildGUID[8];
    /* 0018 */ char m_guildName[256];

    ClientGuildMoveStarting(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMoveStarting();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientUpdateCurrencyWeekLimit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateCurrencyWeekLimit* msg);

    /* 0010 */ int32_t m_type;
    /* 0014 */ int32_t m_weekCap;

    ClientUpdateCurrencyWeekLimit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateCurrencyWeekLimit();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientRolePollBegin : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRolePollBegin* msg);

    /* 0010 */ uint8_t m_initiatorGUID[8];

    ClientRolePollBegin(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRolePollBegin();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemSendPassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemSendPassive* msg);

    /* 0010 */ TSGrowableArray<int32_t> m_spellIDs;

    ClientItemSendPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemSendPassive();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientForgeMasterSet : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientForgeMasterSet* msg);

    /* 0010 */ uint8_t m_forgeMaster[8];

    ClientForgeMasterSet(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientForgeMasterSet();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfGuildApplicationsListChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildApplicationsListChanged* msg);

    ClientLfGuildApplicationsListChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildApplicationsListChanged();
    bool Handle();
};

class ClientBattlefieldStatusFailed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusFailed* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ uint8_t unk_0020;
    /* 0021 */ uint8_t unk_0021;
    /* 0022 */ uint8_t unk_0022;
    /* 0023 */ uint8_t unk_0023;
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ JamClientRideTicket m_ticket;

    ClientBattlefieldStatusFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusFailed();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldList : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldList* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint8_t unk_0020[8];
    /* 0028 */ bool unk_0028;
    /* 0029 */ bool unk_0029;
    /* 002A */ uint8_t unk_002A;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ TSGrowableArray<uint32_t> unk_0030;
    /* 0040 */ bool unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ bool unk_0054;

    ClientBattlefieldList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldList();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientCategoryCooldown : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCategoryCooldown* msg);

    /* 0010 */ TSGrowableArray<JamCategoryCooldown> m_categoryCooldowns;

    ClientCategoryCooldown(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCategoryCooldown();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientUpdateServerPlayerPosition : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateServerPlayerPosition* msg);

    ClientUpdateServerPlayerPosition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateServerPlayerPosition();
    bool Handle();
};

class ClientBattlefieldStatusActive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusActive* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ JamClientBattlefieldStatusHeader m_header;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint8_t unk_004C;
    /* 004D */ uint8_t unk_004D[3];
    /* 0050 */ uint32_t unk_0050;

    ClientBattlefieldStatusActive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusActive();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildNameChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNameChanged* msg);

    /* 0010 */ uint8_t m_guildGUID[8];

    ClientGuildNameChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNameChanged();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildNewsDeleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNewsDeleted* msg);

    /* 0010 */ int32_t m_newsID;

    ClientGuildNewsDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNewsDeleted();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    void Unpack(CDataStore* msg, bool full);
};

class ClientServerInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientServerInfoResponse* msg);

    /* 0010 */ TSGrowableArray<JamServerInfoField> unk_0010;
    /* 0020 */ TSGrowableArray<JamServerInfoField> unk_0020;

    ClientServerInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientServerInfoResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientServerPerf : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientServerPerf* msg);

    /* 0010 */ TSGrowableArray<JamServerPerfString> unk_0010;

    ClientServerPerf(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientServerPerf();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientQuestNpcQueryResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientQuestNpcQueryResponse* msg);

    /* 0010 */ TSGrowableArray<JamQuestCompletionNPC> m_quests;

    ClientQuestNpcQueryResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientQuestNpcQueryResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldStatusWaitforgroups : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusWaitforgroups* msg);

    /* 0010 */ JamClientBattlefieldStatusHeader m_header;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint8_t unk_0048;
    /* 0049 */ uint8_t unk_0049;
    /* 004A */ uint8_t unk_004A;
    /* 004B */ uint8_t unk_004B;

    ClientBattlefieldStatusWaitforgroups(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusWaitforgroups();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildBankTextQueryResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankTextQueryResult* msg);

    /* 0010 */ uint32_t m_tab;
    /* 0014 */ char m_text[16384];

    ClientGuildBankTextQueryResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankTextQueryResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidStorageContents : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageContents* msg);

    /* 0010 */ TSGrowableArray<JamVoidItem> m_items;

    ClientVoidStorageContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageContents();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidItemSwapResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidItemSwapResponse* msg);

    /* 0010 */ uint64_t m_secondItem;
    /* 0018 */ uint64_t m_firstItem;
    /* 0020 */ uint32_t m_firstSlot;
    /* 0024 */ uint32_t m_secondSlot;

    ClientVoidItemSwapResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidItemSwapResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildBankQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankQueryResults* msg);

    /* 0010 */ uint32_t m_tab;
    /* 0018 */ uint64_t m_money;
    /* 0020 */ bool m_fullUpdate;
    /* 0024 */ TSGrowableArray<JamGuildBankItemInfo> m_itemInfo;
    /* 0034 */ uint32_t m_withdrawalsRemaining;
    /* 0038 */ TSGrowableArray<JamGuildBankTabInfo> m_tabInfo;

    ClientGuildBankQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankQueryResults();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfgQueueStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgQueueStatus* msg);

    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint8_t unk_0020[8];
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C[3];
    /* 0048 */ uint8_t unk_0048[3];

    ClientLfgQueueStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgQueueStatus();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientInspectHonorStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientInspectHonorStats* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint16_t unk_001C;
    /* 001E */ uint16_t unk_001E;
    /* 0020 */ uint8_t unk_0020;

    ClientInspectHonorStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientInspectHonorStats();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientNewWorld : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientNewWorld* msg);

    /* 0010 */ C3Vector m_position;
    /* 001C */ float m_facing;
    /* 0020 */ uint32_t m_mapID;

    ClientNewWorld(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientNewWorld();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildSendMaxDailyXP : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildSendMaxDailyXP* msg);

    /* 0010 */ uint64_t m_xp;

    ClientGuildSendMaxDailyXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildSendMaxDailyXP();
    bool Handle();
};

class ClientBattlefieldMgrQueueRequestResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrQueueRequestResponse* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ bool unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013[5];
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint8_t unk_0028[8];

    ClientBattlefieldMgrQueueRequestResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrQueueRequestResponse();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientTimeAdjustment : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTimeAdjustment* msg);

    /* 0010 */ uint32_t m_sequenceIndex;
    /* 0014 */ float m_timeScale;

    ClientTimeAdjustment(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTimeAdjustment();
    bool Handle();
};

class ClientGuildMemberUpdateNote : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMemberUpdateNote* msg);

    /* 0010 */ bool m_isPublic;
    /* 0011 */ uint8_t unk_0011[7];
    /* 0018 */ uint8_t m_memberGUID[8];
    /* 0020 */ char m_note[256];

    ClientGuildMemberUpdateNote(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMemberUpdateNote();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfgUpdateStatusNone : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgUpdateStatusNone* msg);

    ClientLfgUpdateStatusNone(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgUpdateStatusNone();
    bool Handle();
};

class ClientSorStartExperienceIncomplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSorStartExperienceIncomplete* msg);

    ClientSorStartExperienceIncomplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSorStartExperienceIncomplete();
    bool Handle();
};

struct JamVendorItem {
    /* 0000 */ int32_t m_muid;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_item;
    /* 000C */ int32_t m_displayID;
    /* 0010 */ int32_t m_quantity;
    /* 0014 */ int32_t m_price;
    /* 0018 */ int32_t m_durability;
    /* 001C */ int32_t m_stackCount;
    /* 0020 */ int32_t m_extendedCostID;
    /* 0024 */ int32_t m_playerConditionID;
};

class ClientVendorInventory : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVendorInventory* msg);

    /* 0010 */ uint8_t m_reason;
    /* 0014 */ TSGrowableArray<JamVendorItem> m_items;
    /* 0024 */ uint8_t unk_0024[4];
    /* 0028 */ uint8_t m_vendor[8];

    ClientVendorInventory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVendorInventory();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemAddPassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemAddPassive* msg);

    /* 0010 */ int32_t m_spellID;

    ClientItemAddPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemAddPassive();
    bool Handle();
};

class ClientBattlefieldStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatus* msg);

    /* 0010 */ JamClientRideTicket m_ticket;

    ClientBattlefieldStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatus();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientLfgProposalUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgProposalUpdate* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[8];
    /* 0038 */ TSGrowableArray<JamLfgProposalMember> unk_0038;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ bool unk_0050;
    /* 0054 */ uint32_t unk_0054;

    ClientLfgProposalUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgProposalUpdate();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildCommandResult* msg);

    /* 0010 */ uint32_t m_command;
    /* 0014 */ char m_name[96];
    /* 0074 */ uint32_t m_result;

    ClientGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCommandResult();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldMgrEjected : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEjected* msg);

    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011[7];
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint8_t unk_001E;
    /* 001F */ uint8_t unk_001F;
    /* 0020 */ uint8_t unk_0020;
    /* 0021 */ uint8_t unk_0021;

    ClientBattlefieldMgrEjected(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEjected();
    bool Handle();
    bool Get(CDataStore* msg, bool full) override;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

#include "WoW/Source/JamClientAutoCode/JamClient.inl"

#endif
