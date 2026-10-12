#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "Engine/Source/Base/CDataStore.h"
#include <cstring>

// PROVEN: 9ee87252 IDENTICAL/IDENTICAL
// OFFSET: [0x00670580/0x140317EC0]
PlayerObjectUpdateFailed::PlayerObjectUpdateFailed() {
}

// PROVEN: eb72bbd9 IDENTICAL/IDENTICAL
// OFFSET: [0x00670880/0x140318530]
PlayerObjectUpdateFailed::~PlayerObjectUpdateFailed() {
}

// PROVEN: 43a4694a IDENTICAL/IDENTICAL
// OFFSET: 0x00674820/0x14031CBF0
void PlayerObjectUpdateFailed::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3808));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guid[6] != 0);
    bits.WriteBit(this->m_guid[7] != 0);
    bits.WriteBit(this->m_guid[4] != 0);
    bits.WriteBit(this->m_guid[0] != 0);
    bits.WriteBit(this->m_guid[1] != 0);
    bits.WriteBit(this->m_guid[5] != 0);
    bits.WriteBit(this->m_guid[3] != 0);
    bits.WriteBit(this->m_guid[2] != 0);
    bits.Flush();

    if (this->m_guid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[6] ^ 1));
    }

    if (this->m_guid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[7] ^ 1));
    }

    if (this->m_guid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[2] ^ 1));
    }

    if (this->m_guid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[3] ^ 1));
    }

    if (this->m_guid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[1] ^ 1));
    }

    if (this->m_guid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[4] ^ 1));
    }

    if (this->m_guid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[0] ^ 1));
    }

    if (this->m_guid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[5] ^ 1));
    }
}

// PROVEN: 14fcc517 IDENTICAL/IDENTICAL
// OFFSET: [0x00673390/0x140317C80]
PlayerObjectUpdateRescued::PlayerObjectUpdateRescued() {
}

// PROVEN: 9f38e47f IDENTICAL/IDENTICAL
// OFFSET: [0x00670910/0x1403187A0]
PlayerObjectUpdateRescued::~PlayerObjectUpdateRescued() {
}

// PROVEN: 54159ac2 IDENTICAL/IDENTICAL
// OFFSET: 0x00674650/0x14031CAB0
void PlayerObjectUpdateRescued::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3906));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guid[0] != 0);
    bits.WriteBit(this->m_guid[5] != 0);
    bits.WriteBit(this->m_guid[6] != 0);
    bits.WriteBit(this->m_guid[2] != 0);
    bits.WriteBit(this->m_guid[4] != 0);
    bits.WriteBit(this->m_guid[3] != 0);
    bits.WriteBit(this->m_guid[7] != 0);
    bits.WriteBit(this->m_guid[1] != 0);
    bits.Flush();

    if (this->m_guid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[0] ^ 1));
    }

    if (this->m_guid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[3] ^ 1));
    }

    if (this->m_guid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[6] ^ 1));
    }

    if (this->m_guid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[1] ^ 1));
    }

    if (this->m_guid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[2] ^ 1));
    }

    if (this->m_guid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[5] ^ 1));
    }

    if (this->m_guid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[4] ^ 1));
    }

    if (this->m_guid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[7] ^ 1));
    }
}

// PROVEN: ca0eb21c IDENTICAL/IDENTICAL
// OFFSET: 0x00670A10/0x140318B00
PlayerSetCurrencyFlags::PlayerSetCurrencyFlags() {
}

// PROVEN: c8bbd8e3 IDENTICAL/IDENTICAL
// OFFSET: 0x006703A0/0x140317BF0
PlayerSetCurrencyFlags::~PlayerSetCurrencyFlags() {
}

// PROVEN: 7634b087 IDENTICAL/IDENTICAL
// OFFSET: 0x00674080/0x140318200
void PlayerSetCurrencyFlags::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7306));
    msg->Put(this->m_flags);
    msg->Put(static_cast<uint32_t>(this->m_type));
}

// PROVEN: fcbab52c IDENTICAL/IDENTICAL
// OFFSET: 0x006880B0/0x140332EE0
PlayerQueryQuestCompletionNPCs::PlayerQueryQuestCompletionNPCs() {
}

// PROVEN: 76fd9e1f IDENTICAL/UNPROVEN
// OFFSET: 0x00688020/0x140332E40
PlayerQueryQuestCompletionNPCs::~PlayerQueryQuestCompletionNPCs() {
}

// PROVEN: 26c538a9 IDENTICAL/DIFFERENT
// OFFSET: 0x00680AB0/0x14032A460
void PlayerQueryQuestCompletionNPCs::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7302));
    JamBitWriter bits(msg);
    bits.WriteBits<24>(this->m_questIds.m_count);
    bits.Flush();

    for (uint32_t i = 0; i < this->m_questIds.m_count; i++) {
        msg->Put(static_cast<uint32_t>(this->m_questIds.m_data[i]));
    }
}

// PROVEN: 485d8017 IDENTICAL/IDENTICAL
// OFFSET: 0x006708D0/0x140318630
PlayerSetPetSlot::PlayerSetPetSlot() {
}

// PROVEN: be91331a IDENTICAL/IDENTICAL
// OFFSET: 0x006704E0/0x140317DC0
PlayerSetPetSlot::~PlayerSetPetSlot() {
}

// PROVEN: ec9989a7 IDENTICAL/IDENTICAL
// OFFSET: 0x00674B60/0x14031CDA0
void PlayerSetPetSlot::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3A04));
    msg->Put(this->m_petNumber);
    msg->Put(this->m_destSlot);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_stableMaster[3] != 0);
    bits.WriteBit(this->m_stableMaster[2] != 0);
    bits.WriteBit(this->m_stableMaster[0] != 0);
    bits.WriteBit(this->m_stableMaster[7] != 0);
    bits.WriteBit(this->m_stableMaster[5] != 0);
    bits.WriteBit(this->m_stableMaster[6] != 0);
    bits.WriteBit(this->m_stableMaster[1] != 0);
    bits.WriteBit(this->m_stableMaster[4] != 0);
    bits.Flush();

    if (this->m_stableMaster[5]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[5] ^ 1));
    }

    if (this->m_stableMaster[3]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[3] ^ 1));
    }

    if (this->m_stableMaster[1]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[1] ^ 1));
    }

    if (this->m_stableMaster[7]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[7] ^ 1));
    }

    if (this->m_stableMaster[4]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[4] ^ 1));
    }

    if (this->m_stableMaster[0]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[0] ^ 1));
    }

    if (this->m_stableMaster[6]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[6] ^ 1));
    }

    if (this->m_stableMaster[2]) {
        msg->Put(static_cast<uint8_t>(this->m_stableMaster[2] ^ 1));
    }
}

// PROVEN: f482eeab IDENTICAL/IDENTICAL
// OFFSET: 0x006733E0/0x140317CC0
PlayerLootCurrency::PlayerLootCurrency() {
}

// PROVEN: b13196ef IDENTICAL/IDENTICAL
// OFFSET: 0x00670530/0x140317E60
PlayerLootCurrency::~PlayerLootCurrency() {
}

// PROVEN: ea002873 IDENTICAL/IDENTICAL
// OFFSET: 0x00670CD0/0x140318FB0
void PlayerLootCurrency::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x781C));
    msg->Put(this->m_lootListID);
}

// PROVEN: 5ae7a29f IDENTICAL/IDENTICAL
// OFFSET: 0x00670640/0x140317FD0
PlayerCliCancelTrade::PlayerCliCancelTrade() {
}

// PROVEN: 37bd8434 IDENTICAL/IDENTICAL
// OFFSET: 0x006703F0/0x140317C60
PlayerCliCancelTrade::~PlayerCliCancelTrade() {
}

// PROVEN: b78ea747 IDENTICAL/IDENTICAL
// OFFSET: 0x00670460/0x140317D80
void PlayerCliCancelTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x731E));
}

// PROVEN: e50f3409 IDENTICAL/IDENTICAL
// OFFSET: 0x00670800/0x140318310
PlayerCliBeginTrade::PlayerCliBeginTrade() {
}

// PROVEN: 5d39ff15 IDENTICAL/IDENTICAL
// OFFSET: 0x00670620/0x140317FA0
PlayerCliBeginTrade::~PlayerCliBeginTrade() {
}

// PROVEN: efff1a79 IDENTICAL/IDENTICAL
// OFFSET: 0x006706B0/0x140318090
void PlayerCliBeginTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x721E));
}

// PROVEN: fbcb5f03 IDENTICAL/IDENTICAL
// OFFSET: 0x00670790/0x140318140
PlayerCliUnacceptTrade::PlayerCliUnacceptTrade() {
}

// PROVEN: a0e94c69 IDENTICAL/IDENTICAL
// OFFSET: 0x00670DF0/0x140319140
PlayerCliUnacceptTrade::~PlayerCliUnacceptTrade() {
}

// PROVEN: e6ddd6d4 IDENTICAL/IDENTICAL
// OFFSET: 0x00670760/0x140318110
void PlayerCliUnacceptTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x391A));
}

// PROVEN: 33969aeb IDENTICAL/NONE
// OFFSET: 0x00670A50
PlayerCliAcceptTrade::PlayerCliAcceptTrade() {
}

// PROVEN: e2f4d73f IDENTICAL/NONE
// OFFSET: 0x00670B70
PlayerCliAcceptTrade::~PlayerCliAcceptTrade() {
}

// PROVEN: 3e0ee9f1 IDENTICAL/DIFFERENT
// OFFSET: 0x00670C10/0x140317E50
void PlayerCliAcceptTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7110));
    msg->Put(this->m_stateIndex);
}

// PROVEN: 43f3188f IDENTICAL/NONE
// OFFSET: 0x00673490
PlayerCliIgnoreTrade::PlayerCliIgnoreTrade() {
}

// PROVEN: 12a79b3b IDENTICAL/NONE
// OFFSET: 0x00670940
PlayerCliIgnoreTrade::~PlayerCliIgnoreTrade() {
}

// PROVEN: a2c4ad20 IDENTICAL/NONE
// OFFSET: 0x00670A60
void PlayerCliIgnoreTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7112));
}

// PROVEN: e6f31239 IDENTICAL/NONE
// OFFSET: 0x006708A0
PlayerCliBusyTrade::PlayerCliBusyTrade() {
}

// PROVEN: 194e7b56 IDENTICAL/NONE
// OFFSET: 0x00670900
PlayerCliBusyTrade::~PlayerCliBusyTrade() {
}

// PROVEN: 3ed84277 IDENTICAL/NONE
// OFFSET: 0x00670510
void PlayerCliBusyTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x331C));
}

// PROVEN: 4d3d1ac3 IDENTICAL/IDENTICAL
// OFFSET: 0x00670A00/0x140318AE0
PlayerInitiateTrade::PlayerInitiateTrade() {
}

// PROVEN: 2601caf3 IDENTICAL/IDENTICAL
// OFFSET: 0x00670C00/0x140318E60
PlayerInitiateTrade::~PlayerInitiateTrade() {
}

// PROVEN: 288e3d93 IDENTICAL/IDENTICAL
// OFFSET: 0x00673F40/0x14031C570
void PlayerInitiateTrade::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7916));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guid[0] != 0);
    bits.WriteBit(this->m_guid[3] != 0);
    bits.WriteBit(this->m_guid[5] != 0);
    bits.WriteBit(this->m_guid[1] != 0);
    bits.WriteBit(this->m_guid[4] != 0);
    bits.WriteBit(this->m_guid[6] != 0);
    bits.WriteBit(this->m_guid[7] != 0);
    bits.WriteBit(this->m_guid[2] != 0);
    bits.Flush();

    if (this->m_guid[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[7] ^ 1));
    }

    if (this->m_guid[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[4] ^ 1));
    }

    if (this->m_guid[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[3] ^ 1));
    }

    if (this->m_guid[5]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[5] ^ 1));
    }

    if (this->m_guid[1]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[1] ^ 1));
    }

    if (this->m_guid[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[2] ^ 1));
    }

    if (this->m_guid[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[6] ^ 1));
    }

    if (this->m_guid[0]) {
        msg->Put(static_cast<uint8_t>(this->m_guid[0] ^ 1));
    }
}

// PROVEN: 18df6d33 IDENTICAL/IDENTICAL
// OFFSET: 0x00670650/0x140317FF0
PlayerSetTradeGold::PlayerSetTradeGold() {
}

// PROVEN: f0b9eb10 IDENTICAL/IDENTICAL
// OFFSET: 0x00670820/0x140318360
PlayerSetTradeGold::~PlayerSetTradeGold() {
}

// PROVEN: 8cf448d4 IDENTICAL/IDENTICAL
// OFFSET: 0x00673AF0/0x14031BFE0
void PlayerSetTradeGold::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3008));
    msg->Put(this->m_coinage);
}

// PROVEN: e70a8987 IDENTICAL/IDENTICAL
// OFFSET: 0x00670840/0x140318390
PlayerSetTradeCurrency::PlayerSetTradeCurrency() {
}

// PROVEN: facc5c72 IDENTICAL/IDENTICAL
// OFFSET: 0x006703C0/0x140317C10
PlayerSetTradeCurrency::~PlayerSetTradeCurrency() {
}

// PROVEN: caec9297 IDENTICAL/IDENTICAL
// OFFSET: 0x00673A90/0x14031BF70
void PlayerSetTradeCurrency::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3312));
    msg->Put(static_cast<uint32_t>(this->m_quantity));
    msg->Put(static_cast<uint32_t>(this->m_type));
}

// PROVEN: 676131b1 IDENTICAL/IDENTICAL
// OFFSET: 0x00673440/0x140317D00
PlayerSetTradeItem::PlayerSetTradeItem() {
}

// PROVEN: e24e1e5a IDENTICAL/IDENTICAL
// OFFSET: 0x006705B0/0x140317F20
PlayerSetTradeItem::~PlayerSetTradeItem() {
}

// PROVEN: 68c76bca IDENTICAL/IDENTICAL
// OFFSET: 0x00670BC0/0x140318E10
void PlayerSetTradeItem::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7B0C));
    msg->Put(this->m_itemSlotInPack);
    msg->Put(this->m_tradeSlot);
    msg->Put(this->m_packSlot);
}

// PROVEN: 6ae92b85 IDENTICAL/IDENTICAL
// OFFSET: 0x00670780/0x140318120
PlayerClearTradeItem::PlayerClearTradeItem() {
}

// PROVEN: 0e53ab5b IDENTICAL/IDENTICAL
// OFFSET: 0x00670540/0x140317E70
PlayerClearTradeItem::~PlayerClearTradeItem() {
}

// PROVEN: 90839725 IDENTICAL/IDENTICAL
// OFFSET: 0x00674390/0x140318290
void PlayerClearTradeItem::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7018));
    msg->Put(this->m_tradeSlot);
}

// PROVEN: cc5a9340 IDENTICAL/IDENTICAL
// OFFSET: 0x006743E0/0x14031C950
PlayerRequestGuildPartyState::PlayerRequestGuildPartyState(uint64_t guildGUID) {
    memcpy(this->m_guildGUID, &guildGUID, sizeof(this->m_guildGUID));
}

// PROVEN: 4fe4cb77 IDENTICAL/IDENTICAL
// OFFSET: 0x006706F0/0x1403180C0
PlayerRequestGuildPartyState::~PlayerRequestGuildPartyState() {
}

// PROVEN: 1b1d1bbe IDENTICAL/IDENTICAL
// OFFSET: 0x00674D50/0x14031CF50
void PlayerRequestGuildPartyState::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3900));

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_guildGUID[0] != 0);
    bits.WriteBit(this->m_guildGUID[6] != 0);
    bits.WriteBit(this->m_guildGUID[7] != 0);
    bits.WriteBit(this->m_guildGUID[3] != 0);
    bits.WriteBit(this->m_guildGUID[5] != 0);
    bits.WriteBit(this->m_guildGUID[1] != 0);
    bits.WriteBit(this->m_guildGUID[2] != 0);
    bits.WriteBit(this->m_guildGUID[4] != 0);
    bits.Flush();

    if (this->m_guildGUID[6]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[6] ^ 1));
    }

    if (this->m_guildGUID[3]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[3] ^ 1));
    }

    if (this->m_guildGUID[2]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[2] ^ 1));
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

    if (this->m_guildGUID[7]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[7] ^ 1));
    }

    if (this->m_guildGUID[4]) {
        msg->Put(static_cast<uint8_t>(this->m_guildGUID[4] ^ 1));
    }
}

// PROVEN: 8e842b20 IDENTICAL/IDENTICAL
// OFFSET: 0x00673460/0x140317D40
PlayerRequestCategoryCooldowns::PlayerRequestCategoryCooldowns() {
}

// PROVEN: 7b095c05 IDENTICAL/IDENTICAL
// OFFSET: 0x006707A0/0x140318160
PlayerRequestCategoryCooldowns::~PlayerRequestCategoryCooldowns() {
}

// PROVEN: 1751e968 IDENTICAL/IDENTICAL
// OFFSET: 0x00670700/0x1403180D0
void PlayerRequestCategoryCooldowns::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7102));
}

// PROVEN: 5565539e IDENTICAL/IDENTICAL
// OFFSET: 0x00670690/0x140318060
PlayerTimeSyncResponse::PlayerTimeSyncResponse() {
}

// PROVEN: 115494d2 IDENTICAL/IDENTICAL
// OFFSET: 0x00670950/0x140318930
PlayerTimeSyncResponse::~PlayerTimeSyncResponse() {
}

// PROVEN: c89e17de IDENTICAL/IDENTICAL
// OFFSET: 0x00673ED0/0x140318170
void PlayerTimeSyncResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3B0C));
    msg->Put(this->m_sequenceIndex);
    msg->Put(this->m_clientTime);
}

// PROVEN: 0e9f26a1 IDENTICAL/IDENTICAL
// OFFSET: 0x00674630/0x1403184F0
PlayerTimeSyncResponseFailed::PlayerTimeSyncResponseFailed(uint32_t sequenceIndex) {
    this->m_sequenceIndex = sequenceIndex;
}

// PROVEN: c9ac4f68 IDENTICAL/NONE
// OFFSET: 0x006703B0
PlayerTimeSyncResponseFailed::~PlayerTimeSyncResponseFailed() {
}

// PROVEN: f2b597c2 IDENTICAL/IDENTICAL
// OFFSET: 0x00670C40/0x140318EB0
void PlayerTimeSyncResponseFailed::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x710A));
    msg->Put(this->m_sequenceIndex);
}

// PROVEN: c90886cb IDENTICAL/IDENTICAL
// OFFSET: 0x00670B80/0x140318D60
PlayerTimeAdjustmentResponse::PlayerTimeAdjustmentResponse() {
}

// PROVEN: 9474a4ff IDENTICAL/IDENTICAL
// OFFSET: 0x00670450/0x140317D30
PlayerTimeAdjustmentResponse::~PlayerTimeAdjustmentResponse() {
}

// PROVEN: 43963d4b IDENTICAL/IDENTICAL
// OFFSET: 0x00674480/0x140318410
void PlayerTimeAdjustmentResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3818));
    msg->Put(this->m_sequenceIndex);
    msg->Put(this->m_clientTime);
}

