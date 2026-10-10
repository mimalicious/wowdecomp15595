#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
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
    void Unpack(CDataStore* msg, bool full);
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientHotfixNotify : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientHotfixNotify* msg);

    /* 0010 */ TSGrowableArray<HotfixNotify> m_hotfixes;

    ClientHotfixNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientHotfixNotify();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientRaidMarkersChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRaidMarkersChanged* msg);

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
};

class ClientAccountInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAccountInfoResponse* msg);

    ClientAccountInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAccountInfoResponse();
    bool Handle();
};

class ClientGuildEventLogQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildEventLogQueryResults* msg);

    ClientGuildEventLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildEventLogQueryResults();
    bool Handle();
};

class ClientGuildKnownRecipes : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildKnownRecipes* msg);

    ClientGuildKnownRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildKnownRecipes();
    bool Handle();
};

class ClientSetupResearchHistory : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetupResearchHistory* msg);

    ClientSetupResearchHistory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetupResearchHistory();
    bool Handle();
};

class ClientDumpRideTicketsResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDumpRideTicketsResponse* msg);

    ClientDumpRideTicketsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDumpRideTicketsResponse();
    bool Handle();
};

class ClientLootContents : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLootContents* msg);

    ClientLootContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLootContents();
    bool Handle();
};

class ClientGuildMoveComplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMoveComplete* msg);

    ClientGuildMoveComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMoveComplete();
    bool Handle();
};

class ClientMoveSetActiveMover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetActiveMover* msg);

    ClientMoveSetActiveMover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetActiveMover();
    bool Handle();
};

class ClientShowRatings : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientShowRatings* msg);

    ClientShowRatings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientShowRatings();
    bool Handle();
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

class ClientGuildXpGain : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildXpGain* msg);

    ClientGuildXpGain(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildXpGain();
    bool Handle();
};

class ClientGuildInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildInvite* msg);

    ClientGuildInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildInvite();
    bool Handle();
};

class ClientSuspendToken : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSuspendToken* msg);

    /* 0010 */ uint32_t m_sequence;
    /* 0014 */ uint8_t m_reason;

    ClientSuspendToken(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSuspendToken();
    bool Handle();
};

class ClientGuildCriteriaData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildCriteriaData* msg);

    ClientGuildCriteriaData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCriteriaData();
    bool Handle();
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

    ClientRatedBgRating(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRatedBgRating();
    bool Handle();
};

class ClientItemRefundInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemRefundInfoResponse* msg);

    ClientItemRefundInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRefundInfoResponse();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientBattlefieldMgrQueueInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrQueueInvite* msg);

    ClientBattlefieldMgrQueueInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrQueueInvite();
    bool Handle();
};

class ClientRespondInspectAchievements : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRespondInspectAchievements* msg);

    ClientRespondInspectAchievements(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRespondInspectAchievements();
    bool Handle();
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

    ClientStreamingMoviesUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientStreamingMoviesUpdate();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidStorageFailed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageFailed* msg);

    ClientVoidStorageFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageFailed();
    bool Handle();
};

class ClientGuildRosterUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRosterUpdate* msg);

    ClientGuildRosterUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRosterUpdate();
    bool Handle();
};

class ClientGuildChallengeUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChallengeUpdate* msg);

    ClientGuildChallengeUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChallengeUpdate();
    bool Handle();
};

class ClientSummonRaidMemberValidateFailed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSummonRaidMemberValidateFailed* msg);

    ClientSummonRaidMemberValidateFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSummonRaidMemberValidateFailed();
    bool Handle();
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

    ClientFailedPlayerCondition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientFailedPlayerCondition();
    bool Handle();
};

class ClientInspectRatedBgStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientInspectRatedBgStats* msg);

    ClientInspectRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientInspectRatedBgStats();
    bool Handle();
};

class ClientItemExpirePurchaseRefund : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemExpirePurchaseRefund* msg);

    ClientItemExpirePurchaseRefund(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemExpirePurchaseRefund();
    bool Handle();
};

class ClientLfGuildMembershipListUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildMembershipListUpdated* msg);

    ClientLfGuildMembershipListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildMembershipListUpdated();
    bool Handle();
};

class ClientGuildMemberRecipes : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMemberRecipes* msg);

    ClientGuildMemberRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMemberRecipes();
    bool Handle();
};

class ClientLfGuildRecruitListUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildRecruitListUpdated* msg);

    ClientLfGuildRecruitListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildRecruitListUpdated();
    bool Handle();
};

class ClientGuildReset : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReset* msg);

    ClientGuildReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReset();
    bool Handle();
};

class ClientGuildMembersWithRecipe : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMembersWithRecipe* msg);

    ClientGuildMembersWithRecipe(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMembersWithRecipe();
    bool Handle();
};

class ClientLfGuildBrowseUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildBrowseUpdated* msg);

    ClientLfGuildBrowseUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildBrowseUpdated();
    bool Handle();
};

class ClientVoidTransferResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidTransferResult* msg);

    ClientVoidTransferResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidTransferResult();
    bool Handle();
};

class ClientSpellinterruptlog : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSpellinterruptlog* msg);

    ClientSpellinterruptlog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSpellinterruptlog();
    bool Handle();
};

class ClientGuildRewardList : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRewardList* msg);

    ClientGuildRewardList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRewardList();
    bool Handle();
};

class ClientCurrencyLootRemoved : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCurrencyLootRemoved* msg);

    ClientCurrencyLootRemoved(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCurrencyLootRemoved();
    bool Handle();
};

class ClientCustomLoadScreen : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCustomLoadScreen* msg);

    ClientCustomLoadScreen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCustomLoadScreen();
    bool Handle();
};

class ClientCurrencyLootRestored : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCurrencyLootRestored* msg);

    ClientCurrencyLootRestored(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCurrencyLootRestored();
    bool Handle();
};

class ClientMessageBox : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMessageBox* msg);

    /* 0010 */ WDataStore m_text;

    ClientMessageBox(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMessageBox();
    bool Handle();
};

class ClientRequestCemeteryListResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRequestCemeteryListResponse* msg);

    ClientRequestCemeteryListResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRequestCemeteryListResponse();
    bool Handle();
};

class ClientGuildBankLogQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankLogQueryResults* msg);

    ClientGuildBankLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankLogQueryResults();
    bool Handle();
};

class ClientGuildRanks : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRanks* msg);

    ClientGuildRanks(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRanks();
    bool Handle();
};

class ClientGuildFlaggedForRename : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildFlaggedForRename* msg);

    ClientGuildFlaggedForRename(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildFlaggedForRename();
    bool Handle();
};

class ClientGuildReputationWeeklyCap : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReputationWeeklyCap* msg);

    ClientGuildReputationWeeklyCap(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReputationWeeklyCap();
    bool Handle();
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

    ClientWorldServerInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWorldServerInfo();
    bool Handle();
};

class ClientLfgUpdateStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgUpdateStatus* msg);

    ClientLfgUpdateStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgUpdateStatus();
    bool Handle();
};

class ClientDisplayGameError : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDisplayGameError* msg);

    ClientDisplayGameError(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDisplayGameError();
    bool Handle();
};

class ClientGameEventDebugLog : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGameEventDebugLog* msg);

    ClientGameEventDebugLog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGameEventDebugLog();
    bool Handle();
};

class ClientPartyInvite : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPartyInvite* msg);

    ClientPartyInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPartyInvite();
    bool Handle();
};

class ClientDebugRuneRegen : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDebugRuneRegen* msg);

    ClientDebugRuneRegen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDebugRuneRegen();
    bool Handle();
};

class ClientRatedBgStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRatedBgStats* msg);

    ClientRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRatedBgStats();
    bool Handle();
};

class ClientBattlefieldMgrEjectPending : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEjectPending* msg);

    ClientBattlefieldMgrEjectPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEjectPending();
    bool Handle();
};

class ClientGuildPermissionsQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildPermissionsQueryResults* msg);

    ClientGuildPermissionsQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildPermissionsQueryResults();
    bool Handle();
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

    ClientBattlefieldMgrEntryInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEntryInvite();
    bool Handle();
};

class ClientGuildAchievementDeleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementDeleted* msg);

    ClientGuildAchievementDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementDeleted();
    bool Handle();
};

class ClientBattlefieldStatusQueued : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusQueued* msg);

    ClientBattlefieldStatusQueued(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusQueued();
    bool Handle();
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

    ClientResearchComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientResearchComplete();
    bool Handle();
};

class ClientGuildNews : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNews* msg);

    ClientGuildNews(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNews();
    bool Handle();
};

class ClientSupercededSpells : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSupercededSpells* msg);

    ClientSupercededSpells(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSupercededSpells();
    bool Handle();
};

class ClientBattlefieldMgrStateChange : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrStateChange* msg);

    ClientBattlefieldMgrStateChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrStateChange();
    bool Handle();
};

class ClientSetDfFastLaunchResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetDfFastLaunchResult* msg);

    ClientSetDfFastLaunchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetDfFastLaunchResult();
    bool Handle();
};

class ClientLfGuildPostUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildPostUpdated* msg);

    ClientLfGuildPostUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildPostUpdated();
    bool Handle();
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

    ClientGuildAchievementMembers(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementMembers();
    bool Handle();
};

class ClientCommentatorPartyInfo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCommentatorPartyInfo* msg);

    ClientCommentatorPartyInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCommentatorPartyInfo();
    bool Handle();
};

class ClientUpdateActionButtons : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateActionButtons* msg);

    ClientUpdateActionButtons(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateActionButtons();
    bool Handle();
};

class ClientLfgJoinResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgJoinResult* msg);

    ClientLfgJoinResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgJoinResult();
    bool Handle();
};

class ClientItemRemovePassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemRemovePassive* msg);

    ClientItemRemovePassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRemovePassive();
    bool Handle();
};

class ClientGuildChallengeCompleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChallengeCompleted* msg);

    ClientGuildChallengeCompleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChallengeCompleted();
    bool Handle();
};

class ClientRoleChangedInform : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRoleChangedInform* msg);

    ClientRoleChangedInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRoleChangedInform();
    bool Handle();
};

class ClientArenaTeamCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientArenaTeamCommandResult* msg);

    ClientArenaTeamCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientArenaTeamCommandResult();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildChangeNameResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildChangeNameResult* msg);

    ClientGuildChangeNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildChangeNameResult();
    bool Handle();
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

    ClientGuildRoster(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRoster();
    bool Handle();
};

class ClientWargameCheckEntry : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWargameCheckEntry* msg);

    ClientWargameCheckEntry(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWargameCheckEntry();
    bool Handle();
};

class ClientGuildXp : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildXp* msg);

    ClientGuildXp(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildXp();
    bool Handle();
};

class ClientAreaTriggerMovementUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAreaTriggerMovementUpdate* msg);

    ClientAreaTriggerMovementUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAreaTriggerMovementUpdate();
    bool Handle();
};

class ClientMissileCancel : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMissileCancel* msg);

    ClientMissileCancel(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMissileCancel();
    bool Handle();
};

class ClientFeatureSystemStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientFeatureSystemStatus* msg);

    ClientFeatureSystemStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientFeatureSystemStatus();
    bool Handle();
};

class ClientPvpOptionsEnabled : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPvpOptionsEnabled* msg);

    ClientPvpOptionsEnabled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPvpOptionsEnabled();
    bool Handle();
};

class ClientWeeklyLastReset : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWeeklyLastReset* msg);

    ClientWeeklyLastReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWeeklyLastReset();
    bool Handle();
};

class ClientGuildPartyStateResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildPartyStateResponse* msg);

    ClientGuildPartyStateResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildPartyStateResponse();
    bool Handle();
};

class ClientBattlegroundPlayerJoined : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlegroundPlayerJoined* msg);

    ClientBattlegroundPlayerJoined(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlegroundPlayerJoined();
    bool Handle();
};

class ClientLoadCufProfiles : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLoadCufProfiles* msg);

    ClientLoadCufProfiles(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLoadCufProfiles();
    bool Handle();
};

class ClientXpGainAborted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientXpGainAborted* msg);

    ClientXpGainAborted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientXpGainAborted();
    bool Handle();
};

class ClientGuildAchievementEarned : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementEarned* msg);

    ClientGuildAchievementEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementEarned();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientVoidStorageTransferChanges : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageTransferChanges* msg);

    ClientVoidStorageTransferChanges(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageTransferChanges();
    bool Handle();
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

class ClientLfgSearchResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgSearchResult* msg);

    ClientLfgSearchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgSearchResult();
    bool Handle();
};

class ClientBattlefieldRatedInfo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldRatedInfo* msg);

    ClientBattlefieldRatedInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldRatedInfo();
    bool Handle();
};

class ClientLfGuildCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfGuildCommandResult* msg);

    ClientLfGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfGuildCommandResult();
    bool Handle();
};

class ClientLfgSlotInvalid : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgSlotInvalid* msg);

    ClientLfgSlotInvalid(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgSlotInvalid();
    bool Handle();
};

class ClientGuildAchievementData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildAchievementData* msg);

    ClientGuildAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildAchievementData();
    bool Handle();
};

class ClientDisenchantCredit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDisenchantCredit* msg);

    ClientDisenchantCredit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDisenchantCredit();
    bool Handle();
};

class ClientQuestGiverQuestComplete : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientQuestGiverQuestComplete* msg);

    ClientQuestGiverQuestComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientQuestGiverQuestComplete();
    bool Handle();
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

    ClientGuildCriteriaDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCriteriaDeleted();
    bool Handle();
};

class ClientMoneyNotify : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoneyNotify* msg);

    ClientMoneyNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoneyNotify();
    bool Handle();
};

class ClientLearnedSpell : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLearnedSpell* msg);

    ClientLearnedSpell(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLearnedSpell();
    bool Handle();
};

class ClientReforgeResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientReforgeResult* msg);

    ClientReforgeResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientReforgeResult();
    bool Handle();
};

class ClientAllAchievementData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAllAchievementData* msg);

    ClientAllAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAllAchievementData();
    bool Handle();
};

class ClientBattlefieldPlayerPositions : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldPlayerPositions* msg);

    ClientBattlefieldPlayerPositions(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldPlayerPositions();
    bool Handle();
};

class ClientBattlefieldStatusNeedconfirmation : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusNeedconfirmation* msg);

    ClientBattlefieldStatusNeedconfirmation(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusNeedconfirmation();
    bool Handle();
};

class ClientStartTimer : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientStartTimer* msg);

    ClientStartTimer(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientStartTimer();
    bool Handle();
};

class ClientBattlegroundPlayerLeft : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlegroundPlayerLeft* msg);

    ClientBattlegroundPlayerLeft(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlegroundPlayerLeft();
    bool Handle();
};

class ClientSetCurrency : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetCurrency* msg);

    /* 0010 */ int32_t m_quantity;
    /* 0014 */ int32_t m_type;
    /* 0018 */ bool m_suppressChatLog;
    /* 001C */ bool m_hasWeeklyQuantity;
    /* 0020 */ int32_t m_weeklyQuantity;
    /* 0024 */ bool m_hasTrackedQuantity;
    /* 0028 */ int32_t m_trackedQuantity;

    ClientSetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetCurrency();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientWargameRequestSent : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWargameRequestSent* msg);

    ClientWargameRequestSent(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWargameRequestSent();
    bool Handle();
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

    ClientBattlefieldMgrEntered(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEntered();
    bool Handle();
};

class ClientTradeStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTradeStatus* msg);

    ClientTradeStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTradeStatus();
    bool Handle();
};

class ClientPvpLogData : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPvpLogData* msg);

    ClientPvpLogData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPvpLogData();
    bool Handle();
};

class ClientGuildRanksUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildRanksUpdate* msg);

    ClientGuildRanksUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildRanksUpdate();
    bool Handle();
};

class ClientPetitionAlreadySigned : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPetitionAlreadySigned* msg);

    ClientPetitionAlreadySigned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPetitionAlreadySigned();
    bool Handle();
};

class ClientRequestPvpRewardsResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRequestPvpRewardsResponse* msg);

    ClientRequestPvpRewardsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRequestPvpRewardsResponse();
    bool Handle();
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

    ClientItemRefundResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemRefundResult();
    bool Handle();
};

class ClientGuildBankMoneyWithdrawn : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankMoneyWithdrawn* msg);

    ClientGuildBankMoneyWithdrawn(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankMoneyWithdrawn();
    bool Handle();
};

class ClientPhaseShiftChange : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPhaseShiftChange* msg);

    ClientPhaseShiftChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPhaseShiftChange();
    bool Handle();
};

class ClientTradeUpdated : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTradeUpdated* msg);

    ClientTradeUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTradeUpdated();
    bool Handle();
};

class ClientGuildMoveStarting : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMoveStarting* msg);

    ClientGuildMoveStarting(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMoveStarting();
    bool Handle();
};

class ClientUpdateCurrencyWeekLimit : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientUpdateCurrencyWeekLimit* msg);

    /* 0010 */ int32_t m_type;
    /* 0014 */ int32_t m_weekCap;

    ClientUpdateCurrencyWeekLimit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientUpdateCurrencyWeekLimit();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientRolePollBegin : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientRolePollBegin* msg);

    ClientRolePollBegin(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientRolePollBegin();
    bool Handle();
};

class ClientItemSendPassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemSendPassive* msg);

    ClientItemSendPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemSendPassive();
    bool Handle();
};

class ClientForgeMasterSet : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientForgeMasterSet* msg);

    ClientForgeMasterSet(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientForgeMasterSet();
    bool Handle();
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

    ClientBattlefieldStatusFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusFailed();
    bool Handle();
};

class ClientBattlefieldList : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldList* msg);

    ClientBattlefieldList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldList();
    bool Handle();
};

class ClientCategoryCooldown : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientCategoryCooldown* msg);

    ClientCategoryCooldown(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientCategoryCooldown();
    bool Handle();
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

    ClientBattlefieldStatusActive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusActive();
    bool Handle();
};

class ClientGuildNameChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNameChanged* msg);

    ClientGuildNameChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNameChanged();
    bool Handle();
};

class ClientGuildNewsDeleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildNewsDeleted* msg);

    ClientGuildNewsDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildNewsDeleted();
    bool Handle();
};

class ClientGuildReputationReactionChanged : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildReputationReactionChanged* msg);

    ClientGuildReputationReactionChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildReputationReactionChanged();
    bool Handle();
};

class ClientServerInfoResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientServerInfoResponse* msg);

    ClientServerInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientServerInfoResponse();
    bool Handle();
};

class ClientServerPerf : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientServerPerf* msg);

    ClientServerPerf(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientServerPerf();
    bool Handle();
};

class ClientQuestNpcQueryResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientQuestNpcQueryResponse* msg);

    ClientQuestNpcQueryResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientQuestNpcQueryResponse();
    bool Handle();
};

class ClientBattlefieldStatusWaitforgroups : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatusWaitforgroups* msg);

    ClientBattlefieldStatusWaitforgroups(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatusWaitforgroups();
    bool Handle();
};

class ClientGuildBankTextQueryResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankTextQueryResult* msg);

    ClientGuildBankTextQueryResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankTextQueryResult();
    bool Handle();
};

class ClientVoidStorageContents : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidStorageContents* msg);

    ClientVoidStorageContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidStorageContents();
    bool Handle();
};

class ClientVoidItemSwapResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientVoidItemSwapResponse* msg);

    ClientVoidItemSwapResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVoidItemSwapResponse();
    bool Handle();
};

class ClientGuildBankQueryResults : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildBankQueryResults* msg);

    ClientGuildBankQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildBankQueryResults();
    bool Handle();
};

class ClientLfgQueueStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgQueueStatus* msg);

    ClientLfgQueueStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgQueueStatus();
    bool Handle();
};

class ClientInspectHonorStats : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientInspectHonorStats* msg);

    ClientInspectHonorStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientInspectHonorStats();
    bool Handle();
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
    void Unpack(CDataStore* msg, bool full);
};

class ClientGuildMaxDailyXp : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMaxDailyXp* msg);

    ClientGuildMaxDailyXp(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMaxDailyXp();
    bool Handle();
};

class ClientBattlefieldMgrQueueRequestResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrQueueRequestResponse* msg);

    ClientBattlefieldMgrQueueRequestResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrQueueRequestResponse();
    bool Handle();
};

class ClientTimeAdjustment : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientTimeAdjustment* msg);

    ClientTimeAdjustment(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientTimeAdjustment();
    bool Handle();
};

class ClientGuildMemberUpdateNote : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildMemberUpdateNote* msg);

    ClientGuildMemberUpdateNote(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildMemberUpdateNote();
    bool Handle();
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
    /* 0028 */ uint8_t m_vendor[8];

    ClientVendorInventory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientVendorInventory();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientItemAddPassive : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientItemAddPassive* msg);

    ClientItemAddPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientItemAddPassive();
    bool Handle();
};

class ClientBattlefieldStatus : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldStatus* msg);

    ClientBattlefieldStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldStatus();
    bool Handle();
};

class ClientLfgProposalUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientLfgProposalUpdate* msg);

    ClientLfgProposalUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientLfgProposalUpdate();
    bool Handle();
};

class ClientGuildCommandResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGuildCommandResult* msg);

    ClientGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGuildCommandResult();
    bool Handle();
};

class ClientBattlefieldMgrEjected : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientBattlefieldMgrEjected* msg);

    ClientBattlefieldMgrEjected(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientBattlefieldMgrEjected();
    bool Handle();
};

#endif
