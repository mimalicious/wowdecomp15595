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

// OFFSET: 0x006708E0/0x140318700
UserClientVoidStorageQuery::UserClientVoidStorageQuery() {
}

// OFFSET: 0x006708B0/0x1403185A0
UserClientVoidStorageQuery::~UserClientVoidStorageQuery() {
}

// OFFSET: 0x00674250
void UserClientVoidStorageQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x790A));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_npc[4] != 0);
    bits.WriteBit(this->m_npc[0] != 0);
    bits.WriteBit(this->m_npc[5] != 0);
    bits.WriteBit(this->m_npc[7] != 0);
    bits.WriteBit(this->m_npc[6] != 0);
    bits.WriteBit(this->m_npc[3] != 0);
    bits.WriteBit(this->m_npc[1] != 0);
    bits.WriteBit(this->m_npc[2] != 0);
    bits.Flush();

    if (this->m_npc[5]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[5] ^ 1));
    }

    if (this->m_npc[6]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[6] ^ 1));
    }

    if (this->m_npc[3]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[3] ^ 1));
    }

    if (this->m_npc[7]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[7] ^ 1));
    }

    if (this->m_npc[1]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[1] ^ 1));
    }

    if (this->m_npc[0]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[0] ^ 1));
    }

    if (this->m_npc[4]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[4] ^ 1));
    }

    if (this->m_npc[2]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[2] ^ 1));
    }
}

// OFFSET: 0x00670560/0x140317E90
UserClientVoidStorageUnlock::UserClientVoidStorageUnlock() {
}

// OFFSET: 0x00670810/0x140318330
UserClientVoidStorageUnlock::~UserClientVoidStorageUnlock() {
}

// OFFSET: 0x006738B0
void UserClientVoidStorageUnlock::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7B14));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_npc[4] != 0);
    bits.WriteBit(this->m_npc[5] != 0);
    bits.WriteBit(this->m_npc[3] != 0);
    bits.WriteBit(this->m_npc[0] != 0);
    bits.WriteBit(this->m_npc[2] != 0);
    bits.WriteBit(this->m_npc[1] != 0);
    bits.WriteBit(this->m_npc[7] != 0);
    bits.WriteBit(this->m_npc[6] != 0);
    bits.Flush();

    if (this->m_npc[7]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[7] ^ 1));
    }

    if (this->m_npc[1]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[1] ^ 1));
    }

    if (this->m_npc[2]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[2] ^ 1));
    }

    if (this->m_npc[3]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[3] ^ 1));
    }

    if (this->m_npc[5]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[5] ^ 1));
    }

    if (this->m_npc[0]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[0] ^ 1));
    }

    if (this->m_npc[6]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[6] ^ 1));
    }

    if (this->m_npc[4]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[4] ^ 1));
    }
}

// OFFSET: 0x00673110/0x140317BD0
UserClientVoidSwapItem::UserClientVoidSwapItem() {
}

// OFFSET: 0x00670500/0x140317DE0
UserClientVoidSwapItem::~UserClientVoidSwapItem() {
}

// OFFSET: 0x00675000
void UserClientVoidSwapItem::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3204));
    msg->Put(this->m_slot);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_npc[2] != 0);
    bits.WriteBit(this->m_npc[4] != 0);
    bits.WriteBit(this->m_npc[0] != 0);
    bits.WriteBit(this->m_item[2] != 0);
    bits.WriteBit(this->m_item[6] != 0);
    bits.WriteBit(this->m_item[5] != 0);
    bits.WriteBit(this->m_npc[1] != 0);
    bits.WriteBit(this->m_npc[7] != 0);
    bits.WriteBit(this->m_item[3] != 0);
    bits.WriteBit(this->m_item[7] != 0);
    bits.WriteBit(this->m_item[0] != 0);
    bits.WriteBit(this->m_npc[6] != 0);
    bits.WriteBit(this->m_npc[5] != 0);
    bits.WriteBit(this->m_npc[3] != 0);
    bits.WriteBit(this->m_item[1] != 0);
    bits.WriteBit(this->m_item[4] != 0);
    bits.Flush();

    if (this->m_npc[1]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[1] ^ 1));
    }

    if (this->m_item[3]) {
        msg->Put(static_cast<uint8_t>(this->m_item[3] ^ 1));
    }

    if (this->m_item[2]) {
        msg->Put(static_cast<uint8_t>(this->m_item[2] ^ 1));
    }

    if (this->m_item[4]) {
        msg->Put(static_cast<uint8_t>(this->m_item[4] ^ 1));
    }

    if (this->m_npc[3]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[3] ^ 1));
    }

    if (this->m_npc[0]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[0] ^ 1));
    }

    if (this->m_item[6]) {
        msg->Put(static_cast<uint8_t>(this->m_item[6] ^ 1));
    }

    if (this->m_item[1]) {
        msg->Put(static_cast<uint8_t>(this->m_item[1] ^ 1));
    }

    if (this->m_npc[5]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[5] ^ 1));
    }

    if (this->m_item[5]) {
        msg->Put(static_cast<uint8_t>(this->m_item[5] ^ 1));
    }

    if (this->m_npc[6]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[6] ^ 1));
    }

    if (this->m_item[0]) {
        msg->Put(static_cast<uint8_t>(this->m_item[0] ^ 1));
    }

    if (this->m_npc[2]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[2] ^ 1));
    }

    if (this->m_npc[7]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[7] ^ 1));
    }

    if (this->m_npc[4]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[4] ^ 1));
    }

    if (this->m_item[7]) {
        msg->Put(static_cast<uint8_t>(this->m_item[7] ^ 1));
    }
}

// OFFSET: 0x006880F0/0x140332F20
UserClientVoidStorageTransfer::UserClientVoidStorageTransfer() {
}

// OFFSET: 0x00688060/0x14032F260
UserClientVoidStorageTransfer::~UserClientVoidStorageTransfer() {
}

// OFFSET: 0x006837D0
void UserClientVoidStorageTransfer::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x380E));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_npc[1] != 0);
    bits.WriteBits<26>(this->m_deposits.m_count);

    for (uint32_t i = 0; i < this->m_deposits.m_count; i++) {
        const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_deposits.m_data[i]);
        bits.WriteBit(guid[4] != 0);
        bits.WriteBit(guid[6] != 0);
        bits.WriteBit(guid[7] != 0);
        bits.WriteBit(guid[0] != 0);
        bits.WriteBit(guid[1] != 0);
        bits.WriteBit(guid[5] != 0);
        bits.WriteBit(guid[3] != 0);
        bits.WriteBit(guid[2] != 0);
    }

    bits.WriteBit(this->m_npc[2] != 0);
    bits.WriteBit(this->m_npc[0] != 0);
    bits.WriteBit(this->m_npc[3] != 0);
    bits.WriteBit(this->m_npc[5] != 0);
    bits.WriteBit(this->m_npc[6] != 0);
    bits.WriteBit(this->m_npc[4] != 0);
    bits.WriteBits<26>(this->m_withdrawals.m_count);

    for (uint32_t i = 0; i < this->m_withdrawals.m_count; i++) {
        const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_withdrawals.m_data[i]);
        bits.WriteBit(guid[4] != 0);
        bits.WriteBit(guid[7] != 0);
        bits.WriteBit(guid[1] != 0);
        bits.WriteBit(guid[0] != 0);
        bits.WriteBit(guid[2] != 0);
        bits.WriteBit(guid[3] != 0);
        bits.WriteBit(guid[5] != 0);
        bits.WriteBit(guid[6] != 0);
    }

    bits.WriteBit(this->m_npc[7] != 0);
    bits.Flush();

    for (uint32_t i = 0; i < this->m_deposits.m_count; i++) {
        const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_deposits.m_data[i]);

        if (guid[6]) {
            msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
        }

        if (guid[1]) {
            msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
        }

        if (guid[0]) {
            msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
        }

        if (guid[2]) {
            msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
        }

        if (guid[4]) {
            msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
        }

        if (guid[5]) {
            msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
        }

        if (guid[3]) {
            msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
        }

        if (guid[7]) {
            msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
        }
    }

    if (this->m_npc[5]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[5] ^ 1));
    }

    if (this->m_npc[6]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[6] ^ 1));
    }

    for (uint32_t i = 0; i < this->m_withdrawals.m_count; i++) {
        const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_withdrawals.m_data[i]);

        if (guid[3]) {
            msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
        }

        if (guid[0]) {
            msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
        }

        if (guid[1]) {
            msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
        }

        if (guid[6]) {
            msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
        }

        if (guid[2]) {
            msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
        }

        if (guid[7]) {
            msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
        }

        if (guid[5]) {
            msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
        }

        if (guid[4]) {
            msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
        }
    }

    if (this->m_npc[1]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[1] ^ 1));
    }

    if (this->m_npc[4]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[4] ^ 1));
    }

    if (this->m_npc[7]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[7] ^ 1));
    }

    if (this->m_npc[3]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[3] ^ 1));
    }

    if (this->m_npc[2]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[2] ^ 1));
    }

    if (this->m_npc[0]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[0] ^ 1));
    }
}

// OFFSET: [0x0066BC00/0x140312430]
JamTransmogrifyItem::JamTransmogrifyItem() {
}

// OFFSET: 0x00688120
UserClientTransmogrifyItems::UserClientTransmogrifyItems() {
}

// OFFSET: 0x00688140/0x140332EA0
UserClientTransmogrifyItems::~UserClientTransmogrifyItems() {
}

// OFFSET: 0x0067F1F0/0x140328810
void UserClientTransmogrifyItems::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3B0E));

    JamBitWriter bits(msg);
    bits.WriteBits<22>(this->m_items.m_count);

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[0] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[5] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[6] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[2] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[3] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[7] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[4] != 0);
        bits.WriteBit(this->m_items.m_data[i].m_itemGuid[1] != 0);
    }

    bits.WriteBit(this->m_npc[7] != 0);
    bits.WriteBit(this->m_npc[3] != 0);
    bits.WriteBit(this->m_npc[5] != 0);
    bits.WriteBit(this->m_npc[6] != 0);
    bits.WriteBit(this->m_npc[1] != 0);
    bits.WriteBit(this->m_npc[4] != 0);
    bits.WriteBit(this->m_npc[0] != 0);
    bits.WriteBit(this->m_npc[2] != 0);
    bits.Flush();

    for (uint32_t i = 0; i < this->m_items.m_count; i++) {
        msg->Put(static_cast<uint32_t>(this->m_items.m_data[i].m_itemID));

        if (this->m_items.m_data[i].m_itemGuid[1]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[1] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[5]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[5] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[0]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[0] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[4]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[4] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[6]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[6] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[7]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[7] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[3]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[3] ^ 1));
        }

        if (this->m_items.m_data[i].m_itemGuid[2]) {
            msg->Put(static_cast<uint8_t>(this->m_items.m_data[i].m_itemGuid[2] ^ 1));
        }

        msg->Put(this->m_items.m_data[i].m_slot);
    }

    if (this->m_npc[7]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[7] ^ 1));
    }

    if (this->m_npc[2]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[2] ^ 1));
    }

    if (this->m_npc[5]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[5] ^ 1));
    }

    if (this->m_npc[4]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[4] ^ 1));
    }

    if (this->m_npc[3]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[3] ^ 1));
    }

    if (this->m_npc[1]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[1] ^ 1));
    }

    if (this->m_npc[6]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[6] ^ 1));
    }

    if (this->m_npc[0]) {
        msg->Put(static_cast<uint8_t>(this->m_npc[0] ^ 1));
    }
}

// OFFSET: 0x006736F0
UserClientReforgeItem::UserClientReforgeItem() {
    this->m_flags = 0;
}

// OFFSET: 0x006703D0
UserClientReforgeItem::~UserClientReforgeItem() {
}

// OFFSET: 0x006753B0
void UserClientReforgeItem::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x331A));
    msg->Put(static_cast<uint32_t>(this->m_reforgeId));
    msg->Put(this->m_slot);
    msg->Put(this->m_bag);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_forgeMaster[2] != 0);
    bits.WriteBit(this->m_forgeMaster[6] != 0);
    bits.WriteBit(this->m_forgeMaster[3] != 0);
    bits.WriteBit(this->m_forgeMaster[4] != 0);
    bits.WriteBit(this->m_forgeMaster[1] != 0);
    bits.WriteBit(this->m_forgeMaster[0] != 0);
    bits.WriteBit(this->m_forgeMaster[7] != 0);
    bits.WriteBit(this->m_forgeMaster[5] != 0);
    bits.Flush();

    if (this->m_forgeMaster[2]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[2] ^ 1));
    }

    if (this->m_forgeMaster[3]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[3] ^ 1));
    }

    if (this->m_forgeMaster[6]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[6] ^ 1));
    }

    if (this->m_forgeMaster[4]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[4] ^ 1));
    }

    if (this->m_forgeMaster[1]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[1] ^ 1));
    }

    if (this->m_forgeMaster[0]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[0] ^ 1));
    }

    if (this->m_forgeMaster[7]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[7] ^ 1));
    }

    if (this->m_forgeMaster[5]) {
        msg->Put(static_cast<uint8_t>(this->m_forgeMaster[5] ^ 1));
    }
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

// OFFSET: 0x00670920/0x1403187B0
UserClientCancelQueuedSpell::UserClientCancelQueuedSpell() {
}

// OFFSET: 0x00670B40/0x140318D10
UserClientCancelQueuedSpell::~UserClientCancelQueuedSpell() {
}

// OFFSET: 0x00670400
void UserClientCancelQueuedSpell::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7B1C));
}

// OFFSET: 0x0066D140/0x140314140
UserClientPartyInvite::UserClientPartyInvite() {
}

// OFFSET: 0x0066BBD0/0x140312410
UserClientPartyInvite::~UserClientPartyInvite() {
}

// OFFSET: 0x0066F350
void UserClientPartyInvite::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x513));
    msg->Put(this->m_targetCfgRealmID);
    msg->Put(this->m_proposedRoles);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_targetGUID[2] != 0);
    bits.WriteBit(this->m_targetGUID[7] != 0);
    bits.WriteBits<9>(static_cast<uint32_t>(strlen(this->m_targetRealm)));
    bits.WriteBit(this->m_targetGUID[3] != 0);
    bits.WriteBits<10>(static_cast<uint32_t>(strlen(this->m_targetName)));
    bits.WriteBit(this->m_targetGUID[5] != 0);
    bits.WriteBit(this->m_targetGUID[4] != 0);
    bits.WriteBit(this->m_targetGUID[6] != 0);
    bits.WriteBit(this->m_targetGUID[0] != 0);
    bits.WriteBit(this->m_targetGUID[1] != 0);
    bits.Flush();

    if (this->m_targetGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[4] ^ 1));
    }

    if (this->m_targetGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[7] ^ 1));
    }

    if (this->m_targetGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[6] ^ 1));
    }

    msg->PutData(this->m_targetName, static_cast<uint32_t>(strlen(this->m_targetName)));
    msg->PutData(this->m_targetRealm, static_cast<uint32_t>(strlen(this->m_targetRealm)));

    if (this->m_targetGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[1] ^ 1));
    }

    if (this->m_targetGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[0] ^ 1));
    }

    if (this->m_targetGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[5] ^ 1));
    }

    if (this->m_targetGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[3] ^ 1));
    }

    if (this->m_targetGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[2] ^ 1));
    }
}

// OFFSET: 0x0066FA10/0x140316A30
UserClientPartyInviteResponse::UserClientPartyInviteResponse() {
    this->m_hasRolesDesired = false;
}

// OFFSET: 0x0066BA20/0x140312210
UserClientPartyInviteResponse::~UserClientPartyInviteResponse() {
}

// OFFSET: 0x0066E4D0
void UserClientPartyInviteResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x410));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_hasRolesDesired);
    bits.WriteBit(this->m_accept);
    bits.Flush();

    if (this->m_hasRolesDesired) {
        msg->Put(this->m_rolesDesired);
    }
}

// OFFSET: 0x0066D150/0x140314160
UserClientSetEveryoneIsAssistant::UserClientSetEveryoneIsAssistant() {
}

// OFFSET: 0x0066BAB0/0x140312290
UserClientSetEveryoneIsAssistant::~UserClientSetEveryoneIsAssistant() {
}

// OFFSET: 0x0066D760
void UserClientSetEveryoneIsAssistant::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2530));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_everyoneIsAssistant);
    bits.Flush();
}

// OFFSET: 0x00670980/0x1403189C0
UserClientClearRaidMarker::UserClientClearRaidMarker() {
}

// OFFSET: 0x006704F0/0x140317DD0
UserClientClearRaidMarker::~UserClientClearRaidMarker() {
}

// OFFSET: 0x00674D20
void UserClientClearRaidMarker::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7300));
    msg->Put(this->m_markerId);
}

// OFFSET: 0x0066D680/0x140314730
UserClientRequestPartyJoinUpdates::UserClientRequestPartyJoinUpdates() {
}

// OFFSET: 0x0066BB10/0x1403122E0
UserClientRequestPartyJoinUpdates::~UserClientRequestPartyJoinUpdates() {
}

// OFFSET: 0x0066BB50
void UserClientRequestPartyJoinUpdates::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2583));
}

// OFFSET: 0x00B3BD90/0x140908C40
UserLFGuildAddRecruit::UserLFGuildAddRecruit() {
}

// OFFSET: 0x00B3BCF0/0x140908B70
UserLFGuildAddRecruit::~UserLFGuildAddRecruit() {
}

// OFFSET: 0x00B3BE80/0x140908D40
void UserLFGuildAddRecruit::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4448));
    msg->Put(this->m_classRoles);
    msg->Put(this->m_interests);
    msg->Put(this->m_availability);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    uint32_t length = strlen(this->m_comment);
    bits.WriteBits<11>(length);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    length = strlen(this->m_name);
    bits.WriteBits<7>(length);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.Flush();

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    msg->PutData(this->m_comment, strlen(this->m_comment));
    msg->PutData(this->m_name, strlen(this->m_name));

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }
}
