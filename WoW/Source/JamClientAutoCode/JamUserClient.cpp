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

// OFFSET: 0x0066BC90/0x1403124F0
UserClientAcceptWargameInvite::~UserClientAcceptWargameInvite() {
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

// OFFSET: 0x0066D3A0
UserClientSupportTicketSubmitComplaint::UserClientSupportTicketSubmitComplaint() {
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

// OFFSET: 0x0066DE90/0x140315090
UserClientAcceptWargameInvite::UserClientAcceptWargameInvite() {
}

// OFFSET: 0x0066DEA0
UserClientSupportTicketSubmitComplaint::~UserClientSupportTicketSubmitComplaint() {
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

// OFFSET: 0x0066E280
void UserClientAcceptWargameInvite::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2410));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_accept);
    bits.WriteBit(this->m_queueID[3] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[3] != 0);
    bits.WriteBit(this->m_queueID[7] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[2] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[0] != 0);
    bits.WriteBit(this->m_queueID[1] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[5] != 0);
    bits.WriteBit(this->m_queueID[6] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[6] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[1] != 0);
    bits.WriteBit(this->m_queueID[0] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[7] != 0);
    bits.WriteBit(this->m_opposingPartyMemberGUID[4] != 0);
    bits.WriteBit(this->m_queueID[5] != 0);
    bits.WriteBit(this->m_queueID[4] != 0);
    bits.WriteBit(this->m_queueID[2] != 0);
    bits.Flush();

    if (this->m_queueID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[2] ^ 1));
    }
    if (this->m_queueID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[6] ^ 1));
    }
    if (this->m_queueID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[4] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[0] ^ 1));
    }
    if (this->m_queueID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[5] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[1] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[6] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[3] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[5] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[2] ^ 1));
    }
    if (this->m_queueID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[3] ^ 1));
    }
    if (this->m_queueID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[1] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[7] ^ 1));
    }
    if (this->m_queueID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[0] ^ 1));
    }
    if (this->m_queueID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_queueID[7] ^ 1));
    }
    if (this->m_opposingPartyMemberGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_opposingPartyMemberGUID[4] ^ 1));
    }
}

// OFFSET: 0x0066FFA0
void UserClientSupportTicketSubmitComplaint::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2501));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_targetCharacterGUID[5] != 0);
    bits.WriteBit(this->m_targetCharacterGUID[0] != 0);
    bits.WriteBit(this->m_targetCharacterGUID[1] != 0);
    bits.WriteBits<12>(strlen(this->m_note));
    bits.WriteBit(this->m_targetCharacterGUID[3] != 0);
    bits.WriteBit(this->m_targetCharacterGUID[2] != 0);
    bits.WriteBit(this->m_targetCharacterGUID[4] != 0);
    bits.WriteBit(this->m_targetCharacterGUID[7] != 0);
    bits.WriteBits<4>(this->m_complaintType);
    bits.WriteBit(this->m_targetCharacterGUID[6] != 0);
    bits.Flush();

    if (this->m_targetCharacterGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[3] ^ 1));
    }
    if (this->m_targetCharacterGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[5] ^ 1));
    }
    if (this->m_targetCharacterGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[1] ^ 1));
    }
    if (this->m_targetCharacterGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[2] ^ 1));
    }
    if (this->m_targetCharacterGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[6] ^ 1));
    }
    if (this->m_targetCharacterGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[0] ^ 1));
    }

    msg->PutData(this->m_note, strlen(this->m_note));

    if (this->m_targetCharacterGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[7] ^ 1));
    }
    if (this->m_targetCharacterGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_targetCharacterGUID[4] ^ 1));
    }

    this->m_header.Put(msg);
    this->m_chatLog.Put(msg);
}

// OFFSET: 0x00670260
UserClientSendMail::UserClientSendMail() {
}

// PROVEN: bb06eec1 IDENTICAL/NONE
// OFFSET: 0x00670290
UserClientReorderCharacters::UserClientReorderCharacters() {
}

// OFFSET: 0x00670310
UserClientSendMail::~UserClientSendMail() {
}

// PROVEN: 2291a9d0 IDENTICAL/NONE
// OFFSET: 0x00670360
UserClientReorderCharacters::~UserClientReorderCharacters() {
}

// OFFSET: 0x0066ECF0
void UserClientSendMail::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x523));
    msg->Put(static_cast<uint32_t>(this->m_packageID));
    msg->Put(static_cast<uint32_t>(this->m_stationeryID));
    msg->Put(this->m_cod);
    msg->Put(this->m_sendMoney);

    JamBitWriter bits(msg);
    bits.WriteBits<12>(strlen(this->m_body));
    bits.WriteBits<9>(strlen(this->m_subject));
    bits.WriteBits<5>(this->m_attachments.Count());
    bits.WriteBit(this->m_mailbox[0] != 0);

    for (uint32_t i = 0; i < this->m_attachments.Count(); i++) {
        const JamCliStructMailAttachment& attachment = this->m_attachments[i];
        bits.WriteBit(attachment.m_itemGUID[2] != 0);
        bits.WriteBit(attachment.m_itemGUID[6] != 0);
        bits.WriteBit(attachment.m_itemGUID[3] != 0);
        bits.WriteBit(attachment.m_itemGUID[7] != 0);
        bits.WriteBit(attachment.m_itemGUID[1] != 0);
        bits.WriteBit(attachment.m_itemGUID[0] != 0);
        bits.WriteBit(attachment.m_itemGUID[4] != 0);
        bits.WriteBit(attachment.m_itemGUID[5] != 0);
    }

    bits.WriteBit(this->m_mailbox[3] != 0);
    bits.WriteBit(this->m_mailbox[4] != 0);
    bits.WriteBits<7>(strlen(this->m_target));
    bits.WriteBit(this->m_mailbox[2] != 0);
    bits.WriteBit(this->m_mailbox[6] != 0);
    bits.WriteBit(this->m_mailbox[1] != 0);
    bits.WriteBit(this->m_mailbox[7] != 0);
    bits.WriteBit(this->m_mailbox[5] != 0);
    bits.Flush();

    if (this->m_mailbox[4]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[4] ^ 1));
    }

    for (uint32_t i = 0; i < this->m_attachments.Count(); i++) {
        const JamCliStructMailAttachment& attachment = this->m_attachments[i];
        if (attachment.m_itemGUID[6]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[6] ^ 1));
        }

        if (attachment.m_itemGUID[1]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[1] ^ 1));
        }

        if (attachment.m_itemGUID[7]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[7] ^ 1));
        }

        if (attachment.m_itemGUID[2]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[2] ^ 1));
        }

        msg->Put(attachment.m_attachPosition);

        if (attachment.m_itemGUID[3]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[3] ^ 1));
        }

        if (attachment.m_itemGUID[0]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[0] ^ 1));
        }

        if (attachment.m_itemGUID[4]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[4] ^ 1));
        }

        if (attachment.m_itemGUID[5]) {
            msg->Put(static_cast<uint8_t>(attachment.m_itemGUID[5] ^ 1));
        }
    }

    if (this->m_mailbox[7]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[7] ^ 1));
    }

    if (this->m_mailbox[3]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[3] ^ 1));
    }

    if (this->m_mailbox[6]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[6] ^ 1));
    }

    if (this->m_mailbox[5]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[5] ^ 1));
    }

    msg->PutData(this->m_subject, strlen(this->m_subject));
    msg->PutData(this->m_target, strlen(this->m_target));

    if (this->m_mailbox[2]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[2] ^ 1));
    }

    if (this->m_mailbox[0]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[0] ^ 1));
    }

    msg->PutData(this->m_body, strlen(this->m_body));

    if (this->m_mailbox[1]) {
        msg->Put(static_cast<uint8_t>(this->m_mailbox[1] ^ 1));
    }
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

// OFFSET: 0x00670B30
UserClientBattlefieldPort::~UserClientBattlefieldPort() {
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

// OFFSET: 0x00673DA0
UserClientBattlefieldPort::UserClientBattlefieldPort() {
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

// OFFSET: 0x00675250
void UserClientBattlefieldPort::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x711A));
    msg->Put(this->m_ticket.m_time);
    msg->Put(this->m_ticket.m_id);
    msg->Put(this->m_ticket.m_type);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_ticket.m_requesterGuid[0] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[1] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[5] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[6] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[7] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[4] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[3] != 0);
    bits.WriteBit(this->m_ticket.m_requesterGuid[2] != 0);
    bits.WriteBit(this->m_accept);
    bits.Flush();

    if (this->m_ticket.m_requesterGuid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[1] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[3] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[5] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[7] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[0] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[2] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[6] ^ 1));
    }
    if (this->m_ticket.m_requesterGuid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_ticket.m_requesterGuid[4] ^ 1));
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

// OFFSET: 0x006880D0
UserClientSaveCufProfiles::UserClientSaveCufProfiles() {
}

// OFFSET: 0x00688050/0x140332E80
UserClientSaveCufProfiles::~UserClientSaveCufProfiles() {
}

// OFFSET: 0x006874D0
void UserClientSaveCufProfiles::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x730E));

    JamBitWriter bits(msg);
    bits.WriteBits<20>(this->m_profiles.m_count);

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivateSpec2);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate10Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_shown);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayHealPrediction);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivateSpec1);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivatePvP);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayPowerBar);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate15Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate40Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayPets);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate5Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayOnlyDispellableDebuffs);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate2Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_locked);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayNonBossDebuffs);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayMainTankAndAssist);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayAggroHighlight);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate3Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_displayBorder);
        bits.WriteBit(this->m_profiles.m_data[i].m_useClassColors);
        bits.WriteBit(this->m_profiles.m_data[i].m_dynamicPosition);
        bits.WriteBits<8>(strlen(this->m_profiles.m_data[i].m_name));
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivatePvE);
        bits.WriteBit(this->m_profiles.m_data[i].m_horizontalGroups);
        bits.WriteBit(this->m_profiles.m_data[i].m_autoActivate25Players);
        bits.WriteBit(this->m_profiles.m_data[i].m_keepGroupsTogether);
    }

    bits.Flush();

    for (uint32_t i = 0; i < this->m_profiles.m_count; i++) {
        msg->Put(this->m_profiles.m_data[i].m_topPoint);
        msg->PutData(this->m_profiles.m_data[i].m_name, strlen(this->m_profiles.m_data[i].m_name));
        msg->Put(this->m_profiles.m_data[i].m_bottomOffset);
        msg->Put(this->m_profiles.m_data[i].m_frameHeight);
        msg->Put(this->m_profiles.m_data[i].m_frameWidth);
        msg->Put(this->m_profiles.m_data[i].m_topOffset);
        msg->Put(this->m_profiles.m_data[i].m_healthText);
        msg->Put(this->m_profiles.m_data[i].m_bottomPoint);
        msg->Put(this->m_profiles.m_data[i].m_sortBy);
        msg->Put(this->m_profiles.m_data[i].m_leftOffset);
        msg->Put(this->m_profiles.m_data[i].m_leftPoint);
    }
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

// OFFSET: 0x0069CD50/0x140349280
UserClientChatReportFiltered::~UserClientChatReportFiltered() {
}

// OFFSET: 0x0069CDF0
UserClientChatUnregisterAllAddonPrefixes::~UserClientChatUnregisterAllAddonPrefixes() {
}

// OFFSET: 0x0069CE30/0x140349360
void UserClientChatUnregisterAllAddonPrefixes::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3D54));
}

// OFFSET: 0x0069CEB0/0x1403493D0
UserClientChatReportIgnored::~UserClientChatReportIgnored() {
}

// OFFSET: 0x0069CF00
UserClientChatLeaveChannel::~UserClientChatLeaveChannel() {
}

// OFFSET: 0x0069DC10
UserClientChatReportFiltered::UserClientChatReportFiltered() {
}

// OFFSET: 0x0069DC20/0x14034A2F0
UserClientChatReportIgnored::UserClientChatReportIgnored() {
}

// OFFSET: 0x0069DF40/0x14034A780
void UserClientChatReportIgnored::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0xD54));
    msg->Put(this->m_reason);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_ignoredGUID[2] != 0);
    bits.WriteBit(this->m_ignoredGUID[5] != 0);
    bits.WriteBit(this->m_ignoredGUID[6] != 0);
    bits.WriteBit(this->m_ignoredGUID[4] != 0);
    bits.WriteBit(this->m_ignoredGUID[7] != 0);
    bits.WriteBit(this->m_ignoredGUID[0] != 0);
    bits.WriteBit(this->m_ignoredGUID[1] != 0);
    bits.WriteBit(this->m_ignoredGUID[3] != 0);
    bits.Flush();

    if (this->m_ignoredGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[0] ^ 1));
    }
    if (this->m_ignoredGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[6] ^ 1));
    }
    if (this->m_ignoredGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[5] ^ 1));
    }
    if (this->m_ignoredGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[1] ^ 1));
    }
    if (this->m_ignoredGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[4] ^ 1));
    }
    if (this->m_ignoredGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[3] ^ 1));
    }
    if (this->m_ignoredGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[7] ^ 1));
    }
    if (this->m_ignoredGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_ignoredGUID[2] ^ 1));
    }
}

// OFFSET: 0x0069E460
UserClientChatLeaveChannel::UserClientChatLeaveChannel(uint32_t channelID, const char* channelName) {
    SStrCopy(this->m_channelName, channelName, sizeof(this->m_channelName));
    this->m_channelID = channelID;
}

// OFFSET: 0x0069E850/0x14034B260
void UserClientChatLeaveChannel::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2D56));
    msg->Put(this->m_channelID);

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->m_channelName));
    bits.Flush();

    msg->PutData(this->m_channelName, strlen(this->m_channelName));
}

// OFFSET: 0x0069EA70
UserClientChatUnregisterAllAddonPrefixes::UserClientChatUnregisterAllAddonPrefixes() {
}

// OFFSET: 0x0069EB40/0x14034B5C0
void UserClientChatReportFiltered::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x946));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_filteredGUID[7] != 0);
    bits.WriteBit(this->m_filteredGUID[1] != 0);
    bits.WriteBit(this->m_filteredGUID[5] != 0);
    bits.WriteBit(this->m_filteredGUID[6] != 0);
    bits.WriteBit(this->m_filteredGUID[2] != 0);
    bits.WriteBit(this->m_filteredGUID[3] != 0);
    bits.WriteBit(this->m_filteredGUID[0] != 0);
    bits.WriteBit(this->m_filteredGUID[4] != 0);
    bits.Flush();

    if (this->m_filteredGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[1] ^ 1));
    }
    if (this->m_filteredGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[2] ^ 1));
    }
    if (this->m_filteredGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[7] ^ 1));
    }
    if (this->m_filteredGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[5] ^ 1));
    }
    if (this->m_filteredGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[6] ^ 1));
    }
    if (this->m_filteredGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[0] ^ 1));
    }
    if (this->m_filteredGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[4] ^ 1));
    }
    if (this->m_filteredGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_filteredGUID[3] ^ 1));
    }
}

// OFFSET: 0x0069FD40/0x14034CEB0
void UserClientChatRegisterAddonPrefixes::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x954));

    JamBitWriter bits(msg);
    bits.WriteBits<25>(this->m_prefixes.m_count);

    for (uint32_t i = 0; i < this->m_prefixes.m_count; i++) {
        bits.WriteBits<5>(strlen(this->m_prefixes.m_data[i].m_prefix));
    }

    bits.Flush();

    for (uint32_t i = 0; i < this->m_prefixes.m_count; i++) {
        msg->PutData(this->m_prefixes.m_data[i].m_prefix, strlen(this->m_prefixes.m_data[i].m_prefix));
    }
}

// OFFSET: 0x0069FF50/0x14034D1E0
UserClientChatRegisterAddonPrefixes::UserClientChatRegisterAddonPrefixes() {
}

// OFFSET: 0x0069FF70/0x14034D200
UserClientChatRegisterAddonPrefixes::~UserClientChatRegisterAddonPrefixes() {
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

// OFFSET: 0x00673430/0x140317CE0
PlayerRequestResearchHistory::PlayerRequestResearchHistory() {
}

// OFFSET: 0x006709D0/0x140318AA0
PlayerRequestResearchHistory::~PlayerRequestResearchHistory() {
}

// OFFSET: 0x006704A0
void PlayerRequestResearchHistory::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3306));
}

// OFFSET: 0x0066E710/0x140312490
UserClientSetRole::UserClientSetRole() {
}

// OFFSET: 0x0066BA40/0x140312230
UserClientSetRole::~UserClientSetRole() {
}

// OFFSET: 0x0066E730
void UserClientSetRole::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x25B1));
    msg->Put(this->m_role);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_targetGUID[2] != 0);
    bits.WriteBit(this->m_targetGUID[6] != 0);
    bits.WriteBit(this->m_targetGUID[3] != 0);
    bits.WriteBit(this->m_targetGUID[7] != 0);
    bits.WriteBit(this->m_targetGUID[5] != 0);
    bits.WriteBit(this->m_targetGUID[1] != 0);
    bits.WriteBit(this->m_targetGUID[0] != 0);
    bits.WriteBit(this->m_targetGUID[4] != 0);
    bits.Flush();

    if (this->m_targetGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[6] ^ 1));
    }

    if (this->m_targetGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[4] ^ 1));
    }

    if (this->m_targetGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[1] ^ 1));
    }

    if (this->m_targetGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[3] ^ 1));
    }

    if (this->m_targetGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[0] ^ 1));
    }

    if (this->m_targetGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[5] ^ 1));
    }

    if (this->m_targetGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[2] ^ 1));
    }

    if (this->m_targetGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[7] ^ 1));
    }
}

// OFFSET: 0x0066DB00/0x140314C90
UserClientInitiateRolePoll::UserClientInitiateRolePoll() {
}

// OFFSET: 0x0066BAF0/0x1403122C0
UserClientInitiateRolePoll::~UserClientInitiateRolePoll() {
}

// OFFSET: 0x0066BC40
void UserClientInitiateRolePoll::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x430));
}

// OFFSET: 0x0066D720/0x140314800
UserClientRequestBattlefieldStatus::UserClientRequestBattlefieldStatus() {
}

// OFFSET: 0x0066BA30/0x140312220
UserClientRequestBattlefieldStatus::~UserClientRequestBattlefieldStatus() {
}

// OFFSET: 0x0066BAC0
void UserClientRequestBattlefieldStatus::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2500));
}

// OFFSET: 0x0066DEE0/0x140312320
UserClientGuildAccept::UserClientGuildAccept() {
}

// OFFSET: 0x0066BA70/0x140312260
UserClientGuildAccept::~UserClientGuildAccept() {
}

// OFFSET: 0x0066BB80
void UserClientGuildAccept::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2531));
}

// OFFSET: 0x0066E560/0x1403123E0
UserClientLfgSetRoles::UserClientLfgSetRoles() {
}

// OFFSET: 0x0066B9E0/0x1403121D0
UserClientLfgSetRoles::~UserClientLfgSetRoles() {
}

// OFFSET: 0x0066D0E0
void UserClientLfgSetRoles::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x480));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x0066D920
UserClientLfgLfrJoin::UserClientLfgLfrJoin() {
}

// OFFSET: 0x0066B950
UserClientLfgLfrJoin::~UserClientLfgLfrJoin() {
}

// OFFSET: 0x0066D110
void UserClientLfgLfrJoin::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x531));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x0066D3D0/0x140314460
UserClientArenaTeamCreate::UserClientArenaTeamCreate() {
}

// OFFSET: 0x0066B930
UserClientArenaTeamCreate::~UserClientArenaTeamCreate() {
}

// OFFSET: 0x0066D820
void UserClientArenaTeamCreate::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4A1));
    msg->Put(this->unk_0010);
    msg->Put(this->unk_0084);
    msg->Put(this->unk_0014);
    msg->Put(this->unk_0018);
    msg->Put(this->unk_001C);
    msg->Put(this->unk_0020);

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0024));
    bits.Flush();

    msg->PutData(this->unk_0024, strlen(this->unk_0024));
}

// OFFSET: 0x0066D750/0x140314860
UserClientLfgLfrLeave::UserClientLfgLfrLeave() {
}

// OFFSET: 0x0066BC10/0x140312440
UserClientLfgLfrLeave::~UserClientLfgLfrLeave() {
}

// OFFSET: 0x0066DB10
void UserClientLfgLfrLeave::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x500));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x0066CF50/0x140313F20
UserClientGuildInvite::UserClientGuildInvite() {
}

// OFFSET: 0x0066B9F0/0x1403121E0
UserClientGuildInvite::~UserClientGuildInvite() {
}

// OFFSET: 0x0066E050
void UserClientGuildInvite::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x24B0));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0066DE70/0x140315050
UserClientLfgSetComment::UserClientLfgSetComment() {
}

// OFFSET: 0x0066BB70/0x140312360
UserClientLfgSetComment::~UserClientLfgSetComment() {
}

// OFFSET: 0x0066F2E0
void UserClientLfgSetComment::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x530));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x00670890/0x140318540
UserClientBattlefieldLeave::UserClientBattlefieldLeave() {
}

// OFFSET: 0x00670550/0x140317E80
UserClientBattlefieldLeave::~UserClientBattlefieldLeave() {
}

// OFFSET: 0x006705C0
void UserClientBattlefieldLeave::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3018));
}

// OFFSET: 0x00670D00/0x140318FF0
UserClientBattlemasterJoinArena::UserClientBattlemasterJoinArena(uint8_t a1) {
    this->unk_0010 = a1;
}

// OFFSET: 0x006708C0/0x140318620
UserClientBattlemasterJoinArena::~UserClientBattlemasterJoinArena() {
}

// OFFSET: 0x00670A20
void UserClientBattlemasterJoinArena::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x701C));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x00673470
UserClientBattlefieldList::UserClientBattlefieldList(uint32_t a1) {
    this->unk_0010 = a1;
}

// OFFSET: 0x00670D20
UserClientBattlefieldList::~UserClientBattlefieldList() {
}

// OFFSET: 0x00670E30
void UserClientBattlefieldList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3814));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x00673690/0x140317E10
UserClientBattlemasterJoin::UserClientBattlemasterJoin() {
}

// OFFSET: 0x00670430/0x140317CB0
UserClientBattlemasterJoin::~UserClientBattlemasterJoin() {
}

// OFFSET: 0x006734A0
void UserClientBattlemasterJoin::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7902));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0018[2] != 0);
    bits.WriteBit(this->unk_0018[0] != 0);
    bits.WriteBit(this->unk_0018[3] != 0);
    bits.WriteBit(this->unk_0018[1] != 0);
    bits.WriteBit(this->unk_0018[5] != 0);
    bits.WriteBit(this->unk_0014);
    bits.WriteBit(this->unk_0018[4] != 0);
    bits.WriteBit(this->unk_0018[6] != 0);
    bits.WriteBit(this->unk_0018[7] != 0);
    bits.Flush();

    if (this->unk_0018[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[2] ^ 1));
    }
    if (this->unk_0018[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[6] ^ 1));
    }
    if (this->unk_0018[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[4] ^ 1));
    }
    if (this->unk_0018[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[3] ^ 1));
    }
    if (this->unk_0018[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[7] ^ 1));
    }
    if (this->unk_0018[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[0] ^ 1));
    }
    if (this->unk_0018[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[5] ^ 1));
    }
    if (this->unk_0018[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[1] ^ 1));
    }
}

// OFFSET: 0x0069C160/0x140348660
UserClientGuildDisband::UserClientGuildDisband() {
}

// OFFSET: 0x0069A420/0x140346970
UserClientGuildDisband::~UserClientGuildDisband() {
}

// OFFSET: 0x00699FE0/0x140346560
void UserClientGuildDisband::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3226));
}

// OFFSET: 0x0069C200/0x140348750
UserClientGuildLeave::UserClientGuildLeave() {
}

// OFFSET: 0x0069A1C0/0x140346740
UserClientGuildLeave::~UserClientGuildLeave() {
}

// OFFSET: 0x0069A3B0/0x140346910
void UserClientGuildLeave::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1021));
}

// OFFSET: 0x0069C330/0x1403488A0
UserClientGuildLeader::UserClientGuildLeader(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069A200/0x140346770
UserClientGuildLeader::~UserClientGuildLeader() {
}

// OFFSET: 0x0069A880/0x140346E20
void UserClientGuildLeader::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3034));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069C210/0x140348770
UserClientGuildRemove::UserClientGuildRemove(uint64_t a1) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = a1;
}

// OFFSET: 0x0069A180/0x140346700
UserClientGuildRemove::~UserClientGuildRemove() {
}

// OFFSET: 0x0069AF20/0x140347440
void UserClientGuildRemove::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1231));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.Flush();

    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
}

// OFFSET: 0x0069AB80/0x140347120
UserClientGuildPromote::UserClientGuildPromote(uint64_t a1) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = a1;
}

// OFFSET: 0x0069A110/0x140346680
UserClientGuildPromote::~UserClientGuildPromote() {
}

// OFFSET: 0x0069B070/0x1403475A0
void UserClientGuildPromote::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1030));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.Flush();

    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
}

// OFFSET: 0x0069B730/0x140347C60
UserClientGuildDemote::UserClientGuildDemote(uint64_t a1) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = a1;
}

// OFFSET: 0x0069A2A0/0x140346810
UserClientGuildDemote::~UserClientGuildDemote() {
}

// OFFSET: 0x0069B200/0x140347730
void UserClientGuildDemote::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1020));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.Flush();

    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
}

// OFFSET: 0x0069B8E0/0x140347DE0
UserClientGuildMotd::UserClientGuildMotd(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069A390/0x1403468F0
UserClientGuildMotd::~UserClientGuildMotd() {
}

// OFFSET: 0x0069CBA0/0x140349080
void UserClientGuildMotd::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1035));

    JamBitWriter bits(msg);
    bits.WriteBits<11>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E800/0x14034B210
UserClientChatChannelVoiceOn::UserClientChatChannelVoiceOn(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CF20/0x140349440
UserClientChatChannelVoiceOn::~UserClientChatChannelVoiceOn() {
}

// OFFSET: 0x0069D000/0x1403494E0
void UserClientChatChannelVoiceOn::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1144));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D5F0
UserClientChatChannelKick::UserClientChatChannelKick(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CD70
UserClientChatChannelKick::~UserClientChatChannelKick() {
}

// OFFSET: 0x0069D070/0x140349570
void UserClientChatChannelKick::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3156));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0090, strlen(this->unk_0090));
}

// OFFSET: 0x0069DEF0/0x14034A730
UserClientChatChannelOwner::UserClientChatChannelOwner(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CF80/0x1403494A0
UserClientChatChannelOwner::~UserClientChatChannelOwner() {
}

// OFFSET: 0x0069D130/0x140349650
void UserClientChatChannelOwner::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3D44));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D670
UserClientChatChannelBan::UserClientChatChannelBan(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CE00
UserClientChatChannelBan::~UserClientChatChannelBan() {
}

// OFFSET: 0x0069D1A0/0x1403496E0
void UserClientChatChannelBan::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3D56));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0090, strlen(this->unk_0090));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069ECD0
UserClientChatChannelUnsilenceAll::UserClientChatChannelUnsilenceAll(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CCF0
UserClientChatChannelUnsilenceAll::~UserClientChatChannelUnsilenceAll() {
}

// OFFSET: 0x0069D530/0x140349AB0
void UserClientChatChannelUnsilenceAll::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2546));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0090, strlen(this->unk_0090));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069EED0
UserClientChatChannelSilenceAll::UserClientChatChannelSilenceAll(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CE50
UserClientChatChannelSilenceAll::~UserClientChatChannelSilenceAll() {
}

// OFFSET: 0x0069D6F0/0x140349CB0
void UserClientChatChannelSilenceAll::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2154));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0040, strlen(this->unk_0040));
}

// OFFSET: 0x0069D260/0x1403497C0
UserClientChatChannelDisplayList::UserClientChatChannelDisplayList(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CE10/0x140349340
UserClientChatChannelDisplayList::~UserClientChatChannelDisplayList() {
}

// OFFSET: 0x0069DA40/0x14034A0C0
void UserClientChatChannelDisplayList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2144));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069EC80/0x14034B700
UserClientChatChannelList::UserClientChatChannelList(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CF50/0x140349470
UserClientChatChannelList::~UserClientChatChannelList() {
}

// OFFSET: 0x0069DB20/0x14034A1A0
void UserClientChatChannelList::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1556));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D7F0
UserClientChatChannelUnban::UserClientChatChannelUnban(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CEF0
UserClientChatChannelUnban::~UserClientChatChannelUnban() {
}

// OFFSET: 0x0069DD20/0x14034A440
void UserClientChatChannelUnban::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2D46));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.Flush();

    msg->PutData(this->unk_0040, strlen(this->unk_0040));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D860
UserClientChatChannelInvite::UserClientChatChannelInvite(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CDA0
UserClientChatChannelInvite::~UserClientChatChannelInvite() {
}

// OFFSET: 0x0069DDD0/0x14034A570
void UserClientChatChannelInvite::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x144));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0040, strlen(this->unk_0040));
}

// OFFSET: 0x0069E640
UserClientChatChannelVoiceOff::UserClientChatChannelVoiceOff(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CE80/0x1403493A0
UserClientChatChannelVoiceOff::~UserClientChatChannelVoiceOff() {
}

// OFFSET: 0x0069E0E0/0x14034A920
void UserClientChatChannelVoiceOff::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3144));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069DE80/0x14034A6A0
UserClientChatChannelPassword::UserClientChatChannelPassword(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CFA0/0x1403494C0
UserClientChatChannelPassword::~UserClientChatChannelPassword() {
}

// OFFSET: 0x0069E150/0x14034A9B0
void UserClientChatChannelPassword::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2556));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0040, strlen(this->unk_0040));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E5D0
UserClientChatChannelUnmute::UserClientChatChannelUnmute(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CD80
UserClientChatChannelUnmute::~UserClientChatChannelUnmute() {
}

// OFFSET: 0x0069E2C0/0x14034AB70
void UserClientChatChannelUnmute::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3554));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0040, strlen(this->unk_0040));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E990
UserClientChatChannelUnsilenceVoice::UserClientChatChannelUnsilenceVoice(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0040, a1, sizeof(this->unk_0040));
}

// OFFSET: 0x0069CF30
UserClientChatChannelUnsilenceVoice::~UserClientChatChannelUnsilenceVoice() {
}

// OFFSET: 0x0069E690/0x14034AFB0
void UserClientChatChannelUnsilenceVoice::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3146));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0010));
    bits.WriteBits<8>(strlen(this->unk_0040));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0040, strlen(this->unk_0040));
}

// OFFSET: 0x0069EF40
UserClientChatChannelUnmoderator::UserClientChatChannelUnmoderator(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CD40
UserClientChatChannelUnmoderator::~UserClientChatChannelUnmoderator() {
}

// OFFSET: 0x0069E740/0x14034B0E0
void UserClientChatChannelUnmoderator::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1954));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0090, strlen(this->unk_0090));
}

// OFFSET: 0x0069D9C0
UserClientChatChannelSilenceVoice::UserClientChatChannelSilenceVoice(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CF40
UserClientChatChannelSilenceVoice::~UserClientChatChannelSilenceVoice() {
}

// OFFSET: 0x0069EA80/0x14034B4E0
void UserClientChatChannelSilenceVoice::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2D54));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0090, strlen(this->unk_0090));
}

// OFFSET: 0x0069EFC0
UserClientChatChannelSetOwner::UserClientChatChannelSetOwner(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CE60
UserClientChatChannelSetOwner::~UserClientChatChannelSetOwner() {
}

// OFFSET: 0x0069ED50/0x14034B7E0
void UserClientChatChannelSetOwner::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3556));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0090, strlen(this->unk_0090));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D8D0
UserClientChatChannelMute::UserClientChatChannelMute(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CDD0
UserClientChatChannelMute::~UserClientChatChannelMute() {
}

// OFFSET: 0x0069EE10/0x14034B8C0
void UserClientChatChannelMute::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2554));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0090, strlen(this->unk_0090));
}

// OFFSET: 0x0069D440
UserClientChatChannelModerator::UserClientChatChannelModerator(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0090, a2, sizeof(this->unk_0090));
}

// OFFSET: 0x0069CDE0
UserClientChatChannelModerator::~UserClientChatChannelModerator() {
}

// OFFSET: 0x0069F040/0x14034BB50
void UserClientChatChannelModerator::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x146));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.WriteBits<7>(strlen(this->unk_0090));
    bits.Flush();

    msg->PutData(this->unk_0090, strlen(this->unk_0090));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D320/0x1403498A0
UserClientChatChannelAnnouncements::UserClientChatChannelAnnouncements(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CD00/0x140349230
UserClientChatChannelAnnouncements::~UserClientChatChannelAnnouncements() {
}

// OFFSET: 0x0069F100/0x14034BC30
void UserClientChatChannelAnnouncements::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1146));

    JamBitWriter bits(msg);
    bits.WriteBits<8>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E4D0/0x14034ADD0
UserClientChatJoinChannel::UserClientChatJoinChannel(uint32_t channelID, bool a2, bool a3, const char* channelName, const char* password) {
    this->unk_0010 = a2;
    this->unk_0011 = a3;
    SStrCopy(this->m_channelName, channelName, sizeof(this->m_channelName));
    this->m_channelID = channelID;
    SStrCopy(this->m_password, password, sizeof(this->m_password));
}

// OFFSET: 0x0069CEA0/0x1403493C0
UserClientChatJoinChannel::~UserClientChatJoinChannel() {
}

// OFFSET: 0x0069F170/0x14034BCC0
void UserClientChatJoinChannel::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x156));
    msg->Put(this->m_channelID);

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0011);
    bits.WriteBit(this->unk_0010);
    bits.WriteBits<8>(strlen(this->m_channelName));
    bits.WriteBits<8>(strlen(this->m_password));
    bits.Flush();

    msg->PutData(this->m_channelName, strlen(this->m_channelName));
    msg->PutData(this->m_password, strlen(this->m_password));
}

// OFFSET: 0x0069E200/0x14034AA90
UserClientMessageChatAfk::UserClientMessageChatAfk(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CDB0/0x1403492E0
UserClientMessageChatAfk::~UserClientMessageChatAfk() {
}

// OFFSET: 0x0069F420/0x14034C090
void UserClientMessageChatAfk::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0xD44));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069EA00
UserClientMessageChatAddonRaid::UserClientMessageChatAddonRaid(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0021, a2, sizeof(this->unk_0021));
}

// OFFSET: 0x0069CD30
UserClientMessageChatAddonRaid::~UserClientMessageChatAddonRaid() {
}

// OFFSET: 0x0069F270/0x14034BDE0
void UserClientMessageChatAddonRaid::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1D56));

    JamBitWriter bits(msg);
    bits.WriteBits<5>(strlen(this->unk_0010));
    bits.WriteBits<9>(strlen(this->unk_0021));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0021, strlen(this->unk_0021));
}

// OFFSET: 0x0069D370
UserClientMessageChatRaidWarning::UserClientMessageChatRaidWarning(uint32_t a1, const char* a2) {
    this->unk_0010 = a1;
    SStrCopy(this->unk_0014, a2, sizeof(this->unk_0014));
}

// OFFSET: 0x0069CF10/0x140349430
UserClientMessageChatRaidWarning::~UserClientMessageChatRaidWarning() {
}

// OFFSET: 0x0069F320/0x14034BF50
void UserClientMessageChatRaidWarning::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x944));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0014));
    bits.Flush();

    msg->PutData(this->unk_0014, strlen(this->unk_0014));
}

// OFFSET: 0x0069E8D0
UserClientMessageChatSay::UserClientMessageChatSay(uint32_t a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0110 = a1;
}

// OFFSET: 0x0069CD10
UserClientMessageChatSay::~UserClientMessageChatSay() {
}

// OFFSET: 0x0069F3A0/0x14034BFF0
void UserClientMessageChatSay::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1154));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D3C0
UserClientMessageChatAddonOfficer::UserClientMessageChatAddonOfficer(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    SStrCopy(this->unk_0110, a1, sizeof(this->unk_0110));
}

// OFFSET: 0x0069CEE0
UserClientMessageChatAddonOfficer::~UserClientMessageChatAddonOfficer() {
}

// OFFSET: 0x0069F490/0x14034C120
void UserClientMessageChatAddonOfficer::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3954));

    JamBitWriter bits(msg);
    bits.WriteBits<5>(strlen(this->unk_0110));
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0110, strlen(this->unk_0110));
}

// OFFSET: 0x0069E370
UserClientMessageChatAddonParty::UserClientMessageChatAddonParty(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0021, a2, sizeof(this->unk_0021));
}

// OFFSET: 0x0069CE20
UserClientMessageChatAddonParty::~UserClientMessageChatAddonParty() {
}

// OFFSET: 0x0069F550/0x14034C290
void UserClientMessageChatAddonParty::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x546));

    JamBitWriter bits(msg);
    bits.WriteBits<5>(strlen(this->unk_0010));
    bits.WriteBits<9>(strlen(this->unk_0021));
    bits.Flush();

    msg->PutData(this->unk_0021, strlen(this->unk_0021));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E090
UserClientMessageChatEmote::UserClientMessageChatEmote(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CF70
UserClientMessageChatEmote::~UserClientMessageChatEmote() {
}

// OFFSET: 0x0069F600/0x14034C400
void UserClientMessageChatEmote::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1156));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E560
UserClientMessageChatYell::UserClientMessageChatYell(uint32_t a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0110 = a1;
}

// OFFSET: 0x0069CD20
UserClientMessageChatYell::~UserClientMessageChatYell() {
}

// OFFSET: 0x0069F670/0x14034C490
void UserClientMessageChatYell::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3544));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069DC80/0x14034A360
UserClientMessageChatAddonWhisper::UserClientMessageChatAddonWhisper(const char* a1, const char* a2, const char* a3) {
    SStrCopy(this->unk_0010, a3, sizeof(this->unk_0010));
    SStrCopy(this->unk_0110, a2, sizeof(this->unk_0110));
    SStrCopy(this->unk_0121, a1, sizeof(this->unk_0121));
}

// OFFSET: 0x0069CD90
UserClientMessageChatAddonWhisper::~UserClientMessageChatAddonWhisper() {
}

// OFFSET: 0x0069F6F0/0x14034C530
void UserClientMessageChatAddonWhisper::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2146));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.WriteBits<5>(strlen(this->unk_0110));
    bits.WriteBits<10>(strlen(this->unk_0121));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0110, strlen(this->unk_0110));
    msg->PutData(this->unk_0121, strlen(this->unk_0121));
}

// OFFSET: 0x0069D2B0
UserClientMessageChatAddonGuild::UserClientMessageChatAddonGuild(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0021, a2, sizeof(this->unk_0021));
}

// OFFSET: 0x0069CEC0
UserClientMessageChatAddonGuild::~UserClientMessageChatAddonGuild() {
}

// OFFSET: 0x0069F7F0/0x14034C730
void UserClientMessageChatAddonGuild::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x544));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0021));
    bits.WriteBits<5>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0021, strlen(this->unk_0021));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D4C0
UserClientMessageChatParty::UserClientMessageChatParty(uint32_t a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0110 = a1;
}

// OFFSET: 0x0069CF90
UserClientMessageChatParty::~UserClientMessageChatParty() {
}

// OFFSET: 0x0069F8A0/0x14034C810
void UserClientMessageChatParty::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1D46));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069D7A0
UserClientMessageChatBattleground::UserClientMessageChatBattleground(uint32_t a1, const char* a2) {
    this->unk_0010 = a1;
    SStrCopy(this->unk_0014, a2, sizeof(this->unk_0014));
}

// OFFSET: 0x0069CE70
UserClientMessageChatBattleground::~UserClientMessageChatBattleground() {
}

// OFFSET: 0x0069F920/0x14034C8B0
void UserClientMessageChatBattleground::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2156));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0014));
    bits.Flush();

    msg->PutData(this->unk_0014, strlen(this->unk_0014));
}

// OFFSET: 0x0069E3E0
UserClientMessageChatChannel::UserClientMessageChatChannel(uint32_t a1, const char* a2, const char* a3) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0144 = a1;
    SStrCopy(this->unk_0148, a3, sizeof(this->unk_0148));
}

// OFFSET: 0x0069CCE0
UserClientMessageChatChannel::~UserClientMessageChatChannel() {
}

// OFFSET: 0x0069F9A0/0x14034C950
void UserClientMessageChatChannel::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1D44));
    msg->Put(this->unk_0144);

    JamBitWriter bits(msg);
    bits.WriteBits<10>(strlen(this->unk_0010));
    bits.WriteBits<9>(strlen(this->unk_0148));
    bits.Flush();

    msg->PutData(this->unk_0148, strlen(this->unk_0148));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E250
UserClientMessageChatAddonBattleground::UserClientMessageChatAddonBattleground(const char* a1, const char* a2) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
    SStrCopy(this->unk_0021, a2, sizeof(this->unk_0021));
}

// OFFSET: 0x0069CDC0
UserClientMessageChatAddonBattleground::~UserClientMessageChatAddonBattleground() {
}

// OFFSET: 0x0069FA60/0x14034CA60
void UserClientMessageChatAddonBattleground::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0xD46));

    JamBitWriter bits(msg);
    bits.WriteBits<5>(strlen(this->unk_0010));
    bits.WriteBits<9>(strlen(this->unk_0021));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
    msg->PutData(this->unk_0021, strlen(this->unk_0021));
}

// OFFSET: 0x0069DC30/0x14034A310
UserClientMessageChatDnd::UserClientMessageChatDnd(const char* a1) {
    SStrCopy(this->unk_0010, a1, sizeof(this->unk_0010));
}

// OFFSET: 0x0069CE90
UserClientMessageChatDnd::~UserClientMessageChatDnd() {
}

// OFFSET: 0x0069FB10/0x14034CBD0
void UserClientMessageChatDnd::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2946));

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069DB90
UserClientMessageChatWhisper::UserClientMessageChatWhisper(uint32_t a1, const char* a2, const char* a3) {
    SStrCopy(this->unk_0010, a3, sizeof(this->unk_0010));
    this->unk_0110 = a1;
    SStrCopy(this->unk_0114, a2, sizeof(this->unk_0114));
}

// OFFSET: 0x0069CED0
UserClientMessageChatWhisper::~UserClientMessageChatWhisper() {
}

// OFFSET: 0x0069FB80/0x14034CC60
void UserClientMessageChatWhisper::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0xD56));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<10>(strlen(this->unk_0114));
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0114, strlen(this->unk_0114));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069E940
UserClientMessageChatGuild::UserClientMessageChatGuild(uint32_t a1, const char* a2) {
    this->unk_0010 = a1;
    SStrCopy(this->unk_0014, a2, sizeof(this->unk_0014));
}

// OFFSET: 0x0069CD60
UserClientMessageChatGuild::~UserClientMessageChatGuild() {
}

// OFFSET: 0x0069FC40/0x14034CD70
void UserClientMessageChatGuild::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3956));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0014));
    bits.Flush();

    msg->PutData(this->unk_0014, strlen(this->unk_0014));
}

// OFFSET: 0x0069D950
UserClientMessageChatRaid::UserClientMessageChatRaid(uint32_t a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0110 = a1;
}

// OFFSET: 0x0069CF60
UserClientMessageChatRaid::~UserClientMessageChatRaid() {
}

// OFFSET: 0x0069FCC0/0x14034CE10
void UserClientMessageChatRaid::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2D44));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0069DAB0
UserClientMessageChatOfficer::UserClientMessageChatOfficer(uint32_t a1, const char* a2) {
    SStrCopy(this->unk_0010, a2, sizeof(this->unk_0010));
    this->unk_0110 = a1;
}

// OFFSET: 0x0069CFB0/0x1403494D0
UserClientMessageChatOfficer::~UserClientMessageChatOfficer() {
}

// OFFSET: 0x0069FE00/0x14034D020
void UserClientMessageChatOfficer::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1946));
    msg->Put(this->unk_0110);

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0066D170/0x1403141A0
UserClientRequestRatedBgInfo::UserClientRequestRatedBgInfo() {
}

// OFFSET: 0x0066BBC0/0x140312400
UserClientRequestRatedBgInfo::~UserClientRequestRatedBgInfo() {
}

// OFFSET: 0x0066B900
void UserClientRequestRatedBgInfo::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x5B3));
}

// OFFSET: 0x0066E120/0x140312380
UserClientGetPVPOptionsEnabled::UserClientGetPVPOptionsEnabled() {
}

// OFFSET: 0x0066BB40/0x140312300
UserClientGetPVPOptionsEnabled::~UserClientGetPVPOptionsEnabled() {
}

// OFFSET: 0x0066BA80
void UserClientGetPVPOptionsEnabled::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x24A1));
}

// OFFSET: 0x0066D390/0x140314400
UserClientLfgLockInfoRequest::UserClientLfgLockInfoRequest() {
}

// OFFSET: 0x0066BAA0/0x140312280
UserClientLfgLockInfoRequest::~UserClientLfgLockInfoRequest() {
}

// OFFSET: 0x0066D180
void UserClientLfgLockInfoRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x412));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010);
    bits.Flush();
}

// OFFSET: 0x0066D690/0x140314750
UserClientLfgSetBootVote::UserClientLfgSetBootVote() {
}

// OFFSET: 0x0066BA10/0x140312200
UserClientLfgSetBootVote::~UserClientLfgSetBootVote() {
}

// OFFSET: 0x0066D300
void UserClientLfgSetBootVote::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4B3));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010);
    bits.Flush();
}

// OFFSET: 0x0066D730/0x140314820
UserClientStartWarGame::UserClientStartWarGame() {
}

// OFFSET: 0x0066BB00/0x1403122D0
UserClientStartWarGame::~UserClientStartWarGame() {
}

// OFFSET: 0x0066D440
void UserClientStartWarGame::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x5A0));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0018[0] != 0);
    bits.WriteBit(this->unk_0018[7] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0018[5] != 0);
    bits.WriteBit(this->unk_0018[1] != 0);
    bits.WriteBit(this->unk_0018[2] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0018[4] != 0);
    bits.WriteBit(this->unk_0018[3] != 0);
    bits.WriteBit(this->unk_0018[6] != 0);
    bits.Flush();

    if (this->unk_0018[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[6] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0018[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[4] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0018[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[1] ^ 1));
    }
    if (this->unk_0018[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[3] ^ 1));
    }
    if (this->unk_0018[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[5] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0018[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[2] ^ 1));
    }
    if (this->unk_0018[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[7] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0018[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[0] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
}

// OFFSET: 0x0066E8B0/0x140312500
UserClientLfgTeleport::UserClientLfgTeleport() {
}

// OFFSET: 0x0066BA00/0x1403121F0
UserClientLfgTeleport::~UserClientLfgTeleport() {
}

// OFFSET: 0x0066D6B0
void UserClientLfgTeleport::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2482));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010);
    bits.Flush();
}

// OFFSET: 0x0066D380/0x1403143E0
UserClientCommentatorGetPartyInfo::UserClientCommentatorGetPartyInfo() {
}

// OFFSET: 0x0066BC30/0x140312470
UserClientCommentatorGetPartyInfo::~UserClientCommentatorGetPartyInfo() {
}

// OFFSET: 0x0066D7A0
void UserClientCommentatorGetPartyInfo::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2412));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0014));
    bits.Flush();

    msg->PutData(this->unk_0014, strlen(this->unk_0014));
}

// OFFSET: 0x0066D740/0x140314840
UserClientCommentatorStartWargame::UserClientCommentatorStartWargame() {
}

// OFFSET: 0x0066B920
UserClientCommentatorStartWargame::~UserClientCommentatorStartWargame() {
}

// OFFSET: 0x0066DB60
void UserClientCommentatorStartWargame::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x25A0));

    JamBitWriter bits(msg);
    bits.WriteBits<7>(strlen(this->unk_0048));
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBits<7>(strlen(this->unk_0018));
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.Flush();

    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    msg->PutData(this->unk_0018, strlen(this->unk_0018));
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    msg->PutData(this->unk_0048, strlen(this->unk_0048));
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
}

// OFFSET: 0x0066DE30/0x140315010
UserClientRequestRatedInfo::UserClientRequestRatedInfo() {
}

// OFFSET: 0x0066B970/0x140312170
UserClientRequestRatedInfo::~UserClientRequestRatedInfo() {
}

// OFFSET: 0x0066DE00
void UserClientRequestRatedInfo::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2423));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x0066D6A0/0x140314770
UserClientSendSorRequestViaAddress::UserClientSendSorRequestViaAddress() {
}

// OFFSET: 0x0066BBB0/0x1403123B0
UserClientSendSorRequestViaAddress::~UserClientSendSorRequestViaAddress() {
}

// OFFSET: 0x0066F8D0
void UserClientSendSorRequestViaAddress::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x420));

    JamBitWriter bits(msg);
    bits.WriteBits<12>(strlen(this->unk_0010));
    bits.WriteBits<11>(strlen(this->unk_0510));
    bits.Flush();

    msg->PutData(this->unk_0510, strlen(this->unk_0510));
    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x0066D160
UserClientSendSorRequestViaBnetAccountId::UserClientSendSorRequestViaBnetAccountId() {
}

// OFFSET: 0x0066BA50/0x140312240
UserClientSendSorRequestViaBnetAccountId::~UserClientSendSorRequestViaBnetAccountId() {
}

// OFFSET: 0x0066FBA0
void UserClientSendSorRequestViaBnetAccountId::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x482));
    msg->Put(this->unk_040C);

    JamBitWriter bits(msg);
    bits.WriteBits<11>(strlen(this->unk_0010));
    bits.Flush();

    msg->PutData(this->unk_0010, strlen(this->unk_0010));
}

// OFFSET: 0x00670590/0x140317EE0
UserClientRequestPvpRewards::UserClientRequestPvpRewards() {
}

// OFFSET: 0x00670440/0x140317D20
UserClientRequestPvpRewards::~UserClientRequestPvpRewards() {
}

// OFFSET: 0x006707D0
void UserClientRequestPvpRewards::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x780C));
}

// OFFSET: 0x00670670/0x140318030
UserClientBattlemasterJoinRated::UserClientBattlemasterJoinRated() {
}

// OFFSET: 0x006707F0/0x1403182E0
UserClientBattlemasterJoinRated::~UserClientBattlemasterJoinRated() {
}

// OFFSET: 0x00670D70
void UserClientBattlemasterJoinRated::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3B18));
}

// OFFSET: 0x0069A8F0
UserClientGuildAchievementMembers::UserClientGuildAchievementMembers(uint64_t a1, uint64_t a3, uint32_t a5) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = a3;
    *reinterpret_cast<uint64_t*>(this->unk_0018) = a1;
    this->unk_0020 = a5;
}

// OFFSET: 0x0069A190/0x140346710
UserClientGuildAchievementMembers::~UserClientGuildAchievementMembers() {
}

// OFFSET: 0x0069A930/0x140346EE0
void UserClientGuildAchievementMembers::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3025));
    msg->Put(this->unk_0020);

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0018[5] != 0);
    bits.WriteBit(this->unk_0018[4] != 0);
    bits.WriteBit(this->unk_0018[0] != 0);
    bits.WriteBit(this->unk_0018[7] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0018[3] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0018[2] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0018[6] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0018[1] != 0);
    bits.Flush();

    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0018[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[2] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0018[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[5] ^ 1));
    }
    if (this->unk_0018[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[0] ^ 1));
    }
    if (this->unk_0018[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[3] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0018[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[4] ^ 1));
    }
    if (this->unk_0018[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[6] ^ 1));
    }
    if (this->unk_0018[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[1] ^ 1));
    }
    if (this->unk_0018[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[7] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
}

// OFFSET: 0x0066D340/0x1403143A0
UserClientBFMgrQueueInviteResponse::UserClientBFMgrQueueInviteResponse(uint64_t queueID, bool accept) {
    this->unk_0010 = accept;
    *reinterpret_cast<uint64_t*>(this->unk_0018) = queueID;
}

// OFFSET: 0x0066BAE0/0x1403122B0
UserClientBFMgrQueueInviteResponse::~UserClientBFMgrQueueInviteResponse() {
}

// OFFSET: 0x0066D1C0
void UserClientBFMgrQueueInviteResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x413));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0018[2] != 0);
    bits.WriteBit(this->unk_0018[0] != 0);
    bits.WriteBit(this->unk_0018[4] != 0);
    bits.WriteBit(this->unk_0018[3] != 0);
    bits.WriteBit(this->unk_0018[5] != 0);
    bits.WriteBit(this->unk_0018[7] != 0);
    bits.WriteBit(this->unk_0010);
    bits.WriteBit(this->unk_0018[1] != 0);
    bits.WriteBit(this->unk_0018[6] != 0);
    bits.Flush();

    if (this->unk_0018[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[1] ^ 1));
    }
    if (this->unk_0018[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[3] ^ 1));
    }
    if (this->unk_0018[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[2] ^ 1));
    }
    if (this->unk_0018[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[4] ^ 1));
    }
    if (this->unk_0018[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[6] ^ 1));
    }
    if (this->unk_0018[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[7] ^ 1));
    }
    if (this->unk_0018[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[0] ^ 1));
    }
    if (this->unk_0018[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0018[5] ^ 1));
    }
}

// OFFSET: 0x0066E880
UserClientBFMgrQueueExitRequest::UserClientBFMgrQueueExitRequest(uint64_t queueID) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = queueID;
}

// OFFSET: 0x0066B980
UserClientBFMgrQueueExitRequest::~UserClientBFMgrQueueExitRequest() {
}

// OFFSET: 0x0066D950
void UserClientBFMgrQueueExitRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2490));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.Flush();

    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
}

// OFFSET: 0x0066E4C0/0x1403123C0
UserClientBFMgrEntryInviteResponse::UserClientBFMgrEntryInviteResponse() {
}

// OFFSET: 0x0066BA60/0x140312250
UserClientBFMgrEntryInviteResponse::~UserClientBFMgrEntryInviteResponse() {
}

// OFFSET: 0x0066E570
void UserClientBFMgrEntryInviteResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x5A3));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0018);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.Flush();

    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
}

// OFFSET: 0x006705A0/0x140317F00
UserClientRequestPvpLogData::UserClientRequestPvpLogData() {
}

// OFFSET: 0x006706E0/0x1403180B0
UserClientRequestPvpLogData::~UserClientRequestPvpLogData() {
}

// OFFSET: 0x00670480
void UserClientRequestPvpLogData::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7308));
}

// OFFSET: 0x00670630/0x140317FB0
UserClientRequestBattlefieldPositions::UserClientRequestBattlefieldPositions() {
}

// OFFSET: 0x00670390/0x140317BC0
UserClientRequestBattlefieldPositions::~UserClientRequestBattlefieldPositions() {
}

// OFFSET: 0x006707B0
void UserClientRequestBattlefieldPositions::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3902));
}

// OFFSET: 0x00674960
UserClientBFMgrQueueRequest::UserClientBFMgrQueueRequest(uint64_t queueID) {
    *reinterpret_cast<uint64_t*>(this->unk_0010) = queueID;
}

// OFFSET: 0x00670E80
UserClientBFMgrQueueRequest::~UserClientBFMgrQueueRequest() {
}

// OFFSET: 0x00674110
void UserClientBFMgrQueueRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x710C));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.Flush();

    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
}

// OFFSET: 0x0066E110/0x140312340
UserClientOpcode2581::UserClientOpcode2581() {
}

// OFFSET: 0x0066B940/0x140312140
UserClientOpcode2581::~UserClientOpcode2581() {
}

// OFFSET: 0x0066BBE0
void UserClientOpcode2581::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2581));
}

// OFFSET: 0x0066DAF0/0x140314C70
UserClientOpcode2422::UserClientOpcode2422() {
}

// OFFSET: 0x0066B9C0/0x1403121B0
UserClientOpcode2422::~UserClientOpcode2422() {
}

// OFFSET: 0x0066E0C0
void UserClientOpcode2422::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2422));
    msg->Put(this->unk_0010);

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0014);
    bits.Flush();
}

// OFFSET: 0x00673180/0x140317C40
UserClientOpcode7912::UserClientOpcode7912() {
}

// OFFSET: 0x006706A0/0x140318080
UserClientOpcode7912::~UserClientOpcode7912() {
}

// OFFSET: 0x006709A0
void UserClientOpcode7912::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7912));
}

// OFFSET: 0x006708F0/0x140318770
UserClientOpcode791E::UserClientOpcode791E() {
}

// OFFSET: 0x00670420/0x140317CA0
UserClientOpcode791E::~UserClientOpcode791E() {
}

// OFFSET: 0x006731E0
void UserClientOpcode791E::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x791E));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.Flush();

    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
}

// OFFSET: 0x00670830/0x140318370
UserClientOpcode7816::UserClientOpcode7816() {
}

// OFFSET: 0x00670680/0x140318050
UserClientOpcode7816::~UserClientOpcode7816() {
}

// OFFSET: 0x006733A0
void UserClientOpcode7816::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7816));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x00670660/0x140318010
UserClientOpcode3010::UserClientOpcode3010() {
}

// OFFSET: 0x006706D0/0x1403180A0
UserClientOpcode3010::~UserClientOpcode3010() {
}

// OFFSET: 0x006744E0
void UserClientOpcode3010::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3010));

    JamBitWriter bits(msg);
    bits.WriteBit(this->unk_0010[1] != 0);
    bits.WriteBit(this->unk_0010[4] != 0);
    bits.WriteBit(this->unk_0010[6] != 0);
    bits.WriteBit(this->unk_0010[5] != 0);
    bits.WriteBit(this->unk_0010[0] != 0);
    bits.WriteBit(this->unk_0010[2] != 0);
    bits.WriteBit(this->unk_0010[7] != 0);
    bits.WriteBit(this->unk_0010[3] != 0);
    bits.Flush();

    if (this->unk_0010[4]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[4] ^ 1));
    }
    if (this->unk_0010[7]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[7] ^ 1));
    }
    if (this->unk_0010[2]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[2] ^ 1));
    }
    if (this->unk_0010[5]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[5] ^ 1));
    }
    if (this->unk_0010[6]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[6] ^ 1));
    }
    if (this->unk_0010[3]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[3] ^ 1));
    }
    if (this->unk_0010[0]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[0] ^ 1));
    }
    if (this->unk_0010[1]) {
        msg->Put(static_cast<uint8_t>(this->unk_0010[1] ^ 1));
    }
}

// OFFSET: 0x0069C180
UserClientOpcode3235::UserClientOpcode3235() {
}

// OFFSET: 0x0069A320
UserClientOpcode3235::~UserClientOpcode3235() {
}

// OFFSET: 0x0069A0A0/0x140346600
void UserClientOpcode3235::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3235));
    msg->Put(this->unk_0010);
}

// OFFSET: 0x0069C110/0x1403485F0
UserClientOpcode3231::UserClientOpcode3231() {
}

// OFFSET: 0x0069A120/0x140346690
UserClientOpcode3231::~UserClientOpcode3231() {
}

// OFFSET: 0x0069A4A0/0x140346A50
void UserClientOpcode3231::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3231));
}

// OFFSET: 0x006702B0/0x140317410
UserClientLfgJoin::UserClientLfgJoin() {
}

// OFFSET: 0x006701E0/0x1403172E0
UserClientLfgJoin::~UserClientLfgJoin() {
}

// OFFSET: 0x0066FA30
void UserClientLfgJoin::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2430));
    msg->Put(this->m_roles);

    for (uint32_t i = 0; i < 3; i++) {
        msg->Put(this->m_needs[i]);
    }

    JamBitWriter bits(msg);
    bits.WriteBits<9>(strlen(this->m_comment));
    bits.WriteBits<24>(this->m_slots.m_count);
    bits.Flush();

    msg->PutData(this->m_comment, strlen(this->m_comment));

    for (uint32_t i = 0; i < this->m_slots.m_count; i++) {
        msg->Put(this->m_slots.m_data[i]);
    }
}

// OFFSET: 0x0066DB40/0x140314CF0
UserClientLfgLeave::UserClientLfgLeave() {
}

// OFFSET: 0x0066E700/0x140315930
UserClientLfgLeave::~UserClientLfgLeave() {
}

// OFFSET: 0x0066E6D0
void UserClientLfgLeave::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2433));
    msg->Put(this->unk_0028);
    this->m_ticket.Put(msg);
}

// OFFSET: 0x0066E6B0
UserClientLfgProposalResponse::UserClientLfgProposalResponse() {
}

// OFFSET: 0x0066D940
UserClientLfgProposalResponse::~UserClientLfgProposalResponse() {
}

// OFFSET: 0x0066E130
void UserClientLfgProposalResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x403));
    msg->Put(this->m_proposalID);
    this->m_ticket.Put(msg);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_instanceGuid[7] != 0);
    bits.WriteBit(this->m_accept);
    bits.WriteBit(this->m_instanceGuid[1] != 0);
    bits.WriteBit(this->m_instanceGuid[3] != 0);
    bits.WriteBit(this->m_instanceGuid[0] != 0);
    bits.WriteBit(this->m_instanceGuid[5] != 0);
    bits.WriteBit(this->m_instanceGuid[4] != 0);
    bits.WriteBit(this->m_instanceGuid[6] != 0);
    bits.WriteBit(this->m_instanceGuid[2] != 0);
    bits.Flush();

    if (this->m_instanceGuid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[7] ^ 1));
    }
    if (this->m_instanceGuid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[1] ^ 1));
    }
    if (this->m_instanceGuid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[5] ^ 1));
    }
    if (this->m_instanceGuid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[6] ^ 1));
    }
    if (this->m_instanceGuid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[3] ^ 1));
    }
    if (this->m_instanceGuid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[4] ^ 1));
    }
    if (this->m_instanceGuid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[0] ^ 1));
    }
    if (this->m_instanceGuid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_instanceGuid[2] ^ 1));
    }
}

// OFFSET: 0x0066DA90
UserClientWorldTeleport::UserClientWorldTeleport(uint32_t mapID, uint64_t transportGUID, const C3Vector& position, float facing) {
    memcpy(this->m_transportGUID, &transportGUID, sizeof(this->m_transportGUID));
    this->m_mapID = mapID;
    this->m_facing = facing;
    this->m_position = position;
}

// OFFSET: 0x0066B990
UserClientWorldTeleport::~UserClientWorldTeleport() {
}

// OFFSET: 0x0066CF60
void UserClientWorldTeleport::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x24B2));
    msg->Put(this->m_mapID);
    msg->Put(this->m_facing);
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_transportGUID[6] != 0);
    bits.WriteBit(this->m_transportGUID[0] != 0);
    bits.WriteBit(this->m_transportGUID[5] != 0);
    bits.WriteBit(this->m_transportGUID[4] != 0);
    bits.WriteBit(this->m_transportGUID[3] != 0);
    bits.WriteBit(this->m_transportGUID[7] != 0);
    bits.WriteBit(this->m_transportGUID[1] != 0);
    bits.WriteBit(this->m_transportGUID[2] != 0);
    bits.Flush();

    if (this->m_transportGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[1] ^ 1));
    }
    if (this->m_transportGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[0] ^ 1));
    }
    if (this->m_transportGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[2] ^ 1));
    }
    if (this->m_transportGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[7] ^ 1));
    }
    if (this->m_transportGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[4] ^ 1));
    }
    if (this->m_transportGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[5] ^ 1));
    }
    if (this->m_transportGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[6] ^ 1));
    }
    if (this->m_transportGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_transportGUID[3] ^ 1));
    }
}

// OFFSET: 0x0066D3E0
UserClientSupportTicketSubmitBug::UserClientSupportTicketSubmitBug() {
}

// OFFSET: 0x0066D930
UserClientSupportTicketSubmitBug::~UserClientSupportTicketSubmitBug() {
}

// OFFSET: 0x0066FB20
void UserClientSupportTicketSubmitBug::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2520));

    JamBitWriter bits(msg);
    bits.WriteBits<12>(strlen(this->m_note), JamBitCount<true>());
    bits.Flush();

    msg->PutData(this->m_note, strlen(this->m_note));
    this->m_header.Put(msg);
}

// OFFSET: 0x0066DEC0
UserClientSupportTicketSubmitSuggestion::UserClientSupportTicketSubmitSuggestion() {
}

// OFFSET: 0x0066E030
UserClientSupportTicketSubmitSuggestion::~UserClientSupportTicketSubmitSuggestion() {
}

// OFFSET: 0x0066F990
void UserClientSupportTicketSubmitSuggestion::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2512));

    JamBitWriter bits(msg);
    bits.WriteBits<12>(strlen(this->m_note), JamBitCount<true>());
    bits.Flush();

    msg->PutData(this->m_note, strlen(this->m_note));
    this->m_header.Put(msg);
}

// OFFSET: 0x0066FEF0
UserClientWardenData::UserClientWardenData() {
}

// OFFSET: 0x0066FF50
UserClientWardenData::~UserClientWardenData() {
}

// OFFSET: 0x0066D400
void UserClientWardenData::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x25A2));
    msg->Put(this->m_data.m_size);
    msg->PutData(this->m_data.m_data, this->m_data.m_size);
}
