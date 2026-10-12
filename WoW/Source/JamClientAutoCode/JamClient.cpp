#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 9574b3a0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
JamGuildRewardItem::~JamGuildRewardItem() {
}

// PROVEN: 95dd8177 IDENTICAL/IDENTICAL
// OFFSET: [0x00401000/0x1407D7E80]
JamCliSupportTicketHeader::~JamCliSupportTicketHeader() {
}

// PROVEN: 2029ff97 IDENTICAL/IDENTICAL
// OFFSET: [0x00401000/0x1407D7E80]
JamCliStructAddonPrefix::~JamCliStructAddonPrefix() {
}

// PROVEN: b18bd204 IDENTICAL/IDENTICAL
// OFFSET: [0x0066BC00/0x140312430]
JamCliStructAddonPrefix::JamCliStructAddonPrefix() {
}

// PROVEN: a1983157 IDENTICAL/IDENTICAL
// OFFSET: 0x00696AB0/0x140342100
void JamCliSupportTicketHeader::Put(CDataStore* msg) const {
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.z);
    msg->Put(this->m_position.x);
    msg->Put(this->m_mapID);
    msg->Put(this->m_facing);
}

// PROVEN: 08bcfab4 IDENTICAL/IDENTICAL
// OFFSET: 0x00696E60/0x1403424C0
JamCliSupportTicketHeader::JamCliSupportTicketHeader() {
}

// OFFSET: 0x00696FD0
void JamCliSupportTicketChatLine::operator=(const JamCliSupportTicketChatLine& source) {
    SStrCopy(this->m_text, source.m_text, sizeof(this->m_text));
    this->m_timestamp = source.m_timestamp;
}

// PROVEN: 95e2bdc9 IDENTICAL/DIFFERENT
// OFFSET: 0x00697DB0/0x140343470
void JamCliSupportTicketChatLog::Put(CDataStore* msg) const {
    JamBitWriter bits(msg);
    bits.WriteBit(this->m_hasReportLineIndex);
    bits.WriteBits<22>(this->m_lines.m_count);

    for (uint32_t i = 0; i < this->m_lines.m_count; i++) {
        bits.WriteBits<13>(strlen(this->m_lines.m_data[i].m_text));
    }

    bits.Flush();

    for (uint32_t i = 0; i < this->m_lines.m_count; i++) {
        msg->Put(this->m_lines.m_data[i].m_timestamp);
        msg->PutData(this->m_lines.m_data[i].m_text, strlen(this->m_lines.m_data[i].m_text));
    }

    if (this->m_hasReportLineIndex) {
        msg->Put(this->m_reportLineIndex);
    }
}

// PROVEN: fc45a404 IDENTICAL/IDENTICAL
// OFFSET: 0x006981E0/0x140343A20
JamCliSupportTicketChatLog::JamCliSupportTicketChatLog() {
    this->m_hasReportLineIndex = false;
}

// PROVEN: 39f2939a NONE/IDENTICAL
// OFFSET: 0x006982F0/0x140343AC0
JamCliSupportTicketChatLog::~JamCliSupportTicketChatLog() {
}

// PROVEN: b86a0fde IDENTICAL/IDENTICAL
// OFFSET: 0x006A2770/0x14034FC80
void JamGuildRewardItem::operator=(const JamGuildRewardItem& other) {
    this->m_cost = other.m_cost;
    this->m_minGuildLevel = other.m_minGuildLevel;
    this->m_minGuildRep = other.m_minGuildRep;
    this->m_itemID = other.m_itemID;
    this->m_raceMask = other.m_raceMask;
    this->m_achievementRequired = other.m_achievementRequired;
}

// PROVEN: c4a66ca1 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8460/0x14037F500
JamGuildNewsEvent::JamGuildNewsEvent() {
}

// PROVEN: 78cb8955 IDENTICAL/IDENTICAL
// OFFSET: 0x006D3D30/0x140390240
JamGuildNewsEvent::~JamGuildNewsEvent() {
}

// OFFSET: 0x006D9F30/0x140398530
void JamGuildNewsEvent::operator=(const JamGuildNewsEvent& other) {
    this->m_date = other.m_date;
    this->m_memberList = other.m_memberList;
    this->m_memberGUID = other.m_memberGUID;
    this->m_id = other.m_id;
    this->m_type = other.m_type;
    this->m_flags = other.m_flags;
    this->m_data[0] = other.m_data[0];
    this->m_data[1] = other.m_data[1];
}

// STATIC: 0x00E4D570/0x140D364B8
bool (*ClientPartyInvite::s_handler)(ClientPartyInvite* msg);

// STATIC: 0x00E4D680/0x140D366D8
bool (*ClientGuildRoster::s_handler)(ClientGuildRoster* msg);

// STATIC: 0x00E4D450/0x140D36278
bool (*ClientGuildRosterUpdate::s_handler)(ClientGuildRosterUpdate* msg);

// STATIC: 0x00E4D5B4/0x140D36540
bool (*ClientGuildRewardList::s_handler)(ClientGuildRewardList* msg);

// STATIC: 0x00E4D820/0x140D36A18
bool (*ClientGuildNews::s_handler)(ClientGuildNews* msg);

// STATIC: 0x00E4D464/0x140D362A0
bool (*ClientGuildNewsDeleted::s_handler)(ClientGuildNewsDeleted* msg);

// STATIC: 0x00E4D7DC/0x140D36990
bool (*ClientGuildReputationWeeklyCap::s_handler)(ClientGuildReputationWeeklyCap* msg);

// STATIC: 0x00E4D770/0x140D368B8
bool (*ClientGuildXPEarned::s_handler)(ClientGuildXPEarned* msg);

// STATIC: 0x00E4D6B4/0x140D36740
bool (*ClientGuildBankMoneyWithdrawn::s_handler)(ClientGuildBankMoneyWithdrawn* msg);

// STATIC: 0x00E4D810/0x140D369F8
bool (*ClientVoidStorageContents::s_handler)(ClientVoidStorageContents* msg);

// STATIC: 0x00E4D59C/0x140D36510
bool (*ClientVoidStorageFailed::s_handler)(ClientVoidStorageFailed* msg);

// STATIC: 0x00E4D7C4/0x140D36960
bool (*ClientVoidStorageTransferChanges::s_handler)(ClientVoidStorageTransferChanges* msg);

// STATIC: 0x00E4D5A4/0x140D36520
bool (*ClientVoidTransferResult::s_handler)(ClientVoidTransferResult* msg);

// STATIC: 0x00E4D468/0x140D362A8
bool (*ClientLfGuildPostUpdated::s_handler)(ClientLfGuildPostUpdated* msg);

// STATIC: 0x00E4D5CC/0x140D36570
bool (*ClientSetupResearchHistory::s_handler)(ClientSetupResearchHistory* msg);

// STATIC: 0x00E4D6E0/0x140D36798
bool (*ClientResearchComplete::s_handler)(ClientResearchComplete* msg);

// STATIC: 0x00E4D598/0x140D36508
bool (*ClientLfGuildBrowseUpdated::s_handler)(ClientLfGuildBrowseUpdated* msg);

// STATIC: 0x00E4D718/0x140D36808
bool (*ClientLfGuildRecruitListUpdated::s_handler)(ClientLfGuildRecruitListUpdated* msg);

// STATIC: 0x00E4D508/0x140D363E8
bool (*ClientLfGuildCommandResult::s_handler)(ClientLfGuildCommandResult* msg);

// STATIC: 0x00E4D664/0x140D366A0
bool (*ClientLfGuildMembershipListUpdated::s_handler)(ClientLfGuildMembershipListUpdated* msg);

// STATIC: 0x00E4D704/0x140D367E0
bool (*ClientLfGuildApplicantListUpdated::s_handler)(ClientLfGuildApplicantListUpdated* msg);

// STATIC: 0x00E4D6FC/0x140D367D0
bool (*ClientLfGuildApplicationsListChanged::s_handler)(ClientLfGuildApplicationsListChanged* msg);

// STATIC: 0x00E4D5A8/0x140D36528
bool (*ClientVoidItemSwapResponse::s_handler)(ClientVoidItemSwapResponse* msg);

// STATIC: 0x00E4D80C/0x140D369F0
bool (*ClientReforgeResult::s_handler)(ClientReforgeResult* msg);

// STATIC: 0x00E4D494/0x140D36300
bool (*ClientForgeMasterSet::s_handler)(ClientForgeMasterSet* msg);

// STATIC: 0x00E4D60C/0x140D365F0
bool (*ClientGuildChangeNameResult::s_handler)(ClientGuildChangeNameResult* msg);

// STATIC: 0x00E4D51C/0x140D36410
bool (*ClientGuildFlaggedForRename::s_handler)(ClientGuildFlaggedForRename* msg);

// STATIC: 0x00E4D4D8/0x140D36388
bool (*ClientGuildRanks::s_handler)(ClientGuildRanks* msg);

// STATIC: 0x00E4D7B8/0x140D36948
bool (*ClientGuildMemberUpdateNote::s_handler)(ClientGuildMemberUpdateNote* msg);

// STATIC: 0x00E4D4C0/0x140D36358
bool (*ClientGuildInvite::s_handler)(ClientGuildInvite* msg);

// STATIC: 0x00E4D690/0x140D366F8
bool (*ClientGuildPartyState::s_handler)(ClientGuildPartyState* msg);

// STATIC: 0x00E4D780/0x140D368D8
bool (*ClientGuildReputationReactionChanged::s_handler)(ClientGuildReputationReactionChanged* msg);

// STATIC: 0x00E4D7BC/0x140D36950
bool (*ClientGuildChallengeUpdate::s_handler)(ClientGuildChallengeUpdate* msg);

// STATIC: 0x00E4D580/0x140D364D8
bool (*ClientGuildChallengeCompleted::s_handler)(ClientGuildChallengeCompleted* msg);

// STATIC: 0x00E4D4C4/0x140D36360
bool (*ClientGuildReset::s_handler)(ClientGuildReset* msg);

// STATIC: 0x00E4D670/0x140D366B8
bool (*ClientGuildMoveComplete::s_handler)(ClientGuildMoveComplete* msg);

// STATIC: 0x00E4D55C/0x140D36490
bool (*ClientGuildMoveStarting::s_handler)(ClientGuildMoveStarting* msg);

// STATIC: 0x00E4D594/0x140D36500
bool (*ClientGuildNameChanged::s_handler)(ClientGuildNameChanged* msg);

// STATIC: 0x00E4D430/0x140D36238
bool (*ClientGuildPermissionsQueryResults::s_handler)(ClientGuildPermissionsQueryResults* msg);

// STATIC: 0x00E4D474/0x140D362C0
bool (*ClientGuildEventLogQueryResults::s_handler)(ClientGuildEventLogQueryResults* msg);

// STATIC: 0x00E4D65C/0x140D36690
bool (*ClientGuildMemberDailyReset::s_handler)(ClientGuildMemberDailyReset* msg);

// STATIC: 0x00E4D500/0x140D363D8
bool (*ClientGuildBankQueryResults::s_handler)(ClientGuildBankQueryResults* msg);

// STATIC: 0x00E4D710/0x140D367F8
bool (*ClientGuildBankLogQueryResults::s_handler)(ClientGuildBankLogQueryResults* msg);

// STATIC: 0x00E4D624/0x140D36620
bool (*ClientGuildBankTextQueryResult::s_handler)(ClientGuildBankTextQueryResult* msg);

// STATIC: 0x00E4D6A8/0x140D36728
bool (*ClientTradeStatus::s_handler)(ClientTradeStatus* msg);

// STATIC: 0x00E4D56C/0x140D364B0
bool (*ClientTradeUpdated::s_handler)(ClientTradeUpdated* msg);

// STATIC: 0x00E4D694/0x140D36700
bool (*ClientSetupCurrency::s_handler)(ClientSetupCurrency* msg);

// STATIC: 0x00E4D728/0x140D36828
bool (*ClientSetCurrency::s_handler)(ClientSetCurrency* msg);

// STATIC: 0x00E4D644/0x140D36660
bool (*ClientWeeklyResetCurrency::s_handler)(ClientWeeklyResetCurrency* msg);

// STATIC: 0x00E4D42C/0x140D36230
bool (*ClientUpdateCurrencyWeekLimit::s_handler)(ClientUpdateCurrencyWeekLimit* msg);

// STATIC: 0x00E4D734/0x140D36840
bool (*ClientVendorInventory::s_handler)(ClientVendorInventory* msg);

// STATIC: 0x00E4D518/0x140D36408
bool (*ClientPetSlotUpdated::s_handler)(ClientPetSlotUpdated* msg);

// STATIC: 0x00E4D66C/0x140D366B0
bool (*ClientPetAdded::s_handler)(ClientPetAdded* msg);

// STATIC: 0x00E4D7D0/0x140D36978
bool (*ClientAuthResponse::s_handler)(ClientAuthResponse* msg);

// STATIC: 0x00E4D4B4/0x140D36340
bool (*ClientWaitQueueUpdate::s_handler)(ClientWaitQueueUpdate* msg);

// STATIC: 0x00E4D52C/0x140D36430
bool (*ClientWaitQueueFinish::s_handler)(ClientWaitQueueFinish* msg);

// STATIC: 0x00E4D758/0x140D36888
bool (*ClientGenerateRandomCharacterNameResult::s_handler)(ClientGenerateRandomCharacterNameResult* msg);

// STATIC: 0x00E4D5E0/0x140D36598
bool (*ClientEnumCharactersResult::s_handler)(ClientEnumCharactersResult* msg);

// STATIC: 0x00E4D7E0/0x140D36998
bool (*ClientDBReply::s_handler)(ClientDBReply* msg);

// STATIC: 0x00E4D674/0x140D366C0
bool (*ClientHotfixNotify::s_handler)(ClientHotfixNotify* msg);

// STATIC: 0x00E4D6D8/0x140D36788
bool (*ClientHotfixNotifyBlob::s_handler)(ClientHotfixNotifyBlob* msg);

// STATIC: 0x00E4D5BC/0x140D36550
bool (*ClientGameObjectActivateAnimKit::s_handler)(ClientGameObjectActivateAnimKit* msg);

// STATIC: 0x00E4D520/0x140D36418
bool (*ClientMapObjEvents::s_handler)(ClientMapObjEvents* msg);

// STATIC: 0x00E4D5B0/0x140D36538
bool (*ClientNotification::s_handler)(ClientNotification* msg);

// STATIC: 0x00E4D440/0x140D36258
bool (*ClientPhaseShiftChange::s_handler)(ClientPhaseShiftChange* msg);

// STATIC: 0x00E4D804/0x140D369E0
bool (*ClientLfgSearchResult::s_handler)(ClientLfgSearchResult* msg);

// STATIC: 0x00E4D460/0x140D36298
bool (*ClientSuspendToken::s_handler)(ClientSuspendToken* msg);

// STATIC: 0x00E4D764/0x140D368A0
bool (*ClientNewWorldAbort::s_handler)(ClientNewWorldAbort* msg);

// STATIC: 0x00E4D590/0x140D364F8
bool (*ClientTransferPending::s_handler)(ClientTransferPending* msg);

// STATIC: 0x00E4D54C/0x140D36470
bool (*ClientMessageBox::s_handler)(ClientMessageBox* msg);

// STATIC: 0x00E4D784/0x140D368E0
bool (*ClientNewWorld::s_handler)(ClientNewWorld* msg);

// STATIC: 0x00E4D768/0x140D368A8
bool (*ClientAccountRestrictedWarning::s_handler)(ClientAccountRestrictedWarning* msg);

// STATIC: 0x00E4D7F8/0x140D369C8
bool (*ClientAreaTriggerMovementUpdate::s_handler)(ClientAreaTriggerMovementUpdate* msg);

// STATIC: 0x00E4D448/0x140D36268
bool (*ClientAverageItemLevelInform::s_handler)(ClientAverageItemLevelInform* msg);

// STATIC: 0x00E4D5D8/0x140D36588
bool (*ClientBattlefieldMgrEjectPending::s_handler)(ClientBattlefieldMgrEjectPending* msg);

// STATIC: 0x00E4D558/0x140D36488
bool (*ClientBattlefieldMgrEjected::s_handler)(ClientBattlefieldMgrEjected* msg);

// STATIC: 0x00E4D774/0x140D368C0
bool (*ClientBattlefieldMgrEntryInvite::s_handler)(ClientBattlefieldMgrEntryInvite* msg);

// STATIC: 0x00E4D7FC/0x140D369D0
bool (*ClientBattlefieldMgrExitRequest::s_handler)(ClientBattlefieldMgrExitRequest* msg);

// STATIC: 0x00E4D6E4/0x140D367A0
bool (*ClientBattlefieldMgrStateChange::s_handler)(ClientBattlefieldMgrStateChange* msg);

// STATIC: 0x00E4D720/0x140D36818
bool (*ClientBattlefieldPortDenied::s_handler)(ClientBattlefieldPortDenied* msg);

// STATIC: 0x00E4D5F0/0x140D365B8
bool (*ClientBattlefieldRatedInfo::s_handler)(ClientBattlefieldRatedInfo* msg);

// STATIC: 0x00E4D814/0x140D36A00
bool (*ClientBattlefieldStatusFailed::s_handler)(ClientBattlefieldStatusFailed* msg);

// STATIC: 0x00E4D6AC/0x140D36730
bool (*ClientBattlefieldStatusQueued::s_handler)(ClientBattlefieldStatusQueued* msg);

// STATIC: 0x00E4D6EC/0x140D367B0
bool (*ClientBattlefieldStatusWaitforgroups::s_handler)(ClientBattlefieldStatusWaitforgroups* msg);

// STATIC: 0x00E4D610/0x140D365F8
bool (*ClientBattlegroundPlayerJoined::s_handler)(ClientBattlegroundPlayerJoined* msg);

// STATIC: 0x00E4D6F0/0x140D367B8
bool (*ClientBattlegroundPlayerLeft::s_handler)(ClientBattlegroundPlayerLeft* msg);

// STATIC: 0x00E4D650/0x140D36678
bool (*ClientClearBossEmotes::s_handler)(ClientClearBossEmotes* msg);

// STATIC: 0x00E4D6A4/0x140D36720
bool (*ClientCustomLoadScreen::s_handler)(ClientCustomLoadScreen* msg);

// STATIC: 0x00E4D47C/0x140D362D0
bool (*ClientDifferentInstanceFromParty::s_handler)(ClientDifferentInstanceFromParty* msg);

// STATIC: 0x00E4D5C0/0x140D36558
bool (*ClientDontAutoPushSpellsToActionBar::s_handler)(ClientDontAutoPushSpellsToActionBar* msg);

// STATIC: 0x00E4D544/0x140D36460
bool (*ClientFailedPlayerCondition::s_handler)(ClientFailedPlayerCondition* msg);

// STATIC: 0x00E4D7F0/0x140D369B8
bool (*ClientInspectHonorStats::s_handler)(ClientInspectHonorStats* msg);

// STATIC: 0x00E4D4F0/0x140D363B8
bool (*ClientInspectRatedBgStats::s_handler)(ClientInspectRatedBgStats* msg);

// STATIC: 0x00E4D454/0x140D36280
bool (*ClientLfgSlotInvalid::s_handler)(ClientLfgSlotInvalid* msg);

// STATIC: 0x00E4D470/0x140D362B8
bool (*ClientLfgUpdateStatusNone::s_handler)(ClientLfgUpdateStatusNone* msg);

// STATIC: 0x00E4D554/0x140D36480
bool (*ClientMissileCancel::s_handler)(ClientMissileCancel* msg);

// STATIC: 0x00E4D6C0/0x140D36758
bool (*ClientMoneyNotify::s_handler)(ClientMoneyNotify* msg);

// STATIC: 0x00E4D49C/0x140D36310
bool (*ClientRatedBgRating::s_handler)(ClientRatedBgRating* msg);

// STATIC: 0x00E4D4B8/0x140D36348
bool (*ClientRatedBgStats::s_handler)(ClientRatedBgStats* msg);

// STATIC: 0x00E4D6A0/0x140D36718
bool (*ClientPvpLogData::s_handler)(ClientPvpLogData* msg);

// STATIC: 0x00E4D620/0x140D36618
bool (*ClientRequestPvpRewardsResponse::s_handler)(ClientRequestPvpRewardsResponse* msg);

// STATIC: 0x00E4D630/0x140D36638
bool (*ClientSetDfFastLaunchResult::s_handler)(ClientSetDfFastLaunchResult* msg);

// STATIC: 0x00E4D6B8/0x140D36748
bool (*ClientUpdateDungeonEncounterForLoot::s_handler)(ClientUpdateDungeonEncounterForLoot* msg);

// STATIC: 0x00E4D4F4/0x140D363C0
bool (*ClientUpdateServerPlayerPosition::s_handler)(ClientUpdateServerPlayerPosition* msg);

// STATIC: 0x00E4D75C/0x140D36890
bool (*ClientWardenData::s_handler)(ClientWardenData* msg);

// STATIC: 0x00E4D7C8/0x140D36968
bool (*ClientWargameCheckEntry::s_handler)(ClientWargameCheckEntry* msg);

// STATIC: 0x00E4D7C0/0x140D36958
bool (*ClientWargameRequestSent::s_handler)(ClientWargameRequestSent* msg);

// STATIC: 0x00E4D7CC/0x140D36970
bool (*ClientWeeklyLastReset::s_handler)(ClientWeeklyLastReset* msg);

// STATIC: 0x00E4D658/0x140D36688
bool (*ClientBattlefieldMgrEntered::s_handler)(ClientBattlefieldMgrEntered* msg);

// STATIC: 0x00E4D5F4/0x140D365C0
bool (*ClientBattlefieldMgrQueueInvite::s_handler)(ClientBattlefieldMgrQueueInvite* msg);

// STATIC: 0x00E4D7B0/0x140D36938
bool (*ClientBattlefieldMgrQueueRequestResponse::s_handler)(ClientBattlefieldMgrQueueRequestResponse* msg);

// STATIC: 0x00E4D648/0x140D36668
bool (*ClientBattlefieldStatus::s_handler)(ClientBattlefieldStatus* msg);

// STATIC: 0x00E4D724/0x140D36820
bool (*ClientBattlefieldStatusActive::s_handler)(ClientBattlefieldStatusActive* msg);

// STATIC: 0x00E4D480/0x140D362D8
bool (*ClientDisplayGameError::s_handler)(ClientDisplayGameError* msg);

// STATIC: 0x00E4D760/0x140D36898
bool (*ClientPvpOptionsEnabled::s_handler)(ClientPvpOptionsEnabled* msg);

// STATIC: 0x00E4D504/0x140D363E0
bool (*ClientShowRatings::s_handler)(ClientShowRatings* msg);

// STATIC: 0x00E4D548/0x140D36468
bool (*ClientBattlefieldStatusNeedconfirmation::s_handler)(ClientBattlefieldStatusNeedconfirmation* msg);

// STATIC: 0x00E4D584/0x140D364E0
bool (*ClientGameEventDebugLog::s_handler)(ClientGameEventDebugLog* msg);

// STATIC: 0x00E4D5DC/0x140D36590
bool (*ClientGuildAchievementDeleted::s_handler)(ClientGuildAchievementDeleted* msg);

// STATIC: 0x00E4D4C8/0x140D36368
bool (*ClientGuildAchievementEarned::s_handler)(ClientGuildAchievementEarned* msg);

// STATIC: 0x00E4D45C/0x140D36290
bool (*ClientGuildCriteriaDeleted::s_handler)(ClientGuildCriteriaDeleted* msg);

// STATIC: 0x00E4D534/0x140D36440
bool (*ClientLfgQueueStatus::s_handler)(ClientLfgQueueStatus* msg);

// STATIC: 0x00E4D6DC/0x140D36790
bool (*ClientServerInfoResponse::s_handler)(ClientServerInfoResponse* msg);

// STATIC: 0x00E4D538/0x140D36448
bool (*ClientServerPerf::s_handler)(ClientServerPerf* msg);

// STATIC: 0x00E4D46C/0x140D362B0
bool (*ClientStreamingMoviesUpdate::s_handler)(ClientStreamingMoviesUpdate* msg);

// STATIC: 0x00E4D5F8/0x140D365C8
bool (*ClientLoadCufProfiles::s_handler)(ClientLoadCufProfiles* msg);

// STATIC: 0x00E4D4DC/0x140D36390
bool (*ClientAccountInfoResponse::s_handler)(ClientAccountInfoResponse* msg);

// STATIC: 0x00E4D61C/0x140D36610
bool (*ClientDumpRideTicketsResponse::s_handler)(ClientDumpRideTicketsResponse* msg);

// STATIC: 0x00E4D50C/0x140D363F0
bool (*ClientGuildCriteriaData::s_handler)(ClientGuildCriteriaData* msg);

// STATIC: 0x00E4D684/0x140D366E0
bool (*ClientLfgUpdateStatus::s_handler)(ClientLfgUpdateStatus* msg);

// STATIC: 0x00E4D618/0x140D36608
bool (*ClientRespondInspectAchievements::s_handler)(ClientRespondInspectAchievements* msg);

// STATIC: 0x00E4D5E8/0x140D365A8
bool (*ClientAllAchievementData::s_handler)(ClientAllAchievementData* msg);

// STATIC: 0x00E4D7E8/0x140D369A8
bool (*ClientBattlefieldList::s_handler)(ClientBattlefieldList* msg);

// STATIC: 0x00E4D428/0x140D36228
bool (*ClientBattlefieldPlayerPositions::s_handler)(ClientBattlefieldPlayerPositions* msg);

// STATIC: 0x00E4D4A0/0x140D36318
bool (*ClientCommentatorPartyInfo::s_handler)(ClientCommentatorPartyInfo* msg);

// STATIC: 0x00E4D738/0x140D36848
bool (*ClientGuildAchievementData::s_handler)(ClientGuildAchievementData* msg);

// STATIC: 0x00E4D510/0x140D363F8
bool (*ClientGuildAchievementMembers::s_handler)(ClientGuildAchievementMembers* msg);

// STATIC: 0x00E4D478/0x140D362C8
bool (*ClientLfgJoinResult::s_handler)(ClientLfgJoinResult* msg);

// STATIC: 0x00E4D6CC/0x140D36770
bool (*ClientLfgProposalUpdate::s_handler)(ClientLfgProposalUpdate* msg);

// OFFSET: 0x006C98F0/0x140381330
ClientAuthResponse::ClientAuthResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasSuccessInfo = false;
    this->m_hasWaitInfo = false;
    this->Unpack(msg, true);
}

// PROVEN: 86b6f543 IDENTICAL/IDENTICAL
// OFFSET: 0x006CCA80/0x140385BD0
ClientAuthResponse::~ClientAuthResponse() {
}

// PROVEN: d0b945c8 IDENTICAL/IDENTICAL
// OFFSET: 0x006AD4C0/0x14035D6C0
void ClientAuthResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasWaitInfo = bits.ReadBit() != 0;

    if (this->m_hasWaitInfo) {
        this->m_freeCharacterMigration = bits.ReadBit() != 0;
    }

    this->m_hasSuccessInfo = bits.ReadBit();

    if (this->m_hasSuccessInfo) {
        {
            uint32_t billingTimeRemaining = 0;
            msg->Get(billingTimeRemaining);
            this->m_billingTimeRemaining = billingTimeRemaining;
        }

        {
            uint8_t accountExpansion = 0;
            msg->Get(accountExpansion);
            this->m_accountExpansion = accountExpansion;
        }

        {
            uint32_t timeRemaining = 0;
            msg->Get(timeRemaining);
            this->m_timeRemaining = timeRemaining;
        }

        {
            uint8_t activeExpansion = 0;
            msg->Get(activeExpansion);
            this->m_activeExpansion = activeExpansion;
        }

        {
            uint32_t billingTimeRested = 0;
            msg->Get(billingTimeRested);
            this->m_billingTimeRested = billingTimeRested;
        }

        {
            uint8_t billingFlags = 0;
            msg->Get(billingFlags);
            this->m_billingFlags = billingFlags;
        }
    }

    {
        uint8_t result = 0;
        msg->Get(result);
        this->m_result = result;
    }

    if (this->m_hasWaitInfo) {
        uint32_t waitCount = 0;
        msg->Get(waitCount);
        this->m_waitCount = waitCount;
    }
}

// PROVEN: 3f03c521 IDENTICAL/UNPROVEN
// OFFSET: 0x006D7380/0x140395030
bool ClientAuthResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 39322324 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB520/0x140383B80
bool ClientAuthResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasWaitInfo = bits.ReadBit() != 0;

    if (this->m_hasWaitInfo) {
        this->m_freeCharacterMigration = bits.ReadBit() != 0;
    }

    this->m_hasSuccessInfo = bits.ReadBit();

    if (this->m_hasSuccessInfo) {
        {
            uint32_t billingTimeRemaining = 0;
            msg->Get(billingTimeRemaining);
            this->m_billingTimeRemaining = billingTimeRemaining;
        }

        {
            uint8_t accountExpansion = 0;
            msg->Get(accountExpansion);
            this->m_accountExpansion = accountExpansion;
        }

        {
            uint32_t timeRemaining = 0;
            msg->Get(timeRemaining);
            this->m_timeRemaining = timeRemaining;
        }

        {
            uint8_t activeExpansion = 0;
            msg->Get(activeExpansion);
            this->m_activeExpansion = activeExpansion;
        }

        {
            uint32_t billingTimeRested = 0;
            msg->Get(billingTimeRested);
            this->m_billingTimeRested = billingTimeRested;
        }

        {
            uint8_t billingFlags = 0;
            msg->Get(billingFlags);
            this->m_billingFlags = billingFlags;
        }
    }

    {
        uint8_t result = 0;
        msg->Get(result);
        this->m_result = result;
    }

    if (this->m_hasWaitInfo) {
        uint32_t waitCount = 0;
        msg->Get(waitCount);
        this->m_waitCount = waitCount;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 737aa6a1 IDENTICAL/UNPROVEN
// OFFSET: 0x006B72D0/0x14036A040
bool ClientAuthResponse::Handle() {
    if (ClientAuthResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF274.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF274.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAuthResponse*)>(reinterpret_cast<uintptr_t>(ClientAuthResponse::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2e6a7da6 IDENTICAL/UNPROVEN
// OFFSET: 0x006AA2E0/0x140364670
ClientWaitQueueUpdate::ClientWaitQueueUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint32_t waitCount = 0;
        msg->Get(waitCount);
        this->m_waitCount = waitCount;
    }

    {
        uint8_t bits = 0;
        msg->Get(bits);
        this->m_freeCharacterMigration = bits >> 7;
    }
}

// PROVEN: a94edac9 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1CD0/0x14034F120
ClientWaitQueueUpdate::~ClientWaitQueueUpdate() {
}

// PROVEN: df58f9e3 IDENTICAL/UNPROVEN
// OFFSET: 0x006CF330/0x14038E350
bool ClientWaitQueueUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9dde5a5c IDENTICAL/DIFFERENT
// OFFSET: 0x006BFF20/0x14037B2C0
bool ClientWaitQueueUpdate::Read(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_waitCount = value;

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_freeCharacterMigration = value >> 7;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 0bb1827f IDENTICAL/UNPROVEN
// OFFSET: 0x006B2BD0/0x1403643A0
bool ClientWaitQueueUpdate::Handle() {
    if (ClientWaitQueueUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD04C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD04C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWaitQueueUpdate*)>(reinterpret_cast<uintptr_t>(ClientWaitQueueUpdate::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: eb2a49e5 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientWaitQueueFinish::ClientWaitQueueFinish(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 5072689e IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientWaitQueueFinish::~ClientWaitQueueFinish() {
}

// PROVEN: 10435afa IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientWaitQueueFinish::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: c6198f10 IDENTICAL/UNPROVEN
// OFFSET: 0x006BDFA0/0x140372B10
bool ClientWaitQueueFinish::Handle() {
    if (ClientWaitQueueFinish::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDD28.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDD28.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWaitQueueFinish*)>(reinterpret_cast<uintptr_t>(ClientWaitQueueFinish::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 7a683857 IDENTICAL/IDENTICAL
// OFFSET: 0x006F4180/0x1403AA150
ClientPvpLogData::~ClientPvpLogData() {
}

// PROVEN: 9ed5726a IDENTICAL/IDENTICAL
// OFFSET: 0x006F4330/0x1403BB740
ClientEnumCharactersResult::ClientEnumCharactersResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 67bb880e IDENTICAL/IDENTICAL
// OFFSET: 0x006F0070/0x1403B57E0
ClientEnumCharactersResult::~ClientEnumCharactersResult() {
}

// OFFSET: 0x006F36B0/0x1403BA3E0
void ClientEnumCharactersResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_factionChangeRestrictions.SetCount(bits.ReadBits<23>());
    this->m_success = bits.ReadBit();
    this->m_characters.SetCount(bits.ReadBits<17>());

    for (uint32_t i = 0; i < this->m_characters.Count(); i++) {
        JamClientCharacterListEntry& character = this->m_characters[i];
        character.m_guid[3] = bits.ReadBit();
        character.m_guildGUID[1] = bits.ReadBit();
        character.m_guildGUID[7] = bits.ReadBit();
        character.m_guildGUID[2] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(character.m_name) = bits.ReadBits<7>();
        character.m_guid[4] = bits.ReadBit();
        character.m_guid[7] = bits.ReadBit();
        character.m_guildGUID[3] = bits.ReadBit();
        character.m_guid[5] = bits.ReadBit();
        character.m_guildGUID[6] = bits.ReadBit();
        character.m_guid[1] = bits.ReadBit();
        character.m_guildGUID[5] = bits.ReadBit();
        character.m_guildGUID[4] = bits.ReadBit();
        character.m_firstLogin = bits.ReadBit();
        character.m_guid[0] = bits.ReadBit();
        character.m_guid[2] = bits.ReadBit();
        character.m_guid[6] = bits.ReadBit();
        character.m_guildGUID[0] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_characters.Count(); i++) {
        JamClientCharacterListEntry& character = this->m_characters[i];
        uint8_t byteValue;
        uint32_t intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_classID = byteValue;

        for (uint32_t j = 0; j < 23; j++) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_items[j].m_inventoryType = byteValue;

            intValue = 0;
            msg->Get(intValue);
            character.m_items[j].m_displayID = intValue;

            intValue = 0;
            msg->Get(intValue);
            character.m_items[j].m_auraVisualID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petCreatureFamilyID = intValue;

        if (character.m_guildGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[2] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        character.m_listOrder = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_hairStyleID = byteValue;

        if (character.m_guildGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[3] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petDisplayInfoID = intValue;

        intValue = 0;
        msg->Get(intValue);
        character.m_flags = intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_hairColorID = byteValue;

        if (character.m_guid[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_mapID = intValue;

        if (character.m_guildGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[5] ^= byteValue;
        }

        float floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.z = floatValue;

        if (character.m_guildGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[6] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petExperienceLevel = intValue;

        if (character.m_guid[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[3] ^= byteValue;
        }

        floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.y = floatValue;

        intValue = 0;
        msg->Get(intValue);
        character.m_customizeFlags = intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_facialHairStyleID = byteValue;

        if (character.m_guid[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[7] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        character.m_sexID = byteValue;

        uint32_t nameLength = *reinterpret_cast<uint32_t*>(character.m_name);
        msg->GetArray(reinterpret_cast<uint8_t*>(character.m_name), nameLength);
        character.m_name[nameLength] = '\0';

        byteValue = 0;
        msg->Get(byteValue);
        character.m_faceID = byteValue;

        if (character.m_guid[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[0] ^= byteValue;
        }

        if (character.m_guid[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[2] ^= byteValue;
        }

        if (character.m_guildGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[1] ^= byteValue;
        }

        if (character.m_guildGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[7] ^= byteValue;
        }

        floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.x = floatValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_skinColorID = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_raceID = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_experienceLevel = byteValue;

        if (character.m_guid[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[6] ^= byteValue;
        }

        if (character.m_guildGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[4] ^= byteValue;
        }

        if (character.m_guildGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[0] ^= byteValue;
        }

        if (character.m_guid[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[5] ^= byteValue;
        }

        if (character.m_guid[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[1] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_zoneID = intValue;
    }

    for (uint32_t i = 0; i < this->m_factionChangeRestrictions.Count(); i++) {
        uint8_t raceID = 0;
        msg->Get(raceID);
        this->m_factionChangeRestrictions[i].m_raceID = raceID;

        uint32_t mask = 0;
        msg->Get(mask);
        this->m_factionChangeRestrictions[i].m_mask = mask;
    }
}

// PROVEN: 0f7be70f IDENTICAL/UNPROVEN
// OFFSET: 0x006ECDF0/0x1403B1210
bool ClientEnumCharactersResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E98A0/0x1403AC900
bool ClientEnumCharactersResult::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    uint32_t count2 = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 5 * count2) {
        return false;
    }

    this->m_factionChangeRestrictions.SetCount(count2);

    this->m_success = bits.ReadBit();
    uint32_t count = bits.ReadBits<17>();

    if (msg->m_size - msg->m_read < ((24 * count + 7) >> 3) + 257 * count) {
        return false;
    }

    this->m_characters.SetCount(count);

    for (uint32_t i = 0; i < this->m_characters.Count(); i++) {
        JamClientCharacterListEntry& character = this->m_characters[i];
        character.m_guid[3] = bits.ReadBit();
        character.m_guildGUID[1] = bits.ReadBit();
        character.m_guildGUID[7] = bits.ReadBit();
        character.m_guildGUID[2] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(character.m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(character.m_name) >= sizeof(character.m_name)) {
            return false;
        }

        character.m_guid[4] = bits.ReadBit();
        character.m_guid[7] = bits.ReadBit();
        character.m_guildGUID[3] = bits.ReadBit();
        character.m_guid[5] = bits.ReadBit();
        character.m_guildGUID[6] = bits.ReadBit();
        character.m_guid[1] = bits.ReadBit();
        character.m_guildGUID[5] = bits.ReadBit();
        character.m_guildGUID[4] = bits.ReadBit();
        character.m_firstLogin = bits.ReadBit();
        character.m_guid[0] = bits.ReadBit();
        character.m_guid[2] = bits.ReadBit();
        character.m_guid[6] = bits.ReadBit();
        character.m_guildGUID[0] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_characters.Count(); i++) {
        JamClientCharacterListEntry& character = this->m_characters[i];
        uint8_t byteValue;
        uint32_t intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_classID = byteValue;

        for (uint32_t j = 0; j < 23; j++) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_items[j].m_inventoryType = byteValue;

            intValue = 0;
            msg->Get(intValue);
            character.m_items[j].m_displayID = intValue;

            intValue = 0;
            msg->Get(intValue);
            character.m_items[j].m_auraVisualID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petCreatureFamilyID = intValue;

        if (character.m_guildGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[2] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        character.m_listOrder = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_hairStyleID = byteValue;

        if (character.m_guildGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[3] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petDisplayInfoID = intValue;

        intValue = 0;
        msg->Get(intValue);
        character.m_flags = intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_hairColorID = byteValue;

        if (character.m_guid[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_mapID = intValue;

        if (character.m_guildGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[5] ^= byteValue;
        }

        float floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.z = floatValue;

        if (character.m_guildGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[6] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_petExperienceLevel = intValue;

        if (character.m_guid[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[3] ^= byteValue;
        }

        floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.y = floatValue;

        intValue = 0;
        msg->Get(intValue);
        character.m_customizeFlags = intValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_facialHairStyleID = byteValue;

        if (character.m_guid[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[7] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        character.m_sexID = byteValue;

        uint32_t nameLength = *reinterpret_cast<uint32_t*>(character.m_name);
        msg->GetArray(reinterpret_cast<uint8_t*>(character.m_name), nameLength);
        character.m_name[nameLength] = '\0';

        byteValue = 0;
        msg->Get(byteValue);
        character.m_faceID = byteValue;

        if (character.m_guid[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[0] ^= byteValue;
        }

        if (character.m_guid[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[2] ^= byteValue;
        }

        if (character.m_guildGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[1] ^= byteValue;
        }

        if (character.m_guildGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[7] ^= byteValue;
        }

        floatValue = 0.0f;
        msg->Get(floatValue);
        character.m_position.x = floatValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_skinColorID = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_raceID = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        character.m_experienceLevel = byteValue;

        if (character.m_guid[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[6] ^= byteValue;
        }

        if (character.m_guildGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[4] ^= byteValue;
        }

        if (character.m_guildGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guildGUID[0] ^= byteValue;
        }

        if (character.m_guid[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[5] ^= byteValue;
        }

        if (character.m_guid[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            character.m_guid[1] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        character.m_zoneID = intValue;
    }

    for (uint32_t i = 0; i < this->m_factionChangeRestrictions.Count(); i++) {
        uint8_t raceID = 0;
        msg->Get(raceID);
        this->m_factionChangeRestrictions[i].m_raceID = raceID;

        uint32_t mask = 0;
        msg->Get(mask);
        this->m_factionChangeRestrictions[i].m_mask = mask;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: fa8973de IDENTICAL/NONE
// OFFSET: 0x006CF2E0
bool ClientEnumCharactersResult::IsValid() const {
    for (uint32_t i = 0; i < this->m_characters.m_count; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->m_characters.m_data[i].m_name))) {
                    break;
                }

                terminated |= this->m_characters.m_data[i].m_name[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: acb11c1c IDENTICAL/UNPROVEN
// OFFSET: 0x006B5040/0x140367220
bool ClientEnumCharactersResult::Handle() {
    if (ClientEnumCharactersResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE57C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE57C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientEnumCharactersResult*)>(reinterpret_cast<uintptr_t>(ClientEnumCharactersResult::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 7e4fd766 IDENTICAL/IDENTICAL
// OFFSET: 0x006BE090/0x140372C80
ClientGenerateRandomCharacterNameResult::ClientGenerateRandomCharacterNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 2a6fff5d IDENTICAL/IDENTICAL
// OFFSET: 0x006A0FF0/0x14034E250
ClientGenerateRandomCharacterNameResult::~ClientGenerateRandomCharacterNameResult() {
}

// PROVEN: 3c09abf5 IDENTICAL/DIFFERENT
// OFFSET: 0x006B5480/0x1403677B0
void ClientGenerateRandomCharacterNameResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_success = bits.ReadBit() != 0;
    uint32_t length = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = '\0';
}

// PROVEN: f222e7b0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C60B0/0x14037C4D0
bool ClientGenerateRandomCharacterNameResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5013df56 IDENTICAL/DIFFERENT
// OFFSET: 0x006C4620/0x14037A4D0
bool ClientGenerateRandomCharacterNameResult::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_success = bits.ReadBit() != 0;
    uint32_t length = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;

    if (length >= sizeof(this->m_name)) {
        return false;
    }

    msg->GetData(this->m_name, length);
    this->m_name[length] = '\0';

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 4dbfd589 IDENTICAL/NONE
// OFFSET: 0x006A8410
bool ClientGenerateRandomCharacterNameResult::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
            break;
        }

        terminated |= this->m_name[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 810779e6 IDENTICAL/UNPROVEN
// OFFSET: 0x006A4D80/0x140352C70
bool ClientGenerateRandomCharacterNameResult::Handle() {
    if (ClientGenerateRandomCharacterNameResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD98C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD98C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGenerateRandomCharacterNameResult*)>(reinterpret_cast<uintptr_t>(ClientGenerateRandomCharacterNameResult::s_handler) - (right ^ left))(this);
    }

    return false;
}

// OFFSET: 0x006BAE70/0x14036EB20
ClientDBReply::ClientDBReply(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006B0480/0x140361210
ClientDBReply::~ClientDBReply() {
}

// PROVEN: 00cc5b51 IDENTICAL/UNPROVEN
// OFFSET: 0x006B1CB0/0x1403630A0
bool ClientDBReply::Handle() {
    if (ClientDBReply::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCEF8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCEF8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDBReply*)>(reinterpret_cast<uintptr_t>(ClientDBReply::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006AFA80/0x140360530
void ClientDBReply::Unpack(CDataStore* msg, bool full) {
    uint32_t recordID = 0;
    msg->Get(recordID);
    this->m_recordID = recordID;

    uint32_t tableHash = 0;
    msg->Get(tableHash);
    this->m_tableHash = tableHash;

    uint32_t timestamp = 0;
    msg->Get(timestamp);
    this->m_timestamp = timestamp;

    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.PutData(data, size);
        this->m_data.Finalize();
    }
}

// PROVEN: 0e19ab34 IDENTICAL/NONE
// OFFSET: 0x006C05F0
bool ClientDBReply::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BDA10/0x1403723F0
bool ClientDBReply::Read(CDataStore* msg, bool full) {
    uint32_t recordID = 0;
    msg->Get(recordID);
    this->m_recordID = recordID;

    uint32_t tableHash = 0;
    msg->Get(tableHash);
    this->m_tableHash = tableHash;

    uint32_t timestamp = 0;
    msg->Get(timestamp);
    this->m_timestamp = timestamp;

    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.PutData(data, size);
        this->m_data.Finalize();
    }

    return msg->IsRead() != 0;
}

// PROVEN: 8e2babe4 IDENTICAL/IDENTICAL
// OFFSET: 0x006E1E10/0x1403A2620
ClientHotfixNotify::ClientHotfixNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b2c35c49 IDENTICAL/IDENTICAL
// OFFSET: 0x006DDEB0/0x14039D900
ClientHotfixNotify::~ClientHotfixNotify() {
}

// PROVEN: 6c453a3c IDENTICAL/UNPROVEN
// OFFSET: 0x006C09F0/0x140375DE0
bool ClientHotfixNotify::Handle() {
    if (ClientHotfixNotify::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF970.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF970.m_left->Get();
        return reinterpret_cast<bool (*)(ClientHotfixNotify*)>(reinterpret_cast<uintptr_t>(ClientHotfixNotify::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// OFFSET: 0x006DC4D0/0x14039B670
void ClientHotfixNotify::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hotfixes.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_hotfixes.Count(); i++) {
        {
            uint32_t recordID = 0;
            msg->Get(recordID);
            this->m_hotfixes[i].m_recordID = recordID;
        }

        {
            uint32_t timestamp = 0;
            msg->Get(timestamp);
            this->m_hotfixes[i].m_timestamp = timestamp;
        }

        {
            uint32_t tableHash = 0;
            msg->Get(tableHash);
            this->m_hotfixes[i].m_tableHash = tableHash;
        }
    }
}

// PROVEN: d48b683d IDENTICAL/UNPROVEN
// OFFSET: 0x006ECD80/0x1403B1190
bool ClientHotfixNotify::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 315a906d IDENTICAL/DIFFERENT
// OFFSET: 0x006E1FF0/0x1403A2860
bool ClientHotfixNotify::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < 12 * count) {
        return false;
    }

    this->m_hotfixes.SetCount(count);

    for (uint32_t i = 0; i < this->m_hotfixes.Count(); i++) {
        {
            uint32_t recordID = 0;
            msg->Get(recordID);
            this->m_hotfixes[i].m_recordID = recordID;
        }

        {
            uint32_t timestamp = 0;
            msg->Get(timestamp);
            this->m_hotfixes[i].m_timestamp = timestamp;
        }

        {
            uint32_t tableHash = 0;
            msg->Get(tableHash);
            this->m_hotfixes[i].m_tableHash = tableHash;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6bb128d3 IDENTICAL/IDENTICAL
// OFFSET: 0x006BE7A0/0x14034FE70
ClientHotfixNotifyBlob::ClientHotfixNotifyBlob(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3fb6fb72 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2280/0x14034F780
ClientHotfixNotifyBlob::~ClientHotfixNotifyBlob() {
}

// PROVEN: 08f0378a IDENTICAL/UNPROVEN
// OFFSET: 0x006C0DE0/0x1403762B0
bool ClientHotfixNotifyBlob::Handle() {
    if (ClientHotfixNotifyBlob::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF52C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF52C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientHotfixNotifyBlob*)>(reinterpret_cast<uintptr_t>(ClientHotfixNotifyBlob::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 8a6ab2c7 IDENTICAL/IDENTICAL
// OFFSET: 0x006A5360/0x14034DDF0
void ClientHotfixNotifyBlob::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t tableHash = 0;
        msg->Get(tableHash);
        this->m_tableHash = tableHash;
    }

    {
        uint32_t timestamp = 0;
        msg->Get(timestamp);
        this->m_timestamp = timestamp;
    }

    {
        uint32_t recordID = 0;
        msg->Get(recordID);
        this->m_recordID = recordID;
    }
}

// PROVEN: 19efe1f9 IDENTICAL/UNPROVEN
// OFFSET: 0x006CF050/0x140389200
bool ClientHotfixNotifyBlob::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 791e39b8 IDENTICAL/IDENTICAL
// OFFSET: 0x006BF7C0/0x140374940
bool ClientHotfixNotifyBlob::Read(CDataStore* msg, bool full) {
    {
        uint32_t tableHash = 0;
        msg->Get(tableHash);
        this->m_tableHash = tableHash;
    }

    {
        uint32_t timestamp = 0;
        msg->Get(timestamp);
        this->m_timestamp = timestamp;
    }

    {
        uint32_t recordID = 0;
        msg->Get(recordID);
        this->m_recordID = recordID;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 148a6e7c IDENTICAL/IDENTICAL
// OFFSET: 0x006BE8D0/0x140373690
ClientGameObjectActivateAnimKit::ClientGameObjectActivateAnimKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 33d3f857 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2EB0/0x1403503B0
ClientGameObjectActivateAnimKit::~ClientGameObjectActivateAnimKit() {
}

// PROVEN: acb1a2a0 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF780/0x1403748E0
bool ClientGameObjectActivateAnimKit::Handle() {
    if (ClientGameObjectActivateAnimKit::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDCEC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDCEC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGameObjectActivateAnimKit*)>(reinterpret_cast<uintptr_t>(ClientGameObjectActivateAnimKit::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 9b818cb8 IDENTICAL/IDENTICAL
// OFFSET: 0x006B50D0/0x1403672E0
void ClientGameObjectActivateAnimKit::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_objectGUID[5] = bits.ReadBit();
    this->m_objectGUID[1] = bits.ReadBit();
    this->m_objectGUID[0] = bits.ReadBit();
    this->m_objectGUID[4] = bits.ReadBit();
    this->m_objectGUID[7] = bits.ReadBit();
    this->m_objectGUID[2] = bits.ReadBit();
    this->m_objectGUID[3] = bits.ReadBit();
    this->m_objectGUID[6] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_objectGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[5] ^= byteValue;
    }

    if (this->m_objectGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[1] ^= byteValue;
    }

    if (this->m_objectGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[0] ^= byteValue;
    }

    if (this->m_objectGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[3] ^= byteValue;
    }

    if (this->m_objectGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[4] ^= byteValue;
    }

    if (this->m_objectGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[6] ^= byteValue;
    }

    if (this->m_objectGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[2] ^= byteValue;
    }

    if (this->m_objectGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[7] ^= byteValue;
    }

    {
        uint32_t animKitID = 0;
        msg->Get(animKitID);
        this->m_animKitID = animKitID;
    }
}

// PROVEN: bfe0c2c4 IDENTICAL/UNPROVEN
// OFFSET: 0x006ADB80/0x14035DFA0
bool ClientGameObjectActivateAnimKit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f0e80405 IDENTICAL/IDENTICAL
// OFFSET: 0x006A7FF0/0x140356C60
bool ClientGameObjectActivateAnimKit::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_objectGUID[5] = bits.ReadBit();
    this->m_objectGUID[1] = bits.ReadBit();
    this->m_objectGUID[0] = bits.ReadBit();
    this->m_objectGUID[4] = bits.ReadBit();
    this->m_objectGUID[7] = bits.ReadBit();
    this->m_objectGUID[2] = bits.ReadBit();
    this->m_objectGUID[3] = bits.ReadBit();
    this->m_objectGUID[6] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_objectGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[5] ^= byteValue;
    }

    if (this->m_objectGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[1] ^= byteValue;
    }

    if (this->m_objectGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[0] ^= byteValue;
    }

    if (this->m_objectGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[3] ^= byteValue;
    }

    if (this->m_objectGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[4] ^= byteValue;
    }

    if (this->m_objectGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[6] ^= byteValue;
    }

    if (this->m_objectGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[2] ^= byteValue;
    }

    if (this->m_objectGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[7] ^= byteValue;
    }

    {
        uint32_t animKitID = 0;
        msg->Get(animKitID);
        this->m_animKitID = animKitID;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B2750/0x140363E60
ClientMapObjEvents::ClientMapObjEvents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006B19D0/0x140362CE0
ClientMapObjEvents::~ClientMapObjEvents() {
}

// PROVEN: a1e3d220 IDENTICAL/UNPROVEN
// OFFSET: 0x006BE300/0x140372FB0
bool ClientMapObjEvents::Handle() {
    if (ClientMapObjEvents::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFA8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFA8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMapObjEvents*)>(reinterpret_cast<uintptr_t>(ClientMapObjEvents::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// OFFSET: 0x006A9B60/0x140358FC0
void ClientMapObjEvents::Unpack(CDataStore* msg, bool full) {
    uint32_t uniqueID = 0;
    msg->Get(uniqueID);
    this->m_uniqueID = uniqueID;

    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.PutData(data, size);
        this->m_data.Finalize();
    }
}

// PROVEN: 2b93c991 IDENTICAL/NONE
// OFFSET: 0x006C50E0
bool ClientMapObjEvents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BCA30/0x140370F90
bool ClientMapObjEvents::Read(CDataStore* msg, bool full) {
    uint32_t uniqueID = 0;
    msg->Get(uniqueID);
    this->m_uniqueID = uniqueID;

    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.PutData(data, size);
        this->m_data.Finalize();
    }

    return msg->IsRead() != 0;
}

// PROVEN: 7103bbbf IDENTICAL/DIFFERENT
// OFFSET: 0x006D5A00/0x140392D40
ClientNotification::ClientNotification(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<13>();
    *reinterpret_cast<uint32_t*>(this->m_text) = length;
    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;
}

// PROVEN: 155fb61b IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DF0/0x14034DE90
ClientNotification::~ClientNotification() {
}

// PROVEN: 0d06a30d IDENTICAL/UNPROVEN
// OFFSET: 0x006D9230/0x140397630
bool ClientNotification::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1368378a IDENTICAL/DIFFERENT
// OFFSET: 0x006D4F10/0x140391D10
bool ClientNotification::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<13>(JamBitCount<true>());
    *reinterpret_cast<uint32_t*>(this->m_text) = length;

    if (length >= sizeof(this->m_text)) {
        return false;
    }

    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 666b3d41 IDENTICAL/NONE
// OFFSET: 0x006BE770
bool ClientNotification::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_text))) {
            break;
        }

        terminated |= this->m_text[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 05cb5c8e IDENTICAL/UNPROVEN
// OFFSET: 0x006A9870/0x140358BD0
bool ClientNotification::Handle() {
    if (ClientNotification::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF188.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF188.m_left->Get();
        return reinterpret_cast<bool (*)(ClientNotification*)>(reinterpret_cast<uintptr_t>(ClientNotification::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 592bd761 IDENTICAL/UNPROVEN
// OFFSET: 0x006CF330/0x14038E350
bool ClientSuspendToken::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5226a4df IDENTICAL/DIFFERENT
// OFFSET: 0x006BFF20/0x14037B2C0
bool ClientSuspendToken::Read(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequence = value;

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_reason = value >> 7;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 34908b15 IDENTICAL/UNPROVEN
// OFFSET: 0x006BE000/0x140372BC0
bool ClientSuspendToken::Handle() {
    if (ClientSuspendToken::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFF4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFF4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSuspendToken*)>(reinterpret_cast<uintptr_t>(ClientSuspendToken::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: d66db73d IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientNewWorldAbort::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 0ae08077 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6E00/0x140355740
bool ClientNewWorldAbort::Handle() {
    if (ClientNewWorldAbort::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDEB4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDEB4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientNewWorldAbort*)>(reinterpret_cast<uintptr_t>(ClientNewWorldAbort::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006CCA40/0x140385B90
ClientTransferPending::ClientTransferPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTransferSpellID = false;
    this->m_hasShip = false;
    this->Unpack(msg, true);
}

// PROVEN: 830ce099 IDENTICAL/UNPROVEN
// OFFSET: 0x006B2270/0x140363830
bool ClientTransferPending::Handle() {
    if (ClientTransferPending::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEF80.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEF80.m_left->Get();
        return reinterpret_cast<bool (*)(ClientTransferPending*)>(reinterpret_cast<uintptr_t>(ClientTransferPending::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: bf3948e5 IDENTICAL/IDENTICAL
// OFFSET: 0x006B5BF0/0x140368130
void ClientTransferPending::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasTransferSpellID = bits.ReadBit() != 0;
    this->m_hasShip = bits.ReadBit();

    if (this->m_hasShip) {
        {
            uint32_t shipOriginMapID = 0;
            msg->Get(shipOriginMapID);
            this->m_shipOriginMapID = shipOriginMapID;
        }

        {
            uint32_t shipID = 0;
            msg->Get(shipID);
            this->m_shipID = shipID;
        }
    }

    if (this->m_hasTransferSpellID) {
        uint32_t transferSpellID = 0;
        msg->Get(transferSpellID);
        this->m_transferSpellID = transferSpellID;
    }

    {
        uint32_t mapID = 0;
        msg->Get(mapID);
        this->m_mapID = mapID;
    }
}

// PROVEN: a3334c43 IDENTICAL/UNPROVEN
// OFFSET: 0x006D1EF0/0x14038D4B0
bool ClientTransferPending::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4fe616ad IDENTICAL/IDENTICAL
// OFFSET: 0x006BBB00/0x14036FBB0
bool ClientTransferPending::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasTransferSpellID = bits.ReadBit() != 0;
    this->m_hasShip = bits.ReadBit();

    if (this->m_hasShip) {
        {
            uint32_t shipOriginMapID = 0;
            msg->Get(shipOriginMapID);
            this->m_shipOriginMapID = shipOriginMapID;
        }

        {
            uint32_t shipID = 0;
            msg->Get(shipID);
            this->m_shipID = shipID;
        }
    }

    if (this->m_hasTransferSpellID) {
        uint32_t transferSpellID = 0;
        msg->Get(transferSpellID);
        this->m_transferSpellID = transferSpellID;
    }

    {
        uint32_t mapID = 0;
        msg->Get(mapID);
        this->m_mapID = mapID;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 7a9fd06b IDENTICAL/NONE
// OFFSET: 0x006C7920
bool ClientMessageBox::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C5EB0/0x14037C2B0
bool ClientMessageBox::Read(CDataStore* msg, bool full) {
    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_text.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_text.PutData(data, size);
        this->m_text.Finalize();
    }

    return msg->IsRead() != 0;
}

// PROVEN: 08bca03b IDENTICAL/UNPROVEN
// OFFSET: 0x006B05D0/0x140361340
bool ClientMessageBox::Handle() {
    if (ClientMessageBox::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEF80.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEF80.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMessageBox*)>(reinterpret_cast<uintptr_t>(ClientMessageBox::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 5d8eed6b DIFFERENT/IDENTICAL
// OFFSET: 0x006A5BF0/0x140353BB0
ClientNewWorld::ClientNewWorld(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->Unpack(msg, true);
}

// PROVEN: 8b5742fe IDENTICAL/UNPROVEN
// OFFSET: 0x006A9260/0x1403583C0
bool ClientNewWorld::Handle() {
    if (ClientNewWorld::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD85C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD85C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientNewWorld*)>(reinterpret_cast<uintptr_t>(ClientNewWorld::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 57c87b48 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2530/0x14034FA60
void ClientNewWorld::Unpack(CDataStore* msg, bool full) {
    {
        float x = 0.0f;
        msg->Get(x);
        this->m_position.x = x;
    }

    {
        float facing = 0.0f;
        msg->Get(facing);
        this->m_facing = facing;
    }

    {
        float y = 0.0f;
        msg->Get(y);
        this->m_position.y = y;
    }

    {
        uint32_t mapID = 0;
        msg->Get(mapID);
        this->m_mapID = mapID;
    }

    {
        float z = 0.0f;
        msg->Get(z);
        this->m_position.z = z;
    }
}

// PROVEN: b9ecc574 IDENTICAL/UNPROVEN
// OFFSET: 0x006D7300/0x140394F80
bool ClientNewWorld::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3c8412b0 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8360/0x14037F360
bool ClientNewWorld::Read(CDataStore* msg, bool full) {
    {
        float x = 0.0f;
        msg->Get(x);
        this->m_position.x = x;
    }

    {
        float facing = 0.0f;
        msg->Get(facing);
        this->m_facing = facing;
    }

    {
        float y = 0.0f;
        msg->Get(y);
        this->m_position.y = y;
    }

    {
        uint32_t mapID = 0;
        msg->Get(mapID);
        this->m_mapID = mapID;
    }

    {
        float z = 0.0f;
        msg->Get(z);
        this->m_position.z = z;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 48018d85 IDENTICAL/IDENTICAL
// OFFSET: [0x0066BC00/0x140312430]
JamWeeklySpellUse::JamWeeklySpellUse() {
}

// PROVEN: 683a58c9 IDENTICAL/IDENTICAL
// OFFSET: 0x00696A30/0x140342090
void JamPartyMemberState::operator=(const JamPartyMemberState& source) {
    this->m_guid = source.m_guid;
    this->m_numAuras = source.m_numAuras;
    this->m_vehicleSeatId = source.m_vehicleSeatId;
    this->m_maxHealth = source.m_maxHealth;
    this->m_health = source.m_health;
    this->unk_0022 = source.unk_0022;
    this->m_power = source.m_power;
    this->m_flags = source.m_flags;
    this->m_posZ = source.m_posZ;
    this->m_level = source.m_level;
    this->m_posY = source.m_posY;
    this->m_maxPower = source.m_maxPower;
    this->m_posX = source.m_posX;
    this->m_zoneID = source.m_zoneID;
    this->m_powerType = source.m_powerType;
}

// PROVEN: 077ba4ba IDENTICAL/IDENTICAL
// OFFSET: 0x006969C0/0x140342020
void JamPartyMemberState::Clear() {
    this->m_guid.m_guid = 0;
    this->m_health = 0;
    this->m_maxHealth = 0;
    this->m_vehicleSeatId = 0;
    this->m_numAuras = 0;
    this->m_flags = 0;
    this->m_power = 0;
    this->m_maxPower = 0;
    this->m_level = 0;
    this->m_zoneID = 0;
    this->unk_0022 = 0;
    this->m_posX = 0;
    this->m_posY = 0;
    this->m_posZ = 0;
    this->m_powerType = 0;
}

// PROVEN: 1f14fbc5 IDENTICAL/DIFFERENT
// OFFSET: 0x00696990/0x140342000
void JamClientRideTicket::operator=(const JamClientRideTicket& source) {
    this->m_id = source.m_id;
    *reinterpret_cast<uint64_t*>(this->m_requesterGuid) = *reinterpret_cast<const uint64_t*>(source.m_requesterGuid);
    this->m_type = source.m_type;
    this->m_time = source.m_time;
}

// OFFSET: 0x00696BE0
bool JamClientRideTicket::operator==(const JamClientRideTicket& other) const {
    return this->m_id == other.m_id && *reinterpret_cast<const uint64_t*>(this->m_requesterGuid) == *reinterpret_cast<const uint64_t*>(other.m_requesterGuid) && this->m_type == other.m_type && this->m_time == other.m_time;
}

// PROVEN: 17abd857 IDENTICAL/NONE
// OFFSET: 0x00696BC0/0x140342200
void JamClientRideTicket::Clear() {
    *reinterpret_cast<uint64_t*>(this->m_requesterGuid) = 0;
    this->m_id = 0;
    this->m_type = 0;
    this->m_time = 0;
}

// PROVEN: 5fa9e12c IDENTICAL/IDENTICAL
// OFFSET: 0x00696E80/0x1403424D0
void JamClientRideTicket::Put(CDataStore* msg) const {
    msg->Put(this->m_time);
    msg->Put(this->m_type);
    msg->Put(this->m_id);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_requesterGuid[4] != 0);
    bits.WriteBit(this->m_requesterGuid[5] != 0);
    bits.WriteBit(this->m_requesterGuid[0] != 0);
    bits.WriteBit(this->m_requesterGuid[6] != 0);
    bits.WriteBit(this->m_requesterGuid[2] != 0);
    bits.WriteBit(this->m_requesterGuid[7] != 0);
    bits.WriteBit(this->m_requesterGuid[1] != 0);
    bits.WriteBit(this->m_requesterGuid[3] != 0);
    bits.Flush();

    if (this->m_requesterGuid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[7] ^ 1));
    }

    if (this->m_requesterGuid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[4] ^ 1));
    }

    if (this->m_requesterGuid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[3] ^ 1));
    }

    if (this->m_requesterGuid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[2] ^ 1));
    }

    if (this->m_requesterGuid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[6] ^ 1));
    }

    if (this->m_requesterGuid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[0] ^ 1));
    }

    if (this->m_requesterGuid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[1] ^ 1));
    }

    if (this->m_requesterGuid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_requesterGuid[5] ^ 1));
    }
}

// PROVEN: c1eaed7d IDENTICAL/IDENTICAL
// OFFSET: 0x006A1DD0/0x14034F250
void JamWeeklySpellUse::operator=(const JamWeeklySpellUse& source) {
    this->m_category = source.m_category;
    this->m_uses = source.m_uses;
}

// STATIC: 0x00E4D4D4/0x140D36380
bool (*ClientPlaySpellVisualKit::s_handler)(ClientPlaySpellVisualKit* msg);

// STATIC: 0x00E4D488/0x140D362E8
bool (*ClientWeeklySpellUsage::s_handler)(ClientWeeklySpellUsage* msg);

// STATIC: 0x00E4D6F8/0x140D367C8
bool (*ClientWeeklySpellUsageUpdate::s_handler)(ClientWeeklySpellUsageUpdate* msg);

// STATIC: 0x00E4D77C/0x140D368D0
bool (*ClientClearCooldowns::s_handler)(ClientClearCooldowns* msg);

// STATIC: 0x00E4D654/0x140D36680
bool (*ClientCurrencyLootRemoved::s_handler)(ClientCurrencyLootRemoved* msg);

// STATIC: 0x00E4D43C/0x140D36250
bool (*ClientCurrencyLootRestored::s_handler)(ClientCurrencyLootRestored* msg);

// STATIC: 0x00E4D5D4/0x140D36580
bool (*ClientLootContents::s_handler)(ClientLootContents* msg);

// STATIC: 0x00E4D754/0x140D36880
bool (*ClientGuildSendGuildXP::s_handler)(ClientGuildSendGuildXP* msg);

// STATIC: 0x00E4D4AC/0x140D36330
bool (*ClientGuildSendMaxDailyXP::s_handler)(ClientGuildSendMaxDailyXP* msg);

// STATIC: 0x00E4D7B4/0x140D36940
bool (*ClientGuildSendRankChange::s_handler)(ClientGuildSendRankChange* msg);

// STATIC: 0x00E4D5E4/0x140D365A0
bool (*ClientBattlegroundInfoThrottled::s_handler)(ClientBattlegroundInfoThrottled* msg);

// STATIC: 0x00E4D53C/0x140D36450
bool (*ClientGuildMemberRecipes::s_handler)(ClientGuildMemberRecipes* msg);

// STATIC: 0x00E4D5A0/0x140D36518
bool (*ClientGuildMembersWithRecipe::s_handler)(ClientGuildMembersWithRecipe* msg);

// STATIC: 0x00E4D530/0x140D36438
bool (*ClientGuildCommandResult::s_handler)(ClientGuildCommandResult* msg);

// STATIC: 0x00E4D7A0/0x140D36918
bool (*ClientRolePollBegin::s_handler)(ClientRolePollBegin* msg);

// STATIC: 0x00E4D78C/0x140D368F0
bool (*ClientGuildKnownRecipes::s_handler)(ClientGuildKnownRecipes* msg);

// STATIC: 0x00E4D6C4/0x140D36760
bool (*ClientRaidMarkersChanged::s_handler)(ClientRaidMarkersChanged* msg);

// STATIC: 0x00E4D7D8/0x140D36988
bool (*ClientQuestNpcQueryResponse::s_handler)(ClientQuestNpcQueryResponse* msg);

// STATIC: 0x00E4D564/0x140D364A0
bool (*ClientRoleChangedInform::s_handler)(ClientRoleChangedInform* msg);

// STATIC: 0x00E4D574/0x140D364C0
bool (*ClientPetitionAlreadySigned::s_handler)(ClientPetitionAlreadySigned* msg);

// STATIC: 0x00E4D444/0x140D36260
bool (*ClientSummonRaidMemberValidateFailed::s_handler)(ClientSummonRaidMemberValidateFailed* msg);

// STATIC: 0x00E4D7E4/0x140D369A0
bool (*ClientStartTimer::s_handler)(ClientStartTimer* msg);

// STATIC: 0x00E4D748/0x140D36868
bool (*ClientSpellinterruptlog::s_handler)(ClientSpellinterruptlog* msg);

// STATIC: 0x00E4D4A8/0x140D36328
bool (*ClientArenaTeamCommandResult::s_handler)(ClientArenaTeamCommandResult* msg);

// STATIC: 0x00E4D514/0x140D36400
bool (*ClientXpGainAborted::s_handler)(ClientXpGainAborted* msg);

// STATIC: 0x00E4D714/0x140D36800
bool (*ClientSorStartExperienceIncomplete::s_handler)(ClientSorStartExperienceIncomplete* msg);

// STATIC: 0x00E4D7EC/0x140D369B0
bool (*ClientDebugRuneRegen::s_handler)(ClientDebugRuneRegen* msg);

// STATIC: 0x00E4D600/0x140D365D8
bool (*ClientChatIgnoredAccountMuted::s_handler)(ClientChatIgnoredAccountMuted* msg);

// STATIC: 0x00E4D830/0x140D36A38
bool (*ClientItemRefundInfoResponse::s_handler)(ClientItemRefundInfoResponse* msg);

// STATIC: 0x00E4D578/0x140D364C8
bool (*ClientItemExpirePurchaseRefund::s_handler)(ClientItemExpirePurchaseRefund* msg);

// STATIC: 0x00E4D6D0/0x140D36778
bool (*ClientItemRefundResult::s_handler)(ClientItemRefundResult* msg);

// STATIC: 0x00E4D824/0x140D36A20
bool (*ClientQuestGiverQuestComplete::s_handler)(ClientQuestGiverQuestComplete* msg);

// STATIC: 0x00E4D4E8/0x140D363A8
bool (*ClientSupercededSpells::s_handler)(ClientSupercededSpells* msg);

// STATIC: 0x00E4D74C/0x140D36870
bool (*ClientLearnedSpell::s_handler)(ClientLearnedSpell* msg);

// STATIC: 0x00E4D48C/0x140D362F0
bool (*ClientMoveSetActiveMover::s_handler)(ClientMoveSetActiveMover* msg);

// STATIC: 0x00E4D71C/0x140D36810
bool (*ClientRequestCemeteryListResponse::s_handler)(ClientRequestCemeteryListResponse* msg);

// STATIC: 0x00E4D5D0/0x140D36578
bool (*ClientWorldServerInfo::s_handler)(ClientWorldServerInfo* msg);

// STATIC: 0x00E4D698/0x140D36708
bool (*ClientUpdateActionButtons::s_handler)(ClientUpdateActionButtons* msg);

// STATIC: 0x00E4D818/0x140D36A08
bool (*ClientTimeSyncReq::s_handler)(ClientTimeSyncReq* msg);

// STATIC: 0x00E4D798/0x140D36908
bool (*ClientFeatureSystemStatus::s_handler)(ClientFeatureSystemStatus* msg);

// STATIC: 0x00E4D4EC/0x140D363B0
bool (*ClientCategoryCooldown::s_handler)(ClientCategoryCooldown* msg);

// STATIC: 0x00E4D568/0x140D364A8
bool (*ClientItemRemovePassive::s_handler)(ClientItemRemovePassive* msg);

// STATIC: 0x00E4D458/0x140D36288
bool (*ClientDisenchantCredit::s_handler)(ClientDisenchantCredit* msg);

// STATIC: 0x00E4D6F4/0x140D367C0
bool (*ClientItemSendPassive::s_handler)(ClientItemSendPassive* msg);

// STATIC: 0x00E4D640/0x140D36658
bool (*ClientTimeAdjustment::s_handler)(ClientTimeAdjustment* msg);

// STATIC: 0x00E4D4F8/0x140D363C8
bool (*ClientItemAddPassive::s_handler)(ClientItemAddPassive* msg);

// PROVEN: 309ffff0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C10E0/0x1403765F0
bool ClientPvpLogData::Handle() {
    if (ClientPvpLogData::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPvpLogData*)>(reinterpret_cast<uintptr_t>(ClientPvpLogData::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 87b01d44 IDENTICAL/IDENTICAL
// OFFSET: 0x006C1130/0x140376650
ClientPlaySpellVisualKit::ClientPlaySpellVisualKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8c70b068 IDENTICAL/IDENTICAL
// OFFSET: 0x006A28B0/0x14034FD30
ClientPlaySpellVisualKit::~ClientPlaySpellVisualKit() {
}

// PROVEN: 66b3709f IDENTICAL/UNPROVEN
// OFFSET: 0x006CA820/0x1403828F0
bool ClientPlaySpellVisualKit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: c70662c9 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0920/0x140375CB0
bool ClientPlaySpellVisualKit::Handle() {
    if (ClientPlaySpellVisualKit::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFB4C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFB4C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPlaySpellVisualKit*)>(reinterpret_cast<uintptr_t>(ClientPlaySpellVisualKit::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 22e5591d IDENTICAL/IDENTICAL
// OFFSET: 0x006B7900/0x14036A840
bool ClientPlaySpellVisualKit::Read(CDataStore* msg, bool full) {
    {
        uint32_t timeOffset = 0;
        msg->Get(timeOffset);
        this->m_timeOffset = timeOffset;
    }

    {
        uint32_t kitRecID = 0;
        msg->Get(kitRecID);
        this->m_kitRecID = kitRecID;
    }

    {
        uint32_t kitType = 0;
        msg->Get(kitType);
        this->m_kitType = kitType;
    }

    JamBitReader bits(msg);
    this->m_unitGUID[4] = bits.ReadBit();
    this->m_unitGUID[7] = bits.ReadBit();
    this->m_unitGUID[5] = bits.ReadBit();
    this->m_unitGUID[3] = bits.ReadBit();
    this->m_unitGUID[1] = bits.ReadBit();
    this->m_unitGUID[2] = bits.ReadBit();
    this->m_unitGUID[0] = bits.ReadBit();
    this->m_unitGUID[6] = bits.ReadBit();

    if (this->m_unitGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[0] ^= byteValue;
    }

    if (this->m_unitGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[4] ^= byteValue;
    }

    if (this->m_unitGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[1] ^= byteValue;
    }

    if (this->m_unitGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[6] ^= byteValue;
    }

    if (this->m_unitGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[7] ^= byteValue;
    }

    if (this->m_unitGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[2] ^= byteValue;
    }

    if (this->m_unitGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[3] ^= byteValue;
    }

    if (this->m_unitGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[5] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 53455a88 IDENTICAL/IDENTICAL
// OFFSET: 0x006AE450/0x14035EA40
void ClientPlaySpellVisualKit::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t timeOffset = 0;
        msg->Get(timeOffset);
        this->m_timeOffset = timeOffset;
    }

    {
        uint32_t kitRecID = 0;
        msg->Get(kitRecID);
        this->m_kitRecID = kitRecID;
    }

    {
        uint32_t kitType = 0;
        msg->Get(kitType);
        this->m_kitType = kitType;
    }

    JamBitReader bits(msg);
    this->m_unitGUID[4] = bits.ReadBit();
    this->m_unitGUID[7] = bits.ReadBit();
    this->m_unitGUID[5] = bits.ReadBit();
    this->m_unitGUID[3] = bits.ReadBit();
    this->m_unitGUID[1] = bits.ReadBit();
    this->m_unitGUID[2] = bits.ReadBit();
    this->m_unitGUID[0] = bits.ReadBit();
    this->m_unitGUID[6] = bits.ReadBit();

    if (this->m_unitGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[0] ^= byteValue;
    }

    if (this->m_unitGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[4] ^= byteValue;
    }

    if (this->m_unitGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[1] ^= byteValue;
    }

    if (this->m_unitGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[6] ^= byteValue;
    }

    if (this->m_unitGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[7] ^= byteValue;
    }

    if (this->m_unitGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[2] ^= byteValue;
    }

    if (this->m_unitGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[3] ^= byteValue;
    }

    if (this->m_unitGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_unitGUID[5] ^= byteValue;
    }
}

// PROVEN: 6083fa7e IDENTICAL/IDENTICAL
// OFFSET: 0x006DEB60/0x14039E700
ClientWeeklySpellUsage::ClientWeeklySpellUsage(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fa5189a5 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5A60/0x1403A6FE0
ClientWeeklySpellUsage::~ClientWeeklySpellUsage() {
}

// PROVEN: bbe92838 IDENTICAL/UNPROVEN
// OFFSET: 0x006E1F70/0x1403A2800
bool ClientWeeklySpellUsage::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 217e1ccd IDENTICAL/UNPROVEN
// OFFSET: 0x006A5CC0/0x140353CC0
bool ClientWeeklySpellUsage::Handle() {
    if (ClientWeeklySpellUsage::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE04C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE04C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWeeklySpellUsage*)>(reinterpret_cast<uintptr_t>(ClientWeeklySpellUsage::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006DC5B0/0x14039B770
bool ClientWeeklySpellUsage::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 5 * count) {
        return false;
    }

    this->m_usage.SetCount(count);

    for (uint32_t i = 0; i < this->m_usage.Count(); i++) {
        {
            uint32_t category = 0;
            msg->Get(category);
            this->m_usage[i].m_category = category;
        }

        {
            uint8_t uses = 0;
            msg->Get(uses);
            this->m_usage[i].m_uses = uses;
        }
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006DEA10/0x14039E530
void ClientWeeklySpellUsage::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_usage.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_usage.Count(); i++) {
        {
            uint32_t category = 0;
            msg->Get(category);
            this->m_usage[i].m_category = category;
        }

        {
            uint8_t uses = 0;
            msg->Get(uses);
            this->m_usage[i].m_uses = uses;
        }
    }
}

// PROVEN: bc1123a3 IDENTICAL/UNPROVEN
// OFFSET: 0x006AEF10/0x14034ED70
ClientWeeklySpellUsageUpdate::ClientWeeklySpellUsageUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint8_t uses = 0;
        msg->Get(uses);
        this->m_usage.m_uses = uses;
    }

    {
        uint32_t category = 0;
        msg->Get(category);
        this->m_usage.m_category = category;
    }
}

// PROVEN: f6a5c43f IDENTICAL/UNPROVEN
// OFFSET: 0x006B1AE0/0x140350000
ClientWeeklySpellUsageUpdate::~ClientWeeklySpellUsageUpdate() {
}

// PROVEN: f69565c9 IDENTICAL/IDENTICAL
// OFFSET: 0x006C52E0/0x14037A990
bool ClientWeeklySpellUsageUpdate::Get(CDataStore* msg, bool full) {
    uint8_t uses = 0;
    msg->Get(uses);
    this->m_usage.m_uses = uses;

    uint32_t category = 0;
    msg->Get(category);
    this->m_usage.m_category = category;

    return msg->IsRead() != 0;
}

// PROVEN: 7b865186 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD140/0x14035D210
bool ClientWeeklySpellUsageUpdate::Handle() {
    if (ClientWeeklySpellUsageUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFA8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFA8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWeeklySpellUsageUpdate*)>(reinterpret_cast<uintptr_t>(ClientWeeklySpellUsageUpdate::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: d7ff0f75 IDENTICAL/IDENTICAL
// OFFSET: 0x006D8620/0x140396710
ClientClearCooldowns::ClientClearCooldowns(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f9e43d14 IDENTICAL/UNPROVEN
// OFFSET: 0x006C9A90/0x1403814F0
ClientClearCooldowns::~ClientClearCooldowns() {
}

// PROVEN: dbd0a56e IDENTICAL/UNPROVEN
// OFFSET: 0x006D97A0/0x140397C50
bool ClientClearCooldowns::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b8777f48 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6020/0x140368670
bool ClientClearCooldowns::Handle() {
    if (ClientClearCooldowns::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDED4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDED4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientClearCooldowns*)>(reinterpret_cast<uintptr_t>(ClientClearCooldowns::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 05311f7e IDENTICAL/DIFFERENT
// OFFSET: 0x006D9350/0x140397750
bool ClientClearCooldowns::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_guid[1] = bits.ReadBit();
    this->m_guid[3] = bits.ReadBit();
    this->m_guid[6] = bits.ReadBit();
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->m_spellID.SetCount(count);
    this->m_guid[7] = bits.ReadBit();
    this->m_guid[5] = bits.ReadBit();
    this->m_guid[2] = bits.ReadBit();
    this->m_guid[4] = bits.ReadBit();
    this->m_guid[0] = bits.ReadBit();

    if (this->m_guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[7] ^= byteValue;
    }

    if (this->m_guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[2] ^= byteValue;
    }

    if (this->m_guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[4] ^= byteValue;
    }

    if (this->m_guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[5] ^= byteValue;
    }

    if (this->m_guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[1] ^= byteValue;
    }

    if (this->m_guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[3] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->m_spellID.Count(); i++) {
        uint32_t spellID = 0;
        msg->Get(spellID);
        this->m_spellID[i] = spellID;
    }

    if (this->m_guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[0] ^= byteValue;
    }

    if (this->m_guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006D83B0/0x140396360
void ClientClearCooldowns::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_guid[1] = bits.ReadBit();
    this->m_guid[3] = bits.ReadBit();
    this->m_guid[6] = bits.ReadBit();
    this->m_spellID.SetCount(bits.ReadBits<24>());
    this->m_guid[7] = bits.ReadBit();
    this->m_guid[5] = bits.ReadBit();
    this->m_guid[2] = bits.ReadBit();
    this->m_guid[4] = bits.ReadBit();
    this->m_guid[0] = bits.ReadBit();

    if (this->m_guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[7] ^= byteValue;
    }

    if (this->m_guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[2] ^= byteValue;
    }

    if (this->m_guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[4] ^= byteValue;
    }

    if (this->m_guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[5] ^= byteValue;
    }

    if (this->m_guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[1] ^= byteValue;
    }

    if (this->m_guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[3] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->m_spellID.Count(); i++) {
        uint32_t spellID = 0;
        msg->Get(spellID);
        this->m_spellID[i] = spellID;
    }

    if (this->m_guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[0] ^= byteValue;
    }

    if (this->m_guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[6] ^= byteValue;
    }
}

// PROVEN: 3222d045 IDENTICAL/IDENTICAL
// OFFSET: 0x006F2DB0/0x1403B9980
ClientSetupCurrency::ClientSetupCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fb652ae1 IDENTICAL/IDENTICAL
// OFFSET: 0x006DFCD0/0x14039FBB0
ClientSetupCurrency::~ClientSetupCurrency() {
}

// OFFSET: 0x006F0580/0x1403B5E30
void ClientSetupCurrency::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_data.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_data.Count(); i++) {
        ClientSetupCurrencyRecord& data = this->m_data[i];
        data.m_hasWeeklyQuantity = bits.ReadBit() != 0;
        data.m_flags = static_cast<uint8_t>(bits.ReadBits<4>());
        data.m_hasMaxWeeklyQuantity = bits.ReadBit() != 0;
        data.m_hasTrackedQuantity = bits.ReadBit() != 0;
    }

    for (uint32_t i = 0; i < this->m_data.Count(); i++) {
        ClientSetupCurrencyRecord& data = this->m_data[i];
        uint32_t value = 0;
        msg->Get(value);
        data.m_quantity = value;

        if (data.m_hasMaxWeeklyQuantity) {
            value = 0;
            msg->Get(value);
            data.m_maxWeeklyQuantity = value;
        }

        if (data.m_hasTrackedQuantity) {
            value = 0;
            msg->Get(value);
            data.m_trackedQuantity = value;
        }

        value = 0;
        msg->Get(value);
        data.m_type = value;

        if (data.m_hasWeeklyQuantity) {
            value = 0;
            msg->Get(value);
            data.m_weeklyQuantity = value;
        }
    }
}

// PROVEN: b40e01fb IDENTICAL/UNPROVEN
// OFFSET: 0x006F2DA0/0x1403B9970
bool ClientSetupCurrency::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ECB30/0x1403B0E60
bool ClientSetupCurrency::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((7 * count + 7) >> 3) + 8 * count) {
        return false;
    }

    this->m_data.SetCount(count);

    for (uint32_t i = 0; i < this->m_data.Count(); i++) {
        ClientSetupCurrencyRecord& data = this->m_data[i];
        data.m_hasWeeklyQuantity = bits.ReadBit() != 0;
        data.m_flags = static_cast<uint8_t>(bits.ReadBits<4>());
        data.m_hasMaxWeeklyQuantity = bits.ReadBit() != 0;
        data.m_hasTrackedQuantity = bits.ReadBit() != 0;
    }

    for (uint32_t i = 0; i < this->m_data.Count(); i++) {
        ClientSetupCurrencyRecord& data = this->m_data[i];

        {
            uint32_t value = 0;
            msg->Get(value);
            data.m_quantity = value;
        }

        if (data.m_hasMaxWeeklyQuantity) {
            uint32_t value = 0;
            msg->Get(value);
            data.m_maxWeeklyQuantity = value;
        }

        if (data.m_hasTrackedQuantity) {
            uint32_t value = 0;
            msg->Get(value);
            data.m_trackedQuantity = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            data.m_type = value;
        }

        if (data.m_hasWeeklyQuantity) {
            uint32_t value = 0;
            msg->Get(value);
            data.m_weeklyQuantity = value;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: f19dfbc8 IDENTICAL/UNPROVEN
// OFFSET: 0x006B0440/0x1403611B0
bool ClientSetupCurrency::Handle() {
    if (ClientSetupCurrency::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD770.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD770.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSetupCurrency*)>(reinterpret_cast<uintptr_t>(ClientSetupCurrency::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006C4580/0x14037A430
ClientSetCurrency::ClientSetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasWeeklyQuantity = false;
    this->m_hasTrackedQuantity = false;
    this->Unpack(msg, true);
}

// PROVEN: 1aa06d30 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2A00/0x14034FEF0
ClientSetCurrency::~ClientSetCurrency() {
}

// PROVEN: 01f13a06 IDENTICAL/DIFFERENT
// OFFSET: 0x006B0730/0x140361510
void ClientSetCurrency::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasWeeklyQuantity = bits.ReadBit() != 0;
    this->m_hasTrackedQuantity = bits.ReadBit() != 0;
    this->m_suppressChatLog = bits.ReadBit() != 0;

    uint32_t value;

    if (this->m_hasTrackedQuantity) {
        value = 0;
        msg->Get(value);
        this->m_trackedQuantity = value;
    }

    value = 0;
    msg->Get(value);
    this->m_quantity = value;

    value = 0;
    msg->Get(value);
    this->m_type = value;

    if (this->m_hasWeeklyQuantity) {
        value = 0;
        msg->Get(value);
        this->m_weeklyQuantity = value;
    }
}

// PROVEN: 391c889b IDENTICAL/UNPROVEN
// OFFSET: 0x006BF180/0x140374160
bool ClientSetCurrency::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 86be04af IDENTICAL/DIFFERENT
// OFFSET: 0x006B9470/0x14036CA30
bool ClientSetCurrency::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasWeeklyQuantity = bits.ReadBit() != 0;
    this->m_hasTrackedQuantity = bits.ReadBit() != 0;
    this->m_suppressChatLog = bits.ReadBit() != 0;

    uint32_t value;

    if (this->m_hasTrackedQuantity) {
        value = 0;
        msg->Get(value);
        this->m_trackedQuantity = value;
    }

    value = 0;
    msg->Get(value);
    this->m_quantity = value;

    value = 0;
    msg->Get(value);
    this->m_type = value;

    if (this->m_hasWeeklyQuantity) {
        value = 0;
        msg->Get(value);
        this->m_weeklyQuantity = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6c50746b IDENTICAL/UNPROVEN
// OFFSET: 0x006BFD40/0x140374F80
bool ClientSetCurrency::Handle() {
    if (ClientSetCurrency::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDE38.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDE38.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSetCurrency*)>(reinterpret_cast<uintptr_t>(ClientSetCurrency::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: f5a18197 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientWeeklyResetCurrency::ClientWeeklyResetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 0b497e5b IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientWeeklyResetCurrency::~ClientWeeklyResetCurrency() {
}

// PROVEN: a7db88d3 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientWeeklyResetCurrency::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 944a2500 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3C80/0x140351500
bool ClientWeeklyResetCurrency::Handle() {
    if (ClientWeeklyResetCurrency::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF594.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF594.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWeeklyResetCurrency*)>(reinterpret_cast<uintptr_t>(ClientWeeklyResetCurrency::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 0423d38f IDENTICAL/UNPROVEN
// OFFSET: 0x006BD0A0/0x1403717A0
ClientUpdateCurrencyWeekLimit::ClientUpdateCurrencyWeekLimit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_weekCap = value;

    value = 0;
    msg->Get(value);
    this->m_type = value;
}

// PROVEN: 7fbce4c0 IDENTICAL/UNPROVEN
// OFFSET: 0x006A10B0/0x14034F400
ClientUpdateCurrencyWeekLimit::~ClientUpdateCurrencyWeekLimit() {
}

// PROVEN: f9f5e65f IDENTICAL/IDENTICAL
// OFFSET: 0x006C8450/0x14037F2B0
bool ClientUpdateCurrencyWeekLimit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3e5339f4 IDENTICAL/NONE
// OFFSET: 0x006B7A90
bool ClientUpdateCurrencyWeekLimit::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_weekCap = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_type = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 4ebb3088 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6E40/0x1403557A0
bool ClientUpdateCurrencyWeekLimit::Handle() {
    if (ClientUpdateCurrencyWeekLimit::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF554.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF554.m_left->Get();
        return reinterpret_cast<bool (*)(ClientUpdateCurrencyWeekLimit*)>(reinterpret_cast<uintptr_t>(ClientUpdateCurrencyWeekLimit::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 6252877f IDENTICAL/IDENTICAL
// OFFSET: 0x006DD810/0x14039CE50
ClientVendorInventory::ClientVendorInventory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9b4caf01 IDENTICAL/IDENTICAL
// OFFSET: 0x006D9770/0x140397C10
ClientVendorInventory::~ClientVendorInventory() {
}

// OFFSET: 0x006DC6F0/0x14039B920
void ClientVendorInventory::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_vendor[1] = bits.ReadBit();
    this->m_vendor[0] = bits.ReadBit();
    this->m_items.SetCount(bits.ReadBits<21>());
    this->m_vendor[3] = bits.ReadBit();
    this->m_vendor[6] = bits.ReadBit();
    this->m_vendor[5] = bits.ReadBit();
    this->m_vendor[2] = bits.ReadBit();
    this->m_vendor[7] = bits.ReadBit();

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamVendorItem& item = this->m_items[i];
        item.m_extendedCostID = bits.ReadBit() == 0;
        item.m_playerConditionID = bits.ReadBit() == 0;
    }

    this->m_vendor[4] = bits.ReadBit();
    uint8_t byteValue;
    uint32_t intValue;

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamVendorItem& item = this->m_items[i];

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_muid = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_durability = intValue;
        }

        if (item.m_extendedCostID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_extendedCostID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_item = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_type = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_price = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_displayID = intValue;
        }

        if (item.m_playerConditionID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_playerConditionID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_quantity = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_stackCount = intValue;
        }
    }

    if (this->m_vendor[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[5] ^= byteValue;
    }

    if (this->m_vendor[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[4] ^= byteValue;
    }

    if (this->m_vendor[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[1] ^= byteValue;
    }

    if (this->m_vendor[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[0] ^= byteValue;
    }

    if (this->m_vendor[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[6] ^= byteValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_reason = byteValue;
    }

    if (this->m_vendor[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[2] ^= byteValue;
    }

    if (this->m_vendor[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[3] ^= byteValue;
    }

    if (this->m_vendor[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[7] ^= byteValue;
    }
}

// PROVEN: a142c244 IDENTICAL/UNPROVEN
// OFFSET: 0x006E5360/0x1403A66D0
bool ClientVendorInventory::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E4570/0x1403A5760
bool ClientVendorInventory::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_vendor[1] = bits.ReadBit();
    this->m_vendor[0] = bits.ReadBit();
    uint32_t count = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((2 * count + 7) >> 3) + 32 * count) {
        return false;
    }

    this->m_items.SetCount(count);
    this->m_vendor[3] = bits.ReadBit();
    this->m_vendor[6] = bits.ReadBit();
    this->m_vendor[5] = bits.ReadBit();
    this->m_vendor[2] = bits.ReadBit();
    this->m_vendor[7] = bits.ReadBit();

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamVendorItem& item = this->m_items[i];
        item.m_extendedCostID = bits.ReadBit() == 0;
        item.m_playerConditionID = bits.ReadBit() == 0;
    }

    this->m_vendor[4] = bits.ReadBit();
    uint8_t byteValue;
    uint32_t intValue;

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamVendorItem& item = this->m_items[i];

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_muid = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_durability = intValue;
        }

        if (item.m_extendedCostID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_extendedCostID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_item = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_type = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_price = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_displayID = intValue;
        }

        if (item.m_playerConditionID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_playerConditionID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_quantity = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_stackCount = intValue;
        }
    }

    if (this->m_vendor[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[5] ^= byteValue;
    }

    if (this->m_vendor[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[4] ^= byteValue;
    }

    if (this->m_vendor[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[1] ^= byteValue;
    }

    if (this->m_vendor[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[0] ^= byteValue;
    }

    if (this->m_vendor[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[6] ^= byteValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_reason = byteValue;
    }

    if (this->m_vendor[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[2] ^= byteValue;
    }

    if (this->m_vendor[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[3] ^= byteValue;
    }

    if (this->m_vendor[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[7] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 19a1e543 IDENTICAL/UNPROVEN
// OFFSET: 0x006AA040/0x140359600
bool ClientVendorInventory::Handle() {
    if (ClientVendorInventory::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE1B8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE1B8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVendorInventory*)>(reinterpret_cast<uintptr_t>(ClientVendorInventory::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: ee6b718b IDENTICAL/IDENTICAL
// OFFSET: 0x006A1640/0x14034E980
ClientPetSlotUpdated::ClientPetSlotUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8b19eaf9 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0E10/0x14034DEB0
ClientPetSlotUpdated::~ClientPetSlotUpdated() {
}

// PROVEN: 81be20c9 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0E40/0x14034DEE0
void ClientPetSlotUpdated::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_newSlot = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_prevPetNumber = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_petNumber = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_prevSlot = value;
    }
}

// PROVEN: 925631b9 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAB10/0x140382C80
bool ClientPetSlotUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: fde91b02 IDENTICAL/IDENTICAL
// OFFSET: 0x006C47A0/0x14037A6B0
bool ClientPetSlotUpdated::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_newSlot = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_prevPetNumber = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_petNumber = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_prevSlot = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 93f68099 IDENTICAL/UNPROVEN
// OFFSET: 0x006A92A0/0x140358420
bool ClientPetSlotUpdated::Handle() {
    if (ClientPetSlotUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF628.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF628.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPetSlotUpdated*)>(reinterpret_cast<uintptr_t>(ClientPetSlotUpdated::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 4c0e3b26 IDENTICAL/IDENTICAL
// OFFSET: 0x006B35E0/0x1403650D0
ClientPetAdded::ClientPetAdded(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c66a67bf IDENTICAL/IDENTICAL
// OFFSET: 0x006A12C0/0x14034E590
ClientPetAdded::~ClientPetAdded() {
}

// PROVEN: 3611807e IDENTICAL/DIFFERENT
// OFFSET: 0x006A7630/0x140356120
void ClientPetAdded::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_level = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_slot = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_flags = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_creatureID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_petNumber = value;
    }

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;
}

// PROVEN: 8ebd498d IDENTICAL/UNPROVEN
// OFFSET: 0x006ADFB0/0x14035E500
bool ClientPetAdded::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9f52488a IDENTICAL/DIFFERENT
// OFFSET: 0x006A98B0/0x140358C30
bool ClientPetAdded::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_level = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_slot = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_flags = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_creatureID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_petNumber = value;
    }

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;

    if (length >= sizeof(this->m_name)) {
        return false;
    }

    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 2b6c285b IDENTICAL/NONE
// OFFSET: 0x006A20E0
bool ClientPetAdded::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
            break;
        }

        terminated |= this->m_name[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: a424f711 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6D40/0x140369940
bool ClientPetAdded::Handle() {
    if (ClientPetAdded::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDE78.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDE78.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPetAdded*)>(reinterpret_cast<uintptr_t>(ClientPetAdded::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 8694c9a8 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3270/0x14034F7C0
ClientCurrencyLootRemoved::ClientCurrencyLootRemoved(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_lootListID = value;
}

// PROVEN: 44c7c9ef IDENTICAL/UNPROVEN
// OFFSET: 0x006A19B0/0x14034E9E0
ClientCurrencyLootRemoved::~ClientCurrencyLootRemoved() {
}

// PROVEN: 882000ea IDENTICAL/IDENTICAL
// OFFSET: 0x006BA100/0x14037E740
bool ClientCurrencyLootRemoved::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_lootListID = value;

    return msg->IsRead() != 0;
}

// PROVEN: 6b70c87c IDENTICAL/UNPROVEN
// OFFSET: 0x006B0650/0x1403613F0
bool ClientCurrencyLootRemoved::Handle() {
    if (ClientCurrencyLootRemoved::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6A4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6A4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientCurrencyLootRemoved*)>(reinterpret_cast<uintptr_t>(ClientCurrencyLootRemoved::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: f156ebdc IDENTICAL/IDENTICAL
// OFFSET: 0x006A9430/0x140358630
ClientCurrencyLootRestored::ClientCurrencyLootRestored(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 069ba1e7 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0D70/0x14034DD90
ClientCurrencyLootRestored::~ClientCurrencyLootRestored() {
}

// PROVEN: 19fad90a IDENTICAL/NONE
// OFFSET: 0x006A1B80
void ClientCurrencyLootRestored::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_quantity = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootListID = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_currencyID = value;
    }
}

// PROVEN: 176b1fc5 IDENTICAL/UNPROVEN
// OFFSET: 0x006B2E40/0x140364730
bool ClientCurrencyLootRestored::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ae1e94f4 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2140/0x14034F620
bool ClientCurrencyLootRestored::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_quantity = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootListID = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_currencyID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 9ecddc97 IDENTICAL/UNPROVEN
// OFFSET: 0x006B06E0/0x1403614B0
bool ClientCurrencyLootRestored::Handle() {
    if (ClientCurrencyLootRestored::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE850.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE850.m_left->Get();
        return reinterpret_cast<bool (*)(ClientCurrencyLootRestored*)>(reinterpret_cast<uintptr_t>(ClientCurrencyLootRestored::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 5445a217 IDENTICAL/IDENTICAL
// OFFSET: 0x006EF050/0x1403B4150
ClientLootContents::ClientLootContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 20fceee8 IDENTICAL/IDENTICAL
// OFFSET: 0x006E02A0/0x1403A02D0
ClientLootContents::~ClientLootContents() {
}

// OFFSET: 0x006E5860/0x1403A6D80
void ClientLootContents::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_items.SetCount(bits.ReadBits<21>());
    uint32_t value;

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamLootItem& item = this->m_items[i];
        value = 0;
        msg->Get(value);
        item.m_displayID = value;
        value = 0;
        msg->Get(value);
        item.m_randomSuffix = value;
        value = 0;
        msg->Get(value);
        item.m_quantity = value;
        value = 0;
        msg->Get(value);
        item.m_itemID = value;
        value = 0;
        msg->Get(value);
        item.m_randomPropertiesID = value;
    }
}

// PROVEN: 8ffc7d15 IDENTICAL/UNPROVEN
// OFFSET: 0x006ECD90/0x1403B11A0
bool ClientLootContents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E23F0/0x1403A2EB0
bool ClientLootContents::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < 20 * count) {
        return false;
    }

    this->m_items.SetCount(count);
    uint32_t value;

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamLootItem& item = this->m_items[i];
        value = 0;
        msg->Get(value);
        item.m_displayID = value;
        value = 0;
        msg->Get(value);
        item.m_randomSuffix = value;
        value = 0;
        msg->Get(value);
        item.m_quantity = value;
        value = 0;
        msg->Get(value);
        item.m_itemID = value;
        value = 0;
        msg->Get(value);
        item.m_randomPropertiesID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: e39ad5a0 IDENTICAL/UNPROVEN
// OFFSET: 0x006B81A0/0x14036B250
bool ClientLootContents::Handle() {
    if (ClientLootContents::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE748.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE748.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLootContents*)>(reinterpret_cast<uintptr_t>(ClientLootContents::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: e3dee12f IDENTICAL/IDENTICAL
// OFFSET: 0x006E1710/0x1403A1DF0
ClientPartyInvite::ClientPartyInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 07497059 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAB30/0x140382CA0
ClientPartyInvite::~ClientPartyInvite() {
}

// OFFSET: 0x006DAF30/0x140399730
void ClientPartyInvite::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_mustBeBNetFriend = bits.ReadBit() != 0;
    this->m_inviterGUID[0] = bits.ReadBit();
    this->m_inviterGUID[3] = bits.ReadBit();
    this->m_inviterGUID[2] = bits.ReadBit();
    this->m_canAccept = bits.ReadBit() != 0;
    this->m_inviterGUID[6] = bits.ReadBit();
    this->m_inviterGUID[5] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_inviterRealmName) = bits.ReadBits<9>();
    this->m_inviterGUID[4] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_inviterName) = bits.ReadBits<7>();
    this->m_lfgSlots.SetCount(bits.ReadBits<24>());
    this->m_isXRealm = bits.ReadBit() != 0;
    this->m_inviterGUID[1] = bits.ReadBit();
    this->m_inviterGUID[7] = bits.ReadBit();

    uint8_t byteValue;
    uint32_t intValue;

    if (this->m_inviterGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[1] ^= byteValue;
    }

    if (this->m_inviterGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[4] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_inviterCfgRealmID = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_proposedRoles = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_lfgCompletedMask = intValue;
    }

    if (this->m_inviterGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[6] ^= byteValue;
    }

    if (this->m_inviterGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[0] ^= byteValue;
    }

    if (this->m_inviterGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[2] ^= byteValue;
    }

    if (this->m_inviterGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[3] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->m_lfgSlots.Count(); i++) {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_lfgSlots[i] = intValue;
    }

    if (this->m_inviterGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[5] ^= byteValue;
    }

    uint32_t length = *reinterpret_cast<uint32_t*>(this->m_inviterRealmName);
    msg->GetData(this->m_inviterRealmName, length);
    this->m_inviterRealmName[length] = 0;

    if (this->m_inviterGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[7] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_serial = intValue;
    }
}

// PROVEN: 8e8ea71c IDENTICAL/UNPROVEN
// OFFSET: 0x006DAE60/0x1403996D0
bool ClientPartyInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D8A00/0x140396CC0
bool ClientPartyInvite::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_mustBeBNetFriend = bits.ReadBit() != 0;
    this->m_inviterGUID[0] = bits.ReadBit();
    this->m_inviterGUID[3] = bits.ReadBit();
    this->m_inviterGUID[2] = bits.ReadBit();
    this->m_canAccept = bits.ReadBit() != 0;
    this->m_inviterGUID[6] = bits.ReadBit();
    this->m_inviterGUID[5] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<9>();
        *reinterpret_cast<uint32_t*>(this->m_inviterRealmName) = length;

        if (length >= sizeof(this->m_inviterRealmName)) {
            return false;
        }
    }
    this->m_inviterGUID[4] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<7>();
        *reinterpret_cast<uint32_t*>(this->m_inviterName) = length;

        if (length >= sizeof(this->m_inviterName)) {
            return false;
        }
    }
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->m_lfgSlots.SetCount(count);
    this->m_isXRealm = bits.ReadBit() != 0;
    this->m_inviterGUID[1] = bits.ReadBit();
    this->m_inviterGUID[7] = bits.ReadBit();

    uint8_t byteValue;
    uint32_t intValue;

    if (this->m_inviterGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[1] ^= byteValue;
    }

    if (this->m_inviterGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[4] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_inviterCfgRealmID = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_proposedRoles = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_lfgCompletedMask = intValue;
    }

    if (this->m_inviterGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[6] ^= byteValue;
    }

    if (this->m_inviterGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[0] ^= byteValue;
    }

    if (this->m_inviterGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[2] ^= byteValue;
    }

    if (this->m_inviterGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[3] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->m_lfgSlots.Count(); i++) {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_lfgSlots[i] = intValue;
    }

    if (this->m_inviterGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[5] ^= byteValue;
    }

    uint32_t length = *reinterpret_cast<uint32_t*>(this->m_inviterRealmName);
    msg->GetData(this->m_inviterRealmName, length);
    this->m_inviterRealmName[length] = 0;

    if (this->m_inviterGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[7] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_serial = intValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 987d9fc6 IDENTICAL/DIFFERENT
// OFFSET: 0x006A13C0/0x14034FFA0
bool ClientPartyInvite::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_inviterName))) {
                break;
            }

            terminated |= this->m_inviterName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_inviterRealmName))) {
                break;
            }

            terminated |= this->m_inviterRealmName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: e5a89f46 IDENTICAL/UNPROVEN
// OFFSET: 0x006AC2C0/0x14035C140
bool ClientPartyInvite::Handle() {
    if (ClientPartyInvite::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFB4C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFB4C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPartyInvite*)>(reinterpret_cast<uintptr_t>(ClientPartyInvite::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b3b75ff4 IDENTICAL/IDENTICAL
// OFFSET: 0x006B1AF0/0x140362E70
ClientTradeStatus::ClientTradeStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 61feff12 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0EE0/0x14034DFC0
ClientTradeStatus::~ClientTradeStatus() {
}

// PROVEN: 4fe0cd66 IDENTICAL/IDENTICAL
// OFFSET: 0x006B16A0/0x1403628D0
void ClientTradeStatus::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_partnerIsSameBnetAccount = bits.ReadBit() != 0;
    this->m_status = bits.ReadBits<5>();

    if (this->m_status == 12) {
        this->m_partner[2] = bits.ReadBit();
        this->m_partner[4] = bits.ReadBit();
        this->m_partner[6] = bits.ReadBit();
        this->m_partner[0] = bits.ReadBit();
        this->m_partner[1] = bits.ReadBit();
        this->m_partner[3] = bits.ReadBit();
        this->m_partner[7] = bits.ReadBit();
        this->m_partner[5] = bits.ReadBit();
    }

    if (this->m_status == 31) {
        this->m_failureForYou = bits.ReadBit() != 0;
    }

    uint8_t byteValue;
    uint32_t intValue;

    if (this->m_status == 31) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_bagResult = intValue;
        }
    }

    if (this->m_status == 12) {
        if (this->m_partner[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[4] ^= byteValue;
        }

        if (this->m_partner[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[1] ^= byteValue;
        }

        if (this->m_partner[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[2] ^= byteValue;
        }

        if (this->m_partner[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[3] ^= byteValue;
        }

        if (this->m_partner[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[0] ^= byteValue;
        }

        if (this->m_partner[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[7] ^= byteValue;
        }

        if (this->m_partner[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[6] ^= byteValue;
        }

        if (this->m_partner[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[5] ^= byteValue;
        }
    }

    if (this->m_status == 24 || this->m_status == 19) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_currencyQuantity = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_currencyType = intValue;
        }
    }

    if (this->m_status == 2 || this->m_status == 26) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_tradeSlot = byteValue;
    }

    if (this->m_status == 0) {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_id = intValue;
    }
}

// PROVEN: b47ffa06 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAB20/0x140382C90
bool ClientTradeStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f9dc52e8 IDENTICAL/DIFFERENT
// OFFSET: 0x006C61B0/0x14037C5E0
bool ClientTradeStatus::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_partnerIsSameBnetAccount = bits.ReadBit() != 0;
    this->m_status = bits.ReadBits<5>();

    if (this->m_status == 12) {
        this->m_partner[2] = bits.ReadBit();
        this->m_partner[4] = bits.ReadBit();
        this->m_partner[6] = bits.ReadBit();
        this->m_partner[0] = bits.ReadBit();
        this->m_partner[1] = bits.ReadBit();
        this->m_partner[3] = bits.ReadBit();
        this->m_partner[7] = bits.ReadBit();
        this->m_partner[5] = bits.ReadBit();
    }

    if (this->m_status == 31) {
        this->m_failureForYou = bits.ReadBit() != 0;
    }

    uint8_t byteValue;
    uint32_t intValue;

    if (this->m_status == 31) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_bagResult = intValue;
        }
    }

    if (this->m_status == 12) {
        if (this->m_partner[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[4] ^= byteValue;
        }

        if (this->m_partner[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[1] ^= byteValue;
        }

        if (this->m_partner[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[2] ^= byteValue;
        }

        if (this->m_partner[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[3] ^= byteValue;
        }

        if (this->m_partner[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[0] ^= byteValue;
        }

        if (this->m_partner[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[7] ^= byteValue;
        }

        if (this->m_partner[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[6] ^= byteValue;
        }

        if (this->m_partner[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[5] ^= byteValue;
        }
    }

    if (this->m_status == 24 || this->m_status == 19) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_currencyQuantity = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_currencyType = intValue;
        }
    }

    if (this->m_status == 2 || this->m_status == 26) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_tradeSlot = byteValue;
    }

    if (this->m_status == 0) {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_id = intValue;
    }

    return msg->IsRead() && this->IsValid();
}

// OFFSET: 0x006B6490
bool ClientTradeStatus::IsValid() const {
    switch (this->m_status) {
        default:
            return false;

        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
        case 10:
        case 12:
        case 13:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 29:
        case 31:
            return true;
    }
}

// PROVEN: 11bdcf91 IDENTICAL/UNPROVEN
// OFFSET: 0x006C08D0/0x140375C50
bool ClientTradeStatus::Handle() {
    if (ClientTradeStatus::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF0F0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF0F0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientTradeStatus*)>(reinterpret_cast<uintptr_t>(ClientTradeStatus::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// INLINED
JamTradeItem::JamTradeItem() {
    this->m_hasUnwrapped = false;
}

// PROVEN: 62086e1e IDENTICAL/IDENTICAL
// OFFSET: 0x006F2500/0x1403B8D30
ClientTradeUpdated::ClientTradeUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: af3a906f IDENTICAL/IDENTICAL
// OFFSET: 0x006EE960/0x1403B3710
ClientTradeUpdated::~ClientTradeUpdated() {
}

// OFFSET: 0x006EE100/0x1403B2D80
void ClientTradeUpdated::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_id = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currencyType = intValue;
    }

    {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_gold = longValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_proposedEnchantment = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_clientStateIndex = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currencyQuantity = intValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_whichPlayer = byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_stateIndex = intValue;
    }

    JamBitReader bits(msg);
    this->m_items.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamTradeItem& item = this->m_items[i];
        item.m_giftCreator[7] = bits.ReadBit();
        item.m_giftCreator[1] = bits.ReadBit();
        item.m_hasUnwrapped = bits.ReadBit() != 0;
        item.m_giftCreator[3] = bits.ReadBit();

        if (item.m_hasUnwrapped) {
            item.m_unwrapped.m_creator[7] = bits.ReadBit();
            item.m_unwrapped.m_creator[1] = bits.ReadBit();
            item.m_unwrapped.m_creator[4] = bits.ReadBit();
            item.m_unwrapped.m_creator[6] = bits.ReadBit();
            item.m_unwrapped.m_creator[2] = bits.ReadBit();
            item.m_unwrapped.m_creator[3] = bits.ReadBit();
            item.m_unwrapped.m_creator[5] = bits.ReadBit();
            item.m_unwrapped.m_lock = bits.ReadBit() != 0;
            item.m_unwrapped.m_creator[0] = bits.ReadBit();
        }

        item.m_giftCreator[6] = bits.ReadBit();
        item.m_giftCreator[4] = bits.ReadBit();
        item.m_giftCreator[2] = bits.ReadBit();
        item.m_giftCreator[0] = bits.ReadBit();
        item.m_giftCreator[5] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamTradeItem& item = this->m_items[i];

        if (item.m_hasUnwrapped) {
            if (item.m_unwrapped.m_creator[1]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[1] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_enchantID = intValue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_socketEnchant[j] = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_maxDurability = intValue;
            }

            if (item.m_unwrapped.m_creator[6]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[6] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[2]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[2] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[7]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[7] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[4]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[4] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_reforgeID = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_durability = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_randomPropertiesID = intValue;
            }

            if (item.m_unwrapped.m_creator[3]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[3] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_onUseEnchantmentID = intValue;
            }

            if (item.m_unwrapped.m_creator[0]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[0] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_charges = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_randomPropertiesSeed = intValue;
            }

            if (item.m_unwrapped.m_creator[5]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[5] ^= byteValue;
            }
        }

        if (item.m_giftCreator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[6] ^= byteValue;
        }

        if (item.m_giftCreator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[1] ^= byteValue;
        }

        if (item.m_giftCreator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[7] ^= byteValue;
        }

        if (item.m_giftCreator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[4] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_itemID = intValue;
        }

        if (item.m_giftCreator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[0] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_stackCount = intValue;
        }

        if (item.m_giftCreator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_slot = byteValue;
        }

        if (item.m_giftCreator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[2] ^= byteValue;
        }

        if (item.m_giftCreator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[3] ^= byteValue;
        }
    }
}

// PROVEN: ecd203af IDENTICAL/UNPROVEN
// OFFSET: 0x006F4250/0x1403BB600
bool ClientTradeUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F2DF0/0x1403B99D0
bool ClientTradeUpdated::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_id = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currencyType = intValue;
    }

    {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_gold = longValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_proposedEnchantment = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_clientStateIndex = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currencyQuantity = intValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_whichPlayer = byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_stateIndex = intValue;
    }

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < ((9 * count + 7) >> 3) + 9 * count) {
        return false;
    }

    this->m_items.SetCount(count);

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamTradeItem& item = this->m_items[i];
        item.m_giftCreator[7] = bits.ReadBit();
        item.m_giftCreator[1] = bits.ReadBit();
        item.m_hasUnwrapped = bits.ReadBit() != 0;
        item.m_giftCreator[3] = bits.ReadBit();

        if (item.m_hasUnwrapped) {
            item.m_unwrapped.m_creator[7] = bits.ReadBit();
            item.m_unwrapped.m_creator[1] = bits.ReadBit();
            item.m_unwrapped.m_creator[4] = bits.ReadBit();
            item.m_unwrapped.m_creator[6] = bits.ReadBit();
            item.m_unwrapped.m_creator[2] = bits.ReadBit();
            item.m_unwrapped.m_creator[3] = bits.ReadBit();
            item.m_unwrapped.m_creator[5] = bits.ReadBit();
            item.m_unwrapped.m_lock = bits.ReadBit() != 0;
            item.m_unwrapped.m_creator[0] = bits.ReadBit();
        }

        item.m_giftCreator[6] = bits.ReadBit();
        item.m_giftCreator[4] = bits.ReadBit();
        item.m_giftCreator[2] = bits.ReadBit();
        item.m_giftCreator[0] = bits.ReadBit();
        item.m_giftCreator[5] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        JamTradeItem& item = this->m_items[i];

        if (item.m_hasUnwrapped) {
            if (item.m_unwrapped.m_creator[1]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[1] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_enchantID = intValue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_socketEnchant[j] = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_maxDurability = intValue;
            }

            if (item.m_unwrapped.m_creator[6]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[6] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[2]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[2] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[7]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[7] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[4]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[4] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_reforgeID = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_durability = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_randomPropertiesID = intValue;
            }

            if (item.m_unwrapped.m_creator[3]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[3] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_onUseEnchantmentID = intValue;
            }

            if (item.m_unwrapped.m_creator[0]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[0] ^= byteValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_charges = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_randomPropertiesSeed = intValue;
            }

            if (item.m_unwrapped.m_creator[5]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[5] ^= byteValue;
            }
        }

        if (item.m_giftCreator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[6] ^= byteValue;
        }

        if (item.m_giftCreator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[1] ^= byteValue;
        }

        if (item.m_giftCreator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[7] ^= byteValue;
        }

        if (item.m_giftCreator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[4] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_itemID = intValue;
        }

        if (item.m_giftCreator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[0] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_stackCount = intValue;
        }

        if (item.m_giftCreator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_slot = byteValue;
        }

        if (item.m_giftCreator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[2] ^= byteValue;
        }

        if (item.m_giftCreator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[3] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: d304a28a IDENTICAL/UNPROVEN
// OFFSET: 0x006B1600/0x140362800
bool ClientTradeUpdated::Handle() {
    if (ClientTradeUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD350.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD350.m_left->Get();
        return reinterpret_cast<bool (*)(ClientTradeUpdated*)>(reinterpret_cast<uintptr_t>(ClientTradeUpdated::s_handler) - (right ^ left))(this);
    }

    return false;
}

// OFFSET: 0x006F4190/0x1403BB500
ClientGuildRoster::ClientGuildRoster(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d7163deb IDENTICAL/IDENTICAL
// OFFSET: 0x006DAE30/0x140399690
ClientGuildRoster::~ClientGuildRoster() {
}

// OFFSET: 0x006F2550/0x1403B8D90
void ClientGuildRoster::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;
    float floatValue;
    uint32_t length;

    JamBitReader bits(msg);
    *reinterpret_cast<uint32_t*>(this->m_motd) = bits.ReadBits<11>();
    this->m_memberData.SetCount(bits.ReadBits<18>());

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];
        member.m_guid[3] = bits.ReadBit();
        member.m_guid[4] = bits.ReadBit();
        member.m_authenticated = bits.ReadBit() != 0;
        member.m_sorEligible = bits.ReadBit() != 0;
        *reinterpret_cast<uint32_t*>(member.m_note) = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(member.m_officerNote) = bits.ReadBits<8>();
        member.m_guid[0] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(member.m_name) = bits.ReadBits<7>();
        member.m_guid[1] = bits.ReadBit();
        member.m_guid[2] = bits.ReadBit();
        member.m_guid[6] = bits.ReadBit();
        member.m_guid[5] = bits.ReadBit();
        member.m_guid[7] = bits.ReadBit();
    }

    *reinterpret_cast<uint32_t*>(this->m_infoText) = bits.ReadBits<12>();

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];
        byteValue = 0;
        msg->Get(byteValue);
        member.m_classID = byteValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_guildReputation = intValue;

        if (member.m_guid[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[0] ^= byteValue;
        }

        longValue = 0;
        msg->Get(longValue);
        member.m_weeklyXP = longValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_rankID = intValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_personalAchievementPoints = intValue;

        for (uint32_t j = 0; j < 2; j++) {
            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_step = intValue;

            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_rank = intValue;

            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_dbID = intValue;
        }

        if (member.m_guid[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[2] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        member.m_status = byteValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_areaID = intValue;

        longValue = 0;
        msg->Get(longValue);
        member.m_totalXP = longValue;

        if (member.m_guid[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[7] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        member.m_guildRepToCap = intValue;

        length = *reinterpret_cast<uint32_t*>(member.m_note);
        msg->GetData(member.m_note, length);
        member.m_note[length] = 0;

        if (member.m_guid[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[3] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        member.m_level = byteValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_bgRating = intValue;

        if (member.m_guid[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[5] ^= byteValue;
        }

        if (member.m_guid[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[4] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        member.m_gender = byteValue;

        if (member.m_guid[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[1] ^= byteValue;
        }

        floatValue = 0.0f;
        msg->Get(floatValue);
        member.m_lastSave = floatValue;

        length = *reinterpret_cast<uint32_t*>(member.m_officerNote);
        msg->GetData(member.m_officerNote, length);
        member.m_officerNote[length] = 0;

        if (member.m_guid[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[6] ^= byteValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_name);
        msg->GetData(member.m_name, length);
        member.m_name[length] = 0;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_infoText);
    msg->GetData(this->m_infoText, length);
    this->m_infoText[length] = 0;

    length = *reinterpret_cast<uint32_t*>(this->m_motd);
    msg->GetData(this->m_motd, length);
    this->m_motd[length] = 0;

    intValue = 0;
    msg->Get(intValue);
    this->m_numAccounts = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_weeklyRepCap = intValue;

    *msg >> this->m_createDate;

    intValue = 0;
    msg->Get(intValue);
    this->m_guildFlags = intValue;
}

// PROVEN: 82cb211a IDENTICAL/UNPROVEN
// OFFSET: 0x006ED360/0x1403B19C0
bool ClientGuildRoster::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E8BC0/0x1403AB620
bool ClientGuildRoster::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;
    float floatValue;
    uint32_t length;

    JamBitReader bits(msg);
    {
        uint32_t length = bits.ReadBits<11>();
        *reinterpret_cast<uint32_t*>(this->m_motd) = length;

        if (length >= sizeof(this->m_motd)) {
            return false;
        }
    }
    uint32_t count = bits.ReadBits<18>();

    if (msg->m_size - msg->m_read < ((33 * count + 7) >> 3) + 72 * count) {
        return false;
    }

    this->m_memberData.SetCount(count);

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];
        member.m_guid[3] = bits.ReadBit();
        member.m_guid[4] = bits.ReadBit();
        member.m_authenticated = bits.ReadBit() != 0;
        member.m_sorEligible = bits.ReadBit() != 0;

        *reinterpret_cast<uint32_t*>(member.m_note) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(member.m_note) >= sizeof(member.m_note)) {
            return false;
        }

        *reinterpret_cast<uint32_t*>(member.m_officerNote) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(member.m_officerNote) >= sizeof(member.m_officerNote)) {
            return false;
        }

        member.m_guid[0] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(member.m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(member.m_name) >= sizeof(member.m_name)) {
            return false;
        }

        member.m_guid[1] = bits.ReadBit();
        member.m_guid[2] = bits.ReadBit();
        member.m_guid[6] = bits.ReadBit();
        member.m_guid[5] = bits.ReadBit();
        member.m_guid[7] = bits.ReadBit();
    }

    {
        uint32_t length = bits.ReadBits<12>();
        *reinterpret_cast<uint32_t*>(this->m_infoText) = length;

        if (length >= sizeof(this->m_infoText)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_classID = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildReputation = intValue;
        }

        if (member.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[0] ^= byteValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_weeklyXP = longValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_rankID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_personalAchievementPoints = intValue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_step = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_rank = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_dbID = intValue;
            }
        }

        if (member.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[2] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_status = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_areaID = intValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_totalXP = longValue;
        }

        if (member.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[7] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildRepToCap = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_note);
        msg->GetData(member.m_note, length);
        member.m_note[length] = 0;

        if (member.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[3] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_level = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_bgRating = intValue;
        }

        if (member.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[5] ^= byteValue;
        }

        if (member.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[4] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_gender = byteValue;
        }

        if (member.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[1] ^= byteValue;
        }

        {
            float floatValue = 0.0f;
            msg->Get(floatValue);
            member.m_lastSave = floatValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_officerNote);
        msg->GetData(member.m_officerNote, length);
        member.m_officerNote[length] = 0;

        if (member.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[6] ^= byteValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_name);
        msg->GetData(member.m_name, length);
        member.m_name[length] = 0;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_infoText);
    msg->GetData(this->m_infoText, length);
    this->m_infoText[length] = 0;

    length = *reinterpret_cast<uint32_t*>(this->m_motd);
    msg->GetData(this->m_motd, length);
    this->m_motd[length] = 0;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numAccounts = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_weeklyRepCap = intValue;
    }

    *msg >> this->m_createDate;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_guildFlags = intValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 6a5d8640 IDENTICAL/IDENTICAL
// OFFSET: 0x006A65C0/0x1403548C0
bool ClientGuildRoster::IsValid() const {
    for (uint32_t i = 0; i < this->m_memberData.m_count; i++) {
        if (!this->m_memberData.m_data[i].IsValid()) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t j = 0; !terminated; j++) {
            if (j >= static_cast<int32_t>(sizeof(this->m_infoText))) {
                break;
            }

            terminated |= this->m_infoText[j] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t j = 0; !terminated; j++) {
            if (j >= static_cast<int32_t>(sizeof(this->m_motd))) {
                break;
            }

            terminated |= this->m_motd[j] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: 02dcd90c IDENTICAL/UNPROVEN
// OFFSET: 0x006A5B70/0x140353AF0
bool ClientGuildRoster::Handle() {
    if (ClientGuildRoster::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDE64.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDE64.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildRoster*)>(reinterpret_cast<uintptr_t>(ClientGuildRoster::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 017613d2 IDENTICAL/IDENTICAL
// OFFSET: 0x006F0820/0x1403B6200
ClientGuildRosterUpdate::ClientGuildRosterUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 64e29f58 IDENTICAL/IDENTICAL
// OFFSET: 0x006D59C0/0x140392CF0
ClientGuildRosterUpdate::~ClientGuildRosterUpdate() {
}

// OFFSET: 0x006EF8C0/0x1403B4DB0
void ClientGuildRosterUpdate::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;
    float floatValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_memberData.SetCount(bits.ReadBits<18>());

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];
        *reinterpret_cast<uint32_t*>(member.m_name) = bits.ReadBits<7>();
        member.m_guid[5] = bits.ReadBit();
        member.m_guid[4] = bits.ReadBit();
        member.m_guid[0] = bits.ReadBit();
        member.m_guid[7] = bits.ReadBit();
        member.m_guid[1] = bits.ReadBit();
        member.m_guid[3] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(member.m_note) = bits.ReadBits<8>();
        member.m_guid[6] = bits.ReadBit();
        member.m_sorEligible = bits.ReadBit() != 0;
        member.m_guid[2] = bits.ReadBit();
        member.m_authenticated = bits.ReadBit() != 0;
        *reinterpret_cast<uint32_t*>(member.m_officerNote) = bits.ReadBits<8>();
    }

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_gender = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildRepToCap = intValue;
        }

        if (member.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[1] ^= byteValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_totalXP = longValue;
        }

        if (member.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[2] ^= byteValue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_step = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_dbID = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_rank = intValue;
            }
        }

        if (member.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[0] ^= byteValue;
        }

        if (member.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[6] ^= byteValue;
        }

        if (member.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[7] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_rankID = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_note);
        msg->GetData(member.m_note, length);
        member.m_note[length] = 0;

        length = *reinterpret_cast<uint32_t*>(member.m_officerNote);
        msg->GetData(member.m_officerNote, length);
        member.m_officerNote[length] = 0;

        if (member.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[4] ^= byteValue;
        }

        if (member.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[5] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildReputation = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_personalAchievementPoints = intValue;
        }

        {
            float floatValue = 0.0f;
            msg->Get(floatValue);
            member.m_lastSave = floatValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_weeklyXP = longValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_level = byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_classID = byteValue;
        }

        if (member.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[3] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_status = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_areaID = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_name);
        msg->GetData(member.m_name, length);
        member.m_name[length] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_bgRating = intValue;
        }
    }
}

// PROVEN: e063cdf1 IDENTICAL/UNPROVEN
// OFFSET: 0x006F4260/0x1403BB610
bool ClientGuildRosterUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EA290/0x1403AD570
bool ClientGuildRosterUpdate::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;
    float floatValue;
    uint32_t length;

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<18>();

    if (msg->m_size - msg->m_read < ((33 * count + 7) >> 3) + 72 * count) {
        return false;
    }

    this->m_memberData.SetCount(count);

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];
        *reinterpret_cast<uint32_t*>(member.m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(member.m_name) >= sizeof(member.m_name)) {
            return false;
        }

        member.m_guid[5] = bits.ReadBit();
        member.m_guid[4] = bits.ReadBit();
        member.m_guid[0] = bits.ReadBit();
        member.m_guid[7] = bits.ReadBit();
        member.m_guid[1] = bits.ReadBit();
        member.m_guid[3] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(member.m_note) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(member.m_note) >= sizeof(member.m_note)) {
            return false;
        }

        member.m_guid[6] = bits.ReadBit();
        member.m_sorEligible = bits.ReadBit() != 0;
        member.m_guid[2] = bits.ReadBit();
        member.m_authenticated = bits.ReadBit() != 0;

        *reinterpret_cast<uint32_t*>(member.m_officerNote) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(member.m_officerNote) >= sizeof(member.m_officerNote)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->m_memberData.Count(); i++) {
        ClientGuildRosterMemberData& member = this->m_memberData[i];

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_gender = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildRepToCap = intValue;
        }

        if (member.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[1] ^= byteValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_totalXP = longValue;
        }

        if (member.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[2] ^= byteValue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_step = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_dbID = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                member.m_profession[j].m_rank = intValue;
            }
        }

        if (member.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[0] ^= byteValue;
        }

        if (member.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[6] ^= byteValue;
        }

        if (member.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[7] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_rankID = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_note);
        msg->GetData(member.m_note, length);
        member.m_note[length] = 0;

        length = *reinterpret_cast<uint32_t*>(member.m_officerNote);
        msg->GetData(member.m_officerNote, length);
        member.m_officerNote[length] = 0;

        if (member.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[4] ^= byteValue;
        }

        if (member.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[5] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_guildReputation = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_personalAchievementPoints = intValue;
        }

        {
            float floatValue = 0.0f;
            msg->Get(floatValue);
            member.m_lastSave = floatValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            member.m_weeklyXP = longValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_level = byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_classID = byteValue;
        }

        if (member.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[3] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            member.m_status = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_areaID = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(member.m_name);
        msg->GetData(member.m_name, length);
        member.m_name[length] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            member.m_bgRating = intValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: b7ed1fa5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A5B20/0x140353A90
bool ClientGuildRosterUpdate::Handle() {
    if (ClientGuildRosterUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF664.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF664.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildRosterUpdate*)>(reinterpret_cast<uintptr_t>(ClientGuildRosterUpdate::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 5296eda3 IDENTICAL/IDENTICAL
// OFFSET: 0x006E9810/0x1403AC850
ClientGuildRewardList::ClientGuildRewardList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ed15f47 IDENTICAL/IDENTICAL
// OFFSET: 0x006E1FC0/0x1403A2820
ClientGuildRewardList::~ClientGuildRewardList() {
}

// OFFSET: 0x006E3580/0x1403A42E0
void ClientGuildRewardList::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint64_t longValue;

    JamBitReader bits(msg);
    this->m_rewardItems.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->m_rewardItems.Count(); i++) {
        JamGuildRewardItem& item = this->m_rewardItems[i];
        intValue = 0;
        msg->Get(intValue);
        item.m_minGuildRep = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_raceMask = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_itemID = intValue;

        longValue = 0;
        msg->Get(longValue);
        item.m_cost = longValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_minGuildLevel = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_achievementRequired = intValue;
    }

    intValue = 0;
    msg->Get(intValue);
    this->m_version = intValue;
}

// PROVEN: 66751f62 IDENTICAL/UNPROVEN
// OFFSET: 0x006E1E50/0x1403A2670
bool ClientGuildRewardList::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DD610/0x14039CB00
bool ClientGuildRewardList::Read(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint64_t longValue;

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < 28 * count) {
        return false;
    }

    this->m_rewardItems.SetCount(count);

    for (uint32_t i = 0; i < this->m_rewardItems.Count(); i++) {
        JamGuildRewardItem& item = this->m_rewardItems[i];

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_minGuildRep = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_raceMask = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_itemID = intValue;
        }

        {
            uint64_t longValue = 0;
            msg->Get(longValue);
            item.m_cost = longValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_minGuildLevel = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_achievementRequired = intValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_version = intValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: e0bdb004 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0180/0x140375430
bool ClientGuildRewardList::Handle() {
    if (ClientGuildRewardList::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildRewardList*)>(reinterpret_cast<uintptr_t>(ClientGuildRewardList::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006F4EC0/0x1403BC680
void ClientPvpLogData::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_hasTeamNames = bits.ReadBit();
    this->m_hasRatings = bits.ReadBit();

    if (this->m_hasTeamNames) {
        *reinterpret_cast<uint32_t*>(this->m_teamNames[1]) = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->m_teamNames[0]) = bits.ReadBits<8>();
    }

    this->m_players.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        ClientPVPLogData_Player& player = this->m_players[i];
        player.unk_000D = bits.ReadBit();
        player.m_hasRating = bits.ReadBit();
        player.m_playerGUID[2] = bits.ReadBit();
        player.m_hasHonor = bits.ReadBit();
        player.m_hasPreMatchMMR = bits.ReadBit();
        player.m_hasRatingChange = bits.ReadBit();
        player.m_hasMmrChange = bits.ReadBit();
        player.m_playerGUID[3] = bits.ReadBit();
        player.m_playerGUID[0] = bits.ReadBit();
        player.m_playerGUID[5] = bits.ReadBit();
        player.m_playerGUID[1] = bits.ReadBit();
        player.m_playerGUID[6] = bits.ReadBit();
        player.m_faction = bits.ReadBit();
        player.m_playerGUID[7] = bits.ReadBit();
        player.m_stats.SetCount(bits.ReadBits<24>());
        player.m_playerGUID[4] = bits.ReadBit();
    }

    this->m_hasWinner = bits.ReadBit();

    if (this->m_hasRatings) {
        uint32_t intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_matchmakingRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_prematchRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_newRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_matchmakingRating[1] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_prematchRating[1] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_newRating[1] = intValue;
    }

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        ClientPVPLogData_Player& player = this->m_players[i];
        uint8_t byteValue;
        uint32_t intValue;

        intValue = 0;
        msg->Get(intValue);
        player.m_healingDone = intValue;

        intValue = 0;
        msg->Get(intValue);
        player.m_damageDone = intValue;

        if (player.m_hasHonor) {
            intValue = 0;
            msg->Get(intValue);
            player.m_contributionPoints = intValue;

            intValue = 0;
            msg->Get(intValue);
            player.m_deaths = intValue;

            intValue = 0;
            msg->Get(intValue);
            player.m_honorKills = intValue;
        }

        if (player.m_playerGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        player.m_kills = intValue;

        if (player.m_hasRatingChange) {
            intValue = 0;
            msg->Get(intValue);
            player.m_ratingChange = intValue;
        }

        if (player.m_playerGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[5] ^= byteValue;
        }

        if (player.m_hasMmrChange) {
            intValue = 0;
            msg->Get(intValue);
            player.m_mmrChange = intValue;
        }

        if (player.m_hasRating) {
            intValue = 0;
            msg->Get(intValue);
            player.m_rating = intValue;
        }

        if (player.m_playerGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[1] ^= byteValue;
        }

        if (player.m_playerGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[6] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        player.m_primaryTalentTree = intValue;

        for (uint32_t j = 0; j < player.m_stats.Count(); j++) {
            intValue = 0;
            msg->Get(intValue);
            player.m_stats[j] = intValue;
        }

        if (player.m_playerGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[0] ^= byteValue;
        }

        if (player.m_playerGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[3] ^= byteValue;
        }

        if (player.m_hasPreMatchMMR) {
            intValue = 0;
            msg->Get(intValue);
            player.m_preMatchMMR = intValue;
        }

        if (player.m_playerGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[7] ^= byteValue;
        }

        if (player.m_playerGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[2] ^= byteValue;
        }
    }

    if (this->m_hasTeamNames) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_teamNames[1]);
        msg->GetData(this->m_teamNames[1], length);
        this->m_teamNames[1][length] = '\0';
        length = *reinterpret_cast<uint32_t*>(this->m_teamNames[0]);
        msg->GetData(this->m_teamNames[0], length);
        this->m_teamNames[0][length] = '\0';
    }

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_playerCount[0] = byteValue;

    if (this->m_hasWinner) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_winner = byteValue;
    }

    byteValue = 0;
    msg->Get(byteValue);
    this->m_playerCount[1] = byteValue;
}

// PROVEN: 6bcbcc54 IDENTICAL/UNPROVEN
// OFFSET: 0x006F7540/0x1403BEEE0
bool ClientPvpLogData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6510/0x1403BDE40
bool ClientPvpLogData::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_hasTeamNames = bits.ReadBit();
    this->m_hasRatings = bits.ReadBit();

    if (this->m_hasTeamNames) {
        {
            uint32_t length = bits.ReadBits<8>();
            *reinterpret_cast<uint32_t*>(this->m_teamNames[1]) = length;

            if (length >= sizeof(this->m_teamNames[1])) {
                return false;
            }
        }
        {
            uint32_t length = bits.ReadBits<8>();
            *reinterpret_cast<uint32_t*>(this->m_teamNames[0]) = length;

            if (length >= sizeof(this->m_teamNames[0])) {
                return false;
            }
        }
    }

    uint32_t count2 = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((39 * count2 + 7) >> 3) + 16 * count2) {
        return false;
    }

    this->m_players.SetCount(count2);

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        ClientPVPLogData_Player& player = this->m_players[i];
        player.unk_000D = bits.ReadBit();
        player.m_hasRating = bits.ReadBit();
        player.m_playerGUID[2] = bits.ReadBit();
        player.m_hasHonor = bits.ReadBit();
        player.m_hasPreMatchMMR = bits.ReadBit();
        player.m_hasRatingChange = bits.ReadBit();
        player.m_hasMmrChange = bits.ReadBit();
        player.m_playerGUID[3] = bits.ReadBit();
        player.m_playerGUID[0] = bits.ReadBit();
        player.m_playerGUID[5] = bits.ReadBit();
        player.m_playerGUID[1] = bits.ReadBit();
        player.m_playerGUID[6] = bits.ReadBit();
        player.m_faction = bits.ReadBit();
        player.m_playerGUID[7] = bits.ReadBit();
        uint32_t count = bits.ReadBits<24>();

        if (msg->m_size - msg->m_read < 4 * count) {
            return false;
        }

        player.m_stats.SetCount(count);
        player.m_playerGUID[4] = bits.ReadBit();
    }

    this->m_hasWinner = bits.ReadBit();

    if (this->m_hasRatings) {
        uint32_t intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_matchmakingRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_prematchRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_newRating[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_matchmakingRating[1] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_prematchRating[1] = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_newRating[1] = intValue;
    }

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        ClientPVPLogData_Player& player = this->m_players[i];
        uint8_t byteValue;
        uint32_t intValue;

        intValue = 0;
        msg->Get(intValue);
        player.m_healingDone = intValue;

        intValue = 0;
        msg->Get(intValue);
        player.m_damageDone = intValue;

        if (player.m_hasHonor) {
            intValue = 0;
            msg->Get(intValue);
            player.m_contributionPoints = intValue;

            intValue = 0;
            msg->Get(intValue);
            player.m_deaths = intValue;

            intValue = 0;
            msg->Get(intValue);
            player.m_honorKills = intValue;
        }

        if (player.m_playerGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        player.m_kills = intValue;

        if (player.m_hasRatingChange) {
            intValue = 0;
            msg->Get(intValue);
            player.m_ratingChange = intValue;
        }

        if (player.m_playerGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[5] ^= byteValue;
        }

        if (player.m_hasMmrChange) {
            intValue = 0;
            msg->Get(intValue);
            player.m_mmrChange = intValue;
        }

        if (player.m_hasRating) {
            intValue = 0;
            msg->Get(intValue);
            player.m_rating = intValue;
        }

        if (player.m_playerGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[1] ^= byteValue;
        }

        if (player.m_playerGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[6] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        player.m_primaryTalentTree = intValue;

        for (uint32_t j = 0; j < player.m_stats.Count(); j++) {
            intValue = 0;
            msg->Get(intValue);
            player.m_stats[j] = intValue;
        }

        if (player.m_playerGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[0] ^= byteValue;
        }

        if (player.m_playerGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[3] ^= byteValue;
        }

        if (player.m_hasPreMatchMMR) {
            intValue = 0;
            msg->Get(intValue);
            player.m_preMatchMMR = intValue;
        }

        if (player.m_playerGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[7] ^= byteValue;
        }

        if (player.m_playerGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            player.m_playerGUID[2] ^= byteValue;
        }
    }

    if (this->m_hasTeamNames) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_teamNames[1]);
        msg->GetData(this->m_teamNames[1], length);
        this->m_teamNames[1][length] = '\0';
        length = *reinterpret_cast<uint32_t*>(this->m_teamNames[0]);
        msg->GetData(this->m_teamNames[0], length);
        this->m_teamNames[0][length] = '\0';
    }

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_playerCount[0] = byteValue;

    if (this->m_hasWinner) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_winner = byteValue;
    }

    byteValue = 0;
    msg->Get(byteValue);
    this->m_playerCount[1] = byteValue;

    return msg->IsRead() && (!this->m_hasTeamNames || this->m_teamNameData.IsValid());
}

// PROVEN: 56033329 IDENTICAL/NONE
// OFFSET: 0x006A27C0
bool ClientPVPLogData_TeamNames::IsValid() const {
    for (uint32_t i = 0; i < 2; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->m_names[i]))) {
                    break;
                }

                terminated |= this->m_names[i][j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: e5c6111d IDENTICAL/IDENTICAL
// OFFSET: 0x006F64C0/0x1403BDDE0
ClientGuildNews::ClientGuildNews(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4011793e IDENTICAL/IDENTICAL
// OFFSET: 0x006F42E0/0x1403BB6A0
ClientGuildNews::~ClientGuildNews() {
}

// OFFSET: 0x006F4730/0x1403BBBC0
void ClientGuildNews::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    JamBitReader bits(msg);
    this->m_newsEvents.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->m_newsEvents.Count(); i++) {
        JamGuildNewsEvent& news = this->m_newsEvents[i];
        news.m_memberList.SetCount(bits.ReadBits<26>());
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7] = bits.ReadBit();

        for (uint32_t j = 0; j < news.m_memberList.Count(); j++) {
            uint8_t* member = reinterpret_cast<uint8_t*>(&news.m_memberList[j]);
            member[7] = bits.ReadBit();
            member[1] = bits.ReadBit();
            member[5] = bits.ReadBit();
            member[3] = bits.ReadBit();
            member[4] = bits.ReadBit();
            member[6] = bits.ReadBit();
            member[0] = bits.ReadBit();
            member[2] = bits.ReadBit();
        }

        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_newsEvents.Count(); i++) {
        JamGuildNewsEvent& news = this->m_newsEvents[i];

        for (uint32_t j = 0; j < news.m_memberList.Count(); j++) {
            uint8_t* member = reinterpret_cast<uint8_t*>(&news.m_memberList[j]);

            if (member[0]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[0] ^= byteValue;
            }

            if (member[1]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[1] ^= byteValue;
            }

            if (member[4]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[4] ^= byteValue;
            }

            if (member[7]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[7] ^= byteValue;
            }

            if (member[5]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[5] ^= byteValue;
            }

            if (member[6]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[6] ^= byteValue;
            }

            if (member[3]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[3] ^= byteValue;
            }

            if (member[2]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[2] ^= byteValue;
            }
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        news.m_flags = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_data[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_data[1] = intValue;

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        news.m_id = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_type = intValue;

        *msg >> news.m_date;
    }
}

// PROVEN: 6bf1d74b IDENTICAL/UNPROVEN
// OFFSET: 0x006F6310/0x1403BDBE0
bool ClientGuildNews::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F56E0/0x1403BD150
bool ClientGuildNews::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    JamBitReader bits(msg);
    uint32_t count2 = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((34 * count2 + 7) >> 3) + 24 * count2) {
        return false;
    }

    this->m_newsEvents.SetCount(count2);

    for (uint32_t i = 0; i < this->m_newsEvents.Count(); i++) {
        JamGuildNewsEvent& news = this->m_newsEvents[i];
        uint32_t count = bits.ReadBits<26>();

        if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3)) {
            return false;
        }

        news.m_memberList.SetCount(count);
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7] = bits.ReadBit();

        for (uint32_t j = 0; j < news.m_memberList.Count(); j++) {
            uint8_t* member = reinterpret_cast<uint8_t*>(&news.m_memberList[j]);
            member[7] = bits.ReadBit();
            member[1] = bits.ReadBit();
            member[5] = bits.ReadBit();
            member[3] = bits.ReadBit();
            member[4] = bits.ReadBit();
            member[6] = bits.ReadBit();
            member[0] = bits.ReadBit();
            member[2] = bits.ReadBit();
        }

        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1] = bits.ReadBit();
        reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_newsEvents.Count(); i++) {
        JamGuildNewsEvent& news = this->m_newsEvents[i];

        for (uint32_t j = 0; j < news.m_memberList.Count(); j++) {
            uint8_t* member = reinterpret_cast<uint8_t*>(&news.m_memberList[j]);

            if (member[0]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[0] ^= byteValue;
            }

            if (member[1]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[1] ^= byteValue;
            }

            if (member[4]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[4] ^= byteValue;
            }

            if (member[7]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[7] ^= byteValue;
            }

            if (member[5]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[5] ^= byteValue;
            }

            if (member[6]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[6] ^= byteValue;
            }

            if (member[3]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[3] ^= byteValue;
            }

            if (member[2]) {
                byteValue = 0;
                msg->Get(byteValue);
                member[2] ^= byteValue;
            }
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[5] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        news.m_flags = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_data[0] = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_data[1] = intValue;

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[7] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[6] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[2] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[3] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[0] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[4] ^= byteValue;
        }

        if (reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            reinterpret_cast<uint8_t*>(&news.m_memberGUID)[1] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        news.m_id = intValue;

        intValue = 0;
        msg->Get(intValue);
        news.m_type = intValue;

        *msg >> news.m_date;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 0fa146bd IDENTICAL/UNPROVEN
// OFFSET: 0x006AE160/0x14035E710
bool ClientGuildNews::Handle() {
    if (ClientGuildNews::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE624.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE624.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildNews*)>(reinterpret_cast<uintptr_t>(ClientGuildNews::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 0ee8b712 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientGuildNewsDeleted::ClientGuildNewsDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_newsID = intValue;
}

// PROVEN: dbb36425 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildNewsDeleted::~ClientGuildNewsDeleted() {
}

// PROVEN: e15e1f4c IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientGuildNewsDeleted::Get(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_newsID = intValue;

    return msg->IsRead() != 0;
}

// PROVEN: d48fb88a IDENTICAL/UNPROVEN
// OFFSET: 0x006B47E0/0x140366800
bool ClientGuildNewsDeleted::Handle() {
    if (ClientGuildNewsDeleted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD8F4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD8F4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildNewsDeleted*)>(reinterpret_cast<uintptr_t>(ClientGuildNewsDeleted::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: a4a6a874 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientGuildReputationWeeklyCap::ClientGuildReputationWeeklyCap(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_repToCap = intValue;
}

// PROVEN: 82cda865 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildReputationWeeklyCap::~ClientGuildReputationWeeklyCap() {
}

// PROVEN: 94e18a67 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientGuildReputationWeeklyCap::Get(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_repToCap = intValue;

    return msg->IsRead() != 0;
}

// PROVEN: 55de2b12 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3B10/0x140365750
bool ClientGuildReputationWeeklyCap::Handle() {
    if (ClientGuildReputationWeeklyCap::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFB94.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFB94.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildReputationWeeklyCap*)>(reinterpret_cast<uintptr_t>(ClientGuildReputationWeeklyCap::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: d96b4957 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3230/0x14035BB30
ClientGuildXPEarned::ClientGuildXPEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_xp = longValue;
}

// PROVEN: 94b59d00 IDENTICAL/UNPROVEN
// OFFSET: 0x006A2650/0x14034ED30
ClientGuildXPEarned::~ClientGuildXPEarned() {
}

// PROVEN: 0162dfbb IDENTICAL/IDENTICAL
// OFFSET: 0x006C3F80/0x14037E820
bool ClientGuildXPEarned::Get(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_xp = longValue;

    return msg->IsRead() != 0;
}

// PROVEN: d22f2d9f IDENTICAL/UNPROVEN
// OFFSET: 0x006BC8A0/0x140370D80
bool ClientGuildXPEarned::Handle() {
    if (ClientGuildXPEarned::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE34C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE34C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildXPEarned*)>(reinterpret_cast<uintptr_t>(ClientGuildXPEarned::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2aab4209 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3230/0x14035BB30
ClientGuildBankMoneyWithdrawn::ClientGuildBankMoneyWithdrawn(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_remainingWithdrawMoney = longValue;
}

// PROVEN: 34ef704f IDENTICAL/UNPROVEN
// OFFSET: 0x006A2650/0x14034ED30
ClientGuildBankMoneyWithdrawn::~ClientGuildBankMoneyWithdrawn() {
}

// PROVEN: 6f930ebf IDENTICAL/IDENTICAL
// OFFSET: 0x006C3F80/0x14037E820
bool ClientGuildBankMoneyWithdrawn::Get(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_remainingWithdrawMoney = longValue;

    return msg->IsRead() != 0;
}

// PROVEN: eccf99d8 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6060/0x1403686D0
bool ClientGuildBankMoneyWithdrawn::Handle() {
    if (ClientGuildBankMoneyWithdrawn::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD284.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD284.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildBankMoneyWithdrawn*)>(reinterpret_cast<uintptr_t>(ClientGuildBankMoneyWithdrawn::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 9a5c9dd4 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4910/0x14037A850
ClientGuildChangeNameResult::ClientGuildChangeNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;
}

// PROVEN: dc25e0fa IDENTICAL/UNPROVEN
// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildChangeNameResult::~ClientGuildChangeNameResult() {
}

// PROVEN: 2bea025f IDENTICAL/IDENTICAL
// OFFSET: 0x006C9980/0x1403813C0
bool ClientGuildChangeNameResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;

    return msg->IsRead() != 0;
}

// PROVEN: e7fd82bb IDENTICAL/UNPROVEN
// OFFSET: 0x006B93B0/0x14036C950
bool ClientGuildChangeNameResult::Handle() {
    if (ClientGuildChangeNameResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD08C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD08C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildChangeNameResult*)>(reinterpret_cast<uintptr_t>(ClientGuildChangeNameResult::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 7b2b2ae3 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4910/0x14037A850
ClientGuildFlaggedForRename::ClientGuildFlaggedForRename(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_flagSet = value >> 7;
}

// PROVEN: c153634a IDENTICAL/UNPROVEN
// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildFlaggedForRename::~ClientGuildFlaggedForRename() {
}

// PROVEN: 633b06ac IDENTICAL/IDENTICAL
// OFFSET: 0x006C9980/0x1403813C0
bool ClientGuildFlaggedForRename::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_flagSet = value >> 7;

    return msg->IsRead() != 0;
}

// PROVEN: 57639232 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3C50/0x140365900
bool ClientGuildFlaggedForRename::Handle() {
    if (ClientGuildFlaggedForRename::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFD08.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFD08.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildFlaggedForRename*)>(reinterpret_cast<uintptr_t>(ClientGuildFlaggedForRename::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: dee4d591 IDENTICAL/IDENTICAL
// OFFSET: 0x006F42F0/0x1403BB6C0
ClientGuildRanks::ClientGuildRanks(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 168ca6b2 IDENTICAL/IDENTICAL
// OFFSET: 0x006D92C0/0x14039E6C0
ClientGuildRanks::~ClientGuildRanks() {
}

// OFFSET: 0x006F2350/0x1403B8AA0
void ClientGuildRanks::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_ranks.SetCount(bits.ReadBits<18>());

    for (uint32_t i = 0; i < this->m_ranks.Count(); i++) {
        *reinterpret_cast<uint32_t*>(this->m_ranks[i].m_rankName) = bits.ReadBits<7>();
    }

    for (uint32_t i = 0; i < this->m_ranks.Count(); i++) {
        JamCliGuildRankData& rank = this->m_ranks[i];
        intValue = 0;
        msg->Get(intValue);
        rank.m_rankID = intValue;

        for (uint32_t j = 0; j < 8; j++) {
            intValue = 0;
            msg->Get(intValue);
            rank.m_tabWithdrawItemLimit[j] = intValue;

            intValue = 0;
            msg->Get(intValue);
            rank.m_tabFlags[j] = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        rank.m_withdrawGoldLimit = intValue;

        intValue = 0;
        msg->Get(intValue);
        rank.m_flags = intValue;

        length = *reinterpret_cast<uint32_t*>(rank.m_rankName);
        msg->GetData(rank.m_rankName, length);
        rank.m_rankName[length] = 0;

        intValue = 0;
        msg->Get(intValue);
        rank.m_rankOrder = intValue;
    }
}

// PROVEN: c90e097c IDENTICAL/UNPROVEN
// OFFSET: 0x006F42D0/0x1403BB690
bool ClientGuildRanks::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EF690/0x1403B4A40
bool ClientGuildRanks::Read(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint32_t length;

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<18>();

    if (msg->m_size - msg->m_read < ((7 * count + 7) >> 3) + 80 * count) {
        return false;
    }

    this->m_ranks.SetCount(count);

    for (uint32_t i = 0; i < this->m_ranks.Count(); i++) {
        *reinterpret_cast<uint32_t*>(this->m_ranks[i].m_rankName) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(this->m_ranks[i].m_rankName) >= sizeof(this->m_ranks[i].m_rankName)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->m_ranks.Count(); i++) {
        JamCliGuildRankData& rank = this->m_ranks[i];
        intValue = 0;
        msg->Get(intValue);
        rank.m_rankID = intValue;

        for (uint32_t j = 0; j < 8; j++) {
            intValue = 0;
            msg->Get(intValue);
            rank.m_tabWithdrawItemLimit[j] = intValue;

            intValue = 0;
            msg->Get(intValue);
            rank.m_tabFlags[j] = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        rank.m_withdrawGoldLimit = intValue;

        intValue = 0;
        msg->Get(intValue);
        rank.m_flags = intValue;

        length = *reinterpret_cast<uint32_t*>(rank.m_rankName);
        msg->GetData(rank.m_rankName, length);
        rank.m_rankName[length] = 0;

        intValue = 0;
        msg->Get(intValue);
        rank.m_rankOrder = intValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 3adc77e7 IDENTICAL/NONE
// OFFSET: 0x006AF1F0
bool ClientGuildRanks::IsValid() const {
    for (uint32_t i = 0; i < this->m_ranks.m_count; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->m_ranks.m_data[i].m_rankName))) {
                    break;
                }

                terminated |= this->m_ranks.m_data[i].m_rankName[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: a54f332f IDENTICAL/UNPROVEN
// OFFSET: 0x006B2870/0x140363F20
bool ClientGuildRanks::Handle() {
    if (ClientGuildRanks::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD2C0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD2C0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildRanks*)>(reinterpret_cast<uintptr_t>(ClientGuildRanks::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: bf937a43 IDENTICAL/IDENTICAL
// OFFSET: 0x006CF0A0/0x140389250
ClientGuildMemberUpdateNote::ClientGuildMemberUpdateNote(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0fbce0bc IDENTICAL/IDENTICAL
// OFFSET: 0x006A23F0/0x14034F900
ClientGuildMemberUpdateNote::~ClientGuildMemberUpdateNote() {
}

// PROVEN: ca4b1d32 IDENTICAL/DIFFERENT
// OFFSET: 0x006C0E70/0x140376360
void ClientGuildMemberUpdateNote::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_memberGUID[7] = bits.ReadBit();
    this->m_memberGUID[2] = bits.ReadBit();
    this->m_memberGUID[3] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_note) = bits.ReadBits<8>();
    this->m_memberGUID[5] = bits.ReadBit();
    this->m_memberGUID[0] = bits.ReadBit();
    this->m_memberGUID[6] = bits.ReadBit();
    this->m_memberGUID[4] = bits.ReadBit();
    this->m_isPublic = bits.ReadBit() != 0;
    this->m_memberGUID[1] = bits.ReadBit();

    if (this->m_memberGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[3] ^= byteValue;
    }

    if (this->m_memberGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[0] ^= byteValue;
    }

    if (this->m_memberGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[2] ^= byteValue;
    }

    if (this->m_memberGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[5] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_note);
    msg->GetData(this->m_note, length);
    this->m_note[length] = 0;

    if (this->m_memberGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[6] ^= byteValue;
    }

    if (this->m_memberGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[7] ^= byteValue;
    }

    if (this->m_memberGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[1] ^= byteValue;
    }

    if (this->m_memberGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[4] ^= byteValue;
    }
}

// PROVEN: 09de6f0f IDENTICAL/UNPROVEN
// OFFSET: 0x006B0430/0x1403611A0
bool ClientGuildMemberUpdateNote::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4d988039 IDENTICAL/DIFFERENT
// OFFSET: 0x006AC300/0x14035C1A0
bool ClientGuildMemberUpdateNote::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_memberGUID[7] = bits.ReadBit();
    this->m_memberGUID[2] = bits.ReadBit();
    this->m_memberGUID[3] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->m_note) = length;

        if (length >= sizeof(this->m_note)) {
            return false;
        }
    }
    this->m_memberGUID[5] = bits.ReadBit();
    this->m_memberGUID[0] = bits.ReadBit();
    this->m_memberGUID[6] = bits.ReadBit();
    this->m_memberGUID[4] = bits.ReadBit();
    this->m_isPublic = bits.ReadBit() != 0;
    this->m_memberGUID[1] = bits.ReadBit();

    if (this->m_memberGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[3] ^= byteValue;
    }

    if (this->m_memberGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[0] ^= byteValue;
    }

    if (this->m_memberGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[2] ^= byteValue;
    }

    if (this->m_memberGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[5] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_note);
    msg->GetData(this->m_note, length);
    this->m_note[length] = 0;

    if (this->m_memberGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[6] ^= byteValue;
    }

    if (this->m_memberGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[7] ^= byteValue;
    }

    if (this->m_memberGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[1] ^= byteValue;
    }

    if (this->m_memberGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[4] ^= byteValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: f8e23bd9 IDENTICAL/NONE
// OFFSET: 0x006AA110
bool ClientGuildMemberUpdateNote::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_note))) {
            break;
        }

        terminated |= this->m_note[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 1048446b IDENTICAL/UNPROVEN
// OFFSET: 0x006A5610/0x1403533F0
bool ClientGuildMemberUpdateNote::Handle() {
    if (ClientGuildMemberUpdateNote::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFA58.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFA58.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMemberUpdateNote*)>(reinterpret_cast<uintptr_t>(ClientGuildMemberUpdateNote::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 6193c6ef IDENTICAL/IDENTICAL
// OFFSET: 0x006AC0A0/0x14035BEA0
ClientGuildInvite::ClientGuildInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 99efbe43 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1F50/0x14034F3F0
ClientGuildInvite::~ClientGuildInvite() {
}

// PROVEN: 2b825b39 IDENTICAL/DIFFERENT
// OFFSET: 0x006A3660/0x140350EE0
void ClientGuildInvite::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint32_t length;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_guildLevel = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_achievementPoints = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_borderColor = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_emblemStyle = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_background = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_emblemColor = intValue;
    }

    JamBitReader bits(msg);
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_oldGuildName) = bits.ReadBits<8>();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_oldGuildGUID[6] = bits.ReadBit();
    this->m_oldGuildGUID[4] = bits.ReadBit();
    this->m_oldGuildGUID[1] = bits.ReadBit();
    this->m_oldGuildGUID[5] = bits.ReadBit();
    this->m_oldGuildGUID[7] = bits.ReadBit();
    this->m_oldGuildGUID[2] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_guildName) = bits.ReadBits<8>();
    this->m_oldGuildGUID[3] = bits.ReadBit();
    this->m_oldGuildGUID[0] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_inviterName) = bits.ReadBits<7>();
    this->m_guildGUID[4] = bits.ReadBit();

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_oldGuildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[2] ^= byteValue;
    }

    if (this->m_oldGuildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_oldGuildName);
    msg->GetData(this->m_oldGuildName, length);
    this->m_oldGuildName[length] = 0;

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    if (this->m_oldGuildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[7] ^= byteValue;
    }

    if (this->m_oldGuildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[5] ^= byteValue;
    }

    if (this->m_oldGuildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_oldGuildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[4] ^= byteValue;
    }
}

// PROVEN: a209214d IDENTICAL/UNPROVEN
// OFFSET: 0x006CF290/0x140389290
bool ClientGuildInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 7192f8f7 IDENTICAL/DIFFERENT
// OFFSET: 0x006C79B0/0x14037E890
bool ClientGuildInvite::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint32_t length;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_guildLevel = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_achievementPoints = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_borderColor = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_emblemStyle = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_background = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_emblemColor = intValue;
    }

    JamBitReader bits(msg);
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->m_oldGuildName) = length;

        if (length >= sizeof(this->m_oldGuildName)) {
            return false;
        }
    }
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_oldGuildGUID[6] = bits.ReadBit();
    this->m_oldGuildGUID[4] = bits.ReadBit();
    this->m_oldGuildGUID[1] = bits.ReadBit();
    this->m_oldGuildGUID[5] = bits.ReadBit();
    this->m_oldGuildGUID[7] = bits.ReadBit();
    this->m_oldGuildGUID[2] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->m_guildName) = length;

        if (length >= sizeof(this->m_guildName)) {
            return false;
        }
    }
    this->m_oldGuildGUID[3] = bits.ReadBit();
    this->m_oldGuildGUID[0] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<7>();
        *reinterpret_cast<uint32_t*>(this->m_inviterName) = length;

        if (length >= sizeof(this->m_inviterName)) {
            return false;
        }
    }
    this->m_guildGUID[4] = bits.ReadBit();

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_oldGuildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[2] ^= byteValue;
    }

    if (this->m_oldGuildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_oldGuildName);
    msg->GetData(this->m_oldGuildName, length);
    this->m_oldGuildName[length] = 0;

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    if (this->m_oldGuildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[7] ^= byteValue;
    }

    if (this->m_oldGuildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[5] ^= byteValue;
    }

    if (this->m_oldGuildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_oldGuildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[4] ^= byteValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 5cc77e0d IDENTICAL/IDENTICAL
// OFFSET: 0x006A2CE0/0x140350230
bool ClientGuildInvite::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_oldGuildName))) {
                break;
            }

            terminated |= this->m_oldGuildName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_inviterName))) {
                break;
            }

            terminated |= this->m_inviterName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_guildName))) {
                break;
            }

            terminated |= this->m_guildName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: 1e447953 IDENTICAL/UNPROVEN
// OFFSET: 0x006BEEB0/0x140373E20
bool ClientGuildInvite::Handle() {
    if (ClientGuildInvite::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD5C4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD5C4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildInvite*)>(reinterpret_cast<uintptr_t>(ClientGuildInvite::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 7ad1364a IDENTICAL/IDENTICAL
// OFFSET: 0x006CF060/0x140389210
ClientGuildPartyState::ClientGuildPartyState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 23e03bdf IDENTICAL/IDENTICAL
// OFFSET: 0x006A0E00/0x14034DEA0
ClientGuildPartyState::~ClientGuildPartyState() {
}

// PROVEN: 1467822b IDENTICAL/IDENTICAL
// OFFSET: 0x006BC1C0/0x1403704D0
void ClientGuildPartyState::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;
    float floatValue;

    JamBitReader bits(msg);
    this->m_inGuildParty = bits.ReadBit() != 0;

    {
        float floatValue = 0.0f;
        msg->Get(floatValue);
        this->m_guildXPEarnedMult = floatValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numGuildGroupMembers = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numGuildGroupRequired = intValue;
    }
}

// PROVEN: 9de99d5d IDENTICAL/UNPROVEN
// OFFSET: 0x006CF340/0x1403892E0
bool ClientGuildPartyState::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 037a0776 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4CC0/0x14037A8E0
bool ClientGuildPartyState::Read(CDataStore* msg, bool full) {
    uint32_t intValue;
    float floatValue;

    JamBitReader bits(msg);
    this->m_inGuildParty = bits.ReadBit() != 0;

    {
        float floatValue = 0.0f;
        msg->Get(floatValue);
        this->m_guildXPEarnedMult = floatValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numGuildGroupMembers = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numGuildGroupRequired = intValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 82a1fe6a IDENTICAL/UNPROVEN
// OFFSET: 0x006C07C0/0x140375B00
bool ClientGuildPartyState::Handle() {
    if (ClientGuildPartyState::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE300.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE300.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildPartyState*)>(reinterpret_cast<uintptr_t>(ClientGuildPartyState::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 02c6bdcb IDENTICAL/IDENTICAL
// OFFSET: 0x006BE830/0x1403735E0
ClientGuildReputationReactionChanged::ClientGuildReputationReactionChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b7939dbb IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DB0/0x14034DDD0
ClientGuildReputationReactionChanged::~ClientGuildReputationReactionChanged() {
}

// PROVEN: 8607c660 IDENTICAL/DIFFERENT
// OFFSET: 0x006A4740/0x140352230
void ClientGuildReputationReactionChanged::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guid[1] = bits.ReadBit();
    this->m_guid[6] = bits.ReadBit();
    this->m_guid[2] = bits.ReadBit();
    this->m_guid[4] = bits.ReadBit();
    this->m_guid[0] = bits.ReadBit();
    this->m_guid[3] = bits.ReadBit();
    this->m_guid[7] = bits.ReadBit();
    this->m_guid[5] = bits.ReadBit();

    if (this->m_guid[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[4] ^= byteValue;
    }

    if (this->m_guid[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[6] ^= byteValue;
    }

    if (this->m_guid[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[5] ^= byteValue;
    }

    if (this->m_guid[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[7] ^= byteValue;
    }

    if (this->m_guid[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[2] ^= byteValue;
    }

    if (this->m_guid[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[0] ^= byteValue;
    }

    if (this->m_guid[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[3] ^= byteValue;
    }

    if (this->m_guid[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[1] ^= byteValue;
    }
}

// PROVEN: b46b3392 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAD50/0x140382F40
bool ClientGuildReputationReactionChanged::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 76a6b06f IDENTICAL/DIFFERENT
// OFFSET: 0x006B3050/0x1403649F0
bool ClientGuildReputationReactionChanged::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guid[1] = bits.ReadBit();
    this->m_guid[6] = bits.ReadBit();
    this->m_guid[2] = bits.ReadBit();
    this->m_guid[4] = bits.ReadBit();
    this->m_guid[0] = bits.ReadBit();
    this->m_guid[3] = bits.ReadBit();
    this->m_guid[7] = bits.ReadBit();
    this->m_guid[5] = bits.ReadBit();

    if (this->m_guid[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[4] ^= byteValue;
    }

    if (this->m_guid[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[6] ^= byteValue;
    }

    if (this->m_guid[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[5] ^= byteValue;
    }

    if (this->m_guid[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[7] ^= byteValue;
    }

    if (this->m_guid[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[2] ^= byteValue;
    }

    if (this->m_guid[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[0] ^= byteValue;
    }

    if (this->m_guid[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[3] ^= byteValue;
    }

    if (this->m_guid[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guid[1] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 898ce774 IDENTICAL/UNPROVEN
// OFFSET: 0x006BC180/0x140370470
bool ClientGuildReputationReactionChanged::Handle() {
    if (ClientGuildReputationReactionChanged::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF074.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF074.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildReputationReactionChanged*)>(reinterpret_cast<uintptr_t>(ClientGuildReputationReactionChanged::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b394fcef IDENTICAL/IDENTICAL
// OFFSET: 0x006A2B20/0x140350060
ClientGuildChallengeUpdate::ClientGuildChallengeUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 712bf515 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1FD0/0x14034F440
ClientGuildChallengeUpdate::~ClientGuildChallengeUpdate() {
}

// PROVEN: ef75a511 IDENTICAL/IDENTICAL
// OFFSET: 0x006A17D0/0x14034EB50
void ClientGuildChallengeUpdate::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeGoldReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeXPReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeMaxCount[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeMaxLevelGoldReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeCurrentCount[i] = intValue;
    }
}

// PROVEN: 0e28b868 IDENTICAL/UNPROVEN
// OFFSET: 0x006C6100/0x14037C520
bool ClientGuildChallengeUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 08321c70 IDENTICAL/IDENTICAL
// OFFSET: 0x006B9530/0x14036CB20
bool ClientGuildChallengeUpdate::Read(CDataStore* msg, bool full) {
    uint32_t intValue;

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeGoldReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeXPReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeMaxCount[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeMaxLevelGoldReward[i] = intValue;
    }

    for (uint32_t i = 0; i < 4; i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_challengeCurrentCount[i] = intValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 128769ac IDENTICAL/UNPROVEN
// OFFSET: 0x006ADB30/0x14035DF40
bool ClientGuildChallengeUpdate::Handle() {
    if (ClientGuildChallengeUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDB74.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDB74.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildChallengeUpdate*)>(reinterpret_cast<uintptr_t>(ClientGuildChallengeUpdate::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 97cf1077 IDENTICAL/IDENTICAL
// OFFSET: 0x006A22E0/0x14034F810
ClientGuildChallengeCompleted::ClientGuildChallengeCompleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7891572f IDENTICAL/IDENTICAL
// OFFSET: 0x006A2680/0x14034FB60
ClientGuildChallengeCompleted::~ClientGuildChallengeCompleted() {
}

// PROVEN: a079a36e IDENTICAL/IDENTICAL
// OFFSET: 0x006A1500/0x14034E7D0
void ClientGuildChallengeCompleted::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_challengeType = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_goldAwarded = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currentCount = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_xpAwarded = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_maxCount = intValue;
    }
}

// PROVEN: b9861697 IDENTICAL/UNPROVEN
// OFFSET: 0x006BE870/0x140373620
bool ClientGuildChallengeCompleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a5725fd1 IDENTICAL/IDENTICAL
// OFFSET: 0x006B7870/0x14036A780
bool ClientGuildChallengeCompleted::Read(CDataStore* msg, bool full) {
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_challengeType = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_goldAwarded = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_currentCount = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_xpAwarded = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_maxCount = intValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: edd9d1d0 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8CB0/0x140357C90
bool ClientGuildChallengeCompleted::Handle() {
    if (ClientGuildChallengeCompleted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDB38.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDB38.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildChallengeCompleted*)>(reinterpret_cast<uintptr_t>(ClientGuildChallengeCompleted::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 6b976d15 IDENTICAL/IDENTICAL
// OFFSET: 0x006BF330/0x140374390
ClientGuildReset::ClientGuildReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 63a6b684 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1140/0x14034E3E0
ClientGuildReset::~ClientGuildReset() {
}

// PROVEN: 34e0ea27 IDENTICAL/DIFFERENT
// OFFSET: 0x006BCAF0/0x140371070
void ClientGuildReset::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }
}

// PROVEN: c9b3c04e IDENTICAL/UNPROVEN
// OFFSET: 0x006C3E00/0x140379DA0
bool ClientGuildReset::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ba8912d6 IDENTICAL/DIFFERENT
// OFFSET: 0x006A9C10/0x1403590E0
bool ClientGuildReset::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 020cc944 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3230/0x1403509C0
bool ClientGuildReset::Handle() {
    if (ClientGuildReset::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC10.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC10.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildReset*)>(reinterpret_cast<uintptr_t>(ClientGuildReset::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 47d8ec20 IDENTICAL/IDENTICAL
// OFFSET: 0x006D97F0/0x140397CB0
ClientGuildMoveComplete::ClientGuildMoveComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7ccdb10b IDENTICAL/IDENTICAL
// OFFSET: 0x006A21C0/0x14034F6C0
ClientGuildMoveComplete::~ClientGuildMoveComplete() {
}

// PROVEN: d74ba77f IDENTICAL/DIFFERENT
// OFFSET: 0x006D5A80/0x140392D90
void ClientGuildMoveComplete::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_guildName) = bits.ReadBits<9>();
    this->m_guildGUID[0] = bits.ReadBit();

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;
}

// PROVEN: 7570436e IDENTICAL/UNPROVEN
// OFFSET: 0x006C9AC0/0x140381530
bool ClientGuildMoveComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b5dd2632 IDENTICAL/DIFFERENT
// OFFSET: 0x006C8480/0x14037F530
bool ClientGuildMoveComplete::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<9>();
        *reinterpret_cast<uint32_t*>(this->m_guildName) = length;

        if (length >= sizeof(this->m_guildName)) {
            return false;
        }
    }
    this->m_guildGUID[0] = bits.ReadBit();

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 9b663e0f IDENTICAL/NONE
// OFFSET: 0x006A2E80
bool ClientGuildMoveComplete::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_guildName))) {
            break;
        }

        terminated |= this->m_guildName[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 5439d37f IDENTICAL/UNPROVEN
// OFFSET: 0x006ADB90/0x14035DFB0
bool ClientGuildMoveComplete::Handle() {
    if (ClientGuildMoveComplete::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD2EC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD2EC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMoveComplete*)>(reinterpret_cast<uintptr_t>(ClientGuildMoveComplete::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 9fbd3f5b IDENTICAL/IDENTICAL
// OFFSET: 0x006B6E10/0x140369A40
ClientGuildMoveStarting::ClientGuildMoveStarting(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 79df301c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2E20/0x140350380
ClientGuildMoveStarting::~ClientGuildMoveStarting() {
}

// PROVEN: 2a170f22 IDENTICAL/DIFFERENT
// OFFSET: 0x006B02E0/0x140360FF0
void ClientGuildMoveStarting::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }
}

// PROVEN: ce6bb7f8 IDENTICAL/UNPROVEN
// OFFSET: 0x006B74C0/0x14036A2B0
bool ClientGuildMoveStarting::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4bc4f4eb IDENTICAL/DIFFERENT
// OFFSET: 0x006A5970/0x140353870
bool ClientGuildMoveStarting::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;

    JamBitReader bits(msg);
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: aa2e09ab IDENTICAL/UNPROVEN
// OFFSET: 0x006B6D80/0x1403699A0
bool ClientGuildMoveStarting::Handle() {
    if (ClientGuildMoveStarting::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE0C0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE0C0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMoveStarting*)>(reinterpret_cast<uintptr_t>(ClientGuildMoveStarting::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: aa24acdd IDENTICAL/IDENTICAL
// OFFSET: 0x006D1EB0/0x14038D470
ClientGuildNameChanged::ClientGuildNameChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ddfab742 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0EC0/0x14034DF80
ClientGuildNameChanged::~ClientGuildNameChanged() {
}

// PROVEN: 19e2e882 IDENTICAL/DIFFERENT
// OFFSET: 0x006BF920/0x140374AF0
void ClientGuildNameChanged::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_guildGUID[5] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_guildName) = bits.ReadBits<8>();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }
}

// PROVEN: 129fceaa IDENTICAL/UNPROVEN
// OFFSET: 0x006C09A0/0x140375D70
bool ClientGuildNameChanged::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 031fb11f IDENTICAL/DIFFERENT
// OFFSET: 0x006AC570/0x14035C420
bool ClientGuildNameChanged::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_guildGUID[5] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->m_guildName) = length;

        if (length >= sizeof(this->m_guildName)) {
            return false;
        }
    }
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();

    if (this->m_guildGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    if (this->m_guildGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    if (this->m_guildGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: cd825948 IDENTICAL/NONE
// OFFSET: 0x006A1F60
bool ClientGuildNameChanged::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_guildName))) {
            break;
        }

        terminated |= this->m_guildName[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 29c9d7c9 IDENTICAL/UNPROVEN
// OFFSET: 0x006AC860/0x14035C710
bool ClientGuildNameChanged::Handle() {
    if (ClientGuildNameChanged::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD850.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD850.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildNameChanged*)>(reinterpret_cast<uintptr_t>(ClientGuildNameChanged::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 32069e7a IDENTICAL/IDENTICAL
// OFFSET: 0x006E1750/0x1403A1E40
ClientGuildPermissionsQueryResults::ClientGuildPermissionsQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ea740c0b IDENTICAL/IDENTICAL
// OFFSET: 0x006E5A30/0x1403A6FA0
ClientGuildPermissionsQueryResults::~ClientGuildPermissionsQueryResults() {
}

// OFFSET: 0x006DDB70/0x14039D1C0
void ClientGuildPermissionsQueryResults::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_rankID = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numTabs = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_flags = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_withdrawGoldLimit = intValue;
    }

    JamBitReader bits(msg);
    this->m_tabPermissions.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_tabPermissions.Count(); i++) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabPermissions[i].m_flags = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabPermissions[i].m_withdrawItemLimit = intValue;
        }
    }
}

// PROVEN: d9863322 IDENTICAL/UNPROVEN
// OFFSET: 0x006E8110/0x1403AA7F0
bool ClientGuildPermissionsQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 816939ca IDENTICAL/DIFFERENT
// OFFSET: 0x006E1E60/0x1403A2680
bool ClientGuildPermissionsQueryResults::Read(CDataStore* msg, bool full) {
    uint32_t intValue;

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_rankID = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_numTabs = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_flags = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_withdrawGoldLimit = intValue;
    }

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count) {
        return false;
    }

    this->m_tabPermissions.SetCount(count);

    for (uint32_t i = 0; i < this->m_tabPermissions.Count(); i++) {
        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabPermissions[i].m_flags = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabPermissions[i].m_withdrawItemLimit = intValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 15e5de15 IDENTICAL/UNPROVEN
// OFFSET: 0x006AC8B0/0x14035C770
bool ClientGuildPermissionsQueryResults::Handle() {
    if (ClientGuildPermissionsQueryResults::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF964.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF964.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildPermissionsQueryResults*)>(reinterpret_cast<uintptr_t>(ClientGuildPermissionsQueryResults::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 96815916 IDENTICAL/IDENTICAL
// OFFSET: 0x006E9330/0x14039CCB0
ClientGuildEventLogQueryResults::ClientGuildEventLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d47f9597 IDENTICAL/IDENTICAL
// OFFSET: 0x006E57C0/0x1403A6CB0
ClientGuildEventLogQueryResults::~ClientGuildEventLogQueryResults() {
}

// OFFSET: 0x006DCB00/0x14039BDA0
void ClientGuildEventLogQueryResults::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    JamBitReader bits(msg);
    this->m_entry.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamClientGuildEventEntry& entry = this->m_entry[i];
        entry.m_playerGUID[2] = bits.ReadBit();
        entry.m_playerGUID[4] = bits.ReadBit();
        entry.m_otherGUID[7] = bits.ReadBit();
        entry.m_otherGUID[6] = bits.ReadBit();
        entry.m_playerGUID[3] = bits.ReadBit();
        entry.m_otherGUID[3] = bits.ReadBit();
        entry.m_otherGUID[5] = bits.ReadBit();
        entry.m_playerGUID[7] = bits.ReadBit();
        entry.m_playerGUID[5] = bits.ReadBit();
        entry.m_playerGUID[0] = bits.ReadBit();
        entry.m_otherGUID[4] = bits.ReadBit();
        entry.m_otherGUID[2] = bits.ReadBit();
        entry.m_otherGUID[0] = bits.ReadBit();
        entry.m_otherGUID[1] = bits.ReadBit();
        entry.m_playerGUID[1] = bits.ReadBit();
        entry.m_playerGUID[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamClientGuildEventEntry& entry = this->m_entry[i];

        if (entry.m_otherGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[2] ^= byteValue;
        }

        if (entry.m_otherGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_rankID = byteValue;
        }

        if (entry.m_otherGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_timeOffset = intValue;
        }

        if (entry.m_playerGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[0] ^= byteValue;
        }

        if (entry.m_otherGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[6] ^= byteValue;
        }

        if (entry.m_otherGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_type = byteValue;
        }

        if (entry.m_otherGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }
    }
}

// PROVEN: cbc9d657 IDENTICAL/UNPROVEN
// OFFSET: 0x006E23E0/0x1403A2EA0
bool ClientGuildEventLogQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E17A0/0x1403A1EA0
bool ClientGuildEventLogQueryResults::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;

    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((16 * count + 7) >> 3) + 6 * count) {
        return false;
    }

    this->m_entry.SetCount(count);

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamClientGuildEventEntry& entry = this->m_entry[i];
        entry.m_playerGUID[2] = bits.ReadBit();
        entry.m_playerGUID[4] = bits.ReadBit();
        entry.m_otherGUID[7] = bits.ReadBit();
        entry.m_otherGUID[6] = bits.ReadBit();
        entry.m_playerGUID[3] = bits.ReadBit();
        entry.m_otherGUID[3] = bits.ReadBit();
        entry.m_otherGUID[5] = bits.ReadBit();
        entry.m_playerGUID[7] = bits.ReadBit();
        entry.m_playerGUID[5] = bits.ReadBit();
        entry.m_playerGUID[0] = bits.ReadBit();
        entry.m_otherGUID[4] = bits.ReadBit();
        entry.m_otherGUID[2] = bits.ReadBit();
        entry.m_otherGUID[0] = bits.ReadBit();
        entry.m_otherGUID[1] = bits.ReadBit();
        entry.m_playerGUID[1] = bits.ReadBit();
        entry.m_playerGUID[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamClientGuildEventEntry& entry = this->m_entry[i];

        if (entry.m_otherGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[2] ^= byteValue;
        }

        if (entry.m_otherGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_rankID = byteValue;
        }

        if (entry.m_otherGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_timeOffset = intValue;
        }

        if (entry.m_playerGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[0] ^= byteValue;
        }

        if (entry.m_otherGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[6] ^= byteValue;
        }

        if (entry.m_otherGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_type = byteValue;
        }

        if (entry.m_otherGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6e0cf8fc IDENTICAL/UNPROVEN
// OFFSET: 0x006BFD00/0x140374F20
bool ClientGuildEventLogQueryResults::Handle() {
    if (ClientGuildEventLogQueryResults::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFA58.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFA58.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildEventLogQueryResults*)>(reinterpret_cast<uintptr_t>(ClientGuildEventLogQueryResults::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 5255e1d6 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientGuildMemberDailyReset::ClientGuildMemberDailyReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 82de6f74 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientGuildMemberDailyReset::~ClientGuildMemberDailyReset() {
}

// PROVEN: ea518004 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientGuildMemberDailyReset::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 4e0aa562 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF450/0x1403744E0
bool ClientGuildMemberDailyReset::Handle() {
    if (ClientGuildMemberDailyReset::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE5FC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE5FC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMemberDailyReset*)>(reinterpret_cast<uintptr_t>(ClientGuildMemberDailyReset::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 80d95de0 IDENTICAL/IDENTICAL
// OFFSET: 0x006F73B0/0x1403C1170
ClientGuildBankQueryResults::ClientGuildBankQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 640fa878 IDENTICAL/IDENTICAL
// OFFSET: 0x006F6EE0/0x1403BEB70
ClientGuildBankQueryResults::~ClientGuildBankQueryResults() {
}

// OFFSET: 0x006F5ED0/0x1403BF7F0
void ClientGuildBankQueryResults::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint64_t longValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_fullUpdate = bits.ReadBit() != 0;
    this->m_itemInfo.SetCount(bits.ReadBits<20>());
    this->m_tabInfo.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_itemInfo.Count(); i++) {
        this->m_itemInfo[i].m_locked = bits.ReadBit() != 0;
        this->m_itemInfo[i].m_socketEnchant.SetCount(bits.ReadBits<23>());
    }

    for (uint32_t i = 0; i < this->m_tabInfo.Count(); i++) {
        *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_icon) = bits.ReadBits<9>();
        *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name) = bits.ReadBits<7>();
    }

    for (uint32_t i = 0; i < this->m_tabInfo.Count(); i++) {
        length = *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_icon);
        msg->GetData(this->m_tabInfo[i].m_icon, length);
        this->m_tabInfo[i].m_icon[length] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabInfo[i].m_tabIndex = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name);
        msg->GetData(this->m_tabInfo[i].m_name, length);
        this->m_tabInfo[i].m_name[length] = 0;
    }

    {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_money = longValue;
    }

    for (uint32_t i = 0; i < this->m_itemInfo.Count(); i++) {
        JamGuildBankItemInfo& item = this->m_itemInfo[i];

        for (uint32_t j = 0; j < item.m_socketEnchant.Count(); j++) {
            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_socketEnchant[j].m_socketIndex = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_socketEnchant[j].m_socketEnchantID = intValue;
            }
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_enchantmentID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_reforgeID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_onUseEnchantmentID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_count = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_slot = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_flags = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_randomPropertiesID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_charges = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_randomPropertiesSeed = intValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_withdrawalsRemaining = intValue;
    }
}

// PROVEN: f4f7cc18 IDENTICAL/UNPROVEN
// OFFSET: 0x006F7550/0x1403C11E0
bool ClientGuildBankQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6F10/0x1403C0880
bool ClientGuildBankQueryResults::Read(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint64_t longValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_fullUpdate = bits.ReadBit() != 0;

    uint32_t count3 = bits.ReadBits<20>();

    if (msg->m_size - msg->m_read < ((24 * count3 + 7) >> 3) + 40 * count3) {
        return false;
    }

    this->m_itemInfo.SetCount(count3);

    uint32_t count2 = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < ((16 * count2 + 7) >> 3) + 4 * count2) {
        return false;
    }

    this->m_tabInfo.SetCount(count2);

    for (uint32_t i = 0; i < this->m_itemInfo.Count(); i++) {
        this->m_itemInfo[i].m_locked = bits.ReadBit() != 0;
        uint32_t count = bits.ReadBits<23>();

        if (msg->m_size - msg->m_read < 8 * count) {
            return false;
        }

        this->m_itemInfo[i].m_socketEnchant.SetCount(count);
    }

    for (uint32_t i = 0; i < this->m_tabInfo.Count(); i++) {
        *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_icon) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_icon) >= sizeof(this->m_tabInfo[i].m_icon)) {
            return false;
        }

        *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name) >= sizeof(this->m_tabInfo[i].m_name)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->m_tabInfo.Count(); i++) {
        length = *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_icon);
        msg->GetData(this->m_tabInfo[i].m_icon, length);
        this->m_tabInfo[i].m_icon[length] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            this->m_tabInfo[i].m_tabIndex = intValue;
        }

        length = *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name);
        msg->GetData(this->m_tabInfo[i].m_name, length);
        this->m_tabInfo[i].m_name[length] = 0;
    }

    {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_money = longValue;
    }

    for (uint32_t i = 0; i < this->m_itemInfo.Count(); i++) {
        JamGuildBankItemInfo& item = this->m_itemInfo[i];

        for (uint32_t j = 0; j < item.m_socketEnchant.Count(); j++) {
            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_socketEnchant[j].m_socketIndex = intValue;
            }

            {
                uint32_t intValue = 0;
                msg->Get(intValue);
                item.m_socketEnchant[j].m_socketEnchantID = intValue;
            }
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_enchantmentID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_reforgeID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_onUseEnchantmentID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_count = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_slot = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_flags = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_randomPropertiesID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_charges = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            item.m_randomPropertiesSeed = intValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_withdrawalsRemaining = intValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 027cdee0 IDENTICAL/NONE
// OFFSET: 0x006C86A0
bool ClientGuildBankQueryResults::IsValid() const {
    for (uint32_t i = 0; i < this->m_tabInfo.m_count; i++) {
        if (!this->m_tabInfo.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: 2446388a IDENTICAL/UNPROVEN
// OFFSET: 0x006B1640/0x140362860
bool ClientGuildBankQueryResults::Handle() {
    if (ClientGuildBankQueryResults::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEF10.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEF10.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildBankQueryResults*)>(reinterpret_cast<uintptr_t>(ClientGuildBankQueryResults::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// OFFSET: 0x006EAF10/0x1403AE670
ClientGuildBankLogQueryResults::ClientGuildBankLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasWeeklyBonusMoney = false;
    this->Unpack(msg, true);
}

// PROVEN: 5d12f423 IDENTICAL/IDENTICAL
// OFFSET: 0x006D9740/0x140397BD0
ClientGuildBankLogQueryResults::~ClientGuildBankLogQueryResults() {
}

// OFFSET: 0x006EAA00/0x1403AE090
void ClientGuildBankLogQueryResults::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;

    JamBitReader bits(msg);
    this->m_hasWeeklyBonusMoney = bits.ReadBit() != 0;
    this->m_entry.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamGuildBankLogEntry& entry = this->m_entry[i];
        entry.m_hasMoney = bits.ReadBit() != 0;
        entry.m_playerGUID[4] = bits.ReadBit();
        entry.m_playerGUID[1] = bits.ReadBit();
        entry.m_hasItemID = bits.ReadBit() != 0;
        entry.m_hasCount = bits.ReadBit() != 0;
        entry.m_playerGUID[2] = bits.ReadBit();
        entry.m_playerGUID[5] = bits.ReadBit();
        entry.m_playerGUID[3] = bits.ReadBit();
        entry.m_playerGUID[6] = bits.ReadBit();
        entry.m_playerGUID[0] = bits.ReadBit();
        entry.m_hasOtherTab = bits.ReadBit() != 0;
        entry.m_playerGUID[7] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamGuildBankLogEntry& entry = this->m_entry[i];

        if (entry.m_playerGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        if (entry.m_hasCount) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_count = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_entryType = byteValue;
        }

        if (entry.m_playerGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_hasItemID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_timeOffset = intValue;
        }

        if (entry.m_hasMoney) {
            uint64_t longValue = 0;
            msg->Get(longValue);
            entry.m_money = longValue;
        }

        if (entry.m_hasOtherTab) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherTab = byteValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    if (this->m_hasWeeklyBonusMoney) {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_weeklyBonusMoney = longValue;
    }
}

// PROVEN: 3e36ca55 IDENTICAL/UNPROVEN
// OFFSET: 0x006F0060/0x1403B57D0
bool ClientGuildBankLogQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EDBA0/0x1403B2760
bool ClientGuildBankLogQueryResults::Read(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t intValue;
    uint64_t longValue;

    JamBitReader bits(msg);
    this->m_hasWeeklyBonusMoney = bits.ReadBit() != 0;
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((12 * count + 7) >> 3) + 5 * count) {
        return false;
    }

    this->m_entry.SetCount(count);

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamGuildBankLogEntry& entry = this->m_entry[i];
        entry.m_hasMoney = bits.ReadBit() != 0;
        entry.m_playerGUID[4] = bits.ReadBit();
        entry.m_playerGUID[1] = bits.ReadBit();
        entry.m_hasItemID = bits.ReadBit() != 0;
        entry.m_hasCount = bits.ReadBit() != 0;
        entry.m_playerGUID[2] = bits.ReadBit();
        entry.m_playerGUID[5] = bits.ReadBit();
        entry.m_playerGUID[3] = bits.ReadBit();
        entry.m_playerGUID[6] = bits.ReadBit();
        entry.m_playerGUID[0] = bits.ReadBit();
        entry.m_hasOtherTab = bits.ReadBit() != 0;
        entry.m_playerGUID[7] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_entry.Count(); i++) {
        JamGuildBankLogEntry& entry = this->m_entry[i];

        if (entry.m_playerGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        if (entry.m_hasCount) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_count = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_entryType = byteValue;
        }

        if (entry.m_playerGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_hasItemID) {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_itemID = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            entry.m_timeOffset = intValue;
        }

        if (entry.m_hasMoney) {
            uint64_t longValue = 0;
            msg->Get(longValue);
            entry.m_money = longValue;
        }

        if (entry.m_hasOtherTab) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherTab = byteValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    if (this->m_hasWeeklyBonusMoney) {
        uint64_t longValue = 0;
        msg->Get(longValue);
        this->m_weeklyBonusMoney = longValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 0e4ee45a IDENTICAL/UNPROVEN
// OFFSET: 0x006AA4E0/0x140359B60
bool ClientGuildBankLogQueryResults::Handle() {
    if (ClientGuildBankLogQueryResults::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF1A8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF1A8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildBankLogQueryResults*)>(reinterpret_cast<uintptr_t>(ClientGuildBankLogQueryResults::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 7a458aad IDENTICAL/IDENTICAL
// OFFSET: 0x006D7730/0x1403953E0
ClientGuildBankTextQueryResult::ClientGuildBankTextQueryResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0e01e926 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0D80/0x14034DDA0
ClientGuildBankTextQueryResult::~ClientGuildBankTextQueryResult() {
}

// PROVEN: 116b2f72 IDENTICAL/DIFFERENT
// OFFSET: 0x006C9AD0/0x140381540
void ClientGuildBankTextQueryResult::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint32_t length;

    JamBitReader bits(msg);
    *reinterpret_cast<uint32_t*>(this->m_text) = bits.ReadBits<14>();

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_text);
    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;
}

// PROVEN: 0f16e261 IDENTICAL/UNPROVEN
// OFFSET: 0x006D90C0/0x140397580
bool ClientGuildBankTextQueryResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f51bdb25 IDENTICAL/DIFFERENT
// OFFSET: 0x006D45D0/0x140390FD0
bool ClientGuildBankTextQueryResult::Read(CDataStore* msg, bool full) {
    uint32_t intValue;
    uint32_t length;

    JamBitReader bits(msg);
    {
        uint32_t length = bits.ReadBits<14>();
        *reinterpret_cast<uint32_t*>(this->m_text) = length;

        if (length >= sizeof(this->m_text)) {
            return false;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_tab = intValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_text);
    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 13de745c IDENTICAL/NONE
// OFFSET: 0x006A2290
bool ClientGuildBankTextQueryResult::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_text))) {
            break;
        }

        terminated |= this->m_text[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: 6fa2f538 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4220/0x140366080
bool ClientGuildBankTextQueryResult::Handle() {
    if (ClientGuildBankTextQueryResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4E8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4E8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildBankTextQueryResult*)>(reinterpret_cast<uintptr_t>(ClientGuildBankTextQueryResult::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: c3d69763 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8800/0x14037F8D0
ClientMoveSetActiveMover::ClientMoveSetActiveMover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b3934478 IDENTICAL/IDENTICAL
// OFFSET: 0x006A20D0/0x14034F550
ClientMoveSetActiveMover::~ClientMoveSetActiveMover() {
}

// PROVEN: bf98a78a IDENTICAL/DIFFERENT
// OFFSET: 0x006BF1A0/0x140374180
void ClientMoveSetActiveMover::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_moverGUID[5] = bits.ReadBit();
    this->m_moverGUID[7] = bits.ReadBit();
    this->m_moverGUID[3] = bits.ReadBit();
    this->m_moverGUID[6] = bits.ReadBit();
    this->m_moverGUID[0] = bits.ReadBit();
    this->m_moverGUID[4] = bits.ReadBit();
    this->m_moverGUID[1] = bits.ReadBit();
    this->m_moverGUID[2] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_moverGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[6] ^= byteValue;
    }

    if (this->m_moverGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[2] ^= byteValue;
    }

    if (this->m_moverGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[3] ^= byteValue;
    }

    if (this->m_moverGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[0] ^= byteValue;
    }

    if (this->m_moverGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[5] ^= byteValue;
    }

    if (this->m_moverGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[7] ^= byteValue;
    }

    if (this->m_moverGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[1] ^= byteValue;
    }

    if (this->m_moverGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[4] ^= byteValue;
    }
}

// PROVEN: 5d688f02 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAB60/0x140382CE0
bool ClientMoveSetActiveMover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 8033c5fe IDENTICAL/DIFFERENT
// OFFSET: 0x006B3FA0/0x140365D60
bool ClientMoveSetActiveMover::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_moverGUID[5] = bits.ReadBit();
    this->m_moverGUID[7] = bits.ReadBit();
    this->m_moverGUID[3] = bits.ReadBit();
    this->m_moverGUID[6] = bits.ReadBit();
    this->m_moverGUID[0] = bits.ReadBit();
    this->m_moverGUID[4] = bits.ReadBit();
    this->m_moverGUID[1] = bits.ReadBit();
    this->m_moverGUID[2] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_moverGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[6] ^= byteValue;
    }

    if (this->m_moverGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[2] ^= byteValue;
    }

    if (this->m_moverGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[3] ^= byteValue;
    }

    if (this->m_moverGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[0] ^= byteValue;
    }

    if (this->m_moverGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[5] ^= byteValue;
    }

    if (this->m_moverGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[7] ^= byteValue;
    }

    if (this->m_moverGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[1] ^= byteValue;
    }

    if (this->m_moverGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_moverGUID[4] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 1cbaff5c IDENTICAL/UNPROVEN
// OFFSET: 0x006B2A30/0x140364170
bool ClientMoveSetActiveMover::Handle() {
    if (ClientMoveSetActiveMover::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD138.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD138.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetActiveMover*)>(reinterpret_cast<uintptr_t>(ClientMoveSetActiveMover::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: b119eebf IDENTICAL/IDENTICAL
// OFFSET: 0x006DD590/0x14039CA20
ClientRequestCemeteryListResponse::ClientRequestCemeteryListResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 273c06a6 IDENTICAL/UNPROVEN
// OFFSET: 0x006CAD20/0x140382F00
ClientRequestCemeteryListResponse::~ClientRequestCemeteryListResponse() {
}

// OFFSET: 0x006DAD70/0x140399590
void ClientRequestCemeteryListResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_isGossipTriggered = bits.ReadBit() != 0;
    this->m_cemeteryIDs.SetCount(bits.ReadBits<24>());

    for (uint32_t i = 0; i < this->m_cemeteryIDs.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cemeteryIDs.m_data[i] = value;
    }
}

// PROVEN: 4378ee30 IDENTICAL/UNPROVEN
// OFFSET: 0x006DDB60/0x14039D1B0
bool ClientRequestCemeteryListResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D9B00/0x140398030
bool ClientRequestCemeteryListResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_isGossipTriggered = bits.ReadBit() != 0;
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->m_cemeteryIDs.SetCount(count);

    for (uint32_t i = 0; i < this->m_cemeteryIDs.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cemeteryIDs.m_data[i] = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: be310271 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0DA0/0x140376250
bool ClientRequestCemeteryListResponse::Handle() {
    if (ClientRequestCemeteryListResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCF04.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCF04.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRequestCemeteryListResponse*)>(reinterpret_cast<uintptr_t>(ClientRequestCemeteryListResponse::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006B4F80/0x140367130
ClientWorldServerInfo::ClientWorldServerInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasRestrictedMoney = false;
    this->m_hasRestrictedLevel = false;
    this->m_hasIneligibleForLootMask = false;
    this->Unpack(msg, true);
}

// PROVEN: 232ae468 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1F10/0x14034F390
ClientWorldServerInfo::~ClientWorldServerInfo() {
}

// PROVEN: 8c524b58 IDENTICAL/DIFFERENT
// OFFSET: 0x006A8DD0/0x140357DC0
void ClientWorldServerInfo::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_hasRestrictedLevel = bits.ReadBit() != 0;
    this->m_hasRestrictedMoney = bits.ReadBit() != 0;
    this->m_hasIneligibleForLootMask = bits.ReadBit() != 0;

    uint32_t value;

    if (this->m_hasIneligibleForLootMask) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ineligibleForLootMask = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_isTournamentRealm = byteValue;
    }

    if (this->m_hasRestrictedMoney) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_restrictedMoney = value;
    }

    if (this->m_hasRestrictedLevel) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_restrictedLevel = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_weeklyReset = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_difficultyID = value;
    }
}

// PROVEN: 5cc110f8 IDENTICAL/UNPROVEN
// OFFSET: 0x006C7950/0x14037E7D0
bool ClientWorldServerInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4325e505 IDENTICAL/DIFFERENT
// OFFSET: 0x006B4260/0x1403660E0
bool ClientWorldServerInfo::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_hasRestrictedLevel = bits.ReadBit() != 0;
    this->m_hasRestrictedMoney = bits.ReadBit() != 0;
    this->m_hasIneligibleForLootMask = bits.ReadBit() != 0;

    uint32_t value;

    if (this->m_hasIneligibleForLootMask) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ineligibleForLootMask = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_isTournamentRealm = byteValue;
    }

    if (this->m_hasRestrictedMoney) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_restrictedMoney = value;
    }

    if (this->m_hasRestrictedLevel) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_restrictedLevel = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_weeklyReset = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_difficultyID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: a16861a8 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF840/0x1403749E0
bool ClientWorldServerInfo::Handle() {
    if (ClientWorldServerInfo::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEF00.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEF00.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWorldServerInfo*)>(reinterpret_cast<uintptr_t>(ClientWorldServerInfo::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 389e9089 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2DA0/0x140350300
ClientUpdateActionButtons::ClientUpdateActionButtons(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c96b55db IDENTICAL/IDENTICAL
// OFFSET: 0x006A1990/0x14034ED50
ClientUpdateActionButtons::~ClientUpdateActionButtons() {
}

// PROVEN: 54bb9da6 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2710/0x14034FBF0
void ClientUpdateActionButtons::Unpack(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 144; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_actionButtons[i] = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_reason = byteValue;
    }
}

// PROVEN: b14848d8 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF190/0x140374170
bool ClientUpdateActionButtons::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3786ce6d IDENTICAL/IDENTICAL
// OFFSET: 0x006AE760/0x14035EE50
bool ClientUpdateActionButtons::Read(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 144; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_actionButtons[i] = value;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_reason = byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: c7fead8b IDENTICAL/UNPROVEN
// OFFSET: 0x006A9220/0x140358360
bool ClientUpdateActionButtons::Handle() {
    if (ClientUpdateActionButtons::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD23C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD23C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientUpdateActionButtons*)>(reinterpret_cast<uintptr_t>(ClientUpdateActionButtons::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: ab9866c9 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientTimeSyncReq::ClientTimeSyncReq(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;
}

// PROVEN: ba16ea2b IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientTimeSyncReq::~ClientTimeSyncReq() {
}

// PROVEN: d51f2e20 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientTimeSyncReq::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;

    return msg->IsRead() != 0;
}

// PROVEN: bfe296b7 IDENTICAL/UNPROVEN
// OFFSET: 0x006A5C80/0x140353C60
bool ClientTimeSyncReq::Handle() {
    if (ClientTimeSyncReq::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDCEC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDCEC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientTimeSyncReq*)>(reinterpret_cast<uintptr_t>(ClientTimeSyncReq::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006D7160/0x140394DC0
ClientFeatureSystemStatus::ClientFeatureSystemStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_sessionAlertEnabled = false;
    this->m_europaTicketSystemEnabled = false;
    this->Unpack(msg, true);
}

// PROVEN: 7f9240ec IDENTICAL/UNPROVEN
// OFFSET: 0x006D0470/0x14038AE70
ClientFeatureSystemStatus::~ClientFeatureSystemStatus() {
}

// PROVEN: 25d4cd80 IDENTICAL/IDENTICAL
// OFFSET: 0x006BA770/0x14036E250
void ClientFeatureSystemStatus::Unpack(CDataStore* msg, bool full) {
    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_complaintStatus = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_scrollOfResurrectionMaxRequestsPerDay = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_scrollOfResurrectionRequestsRemaining = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cfgRealmID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cfgRealmRecID = value;
    }

    JamBitReader bits(msg);
    this->m_itemRestorationButtonEnabled = bits.ReadBit() != 0;
    this->m_travelPassEnabled = bits.ReadBit() != 0;
    this->m_scrollOfResurrectionEnabled = bits.ReadBit() != 0;
    this->m_europaTicketSystemEnabled = bits.ReadBit() != 0;
    this->m_sessionAlertEnabled = bits.ReadBit() != 0;
    this->m_voiceEnabled = bits.ReadBit() != 0;

    if (this->m_europaTicketSystemEnabled) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_tryCount = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_lastResetTimeBeforeNow = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_maxTries = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_perMilliseconds = value;
        }
    }

    if (this->m_sessionAlertEnabled) {
        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertPeriod = alert;
        }

        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertDelay = alert;
        }

        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertDisplayTime = alert;
        }
    }
}

// PROVEN: 4d14ec54 IDENTICAL/UNPROVEN
// OFFSET: 0x006D9600/0x140397A40
bool ClientFeatureSystemStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5800/0x140392AC0
bool ClientFeatureSystemStatus::Read(CDataStore* msg, bool full) {
    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_complaintStatus = byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_scrollOfResurrectionMaxRequestsPerDay = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_scrollOfResurrectionRequestsRemaining = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cfgRealmID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_cfgRealmRecID = value;
    }

    JamBitReader bits(msg);
    this->m_itemRestorationButtonEnabled = bits.ReadBit() != 0;
    this->m_travelPassEnabled = bits.ReadBit() != 0;
    this->m_scrollOfResurrectionEnabled = bits.ReadBit() != 0;
    this->m_europaTicketSystemEnabled = bits.ReadBit() != 0;
    this->m_sessionAlertEnabled = bits.ReadBit() != 0;
    this->m_voiceEnabled = bits.ReadBit() != 0;

    if (this->m_europaTicketSystemEnabled) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_tryCount = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_lastResetTimeBeforeNow = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_maxTries = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_europaTicketThrottle.m_perMilliseconds = value;
        }
    }

    if (this->m_sessionAlertEnabled) {
        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertPeriod = alert;
        }

        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertDelay = alert;
        }

        {
            uint32_t alert = 0;
            msg->Get(alert);
            this->m_sessionAlertDisplayTime = alert;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 01384ea3 IDENTICAL/UNPROVEN
// OFFSET: 0x006A72E0/0x140355D30
bool ClientFeatureSystemStatus::Handle() {
    if (ClientFeatureSystemStatus::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFCA4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFCA4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientFeatureSystemStatus*)>(reinterpret_cast<uintptr_t>(ClientFeatureSystemStatus::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 0b9955d3 IDENTICAL/IDENTICAL
// OFFSET: 0x006E9850/0x1403AC8A0
ClientCategoryCooldown::ClientCategoryCooldown(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4db57341 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5330/0x1403A6690
ClientCategoryCooldown::~ClientCategoryCooldown() {
}

// OFFSET: 0x006DFD00/0x14039FBF0
void ClientCategoryCooldown::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_categoryCooldowns.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_categoryCooldowns.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_category = value;

        value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_modCooldown = value;
    }
}

// PROVEN: 537a6513 IDENTICAL/UNPROVEN
// OFFSET: 0x006ED920/0x1403B2350
bool ClientCategoryCooldown::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f6c14554 IDENTICAL/DIFFERENT
// OFFSET: 0x006E37D0/0x1403A4560
bool ClientCategoryCooldown::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count) {
        return false;
    }

    this->m_categoryCooldowns.SetCount(count);

    for (uint32_t i = 0; i < this->m_categoryCooldowns.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_category = value;

        value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_modCooldown = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: d4194458 IDENTICAL/UNPROVEN
// OFFSET: 0x006AAF60/0x14035A8D0
bool ClientCategoryCooldown::Handle() {
    if (ClientCategoryCooldown::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF248.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF248.m_left->Get();
        return reinterpret_cast<bool (*)(ClientCategoryCooldown*)>(reinterpret_cast<uintptr_t>(ClientCategoryCooldown::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: a5003a30 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientItemRemovePassive::ClientItemRemovePassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// PROVEN: 90dccb05 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientItemRemovePassive::~ClientItemRemovePassive() {
}

// PROVEN: ff861219 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientItemRemovePassive::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    return msg->IsRead() != 0;
}

// PROVEN: 2324ce8a IDENTICAL/UNPROVEN
// OFFSET: 0x006C1320/0x1403768B0
bool ClientItemRemovePassive::Handle() {
    if (ClientItemRemovePassive::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD500.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD500.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemRemovePassive*)>(reinterpret_cast<uintptr_t>(ClientItemRemovePassive::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 8fbd77e1 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0020/0x1403752C0
ClientDisenchantCredit::ClientDisenchantCredit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: cbc46dc7 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1F90/0x14034F410
ClientDisenchantCredit::~ClientDisenchantCredit() {
}

// PROVEN: c9f1da9c IDENTICAL/IDENTICAL
// OFFSET: 0x006BD850/0x140372190
void ClientDisenchantCredit::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_lootedGUID[0] = bits.ReadBit();
    this->m_lootedGUID[6] = bits.ReadBit();
    this->m_lootedGUID[3] = bits.ReadBit();
    this->m_lootedGUID[1] = bits.ReadBit();
    this->m_lootedGUID[7] = bits.ReadBit();
    this->m_lootedGUID[5] = bits.ReadBit();
    this->m_lootedGUID[2] = bits.ReadBit();
    this->m_lootedGUID[4] = bits.ReadBit();

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_itemID = value;
    }

    uint8_t byteValue;

    if (this->m_lootedGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[4] ^= byteValue;
    }

    if (this->m_lootedGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_randomPropertiesSeed = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_randomPropertiesID = value;
    }

    if (this->m_lootedGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[6] ^= byteValue;
    }

    if (this->m_lootedGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[5] ^= byteValue;
    }

    if (this->m_lootedGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[2] ^= byteValue;
    }

    if (this->m_lootedGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[7] ^= byteValue;
    }

    if (this->m_lootedGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[3] ^= byteValue;
    }

    if (this->m_lootedGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[0] ^= byteValue;
    }
}

// PROVEN: c14e347d IDENTICAL/UNPROVEN
// OFFSET: 0x006C4390/0x14037A1E0
bool ClientDisenchantCredit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5d3cbbf2 IDENTICAL/IDENTICAL
// OFFSET: 0x006BFD90/0x140374FE0
bool ClientDisenchantCredit::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);

    this->m_lootedGUID[0] = bits.ReadBit();
    this->m_lootedGUID[6] = bits.ReadBit();
    this->m_lootedGUID[3] = bits.ReadBit();
    this->m_lootedGUID[1] = bits.ReadBit();
    this->m_lootedGUID[7] = bits.ReadBit();
    this->m_lootedGUID[5] = bits.ReadBit();
    this->m_lootedGUID[2] = bits.ReadBit();
    this->m_lootedGUID[4] = bits.ReadBit();

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_itemID = value;
    }

    uint8_t byteValue;

    if (this->m_lootedGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[4] ^= byteValue;
    }

    if (this->m_lootedGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_randomPropertiesSeed = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_randomPropertiesID = value;
    }

    if (this->m_lootedGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[6] ^= byteValue;
    }

    if (this->m_lootedGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[5] ^= byteValue;
    }

    if (this->m_lootedGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[2] ^= byteValue;
    }

    if (this->m_lootedGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[7] ^= byteValue;
    }

    if (this->m_lootedGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[3] ^= byteValue;
    }

    if (this->m_lootedGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[0] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: d468a4e4 IDENTICAL/UNPROVEN
// OFFSET: 0x006BC580/0x140370980
bool ClientDisenchantCredit::Handle() {
    if (ClientDisenchantCredit::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE1F8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE1F8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDisenchantCredit*)>(reinterpret_cast<uintptr_t>(ClientDisenchantCredit::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: ce530b7d IDENTICAL/IDENTICAL
// OFFSET: 0x006D97B0/0x140397C60
ClientItemSendPassive::ClientItemSendPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ae21f6ce IDENTICAL/UNPROVEN
// OFFSET: 0x006C50F0/0x14037AE20
ClientItemSendPassive::~ClientItemSendPassive() {
}

// OFFSET: 0x006D7210/0x140394E80
void ClientItemSendPassive::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_spellIDs.SetCount(bits.ReadBits<24>());

    for (uint32_t i = 0; i < this->m_spellIDs.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellIDs.m_data[i] = value;
    }
}

// PROVEN: 2c2165ce IDENTICAL/UNPROVEN
// OFFSET: 0x006DD770/0x14039CCA0
bool ClientItemSendPassive::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5df78aec IDENTICAL/DIFFERENT
// OFFSET: 0x006D9660/0x140397AA0
bool ClientItemSendPassive::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->m_spellIDs.SetCount(count);

    for (uint32_t i = 0; i < this->m_spellIDs.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellIDs.m_data[i] = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 609f4f1e IDENTICAL/UNPROVEN
// OFFSET: 0x006BF4F0/0x1403745B0
bool ClientItemSendPassive::Handle() {
    if (ClientItemSendPassive::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF894.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF894.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemSendPassive*)>(reinterpret_cast<uintptr_t>(ClientItemSendPassive::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 57b49caa IDENTICAL/IDENTICAL
// OFFSET: 0x006A7590/0x140356040
ClientTimeAdjustment::ClientTimeAdjustment(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_sequenceIndex = value;
    }

    {
        float scale = 0.0f;
        msg->Get(scale);
        this->m_timeScale = scale;
    }
}

// PROVEN: 722ae6b9 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1090/0x14034E2D0
ClientTimeAdjustment::~ClientTimeAdjustment() {
}

// PROVEN: adfa9734 IDENTICAL/DIFFERENT
// OFFSET: 0x006C3190/0x140378BA0
bool ClientTimeAdjustment::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;

    {
        float value = 0.0f;
        msg->Get(value);
        this->m_timeScale = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 1508e642 IDENTICAL/UNPROVEN
// OFFSET: 0x006BAE20/0x14036EAC0
bool ClientTimeAdjustment::Handle() {
    if (ClientTimeAdjustment::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE008.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE008.m_left->Get();
        return reinterpret_cast<bool (*)(ClientTimeAdjustment*)>(reinterpret_cast<uintptr_t>(ClientTimeAdjustment::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: f4ecef72 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientItemAddPassive::ClientItemAddPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// PROVEN: 2d7f6554 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientItemAddPassive::~ClientItemAddPassive() {
}

// PROVEN: 572194c0 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientItemAddPassive::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    return msg->IsRead() != 0;
}

// PROVEN: 0be4a55c IDENTICAL/UNPROVEN
// OFFSET: 0x006B0510/0x1403612E0
bool ClientItemAddPassive::Handle() {
    if (ClientItemAddPassive::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE358.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE358.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemAddPassive*)>(reinterpret_cast<uintptr_t>(ClientItemAddPassive::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: fbe0bf61 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlegroundInfoThrottled::ClientBattlegroundInfoThrottled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 0913f563 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientBattlegroundInfoThrottled::~ClientBattlegroundInfoThrottled() {
}

// PROVEN: 0a03847a IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientBattlegroundInfoThrottled::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 2cc543e4 IDENTICAL/UNPROVEN
// OFFSET: 0x006B04D0/0x140361280
bool ClientBattlegroundInfoThrottled::Handle() {
    if (ClientBattlegroundInfoThrottled::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6FC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6FC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlegroundInfoThrottled*)>(reinterpret_cast<uintptr_t>(ClientBattlegroundInfoThrottled::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: b7233262 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientChatIgnoredAccountMuted::ClientChatIgnoredAccountMuted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 060cc61c IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientChatIgnoredAccountMuted::~ClientChatIgnoredAccountMuted() {
}

// PROVEN: de05cd91 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientChatIgnoredAccountMuted::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: aa51126c IDENTICAL/UNPROVEN
// OFFSET: 0x006B2230/0x1403637D0
bool ClientChatIgnoredAccountMuted::Handle() {
    if (ClientChatIgnoredAccountMuted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE448.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE448.m_left->Get();
        return reinterpret_cast<bool (*)(ClientChatIgnoredAccountMuted*)>(reinterpret_cast<uintptr_t>(ClientChatIgnoredAccountMuted::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 25a5a8bd IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientSorStartExperienceIncomplete::ClientSorStartExperienceIncomplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: a22e77e5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientSorStartExperienceIncomplete::~ClientSorStartExperienceIncomplete() {
}

// PROVEN: 6132d17a IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientSorStartExperienceIncomplete::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 0d001a5f IDENTICAL/UNPROVEN
// OFFSET: 0x006B5D30/0x1403682B0
bool ClientSorStartExperienceIncomplete::Handle() {
    if (ClientSorStartExperienceIncomplete::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD0CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD0CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSorStartExperienceIncomplete*)>(reinterpret_cast<uintptr_t>(ClientSorStartExperienceIncomplete::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: cff851a2 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8760/0x14037F830
ClientGuildCommandResult::ClientGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e18e95fb IDENTICAL/IDENTICAL
// OFFSET: 0x006A1DC0/0x14034F240
ClientGuildCommandResult::~ClientGuildCommandResult() {
}

// PROVEN: 3ef32c8f IDENTICAL/DIFFERENT
// OFFSET: 0x006BE9A0/0x140373780
void ClientGuildCommandResult::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_command = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_result = value;
    }

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;
}

// PROVEN: e7f160e4 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5BA0/0x1403680D0
bool ClientGuildCommandResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a3d88e5b IDENTICAL/DIFFERENT
// OFFSET: 0x006B3B60/0x1403657B0
bool ClientGuildCommandResult::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_command = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_result = value;
    }

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;

    if (length >= sizeof(this->m_name)) {
        return false;
    }

    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;

    return msg->IsRead() && this->IsValid();
}

// PROVEN: b7baa9e0 IDENTICAL/NONE
// OFFSET: 0x006A21D0
bool ClientGuildCommandResult::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
            break;
        }

        terminated |= this->m_name[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: dce33801 IDENTICAL/UNPROVEN
// OFFSET: 0x006BD100/0x140371820
bool ClientGuildCommandResult::Handle() {
    if (ClientGuildCommandResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD694.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD694.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildCommandResult*)>(reinterpret_cast<uintptr_t>(ClientGuildCommandResult::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: eeba18b0 IDENTICAL/IDENTICAL
// OFFSET: 0x006C60C0/0x14037C4E0
ClientArenaTeamCommandResult::ClientArenaTeamCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fa8d5edb IDENTICAL/IDENTICAL
// OFFSET: 0x006A2A90/0x14034FF70
ClientArenaTeamCommandResult::~ClientArenaTeamCommandResult() {
}

// PROVEN: 82eb35b1 IDENTICAL/DIFFERENT
// OFFSET: 0x006AD390/0x14035D4E0
void ClientArenaTeamCommandResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    *reinterpret_cast<uint32_t*>(this->m_name) = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_teamName) = bits.ReadBits<8>();

    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_name);
        msg->GetData(this->m_name, length);
        this->m_name[length] = 0;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_action = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_result = value;
    }

    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_teamName);
        msg->GetData(this->m_teamName, length);
        this->m_teamName[length] = 0;
    }
}

// PROVEN: ee632601 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF0D0/0x1403740F0
bool ClientArenaTeamCommandResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b537a54e IDENTICAL/DIFFERENT
// OFFSET: 0x006AEDD0/0x14035F680
bool ClientArenaTeamCommandResult::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t nameLength = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = nameLength;

    if (nameLength >= sizeof(this->m_name)) {
        return false;
    }

    uint32_t teamLength = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_teamName) = teamLength;

    if (teamLength >= sizeof(this->m_teamName)) {
        return false;
    }

    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_name);
        msg->GetData(this->m_name, length);
        this->m_name[length] = 0;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_action = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_result = value;
    }

    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->m_teamName);
        msg->GetData(this->m_teamName, length);
        this->m_teamName[length] = 0;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: d362efa4 IDENTICAL/DIFFERENT
// OFFSET: 0x006A25F0/0x140353FB0
bool ClientArenaTeamCommandResult::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_teamName))) {
                break;
            }

            terminated |= this->m_teamName[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: ba35b6ac IDENTICAL/UNPROVEN
// OFFSET: 0x006B3E10/0x140365B50
bool ClientArenaTeamCommandResult::Handle() {
    if (ClientArenaTeamCommandResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEAD0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEAD0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientArenaTeamCommandResult*)>(reinterpret_cast<uintptr_t>(ClientArenaTeamCommandResult::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 87f18e2a IDENTICAL/IDENTICAL
// OFFSET: 0x006DC660/0x14039B870
ClientGuildMembersWithRecipe::ClientGuildMembersWithRecipe(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f90b9f5e IDENTICAL/IDENTICAL
// OFFSET: 0x006D7310/0x140394F90
ClientGuildMembersWithRecipe::~ClientGuildMembersWithRecipe() {
}

// OFFSET: 0x006DA9B0/0x140399100
void ClientGuildMembersWithRecipe::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_members.SetCount(bits.ReadBits<26>());

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_members.m_data[i]);
        guid[2] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[5] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_members.m_data[i]);
        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillLineID = value;
    }
}

// PROVEN: f14f867f IDENTICAL/UNPROVEN
// OFFSET: 0x006D9270/0x140397680
bool ClientGuildMembersWithRecipe::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D8D60/0x140397160
bool ClientGuildMembersWithRecipe::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<26>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3)) {
        return false;
    }

    this->m_members.SetCount(count);

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_members.m_data[i]);
        guid[2] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[5] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_members.m_data[i]);
        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillLineID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 980eaefd IDENTICAL/UNPROVEN
// OFFSET: 0x006C0CC0/0x140376130
bool ClientGuildMembersWithRecipe::Handle() {
    if (ClientGuildMembersWithRecipe::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE1F8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE1F8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMembersWithRecipe*)>(reinterpret_cast<uintptr_t>(ClientGuildMembersWithRecipe::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: e5ab7ea2 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0A50/0x140375E50
ClientRoleChangedInform::ClientRoleChangedInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e5668696 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2EE0/0x1403503F0
ClientRoleChangedInform::~ClientRoleChangedInform() {
}

// PROVEN: 38cd75c5 IDENTICAL/IDENTICAL
// OFFSET: 0x006AAFA0/0x14035A930
void ClientRoleChangedInform::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_changedByGUID[1] = bits.ReadBit();
    this->m_targetGUID[0] = bits.ReadBit();
    this->m_targetGUID[2] = bits.ReadBit();
    this->m_targetGUID[4] = bits.ReadBit();
    this->m_targetGUID[7] = bits.ReadBit();
    this->m_targetGUID[3] = bits.ReadBit();
    this->m_changedByGUID[7] = bits.ReadBit();
    this->m_targetGUID[5] = bits.ReadBit();
    this->m_changedByGUID[5] = bits.ReadBit();
    this->m_changedByGUID[4] = bits.ReadBit();
    this->m_changedByGUID[3] = bits.ReadBit();
    this->m_targetGUID[6] = bits.ReadBit();
    this->m_changedByGUID[2] = bits.ReadBit();
    this->m_changedByGUID[6] = bits.ReadBit();
    this->m_targetGUID[1] = bits.ReadBit();
    this->m_changedByGUID[0] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_changedByGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[7] ^= byteValue;
    }

    if (this->m_targetGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[0] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_newRole = value;
    }

    if (this->m_targetGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[2] ^= byteValue;
    }

    if (this->m_changedByGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[0] ^= byteValue;
    }

    if (this->m_changedByGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[1] ^= byteValue;
    }

    if (this->m_changedByGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[5] ^= byteValue;
    }

    if (this->m_changedByGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[2] ^= byteValue;
    }

    if (this->m_targetGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[5] ^= byteValue;
    }

    if (this->m_targetGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[7] ^= byteValue;
    }

    if (this->m_changedByGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_oldRole = value;
    }
}

// PROVEN: 044376fa IDENTICAL/UNPROVEN
// OFFSET: 0x006A6DF0/0x140355730
bool ClientRoleChangedInform::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e59491ec IDENTICAL/IDENTICAL
// OFFSET: 0x006A68D0/0x140354CD0
bool ClientRoleChangedInform::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_changedByGUID[1] = bits.ReadBit();
    this->m_targetGUID[0] = bits.ReadBit();
    this->m_targetGUID[2] = bits.ReadBit();
    this->m_targetGUID[4] = bits.ReadBit();
    this->m_targetGUID[7] = bits.ReadBit();
    this->m_targetGUID[3] = bits.ReadBit();
    this->m_changedByGUID[7] = bits.ReadBit();
    this->m_targetGUID[5] = bits.ReadBit();
    this->m_changedByGUID[5] = bits.ReadBit();
    this->m_changedByGUID[4] = bits.ReadBit();
    this->m_changedByGUID[3] = bits.ReadBit();
    this->m_targetGUID[6] = bits.ReadBit();
    this->m_changedByGUID[2] = bits.ReadBit();
    this->m_changedByGUID[6] = bits.ReadBit();
    this->m_targetGUID[1] = bits.ReadBit();
    this->m_changedByGUID[0] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_changedByGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[7] ^= byteValue;
    }

    if (this->m_targetGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[0] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_newRole = value;
    }

    if (this->m_targetGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[2] ^= byteValue;
    }

    if (this->m_changedByGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[0] ^= byteValue;
    }

    if (this->m_changedByGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[1] ^= byteValue;
    }

    if (this->m_changedByGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[5] ^= byteValue;
    }

    if (this->m_changedByGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[2] ^= byteValue;
    }

    if (this->m_targetGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[5] ^= byteValue;
    }

    if (this->m_targetGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[7] ^= byteValue;
    }

    if (this->m_changedByGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_oldRole = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: c991d60a IDENTICAL/UNPROVEN
// OFFSET: 0x006AB430/0x14035AF30
bool ClientRoleChangedInform::Handle() {
    if (ClientRoleChangedInform::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRoleChangedInform*)>(reinterpret_cast<uintptr_t>(ClientRoleChangedInform::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 7a3d7765 IDENTICAL/IDENTICAL
// OFFSET: 0x006C45E0/0x14037A490
ClientRolePollBegin::ClientRolePollBegin(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 780ef2bb IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DC0/0x14034DDE0
ClientRolePollBegin::~ClientRolePollBegin() {
}

// PROVEN: f3323172 IDENTICAL/IDENTICAL
// OFFSET: 0x006B8DB0/0x14036C1A0
void ClientRolePollBegin::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_initiatorGUID[1] = bits.ReadBit();
    this->m_initiatorGUID[5] = bits.ReadBit();
    this->m_initiatorGUID[7] = bits.ReadBit();
    this->m_initiatorGUID[3] = bits.ReadBit();
    this->m_initiatorGUID[2] = bits.ReadBit();
    this->m_initiatorGUID[4] = bits.ReadBit();
    this->m_initiatorGUID[0] = bits.ReadBit();
    this->m_initiatorGUID[6] = bits.ReadBit();

    if (this->m_initiatorGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[4] ^= byteValue;
    }

    if (this->m_initiatorGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[7] ^= byteValue;
    }

    if (this->m_initiatorGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[0] ^= byteValue;
    }

    if (this->m_initiatorGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[5] ^= byteValue;
    }

    if (this->m_initiatorGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[1] ^= byteValue;
    }

    if (this->m_initiatorGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[6] ^= byteValue;
    }

    if (this->m_initiatorGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[2] ^= byteValue;
    }

    if (this->m_initiatorGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[3] ^= byteValue;
    }
}

// PROVEN: 3f17f8c3 IDENTICAL/UNPROVEN
// OFFSET: 0x006B02D0/0x140360FE0
bool ClientRolePollBegin::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1bf8c301 IDENTICAL/IDENTICAL
// OFFSET: 0x006AC160/0x14035BF80
bool ClientRolePollBegin::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_initiatorGUID[1] = bits.ReadBit();
    this->m_initiatorGUID[5] = bits.ReadBit();
    this->m_initiatorGUID[7] = bits.ReadBit();
    this->m_initiatorGUID[3] = bits.ReadBit();
    this->m_initiatorGUID[2] = bits.ReadBit();
    this->m_initiatorGUID[4] = bits.ReadBit();
    this->m_initiatorGUID[0] = bits.ReadBit();
    this->m_initiatorGUID[6] = bits.ReadBit();

    if (this->m_initiatorGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[4] ^= byteValue;
    }

    if (this->m_initiatorGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[7] ^= byteValue;
    }

    if (this->m_initiatorGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[0] ^= byteValue;
    }

    if (this->m_initiatorGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[5] ^= byteValue;
    }

    if (this->m_initiatorGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[1] ^= byteValue;
    }

    if (this->m_initiatorGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[6] ^= byteValue;
    }

    if (this->m_initiatorGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[2] ^= byteValue;
    }

    if (this->m_initiatorGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_initiatorGUID[3] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 7ebf226f IDENTICAL/UNPROVEN
// OFFSET: 0x006AE720/0x14035EDF0
bool ClientRolePollBegin::Handle() {
    if (ClientRolePollBegin::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF35C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF35C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRolePollBegin*)>(reinterpret_cast<uintptr_t>(ClientRolePollBegin::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 037c35d1 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientRaidMarkersChanged::ClientRaidMarkersChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_raidMarkers = value;
}

// PROVEN: a199df25 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientRaidMarkersChanged::~ClientRaidMarkersChanged() {
}

// PROVEN: df38864b IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientRaidMarkersChanged::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_raidMarkers = value;

    return msg->IsRead() != 0;
}

// PROVEN: aca51efa IDENTICAL/UNPROVEN
// OFFSET: 0x006B8360/0x14036B4B0
bool ClientRaidMarkersChanged::Handle() {
    if (ClientRaidMarkersChanged::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDF50.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDF50.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRaidMarkersChanged*)>(reinterpret_cast<uintptr_t>(ClientRaidMarkersChanged::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 358f7d24 IDENTICAL/IDENTICAL
// OFFSET: 0x006B3560/0x140365030
ClientPetitionAlreadySigned::ClientPetitionAlreadySigned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b2d0f15b IDENTICAL/IDENTICAL
// OFFSET: 0x006A14E0/0x14034E7B0
ClientPetitionAlreadySigned::~ClientPetitionAlreadySigned() {
}

// PROVEN: 9fba4f6f IDENTICAL/IDENTICAL
// OFFSET: 0x006A8EF0/0x140357F40
void ClientPetitionAlreadySigned::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_signerGUID[2] = bits.ReadBit();
    this->m_signerGUID[3] = bits.ReadBit();
    this->m_signerGUID[7] = bits.ReadBit();
    this->m_signerGUID[6] = bits.ReadBit();
    this->m_signerGUID[1] = bits.ReadBit();
    this->m_signerGUID[5] = bits.ReadBit();
    this->m_signerGUID[4] = bits.ReadBit();
    this->m_signerGUID[0] = bits.ReadBit();

    if (this->m_signerGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[4] ^= byteValue;
    }

    if (this->m_signerGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[0] ^= byteValue;
    }

    if (this->m_signerGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[3] ^= byteValue;
    }

    if (this->m_signerGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[1] ^= byteValue;
    }

    if (this->m_signerGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[5] ^= byteValue;
    }

    if (this->m_signerGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[6] ^= byteValue;
    }

    if (this->m_signerGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[7] ^= byteValue;
    }

    if (this->m_signerGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[2] ^= byteValue;
    }
}

// PROVEN: 5150ff1c IDENTICAL/UNPROVEN
// OFFSET: 0x006BC650/0x140370A80
bool ClientPetitionAlreadySigned::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 37e62acb IDENTICAL/IDENTICAL
// OFFSET: 0x006B1D40/0x140363160
bool ClientPetitionAlreadySigned::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_signerGUID[2] = bits.ReadBit();
    this->m_signerGUID[3] = bits.ReadBit();
    this->m_signerGUID[7] = bits.ReadBit();
    this->m_signerGUID[6] = bits.ReadBit();
    this->m_signerGUID[1] = bits.ReadBit();
    this->m_signerGUID[5] = bits.ReadBit();
    this->m_signerGUID[4] = bits.ReadBit();
    this->m_signerGUID[0] = bits.ReadBit();

    if (this->m_signerGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[4] ^= byteValue;
    }

    if (this->m_signerGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[0] ^= byteValue;
    }

    if (this->m_signerGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[3] ^= byteValue;
    }

    if (this->m_signerGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[1] ^= byteValue;
    }

    if (this->m_signerGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[5] ^= byteValue;
    }

    if (this->m_signerGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[6] ^= byteValue;
    }

    if (this->m_signerGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[7] ^= byteValue;
    }

    if (this->m_signerGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_signerGUID[2] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 0fb902cf IDENTICAL/UNPROVEN
// OFFSET: 0x006A97D0/0x140358B10
bool ClientPetitionAlreadySigned::Handle() {
    if (ClientPetitionAlreadySigned::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC58.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC58.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPetitionAlreadySigned*)>(reinterpret_cast<uintptr_t>(ClientPetitionAlreadySigned::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 34137d9d IDENTICAL/IDENTICAL
// OFFSET: 0x006E2C50/0x1403A3780
ClientSummonRaidMemberValidateFailed::ClientSummonRaidMemberValidateFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 64c0cb18 IDENTICAL/IDENTICAL
// OFFSET: 0x006DB2D0/0x1403A7A50
ClientSummonRaidMemberValidateFailed::~ClientSummonRaidMemberValidateFailed() {
}

// OFFSET: 0x006DD220/0x14039C640
void ClientSummonRaidMemberValidateFailed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_members.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = this->m_members.m_data[i].m_memberGUID;
        guid[5] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[4] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = this->m_members.m_data[i].m_memberGUID;

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        uint32_t value = 0;
        msg->Get(value);
        this->m_members.m_data[i].m_reasonCode = value;

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }
    }
}

// PROVEN: ff003015 IDENTICAL/UNPROVEN
// OFFSET: 0x006E53B0/0x1403A6730
bool ClientSummonRaidMemberValidateFailed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DE630/0x14039E120
bool ClientSummonRaidMemberValidateFailed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3) + 4 * count) {
        return false;
    }

    this->m_members.SetCount(count);

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = this->m_members.m_data[i].m_memberGUID;
        guid[5] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[4] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_members.Count(); i++) {
        uint8_t* guid = this->m_members.m_data[i].m_memberGUID;

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        uint32_t value = 0;
        msg->Get(value);
        this->m_members.m_data[i].m_reasonCode = value;

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: b5ed9332 IDENTICAL/UNPROVEN
// OFFSET: 0x006A5650/0x140353450
bool ClientSummonRaidMemberValidateFailed::Handle() {
    if (ClientSummonRaidMemberValidateFailed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDDD4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDDD4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSummonRaidMemberValidateFailed*)>(reinterpret_cast<uintptr_t>(ClientSummonRaidMemberValidateFailed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 889f10a0 IDENTICAL/DIFFERENT
// OFFSET: 0x006F74F0/0x1403BEE80
ClientPvpLogData::ClientPvpLogData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTeamNames = false;
    this->m_hasWinner = false;
    this->m_hasRatings = false;
    this->Unpack(msg, true);
}

// PROVEN: 4f0a870a IDENTICAL/IDENTICAL
// OFFSET: 0x006F8280/0x1403C0830
ClientQuestNpcQueryResponse::ClientQuestNpcQueryResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 65acf6c2 IDENTICAL/IDENTICAL
// OFFSET: 0x006EE990/0x1403B3750
ClientQuestNpcQueryResponse::~ClientQuestNpcQueryResponse() {
}

// OFFSET: 0x006F6DB0/0x1403BE970
void ClientQuestNpcQueryResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_quests.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        this->m_quests.m_data[i].m_npcs.SetCount(bits.ReadBits<24>());
    }

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_quests.m_data[i].m_questID = value;
        }

        for (uint32_t j = 0; j < this->m_quests.m_data[i].m_npcs.Count(); j++) {
            uint32_t value = 0;
            msg->Get(value);
            this->m_quests.m_data[i].m_npcs.m_data[j] = value;
        }
    }
}

// PROVEN: c029172e IDENTICAL/UNPROVEN
// OFFSET: 0x006F6500/0x1403BDE30
bool ClientQuestNpcQueryResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6320/0x1403BDBF0
bool ClientQuestNpcQueryResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t questCount = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((24 * questCount + 7) >> 3) + 4 * questCount) {
        return false;
    }

    this->m_quests.SetCount(questCount);

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        uint32_t count = bits.ReadBits<24>();

        if (msg->m_size - msg->m_read < 4 * count) {
            return false;
        }

        this->m_quests.m_data[i].m_npcs.SetCount(count);
    }

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_quests.m_data[i].m_questID = value;
        }

        for (uint32_t j = 0; j < this->m_quests.m_data[i].m_npcs.Count(); j++) {
            uint32_t value = 0;
            msg->Get(value);
            this->m_quests.m_data[i].m_npcs.m_data[j] = value;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 9d35dc29 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5090/0x140367280
bool ClientQuestNpcQueryResponse::Handle() {
    if (ClientQuestNpcQueryResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE648.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE648.m_left->Get();
        return reinterpret_cast<bool (*)(ClientQuestNpcQueryResponse*)>(reinterpret_cast<uintptr_t>(ClientQuestNpcQueryResponse::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 31da1902 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0D00/0x140376190
ClientSpellinterruptlog::ClientSpellinterruptlog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 595e5c51 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2D90/0x1403502F0
ClientSpellinterruptlog::~ClientSpellinterruptlog() {
}

// PROVEN: 8c8f2174 IDENTICAL/IDENTICAL
// OFFSET: 0x006AC8F0/0x14035C7D0
void ClientSpellinterruptlog::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_victimGUID[4] = bits.ReadBit();
    this->m_casterGUID[5] = bits.ReadBit();
    this->m_casterGUID[6] = bits.ReadBit();
    this->m_casterGUID[1] = bits.ReadBit();
    this->m_casterGUID[3] = bits.ReadBit();
    this->m_casterGUID[0] = bits.ReadBit();
    this->m_victimGUID[3] = bits.ReadBit();
    this->m_victimGUID[5] = bits.ReadBit();
    this->m_victimGUID[1] = bits.ReadBit();
    this->m_casterGUID[4] = bits.ReadBit();
    this->m_casterGUID[7] = bits.ReadBit();
    this->m_victimGUID[7] = bits.ReadBit();
    this->m_victimGUID[6] = bits.ReadBit();
    this->m_casterGUID[2] = bits.ReadBit();
    this->m_victimGUID[2] = bits.ReadBit();
    this->m_victimGUID[0] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_casterGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[7] ^= byteValue;
    }

    if (this->m_casterGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[6] ^= byteValue;
    }

    if (this->m_casterGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[3] ^= byteValue;
    }

    if (this->m_casterGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[4] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_interruptingSpellID = value;
    }

    if (this->m_victimGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_casterGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[5] ^= byteValue;
    }

    if (this->m_casterGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_interruptedSpellID = value;
    }

    if (this->m_victimGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }
}

// PROVEN: 75119e83 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF370/0x1403743D0
bool ClientSpellinterruptlog::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 7f9f40c7 IDENTICAL/IDENTICAL
// OFFSET: 0x006BDCB0/0x140372750
bool ClientSpellinterruptlog::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_victimGUID[4] = bits.ReadBit();
    this->m_casterGUID[5] = bits.ReadBit();
    this->m_casterGUID[6] = bits.ReadBit();
    this->m_casterGUID[1] = bits.ReadBit();
    this->m_casterGUID[3] = bits.ReadBit();
    this->m_casterGUID[0] = bits.ReadBit();
    this->m_victimGUID[3] = bits.ReadBit();
    this->m_victimGUID[5] = bits.ReadBit();
    this->m_victimGUID[1] = bits.ReadBit();
    this->m_casterGUID[4] = bits.ReadBit();
    this->m_casterGUID[7] = bits.ReadBit();
    this->m_victimGUID[7] = bits.ReadBit();
    this->m_victimGUID[6] = bits.ReadBit();
    this->m_casterGUID[2] = bits.ReadBit();
    this->m_victimGUID[2] = bits.ReadBit();
    this->m_victimGUID[0] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_casterGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[7] ^= byteValue;
    }

    if (this->m_casterGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[6] ^= byteValue;
    }

    if (this->m_casterGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[3] ^= byteValue;
    }

    if (this->m_casterGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[4] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_interruptingSpellID = value;
    }

    if (this->m_victimGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_casterGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[5] ^= byteValue;
    }

    if (this->m_casterGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[1] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_interruptedSpellID = value;
    }

    if (this->m_victimGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 2ca37e9e IDENTICAL/UNPROVEN
// OFFSET: 0x006C0800/0x140375B60
bool ClientSpellinterruptlog::Handle() {
    if (ClientSpellinterruptlog::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD0CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD0CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSpellinterruptlog*)>(reinterpret_cast<uintptr_t>(ClientSpellinterruptlog::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 484b7fc9 IDENTICAL/IDENTICAL
// OFFSET: 0x006C7960/0x14037E7E0
ClientXpGainAborted::ClientXpGainAborted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1a3cab71 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2640/0x14034FB30
ClientXpGainAborted::~ClientXpGainAborted() {
}

// PROVEN: 85996c46 IDENTICAL/IDENTICAL
// OFFSET: 0x006ACF00/0x14035CF50
void ClientXpGainAborted::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_victimGUID[4] = bits.ReadBit();
    this->m_victimGUID[0] = bits.ReadBit();
    this->m_victimGUID[1] = bits.ReadBit();
    this->m_victimGUID[7] = bits.ReadBit();
    this->m_victimGUID[2] = bits.ReadBit();
    this->m_victimGUID[6] = bits.ReadBit();
    this->m_victimGUID[5] = bits.ReadBit();
    this->m_victimGUID[3] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_victimGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_xpBonus = value;
    }

    if (this->m_victimGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_reason = value;
    }

    if (this->m_victimGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_xp = value;
    }
}

// PROVEN: 36fb169c IDENTICAL/UNPROVEN
// OFFSET: 0x006CB4B0/0x140383AB0
bool ClientXpGainAborted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b2890328 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4050/0x140379FD0
bool ClientXpGainAborted::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_victimGUID[4] = bits.ReadBit();
    this->m_victimGUID[0] = bits.ReadBit();
    this->m_victimGUID[1] = bits.ReadBit();
    this->m_victimGUID[7] = bits.ReadBit();
    this->m_victimGUID[2] = bits.ReadBit();
    this->m_victimGUID[6] = bits.ReadBit();
    this->m_victimGUID[5] = bits.ReadBit();
    this->m_victimGUID[3] = bits.ReadBit();

    uint8_t byteValue;

    if (this->m_victimGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_xpBonus = value;
    }

    if (this->m_victimGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_reason = value;
    }

    if (this->m_victimGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_xp = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 62a3a392 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD340/0x14035D480
bool ClientXpGainAborted::Handle() {
    if (ClientXpGainAborted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE774.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE774.m_left->Get();
        return reinterpret_cast<bool (*)(ClientXpGainAborted*)>(reinterpret_cast<uintptr_t>(ClientXpGainAborted::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 5d861c08 IDENTICAL/IDENTICAL
// OFFSET: 0x006B1EA0/0x140363320
ClientItemExpirePurchaseRefund::ClientItemExpirePurchaseRefund(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d3c55b32 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1230/0x14034E510
ClientItemExpirePurchaseRefund::~ClientItemExpirePurchaseRefund() {
}

// PROVEN: 2024df55 IDENTICAL/IDENTICAL
// OFFSET: 0x006AABB0/0x14035A3E0
void ClientItemExpirePurchaseRefund::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_itemGUID[3] = bits.ReadBit();
    this->m_itemGUID[0] = bits.ReadBit();
    this->m_itemGUID[5] = bits.ReadBit();
    this->m_itemGUID[4] = bits.ReadBit();
    this->m_itemGUID[6] = bits.ReadBit();
    this->m_itemGUID[2] = bits.ReadBit();
    this->m_itemGUID[1] = bits.ReadBit();
    this->m_itemGUID[7] = bits.ReadBit();

    if (this->m_itemGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[1] ^= byteValue;
    }

    if (this->m_itemGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[0] ^= byteValue;
    }

    if (this->m_itemGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[3] ^= byteValue;
    }

    if (this->m_itemGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[4] ^= byteValue;
    }

    if (this->m_itemGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[7] ^= byteValue;
    }

    if (this->m_itemGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[6] ^= byteValue;
    }

    if (this->m_itemGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[5] ^= byteValue;
    }

    if (this->m_itemGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[2] ^= byteValue;
    }
}

// PROVEN: fc3cd88e IDENTICAL/UNPROVEN
// OFFSET: 0x006AF240/0x14035FBA0
bool ClientItemExpirePurchaseRefund::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b91bd39e IDENTICAL/IDENTICAL
// OFFSET: 0x006A30D0/0x140350800
bool ClientItemExpirePurchaseRefund::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_itemGUID[3] = bits.ReadBit();
    this->m_itemGUID[0] = bits.ReadBit();
    this->m_itemGUID[5] = bits.ReadBit();
    this->m_itemGUID[4] = bits.ReadBit();
    this->m_itemGUID[6] = bits.ReadBit();
    this->m_itemGUID[2] = bits.ReadBit();
    this->m_itemGUID[1] = bits.ReadBit();
    this->m_itemGUID[7] = bits.ReadBit();

    if (this->m_itemGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[1] ^= byteValue;
    }

    if (this->m_itemGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[0] ^= byteValue;
    }

    if (this->m_itemGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[3] ^= byteValue;
    }

    if (this->m_itemGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[4] ^= byteValue;
    }

    if (this->m_itemGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[7] ^= byteValue;
    }

    if (this->m_itemGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[6] ^= byteValue;
    }

    if (this->m_itemGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[5] ^= byteValue;
    }

    if (this->m_itemGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_itemGUID[2] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 815619c3 IDENTICAL/UNPROVEN
// OFFSET: 0x006BE2C0/0x140372F50
bool ClientItemExpirePurchaseRefund::Handle() {
    if (ClientItemExpirePurchaseRefund::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFF4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFF4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemExpirePurchaseRefund*)>(reinterpret_cast<uintptr_t>(ClientItemExpirePurchaseRefund::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: af5a1b99 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2A20/0x14034FF10
ClientGuildSendGuildXP::ClientGuildSendGuildXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ddcd1f67 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DE0/0x14034DE80
ClientGuildSendGuildXP::~ClientGuildSendGuildXP() {
}

// PROVEN: d0b87dc0 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1BE0/0x14034F040
void ClientGuildSendGuildXP::Unpack(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_memberTotalXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildRemainingXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildTodayXP = value;

    value = 0;
    msg->Get(value);
    this->m_memberWeeklyXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildCurrentXP = value;
}

// PROVEN: cad89c95 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4D50/0x14037AA00
bool ClientGuildSendGuildXP::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 43a88014 IDENTICAL/IDENTICAL
// OFFSET: 0x006B4EC0/0x140367070
bool ClientGuildSendGuildXP::Read(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_memberTotalXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildRemainingXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildTodayXP = value;

    value = 0;
    msg->Get(value);
    this->m_memberWeeklyXP = value;

    value = 0;
    msg->Get(value);
    this->m_guildCurrentXP = value;

    return msg->IsRead() != 0;
}

// PROVEN: c7eadaae IDENTICAL/UNPROVEN
// OFFSET: 0x006B5720/0x140367AF0
bool ClientGuildSendGuildXP::Handle() {
    if (ClientGuildSendGuildXP::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF5C0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF5C0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildSendGuildXP*)>(reinterpret_cast<uintptr_t>(ClientGuildSendGuildXP::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 56606a28 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3230/0x14035BB30
ClientGuildSendMaxDailyXP::ClientGuildSendMaxDailyXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_xp = value;
}

// PROVEN: 70b06b18 IDENTICAL/UNPROVEN
// OFFSET: 0x006A2650/0x14034ED30
ClientGuildSendMaxDailyXP::~ClientGuildSendMaxDailyXP() {
}

// PROVEN: 2959bacd IDENTICAL/IDENTICAL
// OFFSET: 0x006C3F80/0x14037E820
bool ClientGuildSendMaxDailyXP::Get(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_xp = value;

    return msg->IsRead() != 0;
}

// PROVEN: 441555da IDENTICAL/UNPROVEN
// OFFSET: 0x006BE040/0x140372C20
bool ClientGuildSendMaxDailyXP::Handle() {
    if (ClientGuildSendMaxDailyXP::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF7BC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF7BC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildSendMaxDailyXP*)>(reinterpret_cast<uintptr_t>(ClientGuildSendMaxDailyXP::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 1eb204a6 IDENTICAL/IDENTICAL
// OFFSET: 0x006B93F0/0x14036C9B0
ClientGuildSendRankChange::ClientGuildSendRankChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5f16882c IDENTICAL/IDENTICAL
// OFFSET: 0x006A0ED0/0x14034DF90
ClientGuildSendRankChange::~ClientGuildSendRankChange() {
}

// PROVEN: e9f3a635 IDENTICAL/DIFFERENT
// OFFSET: 0x006A4AD0/0x1403528F0
void ClientGuildSendRankChange::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_officerGUID[7] = bits.ReadBit();
    this->m_officerGUID[2] = bits.ReadBit();
    this->m_otherGUID[2] = bits.ReadBit();
    this->m_officerGUID[1] = bits.ReadBit();
    this->m_otherGUID[1] = bits.ReadBit();
    this->m_otherGUID[7] = bits.ReadBit();
    this->m_otherGUID[0] = bits.ReadBit();
    this->m_otherGUID[5] = bits.ReadBit();
    this->m_otherGUID[4] = bits.ReadBit();
    this->m_promote = bits.ReadBit() != 0;
    this->m_officerGUID[5] = bits.ReadBit();
    this->m_officerGUID[0] = bits.ReadBit();
    this->m_otherGUID[6] = bits.ReadBit();
    this->m_officerGUID[3] = bits.ReadBit();
    this->m_officerGUID[6] = bits.ReadBit();
    this->m_otherGUID[3] = bits.ReadBit();
    this->m_officerGUID[4] = bits.ReadBit();

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_rankID = value;
    }

    uint8_t* guids = this->m_officerGUID;
    if (guids[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[3] ^= byteValue;
    }

    if (guids[15]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[15] ^= byteValue;
    }

    if (guids[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[6] ^= byteValue;
    }

    if (guids[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[2] ^= byteValue;
    }

    if (guids[13]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[13] ^= byteValue;
    }

    if (guids[8]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[8] ^= byteValue;
    }

    if (guids[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[7] ^= byteValue;
    }

    if (guids[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[5] ^= byteValue;
    }

    if (guids[10]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[10] ^= byteValue;
    }

    if (guids[9]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[9] ^= byteValue;
    }

    if (guids[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[0] ^= byteValue;
    }

    if (guids[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[4] ^= byteValue;
    }

    if (guids[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[1] ^= byteValue;
    }

    if (guids[11]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[11] ^= byteValue;
    }

    if (guids[14]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[14] ^= byteValue;
    }

    if (guids[12]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[12] ^= byteValue;
    }
}

// PROVEN: 5f79c5c4 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4570/0x14037A420
bool ClientGuildSendRankChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3703ea09 IDENTICAL/DIFFERENT
// OFFSET: 0x006B5760/0x140367B50
bool ClientGuildSendRankChange::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_officerGUID[7] = bits.ReadBit();
    this->m_officerGUID[2] = bits.ReadBit();
    this->m_otherGUID[2] = bits.ReadBit();
    this->m_officerGUID[1] = bits.ReadBit();
    this->m_otherGUID[1] = bits.ReadBit();
    this->m_otherGUID[7] = bits.ReadBit();
    this->m_otherGUID[0] = bits.ReadBit();
    this->m_otherGUID[5] = bits.ReadBit();
    this->m_otherGUID[4] = bits.ReadBit();
    this->m_promote = bits.ReadBit() != 0;
    this->m_officerGUID[5] = bits.ReadBit();
    this->m_officerGUID[0] = bits.ReadBit();
    this->m_otherGUID[6] = bits.ReadBit();
    this->m_officerGUID[3] = bits.ReadBit();
    this->m_officerGUID[6] = bits.ReadBit();
    this->m_otherGUID[3] = bits.ReadBit();
    this->m_officerGUID[4] = bits.ReadBit();

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_rankID = value;
    }

    uint8_t* guids = this->m_officerGUID;
    if (guids[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[3] ^= byteValue;
    }

    if (guids[15]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[15] ^= byteValue;
    }

    if (guids[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[6] ^= byteValue;
    }

    if (guids[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[2] ^= byteValue;
    }

    if (guids[13]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[13] ^= byteValue;
    }

    if (guids[8]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[8] ^= byteValue;
    }

    if (guids[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[7] ^= byteValue;
    }

    if (guids[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[5] ^= byteValue;
    }

    if (guids[10]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[10] ^= byteValue;
    }

    if (guids[9]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[9] ^= byteValue;
    }

    if (guids[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[0] ^= byteValue;
    }

    if (guids[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[4] ^= byteValue;
    }

    if (guids[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[1] ^= byteValue;
    }

    if (guids[11]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[11] ^= byteValue;
    }

    if (guids[14]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[14] ^= byteValue;
    }

    if (guids[12]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guids[12] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 9aa6bf1b IDENTICAL/UNPROVEN
// OFFSET: 0x006AB240/0x14035ACA0
bool ClientGuildSendRankChange::Handle() {
    if (ClientGuildSendRankChange::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE3BC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE3BC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildSendRankChange*)>(reinterpret_cast<uintptr_t>(ClientGuildSendRankChange::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 0698851a IDENTICAL/IDENTICAL
// OFFSET: 0x006C7880/0x14037E6B0
ClientGuildMemberRecipes::ClientGuildMemberRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1b288172 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1980/0x14034ED40
ClientGuildMemberRecipes::~ClientGuildMemberRecipes() {
}

// PROVEN: 8ff528fe IDENTICAL/IDENTICAL
// OFFSET: 0x006BDAF0/0x140372500
void ClientGuildMemberRecipes::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_memberGUID[0] = bits.ReadBit();
    this->m_memberGUID[3] = bits.ReadBit();
    this->m_memberGUID[7] = bits.ReadBit();
    this->m_memberGUID[4] = bits.ReadBit();
    this->m_memberGUID[6] = bits.ReadBit();
    this->m_memberGUID[2] = bits.ReadBit();
    this->m_memberGUID[1] = bits.ReadBit();
    this->m_memberGUID[5] = bits.ReadBit();


    if (this->m_memberGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[0] ^= byteValue;
    }

    if (this->m_memberGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[1] ^= byteValue;
    }

    if (this->m_memberGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[7] ^= byteValue;
    }

    if (this->m_memberGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[4] ^= byteValue;
    }

    if (this->m_memberGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillRank = value;
    }

    for (uint32_t i = 0; i < 300; i++) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_skillLineBitArray[i] = byteValue;
    }

    if (this->m_memberGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[3] ^= byteValue;
    }

    if (this->m_memberGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[6] ^= byteValue;
    }

    if (this->m_memberGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[2] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillLineID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillStep = value;
    }
}

// PROVEN: 707acb3f IDENTICAL/UNPROVEN
// OFFSET: 0x006C1170/0x140376690
bool ClientGuildMemberRecipes::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 12c93e65 IDENTICAL/IDENTICAL
// OFFSET: 0x006B0E30/0x140361DF0
bool ClientGuildMemberRecipes::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_memberGUID[0] = bits.ReadBit();
    this->m_memberGUID[3] = bits.ReadBit();
    this->m_memberGUID[7] = bits.ReadBit();
    this->m_memberGUID[4] = bits.ReadBit();
    this->m_memberGUID[6] = bits.ReadBit();
    this->m_memberGUID[2] = bits.ReadBit();
    this->m_memberGUID[1] = bits.ReadBit();
    this->m_memberGUID[5] = bits.ReadBit();


    if (this->m_memberGUID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[0] ^= byteValue;
    }

    if (this->m_memberGUID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[1] ^= byteValue;
    }

    if (this->m_memberGUID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[7] ^= byteValue;
    }

    if (this->m_memberGUID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[4] ^= byteValue;
    }

    if (this->m_memberGUID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillRank = value;
    }

    for (uint32_t i = 0; i < 300; i++) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_skillLineBitArray[i] = byteValue;
    }

    if (this->m_memberGUID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[3] ^= byteValue;
    }

    if (this->m_memberGUID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[6] ^= byteValue;
    }

    if (this->m_memberGUID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[2] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillLineID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_skillStep = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: ed1c6915 IDENTICAL/UNPROVEN
// OFFSET: 0x006BE7F0/0x140373580
bool ClientGuildMemberRecipes::Handle() {
    if (ClientGuildMemberRecipes::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF678.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF678.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildMemberRecipes*)>(reinterpret_cast<uintptr_t>(ClientGuildMemberRecipes::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 8d9c2995 IDENTICAL/IDENTICAL
// OFFSET: 0x006EE9F0/0x1403B37D0
ClientGuildKnownRecipes::ClientGuildKnownRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: bf648a98 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5530/0x1403A6920
ClientGuildKnownRecipes::~ClientGuildKnownRecipes() {
}

// OFFSET: 0x006E36E0/0x1403A4460
void ClientGuildKnownRecipes::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t high = bits.ReadBits<8>() << 8;
    this->m_recipes.SetCount(high | bits.ReadBits<8>());

    for (uint32_t i = 0; i < this->m_recipes.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_recipes.m_data[i].m_skillLineID = value;

        for (uint32_t j = 0; j < 300; j++) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_recipes.m_data[i].m_skillLineBitArray[j] = byteValue;
        }
    }
}

// PROVEN: d0392b4c IDENTICAL/UNPROVEN
// OFFSET: 0x006E5A20/0x1403A6F90
bool ClientGuildKnownRecipes::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2329671e IDENTICAL/DIFFERENT
// OFFSET: 0x006E21F0/0x1403A2C00
bool ClientGuildKnownRecipes::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t high = bits.ReadBits<8>() << 8;
    uint32_t count = high | bits.ReadBits<8>();

    if (msg->m_size - msg->m_read < 304 * count) {
        return false;
    }

    this->m_recipes.SetCount(count);

    for (uint32_t i = 0; i < this->m_recipes.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_recipes.m_data[i].m_skillLineID = value;

        for (uint32_t j = 0; j < 300; j++) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_recipes.m_data[i].m_skillLineBitArray[j] = byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 915ae531 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3300/0x140350A80
bool ClientGuildKnownRecipes::Handle() {
    if (ClientGuildKnownRecipes::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF000.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF000.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildKnownRecipes*)>(reinterpret_cast<uintptr_t>(ClientGuildKnownRecipes::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: d0abc103 IDENTICAL/IDENTICAL
// OFFSET: 0x006A6B80/0x140355050
ClientStartTimer::ClientStartTimer(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fbf4b7cf IDENTICAL/IDENTICAL
// OFFSET: 0x006A13A0/0x14034E680
ClientStartTimer::~ClientStartTimer() {
}

// PROVEN: f60d645b IDENTICAL/DIFFERENT
// OFFSET: 0x006A2460/0x14034F960
void ClientStartTimer::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_type = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_timeLeft = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_totalTime = value;
    }
}

// PROVEN: 55311443 IDENTICAL/UNPROVEN
// OFFSET: 0x006B8420/0x14036B5B0
bool ClientStartTimer::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5110d71f IDENTICAL/DIFFERENT
// OFFSET: 0x006AE3E0/0x14035E9B0
bool ClientStartTimer::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_type = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_timeLeft = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_totalTime = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 08421d12 IDENTICAL/UNPROVEN
// OFFSET: 0x006B64E0/0x140368C60
bool ClientStartTimer::Handle() {
    if (ClientStartTimer::s_handler) {
        uint32_t right = g_jamHandlerKey_CFED68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFED68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientStartTimer*)>(reinterpret_cast<uintptr_t>(ClientStartTimer::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 353779a4 IDENTICAL/IDENTICAL
// OFFSET: 0x006D6510/0x140393A60
ClientItemRefundInfoResponse::ClientItemRefundInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0b024b2c IDENTICAL/IDENTICAL
// OFFSET: 0x006CA830/0x140382900
ClientItemRefundInfoResponse::~ClientItemRefundInfoResponse() {
}

// PROVEN: 7c4d3802 IDENTICAL/IDENTICAL
// OFFSET: 0x006A63A0/0x140354600
void ClientItemRefundInfoResponse::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = this->m_info.m_itemGUID;
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();

    uint8_t byteValue;

    if (guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[7] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_refundTimeLeft = value;
    }

    for (uint32_t i = 0; i < 5; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_items[i].m_count = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_items[i].m_id = value;
        }
    }

    if (guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[6] ^= byteValue;
    }

    if (guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[4] ^= byteValue;
    }

    if (guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[3] ^= byteValue;
    }

    if (guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[2] ^= byteValue;
    }

    for (uint32_t i = 0; i < 5; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_currencies[i].m_count = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_currencies[i].m_id = value;
        }
    }

    if (guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[1] ^= byteValue;
    }

    if (guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_flags = value;
    }

    if (guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[0] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_money = value;
    }
}

// PROVEN: 7cb48fbd IDENTICAL/UNPROVEN
// OFFSET: 0x006E1F80/0x1403A2810
bool ClientItemRefundInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a8034dc4 IDENTICAL/IDENTICAL
// OFFSET: 0x006D98D0/0x140397D80
bool ClientItemRefundInfoResponse::Read(CDataStore* msg, bool full) {
    uint8_t* guid = this->m_info.m_itemGUID;
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();

    uint8_t byteValue;

    if (guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[7] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_refundTimeLeft = value;
    }

    for (uint32_t i = 0; i < 5; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_items[i].m_count = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_items[i].m_id = value;
        }
    }

    if (guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[6] ^= byteValue;
    }

    if (guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[4] ^= byteValue;
    }

    if (guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[3] ^= byteValue;
    }

    if (guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[2] ^= byteValue;
    }

    for (uint32_t i = 0; i < 5; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_currencies[i].m_count = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_info.m_cost.m_currencies[i].m_id = value;
        }
    }

    if (guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[1] ^= byteValue;
    }

    if (guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[5] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_flags = value;
    }

    if (guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[0] ^= byteValue;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_money = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 36052659 IDENTICAL/UNPROVEN
// OFFSET: 0x006ABBF0/0x14035B8E0
bool ClientItemRefundInfoResponse::Handle() {
    if (ClientItemRefundInfoResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE5F0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE5F0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemRefundInfoResponse*)>(reinterpret_cast<uintptr_t>(ClientItemRefundInfoResponse::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006D9620/0x140397A60
ClientItemRefundResult::ClientItemRefundResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_result.m_hasCost = false;
    this->Unpack(msg, true);
}

// PROVEN: cbb77fee IDENTICAL/IDENTICAL
// OFFSET: 0x006DA120/0x140398720
ClientItemRefundResult::~ClientItemRefundResult() {
}

// PROVEN: 95d30290 IDENTICAL/IDENTICAL
// OFFSET: 0x006B97B0/0x14036CE20
void ClientItemRefundResult::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = this->m_result.m_itemGUID;
    JamBitReader bits(msg);
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_result.m_hasCost = bits.ReadBit() != 0;

    if (this->m_result.m_hasCost) {
        uint32_t value;

        for (uint32_t i = 0; i < 5; i++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_currencies[i].m_count = value;
            }

            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_currencies[i].m_id = value;
            }
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_money = value;
        }

        for (uint32_t i = 0; i < 5; i++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_items[i].m_count = value;
            }

            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_items[i].m_id = value;
            }
        }
    }

    if (guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[0] ^= byteValue;
    }

    if (guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[3] ^= byteValue;
    }

    if (guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[1] ^= byteValue;
    }

    if (guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[6] ^= byteValue;
    }

    if (guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[4] ^= byteValue;
    }

    if (guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[2] ^= byteValue;
    }

    if (guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[7] ^= byteValue;
    }

    if (guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[5] ^= byteValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_result.m_result = byteValue;
    }
}

// PROVEN: 61dc7aeb IDENTICAL/UNPROVEN
// OFFSET: 0x006E1F60/0x1403A27F0
bool ClientItemRefundResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DD910/0x14039CEA0
bool ClientItemRefundResult::Read(CDataStore* msg, bool full) {
    uint8_t* guid = this->m_result.m_itemGUID;
    JamBitReader bits(msg);
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_result.m_hasCost = bits.ReadBit() != 0;

    if (this->m_result.m_hasCost) {
        uint32_t value;

        for (uint32_t i = 0; i < 5; i++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_currencies[i].m_count = value;
            }

            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_currencies[i].m_id = value;
            }
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_money = value;
        }

        for (uint32_t i = 0; i < 5; i++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_items[i].m_count = value;
            }

            {
                uint32_t value = 0;
                msg->Get(value);
                this->m_result.m_cost.m_items[i].m_id = value;
            }
        }
    }

    if (guid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[0] ^= byteValue;
    }

    if (guid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[3] ^= byteValue;
    }

    if (guid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[1] ^= byteValue;
    }

    if (guid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[6] ^= byteValue;
    }

    if (guid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[4] ^= byteValue;
    }

    if (guid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[2] ^= byteValue;
    }

    if (guid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[7] ^= byteValue;
    }

    if (guid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        guid[5] ^= byteValue;
    }

    {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_result.m_result = byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 343f81cf IDENTICAL/UNPROVEN
// OFFSET: 0x006BB780/0x14036F740
bool ClientItemRefundResult::Handle() {
    if (ClientItemRefundResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE17C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE17C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientItemRefundResult*)>(reinterpret_cast<uintptr_t>(ClientItemRefundResult::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: a1bd9c8d IDENTICAL/IDENTICAL
// OFFSET: 0x006C1180/0x1403766A0
ClientQuestGiverQuestComplete::ClientQuestGiverQuestComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 6ad11a2e IDENTICAL/IDENTICAL
// OFFSET: 0x006A1DB0/0x14034F230
ClientQuestGiverQuestComplete::~ClientQuestGiverQuestComplete() {
}

// PROVEN: d29cc43c IDENTICAL/IDENTICAL
// OFFSET: 0x006B54F0/0x140367840
void ClientQuestGiverQuestComplete::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_talentReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_numSkillUpsReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_moneyReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_xpReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_questID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_skillLineIDReward = value;
    }

    JamBitReader bits(msg);
    this->m_complete.m_useQuestReward = bits.ReadBit() != 0;
    this->m_complete.m_launchGossip = bits.ReadBit() != 0;
}

// PROVEN: 6b4821fd IDENTICAL/UNPROVEN
// OFFSET: 0x006C7910/0x14037E790
bool ClientQuestGiverQuestComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5260feb1 IDENTICAL/IDENTICAL
// OFFSET: 0x006B6C70/0x140369830
bool ClientQuestGiverQuestComplete::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_talentReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_numSkillUpsReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_moneyReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_xpReward = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_questID = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_complete.m_skillLineIDReward = value;
    }

    JamBitReader bits(msg);
    this->m_complete.m_useQuestReward = bits.ReadBit() != 0;
    this->m_complete.m_launchGossip = bits.ReadBit() != 0;

    return msg->IsRead() != 0;
}

// PROVEN: f53c9eb7 IDENTICAL/UNPROVEN
// OFFSET: 0x006B07E0/0x1403615F0
bool ClientQuestGiverQuestComplete::Handle() {
    if (ClientQuestGiverQuestComplete::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFC8C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFC8C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientQuestGiverQuestComplete*)>(reinterpret_cast<uintptr_t>(ClientQuestGiverQuestComplete::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 7887caa1 IDENTICAL/UNPROVEN
// OFFSET: 0x006BD0A0/0x1403717A0
ClientSupercededSpells::ClientSupercededSpells(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_supercededSpellID = value;

    value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// PROVEN: 53138902 IDENTICAL/UNPROVEN
// OFFSET: 0x006A10B0/0x14034F400
ClientSupercededSpells::~ClientSupercededSpells() {
}

// PROVEN: d1160811 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8450/0x14037F2B0
bool ClientSupercededSpells::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: c52af815 IDENTICAL/NONE
// OFFSET: 0x006B7A90
bool ClientSupercededSpells::Read(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_supercededSpellID = value;

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 57acec47 IDENTICAL/UNPROVEN
// OFFSET: 0x006B44D0/0x140366400
bool ClientSupercededSpells::Handle() {
    if (ClientSupercededSpells::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF820.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF820.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSupercededSpells*)>(reinterpret_cast<uintptr_t>(ClientSupercededSpells::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: e4b68068 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2200/0x14034F6D0
ClientLearnedSpell::ClientLearnedSpell(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_spellID = value;
    }

    {
        uint32_t suppress = 0;
        msg->Get(suppress);
        this->m_suppressMessaging = suppress;
    }
}

// PROVEN: f2173085 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E70/0x14034F2E0
ClientLearnedSpell::~ClientLearnedSpell() {
}

// PROVEN: 79f90ab7 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB500/0x140383B00
bool ClientLearnedSpell::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2a3d4619 IDENTICAL/NONE
// OFFSET: 0x006C0550
bool ClientLearnedSpell::Read(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_suppressMessaging = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 375a068a IDENTICAL/UNPROVEN
// OFFSET: 0x006ABD00/0x14035B9A0
bool ClientLearnedSpell::Handle() {
    if (ClientLearnedSpell::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD66C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD66C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLearnedSpell*)>(reinterpret_cast<uintptr_t>(ClientLearnedSpell::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 96e29448 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2AD0/0x140350010
ClientDebugRuneRegen::ClientDebugRuneRegen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c4794dee IDENTICAL/IDENTICAL
// OFFSET: 0x006A0D90/0x14034DDB0
ClientDebugRuneRegen::~ClientDebugRuneRegen() {
}

// PROVEN: 5bb37c6b IDENTICAL/IDENTICAL
// OFFSET: 0x006A1410/0x14034E6A0
void ClientDebugRuneRegen::Unpack(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 6; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeCount[i] = value;
    }

    for (uint32_t i = 0; i < 6; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeCooldown[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        float value = 0.0f;
        msg->Get(value);
        this->m_regen[i] = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeMask = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeState = value;
    }
}

// PROVEN: 43698a19 IDENTICAL/UNPROVEN
// OFFSET: 0x006C43A0/0x14037A1F0
bool ClientDebugRuneRegen::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e87b017f IDENTICAL/IDENTICAL
// OFFSET: 0x006ACD80/0x14035CDC0
bool ClientDebugRuneRegen::Read(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 6; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeCount[i] = value;
    }

    for (uint32_t i = 0; i < 6; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeCooldown[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        float value = 0.0f;
        msg->Get(value);
        this->m_regen[i] = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeMask = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_runeState = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 02075f08 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8D50/0x140357D00
bool ClientDebugRuneRegen::Handle() {
    if (ClientDebugRuneRegen::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCF9C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCF9C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDebugRuneRegen*)>(reinterpret_cast<uintptr_t>(ClientDebugRuneRegen::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: d0537776 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4910/0x14037A850
ClientReforgeResult::ClientReforgeResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;
}

// PROVEN: 5e51f78b IDENTICAL/UNPROVEN
// OFFSET: 0x006A1F40/0x14034F3E0
ClientReforgeResult::~ClientReforgeResult() {
}

// PROVEN: 7fa1513b IDENTICAL/IDENTICAL
// OFFSET: 0x006C9980/0x1403813C0
bool ClientReforgeResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;

    return msg->IsRead() != 0;
}

// PROVEN: 962fa9d0 IDENTICAL/UNPROVEN
// OFFSET: 0x006A4A80/0x140352890
bool ClientReforgeResult::Handle() {
    if (ClientReforgeResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE754.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE754.m_left->Get();
        return reinterpret_cast<bool (*)(ClientReforgeResult*)>(reinterpret_cast<uintptr_t>(ClientReforgeResult::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 641ae5dd IDENTICAL/IDENTICAL
// OFFSET: 0x006C0A90/0x140375E90
ClientForgeMasterSet::ClientForgeMasterSet(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 99b9427f IDENTICAL/IDENTICAL
// OFFSET: 0x006A1220/0x14034E500
ClientForgeMasterSet::~ClientForgeMasterSet() {
}

// PROVEN: 60002a0a IDENTICAL/IDENTICAL
// OFFSET: 0x006BB410/0x14036F2D0
void ClientForgeMasterSet::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_forgeMaster[1] = bits.ReadBit();
    this->m_forgeMaster[3] = bits.ReadBit();
    this->m_forgeMaster[4] = bits.ReadBit();
    this->m_forgeMaster[2] = bits.ReadBit();
    this->m_forgeMaster[6] = bits.ReadBit();
    this->m_forgeMaster[0] = bits.ReadBit();
    this->m_forgeMaster[7] = bits.ReadBit();
    this->m_forgeMaster[5] = bits.ReadBit();

    if (this->m_forgeMaster[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[6] ^= byteValue;
    }

    if (this->m_forgeMaster[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[2] ^= byteValue;
    }

    if (this->m_forgeMaster[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[3] ^= byteValue;
    }

    if (this->m_forgeMaster[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[4] ^= byteValue;
    }

    if (this->m_forgeMaster[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[5] ^= byteValue;
    }

    if (this->m_forgeMaster[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[0] ^= byteValue;
    }

    if (this->m_forgeMaster[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[7] ^= byteValue;
    }

    if (this->m_forgeMaster[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[1] ^= byteValue;
    }
}

// PROVEN: 68b841af IDENTICAL/UNPROVEN
// OFFSET: 0x006BED10/0x140373C00
bool ClientForgeMasterSet::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 37402ec0 IDENTICAL/IDENTICAL
// OFFSET: 0x006A4230/0x140351C40
bool ClientForgeMasterSet::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_forgeMaster[1] = bits.ReadBit();
    this->m_forgeMaster[3] = bits.ReadBit();
    this->m_forgeMaster[4] = bits.ReadBit();
    this->m_forgeMaster[2] = bits.ReadBit();
    this->m_forgeMaster[6] = bits.ReadBit();
    this->m_forgeMaster[0] = bits.ReadBit();
    this->m_forgeMaster[7] = bits.ReadBit();
    this->m_forgeMaster[5] = bits.ReadBit();

    if (this->m_forgeMaster[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[6] ^= byteValue;
    }

    if (this->m_forgeMaster[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[2] ^= byteValue;
    }

    if (this->m_forgeMaster[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[3] ^= byteValue;
    }

    if (this->m_forgeMaster[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[4] ^= byteValue;
    }

    if (this->m_forgeMaster[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[5] ^= byteValue;
    }

    if (this->m_forgeMaster[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[0] ^= byteValue;
    }

    if (this->m_forgeMaster[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[7] ^= byteValue;
    }

    if (this->m_forgeMaster[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_forgeMaster[1] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: ff147110 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8160/0x140356E40
bool ClientForgeMasterSet::Handle() {
    if (ClientForgeMasterSet::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD934.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD934.m_left->Get();
        return reinterpret_cast<bool (*)(ClientForgeMasterSet*)>(reinterpret_cast<uintptr_t>(ClientForgeMasterSet::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: d7595109 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3270/0x14034F7C0
ClientVoidStorageFailed::ClientVoidStorageFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;
}

// PROVEN: 7ee7c7c8 IDENTICAL/UNPROVEN
// OFFSET: 0x006A19B0/0x14034E9E0
ClientVoidStorageFailed::~ClientVoidStorageFailed() {
}

// PROVEN: e78ce64e IDENTICAL/IDENTICAL
// OFFSET: 0x006BA100/0x14037E740
bool ClientVoidStorageFailed::Get(CDataStore* msg, bool full) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;

    return msg->IsRead() != 0;
}

// PROVEN: c4ae1714 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD480/0x14035D660
bool ClientVoidStorageFailed::Handle() {
    if (ClientVoidStorageFailed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF164.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF164.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVoidStorageFailed*)>(reinterpret_cast<uintptr_t>(ClientVoidStorageFailed::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: c1695432 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientVoidTransferResult::ClientVoidTransferResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value;

    value = 0;
    msg->Get(value);
    this->m_result = value;
}

// PROVEN: f1b430f3 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientVoidTransferResult::~ClientVoidTransferResult() {
}

// PROVEN: 5a75c871 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientVoidTransferResult::Get(CDataStore* msg, bool full) {
    uint32_t value;

    value = 0;
    msg->Get(value);
    this->m_result = value;

    return msg->IsRead() != 0;
}

// PROVEN: b540bfd5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3CC0/0x140351560
bool ClientVoidTransferResult::Handle() {
    if (ClientVoidTransferResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE088.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE088.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVoidTransferResult*)>(reinterpret_cast<uintptr_t>(ClientVoidTransferResult::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2b7f7e79 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5370/0x1403A66E0
ClientVoidStorageContents::ClientVoidStorageContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3100b230 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5980/0x1403A6EC0
ClientVoidStorageContents::~ClientVoidStorageContents() {
}

// OFFSET: 0x006DDEE0/0x14039D940
void ClientVoidStorageContents::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_items.SetCount(bits.ReadBits<8>());

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        uint8_t* guid = this->m_items.m_data[i].m_guid;
        uint8_t* creator = this->m_items.m_data[i].m_creator;
        creator[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        creator[6] = bits.ReadBit();
        creator[1] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        creator[5] = bits.ReadBit();
        creator[2] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        creator[4] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        creator[0] = bits.ReadBit();
        creator[7] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        JamVoidItem& item = this->m_items.m_data[i];
        if (item.m_creator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[3] ^= byteValue;
        }

        uint32_t value20 = 0;
        msg->Get(value20);
        item.m_propertySeed = value20;

        if (item.m_creator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[4] ^= byteValue;
        }

        uint32_t value28 = 0;
        msg->Get(value28);
        item.m_slot = value28;

        if (item.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[0] ^= byteValue;
        }

        if (item.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[6] ^= byteValue;
        }

        if (item.m_creator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[0] ^= byteValue;
        }

        if (item.m_creator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[1] ^= byteValue;
        }

        uint32_t value24 = 0;
        msg->Get(value24);
        item.m_randomPropertiesId = value24;

        if (item.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[4] ^= byteValue;
        }

        if (item.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[5] ^= byteValue;
        }

        if (item.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[2] ^= byteValue;
        }

        if (item.m_creator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[2] ^= byteValue;
        }

        if (item.m_creator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[6] ^= byteValue;
        }

        if (item.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[1] ^= byteValue;
        }

        if (item.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[3] ^= byteValue;
        }

        if (item.m_creator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[5] ^= byteValue;
        }

        if (item.m_creator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[7] ^= byteValue;
        }

        uint32_t value16 = 0;
        msg->Get(value16);
        item.m_itemID = value16;

        if (item.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[7] ^= byteValue;
        }
    }
}

// PROVEN: d5983fa3 IDENTICAL/UNPROVEN
// OFFSET: 0x006E1790/0x1403A1E90
bool ClientVoidStorageContents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E1090/0x1403A16D0
bool ClientVoidStorageContents::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<8>();

    if (msg->m_size - msg->m_read < ((16 * count + 7) >> 3) + 16 * count) {
        return false;
    }

    this->m_items.SetCount(count);

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        uint8_t* guid = this->m_items.m_data[i].m_guid;
        uint8_t* creator = this->m_items.m_data[i].m_creator;
        creator[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        creator[6] = bits.ReadBit();
        creator[1] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        creator[5] = bits.ReadBit();
        creator[2] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        creator[4] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        creator[0] = bits.ReadBit();
        creator[7] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        JamVoidItem& item = this->m_items.m_data[i];
        if (item.m_creator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[3] ^= byteValue;
        }

        uint32_t value20 = 0;
        msg->Get(value20);
        item.m_propertySeed = value20;

        if (item.m_creator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[4] ^= byteValue;
        }

        uint32_t value28 = 0;
        msg->Get(value28);
        item.m_slot = value28;

        if (item.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[0] ^= byteValue;
        }

        if (item.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[6] ^= byteValue;
        }

        if (item.m_creator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[0] ^= byteValue;
        }

        if (item.m_creator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[1] ^= byteValue;
        }

        uint32_t value24 = 0;
        msg->Get(value24);
        item.m_randomPropertiesId = value24;

        if (item.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[4] ^= byteValue;
        }

        if (item.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[5] ^= byteValue;
        }

        if (item.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[2] ^= byteValue;
        }

        if (item.m_creator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[2] ^= byteValue;
        }

        if (item.m_creator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[6] ^= byteValue;
        }

        if (item.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[1] ^= byteValue;
        }

        if (item.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[3] ^= byteValue;
        }

        if (item.m_creator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[5] ^= byteValue;
        }

        if (item.m_creator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[7] ^= byteValue;
        }

        uint32_t value16 = 0;
        msg->Get(value16);
        item.m_itemID = value16;

        if (item.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[7] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 20eb41be IDENTICAL/UNPROVEN
// OFFSET: 0x006B06A0/0x140361450
bool ClientVoidStorageContents::Handle() {
    if (ClientVoidStorageContents::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6D4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6D4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVoidStorageContents*)>(reinterpret_cast<uintptr_t>(ClientVoidStorageContents::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: f03eec51 IDENTICAL/IDENTICAL
// OFFSET: 0x006DEAD0/0x14039E610
ClientVoidStorageTransferChanges::ClientVoidStorageTransferChanges(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 22633fcc IDENTICAL/IDENTICAL
// OFFSET: 0x006DA920/0x140399040
ClientVoidStorageTransferChanges::~ClientVoidStorageTransferChanges() {
}

// OFFSET: 0x006DBBA0/0x14039AC60
void ClientVoidStorageTransferChanges::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_added.SetCount(bits.ReadBits<5>());
    this->m_removed.SetCount(bits.ReadBits<5>());

    for (uint32_t i = 0; i < this->m_added.m_count; i++) {
        uint8_t* guid = this->m_added.m_data[i].m_guid;
        uint8_t* creator = this->m_added.m_data[i].m_creator;
        creator[5] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        creator[7] = bits.ReadBit();
        creator[6] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        creator[4] = bits.ReadBit();
        creator[2] = bits.ReadBit();
        creator[0] = bits.ReadBit();
        creator[3] = bits.ReadBit();
        creator[1] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_removed.m_count; i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_removed.m_data[i]);
        guid[1] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[0] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_removed.m_count; i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_removed.m_data[i]);

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->m_added.m_count; i++) {
        JamVoidItem& item = this->m_added.m_data[i];
        uint32_t value20 = 0;
        msg->Get(value20);
        item.m_propertySeed = value20;

        if (item.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[6] ^= byteValue;
        }

        if (item.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[4] ^= byteValue;
        }

        if (item.m_creator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[4] ^= byteValue;
        }

        if (item.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[2] ^= byteValue;
        }

        if (item.m_creator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[1] ^= byteValue;
        }

        if (item.m_creator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[3] ^= byteValue;
        }

        if (item.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[3] ^= byteValue;
        }

        if (item.m_creator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[0] ^= byteValue;
        }

        if (item.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[0] ^= byteValue;
        }

        if (item.m_creator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[6] ^= byteValue;
        }

        if (item.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[5] ^= byteValue;
        }

        if (item.m_creator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[5] ^= byteValue;
        }

        if (item.m_creator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[7] ^= byteValue;
        }

        uint32_t value16 = 0;
        msg->Get(value16);
        item.m_itemID = value16;

        if (item.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[1] ^= byteValue;
        }

        uint32_t value28 = 0;
        msg->Get(value28);
        item.m_slot = value28;

        if (item.m_creator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[2] ^= byteValue;
        }

        if (item.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[7] ^= byteValue;
        }

        uint32_t value24 = 0;
        msg->Get(value24);
        item.m_randomPropertiesId = value24;
    }
}

// PROVEN: 8fc24b1b IDENTICAL/UNPROVEN
// OFFSET: 0x006EE950/0x1403B3700
bool ClientVoidStorageTransferChanges::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E49B0/0x1403A5C10
bool ClientVoidStorageTransferChanges::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count2 = bits.ReadBits<5>();

    if (msg->m_size - msg->m_read < ((16 * count2 + 7) >> 3) + 16 * count2) {
        return false;
    }

    this->m_added.SetCount(count2);

    uint32_t count = bits.ReadBits<5>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3)) {
        return false;
    }

    this->m_removed.SetCount(count);

    for (uint32_t i = 0; i < this->m_added.m_count; i++) {
        uint8_t* guid = this->m_added.m_data[i].m_guid;
        uint8_t* creator = this->m_added.m_data[i].m_creator;
        creator[5] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        creator[7] = bits.ReadBit();
        creator[6] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        creator[4] = bits.ReadBit();
        creator[2] = bits.ReadBit();
        creator[0] = bits.ReadBit();
        creator[3] = bits.ReadBit();
        creator[1] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[0] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        guid[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_removed.m_count; i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_removed.m_data[i]);
        guid[1] = bits.ReadBit();
        guid[7] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[5] = bits.ReadBit();
        guid[6] = bits.ReadBit();
        guid[2] = bits.ReadBit();
        guid[4] = bits.ReadBit();
        guid[0] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_removed.m_count; i++) {
        uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_removed.m_data[i]);

        if (guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[3] ^= byteValue;
        }

        if (guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[1] ^= byteValue;
        }

        if (guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[0] ^= byteValue;
        }

        if (guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[2] ^= byteValue;
        }

        if (guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[7] ^= byteValue;
        }

        if (guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[5] ^= byteValue;
        }

        if (guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[6] ^= byteValue;
        }

        if (guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[4] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->m_added.m_count; i++) {
        JamVoidItem& item = this->m_added.m_data[i];
        uint32_t value20 = 0;
        msg->Get(value20);
        item.m_propertySeed = value20;

        if (item.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[6] ^= byteValue;
        }

        if (item.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[4] ^= byteValue;
        }

        if (item.m_creator[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[4] ^= byteValue;
        }

        if (item.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[2] ^= byteValue;
        }

        if (item.m_creator[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[1] ^= byteValue;
        }

        if (item.m_creator[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[3] ^= byteValue;
        }

        if (item.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[3] ^= byteValue;
        }

        if (item.m_creator[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[0] ^= byteValue;
        }

        if (item.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[0] ^= byteValue;
        }

        if (item.m_creator[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[6] ^= byteValue;
        }

        if (item.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[5] ^= byteValue;
        }

        if (item.m_creator[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[5] ^= byteValue;
        }

        if (item.m_creator[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[7] ^= byteValue;
        }

        uint32_t value16 = 0;
        msg->Get(value16);
        item.m_itemID = value16;

        if (item.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[1] ^= byteValue;
        }

        uint32_t value28 = 0;
        msg->Get(value28);
        item.m_slot = value28;

        if (item.m_creator[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_creator[2] ^= byteValue;
        }

        if (item.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            item.m_guid[7] ^= byteValue;
        }

        uint32_t value24 = 0;
        msg->Get(value24);
        item.m_randomPropertiesId = value24;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 1ad4bba2 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6DB0/0x1403556D0
bool ClientVoidStorageTransferChanges::Handle() {
    if (ClientVoidStorageTransferChanges::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD540.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD540.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVoidStorageTransferChanges*)>(reinterpret_cast<uintptr_t>(ClientVoidStorageTransferChanges::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 5c83e47a IDENTICAL/IDENTICAL
// OFFSET: 0x006C9940/0x140381380
ClientVoidItemSwapResponse::ClientVoidItemSwapResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5bbcb742 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2E70/0x1403503A0
ClientVoidItemSwapResponse::~ClientVoidItemSwapResponse() {
}

// PROVEN: 7df61317 DIFFERENT/IDENTICAL
// OFFSET: 0x006B8470/0x14036B600
void ClientVoidItemSwapResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* second = reinterpret_cast<uint8_t*>(&this->m_secondItem);
    uint8_t* first = reinterpret_cast<uint8_t*>(&this->m_firstItem);
    this->m_secondItem = !bits.ReadBit();
    this->m_firstItem = !bits.ReadBit();

    if (this->m_firstItem) {
        first[5] = bits.ReadBit();
        first[2] = bits.ReadBit();
        first[1] = bits.ReadBit();
        first[4] = bits.ReadBit();
        first[0] = bits.ReadBit();
        first[6] = bits.ReadBit();
        first[7] = bits.ReadBit();
        first[3] = bits.ReadBit();
    }

    this->m_secondSlot = !bits.ReadBit();

    if (this->m_secondItem) {
        second[7] = bits.ReadBit();
        second[3] = bits.ReadBit();
        second[4] = bits.ReadBit();
        second[0] = bits.ReadBit();
        second[5] = bits.ReadBit();
        second[1] = bits.ReadBit();
        second[2] = bits.ReadBit();
        second[6] = bits.ReadBit();
    }

    this->m_firstSlot = !bits.ReadBit();

    if (this->m_secondItem) {
        if (second[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[4] ^= byteValue;
        }

        if (second[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[6] ^= byteValue;
        }

        if (second[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[5] ^= byteValue;
        }

        if (second[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[2] ^= byteValue;
        }

        if (second[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[3] ^= byteValue;
        }

        if (second[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[1] ^= byteValue;
        }

        if (second[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[7] ^= byteValue;
        }

        if (second[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[0] ^= byteValue;
        }
    }

    if (this->m_firstItem) {
        if (first[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[6] ^= byteValue;
        }

        if (first[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[3] ^= byteValue;
        }

        if (first[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[5] ^= byteValue;
        }

        if (first[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[0] ^= byteValue;
        }

        if (first[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[1] ^= byteValue;
        }

        if (first[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[2] ^= byteValue;
        }

        if (first[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[4] ^= byteValue;
        }

        if (first[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[7] ^= byteValue;
        }
    }

    if (this->m_secondSlot) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_secondSlot = value;
    }

    if (this->m_firstSlot) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_firstSlot = value;
    }
}

// PROVEN: 2d63d96f IDENTICAL/UNPROVEN
// OFFSET: 0x006B5B90/0x1403680C0
bool ClientVoidItemSwapResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a56c1ef1 DIFFERENT/IDENTICAL
// OFFSET: 0x006A6E90/0x140355800
bool ClientVoidItemSwapResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* second = reinterpret_cast<uint8_t*>(&this->m_secondItem);
    uint8_t* first = reinterpret_cast<uint8_t*>(&this->m_firstItem);
    this->m_secondItem = !bits.ReadBit();
    this->m_firstItem = !bits.ReadBit();

    if (this->m_firstItem) {
        first[5] = bits.ReadBit();
        first[2] = bits.ReadBit();
        first[1] = bits.ReadBit();
        first[4] = bits.ReadBit();
        first[0] = bits.ReadBit();
        first[6] = bits.ReadBit();
        first[7] = bits.ReadBit();
        first[3] = bits.ReadBit();
    }

    this->m_secondSlot = !bits.ReadBit();

    if (this->m_secondItem) {
        second[7] = bits.ReadBit();
        second[3] = bits.ReadBit();
        second[4] = bits.ReadBit();
        second[0] = bits.ReadBit();
        second[5] = bits.ReadBit();
        second[1] = bits.ReadBit();
        second[2] = bits.ReadBit();
        second[6] = bits.ReadBit();
    }

    this->m_firstSlot = !bits.ReadBit();

    if (this->m_secondItem) {
        if (second[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[4] ^= byteValue;
        }

        if (second[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[6] ^= byteValue;
        }

        if (second[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[5] ^= byteValue;
        }

        if (second[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[2] ^= byteValue;
        }

        if (second[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[3] ^= byteValue;
        }

        if (second[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[1] ^= byteValue;
        }

        if (second[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[7] ^= byteValue;
        }

        if (second[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            second[0] ^= byteValue;
        }
    }

    if (this->m_firstItem) {
        if (first[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[6] ^= byteValue;
        }

        if (first[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[3] ^= byteValue;
        }

        if (first[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[5] ^= byteValue;
        }

        if (first[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[0] ^= byteValue;
        }

        if (first[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[1] ^= byteValue;
        }

        if (first[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[2] ^= byteValue;
        }

        if (first[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[4] ^= byteValue;
        }

        if (first[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            first[7] ^= byteValue;
        }
    }

    if (this->m_secondSlot) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_secondSlot = value;
    }

    if (this->m_firstSlot) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_firstSlot = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 66c183fc IDENTICAL/UNPROVEN
// OFFSET: 0x006A5170/0x1403530D0
bool ClientVoidItemSwapResponse::Handle() {
    if (ClientVoidItemSwapResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE448.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE448.m_left->Get();
        return reinterpret_cast<bool (*)(ClientVoidItemSwapResponse*)>(reinterpret_cast<uintptr_t>(ClientVoidItemSwapResponse::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006A9D70/0x1403592A0
void LFGuildBrowseData::operator=(const LFGuildBrowseData& source) {
    this->m_numMembers = source.m_numMembers;
    memcpy(this->m_guildGUID, source.m_guildGUID, sizeof(this->m_guildGUID));
    this->m_membershipRequested = source.m_membershipRequested;
    this->m_cached = source.m_cached;
    this->unk_008C = source.unk_008C;
    this->m_emblemStyle = source.m_emblemStyle;
    this->m_achievementPoints = source.m_achievementPoints;
    this->m_background = source.m_background;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->m_classRoles = source.m_classRoles;
    this->m_availability = source.m_availability;
    this->m_interests = source.m_interests;
    this->m_guildLevel = source.m_guildLevel;
    this->m_emblemColor = source.m_emblemColor;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    this->unk_0080 = source.unk_0080;
    this->m_borderColor = source.m_borderColor;
}

// OFFSET: 0x006AA920
void LFGuildRecruitData::operator=(const LFGuildRecruitData& source) {
    this->m_classID = source.m_classID;
    this->m_timeLeft = source.m_timeLeft;
    this->m_level = source.m_level;
    this->unk_043C = source.unk_043C;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    this->m_classRoles = source.m_classRoles;
    this->m_timeSince = source.m_timeSince;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->m_interests = source.m_interests;
    memcpy(this->m_recruitGUID, source.m_recruitGUID, sizeof(this->m_recruitGUID));
    this->m_availability = source.m_availability;
}

// OFFSET: 0x006B5650
void LFGuildApplicationData::operator=(const LFGuildApplicationData& source) {
    this->m_availability = source.m_availability;
    this->m_timeLeft = source.m_timeLeft;
    this->m_timeSince = source.m_timeSince;
    this->m_interests = source.m_interests;
    this->m_classRoles = source.m_classRoles;
    memcpy(this->m_guildGUID, source.m_guildGUID, sizeof(this->m_guildGUID));
    SStrCopy(this->m_guildName, source.m_guildName, sizeof(this->m_guildName));
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
}

// OFFSET: 0x006E2520/0x14039A7F0
void ClientLFGBlackList::operator=(const ClientLFGBlackList& source) {
    this->unk_0008.SetCount(source.unk_0008.m_count);

    for (uint32_t i = 0; i < this->unk_0008.Count(); i++) {
        this->unk_0008[i] = source.unk_0008.m_data[i];
    }

    memcpy(this->unk_0000, source.unk_0000, sizeof(this->unk_0000));
}

// OFFSET: 0x006DD150/0x14039C530
void JamGuildBankItemInfo::operator=(const JamGuildBankItemInfo& source) {
    this->m_reforgeID = source.m_reforgeID;
    this->m_slot = source.m_slot;
    this->m_enchantmentID = source.m_enchantmentID;
    this->m_socketEnchant.SetCount(source.m_socketEnchant.m_count);
    this->m_randomPropertiesID = source.m_randomPropertiesID;
    this->m_locked = source.m_locked;
    this->m_itemID = source.m_itemID;
    this->m_onUseEnchantmentID = source.m_onUseEnchantmentID;
    this->m_charges = source.m_charges;
    this->m_flags = source.m_flags;

    for (uint32_t i = 0; i < this->m_socketEnchant.Count(); i++) {
        this->m_socketEnchant[i] = source.m_socketEnchant.m_data[i];
    }

    this->m_randomPropertiesSeed = source.m_randomPropertiesSeed;
    this->m_count = source.m_count;
}

// OFFSET: 0x006DAE70
void JamQuestCompletionNPC::operator=(const JamQuestCompletionNPC& source) {
    this->m_npcs.SetCount(source.m_npcs.m_count);
    this->m_questID = source.m_questID;

    for (uint32_t i = 0; i < this->m_npcs.Count(); i++) {
        this->m_npcs[i] = source.m_npcs.m_data[i];
    }
}

// OFFSET: 0x006D9FE0/0x1403985E0
void ClientPVPLogData_Player::operator=(const ClientPVPLogData_Player& source) {
    this->m_stats.SetCount(source.m_stats.m_count);
    this->unk_000D = source.unk_000D;
    this->m_primaryTalentTree = source.m_primaryTalentTree;
    this->m_hasRatingChange = source.m_hasRatingChange;
    this->m_healingDone = source.m_healingDone;
    this->m_hasHonor = source.m_hasHonor;
    memcpy(this->m_playerGUID, source.m_playerGUID, sizeof(this->m_playerGUID));

    if (this->m_hasHonor) {
        this->m_deaths = source.m_deaths;
        this->m_honorKills = source.m_honorKills;
        this->m_contributionPoints = source.m_contributionPoints;
    }

    this->m_damageDone = source.m_damageDone;
    this->m_kills = source.m_kills;
    this->m_faction = source.m_faction;

    if (this->m_hasRatingChange) {
        this->m_ratingChange = source.m_ratingChange;
    }

    this->m_hasPreMatchMMR = source.m_hasPreMatchMMR;

    if (this->m_hasPreMatchMMR) {
        this->m_preMatchMMR = source.m_preMatchMMR;
    }

    for (uint32_t i = 0; i < this->m_stats.Count(); i++) {
        this->m_stats[i] = source.m_stats.m_data[i];
    }

    this->m_hasMmrChange = source.m_hasMmrChange;
    this->m_hasRating = source.m_hasRating;

    if (this->m_hasRating) {
        this->m_rating = source.m_rating;
    }

    if (this->m_hasMmrChange) {
        this->m_mmrChange = source.m_mmrChange;
    }
}

// PROVEN: 4a439661 IDENTICAL/NONE
// OFFSET: 0x006C1780
void JamGuildBankLogEntry::operator=(const JamGuildBankLogEntry& source) {
    this->m_entryType = source.m_entryType;
    this->m_timeOffset = source.m_timeOffset;
    this->m_hasItemID = source.m_hasItemID;
    memcpy(this->m_playerGUID, source.m_playerGUID, sizeof(this->m_playerGUID));

    if (this->m_hasItemID) {
        this->m_itemID = source.m_itemID;
    }

    this->m_hasMoney = source.m_hasMoney;
    this->m_hasCount = source.m_hasCount;

    if (this->m_hasCount) {
        this->m_count = source.m_count;
    }

    if (this->m_hasMoney) {
        this->m_money = source.m_money;
    }

    this->m_hasOtherTab = source.m_hasOtherTab;

    if (this->m_hasOtherTab) {
        this->m_otherTab = source.m_otherTab;
    }
}

// PROVEN: 0589188c IDENTICAL/DIFFERENT
// OFFSET: 0x00697020/0x140342620
void JamCliCUFProfile::operator=(const JamCliCUFProfile& source) {
    this->m_autoActivate10Players = source.m_autoActivate10Players;
    this->m_useClassColors = source.m_useClassColors;
    this->m_displayPowerBar = source.m_displayPowerBar;
    this->m_autoActivatePvE = source.m_autoActivatePvE;
    this->m_healthText = source.m_healthText;
    this->m_frameWidth = source.m_frameWidth;
    this->m_sortBy = source.m_sortBy;
    this->m_leftPoint = source.m_leftPoint;
    this->m_shown = source.m_shown;
    this->m_frameHeight = source.m_frameHeight;
    this->m_displayAggroHighlight = source.m_displayAggroHighlight;
    this->m_displayMainTankAndAssist = source.m_displayMainTankAndAssist;
    this->m_autoActivate5Players = source.m_autoActivate5Players;
    this->m_autoActivate40Players = source.m_autoActivate40Players;
    this->m_autoActivateSpec2 = source.m_autoActivateSpec2;
    this->m_autoActivate3Players = source.m_autoActivate3Players;
    this->m_autoActivate2Players = source.m_autoActivate2Players;
    this->m_bottomOffset = source.m_bottomOffset;
    this->m_autoActivate15Players = source.m_autoActivate15Players;
    this->m_displayPets = source.m_displayPets;
    this->m_displayHealPrediction = source.m_displayHealPrediction;
    this->m_autoActivatePvP = source.m_autoActivatePvP;
    this->m_locked = source.m_locked;
    this->m_dynamicPosition = source.m_dynamicPosition;
    this->m_autoActivateSpec1 = source.m_autoActivateSpec1;
    this->m_displayNonBossDebuffs = source.m_displayNonBossDebuffs;
    this->m_displayOnlyDispellableDebuffs = source.m_displayOnlyDispellableDebuffs;
    this->m_horizontalGroups = source.m_horizontalGroups;
    this->m_bottomPoint = source.m_bottomPoint;
    this->m_displayBorder = source.m_displayBorder;
    this->m_autoActivate25Players = source.m_autoActivate25Players;
    this->m_keepGroupsTogether = source.m_keepGroupsTogether;
    this->m_topOffset = source.m_topOffset;
    this->m_topPoint = source.m_topPoint;
    this->m_leftOffset = source.m_leftOffset;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    this->m_fieldMask = source.m_fieldMask;
    this->m_fieldMask2 = source.m_fieldMask2;
}

// OFFSET: 0x006ABD40/0x14035BA00
void JamCliGuildRankData::operator=(const JamCliGuildRankData& source) {
    this->m_withdrawGoldLimit = source.m_withdrawGoldLimit;
    this->m_rankOrder = source.m_rankOrder;
    this->m_rankID = source.m_rankID;
    this->m_flags = source.m_flags;
    SStrCopy(this->m_rankName, source.m_rankName, sizeof(this->m_rankName));
    this->m_tabWithdrawItemLimit[0] = source.m_tabWithdrawItemLimit[0];
    this->m_tabFlags[0] = source.m_tabFlags[0];
    this->m_tabWithdrawItemLimit[1] = source.m_tabWithdrawItemLimit[1];
    this->m_tabFlags[1] = source.m_tabFlags[1];
    this->m_tabWithdrawItemLimit[2] = source.m_tabWithdrawItemLimit[2];
    this->m_tabFlags[2] = source.m_tabFlags[2];
    this->m_tabWithdrawItemLimit[3] = source.m_tabWithdrawItemLimit[3];
    this->m_tabFlags[3] = source.m_tabFlags[3];
    this->m_tabWithdrawItemLimit[4] = source.m_tabWithdrawItemLimit[4];
    this->m_tabFlags[4] = source.m_tabFlags[4];
    this->m_tabWithdrawItemLimit[5] = source.m_tabWithdrawItemLimit[5];
    this->m_tabFlags[5] = source.m_tabFlags[5];
    this->m_tabWithdrawItemLimit[6] = source.m_tabWithdrawItemLimit[6];
    this->m_tabFlags[6] = source.m_tabFlags[6];
    this->m_tabWithdrawItemLimit[7] = source.m_tabWithdrawItemLimit[7];
    this->m_tabFlags[7] = source.m_tabFlags[7];
}

// PROVEN: e8ff7f19 IDENTICAL/NONE
// OFFSET: 0x006AE1B0
void JamClientCommentatorPartyMember::operator=(const JamClientCommentatorPartyMember& source) {
    this->unk_0048 = source.unk_0048;
    this->unk_0044 = source.unk_0044;
    this->unk_0049 = source.unk_0049;
    memcpy(this->unk_0030, source.unk_0030, sizeof(this->unk_0030));
    this->unk_0040 = source.unk_0040;
    SStrCopy(this->unk_0000, source.unk_0000, sizeof(this->unk_0000));
    this->unk_003C = source.unk_003C;
    this->unk_0038 = source.unk_0038;
}

// OFFSET: 0x006B0560
void JamGuildBankTabInfo::operator=(const JamGuildBankTabInfo& source) {
    this->m_tabIndex = source.m_tabIndex;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    SStrCopy(this->m_icon, source.m_icon, sizeof(this->m_icon));
}

// OFFSET: 0x006B4820
void JamCliServerInfoLine::operator=(const JamCliServerInfoLine& source) {
    SStrCopy(this->m_value, source.m_value, sizeof(this->m_value));
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
}

// OFFSET: 0x006B55B0/0x140367940
void JamLfgSearchResultParty::operator=(const JamLfgSearchResultParty& source) {
    memcpy(this->m_guid, source.m_guid, sizeof(this->m_guid));
    memcpy(this->m_leaderGUID, source.m_leaderGUID, sizeof(this->m_leaderGUID));
    this->unk_0008 = source.unk_0008;
    this->unk_0118 = source.unk_0118;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->unk_011C[0] = source.unk_011C[0];
    this->unk_011C[1] = source.unk_011C[1];
    this->unk_011C[2] = source.unk_011C[2];
}

// OFFSET: 0x006B8C40
void ClientGuildRosterMemberData::operator=(const ClientGuildRosterMemberData& source) {
    this->m_guildRepToCap = source.m_guildRepToCap;
    this->m_guildReputation = source.m_guildReputation;
    this->m_personalAchievementPoints = source.m_personalAchievementPoints;
    this->m_status = source.m_status;
    this->m_sorEligible = source.m_sorEligible;
    this->m_classID = source.m_classID;
    this->m_weeklyXP = source.m_weeklyXP;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    this->m_rankID = source.m_rankID;
    this->m_areaID = source.m_areaID;
    this->m_authenticated = source.m_authenticated;
    this->m_bgRating = source.m_bgRating;
    this->m_lastSave = source.m_lastSave;
    this->m_level = source.m_level;
    SStrCopy(this->m_officerNote, source.m_officerNote, sizeof(this->m_officerNote));
    SStrCopy(this->m_note, source.m_note, sizeof(this->m_note));
    this->m_totalXP = source.m_totalXP;
    memcpy(this->m_guid, source.m_guid, sizeof(this->m_guid));
    this->m_gender = source.m_gender;
    this->m_profession[0].m_step = source.m_profession[0].m_step;
    this->m_profession[0].m_dbID = source.m_profession[0].m_dbID;
    this->m_profession[0].m_rank = source.m_profession[0].m_rank;
    this->m_profession[1].m_step = source.m_profession[1].m_step;
    this->m_profession[1].m_dbID = source.m_profession[1].m_dbID;
    this->m_profession[1].m_rank = source.m_profession[1].m_rank;
}

// OFFSET: 0x006C0070/0x140375310
void JamClientCharacterListEntry::operator=(const JamClientCharacterListEntry& source) {
    memcpy(this->m_guid, source.m_guid, sizeof(this->m_guid));
    this->m_petExperienceLevel = source.m_petExperienceLevel;
    this->m_hairStyleID = source.m_hairStyleID;
    this->m_flags = source.m_flags;
    this->m_experienceLevel = source.m_experienceLevel;
    this->m_zoneID = source.m_zoneID;
    this->m_sexID = source.m_sexID;
    this->m_customizeFlags = source.m_customizeFlags;
    this->m_hairColorID = source.m_hairColorID;
    memcpy(this->m_guildGUID, source.m_guildGUID, sizeof(this->m_guildGUID));
    this->m_mapID = source.m_mapID;
    this->m_raceID = source.m_raceID;
    this->m_facialHairStyleID = source.m_facialHairStyleID;
    this->m_petCreatureFamilyID = source.m_petCreatureFamilyID;
    this->m_listOrder = source.m_listOrder;
    this->m_petDisplayInfoID = source.m_petDisplayInfoID;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    this->m_classID = source.m_classID;
    this->m_position.x = source.m_position.x;
    this->m_position.y = source.m_position.y;
    this->m_position.z = source.m_position.z;
    this->m_firstLogin = source.m_firstLogin;
    this->m_skinColorID = source.m_skinColorID;
    this->m_faceID = source.m_faceID;

    for (uint32_t i = 0; i < 23; i++) {
        this->m_items[i].m_auraVisualID = source.m_items[i].m_auraVisualID;
        this->m_items[i].m_displayID = source.m_items[i].m_displayID;
        this->m_items[i].m_inventoryType = source.m_items[i].m_inventoryType;
    }
}

// OFFSET: 0x006ABA80/0x14035B770
void JamLfgSearchResultPlayer::operator=(const JamLfgSearchResultPlayer& source) {
    this->unk_000E = source.unk_000E;
    this->unk_0044 = source.unk_0044;
    this->unk_0038 = source.unk_0038;
    this->unk_001C = source.unk_001C;
    this->unk_0040 = source.unk_0040;
    this->unk_0058 = source.unk_0058;
    this->unk_0030 = source.unk_0030;
    this->unk_0034 = source.unk_0034;
    this->unk_0164 = source.unk_0164;
    this->unk_000C = source.unk_000C;
    memcpy(this->m_partyGUID, source.m_partyGUID, sizeof(this->m_partyGUID));
    this->unk_003C = source.unk_003C;
    this->unk_004C = source.unk_004C;
    this->unk_000D = source.unk_000D;
    this->unk_0174 = source.unk_0174;
    this->unk_0010 = source.unk_0010;
    this->unk_0020 = source.unk_0020;
    this->unk_002C = source.unk_002C;
    this->unk_0048 = source.unk_0048;
    this->unk_0178 = source.unk_0178;
    memcpy(this->m_guildGUID, source.m_guildGUID, sizeof(this->m_guildGUID));
    memcpy(this->m_guid, source.m_guid, sizeof(this->m_guid));
    this->unk_0008 = source.unk_0008;
    this->unk_0054 = source.unk_0054;
    this->unk_0060 = source.unk_0060;
    this->unk_0050 = source.unk_0050;
    this->unk_0188 = source.unk_0188;
    this->unk_005C = source.unk_005C;
    this->unk_0170 = source.unk_0170;
    this->unk_0028 = source.unk_0028;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->unk_0014 = source.unk_0014;
    this->unk_0024 = source.unk_0024;
    this->unk_0018 = source.unk_0018;
    this->unk_018C = source.unk_018C;
    this->unk_018D = source.unk_018D;
    this->unk_018E = source.unk_018E;
}

// OFFSET: 0x006D7340/0x140394FF0
ClientLfGuildPostUpdated::ClientLfGuildPostUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasPost = false;
    this->Unpack(msg, true);
}

// PROVEN: 0b51fc9e IDENTICAL/IDENTICAL
// OFFSET: 0x006D4FB0/0x140391DF0
ClientLfGuildPostUpdated::~ClientLfGuildPostUpdated() {
}

// PROVEN: 28451b2b IDENTICAL/DIFFERENT
// OFFSET: 0x006D1F00/0x14038D4C0
void ClientLfGuildPostUpdated::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasPost = bits.ReadBit();

    if (this->m_hasPost) {
        this->m_post.m_listed = bits.ReadBit();
        uint32_t length = bits.ReadBits<11>();
        *reinterpret_cast<uint32_t*>(this->m_post.m_settings.m_comment) = length;
    }

    if (this->m_hasPost) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_level = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->m_post.m_settings.m_comment);
            msg->GetData(this->m_post.m_settings.m_comment, length);
            this->m_post.m_settings.m_comment[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.unk_0010 = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_classRoles = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_interests = value;
        }
    }
}

// PROVEN: daf0929d IDENTICAL/UNPROVEN
// OFFSET: 0x006CAB70/0x140382CF0
bool ClientLfGuildPostUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f945ca5a IDENTICAL/DIFFERENT
// OFFSET: 0x006C5D80/0x14037C0F0
bool ClientLfGuildPostUpdated::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_hasPost = bits.ReadBit();

    if (this->m_hasPost) {
        this->m_post.m_listed = bits.ReadBit();
        uint32_t length = bits.ReadBits<11>();
        *reinterpret_cast<uint32_t*>(this->m_post.m_settings.m_comment) = length;

        if (length >= sizeof(this->m_post.m_settings.m_comment)) {
            return false;
        }

    }

    if (this->m_hasPost) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_level = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->m_post.m_settings.m_comment);
            msg->GetData(this->m_post.m_settings.m_comment, length);
            this->m_post.m_settings.m_comment[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.unk_0010 = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_classRoles = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            this->m_post.m_settings.m_interests = value;
        }
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 2293ef74 IDENTICAL/NONE
// OFFSET: 0x006C4010
bool ClientLfGuildPostUpdated::IsValid() const {
    if (!this->m_hasPost) {
        return true;
    }

    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_post.m_settings.m_comment))) {
            break;
        }

        terminated |= this->m_post.m_settings.m_comment[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}

// PROVEN: ac3581d7 IDENTICAL/UNPROVEN
// OFFSET: 0x006BED20/0x140373C10
bool ClientLfGuildPostUpdated::Handle() {
    if (ClientLfGuildPostUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD344.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD344.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildPostUpdated*)>(reinterpret_cast<uintptr_t>(ClientLfGuildPostUpdated::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 9eb3933e IDENTICAL/IDENTICAL
// OFFSET: 0x006F07A0/0x1403B6160
ClientLfGuildBrowseUpdated::ClientLfGuildBrowseUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e1a491aa IDENTICAL/IDENTICAL
// OFFSET: 0x006E57F0/0x1403A6CF0
ClientLfGuildBrowseUpdated::~ClientLfGuildBrowseUpdated() {
}

// OFFSET: 0x006E8120/0x1403AA800
void ClientLfGuildBrowseUpdated::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_guilds.SetCount(bits.ReadBits<19>());

    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        LFGuildBrowseData& guild = this->m_guilds.m_data[i];
        guild.m_guildGUID[7] = bits.ReadBit();
        guild.m_guildGUID[5] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(guild.m_name) = bits.ReadBits<8>();
        guild.m_guildGUID[0] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(guild.m_comment) = bits.ReadBits<11>();
        guild.m_guildGUID[4] = bits.ReadBit();
        guild.m_guildGUID[1] = bits.ReadBit();
        guild.m_guildGUID[2] = bits.ReadBit();
        guild.m_guildGUID[6] = bits.ReadBit();
        guild.m_guildGUID[3] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        LFGuildBrowseData& guild = this->m_guilds.m_data[i];
        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_emblemColor = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.unk_008C = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_emblemStyle = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(guild.m_comment);
            msg->GetData(guild.m_comment, length);
            guild.m_comment[length] = 0;
        }

        {
            uint8_t value = 0;
            msg->Get(value);
            guild.m_cached = value;
        }

        if (guild.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_interests = value;
        }

        if (guild.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[6] ^= byteValue;
        }

        if (guild.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[4] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_guildLevel = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(guild.m_name);
            msg->GetData(guild.m_name, length);
            guild.m_name[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_achievementPoints = value;
        }

        if (guild.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[7] ^= byteValue;
        }

        {
            uint8_t value = 0;
            msg->Get(value);
            guild.m_membershipRequested = value;
        }

        if (guild.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[2] ^= byteValue;
        }

        if (guild.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[0] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_availability = value;
        }

        if (guild.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_background = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.unk_0080 = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_borderColor = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_classRoles = value;
        }

        if (guild.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[3] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_numMembers = value;
        }
    }
}

// PROVEN: db662a42 IDENTICAL/UNPROVEN
// OFFSET: 0x006F42C0/0x1403BB680
bool ClientLfGuildBrowseUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EF090/0x1403B41A0
bool ClientLfGuildBrowseUpdated::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<19>();

    if (msg->m_size - msg->m_read < ((27 * count + 7) >> 3) + 50 * count) {
        return false;
    }

    this->m_guilds.SetCount(count);

    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        LFGuildBrowseData& guild = this->m_guilds.m_data[i];
        guild.m_guildGUID[7] = bits.ReadBit();
        guild.m_guildGUID[5] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(guild.m_name) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(guild.m_name) >= sizeof(guild.m_name)) {
            return false;
        }

        guild.m_guildGUID[0] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(guild.m_comment) = bits.ReadBits<11>();

        if (*reinterpret_cast<uint32_t*>(guild.m_comment) >= sizeof(guild.m_comment)) {
            return false;
        }

        guild.m_guildGUID[4] = bits.ReadBit();
        guild.m_guildGUID[1] = bits.ReadBit();
        guild.m_guildGUID[2] = bits.ReadBit();
        guild.m_guildGUID[6] = bits.ReadBit();
        guild.m_guildGUID[3] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        LFGuildBrowseData& guild = this->m_guilds.m_data[i];
        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_emblemColor = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.unk_008C = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_emblemStyle = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(guild.m_comment);
            msg->GetData(guild.m_comment, length);
            guild.m_comment[length] = 0;
        }

        {
            uint8_t value = 0;
            msg->Get(value);
            guild.m_cached = value;
        }

        if (guild.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_interests = value;
        }

        if (guild.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[6] ^= byteValue;
        }

        if (guild.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[4] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_guildLevel = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(guild.m_name);
            msg->GetData(guild.m_name, length);
            guild.m_name[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_achievementPoints = value;
        }

        if (guild.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[7] ^= byteValue;
        }

        {
            uint8_t value = 0;
            msg->Get(value);
            guild.m_membershipRequested = value;
        }

        if (guild.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[2] ^= byteValue;
        }

        if (guild.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[0] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_availability = value;
        }

        if (guild.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_background = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.unk_0080 = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_borderColor = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_classRoles = value;
        }

        if (guild.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guild.m_guildGUID[3] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            guild.m_numMembers = value;
        }
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 89b84fa2 IDENTICAL/NONE
// OFFSET: 0x006A43D0
bool ClientLfGuildBrowseUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        if (!this->m_guilds.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: a66b4b20 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD100/0x14035D1B0
bool ClientLfGuildBrowseUpdated::Handle() {
    if (ClientLfGuildBrowseUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF49C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF49C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildBrowseUpdated*)>(reinterpret_cast<uintptr_t>(ClientLfGuildBrowseUpdated::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: fe17b545 IDENTICAL/IDENTICAL
// OFFSET: 0x006F3670/0x1403BA390
ClientLfGuildRecruitListUpdated::ClientLfGuildRecruitListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ee7d117f IDENTICAL/IDENTICAL
// OFFSET: 0x006E76A0/0x1403A98C0
ClientLfGuildRecruitListUpdated::~ClientLfGuildRecruitListUpdated() {
}

// OFFSET: 0x006ECE50/0x1403B1280
void ClientLfGuildRecruitListUpdated::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_recruits.SetCount(bits.ReadBits<20>());

    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        LFGuildRecruitData& recruit = this->m_recruits.m_data[i];
        *reinterpret_cast<uint32_t*>(recruit.m_comment) = bits.ReadBits<11>();
        recruit.m_recruitGUID[2] = bits.ReadBit();
        recruit.m_recruitGUID[4] = bits.ReadBit();
        recruit.m_recruitGUID[3] = bits.ReadBit();
        recruit.m_recruitGUID[7] = bits.ReadBit();
        recruit.m_recruitGUID[0] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(recruit.m_name) = bits.ReadBits<7>();
        recruit.m_recruitGUID[5] = bits.ReadBit();
        recruit.m_recruitGUID[1] = bits.ReadBit();
        recruit.m_recruitGUID[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        LFGuildRecruitData& recruit = this->m_recruits.m_data[i];
        if (recruit.m_recruitGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[4] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.unk_043C = value;
        }

        if (recruit.m_recruitGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[3] ^= byteValue;
        }

        if (recruit.m_recruitGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[0] ^= byteValue;
        }

        if (recruit.m_recruitGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_level = value;
        }

        if (recruit.m_recruitGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[6] ^= byteValue;
        }

        if (recruit.m_recruitGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[7] ^= byteValue;
        }

        if (recruit.m_recruitGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[2] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_timeSince = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_classRoles = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_interests = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_timeLeft = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(recruit.m_name);
            msg->GetData(recruit.m_name, length);
            recruit.m_name[length] = 0;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(recruit.m_comment);
            msg->GetData(recruit.m_comment, length);
            recruit.m_comment[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_classID = value;
        }

        if (recruit.m_recruitGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[5] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_updateTime = value;
    }
}

// PROVEN: 95799ed1 IDENTICAL/UNPROVEN
// OFFSET: 0x006ED350/0x1403B1970
bool ClientLfGuildRecruitListUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E76D0/0x1403A9900
bool ClientLfGuildRecruitListUpdated::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<20>();

    if (msg->m_size - msg->m_read < ((26 * count + 7) >> 3) + 32 * count) {
        return false;
    }

    this->m_recruits.SetCount(count);

    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        LFGuildRecruitData& recruit = this->m_recruits.m_data[i];

        *reinterpret_cast<uint32_t*>(recruit.m_comment) = bits.ReadBits<11>();

        if (*reinterpret_cast<uint32_t*>(recruit.m_comment) >= sizeof(recruit.m_comment)) {
            return false;
        }

        recruit.m_recruitGUID[2] = bits.ReadBit();
        recruit.m_recruitGUID[4] = bits.ReadBit();
        recruit.m_recruitGUID[3] = bits.ReadBit();
        recruit.m_recruitGUID[7] = bits.ReadBit();
        recruit.m_recruitGUID[0] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(recruit.m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(recruit.m_name) >= sizeof(recruit.m_name)) {
            return false;
        }

        recruit.m_recruitGUID[5] = bits.ReadBit();
        recruit.m_recruitGUID[1] = bits.ReadBit();
        recruit.m_recruitGUID[6] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        LFGuildRecruitData& recruit = this->m_recruits.m_data[i];
        if (recruit.m_recruitGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[4] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.unk_043C = value;
        }

        if (recruit.m_recruitGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[3] ^= byteValue;
        }

        if (recruit.m_recruitGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[0] ^= byteValue;
        }

        if (recruit.m_recruitGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_level = value;
        }

        if (recruit.m_recruitGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[6] ^= byteValue;
        }

        if (recruit.m_recruitGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[7] ^= byteValue;
        }

        if (recruit.m_recruitGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[2] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_timeSince = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_classRoles = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_interests = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_timeLeft = value;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(recruit.m_name);
            msg->GetData(recruit.m_name, length);
            recruit.m_name[length] = 0;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(recruit.m_comment);
            msg->GetData(recruit.m_comment, length);
            recruit.m_comment[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            recruit.m_classID = value;
        }

        if (recruit.m_recruitGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            recruit.m_recruitGUID[5] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_updateTime = value;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 6bbfc5a7 IDENTICAL/NONE
// OFFSET: 0x006B27C0
bool ClientLfGuildRecruitListUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        if (!this->m_recruits.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: 35228adc IDENTICAL/UNPROVEN
// OFFSET: 0x006BE0D0/0x140372CC0
bool ClientLfGuildRecruitListUpdated::Handle() {
    if (ClientLfGuildRecruitListUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEDB4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEDB4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildRecruitListUpdated*)>(reinterpret_cast<uintptr_t>(ClientLfGuildRecruitListUpdated::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: c18df4c6 IDENTICAL/UNPROVEN
// OFFSET: 0x006AEF10/0x14034ED70
ClientLfGuildCommandResult::ClientLfGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint8_t result = 0;
        msg->Get(result);
        this->m_result = result;
    }

    {
        uint32_t command = 0;
        msg->Get(command);
        this->m_command = command;
    }
}

// PROVEN: ff6bd9ef IDENTICAL/UNPROVEN
// OFFSET: 0x006B1AE0/0x140350000
ClientLfGuildCommandResult::~ClientLfGuildCommandResult() {
}

// PROVEN: 7992158d IDENTICAL/IDENTICAL
// OFFSET: 0x006C52E0/0x14037A990
bool ClientLfGuildCommandResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_result = value;

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_command = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 293b19c5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6650/0x1403549C0
bool ClientLfGuildCommandResult::Handle() {
    if (ClientLfGuildCommandResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE94C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE94C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildCommandResult*)>(reinterpret_cast<uintptr_t>(ClientLfGuildCommandResult::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 7f2e153d IDENTICAL/IDENTICAL
// OFFSET: 0x006EF880/0x1403B4D60
ClientLfGuildMembershipListUpdated::ClientLfGuildMembershipListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f922e493 IDENTICAL/IDENTICAL
// OFFSET: 0x006DAF00/0x1403996F0
ClientLfGuildMembershipListUpdated::~ClientLfGuildMembershipListUpdated() {
}

// OFFSET: 0x006EEB50/0x1403B3AE0
void ClientLfGuildMembershipListUpdated::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_applications.SetCount(bits.ReadBits<20>());

    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        LFGuildApplicationData& application = this->m_applications.m_data[i];
        application.m_guildGUID[1] = bits.ReadBit();
        application.m_guildGUID[0] = bits.ReadBit();
        application.m_guildGUID[5] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(application.m_comment) = bits.ReadBits<11>();
        application.m_guildGUID[3] = bits.ReadBit();
        application.m_guildGUID[7] = bits.ReadBit();
        application.m_guildGUID[4] = bits.ReadBit();
        application.m_guildGUID[6] = bits.ReadBit();
        application.m_guildGUID[2] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(application.m_guildName) = bits.ReadBits<8>();
    }

    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        LFGuildApplicationData& application = this->m_applications.m_data[i];
        if (application.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[2] ^= byteValue;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(application.m_comment);
            msg->GetData(application.m_comment, length);
            application.m_comment[length] = 0;
        }

        if (application.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(application.m_guildName);
            msg->GetData(application.m_guildName, length);
            application.m_guildName[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_timeLeft = value;
        }

        if (application.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[0] ^= byteValue;
        }

        if (application.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[6] ^= byteValue;
        }

        if (application.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[3] ^= byteValue;
        }

        if (application.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[7] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_classRoles = value;
        }

        if (application.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[4] ^= byteValue;
        }

        if (application.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_timeSince = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_interests = value;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_numRemaining = value;
    }
}

// PROVEN: 2c666f6e IDENTICAL/UNPROVEN
// OFFSET: 0x006EF680/0x1403B4A30
bool ClientLfGuildMembershipListUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ED450/0x1403B1BF0
bool ClientLfGuildMembershipListUpdated::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<20>();

    if (msg->m_size - msg->m_read < ((27 * count + 7) >> 3) + 20 * count) {
        return false;
    }

    this->m_applications.SetCount(count);

    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        LFGuildApplicationData& application = this->m_applications.m_data[i];
        application.m_guildGUID[1] = bits.ReadBit();
        application.m_guildGUID[0] = bits.ReadBit();
        application.m_guildGUID[5] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(application.m_comment) = bits.ReadBits<11>();

        if (*reinterpret_cast<uint32_t*>(application.m_comment) >= sizeof(application.m_comment)) {
            return false;
        }

        application.m_guildGUID[3] = bits.ReadBit();
        application.m_guildGUID[7] = bits.ReadBit();
        application.m_guildGUID[4] = bits.ReadBit();
        application.m_guildGUID[6] = bits.ReadBit();
        application.m_guildGUID[2] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(application.m_guildName) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(application.m_guildName) >= sizeof(application.m_guildName)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        LFGuildApplicationData& application = this->m_applications.m_data[i];
        if (application.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[2] ^= byteValue;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(application.m_comment);
            msg->GetData(application.m_comment, length);
            application.m_comment[length] = 0;
        }

        if (application.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t length = *reinterpret_cast<uint32_t*>(application.m_guildName);
            msg->GetData(application.m_guildName, length);
            application.m_guildName[length] = 0;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_availability = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_timeLeft = value;
        }

        if (application.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[0] ^= byteValue;
        }

        if (application.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[6] ^= byteValue;
        }

        if (application.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[3] ^= byteValue;
        }

        if (application.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[7] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_classRoles = value;
        }

        if (application.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[4] ^= byteValue;
        }

        if (application.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            application.m_guildGUID[1] ^= byteValue;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_timeSince = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            application.m_interests = value;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_numRemaining = value;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: e8307e1b IDENTICAL/NONE
// OFFSET: 0x006B7AF0
bool ClientLfGuildMembershipListUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        if (!this->m_applications.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: beb1aace IDENTICAL/UNPROVEN
// OFFSET: 0x006AA9F0/0x14035A1B0
bool ClientLfGuildMembershipListUpdated::Handle() {
    if (ClientLfGuildMembershipListUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE774.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE774.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildMembershipListUpdated*)>(reinterpret_cast<uintptr_t>(ClientLfGuildMembershipListUpdated::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 870abbb5 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfGuildApplicantListUpdated::ClientLfGuildApplicantListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: b009905e IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientLfGuildApplicantListUpdated::~ClientLfGuildApplicantListUpdated() {
}

// PROVEN: a45855a8 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientLfGuildApplicantListUpdated::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 48022627 IDENTICAL/UNPROVEN
// OFFSET: 0x006A78B0/0x140356460
bool ClientLfGuildApplicantListUpdated::Handle() {
    if (ClientLfGuildApplicantListUpdated::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF2D0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF2D0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildApplicantListUpdated*)>(reinterpret_cast<uintptr_t>(ClientLfGuildApplicantListUpdated::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 17794ee4 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfGuildApplicationsListChanged::ClientLfGuildApplicationsListChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 86e6ddb7 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientLfGuildApplicationsListChanged::~ClientLfGuildApplicationsListChanged() {
}

// PROVEN: 7109d546 IDENTICAL/IDENTICAL
// OFFSET: 0x00696970/0x140341FE0
JamCliCUFProfile::JamCliCUFProfile() {
    this->m_fieldMask = 0;
    this->m_fieldMask2 = 0;
}

// PROVEN: b705e9cf IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientLfGuildApplicationsListChanged::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 7772f490 IDENTICAL/UNPROVEN
// OFFSET: 0x006A9B20/0x140358F60
bool ClientLfGuildApplicationsListChanged::Handle() {
    if (ClientLfGuildApplicationsListChanged::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD344.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD344.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfGuildApplicationsListChanged*)>(reinterpret_cast<uintptr_t>(ClientLfGuildApplicationsListChanged::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006A12A0/0x14034E570
JamResearchHistory::JamResearchHistory(const JamResearchHistory& source) {
    this->m_completionCount = source.m_completionCount;
    this->m_projectID = source.m_projectID;
    this->m_firstCompleted = source.m_firstCompleted;
}

// PROVEN: cab9db22 IDENTICAL/IDENTICAL
// OFFSET: 0x006E9370/0x1403AC260
ClientSetupResearchHistory::ClientSetupResearchHistory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 22e6ebdd IDENTICAL/IDENTICAL
// OFFSET: 0x006E53C0/0x1403A6740
ClientSetupResearchHistory::~ClientSetupResearchHistory() {
}

// OFFSET: 0x006DE550/0x14039E020
void ClientSetupResearchHistory::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_history.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_history.m_count; i++) {
        JamResearchHistory& history = this->m_history.m_data[i];
        uint32_t value = 0;
        msg->Get(value);
        history.m_projectID = value;
        value = 0;
        msg->Get(value);
        history.m_completionCount = value;
        value = 0;
        msg->Get(value);
        history.m_firstCompleted = value;
    }
}

// PROVEN: 29fe8a34 IDENTICAL/UNPROVEN
// OFFSET: 0x006E93B0/0x1403AC2B0
bool ClientSetupResearchHistory::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E5450/0x1403A6800
bool ClientSetupResearchHistory::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < 12 * count) {
        return false;
    }

    this->m_history.SetCount(count);

    for (uint32_t i = 0; i < this->m_history.m_count; i++) {
        JamResearchHistory& history = this->m_history.m_data[i];
        uint32_t value = 0;
        msg->Get(value);
        history.m_projectID = value;
        value = 0;
        msg->Get(value);
        history.m_completionCount = value;
        value = 0;
        msg->Get(value);
        history.m_firstCompleted = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 2e41d8fe IDENTICAL/UNPROVEN
// OFFSET: 0x006A72A0/0x140355CD0
bool ClientSetupResearchHistory::Handle() {
    if (ClientSetupResearchHistory::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3AC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3AC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSetupResearchHistory*)>(reinterpret_cast<uintptr_t>(ClientSetupResearchHistory::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: d40cb319 IDENTICAL/DIFFERENT
// OFFSET: 0x006AD080/0x1403590A0
ClientResearchComplete::ClientResearchComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3e01b54d IDENTICAL/UNPROVEN
// OFFSET: 0x006A15D0/0x1403596C0
ClientResearchComplete::~ClientResearchComplete() {
}

// PROVEN: e9ad1e7a IDENTICAL/DIFFERENT
// OFFSET: 0x006A79A0/0x14034E350
void ClientResearchComplete::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_firstCompleted = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_completionCount = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_projectID = value;
    }
}

// PROVEN: ce00df85 IDENTICAL/UNPROVEN
// OFFSET: 0x006B53F0/0x1403994E0
bool ClientResearchComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ef32b75d IDENTICAL/DIFFERENT
// OFFSET: 0x006AB7E0/0x14038EF00
bool ClientResearchComplete::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_firstCompleted = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_completionCount = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_research.m_projectID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 2f08838d IDENTICAL/UNPROVEN
// OFFSET: 0x006A51B0/0x140353130
bool ClientResearchComplete::Handle() {
    if (ClientResearchComplete::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD2FC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD2FC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientResearchComplete*)>(reinterpret_cast<uintptr_t>(ClientResearchComplete::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2e231cf0 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1310/0x14034E5E0
ClientAccountRestrictedWarning::ClientAccountRestrictedWarning(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
}

// PROVEN: 4ff19c39 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1290/0x14034E560
ClientAccountRestrictedWarning::~ClientAccountRestrictedWarning() {
}

// PROVEN: 8a481b2d IDENTICAL/IDENTICAL
// OFFSET: 0x006BFFD0/0x140375250
bool ClientAccountRestrictedWarning::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: f30fce6b IDENTICAL/UNPROVEN
// OFFSET: 0x006B0B20/0x1403619E0
bool ClientAccountRestrictedWarning::Handle() {
    if (ClientAccountRestrictedWarning::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFAEC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFAEC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAccountRestrictedWarning*)>(reinterpret_cast<uintptr_t>(ClientAccountRestrictedWarning::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: c6717dc3 IDENTICAL/IDENTICAL
// OFFSET: 0x006B08A0/0x1403616F0
ClientAreaTriggerMovementUpdate::ClientAreaTriggerMovementUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e2578621 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F50/0x14034E110
ClientAreaTriggerMovementUpdate::~ClientAreaTriggerMovementUpdate() {
}

// PROVEN: 39b2cdda IDENTICAL/IDENTICAL
// OFFSET: 0x006A2000/0x14034F470
void ClientAreaTriggerMovementUpdate::Unpack(CDataStore* msg, bool full) {
    { float value = 0; msg->Get(value); this->unk_0010.z = value; }
    { float value = 0; msg->Get(value); this->unk_001C.z = value; }
    { float value = 0; msg->Get(value); this->unk_0010.y = value; }
    { float value = 0; msg->Get(value); this->unk_0010.x = value; }
    { float value = 0; msg->Get(value); this->unk_001C.y = value; }
    { float value = 0; msg->Get(value); this->unk_001C.x = value; }
}

// PROVEN: 7087dedf IDENTICAL/UNPROVEN
// OFFSET: 0x006C45C0/0x14037A470
bool ClientAreaTriggerMovementUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 15318f55 IDENTICAL/IDENTICAL
// OFFSET: 0x006B4110/0x140365F30
bool ClientAreaTriggerMovementUpdate::Read(CDataStore* msg, bool full) {
    { float value = 0; msg->Get(value); this->unk_0010.z = value; }
    { float value = 0; msg->Get(value); this->unk_001C.z = value; }
    { float value = 0; msg->Get(value); this->unk_0010.y = value; }
    { float value = 0; msg->Get(value); this->unk_0010.x = value; }
    { float value = 0; msg->Get(value); this->unk_001C.y = value; }
    { float value = 0; msg->Get(value); this->unk_001C.x = value; }

    return msg->IsRead() != 0;
}

// PROVEN: 3eff768c IDENTICAL/UNPROVEN
// OFFSET: 0x006B6520/0x140368ED0
bool ClientAreaTriggerMovementUpdate::Handle() {
    if (ClientAreaTriggerMovementUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE708.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE708.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAreaTriggerMovementUpdate*)>(reinterpret_cast<uintptr_t>(ClientAreaTriggerMovementUpdate::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 4258c135 IDENTICAL/IDENTICAL
// OFFSET: 0x006A26C0/0x14034FBA0
ClientAverageItemLevelInform::ClientAverageItemLevelInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    float value = 0.0f;
    msg->Get(value);
    this->m_averageItemLevel = value;
}

// PROVEN: 6fa04c44 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2660/0x14034FB40
ClientAverageItemLevelInform::~ClientAverageItemLevelInform() {
}

// PROVEN: 900d23b7 IDENTICAL/IDENTICAL
// OFFSET: 0x006C6160/0x14037C580
bool ClientAverageItemLevelInform::Get(CDataStore* msg, bool full) {
    float value = 0.0f;
    msg->Get(value);
    this->m_averageItemLevel = value;

    return msg->IsRead() != 0;
}

// PROVEN: 7aba6e65 IDENTICAL/UNPROVEN
// OFFSET: 0x006AC820/0x14035C6B0
bool ClientAverageItemLevelInform::Handle() {
    if (ClientAverageItemLevelInform::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAverageItemLevelInform*)>(reinterpret_cast<uintptr_t>(ClientAverageItemLevelInform::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: ce8d7e1e IDENTICAL/IDENTICAL
// OFFSET: 0x006BC140/0x1403703C0
ClientBattlefieldMgrEjectPending::ClientBattlefieldMgrEjectPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 60fa9dae IDENTICAL/IDENTICAL
// OFFSET: 0x006A1630/0x14034E930
ClientBattlefieldMgrEjectPending::~ClientBattlefieldMgrEjectPending() {
}

// OFFSET: 0x006AB280/0x14035AD00
void ClientBattlefieldMgrEjectPending::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0016 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
}

// PROVEN: f92d34e0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C54B0/0x14037B270
bool ClientBattlefieldMgrEjectPending::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A99A0/0x140358D80
bool ClientBattlefieldMgrEjectPending::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0016 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: d8c3ec3c IDENTICAL/UNPROVEN
// OFFSET: 0x006B7E40/0x14036ADE0
bool ClientBattlefieldMgrEjectPending::Handle() {
    if (ClientBattlefieldMgrEjectPending::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD9E0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD9E0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrEjectPending*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrEjectPending::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 01f684c6 IDENTICAL/IDENTICAL
// OFFSET: 0x006BC610/0x140370A40
ClientBattlefieldMgrEjected::ClientBattlefieldMgrEjected(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: efa36f66 IDENTICAL/IDENTICAL
// OFFSET: 0x006A13B0/0x14034E690
ClientBattlefieldMgrEjected::~ClientBattlefieldMgrEjected() {
}

// OFFSET: 0x006BA180/0x14036DA90
void ClientBattlefieldMgrEjected::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001A = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    { uint8_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->unk_0021 = value; }
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
}

// PROVEN: 27b0959a IDENTICAL/UNPROVEN
// OFFSET: 0x006A78A0/0x140356450
bool ClientBattlefieldMgrEjected::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A4F50/0x140352EC0
bool ClientBattlefieldMgrEjected::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001A = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    { uint8_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->unk_0021 = value; }
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: aac71b0d IDENTICAL/UNPROVEN
// OFFSET: 0x006B9370/0x14036C8F0
bool ClientBattlefieldMgrEjected::Handle() {
    if (ClientBattlefieldMgrEjected::s_handler) {
        uint32_t right = g_jamHandlerKey_CFED90.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFED90.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrEjected*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrEjected::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: d02f0ae8 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB630/0x140383CF0
ClientBattlefieldMgrEntryInvite::ClientBattlefieldMgrEntryInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: aefe644a IDENTICAL/IDENTICAL
// OFFSET: 0x006A29A0/0x14034FE40
ClientBattlefieldMgrEntryInvite::~ClientBattlefieldMgrEntryInvite() {
}

// PROVEN: b59f752a IDENTICAL/DIFFERENT
// OFFSET: 0x006C0AD0/0x140375ED0
void ClientBattlefieldMgrEntryInvite::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001D = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
}

// PROVEN: 4e1575b1 IDENTICAL/UNPROVEN
// OFFSET: 0x006C7930/0x14037E7B0
bool ClientBattlefieldMgrEntryInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f30be0ec IDENTICAL/DIFFERENT
// OFFSET: 0x006ABED0/0x14035BC40
bool ClientBattlefieldMgrEntryInvite::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001D = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: aaf0ed7b IDENTICAL/UNPROVEN
// OFFSET: 0x006AA0C0/0x1403596D0
bool ClientBattlefieldMgrEntryInvite::Handle() {
    if (ClientBattlefieldMgrEntryInvite::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD0CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD0CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrEntryInvite*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrEntryInvite::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 0f2b1254 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlefieldMgrExitRequest::ClientBattlefieldMgrExitRequest(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: dbf2537f IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientBattlefieldMgrExitRequest::~ClientBattlefieldMgrExitRequest() {
}

// PROVEN: 3477407f IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientBattlefieldMgrExitRequest::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: cbffd3c2 IDENTICAL/UNPROVEN
// OFFSET: 0x006A5E70/0x140353EF0
bool ClientBattlefieldMgrExitRequest::Handle() {
    if (ClientBattlefieldMgrExitRequest::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4CC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4CC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrExitRequest*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrExitRequest::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: d15eba7d IDENTICAL/IDENTICAL
// OFFSET: 0x006B7DC0/0x14036AD60
ClientBattlefieldMgrStateChange::ClientBattlefieldMgrStateChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5624b902 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1790/0x14034EB10
ClientBattlefieldMgrStateChange::~ClientBattlefieldMgrStateChange() {
}

// PROVEN: 4c00164c IDENTICAL/IDENTICAL
// OFFSET: 0x006A45E0/0x140352060
void ClientBattlefieldMgrStateChange::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0015 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
}

// PROVEN: b24a8f19 IDENTICAL/UNPROVEN
// OFFSET: 0x006CA810/0x1403828E0
bool ClientBattlefieldMgrStateChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: c10bcd6e IDENTICAL/IDENTICAL
// OFFSET: 0x006C01C0/0x140375490
bool ClientBattlefieldMgrStateChange::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0015 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: c81c0db3 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4510/0x140366460
bool ClientBattlefieldMgrStateChange::Handle() {
    if (ClientBattlefieldMgrStateChange::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA54.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA54.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrStateChange*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrStateChange::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 41c83c65 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlefieldPortDenied::ClientBattlefieldPortDenied(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 15f2786e IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientBattlefieldPortDenied::~ClientBattlefieldPortDenied() {
}

// PROVEN: 08a83bc9 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientBattlefieldPortDenied::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 9bdea2df IDENTICAL/UNPROVEN
// OFFSET: 0x006BBDB0/0x14036FF30
bool ClientBattlefieldPortDenied::Handle() {
    if (ClientBattlefieldPortDenied::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFD08.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFD08.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldPortDenied*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldPortDenied::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 3223be6c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2B60/0x1403500A0
ClientBattlefieldRatedInfo::ClientBattlefieldRatedInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 21770b7d IDENTICAL/IDENTICAL
// OFFSET: 0x006A1C90/0x14034F0F0
ClientBattlefieldRatedInfo::~ClientBattlefieldRatedInfo() {
}

// PROVEN: 21a3f69b IDENTICAL/IDENTICAL
// OFFSET: 0x006A16D0/0x14034EA10
void ClientBattlefieldRatedInfo::Unpack(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_002C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0028 = value; }
}

// PROVEN: ca1c8b96 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0060/0x140375300
bool ClientBattlefieldRatedInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e4063162 IDENTICAL/IDENTICAL
// OFFSET: 0x006AF0E0/0x14035FA30
bool ClientBattlefieldRatedInfo::Read(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_002C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0028 = value; }

    return msg->IsRead() != 0;
}

// PROVEN: 8c552cfd IDENTICAL/UNPROVEN
// OFFSET: 0x006B5CE0/0x140368250
bool ClientBattlefieldRatedInfo::Handle() {
    if (ClientBattlefieldRatedInfo::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDFDC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDFDC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldRatedInfo*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldRatedInfo::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 61d181b9 IDENTICAL/IDENTICAL
// OFFSET: 0x006BF740/0x140374890
ClientBattlefieldStatusFailed::ClientBattlefieldStatusFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ca518db5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A29F0/0x14034FED0
ClientBattlefieldStatusFailed::~ClientBattlefieldStatusFailed() {
}

// PROVEN: 8f2ab580 IDENTICAL/DIFFERENT
// OFFSET: 0x006AFB50/0x140360640
void ClientBattlefieldStatusFailed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0013 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->unk_0023 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0025 = bits.ReadBit();
    this->unk_0026 = bits.ReadBit();
    this->unk_0024 = bits.ReadBit();
    this->unk_0022 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->unk_0021 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0020 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->unk_0027 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_type = value; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_id = value; }
    if (this->unk_0026) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0026 ^= byteValue; }
    if (this->unk_0023) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0023 ^= byteValue; }
    if (this->unk_0027) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0027 ^= byteValue; }
    if (this->unk_0024) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0024 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0025) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0025 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[3] ^= byteValue; }
    if (this->unk_0021) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0021 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[0] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[4] ^= byteValue; }
    if (this->unk_0020) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0020 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0022) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0022 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[2] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_time = value; }
    if (this->m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[5] ^= byteValue; }
}

// PROVEN: 723576d3 IDENTICAL/UNPROVEN
// OFFSET: 0x006BC7C0/0x140370C60
bool ClientBattlefieldStatusFailed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BA8F0/0x14036E440
bool ClientBattlefieldStatusFailed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0013 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->unk_0023 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0025 = bits.ReadBit();
    this->unk_0026 = bits.ReadBit();
    this->unk_0024 = bits.ReadBit();
    this->unk_0022 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->unk_0021 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0020 = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->unk_0027 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_type = value; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_id = value; }
    if (this->unk_0026) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0026 ^= byteValue; }
    if (this->unk_0023) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0023 ^= byteValue; }
    if (this->unk_0027) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0027 ^= byteValue; }
    if (this->unk_0024) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0024 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0025) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0025 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[3] ^= byteValue; }
    if (this->unk_0021) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0021 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[0] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[4] ^= byteValue; }
    if (this->unk_0020) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0020 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0022) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0022 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[2] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_ticket.m_time = value; }
    if (this->m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_ticket.m_requesterGuid[5] ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: 96f8a263 IDENTICAL/UNPROVEN
// OFFSET: 0x006AF1B0/0x14035FB40
bool ClientBattlefieldStatusFailed::Handle() {
    if (ClientBattlefieldStatusFailed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEBBC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEBBC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatusFailed*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatusFailed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 6169440d IDENTICAL/IDENTICAL
// OFFSET: 0x006B0610/0x1403613A0
ClientBattlefieldStatusQueued::ClientBattlefieldStatusQueued(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f0b72248 IDENTICAL/UNPROVEN
// OFFSET: 0x006B81E0/0x14036B2B0
ClientBattlefieldStatusQueued::~ClientBattlefieldStatusQueued() {
}

// OFFSET: 0x006ADC30/0x14035E070
void ClientBattlefieldStatusQueued::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    if (this->m_header.m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_type = value; }
    if (this->m_header.m_queueID[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->m_header.m_queueID[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[7] ^= byteValue; }
    if (this->m_header.m_queueID[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[1] ^= byteValue; }
    if (this->m_header.m_queueID[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[2] ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMin = value; }
    if (this->m_header.m_queueID[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_teamSize = value; }
    if (this->m_header.m_queueID[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[6] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->m_header.m_queueID[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[3] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_header.m_queueID[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[0] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_time = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_id = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMax = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->m_header.m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_instanceID = value; }
    if (this->m_header.m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue; }
}

// PROVEN: f6e24d14 IDENTICAL/UNPROVEN
// OFFSET: 0x006CA470/0x140382440
bool ClientBattlefieldStatusQueued::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C31F0/0x140378BB0
bool ClientBattlefieldStatusQueued::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    if (this->m_header.m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_type = value; }
    if (this->m_header.m_queueID[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->m_header.m_queueID[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[7] ^= byteValue; }
    if (this->m_header.m_queueID[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[1] ^= byteValue; }
    if (this->m_header.m_queueID[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[2] ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMin = value; }
    if (this->m_header.m_queueID[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_teamSize = value; }
    if (this->m_header.m_queueID[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[6] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->m_header.m_queueID[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[3] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_header.m_queueID[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[0] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_time = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_id = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMax = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->m_header.m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_instanceID = value; }
    if (this->m_header.m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: d2bf96f7 IDENTICAL/UNPROVEN
// OFFSET: 0x006A9080/0x140358150
bool ClientBattlefieldStatusQueued::Handle() {
    if (ClientBattlefieldStatusQueued::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE008.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE008.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatusQueued*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatusQueued::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: e1f8ade7 IDENTICAL/IDENTICAL
// OFFSET: 0x006C9A10/0x140381460
ClientBattlefieldStatusWaitforgroups::ClientBattlefieldStatusWaitforgroups(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 276a7eba IDENTICAL/UNPROVEN
// OFFSET: 0x006B5D80/0x140368310
ClientBattlefieldStatusWaitforgroups::~ClientBattlefieldStatusWaitforgroups() {
}

// PROVEN: 53955e97 IDENTICAL/DIFFERENT
// OFFSET: 0x006B4930/0x140366920
void ClientBattlefieldStatusWaitforgroups::Unpack(CDataStore* msg, bool full) {
    { uint8_t value = 0; msg->Get(value); this->unk_004A = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_type = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_id = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0040 = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_instanceID = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_0049 = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_004B = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMax = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_0048 = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_teamSize = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0044 = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_time = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMin = value; }
    JamBitReader bits(msg);
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    if (this->m_header.m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue; }
    if (this->m_header.m_queueID[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue; }
    if (this->m_header.m_queueID[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[1] ^= byteValue; }
    if (this->m_header.m_queueID[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[0] ^= byteValue; }
    if (this->m_header.m_queueID[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[2] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue; }
    if (this->m_header.m_queueID[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[7] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_header.m_queueID[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[6] ^= byteValue; }
    if (this->m_header.m_queueID[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->m_header.m_queueID[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[3] ^= byteValue; }
}

// PROVEN: a1be9c46 IDENTICAL/UNPROVEN
// OFFSET: 0x006D8660/0x140396760
bool ClientBattlefieldStatusWaitforgroups::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CCAA0/0x140385BF0
bool ClientBattlefieldStatusWaitforgroups::Read(CDataStore* msg, bool full) {
    { uint8_t value = 0; msg->Get(value); this->unk_004A = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_type = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_id = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0040 = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_instanceID = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_0049 = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_004B = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMax = value; }
    { uint8_t value = 0; msg->Get(value); this->unk_0048 = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_teamSize = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0044 = value; }
    { uint32_t value = 0; msg->Get(value); this->m_header.m_ticket.m_time = value; }
    { uint8_t value = 0; msg->Get(value); this->m_header.m_rangeMin = value; }
    JamBitReader bits(msg);
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    if (this->m_header.m_ticket.m_requesterGuid[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue; }
    if (this->m_header.m_queueID[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue; }
    if (this->m_header.m_queueID[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[1] ^= byteValue; }
    if (this->m_header.m_queueID[0]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[0] ^= byteValue; }
    if (this->m_header.m_queueID[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[2] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[2]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue; }
    if (this->m_header.m_queueID[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[7] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[1]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue; }
    if (this->m_header.m_queueID[6]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[6] ^= byteValue; }
    if (this->m_header.m_queueID[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[5]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[4]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue; }
    if (this->m_header.m_ticket.m_requesterGuid[7]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue; }
    if (this->m_header.m_queueID[3]) { uint8_t byteValue = 0; msg->Get(byteValue); this->m_header.m_queueID[3] ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: d142fce9 IDENTICAL/UNPROVEN
// OFFSET: 0x006A75F0/0x1403560C0
bool ClientBattlefieldStatusWaitforgroups::Handle() {
    if (ClientBattlefieldStatusWaitforgroups::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF9B8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF9B8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatusWaitforgroups*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatusWaitforgroups::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 29f9f1ab IDENTICAL/IDENTICAL
// OFFSET: 0x006B7830/0x14036A740
ClientBattlegroundPlayerJoined::ClientBattlegroundPlayerJoined(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8403edcd IDENTICAL/IDENTICAL
// OFFSET: 0x006A2440/0x14034F940
ClientBattlegroundPlayerJoined::~ClientBattlegroundPlayerJoined() {
}

// PROVEN: 59afd153 IDENTICAL/IDENTICAL
// OFFSET: 0x006B69C0/0x1403694C0
void ClientBattlegroundPlayerJoined::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
}

// PROVEN: f9a56a4b IDENTICAL/UNPROVEN
// OFFSET: 0x006C79A0/0x14037E880
bool ClientBattlegroundPlayerJoined::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4fddefad IDENTICAL/IDENTICAL
// OFFSET: 0x006B9650/0x14036CC60
bool ClientBattlegroundPlayerJoined::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: db249447 IDENTICAL/UNPROVEN
// OFFSET: 0x006B31F0/0x140364C10
bool ClientBattlegroundPlayerJoined::Handle() {
    if (ClientBattlegroundPlayerJoined::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF638.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF638.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlegroundPlayerJoined*)>(reinterpret_cast<uintptr_t>(ClientBattlegroundPlayerJoined::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 424b5d63 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB6B0/0x140383DC0
ClientBattlegroundPlayerLeft::ClientBattlegroundPlayerLeft(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b572b1b6 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F30/0x14034E010
ClientBattlegroundPlayerLeft::~ClientBattlegroundPlayerLeft() {
}

// PROVEN: 13de56c4 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0330/0x140375670
void ClientBattlegroundPlayerLeft::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
}

// PROVEN: d39cde14 IDENTICAL/UNPROVEN
// OFFSET: 0x006AFA70/0x140360520
bool ClientBattlegroundPlayerLeft::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: da560963 IDENTICAL/IDENTICAL
// OFFSET: 0x006A4030/0x1403519C0
bool ClientBattlegroundPlayerLeft::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: 1bb0b57e IDENTICAL/UNPROVEN
// OFFSET: 0x006AB480/0x14035AF90
bool ClientBattlegroundPlayerLeft::Handle() {
    if (ClientBattlegroundPlayerLeft::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDD44.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDD44.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlegroundPlayerLeft*)>(reinterpret_cast<uintptr_t>(ClientBattlegroundPlayerLeft::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: cd24f7e8 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientClearBossEmotes::ClientClearBossEmotes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 735950e5 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientClearBossEmotes::~ClientClearBossEmotes() {
}

// PROVEN: 1fc9257f IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientClearBossEmotes::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: f0cbaa69 IDENTICAL/UNPROVEN
// OFFSET: 0x006BC5C0/0x1403709E0
bool ClientClearBossEmotes::Handle() {
    if (ClientClearBossEmotes::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF1D8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF1D8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientClearBossEmotes*)>(reinterpret_cast<uintptr_t>(ClientClearBossEmotes::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 6e9bfb25 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientCustomLoadScreen::ClientCustomLoadScreen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// PROVEN: 9460b8ea IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientCustomLoadScreen::~ClientCustomLoadScreen() {
}

// PROVEN: 28a9f118 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientCustomLoadScreen::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// PROVEN: 0848e326 IDENTICAL/UNPROVEN
// OFFSET: 0x006A7500/0x140355F80
bool ClientCustomLoadScreen::Handle() {
    if (ClientCustomLoadScreen::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6D4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6D4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientCustomLoadScreen*)>(reinterpret_cast<uintptr_t>(ClientCustomLoadScreen::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 5f7b0ca2 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientDifferentInstanceFromParty::ClientDifferentInstanceFromParty(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 73d25132 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientDifferentInstanceFromParty::~ClientDifferentInstanceFromParty() {
}

// PROVEN: 4dadc262 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientDifferentInstanceFromParty::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 7726354a IDENTICAL/UNPROVEN
// OFFSET: 0x006AA4A0/0x140359B00
bool ClientDifferentInstanceFromParty::Handle() {
    if (ClientDifferentInstanceFromParty::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE04C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE04C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDifferentInstanceFromParty*)>(reinterpret_cast<uintptr_t>(ClientDifferentInstanceFromParty::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 245d76db IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientDontAutoPushSpellsToActionBar::ClientDontAutoPushSpellsToActionBar(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: fc6e5af9 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientDontAutoPushSpellsToActionBar::~ClientDontAutoPushSpellsToActionBar() {
}

// PROVEN: 95d6cc36 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientDontAutoPushSpellsToActionBar::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 07588fe9 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0500/0x140375820
bool ClientDontAutoPushSpellsToActionBar::Handle() {
    if (ClientDontAutoPushSpellsToActionBar::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCDC0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCDC0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDontAutoPushSpellsToActionBar*)>(reinterpret_cast<uintptr_t>(ClientDontAutoPushSpellsToActionBar::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 3c3e76c7 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientFailedPlayerCondition::ClientFailedPlayerCondition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// PROVEN: a1e735cc IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientFailedPlayerCondition::~ClientFailedPlayerCondition() {
}

// PROVEN: db47b331 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientFailedPlayerCondition::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// PROVEN: 72414139 IDENTICAL/UNPROVEN
// OFFSET: 0x006B0AE0/0x140361980
bool ClientFailedPlayerCondition::Handle() {
    if (ClientFailedPlayerCondition::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE840.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE840.m_left->Get();
        return reinterpret_cast<bool (*)(ClientFailedPlayerCondition*)>(reinterpret_cast<uintptr_t>(ClientFailedPlayerCondition::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 08bc2e36 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0C80/0x1403760F0
ClientInspectHonorStats::ClientInspectHonorStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ac10a33b IDENTICAL/IDENTICAL
// OFFSET: 0x006A2FD0/0x1403504D0
ClientInspectHonorStats::~ClientInspectHonorStats() {
}

// PROVEN: c79ba3f9 IDENTICAL/IDENTICAL
// OFFSET: 0x006A48E0/0x140352660
void ClientInspectHonorStats::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0014 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    { uint8_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint16_t value = 0; msg->Get(value); this->unk_001E = value; }
    { uint16_t value = 0; msg->Get(value); this->unk_001C = value; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
}

// PROVEN: d9fcad74 IDENTICAL/UNPROVEN
// OFFSET: 0x006D9300/0x1403976F0
bool ClientInspectHonorStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 24cab389 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB6F0/0x140383E00
bool ClientInspectHonorStats::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0014 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    { uint8_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint16_t value = 0; msg->Get(value); this->unk_001E = value; }
    { uint16_t value = 0; msg->Get(value); this->unk_001C = value; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: c9b107df IDENTICAL/UNPROVEN
// OFFSET: 0x006B48E0/0x1403668C0
bool ClientInspectHonorStats::Handle() {
    if (ClientInspectHonorStats::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE310.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE310.m_left->Get();
        return reinterpret_cast<bool (*)(ClientInspectHonorStats*)>(reinterpret_cast<uintptr_t>(ClientInspectHonorStats::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 5c27d6c1 IDENTICAL/IDENTICAL
// OFFSET: 0x006D0D30/0x14038B900
ClientInspectRatedBgStats::ClientInspectRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c8d0ac0b IDENTICAL/IDENTICAL
// OFFSET: 0x006A1100/0x14034E330
ClientInspectRatedBgStats::~ClientInspectRatedBgStats() {
}

// PROVEN: 2a722e86 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAF40/0x14036ECC0
void ClientInspectRatedBgStats::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0016 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
}

// PROVEN: e28d6803 IDENTICAL/UNPROVEN
// OFFSET: 0x006CA850/0x140382920
bool ClientInspectRatedBgStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2c6cc651 IDENTICAL/IDENTICAL
// OFFSET: 0x006BEEF0/0x140373E80
bool ClientInspectRatedBgStats::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0016 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: d723a8a2 IDENTICAL/UNPROVEN
// OFFSET: 0x006A4190/0x140351B80
bool ClientInspectRatedBgStats::Handle() {
    if (ClientInspectRatedBgStats::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDB84.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDB84.m_left->Get();
        return reinterpret_cast<bool (*)(ClientInspectRatedBgStats*)>(reinterpret_cast<uintptr_t>(ClientInspectRatedBgStats::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 09061e3f IDENTICAL/DIFFERENT
// OFFSET: 0x006AD080/0x1403590A0
ClientLfgSlotInvalid::ClientLfgSlotInvalid(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010.m_firstCompleted = value;
    value = 0;
    msg->Get(value);
    this->unk_0010.m_completionCount = value;
    value = 0;
    msg->Get(value);
    this->unk_0010.m_projectID = value;
}

// PROVEN: abf0f225 IDENTICAL/UNPROVEN
// OFFSET: 0x006A15D0/0x1403596C0
ClientLfgSlotInvalid::~ClientLfgSlotInvalid() {
}

// PROVEN: 2535a2c8 IDENTICAL/UNPROVEN
// OFFSET: 0x006B53F0/0x1403994E0
bool ClientLfgSlotInvalid::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d21cc805 IDENTICAL/DIFFERENT
// OFFSET: 0x006AB7E0/0x14038EF00
bool ClientLfgSlotInvalid::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010.m_firstCompleted = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010.m_completionCount = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010.m_projectID = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 1bda8360 IDENTICAL/UNPROVEN
// OFFSET: 0x006B2040/0x140363550
bool ClientLfgSlotInvalid::Handle() {
    if (ClientLfgSlotInvalid::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD074.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD074.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgSlotInvalid*)>(reinterpret_cast<uintptr_t>(ClientLfgSlotInvalid::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 9b8bd42e IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfgUpdateStatusNone::ClientLfgUpdateStatusNone(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 4c78fe34 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientLfgUpdateStatusNone::~ClientLfgUpdateStatusNone() {
}

// PROVEN: cae21a06 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientLfgUpdateStatusNone::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 9dd6f1c2 IDENTICAL/UNPROVEN
// OFFSET: 0x006BBD20/0x14036FE70
bool ClientLfgUpdateStatusNone::Handle() {
    if (ClientLfgUpdateStatusNone::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD5B0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD5B0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgUpdateStatusNone*)>(reinterpret_cast<uintptr_t>(ClientLfgUpdateStatusNone::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006BEDF0/0x140373D30
ClientMessageBox::ClientMessageBox(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);
    this->m_text.InitExternal(static_cast<uint8_t*>(data), size);
}

// OFFSET: 0x006A7330/0x140370400
ClientMessageBox::~ClientMessageBox() {
}

// PROVEN: 73069c73 IDENTICAL/IDENTICAL
// OFFSET: 0x006B5C90/0x140368200
ClientMissileCancel::ClientMissileCancel(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 14b7b211 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1040/0x14034E290
ClientMissileCancel::~ClientMissileCancel() {
}

// OFFSET: 0x006B1B30/0x140362EB0
void ClientMissileCancel::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001F = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
}

// PROVEN: c340d82c IDENTICAL/UNPROVEN
// OFFSET: 0x006CA8C0/0x140382990
bool ClientMissileCancel::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B7FD0/0x14036AFF0
bool ClientMissileCancel::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_001F = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: 0f763803 IDENTICAL/UNPROVEN
// OFFSET: 0x006BA460/0x14036DE50
bool ClientMissileCancel::Handle() {
    if (ClientMissileCancel::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE694.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE694.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMissileCancel*)>(reinterpret_cast<uintptr_t>(ClientMissileCancel::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: df756f71 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3230/0x14035BB30
ClientMoneyNotify::ClientMoneyNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// PROVEN: 3a31ed7a IDENTICAL/UNPROVEN
// OFFSET: 0x006A2650/0x14034ED30
ClientMoneyNotify::~ClientMoneyNotify() {
}

// PROVEN: 41117fe6 IDENTICAL/IDENTICAL
// OFFSET: 0x006C3F80/0x14037E820
bool ClientMoneyNotify::Get(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// PROVEN: 51495a54 IDENTICAL/UNPROVEN
// OFFSET: 0x006AB7A0/0x14035B390
bool ClientMoneyNotify::Handle() {
    if (ClientMoneyNotify::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDBB4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDBB4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoneyNotify*)>(reinterpret_cast<uintptr_t>(ClientMoneyNotify::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 87c7bf07 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0E30/0x14034DED0
ClientNewWorld::~ClientNewWorld() {
}

// PROVEN: ef128973 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientNewWorldAbort::ClientNewWorldAbort(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 78dbf47f IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientNewWorldAbort::~ClientNewWorldAbort() {
}

// PROVEN: 64e2188f IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientRatedBgRating::ClientRatedBgRating(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// PROVEN: 3e9f5783 IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientRatedBgRating::~ClientRatedBgRating() {
}

// PROVEN: 0b9679e7 IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientRatedBgRating::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// PROVEN: 49e2d043 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8EB0/0x140357EE0
bool ClientRatedBgRating::Handle() {
    if (ClientRatedBgRating::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDB04.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDB04.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRatedBgRating*)>(reinterpret_cast<uintptr_t>(ClientRatedBgRating::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 49d7427c IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E30/0x14034F2A0
ClientRatedBgStats::ClientRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ea8567eb IDENTICAL/IDENTICAL
// OFFSET: 0x006A2D50/0x1403502B0
ClientRatedBgStats::~ClientRatedBgStats() {
}

// PROVEN: 4d987246 IDENTICAL/IDENTICAL
// OFFSET: 0x006A19F0/0x14034EE30
void ClientRatedBgStats::Unpack(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_003C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0038 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0044 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0048 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0028 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0054 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_002C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0040 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_004C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0034 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0030 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0050 = value; }
}

// PROVEN: 0c69dce7 IDENTICAL/UNPROVEN
// OFFSET: 0x006C54A0/0x14037B260
bool ClientRatedBgStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 47a68806 IDENTICAL/IDENTICAL
// OFFSET: 0x006B7660/0x14036A4D0
bool ClientRatedBgStats::Read(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_003C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0038 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0044 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0048 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0028 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0054 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_002C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0040 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_004C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0034 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0030 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0050 = value; }

    return msg->IsRead() != 0;
}

// PROVEN: 20a129ff IDENTICAL/UNPROVEN
// OFFSET: 0x006AAD80/0x14035A640
bool ClientRatedBgStats::Handle() {
    if (ClientRatedBgStats::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD85C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD85C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRatedBgStats*)>(reinterpret_cast<uintptr_t>(ClientRatedBgStats::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 23a1d4bf IDENTICAL/IDENTICAL
// OFFSET: 0x006A24E0/0x14034FA00
ClientRequestPvpRewardsResponse::ClientRequestPvpRewardsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 93f9e289 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0D60/0x14034DD80
ClientRequestPvpRewardsResponse::~ClientRequestPvpRewardsResponse() {
}

// PROVEN: c9936bbc IDENTICAL/IDENTICAL
// OFFSET: 0x006A1D10/0x14034F150
void ClientRequestPvpRewardsResponse::Unpack(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
}

// PROVEN: d986b35a IDENTICAL/UNPROVEN
// OFFSET: 0x006D7150/0x140394DB0
bool ClientRequestPvpRewardsResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4fc4fed9 IDENTICAL/IDENTICAL
// OFFSET: 0x006CAA70/0x140382BA0
bool ClientRequestPvpRewardsResponse::Read(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }

    return msg->IsRead() != 0;
}

// PROVEN: 61be8753 IDENTICAL/UNPROVEN
// OFFSET: 0x006C10A0/0x140376590
bool ClientRequestPvpRewardsResponse::Handle() {
    if (ClientRequestPvpRewardsResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4F8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4F8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRequestPvpRewardsResponse*)>(reinterpret_cast<uintptr_t>(ClientRequestPvpRewardsResponse::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 50feb197 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4910/0x14037A850
ClientSetDfFastLaunchResult::ClientSetDfFastLaunchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->unk_0010 = value >> 7;
}

// PROVEN: e584138c IDENTICAL/UNPROVEN
// OFFSET: 0x006A1F40/0x14034F3E0
ClientSetDfFastLaunchResult::~ClientSetDfFastLaunchResult() {
}

// PROVEN: f8818acd IDENTICAL/IDENTICAL
// OFFSET: 0x006C9980/0x1403813C0
bool ClientSetDfFastLaunchResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->unk_0010 = value >> 7;

    return msg->IsRead() != 0;
}

// PROVEN: 75f0185a IDENTICAL/UNPROVEN
// OFFSET: 0x006BB7D0/0x14036F7A0
bool ClientSetDfFastLaunchResult::Handle() {
    if (ClientSetDfFastLaunchResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDD28.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDD28.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSetDfFastLaunchResult*)>(reinterpret_cast<uintptr_t>(ClientSetDfFastLaunchResult::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: d55fc038 IDENTICAL/UNPROVEN
// OFFSET: 0x006AA2E0/0x140364670
ClientSuspendToken::ClientSuspendToken(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        uint32_t waitCount = 0;
        msg->Get(waitCount);
        this->m_sequence = waitCount;
    }

    {
        uint8_t bits = 0;
        msg->Get(bits);
        this->m_reason = bits >> 7;
    }
}

// PROVEN: 2b60fe71 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1CD0/0x14034F120
ClientSuspendToken::~ClientSuspendToken() {
}

// PROVEN: bf666f85 IDENTICAL/IDENTICAL
// OFFSET: 0x006D5A70/0x140392D80
ClientTransferPending::~ClientTransferPending() {
}

// PROVEN: 9ea3e9ca IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientUpdateDungeonEncounterForLoot::ClientUpdateDungeonEncounterForLoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 4057fa7f IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientUpdateDungeonEncounterForLoot::~ClientUpdateDungeonEncounterForLoot() {
}

// PROVEN: 15299420 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientUpdateDungeonEncounterForLoot::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: c98f9855 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6D70/0x1403552C0
bool ClientUpdateDungeonEncounterForLoot::Handle() {
    if (ClientUpdateDungeonEncounterForLoot::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDEA8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDEA8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientUpdateDungeonEncounterForLoot*)>(reinterpret_cast<uintptr_t>(ClientUpdateDungeonEncounterForLoot::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 21546869 IDENTICAL/NONE
// OFFSET: 0x006A2810/0x14034F320
ClientUpdateServerPlayerPosition::ClientUpdateServerPlayerPosition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    {
        float value = 0.0f;
        msg->Get(value);
        this->m_position.x = value;
    }

    {
        float value = 0.0f;
        msg->Get(value);
        this->m_position.y = value;
    }

    {
        float value = 0.0f;
        msg->Get(value);
        this->m_position.z = value;
    }
}

// PROVEN: bd99f0fd IDENTICAL/IDENTICAL
// OFFSET: 0x006A2670/0x14034FB50
ClientUpdateServerPlayerPosition::~ClientUpdateServerPlayerPosition() {
}

// PROVEN: 6900d7c7 IDENTICAL/UNPROVEN
// OFFSET: 0x006CCA30/0x140385B80
bool ClientUpdateServerPlayerPosition::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: eaaa2ffa IDENTICAL/DIFFERENT
// OFFSET: 0x006B7D40/0x14036ACD0
bool ClientUpdateServerPlayerPosition::Read(CDataStore* msg, bool full) {
    float value = 0.0f;
    msg->Get(value);
    this->m_position.x = value;

    {
        float value = 0.0f;
        msg->Get(value);
        this->m_position.y = value;
    }

    {
        float value = 0.0f;
        msg->Get(value);
        this->m_position.z = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 2a506087 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4760/0x140366760
bool ClientUpdateServerPlayerPosition::Handle() {
    if (ClientUpdateServerPlayerPosition::s_handler) {
        uint32_t right = g_jamHandlerKey_CFED40.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFED40.m_left->Get();
        return reinterpret_cast<bool (*)(ClientUpdateServerPlayerPosition*)>(reinterpret_cast<uintptr_t>(ClientUpdateServerPlayerPosition::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006BEDF0/0x140373D30
ClientWardenData::ClientWardenData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);
    this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
}

// OFFSET: 0x006A7330/0x140370400
ClientWardenData::~ClientWardenData() {
}

// PROVEN: c9e261b2 IDENTICAL/NONE
// OFFSET: 0x006C7920
bool ClientWardenData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C5EB0/0x14037C2B0
bool ClientWardenData::Read(CDataStore* msg, bool full) {
    uint32_t size = 0;
    msg->Get(size);

    void* data;
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.PutData(data, size);
        this->m_data.Finalize();
    }

    return msg->IsRead() != 0;
}

// PROVEN: b90bc3fa IDENTICAL/UNPROVEN
// OFFSET: 0x006C0960/0x140375D10
bool ClientWardenData::Handle() {
    if (ClientWardenData::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD350.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD350.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWardenData*)>(reinterpret_cast<uintptr_t>(ClientWardenData::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: fae9a71f IDENTICAL/IDENTICAL
// OFFSET: 0x006B7E00/0x14036ADA0
ClientWargameCheckEntry::ClientWargameCheckEntry(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b5628655 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DA0/0x14034DDC0
ClientWargameCheckEntry::~ClientWargameCheckEntry() {
}

// PROVEN: babe2c0f IDENTICAL/DIFFERENT
// OFFSET: 0x006B1000/0x140362050
void ClientWargameCheckEntry::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0019 = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }
}

// PROVEN: 24b4f52c IDENTICAL/UNPROVEN
// OFFSET: 0x006C8100/0x14037F0B0
bool ClientWargameCheckEntry::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: abad7ce2 IDENTICAL/DIFFERENT
// OFFSET: 0x006B8990/0x14036BC70
bool ClientWargameCheckEntry::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0019 = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_0017 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    this->unk_001C = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_001D = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_001F = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();
    this->unk_001B = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_001E = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
    if (this->unk_0018) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0018 ^= byteValue; }
    if (this->unk_001A) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001A ^= byteValue; }
    if (this->unk_001C) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001C ^= byteValue; }
    if (this->unk_001E) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001E ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_001D) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001D ^= byteValue; }
    if (this->unk_001F) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001F ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0019) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0019 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_001B) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_001B ^= byteValue; }

    return msg->IsRead() && this->IsValid();
}

// OFFSET: 0x006A0F70/0x14034E1D0
bool ClientWargameCheckEntry::IsValid() const {
    const uint64_t& queueID = *reinterpret_cast<const uint64_t*>(&this->unk_0010);
    const uint64_t& guid = *reinterpret_cast<const uint64_t*>(&this->unk_0018);

    if ((queueID & 0xF000000000000000ull) == 0x1000000000000000ull && (queueID & 0x0FF0000000000000ull) == 0x0F10000000000000ull && static_cast<uint8_t>(static_cast<uint32_t>(queueID >> 16) & 0xF) == 3 && !(guid & 0xF000000000000000ull) && (guid & 0xF07FFFFFFFFFFFFFull)) {
        return true;
    }

    return false;
}

// PROVEN: 4da03975 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4FC0/0x140367180
bool ClientWargameCheckEntry::Handle() {
    if (ClientWargameCheckEntry::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD374.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD374.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWargameCheckEntry*)>(reinterpret_cast<uintptr_t>(ClientWargameCheckEntry::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: fe7a348a IDENTICAL/IDENTICAL
// OFFSET: 0x006CF2A0/0x1403892A0
ClientWargameRequestSent::ClientWargameRequestSent(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d127b43b IDENTICAL/IDENTICAL
// OFFSET: 0x006A24D0/0x14034F9F0
ClientWargameRequestSent::~ClientWargameRequestSent() {
}

// PROVEN: 4c6606d9 IDENTICAL/IDENTICAL
// OFFSET: 0x006B7170/0x140369E80
void ClientWargameRequestSent::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }
}

// PROVEN: ee6506f2 IDENTICAL/UNPROVEN
// OFFSET: 0x006C7F80/0x14037EEC0
bool ClientWargameRequestSent::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 8245890a IDENTICAL/IDENTICAL
// OFFSET: 0x006BBFE0/0x140370200
bool ClientWargameRequestSent::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0017 = bits.ReadBit();
    this->unk_0016 = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0015 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
    if (this->unk_0015) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0015 ^= byteValue; }
    if (this->unk_0016) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0016 ^= byteValue; }
    if (this->unk_0013) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0013 ^= byteValue; }
    if (this->unk_0010) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0010 ^= byteValue; }
    if (this->unk_0017) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0017 ^= byteValue; }
    if (this->unk_0014) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0014 ^= byteValue; }
    if (this->unk_0012) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0012 ^= byteValue; }
    if (this->unk_0011) { uint8_t byteValue = 0; msg->Get(byteValue); this->unk_0011 ^= byteValue; }

    return msg->IsRead() != 0;
}

// PROVEN: 8bcbc82c IDENTICAL/UNPROVEN
// OFFSET: 0x006BAD90/0x14036EA20
bool ClientWargameRequestSent::Handle() {
    if (ClientWargameRequestSent::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC58.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC58.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWargameRequestSent*)>(reinterpret_cast<uintptr_t>(ClientWargameRequestSent::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: f5b2e94b IDENTICAL/UNPROVEN
// OFFSET: 0x006A1EB0/0x14036EC10
ClientWeeklyLastReset::ClientWeeklyLastReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// PROVEN: a736ab3a IDENTICAL/UNPROVEN
// OFFSET: 0x006A25E0/0x14034EC80
ClientWeeklyLastReset::~ClientWeeklyLastReset() {
}

// PROVEN: a24b83fe IDENTICAL/IDENTICAL
// OFFSET: 0x006CDE50/0x140383D70
bool ClientWeeklyLastReset::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// PROVEN: 9eae1332 IDENTICAL/UNPROVEN
// OFFSET: 0x006B41E0/0x140366020
bool ClientWeeklyLastReset::Handle() {
    if (ClientWeeklyLastReset::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDC80.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDC80.m_left->Get();
        return reinterpret_cast<bool (*)(ClientWeeklyLastReset*)>(reinterpret_cast<uintptr_t>(ClientWeeklyLastReset::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 9598ff82 IDENTICAL/IDENTICAL
// OFFSET: 0x006BF410/0x1403744A0
ClientBattlefieldMgrEntered::ClientBattlefieldMgrEntered(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 51aa227a IDENTICAL/IDENTICAL
// OFFSET: 0x006A17C0/0x14034EB40
ClientBattlefieldMgrEntered::~ClientBattlefieldMgrEntered() {
}

// PROVEN: 123572fe IDENTICAL/IDENTICAL
// OFFSET: 0x006A4DC0/0x140352CD0
void ClientBattlefieldMgrEntered::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018 = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }
}

// PROVEN: 053d9fa6 IDENTICAL/UNPROVEN
// OFFSET: 0x006BC420/0x1403707C0
bool ClientBattlefieldMgrEntered::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 8466bea5 IDENTICAL/IDENTICAL
// OFFSET: 0x006B66C0/0x1403690F0
bool ClientBattlefieldMgrEntered::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018 = bits.ReadBit();
    this->unk_0019 = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_001A = bits.ReadBit();
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 94177b49 IDENTICAL/UNPROVEN
// OFFSET: 0x006B56E0/0x140367A90
bool ClientBattlefieldMgrEntered::Handle() {
    if (ClientBattlefieldMgrEntered::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD2B4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD2B4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrEntered*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrEntered::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: c1a99d0a IDENTICAL/IDENTICAL
// OFFSET: 0x006C3150/0x140378B60
ClientBattlefieldMgrQueueInvite::ClientBattlefieldMgrQueueInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ce3184e IDENTICAL/IDENTICAL
// OFFSET: 0x006A2CD0/0x140350220
ClientBattlefieldMgrQueueInvite::~ClientBattlefieldMgrQueueInvite() {
}

// OFFSET: 0x006A8440/0x1403571B0
void ClientBattlefieldMgrQueueInvite::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018 = bits.ReadBit() ? -1 : 0;
    this->unk_0024 = !bits.ReadBit();
    this->unk_0028 = !bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0020 = !bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_001C = !bits.ReadBit();
    this->unk_0030 = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    this->unk_002C = !bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0028) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0028 = value;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }
    if (this->unk_0024) {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0018 != -1) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    if (this->unk_002C) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_002C = value;
    }
    if (this->unk_0020) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_001C) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_001C = value;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }
}

// PROVEN: 7840e9a3 IDENTICAL/UNPROVEN
// OFFSET: 0x006AE830/0x14035EF50
bool ClientBattlefieldMgrQueueInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A7A50/0x140356520
bool ClientBattlefieldMgrQueueInvite::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018 = bits.ReadBit() ? -1 : 0;
    this->unk_0024 = !bits.ReadBit();
    this->unk_0028 = !bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0020 = !bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_001C = !bits.ReadBit();
    this->unk_0030 = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    this->unk_002C = !bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0028) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0028 = value;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }
    if (this->unk_0024) {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0018 != -1) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    if (this->unk_002C) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_002C = value;
    }
    if (this->unk_0020) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_001C) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_001C = value;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 3ada724e IDENTICAL/UNPROVEN
// OFFSET: 0x006B83A0/0x14036B510
bool ClientBattlefieldMgrQueueInvite::Handle() {
    if (ClientBattlefieldMgrQueueInvite::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF964.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF964.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrQueueInvite*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrQueueInvite::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 1c2c4eb5 IDENTICAL/IDENTICAL
// OFFSET: 0x006BC280/0x1403705B0
ClientBattlefieldMgrQueueRequestResponse::ClientBattlefieldMgrQueueRequestResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 32d2c61c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2130/0x14034F580
ClientBattlefieldMgrQueueRequestResponse::~ClientBattlefieldMgrQueueRequestResponse() {
}

// PROVEN: 143bad25 DIFFERENT/IDENTICAL
// OFFSET: 0x006A8820/0x1403576A0
void ClientBattlefieldMgrQueueRequestResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0028[6] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    *reinterpret_cast<uint64_t*>(this->unk_0018) = !bits.ReadBit();
    this->unk_0028[4] = bits.ReadBit();

    if (*reinterpret_cast<uint64_t*>(this->unk_0018)) {
        this->unk_0018[7] = bits.ReadBit();
        this->unk_0018[3] = bits.ReadBit();
        this->unk_0018[0] = bits.ReadBit();
        this->unk_0018[4] = bits.ReadBit();
        this->unk_0018[2] = bits.ReadBit();
        this->unk_0018[6] = bits.ReadBit();
        this->unk_0018[1] = bits.ReadBit();
        this->unk_0018[5] = bits.ReadBit();
    }

    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[2] = bits.ReadBit();

    if (*reinterpret_cast<uint64_t*>(this->unk_0018)) {
        if (this->unk_0018[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[2] ^= byteValue;
        }
        if (this->unk_0018[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[5] ^= byteValue;
        }
        if (this->unk_0018[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[3] ^= byteValue;
        }
        if (this->unk_0018[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[0] ^= byteValue;
        }
        if (this->unk_0018[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[4] ^= byteValue;
        }
        if (this->unk_0018[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[6] ^= byteValue;
        }
        if (this->unk_0018[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[1] ^= byteValue;
        }
        if (this->unk_0018[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[7] ^= byteValue;
        }
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0012 = value;
    }
    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }
}

// PROVEN: 24a07c9e IDENTICAL/UNPROVEN
// OFFSET: 0x006B1290/0x1403623A0
bool ClientBattlefieldMgrQueueRequestResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A3D00/0x1403515C0
bool ClientBattlefieldMgrQueueRequestResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0028[6] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    *reinterpret_cast<uint64_t*>(this->unk_0018) = !bits.ReadBit();
    this->unk_0028[4] = bits.ReadBit();

    if (*reinterpret_cast<uint64_t*>(this->unk_0018)) {
        this->unk_0018[7] = bits.ReadBit();
        this->unk_0018[3] = bits.ReadBit();
        this->unk_0018[0] = bits.ReadBit();
        this->unk_0018[4] = bits.ReadBit();
        this->unk_0018[2] = bits.ReadBit();
        this->unk_0018[6] = bits.ReadBit();
        this->unk_0018[1] = bits.ReadBit();
        this->unk_0018[5] = bits.ReadBit();
    }

    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[2] = bits.ReadBit();

    if (*reinterpret_cast<uint64_t*>(this->unk_0018)) {
        if (this->unk_0018[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[2] ^= byteValue;
        }
        if (this->unk_0018[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[5] ^= byteValue;
        }
        if (this->unk_0018[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[3] ^= byteValue;
        }
        if (this->unk_0018[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[0] ^= byteValue;
        }
        if (this->unk_0018[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[4] ^= byteValue;
        }
        if (this->unk_0018[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[6] ^= byteValue;
        }
        if (this->unk_0018[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[1] ^= byteValue;
        }
        if (this->unk_0018[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0018[7] ^= byteValue;
        }
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0012 = value;
    }
    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 1ee3aa04 IDENTICAL/IDENTICAL
// OFFSET: 0x006A18F0/0x14034ECA0
bool ClientBattlefieldMgrQueueRequestResponse::IsValid() const {
    const uint64_t& guid = *reinterpret_cast<const uint64_t*>(this->unk_0018);
    const uint64_t& queueID = *reinterpret_cast<const uint64_t*>(this->unk_0028);

    if ((!guid || (!(guid & 0xF000000000000000ull) && (guid & 0xF07FFFFFFFFFFFFFull))) && static_cast<int8_t>(this->unk_0012) >= 0 && static_cast<int8_t>(this->unk_0012) < 44 && (queueID & 0xF000000000000000ull) == 0x1000000000000000ull && (queueID & 0x0FF0000000000000ull) == 0x0F10000000000000ull) {
        return true;
    }

    return false;
}

// PROVEN: fb3e4dbc IDENTICAL/UNPROVEN
// OFFSET: 0x006A4390/0x140351E00
bool ClientBattlefieldMgrQueueRequestResponse::Handle() {
    if (ClientBattlefieldMgrQueueRequestResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD90C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD90C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldMgrQueueRequestResponse*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldMgrQueueRequestResponse::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: a6ef27cd IDENTICAL/IDENTICAL
// OFFSET: 0x006AAD40/0x14035A5F0
ClientBattlefieldStatus::ClientBattlefieldStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: dda094dc IDENTICAL/UNPROVEN
// OFFSET: 0x006A29E0/0x14034FEB0
ClientBattlefieldStatus::~ClientBattlefieldStatus() {
}

// PROVEN: 72b382ac IDENTICAL/DIFFERENT
// OFFSET: 0x006A7D00/0x140356880
void ClientBattlefieldStatus::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_type = value;
    }
    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_id = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_time = value;
    }
    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }
}

// PROVEN: 0325bb09 IDENTICAL/UNPROVEN
// OFFSET: 0x006D59F0/0x140392D30
bool ClientBattlefieldStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C6510/0x14037CA50
bool ClientBattlefieldStatus::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_type = value;
    }
    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_id = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_time = value;
    }
    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 8375cfcd IDENTICAL/UNPROVEN
// OFFSET: 0x006A7A00/0x1403564C0
bool ClientBattlefieldStatus::Handle() {
    if (ClientBattlefieldStatus::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFAEC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFAEC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatus*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatus::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: b0485291 IDENTICAL/IDENTICAL
// OFFSET: 0x006BF890/0x140374A40
ClientBattlefieldStatusActive::ClientBattlefieldStatusActive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4251f8fa IDENTICAL/UNPROVEN
// OFFSET: 0x006ADC20/0x14035E050
ClientBattlefieldStatusActive::~ClientBattlefieldStatusActive() {
}

// PROVEN: 5e52777b IDENTICAL/DIFFERENT
// OFFSET: 0x006AD770/0x14035DA60
void ClientBattlefieldStatusActive::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->unk_004C = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    if (this->m_header.m_queueID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[4] ^= byteValue;
    }
    if (this->m_header.m_queueID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[5] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[1] ^= byteValue;
    }
    if (this->m_header.m_queueID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[6] ^= byteValue;
    }
    if (this->m_header.m_queueID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[3] ^= byteValue;
    }
    if (this->m_header.m_queueID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[7] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_time = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMin = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_id = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_teamSize = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_type = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0050 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMax = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    if (this->m_header.m_queueID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_instanceID = value;
    }
    if (this->m_header.m_queueID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue;
    }
}

// PROVEN: 24dece1b IDENTICAL/UNPROVEN
// OFFSET: 0x006C4710/0x14037A610
bool ClientBattlefieldStatusActive::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BD4B0/0x140371CE0
bool ClientBattlefieldStatusActive::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->unk_004C = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    if (this->m_header.m_queueID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[4] ^= byteValue;
    }
    if (this->m_header.m_queueID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[5] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[1] ^= byteValue;
    }
    if (this->m_header.m_queueID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[6] ^= byteValue;
    }
    if (this->m_header.m_queueID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[3] ^= byteValue;
    }
    if (this->m_header.m_queueID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[7] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_time = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMin = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_id = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_teamSize = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_type = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0050 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMax = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->m_header.m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    if (this->m_header.m_queueID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_instanceID = value;
    }
    if (this->m_header.m_queueID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: a424124d IDENTICAL/UNPROVEN
// OFFSET: 0x006A9820/0x140358B70
bool ClientBattlefieldStatusActive::Handle() {
    if (ClientBattlefieldStatusActive::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA54.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA54.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatusActive*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatusActive::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// OFFSET: 0x006B6DD0/0x140369A00
ClientDisplayGameError::ClientDisplayGameError(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->unk_0010 = false;
    this->unk_0018 = false;
    this->Unpack(msg, true);
}

// PROVEN: ecdf6b23 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F40/0x14034E100
ClientDisplayGameError::~ClientDisplayGameError() {
}

// PROVEN: fe27243b IDENTICAL/IDENTICAL
// OFFSET: 0x006B6380/0x140368AF0
void ClientDisplayGameError::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();

    if (this->unk_0010) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }

    if (this->unk_0018) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_001C = value;
    }
}

// PROVEN: 307e47e0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C87E0/0x14037F8B0
bool ClientDisplayGameError::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: cfdf5f86 IDENTICAL/IDENTICAL
// OFFSET: 0x006B2D60/0x1403645B0
bool ClientDisplayGameError::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0018 = bits.ReadBit();

    if (this->unk_0010) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }

    if (this->unk_0018) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_001C = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6f3a38e3 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6E90/0x140369AE0
bool ClientDisplayGameError::Handle() {
    if (ClientDisplayGameError::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3AC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3AC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDisplayGameError*)>(reinterpret_cast<uintptr_t>(ClientDisplayGameError::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: aba1b4a5 IDENTICAL/IDENTICAL
// OFFSET: 0x006C6F50/0x14037D9A0
ClientPvpOptionsEnabled::ClientPvpOptionsEnabled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 41d02dbb IDENTICAL/IDENTICAL
// OFFSET: 0x006A15C0/0x14034E8C0
ClientPvpOptionsEnabled::~ClientPvpOptionsEnabled() {
}

// PROVEN: d6910055 IDENTICAL/IDENTICAL
// OFFSET: 0x006BAD20/0x14036E9A0
void ClientPvpOptionsEnabled::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
}

// PROVEN: b748487d IDENTICAL/UNPROVEN
// OFFSET: 0x006C45D0/0x14037A480
bool ClientPvpOptionsEnabled::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b40d3e52 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0850/0x140375BC0
bool ClientPvpOptionsEnabled::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();

    return msg->IsRead() != 0;
}

// PROVEN: 6e2c5ae5 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6E50/0x140369A80
bool ClientPvpOptionsEnabled::Handle() {
    if (ClientPvpOptionsEnabled::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDCEC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDCEC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPvpOptionsEnabled*)>(reinterpret_cast<uintptr_t>(ClientPvpOptionsEnabled::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 69033716 IDENTICAL/IDENTICAL
// OFFSET: 0x006A12D0/0x14034E5A0
ClientShowRatings::ClientShowRatings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 135900e6 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F00/0x14034DFE0
ClientShowRatings::~ClientShowRatings() {
}

// PROVEN: 6cb55238 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1150/0x14034E3F0
void ClientShowRatings::Unpack(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 4; i++) {
        float value = 0;
        msg->Get(value);
        this->unk_0040[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        float value = 0;
        msg->Get(value);
        this->unk_0020[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010[i] = value;
    }
}

// PROVEN: c1cafeb0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C48A0/0x14037A7E0
bool ClientShowRatings::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 777cef7e IDENTICAL/IDENTICAL
// OFFSET: 0x006B8F50/0x14036C3B0
bool ClientShowRatings::Read(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 4; i++) {
        float value = 0;
        msg->Get(value);
        this->unk_0040[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        float value = 0;
        msg->Get(value);
        this->unk_0020[i] = value;
    }

    for (uint32_t i = 0; i < 4; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010[i] = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 5b12d6c6 IDENTICAL/UNPROVEN
// OFFSET: 0x006BB740/0x14036F6E0
bool ClientShowRatings::Handle() {
    if (ClientShowRatings::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE4F8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE4F8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientShowRatings*)>(reinterpret_cast<uintptr_t>(ClientShowRatings::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: f6134b71 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4720/0x14037A620
ClientBattlefieldStatusNeedconfirmation::ClientBattlefieldStatusNeedconfirmation(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 457e768c IDENTICAL/UNPROVEN
// OFFSET: 0x006C0D90/0x140376230
ClientBattlefieldStatusNeedconfirmation::~ClientBattlefieldStatusNeedconfirmation() {
}

// PROVEN: d63cb886 IDENTICAL/DIFFERENT
// OFFSET: 0x006BCD50/0x140371340
void ClientBattlefieldStatusNeedconfirmation::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_instanceID = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMin = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_id = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_time = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMax = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_type = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_teamSize = value;
    }

    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    if (this->m_header.m_queueID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[6] ^= byteValue;
    }
    if (this->m_header.m_queueID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[7] ^= byteValue;
    }
    if (this->m_header.m_queueID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[2] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_header.m_queueID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[4] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    if (this->m_header.m_queueID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_header.m_queueID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[1] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[3] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue;
    }
}

// PROVEN: 728b4c32 IDENTICAL/UNPROVEN
// OFFSET: 0x006D7770/0x140395420
bool ClientBattlefieldStatusNeedconfirmation::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CA490/0x140382460
bool ClientBattlefieldStatusNeedconfirmation::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_instanceID = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMin = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_id = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_time = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_rangeMax = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_header.m_ticket.m_type = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_header.m_teamSize = value;
    }

    JamBitReader bits(msg);
    this->m_header.m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_header.m_queueID[2] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_header.m_queueID[6] = bits.ReadBit();
    this->m_header.m_queueID[3] = bits.ReadBit();
    this->m_header.m_registeredMatch = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_header.m_queueID[7] = bits.ReadBit();
    this->m_header.m_queueID[0] = bits.ReadBit();
    this->m_header.m_queueID[4] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_header.m_queueID[5] = bits.ReadBit();
    this->m_header.m_queueID[1] = bits.ReadBit();
    this->m_header.m_ticket.m_requesterGuid[0] = bits.ReadBit();
    if (this->m_header.m_queueID[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[6] ^= byteValue;
    }
    if (this->m_header.m_queueID[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[7] ^= byteValue;
    }
    if (this->m_header.m_queueID[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[2] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_header.m_queueID[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[4] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    if (this->m_header.m_queueID[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[0] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_header.m_queueID[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[1] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_header.m_queueID[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_queueID[3] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    if (this->m_header.m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_header.m_ticket.m_requesterGuid[3] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 2b36b29b IDENTICAL/UNPROVEN
// OFFSET: 0x006B12F0/0x140362400
bool ClientBattlefieldStatusNeedconfirmation::Handle() {
    if (ClientBattlefieldStatusNeedconfirmation::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC7C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC7C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldStatusNeedconfirmation*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldStatusNeedconfirmation::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: f7c63342 IDENTICAL/IDENTICAL
// OFFSET: 0x006B2E00/0x1403646F0
ClientGameEventDebugLog::ClientGameEventDebugLog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 36aab306 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1680/0x14034E9C0
ClientGameEventDebugLog::~ClientGameEventDebugLog() {
}

// PROVEN: dad56f15 IDENTICAL/DIFFERENT
// OFFSET: 0x006AF3B0/0x14035FD70
void ClientGameEventDebugLog::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_00E0 = value;
    }

    JamBitReader bits(msg);
    *reinterpret_cast<uint32_t*>(this->unk_00E4) = bits.ReadBits<8>();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0018[3] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->unk_0060) = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->unk_0020) = bits.ReadBits<7>();
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0020);
        msg->GetData(this->unk_0020, length);
        this->unk_0020[length] = 0;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0060);
        msg->GetData(this->unk_0060, length);
        this->unk_0060[length] = 0;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_00E4);
        msg->GetData(this->unk_00E4, length);
        this->unk_00E4[length] = 0;
    }
}

// PROVEN: 522216a3 IDENTICAL/UNPROVEN
// OFFSET: 0x006D71F0/0x140379D90
bool ClientGameEventDebugLog::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e94f6ebb IDENTICAL/DIFFERENT
// OFFSET: 0x006C4990/0x140355320
bool ClientGameEventDebugLog::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_00E0 = value;
    }

    JamBitReader bits(msg);
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->unk_00E4) = length;

        if (length >= sizeof(this->unk_00E4)) {
            return false;
        }
    }
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0018[3] = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<8>();
        *reinterpret_cast<uint32_t*>(this->unk_0060) = length;

        if (length >= sizeof(this->unk_0060)) {
            return false;
        }
    }
    {
        uint32_t length = bits.ReadBits<7>();
        *reinterpret_cast<uint32_t*>(this->unk_0020) = length;

        if (length >= sizeof(this->unk_0020)) {
            return false;
        }
    }
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0020);
        msg->GetData(this->unk_0020, length);
        this->unk_0020[length] = 0;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0060);
        msg->GetData(this->unk_0060, length);
        this->unk_0060[length] = 0;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_00E4);
        msg->GetData(this->unk_00E4, length);
        this->unk_00E4[length] = 0;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: d8baa43e IDENTICAL/IDENTICAL
// OFFSET: 0x006A78F0/0x14034E050
bool ClientGameEventDebugLog::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->unk_0060))) {
                break;
            }

            terminated |= this->unk_0060[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->unk_0020))) {
                break;
            }

            terminated |= this->unk_0020[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->unk_00E4))) {
                break;
            }

            terminated |= this->unk_00E4[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: 1ec68c54 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD5C0/0x14035D820
bool ClientGameEventDebugLog::Handle() {
    if (ClientGameEventDebugLog::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFCC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFCC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGameEventDebugLog*)>(reinterpret_cast<uintptr_t>(ClientGameEventDebugLog::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 2fce4e81 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4820/0x14037A750
ClientGuildAchievementDeleted::ClientGuildAchievementDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 39a52751 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2120/0x14034F570
ClientGuildAchievementDeleted::~ClientGuildAchievementDeleted() {
}

// PROVEN: c3d12b29 IDENTICAL/IDENTICAL
// OFFSET: 0x006A5D00/0x140353D20
void ClientGuildAchievementDeleted::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0038[4] = bits.ReadBit();
    this->unk_0038[1] = bits.ReadBit();
    this->unk_0038[2] = bits.ReadBit();
    this->unk_0038[3] = bits.ReadBit();
    this->unk_0038[0] = bits.ReadBit();
    this->unk_0038[7] = bits.ReadBit();
    this->unk_0038[5] = bits.ReadBit();
    this->unk_0038[6] = bits.ReadBit();
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    if (this->unk_0038[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[5] ^= byteValue;
    }
    if (this->unk_0038[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[1] ^= byteValue;
    }
    if (this->unk_0038[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[3] ^= byteValue;
    }
    if (this->unk_0038[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[6] ^= byteValue;
    }
    if (this->unk_0038[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[0] ^= byteValue;
    }
    if (this->unk_0038[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[7] ^= byteValue;
    }
    *msg >> this->unk_0010;
    if (this->unk_0038[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[4] ^= byteValue;
    }
    if (this->unk_0038[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[2] ^= byteValue;
    }
}

// PROVEN: a5b1b263 IDENTICAL/UNPROVEN
// OFFSET: 0x006C50D0/0x14037AE00
bool ClientGuildAchievementDeleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f502cb28 IDENTICAL/IDENTICAL
// OFFSET: 0x006B3280/0x140364C70
bool ClientGuildAchievementDeleted::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0038[4] = bits.ReadBit();
    this->unk_0038[1] = bits.ReadBit();
    this->unk_0038[2] = bits.ReadBit();
    this->unk_0038[3] = bits.ReadBit();
    this->unk_0038[0] = bits.ReadBit();
    this->unk_0038[7] = bits.ReadBit();
    this->unk_0038[5] = bits.ReadBit();
    this->unk_0038[6] = bits.ReadBit();
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    if (this->unk_0038[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[5] ^= byteValue;
    }
    if (this->unk_0038[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[1] ^= byteValue;
    }
    if (this->unk_0038[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[3] ^= byteValue;
    }
    if (this->unk_0038[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[6] ^= byteValue;
    }
    if (this->unk_0038[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[0] ^= byteValue;
    }
    if (this->unk_0038[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[7] ^= byteValue;
    }
    *msg >> this->unk_0010;
    if (this->unk_0038[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[4] ^= byteValue;
    }
    if (this->unk_0038[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0038[2] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: d23d78e6 IDENTICAL/UNPROVEN
// OFFSET: 0x006BEDA0/0x140373CD0
bool ClientGuildAchievementDeleted::Handle() {
    if (ClientGuildAchievementDeleted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE850.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE850.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildAchievementDeleted*)>(reinterpret_cast<uintptr_t>(ClientGuildAchievementDeleted::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 6fafa1a8 IDENTICAL/IDENTICAL
// OFFSET: 0x006BE920/0x1403736E0
ClientGuildAchievementEarned::ClientGuildAchievementEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: cc55889c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2F40/0x140350450
ClientGuildAchievementEarned::~ClientGuildAchievementEarned() {
}

// PROVEN: 0430dc3b IDENTICAL/IDENTICAL
// OFFSET: 0x006AF8B0/0x1403602F0
void ClientGuildAchievementEarned::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    *msg >> this->unk_0020;
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
}

// PROVEN: 0ec95eac IDENTICAL/UNPROVEN
// OFFSET: 0x006C4900/0x14037A840
bool ClientGuildAchievementEarned::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2446e4cf IDENTICAL/IDENTICAL
// OFFSET: 0x006A7380/0x140355D90
bool ClientGuildAchievementEarned::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    *msg >> this->unk_0020;
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: b1430182 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5440/0x140367750
bool ClientGuildAchievementEarned::Handle() {
    if (ClientGuildAchievementEarned::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFC14.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFC14.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildAchievementEarned*)>(reinterpret_cast<uintptr_t>(ClientGuildAchievementEarned::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 2d031ea4 IDENTICAL/IDENTICAL
// OFFSET: 0x006C3FD0/0x140379F90
ClientGuildCriteriaDeleted::ClientGuildCriteriaDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4aa0023f IDENTICAL/IDENTICAL
// OFFSET: 0x006A1F00/0x14034F380
ClientGuildCriteriaDeleted::~ClientGuildCriteriaDeleted() {
}

// PROVEN: ee54fcf6 IDENTICAL/IDENTICAL
// OFFSET: 0x006BC2C0/0x1403705F0
void ClientGuildCriteriaDeleted::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }
}

// PROVEN: 60687d95 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5CD0/0x140368240
bool ClientGuildCriteriaDeleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 977fb357 IDENTICAL/IDENTICAL
// OFFSET: 0x006AD600/0x14035D880
bool ClientGuildCriteriaDeleted::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010[6] = bits.ReadBit();
    this->unk_0010[5] = bits.ReadBit();
    this->unk_0010[7] = bits.ReadBit();
    this->unk_0010[0] = bits.ReadBit();
    this->unk_0010[1] = bits.ReadBit();
    this->unk_0010[3] = bits.ReadBit();
    this->unk_0010[2] = bits.ReadBit();
    this->unk_0010[4] = bits.ReadBit();
    if (this->unk_0010[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[2] ^= byteValue;
    }
    if (this->unk_0010[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[3] ^= byteValue;
    }
    if (this->unk_0010[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[4] ^= byteValue;
    }
    if (this->unk_0010[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[1] ^= byteValue;
    }
    if (this->unk_0010[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[7] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    if (this->unk_0010[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[5] ^= byteValue;
    }
    if (this->unk_0010[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[0] ^= byteValue;
    }
    if (this->unk_0010[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0010[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: cd3c8310 IDENTICAL/UNPROVEN
// OFFSET: 0x006BFF80/0x1403751F0
bool ClientGuildCriteriaDeleted::Handle() {
    if (ClientGuildCriteriaDeleted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF304.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF304.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildCriteriaDeleted*)>(reinterpret_cast<uintptr_t>(ClientGuildCriteriaDeleted::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 62e6a727 IDENTICAL/IDENTICAL
// OFFSET: 0x006B5BB0/0x1403680E0
ClientLfgQueueStatus::ClientLfgQueueStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 60682f9f IDENTICAL/UNPROVEN
// OFFSET: 0x006A7A40/0x14034E1A0
ClientLfgQueueStatus::~ClientLfgQueueStatus() {
}

// PROVEN: b7d6d10f IDENTICAL/DIFFERENT
// OFFSET: 0x006AF680/0x140360010
void ClientLfgQueueStatus::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }

    for (uint32_t i = 0; i < 3; i++) {
        {
            uint8_t value = 0;
            msg->Get(value);
            this->unk_0048[i] = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_003C[i] = value;
        }
    }

    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_time = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_id = value;
    }
    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_type = value;
    }
}

// PROVEN: 73a6a93d IDENTICAL/UNPROVEN
// OFFSET: 0x006BE910/0x1403736D0
bool ClientLfgQueueStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 96bcb2cd IDENTICAL/DIFFERENT
// OFFSET: 0x006B5DD0/0x140368370
bool ClientLfgQueueStatus::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }

    for (uint32_t i = 0; i < 3; i++) {
        {
            uint8_t value = 0;
            msg->Get(value);
            this->unk_0048[i] = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_003C[i] = value;
        }
    }

    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_time = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_id = value;
    }
    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }
    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->m_ticket.m_type = value;
    }

    return msg->IsRead() && this->m_ticket.IsValid();
}

// PROVEN: 0ab335a0 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF080/0x140374090
bool ClientLfgQueueStatus::Handle() {
    if (ClientLfgQueueStatus::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFCA4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFCA4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgQueueStatus*)>(reinterpret_cast<uintptr_t>(ClientLfgQueueStatus::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: ff58e8f1 IDENTICAL/IDENTICAL
// OFFSET: 0x006F2300/0x1403B8A40
ClientServerInfoResponse::ClientServerInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ae5d800 IDENTICAL/IDENTICAL
// OFFSET: 0x006E97C0/0x14039CDE0
ClientServerInfoResponse::~ClientServerInfoResponse() {
}

// OFFSET: 0x006E7490/0x1403A9440
void ClientServerInfoResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0020.SetCount(bits.ReadBits<25>());
    this->unk_0010.SetCount(bits.ReadBits<25>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0010.m_data[i];
        *reinterpret_cast<uint32_t*>(field.m_value) = bits.ReadBits<9>();
        *reinterpret_cast<uint32_t*>(field.m_name) = bits.ReadBits<7>();
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0020.m_data[i];
        *reinterpret_cast<uint32_t*>(field.m_name) = bits.ReadBits<7>();
        *reinterpret_cast<uint32_t*>(field.m_value) = bits.ReadBits<9>();
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0020.m_data[i];
        uint32_t length = *reinterpret_cast<uint32_t*>(field.m_value);
        msg->GetData(field.m_value, length);
        field.m_value[length] = 0;
        length = *reinterpret_cast<uint32_t*>(field.m_name);
        msg->GetData(field.m_name, length);
        field.m_name[length] = 0;
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0010.m_data[i];
        uint32_t length = *reinterpret_cast<uint32_t*>(field.m_name);
        msg->GetData(field.m_name, length);
        field.m_name[length] = 0;
        length = *reinterpret_cast<uint32_t*>(field.m_value);
        msg->GetData(field.m_value, length);
        field.m_value[length] = 0;
    }
}

// PROVEN: d29aa9b9 IDENTICAL/UNPROVEN
// OFFSET: 0x006F2540/0x1403B8D80
bool ClientServerInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ED930/0x1403B2360
bool ClientServerInfoResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count2 = bits.ReadBits<25>();

    if (msg->m_size - msg->m_read < ((16 * count2 + 7) >> 3)) {
        return false;
    }

    this->unk_0020.SetCount(count2);
    uint32_t count = bits.ReadBits<25>();

    if (msg->m_size - msg->m_read < ((16 * count + 7) >> 3)) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_value) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_value) >= sizeof(this->unk_0010.m_data[i].m_value)) {
            return false;
        }

        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_name) >= sizeof(this->unk_0010.m_data[i].m_name)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0020.m_data[i].m_name) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0020.m_data[i].m_name) >= sizeof(this->unk_0020.m_data[i].m_name)) {
            return false;
        }

        *reinterpret_cast<uint32_t*>(this->unk_0020.m_data[i].m_value) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0020.m_data[i].m_value) >= sizeof(this->unk_0020.m_data[i].m_value)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0020.m_data[i];
        uint32_t length = *reinterpret_cast<uint32_t*>(field.m_value);
        msg->GetData(field.m_value, length);
        field.m_value[length] = 0;
        length = *reinterpret_cast<uint32_t*>(field.m_name);
        msg->GetData(field.m_name, length);
        field.m_name[length] = 0;
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        JamCliServerInfoLine& field = this->unk_0010.m_data[i];
        uint32_t length = *reinterpret_cast<uint32_t*>(field.m_name);
        msg->GetData(field.m_name, length);
        field.m_name[length] = 0;
        length = *reinterpret_cast<uint32_t*>(field.m_value);
        msg->GetData(field.m_value, length);
        field.m_value[length] = 0;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 167a54bb IDENTICAL/IDENTICAL
// OFFSET: 0x006AA570/0x140359C00
bool ClientServerInfoResponse::IsValid() const {
    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        if (!this->unk_0010.m_data[i].IsValid()) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (!this->unk_0020.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: 80e25735 IDENTICAL/UNPROVEN
// OFFSET: 0x006A32B0/0x140350A20
bool ClientServerInfoResponse::Handle() {
    if (ClientServerInfoResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE754.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE754.m_left->Get();
        return reinterpret_cast<bool (*)(ClientServerInfoResponse*)>(reinterpret_cast<uintptr_t>(ClientServerInfoResponse::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: d4bbaf7c IDENTICAL/IDENTICAL
// OFFSET: 0x006DEB20/0x14039E670
ClientServerPerf::ClientServerPerf(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d862c444 IDENTICAL/IDENTICAL
// OFFSET: 0x006D9240/0x140397640
ClientServerPerf::~ClientServerPerf() {
}

// OFFSET: 0x006DBAC0/0x14039AAB0
void ClientServerPerf::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<25>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) = bits.ReadBits<9>();
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text);
        msg->GetData(this->unk_0010.m_data[i].m_text, length);
        this->unk_0010.m_data[i].m_text[length] = 0;
    }
}

// PROVEN: 3e3ccc8a IDENTICAL/UNPROVEN
// OFFSET: 0x006E59B0/0x1403A6F00
bool ClientServerPerf::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E20E0/0x1403A29A0
bool ClientServerPerf::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<25>();

    if (msg->m_size - msg->m_read < ((9 * count + 7) >> 3)) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) >= sizeof(this->unk_0010.m_data[i].m_text)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text);
        msg->GetData(this->unk_0010.m_data[i].m_text, length);
        this->unk_0010.m_data[i].m_text[length] = 0;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 57f247b7 IDENTICAL/NONE
// OFFSET: 0x006ABC80
bool ClientServerPerf::IsValid() const {
    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            bool terminated = false;

            for (uint32_t j = 0; !terminated; j++) {
                if (j >= sizeof(this->unk_0010.m_data[i].m_text)) {
                    break;
                }

                terminated |= this->unk_0010.m_data[i].m_text[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: 185a4e82 IDENTICAL/UNPROVEN
// OFFSET: 0x006A3C10/0x140351480
bool ClientServerPerf::Handle() {
    if (ClientServerPerf::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD964.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD964.m_left->Get();
        return reinterpret_cast<bool (*)(ClientServerPerf*)>(reinterpret_cast<uintptr_t>(ClientServerPerf::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: acda6460 IDENTICAL/IDENTICAL
// OFFSET: 0x006DA0E0/0x1403986D0
ClientStreamingMoviesUpdate::ClientStreamingMoviesUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 161acd63 IDENTICAL/IDENTICAL
// OFFSET: 0x006DBA60/0x14039AA30
ClientStreamingMoviesUpdate::~ClientStreamingMoviesUpdate() {
}

// PROVEN: 039cbbeb IDENTICAL/IDENTICAL
// OFFSET: 0x006D9830/0x140397CF0
void ClientStreamingMoviesUpdate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<25>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint16_t value = 0;
        msg->Get(value);
        this->unk_0010.m_data[i] = value;
    }
}

// PROVEN: 6e6012d6 IDENTICAL/UNPROVEN
// OFFSET: 0x006DB2C0/0x140399BF0
bool ClientStreamingMoviesUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 65ee9fe1 DIFFERENT/IDENTICAL
// OFFSET: 0x006D7F60/0x140395E10
bool ClientStreamingMoviesUpdate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<25>();

    if (msg->m_size - msg->m_read < 2 * count) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint16_t value = 0;
        msg->Get(value);
        this->unk_0010.m_data[i] = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 1e462b1c IDENTICAL/UNPROVEN
// OFFSET: 0x006B4890/0x140366860
bool ClientStreamingMoviesUpdate::Handle() {
    if (ClientStreamingMoviesUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE774.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE774.m_left->Get();
        return reinterpret_cast<bool (*)(ClientStreamingMoviesUpdate*)>(reinterpret_cast<uintptr_t>(ClientStreamingMoviesUpdate::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 4135a446 IDENTICAL/IDENTICAL
// OFFSET: 0x006DD5D0/0x14039CA70
ClientLoadCufProfiles::ClientLoadCufProfiles(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0cd17aa4 UNPROVEN/IDENTICAL
// OFFSET: 0x006DD580/0x140394FD0
ClientLoadCufProfiles::~ClientLoadCufProfiles() {
}

// OFFSET: 0x006DA130/0x140398730
void ClientLoadCufProfiles::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_profiles.SetCount(bits.ReadBits<20>());

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        this->m_profiles.m_data[i].m_shown = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate10Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate5Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate25Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayHealPrediction = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivatePvE = bits.ReadBit();
        this->m_profiles.m_data[i].m_horizontalGroups = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate40Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate3Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayAggroHighlight = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayBorder = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate2Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayNonBossDebuffs = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayMainTankAndAssist = bits.ReadBit();
        this->m_profiles.m_data[i].m_locked = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivateSpec2 = bits.ReadBit();
        this->m_profiles.m_data[i].m_useClassColors = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayPowerBar = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivateSpec1 = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(this->m_profiles.m_data[i].m_name) = bits.ReadBits<8>();
        this->m_profiles.m_data[i].m_displayOnlyDispellableDebuffs = bits.ReadBit();
        this->m_profiles.m_data[i].m_keepGroupsTogether = bits.ReadBit();
        this->m_profiles.m_data[i].m_dynamicPosition = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate15Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayPets = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivatePvP = bits.ReadBit();
        this->m_profiles.m_data[i].m_fieldMask |= 0xFF81FFE1;
        this->m_profiles.m_data[i].m_fieldMask2 |= 0xF;
    }

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_leftOffset = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_frameHeight = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_bottomOffset = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_bottomPoint = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_topOffset = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_topPoint = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_healthText = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_sortBy = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_frameWidth = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_leftPoint = value;
        }
        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->m_profiles.m_data[i].m_name);
            msg->GetData(this->m_profiles.m_data[i].m_name, length);
            this->m_profiles.m_data[i].m_name[length] = 0;
        }
        this->m_profiles.m_data[i].m_fieldMask |= 0x7E001E;
    }
}

// PROVEN: b0602e4b IDENTICAL/UNPROVEN
// OFFSET: 0x006D9610/0x140397A50
bool ClientLoadCufProfiles::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D7790/0x140395440
bool ClientLoadCufProfiles::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<20>();

    if (msg->m_size - msg->m_read < ((33 * count + 7) >> 3) + 15 * count) {
        return false;
    }

    this->m_profiles.SetCount(count);

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        this->m_profiles.m_data[i].m_shown = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate10Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate5Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate25Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayHealPrediction = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivatePvE = bits.ReadBit();
        this->m_profiles.m_data[i].m_horizontalGroups = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate40Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate3Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayAggroHighlight = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayBorder = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate2Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayNonBossDebuffs = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayMainTankAndAssist = bits.ReadBit();
        this->m_profiles.m_data[i].m_locked = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivateSpec2 = bits.ReadBit();
        this->m_profiles.m_data[i].m_useClassColors = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayPowerBar = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivateSpec1 = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(this->m_profiles.m_data[i].m_name) = bits.ReadBits<8>();

        if (*reinterpret_cast<uint32_t*>(this->m_profiles.m_data[i].m_name) >= sizeof(this->m_profiles.m_data[i].m_name)) {
            return false;
        }

        this->m_profiles.m_data[i].m_displayOnlyDispellableDebuffs = bits.ReadBit();
        this->m_profiles.m_data[i].m_keepGroupsTogether = bits.ReadBit();
        this->m_profiles.m_data[i].m_dynamicPosition = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivate15Players = bits.ReadBit();
        this->m_profiles.m_data[i].m_displayPets = bits.ReadBit();
        this->m_profiles.m_data[i].m_autoActivatePvP = bits.ReadBit();
        this->m_profiles.m_data[i].m_fieldMask |= 0xFF81FFE1;
        this->m_profiles.m_data[i].m_fieldMask2 |= 0xF;
    }

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_leftOffset = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_frameHeight = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_bottomOffset = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_bottomPoint = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_topOffset = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_topPoint = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_healthText = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_sortBy = value;
        }
        {
            uint16_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_frameWidth = value;
        }
        {
            uint8_t value = 0;
            msg->Get(value);
            this->m_profiles.m_data[i].m_leftPoint = value;
        }
        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->m_profiles.m_data[i].m_name);
            msg->GetData(this->m_profiles.m_data[i].m_name, length);
            this->m_profiles.m_data[i].m_name[length] = 0;
        }
        this->m_profiles.m_data[i].m_fieldMask |= 0x7E001E;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 56e64677 IDENTICAL/UNPROVEN
// OFFSET: 0x006ACB90/0x14035CB40
bool ClientLoadCufProfiles::Handle() {
    if (ClientLoadCufProfiles::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCF68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCF68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLoadCufProfiles*)>(reinterpret_cast<uintptr_t>(ClientLoadCufProfiles::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b06f8592 IDENTICAL/IDENTICAL
// OFFSET: 0x006F2290/0x1403B89D0
ClientAccountInfoResponse::ClientAccountInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E20D0
ClientAccountInfoResponse::~ClientAccountInfoResponse() {
}

// OFFSET: 0x006E5560/0x1403A6960
void ClientAccountInfoResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    *reinterpret_cast<uint32_t*>(this->unk_052C) = bits.ReadBits<12>();
    this->unk_0510.SetCount(bits.ReadBits<23>());
    *reinterpret_cast<uint32_t*>(this->unk_0010) = bits.ReadBits<12>();
    this->unk_0A34.SetCount(bits.ReadBits<23>());
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0520 = value;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010);
        msg->GetData(this->unk_0010, length);
        this->unk_0010[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0A30 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0A2C = value;
    }
    for (uint32_t i = 0; i < this->unk_0510.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0510.m_data[i].unk_0004 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0510.m_data[i].unk_0000 = value;
        }
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0528 = value;
    }
    for (uint32_t i = 0; i < this->unk_0A34.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0A34.m_data[i].unk_0000 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0A34.m_data[i].unk_0004 = value;
        }
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_052C);
        msg->GetData(this->unk_052C, length);
        this->unk_052C[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0524 = value;
    }
}

// PROVEN: 686722c0 IDENTICAL/UNPROVEN
// OFFSET: 0x006E9890/0x1403AC8F0
bool ClientAccountInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DDC80/0x14039D310
bool ClientAccountInfoResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    {
        uint32_t length = bits.ReadBits<12>();
        *reinterpret_cast<uint32_t*>(this->unk_052C) = length;

        if (length >= sizeof(this->unk_052C)) {
            return false;
        }
    }
    uint32_t count2 = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count2) {
        return false;
    }

    this->unk_0510.SetCount(count2);
    {
        uint32_t length = bits.ReadBits<12>();
        *reinterpret_cast<uint32_t*>(this->unk_0010) = length;

        if (length >= sizeof(this->unk_0010)) {
            return false;
        }
    }
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count) {
        return false;
    }

    this->unk_0A34.SetCount(count);
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0520 = value;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010);
        msg->GetData(this->unk_0010, length);
        this->unk_0010[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0A30 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0A2C = value;
    }
    for (uint32_t i = 0; i < this->unk_0510.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0510.m_data[i].unk_0004 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0510.m_data[i].unk_0000 = value;
        }
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0528 = value;
    }
    for (uint32_t i = 0; i < this->unk_0A34.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0A34.m_data[i].unk_0000 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0A34.m_data[i].unk_0004 = value;
        }
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_052C);
        msg->GetData(this->unk_052C, length);
        this->unk_052C[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0524 = value;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: 7bf18dc6 IDENTICAL/NONE
// OFFSET: 0x006BF0E0
bool ClientAccountInfoResponse::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->unk_052C))) {
                break;
            }

            terminated |= this->unk_052C[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->unk_0010))) {
                break;
            }

            terminated |= this->unk_0010[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: 673b1b7e IDENTICAL/UNPROVEN
// OFFSET: 0x006B1CF0/0x140363100
bool ClientAccountInfoResponse::Handle() {
    if (ClientAccountInfoResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDC94.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDC94.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAccountInfoResponse*)>(reinterpret_cast<uintptr_t>(ClientAccountInfoResponse::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 73357bc3 IDENTICAL/IDENTICAL
// OFFSET: 0x006F4270/0x1403BB620
ClientDumpRideTicketsResponse::ClientDumpRideTicketsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9de1e567 IDENTICAL/IDENTICAL
// OFFSET: 0x006E9300/0x14039CAC0
ClientDumpRideTicketsResponse::~ClientDumpRideTicketsResponse() {
}

// OFFSET: 0x006ED370/0x1403B19D0
void ClientDumpRideTicketsResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<25>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) = bits.ReadBits<11>();
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text);
        msg->GetData(this->unk_0010.m_data[i].m_text, length);
        this->unk_0010.m_data[i].m_text[length] = 0;
    }
}

// PROVEN: e52ea37b IDENTICAL/UNPROVEN
// OFFSET: 0x006F42B0/0x1403BB670
bool ClientDumpRideTicketsResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EEA30/0x1403B3820
bool ClientDumpRideTicketsResponse::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<25>();

    if (msg->m_size - msg->m_read < ((11 * count + 7) >> 3)) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) = bits.ReadBits<11>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text) >= sizeof(this->unk_0010.m_data[i].m_text)) {
            return false;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0010.m_data[i].m_text);
        msg->GetData(this->unk_0010.m_data[i].m_text, length);
        this->unk_0010.m_data[i].m_text[length] = 0;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: e6ea82ac IDENTICAL/NONE
// OFFSET: 0x006A8D00
bool ClientDumpRideTicketsResponse::IsValid() const {
    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            bool terminated = false;

            for (uint32_t j = 0; !terminated; j++) {
                if (j >= sizeof(this->unk_0010.m_data[i].m_text)) {
                    break;
                }

                terminated |= this->unk_0010.m_data[i].m_text[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: 6a6b3f3c IDENTICAL/UNPROVEN
// OFFSET: 0x006BA140/0x14036DA30
bool ClientDumpRideTicketsResponse::Handle() {
    if (ClientDumpRideTicketsResponse::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF8F0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF8F0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientDumpRideTicketsResponse*)>(reinterpret_cast<uintptr_t>(ClientDumpRideTicketsResponse::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 06c09566 IDENTICAL/IDENTICAL
// OFFSET: 0x006ECE10/0x1403B1230
ClientGuildCriteriaData::ClientGuildCriteriaData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 93fee048 IDENTICAL/IDENTICAL
// OFFSET: 0x006E2C90/0x1403A37D0
ClientGuildCriteriaData::~ClientGuildCriteriaData() {
}

// OFFSET: 0x006E25C0/0x1403A3030
void ClientGuildCriteriaData::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        this->unk_0010.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[4] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        if (this->unk_0010.m_data[i].unk_0018[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[5] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0008 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[7] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0004 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[6] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[4] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[4] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[3] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[6] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_000C = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[5] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0020 = value;
        }
        if (this->unk_0010.m_data[i].unk_0018[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[7] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[0] ^= byteValue;
        }
    }
}

// PROVEN: 58df8f07 IDENTICAL/UNPROVEN
// OFFSET: 0x006EAF60/0x1403AE6C0
bool ClientGuildCriteriaData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DF140/0x14039EE30
bool ClientGuildCriteriaData::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((16 * count + 7) >> 3) + 20 * count) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        this->unk_0010.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0010[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0018[4] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        if (this->unk_0010.m_data[i].unk_0018[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[5] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0008 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[7] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0004 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[6] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[4] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[4] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[3] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[6] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_000C = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        if (this->unk_0010.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[5] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0020 = value;
        }
        if (this->unk_0010.m_data[i].unk_0018[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[7] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0010[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0018[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0018[0] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6b47168a IDENTICAL/UNPROVEN
// OFFSET: 0x006A8D90/0x140357D60
bool ClientGuildCriteriaData::Handle() {
    if (ClientGuildCriteriaData::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF3E4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF3E4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildCriteriaData*)>(reinterpret_cast<uintptr_t>(ClientGuildCriteriaData::s_handler) - (right ^ left))(this);
    }

    return false;
}

// OFFSET: 0x006DC6A0/0x14039B8C0
ClientLfgUpdateStatus::ClientLfgUpdateStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006D71B0/0x140394E20
ClientLfgUpdateStatus::~ClientLfgUpdateStatus() {
}

// OFFSET: 0x006D7FF0/0x140395EE0
void ClientLfgUpdateStatus::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0041 = bits.ReadBit();
    this->unk_0014.SetCount(bits.ReadBits<24>());
    this->unk_0028[6] = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->unk_0042) = bits.ReadBits<9>();
    this->unk_0028[4] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0028[2] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0040 = value;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0042);
        msg->GetData(this->unk_0042, length);
        this->unk_0042[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }

    for (uint32_t i = 0; i < 3; i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0142[i] = value;
    }

    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0034 = value;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014.m_data[i] = value;
    }
}

// PROVEN: f692b03a IDENTICAL/UNPROVEN
// OFFSET: 0x006DAD60/0x140399580
bool ClientLfgUpdateStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D86B0/0x1403967C0
bool ClientLfgUpdateStatus::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0041 = bits.ReadBit();
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->unk_0014.SetCount(count);
    this->unk_0028[6] = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    {
        uint32_t length = bits.ReadBits<9>();
        *reinterpret_cast<uint32_t*>(this->unk_0042) = length;

        if (length >= sizeof(this->unk_0042)) {
            return false;
        }
    }
    this->unk_0028[4] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0028[2] = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0010 = bits.ReadBit();
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0040 = value;
    }
    {
        uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0042);
        msg->GetData(this->unk_0042, length);
        this->unk_0042[length] = 0;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }

    for (uint32_t i = 0; i < 3; i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0142[i] = value;
    }

    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0034 = value;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0014.m_data[i] = value;
    }

    return msg->IsRead() != 0;
}

// PROVEN: fdb98537 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF2F0/0x140374330
bool ClientLfgUpdateStatus::Handle() {
    if (ClientLfgUpdateStatus::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE748.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE748.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgUpdateStatus*)>(reinterpret_cast<uintptr_t>(ClientLfgUpdateStatus::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2fa89295 IDENTICAL/IDENTICAL
// OFFSET: 0x006EE9A0/0x1403B3770
ClientRespondInspectAchievements::ClientRespondInspectAchievements(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3e05ccc2 IDENTICAL/IDENTICAL
// OFFSET: 0x006E59D0/0x1403A6F20
ClientRespondInspectAchievements::~ClientRespondInspectAchievements() {
}

// OFFSET: 0x006E2CF0/0x1403A3850
void ClientRespondInspectAchievements::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0030[7] = bits.ReadBit();
    this->unk_0030[4] = bits.ReadBit();
    this->unk_0030[1] = bits.ReadBit();
    this->unk_0010.SetCount(bits.ReadBits<23>());
    this->unk_0030[0] = bits.ReadBit();
    this->unk_0030[3] = bits.ReadBit();
    this->unk_0020.SetCount(bits.ReadBits<21>());
    this->unk_0030[2] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0018 = bits.ReadBits<2>();
        this->unk_0020.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[7] = bits.ReadBit();
    }

    this->unk_0030[6] = bits.ReadBit();
    this->unk_0030[5] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0008[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[4] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_003C = value;
        }
        if (this->unk_0020.m_data[i].unk_0008[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[1] ^= byteValue;
        }
        *msg >> this->unk_0020.m_data[i].unk_001C;
        if (this->unk_0020.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[2] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[6] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0000 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0040 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[2] ^= byteValue;
        }
    }

    if (this->unk_0030[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[1] ^= byteValue;
    }
    if (this->unk_0030[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[6] ^= byteValue;
    }
    if (this->unk_0030[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[3] ^= byteValue;
    }
    if (this->unk_0030[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[0] ^= byteValue;
    }
    if (this->unk_0030[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[2] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        *msg >> this->unk_0010.m_data[i].unk_0004;
    }

    if (this->unk_0030[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[7] ^= byteValue;
    }
    if (this->unk_0030[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[4] ^= byteValue;
    }
    if (this->unk_0030[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[5] ^= byteValue;
    }
}

// PROVEN: f9729fb4 IDENTICAL/UNPROVEN
// OFFSET: 0x006ECE00/0x1403B1220
bool ClientRespondInspectAchievements::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E6140/0x1403A7A90
bool ClientRespondInspectAchievements::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0030[7] = bits.ReadBit();
    this->unk_0030[4] = bits.ReadBit();
    this->unk_0030[1] = bits.ReadBit();

    uint32_t count2 = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count2) {
        return false;
    }

    this->unk_0010.SetCount(count2);

    this->unk_0030[0] = bits.ReadBit();
    this->unk_0030[3] = bits.ReadBit();
    uint32_t count = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((18 * count + 7) >> 3) + 16 * count) {
        return false;
    }

    this->unk_0020.SetCount(count);
    this->unk_0030[2] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0018 = bits.ReadBits<2>();
        this->unk_0020.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[7] = bits.ReadBit();
    }

    this->unk_0030[6] = bits.ReadBit();
    this->unk_0030[5] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0008[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[4] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_003C = value;
        }
        if (this->unk_0020.m_data[i].unk_0008[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[1] ^= byteValue;
        }
        *msg >> this->unk_0020.m_data[i].unk_001C;
        if (this->unk_0020.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[2] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[6] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0000 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0040 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[2] ^= byteValue;
        }
    }

    if (this->unk_0030[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[1] ^= byteValue;
    }
    if (this->unk_0030[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[6] ^= byteValue;
    }
    if (this->unk_0030[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[3] ^= byteValue;
    }
    if (this->unk_0030[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[0] ^= byteValue;
    }
    if (this->unk_0030[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[2] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        *msg >> this->unk_0010.m_data[i].unk_0004;
    }

    if (this->unk_0030[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[7] ^= byteValue;
    }
    if (this->unk_0030[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[4] ^= byteValue;
    }
    if (this->unk_0030[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[5] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 5b5df737 IDENTICAL/UNPROVEN
// OFFSET: 0x006A55C0/0x140353370
bool ClientRespondInspectAchievements::Handle() {
    if (ClientRespondInspectAchievements::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEB88.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEB88.m_left->Get();
        return reinterpret_cast<bool (*)(ClientRespondInspectAchievements*)>(reinterpret_cast<uintptr_t>(ClientRespondInspectAchievements::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 7d73589d IDENTICAL/IDENTICAL
// OFFSET: 0x006ECA00/0x1403B0E00
ClientAllAchievementData::ClientAllAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 2a88a263 IDENTICAL/IDENTICAL
// OFFSET: 0x006E22E0/0x1403A2D30
ClientAllAchievementData::~ClientAllAchievementData() {
}

// OFFSET: 0x006E09E0/0x1403A0D90
void ClientAllAchievementData::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0020.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0018 = bits.ReadBits<2>();
        this->unk_0020.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[1] = bits.ReadBit();
    }

    this->unk_0010.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[2] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0040 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[2] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0000 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[7] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_003C = value;
        }
        *msg >> this->unk_0020.m_data[i].unk_001C;
        if (this->unk_0020.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[1] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        *msg >> this->unk_0010.m_data[i].unk_0004;
    }
}

// PROVEN: a09c7e37 IDENTICAL/UNPROVEN
// OFFSET: 0x006E59C0/0x1403A6F10
bool ClientAllAchievementData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E3E70/0x1403A4DD0
bool ClientAllAchievementData::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count2 = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((18 * count2 + 7) >> 3) + 16 * count2) {
        return false;
    }

    this->unk_0020.SetCount(count2);

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0010[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0018 = bits.ReadBits<2>();
        this->unk_0020.m_data[i].unk_0010[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0008[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0010[1] = bits.ReadBit();
    }

    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0010[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[2] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0040 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[2] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0000 = value;
        }
        if (this->unk_0020.m_data[i].unk_0010[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0010[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0008[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0008[7] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_003C = value;
        }
        *msg >> this->unk_0020.m_data[i].unk_001C;
        if (this->unk_0020.m_data[i].unk_0010[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0010[1] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
        *msg >> this->unk_0010.m_data[i].unk_0004;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 69c11adb IDENTICAL/UNPROVEN
// OFFSET: 0x006ADAE0/0x14035DEE0
bool ClientAllAchievementData::Handle() {
    if (ClientAllAchievementData::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDD28.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDD28.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAllAchievementData*)>(reinterpret_cast<uintptr_t>(ClientAllAchievementData::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: c1c3e9f2 IDENTICAL/IDENTICAL
// OFFSET: 0x006D95B0/0x1403979E0
ClientBattlefieldList::ClientBattlefieldList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 720b7006 IDENTICAL/UNPROVEN
// OFFSET: 0x006DAD30/0x140399540
ClientBattlefieldList::~ClientBattlefieldList() {
}

// OFFSET: 0x006D7390/0x140395040
void ClientBattlefieldList::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0044 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_002C = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0050 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_004C = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_002A = value;
    }

    JamBitReader bits(msg);
    this->unk_0020[0] = bits.ReadBit();
    this->unk_0020[1] = bits.ReadBit();
    this->unk_0020[7] = bits.ReadBit();
    this->unk_0029 = bits.ReadBit();
    this->unk_0054 = bits.ReadBit();
    this->unk_0030.SetCount(bits.ReadBits<24>());
    this->unk_0020[6] = bits.ReadBit();
    this->unk_0020[4] = bits.ReadBit();
    this->unk_0020[2] = bits.ReadBit();
    this->unk_0020[3] = bits.ReadBit();
    this->unk_0028 = bits.ReadBit();
    this->unk_0020[5] = bits.ReadBit();
    this->unk_0040 = bits.ReadBit();
    if (this->unk_0020[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[6] ^= byteValue;
    }
    if (this->unk_0020[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[1] ^= byteValue;
    }
    if (this->unk_0020[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[7] ^= byteValue;
    }
    if (this->unk_0020[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[5] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0030.m_count; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030.m_data[i] = value;
    }

    if (this->unk_0020[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[0] ^= byteValue;
    }
    if (this->unk_0020[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[2] ^= byteValue;
    }
    if (this->unk_0020[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[4] ^= byteValue;
    }
    if (this->unk_0020[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[3] ^= byteValue;
    }
}

// PROVEN: d5508548 IDENTICAL/UNPROVEN
// OFFSET: 0x006D9F20/0x140398520
bool ClientBattlefieldList::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D9BB0/0x140398150
bool ClientBattlefieldList::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0044 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_002C = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0050 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0018 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_004C = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0014 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_002A = value;
    }

    JamBitReader bits(msg);
    this->unk_0020[0] = bits.ReadBit();
    this->unk_0020[1] = bits.ReadBit();
    this->unk_0020[7] = bits.ReadBit();
    this->unk_0029 = bits.ReadBit();
    this->unk_0054 = bits.ReadBit();
    uint32_t count = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < 4 * count) {
        return false;
    }

    this->unk_0030.SetCount(count);
    this->unk_0020[6] = bits.ReadBit();
    this->unk_0020[4] = bits.ReadBit();
    this->unk_0020[2] = bits.ReadBit();
    this->unk_0020[3] = bits.ReadBit();
    this->unk_0028 = bits.ReadBit();
    this->unk_0020[5] = bits.ReadBit();
    this->unk_0040 = bits.ReadBit();
    if (this->unk_0020[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[6] ^= byteValue;
    }
    if (this->unk_0020[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[1] ^= byteValue;
    }
    if (this->unk_0020[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[7] ^= byteValue;
    }
    if (this->unk_0020[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[5] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0030.m_count; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030.m_data[i] = value;
    }

    if (this->unk_0020[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[0] ^= byteValue;
    }
    if (this->unk_0020[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[2] ^= byteValue;
    }
    if (this->unk_0020[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[4] ^= byteValue;
    }
    if (this->unk_0020[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0020[3] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: d1d8ebdb IDENTICAL/UNPROVEN
// OFFSET: 0x006A5EB0/0x140353F50
bool ClientBattlefieldList::Handle() {
    if (ClientBattlefieldList::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDEC4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDEC4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldList*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldList::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: ae6f820a IDENTICAL/IDENTICAL
// OFFSET: 0x006ECDA0/0x1403B11B0
ClientBattlefieldPlayerPositions::ClientBattlefieldPlayerPositions(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b39446d4 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5400/0x1403A6790
ClientBattlefieldPlayerPositions::~ClientBattlefieldPlayerPositions() {
}

// OFFSET: 0x006E5A90/0x1403A7020
void ClientBattlefieldPlayerPositions::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        this->unk_0010.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[4] = bits.ReadBit();
    }

    this->unk_0020.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[3] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[1] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_000C = value;
        }
        if (this->unk_0020.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[3] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0008 = value;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        if (this->unk_0010.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[6] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0008 = value;
        }
        if (this->unk_0010.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[3] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_000C = value;
        }
        if (this->unk_0010.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[4] ^= byteValue;
        }
    }
}

// PROVEN: caa3f70e IDENTICAL/UNPROVEN
// OFFSET: 0x006E2510/0x1403A3020
bool ClientBattlefieldPlayerPositions::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E02D0/0x1403A0310
bool ClientBattlefieldPlayerPositions::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count2 = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < ((8 * count2 + 7) >> 3) + 8 * count2) {
        return false;
    }

    this->unk_0010.SetCount(count2);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        this->unk_0010.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0010.m_data[i].unk_0000[4] = bits.ReadBit();
    }

    uint32_t count = bits.ReadBits<22>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3) + 8 * count) {
        return false;
    }

    this->unk_0020.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[3] = bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[1] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_000C = value;
        }
        if (this->unk_0020.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[3] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0020.m_data[i].unk_0008 = value;
        }
    }

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        if (this->unk_0010.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[6] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0008 = value;
        }
        if (this->unk_0010.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[3] ^= byteValue;
        }
        {
            float value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_000C = value;
        }
        if (this->unk_0010.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0010.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0010.m_data[i].unk_0000[4] ^= byteValue;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: fcaa7bfa IDENTICAL/UNPROVEN
// OFFSET: 0x006A71D0/0x140355C10
bool ClientBattlefieldPlayerPositions::Handle() {
    if (ClientBattlefieldPlayerPositions::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA48.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA48.m_left->Get();
        return reinterpret_cast<bool (*)(ClientBattlefieldPlayerPositions*)>(reinterpret_cast<uintptr_t>(ClientBattlefieldPlayerPositions::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 6c6c1dc2 IDENTICAL/IDENTICAL
// OFFSET: 0x006F0760/0x1403B6110
ClientCommentatorPartyInfo::ClientCommentatorPartyInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a4903eac IDENTICAL/IDENTICAL
// OFFSET: 0x006DBA90/0x14039AA70
ClientCommentatorPartyInfo::~ClientCommentatorPartyInfo() {
}

// OFFSET: 0x006F00C0/0x1403B5850
void ClientCommentatorPartyInfo::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0014.SetCount(bits.ReadBits<20>());

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        this->unk_0014.m_data[i].unk_0030[6] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[0] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[7] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[4] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[3] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0048 = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(this->unk_0014.m_data[i].unk_0000) = bits.ReadBits<7>();
        this->unk_0014.m_data[i].unk_0030[5] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[2] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[1] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0049 = bits.ReadBit();
    }

    this->unk_0028 = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        if (this->unk_0014.m_data[i].unk_0030[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[6] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[5] ^= byteValue;
        }
        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0014.m_data[i].unk_0000);
            msg->GetData(this->unk_0014.m_data[i].unk_0000, length);
            this->unk_0014.m_data[i].unk_0000[length] = 0;
        }
        if (this->unk_0014.m_data[i].unk_0030[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[1] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[0] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[4] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0038 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_003C = value;
        }
        if (this->unk_0014.m_data[i].unk_0030[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[3] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0040 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0044 = value;
        }
        if (this->unk_0014.m_data[i].unk_0030[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[2] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[7] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }
}

// PROVEN: 9d0718d1 IDENTICAL/UNPROVEN
// OFFSET: 0x006EA9F0/0x1403AE080
bool ClientCommentatorPartyInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E7C10/0x1403AA170
bool ClientCommentatorPartyInfo::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<20>();

    if (msg->m_size - msg->m_read < ((17 * count + 7) >> 3) + 16 * count) {
        return false;
    }

    this->unk_0014.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        this->unk_0014.m_data[i].unk_0030[6] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[0] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[7] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[4] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[3] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0048 = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(this->unk_0014.m_data[i].unk_0000) = bits.ReadBits<7>();

        if (*reinterpret_cast<uint32_t*>(this->unk_0014.m_data[i].unk_0000) >= sizeof(this->unk_0014.m_data[i].unk_0000)) {
            return false;
        }

        this->unk_0014.m_data[i].unk_0030[5] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[2] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0030[1] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0049 = bits.ReadBit();
    }

    this->unk_0028 = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        if (this->unk_0014.m_data[i].unk_0030[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[6] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[5] ^= byteValue;
        }
        {
            uint32_t length = *reinterpret_cast<uint32_t*>(this->unk_0014.m_data[i].unk_0000);
            msg->GetData(this->unk_0014.m_data[i].unk_0000, length);
            this->unk_0014.m_data[i].unk_0000[length] = 0;
        }
        if (this->unk_0014.m_data[i].unk_0030[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[1] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[0] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[4] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0038 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_003C = value;
        }
        if (this->unk_0014.m_data[i].unk_0030[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[3] ^= byteValue;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0040 = value;
        }
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0014.m_data[i].unk_0044 = value;
        }
        if (this->unk_0014.m_data[i].unk_0030[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[2] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0030[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0030[7] ^= byteValue;
        }
    }

    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: d1d0a8b2 IDENTICAL/NONE
// OFFSET: 0x006ACEB0
bool ClientCommentatorPartyInfo::IsValid() const {
    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->unk_0014.m_data[i].unk_0000))) {
                    break;
                }

                terminated |= this->unk_0014.m_data[i].unk_0000[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: c489edb9 IDENTICAL/UNPROVEN
// OFFSET: 0x006A41E0/0x140351BE0
bool ClientCommentatorPartyInfo::Handle() {
    if (ClientCommentatorPartyInfo::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDEF0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDEF0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientCommentatorPartyInfo*)>(reinterpret_cast<uintptr_t>(ClientCommentatorPartyInfo::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: ff497614 IDENTICAL/IDENTICAL
// OFFSET: 0x006E5820/0x1403A6D30
ClientGuildAchievementData::ClientGuildAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4e160af2 IDENTICAL/IDENTICAL
// OFFSET: 0x006D9310/0x140397700
ClientGuildAchievementData::~ClientGuildAchievementData() {
}

// PROVEN: 7bd23a85 IDENTICAL/DIFFERENT
// OFFSET: 0x006DD780/0x14039CD00
void ClientGuildAchievementData::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *msg >> this->unk_0010.m_data[i].unk_0004;
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
    }
}

// PROVEN: 2c10e65c IDENTICAL/UNPROVEN
// OFFSET: 0x006EE0F0/0x1403B2D70
bool ClientGuildAchievementData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 6ca6396b IDENTICAL/DIFFERENT
// OFFSET: 0x006E2330/0x1403A2DA0
bool ClientGuildAchievementData::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < 8 * count) {
        return false;
    }

    this->unk_0010.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0010.m_count; i++) {
        *msg >> this->unk_0010.m_data[i].unk_0004;
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0010.m_data[i].unk_0000 = value;
        }
    }

    return msg->IsRead() != 0;
}

// PROVEN: af76cb2b IDENTICAL/UNPROVEN
// OFFSET: 0x006BBF50/0x140370160
bool ClientGuildAchievementData::Handle() {
    if (ClientGuildAchievementData::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFA8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFA8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildAchievementData*)>(reinterpret_cast<uintptr_t>(ClientGuildAchievementData::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 8f9a10af IDENTICAL/IDENTICAL
// OFFSET: 0x006E3E30/0x1403A4D80
ClientGuildAchievementMembers::ClientGuildAchievementMembers(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: b672e4d3 IDENTICAL/IDENTICAL
// OFFSET: 0x006E2CC0/0x1403A3810
ClientGuildAchievementMembers::~ClientGuildAchievementMembers() {
}

// OFFSET: 0x006DFDC0/0x14039FCD0
void ClientGuildAchievementMembers::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0020.SetCount(bits.ReadBits<26>());

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[2] = bits.ReadBit();
    }

    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[2] ^= byteValue;
        }
    }

    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }
}

// PROVEN: ff2717ce IDENTICAL/UNPROVEN
// OFFSET: 0x006E53F0/0x1403A6780
bool ClientGuildAchievementMembers::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DF7D0/0x14039F5B0
bool ClientGuildAchievementMembers::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    uint32_t count = bits.ReadBits<26>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3)) {
        return false;
    }

    this->unk_0020.SetCount(count);

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        this->unk_0020.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0020.m_data[i].unk_0000[2] = bits.ReadBit();
    }

    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0020.m_count; i++) {
        if (this->unk_0020.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[7] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[3] ^= byteValue;
        }
        if (this->unk_0020.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0020.m_data[i].unk_0000[2] ^= byteValue;
        }
    }

    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 80a34d71 IDENTICAL/UNPROVEN
// OFFSET: 0x006C1990/0x140377040
bool ClientGuildAchievementMembers::Handle() {
    if (ClientGuildAchievementMembers::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE514.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE514.m_left->Get();
        return reinterpret_cast<bool (*)(ClientGuildAchievementMembers*)>(reinterpret_cast<uintptr_t>(ClientGuildAchievementMembers::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// OFFSET: 0x006F82D0/0x1403C11F0
ClientLfgJoinResult::ClientLfgJoinResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006F64A0/0x1403BB710
ClientLfgJoinResult::~ClientLfgJoinResult() {
}

// OFFSET: 0x006F7C20/0x1403BFF40
void ClientLfgJoinResult::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0034 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0011 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }

    JamBitReader bits(msg);
    this->unk_0028[2] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    this->unk_0014.SetCount(bits.ReadBits<24>());

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        this->unk_0014.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[1] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0008.SetCount(bits.ReadBits<22>());
    }

    this->unk_0028[4] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0028[6] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        for (uint32_t j = 0; j < this->unk_0014.m_data[i].unk_0008.m_count; j++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0004 = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_000C = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0008 = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0000 = value;
            }
        }

        if (this->unk_0014.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[3] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[7] ^= byteValue;
        }
    }

    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }
}

// PROVEN: 9db6970e IDENTICAL/NONE
// OFFSET: 0x006F82C0
bool ClientLfgJoinResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F7560
bool ClientLfgJoinResult::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0034 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0030 = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0011 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0038 = value;
    }

    JamBitReader bits(msg);
    this->unk_0028[2] = bits.ReadBit();
    this->unk_0028[7] = bits.ReadBit();
    this->unk_0028[3] = bits.ReadBit();
    this->unk_0028[0] = bits.ReadBit();
    uint32_t count2 = bits.ReadBits<24>();

    if (msg->m_size - msg->m_read < ((30 * count2 + 7) >> 3)) {
        return false;
    }

    this->unk_0014.SetCount(count2);

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        this->unk_0014.m_data[i].unk_0000[7] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[5] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[3] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[6] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[0] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[2] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[4] = bits.ReadBit();
        this->unk_0014.m_data[i].unk_0000[1] = bits.ReadBit();
        uint32_t count = bits.ReadBits<22>();

        if (msg->m_size - msg->m_read < 16 * count) {
            return false;
        }

        this->unk_0014.m_data[i].unk_0008.SetCount(count);
    }

    this->unk_0028[4] = bits.ReadBit();
    this->unk_0028[5] = bits.ReadBit();
    this->unk_0028[1] = bits.ReadBit();
    this->unk_0028[6] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0014.m_count; i++) {
        for (uint32_t j = 0; j < this->unk_0014.m_data[i].unk_0008.m_count; j++) {
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0004 = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_000C = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0008 = value;
            }
            {
                uint32_t value = 0;
                msg->Get(value);
                this->unk_0014.m_data[i].unk_0008.m_data[j].unk_0000 = value;
            }
        }

        if (this->unk_0014.m_data[i].unk_0000[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[2] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[5] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[1] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[0] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[4] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[3] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[6] ^= byteValue;
        }
        if (this->unk_0014.m_data[i].unk_0000[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->unk_0014.m_data[i].unk_0000[7] ^= byteValue;
        }
    }

    if (this->unk_0028[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[1] ^= byteValue;
    }
    if (this->unk_0028[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[4] ^= byteValue;
    }
    if (this->unk_0028[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[3] ^= byteValue;
    }
    if (this->unk_0028[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[5] ^= byteValue;
    }
    if (this->unk_0028[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[0] ^= byteValue;
    }
    if (this->unk_0028[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[7] ^= byteValue;
    }
    if (this->unk_0028[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[2] ^= byteValue;
    }
    if (this->unk_0028[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0028[6] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: feb3d680 IDENTICAL/UNPROVEN
// OFFSET: 0x006AD440/0x14035D600
bool ClientLfgJoinResult::Handle() {
    if (ClientLfgJoinResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDD18.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDD18.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgJoinResult*)>(reinterpret_cast<uintptr_t>(ClientLfgJoinResult::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006EA9A0/0x1403AE020
ClientLfgProposalUpdate::ClientLfgProposalUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E1DE0/0x1403A25D0
ClientLfgProposalUpdate::~ClientLfgProposalUpdate() {
}

// OFFSET: 0x006DEBA0/0x14039E750
void ClientLfgProposalUpdate::Unpack(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0028 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0054 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_004C = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }

    JamBitReader bits(msg);
    this->unk_0030[4] = bits.ReadBit();
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0030[1] = bits.ReadBit();
    this->unk_0050 = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0030[3] = bits.ReadBit();
    this->unk_0038.SetCount(bits.ReadBits<23>());
    this->unk_0030[7] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0038.m_count; i++) {
        this->unk_0038.m_data[i].unk_0005 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0006 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0008 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0007 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0004 = bits.ReadBit();
    }

    this->unk_0030[5] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0030[2] = bits.ReadBit();
    this->unk_0030[6] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0030[0] = bits.ReadBit();
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0030[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[3] ^= byteValue;
    }
    if (this->unk_0030[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[6] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0030[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[5] ^= byteValue;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0038.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0038.m_data[i].unk_0000 = value;
        }
    }

    if (this->unk_0030[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[7] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0030[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[0] ^= byteValue;
    }
    if (this->unk_0030[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[1] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0030[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[2] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    if (this->unk_0030[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[4] ^= byteValue;
    }
}

// PROVEN: cf771e5c IDENTICAL/UNPROVEN
// OFFSET: 0x006ECD70/0x1403B1180
bool ClientLfgProposalUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E3880/0x1403A4660
bool ClientLfgProposalUpdate::Read(CDataStore* msg, bool full) {
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0028 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0048 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0020 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0024 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_0054 = value;
    }
    {
        uint32_t value = 0;
        msg->Get(value);
        this->unk_004C = value;
    }
    {
        uint8_t value = 0;
        msg->Get(value);
        this->unk_0010 = value;
    }

    JamBitReader bits(msg);
    this->unk_0030[4] = bits.ReadBit();
    this->unk_0018[3] = bits.ReadBit();
    this->unk_0018[7] = bits.ReadBit();
    this->unk_0018[0] = bits.ReadBit();
    this->unk_0030[1] = bits.ReadBit();
    this->unk_0050 = bits.ReadBit();
    this->unk_0018[4] = bits.ReadBit();
    this->unk_0018[5] = bits.ReadBit();
    this->unk_0030[3] = bits.ReadBit();
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((5 * count + 7) >> 3) + 4 * count) {
        return false;
    }

    this->unk_0038.SetCount(count);
    this->unk_0030[7] = bits.ReadBit();

    for (uint32_t i = 0; i < this->unk_0038.m_count; i++) {
        this->unk_0038.m_data[i].unk_0005 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0006 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0008 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0007 = bits.ReadBit();
        this->unk_0038.m_data[i].unk_0004 = bits.ReadBit();
    }

    this->unk_0030[5] = bits.ReadBit();
    this->unk_0018[6] = bits.ReadBit();
    this->unk_0030[2] = bits.ReadBit();
    this->unk_0030[6] = bits.ReadBit();
    this->unk_0018[2] = bits.ReadBit();
    this->unk_0018[1] = bits.ReadBit();
    this->unk_0030[0] = bits.ReadBit();
    if (this->unk_0018[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[5] ^= byteValue;
    }
    if (this->unk_0030[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[3] ^= byteValue;
    }
    if (this->unk_0030[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[6] ^= byteValue;
    }
    if (this->unk_0018[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[6] ^= byteValue;
    }
    if (this->unk_0018[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[0] ^= byteValue;
    }
    if (this->unk_0030[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[5] ^= byteValue;
    }
    if (this->unk_0018[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[1] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->unk_0038.m_count; i++) {
        {
            uint32_t value = 0;
            msg->Get(value);
            this->unk_0038.m_data[i].unk_0000 = value;
        }
    }

    if (this->unk_0030[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[7] ^= byteValue;
    }
    if (this->unk_0018[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[4] ^= byteValue;
    }
    if (this->unk_0030[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[0] ^= byteValue;
    }
    if (this->unk_0030[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[1] ^= byteValue;
    }
    if (this->unk_0018[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[2] ^= byteValue;
    }
    if (this->unk_0018[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[7] ^= byteValue;
    }
    if (this->unk_0030[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[2] ^= byteValue;
    }
    if (this->unk_0018[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0018[3] ^= byteValue;
    }
    if (this->unk_0030[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->unk_0030[4] ^= byteValue;
    }

    return msg->IsRead() != 0;
}

// PROVEN: e85ab99f IDENTICAL/UNPROVEN
// OFFSET: 0x006B77F0/0x14036A6E0
bool ClientLfgProposalUpdate::Handle() {
    if (ClientLfgProposalUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE6DC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE6DC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgProposalUpdate*)>(reinterpret_cast<uintptr_t>(ClientLfgProposalUpdate::s_handler) - (right + left))(this);
    }

    return false;
}

// OFFSET: 0x006A53C0/0x140350570
JamPhaseShiftChange::JamPhaseShiftChange() {
}

// OFFSET: 0x006A54B0/0x140350660
JamPhaseShiftChange::~JamPhaseShiftChange() {
}

// OFFSET: 0x006BE960/0x140373730
ClientPhaseShiftChange::ClientPhaseShiftChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A5600/0x1403533D0
ClientPhaseShiftChange::~ClientPhaseShiftChange() {
}

// PROVEN: 96af3f51 IDENTICAL/UNPROVEN
// OFFSET: 0x006ABE90/0x14035BBE0
bool ClientPhaseShiftChange::Handle() {
    if (ClientPhaseShiftChange::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD350.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD350.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPhaseShiftChange*)>(reinterpret_cast<uintptr_t>(ClientPhaseShiftChange::s_handler) - (right ^ left))(this);
    }

    return false;
}

// OFFSET: 0x006BD140/0x140371880
void ClientPhaseShiftChange::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_data.m_guid[2] = bits.ReadBit();
    this->m_data.m_guid[3] = bits.ReadBit();
    this->m_data.m_guid[1] = bits.ReadBit();
    this->m_data.m_guid[6] = bits.ReadBit();
    this->m_data.m_guid[4] = bits.ReadBit();
    this->m_data.m_guid[5] = bits.ReadBit();
    this->m_data.m_guid[0] = bits.ReadBit();
    this->m_data.m_guid[7] = bits.ReadBit();

    uint32_t size;
    void* data;

    if (this->m_data.m_guid[7]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[7] ^= byte;
    }

    if (this->m_data.m_guid[4]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[4] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_uiWorldMapAreaIDSwaps.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_uiWorldMapAreaIDSwaps.PutData(data, size);
        this->m_data.m_uiWorldMapAreaIDSwaps.Finalize();
    }

    if (this->m_data.m_guid[1]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[1] ^= byte;
    }

    {
        uint32_t flags = 0;
        msg->Get(flags);
        this->m_data.m_flags = flags;
    }

    if (this->m_data.m_guid[2]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[2] ^= byte;
    }

    if (this->m_data.m_guid[6]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[6] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_preloadMapIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_preloadMapIDs.PutData(data, size);
        this->m_data.m_preloadMapIDs.Finalize();
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_phaseIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_phaseIDs.PutData(data, size);
        this->m_data.m_phaseIDs.Finalize();
    }

    if (this->m_data.m_guid[3]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[3] ^= byte;
    }

    if (this->m_data.m_guid[0]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[0] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_visibleMapIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_visibleMapIDs.PutData(data, size);
        this->m_data.m_visibleMapIDs.Finalize();
    }

    if (this->m_data.m_guid[5]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[5] ^= byte;
    }
}

// PROVEN: 58c94e70 IDENTICAL/UNPROVEN
// OFFSET: 0x006B53E0/0x1403676D0
bool ClientPhaseShiftChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AFF50/0x140360B80
bool ClientPhaseShiftChange::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_data.m_guid[2] = bits.ReadBit();
    this->m_data.m_guid[3] = bits.ReadBit();
    this->m_data.m_guid[1] = bits.ReadBit();
    this->m_data.m_guid[6] = bits.ReadBit();
    this->m_data.m_guid[4] = bits.ReadBit();
    this->m_data.m_guid[5] = bits.ReadBit();
    this->m_data.m_guid[0] = bits.ReadBit();
    this->m_data.m_guid[7] = bits.ReadBit();

    uint32_t size;
    void* data;

    if (this->m_data.m_guid[7]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[7] ^= byte;
    }

    if (this->m_data.m_guid[4]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[4] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_uiWorldMapAreaIDSwaps.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_uiWorldMapAreaIDSwaps.PutData(data, size);
        this->m_data.m_uiWorldMapAreaIDSwaps.Finalize();
    }

    if (this->m_data.m_guid[1]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[1] ^= byte;
    }

    {
        uint32_t flags = 0;
        msg->Get(flags);
        this->m_data.m_flags = flags;
    }

    if (this->m_data.m_guid[2]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[2] ^= byte;
    }

    if (this->m_data.m_guid[6]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[6] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_preloadMapIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_preloadMapIDs.PutData(data, size);
        this->m_data.m_preloadMapIDs.Finalize();
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_phaseIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_phaseIDs.PutData(data, size);
        this->m_data.m_phaseIDs.Finalize();
    }

    if (this->m_data.m_guid[3]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[3] ^= byte;
    }

    if (this->m_data.m_guid[0]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[0] ^= byte;
    }

    size = 0;
    msg->Get(size);
    msg->GetDataInSitu(data, size);

    if (full) {
        this->m_data.m_visibleMapIDs.InitExternal(static_cast<uint8_t*>(data), size);
    } else {
        this->m_data.m_visibleMapIDs.PutData(data, size);
        this->m_data.m_visibleMapIDs.Finalize();
    }

    if (this->m_data.m_guid[5]) {
        uint8_t byte = 0;
        msg->Get(byte);
        this->m_data.m_guid[5] ^= byte;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 554fbc70 IDENTICAL/DIFFERENT
// OFFSET: 0x006F41E0/0x1403BB570
ClientLfgSearchResult::ClientLfgSearchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E0290
ClientLfgSearchResult::~ClientLfgSearchResult() {
}

// PROVEN: dfa6ec2f IDENTICAL/UNPROVEN
// OFFSET: 0x006BF3C0/0x140374440
bool ClientLfgSearchResult::Handle() {
    if (ClientLfgSearchResult::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE9C4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE9C4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientLfgSearchResult*)>(reinterpret_cast<uintptr_t>(ClientLfgSearchResult::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// OFFSET: 0x006F0860/0x1403B6250
void ClientLfgSearchResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t byteValue;
    uint32_t intValue;
    float floatValue;

    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    this->m_parties.SetCount(bits.ReadBits<21>());

    for (uint32_t i = 0; i < this->m_parties.Count(); i++) {
        JamLfgSearchResultParty& party = this->m_parties[i];
        party.m_leaderGUID[6] = bits.ReadBit();
        party.m_guid[3] = bits.ReadBit();
        party.m_guid[1] = bits.ReadBit();
        *reinterpret_cast<uint32_t*>(party.m_comment) = bits.ReadBits<9>();
        party.m_leaderGUID[0] = bits.ReadBit();
        party.m_guid[5] = bits.ReadBit();
        party.m_guid[2] = bits.ReadBit();
        party.m_leaderGUID[1] = bits.ReadBit();
        party.m_leaderGUID[4] = bits.ReadBit();
        party.m_guid[4] = bits.ReadBit();
        party.m_leaderGUID[7] = bits.ReadBit();
        party.m_guid[7] = bits.ReadBit();
        party.m_guid[0] = bits.ReadBit();
        party.m_leaderGUID[3] = bits.ReadBit();
        party.m_guid[6] = bits.ReadBit();
        party.m_leaderGUID[2] = bits.ReadBit();
        party.m_leaderGUID[5] = bits.ReadBit();
    }

    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->unk_0028 = bits.ReadBit() != 0;
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    this->m_players.SetCount(bits.ReadBits<18>());

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        JamLfgSearchResultPlayer& player = this->m_players[i];
        *reinterpret_cast<uint32_t*>(player.m_comment) = bits.ReadBits<9>();
        player.m_partyGUID[3] = bits.ReadBit();
        player.m_guid[7] = bits.ReadBit();
        player.m_partyGUID[1] = bits.ReadBit();
        player.m_guildGUID[7] = bits.ReadBit();
        player.m_guildGUID[4] = bits.ReadBit();
        player.m_guid[3] = bits.ReadBit();
        player.m_guid[1] = bits.ReadBit();
        player.m_guildGUID[6] = bits.ReadBit();
        player.m_partyGUID[2] = bits.ReadBit();
        player.unk_0164 = bits.ReadBit() != 0;
        player.m_guildGUID[0] = bits.ReadBit();
        player.m_partyGUID[0] = bits.ReadBit();
        player.m_guid[2] = bits.ReadBit();
        player.m_partyGUID[5] = bits.ReadBit();
        player.m_guildGUID[3] = bits.ReadBit();
        player.m_guid[4] = bits.ReadBit();
        player.m_guid[6] = bits.ReadBit();
        player.m_guildGUID[5] = bits.ReadBit();
        player.m_guid[0] = bits.ReadBit();
        player.m_partyGUID[7] = bits.ReadBit();
        player.m_guildGUID[1] = bits.ReadBit();
        player.m_partyGUID[4] = bits.ReadBit();
        player.m_partyGUID[6] = bits.ReadBit();
        player.m_guildGUID[2] = bits.ReadBit();
        player.m_guid[5] = bits.ReadBit();
    }

    this->m_removed.SetCount(bits.ReadBits<26>());

    for (uint32_t i = 0; i < this->m_removed.Count(); i++) {
        this->m_removed[i].m_guid[7] = bits.ReadBit();
        this->m_removed[i].m_guid[3] = bits.ReadBit();
        this->m_removed[i].m_guid[6] = bits.ReadBit();
        this->m_removed[i].m_guid[2] = bits.ReadBit();
        this->m_removed[i].m_guid[1] = bits.ReadBit();
        this->m_removed[i].m_guid[4] = bits.ReadBit();
        this->m_removed[i].m_guid[0] = bits.ReadBit();
        this->m_removed[i].m_guid[5] = bits.ReadBit();
    }

    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();

    for (uint32_t i = 0; i < this->m_removed.Count(); i++) {
        if (this->m_removed[i].m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[3] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[6] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[4] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[2] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[1] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[0] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[5] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[7] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        JamLfgSearchResultPlayer& player = this->m_players[i];

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0020 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018C = byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0044 = floatValue;
        }

        if (player.m_partyGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[4] ^= byteValue;
        }

        if (player.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_004C = intValue;
        }

        if (player.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[7] ^= byteValue;
        }

        intValue = *reinterpret_cast<uint32_t*>(player.m_comment);
        msg->GetData(player.m_comment, intValue);
        player.m_comment[intValue] = 0;

        if (player.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[4] ^= byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_002C = floatValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0050 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000C = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0040 = intValue;
        }

        if (player.m_partyGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_005C = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0060 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0058 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000E = byteValue;
        }

        if (player.m_partyGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[5] ^= byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0048 = floatValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0038 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0024 = intValue;
        }

        if (player.m_partyGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[0] ^= byteValue;
        }

        if (player.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[4] ^= byteValue;
        }

        if (player.m_partyGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[7] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0010 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_0178 = byteValue;
        }

        if (player.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[6] ^= byteValue;
        }

        if (player.m_partyGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[2] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018D = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0034 = intValue;
        }

        if (player.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[2] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018E = byteValue;
        }

        if (player.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[1] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000D = byteValue;
        }

        if (player.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[0] ^= byteValue;
        }

        if (player.m_partyGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[1] ^= byteValue;
        }

        if (player.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[7] ^= byteValue;
        }

        if (player.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[2] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0054 = intValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0028 = floatValue;
        }

        if (player.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[0] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_0170 = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_001C = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0008 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0014 = intValue;
        }

        if (player.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_003C = intValue;
        }

        if (player.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[1] ^= byteValue;
        }

        if (player.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[3] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0188 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0018 = intValue;
        }

        if (player.m_partyGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[3] ^= byteValue;
        }

        if (player.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[5] ^= byteValue;
        }

        if (player.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[3] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0030 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0174 = intValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0024 = intValue;
    }

    for (uint32_t i = 0; i < this->m_parties.Count(); i++) {
        JamLfgSearchResultParty& party = this->m_parties[i];
        intValue = *reinterpret_cast<uint32_t*>(party.m_comment);
        msg->GetData(party.m_comment, intValue);
        party.m_comment[intValue] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            party.unk_0008 = intValue;
        }

        if (party.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            party.unk_0118 = intValue;
        }

        if (party.m_leaderGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[6] ^= byteValue;
        }

        if (party.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[2] ^= byteValue;
        }

        if (party.m_leaderGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[4] ^= byteValue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.unk_011C[j] = byteValue;
        }

        if (party.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[4] ^= byteValue;
        }

        if (party.m_leaderGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[7] ^= byteValue;
        }

        if (party.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[7] ^= byteValue;
        }

        if (party.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[3] ^= byteValue;
        }

        if (party.m_leaderGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[3] ^= byteValue;
        }

        if (party.m_leaderGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[5] ^= byteValue;
        }

        if (party.m_leaderGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[0] ^= byteValue;
        }

        if (party.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[5] ^= byteValue;
        }

        if (party.m_leaderGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[1] ^= byteValue;
        }

        if (party.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[1] ^= byteValue;
        }

        if (party.m_leaderGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[2] ^= byteValue;
        }

        if (party.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[0] ^= byteValue;
        }
    }

    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0058 = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_type = intValue;
    }

    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_id = intValue;
    }

    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0010 = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_time = intValue;
    }

    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_006C = intValue;
    }

    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }
}

// PROVEN: 5df0671e IDENTICAL/UNPROVEN
// OFFSET: 0x006F2D90/0x1403B9960
bool ClientLfgSearchResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EAF70/0x1403AE6D0
bool ClientLfgSearchResult::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t byteValue;
    uint32_t intValue;
    float floatValue;

    this->m_ticket.m_requesterGuid[3] = bits.ReadBit();
    uint32_t count3 = bits.ReadBits<21>();

    if (msg->m_size - msg->m_read < ((25 * count3 + 7) >> 3) + 11 * count3) {
        return false;
    }

    this->m_parties.SetCount(count3);

    for (uint32_t i = 0; i < this->m_parties.Count(); i++) {
        JamLfgSearchResultParty& party = this->m_parties[i];
        party.m_leaderGUID[6] = bits.ReadBit();
        party.m_guid[3] = bits.ReadBit();
        party.m_guid[1] = bits.ReadBit();

        *reinterpret_cast<uint32_t*>(party.m_comment) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(party.m_comment) >= sizeof(party.m_comment)) {
            return false;
        }

        party.m_leaderGUID[0] = bits.ReadBit();
        party.m_guid[5] = bits.ReadBit();
        party.m_guid[2] = bits.ReadBit();
        party.m_leaderGUID[1] = bits.ReadBit();
        party.m_leaderGUID[4] = bits.ReadBit();
        party.m_guid[4] = bits.ReadBit();
        party.m_leaderGUID[7] = bits.ReadBit();
        party.m_guid[7] = bits.ReadBit();
        party.m_guid[0] = bits.ReadBit();
        party.m_leaderGUID[3] = bits.ReadBit();
        party.m_guid[6] = bits.ReadBit();
        party.m_leaderGUID[2] = bits.ReadBit();
        party.m_leaderGUID[5] = bits.ReadBit();
    }

    this->m_ticket.m_requesterGuid[6] = bits.ReadBit();
    this->unk_0028 = bits.ReadBit() != 0;
    this->m_ticket.m_requesterGuid[7] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[5] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[2] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[4] = bits.ReadBit();
    uint32_t count2 = bits.ReadBits<18>();

    if (msg->m_size - msg->m_read < ((34 * count2 + 7) >> 3) + 104 * count2) {
        return false;
    }

    this->m_players.SetCount(count2);

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        JamLfgSearchResultPlayer& player = this->m_players[i];
        *reinterpret_cast<uint32_t*>(player.m_comment) = bits.ReadBits<9>();

        if (*reinterpret_cast<uint32_t*>(player.m_comment) >= sizeof(player.m_comment)) {
            return false;
        }

        player.m_partyGUID[3] = bits.ReadBit();
        player.m_guid[7] = bits.ReadBit();
        player.m_partyGUID[1] = bits.ReadBit();
        player.m_guildGUID[7] = bits.ReadBit();
        player.m_guildGUID[4] = bits.ReadBit();
        player.m_guid[3] = bits.ReadBit();
        player.m_guid[1] = bits.ReadBit();
        player.m_guildGUID[6] = bits.ReadBit();
        player.m_partyGUID[2] = bits.ReadBit();
        player.unk_0164 = bits.ReadBit() != 0;
        player.m_guildGUID[0] = bits.ReadBit();
        player.m_partyGUID[0] = bits.ReadBit();
        player.m_guid[2] = bits.ReadBit();
        player.m_partyGUID[5] = bits.ReadBit();
        player.m_guildGUID[3] = bits.ReadBit();
        player.m_guid[4] = bits.ReadBit();
        player.m_guid[6] = bits.ReadBit();
        player.m_guildGUID[5] = bits.ReadBit();
        player.m_guid[0] = bits.ReadBit();
        player.m_partyGUID[7] = bits.ReadBit();
        player.m_guildGUID[1] = bits.ReadBit();
        player.m_partyGUID[4] = bits.ReadBit();
        player.m_partyGUID[6] = bits.ReadBit();
        player.m_guildGUID[2] = bits.ReadBit();
        player.m_guid[5] = bits.ReadBit();
    }

    uint32_t count = bits.ReadBits<26>();

    if (msg->m_size - msg->m_read < ((8 * count + 7) >> 3)) {
        return false;
    }

    this->m_removed.SetCount(count);

    for (uint32_t i = 0; i < this->m_removed.Count(); i++) {
        this->m_removed[i].m_guid[7] = bits.ReadBit();
        this->m_removed[i].m_guid[3] = bits.ReadBit();
        this->m_removed[i].m_guid[6] = bits.ReadBit();
        this->m_removed[i].m_guid[2] = bits.ReadBit();
        this->m_removed[i].m_guid[1] = bits.ReadBit();
        this->m_removed[i].m_guid[4] = bits.ReadBit();
        this->m_removed[i].m_guid[0] = bits.ReadBit();
        this->m_removed[i].m_guid[5] = bits.ReadBit();
    }

    this->m_ticket.m_requesterGuid[0] = bits.ReadBit();
    this->m_ticket.m_requesterGuid[1] = bits.ReadBit();

    for (uint32_t i = 0; i < this->m_removed.Count(); i++) {
        if (this->m_removed[i].m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[3] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[6] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[4] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[2] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[1] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[0] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[5] ^= byteValue;
        }

        if (this->m_removed[i].m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_removed[i].m_guid[7] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < this->m_players.Count(); i++) {
        JamLfgSearchResultPlayer& player = this->m_players[i];

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0020 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018C = byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0044 = floatValue;
        }

        if (player.m_partyGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[4] ^= byteValue;
        }

        if (player.m_guildGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[5] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_004C = intValue;
        }

        if (player.m_guildGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[7] ^= byteValue;
        }

        intValue = *reinterpret_cast<uint32_t*>(player.m_comment);
        msg->GetData(player.m_comment, intValue);
        player.m_comment[intValue] = 0;

        if (player.m_guildGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[4] ^= byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_002C = floatValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0050 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000C = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0040 = intValue;
        }

        if (player.m_partyGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_005C = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0060 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0058 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000E = byteValue;
        }

        if (player.m_partyGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[5] ^= byteValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0048 = floatValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0038 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0024 = intValue;
        }

        if (player.m_partyGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[0] ^= byteValue;
        }

        if (player.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[4] ^= byteValue;
        }

        if (player.m_partyGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[7] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0010 = intValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_0178 = byteValue;
        }

        if (player.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[6] ^= byteValue;
        }

        if (player.m_partyGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[2] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018D = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0034 = intValue;
        }

        if (player.m_guildGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[2] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_018E = byteValue;
        }

        if (player.m_guildGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[1] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_000D = byteValue;
        }

        if (player.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[0] ^= byteValue;
        }

        if (player.m_partyGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[1] ^= byteValue;
        }

        if (player.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[7] ^= byteValue;
        }

        if (player.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[2] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0054 = intValue;
        }

        {
            float floatValue = 0;
            msg->Get(floatValue);
            player.unk_0028 = floatValue;
        }

        if (player.m_guildGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[0] ^= byteValue;
        }

        {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.unk_0170 = byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_001C = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0008 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0014 = intValue;
        }

        if (player.m_guildGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_003C = intValue;
        }

        if (player.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[1] ^= byteValue;
        }

        if (player.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[3] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0188 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0018 = intValue;
        }

        if (player.m_partyGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_partyGUID[3] ^= byteValue;
        }

        if (player.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guid[5] ^= byteValue;
        }

        if (player.m_guildGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            player.m_guildGUID[3] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0030 = intValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            player.unk_0174 = intValue;
        }
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0024 = intValue;
    }

    for (uint32_t i = 0; i < this->m_parties.Count(); i++) {
        JamLfgSearchResultParty& party = this->m_parties[i];
        intValue = *reinterpret_cast<uint32_t*>(party.m_comment);
        msg->GetData(party.m_comment, intValue);
        party.m_comment[intValue] = 0;

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            party.unk_0008 = intValue;
        }

        if (party.m_guid[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[6] ^= byteValue;
        }

        {
            uint32_t intValue = 0;
            msg->Get(intValue);
            party.unk_0118 = intValue;
        }

        if (party.m_leaderGUID[6]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[6] ^= byteValue;
        }

        if (party.m_guid[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[2] ^= byteValue;
        }

        if (party.m_leaderGUID[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[4] ^= byteValue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.unk_011C[j] = byteValue;
        }

        if (party.m_guid[4]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[4] ^= byteValue;
        }

        if (party.m_leaderGUID[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[7] ^= byteValue;
        }

        if (party.m_guid[7]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[7] ^= byteValue;
        }

        if (party.m_guid[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[3] ^= byteValue;
        }

        if (party.m_leaderGUID[3]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[3] ^= byteValue;
        }

        if (party.m_leaderGUID[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[5] ^= byteValue;
        }

        if (party.m_leaderGUID[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[0] ^= byteValue;
        }

        if (party.m_guid[5]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[5] ^= byteValue;
        }

        if (party.m_leaderGUID[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[1] ^= byteValue;
        }

        if (party.m_guid[1]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[1] ^= byteValue;
        }

        if (party.m_leaderGUID[2]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_leaderGUID[2] ^= byteValue;
        }

        if (party.m_guid[0]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            party.m_guid[0] ^= byteValue;
        }
    }

    if (this->m_ticket.m_requesterGuid[7]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[7] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[1]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[1] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0058 = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_type = intValue;
    }

    if (this->m_ticket.m_requesterGuid[4]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[4] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[2]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[2] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_id = intValue;
    }

    if (this->m_ticket.m_requesterGuid[6]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[6] ^= byteValue;
    }

    if (this->m_ticket.m_requesterGuid[5]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[5] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_0010 = intValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->m_ticket.m_time = intValue;
    }

    if (this->m_ticket.m_requesterGuid[0]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[0] ^= byteValue;
    }

    {
        uint32_t intValue = 0;
        msg->Get(intValue);
        this->unk_006C = intValue;
    }

    if (this->m_ticket.m_requesterGuid[3]) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_ticket.m_requesterGuid[3] ^= byteValue;
    }

    return msg->IsRead() && this->IsValid();
}

// PROVEN: e693261e IDENTICAL/DIFFERENT
// OFFSET: 0x006B4E20/0x140366FA0
bool ClientLfgSearchResult::IsValid() const {
    if (!this->m_ticket.IsValid()) {
        return false;
    }

    for (uint32_t i = 0; i < this->m_parties.m_count; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->m_parties.m_data[i].m_comment))) {
                    break;
                }

                terminated |= this->m_parties.m_data[i].m_comment[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    for (uint32_t i = 0; i < this->m_players.m_count; i++) {
        {
            bool terminated = false;

            for (int32_t j = 0; !terminated; j++) {
                if (j >= static_cast<int32_t>(sizeof(this->m_players.m_data[i].m_comment))) {
                    break;
                }

                terminated |= this->m_players.m_data[i].m_comment[j] == 0;
            }

            if (!terminated) {
                return false;
            }
        }
    }

    return true;
}

// PROVEN: 3e267652 IDENTICAL/NONE
// OFFSET: 0x006A50F0
bool ClientGuildRosterMemberData::IsValid() const {
    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
                break;
            }

            terminated |= this->m_name[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_officerNote))) {
                break;
            }

            terminated |= this->m_officerNote[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    {
        bool terminated = false;

        for (int32_t i = 0; !terminated; i++) {
            if (i >= static_cast<int32_t>(sizeof(this->m_note))) {
                break;
            }

            terminated |= this->m_note[i] == 0;
        }

        if (!terminated) {
            return false;
        }
    }

    return true;
}

// PROVEN: f9dc1b85 IDENTICAL/IDENTICAL
// OFFSET: 0x00696A00/0x140342060
bool JamCliCUFProfile::IsValid() const {
    bool terminated = false;

    for (int32_t i = 0; !terminated; i++) {
        if (i >= static_cast<int32_t>(sizeof(this->m_name))) {
            break;
        }

        terminated |= this->m_name[i] == 0;
    }

    if (!terminated) {
        return false;
    }

    return true;
}
