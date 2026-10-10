#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "Engine/Source/Base/CDataStore.h"

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

// OFFSET: 0x006C1130/0x140376650
ClientPlaySpellVisualKit::ClientPlaySpellVisualKit(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006A28B0/0x14034FD30
ClientPlaySpellVisualKit::~ClientPlaySpellVisualKit() {
}

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

// OFFSET: 0x006DEB60/0x14039E700
ClientWeeklySpellUsage::ClientWeeklySpellUsage(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006E5A60/0x1403A6FE0
ClientWeeklySpellUsage::~ClientWeeklySpellUsage() {
}

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

// OFFSET: 0x006B1AE0/0x140350000
ClientWeeklySpellUsageUpdate::~ClientWeeklySpellUsageUpdate() {
}

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

// OFFSET: 0x006D8620/0x140396710
ClientClearCooldowns::ClientClearCooldowns(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x006C9A90/0x1403814F0
ClientClearCooldowns::~ClientClearCooldowns() {
}

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

// OFFSET: 0x006A2E40/0x14036ACB0
ClientWeeklyResetCurrency::ClientWeeklyResetCurrency(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

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
