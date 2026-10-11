#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 9574b3a0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
JamGuildRewardItem::~JamGuildRewardItem() {
}

// OFFSET: [0x00401000/0x1407D7E80]
JamCliSupportTicketHeader::~JamCliSupportTicketHeader() {
}

// OFFSET: [0x00401000/0x1407D7E80]
JamCliStructAddonPrefix::~JamCliStructAddonPrefix() {
}

// OFFSET: [0x0066BC00/0x140312430]
JamCliStructAddonPrefix::JamCliStructAddonPrefix() {
}

// OFFSET: 0x00696AB0
void JamCliSupportTicketHeader::Put(CDataStore* msg) const {
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.z);
    msg->Put(this->m_position.x);
    msg->Put(this->m_mapID);
    msg->Put(this->m_facing);
}

// OFFSET: 0x00696E60/0x1403424C0
JamCliSupportTicketHeader::JamCliSupportTicketHeader() {
}

// OFFSET: 0x00696FD0
void JamCliSupportTicketChatLine::operator=(const JamCliSupportTicketChatLine& source) {
    SStrCopy(this->m_text, source.m_text, sizeof(this->m_text));
    this->m_timestamp = source.m_timestamp;
}

// OFFSET: 0x00697DB0
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

// OFFSET: 0x006981E0/0x140343A20
JamCliSupportTicketChatLog::JamCliSupportTicketChatLog() {
    this->m_hasReportLineIndex = false;
}

// OFFSET: 0x006982F0
JamCliSupportTicketChatLog::~JamCliSupportTicketChatLog() {
}

// OFFSET: 0x006A2770/0x14034FC80
void JamGuildRewardItem::operator=(const JamGuildRewardItem& other) {
    this->m_cost = other.m_cost;
    this->m_minGuildLevel = other.m_minGuildLevel;
    this->m_minGuildRep = other.m_minGuildRep;
    this->m_itemID = other.m_itemID;
    this->m_raceMask = other.m_raceMask;
    this->m_achievementRequired = other.m_achievementRequired;
}

// PROVEN: c4a66ca1 IDENTICAL/NONE
// OFFSET: 0x006C8460/0x14037F500
JamGuildNewsEvent::JamGuildNewsEvent() {
}

// PROVEN: 78cb8955 IDENTICAL/NONE
// OFFSET: 0x006D3D30
JamGuildNewsEvent::~JamGuildNewsEvent() {
}

// OFFSET: 0x006D9F30
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

// STATIC: 0x00E4D570
bool (*ClientPartyInvite::s_handler)(ClientPartyInvite* msg);

// STATIC: 0x00E4D680
bool (*ClientGuildRoster::s_handler)(ClientGuildRoster* msg);

// STATIC: 0x00E4D450
bool (*ClientGuildRosterUpdate::s_handler)(ClientGuildRosterUpdate* msg);

// STATIC: 0x00E4D5B4
bool (*ClientGuildRewardList::s_handler)(ClientGuildRewardList* msg);

// STATIC: 0x00E4D820
bool (*ClientGuildNews::s_handler)(ClientGuildNews* msg);

// STATIC: 0x00E4D464
bool (*ClientGuildNewsDeleted::s_handler)(ClientGuildNewsDeleted* msg);

// STATIC: 0x00E4D7DC
bool (*ClientGuildReputationWeeklyCap::s_handler)(ClientGuildReputationWeeklyCap* msg);

// STATIC: 0x00E4D770
bool (*ClientGuildXPEarned::s_handler)(ClientGuildXPEarned* msg);

// STATIC: 0x00E4D6B4
bool (*ClientGuildBankMoneyWithdrawn::s_handler)(ClientGuildBankMoneyWithdrawn* msg);

// STATIC: 0x00E4D810
bool (*ClientVoidStorageContents::s_handler)(ClientVoidStorageContents* msg);

// STATIC: 0x00E4D59C
bool (*ClientVoidStorageFailed::s_handler)(ClientVoidStorageFailed* msg);

// STATIC: 0x00E4D7C4
bool (*ClientVoidStorageTransferChanges::s_handler)(ClientVoidStorageTransferChanges* msg);

// STATIC: 0x00E4D5A4
bool (*ClientVoidTransferResult::s_handler)(ClientVoidTransferResult* msg);

// STATIC: 0x00E4D468
bool (*ClientLfGuildPostUpdated::s_handler)(ClientLfGuildPostUpdated* msg);

// STATIC: 0x00E4D5CC
bool (*ClientSetupResearchHistory::s_handler)(ClientSetupResearchHistory* msg);

// STATIC: 0x00E4D6E0
bool (*ClientResearchComplete::s_handler)(ClientResearchComplete* msg);

// STATIC: 0x00E4D598
bool (*ClientLfGuildBrowseUpdated::s_handler)(ClientLfGuildBrowseUpdated* msg);

// STATIC: 0x00E4D718
bool (*ClientLfGuildRecruitListUpdated::s_handler)(ClientLfGuildRecruitListUpdated* msg);

// STATIC: 0x00E4D508
bool (*ClientLfGuildCommandResult::s_handler)(ClientLfGuildCommandResult* msg);

// STATIC: 0x00E4D664
bool (*ClientLfGuildMembershipListUpdated::s_handler)(ClientLfGuildMembershipListUpdated* msg);

// STATIC: 0x00E4D704
bool (*ClientLfGuildApplicantListUpdated::s_handler)(ClientLfGuildApplicantListUpdated* msg);

// STATIC: 0x00E4D6FC
bool (*ClientLfGuildApplicationsListChanged::s_handler)(ClientLfGuildApplicationsListChanged* msg);

// STATIC: 0x00E4D5A8
bool (*ClientVoidItemSwapResponse::s_handler)(ClientVoidItemSwapResponse* msg);

// STATIC: 0x00E4D80C
bool (*ClientReforgeResult::s_handler)(ClientReforgeResult* msg);

// STATIC: 0x00E4D494
bool (*ClientForgeMasterSet::s_handler)(ClientForgeMasterSet* msg);

// STATIC: 0x00E4D60C
bool (*ClientGuildChangeNameResult::s_handler)(ClientGuildChangeNameResult* msg);

// STATIC: 0x00E4D51C
bool (*ClientGuildFlaggedForRename::s_handler)(ClientGuildFlaggedForRename* msg);

// STATIC: 0x00E4D4D8
bool (*ClientGuildRanks::s_handler)(ClientGuildRanks* msg);

// STATIC: 0x00E4D7B8
bool (*ClientGuildMemberUpdateNote::s_handler)(ClientGuildMemberUpdateNote* msg);

// STATIC: 0x00E4D4C0
bool (*ClientGuildInvite::s_handler)(ClientGuildInvite* msg);

// STATIC: 0x00E4D690
bool (*ClientGuildPartyState::s_handler)(ClientGuildPartyState* msg);

// STATIC: 0x00E4D780
bool (*ClientGuildReputationReactionChanged::s_handler)(ClientGuildReputationReactionChanged* msg);

// STATIC: 0x00E4D7BC
bool (*ClientGuildChallengeUpdate::s_handler)(ClientGuildChallengeUpdate* msg);

// STATIC: 0x00E4D580
bool (*ClientGuildChallengeCompleted::s_handler)(ClientGuildChallengeCompleted* msg);

// STATIC: 0x00E4D4C4
bool (*ClientGuildReset::s_handler)(ClientGuildReset* msg);

// STATIC: 0x00E4D670
bool (*ClientGuildMoveComplete::s_handler)(ClientGuildMoveComplete* msg);

// STATIC: 0x00E4D55C
bool (*ClientGuildMoveStarting::s_handler)(ClientGuildMoveStarting* msg);

// STATIC: 0x00E4D594
bool (*ClientGuildNameChanged::s_handler)(ClientGuildNameChanged* msg);

// STATIC: 0x00E4D430
bool (*ClientGuildPermissionsQueryResults::s_handler)(ClientGuildPermissionsQueryResults* msg);

// STATIC: 0x00E4D474
bool (*ClientGuildEventLogQueryResults::s_handler)(ClientGuildEventLogQueryResults* msg);

// STATIC: 0x00E4D65C
bool (*ClientGuildMemberDailyReset::s_handler)(ClientGuildMemberDailyReset* msg);

// STATIC: 0x00E4D500
bool (*ClientGuildBankQueryResults::s_handler)(ClientGuildBankQueryResults* msg);

// STATIC: 0x00E4D710
bool (*ClientGuildBankLogQueryResults::s_handler)(ClientGuildBankLogQueryResults* msg);

// STATIC: 0x00E4D624
bool (*ClientGuildBankTextQueryResult::s_handler)(ClientGuildBankTextQueryResult* msg);

// STATIC: 0x00E4D6A8
bool (*ClientTradeStatus::s_handler)(ClientTradeStatus* msg);

// STATIC: 0x00E4D56C
bool (*ClientTradeUpdated::s_handler)(ClientTradeUpdated* msg);

// STATIC: 0x00E4D694
bool (*ClientSetupCurrency::s_handler)(ClientSetupCurrency* msg);

// STATIC: 0x00E4D728
bool (*ClientSetCurrency::s_handler)(ClientSetCurrency* msg);

// STATIC: 0x00E4D644
bool (*ClientWeeklyResetCurrency::s_handler)(ClientWeeklyResetCurrency* msg);

// STATIC: 0x00E4D42C
bool (*ClientUpdateCurrencyWeekLimit::s_handler)(ClientUpdateCurrencyWeekLimit* msg);

// STATIC: 0x00E4D734
bool (*ClientVendorInventory::s_handler)(ClientVendorInventory* msg);

// STATIC: 0x00E4D518
bool (*ClientPetSlotUpdated::s_handler)(ClientPetSlotUpdated* msg);

// STATIC: 0x00E4D66C
bool (*ClientPetAdded::s_handler)(ClientPetAdded* msg);

// STATIC: 0x00E4D7D0
bool (*ClientAuthResponse::s_handler)(ClientAuthResponse* msg);

// STATIC: 0x00E4D4B4
bool (*ClientWaitQueueUpdate::s_handler)(ClientWaitQueueUpdate* msg);

// STATIC: 0x00E4D52C
bool (*ClientWaitQueueFinish::s_handler)(ClientWaitQueueFinish* msg);

// STATIC: 0x00E4D758
bool (*ClientGenerateRandomCharacterNameResult::s_handler)(ClientGenerateRandomCharacterNameResult* msg);

// STATIC: 0x00E4D5E0
bool (*ClientEnumCharactersResult::s_handler)(ClientEnumCharactersResult* msg);

// STATIC: 0x00E4D7E0
bool (*ClientDBReply::s_handler)(ClientDBReply* msg);

// STATIC: 0x00E4D674
bool (*ClientHotfixNotify::s_handler)(ClientHotfixNotify* msg);

// STATIC: 0x00E4D6D8
bool (*ClientHotfixNotifyBlob::s_handler)(ClientHotfixNotifyBlob* msg);

// STATIC: 0x00E4D5BC
bool (*ClientGameObjectActivateAnimKit::s_handler)(ClientGameObjectActivateAnimKit* msg);

// STATIC: 0x00E4D520
bool (*ClientMapObjEvents::s_handler)(ClientMapObjEvents* msg);

// STATIC: 0x00E4D5B0
bool (*ClientNotification::s_handler)(ClientNotification* msg);

// STATIC: 0x00E4D440
bool (*ClientPhaseShiftChange::s_handler)(ClientPhaseShiftChange* msg);

// STATIC: 0x00E4D804
bool (*ClientLfgSearchResult::s_handler)(ClientLfgSearchResult* msg);

// STATIC: 0x00E4D460
bool (*ClientSuspendToken::s_handler)(ClientSuspendToken* msg);

// STATIC: 0x00E4D764
bool (*ClientNewWorldAbort::s_handler)(ClientNewWorldAbort* msg);

// STATIC: 0x00E4D590
bool (*ClientTransferPending::s_handler)(ClientTransferPending* msg);

// STATIC: 0x00E4D54C
bool (*ClientMessageBox::s_handler)(ClientMessageBox* msg);

// STATIC: 0x00E4D784
bool (*ClientNewWorld::s_handler)(ClientNewWorld* msg);

// STATIC: 0x00E4D768
bool (*ClientAccountRestrictedWarning::s_handler)(ClientAccountRestrictedWarning* msg);

// STATIC: 0x00E4D7F8
bool (*ClientAreaTriggerMovementUpdate::s_handler)(ClientAreaTriggerMovementUpdate* msg);

// STATIC: 0x00E4D448
bool (*ClientAverageItemLevelInform::s_handler)(ClientAverageItemLevelInform* msg);

// STATIC: 0x00E4D5D8
bool (*ClientBattlefieldMgrEjectPending::s_handler)(ClientBattlefieldMgrEjectPending* msg);

// STATIC: 0x00E4D558
bool (*ClientBattlefieldMgrEjected::s_handler)(ClientBattlefieldMgrEjected* msg);

// STATIC: 0x00E4D774
bool (*ClientBattlefieldMgrEntryInvite::s_handler)(ClientBattlefieldMgrEntryInvite* msg);

// STATIC: 0x00E4D7FC
bool (*ClientBattlefieldMgrExitRequest::s_handler)(ClientBattlefieldMgrExitRequest* msg);

// STATIC: 0x00E4D6E4
bool (*ClientBattlefieldMgrStateChange::s_handler)(ClientBattlefieldMgrStateChange* msg);

// STATIC: 0x00E4D720
bool (*ClientBattlefieldPortDenied::s_handler)(ClientBattlefieldPortDenied* msg);

// STATIC: 0x00E4D5F0
bool (*ClientBattlefieldRatedInfo::s_handler)(ClientBattlefieldRatedInfo* msg);

// STATIC: 0x00E4D814
bool (*ClientBattlefieldStatusFailed::s_handler)(ClientBattlefieldStatusFailed* msg);

// STATIC: 0x00E4D6AC
bool (*ClientBattlefieldStatusQueued::s_handler)(ClientBattlefieldStatusQueued* msg);

// STATIC: 0x00E4D6EC
bool (*ClientBattlefieldStatusWaitforgroups::s_handler)(ClientBattlefieldStatusWaitforgroups* msg);

// STATIC: 0x00E4D610
bool (*ClientBattlegroundPlayerJoined::s_handler)(ClientBattlegroundPlayerJoined* msg);

// STATIC: 0x00E4D6F0
bool (*ClientBattlegroundPlayerLeft::s_handler)(ClientBattlegroundPlayerLeft* msg);

// STATIC: 0x00E4D650
bool (*ClientClearBossEmotes::s_handler)(ClientClearBossEmotes* msg);

// STATIC: 0x00E4D6A4
bool (*ClientCustomLoadScreen::s_handler)(ClientCustomLoadScreen* msg);

// STATIC: 0x00E4D47C
bool (*ClientDifferentInstanceFromParty::s_handler)(ClientDifferentInstanceFromParty* msg);

// STATIC: 0x00E4D5C0
bool (*ClientDontAutoPushSpellsToActionBar::s_handler)(ClientDontAutoPushSpellsToActionBar* msg);

// STATIC: 0x00E4D544
bool (*ClientFailedPlayerCondition::s_handler)(ClientFailedPlayerCondition* msg);

// STATIC: 0x00E4D7F0
bool (*ClientInspectHonorStats::s_handler)(ClientInspectHonorStats* msg);

// STATIC: 0x00E4D4F0
bool (*ClientInspectRatedBgStats::s_handler)(ClientInspectRatedBgStats* msg);

// STATIC: 0x00E4D454
bool (*ClientLfgSlotInvalid::s_handler)(ClientLfgSlotInvalid* msg);

// STATIC: 0x00E4D470
bool (*ClientLfgUpdateStatusNone::s_handler)(ClientLfgUpdateStatusNone* msg);

// STATIC: 0x00E4D554
bool (*ClientMissileCancel::s_handler)(ClientMissileCancel* msg);

// STATIC: 0x00E4D6C0
bool (*ClientMoneyNotify::s_handler)(ClientMoneyNotify* msg);

// STATIC: 0x00E4D49C
bool (*ClientRatedBgRating::s_handler)(ClientRatedBgRating* msg);

// STATIC: 0x00E4D4B8
bool (*ClientRatedBgStats::s_handler)(ClientRatedBgStats* msg);

// STATIC: 0x00E4D6A0
bool (*ClientPvpLogData::s_handler)(ClientPvpLogData* msg);

// STATIC: 0x00E4D620
bool (*ClientRequestPvpRewardsResponse::s_handler)(ClientRequestPvpRewardsResponse* msg);

// STATIC: 0x00E4D630
bool (*ClientSetDfFastLaunchResult::s_handler)(ClientSetDfFastLaunchResult* msg);

// STATIC: 0x00E4D6B8
bool (*ClientUpdateDungeonEncounterForLoot::s_handler)(ClientUpdateDungeonEncounterForLoot* msg);

// STATIC: 0x00E4D4F4
bool (*ClientUpdateServerPlayerPosition::s_handler)(ClientUpdateServerPlayerPosition* msg);

// STATIC: 0x00E4D75C
bool (*ClientWardenData::s_handler)(ClientWardenData* msg);

// STATIC: 0x00E4D7C8
bool (*ClientWargameCheckEntry::s_handler)(ClientWargameCheckEntry* msg);

// STATIC: 0x00E4D7C0
bool (*ClientWargameRequestSent::s_handler)(ClientWargameRequestSent* msg);

// STATIC: 0x00E4D7CC
bool (*ClientWeeklyLastReset::s_handler)(ClientWeeklyLastReset* msg);

// STATIC: 0x00E4D658
bool (*ClientBattlefieldMgrEntered::s_handler)(ClientBattlefieldMgrEntered* msg);

// STATIC: 0x00E4D5F4
bool (*ClientBattlefieldMgrQueueInvite::s_handler)(ClientBattlefieldMgrQueueInvite* msg);

// STATIC: 0x00E4D7B0
bool (*ClientBattlefieldMgrQueueRequestResponse::s_handler)(ClientBattlefieldMgrQueueRequestResponse* msg);

// STATIC: 0x00E4D648
bool (*ClientBattlefieldStatus::s_handler)(ClientBattlefieldStatus* msg);

// STATIC: 0x00E4D724
bool (*ClientBattlefieldStatusActive::s_handler)(ClientBattlefieldStatusActive* msg);

// STATIC: 0x00E4D480
bool (*ClientDisplayGameError::s_handler)(ClientDisplayGameError* msg);

// STATIC: 0x00E4D760
bool (*ClientPvpOptionsEnabled::s_handler)(ClientPvpOptionsEnabled* msg);

// STATIC: 0x00E4D504
bool (*ClientShowRatings::s_handler)(ClientShowRatings* msg);

// STATIC: 0x00E4D548
bool (*ClientBattlefieldStatusNeedconfirmation::s_handler)(ClientBattlefieldStatusNeedconfirmation* msg);

// STATIC: 0x00E4D584
bool (*ClientGameEventDebugLog::s_handler)(ClientGameEventDebugLog* msg);

// STATIC: 0x00E4D5DC
bool (*ClientGuildAchievementDeleted::s_handler)(ClientGuildAchievementDeleted* msg);

// STATIC: 0x00E4D4C8
bool (*ClientGuildAchievementEarned::s_handler)(ClientGuildAchievementEarned* msg);

// STATIC: 0x00E4D45C
bool (*ClientGuildCriteriaDeleted::s_handler)(ClientGuildCriteriaDeleted* msg);

// STATIC: 0x00E4D534
bool (*ClientLfgQueueStatus::s_handler)(ClientLfgQueueStatus* msg);

// STATIC: 0x00E4D6DC
bool (*ClientServerInfoResponse::s_handler)(ClientServerInfoResponse* msg);

// STATIC: 0x00E4D538
bool (*ClientServerPerf::s_handler)(ClientServerPerf* msg);

// STATIC: 0x00E4D46C
bool (*ClientStreamingMoviesUpdate::s_handler)(ClientStreamingMoviesUpdate* msg);

// STATIC: 0x00E4D5F8
bool (*ClientLoadCufProfiles::s_handler)(ClientLoadCufProfiles* msg);

// STATIC: 0x00E4D4DC
bool (*ClientAccountInfoResponse::s_handler)(ClientAccountInfoResponse* msg);

// STATIC: 0x00E4D61C
bool (*ClientDumpRideTicketsResponse::s_handler)(ClientDumpRideTicketsResponse* msg);

// STATIC: 0x00E4D50C
bool (*ClientGuildCriteriaData::s_handler)(ClientGuildCriteriaData* msg);

// STATIC: 0x00E4D684
bool (*ClientLfgUpdateStatus::s_handler)(ClientLfgUpdateStatus* msg);

// STATIC: 0x00E4D618
bool (*ClientRespondInspectAchievements::s_handler)(ClientRespondInspectAchievements* msg);

// STATIC: 0x00E4D5E8
bool (*ClientAllAchievementData::s_handler)(ClientAllAchievementData* msg);

// STATIC: 0x00E4D7E8
bool (*ClientBattlefieldList::s_handler)(ClientBattlefieldList* msg);

// STATIC: 0x00E4D428
bool (*ClientBattlefieldPlayerPositions::s_handler)(ClientBattlefieldPlayerPositions* msg);

// STATIC: 0x00E4D4A0
bool (*ClientCommentatorPartyInfo::s_handler)(ClientCommentatorPartyInfo* msg);

// STATIC: 0x00E4D738
bool (*ClientGuildAchievementData::s_handler)(ClientGuildAchievementData* msg);

// STATIC: 0x00E4D510
bool (*ClientGuildAchievementMembers::s_handler)(ClientGuildAchievementMembers* msg);

// STATIC: 0x00E4D478
bool (*ClientLfgJoinResult::s_handler)(ClientLfgJoinResult* msg);

// STATIC: 0x00E4D6CC
bool (*ClientLfgProposalUpdate::s_handler)(ClientLfgProposalUpdate* msg);

// OFFSET: 0x006C98F0/0x140381330
ClientAuthResponse::ClientAuthResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasSuccessInfo = false;
    this->m_hasWaitInfo = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006CCA80/0x140385BD0
ClientAuthResponse::~ClientAuthResponse() {
}

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

// OFFSET: 0x006D7380
bool ClientAuthResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CB520
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

// OFFSET: 0x006B72D0/0x14036A040
bool ClientAuthResponse::Handle() {
    if (!ClientAuthResponse::s_handler) {
        return false;
    }

    return ClientAuthResponse::s_handler(this);
}

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

// OFFSET: 0x006A1CD0/0x14034F120
ClientWaitQueueUpdate::~ClientWaitQueueUpdate() {
}

// OFFSET: 0x006CF330
bool ClientWaitQueueUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BFF20
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

// OFFSET: 0x006B2BD0/0x1403643A0
bool ClientWaitQueueUpdate::Handle() {
    if (!ClientWaitQueueUpdate::s_handler) {
        return false;
    }

    return ClientWaitQueueUpdate::s_handler(this);
}

// PROVEN: eb2a49e5 IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientWaitQueueFinish::ClientWaitQueueFinish(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 5072689e IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientWaitQueueFinish::~ClientWaitQueueFinish() {
}

// OFFSET: 0x006BAEE0
bool ClientWaitQueueFinish::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006BDFA0/0x140372B10
bool ClientWaitQueueFinish::Handle() {
    if (!ClientWaitQueueFinish::s_handler) {
        return false;
    }

    return ClientWaitQueueFinish::s_handler(this);
}

// OFFSET: 0x006F4180/0x1403AA150
ClientPvpLogData::~ClientPvpLogData() {
}

// OFFSET: 0x006F4330/0x1403BB740
ClientEnumCharactersResult::ClientEnumCharactersResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ECDF0
bool ClientEnumCharactersResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E98A0
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

// OFFSET: 0x006B5040/0x140367220
bool ClientEnumCharactersResult::Handle() {
    if (!ClientEnumCharactersResult::s_handler) {
        return false;
    }

    return ClientEnumCharactersResult::s_handler(this);
}

// OFFSET: 0x006BE090/0x140372C80
ClientGenerateRandomCharacterNameResult::ClientGenerateRandomCharacterNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0FF0/0x14034E250
ClientGenerateRandomCharacterNameResult::~ClientGenerateRandomCharacterNameResult() {
}

// OFFSET: 0x006B5480/0x1403677B0
void ClientGenerateRandomCharacterNameResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_success = bits.ReadBit() != 0;
    uint32_t length = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = '\0';
}

// OFFSET: 0x006C60B0
bool ClientGenerateRandomCharacterNameResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C4620
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

// OFFSET: 0x006A4D80/0x140352C70
bool ClientGenerateRandomCharacterNameResult::Handle() {
    if (!ClientGenerateRandomCharacterNameResult::s_handler) {
        return false;
    }

    return ClientGenerateRandomCharacterNameResult::s_handler(this);
}

// OFFSET: 0x006BAE70/0x14036EB20
ClientDBReply::ClientDBReply(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006B0480/0x140361210
ClientDBReply::~ClientDBReply() {
}

// OFFSET: 0x006B1CB0/0x1403630A0
bool ClientDBReply::Handle() {
    if (!ClientDBReply::s_handler) {
        return false;
    }

    return ClientDBReply::s_handler(this);
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

// OFFSET: 0x006C05F0
bool ClientDBReply::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BDA10
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

// OFFSET: 0x006E1E10/0x1403A2620
ClientHotfixNotify::ClientHotfixNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006DDEB0/0x14039D900
ClientHotfixNotify::~ClientHotfixNotify() {
}

// OFFSET: 0x006C09F0/0x140375DE0
bool ClientHotfixNotify::Handle() {
    if (!ClientHotfixNotify::s_handler) {
        return false;
    }

    return ClientHotfixNotify::s_handler(this);
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

// OFFSET: 0x006ECD80
bool ClientHotfixNotify::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E1FF0
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

// OFFSET: 0x006BE7A0/0x14034FE70
ClientHotfixNotifyBlob::ClientHotfixNotifyBlob(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2280/0x14034F780
ClientHotfixNotifyBlob::~ClientHotfixNotifyBlob() {
}

// OFFSET: 0x006C0DE0/0x1403762B0
bool ClientHotfixNotifyBlob::Handle() {
    if (!ClientHotfixNotifyBlob::s_handler) {
        return false;
    }

    return ClientHotfixNotifyBlob::s_handler(this);
}

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

// OFFSET: 0x006CF050
bool ClientHotfixNotifyBlob::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BF7C0
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

// OFFSET: 0x006BE8D0/0x140373690
ClientGameObjectActivateAnimKit::ClientGameObjectActivateAnimKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2EB0/0x1403503B0
ClientGameObjectActivateAnimKit::~ClientGameObjectActivateAnimKit() {
}

// OFFSET: 0x006BF780/0x1403748E0
bool ClientGameObjectActivateAnimKit::Handle() {
    if (!ClientGameObjectActivateAnimKit::s_handler) {
        return false;
    }

    return ClientGameObjectActivateAnimKit::s_handler(this);
}

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

// OFFSET: 0x006ADB80
bool ClientGameObjectActivateAnimKit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A7FF0
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

// OFFSET: 0x006BE300/0x140372FB0
bool ClientMapObjEvents::Handle() {
    if (!ClientMapObjEvents::s_handler) {
        return false;
    }

    return ClientMapObjEvents::s_handler(this);
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

// OFFSET: 0x006C50E0
bool ClientMapObjEvents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BCA30
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

// OFFSET: 0x006D5A00/0x140392D40
ClientNotification::ClientNotification(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<13>();
    *reinterpret_cast<uint32_t*>(this->m_text) = length;
    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;
}

// OFFSET: 0x006A0DF0/0x14034DE90
ClientNotification::~ClientNotification() {
}

// OFFSET: 0x006D9230
bool ClientNotification::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D4F10
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

// OFFSET: 0x006A9870/0x140358BD0
bool ClientNotification::Handle() {
    if (!ClientNotification::s_handler) {
        return false;
    }

    return ClientNotification::s_handler(this);
}

// OFFSET: 0x006CF330
bool ClientSuspendToken::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BFF20
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

// OFFSET: 0x006BE000/0x140372BC0
bool ClientSuspendToken::Handle() {
    if (!ClientSuspendToken::s_handler) {
        return false;
    }

    return ClientSuspendToken::s_handler(this);
}

// OFFSET: 0x006BAEE0
bool ClientNewWorldAbort::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A6E00/0x140355740
bool ClientNewWorldAbort::Handle() {
    if (!ClientNewWorldAbort::s_handler) {
        return false;
    }

    return ClientNewWorldAbort::s_handler(this);
}

// OFFSET: 0x006CCA40/0x140385B90
ClientTransferPending::ClientTransferPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTransferSpellID = false;
    this->m_hasShip = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006B2270/0x140363830
bool ClientTransferPending::Handle() {
    if (!ClientTransferPending::s_handler) {
        return false;
    }

    return ClientTransferPending::s_handler(this);
}

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

// OFFSET: 0x006D1EF0
bool ClientTransferPending::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BBB00
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

// OFFSET: 0x006C7920
bool ClientMessageBox::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C5EB0
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

// OFFSET: 0x006B05D0/0x140361340
bool ClientMessageBox::Handle() {
    if (!ClientMessageBox::s_handler) {
        return false;
    }

    return ClientMessageBox::s_handler(this);
}

// OFFSET: 0x006A5BF0/0x140353BB0
ClientNewWorld::ClientNewWorld(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->Unpack(msg, true);
}

// OFFSET: 0x006A9260/0x1403583C0
bool ClientNewWorld::Handle() {
    if (!ClientNewWorld::s_handler) {
        return false;
    }

    return ClientNewWorld::s_handler(this);
}

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

// OFFSET: 0x006D7300
bool ClientNewWorld::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C8360
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

// PROVEN: 48018d85 IDENTICAL/NONE
// OFFSET: [0x0066BC00/0x140312430]
JamWeeklySpellUse::JamWeeklySpellUse() {
}

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

// OFFSET: 0x00696BC0
void JamClientRideTicket::Clear() {
    *reinterpret_cast<uint64_t*>(this->m_requesterGuid) = 0;
    this->m_id = 0;
    this->m_type = 0;
    this->m_time = 0;
}

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

// OFFSET: 0x006A1DD0/0x14034F250
void JamWeeklySpellUse::operator=(const JamWeeklySpellUse& source) {
    this->m_category = source.m_category;
    this->m_uses = source.m_uses;
}

// STATIC: 0x00E4D4D4
bool (*ClientPlaySpellVisualKit::s_handler)(ClientPlaySpellVisualKit* msg);

// STATIC: 0x00E4D488
bool (*ClientWeeklySpellUsage::s_handler)(ClientWeeklySpellUsage* msg);

// STATIC: 0x00E4D6F8
bool (*ClientWeeklySpellUsageUpdate::s_handler)(ClientWeeklySpellUsageUpdate* msg);

// STATIC: 0x00E4D77C
bool (*ClientClearCooldowns::s_handler)(ClientClearCooldowns* msg);

// STATIC: 0x00E4D654
bool (*ClientCurrencyLootRemoved::s_handler)(ClientCurrencyLootRemoved* msg);

// STATIC: 0x00E4D43C
bool (*ClientCurrencyLootRestored::s_handler)(ClientCurrencyLootRestored* msg);

// STATIC: 0x00E4D5D4
bool (*ClientLootContents::s_handler)(ClientLootContents* msg);

// STATIC: 0x00E4D754
bool (*ClientGuildSendGuildXP::s_handler)(ClientGuildSendGuildXP* msg);

// STATIC: 0x00E4D4AC
bool (*ClientGuildSendMaxDailyXP::s_handler)(ClientGuildSendMaxDailyXP* msg);

// STATIC: 0x00E4D7B4
bool (*ClientGuildSendRankChange::s_handler)(ClientGuildSendRankChange* msg);

// STATIC: 0x00E4D5E4
bool (*ClientBattlegroundInfoThrottled::s_handler)(ClientBattlegroundInfoThrottled* msg);

// STATIC: 0x00E4D53C
bool (*ClientGuildMemberRecipes::s_handler)(ClientGuildMemberRecipes* msg);

// STATIC: 0x00E4D5A0
bool (*ClientGuildMembersWithRecipe::s_handler)(ClientGuildMembersWithRecipe* msg);

// STATIC: 0x00E4D530
bool (*ClientGuildCommandResult::s_handler)(ClientGuildCommandResult* msg);

// STATIC: 0x00E4D7A0
bool (*ClientRolePollBegin::s_handler)(ClientRolePollBegin* msg);

// STATIC: 0x00E4D78C
bool (*ClientGuildKnownRecipes::s_handler)(ClientGuildKnownRecipes* msg);

// STATIC: 0x00E4D6C4
bool (*ClientRaidMarkersChanged::s_handler)(ClientRaidMarkersChanged* msg);

// STATIC: 0x00E4D7D8
bool (*ClientQuestNpcQueryResponse::s_handler)(ClientQuestNpcQueryResponse* msg);

// STATIC: 0x00E4D564
bool (*ClientRoleChangedInform::s_handler)(ClientRoleChangedInform* msg);

// STATIC: 0x00E4D574
bool (*ClientPetitionAlreadySigned::s_handler)(ClientPetitionAlreadySigned* msg);

// STATIC: 0x00E4D444
bool (*ClientSummonRaidMemberValidateFailed::s_handler)(ClientSummonRaidMemberValidateFailed* msg);

// STATIC: 0x00E4D7E4
bool (*ClientStartTimer::s_handler)(ClientStartTimer* msg);

// STATIC: 0x00E4D748
bool (*ClientSpellinterruptlog::s_handler)(ClientSpellinterruptlog* msg);

// STATIC: 0x00E4D4A8
bool (*ClientArenaTeamCommandResult::s_handler)(ClientArenaTeamCommandResult* msg);

// STATIC: 0x00E4D514
bool (*ClientXpGainAborted::s_handler)(ClientXpGainAborted* msg);

// STATIC: 0x00E4D714
bool (*ClientSorStartExperienceIncomplete::s_handler)(ClientSorStartExperienceIncomplete* msg);

// STATIC: 0x00E4D7EC
bool (*ClientDebugRuneRegen::s_handler)(ClientDebugRuneRegen* msg);

// STATIC: 0x00E4D600
bool (*ClientChatIgnoredAccountMuted::s_handler)(ClientChatIgnoredAccountMuted* msg);

// STATIC: 0x00E4D830
bool (*ClientItemRefundInfoResponse::s_handler)(ClientItemRefundInfoResponse* msg);

// STATIC: 0x00E4D578
bool (*ClientItemExpirePurchaseRefund::s_handler)(ClientItemExpirePurchaseRefund* msg);

// STATIC: 0x00E4D6D0
bool (*ClientItemRefundResult::s_handler)(ClientItemRefundResult* msg);

// STATIC: 0x00E4D824
bool (*ClientQuestGiverQuestComplete::s_handler)(ClientQuestGiverQuestComplete* msg);

// STATIC: 0x00E4D4E8
bool (*ClientSupercededSpells::s_handler)(ClientSupercededSpells* msg);

// STATIC: 0x00E4D74C
bool (*ClientLearnedSpell::s_handler)(ClientLearnedSpell* msg);

// STATIC: 0x00E4D48C
bool (*ClientMoveSetActiveMover::s_handler)(ClientMoveSetActiveMover* msg);

// STATIC: 0x00E4D71C
bool (*ClientRequestCemeteryListResponse::s_handler)(ClientRequestCemeteryListResponse* msg);

// STATIC: 0x00E4D5D0
bool (*ClientWorldServerInfo::s_handler)(ClientWorldServerInfo* msg);

// STATIC: 0x00E4D698
bool (*ClientUpdateActionButtons::s_handler)(ClientUpdateActionButtons* msg);

// STATIC: 0x00E4D818
bool (*ClientTimeSyncReq::s_handler)(ClientTimeSyncReq* msg);

// STATIC: 0x00E4D798
bool (*ClientFeatureSystemStatus::s_handler)(ClientFeatureSystemStatus* msg);

// STATIC: 0x00E4D4EC
bool (*ClientCategoryCooldown::s_handler)(ClientCategoryCooldown* msg);

// STATIC: 0x00E4D568
bool (*ClientItemRemovePassive::s_handler)(ClientItemRemovePassive* msg);

// STATIC: 0x00E4D458
bool (*ClientDisenchantCredit::s_handler)(ClientDisenchantCredit* msg);

// STATIC: 0x00E4D6F4
bool (*ClientItemSendPassive::s_handler)(ClientItemSendPassive* msg);

// STATIC: 0x00E4D640
bool (*ClientTimeAdjustment::s_handler)(ClientTimeAdjustment* msg);

// STATIC: 0x00E4D4F8
bool (*ClientItemAddPassive::s_handler)(ClientItemAddPassive* msg);

// OFFSET: 0x006C10E0/0x1403765F0
bool ClientPvpLogData::Handle() {
    if (!ClientPvpLogData::s_handler) {
        return false;
    }

    return ClientPvpLogData::s_handler(this);
}

// PROVEN: 87b01d44 IDENTICAL/NONE
// OFFSET: 0x006C1130/0x140376650
ClientPlaySpellVisualKit::ClientPlaySpellVisualKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8c70b068 IDENTICAL/NONE
// OFFSET: 0x006A28B0/0x14034FD30
ClientPlaySpellVisualKit::~ClientPlaySpellVisualKit() {
}

// PROVEN: 66b3709f IDENTICAL/NONE
// OFFSET: 0x006CA820
bool ClientPlaySpellVisualKit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C0920/0x140375CB0
bool ClientPlaySpellVisualKit::Handle() {
    if (!ClientPlaySpellVisualKit::s_handler) {
        return false;
    }

    return ClientPlaySpellVisualKit::s_handler(this);
}

// OFFSET: 0x006B7900
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

// PROVEN: 6083fa7e IDENTICAL/NONE
// OFFSET: 0x006DEB60/0x14039E700
ClientWeeklySpellUsage::ClientWeeklySpellUsage(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fa5189a5 IDENTICAL/NONE
// OFFSET: 0x006E5A60/0x1403A6FE0
ClientWeeklySpellUsage::~ClientWeeklySpellUsage() {
}

// PROVEN: bbe92838 IDENTICAL/NONE
// OFFSET: 0x006E1F70
bool ClientWeeklySpellUsage::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A5CC0/0x140353CC0
bool ClientWeeklySpellUsage::Handle() {
    if (!ClientWeeklySpellUsage::s_handler) {
        return false;
    }

    return ClientWeeklySpellUsage::s_handler(this);
}

// OFFSET: 0x006DC5B0
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

// PROVEN: f6a5c43f IDENTICAL/NONE
// OFFSET: 0x006B1AE0/0x140350000
ClientWeeklySpellUsageUpdate::~ClientWeeklySpellUsageUpdate() {
}

// PROVEN: f69565c9 IDENTICAL/NONE
// OFFSET: 0x006C52E0
bool ClientWeeklySpellUsageUpdate::Get(CDataStore* msg, bool full) {
    uint8_t uses = 0;
    msg->Get(uses);
    this->m_usage.m_uses = uses;

    uint32_t category = 0;
    msg->Get(category);
    this->m_usage.m_category = category;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AD140/0x14035D210
bool ClientWeeklySpellUsageUpdate::Handle() {
    if (!ClientWeeklySpellUsageUpdate::s_handler) {
        return false;
    }

    return ClientWeeklySpellUsageUpdate::s_handler(this);
}

// PROVEN: d7ff0f75 IDENTICAL/NONE
// OFFSET: 0x006D8620/0x140396710
ClientClearCooldowns::ClientClearCooldowns(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f9e43d14 IDENTICAL/NONE
// OFFSET: 0x006C9A90/0x1403814F0
ClientClearCooldowns::~ClientClearCooldowns() {
}

// PROVEN: dbd0a56e IDENTICAL/NONE
// OFFSET: 0x006D97A0
bool ClientClearCooldowns::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B6020/0x140368670
bool ClientClearCooldowns::Handle() {
    if (!ClientClearCooldowns::s_handler) {
        return false;
    }

    return ClientClearCooldowns::s_handler(this);
}

// OFFSET: 0x006D9350
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

// OFFSET: 0x006D83B0
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

// OFFSET: 0x006F2DB0/0x1403B9980
ClientSetupCurrency::ClientSetupCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F2DA0
bool ClientSetupCurrency::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ECB30
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

// OFFSET: 0x006B0440/0x1403611B0
bool ClientSetupCurrency::Handle() {
    if (!ClientSetupCurrency::s_handler) {
        return false;
    }

    return ClientSetupCurrency::s_handler(this);
}

// OFFSET: 0x006C4580/0x14037A430
ClientSetCurrency::ClientSetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasWeeklyQuantity = false;
    this->m_hasTrackedQuantity = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2A00/0x14034FEF0
ClientSetCurrency::~ClientSetCurrency() {
}

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

// OFFSET: 0x006BF180
bool ClientSetCurrency::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B9470
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

// OFFSET: 0x006BFD40/0x140374F80
bool ClientSetCurrency::Handle() {
    if (!ClientSetCurrency::s_handler) {
        return false;
    }

    return ClientSetCurrency::s_handler(this);
}

// PROVEN: f5a18197 IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientWeeklyResetCurrency::ClientWeeklyResetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 0b497e5b IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientWeeklyResetCurrency::~ClientWeeklyResetCurrency() {
}

// OFFSET: 0x006BAEE0
bool ClientWeeklyResetCurrency::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A3C80/0x140351500
bool ClientWeeklyResetCurrency::Handle() {
    if (!ClientWeeklyResetCurrency::s_handler) {
        return false;
    }

    return ClientWeeklyResetCurrency::s_handler(this);
}

// OFFSET: 0x006BD0A0/0x1403717A0
ClientUpdateCurrencyWeekLimit::ClientUpdateCurrencyWeekLimit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_weekCap = value;

    value = 0;
    msg->Get(value);
    this->m_type = value;
}

// OFFSET: 0x006A10B0/0x14034F400
ClientUpdateCurrencyWeekLimit::~ClientUpdateCurrencyWeekLimit() {
}

// OFFSET: 0x006C8450
bool ClientUpdateCurrencyWeekLimit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

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

// OFFSET: 0x006A6E40/0x1403557A0
bool ClientUpdateCurrencyWeekLimit::Handle() {
    if (!ClientUpdateCurrencyWeekLimit::s_handler) {
        return false;
    }

    return ClientUpdateCurrencyWeekLimit::s_handler(this);
}

// OFFSET: 0x006DD810/0x14039CE50
ClientVendorInventory::ClientVendorInventory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E5360
bool ClientVendorInventory::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E4570
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

// OFFSET: 0x006AA040/0x140359600
bool ClientVendorInventory::Handle() {
    if (!ClientVendorInventory::s_handler) {
        return false;
    }

    return ClientVendorInventory::s_handler(this);
}

// OFFSET: 0x006A1640/0x14034E980
ClientPetSlotUpdated::ClientPetSlotUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0E10/0x14034DEB0
ClientPetSlotUpdated::~ClientPetSlotUpdated() {
}

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

// OFFSET: 0x006CAB10
bool ClientPetSlotUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C47A0
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

// OFFSET: 0x006A92A0/0x140358420
bool ClientPetSlotUpdated::Handle() {
    if (!ClientPetSlotUpdated::s_handler) {
        return false;
    }

    return ClientPetSlotUpdated::s_handler(this);
}

// OFFSET: 0x006B35E0/0x1403650D0
ClientPetAdded::ClientPetAdded(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A12C0/0x14034E590
ClientPetAdded::~ClientPetAdded() {
}

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

// OFFSET: 0x006ADFB0
bool ClientPetAdded::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A98B0
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

// OFFSET: 0x006B6D40/0x140369940
bool ClientPetAdded::Handle() {
    if (!ClientPetAdded::s_handler) {
        return false;
    }

    return ClientPetAdded::s_handler(this);
}

// OFFSET: 0x006A3270/0x14034F7C0
ClientCurrencyLootRemoved::ClientCurrencyLootRemoved(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_lootListID = value;
}

// OFFSET: 0x006A19B0/0x14034E9E0
ClientCurrencyLootRemoved::~ClientCurrencyLootRemoved() {
}

// OFFSET: 0x006BA100
bool ClientCurrencyLootRemoved::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_lootListID = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B0650/0x1403613F0
bool ClientCurrencyLootRemoved::Handle() {
    if (!ClientCurrencyLootRemoved::s_handler) {
        return false;
    }

    return ClientCurrencyLootRemoved::s_handler(this);
}

// OFFSET: 0x006A9430/0x140358630
ClientCurrencyLootRestored::ClientCurrencyLootRestored(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0D70/0x14034DD90
ClientCurrencyLootRestored::~ClientCurrencyLootRestored() {
}

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

// OFFSET: 0x006B2E40
bool ClientCurrencyLootRestored::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A2140
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

// OFFSET: 0x006B06E0/0x1403614B0
bool ClientCurrencyLootRestored::Handle() {
    if (!ClientCurrencyLootRestored::s_handler) {
        return false;
    }

    return ClientCurrencyLootRestored::s_handler(this);
}

// OFFSET: 0x006EF050/0x1403B4150
ClientLootContents::ClientLootContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ECD90
bool ClientLootContents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E23F0
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

// OFFSET: 0x006B81A0/0x14036B250
bool ClientLootContents::Handle() {
    if (!ClientLootContents::s_handler) {
        return false;
    }

    return ClientLootContents::s_handler(this);
}

// OFFSET: 0x006E1710/0x1403A1DF0
ClientPartyInvite::ClientPartyInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006DAE60
bool ClientPartyInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D8A00
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

// OFFSET: 0x006A13C0
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

// OFFSET: 0x006AC2C0/0x14035C140
bool ClientPartyInvite::Handle() {
    if (!ClientPartyInvite::s_handler) {
        return false;
    }

    return ClientPartyInvite::s_handler(this);
}

// OFFSET: 0x006B1AF0/0x140362E70
ClientTradeStatus::ClientTradeStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0EE0/0x14034DFC0
ClientTradeStatus::~ClientTradeStatus() {
}

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

// OFFSET: 0x006CAB20
bool ClientTradeStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C61B0
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

// OFFSET: 0x006C08D0/0x140375C50
bool ClientTradeStatus::Handle() {
    if (!ClientTradeStatus::s_handler) {
        return false;
    }

    return ClientTradeStatus::s_handler(this);
}

// INLINED
JamTradeItem::JamTradeItem() {
    this->m_hasUnwrapped = false;
}

// OFFSET: 0x006F2500/0x1403B8D30
ClientTradeUpdated::ClientTradeUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F4250
bool ClientTradeUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F2DF0
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

// OFFSET: 0x006B1600/0x140362800
bool ClientTradeUpdated::Handle() {
    if (!ClientTradeUpdated::s_handler) {
        return false;
    }

    return ClientTradeUpdated::s_handler(this);
}

// OFFSET: 0x006F4190/0x1403BB500
ClientGuildRoster::ClientGuildRoster(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ED360
bool ClientGuildRoster::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E8BC0
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

// OFFSET: 0x006A5B70/0x140353AF0
bool ClientGuildRoster::Handle() {
    if (!ClientGuildRoster::s_handler) {
        return false;
    }

    return ClientGuildRoster::s_handler(this);
}

// OFFSET: 0x006F0820/0x1403B6200
ClientGuildRosterUpdate::ClientGuildRosterUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F4260
bool ClientGuildRosterUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EA290
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

// OFFSET: 0x006A5B20/0x140353A90
bool ClientGuildRosterUpdate::Handle() {
    if (!ClientGuildRosterUpdate::s_handler) {
        return false;
    }

    return ClientGuildRosterUpdate::s_handler(this);
}

// OFFSET: 0x006E9810/0x1403AC850
ClientGuildRewardList::ClientGuildRewardList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E1E50
bool ClientGuildRewardList::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DD610
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

// OFFSET: 0x006C0180/0x140375430
bool ClientGuildRewardList::Handle() {
    if (!ClientGuildRewardList::s_handler) {
        return false;
    }

    return ClientGuildRewardList::s_handler(this);
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

// OFFSET: 0x006F7540
bool ClientPvpLogData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6510
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

// OFFSET: 0x006F64C0/0x1403BDDE0
ClientGuildNews::ClientGuildNews(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F6310
bool ClientGuildNews::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F56E0
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

// OFFSET: 0x006AE160/0x14035E710
bool ClientGuildNews::Handle() {
    if (!ClientGuildNews::s_handler) {
        return false;
    }

    return ClientGuildNews::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientGuildNewsDeleted::ClientGuildNewsDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_newsID = intValue;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildNewsDeleted::~ClientGuildNewsDeleted() {
}

// OFFSET: 0x006CDE50
bool ClientGuildNewsDeleted::Get(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_newsID = intValue;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B47E0/0x140366800
bool ClientGuildNewsDeleted::Handle() {
    if (!ClientGuildNewsDeleted::s_handler) {
        return false;
    }

    return ClientGuildNewsDeleted::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientGuildReputationWeeklyCap::ClientGuildReputationWeeklyCap(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_repToCap = intValue;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildReputationWeeklyCap::~ClientGuildReputationWeeklyCap() {
}

// OFFSET: 0x006CDE50
bool ClientGuildReputationWeeklyCap::Get(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_repToCap = intValue;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B3B10/0x140365750
bool ClientGuildReputationWeeklyCap::Handle() {
    if (!ClientGuildReputationWeeklyCap::s_handler) {
        return false;
    }

    return ClientGuildReputationWeeklyCap::s_handler(this);
}

// OFFSET: 0x006B3230/0x14035BB30
ClientGuildXPEarned::ClientGuildXPEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_xp = longValue;
}

// OFFSET: 0x006A2650/0x14034ED30
ClientGuildXPEarned::~ClientGuildXPEarned() {
}

// OFFSET: 0x006C3F80
bool ClientGuildXPEarned::Get(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_xp = longValue;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BC8A0/0x140370D80
bool ClientGuildXPEarned::Handle() {
    if (!ClientGuildXPEarned::s_handler) {
        return false;
    }

    return ClientGuildXPEarned::s_handler(this);
}

// OFFSET: 0x006B3230/0x14035BB30
ClientGuildBankMoneyWithdrawn::ClientGuildBankMoneyWithdrawn(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_remainingWithdrawMoney = longValue;
}

// OFFSET: 0x006A2650/0x14034ED30
ClientGuildBankMoneyWithdrawn::~ClientGuildBankMoneyWithdrawn() {
}

// OFFSET: 0x006C3F80
bool ClientGuildBankMoneyWithdrawn::Get(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_remainingWithdrawMoney = longValue;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B6060/0x1403686D0
bool ClientGuildBankMoneyWithdrawn::Handle() {
    if (!ClientGuildBankMoneyWithdrawn::s_handler) {
        return false;
    }

    return ClientGuildBankMoneyWithdrawn::s_handler(this);
}

// OFFSET: 0x006C4910/0x14037A850
ClientGuildChangeNameResult::ClientGuildChangeNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildChangeNameResult::~ClientGuildChangeNameResult() {
}

// OFFSET: 0x006C9980
bool ClientGuildChangeNameResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B93B0/0x14036C950
bool ClientGuildChangeNameResult::Handle() {
    if (!ClientGuildChangeNameResult::s_handler) {
        return false;
    }

    return ClientGuildChangeNameResult::s_handler(this);
}

// OFFSET: 0x006C4910/0x14037A850
ClientGuildFlaggedForRename::ClientGuildFlaggedForRename(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_flagSet = value >> 7;
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildFlaggedForRename::~ClientGuildFlaggedForRename() {
}

// OFFSET: 0x006C9980
bool ClientGuildFlaggedForRename::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_flagSet = value >> 7;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B3C50/0x140365900
bool ClientGuildFlaggedForRename::Handle() {
    if (!ClientGuildFlaggedForRename::s_handler) {
        return false;
    }

    return ClientGuildFlaggedForRename::s_handler(this);
}

// OFFSET: 0x006F42F0/0x1403BB6C0
ClientGuildRanks::ClientGuildRanks(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F42D0
bool ClientGuildRanks::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EF690
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

// OFFSET: 0x006B2870/0x140363F20
bool ClientGuildRanks::Handle() {
    if (!ClientGuildRanks::s_handler) {
        return false;
    }

    return ClientGuildRanks::s_handler(this);
}

// OFFSET: 0x006CF0A0/0x140389250
ClientGuildMemberUpdateNote::ClientGuildMemberUpdateNote(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A23F0/0x14034F900
ClientGuildMemberUpdateNote::~ClientGuildMemberUpdateNote() {
}

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

// OFFSET: 0x006B0430
bool ClientGuildMemberUpdateNote::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AC300
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

// OFFSET: 0x006A5610/0x1403533F0
bool ClientGuildMemberUpdateNote::Handle() {
    if (!ClientGuildMemberUpdateNote::s_handler) {
        return false;
    }

    return ClientGuildMemberUpdateNote::s_handler(this);
}

// OFFSET: 0x006AC0A0/0x14035BEA0
ClientGuildInvite::ClientGuildInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F50/0x14034F3F0
ClientGuildInvite::~ClientGuildInvite() {
}

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

// OFFSET: 0x006CF290
bool ClientGuildInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C79B0
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

// OFFSET: 0x006BEEB0/0x140373E20
bool ClientGuildInvite::Handle() {
    if (!ClientGuildInvite::s_handler) {
        return false;
    }

    return ClientGuildInvite::s_handler(this);
}

// OFFSET: 0x006CF060/0x140389210
ClientGuildPartyState::ClientGuildPartyState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0E00/0x14034DEA0
ClientGuildPartyState::~ClientGuildPartyState() {
}

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

// OFFSET: 0x006CF340
bool ClientGuildPartyState::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C4CC0
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

// OFFSET: 0x006C07C0/0x140375B00
bool ClientGuildPartyState::Handle() {
    if (!ClientGuildPartyState::s_handler) {
        return false;
    }

    return ClientGuildPartyState::s_handler(this);
}

// OFFSET: 0x006BE830/0x1403735E0
ClientGuildReputationReactionChanged::ClientGuildReputationReactionChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DB0/0x14034DDD0
ClientGuildReputationReactionChanged::~ClientGuildReputationReactionChanged() {
}

// PROVEN: ed58802d IDENTICAL/NONE
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

// OFFSET: 0x006CAD50
bool ClientGuildReputationReactionChanged::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B3050
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

// OFFSET: 0x006BC180/0x140370470
bool ClientGuildReputationReactionChanged::Handle() {
    if (!ClientGuildReputationReactionChanged::s_handler) {
        return false;
    }

    return ClientGuildReputationReactionChanged::s_handler(this);
}

// OFFSET: 0x006A2B20/0x140350060
ClientGuildChallengeUpdate::ClientGuildChallengeUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1FD0/0x14034F440
ClientGuildChallengeUpdate::~ClientGuildChallengeUpdate() {
}

// PROVEN: ef75a511 IDENTICAL/NONE
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

// OFFSET: 0x006C6100
bool ClientGuildChallengeUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B9530
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

// OFFSET: 0x006ADB30/0x14035DF40
bool ClientGuildChallengeUpdate::Handle() {
    if (!ClientGuildChallengeUpdate::s_handler) {
        return false;
    }

    return ClientGuildChallengeUpdate::s_handler(this);
}

// OFFSET: 0x006A22E0/0x14034F810
ClientGuildChallengeCompleted::ClientGuildChallengeCompleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2680/0x14034FB60
ClientGuildChallengeCompleted::~ClientGuildChallengeCompleted() {
}

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

// OFFSET: 0x006BE870
bool ClientGuildChallengeCompleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B7870
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

// OFFSET: 0x006A8CB0/0x140357C90
bool ClientGuildChallengeCompleted::Handle() {
    if (!ClientGuildChallengeCompleted::s_handler) {
        return false;
    }

    return ClientGuildChallengeCompleted::s_handler(this);
}

// OFFSET: 0x006BF330/0x140374390
ClientGuildReset::ClientGuildReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1140/0x14034E3E0
ClientGuildReset::~ClientGuildReset() {
}

// PROVEN: 34e0ea27 IDENTICAL/NONE
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

// OFFSET: 0x006C3E00
bool ClientGuildReset::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A9C10
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

// OFFSET: 0x006A3230/0x1403509C0
bool ClientGuildReset::Handle() {
    if (!ClientGuildReset::s_handler) {
        return false;
    }

    return ClientGuildReset::s_handler(this);
}

// OFFSET: 0x006D97F0/0x140397CB0
ClientGuildMoveComplete::ClientGuildMoveComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A21C0/0x14034F6C0
ClientGuildMoveComplete::~ClientGuildMoveComplete() {
}

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

// OFFSET: 0x006C9AC0
bool ClientGuildMoveComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C8480
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

// OFFSET: 0x006ADB90/0x14035DFB0
bool ClientGuildMoveComplete::Handle() {
    if (!ClientGuildMoveComplete::s_handler) {
        return false;
    }

    return ClientGuildMoveComplete::s_handler(this);
}

// OFFSET: 0x006B6E10/0x140369A40
ClientGuildMoveStarting::ClientGuildMoveStarting(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2E20/0x140350380
ClientGuildMoveStarting::~ClientGuildMoveStarting() {
}

// PROVEN: 78b17c60 IDENTICAL/NONE
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

// OFFSET: 0x006B74C0
bool ClientGuildMoveStarting::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A5970
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

// OFFSET: 0x006B6D80/0x1403699A0
bool ClientGuildMoveStarting::Handle() {
    if (!ClientGuildMoveStarting::s_handler) {
        return false;
    }

    return ClientGuildMoveStarting::s_handler(this);
}

// OFFSET: 0x006D1EB0/0x14038D470
ClientGuildNameChanged::ClientGuildNameChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0EC0/0x14034DF80
ClientGuildNameChanged::~ClientGuildNameChanged() {
}

// OFFSET: 0x006BF920
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

// OFFSET: 0x006C09A0
bool ClientGuildNameChanged::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AC570
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

// OFFSET: 0x006AC860/0x14035C710
bool ClientGuildNameChanged::Handle() {
    if (!ClientGuildNameChanged::s_handler) {
        return false;
    }

    return ClientGuildNameChanged::s_handler(this);
}

// OFFSET: 0x006E1750/0x1403A1E40
ClientGuildPermissionsQueryResults::ClientGuildPermissionsQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E8110
bool ClientGuildPermissionsQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E1E60
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

// OFFSET: 0x006AC8B0/0x14035C770
bool ClientGuildPermissionsQueryResults::Handle() {
    if (!ClientGuildPermissionsQueryResults::s_handler) {
        return false;
    }

    return ClientGuildPermissionsQueryResults::s_handler(this);
}

// OFFSET: 0x006E9330/0x14039CCB0
ClientGuildEventLogQueryResults::ClientGuildEventLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E23E0
bool ClientGuildEventLogQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E17A0
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

// OFFSET: 0x006BFD00/0x140374F20
bool ClientGuildEventLogQueryResults::Handle() {
    if (!ClientGuildEventLogQueryResults::s_handler) {
        return false;
    }

    return ClientGuildEventLogQueryResults::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientGuildMemberDailyReset::ClientGuildMemberDailyReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 82de6f74 IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientGuildMemberDailyReset::~ClientGuildMemberDailyReset() {
}

// OFFSET: 0x006BAEE0
bool ClientGuildMemberDailyReset::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006BF450/0x1403744E0
bool ClientGuildMemberDailyReset::Handle() {
    if (!ClientGuildMemberDailyReset::s_handler) {
        return false;
    }

    return ClientGuildMemberDailyReset::s_handler(this);
}

// OFFSET: 0x006F73B0/0x1403C1170
ClientGuildBankQueryResults::ClientGuildBankQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F7550
bool ClientGuildBankQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6F10
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

// OFFSET: 0x006C86A0
bool ClientGuildBankQueryResults::IsValid() const {
    for (uint32_t i = 0; i < this->m_tabInfo.m_count; i++) {
        if (!this->m_tabInfo.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x006B1640/0x140362860
bool ClientGuildBankQueryResults::Handle() {
    if (!ClientGuildBankQueryResults::s_handler) {
        return false;
    }

    return ClientGuildBankQueryResults::s_handler(this);
}

// OFFSET: 0x006EAF10/0x1403AE670
ClientGuildBankLogQueryResults::ClientGuildBankLogQueryResults(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasWeeklyBonusMoney = false;
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F0060
bool ClientGuildBankLogQueryResults::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EDBA0
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

// OFFSET: 0x006AA4E0/0x140359B60
bool ClientGuildBankLogQueryResults::Handle() {
    if (!ClientGuildBankLogQueryResults::s_handler) {
        return false;
    }

    return ClientGuildBankLogQueryResults::s_handler(this);
}

// OFFSET: 0x006D7730/0x1403953E0
ClientGuildBankTextQueryResult::ClientGuildBankTextQueryResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0D80/0x14034DDA0
ClientGuildBankTextQueryResult::~ClientGuildBankTextQueryResult() {
}

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

// OFFSET: 0x006D90C0
bool ClientGuildBankTextQueryResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D45D0
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

// OFFSET: 0x006B4220/0x140366080
bool ClientGuildBankTextQueryResult::Handle() {
    if (!ClientGuildBankTextQueryResult::s_handler) {
        return false;
    }

    return ClientGuildBankTextQueryResult::s_handler(this);
}

// OFFSET: 0x006C8800/0x14037F8D0
ClientMoveSetActiveMover::ClientMoveSetActiveMover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A20D0/0x14034F550
ClientMoveSetActiveMover::~ClientMoveSetActiveMover() {
}

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

// OFFSET: 0x006CAB60
bool ClientMoveSetActiveMover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B3FA0
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

// OFFSET: 0x006B2A30/0x140364170
bool ClientMoveSetActiveMover::Handle() {
    if (!ClientMoveSetActiveMover::s_handler) {
        return false;
    }

    return ClientMoveSetActiveMover::s_handler(this);
}

// OFFSET: 0x006DD590/0x14039CA20
ClientRequestCemeteryListResponse::ClientRequestCemeteryListResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006DDB60
bool ClientRequestCemeteryListResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D9B00
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

// OFFSET: 0x006C0DA0/0x140376250
bool ClientRequestCemeteryListResponse::Handle() {
    if (!ClientRequestCemeteryListResponse::s_handler) {
        return false;
    }

    return ClientRequestCemeteryListResponse::s_handler(this);
}

// OFFSET: 0x006B4F80/0x140367130
ClientWorldServerInfo::ClientWorldServerInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasRestrictedMoney = false;
    this->m_hasRestrictedLevel = false;
    this->m_hasIneligibleForLootMask = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F10/0x14034F390
ClientWorldServerInfo::~ClientWorldServerInfo() {
}

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

// OFFSET: 0x006C7950
bool ClientWorldServerInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B4260
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

// OFFSET: 0x006BF840/0x1403749E0
bool ClientWorldServerInfo::Handle() {
    if (!ClientWorldServerInfo::s_handler) {
        return false;
    }

    return ClientWorldServerInfo::s_handler(this);
}

// OFFSET: 0x006A2DA0/0x140350300
ClientUpdateActionButtons::ClientUpdateActionButtons(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1990/0x14034ED50
ClientUpdateActionButtons::~ClientUpdateActionButtons() {
}

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

// OFFSET: 0x006BF190
bool ClientUpdateActionButtons::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AE760
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

// OFFSET: 0x006A9220/0x140358360
bool ClientUpdateActionButtons::Handle() {
    if (!ClientUpdateActionButtons::s_handler) {
        return false;
    }

    return ClientUpdateActionButtons::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientTimeSyncReq::ClientTimeSyncReq(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientTimeSyncReq::~ClientTimeSyncReq() {
}

// OFFSET: 0x006CDE50
bool ClientTimeSyncReq::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A5C80/0x140353C60
bool ClientTimeSyncReq::Handle() {
    if (!ClientTimeSyncReq::s_handler) {
        return false;
    }

    return ClientTimeSyncReq::s_handler(this);
}

// OFFSET: 0x006D7160/0x140394DC0
ClientFeatureSystemStatus::ClientFeatureSystemStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_sessionAlertEnabled = false;
    this->m_europaTicketSystemEnabled = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006D0470/0x14038AE70
ClientFeatureSystemStatus::~ClientFeatureSystemStatus() {
}

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

// OFFSET: 0x006D9600
bool ClientFeatureSystemStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5800
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

// OFFSET: 0x006A72E0/0x140355D30
bool ClientFeatureSystemStatus::Handle() {
    if (!ClientFeatureSystemStatus::s_handler) {
        return false;
    }

    return ClientFeatureSystemStatus::s_handler(this);
}

// OFFSET: 0x006E9850/0x1403AC8A0
ClientCategoryCooldown::ClientCategoryCooldown(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ED920
bool ClientCategoryCooldown::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E37D0
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

// OFFSET: 0x006AAF60/0x14035A8D0
bool ClientCategoryCooldown::Handle() {
    if (!ClientCategoryCooldown::s_handler) {
        return false;
    }

    return ClientCategoryCooldown::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientItemRemovePassive::ClientItemRemovePassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientItemRemovePassive::~ClientItemRemovePassive() {
}

// OFFSET: 0x006CDE50
bool ClientItemRemovePassive::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C1320/0x1403768B0
bool ClientItemRemovePassive::Handle() {
    if (!ClientItemRemovePassive::s_handler) {
        return false;
    }

    return ClientItemRemovePassive::s_handler(this);
}

// OFFSET: 0x006C0020/0x1403752C0
ClientDisenchantCredit::ClientDisenchantCredit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F90/0x14034F410
ClientDisenchantCredit::~ClientDisenchantCredit() {
}

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

// OFFSET: 0x006C4390
bool ClientDisenchantCredit::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BFD90
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

// OFFSET: 0x006BC580/0x140370980
bool ClientDisenchantCredit::Handle() {
    if (!ClientDisenchantCredit::s_handler) {
        return false;
    }

    return ClientDisenchantCredit::s_handler(this);
}

// OFFSET: 0x006D97B0/0x140397C60
ClientItemSendPassive::ClientItemSendPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006DD770
bool ClientItemSendPassive::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D9660
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

// OFFSET: 0x006BF4F0/0x1403745B0
bool ClientItemSendPassive::Handle() {
    if (!ClientItemSendPassive::s_handler) {
        return false;
    }

    return ClientItemSendPassive::s_handler(this);
}

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

// OFFSET: 0x006A1090/0x14034E2D0
ClientTimeAdjustment::~ClientTimeAdjustment() {
}

// OFFSET: 0x006C3190
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

// OFFSET: 0x006BAE20/0x14036EAC0
bool ClientTimeAdjustment::Handle() {
    if (!ClientTimeAdjustment::s_handler) {
        return false;
    }

    return ClientTimeAdjustment::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientItemAddPassive::ClientItemAddPassive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientItemAddPassive::~ClientItemAddPassive() {
}

// OFFSET: 0x006CDE50
bool ClientItemAddPassive::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B0510/0x1403612E0
bool ClientItemAddPassive::Handle() {
    if (!ClientItemAddPassive::s_handler) {
        return false;
    }

    return ClientItemAddPassive::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlegroundInfoThrottled::ClientBattlegroundInfoThrottled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientBattlegroundInfoThrottled::~ClientBattlegroundInfoThrottled() {
}

// OFFSET: 0x006BAEE0
bool ClientBattlegroundInfoThrottled::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006B04D0/0x140361280
bool ClientBattlegroundInfoThrottled::Handle() {
    if (!ClientBattlegroundInfoThrottled::s_handler) {
        return false;
    }

    return ClientBattlegroundInfoThrottled::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientChatIgnoredAccountMuted::ClientChatIgnoredAccountMuted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientChatIgnoredAccountMuted::~ClientChatIgnoredAccountMuted() {
}

// OFFSET: 0x006BAEE0
bool ClientChatIgnoredAccountMuted::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006B2230/0x1403637D0
bool ClientChatIgnoredAccountMuted::Handle() {
    if (!ClientChatIgnoredAccountMuted::s_handler) {
        return false;
    }

    return ClientChatIgnoredAccountMuted::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientSorStartExperienceIncomplete::ClientSorStartExperienceIncomplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientSorStartExperienceIncomplete::~ClientSorStartExperienceIncomplete() {
}

// OFFSET: 0x006BAEE0
bool ClientSorStartExperienceIncomplete::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006B5D30/0x1403682B0
bool ClientSorStartExperienceIncomplete::Handle() {
    if (!ClientSorStartExperienceIncomplete::s_handler) {
        return false;
    }

    return ClientSorStartExperienceIncomplete::s_handler(this);
}

// OFFSET: 0x006C8760/0x14037F830
ClientGuildCommandResult::ClientGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1DC0/0x14034F240
ClientGuildCommandResult::~ClientGuildCommandResult() {
}

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

// OFFSET: 0x006B5BA0
bool ClientGuildCommandResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B3B60
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

// OFFSET: 0x006BD100/0x140371820
bool ClientGuildCommandResult::Handle() {
    if (!ClientGuildCommandResult::s_handler) {
        return false;
    }

    return ClientGuildCommandResult::s_handler(this);
}

// OFFSET: 0x006C60C0/0x14037C4E0
ClientArenaTeamCommandResult::ClientArenaTeamCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2A90/0x14034FF70
ClientArenaTeamCommandResult::~ClientArenaTeamCommandResult() {
}

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

// OFFSET: 0x006BF0D0
bool ClientArenaTeamCommandResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AEDD0
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

// OFFSET: 0x006A25F0
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

// OFFSET: 0x006B3E10/0x140365B50
bool ClientArenaTeamCommandResult::Handle() {
    if (!ClientArenaTeamCommandResult::s_handler) {
        return false;
    }

    return ClientArenaTeamCommandResult::s_handler(this);
}

// OFFSET: 0x006DC660/0x14039B870
ClientGuildMembersWithRecipe::ClientGuildMembersWithRecipe(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006D9270
bool ClientGuildMembersWithRecipe::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D8D60
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

// OFFSET: 0x006C0CC0/0x140376130
bool ClientGuildMembersWithRecipe::Handle() {
    if (!ClientGuildMembersWithRecipe::s_handler) {
        return false;
    }

    return ClientGuildMembersWithRecipe::s_handler(this);
}

// OFFSET: 0x006C0A50/0x140375E50
ClientRoleChangedInform::ClientRoleChangedInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2EE0/0x1403503F0
ClientRoleChangedInform::~ClientRoleChangedInform() {
}

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

// OFFSET: 0x006A6DF0
bool ClientRoleChangedInform::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A68D0
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

// OFFSET: 0x006AB430/0x14035AF30
bool ClientRoleChangedInform::Handle() {
    if (!ClientRoleChangedInform::s_handler) {
        return false;
    }

    return ClientRoleChangedInform::s_handler(this);
}

// OFFSET: 0x006C45E0/0x14037A490
ClientRolePollBegin::ClientRolePollBegin(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DC0/0x14034DDE0
ClientRolePollBegin::~ClientRolePollBegin() {
}

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

// OFFSET: 0x006B02D0
bool ClientRolePollBegin::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AC160
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

// OFFSET: 0x006AE720/0x14035EDF0
bool ClientRolePollBegin::Handle() {
    if (!ClientRolePollBegin::s_handler) {
        return false;
    }

    return ClientRolePollBegin::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientRaidMarkersChanged::ClientRaidMarkersChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_raidMarkers = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientRaidMarkersChanged::~ClientRaidMarkersChanged() {
}

// OFFSET: 0x006CDE50
bool ClientRaidMarkersChanged::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_raidMarkers = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B8360/0x14036B4B0
bool ClientRaidMarkersChanged::Handle() {
    if (!ClientRaidMarkersChanged::s_handler) {
        return false;
    }

    return ClientRaidMarkersChanged::s_handler(this);
}

// OFFSET: 0x006B3560/0x140365030
ClientPetitionAlreadySigned::ClientPetitionAlreadySigned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A14E0/0x14034E7B0
ClientPetitionAlreadySigned::~ClientPetitionAlreadySigned() {
}

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

// OFFSET: 0x006BC650
bool ClientPetitionAlreadySigned::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B1D40
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

// OFFSET: 0x006A97D0/0x140358B10
bool ClientPetitionAlreadySigned::Handle() {
    if (!ClientPetitionAlreadySigned::s_handler) {
        return false;
    }

    return ClientPetitionAlreadySigned::s_handler(this);
}

// OFFSET: 0x006E2C50/0x1403A3780
ClientSummonRaidMemberValidateFailed::ClientSummonRaidMemberValidateFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E53B0
bool ClientSummonRaidMemberValidateFailed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DE630
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

// OFFSET: 0x006A5650/0x140353450
bool ClientSummonRaidMemberValidateFailed::Handle() {
    if (!ClientSummonRaidMemberValidateFailed::s_handler) {
        return false;
    }

    return ClientSummonRaidMemberValidateFailed::s_handler(this);
}

// OFFSET: 0x006F74F0/0x1403BEE80
ClientPvpLogData::ClientPvpLogData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTeamNames = false;
    this->m_hasWinner = false;
    this->m_hasRatings = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006F8280/0x1403C0830
ClientQuestNpcQueryResponse::ClientQuestNpcQueryResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F6500
bool ClientQuestNpcQueryResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006F6320
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

// OFFSET: 0x006B5090/0x140367280
bool ClientQuestNpcQueryResponse::Handle() {
    if (!ClientQuestNpcQueryResponse::s_handler) {
        return false;
    }

    return ClientQuestNpcQueryResponse::s_handler(this);
}

// OFFSET: 0x006C0D00/0x140376190
ClientSpellinterruptlog::ClientSpellinterruptlog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2D90/0x1403502F0
ClientSpellinterruptlog::~ClientSpellinterruptlog() {
}

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

// OFFSET: 0x006BF370
bool ClientSpellinterruptlog::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BDCB0
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

// OFFSET: 0x006C0800/0x140375B60
bool ClientSpellinterruptlog::Handle() {
    if (!ClientSpellinterruptlog::s_handler) {
        return false;
    }

    return ClientSpellinterruptlog::s_handler(this);
}

// OFFSET: 0x006C7960/0x14037E7E0
ClientXpGainAborted::ClientXpGainAborted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2640/0x14034FB30
ClientXpGainAborted::~ClientXpGainAborted() {
}

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

// OFFSET: 0x006CB4B0
bool ClientXpGainAborted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C4050
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

// OFFSET: 0x006AD340/0x14035D480
bool ClientXpGainAborted::Handle() {
    if (!ClientXpGainAborted::s_handler) {
        return false;
    }

    return ClientXpGainAborted::s_handler(this);
}

// OFFSET: 0x006B1EA0/0x140363320
ClientItemExpirePurchaseRefund::ClientItemExpirePurchaseRefund(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1230/0x14034E510
ClientItemExpirePurchaseRefund::~ClientItemExpirePurchaseRefund() {
}

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

// OFFSET: 0x006AF240
bool ClientItemExpirePurchaseRefund::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A30D0
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

// OFFSET: 0x006BE2C0/0x140372F50
bool ClientItemExpirePurchaseRefund::Handle() {
    if (!ClientItemExpirePurchaseRefund::s_handler) {
        return false;
    }

    return ClientItemExpirePurchaseRefund::s_handler(this);
}

// OFFSET: 0x006A2A20/0x14034FF10
ClientGuildSendGuildXP::ClientGuildSendGuildXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DE0/0x14034DE80
ClientGuildSendGuildXP::~ClientGuildSendGuildXP() {
}

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

// OFFSET: 0x006C4D50
bool ClientGuildSendGuildXP::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B4EC0
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

// OFFSET: 0x006B5720/0x140367AF0
bool ClientGuildSendGuildXP::Handle() {
    if (!ClientGuildSendGuildXP::s_handler) {
        return false;
    }

    return ClientGuildSendGuildXP::s_handler(this);
}

// OFFSET: 0x006B3230/0x14035BB30
ClientGuildSendMaxDailyXP::ClientGuildSendMaxDailyXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_xp = value;
}

// OFFSET: 0x006A2650/0x14034ED30
ClientGuildSendMaxDailyXP::~ClientGuildSendMaxDailyXP() {
}

// OFFSET: 0x006C3F80
bool ClientGuildSendMaxDailyXP::Get(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_xp = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BE040/0x140372C20
bool ClientGuildSendMaxDailyXP::Handle() {
    if (!ClientGuildSendMaxDailyXP::s_handler) {
        return false;
    }

    return ClientGuildSendMaxDailyXP::s_handler(this);
}

// OFFSET: 0x006B93F0/0x14036C9B0
ClientGuildSendRankChange::ClientGuildSendRankChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0ED0/0x14034DF90
ClientGuildSendRankChange::~ClientGuildSendRankChange() {
}

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

// OFFSET: 0x006C4570
bool ClientGuildSendRankChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B5760
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

// OFFSET: 0x006AB240/0x14035ACA0
bool ClientGuildSendRankChange::Handle() {
    if (!ClientGuildSendRankChange::s_handler) {
        return false;
    }

    return ClientGuildSendRankChange::s_handler(this);
}

// OFFSET: 0x006C7880/0x14037E6B0
ClientGuildMemberRecipes::ClientGuildMemberRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1980/0x14034ED40
ClientGuildMemberRecipes::~ClientGuildMemberRecipes() {
}

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

// OFFSET: 0x006C1170
bool ClientGuildMemberRecipes::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B0E30
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

// OFFSET: 0x006BE7F0/0x140373580
bool ClientGuildMemberRecipes::Handle() {
    if (!ClientGuildMemberRecipes::s_handler) {
        return false;
    }

    return ClientGuildMemberRecipes::s_handler(this);
}

// OFFSET: 0x006EE9F0/0x1403B37D0
ClientGuildKnownRecipes::ClientGuildKnownRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E5A20
bool ClientGuildKnownRecipes::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E21F0
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

// OFFSET: 0x006A3300/0x140350A80
bool ClientGuildKnownRecipes::Handle() {
    if (!ClientGuildKnownRecipes::s_handler) {
        return false;
    }

    return ClientGuildKnownRecipes::s_handler(this);
}

// OFFSET: 0x006A6B80/0x140355050
ClientStartTimer::ClientStartTimer(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A13A0/0x14034E680
ClientStartTimer::~ClientStartTimer() {
}

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

// OFFSET: 0x006B8420
bool ClientStartTimer::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AE3E0
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

// OFFSET: 0x006B64E0/0x140368C60
bool ClientStartTimer::Handle() {
    if (!ClientStartTimer::s_handler) {
        return false;
    }

    return ClientStartTimer::s_handler(this);
}

// OFFSET: 0x006D6510/0x140393A60
ClientItemRefundInfoResponse::ClientItemRefundInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006CA830/0x140382900
ClientItemRefundInfoResponse::~ClientItemRefundInfoResponse() {
}

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

// OFFSET: 0x006E1F80
bool ClientItemRefundInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D98D0
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

// OFFSET: 0x006ABBF0/0x14035B8E0
bool ClientItemRefundInfoResponse::Handle() {
    if (!ClientItemRefundInfoResponse::s_handler) {
        return false;
    }

    return ClientItemRefundInfoResponse::s_handler(this);
}

// OFFSET: 0x006D9620/0x140397A60
ClientItemRefundResult::ClientItemRefundResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_result.m_hasCost = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006DA120/0x140398720
ClientItemRefundResult::~ClientItemRefundResult() {
}

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

// OFFSET: 0x006E1F60
bool ClientItemRefundResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DD910
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

// OFFSET: 0x006BB780/0x14036F740
bool ClientItemRefundResult::Handle() {
    if (!ClientItemRefundResult::s_handler) {
        return false;
    }

    return ClientItemRefundResult::s_handler(this);
}

// OFFSET: 0x006C1180/0x1403766A0
ClientQuestGiverQuestComplete::ClientQuestGiverQuestComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1DB0/0x14034F230
ClientQuestGiverQuestComplete::~ClientQuestGiverQuestComplete() {
}

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

// OFFSET: 0x006C7910
bool ClientQuestGiverQuestComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B6C70
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

// OFFSET: 0x006B07E0/0x1403615F0
bool ClientQuestGiverQuestComplete::Handle() {
    if (!ClientQuestGiverQuestComplete::s_handler) {
        return false;
    }

    return ClientQuestGiverQuestComplete::s_handler(this);
}

// OFFSET: 0x006BD0A0/0x1403717A0
ClientSupercededSpells::ClientSupercededSpells(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_supercededSpellID = value;

    value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A10B0/0x14034F400
ClientSupercededSpells::~ClientSupercededSpells() {
}

// OFFSET: 0x006C8450
bool ClientSupercededSpells::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

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

// OFFSET: 0x006B44D0/0x140366400
bool ClientSupercededSpells::Handle() {
    if (!ClientSupercededSpells::s_handler) {
        return false;
    }

    return ClientSupercededSpells::s_handler(this);
}

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

// OFFSET: 0x006A1E70/0x14034F2E0
ClientLearnedSpell::~ClientLearnedSpell() {
}

// OFFSET: 0x006CB500
bool ClientLearnedSpell::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

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

// OFFSET: 0x006ABD00/0x14035B9A0
bool ClientLearnedSpell::Handle() {
    if (!ClientLearnedSpell::s_handler) {
        return false;
    }

    return ClientLearnedSpell::s_handler(this);
}

// OFFSET: 0x006A2AD0/0x140350010
ClientDebugRuneRegen::ClientDebugRuneRegen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0D90/0x14034DDB0
ClientDebugRuneRegen::~ClientDebugRuneRegen() {
}

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

// OFFSET: 0x006C43A0
bool ClientDebugRuneRegen::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ACD80
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

// OFFSET: 0x006A8D50/0x140357D00
bool ClientDebugRuneRegen::Handle() {
    if (!ClientDebugRuneRegen::s_handler) {
        return false;
    }

    return ClientDebugRuneRegen::s_handler(this);
}

// OFFSET: 0x006C4910/0x14037A850
ClientReforgeResult::ClientReforgeResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientReforgeResult::~ClientReforgeResult() {
}

// OFFSET: 0x006C9980
bool ClientReforgeResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = value >> 7;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A4A80/0x140352890
bool ClientReforgeResult::Handle() {
    if (!ClientReforgeResult::s_handler) {
        return false;
    }

    return ClientReforgeResult::s_handler(this);
}

// OFFSET: 0x006C0A90/0x140375E90
ClientForgeMasterSet::ClientForgeMasterSet(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1220/0x14034E500
ClientForgeMasterSet::~ClientForgeMasterSet() {
}

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

// OFFSET: 0x006BED10
bool ClientForgeMasterSet::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A4230
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

// OFFSET: 0x006A8160/0x140356E40
bool ClientForgeMasterSet::Handle() {
    if (!ClientForgeMasterSet::s_handler) {
        return false;
    }

    return ClientForgeMasterSet::s_handler(this);
}

// OFFSET: 0x006A3270/0x14034F7C0
ClientVoidStorageFailed::ClientVoidStorageFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;
}

// OFFSET: 0x006A19B0/0x14034E9E0
ClientVoidStorageFailed::~ClientVoidStorageFailed() {
}

// OFFSET: 0x006BA100
bool ClientVoidStorageFailed::Get(CDataStore* msg, bool full) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AD480/0x14035D660
bool ClientVoidStorageFailed::Handle() {
    if (!ClientVoidStorageFailed::s_handler) {
        return false;
    }

    return ClientVoidStorageFailed::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientVoidTransferResult::ClientVoidTransferResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value;

    value = 0;
    msg->Get(value);
    this->m_result = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientVoidTransferResult::~ClientVoidTransferResult() {
}

// OFFSET: 0x006CDE50
bool ClientVoidTransferResult::Get(CDataStore* msg, bool full) {
    uint32_t value;

    value = 0;
    msg->Get(value);
    this->m_result = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A3CC0/0x140351560
bool ClientVoidTransferResult::Handle() {
    if (!ClientVoidTransferResult::s_handler) {
        return false;
    }

    return ClientVoidTransferResult::s_handler(this);
}

// OFFSET: 0x006E5370/0x1403A66E0
ClientVoidStorageContents::ClientVoidStorageContents(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E1790
bool ClientVoidStorageContents::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E1090
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

// OFFSET: 0x006B06A0/0x140361450
bool ClientVoidStorageContents::Handle() {
    if (!ClientVoidStorageContents::s_handler) {
        return false;
    }

    return ClientVoidStorageContents::s_handler(this);
}

// OFFSET: 0x006DEAD0/0x14039E610
ClientVoidStorageTransferChanges::ClientVoidStorageTransferChanges(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006EE950
bool ClientVoidStorageTransferChanges::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E49B0
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

// OFFSET: 0x006A6DB0/0x1403556D0
bool ClientVoidStorageTransferChanges::Handle() {
    if (!ClientVoidStorageTransferChanges::s_handler) {
        return false;
    }

    return ClientVoidStorageTransferChanges::s_handler(this);
}

// OFFSET: 0x006C9940/0x140381380
ClientVoidItemSwapResponse::ClientVoidItemSwapResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2E70/0x1403503A0
ClientVoidItemSwapResponse::~ClientVoidItemSwapResponse() {
}

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

// OFFSET: 0x006B5B90
bool ClientVoidItemSwapResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A6E90
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

// OFFSET: 0x006A5170/0x1403530D0
bool ClientVoidItemSwapResponse::Handle() {
    if (!ClientVoidItemSwapResponse::s_handler) {
        return false;
    }

    return ClientVoidItemSwapResponse::s_handler(this);
}

// OFFSET: 0x006A9D70
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

// OFFSET: 0x006E2520
void ClientLFGBlackList::operator=(const ClientLFGBlackList& source) {
    this->unk_0008.SetCount(source.unk_0008.m_count);

    for (uint32_t i = 0; i < this->unk_0008.Count(); i++) {
        this->unk_0008[i] = source.unk_0008.m_data[i];
    }

    memcpy(this->unk_0000, source.unk_0000, sizeof(this->unk_0000));
}

// OFFSET: 0x006DD150
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

// OFFSET: 0x006D9FE0
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

// OFFSET: 0x006ABD40
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

// OFFSET: 0x006B55B0
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

// OFFSET: 0x006C0070
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

// OFFSET: 0x006ABA80
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

// OFFSET: 0x006D4FB0/0x140391DF0
ClientLfGuildPostUpdated::~ClientLfGuildPostUpdated() {
}

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

// OFFSET: 0x006CAB70
bool ClientLfGuildPostUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C5D80
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

// OFFSET: 0x006BED20/0x140373C10
bool ClientLfGuildPostUpdated::Handle() {
    if (!ClientLfGuildPostUpdated::s_handler) {
        return false;
    }

    return ClientLfGuildPostUpdated::s_handler(this);
}

// OFFSET: 0x006F07A0/0x1403B6160
ClientLfGuildBrowseUpdated::ClientLfGuildBrowseUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F42C0
bool ClientLfGuildBrowseUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EF090
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

// OFFSET: 0x006A43D0
bool ClientLfGuildBrowseUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_guilds.m_count; i++) {
        if (!this->m_guilds.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x006AD100/0x14035D1B0
bool ClientLfGuildBrowseUpdated::Handle() {
    if (!ClientLfGuildBrowseUpdated::s_handler) {
        return false;
    }

    return ClientLfGuildBrowseUpdated::s_handler(this);
}

// OFFSET: 0x006F3670/0x1403BA390
ClientLfGuildRecruitListUpdated::ClientLfGuildRecruitListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ED350
bool ClientLfGuildRecruitListUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E76D0
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

// OFFSET: 0x006B27C0
bool ClientLfGuildRecruitListUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_recruits.m_count; i++) {
        if (!this->m_recruits.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x006BE0D0/0x140372CC0
bool ClientLfGuildRecruitListUpdated::Handle() {
    if (!ClientLfGuildRecruitListUpdated::s_handler) {
        return false;
    }

    return ClientLfGuildRecruitListUpdated::s_handler(this);
}

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

// OFFSET: 0x006B1AE0/0x140350000
ClientLfGuildCommandResult::~ClientLfGuildCommandResult() {
}

// OFFSET: 0x006C52E0
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

// OFFSET: 0x006A6650/0x1403549C0
bool ClientLfGuildCommandResult::Handle() {
    if (!ClientLfGuildCommandResult::s_handler) {
        return false;
    }

    return ClientLfGuildCommandResult::s_handler(this);
}

// OFFSET: 0x006EF880/0x1403B4D60
ClientLfGuildMembershipListUpdated::ClientLfGuildMembershipListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006EF680
bool ClientLfGuildMembershipListUpdated::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ED450
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

// OFFSET: 0x006B7AF0
bool ClientLfGuildMembershipListUpdated::IsValid() const {
    for (uint32_t i = 0; i < this->m_applications.m_count; i++) {
        if (!this->m_applications.m_data[i].IsValid()) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x006AA9F0/0x14035A1B0
bool ClientLfGuildMembershipListUpdated::Handle() {
    if (!ClientLfGuildMembershipListUpdated::s_handler) {
        return false;
    }

    return ClientLfGuildMembershipListUpdated::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfGuildApplicantListUpdated::ClientLfGuildApplicantListUpdated(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientLfGuildApplicantListUpdated::~ClientLfGuildApplicantListUpdated() {
}

// OFFSET: 0x006BAEE0
bool ClientLfGuildApplicantListUpdated::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A78B0/0x140356460
bool ClientLfGuildApplicantListUpdated::Handle() {
    if (!ClientLfGuildApplicantListUpdated::s_handler) {
        return false;
    }

    return ClientLfGuildApplicantListUpdated::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfGuildApplicationsListChanged::ClientLfGuildApplicationsListChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientLfGuildApplicationsListChanged::~ClientLfGuildApplicationsListChanged() {
}

// OFFSET: 0x00696970/0x140341FE0
JamCliCUFProfile::JamCliCUFProfile() {
    this->m_fieldMask = 0;
    this->m_fieldMask2 = 0;
}

// OFFSET: 0x006BAEE0
bool ClientLfGuildApplicationsListChanged::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A9B20/0x140358F60
bool ClientLfGuildApplicationsListChanged::Handle() {
    if (!ClientLfGuildApplicationsListChanged::s_handler) {
        return false;
    }

    return ClientLfGuildApplicationsListChanged::s_handler(this);
}

// INLINED
JamResearchHistory::JamResearchHistory() {
}

// OFFSET: 0x006A12A0
JamResearchHistory::JamResearchHistory(const JamResearchHistory& source) {
    this->m_completionCount = source.m_completionCount;
    this->m_projectID = source.m_projectID;
    this->m_firstCompleted = source.m_firstCompleted;
}

// INLINED
JamResearchHistory::~JamResearchHistory() {
}

// OFFSET: 0x006E9370/0x1403AC260
ClientSetupResearchHistory::ClientSetupResearchHistory(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E93B0
bool ClientSetupResearchHistory::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E5450
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

// OFFSET: 0x006A72A0/0x140355CD0
bool ClientSetupResearchHistory::Handle() {
    if (!ClientSetupResearchHistory::s_handler) {
        return false;
    }

    return ClientSetupResearchHistory::s_handler(this);
}

// OFFSET: 0x006AD080/0x1403590A0
ClientResearchComplete::ClientResearchComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A15D0/0x1403596C0
ClientResearchComplete::~ClientResearchComplete() {
}

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

// OFFSET: 0x006B53F0
bool ClientResearchComplete::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AB7E0
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

// OFFSET: 0x006A51B0/0x140353130
bool ClientResearchComplete::Handle() {
    if (!ClientResearchComplete::s_handler) {
        return false;
    }

    return ClientResearchComplete::s_handler(this);
}

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

// OFFSET: 0x006A1290/0x14034E560
ClientAccountRestrictedWarning::~ClientAccountRestrictedWarning() {
}

// OFFSET: 0x006BFFD0
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

// OFFSET: 0x006B0B20/0x1403619E0
bool ClientAccountRestrictedWarning::Handle() {
    if (!ClientAccountRestrictedWarning::s_handler) {
        return false;
    }

    return ClientAccountRestrictedWarning::s_handler(this);
}

// OFFSET: 0x006B08A0/0x1403616F0
ClientAreaTriggerMovementUpdate::ClientAreaTriggerMovementUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0F50/0x14034E110
ClientAreaTriggerMovementUpdate::~ClientAreaTriggerMovementUpdate() {
}

// OFFSET: 0x006A2000/0x14034F470
void ClientAreaTriggerMovementUpdate::Unpack(CDataStore* msg, bool full) {
    { float value = 0; msg->Get(value); this->unk_0010.z = value; }
    { float value = 0; msg->Get(value); this->unk_001C.z = value; }
    { float value = 0; msg->Get(value); this->unk_0010.y = value; }
    { float value = 0; msg->Get(value); this->unk_0010.x = value; }
    { float value = 0; msg->Get(value); this->unk_001C.y = value; }
    { float value = 0; msg->Get(value); this->unk_001C.x = value; }
}

// OFFSET: 0x006C45C0
bool ClientAreaTriggerMovementUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B4110
bool ClientAreaTriggerMovementUpdate::Read(CDataStore* msg, bool full) {
    { float value = 0; msg->Get(value); this->unk_0010.z = value; }
    { float value = 0; msg->Get(value); this->unk_001C.z = value; }
    { float value = 0; msg->Get(value); this->unk_0010.y = value; }
    { float value = 0; msg->Get(value); this->unk_0010.x = value; }
    { float value = 0; msg->Get(value); this->unk_001C.y = value; }
    { float value = 0; msg->Get(value); this->unk_001C.x = value; }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B6520/0x140368ED0
bool ClientAreaTriggerMovementUpdate::Handle() {
    if (!ClientAreaTriggerMovementUpdate::s_handler) {
        return false;
    }

    return ClientAreaTriggerMovementUpdate::s_handler(this);
}

// OFFSET: 0x006A26C0/0x14034FBA0
ClientAverageItemLevelInform::ClientAverageItemLevelInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    float value = 0.0f;
    msg->Get(value);
    this->m_averageItemLevel = value;
}

// OFFSET: 0x006A2660/0x14034FB40
ClientAverageItemLevelInform::~ClientAverageItemLevelInform() {
}

// OFFSET: 0x006C6160
bool ClientAverageItemLevelInform::Get(CDataStore* msg, bool full) {
    float value = 0.0f;
    msg->Get(value);
    this->m_averageItemLevel = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AC820/0x14035C6B0
bool ClientAverageItemLevelInform::Handle() {
    if (!ClientAverageItemLevelInform::s_handler) {
        return false;
    }

    return ClientAverageItemLevelInform::s_handler(this);
}

// OFFSET: 0x006BC140/0x1403703C0
ClientBattlefieldMgrEjectPending::ClientBattlefieldMgrEjectPending(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006C54B0
bool ClientBattlefieldMgrEjectPending::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A99A0
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

// OFFSET: 0x006B7E40/0x14036ADE0
bool ClientBattlefieldMgrEjectPending::Handle() {
    if (!ClientBattlefieldMgrEjectPending::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrEjectPending::s_handler(this);
}

// OFFSET: 0x006BC610/0x140370A40
ClientBattlefieldMgrEjected::ClientBattlefieldMgrEjected(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006A78A0
bool ClientBattlefieldMgrEjected::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A4F50
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

// OFFSET: 0x006B9370/0x14036C8F0
bool ClientBattlefieldMgrEjected::Handle() {
    if (!ClientBattlefieldMgrEjected::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrEjected::s_handler(this);
}

// OFFSET: 0x006CB630/0x140383CF0
ClientBattlefieldMgrEntryInvite::ClientBattlefieldMgrEntryInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A29A0/0x14034FE40
ClientBattlefieldMgrEntryInvite::~ClientBattlefieldMgrEntryInvite() {
}

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

// OFFSET: 0x006C7930
bool ClientBattlefieldMgrEntryInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ABED0
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

// OFFSET: 0x006AA0C0/0x1403596D0
bool ClientBattlefieldMgrEntryInvite::Handle() {
    if (!ClientBattlefieldMgrEntryInvite::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrEntryInvite::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlefieldMgrExitRequest::ClientBattlefieldMgrExitRequest(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientBattlefieldMgrExitRequest::~ClientBattlefieldMgrExitRequest() {
}

// OFFSET: 0x006BAEE0
bool ClientBattlefieldMgrExitRequest::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A5E70/0x140353EF0
bool ClientBattlefieldMgrExitRequest::Handle() {
    if (!ClientBattlefieldMgrExitRequest::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrExitRequest::s_handler(this);
}

// OFFSET: 0x006B7DC0/0x14036AD60
ClientBattlefieldMgrStateChange::ClientBattlefieldMgrStateChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1790/0x14034EB10
ClientBattlefieldMgrStateChange::~ClientBattlefieldMgrStateChange() {
}

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

// OFFSET: 0x006CA810
bool ClientBattlefieldMgrStateChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C01C0
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

// OFFSET: 0x006B4510/0x140366460
bool ClientBattlefieldMgrStateChange::Handle() {
    if (!ClientBattlefieldMgrStateChange::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrStateChange::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientBattlefieldPortDenied::ClientBattlefieldPortDenied(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientBattlefieldPortDenied::~ClientBattlefieldPortDenied() {
}

// OFFSET: 0x006BAEE0
bool ClientBattlefieldPortDenied::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006BBDB0/0x14036FF30
bool ClientBattlefieldPortDenied::Handle() {
    if (!ClientBattlefieldPortDenied::s_handler) {
        return false;
    }

    return ClientBattlefieldPortDenied::s_handler(this);
}

// OFFSET: 0x006A2B60/0x1403500A0
ClientBattlefieldRatedInfo::ClientBattlefieldRatedInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1C90/0x14034F0F0
ClientBattlefieldRatedInfo::~ClientBattlefieldRatedInfo() {
}

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

// OFFSET: 0x006C0060
bool ClientBattlefieldRatedInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AF0E0
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

// OFFSET: 0x006B5CE0/0x140368250
bool ClientBattlefieldRatedInfo::Handle() {
    if (!ClientBattlefieldRatedInfo::s_handler) {
        return false;
    }

    return ClientBattlefieldRatedInfo::s_handler(this);
}

// OFFSET: 0x006BF740/0x140374890
ClientBattlefieldStatusFailed::ClientBattlefieldStatusFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A29F0/0x14034FED0
ClientBattlefieldStatusFailed::~ClientBattlefieldStatusFailed() {
}

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

// OFFSET: 0x006BC7C0
bool ClientBattlefieldStatusFailed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BA8F0
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

// OFFSET: 0x006AF1B0/0x14035FB40
bool ClientBattlefieldStatusFailed::Handle() {
    if (!ClientBattlefieldStatusFailed::s_handler) {
        return false;
    }

    return ClientBattlefieldStatusFailed::s_handler(this);
}

// OFFSET: 0x006B0610/0x1403613A0
ClientBattlefieldStatusQueued::ClientBattlefieldStatusQueued(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006CA470
bool ClientBattlefieldStatusQueued::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C31F0
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

// OFFSET: 0x006A9080/0x140358150
bool ClientBattlefieldStatusQueued::Handle() {
    if (!ClientBattlefieldStatusQueued::s_handler) {
        return false;
    }

    return ClientBattlefieldStatusQueued::s_handler(this);
}

// OFFSET: 0x006C9A10/0x140381460
ClientBattlefieldStatusWaitforgroups::ClientBattlefieldStatusWaitforgroups(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006B5D80/0x140368310
ClientBattlefieldStatusWaitforgroups::~ClientBattlefieldStatusWaitforgroups() {
}

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

// OFFSET: 0x006D8660
bool ClientBattlefieldStatusWaitforgroups::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CCAA0
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

// OFFSET: 0x006A75F0/0x1403560C0
bool ClientBattlefieldStatusWaitforgroups::Handle() {
    if (!ClientBattlefieldStatusWaitforgroups::s_handler) {
        return false;
    }

    return ClientBattlefieldStatusWaitforgroups::s_handler(this);
}

// OFFSET: 0x006B7830/0x14036A740
ClientBattlegroundPlayerJoined::ClientBattlegroundPlayerJoined(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2440/0x14034F940
ClientBattlegroundPlayerJoined::~ClientBattlegroundPlayerJoined() {
}

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

// OFFSET: 0x006C79A0
bool ClientBattlegroundPlayerJoined::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B9650
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

// OFFSET: 0x006B31F0/0x140364C10
bool ClientBattlegroundPlayerJoined::Handle() {
    if (!ClientBattlegroundPlayerJoined::s_handler) {
        return false;
    }

    return ClientBattlegroundPlayerJoined::s_handler(this);
}

// OFFSET: 0x006CB6B0/0x140383DC0
ClientBattlegroundPlayerLeft::ClientBattlegroundPlayerLeft(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0F30/0x14034E010
ClientBattlegroundPlayerLeft::~ClientBattlegroundPlayerLeft() {
}

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

// OFFSET: 0x006AFA70
bool ClientBattlegroundPlayerLeft::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A4030
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

// OFFSET: 0x006AB480/0x14035AF90
bool ClientBattlegroundPlayerLeft::Handle() {
    if (!ClientBattlegroundPlayerLeft::s_handler) {
        return false;
    }

    return ClientBattlegroundPlayerLeft::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientClearBossEmotes::ClientClearBossEmotes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientClearBossEmotes::~ClientClearBossEmotes() {
}

// OFFSET: 0x006BAEE0
bool ClientClearBossEmotes::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006BC5C0/0x1403709E0
bool ClientClearBossEmotes::Handle() {
    if (!ClientClearBossEmotes::s_handler) {
        return false;
    }

    return ClientClearBossEmotes::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientCustomLoadScreen::ClientCustomLoadScreen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientCustomLoadScreen::~ClientCustomLoadScreen() {
}

// OFFSET: 0x006CDE50
bool ClientCustomLoadScreen::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A7500/0x140355F80
bool ClientCustomLoadScreen::Handle() {
    if (!ClientCustomLoadScreen::s_handler) {
        return false;
    }

    return ClientCustomLoadScreen::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientDifferentInstanceFromParty::ClientDifferentInstanceFromParty(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientDifferentInstanceFromParty::~ClientDifferentInstanceFromParty() {
}

// OFFSET: 0x006BAEE0
bool ClientDifferentInstanceFromParty::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006AA4A0/0x140359B00
bool ClientDifferentInstanceFromParty::Handle() {
    if (!ClientDifferentInstanceFromParty::s_handler) {
        return false;
    }

    return ClientDifferentInstanceFromParty::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientDontAutoPushSpellsToActionBar::ClientDontAutoPushSpellsToActionBar(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientDontAutoPushSpellsToActionBar::~ClientDontAutoPushSpellsToActionBar() {
}

// OFFSET: 0x006BAEE0
bool ClientDontAutoPushSpellsToActionBar::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006C0500/0x140375820
bool ClientDontAutoPushSpellsToActionBar::Handle() {
    if (!ClientDontAutoPushSpellsToActionBar::s_handler) {
        return false;
    }

    return ClientDontAutoPushSpellsToActionBar::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientFailedPlayerCondition::ClientFailedPlayerCondition(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientFailedPlayerCondition::~ClientFailedPlayerCondition() {
}

// OFFSET: 0x006CDE50
bool ClientFailedPlayerCondition::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B0AE0/0x140361980
bool ClientFailedPlayerCondition::Handle() {
    if (!ClientFailedPlayerCondition::s_handler) {
        return false;
    }

    return ClientFailedPlayerCondition::s_handler(this);
}

// OFFSET: 0x006C0C80/0x1403760F0
ClientInspectHonorStats::ClientInspectHonorStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2FD0/0x1403504D0
ClientInspectHonorStats::~ClientInspectHonorStats() {
}

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

// OFFSET: 0x006D9300
bool ClientInspectHonorStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CB6F0
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

// OFFSET: 0x006B48E0/0x1403668C0
bool ClientInspectHonorStats::Handle() {
    if (!ClientInspectHonorStats::s_handler) {
        return false;
    }

    return ClientInspectHonorStats::s_handler(this);
}

// OFFSET: 0x006D0D30/0x14038B900
ClientInspectRatedBgStats::ClientInspectRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1100/0x14034E330
ClientInspectRatedBgStats::~ClientInspectRatedBgStats() {
}

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

// OFFSET: 0x006CA850
bool ClientInspectRatedBgStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BEEF0
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

// OFFSET: 0x006A4190/0x140351B80
bool ClientInspectRatedBgStats::Handle() {
    if (!ClientInspectRatedBgStats::s_handler) {
        return false;
    }

    return ClientInspectRatedBgStats::s_handler(this);
}

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

// OFFSET: 0x006A15D0/0x1403596C0
ClientLfgSlotInvalid::~ClientLfgSlotInvalid() {
}

// OFFSET: 0x006B53F0
bool ClientLfgSlotInvalid::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AB7E0
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

// OFFSET: 0x006B2040/0x140363550
bool ClientLfgSlotInvalid::Handle() {
    if (!ClientLfgSlotInvalid::s_handler) {
        return false;
    }

    return ClientLfgSlotInvalid::s_handler(this);
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientLfgUpdateStatusNone::ClientLfgUpdateStatusNone(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientLfgUpdateStatusNone::~ClientLfgUpdateStatusNone() {
}

// OFFSET: 0x006BAEE0
bool ClientLfgUpdateStatusNone::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006BBD20/0x14036FE70
bool ClientLfgUpdateStatusNone::Handle() {
    if (!ClientLfgUpdateStatusNone::s_handler) {
        return false;
    }

    return ClientLfgUpdateStatusNone::s_handler(this);
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

// OFFSET: 0x006B5C90/0x140368200
ClientMissileCancel::ClientMissileCancel(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006CA8C0
bool ClientMissileCancel::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B7FD0
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

// OFFSET: 0x006BA460/0x14036DE50
bool ClientMissileCancel::Handle() {
    if (!ClientMissileCancel::s_handler) {
        return false;
    }

    return ClientMissileCancel::s_handler(this);
}

// OFFSET: 0x006B3230/0x14035BB30
ClientMoneyNotify::ClientMoneyNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// OFFSET: 0x006A2650/0x14034ED30
ClientMoneyNotify::~ClientMoneyNotify() {
}

// OFFSET: 0x006C3F80
bool ClientMoneyNotify::Get(CDataStore* msg, bool full) {
    uint64_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AB7A0/0x14035B390
bool ClientMoneyNotify::Handle() {
    if (!ClientMoneyNotify::s_handler) {
        return false;
    }

    return ClientMoneyNotify::s_handler(this);
}

// OFFSET: 0x006A0E30/0x14034DED0
ClientNewWorld::~ClientNewWorld() {
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientNewWorldAbort::ClientNewWorldAbort(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientNewWorldAbort::~ClientNewWorldAbort() {
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientRatedBgRating::ClientRatedBgRating(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientRatedBgRating::~ClientRatedBgRating() {
}

// OFFSET: 0x006CDE50
bool ClientRatedBgRating::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A8EB0/0x140357EE0
bool ClientRatedBgRating::Handle() {
    if (!ClientRatedBgRating::s_handler) {
        return false;
    }

    return ClientRatedBgRating::s_handler(this);
}

// OFFSET: 0x006A1E30/0x14034F2A0
ClientRatedBgStats::ClientRatedBgStats(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2D50/0x1403502B0
ClientRatedBgStats::~ClientRatedBgStats() {
}

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

// OFFSET: 0x006C54A0
bool ClientRatedBgStats::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B7660
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

// OFFSET: 0x006AAD80/0x14035A640
bool ClientRatedBgStats::Handle() {
    if (!ClientRatedBgStats::s_handler) {
        return false;
    }

    return ClientRatedBgStats::s_handler(this);
}

// OFFSET: 0x006A24E0/0x14034FA00
ClientRequestPvpRewardsResponse::ClientRequestPvpRewardsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0D60/0x14034DD80
ClientRequestPvpRewardsResponse::~ClientRequestPvpRewardsResponse() {
}

// OFFSET: 0x006A1D10/0x14034F150
void ClientRequestPvpRewardsResponse::Unpack(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }
}

// OFFSET: 0x006D7150
bool ClientRequestPvpRewardsResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CAA70
bool ClientRequestPvpRewardsResponse::Read(CDataStore* msg, bool full) {
    { uint32_t value = 0; msg->Get(value); this->unk_0018 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0024 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0010 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0014 = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_001C = value; }
    { uint32_t value = 0; msg->Get(value); this->unk_0020 = value; }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C10A0/0x140376590
bool ClientRequestPvpRewardsResponse::Handle() {
    if (!ClientRequestPvpRewardsResponse::s_handler) {
        return false;
    }

    return ClientRequestPvpRewardsResponse::s_handler(this);
}

// OFFSET: 0x006C4910/0x14037A850
ClientSetDfFastLaunchResult::ClientSetDfFastLaunchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t value = 0;
    msg->Get(value);
    this->unk_0010 = value >> 7;
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientSetDfFastLaunchResult::~ClientSetDfFastLaunchResult() {
}

// OFFSET: 0x006C9980
bool ClientSetDfFastLaunchResult::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->unk_0010 = value >> 7;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BB7D0/0x14036F7A0
bool ClientSetDfFastLaunchResult::Handle() {
    if (!ClientSetDfFastLaunchResult::s_handler) {
        return false;
    }

    return ClientSetDfFastLaunchResult::s_handler(this);
}

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

// OFFSET: 0x006A1CD0/0x14034F120
ClientSuspendToken::~ClientSuspendToken() {
}

// OFFSET: 0x006D5A70/0x140392D80
ClientTransferPending::~ClientTransferPending() {
}

// OFFSET: 0x006A2E40/0x14036ACB0
ClientUpdateDungeonEncounterForLoot::ClientUpdateDungeonEncounterForLoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120/0x14033F780
ClientUpdateDungeonEncounterForLoot::~ClientUpdateDungeonEncounterForLoot() {
}

// OFFSET: 0x006BAEE0
bool ClientUpdateDungeonEncounterForLoot::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006A6D70/0x1403552C0
bool ClientUpdateDungeonEncounterForLoot::Handle() {
    if (!ClientUpdateDungeonEncounterForLoot::s_handler) {
        return false;
    }

    return ClientUpdateDungeonEncounterForLoot::s_handler(this);
}

// OFFSET: 0x006A2810
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

// OFFSET: 0x006A2670/0x14034FB50
ClientUpdateServerPlayerPosition::~ClientUpdateServerPlayerPosition() {
}

// OFFSET: 0x006CCA30
bool ClientUpdateServerPlayerPosition::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B7D40
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

// OFFSET: 0x006B4760/0x140366760
bool ClientUpdateServerPlayerPosition::Handle() {
    if (!ClientUpdateServerPlayerPosition::s_handler) {
        return false;
    }

    return ClientUpdateServerPlayerPosition::s_handler(this);
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

// OFFSET: 0x006C7920
bool ClientWardenData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C5EB0
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

// OFFSET: 0x006C0960/0x140375D10
bool ClientWardenData::Handle() {
    if (!ClientWardenData::s_handler) {
        return false;
    }

    return ClientWardenData::s_handler(this);
}

// OFFSET: 0x006B7E00/0x14036ADA0
ClientWargameCheckEntry::ClientWargameCheckEntry(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DA0/0x14034DDC0
ClientWargameCheckEntry::~ClientWargameCheckEntry() {
}

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

// OFFSET: 0x006C8100
bool ClientWargameCheckEntry::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B8990
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

// OFFSET: 0x006A0F70
bool ClientWargameCheckEntry::IsValid() const {
    const uint64_t& queueID = *reinterpret_cast<const uint64_t*>(&this->unk_0010);
    const uint64_t& guid = *reinterpret_cast<const uint64_t*>(&this->unk_0018);

    if ((queueID & 0xF000000000000000ull) == 0x1000000000000000ull && (queueID & 0x0FF0000000000000ull) == 0x0F10000000000000ull && static_cast<uint8_t>(static_cast<uint32_t>(queueID >> 16) & 0xF) == 3 && !(guid & 0xF000000000000000ull) && (guid & 0xF07FFFFFFFFFFFFFull)) {
        return true;
    }

    return false;
}

// OFFSET: 0x006B4FC0/0x140367180
bool ClientWargameCheckEntry::Handle() {
    if (!ClientWargameCheckEntry::s_handler) {
        return false;
    }

    return ClientWargameCheckEntry::s_handler(this);
}

// OFFSET: 0x006CF2A0/0x1403892A0
ClientWargameRequestSent::ClientWargameRequestSent(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A24D0/0x14034F9F0
ClientWargameRequestSent::~ClientWargameRequestSent() {
}

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

// OFFSET: 0x006C7F80
bool ClientWargameRequestSent::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BBFE0
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

// OFFSET: 0x006BAD90/0x14036EA20
bool ClientWargameRequestSent::Handle() {
    if (!ClientWargameRequestSent::s_handler) {
        return false;
    }

    return ClientWargameRequestSent::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientWeeklyLastReset::ClientWeeklyLastReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientWeeklyLastReset::~ClientWeeklyLastReset() {
}

// OFFSET: 0x006CDE50
bool ClientWeeklyLastReset::Get(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->unk_0010 = value;

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B41E0/0x140366020
bool ClientWeeklyLastReset::Handle() {
    if (!ClientWeeklyLastReset::s_handler) {
        return false;
    }

    return ClientWeeklyLastReset::s_handler(this);
}

// OFFSET: 0x006BF410/0x1403744A0
ClientBattlefieldMgrEntered::ClientBattlefieldMgrEntered(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A17C0/0x14034EB40
ClientBattlefieldMgrEntered::~ClientBattlefieldMgrEntered() {
}

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

// OFFSET: 0x006BC420
bool ClientBattlefieldMgrEntered::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B66C0
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

// OFFSET: 0x006B56E0/0x140367A90
bool ClientBattlefieldMgrEntered::Handle() {
    if (!ClientBattlefieldMgrEntered::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrEntered::s_handler(this);
}

// OFFSET: 0x006C3150/0x140378B60
ClientBattlefieldMgrQueueInvite::ClientBattlefieldMgrQueueInvite(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006AE830
bool ClientBattlefieldMgrQueueInvite::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A7A50
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

// OFFSET: 0x006B83A0/0x14036B510
bool ClientBattlefieldMgrQueueInvite::Handle() {
    if (!ClientBattlefieldMgrQueueInvite::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrQueueInvite::s_handler(this);
}

// OFFSET: 0x006BC280/0x1403705B0
ClientBattlefieldMgrQueueRequestResponse::ClientBattlefieldMgrQueueRequestResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2130/0x14034F580
ClientBattlefieldMgrQueueRequestResponse::~ClientBattlefieldMgrQueueRequestResponse() {
}

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

// OFFSET: 0x006B1290
bool ClientBattlefieldMgrQueueRequestResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A3D00
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

// OFFSET: 0x006A18F0/0x14034ECA0
bool ClientBattlefieldMgrQueueRequestResponse::IsValid() const {
    const uint64_t& guid = *reinterpret_cast<const uint64_t*>(this->unk_0018);
    const uint64_t& queueID = *reinterpret_cast<const uint64_t*>(this->unk_0028);

    if ((!guid || (!(guid & 0xF000000000000000ull) && (guid & 0xF07FFFFFFFFFFFFFull))) && static_cast<int8_t>(this->unk_0012) >= 0 && static_cast<int8_t>(this->unk_0012) < 44 && (queueID & 0xF000000000000000ull) == 0x1000000000000000ull && (queueID & 0x0FF0000000000000ull) == 0x0F10000000000000ull) {
        return true;
    }

    return false;
}

// OFFSET: 0x006A4390/0x140351E00
bool ClientBattlefieldMgrQueueRequestResponse::Handle() {
    if (!ClientBattlefieldMgrQueueRequestResponse::s_handler) {
        return false;
    }

    return ClientBattlefieldMgrQueueRequestResponse::s_handler(this);
}

// OFFSET: 0x006AAD40/0x14035A5F0
ClientBattlefieldStatus::ClientBattlefieldStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A29E0/0x14034FEB0
ClientBattlefieldStatus::~ClientBattlefieldStatus() {
}

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

// OFFSET: 0x006D59F0
bool ClientBattlefieldStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C6510
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

// OFFSET: 0x006A7A00/0x1403564C0
bool ClientBattlefieldStatus::Handle() {
    if (!ClientBattlefieldStatus::s_handler) {
        return false;
    }

    return ClientBattlefieldStatus::s_handler(this);
}

// OFFSET: 0x006BF890/0x140374A40
ClientBattlefieldStatusActive::ClientBattlefieldStatusActive(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006ADC20/0x14035E050
ClientBattlefieldStatusActive::~ClientBattlefieldStatusActive() {
}

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

// OFFSET: 0x006C4710
bool ClientBattlefieldStatusActive::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006BD4B0
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

// OFFSET: 0x006A9820/0x140358B70
bool ClientBattlefieldStatusActive::Handle() {
    if (!ClientBattlefieldStatusActive::s_handler) {
        return false;
    }

    return ClientBattlefieldStatusActive::s_handler(this);
}

// OFFSET: 0x006B6DD0/0x140369A00
ClientDisplayGameError::ClientDisplayGameError(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->unk_0010 = false;
    this->unk_0018 = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0F40/0x14034E100
ClientDisplayGameError::~ClientDisplayGameError() {
}

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

// OFFSET: 0x006C87E0
bool ClientDisplayGameError::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B2D60
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

// OFFSET: 0x006B6E90/0x140369AE0
bool ClientDisplayGameError::Handle() {
    if (!ClientDisplayGameError::s_handler) {
        return false;
    }

    return ClientDisplayGameError::s_handler(this);
}

// OFFSET: 0x006C6F50/0x14037D9A0
ClientPvpOptionsEnabled::ClientPvpOptionsEnabled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A15C0/0x14034E8C0
ClientPvpOptionsEnabled::~ClientPvpOptionsEnabled() {
}

// OFFSET: 0x006BAD20/0x14036E9A0
void ClientPvpOptionsEnabled::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();
}

// OFFSET: 0x006C45D0
bool ClientPvpOptionsEnabled::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C0850
bool ClientPvpOptionsEnabled::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->unk_0010 = bits.ReadBit();
    this->unk_0011 = bits.ReadBit();
    this->unk_0013 = bits.ReadBit();
    this->unk_0012 = bits.ReadBit();
    this->unk_0014 = bits.ReadBit();

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B6E50/0x140369A80
bool ClientPvpOptionsEnabled::Handle() {
    if (!ClientPvpOptionsEnabled::s_handler) {
        return false;
    }

    return ClientPvpOptionsEnabled::s_handler(this);
}

// OFFSET: 0x006A12D0/0x14034E5A0
ClientShowRatings::ClientShowRatings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0F00/0x14034DFE0
ClientShowRatings::~ClientShowRatings() {
}

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

// OFFSET: 0x006C48A0
bool ClientShowRatings::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B8F50
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

// OFFSET: 0x006BB740/0x14036F6E0
bool ClientShowRatings::Handle() {
    if (!ClientShowRatings::s_handler) {
        return false;
    }

    return ClientShowRatings::s_handler(this);
}

// OFFSET: 0x006C4720/0x14037A620
ClientBattlefieldStatusNeedconfirmation::ClientBattlefieldStatusNeedconfirmation(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006C0D90/0x140376230
ClientBattlefieldStatusNeedconfirmation::~ClientBattlefieldStatusNeedconfirmation() {
}

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

// OFFSET: 0x006D7770
bool ClientBattlefieldStatusNeedconfirmation::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CA490
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

// OFFSET: 0x006B12F0/0x140362400
bool ClientBattlefieldStatusNeedconfirmation::Handle() {
    if (!ClientBattlefieldStatusNeedconfirmation::s_handler) {
        return false;
    }

    return ClientBattlefieldStatusNeedconfirmation::s_handler(this);
}

// OFFSET: 0x006B2E00/0x1403646F0
ClientGameEventDebugLog::ClientGameEventDebugLog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1680/0x14034E9C0
ClientGameEventDebugLog::~ClientGameEventDebugLog() {
}

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

// OFFSET: 0x006D71F0
bool ClientGameEventDebugLog::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C4990
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

// OFFSET: 0x006AD5C0/0x14035D820
bool ClientGameEventDebugLog::Handle() {
    if (!ClientGameEventDebugLog::s_handler) {
        return false;
    }

    return ClientGameEventDebugLog::s_handler(this);
}

// OFFSET: 0x006C4820/0x14037A750
ClientGuildAchievementDeleted::ClientGuildAchievementDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2120/0x14034F570
ClientGuildAchievementDeleted::~ClientGuildAchievementDeleted() {
}

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

// OFFSET: 0x006C50D0
bool ClientGuildAchievementDeleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B3280
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

// OFFSET: 0x006BEDA0/0x140373CD0
bool ClientGuildAchievementDeleted::Handle() {
    if (!ClientGuildAchievementDeleted::s_handler) {
        return false;
    }

    return ClientGuildAchievementDeleted::s_handler(this);
}

// OFFSET: 0x006BE920/0x1403736E0
ClientGuildAchievementEarned::ClientGuildAchievementEarned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2F40/0x140350450
ClientGuildAchievementEarned::~ClientGuildAchievementEarned() {
}

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

// OFFSET: 0x006C4900
bool ClientGuildAchievementEarned::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A7380
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

// OFFSET: 0x006B5440/0x140367750
bool ClientGuildAchievementEarned::Handle() {
    if (!ClientGuildAchievementEarned::s_handler) {
        return false;
    }

    return ClientGuildAchievementEarned::s_handler(this);
}

// OFFSET: 0x006C3FD0/0x140379F90
ClientGuildCriteriaDeleted::ClientGuildCriteriaDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F00/0x14034F380
ClientGuildCriteriaDeleted::~ClientGuildCriteriaDeleted() {
}

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

// OFFSET: 0x006B5CD0
bool ClientGuildCriteriaDeleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AD600
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

// OFFSET: 0x006BFF80/0x1403751F0
bool ClientGuildCriteriaDeleted::Handle() {
    if (!ClientGuildCriteriaDeleted::s_handler) {
        return false;
    }

    return ClientGuildCriteriaDeleted::s_handler(this);
}

// OFFSET: 0x006B5BB0/0x1403680E0
ClientLfgQueueStatus::ClientLfgQueueStatus(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A7A40/0x14034E1A0
ClientLfgQueueStatus::~ClientLfgQueueStatus() {
}

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

// OFFSET: 0x006BE910
bool ClientLfgQueueStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B5DD0
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

// OFFSET: 0x006BF080/0x140374090
bool ClientLfgQueueStatus::Handle() {
    if (!ClientLfgQueueStatus::s_handler) {
        return false;
    }

    return ClientLfgQueueStatus::s_handler(this);
}

// OFFSET: 0x006F2300/0x1403B8A40
ClientServerInfoResponse::ClientServerInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F2540
bool ClientServerInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006ED930
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

// OFFSET: 0x006A32B0/0x140350A20
bool ClientServerInfoResponse::Handle() {
    if (!ClientServerInfoResponse::s_handler) {
        return false;
    }

    return ClientServerInfoResponse::s_handler(this);
}

// OFFSET: 0x006DEB20/0x14039E670
ClientServerPerf::ClientServerPerf(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E59B0
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

// OFFSET: 0x006A3C10/0x140351480
bool ClientServerPerf::Handle() {
    if (!ClientServerPerf::s_handler) {
        return false;
    }

    return ClientServerPerf::s_handler(this);
}

// OFFSET: 0x006DA0E0/0x1403986D0
ClientStreamingMoviesUpdate::ClientStreamingMoviesUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006DBA60/0x14039AA30
ClientStreamingMoviesUpdate::~ClientStreamingMoviesUpdate() {
}

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

// OFFSET: 0x006DB2C0
bool ClientStreamingMoviesUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

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

// OFFSET: 0x006B4890/0x140366860
bool ClientStreamingMoviesUpdate::Handle() {
    if (!ClientStreamingMoviesUpdate::s_handler) {
        return false;
    }

    return ClientStreamingMoviesUpdate::s_handler(this);
}

// OFFSET: 0x006DD5D0/0x14039CA70
ClientLoadCufProfiles::ClientLoadCufProfiles(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006D9610
bool ClientLoadCufProfiles::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D7790
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

// OFFSET: 0x006ACB90/0x14035CB40
bool ClientLoadCufProfiles::Handle() {
    if (!ClientLoadCufProfiles::s_handler) {
        return false;
    }

    return ClientLoadCufProfiles::s_handler(this);
}

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

// OFFSET: 0x006E9890
bool ClientAccountInfoResponse::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DDC80
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

// OFFSET: 0x006B1CF0/0x140363100
bool ClientAccountInfoResponse::Handle() {
    if (!ClientAccountInfoResponse::s_handler) {
        return false;
    }

    return ClientAccountInfoResponse::s_handler(this);
}

// OFFSET: 0x006F4270/0x1403BB620
ClientDumpRideTicketsResponse::ClientDumpRideTicketsResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006F42B0
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

// OFFSET: 0x006BA140/0x14036DA30
bool ClientDumpRideTicketsResponse::Handle() {
    if (!ClientDumpRideTicketsResponse::s_handler) {
        return false;
    }

    return ClientDumpRideTicketsResponse::s_handler(this);
}

// OFFSET: 0x006ECE10/0x1403B1230
ClientGuildCriteriaData::ClientGuildCriteriaData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006EAF60
bool ClientGuildCriteriaData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DF140
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

// OFFSET: 0x006A8D90/0x140357D60
bool ClientGuildCriteriaData::Handle() {
    if (!ClientGuildCriteriaData::s_handler) {
        return false;
    }

    return ClientGuildCriteriaData::s_handler(this);
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

// OFFSET: 0x006DAD60
bool ClientLfgUpdateStatus::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D86B0
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

// OFFSET: 0x006BF2F0/0x140374330
bool ClientLfgUpdateStatus::Handle() {
    if (!ClientLfgUpdateStatus::s_handler) {
        return false;
    }

    return ClientLfgUpdateStatus::s_handler(this);
}

// OFFSET: 0x006EE9A0/0x1403B3770
ClientRespondInspectAchievements::ClientRespondInspectAchievements(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006ECE00
bool ClientRespondInspectAchievements::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E6140
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

// OFFSET: 0x006A55C0/0x140353370
bool ClientRespondInspectAchievements::Handle() {
    if (!ClientRespondInspectAchievements::s_handler) {
        return false;
    }

    return ClientRespondInspectAchievements::s_handler(this);
}

// OFFSET: 0x006ECA00/0x1403B0E00
ClientAllAchievementData::ClientAllAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E59C0
bool ClientAllAchievementData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E3E70
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

// OFFSET: 0x006ADAE0/0x14035DEE0
bool ClientAllAchievementData::Handle() {
    if (!ClientAllAchievementData::s_handler) {
        return false;
    }

    return ClientAllAchievementData::s_handler(this);
}

// OFFSET: 0x006D95B0/0x1403979E0
ClientBattlefieldList::ClientBattlefieldList(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006D9F20
bool ClientBattlefieldList::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D9BB0
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

// OFFSET: 0x006A5EB0/0x140353F50
bool ClientBattlefieldList::Handle() {
    if (!ClientBattlefieldList::s_handler) {
        return false;
    }

    return ClientBattlefieldList::s_handler(this);
}

// OFFSET: 0x006ECDA0/0x1403B11B0
ClientBattlefieldPlayerPositions::ClientBattlefieldPlayerPositions(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E2510
bool ClientBattlefieldPlayerPositions::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E02D0
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

// OFFSET: 0x006A71D0/0x140355C10
bool ClientBattlefieldPlayerPositions::Handle() {
    if (!ClientBattlefieldPlayerPositions::s_handler) {
        return false;
    }

    return ClientBattlefieldPlayerPositions::s_handler(this);
}

// OFFSET: 0x006F0760/0x1403B6110
ClientCommentatorPartyInfo::ClientCommentatorPartyInfo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006EA9F0
bool ClientCommentatorPartyInfo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E7C10
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

// OFFSET: 0x006A41E0/0x140351BE0
bool ClientCommentatorPartyInfo::Handle() {
    if (!ClientCommentatorPartyInfo::s_handler) {
        return false;
    }

    return ClientCommentatorPartyInfo::s_handler(this);
}

// OFFSET: 0x006E5820/0x1403A6D30
ClientGuildAchievementData::ClientGuildAchievementData(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006D9310/0x140397700
ClientGuildAchievementData::~ClientGuildAchievementData() {
}

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

// OFFSET: 0x006EE0F0
bool ClientGuildAchievementData::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E2330
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

// OFFSET: 0x006BBF50/0x140370160
bool ClientGuildAchievementData::Handle() {
    if (!ClientGuildAchievementData::s_handler) {
        return false;
    }

    return ClientGuildAchievementData::s_handler(this);
}

// OFFSET: 0x006E3E30/0x1403A4D80
ClientGuildAchievementMembers::ClientGuildAchievementMembers(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

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

// OFFSET: 0x006E53F0
bool ClientGuildAchievementMembers::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006DF7D0
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

// OFFSET: 0x006C1990/0x140377040
bool ClientGuildAchievementMembers::Handle() {
    if (!ClientGuildAchievementMembers::s_handler) {
        return false;
    }

    return ClientGuildAchievementMembers::s_handler(this);
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

// OFFSET: 0x006AD440/0x14035D600
bool ClientLfgJoinResult::Handle() {
    if (!ClientLfgJoinResult::s_handler) {
        return false;
    }

    return ClientLfgJoinResult::s_handler(this);
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

// OFFSET: 0x006ECD70
bool ClientLfgProposalUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E3880
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

// OFFSET: 0x006B77F0/0x14036A6E0
bool ClientLfgProposalUpdate::Handle() {
    if (!ClientLfgProposalUpdate::s_handler) {
        return false;
    }

    return ClientLfgProposalUpdate::s_handler(this);
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

// OFFSET: 0x006ABE90/0x14035BBE0
bool ClientPhaseShiftChange::Handle() {
    if (!ClientPhaseShiftChange::s_handler) {
        return false;
    }

    return ClientPhaseShiftChange::s_handler(this);
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

// OFFSET: 0x006B53E0
bool ClientPhaseShiftChange::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006AFF50
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

// OFFSET: 0x006F41E0/0x1403BB570
ClientLfgSearchResult::ClientLfgSearchResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E0290
ClientLfgSearchResult::~ClientLfgSearchResult() {
}

// OFFSET: 0x006BF3C0/0x140374440
bool ClientLfgSearchResult::Handle() {
    if (!ClientLfgSearchResult::s_handler) {
        return false;
    }

    return ClientLfgSearchResult::s_handler(this);
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

// OFFSET: 0x006F2D90
bool ClientLfgSearchResult::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006EAF70
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
