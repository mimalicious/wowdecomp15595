#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: 9574b3a0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
JamGuildRewardItem::~JamGuildRewardItem() {
}

// OFFSET: 0x006A2770
JamGuildRewardItem& JamGuildRewardItem::operator=(const JamGuildRewardItem& other) {
    this->m_cost = other.m_cost;
    this->m_minGuildLevel = other.m_minGuildLevel;
    this->m_minGuildRep = other.m_minGuildRep;
    this->m_itemID = other.m_itemID;
    this->m_raceMask = other.m_raceMask;
    this->m_achievementRequired = other.m_achievementRequired;
    return *this;
}

// PROVEN: c4a66ca1 IDENTICAL/NONE
// OFFSET: 0x006C8460
JamGuildNewsEvent::JamGuildNewsEvent() {
}

// PROVEN: 78cb8955 IDENTICAL/NONE
// OFFSET: 0x006D3D30
JamGuildNewsEvent::~JamGuildNewsEvent() {
}

// OFFSET: 0x006D9F30
JamGuildNewsEvent& JamGuildNewsEvent::operator=(const JamGuildNewsEvent& other) {
    this->m_date = other.m_date;
    this->m_memberList = other.m_memberList;
    this->m_memberGUID = other.m_memberGUID;
    this->m_id = other.m_id;
    this->m_type = other.m_type;
    this->m_flags = other.m_flags;
    this->m_data[0] = other.m_data[0];
    this->m_data[1] = other.m_data[1];
    return *this;
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
bool (*ClientGuildNameChanged::s_handler)(ClientGuildNameChanged* msg);

// STATIC: 0x00E4D594
bool (*ClientGuildMoveStarting::s_handler)(ClientGuildMoveStarting* msg);

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
    uint8_t bits = 0;
    msg->Get(bits);

    uint8_t value = bits << 1;
    this->m_hasWaitInfo = (bits >> 7) != 0;

    if (this->m_hasWaitInfo) {
        this->m_freeCharacterMigration = (value >> 7) != 0;
        value <<= 1;
    }

    this->m_hasSuccessInfo = value >> 7;

    if (this->m_hasSuccessInfo) {
        uint32_t billingTimeRemaining = 0;
        msg->Get(billingTimeRemaining);
        this->m_billingTimeRemaining = billingTimeRemaining;

        uint8_t accountExpansion = 0;
        msg->Get(accountExpansion);
        this->m_accountExpansion = accountExpansion;

        uint32_t timeRemaining = 0;
        msg->Get(timeRemaining);
        this->m_timeRemaining = timeRemaining;

        uint8_t activeExpansion = 0;
        msg->Get(activeExpansion);
        this->m_activeExpansion = activeExpansion;

        uint32_t billingTimeRested = 0;
        msg->Get(billingTimeRested);
        this->m_billingTimeRested = billingTimeRested;

        uint8_t billingFlags = 0;
        msg->Get(billingFlags);
        this->m_billingFlags = billingFlags;
    }

    uint8_t result = 0;
    msg->Get(result);
    this->m_result = result;

    if (this->m_hasWaitInfo) {
        uint32_t waitCount = 0;
        msg->Get(waitCount);
        this->m_waitCount = waitCount;
    }
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
    uint32_t waitCount = 0;
    msg->Get(waitCount);
    this->m_waitCount = waitCount;

    uint8_t bits = 0;
    msg->Get(bits);
    this->m_freeCharacterMigration = bits >> 7;
}

// OFFSET: 0x006A1CD0/0x14034F120
ClientWaitQueueUpdate::~ClientWaitQueueUpdate() {
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

// OFFSET: 0x006BDFA0/0x140372B10
bool ClientWaitQueueFinish::Handle() {
    if (!ClientWaitQueueFinish::s_handler) {
        return false;
    }

    return ClientWaitQueueFinish::s_handler(this);
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
    uint8_t value = 0;
    msg->Get(value);
    this->m_success = (value >> 7) != 0;

    JamBitReader bits(msg);
    bits.m_value = value << 1;
    bits.m_bits = 1;

    uint32_t length = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = '\0';
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
        uint32_t recordID = 0;
        msg->Get(recordID);
        this->m_hotfixes[i].m_recordID = recordID;

        uint32_t timestamp = 0;
        msg->Get(timestamp);
        this->m_hotfixes[i].m_timestamp = timestamp;

        uint32_t tableHash = 0;
        msg->Get(tableHash);
        this->m_hotfixes[i].m_tableHash = tableHash;
    }
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
    uint32_t tableHash = 0;
    msg->Get(tableHash);
    this->m_tableHash = tableHash;

    uint32_t timestamp = 0;
    msg->Get(timestamp);
    this->m_timestamp = timestamp;

    uint32_t recordID = 0;
    msg->Get(recordID);
    this->m_recordID = recordID;
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
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[5] ^= byteValue;
    }

    if (this->m_objectGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[1] ^= byteValue;
    }

    if (this->m_objectGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[0] ^= byteValue;
    }

    if (this->m_objectGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[3] ^= byteValue;
    }

    if (this->m_objectGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[4] ^= byteValue;
    }

    if (this->m_objectGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[6] ^= byteValue;
    }

    if (this->m_objectGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[2] ^= byteValue;
    }

    if (this->m_objectGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_objectGUID[7] ^= byteValue;
    }

    uint32_t animKitID = 0;
    msg->Get(animKitID);
    this->m_animKitID = animKitID;
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

// OFFSET: 0x006A9870/0x140358BD0
bool ClientNotification::Handle() {
    if (!ClientNotification::s_handler) {
        return false;
    }

    return ClientNotification::s_handler(this);
}

// OFFSET: 0x006BE000/0x140372BC0
bool ClientSuspendToken::Handle() {
    if (!ClientSuspendToken::s_handler) {
        return false;
    }

    return ClientSuspendToken::s_handler(this);
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
    uint8_t bits = 0;
    msg->Get(bits);

    uint8_t value = bits << 1;
    this->m_hasTransferSpellID = (bits >> 7) != 0;
    this->m_hasShip = value >> 7;

    if (this->m_hasShip) {
        uint32_t shipOriginMapID = 0;
        msg->Get(shipOriginMapID);
        this->m_shipOriginMapID = shipOriginMapID;

        uint32_t shipID = 0;
        msg->Get(shipID);
        this->m_shipID = shipID;
    }

    if (this->m_hasTransferSpellID) {
        uint32_t transferSpellID = 0;
        msg->Get(transferSpellID);
        this->m_transferSpellID = transferSpellID;
    }

    uint32_t mapID = 0;
    msg->Get(mapID);
    this->m_mapID = mapID;
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
    float x = 0.0f;
    msg->Get(x);
    this->m_position.x = x;

    float facing = 0.0f;
    msg->Get(facing);
    this->m_facing = facing;

    float y = 0.0f;
    msg->Get(y);
    this->m_position.y = y;

    uint32_t mapID = 0;
    msg->Get(mapID);
    this->m_mapID = mapID;

    float z = 0.0f;
    msg->Get(z);
    this->m_position.z = z;
}

// PROVEN: 48018d85 IDENTICAL/NONE
// OFFSET: [0x0066BC00/0x140312430]
JamWeeklySpellUse::JamWeeklySpellUse() {
}

// OFFSET: 0x006A1DD0
JamWeeklySpellUse& JamWeeklySpellUse::operator=(const JamWeeklySpellUse& source) {
    this->m_category = source.m_category;
    this->m_uses = source.m_uses;
    return *this;
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
    uint32_t timeOffset = 0;
    msg->Get(timeOffset);
    this->m_timeOffset = timeOffset;

    uint32_t kitRecID = 0;
    msg->Get(kitRecID);
    this->m_kitRecID = kitRecID;

    uint32_t kitType = 0;
    msg->Get(kitType);
    this->m_kitType = kitType;

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
    uint32_t timeOffset = 0;
    msg->Get(timeOffset);
    this->m_timeOffset = timeOffset;

    uint32_t kitRecID = 0;
    msg->Get(kitRecID);
    this->m_kitRecID = kitRecID;

    uint32_t kitType = 0;
    msg->Get(kitType);
    this->m_kitType = kitType;

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
        uint32_t category = 0;
        msg->Get(category);
        this->m_usage[i].m_category = category;

        uint8_t uses = 0;
        msg->Get(uses);
        this->m_usage[i].m_uses = uses;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006DEA10/0x14039E530
void ClientWeeklySpellUsage::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_usage.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_usage.Count(); i++) {
        uint32_t category = 0;
        msg->Get(category);
        this->m_usage[i].m_category = category;

        uint8_t uses = 0;
        msg->Get(uses);
        this->m_usage[i].m_uses = uses;
    }
}

// OFFSET: 0x006AEF10/0x14034ED70
ClientWeeklySpellUsageUpdate::ClientWeeklySpellUsageUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint8_t uses = 0;
    msg->Get(uses);
    this->m_usage.m_uses = uses;

    uint32_t category = 0;
    msg->Get(category);
    this->m_usage.m_category = category;
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
        JamClientCurrencyData& data = this->m_data[i];
        data.m_hasWeeklyQuantity = bits.ReadBit() != 0;
        data.m_flags = static_cast<uint8_t>(bits.ReadBits<4>());
        data.m_hasMaxWeeklyQuantity = bits.ReadBit() != 0;
        data.m_hasTrackedQuantity = bits.ReadBit() != 0;
    }

    for (uint32_t i = 0; i < this->m_data.Count(); i++) {
        JamClientCurrencyData& data = this->m_data[i];
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

// OFFSET: 0x006A3C80/0x140351500
bool ClientWeeklyResetCurrency::Handle() {
    if (!ClientWeeklyResetCurrency::s_handler) {
        return false;
    }

    return ClientWeeklyResetCurrency::s_handler(this);
}

// OFFSET: 0x006BD0A0/0x1403717A0
ClientUpdateCurrencyWeekLimit::ClientUpdateCurrencyWeekLimit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A10B0/0x14034F400
ClientUpdateCurrencyWeekLimit::~ClientUpdateCurrencyWeekLimit() {
}

// INLINED
void ClientUpdateCurrencyWeekLimit::Unpack(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_weekCap = value;

    value = 0;
    msg->Get(value);
    this->m_type = value;
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

        intValue = 0;
        msg->Get(intValue);
        item.m_muid = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_durability = intValue;

        if (item.m_extendedCostID) {
            intValue = 0;
            msg->Get(intValue);
            item.m_extendedCostID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        item.m_item = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_type = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_price = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_displayID = intValue;

        if (item.m_playerConditionID) {
            intValue = 0;
            msg->Get(intValue);
            item.m_playerConditionID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        item.m_quantity = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_stackCount = intValue;
    }

    if (this->m_vendor[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[5] ^= byteValue;
    }

    if (this->m_vendor[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[4] ^= byteValue;
    }

    if (this->m_vendor[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[1] ^= byteValue;
    }

    if (this->m_vendor[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[0] ^= byteValue;
    }

    if (this->m_vendor[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[6] ^= byteValue;
    }

    byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;

    if (this->m_vendor[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[2] ^= byteValue;
    }

    if (this->m_vendor[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[3] ^= byteValue;
    }

    if (this->m_vendor[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_vendor[7] ^= byteValue;
    }
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
    uint32_t value = 0;
    msg->Get(value);
    this->m_newSlot = value;

    value = 0;
    msg->Get(value);
    this->m_prevPetNumber = value;

    value = 0;
    msg->Get(value);
    this->m_petNumber = value;

    value = 0;
    msg->Get(value);
    this->m_prevSlot = value;
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
    uint32_t value = 0;
    msg->Get(value);
    this->m_level = value;

    value = 0;
    msg->Get(value);
    this->m_slot = value;

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_flags = byteValue;

    value = 0;
    msg->Get(value);
    this->m_creatureID = value;

    value = 0;
    msg->Get(value);
    this->m_petNumber = value;

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;
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
    uint32_t value = 0;
    msg->Get(value);
    this->m_quantity = value;
    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_lootListID = byteValue;
    value = 0;
    msg->Get(value);
    this->m_currencyID = value;
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
    this->m_inviterGUID[4] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_inviterRealmName) = bits.ReadBits<9>();
    *reinterpret_cast<uint32_t*>(this->m_inviterName) = bits.ReadBits<7>();
    this->m_lfgSlots.SetCount(bits.ReadBits<24>());
    this->m_isXRealm = bits.ReadBit() != 0;
    this->m_inviterGUID[1] = bits.ReadBit();
    this->m_inviterGUID[7] = bits.ReadBit();

    uint8_t byteValue;
    uint32_t intValue;

    if (this->m_inviterGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[1] ^= byteValue;
    }

    if (this->m_inviterGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[4] ^= byteValue;
    }

    intValue = 0;
    msg->Get(intValue);
    this->m_inviterCfgRealmID = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_proposedRoles = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_lfgCompletedMask = intValue;

    if (this->m_inviterGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[6] ^= byteValue;
    }

    if (this->m_inviterGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[0] ^= byteValue;
    }

    if (this->m_inviterGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[2] ^= byteValue;
    }

    if (this->m_inviterGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[3] ^= byteValue;
    }

    for (uint32_t i = 0; i < this->m_lfgSlots.Count(); i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_lfgSlots[i] = intValue;
    }

    if (this->m_inviterGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[5] ^= byteValue;
    }

    uint32_t length = *reinterpret_cast<uint32_t*>(this->m_inviterRealmName);
    msg->GetData(this->m_inviterRealmName, length);
    this->m_inviterRealmName[length] = 0;

    if (this->m_inviterGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_inviterGUID[7] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    intValue = 0;
    msg->Get(intValue);
    this->m_serial = intValue;
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
        intValue = 0;
        msg->Get(intValue);
        this->m_itemID = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_bagResult = intValue;
    }

    if (this->m_status == 12) {
        if (this->m_partner[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[4] ^= byteValue;
        }

        if (this->m_partner[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[1] ^= byteValue;
        }

        if (this->m_partner[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[2] ^= byteValue;
        }

        if (this->m_partner[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[3] ^= byteValue;
        }

        if (this->m_partner[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[0] ^= byteValue;
        }

        if (this->m_partner[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[7] ^= byteValue;
        }

        if (this->m_partner[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[6] ^= byteValue;
        }

        if (this->m_partner[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            this->m_partner[5] ^= byteValue;
        }
    }

    if (this->m_status == 24 || this->m_status == 19) {
        intValue = 0;
        msg->Get(intValue);
        this->m_currencyQuantity = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_currencyType = intValue;
    }

    if (this->m_status == 2 || this->m_status == 26) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_tradeSlot = byteValue;
    }

    if (this->m_status == 0) {
        intValue = 0;
        msg->Get(intValue);
        this->m_id = intValue;
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
ClientTradeItem::ClientTradeItem() {
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

    intValue = 0;
    msg->Get(intValue);
    this->m_id = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_currencyType = intValue;

    uint64_t longValue = 0;
    msg->Get(longValue);
    this->m_gold = longValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_proposedEnchantment = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_clientStateIndex = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_currencyQuantity = intValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_whichPlayer = byteValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_stateIndex = intValue;

    JamBitReader bits(msg);
    this->m_items.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        ClientTradeItem& item = this->m_items[i];
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
        ClientTradeItem& item = this->m_items[i];

        if (item.m_hasUnwrapped) {
            if (item.m_unwrapped.m_creator[1]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[1] ^= byteValue;
            }

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_enchantID = intValue;

            for (uint32_t j = 0; j < 3; j++) {
                intValue = 0;
                msg->Get(intValue);
                item.m_unwrapped.m_socketEnchant[j] = intValue;
            }

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_maxDurability = intValue;

            if (item.m_unwrapped.m_creator[6]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[6] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[2]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[2] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[7]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[7] ^= byteValue;
            }

            if (item.m_unwrapped.m_creator[4]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[4] ^= byteValue;
            }

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_reforgeID = intValue;

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_durability = intValue;

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_randomPropertiesID = intValue;

            if (item.m_unwrapped.m_creator[3]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[3] ^= byteValue;
            }

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_onUseEnchantmentID = intValue;

            if (item.m_unwrapped.m_creator[0]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[0] ^= byteValue;
            }

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_charges = intValue;

            intValue = 0;
            msg->Get(intValue);
            item.m_unwrapped.m_randomPropertiesSeed = intValue;

            if (item.m_unwrapped.m_creator[5]) {
                byteValue = 0;
                msg->Get(byteValue);
                item.m_unwrapped.m_creator[5] ^= byteValue;
            }
        }

        if (item.m_giftCreator[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[6] ^= byteValue;
        }

        if (item.m_giftCreator[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[1] ^= byteValue;
        }

        if (item.m_giftCreator[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[7] ^= byteValue;
        }

        if (item.m_giftCreator[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        item.m_itemID = intValue;

        if (item.m_giftCreator[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[0] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        item.m_stackCount = intValue;

        if (item.m_giftCreator[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[5] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        item.m_slot = byteValue;

        if (item.m_giftCreator[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[2] ^= byteValue;
        }

        if (item.m_giftCreator[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            item.m_giftCreator[3] ^= byteValue;
        }
    }
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
        byteValue = 0;
        msg->Get(byteValue);
        member.m_gender = byteValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_guildRepToCap = intValue;

        if (member.m_guid[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[1] ^= byteValue;
        }

        longValue = 0;
        msg->Get(longValue);
        member.m_totalXP = longValue;

        if (member.m_guid[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[2] ^= byteValue;
        }

        for (uint32_t j = 0; j < 2; j++) {
            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_step = intValue;

            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_dbID = intValue;

            intValue = 0;
            msg->Get(intValue);
            member.m_profession[j].m_rank = intValue;
        }

        if (member.m_guid[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[0] ^= byteValue;
        }

        if (member.m_guid[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[6] ^= byteValue;
        }

        if (member.m_guid[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[7] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        member.m_rankID = intValue;

        length = *reinterpret_cast<uint32_t*>(member.m_note);
        msg->GetData(member.m_note, length);
        member.m_note[length] = 0;

        length = *reinterpret_cast<uint32_t*>(member.m_officerNote);
        msg->GetData(member.m_officerNote, length);
        member.m_officerNote[length] = 0;

        if (member.m_guid[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[4] ^= byteValue;
        }

        if (member.m_guid[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[5] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        member.m_guildReputation = intValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_personalAchievementPoints = intValue;

        floatValue = 0.0f;
        msg->Get(floatValue);
        member.m_lastSave = floatValue;

        longValue = 0;
        msg->Get(longValue);
        member.m_weeklyXP = longValue;

        byteValue = 0;
        msg->Get(byteValue);
        member.m_level = byteValue;

        byteValue = 0;
        msg->Get(byteValue);
        member.m_classID = byteValue;

        if (member.m_guid[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            member.m_guid[3] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        member.m_status = byteValue;

        intValue = 0;
        msg->Get(intValue);
        member.m_areaID = intValue;

        length = *reinterpret_cast<uint32_t*>(member.m_name);
        msg->GetData(member.m_name, length);
        member.m_name[length] = 0;

        intValue = 0;
        msg->Get(intValue);
        member.m_bgRating = intValue;
    }
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

// OFFSET: 0x006C0180/0x140375430
bool ClientGuildRewardList::Handle() {
    if (!ClientGuildRewardList::s_handler) {
        return false;
    }

    return ClientGuildRewardList::s_handler(this);
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

// OFFSET: 0x006AE160/0x14035E710
bool ClientGuildNews::Handle() {
    if (!ClientGuildNews::s_handler) {
        return false;
    }

    return ClientGuildNews::s_handler(this);
}

// OFFSET: 0x006A1EB0/0x14036EC10
ClientGuildNewsDeleted::ClientGuildNewsDeleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildNewsDeleted::~ClientGuildNewsDeleted() {
}

// INLINED
void ClientGuildNewsDeleted::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_newsID = intValue;
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
    this->Unpack(msg, true);
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientGuildReputationWeeklyCap::~ClientGuildReputationWeeklyCap() {
}

// INLINED
void ClientGuildReputationWeeklyCap::Unpack(CDataStore* msg, bool full) {
    uint32_t intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_repToCap = intValue;
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
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2650/0x14034ED30
ClientGuildXPEarned::~ClientGuildXPEarned() {
}

// INLINED
void ClientGuildXPEarned::Unpack(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_xp = longValue;
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
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2650/0x14034ED30
ClientGuildBankMoneyWithdrawn::~ClientGuildBankMoneyWithdrawn() {
}

// INLINED
void ClientGuildBankMoneyWithdrawn::Unpack(CDataStore* msg, bool full) {
    uint64_t longValue;

    longValue = 0;
    msg->Get(longValue);
    this->m_remainingWithdrawMoney = longValue;
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
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildChangeNameResult::~ClientGuildChangeNameResult() {
}

// INLINED
void ClientGuildChangeNameResult::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_success = byteValue;
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
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1F40/0x14034F3E0
ClientGuildFlaggedForRename::~ClientGuildFlaggedForRename() {
}

// INLINED
void ClientGuildFlaggedForRename::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;

    byteValue = 0;
    msg->Get(byteValue);
    this->m_flagSet = byteValue;
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
    this->m_memberGUID[5] = bits.ReadBit();
    this->m_memberGUID[0] = bits.ReadBit();
    this->m_memberGUID[6] = bits.ReadBit();
    this->m_memberGUID[4] = bits.ReadBit();
    this->m_isPublic = bits.ReadBit() != 0;
    *reinterpret_cast<uint32_t*>(this->m_note) = bits.ReadBits<8>();
    this->m_memberGUID[1] = bits.ReadBit();

    if (this->m_memberGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[3] ^= byteValue;
    }

    if (this->m_memberGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[0] ^= byteValue;
    }

    if (this->m_memberGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[2] ^= byteValue;
    }

    if (this->m_memberGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[5] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_note);
    msg->GetData(this->m_note, length);
    this->m_note[length] = 0;

    if (this->m_memberGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[6] ^= byteValue;
    }

    if (this->m_memberGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[7] ^= byteValue;
    }

    if (this->m_memberGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[1] ^= byteValue;
    }

    if (this->m_memberGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_memberGUID[4] ^= byteValue;
    }
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

    intValue = 0;
    msg->Get(intValue);
    this->m_guildLevel = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_achievementPoints = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_borderColor = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_emblemStyle = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_background = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_emblemColor = intValue;

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
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
    }

    if (this->m_oldGuildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[3] ^= byteValue;
    }

    if (this->m_guildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[2] ^= byteValue;
    }

    if (this->m_oldGuildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[1] ^= byteValue;
    }

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_oldGuildName);
    msg->GetData(this->m_oldGuildName, length);
    this->m_oldGuildName[length] = 0;

    if (this->m_guildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[7] ^= byteValue;
    }

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_inviterName);
    msg->GetData(this->m_inviterName, length);
    this->m_inviterName[length] = 0;

    if (this->m_oldGuildGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[7] ^= byteValue;
    }

    if (this->m_oldGuildGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[6] ^= byteValue;
    }

    if (this->m_oldGuildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[5] ^= byteValue;
    }

    if (this->m_oldGuildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[4] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
    }

    if (this->m_oldGuildGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_oldGuildGUID[4] ^= byteValue;
    }
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

    floatValue = 0.0f;
    msg->Get(floatValue);
    this->m_guildXPEarnedMult = floatValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_numGuildGroupMembers = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_numGuildGroupRequired = intValue;
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

    intValue = 0;
    msg->Get(intValue);
    this->m_challengeType = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_goldAwarded = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_currentCount = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_xpAwarded = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_maxCount = intValue;
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
    this->m_guildGUID[0] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_guildName) = bits.ReadBits<9>();

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

    if (this->m_guildGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[0] ^= byteValue;
    }

    if (this->m_guildGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[5] ^= byteValue;
    }

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
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

    if (this->m_guildGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[2] ^= byteValue;
    }

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;
}

// OFFSET: 0x006ADB90/0x14035DFB0
bool ClientGuildMoveComplete::Handle() {
    if (!ClientGuildMoveComplete::s_handler) {
        return false;
    }

    return ClientGuildMoveComplete::s_handler(this);
}

// OFFSET: 0x006B6E10/0x140369A40
ClientGuildNameChanged::ClientGuildNameChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2E20/0x140350380
ClientGuildNameChanged::~ClientGuildNameChanged() {
}

// PROVEN: 78b17c60 IDENTICAL/NONE
// OFFSET: 0x006B02E0/0x140360FF0
void ClientGuildNameChanged::Unpack(CDataStore* msg, bool full) {
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

// OFFSET: 0x006B6D80/0x1403699A0
bool ClientGuildNameChanged::Handle() {
    if (!ClientGuildNameChanged::s_handler) {
        return false;
    }

    return ClientGuildNameChanged::s_handler(this);
}

// OFFSET: 0x006D1EB0/0x14038D470
ClientGuildMoveStarting::ClientGuildMoveStarting(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0EC0/0x14034DF80
ClientGuildMoveStarting::~ClientGuildMoveStarting() {
}

// OFFSET: 0x006BF920
void ClientGuildMoveStarting::Unpack(CDataStore* msg, bool full) {
    uint8_t byteValue;
    uint32_t length;

    JamBitReader bits(msg);
    this->m_guildGUID[5] = bits.ReadBit();
    this->m_guildGUID[4] = bits.ReadBit();
    this->m_guildGUID[0] = bits.ReadBit();
    this->m_guildGUID[6] = bits.ReadBit();
    this->m_guildGUID[3] = bits.ReadBit();
    this->m_guildGUID[1] = bits.ReadBit();
    this->m_guildGUID[7] = bits.ReadBit();
    this->m_guildGUID[2] = bits.ReadBit();
    *reinterpret_cast<uint32_t*>(this->m_guildName) = bits.ReadBits<8>();

    if (this->m_guildGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[3] ^= byteValue;
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

    if (this->m_guildGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_guildGUID[1] ^= byteValue;
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

    length = *reinterpret_cast<uint32_t*>(this->m_guildName);
    msg->GetData(this->m_guildName, length);
    this->m_guildName[length] = 0;

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
}

// OFFSET: 0x006AC860/0x14035C710
bool ClientGuildMoveStarting::Handle() {
    if (!ClientGuildMoveStarting::s_handler) {
        return false;
    }

    return ClientGuildMoveStarting::s_handler(this);
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

    intValue = 0;
    msg->Get(intValue);
    this->m_rankID = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_numTabs = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_flags = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_withdrawGoldLimit = intValue;

    JamBitReader bits(msg);
    this->m_tabPermissions.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_tabPermissions.Count(); i++) {
        intValue = 0;
        msg->Get(intValue);
        this->m_tabPermissions[i].m_flags = intValue;

        intValue = 0;
        msg->Get(intValue);
        this->m_tabPermissions[i].m_withdrawItemLimit = intValue;
    }
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
        JamGuildEventLogEntry& entry = this->m_entry[i];
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
        JamGuildEventLogEntry& entry = this->m_entry[i];

        if (entry.m_otherGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[2] ^= byteValue;
        }

        if (entry.m_otherGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[5] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        entry.m_rankID = byteValue;

        if (entry.m_otherGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        intValue = 0;
        msg->Get(intValue);
        entry.m_timeOffset = intValue;

        if (entry.m_playerGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_otherGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[0] ^= byteValue;
        }

        if (entry.m_otherGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[6] ^= byteValue;
        }

        if (entry.m_otherGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        entry.m_type = byteValue;

        if (entry.m_otherGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[6]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }
    }
}

// OFFSET: 0x006BFD00/0x140374F20
bool ClientGuildEventLogQueryResults::Handle() {
    if (!ClientGuildEventLogQueryResults::s_handler) {
        return false;
    }

    return ClientGuildEventLogQueryResults::s_handler(this);
}

// PROVEN: dcb0747f IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientGuildMemberDailyReset::ClientGuildMemberDailyReset(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 82de6f74 IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientGuildMemberDailyReset::~ClientGuildMemberDailyReset() {
}

// INLINED
void ClientGuildMemberDailyReset::Unpack(CDataStore* msg, bool full) {
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

        intValue = 0;
        msg->Get(intValue);
        this->m_tabInfo[i].m_tabIndex = intValue;

        length = *reinterpret_cast<uint32_t*>(this->m_tabInfo[i].m_name);
        msg->GetData(this->m_tabInfo[i].m_name, length);
        this->m_tabInfo[i].m_name[length] = 0;
    }

    longValue = 0;
    msg->Get(longValue);
    this->m_money = longValue;

    for (uint32_t i = 0; i < this->m_itemInfo.Count(); i++) {
        JamGuildBankItemInfo& item = this->m_itemInfo[i];

        for (uint32_t j = 0; j < item.m_socketEnchant.Count(); j++) {
            intValue = 0;
            msg->Get(intValue);
            item.m_socketEnchant[j].m_socketIndex = intValue;

            intValue = 0;
            msg->Get(intValue);
            item.m_socketEnchant[j].m_socketEnchantID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        item.m_enchantmentID = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_reforgeID = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_onUseEnchantmentID = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_count = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_slot = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_flags = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_itemID = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_randomPropertiesID = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_charges = intValue;

        intValue = 0;
        msg->Get(intValue);
        item.m_randomPropertiesSeed = intValue;
    }

    intValue = 0;
    msg->Get(intValue);
    this->m_tab = intValue;

    intValue = 0;
    msg->Get(intValue);
    this->m_withdrawalsRemaining = intValue;
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
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[6] ^= byteValue;
        }

        if (entry.m_playerGUID[1]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[1] ^= byteValue;
        }

        if (entry.m_playerGUID[5]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[5] ^= byteValue;
        }

        if (entry.m_hasCount) {
            intValue = 0;
            msg->Get(intValue);
            entry.m_count = intValue;
        }

        byteValue = 0;
        msg->Get(byteValue);
        entry.m_entryType = byteValue;

        if (entry.m_playerGUID[2]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[2] ^= byteValue;
        }

        if (entry.m_playerGUID[4]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[4] ^= byteValue;
        }

        if (entry.m_playerGUID[0]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[0] ^= byteValue;
        }

        if (entry.m_playerGUID[7]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[7] ^= byteValue;
        }

        if (entry.m_playerGUID[3]) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_playerGUID[3] ^= byteValue;
        }

        if (entry.m_hasItemID) {
            intValue = 0;
            msg->Get(intValue);
            entry.m_itemID = intValue;
        }

        intValue = 0;
        msg->Get(intValue);
        entry.m_timeOffset = intValue;

        if (entry.m_hasMoney) {
            longValue = 0;
            msg->Get(longValue);
            entry.m_money = longValue;
        }

        if (entry.m_hasOtherTab) {
            byteValue = 0;
            msg->Get(byteValue);
            entry.m_otherTab = byteValue;
        }
    }

    intValue = 0;
    msg->Get(intValue);
    this->m_tab = intValue;

    if (this->m_hasWeeklyBonusMoney) {
        longValue = 0;
        msg->Get(longValue);
        this->m_weeklyBonusMoney = longValue;
    }
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

    intValue = 0;
    msg->Get(intValue);
    this->m_tab = intValue;

    length = *reinterpret_cast<uint32_t*>(this->m_text);
    msg->GetData(this->m_text, length);
    this->m_text[length] = 0;
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
        value = 0;
        msg->Get(value);
        this->m_ineligibleForLootMask = value;
    }

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_isTournamentRealm = byteValue;

    if (this->m_hasRestrictedMoney) {
        value = 0;
        msg->Get(value);
        this->m_restrictedMoney = value;
    }

    if (this->m_hasRestrictedLevel) {
        value = 0;
        msg->Get(value);
        this->m_restrictedLevel = value;
    }

    value = 0;
    msg->Get(value);
    this->m_weeklyReset = value;

    value = 0;
    msg->Get(value);
    this->m_difficultyID = value;
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
        int32_t value = 0;
        msg->Get(value);
        this->m_actionButtons[i] = value;
    }

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_reason = byteValue;
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
    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_complaintStatus = byteValue;

    uint32_t value = 0;
    msg->Get(value);
    this->m_scrollOfResurrectionMaxRequestsPerDay = value;

    value = 0;
    msg->Get(value);
    this->m_scrollOfResurrectionRequestsRemaining = value;

    value = 0;
    msg->Get(value);
    this->m_cfgRealmID = value;

    value = 0;
    msg->Get(value);
    this->m_cfgRealmRecID = value;

    JamBitReader bits(msg);
    this->m_itemRestorationButtonEnabled = bits.ReadBit() != 0;
    this->m_travelPassEnabled = bits.ReadBit() != 0;
    this->m_scrollOfResurrectionEnabled = bits.ReadBit() != 0;
    this->m_europaTicketSystemEnabled = bits.ReadBit() != 0;
    this->m_sessionAlertEnabled = bits.ReadBit() != 0;
    this->m_voiceEnabled = bits.ReadBit() != 0;

    if (this->m_europaTicketSystemEnabled) {
        value = 0;
        msg->Get(value);
        this->m_europaTicketThrottle.m_tryCount = value;

        value = 0;
        msg->Get(value);
        this->m_europaTicketThrottle.m_lastResetTimeBeforeNow = value;

        value = 0;
        msg->Get(value);
        this->m_europaTicketThrottle.m_maxTries = value;

        value = 0;
        msg->Get(value);
        this->m_europaTicketThrottle.m_perMilliseconds = value;
    }

    if (this->m_sessionAlertEnabled) {
        int32_t alert = 0;
        msg->Get(alert);
        this->m_sessionAlertPeriod = alert;

        alert = 0;
        msg->Get(alert);
        this->m_sessionAlertDelay = alert;

        alert = 0;
        msg->Get(alert);
        this->m_sessionAlertDisplayTime = alert;
    }
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
        int32_t value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_category = value;

        value = 0;
        msg->Get(value);
        this->m_categoryCooldowns.m_data[i].m_modCooldown = value;
    }
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
    int32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientItemRemovePassive::~ClientItemRemovePassive() {
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

    int32_t value = 0;
    msg->Get(value);
    this->m_itemID = value;

    uint8_t byteValue;

    if (this->m_lootedGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[4] ^= byteValue;
    }

    if (this->m_lootedGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[1] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_randomPropertiesSeed = value;

    value = 0;
    msg->Get(value);
    this->m_randomPropertiesID = value;

    if (this->m_lootedGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[6] ^= byteValue;
    }

    if (this->m_lootedGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[5] ^= byteValue;
    }

    if (this->m_lootedGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[2] ^= byteValue;
    }

    if (this->m_lootedGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[7] ^= byteValue;
    }

    if (this->m_lootedGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[3] ^= byteValue;
    }

    if (this->m_lootedGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_lootedGUID[0] ^= byteValue;
    }
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
        int32_t value = 0;
        msg->Get(value);
        this->m_spellIDs.m_data[i] = value;
    }
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
    uint32_t value = 0;
    msg->Get(value);
    this->m_sequenceIndex = value;

    float scale = 0.0f;
    msg->Get(scale);
    this->m_timeScale = scale;
}

// OFFSET: 0x006A1090/0x14034E2D0
ClientTimeAdjustment::~ClientTimeAdjustment() {
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
    int32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A25E0/0x14034EC80
ClientItemAddPassive::~ClientItemAddPassive() {
}

// OFFSET: 0x006B0510/0x1403612E0
bool ClientItemAddPassive::Handle() {
    if (!ClientItemAddPassive::s_handler) {
        return false;
    }

    return ClientItemAddPassive::s_handler(this);
}

// OFFSET: 0x006A2E40
ClientBattlegroundInfoThrottled::ClientBattlegroundInfoThrottled(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120
ClientBattlegroundInfoThrottled::~ClientBattlegroundInfoThrottled() {
}

// OFFSET: 0x006B04D0
bool ClientBattlegroundInfoThrottled::Handle() {
    if (!ClientBattlegroundInfoThrottled::s_handler) {
        return false;
    }

    return ClientBattlegroundInfoThrottled::s_handler(this);
}

// OFFSET: 0x006A2E40
ClientChatIgnoredAccountMuted::ClientChatIgnoredAccountMuted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120
ClientChatIgnoredAccountMuted::~ClientChatIgnoredAccountMuted() {
}

// OFFSET: 0x006B2230
bool ClientChatIgnoredAccountMuted::Handle() {
    if (!ClientChatIgnoredAccountMuted::s_handler) {
        return false;
    }

    return ClientChatIgnoredAccountMuted::s_handler(this);
}

// OFFSET: 0x006A2E40
ClientSorStartExperienceIncomplete::ClientSorStartExperienceIncomplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// OFFSET: 0x006A1120
ClientSorStartExperienceIncomplete::~ClientSorStartExperienceIncomplete() {
}

// OFFSET: 0x006B5D30
bool ClientSorStartExperienceIncomplete::Handle() {
    if (!ClientSorStartExperienceIncomplete::s_handler) {
        return false;
    }

    return ClientSorStartExperienceIncomplete::s_handler(this);
}

// OFFSET: 0x006C8760
ClientGuildCommandResult::ClientGuildCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1DC0
ClientGuildCommandResult::~ClientGuildCommandResult() {
}

// OFFSET: 0x006BE9A0
void ClientGuildCommandResult::Unpack(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_command = value;

    value = 0;
    msg->Get(value);
    this->m_result = value;

    JamBitReader bits(msg);
    uint32_t length = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_name) = length;
    msg->GetData(this->m_name, length);
    this->m_name[length] = 0;
}

// OFFSET: 0x006BD100
bool ClientGuildCommandResult::Handle() {
    if (!ClientGuildCommandResult::s_handler) {
        return false;
    }

    return ClientGuildCommandResult::s_handler(this);
}

// OFFSET: 0x006C60C0
ClientArenaTeamCommandResult::ClientArenaTeamCommandResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2A90
ClientArenaTeamCommandResult::~ClientArenaTeamCommandResult() {
}

// OFFSET: 0x006AD390
void ClientArenaTeamCommandResult::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t nameLength = bits.ReadBits<7>();
    *reinterpret_cast<uint32_t*>(this->m_name) = nameLength;
    uint32_t teamLength = bits.ReadBits<8>();
    *reinterpret_cast<uint32_t*>(this->m_teamName) = teamLength;
    msg->GetData(this->m_name, nameLength);
    this->m_name[nameLength] = 0;

    uint32_t value = 0;
    msg->Get(value);
    this->m_action = value;

    value = 0;
    msg->Get(value);
    this->m_result = value;

    msg->GetData(this->m_teamName, teamLength);
    this->m_teamName[teamLength] = 0;
}

// OFFSET: 0x006B3E10
bool ClientArenaTeamCommandResult::Handle() {
    if (!ClientArenaTeamCommandResult::s_handler) {
        return false;
    }

    return ClientArenaTeamCommandResult::s_handler(this);
}

// OFFSET: 0x006DC660
ClientGuildMembersWithRecipe::ClientGuildMembersWithRecipe(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006D7310
ClientGuildMembersWithRecipe::~ClientGuildMembersWithRecipe() {
}

// OFFSET: 0x006DA9B0
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
        static const uint8_t order[8] = { 1, 5, 6, 7, 2, 3, 0, 4 };

        for (uint32_t j = 0; j < 8; j++) {
            if (guid[order[j]]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                guid[order[j]] ^= byteValue;
            }
        }
    }

    int32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    value = 0;
    msg->Get(value);
    this->m_skillLineID = value;
}

// OFFSET: 0x006C0CC0
bool ClientGuildMembersWithRecipe::Handle() {
    if (!ClientGuildMembersWithRecipe::s_handler) {
        return false;
    }

    return ClientGuildMembersWithRecipe::s_handler(this);
}

// OFFSET: 0x006C0A50
ClientRoleChangedInform::ClientRoleChangedInform(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2EE0
ClientRoleChangedInform::~ClientRoleChangedInform() {
}

// OFFSET: 0x006AAFA0
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
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[7] ^= byteValue;
    }

    if (this->m_targetGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[0] ^= byteValue;
    }

    uint32_t value = 0;
    msg->Get(value);
    this->m_newRole = value;

    if (this->m_targetGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[6] ^= byteValue;
    }

    if (this->m_targetGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[2] ^= byteValue;
    }

    if (this->m_changedByGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[0] ^= byteValue;
    }

    if (this->m_changedByGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[4] ^= byteValue;
    }

    if (this->m_targetGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[1] ^= byteValue;
    }

    if (this->m_changedByGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[3] ^= byteValue;
    }

    if (this->m_changedByGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[5] ^= byteValue;
    }

    if (this->m_changedByGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[2] ^= byteValue;
    }

    if (this->m_targetGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[5] ^= byteValue;
    }

    if (this->m_targetGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_targetGUID[7] ^= byteValue;
    }

    if (this->m_changedByGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_changedByGUID[1] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_oldRole = value;
}

// OFFSET: 0x006AB430
bool ClientRoleChangedInform::Handle() {
    if (!ClientRoleChangedInform::s_handler) {
        return false;
    }

    return ClientRoleChangedInform::s_handler(this);
}

// OFFSET: 0x006C45E0
ClientRolePollBegin::ClientRolePollBegin(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DC0
ClientRolePollBegin::~ClientRolePollBegin() {
}

// OFFSET: 0x006B8DB0
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

    static const uint8_t order[8] = { 4, 7, 0, 5, 1, 6, 2, 3 };

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_initiatorGUID[order[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_initiatorGUID[order[i]] ^= byteValue;
        }
    }
}

// OFFSET: 0x006AE720
bool ClientRolePollBegin::Handle() {
    if (!ClientRolePollBegin::s_handler) {
        return false;
    }

    return ClientRolePollBegin::s_handler(this);
}

// OFFSET: 0x006A1EB0
ClientRaidMarkersChanged::ClientRaidMarkersChanged(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_raidMarkers = value;
}

// OFFSET: 0x006A25E0
ClientRaidMarkersChanged::~ClientRaidMarkersChanged() {
}

// OFFSET: 0x006B8360
bool ClientRaidMarkersChanged::Handle() {
    if (!ClientRaidMarkersChanged::s_handler) {
        return false;
    }

    return ClientRaidMarkersChanged::s_handler(this);
}

// OFFSET: 0x006B3560
ClientPetitionAlreadySigned::ClientPetitionAlreadySigned(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A14E0
ClientPetitionAlreadySigned::~ClientPetitionAlreadySigned() {
}

// OFFSET: 0x006A8EF0
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

    static const uint8_t order[8] = { 4, 0, 3, 1, 5, 6, 7, 2 };

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_signerGUID[order[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_signerGUID[order[i]] ^= byteValue;
        }
    }
}

// OFFSET: 0x006A97D0
bool ClientPetitionAlreadySigned::Handle() {
    if (!ClientPetitionAlreadySigned::s_handler) {
        return false;
    }

    return ClientPetitionAlreadySigned::s_handler(this);
}

// OFFSET: 0x006E2C50
ClientSummonRaidMemberValidateFailed::ClientSummonRaidMemberValidateFailed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006DB2D0
ClientSummonRaidMemberValidateFailed::~ClientSummonRaidMemberValidateFailed() {
}

// OFFSET: 0x006DD220
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
        static const uint8_t first[5] = { 4, 2, 0, 6, 5 };
        static const uint8_t last[3] = { 7, 3, 1 };

        for (uint32_t j = 0; j < 5; j++) {
            if (guid[first[j]]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                guid[first[j]] ^= byteValue;
            }
        }

        int32_t value = 0;
        msg->Get(value);
        this->m_members.m_data[i].m_reasonCode = value;

        for (uint32_t j = 0; j < 3; j++) {
            if (guid[last[j]]) {
                uint8_t byteValue = 0;
                msg->Get(byteValue);
                guid[last[j]] ^= byteValue;
            }
        }
    }
}

// OFFSET: 0x006A5650
bool ClientSummonRaidMemberValidateFailed::Handle() {
    if (!ClientSummonRaidMemberValidateFailed::s_handler) {
        return false;
    }

    return ClientSummonRaidMemberValidateFailed::s_handler(this);
}

// OFFSET: 0x006F8280
ClientQuestNpcQueryResponse::ClientQuestNpcQueryResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006EE990
ClientQuestNpcQueryResponse::~ClientQuestNpcQueryResponse() {
}

// OFFSET: 0x006F6DB0
void ClientQuestNpcQueryResponse::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    this->m_quests.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        this->m_quests.m_data[i].m_npcs.SetCount(bits.ReadBits<24>());
    }

    for (uint32_t i = 0; i < this->m_quests.Count(); i++) {
        int32_t value = 0;
        msg->Get(value);
        this->m_quests.m_data[i].m_questID = value;

        for (uint32_t j = 0; j < this->m_quests.m_data[i].m_npcs.Count(); j++) {
            value = 0;
            msg->Get(value);
            this->m_quests.m_data[i].m_npcs.m_data[j] = value;
        }
    }
}

// OFFSET: 0x006B5090
bool ClientQuestNpcQueryResponse::Handle() {
    if (!ClientQuestNpcQueryResponse::s_handler) {
        return false;
    }

    return ClientQuestNpcQueryResponse::s_handler(this);
}

// OFFSET: 0x006C0D00
ClientSpellinterruptlog::ClientSpellinterruptlog(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2D90
ClientSpellinterruptlog::~ClientSpellinterruptlog() {
}

// OFFSET: 0x006AC8F0
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
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[7] ^= byteValue;
    }

    if (this->m_casterGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[6] ^= byteValue;
    }

    if (this->m_casterGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[3] ^= byteValue;
    }

    if (this->m_casterGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[7]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[4] ^= byteValue;
    }

    int32_t value = 0;
    msg->Get(value);
    this->m_interruptingSpellID = value;

    if (this->m_victimGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_casterGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[0] ^= byteValue;
    }

    if (this->m_casterGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[5] ^= byteValue;
    }

    if (this->m_casterGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_casterGUID[1] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_interruptedSpellID = value;

    if (this->m_victimGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }
}

// OFFSET: 0x006C0800
bool ClientSpellinterruptlog::Handle() {
    if (!ClientSpellinterruptlog::s_handler) {
        return false;
    }

    return ClientSpellinterruptlog::s_handler(this);
}

// OFFSET: 0x006C7960
ClientXpGainAborted::ClientXpGainAborted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A2640
ClientXpGainAborted::~ClientXpGainAborted() {
}

// OFFSET: 0x006ACF00
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
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[7] ^= byteValue;
    }

    if (this->m_victimGUID[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[1] ^= byteValue;
    }

    if (this->m_victimGUID[4]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[4] ^= byteValue;
    }

    if (this->m_victimGUID[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[0] ^= byteValue;
    }

    if (this->m_victimGUID[2]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[2] ^= byteValue;
    }

    uint32_t value = 0;
    msg->Get(value);
    this->m_xpBonus = value;

    if (this->m_victimGUID[6]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[6] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_reason = value;

    if (this->m_victimGUID[3]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[3] ^= byteValue;
    }

    if (this->m_victimGUID[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        this->m_victimGUID[5] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_xp = value;
}

// OFFSET: 0x006AD340
bool ClientXpGainAborted::Handle() {
    if (!ClientXpGainAborted::s_handler) {
        return false;
    }

    return ClientXpGainAborted::s_handler(this);
}

// OFFSET: 0x006B1EA0
ClientItemExpirePurchaseRefund::ClientItemExpirePurchaseRefund(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1230
ClientItemExpirePurchaseRefund::~ClientItemExpirePurchaseRefund() {
}

// OFFSET: 0x006AABB0
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

    static const uint8_t order[8] = { 1, 0, 3, 4, 7, 6, 5, 2 };

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_itemGUID[order[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_itemGUID[order[i]] ^= byteValue;
        }
    }
}

// OFFSET: 0x006BE2C0
bool ClientItemExpirePurchaseRefund::Handle() {
    if (!ClientItemExpirePurchaseRefund::s_handler) {
        return false;
    }

    return ClientItemExpirePurchaseRefund::s_handler(this);
}

// OFFSET: 0x006A2A20
ClientGuildSendGuildXP::ClientGuildSendGuildXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0DE0
ClientGuildSendGuildXP::~ClientGuildSendGuildXP() {
}

// OFFSET: 0x006A1BE0
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

// OFFSET: 0x006B5720
bool ClientGuildSendGuildXP::Handle() {
    if (!ClientGuildSendGuildXP::s_handler) {
        return false;
    }

    return ClientGuildSendGuildXP::s_handler(this);
}

// OFFSET: 0x006B3230
ClientGuildSendMaxDailyXP::ClientGuildSendMaxDailyXP(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    uint64_t value = 0;
    msg->Get(value);
    this->m_xp = value;
}

// OFFSET: 0x006A2650
ClientGuildSendMaxDailyXP::~ClientGuildSendMaxDailyXP() {
}

// OFFSET: 0x006BE040
bool ClientGuildSendMaxDailyXP::Handle() {
    if (!ClientGuildSendMaxDailyXP::s_handler) {
        return false;
    }

    return ClientGuildSendMaxDailyXP::s_handler(this);
}

// OFFSET: 0x006B93F0
ClientGuildSendRankChange::ClientGuildSendRankChange(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0ED0
ClientGuildSendRankChange::~ClientGuildSendRankChange() {
}

// OFFSET: 0x006A4AD0
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

    uint32_t value = 0;
    msg->Get(value);
    this->m_rankID = value;

    uint8_t* guids = this->m_officerGUID;
    static const uint8_t order[16] = { 3, 15, 6, 2, 13, 8, 7, 5, 10, 9, 0, 4, 1, 11, 14, 12 };

    for (uint32_t i = 0; i < 16; i++) {
        if (guids[order[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guids[order[i]] ^= byteValue;
        }
    }
}

// OFFSET: 0x006AB240
bool ClientGuildSendRankChange::Handle() {
    if (!ClientGuildSendRankChange::s_handler) {
        return false;
    }

    return ClientGuildSendRankChange::s_handler(this);
}

// OFFSET: 0x006C7880
ClientGuildMemberRecipes::ClientGuildMemberRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1980
ClientGuildMemberRecipes::~ClientGuildMemberRecipes() {
}

// OFFSET: 0x006BDAF0
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

    static const uint8_t first[5] = { 0, 1, 7, 4, 5 };
    static const uint8_t last[3] = { 3, 6, 2 };

    for (uint32_t i = 0; i < 5; i++) {
        if (this->m_memberGUID[first[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_memberGUID[first[i]] ^= byteValue;
        }
    }

    int32_t value = 0;
    msg->Get(value);
    this->m_skillRank = value;

    for (uint32_t i = 0; i < 300; i++) {
        uint8_t byteValue = 0;
        msg->Get(byteValue);
        this->m_skillLineBitArray[i] = byteValue;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (this->m_memberGUID[last[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_memberGUID[last[i]] ^= byteValue;
        }
    }

    value = 0;
    msg->Get(value);
    this->m_skillLineID = value;

    value = 0;
    msg->Get(value);
    this->m_skillStep = value;
}

// OFFSET: 0x006BE7F0
bool ClientGuildMemberRecipes::Handle() {
    if (!ClientGuildMemberRecipes::s_handler) {
        return false;
    }

    return ClientGuildMemberRecipes::s_handler(this);
}

// OFFSET: 0x006EE9F0
ClientGuildKnownRecipes::ClientGuildKnownRecipes(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E5530
ClientGuildKnownRecipes::~ClientGuildKnownRecipes() {
}

// OFFSET: 0x006E36E0
void ClientGuildKnownRecipes::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint32_t high = bits.ReadBits<8>() << 8;
    this->m_recipes.SetCount(high | bits.ReadBits<8>());

    for (uint32_t i = 0; i < this->m_recipes.Count(); i++) {
        int32_t value = 0;
        msg->Get(value);
        this->m_recipes.m_data[i].m_skillLineID = value;

        for (uint32_t j = 0; j < 300; j++) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            this->m_recipes.m_data[i].m_skillLineBitArray[j] = byteValue;
        }
    }
}

// OFFSET: 0x006A3300
bool ClientGuildKnownRecipes::Handle() {
    if (!ClientGuildKnownRecipes::s_handler) {
        return false;
    }

    return ClientGuildKnownRecipes::s_handler(this);
}

// OFFSET: 0x006A6B80
ClientStartTimer::ClientStartTimer(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A13A0
ClientStartTimer::~ClientStartTimer() {
}

// OFFSET: 0x006A2460
void ClientStartTimer::Unpack(CDataStore* msg, bool full) {
    uint32_t value = 0;
    msg->Get(value);
    this->m_type = value;

    value = 0;
    msg->Get(value);
    this->m_timeLeft = value;

    value = 0;
    msg->Get(value);
    this->m_totalTime = value;
}

// OFFSET: 0x006B64E0
bool ClientStartTimer::Handle() {
    if (!ClientStartTimer::s_handler) {
        return false;
    }

    return ClientStartTimer::s_handler(this);
}

// OFFSET: 0x006D6510
ClientItemRefundInfoResponse::ClientItemRefundInfoResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006CA830
ClientItemRefundInfoResponse::~ClientItemRefundInfoResponse() {
}

// OFFSET: 0x006A63A0
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
        byteValue = 0;
        msg->Get(byteValue);
        guid[7] ^= byteValue;
    }

    uint32_t value = 0;
    msg->Get(value);
    this->m_info.m_refundTimeLeft = value;

    for (uint32_t i = 0; i < 5; i++) {
        value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_items[i].m_count = value;

        value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_items[i].m_id = value;
    }

    static const uint8_t middle[4] = { 6, 4, 3, 2 };

    for (uint32_t i = 0; i < 4; i++) {
        if (guid[middle[i]]) {
            byteValue = 0;
            msg->Get(byteValue);
            guid[middle[i]] ^= byteValue;
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_currencies[i].m_count = value;

        value = 0;
        msg->Get(value);
        this->m_info.m_cost.m_currencies[i].m_id = value;
    }

    if (guid[1]) {
        byteValue = 0;
        msg->Get(byteValue);
        guid[1] ^= byteValue;
    }

    if (guid[5]) {
        byteValue = 0;
        msg->Get(byteValue);
        guid[5] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_info.m_flags = value;

    if (guid[0]) {
        byteValue = 0;
        msg->Get(byteValue);
        guid[0] ^= byteValue;
    }

    value = 0;
    msg->Get(value);
    this->m_info.m_cost.m_money = value;
}

// OFFSET: 0x006ABBF0
bool ClientItemRefundInfoResponse::Handle() {
    if (!ClientItemRefundInfoResponse::s_handler) {
        return false;
    }

    return ClientItemRefundInfoResponse::s_handler(this);
}

// OFFSET: 0x006D9620
ClientItemRefundResult::ClientItemRefundResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_result.m_hasCost = false;
    this->Unpack(msg, true);
}

// OFFSET: 0x006DA120
ClientItemRefundResult::~ClientItemRefundResult() {
}

// OFFSET: 0x006B97B0
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
            value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_currencies[i].m_count = value;

            value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_currencies[i].m_id = value;
        }

        value = 0;
        msg->Get(value);
        this->m_result.m_cost.m_money = value;

        for (uint32_t i = 0; i < 5; i++) {
            value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_items[i].m_count = value;

            value = 0;
            msg->Get(value);
            this->m_result.m_cost.m_items[i].m_id = value;
        }
    }

    static const uint8_t order[8] = { 0, 3, 1, 6, 4, 2, 7, 5 };

    for (uint32_t i = 0; i < 8; i++) {
        if (guid[order[i]]) {
            uint8_t byteValue = 0;
            msg->Get(byteValue);
            guid[order[i]] ^= byteValue;
        }
    }

    uint8_t byteValue = 0;
    msg->Get(byteValue);
    this->m_result.m_result = byteValue;
}

// OFFSET: 0x006BB780
bool ClientItemRefundResult::Handle() {
    if (!ClientItemRefundResult::s_handler) {
        return false;
    }

    return ClientItemRefundResult::s_handler(this);
}

// OFFSET: 0x006C1180
ClientQuestGiverQuestComplete::ClientQuestGiverQuestComplete(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A1DB0
ClientQuestGiverQuestComplete::~ClientQuestGiverQuestComplete() {
}

// OFFSET: 0x006B54F0
void ClientQuestGiverQuestComplete::Unpack(CDataStore* msg, bool full) {
    int32_t value = 0;
    msg->Get(value);
    this->m_complete.m_talentReward = value;

    value = 0;
    msg->Get(value);
    this->m_complete.m_numSkillUpsReward = value;

    value = 0;
    msg->Get(value);
    this->m_complete.m_moneyReward = value;

    value = 0;
    msg->Get(value);
    this->m_complete.m_xpReward = value;

    value = 0;
    msg->Get(value);
    this->m_complete.m_questID = value;

    value = 0;
    msg->Get(value);
    this->m_complete.m_skillLineIDReward = value;

    JamBitReader bits(msg);
    this->m_complete.m_useQuestReward = bits.ReadBit() != 0;
    this->m_complete.m_launchGossip = bits.ReadBit() != 0;
}

// OFFSET: 0x006B07E0
bool ClientQuestGiverQuestComplete::Handle() {
    if (!ClientQuestGiverQuestComplete::s_handler) {
        return false;
    }

    return ClientQuestGiverQuestComplete::s_handler(this);
}

// OFFSET: 0x006BD0A0
ClientSupercededSpells::ClientSupercededSpells(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    int32_t value = 0;
    msg->Get(value);
    this->m_supercededSpellID = value;

    value = 0;
    msg->Get(value);
    this->m_spellID = value;
}

// OFFSET: 0x006A10B0
ClientSupercededSpells::~ClientSupercededSpells() {
}

// OFFSET: 0x006B44D0
bool ClientSupercededSpells::Handle() {
    if (!ClientSupercededSpells::s_handler) {
        return false;
    }

    return ClientSupercededSpells::s_handler(this);
}

// OFFSET: 0x006A2200
ClientLearnedSpell::ClientLearnedSpell(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    int32_t value = 0;
    msg->Get(value);
    this->m_spellID = value;

    uint32_t suppress = 0;
    msg->Get(suppress);
    this->m_suppressMessaging = suppress;
}

// OFFSET: 0x006A1E70
ClientLearnedSpell::~ClientLearnedSpell() {
}

// OFFSET: 0x006ABD00
bool ClientLearnedSpell::Handle() {
    if (!ClientLearnedSpell::s_handler) {
        return false;
    }

    return ClientLearnedSpell::s_handler(this);
}

// OFFSET: 0x006A2AD0
ClientDebugRuneRegen::ClientDebugRuneRegen(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A0D90
ClientDebugRuneRegen::~ClientDebugRuneRegen() {
}

// OFFSET: 0x006A1410
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

    uint32_t value = 0;
    msg->Get(value);
    this->m_runeMask = value;

    value = 0;
    msg->Get(value);
    this->m_runeState = value;
}

// OFFSET: 0x006A8D50
bool ClientDebugRuneRegen::Handle() {
    if (!ClientDebugRuneRegen::s_handler) {
        return false;
    }

    return ClientDebugRuneRegen::s_handler(this);
}
