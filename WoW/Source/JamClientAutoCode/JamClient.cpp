#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "Engine/Source/Base/CDataStore.h"

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
