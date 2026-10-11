#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 37ba38cd IDENTICAL/NONE
// OFFSET: 0x0069C520
JamGuildMemberSetNoteData::JamGuildMemberSetNoteData(uint64_t memberGUID, const char* note, bool isPublic) {
    memcpy(this->m_memberGUID, &memberGUID, sizeof(this->m_memberGUID));
    SStrCopy(this->m_note, note, sizeof(this->m_note));
    this->m_isPublic = isPublic;
}

// OFFSET: 0x0069AE90
JamGuildMemberSetNoteData::JamGuildMemberSetNoteData(const JamGuildMemberSetNoteData& other) {
    this->m_isPublic = other.m_isPublic;
    SStrCopy(this->m_note, other.m_note, sizeof(this->m_note));
    memcpy(this->m_memberGUID, other.m_memberGUID, sizeof(this->m_memberGUID));
}

// INLINED
JamGuildMemberSetNoteData::~JamGuildMemberSetNoteData() {
}

// PROVEN: 14124de6 IDENTICAL/NONE
// OFFSET: 0x0069A510/0x140346AB0
GlobalGuildEventLogQuery::GlobalGuildEventLogQuery() {
}

// PROVEN: 08413e6e IDENTICAL/NONE
// OFFSET: 0x0069A000/0x140346570
GlobalGuildEventLogQuery::~GlobalGuildEventLogQuery() {
}

// PROVEN: 2aa9b7a7 IDENTICAL/NONE
// OFFSET: 0x0069A240/0x1403467B0
void GlobalGuildEventLogQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1220));
}

// PROVEN: ca20a65f IDENTICAL/NONE
// OFFSET: 0x0069A840/0x140346DC0
GlobalGuildBankRemainingWithdrawMoneyQuery::GlobalGuildBankRemainingWithdrawMoneyQuery() {
}

// PROVEN: a82a1275 IDENTICAL/NONE
// OFFSET: 0x0069A030/0x140346590
GlobalGuildBankRemainingWithdrawMoneyQuery::~GlobalGuildBankRemainingWithdrawMoneyQuery() {
}

// PROVEN: 290b3956 IDENTICAL/NONE
// OFFSET: 0x0069A470/0x1403469F0
void GlobalGuildBankRemainingWithdrawMoneyQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1225));
}

// PROVEN: c247af2e IDENTICAL/NONE
// OFFSET: 0x0069C810/0x140346A30
GlobalGuildGetRoster::GlobalGuildGetRoster() {
}

// PROVEN: cc3fd3c6 IDENTICAL/NONE
// OFFSET: 0x0069A040/0x1403465A0
GlobalGuildGetRoster::~GlobalGuildGetRoster() {
}

// PROVEN: f0713d13 IDENTICAL/NONE
// OFFSET: 0x0069A590/0x140346B20
void GlobalGuildGetRoster::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1226));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_playerGUID[2] != 0);
    bits.WriteBit(this->m_playerGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_playerGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_playerGUID[6] != 0);
    bits.WriteBit(this->m_playerGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_playerGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_playerGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_playerGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.Flush();

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_playerGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[4] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_playerGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[5] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_playerGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[0] ^ 1));
    }

    if (this->m_playerGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[6] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_playerGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[7] ^ 1));
    }

    if (this->m_playerGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[2] ^ 1));
    }

    if (this->m_playerGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[3] ^ 1));
    }

    if (this->m_playerGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_playerGUID[1] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }
}

// PROVEN: a3b84014 IDENTICAL/NONE
// OFFSET: 0x0069C2C0/0x140348830
GlobalGuildSetRankPermissions::GlobalGuildSetRankPermissions() {
}

// PROVEN: 4469f842 IDENTICAL/NONE
// OFFSET: 0x0069A0D0/0x140346640
GlobalGuildSetRankPermissions::~GlobalGuildSetRankPermissions() {
}

// OFFSET: 0x0069C710/0x140348C10
void GlobalGuildSetRankPermissions::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1024));
    msg->Put(static_cast<uint32_t>(this->m_rankID));
    msg->Put(this->m_oldFlags);
    msg->Put(this->m_newFlags);

    for (uint32_t i = 0; i < 8; i++) {
        msg->Put(this->m_tabFlags[i]);
        msg->Put(static_cast<uint32_t>(this->m_tabWithdrawItemLimit[i]));
    }

    msg->Put(static_cast<uint32_t>(this->m_withdrawGoldLimit));
    msg->Put(static_cast<uint32_t>(this->m_rankOrder));

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_rankName);
    bits.WriteBits<7>(length);
    bits.Flush();
    msg->PutData(this->m_rankName, strlen(this->m_rankName));
}

// PROVEN: cf5339a7 IDENTICAL/NONE
// OFFSET: 0x0069C120
GlobalGuildNewsUpdateSticky::GlobalGuildNewsUpdateSticky(uint64_t guildGUID, int32_t newsID, bool sticky) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
    this->m_sticky = sticky;
    this->m_newsID = newsID;
}

// PROVEN: cc71a7e3 IDENTICAL/NONE
// OFFSET: 0x0069A0E0
GlobalGuildNewsUpdateSticky::~GlobalGuildNewsUpdateSticky() {
}

// PROVEN: 83b2d7f1 IDENTICAL/NONE
// OFFSET: 0x0069B790/0x140347CA0
void GlobalGuildNewsUpdateSticky::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3223));
    msg->Put(static_cast<uint32_t>(this->m_newsID));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_sticky);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.Flush();

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }
}

// PROVEN: cf08e5f4 IDENTICAL/NONE
// OFFSET: 0x0069A860
GlobalGuildBankTextQuery::GlobalGuildBankTextQuery(uint32_t tab) {
    this->m_tab = tab;
}

// PROVEN: 8c699fbf IDENTICAL/NONE
// OFFSET: 0x0069A0F0
GlobalGuildBankTextQuery::~GlobalGuildBankTextQuery() {
}

// PROVEN: ad27b5ec IDENTICAL/NONE
// OFFSET: 0x0069A070/0x1403465C0
void GlobalGuildBankTextQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3220));
    msg->Put(this->m_tab);
}

// PROVEN: 3e1317c7 IDENTICAL/NONE
// OFFSET: 0x0069B1B0/0x1403476E0
GlobalGuildChangeNameRequest::GlobalGuildChangeNameRequest() {
}

// PROVEN: b0b5848b IDENTICAL/NONE
// OFFSET: 0x0069A170/0x1403466F0
GlobalGuildChangeNameRequest::~GlobalGuildChangeNameRequest() {
}

// OFFSET: 0x0069C190/0x1403486C0
void GlobalGuildChangeNameRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1232));

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_newName);
    bits.WriteBits<8>(length);
    bits.Flush();
    msg->PutData(this->m_newName, strlen(this->m_newName));
}

// PROVEN: 5d3a9593 IDENTICAL/NONE
// OFFSET: 0x0069C6D0
GlobalGuildAutoDeclineInvitation::GlobalGuildAutoDeclineInvitation() {
}

// PROVEN: 26aed351 IDENTICAL/NONE
// OFFSET: 0x0069A1A0
GlobalGuildAutoDeclineInvitation::~GlobalGuildAutoDeclineInvitation() {
}

// PROVEN: 6fd6aa4a IDENTICAL/NONE
// OFFSET: 0x0069A340/0x1403468B0
void GlobalGuildAutoDeclineInvitation::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1234));
}

// PROVEN: 0d39af72 IDENTICAL/NONE
// OFFSET: 0x0069AEF0
GlobalGuildShiftRank::GlobalGuildShiftRank(int32_t rankOrder, bool shiftUp) {
    this->m_shiftUp = shiftUp;
    this->m_rankOrder = rankOrder;
}

// PROVEN: d7e2bdf8 IDENTICAL/NONE
// OFFSET: 0x0069A1B0
GlobalGuildShiftRank::~GlobalGuildShiftRank() {
}

// PROVEN: c9c620e3 IDENTICAL/NONE
// OFFSET: 0x0069A7F0/0x140346D70
void GlobalGuildShiftRank::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1221));
    msg->Put(static_cast<uint32_t>(this->m_rankOrder));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_shiftUp);
    bits.Flush();
}

// PROVEN: 09704f78 IDENTICAL/NONE
// OFFSET: 0x0069A4C0/0x140346A60
GlobalGuildUpdateInfoText::GlobalGuildUpdateInfoText(const char* infoText) {
    SStrCopy(this->m_infoText, infoText, sizeof(this->m_infoText));
}

// PROVEN: 0bac6932 IDENTICAL/NONE
// OFFSET: 0x0069A1D0/0x140346750
GlobalGuildUpdateInfoText::~GlobalGuildUpdateInfoText() {
}

// OFFSET: 0x0069CB00/0x140348F90
void GlobalGuildUpdateInfoText::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3227));

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_infoText);
    bits.WriteBits<12>(length);
    bits.Flush();
    msg->PutData(this->m_infoText, strlen(this->m_infoText));
}

// PROVEN: cb6d2f6f IDENTICAL/NONE
// OFFSET: 0x0069C7E0/0x140346A00
GlobalGuildQueryRecipes::GlobalGuildQueryRecipes(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: 8adefe71 IDENTICAL/NONE
// OFFSET: 0x0069A210/0x140346780
GlobalGuildQueryRecipes::~GlobalGuildQueryRecipes() {
}

// PROVEN: e77f3f7e IDENTICAL/NONE
// OFFSET: 0x0069BBC0/0x1403480C0
void GlobalGuildQueryRecipes::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3033));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.Flush();

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }
}

// PROVEN: 2435c64a IDENTICAL/NONE
// OFFSET: 0x0069ABE0
GlobalGuildBankLogQuery::GlobalGuildBankLogQuery(uint32_t tab) {
    this->m_tab = tab;
}

// PROVEN: 8c65ac24 IDENTICAL/NONE
// OFFSET: 0x0069A220/0x140346790
GlobalGuildBankLogQuery::~GlobalGuildBankLogQuery() {
}

// PROVEN: 2f3c1aa0 IDENTICAL/NONE
// OFFSET: 0x0069A260/0x1403467C0
void GlobalGuildBankLogQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3224));
    msg->Put(this->m_tab);
}

// PROVEN: e7739e47 IDENTICAL/NONE
// OFFSET: 0x0069A7D0
GlobalGuildDeleteRank::GlobalGuildDeleteRank(int32_t rankOrder) {
    this->m_rankOrder = rankOrder;
}

// PROVEN: 7469815f IDENTICAL/NONE
// OFFSET: 0x0069A230
GlobalGuildDeleteRank::~GlobalGuildDeleteRank() {
}

// PROVEN: a81b22b4 IDENTICAL/NONE
// OFFSET: 0x0069A2E0/0x140346840
void GlobalGuildDeleteRank::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3234));
    msg->Put(static_cast<uint32_t>(this->m_rankOrder));
}

// PROVEN: a0488dd5 IDENTICAL/NONE
// OFFSET: 0x0069C2D0/0x140348850
GlobalGuildAddRank::GlobalGuildAddRank(const char* name, int32_t rankOrder) {
    SStrCopy(this->m_name, name, sizeof(this->m_name));
    this->m_rankOrder = rankOrder;
}

// PROVEN: 9b7e6de8 IDENTICAL/NONE
// OFFSET: 0x0069A290/0x140346800
GlobalGuildAddRank::~GlobalGuildAddRank() {
}

// OFFSET: 0x0069C240/0x140348790
void GlobalGuildAddRank::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3030));
    msg->Put(static_cast<uint32_t>(this->m_rankOrder));

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_name);
    bits.WriteBits<7>(length);
    bits.Flush();
    msg->PutData(this->m_name, strlen(this->m_name));
}

// PROVEN: 889dfabf IDENTICAL/NONE
// OFFSET: 0x0069C170/0x140348680
GlobalGuildChallengeUpdateRequest::GlobalGuildChallengeUpdateRequest() {
}

// PROVEN: a0301764 IDENTICAL/NONE
// OFFSET: 0x0069A2D0/0x140346830
GlobalGuildChallengeUpdateRequest::~GlobalGuildChallengeUpdateRequest() {
}

// PROVEN: 2ecf5883 IDENTICAL/NONE
// OFFSET: 0x0069A010/0x140346580
void GlobalGuildChallengeUpdateRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1224));
}

// PROVEN: eba46beb IDENTICAL/NONE
// OFFSET: 0x0069B590/0x140347AB0
GlobalGuildPermissionsQuery::GlobalGuildPermissionsQuery() {
}

// PROVEN: ad1741e4 IDENTICAL/NONE
// OFFSET: 0x0069A330
GlobalGuildPermissionsQuery::~GlobalGuildPermissionsQuery() {
}

// PROVEN: 4152d180 IDENTICAL/NONE
// OFFSET: 0x0069A050/0x1403465B0
void GlobalGuildPermissionsQuery::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3022));
}

// PROVEN: cc9634fc IDENTICAL/NONE
// OFFSET: 0x0069B5B0
GlobalGuildQueryMemberRecipes::GlobalGuildQueryMemberRecipes(uint64_t memberGUID, uint64_t guildGUID, int32_t skillLineID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
    memcpy(this->m_memberGUID, &memberGUID, sizeof(this->m_memberGUID));
    this->m_skillLineID = skillLineID;
}

// PROVEN: dca45ed6 IDENTICAL/NONE
// OFFSET: 0x0069A370/0x1403468D0
GlobalGuildQueryMemberRecipes::~GlobalGuildQueryMemberRecipes() {
}

// PROVEN: b2e07760 IDENTICAL/NONE
// OFFSET: 0x0069B340/0x140347870
void GlobalGuildQueryMemberRecipes::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1037));
    msg->Put(static_cast<uint32_t>(this->m_skillLineID));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_memberGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_memberGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_memberGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_memberGUID[5] != 0);
    bits.WriteBit(this->m_memberGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_memberGUID[3] != 0);
    bits.WriteBit(this->m_memberGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_memberGUID[4] != 0);
    bits.Flush();

    if (this->m_memberGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[2] ^ 1));
    }

    if (this->m_memberGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[6] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_memberGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[1] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_memberGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[3] ^ 1));
    }

    if (this->m_memberGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[0] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_memberGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[7] ^ 1));
    }

    if (this->m_memberGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[4] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_memberGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[5] ^ 1));
    }
}

// PROVEN: 925c03de IDENTICAL/NONE
// OFFSET: 0x0069A520/0x140346AD0
GlobalGuildRequestGuildXP::GlobalGuildRequestGuildXP(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: e27173cc IDENTICAL/NONE
// OFFSET: 0x0069A380/0x1403468E0
GlobalGuildRequestGuildXP::~GlobalGuildRequestGuildXP() {
}

// PROVEN: 267f2a9a IDENTICAL/NONE
// OFFSET: 0x0069AD50/0x1403472E0
void GlobalGuildRequestGuildXP::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1237));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.Flush();

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }
}

// PROVEN: f36cbaf7 IDENTICAL/NONE
// OFFSET: 0x0069B1C0
GlobalGuildAssignMemberRank::GlobalGuildAssignMemberRank(uint64_t setterGUID, uint64_t memberGUID, int32_t rankOrder) {
    memcpy(this->m_memberGUID, &memberGUID, sizeof(this->m_memberGUID));
    memcpy(this->m_setterGUID, &setterGUID, sizeof(this->m_setterGUID));
    this->m_rankOrder = rankOrder;
}

// PROVEN: b0480466 IDENTICAL/NONE
// OFFSET: 0x0069A3A0
GlobalGuildAssignMemberRank::~GlobalGuildAssignMemberRank() {
}

// PROVEN: e0be9bb7 IDENTICAL/NONE
// OFFSET: 0x0069BD00/0x140348200
void GlobalGuildAssignMemberRank::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3032));
    msg->Put(static_cast<uint32_t>(this->m_rankOrder));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_memberGUID[1] != 0);
    bits.WriteBit(this->m_memberGUID[7] != 0);
    bits.WriteBit(this->m_setterGUID[4] != 0);
    bits.WriteBit(this->m_setterGUID[2] != 0);
    bits.WriteBit(this->m_memberGUID[4] != 0);
    bits.WriteBit(this->m_memberGUID[5] != 0);
    bits.WriteBit(this->m_memberGUID[6] != 0);
    bits.WriteBit(this->m_setterGUID[1] != 0);
    bits.WriteBit(this->m_setterGUID[7] != 0);
    bits.WriteBit(this->m_memberGUID[2] != 0);
    bits.WriteBit(this->m_memberGUID[3] != 0);
    bits.WriteBit(this->m_memberGUID[0] != 0);
    bits.WriteBit(this->m_setterGUID[6] != 0);
    bits.WriteBit(this->m_setterGUID[3] != 0);
    bits.WriteBit(this->m_setterGUID[0] != 0);
    bits.WriteBit(this->m_setterGUID[5] != 0);
    bits.Flush();

    if (this->m_memberGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[0] ^ 1));
    }

    if (this->m_setterGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[1] ^ 1));
    }

    if (this->m_setterGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[3] ^ 1));
    }

    if (this->m_setterGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[5] ^ 1));
    }

    if (this->m_memberGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[7] ^ 1));
    }

    if (this->m_memberGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[3] ^ 1));
    }

    if (this->m_setterGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[0] ^ 1));
    }

    if (this->m_memberGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[1] ^ 1));
    }

    if (this->m_setterGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[6] ^ 1));
    }

    if (this->m_memberGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[2] ^ 1));
    }

    if (this->m_memberGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[5] ^ 1));
    }

    if (this->m_memberGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[4] ^ 1));
    }

    if (this->m_setterGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[2] ^ 1));
    }

    if (this->m_setterGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[4] ^ 1));
    }

    if (this->m_memberGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_memberGUID[6] ^ 1));
    }

    if (this->m_setterGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_setterGUID[7] ^ 1));
    }
}

// PROVEN: 5393e31c IDENTICAL/NONE
// OFFSET: 0x0069C6E0/0x1403469D0
GlobalGuildQueryNews::GlobalGuildQueryNews(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: 77c4cce0 IDENTICAL/NONE
// OFFSET: 0x0069A3D0/0x140346920
GlobalGuildQueryNews::~GlobalGuildQueryNews() {
}

// PROVEN: ffd746ee IDENTICAL/NONE
// OFFSET: 0x0069BA80/0x140347F80
void GlobalGuildQueryNews::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3020));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.Flush();

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }
}

// PROVEN: f49effb0 IDENTICAL/NONE
// OFFSET: 0x0069A850/0x140346DE0
GlobalGuildBankSetTabText::GlobalGuildBankSetTabText() {
}

// PROVEN: 70c6aecc IDENTICAL/NONE
// OFFSET: 0x0069A3E0/0x140346930
GlobalGuildBankSetTabText::~GlobalGuildBankSetTabText() {
}

// OFFSET: 0x0069CC10/0x140349110
void GlobalGuildBankSetTabText::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3023));
    msg->Put(this->m_tab);

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_tabText);
    bits.WriteBits<14>(length);
    bits.Flush();
    msg->PutData(this->m_tabText, strlen(this->m_tabText));
}

// PROVEN: cc05cd45 IDENTICAL/NONE
// OFFSET: 0x0069B060/0x140347580
GlobalGuildSendSoRRequest::GlobalGuildSendSoRRequest() {
}

// PROVEN: 45b5fba8 IDENTICAL/NONE
// OFFSET: 0x0069A3F0/0x140346940
GlobalGuildSendSoRRequest::~GlobalGuildSendSoRRequest() {
}

// OFFSET: 0x0069C900/0x140348DA0
void GlobalGuildSendSoRRequest::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3225));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_targetGUID[1] != 0);
    bits.WriteBit(this->m_targetGUID[6] != 0);
    bits.WriteBit(this->m_targetGUID[5] != 0);
    bits.WriteBit(this->m_targetGUID[7] != 0);
    bits.WriteBit(this->m_targetGUID[0] != 0);
    uint32_t length = strlen(this->m_text);
    bits.WriteBits<11>(length);
    bits.WriteBit(this->m_targetGUID[4] != 0);
    bits.WriteBit(this->m_targetGUID[3] != 0);
    bits.WriteBit(this->m_targetGUID[2] != 0);
    bits.Flush();

    if (this->m_targetGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[2] ^ 1));
    }

    if (this->m_targetGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[4] ^ 1));
    }

    msg->PutData(this->m_text, strlen(this->m_text));

    if (this->m_targetGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[5] ^ 1));
    }

    if (this->m_targetGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[3] ^ 1));
    }

    if (this->m_targetGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[0] ^ 1));
    }

    if (this->m_targetGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[6] ^ 1));
    }

    if (this->m_targetGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[1] ^ 1));
    }

    if (this->m_targetGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_targetGUID[7] ^ 1));
    }
}

// PROVEN: dcb9cd45 IDENTICAL/NONE
// OFFSET: 0x0069C4E0/0x140348A50
GlobalGuildRequestMaxDailyXP::GlobalGuildRequestMaxDailyXP(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: 8d083571 IDENTICAL/NONE
// OFFSET: 0x0069A400/0x140346950
GlobalGuildRequestMaxDailyXP::~GlobalGuildRequestMaxDailyXP() {
}

// PROVEN: 2b5eefd7 IDENTICAL/NONE
// OFFSET: 0x0069AC10/0x1403471A0
void GlobalGuildRequestMaxDailyXP::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3232));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.Flush();

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }
}

// PROVEN: 33ff181c IDENTICAL/NONE
// OFFSET: 0x0069ABB0/0x140347140
GlobalGuildGetRanks::GlobalGuildGetRanks(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: e9d57966 IDENTICAL/NONE
// OFFSET: 0x0069A410/0x140346960
GlobalGuildGetRanks::~GlobalGuildGetRanks() {
}

// PROVEN: d844b188 IDENTICAL/NONE
// OFFSET: 0x0069B5F0/0x140347B20
void GlobalGuildGetRanks::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1026));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.Flush();

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }
}

// PROVEN: c8ca3f0d IDENTICAL/NONE
// OFFSET: 0x0069AC00/0x140347180
GlobalGuildReplaceGuildMaster::GlobalGuildReplaceGuildMaster() {
}

// PROVEN: 8874d37d IDENTICAL/NONE
// OFFSET: 0x0069A460/0x1403469A0
GlobalGuildReplaceGuildMaster::~GlobalGuildReplaceGuildMaster() {
}

// PROVEN: 60ecb381 IDENTICAL/NONE
// OFFSET: 0x0069A430/0x140346980
void GlobalGuildReplaceGuildMaster::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1034));
}

// PROVEN: c1a65f10 IDENTICAL/NONE
// OFFSET: 0x0069A550/0x140346AF0
GlobalGuildQueryMembersForRecipe::GlobalGuildQueryMembersForRecipe(uint64_t guildGUID, int32_t skillLineID, int32_t spellID, int32_t uniqueBit) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
    this->m_uniqueBit = uniqueBit;
    this->m_skillLineID = skillLineID;
    this->m_spellID = spellID;
}

// PROVEN: 0fc2f3c8 IDENTICAL/NONE
// OFFSET: 0x0069A490/0x140346A20
GlobalGuildQueryMembersForRecipe::~GlobalGuildQueryMembersForRecipe() {
}

// PROVEN: 3703b9b0 IDENTICAL/NONE
// OFFSET: 0x0069C380/0x1403488F0
void GlobalGuildQueryMembersForRecipe::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1036));
    msg->Put(static_cast<uint32_t>(this->m_spellID));
    msg->Put(static_cast<uint32_t>(this->m_skillLineID));
    msg->Put(static_cast<uint32_t>(this->m_uniqueBit));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.Flush();

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }
}

// OFFSET: 0x0069CB70
GlobalGuildSetMemberNote::GlobalGuildSetMemberNote(const JamGuildMemberSetNoteData& noteData) : m_noteData(noteData) {
}

// PROVEN: 6979e2d9 IDENTICAL/NONE
// OFFSET: 0x0069B930/0x140347E30
GlobalGuildSetMemberNote::~GlobalGuildSetMemberNote() {
}

// OFFSET: 0x0069BF50/0x140348440
void GlobalGuildSetMemberNote::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1233));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_noteData.m_memberGUID[1] != 0);
    bits.WriteBit(this->m_noteData.m_memberGUID[4] != 0);
    bits.WriteBit(this->m_noteData.m_memberGUID[5] != 0);
    bits.WriteBit(this->m_noteData.m_memberGUID[3] != 0);
    bits.WriteBit(this->m_noteData.m_memberGUID[0] != 0);
    bits.WriteBit(this->m_noteData.m_memberGUID[7] != 0);
    bits.WriteBit(this->m_noteData.m_isPublic);
    bits.WriteBit(this->m_noteData.m_memberGUID[6] != 0);
    uint32_t length = strlen(this->m_noteData.m_note);
    bits.WriteBits<8>(length);
    bits.WriteBit(this->m_noteData.m_memberGUID[2] != 0);
    bits.Flush();

    if (this->m_noteData.m_memberGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[4] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[5] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[0] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[3] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[1] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[6] ^ 1));
    }

    if (this->m_noteData.m_memberGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[7] ^ 1));
    }

    msg->PutData(this->m_noteData.m_note, strlen(this->m_noteData.m_note));

    if (this->m_noteData.m_memberGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_noteData.m_memberGUID[2] ^ 1));
    }
}

// OFFSET: 0x0069B5A0
GlobalLFGuildGetGuildPost::GlobalLFGuildGetGuildPost() {
}

// OFFSET: 0x0069A360
GlobalLFGuildGetGuildPost::~GlobalLFGuildGetGuildPost() {
}

// OFFSET: 0x0069A1E0/0x140346760
void GlobalLFGuildGetGuildPost::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3237));
}

// OFFSET: 0x0069C0F0/0x1403485D0
GlobalLFGuildGetRecruits::GlobalLFGuildGetRecruits(int32_t lastUpdate) {
    this->m_lastUpdate = lastUpdate;
}

// OFFSET: 0x0069A100/0x140346670
GlobalLFGuildGetRecruits::~GlobalLFGuildGetRecruits() {
}

// OFFSET: 0x0069A140/0x1403466B0
void GlobalLFGuildGetRecruits::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3230));
    msg->Put(static_cast<uint32_t>(this->m_lastUpdate));
}

// OFFSET: 0x0069C510/0x140348A70
GlobalLFGuildGetApplications::GlobalLFGuildGetApplications() {
}

// OFFSET: 0x0069A450/0x140346990
GlobalLFGuildGetApplications::~GlobalLFGuildGetApplications() {
}

// OFFSET: 0x0069A2B0/0x140346820
void GlobalLFGuildGetApplications::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1230));
}

// OFFSET: 0x0069B760
GlobalLFGuildDeclineRecruit::GlobalLFGuildDeclineRecruit(uint64_t recruitGUID) {
    memcpy(this->m_recruitGUID, &recruitGUID, sizeof(this->m_recruitGUID));
}

// OFFSET: 0x0069A310/0x140346880
GlobalLFGuildDeclineRecruit::~GlobalLFGuildDeclineRecruit() {
}

// OFFSET: 0x0069C590/0x140348AD0
void GlobalLFGuildDeclineRecruit::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1031));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_recruitGUID[1] != 0);
    bits.WriteBit(this->m_recruitGUID[4] != 0);
    bits.WriteBit(this->m_recruitGUID[5] != 0);
    bits.WriteBit(this->m_recruitGUID[2] != 0);
    bits.WriteBit(this->m_recruitGUID[6] != 0);
    bits.WriteBit(this->m_recruitGUID[7] != 0);
    bits.WriteBit(this->m_recruitGUID[0] != 0);
    bits.WriteBit(this->m_recruitGUID[3] != 0);
    bits.Flush();

    if (this->m_recruitGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[5] ^ 1));
    }

    if (this->m_recruitGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[7] ^ 1));
    }

    if (this->m_recruitGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[2] ^ 1));
    }

    if (this->m_recruitGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[3] ^ 1));
    }

    if (this->m_recruitGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[4] ^ 1));
    }

    if (this->m_recruitGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[1] ^ 1));
    }

    if (this->m_recruitGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[0] ^ 1));
    }

    if (this->m_recruitGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_recruitGUID[6] ^ 1));
    }
}

// OFFSET: 0x0069C150
GlobalLFGuildRemoveRecruit::GlobalLFGuildRemoveRecruit() {
}

// OFFSET: 0x0069A130/0x1403466A0
GlobalLFGuildRemoveRecruit::~GlobalLFGuildRemoveRecruit() {
}

// OFFSET: 0x0069B940/0x140347E40
void GlobalLFGuildRemoveRecruit::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3027));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.Flush();

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }

    if (this->m_guildGUID[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[0] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[5] ^ 1));
    }

    if (this->m_guildGUID[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[1] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
    }

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }
}

// OFFSET: 0x00B3BD80
GlobalLFGuildSetGuildPost::GlobalLFGuildSetGuildPost() {
}

// OFFSET: 0x00B3BD10
GlobalLFGuildSetGuildPost::~GlobalLFGuildSetGuildPost() {
}

// OFFSET: 0x00B3BDA0/0x140908C60
void GlobalLFGuildSetGuildPost::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x448));
    msg->Put(this->m_level);
    msg->Put(this->m_availability);
    msg->Put(this->m_interests);
    msg->Put(this->m_classRoles);

    JamBitWriter bits(msg);
    uint32_t length = strlen(this->m_comment);
    bits.WriteBits<11>(length);
    bits.WriteBit(this->m_listed);
    bits.Flush();
    msg->PutData(this->m_comment, strlen(this->m_comment));
}

// OFFSET: 0x00B3BD70/0x140908C00
GlobalLFGuildBrowse::GlobalLFGuildBrowse() {
}

// OFFSET: 0x00B3BD00/0x140908B80
GlobalLFGuildBrowse::~GlobalLFGuildBrowse() {
}

// OFFSET: 0x00B3BD20/0x140908BA0
void GlobalLFGuildBrowse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x548));
    msg->Put(this->m_classRoles);
    msg->Put(this->m_availability);
    msg->Put(this->m_interests);
    msg->Put(static_cast<uint32_t>(this->m_characterLevel));
}

// OFFSET: 0x0069CCC0
GlobalGuildSetAchievementTracking::GlobalGuildSetAchievementTracking() {
}

// OFFSET: 0x0069CC90
GlobalGuildSetAchievementTracking::~GlobalGuildSetAchievementTracking() {
}

// OFFSET: 0x0069C890
void GlobalGuildSetAchievementTracking::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x1027));

    JamBitWriter bits(msg);
    bits.WriteBits<24>(this->m_achievementIDs.Count(), JamBitCount<true>());
    bits.Flush();

    for (uint32_t i = 0; i < this->m_achievementIDs.Count(); i++) {
        msg->Put(static_cast<uint32_t>(this->m_achievementIDs[i]));
    }
}
