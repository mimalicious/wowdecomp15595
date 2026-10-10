#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: 5c33784b IDENTICAL/NONE
// OFFSET: 0x0066BC00/0x140312430
JamUserClientReorderEntry::JamUserClientReorderEntry() {
}

// OFFSET: 0x0066BC70
JamUserClientReorderEntry::JamUserClientReorderEntry(const JamUserClientReorderEntry& source) {
    memcpy(this->m_guid, source.m_guid, sizeof(this->m_guid));
    this->m_newIndex = source.m_newIndex;
}

// PROVEN: b446fdc0 IDENTICAL/NONE
// OFFSET: 0x0066D370
UserClientEnumCharacters::UserClientEnumCharacters() {
}

// PROVEN: 814cdb6d IDENTICAL/NONE
// OFFSET: 0x0066B9D0
UserClientEnumCharacters::~UserClientEnumCharacters() {
}

// PROVEN: 336aa2a7 IDENTICAL/NONE
// OFFSET: 0x0066BB20
void UserClientEnumCharacters::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x502));
}

// PROVEN: 71643081 IDENTICAL/NONE
// OFFSET: 0x0066D6F0
UserClientPlayerLogin::UserClientPlayerLogin(uint64_t guid) {
    memcpy(this->m_guid, &guid, sizeof(this->m_guid));
}

// PROVEN: eabc7c9b IDENTICAL/NONE
// OFFSET: 0x0066BC20
UserClientPlayerLogin::~UserClientPlayerLogin() {
}

// PROVEN: 54ede3e3 IDENTICAL/NONE
// OFFSET: 0x0066DEF0
void UserClientPlayerLogin::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x5B1));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guid[2] != 0);
    bits.WriteBit(this->m_guid[3] != 0);
    bits.WriteBit(this->m_guid[0] != 0);
    bits.WriteBit(this->m_guid[6] != 0);
    bits.WriteBit(this->m_guid[4] != 0);
    bits.WriteBit(this->m_guid[5] != 0);
    bits.WriteBit(this->m_guid[1] != 0);
    bits.WriteBit(this->m_guid[7] != 0);
    bits.Flush();

    if (this->m_guid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[2] ^ 1));
    }

    if (this->m_guid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[7] ^ 1));
    }

    if (this->m_guid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[0] ^ 1));
    }

    if (this->m_guid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[3] ^ 1));
    }

    if (this->m_guid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[5] ^ 1));
    }

    if (this->m_guid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[6] ^ 1));
    }

    if (this->m_guid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[1] ^ 1));
    }

    if (this->m_guid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[4] ^ 1));
    }
}

// PROVEN: bb06eec1 IDENTICAL/NONE
// OFFSET: 0x00670290
UserClientReorderCharacters::UserClientReorderCharacters() {
}

// PROVEN: 2291a9d0 IDENTICAL/NONE
// OFFSET: 0x00670360
UserClientReorderCharacters::~UserClientReorderCharacters() {
}

// OFFSET: 0x0066F610
void UserClientReorderCharacters::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x593));

    JamBitWriter bits(msg);
    bits.WriteBits<10>(this->m_entries.Count());

    for (uint32_t i = 0; i < this->m_entries.Count(); i++) {
        const JamUserClientReorderEntry& entry = this->m_entries[i];
        bits.WriteBit(entry.m_guid[1] != 0);
        bits.WriteBit(entry.m_guid[4] != 0);
        bits.WriteBit(entry.m_guid[5] != 0);
        bits.WriteBit(entry.m_guid[3] != 0);
        bits.WriteBit(entry.m_guid[0] != 0);
        bits.WriteBit(entry.m_guid[7] != 0);
        bits.WriteBit(entry.m_guid[6] != 0);
        bits.WriteBit(entry.m_guid[2] != 0);
    }

    bits.Flush();

    for (uint32_t i = 0; i < this->m_entries.Count(); i++) {
        const JamUserClientReorderEntry& entry = this->m_entries[i];

        if (entry.m_guid[6]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[6] ^ 1));
        }

        if (entry.m_guid[5]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[5] ^ 1));
        }

        if (entry.m_guid[1]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[1] ^ 1));
        }

        if (entry.m_guid[4]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[4] ^ 1));
        }

        if (entry.m_guid[0]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[0] ^ 1));
        }

        if (entry.m_guid[3]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[3] ^ 1));
        }

        msg->Put(entry.m_newIndex);

        if (entry.m_guid[2]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[2] ^ 1));
        }

        if (entry.m_guid[7]) {
            msg->Put(static_cast<uint8_t>(entry.m_guid[7] ^ 1));
        }
    }
}

// PROVEN: 31fdb30f IDENTICAL/NONE
// OFFSET: 0x0066DE40/0x140315030
UserClientGenerateRandomCharacterName::UserClientGenerateRandomCharacterName(uint8_t raceID, uint8_t sexID) {
    this->m_raceID = raceID;
    this->m_sexID = sexID;
}

// PROVEN: b94f0e1c IDENTICAL/NONE
// OFFSET: 0x0066B960/0x140312160
UserClientGenerateRandomCharacterName::~UserClientGenerateRandomCharacterName() {
}

// PROVEN: 960528f1 IDENTICAL/NONE
// OFFSET: 0x0066D8E0
void UserClientGenerateRandomCharacterName::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2413));
    msg->Put(this->m_raceID);
    msg->Put(this->m_sexID);
}

// PROVEN: 6fb75a99 IDENTICAL/NONE
// OFFSET: 0x0066BC00/0x140312430
DBQuery::DBQuery() {
}

// PROVEN: 179ddb96 IDENTICAL/NONE
// OFFSET: 0x00670340
UserClientDBQueryBulk::UserClientDBQueryBulk() {
}

// PROVEN: 4d39b4f5 IDENTICAL/NONE
// OFFSET: 0x006702E0/0x140317440
UserClientDBQueryBulk::~UserClientDBQueryBulk() {
}

// OFFSET: 0x0066FC20/0x140316CF0
void UserClientDBQueryBulk::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2401));
    msg->Put(this->m_tableHash);

    JamBitWriter bits(msg);
    bits.WriteBits<23>(this->m_queries.Count());

    for (uint32_t i = 0; i < this->m_queries.Count(); i++) {
        const DBQuery& query = this->m_queries[i];
        bits.WriteBit(query.m_guid[0] != 0);
        bits.WriteBit(query.m_guid[4] != 0);
        bits.WriteBit(query.m_guid[7] != 0);
        bits.WriteBit(query.m_guid[2] != 0);
        bits.WriteBit(query.m_guid[5] != 0);
        bits.WriteBit(query.m_guid[3] != 0);
        bits.WriteBit(query.m_guid[6] != 0);
        bits.WriteBit(query.m_guid[1] != 0);
    }

    bits.Flush();

    for (uint32_t i = 0; i < this->m_queries.Count(); i++) {
        const DBQuery& query = this->m_queries[i];

        if (query.m_guid[5]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[5] ^ 1));
        }

        if (query.m_guid[6]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[6] ^ 1));
        }

        if (query.m_guid[7]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[7] ^ 1));
        }

        if (query.m_guid[0]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[0] ^ 1));
        }

        if (query.m_guid[1]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[1] ^ 1));
        }

        if (query.m_guid[3]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[3] ^ 1));
        }

        if (query.m_guid[4]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[4] ^ 1));
        }

        msg->Put(static_cast<uint32_t>(query.m_id));

        if (query.m_guid[2]) {
            msg->Put(static_cast<uint8_t>(query.m_guid[2] ^ 1));
        }
    }
}

// OFFSET: 0x0066E720
UserClientWorldPortResponse::UserClientWorldPortResponse() {
}

// OFFSET: 0x0066BC60
UserClientWorldPortResponse::~UserClientWorldPortResponse() {
}

// OFFSET: 0x0066B9A0
void UserClientWorldPortResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2411));
}

// OFFSET: 0x00673680/0x140317DF0
UserClientUnlearnSpecialization::UserClientUnlearnSpecialization() {
}

// OFFSET: 0x006703E0/0x140317C30
UserClientUnlearnSpecialization::~UserClientUnlearnSpecialization() {
}

// OFFSET: 0x00673610
void UserClientUnlearnSpecialization::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3210));
    msg->Put(this->m_index);
}

// OFFSET: 0x006709E0/0x140318AB0
UserClientReturnToGraveyard::UserClientReturnToGraveyard() {
}

// OFFSET: 0x00670750/0x140318100
UserClientReturnToGraveyard::~UserClientReturnToGraveyard() {
}

// OFFSET: 0x006704C0
void UserClientReturnToGraveyard::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x301E));
}

// OFFSET: 0x00670870/0x1403184D0
UserClientQueryBattlefieldState::UserClientQueryBattlefieldState() {
}

// OFFSET: 0x006705E0/0x140317F40
UserClientQueryBattlefieldState::~UserClientQueryBattlefieldState() {
}

// OFFSET: 0x00670730
void UserClientQueryBattlefieldState::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7202));
}

// OFFSET: 0x00670610/0x140317F70
UserClientRequestCemeteryList::UserClientRequestCemeteryList() {
}

// OFFSET: 0x00670720
UserClientRequestCemeteryList::~UserClientRequestCemeteryList() {
}

// OFFSET: 0x00670E10
void UserClientRequestCemeteryList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x720A));
}

// OFFSET: 0x00670860/0x1403184B0
UserClientSetPreferredCemetery::UserClientSetPreferredCemetery() {
}

// OFFSET: 0x00670970/0x140318950
UserClientSetPreferredCemetery::~UserClientSetPreferredCemetery() {
}

// OFFSET: 0x00670EC0
void UserClientSetPreferredCemetery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x311E));
    msg->Put(this->m_cemeteryID);
}
