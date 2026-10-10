#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: 9ee87252 IDENTICAL/NONE
// OFFSET: [0x00670580/0x140317EC0]
PlayerObjectUpdateFailed::PlayerObjectUpdateFailed() {
}

// PROVEN: eb72bbd9 IDENTICAL/NONE
// OFFSET: [0x00670880/0x140318530]
PlayerObjectUpdateFailed::~PlayerObjectUpdateFailed() {
}

// PROVEN: 43a4694a IDENTICAL/NONE
// OFFSET: 0x00674820
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

// PROVEN: 14fcc517 IDENTICAL/NONE
// OFFSET: [0x00673390/0x140317C80]
PlayerObjectUpdateRescued::PlayerObjectUpdateRescued() {
}

// PROVEN: 9f38e47f IDENTICAL/NONE
// OFFSET: [0x00670910/0x1403187A0]
PlayerObjectUpdateRescued::~PlayerObjectUpdateRescued() {
}

// PROVEN: 54159ac2 IDENTICAL/NONE
// OFFSET: 0x00674650
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

// OFFSET: 0x00670A10
PlayerSetCurrencyFlags::PlayerSetCurrencyFlags() {
}

// OFFSET: 0x006703A0
PlayerSetCurrencyFlags::~PlayerSetCurrencyFlags() {
}

// OFFSET: 0x00674080
void PlayerSetCurrencyFlags::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7306));
    msg->Put(this->m_flags);
    msg->Put(static_cast<uint32_t>(this->m_type));
}

// OFFSET: 0x006880B0
PlayerQueryQuestCompletionNPCs::PlayerQueryQuestCompletionNPCs() {
}

// OFFSET: 0x00688020/0x140332E40
PlayerQueryQuestCompletionNPCs::~PlayerQueryQuestCompletionNPCs() {
}

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

// OFFSET: 0x006708D0
PlayerSetPetSlot::PlayerSetPetSlot() {
}

// OFFSET: 0x006704E0
PlayerSetPetSlot::~PlayerSetPetSlot() {
}

// OFFSET: 0x00674B60
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
