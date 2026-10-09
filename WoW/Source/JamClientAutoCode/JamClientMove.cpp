#include "WoW/Source/JamClientAutoCode/JamClientMove.h"
#include <cmath>
#include <float.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: 0x00E4D540
bool (*ClientMoveCollisionDisable::s_handler)(ClientMoveCollisionDisable* msg);

// STATIC: 0x00E4D808
bool (*ClientMoveCollisionEnable::s_handler)(ClientMoveCollisionEnable* msg);

// STATIC: 0x00E4D498
bool (*ClientMoveGravityDisable::s_handler)(ClientMoveGravityDisable* msg);

// STATIC: 0x00E4D79C
bool (*ClientMoveGravityEnable::s_handler)(ClientMoveGravityEnable* msg);

// STATIC: 0x00E4D608
bool (*ClientMoveKnockBack::s_handler)(ClientMoveKnockBack* msg);

// STATIC: 0x00E4D70C
bool (*ClientMoveRoot::s_handler)(ClientMoveRoot* msg);

// STATIC: 0x00E4D524
bool (*ClientMoveSetCanFly::s_handler)(ClientMoveSetCanFly* msg);

// STATIC: 0x00E4D67C
bool (*ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler)(ClientMoveSetCanTransitionBetweenSwimAndFly* msg);

// STATIC: 0x00E4D750
bool (*ClientMoveSetCollisionHeight::s_handler)(ClientMoveSetCollisionHeight* msg);

// STATIC: 0x00E4D634
bool (*ClientMoveSetFeatherFall::s_handler)(ClientMoveSetFeatherFall* msg);

// STATIC: 0x00E4D64C
bool (*ClientMoveSetFlightBackSpeed::s_handler)(ClientMoveSetFlightBackSpeed* msg);

// STATIC: 0x00E4D4CC
bool (*ClientMoveSetFlightSpeed::s_handler)(ClientMoveSetFlightSpeed* msg);

// STATIC: 0x00E4D5EC
bool (*ClientMoveSetHover::s_handler)(ClientMoveSetHover* msg);

// STATIC: 0x00E4D6D4
bool (*ClientMoveSetLandWalk::s_handler)(ClientMoveSetLandWalk* msg);

// STATIC: 0x00E4D434
bool (*ClientMoveSetNormalFall::s_handler)(ClientMoveSetNormalFall* msg);

// STATIC: 0x00E4D7A8
bool (*ClientMoveSetPitchRate::s_handler)(ClientMoveSetPitchRate* msg);

// STATIC: 0x00E4D484
bool (*ClientMoveSetRunBackSpeed::s_handler)(ClientMoveSetRunBackSpeed* msg);

// STATIC: 0x00E4D668
bool (*ClientMoveSetRunSpeed::s_handler)(ClientMoveSetRunSpeed* msg);

// STATIC: 0x00E4D4A4
bool (*ClientMoveSetSwimBackSpeed::s_handler)(ClientMoveSetSwimBackSpeed* msg);

// STATIC: 0x00E4D62C
bool (*ClientMoveSetSwimSpeed::s_handler)(ClientMoveSetSwimSpeed* msg);

// STATIC: 0x00E4D614
bool (*ClientMoveSetTurnRate::s_handler)(ClientMoveSetTurnRate* msg);

// STATIC: 0x00E4D800
bool (*ClientMoveSetVehicleRecId::s_handler)(ClientMoveSetVehicleRecId* msg);

// STATIC: 0x00E4D4B0
bool (*ClientMoveSetWalkSpeed::s_handler)(ClientMoveSetWalkSpeed* msg);

// STATIC: 0x00E4D7A4
bool (*ClientMoveSetWaterWalk::s_handler)(ClientMoveSetWaterWalk* msg);

// STATIC: 0x00E4D788
bool (*ClientMoveSkipTime::s_handler)(ClientMoveSkipTime* msg);

// STATIC: 0x00E4D4FC
bool (*ClientMoveTeleport::s_handler)(ClientMoveTeleport* msg);

// STATIC: 0x00E4D6C8
bool (*ClientMoveUnroot::s_handler)(ClientMoveUnroot* msg);

// STATIC: 0x00E4D4E0
bool (*ClientMoveUnsetCanFly::s_handler)(ClientMoveUnsetCanFly* msg);

// STATIC: 0x00E4D528
bool (*ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler)(ClientMoveUnsetCanTransitionBetweenSwimAndFly* msg);

// STATIC: 0x00E4D730
bool (*ClientMoveUnsetHover::s_handler)(ClientMoveUnsetHover* msg);

// STATIC: 0x00E4D4D0
bool (*ClientMoveUpdate::s_handler)(ClientMoveUpdate* msg);

// STATIC: 0x00E4D560
bool (*ClientMoveUpdateCollisionHeight::s_handler)(ClientMoveUpdateCollisionHeight* msg);

// STATIC: 0x00E4D778
bool (*ClientMoveUpdateFlightBackSpeed::s_handler)(ClientMoveUpdateFlightBackSpeed* msg);

// STATIC: 0x00E4D490
bool (*ClientMoveUpdateFlightSpeed::s_handler)(ClientMoveUpdateFlightSpeed* msg);

// STATIC: 0x00E4D63C
bool (*ClientMoveUpdateKnockBack::s_handler)(ClientMoveUpdateKnockBack* msg);

// STATIC: 0x00E4D58C
bool (*ClientMoveUpdatePitchRate::s_handler)(ClientMoveUpdatePitchRate* msg);

// STATIC: 0x00E4D700
bool (*ClientMoveUpdateRunBackSpeed::s_handler)(ClientMoveUpdateRunBackSpeed* msg);

// STATIC: 0x00E4D744
bool (*ClientMoveUpdateRunSpeed::s_handler)(ClientMoveUpdateRunSpeed* msg);

// STATIC: 0x00E4D72C
bool (*ClientMoveUpdateSwimBackSpeed::s_handler)(ClientMoveUpdateSwimBackSpeed* msg);

// STATIC: 0x00E4D5B8
bool (*ClientMoveUpdateSwimSpeed::s_handler)(ClientMoveUpdateSwimSpeed* msg);

// STATIC: 0x00E4D708
bool (*ClientMoveUpdateTeleport::s_handler)(ClientMoveUpdateTeleport* msg);

// STATIC: 0x00E4D7D4
bool (*ClientMoveUpdateTurnRate::s_handler)(ClientMoveUpdateTurnRate* msg);

// STATIC: 0x00E4D5AC
bool (*ClientMoveUpdateWalkSpeed::s_handler)(ClientMoveUpdateWalkSpeed* msg);

// STATIC: 0x00E4D5FC
bool (*ClientPlaySpellVisual::s_handler)(ClientPlaySpellVisual* msg);

// STATIC: 0x00E4D790
bool (*ClientSetFootstepSoundsSuppressed::s_handler)(ClientSetFootstepSoundsSuppressed* msg);

// STATIC: 0x00E4D4BC
bool (*ClientSplineMoveCollisionDisable::s_handler)(ClientSplineMoveCollisionDisable* msg);

// STATIC: 0x00E4D588
bool (*ClientSplineMoveCollisionEnable::s_handler)(ClientSplineMoveCollisionEnable* msg);

// STATIC: 0x00E4D7AC
bool (*ClientSplineMoveGravityDisable::s_handler)(ClientSplineMoveGravityDisable* msg);

// STATIC: 0x00E4D6E8
bool (*ClientSplineMoveGravityEnable::s_handler)(ClientSplineMoveGravityEnable* msg);

// STATIC: 0x00E4D4E4
bool (*ClientSplineMoveRoot::s_handler)(ClientSplineMoveRoot* msg);

// STATIC: 0x00E4D688
bool (*ClientSplineMoveSetFeatherFall::s_handler)(ClientSplineMoveSetFeatherFall* msg);

// STATIC: 0x00E4D81C
bool (*ClientSplineMoveSetFlightBackSpeed::s_handler)(ClientSplineMoveSetFlightBackSpeed* msg);

// STATIC: 0x00E4D68C
bool (*ClientSplineMoveSetFlightSpeed::s_handler)(ClientSplineMoveSetFlightSpeed* msg);

// STATIC: 0x00E4D604
bool (*ClientSplineMoveSetFlying::s_handler)(ClientSplineMoveSetFlying* msg);

// STATIC: 0x00E4D438
bool (*ClientSplineMoveSetHover::s_handler)(ClientSplineMoveSetHover* msg);

// STATIC: 0x00E4D76C
bool (*ClientSplineMoveSetLandWalk::s_handler)(ClientSplineMoveSetLandWalk* msg);

// STATIC: 0x00E4D73C
bool (*ClientSplineMoveSetNormalFall::s_handler)(ClientSplineMoveSetNormalFall* msg);

// STATIC: 0x00E4D638
bool (*ClientSplineMoveSetPitchRate::s_handler)(ClientSplineMoveSetPitchRate* msg);

// STATIC: 0x00E4D5C4
bool (*ClientSplineMoveSetRunBackSpeed::s_handler)(ClientSplineMoveSetRunBackSpeed* msg);

// STATIC: 0x00E4D57C
bool (*ClientSplineMoveSetRunMode::s_handler)(ClientSplineMoveSetRunMode* msg);

// STATIC: 0x00E4D740
bool (*ClientSplineMoveSetRunSpeed::s_handler)(ClientSplineMoveSetRunSpeed* msg);

// STATIC: 0x00E4D678
bool (*ClientSplineMoveSetSwimBackSpeed::s_handler)(ClientSplineMoveSetSwimBackSpeed* msg);

// STATIC: 0x00E4D69C
bool (*ClientSplineMoveSetSwimSpeed::s_handler)(ClientSplineMoveSetSwimSpeed* msg);

// STATIC: 0x00E4D6B0
bool (*ClientSplineMoveSetTurnRate::s_handler)(ClientSplineMoveSetTurnRate* msg);

// STATIC: 0x00E4D44C
bool (*ClientSplineMoveSetWalkMode::s_handler)(ClientSplineMoveSetWalkMode* msg);

// STATIC: 0x00E4D82C
bool (*ClientSplineMoveSetWalkSpeed::s_handler)(ClientSplineMoveSetWalkSpeed* msg);

// STATIC: 0x00E4D660
bool (*ClientSplineMoveSetWaterWalk::s_handler)(ClientSplineMoveSetWaterWalk* msg);

// STATIC: 0x00E4D828
bool (*ClientSplineMoveStartSwim::s_handler)(ClientSplineMoveStartSwim* msg);

// STATIC: 0x00E4D6BC
bool (*ClientSplineMoveStopSwim::s_handler)(ClientSplineMoveStopSwim* msg);

// STATIC: 0x00E4D5C8
bool (*ClientSplineMoveUnroot::s_handler)(ClientSplineMoveUnroot* msg);

// STATIC: 0x00E4D550
bool (*ClientSplineMoveUnsetFlying::s_handler)(ClientSplineMoveUnsetFlying* msg);

// STATIC: 0x00E4D628
bool (*ClientSplineMoveUnsetHover::s_handler)(ClientSplineMoveUnsetHover* msg);

// STATIC: 0x00E4D794
bool (*ClientUnit30A6::s_handler)(ClientUnit30A6* msg);

// STATIC: 0x00E4D7F4
bool (*ClientUnitClearSlotValue::s_handler)(ClientUnitClearSlotValue* msg);

// PROVEN: 14d411e4 IDENTICAL/NONE
// OFFSET: 0x006A2410
ClientMoveCollisionDisable::ClientMoveCollisionDisable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 80e7cadf IDENTICAL/NONE
// OFFSET: 0x006C6EC0/0x14037D910
ClientMoveCollisionDisable::ClientMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7beb2e40 IDENTICAL/NONE
// OFFSET: 0x006A19A0/0x14034ED60
ClientMoveCollisionDisable::~ClientMoveCollisionDisable() {
}

// PROVEN: b3085b98 IDENTICAL/NONE
// OFFSET: 0x006B0CD0/0x140361C20
void ClientMoveCollisionDisable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    {
        uint32_t value51 = 0;
        msg->Get(value51);
        this->m_ackIndex = value51;
    }
    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }
}

// PROVEN: 917e205c IDENTICAL/NONE
// OFFSET: 0x006AB620
bool ClientMoveCollisionDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a40c148e IDENTICAL/NONE
// OFFSET: 0x006A7E80
bool ClientMoveCollisionDisable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    {
        uint32_t value51 = 0;
        msg->Get(value51);
        this->m_ackIndex = value51;
    }
    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BFB60/0x140374CF0
bool ClientMoveCollisionDisable::Handle() {
    if (!ClientMoveCollisionDisable::s_handler) {
        return false;
    }

    return ClientMoveCollisionDisable::s_handler(this);
}

// PROVEN: e2bd4f00 IDENTICAL/NONE
// OFFSET: 0x006A1CE0
ClientMoveCollisionEnable::ClientMoveCollisionEnable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 40047fe6 IDENTICAL/NONE
// OFFSET: 0x006C54C0/0x14037B280
ClientMoveCollisionEnable::ClientMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0feb1c12 IDENTICAL/NONE
// OFFSET: 0x006A2ED0/0x1403503E0
ClientMoveCollisionEnable::~ClientMoveCollisionEnable() {
}

// PROVEN: 9e621a40 IDENTICAL/NONE
// OFFSET: 0x006B9210/0x14036C720
void ClientMoveCollisionEnable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[0]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[0] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }
}

// PROVEN: fda8a59d IDENTICAL/NONE
// OFFSET: 0x006C50C0
bool ClientMoveCollisionEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ba58f97a IDENTICAL/NONE
// OFFSET: 0x006AE9A0
bool ClientMoveCollisionEnable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[0]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[0] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C0D40/0x1403761D0
bool ClientMoveCollisionEnable::Handle() {
    if (!ClientMoveCollisionEnable::s_handler) {
        return false;
    }

    return ClientMoveCollisionEnable::s_handler(this);
}

// PROVEN: 1200b753 IDENTICAL/NONE
// OFFSET: 0x006A29B0
ClientMoveGravityDisable::ClientMoveGravityDisable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 4a25db8c IDENTICAL/NONE
// OFFSET: 0x006C6F10/0x14037D960
ClientMoveGravityDisable::ClientMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ede2728 IDENTICAL/NONE
// OFFSET: 0x006A2FB0/0x1403504B0
ClientMoveGravityDisable::~ClientMoveGravityDisable() {
}

// PROVEN: f5d72615 IDENTICAL/NONE
// OFFSET: 0x006BA4B0/0x14036DEB0
void ClientMoveGravityDisable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[0]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[0] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[5]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[5] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }
}

// PROVEN: 9b5862e9 IDENTICAL/NONE
// OFFSET: 0x006BF730
bool ClientMoveGravityDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 03b81940 IDENTICAL/NONE
// OFFSET: 0x006B0B60
bool ClientMoveGravityDisable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[0]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[0] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[5]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[5] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A34B0/0x140350CA0
bool ClientMoveGravityDisable::Handle() {
    if (!ClientMoveGravityDisable::s_handler) {
        return false;
    }

    return ClientMoveGravityDisable::s_handler(this);
}

// PROVEN: b3144733 IDENTICAL/NONE
// OFFSET: 0x006A1FA0
ClientMoveGravityEnable::ClientMoveGravityEnable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: eda89b6a IDENTICAL/NONE
// OFFSET: 0x006B9430/0x14036C9F0
ClientMoveGravityEnable::ClientMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e8833812 IDENTICAL/NONE
// OFFSET: 0x006A0F20/0x14034E000
ClientMoveGravityEnable::~ClientMoveGravityEnable() {
}

// PROVEN: 8b8fb955 IDENTICAL/NONE
// OFFSET: 0x006B1EE0/0x140363380
void ClientMoveGravityEnable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[7]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[7] ^= value19;
    }

    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[4]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[4] ^= value31;
    }

    if (guid[0]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[0] ^= value37;
    }

    if (guid[1]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[1] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }
}

// PROVEN: 7eb5ee86 IDENTICAL/NONE
// OFFSET: 0x006C8710
bool ClientMoveGravityEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 0401652d IDENTICAL/NONE
// OFFSET: 0x006B5A20
bool ClientMoveGravityEnable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[7]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[7] ^= value19;
    }

    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[4]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[4] ^= value31;
    }

    if (guid[0]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[0] ^= value37;
    }

    if (guid[1]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[1] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A8B20/0x140357A80
bool ClientMoveGravityEnable::Handle() {
    if (!ClientMoveGravityEnable::s_handler) {
        return false;
    }

    return ClientMoveGravityEnable::s_handler(this);
}

// PROVEN: 8ad49aaf IDENTICAL/NONE
// OFFSET: 0x006A2F50
ClientMoveKnockBack::ClientMoveKnockBack(WOWGUID guid, uint32_t ackIndex, const C2Vector& direction, float horizontalSpeed, float verticalSpeed) {
    this->m_guid = guid;
    this->m_horizontalSpeed = horizontalSpeed;
    this->m_ackIndex = ackIndex;
    this->m_verticalSpeed = verticalSpeed;
    this->m_direction = direction;
}

// PROVEN: b90b78bf IDENTICAL/NONE
// OFFSET: 0x006ADBE0/0x14035E010
ClientMoveKnockBack::ClientMoveKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: abab2fa5 IDENTICAL/NONE
// OFFSET: 0x006A23E0/0x14034F8F0
ClientMoveKnockBack::~ClientMoveKnockBack() {
}

// PROVEN: 4a099e15 IDENTICAL/NONE
// OFFSET: 0x006A66D0/0x140354A80
void ClientMoveKnockBack::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_direction.y = value15;
    }
    {
        uint32_t value18 = 0;
        msg->Get(value18);
        this->m_ackIndex = value18;
    }
    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    {
        float value33 = 0;
        msg->Get(value33);
        this->m_horizontalSpeed = value33;
    }
    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[5]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[5] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    {
        float value54 = 0;
        msg->Get(value54);
        this->m_verticalSpeed = value54;
    }
    {
        float value57 = 0;
        msg->Get(value57);
        this->m_direction.x = value57;
    }
    if (guid[2]) {
        uint8_t value61 = 0;
        msg->Get(value61);
        guid[2] ^= value61;
    }

    if (guid[0]) {
        uint8_t value67 = 0;
        msg->Get(value67);
        guid[0] ^= value67;
    }
}

// PROVEN: b2ddde43 IDENTICAL/NONE
// OFFSET: 0x006C87F0
bool ClientMoveKnockBack::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5e34d84f IDENTICAL/NONE
// OFFSET: 0x006B3620
bool ClientMoveKnockBack::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_direction.y = value15;
    }
    {
        uint32_t value18 = 0;
        msg->Get(value18);
        this->m_ackIndex = value18;
    }
    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    {
        float value33 = 0;
        msg->Get(value33);
        this->m_horizontalSpeed = value33;
    }
    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[5]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[5] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    {
        float value54 = 0;
        msg->Get(value54);
        this->m_verticalSpeed = value54;
    }
    {
        float value57 = 0;
        msg->Get(value57);
        this->m_direction.x = value57;
    }
    if (guid[2]) {
        uint8_t value61 = 0;
        msg->Get(value61);
        guid[2] ^= value61;
    }

    if (guid[0]) {
        uint8_t value67 = 0;
        msg->Get(value67);
        guid[0] ^= value67;
    }

    return msg->IsRead() && this->IsValid();
}

// OFFSET: 0x006A28D0
bool ClientMoveKnockBack::IsValid() const {
    if (std::fabs(this->m_horizontalSpeed) < 2.3841858e-7f && std::fabs(this->m_verticalSpeed) < 2.3841858e-7f) {
        return false;
    }

    if (!_finite(this->m_verticalSpeed) || !_finite(this->m_horizontalSpeed)) {
        return false;
    }

    if (std::fabs(this->m_direction.x * this->m_direction.x + this->m_direction.y * this->m_direction.y - 1.0f) >= 1.0e-6f && std::fabs(this->m_direction.x * this->m_direction.x + this->m_direction.y * this->m_direction.y) >= 1.0e-6f) {
        return false;
    }

    return _finite(this->m_direction.y) && _finite(this->m_direction.x);
}

// OFFSET: 0x006A7540/0x140355FE0
bool ClientMoveKnockBack::Handle() {
    if (!ClientMoveKnockBack::s_handler) {
        return false;
    }

    return ClientMoveKnockBack::s_handler(this);
}

// PROVEN: 619c80be IDENTICAL/NONE
// OFFSET: 0x006A3C50
ClientMoveRoot::ClientMoveRoot(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 76e7e259 IDENTICAL/NONE
// OFFSET: 0x006C05B0/0x140375880
ClientMoveRoot::ClientMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7c524104 IDENTICAL/NONE
// OFFSET: 0x006A1B70/0x14034F030
ClientMoveRoot::~ClientMoveRoot() {
}

// PROVEN: cb8cd089 IDENTICAL/NONE
// OFFSET: 0x006B14A0/0x140362630
void ClientMoveRoot::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }
}

// PROVEN: 35921c4e IDENTICAL/NONE
// OFFSET: 0x006BB400
bool ClientMoveRoot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 77c70b26 IDENTICAL/NONE
// OFFSET: 0x006A51F0
bool ClientMoveRoot::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B3C10/0x1403658A0
bool ClientMoveRoot::Handle() {
    if (!ClientMoveRoot::s_handler) {
        return false;
    }

    return ClientMoveRoot::s_handler(this);
}

// PROVEN: 927e999c IDENTICAL/NONE
// OFFSET: 0x006A10C0
ClientMoveSetCanFly::ClientMoveSetCanFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: fd6f3024 IDENTICAL/NONE
// OFFSET: 0x006C5F60/0x14037C380
ClientMoveSetCanFly::ClientMoveSetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5433cb5b IDENTICAL/NONE
// OFFSET: 0x006A1E00/0x14034F270
ClientMoveSetCanFly::~ClientMoveSetCanFly() {
}

// PROVEN: 501b94c6 IDENTICAL/NONE
// OFFSET: 0x006B3400/0x140364E60
void ClientMoveSetCanFly::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[2]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[2] ^= value25;
    }

    if (guid[1]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[1] ^= value31;
    }

    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[7]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[7] ^= value43;
    }

    if (guid[0]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[0] ^= value49;
    }

    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }
}

// PROVEN: 98a7ea07 IDENTICAL/NONE
// OFFSET: 0x006C07B0
bool ClientMoveSetCanFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e7de905f IDENTICAL/NONE
// OFFSET: 0x006BBBB0
bool ClientMoveSetCanFly::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[2]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[2] ^= value25;
    }

    if (guid[1]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[1] ^= value31;
    }

    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[7]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[7] ^= value43;
    }

    if (guid[0]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[0] ^= value49;
    }

    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B2080/0x1403635B0
bool ClientMoveSetCanFly::Handle() {
    if (!ClientMoveSetCanFly::s_handler) {
        return false;
    }

    return ClientMoveSetCanFly::s_handler(this);
}

// PROVEN: 151b66cb IDENTICAL/NONE
// OFFSET: 0x006A1CA0
ClientMoveSetCanTransitionBetweenSwimAndFly::ClientMoveSetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: d3d29216 IDENTICAL/NONE
// OFFSET: 0x006C9A50/0x1403814B0
ClientMoveSetCanTransitionBetweenSwimAndFly::ClientMoveSetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a0b81e8a IDENTICAL/NONE
// OFFSET: 0x006A0F10/0x14034DFF0
ClientMoveSetCanTransitionBetweenSwimAndFly::~ClientMoveSetCanTransitionBetweenSwimAndFly() {
}

// PROVEN: 89b2c708 IDENTICAL/NONE
// OFFSET: 0x006B9FA0/0x14036D860
void ClientMoveSetCanTransitionBetweenSwimAndFly::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    {
        uint32_t value51 = 0;
        msg->Get(value51);
        this->m_ackIndex = value51;
    }
    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }
}

// PROVEN: 78ee688d IDENTICAL/NONE
// OFFSET: 0x006CFCA0
bool ClientMoveSetCanTransitionBetweenSwimAndFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d374ed1d IDENTICAL/NONE
// OFFSET: 0x006C7F90
bool ClientMoveSetCanTransitionBetweenSwimAndFly::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    {
        uint32_t value51 = 0;
        msg->Get(value51);
        this->m_ackIndex = value51;
    }
    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A4890/0x1403523E0
bool ClientMoveSetCanTransitionBetweenSwimAndFly::Handle() {
    if (!ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler) {
        return false;
    }

    return ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler(this);
}

// PROVEN: c9cc0e19 IDENTICAL/NONE
// OFFSET: 0x006A1050
ClientMoveSetCollisionHeight::ClientMoveSetCollisionHeight(WOWGUID guid, uint32_t ackIndex, float height, uint32_t reason) {
    this->m_guid = guid;
    this->m_height = height;
    this->m_ackIndex = ackIndex;
    this->m_reason = reason;
}

// PROVEN: 01adc0df IDENTICAL/NONE
// OFFSET: 0x006C0E20/0x140376310
ClientMoveSetCollisionHeight::ClientMoveSetCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 73ea8b6d IDENTICAL/NONE
// OFFSET: 0x006A1690/0x14034E9D0
ClientMoveSetCollisionHeight::~ClientMoveSetCollisionHeight() {
}

// OFFSET: 0x006A81B0/0x140356EA0
void ClientMoveSetCollisionHeight::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    this->m_reason = bits.ReadBits<2>();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[6] ^= value11;
    }

    if (guid[0]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[0] ^= value17;
    }

    if (guid[4]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[4] ^= value23;
    }

    if (guid[3]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[3] ^= value29;
    }

    if (guid[5]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[5] ^= value35;
    }

    {
        uint32_t value40 = 0;
        msg->Get(value40);
        this->m_ackIndex = value40;
    }
    if (guid[1]) {
        uint8_t value44 = 0;
        msg->Get(value44);
        guid[1] ^= value44;
    }

    if (guid[2]) {
        uint8_t value50 = 0;
        msg->Get(value50);
        guid[2] ^= value50;
    }

    if (guid[7]) {
        uint8_t value56 = 0;
        msg->Get(value56);
        guid[7] ^= value56;
    }

    {
        float value61 = 0;
        msg->Get(value61);
        this->m_height = value61;
    }
}

// PROVEN: ee4fea34 IDENTICAL/NONE
// OFFSET: 0x006B72C0
bool ClientMoveSetCollisionHeight::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006B6ED0
bool ClientMoveSetCollisionHeight::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    this->m_reason = bits.ReadBits<2>();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[6] ^= value11;
    }

    if (guid[0]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[0] ^= value17;
    }

    if (guid[4]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[4] ^= value23;
    }

    if (guid[3]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[3] ^= value29;
    }

    if (guid[5]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[5] ^= value35;
    }

    {
        uint32_t value40 = 0;
        msg->Get(value40);
        this->m_ackIndex = value40;
    }
    if (guid[1]) {
        uint8_t value44 = 0;
        msg->Get(value44);
        guid[1] ^= value44;
    }

    if (guid[2]) {
        uint8_t value50 = 0;
        msg->Get(value50);
        guid[2] ^= value50;
    }

    if (guid[7]) {
        uint8_t value56 = 0;
        msg->Get(value56);
        guid[7] ^= value56;
    }

    {
        float value61 = 0;
        msg->Get(value61);
        this->m_height = value61;
    }

    return msg->IsRead() && _finite(this->m_height) && this->m_reason <= 2;
}

// OFFSET: 0x006A5BB0/0x140353B50
bool ClientMoveSetCollisionHeight::Handle() {
    if (!ClientMoveSetCollisionHeight::s_handler) {
        return false;
    }

    return ClientMoveSetCollisionHeight::s_handler(this);
}

// PROVEN: a60416d8 IDENTICAL/NONE
// OFFSET: 0x006A2AA0
ClientMoveSetFeatherFall::ClientMoveSetFeatherFall(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 1b8328c4 IDENTICAL/NONE
// OFFSET: 0x006C6F90/0x14037D9E0
ClientMoveSetFeatherFall::ClientMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3851f197 IDENTICAL/NONE
// OFFSET: 0x006A1FE0/0x14034F450
ClientMoveSetFeatherFall::~ClientMoveSetFeatherFall() {
}

// PROVEN: bc8e3f8c IDENTICAL/NONE
// OFFSET: 0x006AB8E0/0x14035B540
void ClientMoveSetFeatherFall::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[1]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[1] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }
}

// PROVEN: 92fbbad0 IDENTICAL/NONE
// OFFSET: 0x006C5120
bool ClientMoveSetFeatherFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1ad03a79 IDENTICAL/NONE
// OFFSET: 0x006AEF70
bool ClientMoveSetFeatherFall::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[1]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[1] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AE7D0/0x14035EEE0
bool ClientMoveSetFeatherFall::Handle() {
    if (!ClientMoveSetFeatherFall::s_handler) {
        return false;
    }

    return ClientMoveSetFeatherFall::s_handler(this);
}

// PROVEN: d0d84d45 IDENTICAL/NONE
// OFFSET: 0x006A7210
ClientMoveSetFlightBackSpeed::ClientMoveSetFlightBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 2b01b4ec IDENTICAL/NONE
// OFFSET: 0x006C6030/0x14037C450
ClientMoveSetFlightBackSpeed::ClientMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5bc26de3 IDENTICAL/NONE
// OFFSET: 0x006A2380/0x14034F8A0
ClientMoveSetFlightBackSpeed::~ClientMoveSetFlightBackSpeed() {
}

// PROVEN: 959c8edf IDENTICAL/NONE
// OFFSET: 0x006B3C90/0x140365960
void ClientMoveSetFlightBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    {
        float value24 = 0;
        msg->Get(value24);
        this->m_speed = value24;
    }
    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }

    if (guid[7]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[7] ^= value58;
    }
}

// PROVEN: 282fb0b7 IDENTICAL/NONE
// OFFSET: 0x006D0460
bool ClientMoveSetFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f29ecf4f IDENTICAL/NONE
// OFFSET: 0x006BF540
bool ClientMoveSetFlightBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    {
        float value24 = 0;
        msg->Get(value24);
        this->m_speed = value24;
    }
    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }

    if (guid[7]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[7] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B1A60/0x140362DB0
bool ClientMoveSetFlightBackSpeed::Handle() {
    if (!ClientMoveSetFlightBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetFlightBackSpeed::s_handler(this);
}

// PROVEN: 85f56437 IDENTICAL/NONE
// OFFSET: 0x006A2BA0
ClientMoveSetFlightSpeed::ClientMoveSetFlightSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 3a9e9e88 IDENTICAL/NONE
// OFFSET: 0x006C8410/0x14037F440
ClientMoveSetFlightSpeed::ClientMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: af9ead83 IDENTICAL/NONE
// OFFSET: 0x006A2B10/0x140350050
ClientMoveSetFlightSpeed::~ClientMoveSetFlightSpeed() {
}

// PROVEN: 05482758 IDENTICAL/NONE
// OFFSET: 0x006B3820/0x140365390
void ClientMoveSetFlightSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    {
        float value33 = 0;
        msg->Get(value33);
        this->m_speed = value33;
    }
    {
        uint32_t value36 = 0;
        msg->Get(value36);
        this->m_ackIndex = value36;
    }
    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }
}

// PROVEN: ec443e5f IDENTICAL/NONE
// OFFSET: 0x006C48B0
bool ClientMoveSetFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b4c03838 IDENTICAL/NONE
// OFFSET: 0x006C0600
bool ClientMoveSetFlightSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    {
        float value33 = 0;
        msg->Get(value33);
        this->m_speed = value33;
    }
    {
        uint32_t value36 = 0;
        msg->Get(value36);
        this->m_ackIndex = value36;
    }
    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B8160/0x14036B1F0
bool ClientMoveSetFlightSpeed::Handle() {
    if (!ClientMoveSetFlightSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetFlightSpeed::s_handler(this);
}

// PROVEN: 1cc6263a IDENTICAL/NONE
// OFFSET: 0x006A2340
ClientMoveSetHover::ClientMoveSetHover(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 9526e212 IDENTICAL/NONE
// OFFSET: 0x006C7F40/0x14037EE80
ClientMoveSetHover::ClientMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 88c63a6f IDENTICAL/NONE
// OFFSET: 0x006A2450/0x14034F950
ClientMoveSetHover::~ClientMoveSetHover() {
}

// PROVEN: b43b45fe IDENTICAL/NONE
// OFFSET: 0x006BA610/0x14036E080
void ClientMoveSetHover::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }
}

// PROVEN: 296e79fe IDENTICAL/NONE
// OFFSET: 0x006C5F50
bool ClientMoveSetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: def8fea0 IDENTICAL/NONE
// OFFSET: 0x006AB630
bool ClientMoveSetHover::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B6450/0x140368C00
bool ClientMoveSetHover::Handle() {
    if (!ClientMoveSetHover::s_handler) {
        return false;
    }

    return ClientMoveSetHover::s_handler(this);
}

// PROVEN: eb38b2c3 IDENTICAL/NONE
// OFFSET: 0x006A68A0
ClientMoveSetLandWalk::ClientMoveSetLandWalk(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 63302a1b IDENTICAL/NONE
// OFFSET: 0x006CB670/0x140383D30
ClientMoveSetLandWalk::ClientMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 499bf972 IDENTICAL/NONE
// OFFSET: 0x006A1620/0x14034E920
ClientMoveSetLandWalk::~ClientMoveSetLandWalk() {
}

// PROVEN: 2bda9e81 IDENTICAL/NONE
// OFFSET: 0x006BE160/0x140372D80
void ClientMoveSetLandWalk::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }
}

// PROVEN: 208bd6e1 IDENTICAL/NONE
// OFFSET: 0x006C6F00
bool ClientMoveSetLandWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9b78ef13 IDENTICAL/NONE
// OFFSET: 0x006C3E10
bool ClientMoveSetLandWalk::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006ABE50/0x14035BB80
bool ClientMoveSetLandWalk::Handle() {
    if (!ClientMoveSetLandWalk::s_handler) {
        return false;
    }

    return ClientMoveSetLandWalk::s_handler(this);
}

// PROVEN: ccfccc94 IDENTICAL/NONE
// OFFSET: 0x006A2DF0
ClientMoveSetNormalFall::ClientMoveSetNormalFall(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: fea953ca IDENTICAL/NONE
// OFFSET: 0x006B5D90/0x140368330
ClientMoveSetNormalFall::ClientMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 94ad6c9e IDENTICAL/NONE
// OFFSET: 0x006A1EF0/0x14034F370
ClientMoveSetNormalFall::~ClientMoveSetNormalFall() {
}

// PROVEN: 58ae70ce IDENTICAL/NONE
// OFFSET: 0x006A6BC0/0x140355090
void ClientMoveSetNormalFall::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    {
        uint32_t value0 = 0;
        msg->Get(value0);
        this->m_ackIndex = value0;
    }
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[2] ^= value13;
    }

    if (guid[7]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[7] ^= value19;
    }

    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[4]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[4] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[0]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[0] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }
}

// PROVEN: 72e3ff7c IDENTICAL/NONE
// OFFSET: 0x006D3860
bool ClientMoveSetNormalFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 198c9053 IDENTICAL/NONE
// OFFSET: 0x006CB8A0
bool ClientMoveSetNormalFall::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    {
        uint32_t value0 = 0;
        msg->Get(value0);
        this->m_ackIndex = value0;
    }
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[2] ^= value13;
    }

    if (guid[7]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[7] ^= value19;
    }

    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[4]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[4] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[0]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[0] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B4560/0x1403664C0
bool ClientMoveSetNormalFall::Handle() {
    if (!ClientMoveSetNormalFall::s_handler) {
        return false;
    }

    return ClientMoveSetNormalFall::s_handler(this);
}

// PROVEN: 9cb17202 IDENTICAL/NONE
// OFFSET: 0x006A1000
ClientMoveSetPitchRate::ClientMoveSetPitchRate(WOWGUID guid, uint32_t ackIndex, float rate) {
    this->m_guid = guid;
    this->m_rate = rate;
    this->m_ackIndex = ackIndex;
}

// PROVEN: dfd40978 IDENTICAL/NONE
// OFFSET: 0x006B47A0/0x1403667C0
ClientMoveSetPitchRate::ClientMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 05bc15af IDENTICAL/NONE
// OFFSET: 0x006A0EB0/0x14034DF70
ClientMoveSetPitchRate::~ClientMoveSetPitchRate() {
}

// PROVEN: c726a058 IDENTICAL/NONE
// OFFSET: 0x006A9E70/0x1403593B0
void ClientMoveSetPitchRate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_rate = value9;
    }
    if (guid[6]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[6] ^= value13;
    }

    if (guid[4]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[4] ^= value19;
    }

    if (guid[0]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[0] ^= value25;
    }

    {
        uint32_t value30 = 0;
        msg->Get(value30);
        this->m_ackIndex = value30;
    }
    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    if (guid[5]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[5] ^= value58;
    }
}

// PROVEN: 3c8b2523 IDENTICAL/NONE
// OFFSET: 0x006D30B0
bool ClientMoveSetPitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2761618b IDENTICAL/NONE
// OFFSET: 0x006C41E0
bool ClientMoveSetPitchRate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_rate = value9;
    }
    if (guid[6]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[6] ^= value13;
    }

    if (guid[4]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[4] ^= value19;
    }

    if (guid[0]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[0] ^= value25;
    }

    {
        uint32_t value30 = 0;
        msg->Get(value30);
        this->m_ackIndex = value30;
    }
    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    if (guid[5]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[5] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_rate);
}

// OFFSET: 0x006AB890/0x14035B4E0
bool ClientMoveSetPitchRate::Handle() {
    if (!ClientMoveSetPitchRate::s_handler) {
        return false;
    }

    return ClientMoveSetPitchRate::s_handler(this);
}

// PROVEN: 42afe1bc IDENTICAL/NONE
// OFFSET: 0x006A1240
ClientMoveSetRunBackSpeed::ClientMoveSetRunBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 7d68c2af IDENTICAL/NONE
// OFFSET: 0x006C3DC0/0x140379D50
ClientMoveSetRunBackSpeed::ClientMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 612185b7 IDENTICAL/NONE
// OFFSET: 0x006A2330/0x14034F860
ClientMoveSetRunBackSpeed::~ClientMoveSetRunBackSpeed() {
}

// PROVEN: 5d67cb88 IDENTICAL/NONE
// OFFSET: 0x006B9B20/0x14036D2B0
void ClientMoveSetRunBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    {
        float value18 = 0;
        msg->Get(value18);
        this->m_speed = value18;
    }
    if (guid[0]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[0] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }

    if (guid[6]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[6] ^= value58;
    }
}

// PROVEN: b6ae2303 IDENTICAL/NONE
// OFFSET: 0x006D7720
bool ClientMoveSetRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: db34f697 IDENTICAL/NONE
// OFFSET: 0x006CF0E0
bool ClientMoveSetRunBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    {
        float value18 = 0;
        msg->Get(value18);
        this->m_speed = value18;
    }
    if (guid[0]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[0] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }

    if (guid[6]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[6] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006ACE70/0x14035CEF0
bool ClientMoveSetRunBackSpeed::Handle() {
    if (!ClientMoveSetRunBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetRunBackSpeed::s_handler(this);
}

// PROVEN: b45b2acd IDENTICAL/NONE
// OFFSET: 0x006A15E0
ClientMoveSetRunSpeed::ClientMoveSetRunSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 00e52bb6 IDENTICAL/NONE
// OFFSET: 0x006CA860/0x140382930
ClientMoveSetRunSpeed::ClientMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8350f0e2 IDENTICAL/NONE
// OFFSET: 0x006A24C0/0x14034F9E0
ClientMoveSetRunSpeed::~ClientMoveSetRunSpeed() {
}

// PROVEN: 087d219b IDENTICAL/NONE
// OFFSET: 0x006B6200/0x140368900
void ClientMoveSetRunSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    {
        float value36 = 0;
        msg->Get(value36);
        this->m_speed = value36;
    }
    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[2]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[2] ^= value58;
    }
}

// PROVEN: 4515e61c IDENTICAL/NONE
// OFFSET: 0x006C5FA0
bool ClientMoveSetRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 14099eae IDENTICAL/NONE
// OFFSET: 0x006AD190
bool ClientMoveSetRunSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    {
        float value36 = 0;
        msg->Get(value36);
        this->m_speed = value36;
    }
    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[2]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[2] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006AE220/0x14035E770
bool ClientMoveSetRunSpeed::Handle() {
    if (!ClientMoveSetRunSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetRunSpeed::s_handler(this);
}

// PROVEN: f9ea59f2 IDENTICAL/NONE
// OFFSET: 0x006A45A0
ClientMoveSetSwimBackSpeed::ClientMoveSetSwimBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 34247677 IDENTICAL/NONE
// OFFSET: 0x006C0C40/0x1403760B0
ClientMoveSetSwimBackSpeed::ClientMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ac7260c IDENTICAL/NONE
// OFFSET: 0x006A2FC0/0x1403504C0
ClientMoveSetSwimBackSpeed::~ClientMoveSetSwimBackSpeed() {
}

// PROVEN: cdb81759 IDENTICAL/NONE
// OFFSET: 0x006B2ED0/0x140364800
void ClientMoveSetSwimBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    {
        uint32_t value9 = 0;
        msg->Get(value9);
        this->m_ackIndex = value9;
    }
    if (guid[0]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[0] ^= value13;
    }

    if (guid[3]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[3] ^= value19;
    }

    if (guid[4]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[4] ^= value25;
    }

    if (guid[6]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[6] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[1]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[1] ^= value43;
    }

    {
        float value48 = 0;
        msg->Get(value48);
        this->m_speed = value48;
    }
    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[2]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[2] ^= value58;
    }
}

// PROVEN: 455fbda2 IDENTICAL/NONE
// OFFSET: 0x006BF830
bool ClientMoveSetSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 47223aa0 IDENTICAL/NONE
// OFFSET: 0x006ACBD0
bool ClientMoveSetSwimBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    {
        uint32_t value9 = 0;
        msg->Get(value9);
        this->m_ackIndex = value9;
    }
    if (guid[0]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[0] ^= value13;
    }

    if (guid[3]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[3] ^= value19;
    }

    if (guid[4]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[4] ^= value25;
    }

    if (guid[6]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[6] ^= value31;
    }

    if (guid[5]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[5] ^= value37;
    }

    if (guid[1]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[1] ^= value43;
    }

    {
        float value48 = 0;
        msg->Get(value48);
        this->m_speed = value48;
    }
    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[2]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[2] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B5400/0x1403676F0
bool ClientMoveSetSwimBackSpeed::Handle() {
    if (!ClientMoveSetSwimBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetSwimBackSpeed::s_handler(this);
}

// PROVEN: 347a5d30 IDENTICAL/NONE
// OFFSET: 0x006A2390
ClientMoveSetSwimSpeed::ClientMoveSetSwimSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 2a3981d0 IDENTICAL/NONE
// OFFSET: 0x006C46D0/0x14037A5D0
ClientMoveSetSwimSpeed::ClientMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ecfa107f IDENTICAL/NONE
// OFFSET: 0x006A28C0/0x14034FD40
ClientMoveSetSwimSpeed::~ClientMoveSetSwimSpeed() {
}

// PROVEN: 6785133b IDENTICAL/NONE
// OFFSET: 0x006B28B0/0x140363F80
void ClientMoveSetSwimSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[5]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[5] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    {
        float value42 = 0;
        msg->Get(value42);
        this->m_speed = value42;
    }
    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }
}

// PROVEN: 32617c54 IDENTICAL/NONE
// OFFSET: 0x006C46C0
bool ClientMoveSetSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3d06b3bc IDENTICAL/NONE
// OFFSET: 0x006C43B0
bool ClientMoveSetSwimSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    {
        uint32_t value15 = 0;
        msg->Get(value15);
        this->m_ackIndex = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[5]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[5] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    {
        float value42 = 0;
        msg->Get(value42);
        this->m_speed = value42;
    }
    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006A6690/0x140354A20
bool ClientMoveSetSwimSpeed::Handle() {
    if (!ClientMoveSetSwimSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetSwimSpeed::s_handler(this);
}

// PROVEN: 7e220f79 IDENTICAL/NONE
// OFFSET: 0x006A7960
ClientMoveSetTurnRate::ClientMoveSetTurnRate(WOWGUID guid, uint32_t ackIndex, float rate) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_rate = rate;
}

// PROVEN: 8a1f649b IDENTICAL/NONE
// OFFSET: 0x006AC0E0/0x14035BEE0
ClientMoveSetTurnRate::ClientMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 339b9420 IDENTICAL/NONE
// OFFSET: 0x006A10F0/0x14034E320
ClientMoveSetTurnRate::~ClientMoveSetTurnRate() {
}

// PROVEN: 352c18f5 IDENTICAL/NONE
// OFFSET: 0x006AA640/0x140359D00
void ClientMoveSetTurnRate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        float value27 = 0;
        msg->Get(value27);
        this->m_rate = value27;
    }
    if (guid[3]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[3] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[0]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[0] ^= value43;
    }

    {
        uint32_t value48 = 0;
        msg->Get(value48);
        this->m_ackIndex = value48;
    }
    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }
}

// PROVEN: fc687abb IDENTICAL/NONE
// OFFSET: 0x006C9930
bool ClientMoveSetTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e94ff991 IDENTICAL/NONE
// OFFSET: 0x006C5130
bool ClientMoveSetTurnRate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        float value27 = 0;
        msg->Get(value27);
        this->m_rate = value27;
    }
    if (guid[3]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[3] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[0]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[0] ^= value43;
    }

    {
        uint32_t value48 = 0;
        msg->Get(value48);
        this->m_ackIndex = value48;
    }
    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    if (guid[4]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[4] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_rate);
}

// OFFSET: 0x006B2E50/0x140364740
bool ClientMoveSetTurnRate::Handle() {
    if (!ClientMoveSetTurnRate::s_handler) {
        return false;
    }

    return ClientMoveSetTurnRate::s_handler(this);
}

// PROVEN: 7462773e IDENTICAL/NONE
// OFFSET: 0x006A1E80
ClientMoveSetVehicleRecId::ClientMoveSetVehicleRecId(WOWGUID guid, uint32_t ackIndex, uint32_t vehicleRecId) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_vehicleRecId = vehicleRecId;
}

// PROVEN: 62b3150b IDENTICAL/NONE
// OFFSET: 0x006B7D00/0x14036AC70
ClientMoveSetVehicleRecId::ClientMoveSetVehicleRecId(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 30d9a879 IDENTICAL/NONE
// OFFSET: 0x006A1110/0x14034E340
ClientMoveSetVehicleRecId::~ClientMoveSetVehicleRecId() {
}

// PROVEN: 1d5a817d IDENTICAL/NONE
// OFFSET: 0x006A34F0/0x140350D00
void ClientMoveSetVehicleRecId::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    {
        uint32_t value0 = 0;
        msg->Get(value0);
        this->m_ackIndex = value0;
    }
    {
        uint32_t value3 = 0;
        msg->Get(value3);
        this->m_vehicleRecId = value3;
    }
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }

    if (guid[0]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[0] ^= value58;
    }
}

// PROVEN: 657e45ec IDENTICAL/NONE
// OFFSET: 0x006C61A0
bool ClientMoveSetVehicleRecId::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ef1818f4 IDENTICAL/NONE
// OFFSET: 0x006B74D0
bool ClientMoveSetVehicleRecId::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    {
        uint32_t value0 = 0;
        msg->Get(value0);
        this->m_ackIndex = value0;
    }
    {
        uint32_t value3 = 0;
        msg->Get(value3);
        this->m_vehicleRecId = value3;
    }
    JamBitReader bits(msg);
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }

    if (guid[0]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[0] ^= value58;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006ABE10/0x14035BAD0
bool ClientMoveSetVehicleRecId::Handle() {
    if (!ClientMoveSetVehicleRecId::s_handler) {
        return false;
    }

    return ClientMoveSetVehicleRecId::s_handler(this);
}

// PROVEN: 8d56423c IDENTICAL/NONE
// OFFSET: 0x006A2F00
ClientMoveSetWalkSpeed::ClientMoveSetWalkSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: c9830330 IDENTICAL/NONE
// OFFSET: 0x006AA530/0x140359BC0
ClientMoveSetWalkSpeed::ClientMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d56cecdc IDENTICAL/NONE
// OFFSET: 0x006A2A10/0x14034FF00
ClientMoveSetWalkSpeed::~ClientMoveSetWalkSpeed() {
}

// PROVEN: 7ee3ea6a IDENTICAL/NONE
// OFFSET: 0x006A7720/0x140356260
void ClientMoveSetWalkSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[5]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[5] ^= value22;
    }

    {
        float value27 = 0;
        msg->Get(value27);
        this->m_speed = value27;
    }
    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    {
        uint32_t value36 = 0;
        msg->Get(value36);
        this->m_ackIndex = value36;
    }
    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[3]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[3] ^= value58;
    }
}

// PROVEN: 1f0ebf53 IDENTICAL/NONE
// OFFSET: 0x006C0E60
bool ClientMoveSetWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f02af77e IDENTICAL/NONE
// OFFSET: 0x006B7310
bool ClientMoveSetWalkSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[5]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[5] ^= value22;
    }

    {
        float value27 = 0;
        msg->Get(value27);
        this->m_speed = value27;
    }
    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    {
        uint32_t value36 = 0;
        msg->Get(value36);
        this->m_ackIndex = value36;
    }
    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[7] ^= value52;
    }

    if (guid[3]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[3] ^= value58;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006BD9D0/0x140372390
bool ClientMoveSetWalkSpeed::Handle() {
    if (!ClientMoveSetWalkSpeed::s_handler) {
        return false;
    }

    return ClientMoveSetWalkSpeed::s_handler(this);
}

// PROVEN: 2d20c8fe IDENTICAL/NONE
// OFFSET: 0x006A1370
ClientMoveSetWaterWalk::ClientMoveSetWaterWalk(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 6468136f IDENTICAL/NONE
// OFFSET: 0x006BBFA0/0x1403701C0
ClientMoveSetWaterWalk::ClientMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fed7fe41 IDENTICAL/NONE
// OFFSET: 0x006A14F0/0x14034E7C0
ClientMoveSetWaterWalk::~ClientMoveSetWaterWalk() {
}

// PROVEN: 2e7f5ad9 IDENTICAL/NONE
// OFFSET: 0x006B6860/0x1403692F0
void ClientMoveSetWaterWalk::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[7]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[7] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[1]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[1] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }
}

// PROVEN: b0255e79 IDENTICAL/NONE
// OFFSET: 0x006C9130
bool ClientMoveSetWaterWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 197b37da IDENTICAL/NONE
// OFFSET: 0x006B4360
bool ClientMoveSetWaterWalk::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[7]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[7] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[1]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[1] ^= value49;
    }

    if (guid[6]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[6] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AA080/0x140359660
bool ClientMoveSetWaterWalk::Handle() {
    if (!ClientMoveSetWaterWalk::s_handler) {
        return false;
    }

    return ClientMoveSetWaterWalk::s_handler(this);
}

// PROVEN: 85cfaf5f IDENTICAL/NONE
// OFFSET: 0x006AC7E0/0x14035C670
ClientMoveSkipTime::ClientMoveSkipTime(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 96060234 IDENTICAL/NONE
// OFFSET: 0x006A1DA0/0x14034F220
ClientMoveSkipTime::~ClientMoveSkipTime() {
}

// PROVEN: e13494af IDENTICAL/NONE
// OFFSET: 0x006A4440/0x140351E60
void ClientMoveSkipTime::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    {
        uint32_t value9 = 0;
        msg->Get(value9);
        this->m_time = value9;
    }
    if (guid[0]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[0] ^= value13;
    }

    if (guid[4]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[4] ^= value19;
    }

    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[5]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[5] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }
}

// PROVEN: 63363caa IDENTICAL/NONE
// OFFSET: 0x006D95F0
bool ClientMoveSkipTime::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: edb7f816 IDENTICAL/NONE
// OFFSET: 0x006CEEE0
bool ClientMoveSkipTime::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    {
        uint32_t value9 = 0;
        msg->Get(value9);
        this->m_time = value9;
    }
    if (guid[0]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[0] ^= value13;
    }

    if (guid[4]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[4] ^= value19;
    }

    if (guid[6]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[6] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[1]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[1] ^= value37;
    }

    if (guid[5]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[5] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A7250/0x140355C70
bool ClientMoveSkipTime::Handle() {
    if (!ClientMoveSkipTime::s_handler) {
        return false;
    }

    return ClientMoveSkipTime::s_handler(this);
}

// PROVEN: 76553b1c IDENTICAL/NONE
// OFFSET: 0x006C6110/0x14037C530
ClientMoveTeleport::ClientMoveTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTransport = false;
    this->m_hasVehicle = false;
    this->Unpack(msg, true);
}

// PROVEN: 21fcefc8 IDENTICAL/NONE
// OFFSET: 0x006CCA20/0x140385B70
ClientMoveTeleport::~ClientMoveTeleport() {
}

// OFFSET: 0x006B22B0/0x140363890
void ClientMoveTeleport::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_transportGuid);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_hasVehicle = bits.ReadBit();
    if (this->m_hasVehicle) {
        this->m_exitVoluntary = bits.ReadBit();
        this->m_awaitTransition = bits.ReadBit();
    }

    this->m_hasTransport = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_hasTransport) {
        transportGuid[1] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
    }

    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (this->m_hasTransport) {
        if (transportGuid[5]) {
            uint8_t value29 = 0;
            msg->Get(value29);
            transportGuid[5] ^= value29;
        }

        if (transportGuid[6]) {
            uint8_t value35 = 0;
            msg->Get(value35);
            transportGuid[6] ^= value35;
        }

        if (transportGuid[1]) {
            uint8_t value41 = 0;
            msg->Get(value41);
            transportGuid[1] ^= value41;
        }

        if (transportGuid[7]) {
            uint8_t value47 = 0;
            msg->Get(value47);
            transportGuid[7] ^= value47;
        }

        if (transportGuid[0]) {
            uint8_t value53 = 0;
            msg->Get(value53);
            transportGuid[0] ^= value53;
        }

        if (transportGuid[2]) {
            uint8_t value59 = 0;
            msg->Get(value59);
            transportGuid[2] ^= value59;
        }

        if (transportGuid[4]) {
            uint8_t value65 = 0;
            msg->Get(value65);
            transportGuid[4] ^= value65;
        }

        if (transportGuid[3]) {
            uint8_t value71 = 0;
            msg->Get(value71);
            transportGuid[3] ^= value71;
        }
    }

    {
        uint32_t value78 = 0;
        msg->Get(value78);
        this->m_ackIndex = value78;
    }
    if (guid[1]) {
        uint8_t value82 = 0;
        msg->Get(value82);
        guid[1] ^= value82;
    }

    if (guid[2]) {
        uint8_t value88 = 0;
        msg->Get(value88);
        guid[2] ^= value88;
    }

    if (guid[3]) {
        uint8_t value94 = 0;
        msg->Get(value94);
        guid[3] ^= value94;
    }

    if (guid[5]) {
        uint8_t value100 = 0;
        msg->Get(value100);
        guid[5] ^= value100;
    }

    {
        float value105 = 0;
        msg->Get(value105);
        this->m_position.x = value105;
    }
    if (guid[4]) {
        uint8_t value109 = 0;
        msg->Get(value109);
        guid[4] ^= value109;
    }

    {
        float value114 = 0;
        msg->Get(value114);
        this->m_facing = value114;
    }
    if (guid[7]) {
        uint8_t value118 = 0;
        msg->Get(value118);
        guid[7] ^= value118;
    }

    {
        float value123 = 0;
        msg->Get(value123);
        this->m_position.z = value123;
    }
    if (this->m_hasVehicle) {
        uint8_t value127 = 0;
        msg->Get(value127);
        this->m_seat = value127;
    }

    if (guid[0]) {
        uint8_t value133 = 0;
        msg->Get(value133);
        guid[0] ^= value133;
    }

    if (guid[6]) {
        uint8_t value139 = 0;
        msg->Get(value139);
        guid[6] ^= value139;
    }

    {
        float value144 = 0;
        msg->Get(value144);
        this->m_position.y = value144;
    }
}

// PROVEN: 159579e8 IDENTICAL/NONE
// OFFSET: 0x006D64C0
bool ClientMoveTeleport::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D3870
bool ClientMoveTeleport::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_transportGuid);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_hasVehicle = bits.ReadBit();
    if (this->m_hasVehicle) {
        this->m_exitVoluntary = bits.ReadBit();
        this->m_awaitTransition = bits.ReadBit();
    }

    this->m_hasTransport = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_hasTransport) {
        transportGuid[1] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
    }

    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (this->m_hasTransport) {
        if (transportGuid[5]) {
            uint8_t value29 = 0;
            msg->Get(value29);
            transportGuid[5] ^= value29;
        }

        if (transportGuid[6]) {
            uint8_t value35 = 0;
            msg->Get(value35);
            transportGuid[6] ^= value35;
        }

        if (transportGuid[1]) {
            uint8_t value41 = 0;
            msg->Get(value41);
            transportGuid[1] ^= value41;
        }

        if (transportGuid[7]) {
            uint8_t value47 = 0;
            msg->Get(value47);
            transportGuid[7] ^= value47;
        }

        if (transportGuid[0]) {
            uint8_t value53 = 0;
            msg->Get(value53);
            transportGuid[0] ^= value53;
        }

        if (transportGuid[2]) {
            uint8_t value59 = 0;
            msg->Get(value59);
            transportGuid[2] ^= value59;
        }

        if (transportGuid[4]) {
            uint8_t value65 = 0;
            msg->Get(value65);
            transportGuid[4] ^= value65;
        }

        if (transportGuid[3]) {
            uint8_t value71 = 0;
            msg->Get(value71);
            transportGuid[3] ^= value71;
        }
    }

    {
        uint32_t value78 = 0;
        msg->Get(value78);
        this->m_ackIndex = value78;
    }
    if (guid[1]) {
        uint8_t value82 = 0;
        msg->Get(value82);
        guid[1] ^= value82;
    }

    if (guid[2]) {
        uint8_t value88 = 0;
        msg->Get(value88);
        guid[2] ^= value88;
    }

    if (guid[3]) {
        uint8_t value94 = 0;
        msg->Get(value94);
        guid[3] ^= value94;
    }

    if (guid[5]) {
        uint8_t value100 = 0;
        msg->Get(value100);
        guid[5] ^= value100;
    }

    {
        float value105 = 0;
        msg->Get(value105);
        this->m_position.x = value105;
    }
    if (guid[4]) {
        uint8_t value109 = 0;
        msg->Get(value109);
        guid[4] ^= value109;
    }

    {
        float value114 = 0;
        msg->Get(value114);
        this->m_facing = value114;
    }
    if (guid[7]) {
        uint8_t value118 = 0;
        msg->Get(value118);
        guid[7] ^= value118;
    }

    {
        float value123 = 0;
        msg->Get(value123);
        this->m_position.z = value123;
    }
    if (this->m_hasVehicle) {
        uint8_t value127 = 0;
        msg->Get(value127);
        this->m_seat = value127;
    }

    if (guid[0]) {
        uint8_t value133 = 0;
        msg->Get(value133);
        guid[0] ^= value133;
    }

    if (guid[6]) {
        uint8_t value139 = 0;
        msg->Get(value139);
        guid[6] ^= value139;
    }

    {
        float value144 = 0;
        msg->Get(value144);
        this->m_position.y = value144;
    }

    return msg->IsRead() && this->IsValid();
}

// OFFSET: 0x006BCC80
bool ClientMoveTeleport::IsValid() const {
    return this->m_facing >= 0.0f && this->m_facing < 6.2831864f && _finite(this->m_facing) && _finite(this->m_position.z) && _finite(this->m_position.y) && IsFinite(this->m_position.x);
}

// OFFSET: 0x006AC120/0x14035BF20
bool ClientMoveTeleport::Handle() {
    if (!ClientMoveTeleport::s_handler) {
        return false;
    }

    return ClientMoveTeleport::s_handler(this);
}

// PROVEN: e1a44e1c IDENTICAL/NONE
// OFFSET: 0x006A1580
ClientMoveUnroot::ClientMoveUnroot(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: dfd197c2 IDENTICAL/NONE
// OFFSET: 0x006C5FB0/0x14037C3D0
ClientMoveUnroot::ClientMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9a12c140 IDENTICAL/NONE
// OFFSET: 0x006A2800/0x14034FCC0
ClientMoveUnroot::~ClientMoveUnroot() {
}

// PROVEN: 698067f4 IDENTICAL/NONE
// OFFSET: 0x006BFBA0/0x140374D50
void ClientMoveUnroot::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[0]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[0] ^= value37;
    }

    if (guid[7]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[7] ^= value43;
    }

    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }
}

// PROVEN: fcee716f IDENTICAL/NONE
// OFFSET: 0x006CB510
bool ClientMoveUnroot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3d9b1b60 IDENTICAL/NONE
// OFFSET: 0x006B45F0
bool ClientMoveUnroot::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    {
        uint32_t value27 = 0;
        msg->Get(value27);
        this->m_ackIndex = value27;
    }
    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[0]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[0] ^= value37;
    }

    if (guid[7]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[7] ^= value43;
    }

    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[5]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[5] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C1540/0x140376B80
bool ClientMoveUnroot::Handle() {
    if (!ClientMoveUnroot::s_handler) {
        return false;
    }

    return ClientMoveUnroot::s_handler(this);
}

// PROVEN: 4623cb4c IDENTICAL/NONE
// OFFSET: 0x006A16A0
ClientMoveUnsetCanFly::ClientMoveUnsetCanFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 6872bcda IDENTICAL/NONE
// OFFSET: 0x006C4950/0x14037A8A0
ClientMoveUnsetCanFly::ClientMoveUnsetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5f7ce380 IDENTICAL/NONE
// OFFSET: 0x006A23D0/0x14034F8E0
ClientMoveUnsetCanFly::~ClientMoveUnsetCanFly() {
}

// PROVEN: d68112b0 IDENTICAL/NONE
// OFFSET: 0x006BEB70/0x1403739D0
void ClientMoveUnsetCanFly::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }
}

// PROVEN: abd7ce75 IDENTICAL/NONE
// OFFSET: 0x006CA840
bool ClientMoveUnsetCanFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2a8ecefa IDENTICAL/NONE
// OFFSET: 0x006C8150
bool ClientMoveUnsetCanFly::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    {
        uint32_t value21 = 0;
        msg->Get(value21);
        this->m_ackIndex = value21;
    }
    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B1A20/0x140362D50
bool ClientMoveUnsetCanFly::Handle() {
    if (!ClientMoveUnsetCanFly::s_handler) {
        return false;
    }

    return ClientMoveUnsetCanFly::s_handler(this);
}

// PROVEN: 750dd881 IDENTICAL/NONE
// OFFSET: 0x006A2BE0
ClientMoveUnsetCanTransitionBetweenSwimAndFly::ClientMoveUnsetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 23a70d57 IDENTICAL/NONE
// OFFSET: 0x006CAB80/0x140382D00
ClientMoveUnsetCanTransitionBetweenSwimAndFly::ClientMoveUnsetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 240e4cfb IDENTICAL/NONE
// OFFSET: 0x006A21B0/0x14034F6B0
ClientMoveUnsetCanTransitionBetweenSwimAndFly::~ClientMoveUnsetCanTransitionBetweenSwimAndFly() {
}

// PROVEN: dce4dd1b IDENTICAL/NONE
// OFFSET: 0x006BE4A0/0x1403731C0
void ClientMoveUnsetCanTransitionBetweenSwimAndFly::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    if (guid[6]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[6] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    if (guid[1]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[1] ^= value55;
    }
}

// PROVEN: b067469c IDENTICAL/NONE
// OFFSET: 0x006D72F0
bool ClientMoveUnsetCanTransitionBetweenSwimAndFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b6e6423d IDENTICAL/NONE
// OFFSET: 0x006C5330
bool ClientMoveUnsetCanTransitionBetweenSwimAndFly::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    {
        uint32_t value33 = 0;
        msg->Get(value33);
        this->m_ackIndex = value33;
    }
    if (guid[6]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[6] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[3]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[3] ^= value49;
    }

    if (guid[1]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[1] ^= value55;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A5C40/0x140353C00
bool ClientMoveUnsetCanTransitionBetweenSwimAndFly::Handle() {
    if (!ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler) {
        return false;
    }

    return ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler(this);
}

// PROVEN: a832d2be IDENTICAL/NONE
// OFFSET: 0x006A2A60
ClientMoveUnsetHover::ClientMoveUnsetHover(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 1733a632 IDENTICAL/NONE
// OFFSET: 0x006BCC40/0x140371220
ClientMoveUnsetHover::ClientMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 66456d9d IDENTICAL/NONE
// OFFSET: 0x006A0DD0/0x14034DE70
ClientMoveUnsetHover::~ClientMoveUnsetHover() {
}

// PROVEN: a760bdfb IDENTICAL/NONE
// OFFSET: 0x006A5690/0x1403534B0
void ClientMoveUnsetHover::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }
}

// PROVEN: 2e207e16 IDENTICAL/NONE
// OFFSET: 0x006C9A00
bool ClientMoveUnsetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4162dc96 IDENTICAL/NONE
// OFFSET: 0x006BB820
bool ClientMoveUnsetHover::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }

    {
        uint32_t value57 = 0;
        msg->Get(value57);
        this->m_ackIndex = value57;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AB850/0x14035B480
bool ClientMoveUnsetHover::Handle() {
    if (!ClientMoveUnsetHover::s_handler) {
        return false;
    }

    return ClientMoveUnsetHover::s_handler(this);
}

// PROVEN: 142eadda IDENTICAL/NONE
// OFFSET: 0x006D4540/0x140390F20
ClientMoveUpdate::ClientMoveUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: dd173323 IDENTICAL/NONE
// OFFSET: 0x006A1F30/0x14034F3C0
ClientMoveUpdate::~ClientMoveUpdate() {
}

// OFFSET: 0x006CCE80/0x1403860E0
void ClientMoveUpdate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_hasFall = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[7] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasTransport) {
        transportGuid[3] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    if (guid[5]) {
        uint8_t value45 = 0;
        msg->Get(value45);
        guid[5] ^= value45;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value52 = 0;
                msg->Get(value52);
                this->m_status.m_fall.m_direction.m_xySpeed = value52;
            }
            {
                float value55 = 0;
                msg->Get(value55);
                this->m_status.m_fall.m_direction.m_sinAngle = value55;
            }
            {
                float value58 = 0;
                msg->Get(value58);
                this->m_status.m_fall.m_direction.m_cosAngle = value58;
            }
        }

        {
            float value63 = 0;
            msg->Get(value63);
            this->m_status.m_fall.m_zSpeed = value63;
        }
        {
            uint32_t value66 = 0;
            msg->Get(value66);
            this->m_status.m_fall.m_time = value66;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value72 = 0;
        msg->Get(value72);
        this->m_status.m_splineElevation = value72;
    }

    if (guid[7]) {
        uint8_t value78 = 0;
        msg->Get(value78);
        guid[7] ^= value78;
    }

    {
        float value83 = 0;
        msg->Get(value83);
        this->m_status.m_position.y = value83;
    }
    if (guid[3]) {
        uint8_t value87 = 0;
        msg->Get(value87);
        guid[3] ^= value87;
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value94 = 0;
            msg->Get(value94);
            this->m_status.m_transport.m_vehicleId = value94;
        }

        if (transportGuid[6]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[6] ^= value100;
        }

        {
            uint8_t value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_seat = value105;
        }
        if (transportGuid[5]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[5] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.x = value114;
        }
        if (transportGuid[1]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[1] ^= value118;
        }

        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_facing = value123;
        }
        if (transportGuid[2]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[2] ^= value127;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value133 = 0;
            msg->Get(value133);
            this->m_status.m_transport.m_time2 = value133;
        }

        if (transportGuid[0]) {
            uint8_t value139 = 0;
            msg->Get(value139);
            transportGuid[0] ^= value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_position.z = value144;
        }
        if (transportGuid[7]) {
            uint8_t value148 = 0;
            msg->Get(value148);
            transportGuid[7] ^= value148;
        }

        if (transportGuid[4]) {
            uint8_t value154 = 0;
            msg->Get(value154);
            transportGuid[4] ^= value154;
        }

        if (transportGuid[3]) {
            uint8_t value160 = 0;
            msg->Get(value160);
            transportGuid[3] ^= value160;
        }

        {
            float value165 = 0;
            msg->Get(value165);
            this->m_status.m_transport.m_position.y = value165;
        }
        {
            uint32_t value168 = 0;
            msg->Get(value168);
            this->m_status.m_transport.m_time = value168;
        }
    }

    if (guid[4]) {
        uint8_t value174 = 0;
        msg->Get(value174);
        guid[4] ^= value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_status.m_position.x = value179;
    }
    if (guid[6]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[6] ^= value183;
    }

    {
        float value188 = 0;
        msg->Get(value188);
        this->m_status.m_position.z = value188;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value192 = 0;
        msg->Get(value192);
        this->m_status.m_gameTime = value192;
    }

    if (guid[2]) {
        uint8_t value198 = 0;
        msg->Get(value198);
        guid[2] ^= value198;
    }

    if (this->m_status.m_pitch) {
        float value204 = 0;
        msg->Get(value204);
        this->m_status.m_pitch = value204;
    }

    if (guid[0]) {
        uint8_t value210 = 0;
        msg->Get(value210);
        guid[0] ^= value210;
    }

    if (this->m_status.m_facing) {
        float value216 = 0;
        msg->Get(value216);
        this->m_status.m_facing = value216;
    }

    if (guid[1]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[1] ^= value222;
    }
}

// PROVEN: 89d467ad IDENTICAL/NONE
// OFFSET: 0x006CEED0
bool ClientMoveUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C6FD0
bool ClientMoveUpdate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_hasFall = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[7] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasTransport) {
        transportGuid[3] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    if (guid[5]) {
        uint8_t value45 = 0;
        msg->Get(value45);
        guid[5] ^= value45;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value52 = 0;
                msg->Get(value52);
                this->m_status.m_fall.m_direction.m_xySpeed = value52;
            }
            {
                float value55 = 0;
                msg->Get(value55);
                this->m_status.m_fall.m_direction.m_sinAngle = value55;
            }
            {
                float value58 = 0;
                msg->Get(value58);
                this->m_status.m_fall.m_direction.m_cosAngle = value58;
            }
        }

        {
            float value63 = 0;
            msg->Get(value63);
            this->m_status.m_fall.m_zSpeed = value63;
        }
        {
            uint32_t value66 = 0;
            msg->Get(value66);
            this->m_status.m_fall.m_time = value66;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value72 = 0;
        msg->Get(value72);
        this->m_status.m_splineElevation = value72;
    }

    if (guid[7]) {
        uint8_t value78 = 0;
        msg->Get(value78);
        guid[7] ^= value78;
    }

    {
        float value83 = 0;
        msg->Get(value83);
        this->m_status.m_position.y = value83;
    }
    if (guid[3]) {
        uint8_t value87 = 0;
        msg->Get(value87);
        guid[3] ^= value87;
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value94 = 0;
            msg->Get(value94);
            this->m_status.m_transport.m_vehicleId = value94;
        }

        if (transportGuid[6]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[6] ^= value100;
        }

        {
            uint8_t value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_seat = value105;
        }
        if (transportGuid[5]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[5] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.x = value114;
        }
        if (transportGuid[1]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[1] ^= value118;
        }

        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_facing = value123;
        }
        if (transportGuid[2]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[2] ^= value127;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value133 = 0;
            msg->Get(value133);
            this->m_status.m_transport.m_time2 = value133;
        }

        if (transportGuid[0]) {
            uint8_t value139 = 0;
            msg->Get(value139);
            transportGuid[0] ^= value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_position.z = value144;
        }
        if (transportGuid[7]) {
            uint8_t value148 = 0;
            msg->Get(value148);
            transportGuid[7] ^= value148;
        }

        if (transportGuid[4]) {
            uint8_t value154 = 0;
            msg->Get(value154);
            transportGuid[4] ^= value154;
        }

        if (transportGuid[3]) {
            uint8_t value160 = 0;
            msg->Get(value160);
            transportGuid[3] ^= value160;
        }

        {
            float value165 = 0;
            msg->Get(value165);
            this->m_status.m_transport.m_position.y = value165;
        }
        {
            uint32_t value168 = 0;
            msg->Get(value168);
            this->m_status.m_transport.m_time = value168;
        }
    }

    if (guid[4]) {
        uint8_t value174 = 0;
        msg->Get(value174);
        guid[4] ^= value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_status.m_position.x = value179;
    }
    if (guid[6]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[6] ^= value183;
    }

    {
        float value188 = 0;
        msg->Get(value188);
        this->m_status.m_position.z = value188;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value192 = 0;
        msg->Get(value192);
        this->m_status.m_gameTime = value192;
    }

    if (guid[2]) {
        uint8_t value198 = 0;
        msg->Get(value198);
        guid[2] ^= value198;
    }

    if (this->m_status.m_pitch) {
        float value204 = 0;
        msg->Get(value204);
        this->m_status.m_pitch = value204;
    }

    if (guid[0]) {
        uint8_t value210 = 0;
        msg->Get(value210);
        guid[0] ^= value210;
    }

    if (this->m_status.m_facing) {
        float value216 = 0;
        msg->Get(value216);
        this->m_status.m_facing = value216;
    }

    if (guid[1]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[1] ^= value222;
    }

    return msg->IsRead() && this->m_status.IsValid();
}

// OFFSET: 0x006ABA40/0x14035B710
bool ClientMoveUpdate::Handle() {
    if (!ClientMoveUpdate::s_handler) {
        return false;
    }

    return ClientMoveUpdate::s_handler(this);
}

// PROVEN: 14e401e5 IDENTICAL/NONE
// OFFSET: 0x006DA970/0x1403990B0
ClientMoveUpdateCollisionHeight::ClientMoveUpdateCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 02bb83d0 IDENTICAL/NONE
// OFFSET: 0x006A2EC0/0x1403503C0
ClientMoveUpdateCollisionHeight::~ClientMoveUpdateCollisionHeight() {
}

// OFFSET: 0x006D3D60/0x140390280
void ClientMoveUpdateCollisionHeight::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_height = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.y = value9;
    }
    JamBitReader bits(msg);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
    }

    guid[3] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[1] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (guid[3]) {
        uint8_t value57 = 0;
        msg->Get(value57);
        guid[3] ^= value57;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[7]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[7] ^= value64;
        }

        {
            uint32_t value69 = 0;
            msg->Get(value69);
            this->m_status.m_transport.m_time = value69;
        }
        if (transportGuid[4]) {
            uint8_t value73 = 0;
            msg->Get(value73);
            transportGuid[4] ^= value73;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value79 = 0;
            msg->Get(value79);
            this->m_status.m_transport.m_vehicleId = value79;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value85 = 0;
            msg->Get(value85);
            this->m_status.m_transport.m_time2 = value85;
        }

        if (transportGuid[5]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[5] ^= value91;
        }

        {
            float value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_facing = value96;
        }
        {
            float value99 = 0;
            msg->Get(value99);
            this->m_status.m_transport.m_position.x = value99;
        }
        if (transportGuid[6]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[6] ^= value103;
        }

        if (transportGuid[0]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[0] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.y = value114;
        }
        if (transportGuid[2]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[2] ^= value118;
        }

        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_position.z = value123;
        }
        if (transportGuid[3]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[3] ^= value127;
        }

        if (transportGuid[1]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[1] ^= value133;
        }

        {
            uint8_t value138 = 0;
            msg->Get(value138);
            this->m_status.m_transport.m_seat = value138;
        }
    }

    if (this->m_status.m_pitch) {
        float value144 = 0;
        msg->Get(value144);
        this->m_status.m_pitch = value144;
    }

    if (guid[6]) {
        uint8_t value150 = 0;
        msg->Get(value150);
        guid[6] ^= value150;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value157 = 0;
                msg->Get(value157);
                this->m_status.m_fall.m_direction.m_sinAngle = value157;
            }
            {
                float value160 = 0;
                msg->Get(value160);
                this->m_status.m_fall.m_direction.m_cosAngle = value160;
            }
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_xySpeed = value163;
            }
        }

        {
            float value168 = 0;
            msg->Get(value168);
            this->m_status.m_fall.m_zSpeed = value168;
        }
        {
            uint32_t value171 = 0;
            msg->Get(value171);
            this->m_status.m_fall.m_time = value171;
        }
    }

    if (guid[7]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[7] ^= value177;
    }

    if (this->m_status.m_facing) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_facing = value183;
    }

    if (guid[0]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[0] ^= value189;
    }

    if (guid[5]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[5] ^= value195;
    }

    if (guid[4]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[4] ^= value201;
    }

    if (this->m_status.m_splineElevation) {
        float value207 = 0;
        msg->Get(value207);
        this->m_status.m_splineElevation = value207;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value213 = 0;
        msg->Get(value213);
        this->m_status.m_gameTime = value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }
}

// PROVEN: 5175528c IDENTICAL/NONE
// OFFSET: 0x006CA8B0
bool ClientMoveUpdateCollisionHeight::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C35A0
bool ClientMoveUpdateCollisionHeight::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_height = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.y = value9;
    }
    JamBitReader bits(msg);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
    }

    guid[3] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[1] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (guid[3]) {
        uint8_t value57 = 0;
        msg->Get(value57);
        guid[3] ^= value57;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[7]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[7] ^= value64;
        }

        {
            uint32_t value69 = 0;
            msg->Get(value69);
            this->m_status.m_transport.m_time = value69;
        }
        if (transportGuid[4]) {
            uint8_t value73 = 0;
            msg->Get(value73);
            transportGuid[4] ^= value73;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value79 = 0;
            msg->Get(value79);
            this->m_status.m_transport.m_vehicleId = value79;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value85 = 0;
            msg->Get(value85);
            this->m_status.m_transport.m_time2 = value85;
        }

        if (transportGuid[5]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[5] ^= value91;
        }

        {
            float value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_facing = value96;
        }
        {
            float value99 = 0;
            msg->Get(value99);
            this->m_status.m_transport.m_position.x = value99;
        }
        if (transportGuid[6]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[6] ^= value103;
        }

        if (transportGuid[0]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[0] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.y = value114;
        }
        if (transportGuid[2]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[2] ^= value118;
        }

        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_position.z = value123;
        }
        if (transportGuid[3]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[3] ^= value127;
        }

        if (transportGuid[1]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[1] ^= value133;
        }

        {
            uint8_t value138 = 0;
            msg->Get(value138);
            this->m_status.m_transport.m_seat = value138;
        }
    }

    if (this->m_status.m_pitch) {
        float value144 = 0;
        msg->Get(value144);
        this->m_status.m_pitch = value144;
    }

    if (guid[6]) {
        uint8_t value150 = 0;
        msg->Get(value150);
        guid[6] ^= value150;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value157 = 0;
                msg->Get(value157);
                this->m_status.m_fall.m_direction.m_sinAngle = value157;
            }
            {
                float value160 = 0;
                msg->Get(value160);
                this->m_status.m_fall.m_direction.m_cosAngle = value160;
            }
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_xySpeed = value163;
            }
        }

        {
            float value168 = 0;
            msg->Get(value168);
            this->m_status.m_fall.m_zSpeed = value168;
        }
        {
            uint32_t value171 = 0;
            msg->Get(value171);
            this->m_status.m_fall.m_time = value171;
        }
    }

    if (guid[7]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[7] ^= value177;
    }

    if (this->m_status.m_facing) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_facing = value183;
    }

    if (guid[0]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[0] ^= value189;
    }

    if (guid[5]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[5] ^= value195;
    }

    if (guid[4]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[4] ^= value201;
    }

    if (this->m_status.m_splineElevation) {
        float value207 = 0;
        msg->Get(value207);
        this->m_status.m_splineElevation = value207;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value213 = 0;
        msg->Get(value213);
        this->m_status.m_gameTime = value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_height) && this->m_status.IsValid();
}

// OFFSET: 0x006BED60/0x140373C70
bool ClientMoveUpdateCollisionHeight::Handle() {
    if (!ClientMoveUpdateCollisionHeight::s_handler) {
        return false;
    }

    return ClientMoveUpdateCollisionHeight::s_handler(this);
}

// PROVEN: 8e8d61e2 IDENTICAL/NONE
// OFFSET: 0x006D4FD0/0x140391E10
ClientMoveUpdateFlightBackSpeed::ClientMoveUpdateFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1157518d IDENTICAL/NONE
// OFFSET: 0x006A19D0/0x14034EE00
ClientMoveUpdateFlightBackSpeed::~ClientMoveUpdateFlightBackSpeed() {
}

// OFFSET: 0x006CFCB0/0x14038A240
void ClientMoveUpdateFlightBackSpeed::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.y = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_speed = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasSpline = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[3] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[2] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
        if (this->m_status.m_hasFall) {
            if (this->m_status.m_fall.m_hasDirection) {
                {
                    float value56 = 0;
                    msg->Get(value56);
                    this->m_status.m_fall.m_direction.m_sinAngle = value56;
                }
                {
                    float value59 = 0;
                    msg->Get(value59);
                    this->m_status.m_fall.m_direction.m_xySpeed = value59;
                }
                {
                    float value62 = 0;
                    msg->Get(value62);
                    this->m_status.m_fall.m_direction.m_cosAngle = value62;
                }
            }

            {
                uint32_t value67 = 0;
                msg->Get(value67);
                this->m_status.m_fall.m_time = value67;
            }
            {
                float value70 = 0;
                msg->Get(value70);
                this->m_status.m_fall.m_zSpeed = value70;
            }
        }
    }

    if (this->m_status.m_hasTransport) {
        {
            float value78 = 0;
            msg->Get(value78);
            this->m_status.m_transport.m_position.x = value78;
        }
        {
            uint32_t value81 = 0;
            msg->Get(value81);
            this->m_status.m_transport.m_time = value81;
        }
        {
            float value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_facing = value84;
        }
        if (transportGuid[5]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[5] ^= value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value97 = 0;
            msg->Get(value97);
            this->m_status.m_transport.m_vehicleId = value97;
        }

        if (transportGuid[7]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[7] ^= value103;
        }

        if (transportGuid[1]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[1] ^= value109;
        }

        if (transportGuid[3]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[3] ^= value115;
        }

        if (transportGuid[6]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[6] ^= value121;
        }

        if (transportGuid[0]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[0] ^= value127;
        }

        if (transportGuid[2]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[2] ^= value133;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value139 = 0;
            msg->Get(value139);
            this->m_status.m_transport.m_time2 = value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_position.y = value144;
        }
        {
            uint8_t value147 = 0;
            msg->Get(value147);
            this->m_status.m_transport.m_seat = value147;
        }
        if (transportGuid[4]) {
            uint8_t value151 = 0;
            msg->Get(value151);
            transportGuid[4] ^= value151;
        }
    }

    if (guid[1]) {
        uint8_t value159 = 0;
        msg->Get(value159);
        guid[1] ^= value159;
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (this->m_status.m_facing) {
        float value171 = 0;
        msg->Get(value171);
        this->m_status.m_facing = value171;
    }

    if (guid[5]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[5] ^= value177;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value183 = 0;
        msg->Get(value183);
        this->m_status.m_gameTime = value183;
    }

    if (this->m_status.m_splineElevation) {
        float value189 = 0;
        msg->Get(value189);
        this->m_status.m_splineElevation = value189;
    }

    if (guid[6]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[6] ^= value195;
    }

    if (this->m_status.m_pitch) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_pitch = value201;
    }

    if (guid[3]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[3] ^= value207;
    }

    if (guid[2]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[2] ^= value213;
    }

    if (guid[0]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[0] ^= value219;
    }

    if (guid[7]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[7] ^= value225;
    }
}

// PROVEN: acdf1f57 IDENTICAL/NONE
// OFFSET: 0x006DAEF0
bool ClientMoveUpdateFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5010
bool ClientMoveUpdateFlightBackSpeed::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.y = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_speed = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasSpline = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[3] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[2] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
        if (this->m_status.m_hasFall) {
            if (this->m_status.m_fall.m_hasDirection) {
                {
                    float value56 = 0;
                    msg->Get(value56);
                    this->m_status.m_fall.m_direction.m_sinAngle = value56;
                }
                {
                    float value59 = 0;
                    msg->Get(value59);
                    this->m_status.m_fall.m_direction.m_xySpeed = value59;
                }
                {
                    float value62 = 0;
                    msg->Get(value62);
                    this->m_status.m_fall.m_direction.m_cosAngle = value62;
                }
            }

            {
                uint32_t value67 = 0;
                msg->Get(value67);
                this->m_status.m_fall.m_time = value67;
            }
            {
                float value70 = 0;
                msg->Get(value70);
                this->m_status.m_fall.m_zSpeed = value70;
            }
        }
    }

    if (this->m_status.m_hasTransport) {
        {
            float value78 = 0;
            msg->Get(value78);
            this->m_status.m_transport.m_position.x = value78;
        }
        {
            uint32_t value81 = 0;
            msg->Get(value81);
            this->m_status.m_transport.m_time = value81;
        }
        {
            float value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_facing = value84;
        }
        if (transportGuid[5]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[5] ^= value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value97 = 0;
            msg->Get(value97);
            this->m_status.m_transport.m_vehicleId = value97;
        }

        if (transportGuid[7]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[7] ^= value103;
        }

        if (transportGuid[1]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[1] ^= value109;
        }

        if (transportGuid[3]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[3] ^= value115;
        }

        if (transportGuid[6]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[6] ^= value121;
        }

        if (transportGuid[0]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[0] ^= value127;
        }

        if (transportGuid[2]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[2] ^= value133;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value139 = 0;
            msg->Get(value139);
            this->m_status.m_transport.m_time2 = value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_position.y = value144;
        }
        {
            uint8_t value147 = 0;
            msg->Get(value147);
            this->m_status.m_transport.m_seat = value147;
        }
        if (transportGuid[4]) {
            uint8_t value151 = 0;
            msg->Get(value151);
            transportGuid[4] ^= value151;
        }
    }

    if (guid[1]) {
        uint8_t value159 = 0;
        msg->Get(value159);
        guid[1] ^= value159;
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (this->m_status.m_facing) {
        float value171 = 0;
        msg->Get(value171);
        this->m_status.m_facing = value171;
    }

    if (guid[5]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[5] ^= value177;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value183 = 0;
        msg->Get(value183);
        this->m_status.m_gameTime = value183;
    }

    if (this->m_status.m_splineElevation) {
        float value189 = 0;
        msg->Get(value189);
        this->m_status.m_splineElevation = value189;
    }

    if (guid[6]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[6] ^= value195;
    }

    if (this->m_status.m_pitch) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_pitch = value201;
    }

    if (guid[3]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[3] ^= value207;
    }

    if (guid[2]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[2] ^= value213;
    }

    if (guid[0]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[0] ^= value219;
    }

    if (guid[7]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[7] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006A76E0/0x140356200
bool ClientMoveUpdateFlightBackSpeed::Handle() {
    if (!ClientMoveUpdateFlightBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateFlightBackSpeed::s_handler(this);
}

// PROVEN: e725f957 IDENTICAL/NONE
// OFFSET: 0x006D8670/0x140396770
ClientMoveUpdateFlightSpeed::ClientMoveUpdateFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1ebbbcdf IDENTICAL/NONE
// OFFSET: 0x006A2520/0x14034FA40
ClientMoveUpdateFlightSpeed::~ClientMoveUpdateFlightSpeed() {
}

// OFFSET: 0x006C8840/0x14037F910
void ClientMoveUpdateFlightSpeed::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.y = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_speed = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
    }

    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[0] = bits.ReadBit();
    if (this->m_status.m_splineElevation) {
        float value57 = 0;
        msg->Get(value57);
        this->m_status.m_splineElevation = value57;
    }

    if (this->m_status.m_pitch) {
        float value63 = 0;
        msg->Get(value63);
        this->m_status.m_pitch = value63;
    }

    if (guid[0]) {
        uint8_t value69 = 0;
        msg->Get(value69);
        guid[0] ^= value69;
    }

    if (guid[5]) {
        uint8_t value75 = 0;
        msg->Get(value75);
        guid[5] ^= value75;
    }

    if (this->m_status.m_hasTransport) {
        {
            float value81 = 0;
            msg->Get(value81);
            this->m_status.m_transport.m_facing = value81;
        }
        {
            uint32_t value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_time = value84;
        }
        if (transportGuid[5]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[5] ^= value88;
        }

        if (transportGuid[4]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[4] ^= value94;
        }

        if (transportGuid[6]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[6] ^= value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        if (transportGuid[7]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[7] ^= value109;
        }

        if (transportGuid[0]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[0] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        if (transportGuid[3]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[3] ^= value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value136 = 0;
            msg->Get(value136);
            this->m_status.m_transport.m_vehicleId = value136;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value142 = 0;
            msg->Get(value142);
            this->m_status.m_transport.m_time2 = value142;
        }

        if (transportGuid[1]) {
            uint8_t value148 = 0;
            msg->Get(value148);
            transportGuid[1] ^= value148;
        }

        {
            uint8_t value153 = 0;
            msg->Get(value153);
            this->m_status.m_transport.m_seat = value153;
        }
        {
            float value156 = 0;
            msg->Get(value156);
            this->m_status.m_transport.m_position.x = value156;
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_sinAngle = value163;
            }
            {
                float value166 = 0;
                msg->Get(value166);
                this->m_status.m_fall.m_direction.m_xySpeed = value166;
            }
            {
                float value169 = 0;
                msg->Get(value169);
                this->m_status.m_fall.m_direction.m_cosAngle = value169;
            }
        }

        {
            float value174 = 0;
            msg->Get(value174);
            this->m_status.m_fall.m_zSpeed = value174;
        }
        {
            uint32_t value177 = 0;
            msg->Get(value177);
            this->m_status.m_fall.m_time = value177;
        }
    }

    if (guid[1]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[1] ^= value183;
    }

    if (guid[6]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[6] ^= value189;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value195 = 0;
        msg->Get(value195);
        this->m_status.m_gameTime = value195;
    }

    if (guid[7]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[7] ^= value201;
    }

    if (guid[2]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[2] ^= value207;
    }

    if (guid[4]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[4] ^= value213;
    }

    if (this->m_status.m_facing) {
        float value219 = 0;
        msg->Get(value219);
        this->m_status.m_facing = value219;
    }

    if (guid[3]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[3] ^= value225;
    }
}

// PROVEN: 92efb285 IDENTICAL/NONE
// OFFSET: 0x006CA8A0
bool ClientMoveUpdateFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C9B40
bool ClientMoveUpdateFlightSpeed::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.y = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_speed = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
    }

    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[0] = bits.ReadBit();
    if (this->m_status.m_splineElevation) {
        float value57 = 0;
        msg->Get(value57);
        this->m_status.m_splineElevation = value57;
    }

    if (this->m_status.m_pitch) {
        float value63 = 0;
        msg->Get(value63);
        this->m_status.m_pitch = value63;
    }

    if (guid[0]) {
        uint8_t value69 = 0;
        msg->Get(value69);
        guid[0] ^= value69;
    }

    if (guid[5]) {
        uint8_t value75 = 0;
        msg->Get(value75);
        guid[5] ^= value75;
    }

    if (this->m_status.m_hasTransport) {
        {
            float value81 = 0;
            msg->Get(value81);
            this->m_status.m_transport.m_facing = value81;
        }
        {
            uint32_t value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_time = value84;
        }
        if (transportGuid[5]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[5] ^= value88;
        }

        if (transportGuid[4]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[4] ^= value94;
        }

        if (transportGuid[6]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[6] ^= value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        if (transportGuid[7]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[7] ^= value109;
        }

        if (transportGuid[0]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[0] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        if (transportGuid[3]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[3] ^= value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value136 = 0;
            msg->Get(value136);
            this->m_status.m_transport.m_vehicleId = value136;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value142 = 0;
            msg->Get(value142);
            this->m_status.m_transport.m_time2 = value142;
        }

        if (transportGuid[1]) {
            uint8_t value148 = 0;
            msg->Get(value148);
            transportGuid[1] ^= value148;
        }

        {
            uint8_t value153 = 0;
            msg->Get(value153);
            this->m_status.m_transport.m_seat = value153;
        }
        {
            float value156 = 0;
            msg->Get(value156);
            this->m_status.m_transport.m_position.x = value156;
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_sinAngle = value163;
            }
            {
                float value166 = 0;
                msg->Get(value166);
                this->m_status.m_fall.m_direction.m_xySpeed = value166;
            }
            {
                float value169 = 0;
                msg->Get(value169);
                this->m_status.m_fall.m_direction.m_cosAngle = value169;
            }
        }

        {
            float value174 = 0;
            msg->Get(value174);
            this->m_status.m_fall.m_zSpeed = value174;
        }
        {
            uint32_t value177 = 0;
            msg->Get(value177);
            this->m_status.m_fall.m_time = value177;
        }
    }

    if (guid[1]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[1] ^= value183;
    }

    if (guid[6]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[6] ^= value189;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value195 = 0;
        msg->Get(value195);
        this->m_status.m_gameTime = value195;
    }

    if (guid[7]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[7] ^= value201;
    }

    if (guid[2]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[2] ^= value207;
    }

    if (guid[4]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[4] ^= value213;
    }

    if (this->m_status.m_facing) {
        float value219 = 0;
        msg->Get(value219);
        this->m_status.m_facing = value219;
    }

    if (guid[3]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[3] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006B8950/0x14036BC10
bool ClientMoveUpdateFlightSpeed::Handle() {
    if (!ClientMoveUpdateFlightSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateFlightSpeed::s_handler(this);
}

// PROVEN: d9eb91d8 IDENTICAL/NONE
// OFFSET: 0x006D9280/0x140397690
ClientMoveUpdateKnockBack::ClientMoveUpdateKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 49046eeb IDENTICAL/NONE
// OFFSET: 0x006A2DE0/0x140350340
ClientMoveUpdateKnockBack::~ClientMoveUpdateKnockBack() {
}

// OFFSET: 0x006CAD60/0x140382F50
void ClientMoveUpdateKnockBack::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[7] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_facing) {
        float value45 = 0;
        msg->Get(value45);
        this->m_status.m_facing = value45;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value52 = 0;
                msg->Get(value52);
                this->m_status.m_fall.m_direction.m_sinAngle = value52;
            }
            {
                float value55 = 0;
                msg->Get(value55);
                this->m_status.m_fall.m_direction.m_xySpeed = value55;
            }
            {
                float value58 = 0;
                msg->Get(value58);
                this->m_status.m_fall.m_direction.m_cosAngle = value58;
            }
        }

        {
            uint32_t value63 = 0;
            msg->Get(value63);
            this->m_status.m_fall.m_time = value63;
        }
        {
            float value66 = 0;
            msg->Get(value66);
            this->m_status.m_fall.m_zSpeed = value66;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value72 = 0;
        msg->Get(value72);
        this->m_status.m_splineElevation = value72;
    }

    if (guid[3]) {
        uint8_t value78 = 0;
        msg->Get(value78);
        guid[3] ^= value78;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[5] ^= value85;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value91 = 0;
            msg->Get(value91);
            this->m_status.m_transport.m_vehicleId = value91;
        }

        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        {
            uint8_t value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_seat = value102;
        }
        if (transportGuid[3]) {
            uint8_t value106 = 0;
            msg->Get(value106);
            transportGuid[3] ^= value106;
        }

        if (transportGuid[6]) {
            uint8_t value112 = 0;
            msg->Get(value112);
            transportGuid[6] ^= value112;
        }

        {
            float value117 = 0;
            msg->Get(value117);
            this->m_status.m_transport.m_position.z = value117;
        }
        if (transportGuid[1]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[1] ^= value121;
        }

        {
            float value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_position.y = value126;
        }
        {
            float value129 = 0;
            msg->Get(value129);
            this->m_status.m_transport.m_position.x = value129;
        }
        if (transportGuid[2]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[2] ^= value133;
        }

        if (transportGuid[0]) {
            uint8_t value139 = 0;
            msg->Get(value139);
            transportGuid[0] ^= value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_facing = value144;
        }
        {
            uint32_t value147 = 0;
            msg->Get(value147);
            this->m_status.m_transport.m_time = value147;
        }
        if (transportGuid[4]) {
            uint8_t value151 = 0;
            msg->Get(value151);
            transportGuid[4] ^= value151;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value157 = 0;
            msg->Get(value157);
            this->m_status.m_transport.m_time2 = value157;
        }
    }

    if (this->m_status.m_pitch) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_pitch = value165;
    }

    {
        float value170 = 0;
        msg->Get(value170);
        this->m_status.m_position.z = value170;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value174 = 0;
        msg->Get(value174);
        this->m_status.m_gameTime = value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_status.m_position.x = value179;
    }
    if (guid[4]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[4] ^= value183;
    }

    if (guid[6]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[6] ^= value189;
    }

    if (guid[7]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[7] ^= value195;
    }

    if (guid[2]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[2] ^= value201;
    }

    if (guid[1]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[1] ^= value207;
    }

    {
        float value212 = 0;
        msg->Get(value212);
        this->m_status.m_position.y = value212;
    }
    if (guid[0]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[0] ^= value216;
    }

    if (guid[5]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[5] ^= value222;
    }
}

// PROVEN: df179b92 IDENTICAL/NONE
// OFFSET: 0x006D1FF0
bool ClientMoveUpdateKnockBack::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CD700
bool ClientMoveUpdateKnockBack::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[7] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
    }

    guid[5] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_facing) {
        float value45 = 0;
        msg->Get(value45);
        this->m_status.m_facing = value45;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value52 = 0;
                msg->Get(value52);
                this->m_status.m_fall.m_direction.m_sinAngle = value52;
            }
            {
                float value55 = 0;
                msg->Get(value55);
                this->m_status.m_fall.m_direction.m_xySpeed = value55;
            }
            {
                float value58 = 0;
                msg->Get(value58);
                this->m_status.m_fall.m_direction.m_cosAngle = value58;
            }
        }

        {
            uint32_t value63 = 0;
            msg->Get(value63);
            this->m_status.m_fall.m_time = value63;
        }
        {
            float value66 = 0;
            msg->Get(value66);
            this->m_status.m_fall.m_zSpeed = value66;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value72 = 0;
        msg->Get(value72);
        this->m_status.m_splineElevation = value72;
    }

    if (guid[3]) {
        uint8_t value78 = 0;
        msg->Get(value78);
        guid[3] ^= value78;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[5] ^= value85;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value91 = 0;
            msg->Get(value91);
            this->m_status.m_transport.m_vehicleId = value91;
        }

        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        {
            uint8_t value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_seat = value102;
        }
        if (transportGuid[3]) {
            uint8_t value106 = 0;
            msg->Get(value106);
            transportGuid[3] ^= value106;
        }

        if (transportGuid[6]) {
            uint8_t value112 = 0;
            msg->Get(value112);
            transportGuid[6] ^= value112;
        }

        {
            float value117 = 0;
            msg->Get(value117);
            this->m_status.m_transport.m_position.z = value117;
        }
        if (transportGuid[1]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[1] ^= value121;
        }

        {
            float value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_position.y = value126;
        }
        {
            float value129 = 0;
            msg->Get(value129);
            this->m_status.m_transport.m_position.x = value129;
        }
        if (transportGuid[2]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[2] ^= value133;
        }

        if (transportGuid[0]) {
            uint8_t value139 = 0;
            msg->Get(value139);
            transportGuid[0] ^= value139;
        }

        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_transport.m_facing = value144;
        }
        {
            uint32_t value147 = 0;
            msg->Get(value147);
            this->m_status.m_transport.m_time = value147;
        }
        if (transportGuid[4]) {
            uint8_t value151 = 0;
            msg->Get(value151);
            transportGuid[4] ^= value151;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value157 = 0;
            msg->Get(value157);
            this->m_status.m_transport.m_time2 = value157;
        }
    }

    if (this->m_status.m_pitch) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_pitch = value165;
    }

    {
        float value170 = 0;
        msg->Get(value170);
        this->m_status.m_position.z = value170;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value174 = 0;
        msg->Get(value174);
        this->m_status.m_gameTime = value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_status.m_position.x = value179;
    }
    if (guid[4]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[4] ^= value183;
    }

    if (guid[6]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[6] ^= value189;
    }

    if (guid[7]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[7] ^= value195;
    }

    if (guid[2]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[2] ^= value201;
    }

    if (guid[1]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[1] ^= value207;
    }

    {
        float value212 = 0;
        msg->Get(value212);
        this->m_status.m_position.y = value212;
    }
    if (guid[0]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[0] ^= value216;
    }

    if (guid[5]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[5] ^= value222;
    }

    return msg->IsRead() && this->m_status.m_hasFall && this->m_status.m_fall.m_hasDirection && this->m_status.IsValid();
}

// OFFSET: 0x006BECD0/0x140373BA0
bool ClientMoveUpdateKnockBack::Handle() {
    if (!ClientMoveUpdateKnockBack::s_handler) {
        return false;
    }

    return ClientMoveUpdateKnockBack::s_handler(this);
}

// PROVEN: 8313458e IDENTICAL/NONE
// OFFSET: 0x006CB470/0x140383A60
ClientMoveUpdatePitchRate::ClientMoveUpdatePitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a1f2bed7 IDENTICAL/NONE
// OFFSET: 0x006A30C0/0x1403507E0
ClientMoveUpdatePitchRate::~ClientMoveUpdatePitchRate() {
}

// OFFSET: 0x006C5500/0x14037B330
void ClientMoveUpdatePitchRate::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_rate = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.x = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.y = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
    }

    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_gameTime = !bits.ReadBit();
    if (guid[0]) {
        uint8_t value57 = 0;
        msg->Get(value57);
        guid[0] ^= value57;
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value64 = 0;
            msg->Get(value64);
            this->m_status.m_transport.m_vehicleId = value64;
        }

        if (transportGuid[7]) {
            uint8_t value70 = 0;
            msg->Get(value70);
            transportGuid[7] ^= value70;
        }

        {
            float value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_position.z = value75;
        }
        {
            float value78 = 0;
            msg->Get(value78);
            this->m_status.m_transport.m_position.y = value78;
        }
        if (transportGuid[3]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[3] ^= value82;
        }

        if (transportGuid[4]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[4] ^= value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.x = value93;
        }
        if (transportGuid[6]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[6] ^= value97;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value103 = 0;
            msg->Get(value103);
            this->m_status.m_transport.m_time2 = value103;
        }

        {
            uint32_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_time = value108;
        }
        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_facing = value111;
        }
        if (transportGuid[2]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[2] ^= value115;
        }

        if (transportGuid[5]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[5] ^= value121;
        }

        if (transportGuid[1]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[1] ^= value127;
        }

        if (transportGuid[0]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[0] ^= value133;
        }

        {
            uint8_t value138 = 0;
            msg->Get(value138);
            this->m_status.m_transport.m_seat = value138;
        }
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value144 = 0;
            msg->Get(value144);
            this->m_status.m_fall.m_time = value144;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_cosAngle = value148;
            }
            {
                float value151 = 0;
                msg->Get(value151);
                this->m_status.m_fall.m_direction.m_sinAngle = value151;
            }
            {
                float value154 = 0;
                msg->Get(value154);
                this->m_status.m_fall.m_direction.m_xySpeed = value154;
            }
        }

        {
            float value159 = 0;
            msg->Get(value159);
            this->m_status.m_fall.m_zSpeed = value159;
        }
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (guid[5]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[5] ^= value171;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value177 = 0;
        msg->Get(value177);
        this->m_status.m_gameTime = value177;
    }

    if (this->m_status.m_splineElevation) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_splineElevation = value183;
    }

    if (this->m_status.m_pitch) {
        float value189 = 0;
        msg->Get(value189);
        this->m_status.m_pitch = value189;
    }

    if (guid[7]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[7] ^= value195;
    }

    if (guid[6]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[6] ^= value201;
    }

    if (guid[3]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[3] ^= value207;
    }

    if (this->m_status.m_facing) {
        float value213 = 0;
        msg->Get(value213);
        this->m_status.m_facing = value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }
}

// PROVEN: 2121f2b1 IDENTICAL/NONE
// OFFSET: 0x006D4FC0
bool ClientMoveUpdatePitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CC150
bool ClientMoveUpdatePitchRate::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_rate = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.x = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.y = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
    }

    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_gameTime = !bits.ReadBit();
    if (guid[0]) {
        uint8_t value57 = 0;
        msg->Get(value57);
        guid[0] ^= value57;
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value64 = 0;
            msg->Get(value64);
            this->m_status.m_transport.m_vehicleId = value64;
        }

        if (transportGuid[7]) {
            uint8_t value70 = 0;
            msg->Get(value70);
            transportGuid[7] ^= value70;
        }

        {
            float value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_position.z = value75;
        }
        {
            float value78 = 0;
            msg->Get(value78);
            this->m_status.m_transport.m_position.y = value78;
        }
        if (transportGuid[3]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[3] ^= value82;
        }

        if (transportGuid[4]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[4] ^= value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.x = value93;
        }
        if (transportGuid[6]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[6] ^= value97;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value103 = 0;
            msg->Get(value103);
            this->m_status.m_transport.m_time2 = value103;
        }

        {
            uint32_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_time = value108;
        }
        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_facing = value111;
        }
        if (transportGuid[2]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[2] ^= value115;
        }

        if (transportGuid[5]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[5] ^= value121;
        }

        if (transportGuid[1]) {
            uint8_t value127 = 0;
            msg->Get(value127);
            transportGuid[1] ^= value127;
        }

        if (transportGuid[0]) {
            uint8_t value133 = 0;
            msg->Get(value133);
            transportGuid[0] ^= value133;
        }

        {
            uint8_t value138 = 0;
            msg->Get(value138);
            this->m_status.m_transport.m_seat = value138;
        }
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value144 = 0;
            msg->Get(value144);
            this->m_status.m_fall.m_time = value144;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_cosAngle = value148;
            }
            {
                float value151 = 0;
                msg->Get(value151);
                this->m_status.m_fall.m_direction.m_sinAngle = value151;
            }
            {
                float value154 = 0;
                msg->Get(value154);
                this->m_status.m_fall.m_direction.m_xySpeed = value154;
            }
        }

        {
            float value159 = 0;
            msg->Get(value159);
            this->m_status.m_fall.m_zSpeed = value159;
        }
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (guid[5]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[5] ^= value171;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value177 = 0;
        msg->Get(value177);
        this->m_status.m_gameTime = value177;
    }

    if (this->m_status.m_splineElevation) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_splineElevation = value183;
    }

    if (this->m_status.m_pitch) {
        float value189 = 0;
        msg->Get(value189);
        this->m_status.m_pitch = value189;
    }

    if (guid[7]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[7] ^= value195;
    }

    if (guid[6]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[6] ^= value201;
    }

    if (guid[3]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[3] ^= value207;
    }

    if (this->m_status.m_facing) {
        float value213 = 0;
        msg->Get(value213);
        this->m_status.m_facing = value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_rate) && this->m_status.IsValid();
}

// OFFSET: 0x006B6410/0x140368BA0
bool ClientMoveUpdatePitchRate::Handle() {
    if (!ClientMoveUpdatePitchRate::s_handler) {
        return false;
    }

    return ClientMoveUpdatePitchRate::s_handler(this);
}

// PROVEN: 31a9754c IDENTICAL/NONE
// OFFSET: 0x006D9700/0x140397B80
ClientMoveUpdateRunBackSpeed::ClientMoveUpdateRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3c9118d8 IDENTICAL/NONE
// OFFSET: 0x006A15B0/0x14034E8A0
ClientMoveUpdateRunBackSpeed::~ClientMoveUpdateRunBackSpeed() {
}

// OFFSET: 0x006D2870/0x14038E360
void ClientMoveUpdateRunBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        {
            float value45 = 0;
            msg->Get(value45);
            this->m_status.m_transport.m_position.x = value45;
        }
        if (transportGuid[2]) {
            uint8_t value49 = 0;
            msg->Get(value49);
            transportGuid[2] ^= value49;
        }

        if (transportGuid[5]) {
            uint8_t value55 = 0;
            msg->Get(value55);
            transportGuid[5] ^= value55;
        }

        if (transportGuid[4]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[4] ^= value61;
        }

        if (transportGuid[6]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[6] ^= value67;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value73 = 0;
            msg->Get(value73);
            this->m_status.m_transport.m_time2 = value73;
        }

        if (transportGuid[0]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[0] ^= value79;
        }

        if (transportGuid[3]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[3] ^= value85;
        }

        {
            float value90 = 0;
            msg->Get(value90);
            this->m_status.m_transport.m_position.y = value90;
        }
        if (transportGuid[7]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[7] ^= value94;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value100 = 0;
            msg->Get(value100);
            this->m_status.m_transport.m_vehicleId = value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        {
            uint32_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_time = value108;
        }
        {
            uint8_t value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_seat = value111;
        }
        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_facing = value120;
        }
    }

    if (guid[4]) {
        uint8_t value126 = 0;
        msg->Get(value126);
        guid[4] ^= value126;
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value132 = 0;
            msg->Get(value132);
            this->m_status.m_fall.m_time = value132;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value136 = 0;
                msg->Get(value136);
                this->m_status.m_fall.m_direction.m_xySpeed = value136;
            }
            {
                float value139 = 0;
                msg->Get(value139);
                this->m_status.m_fall.m_direction.m_sinAngle = value139;
            }
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_cosAngle = value142;
            }
        }

        {
            float value147 = 0;
            msg->Get(value147);
            this->m_status.m_fall.m_zSpeed = value147;
        }
    }

    if (this->m_status.m_gameTime) {
        uint32_t value153 = 0;
        msg->Get(value153);
        this->m_status.m_gameTime = value153;
    }

    if (this->m_status.m_splineElevation) {
        float value159 = 0;
        msg->Get(value159);
        this->m_status.m_splineElevation = value159;
    }

    if (guid[1]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[1] ^= value165;
    }

    if (this->m_status.m_facing) {
        float value171 = 0;
        msg->Get(value171);
        this->m_status.m_facing = value171;
    }

    if (guid[0]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[0] ^= value177;
    }

    if (guid[5]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[5] ^= value183;
    }

    if (guid[3]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[3] ^= value189;
    }

    {
        float value194 = 0;
        msg->Get(value194);
        this->m_status.m_position.x = value194;
    }
    {
        float value197 = 0;
        msg->Get(value197);
        this->m_status.m_position.y = value197;
    }
    if (this->m_status.m_pitch) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_pitch = value201;
    }

    if (guid[7]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[7] ^= value207;
    }

    {
        float value212 = 0;
        msg->Get(value212);
        this->m_speed = value212;
    }
    if (guid[2]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[2] ^= value216;
    }

    if (guid[6]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[6] ^= value222;
    }

    {
        float value227 = 0;
        msg->Get(value227);
        this->m_status.m_position.z = value227;
    }
}

// PROVEN: 56b9abd6 IDENTICAL/NONE
// OFFSET: 0x006D92F0
bool ClientMoveUpdateRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D46D0
bool ClientMoveUpdateRunBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[4] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        {
            float value45 = 0;
            msg->Get(value45);
            this->m_status.m_transport.m_position.x = value45;
        }
        if (transportGuid[2]) {
            uint8_t value49 = 0;
            msg->Get(value49);
            transportGuid[2] ^= value49;
        }

        if (transportGuid[5]) {
            uint8_t value55 = 0;
            msg->Get(value55);
            transportGuid[5] ^= value55;
        }

        if (transportGuid[4]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[4] ^= value61;
        }

        if (transportGuid[6]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[6] ^= value67;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value73 = 0;
            msg->Get(value73);
            this->m_status.m_transport.m_time2 = value73;
        }

        if (transportGuid[0]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[0] ^= value79;
        }

        if (transportGuid[3]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[3] ^= value85;
        }

        {
            float value90 = 0;
            msg->Get(value90);
            this->m_status.m_transport.m_position.y = value90;
        }
        if (transportGuid[7]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[7] ^= value94;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value100 = 0;
            msg->Get(value100);
            this->m_status.m_transport.m_vehicleId = value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        {
            uint32_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_time = value108;
        }
        {
            uint8_t value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_seat = value111;
        }
        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_facing = value120;
        }
    }

    if (guid[4]) {
        uint8_t value126 = 0;
        msg->Get(value126);
        guid[4] ^= value126;
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value132 = 0;
            msg->Get(value132);
            this->m_status.m_fall.m_time = value132;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value136 = 0;
                msg->Get(value136);
                this->m_status.m_fall.m_direction.m_xySpeed = value136;
            }
            {
                float value139 = 0;
                msg->Get(value139);
                this->m_status.m_fall.m_direction.m_sinAngle = value139;
            }
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_cosAngle = value142;
            }
        }

        {
            float value147 = 0;
            msg->Get(value147);
            this->m_status.m_fall.m_zSpeed = value147;
        }
    }

    if (this->m_status.m_gameTime) {
        uint32_t value153 = 0;
        msg->Get(value153);
        this->m_status.m_gameTime = value153;
    }

    if (this->m_status.m_splineElevation) {
        float value159 = 0;
        msg->Get(value159);
        this->m_status.m_splineElevation = value159;
    }

    if (guid[1]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[1] ^= value165;
    }

    if (this->m_status.m_facing) {
        float value171 = 0;
        msg->Get(value171);
        this->m_status.m_facing = value171;
    }

    if (guid[0]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[0] ^= value177;
    }

    if (guid[5]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[5] ^= value183;
    }

    if (guid[3]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[3] ^= value189;
    }

    {
        float value194 = 0;
        msg->Get(value194);
        this->m_status.m_position.x = value194;
    }
    {
        float value197 = 0;
        msg->Get(value197);
        this->m_status.m_position.y = value197;
    }
    if (this->m_status.m_pitch) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_pitch = value201;
    }

    if (guid[7]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[7] ^= value207;
    }

    {
        float value212 = 0;
        msg->Get(value212);
        this->m_speed = value212;
    }
    if (guid[2]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[2] ^= value216;
    }

    if (guid[6]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[6] ^= value222;
    }

    {
        float value227 = 0;
        msg->Get(value227);
        this->m_status.m_position.z = value227;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006AA000/0x1403595A0
bool ClientMoveUpdateRunBackSpeed::Handle() {
    if (!ClientMoveUpdateRunBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateRunBackSpeed::s_handler(this);
}

// PROVEN: 1823e0e4 IDENTICAL/NONE
// OFFSET: 0x006D72B0/0x140394F20
ClientMoveUpdateRunSpeed::ClientMoveUpdateRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 90cc337b IDENTICAL/NONE
// OFFSET: 0x006A22D0/0x14034F7A0
ClientMoveUpdateRunSpeed::~ClientMoveUpdateRunSpeed() {
}

// OFFSET: 0x006D0480/0x14038AE90
void ClientMoveUpdateRunSpeed::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.x = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.y = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_speed = value9;
    }
    JamBitReader bits(msg);
    guid[6] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[3] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[4] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        if (transportGuid[4]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[4] ^= value58;
        }

        if (transportGuid[5]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[5] ^= value64;
        }

        {
            float value69 = 0;
            msg->Get(value69);
            this->m_status.m_transport.m_position.x = value69;
        }
        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (transportGuid[1]) {
            uint8_t value76 = 0;
            msg->Get(value76);
            transportGuid[1] ^= value76;
        }

        if (transportGuid[0]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[0] ^= value82;
        }

        if (transportGuid[6]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[6] ^= value88;
        }

        {
            uint32_t value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_time = value93;
        }
        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        {
            uint8_t value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_seat = value102;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value106 = 0;
            msg->Get(value106);
            this->m_status.m_transport.m_time2 = value106;
        }

        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_position.y = value111;
        }
        if (transportGuid[3]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[3] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value127 = 0;
            msg->Get(value127);
            this->m_status.m_transport.m_vehicleId = value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.z = value132;
        }
    }

    if (this->m_status.m_gameTime) {
        uint32_t value138 = 0;
        msg->Get(value138);
        this->m_status.m_gameTime = value138;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_cosAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_xySpeed = value148;
            }
            {
                float value151 = 0;
                msg->Get(value151);
                this->m_status.m_fall.m_direction.m_sinAngle = value151;
            }
        }

        {
            float value156 = 0;
            msg->Get(value156);
            this->m_status.m_fall.m_zSpeed = value156;
        }
        {
            uint32_t value159 = 0;
            msg->Get(value159);
            this->m_status.m_fall.m_time = value159;
        }
    }

    if (this->m_status.m_pitch) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_pitch = value165;
    }

    if (guid[6]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[6] ^= value171;
    }

    if (this->m_status.m_splineElevation) {
        float value177 = 0;
        msg->Get(value177);
        this->m_status.m_splineElevation = value177;
    }

    if (guid[5]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[5] ^= value183;
    }

    if (guid[7]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[7] ^= value189;
    }

    if (guid[4]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[4] ^= value195;
    }

    if (this->m_status.m_facing) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_facing = value201;
    }

    if (guid[0]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[0] ^= value207;
    }

    if (guid[3]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[3] ^= value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }
}

// PROVEN: f605d254 IDENTICAL/NONE
// OFFSET: 0x006D8D50
bool ClientMoveUpdateRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5C20
bool ClientMoveUpdateRunSpeed::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.x = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.y = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_speed = value9;
    }
    JamBitReader bits(msg);
    guid[6] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[3] = bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    guid[4] = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        if (transportGuid[4]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[4] ^= value58;
        }

        if (transportGuid[5]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[5] ^= value64;
        }

        {
            float value69 = 0;
            msg->Get(value69);
            this->m_status.m_transport.m_position.x = value69;
        }
        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (transportGuid[1]) {
            uint8_t value76 = 0;
            msg->Get(value76);
            transportGuid[1] ^= value76;
        }

        if (transportGuid[0]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[0] ^= value82;
        }

        if (transportGuid[6]) {
            uint8_t value88 = 0;
            msg->Get(value88);
            transportGuid[6] ^= value88;
        }

        {
            uint32_t value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_time = value93;
        }
        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        {
            uint8_t value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_seat = value102;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value106 = 0;
            msg->Get(value106);
            this->m_status.m_transport.m_time2 = value106;
        }

        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_position.y = value111;
        }
        if (transportGuid[3]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[3] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value127 = 0;
            msg->Get(value127);
            this->m_status.m_transport.m_vehicleId = value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.z = value132;
        }
    }

    if (this->m_status.m_gameTime) {
        uint32_t value138 = 0;
        msg->Get(value138);
        this->m_status.m_gameTime = value138;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_cosAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_xySpeed = value148;
            }
            {
                float value151 = 0;
                msg->Get(value151);
                this->m_status.m_fall.m_direction.m_sinAngle = value151;
            }
        }

        {
            float value156 = 0;
            msg->Get(value156);
            this->m_status.m_fall.m_zSpeed = value156;
        }
        {
            uint32_t value159 = 0;
            msg->Get(value159);
            this->m_status.m_fall.m_time = value159;
        }
    }

    if (this->m_status.m_pitch) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_pitch = value165;
    }

    if (guid[6]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[6] ^= value171;
    }

    if (this->m_status.m_splineElevation) {
        float value177 = 0;
        msg->Get(value177);
        this->m_status.m_splineElevation = value177;
    }

    if (guid[5]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[5] ^= value183;
    }

    if (guid[7]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[7] ^= value189;
    }

    if (guid[4]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[4] ^= value195;
    }

    if (this->m_status.m_facing) {
        float value201 = 0;
        msg->Get(value201);
        this->m_status.m_facing = value201;
    }

    if (guid[0]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[0] ^= value207;
    }

    if (guid[3]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[3] ^= value213;
    }

    if (guid[2]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[2] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006AC050/0x14035BE40
bool ClientMoveUpdateRunSpeed::Handle() {
    if (!ClientMoveUpdateRunSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateRunSpeed::s_handler(this);
}

// PROVEN: 469341d5 IDENTICAL/NONE
// OFFSET: 0x006D4580/0x140390F70
ClientMoveUpdateSwimBackSpeed::ClientMoveUpdateSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8a6c429f IDENTICAL/NONE
// OFFSET: 0x006A10A0/0x14034E2E0
ClientMoveUpdateSwimBackSpeed::~ClientMoveUpdateSwimBackSpeed() {
}

// OFFSET: 0x006C66D0/0x14037CC90
void ClientMoveUpdateSwimBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[2] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (guid[0]) {
        uint8_t value45 = 0;
        msg->Get(value45);
        guid[0] ^= value45;
    }

    if (guid[4]) {
        uint8_t value51 = 0;
        msg->Get(value51);
        guid[4] ^= value51;
    }

    {
        float value56 = 0;
        msg->Get(value56);
        this->m_status.m_position.y = value56;
    }
    if (this->m_status.m_hasTransport) {
        if (transportGuid[7]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[7] ^= value61;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value67 = 0;
            msg->Get(value67);
            this->m_status.m_transport.m_vehicleId = value67;
        }

        if (transportGuid[6]) {
            uint8_t value73 = 0;
            msg->Get(value73);
            transportGuid[6] ^= value73;
        }

        if (transportGuid[4]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[4] ^= value79;
        }

        if (transportGuid[2]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[2] ^= value85;
        }

        if (transportGuid[3]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[3] ^= value91;
        }

        {
            float value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_facing = value96;
        }
        if (transportGuid[5]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[5] ^= value100;
        }

        {
            uint32_t value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_time = value105;
        }
        if (transportGuid[0]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[0] ^= value109;
        }

        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_position.x = value120;
        }
        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_position.z = value123;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value127 = 0;
            msg->Get(value127);
            this->m_status.m_transport.m_time2 = value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        {
            uint8_t value135 = 0;
            msg->Get(value135);
            this->m_status.m_transport.m_seat = value135;
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_cosAngle = value142;
            }
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_sinAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_xySpeed = value148;
            }
        }

        {
            float value153 = 0;
            msg->Get(value153);
            this->m_status.m_fall.m_zSpeed = value153;
        }
        {
            uint32_t value156 = 0;
            msg->Get(value156);
            this->m_status.m_fall.m_time = value156;
        }
    }

    if (this->m_status.m_pitch) {
        float value162 = 0;
        msg->Get(value162);
        this->m_status.m_pitch = value162;
    }

    if (guid[2]) {
        uint8_t value168 = 0;
        msg->Get(value168);
        guid[2] ^= value168;
    }

    if (guid[7]) {
        uint8_t value174 = 0;
        msg->Get(value174);
        guid[7] ^= value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_speed = value179;
    }
    if (this->m_status.m_splineElevation) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_splineElevation = value183;
    }

    {
        float value188 = 0;
        msg->Get(value188);
        this->m_status.m_position.z = value188;
    }
    if (guid[3]) {
        uint8_t value192 = 0;
        msg->Get(value192);
        guid[3] ^= value192;
    }

    {
        float value197 = 0;
        msg->Get(value197);
        this->m_status.m_position.x = value197;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value201 = 0;
        msg->Get(value201);
        this->m_status.m_gameTime = value201;
    }

    if (guid[5]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[5] ^= value207;
    }

    if (this->m_status.m_facing) {
        float value213 = 0;
        msg->Get(value213);
        this->m_status.m_facing = value213;
    }

    if (guid[6]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[6] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }
}

// PROVEN: 6121481c IDENTICAL/NONE
// OFFSET: 0x006D9340
bool ClientMoveUpdateSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CE690
bool ClientMoveUpdateSwimBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[2] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (guid[0]) {
        uint8_t value45 = 0;
        msg->Get(value45);
        guid[0] ^= value45;
    }

    if (guid[4]) {
        uint8_t value51 = 0;
        msg->Get(value51);
        guid[4] ^= value51;
    }

    {
        float value56 = 0;
        msg->Get(value56);
        this->m_status.m_position.y = value56;
    }
    if (this->m_status.m_hasTransport) {
        if (transportGuid[7]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[7] ^= value61;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value67 = 0;
            msg->Get(value67);
            this->m_status.m_transport.m_vehicleId = value67;
        }

        if (transportGuid[6]) {
            uint8_t value73 = 0;
            msg->Get(value73);
            transportGuid[6] ^= value73;
        }

        if (transportGuid[4]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[4] ^= value79;
        }

        if (transportGuid[2]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[2] ^= value85;
        }

        if (transportGuid[3]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[3] ^= value91;
        }

        {
            float value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_facing = value96;
        }
        if (transportGuid[5]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[5] ^= value100;
        }

        {
            uint32_t value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_time = value105;
        }
        if (transportGuid[0]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[0] ^= value109;
        }

        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_position.x = value120;
        }
        {
            float value123 = 0;
            msg->Get(value123);
            this->m_status.m_transport.m_position.z = value123;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value127 = 0;
            msg->Get(value127);
            this->m_status.m_transport.m_time2 = value127;
        }

        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        {
            uint8_t value135 = 0;
            msg->Get(value135);
            this->m_status.m_transport.m_seat = value135;
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_cosAngle = value142;
            }
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_sinAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_xySpeed = value148;
            }
        }

        {
            float value153 = 0;
            msg->Get(value153);
            this->m_status.m_fall.m_zSpeed = value153;
        }
        {
            uint32_t value156 = 0;
            msg->Get(value156);
            this->m_status.m_fall.m_time = value156;
        }
    }

    if (this->m_status.m_pitch) {
        float value162 = 0;
        msg->Get(value162);
        this->m_status.m_pitch = value162;
    }

    if (guid[2]) {
        uint8_t value168 = 0;
        msg->Get(value168);
        guid[2] ^= value168;
    }

    if (guid[7]) {
        uint8_t value174 = 0;
        msg->Get(value174);
        guid[7] ^= value174;
    }

    {
        float value179 = 0;
        msg->Get(value179);
        this->m_speed = value179;
    }
    if (this->m_status.m_splineElevation) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_splineElevation = value183;
    }

    {
        float value188 = 0;
        msg->Get(value188);
        this->m_status.m_position.z = value188;
    }
    if (guid[3]) {
        uint8_t value192 = 0;
        msg->Get(value192);
        guid[3] ^= value192;
    }

    {
        float value197 = 0;
        msg->Get(value197);
        this->m_status.m_position.x = value197;
    }
    if (this->m_status.m_gameTime) {
        uint32_t value201 = 0;
        msg->Get(value201);
        this->m_status.m_gameTime = value201;
    }

    if (guid[5]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[5] ^= value207;
    }

    if (this->m_status.m_facing) {
        float value213 = 0;
        msg->Get(value213);
        this->m_status.m_facing = value213;
    }

    if (guid[6]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[6] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006B2E90/0x1403647A0
bool ClientMoveUpdateSwimBackSpeed::Handle() {
    if (!ClientMoveUpdateSwimBackSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateSwimBackSpeed::s_handler(this);
}

// PROVEN: 4f7545f6 IDENTICAL/NONE
// OFFSET: 0x006D64D0/0x140393A10
ClientMoveUpdateSwimSpeed::ClientMoveUpdateSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5a22da8c IDENTICAL/NONE
// OFFSET: 0x006A2260/0x14034F750
ClientMoveUpdateSwimSpeed::~ClientMoveUpdateSwimSpeed() {
}

// OFFSET: 0x006D0D70/0x14038B940
void ClientMoveUpdateSwimSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_hasTransport) {
        transportGuid[4] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
    }

    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[1] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            uint8_t value46 = 0;
            msg->Get(value46);
            transportGuid[2] ^= value46;
        }

        {
            float value51 = 0;
            msg->Get(value51);
            this->m_status.m_transport.m_position.x = value51;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value55 = 0;
            msg->Get(value55);
            this->m_status.m_transport.m_vehicleId = value55;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value61 = 0;
            msg->Get(value61);
            this->m_status.m_transport.m_time2 = value61;
        }

        if (transportGuid[6]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[6] ^= value67;
        }

        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (transportGuid[0]) {
            uint8_t value76 = 0;
            msg->Get(value76);
            transportGuid[0] ^= value76;
        }

        if (transportGuid[4]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[4] ^= value82;
        }

        {
            float value87 = 0;
            msg->Get(value87);
            this->m_status.m_transport.m_position.y = value87;
        }
        {
            uint32_t value90 = 0;
            msg->Get(value90);
            this->m_status.m_transport.m_time = value90;
        }
        if (transportGuid[1]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[1] ^= value94;
        }

        if (transportGuid[7]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[7] ^= value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        {
            uint8_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_seat = value108;
        }
        if (transportGuid[3]) {
            uint8_t value112 = 0;
            msg->Get(value112);
            transportGuid[3] ^= value112;
        }

        if (transportGuid[5]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[5] ^= value118;
        }
    }

    {
        float value125 = 0;
        msg->Get(value125);
        this->m_status.m_position.x = value125;
    }
    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value130 = 0;
                msg->Get(value130);
                this->m_status.m_fall.m_direction.m_sinAngle = value130;
            }
            {
                float value133 = 0;
                msg->Get(value133);
                this->m_status.m_fall.m_direction.m_xySpeed = value133;
            }
            {
                float value136 = 0;
                msg->Get(value136);
                this->m_status.m_fall.m_direction.m_cosAngle = value136;
            }
        }

        {
            uint32_t value141 = 0;
            msg->Get(value141);
            this->m_status.m_fall.m_time = value141;
        }
        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_fall.m_zSpeed = value144;
        }
    }

    if (guid[7]) {
        uint8_t value150 = 0;
        msg->Get(value150);
        guid[7] ^= value150;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value156 = 0;
        msg->Get(value156);
        this->m_status.m_gameTime = value156;
    }

    {
        float value161 = 0;
        msg->Get(value161);
        this->m_status.m_position.y = value161;
    }
    if (this->m_status.m_splineElevation) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_splineElevation = value165;
    }

    {
        float value170 = 0;
        msg->Get(value170);
        this->m_status.m_position.z = value170;
    }
    {
        float value173 = 0;
        msg->Get(value173);
        this->m_speed = value173;
    }
    if (guid[4]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[4] ^= value177;
    }

    if (this->m_status.m_facing) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_facing = value183;
    }

    if (guid[2]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[2] ^= value189;
    }

    if (guid[3]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[3] ^= value195;
    }

    if (guid[0]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[0] ^= value201;
    }

    if (guid[6]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[6] ^= value207;
    }

    if (guid[1]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[1] ^= value213;
    }

    if (guid[5]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[5] ^= value219;
    }

    if (this->m_status.m_pitch) {
        float value225 = 0;
        msg->Get(value225);
        this->m_status.m_pitch = value225;
    }
}

// PROVEN: a22945c6 IDENTICAL/NONE
// OFFSET: 0x006D96F0
bool ClientMoveUpdateSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CF350
bool ClientMoveUpdateSwimSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[2] = bits.ReadBit();
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasFall = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_hasTransport) {
        transportGuid[4] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
    }

    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    guid[1] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            uint8_t value46 = 0;
            msg->Get(value46);
            transportGuid[2] ^= value46;
        }

        {
            float value51 = 0;
            msg->Get(value51);
            this->m_status.m_transport.m_position.x = value51;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value55 = 0;
            msg->Get(value55);
            this->m_status.m_transport.m_vehicleId = value55;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value61 = 0;
            msg->Get(value61);
            this->m_status.m_transport.m_time2 = value61;
        }

        if (transportGuid[6]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[6] ^= value67;
        }

        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (transportGuid[0]) {
            uint8_t value76 = 0;
            msg->Get(value76);
            transportGuid[0] ^= value76;
        }

        if (transportGuid[4]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[4] ^= value82;
        }

        {
            float value87 = 0;
            msg->Get(value87);
            this->m_status.m_transport.m_position.y = value87;
        }
        {
            uint32_t value90 = 0;
            msg->Get(value90);
            this->m_status.m_transport.m_time = value90;
        }
        if (transportGuid[1]) {
            uint8_t value94 = 0;
            msg->Get(value94);
            transportGuid[1] ^= value94;
        }

        if (transportGuid[7]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[7] ^= value100;
        }

        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_position.z = value105;
        }
        {
            uint8_t value108 = 0;
            msg->Get(value108);
            this->m_status.m_transport.m_seat = value108;
        }
        if (transportGuid[3]) {
            uint8_t value112 = 0;
            msg->Get(value112);
            transportGuid[3] ^= value112;
        }

        if (transportGuid[5]) {
            uint8_t value118 = 0;
            msg->Get(value118);
            transportGuid[5] ^= value118;
        }
    }

    {
        float value125 = 0;
        msg->Get(value125);
        this->m_status.m_position.x = value125;
    }
    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value130 = 0;
                msg->Get(value130);
                this->m_status.m_fall.m_direction.m_sinAngle = value130;
            }
            {
                float value133 = 0;
                msg->Get(value133);
                this->m_status.m_fall.m_direction.m_xySpeed = value133;
            }
            {
                float value136 = 0;
                msg->Get(value136);
                this->m_status.m_fall.m_direction.m_cosAngle = value136;
            }
        }

        {
            uint32_t value141 = 0;
            msg->Get(value141);
            this->m_status.m_fall.m_time = value141;
        }
        {
            float value144 = 0;
            msg->Get(value144);
            this->m_status.m_fall.m_zSpeed = value144;
        }
    }

    if (guid[7]) {
        uint8_t value150 = 0;
        msg->Get(value150);
        guid[7] ^= value150;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value156 = 0;
        msg->Get(value156);
        this->m_status.m_gameTime = value156;
    }

    {
        float value161 = 0;
        msg->Get(value161);
        this->m_status.m_position.y = value161;
    }
    if (this->m_status.m_splineElevation) {
        float value165 = 0;
        msg->Get(value165);
        this->m_status.m_splineElevation = value165;
    }

    {
        float value170 = 0;
        msg->Get(value170);
        this->m_status.m_position.z = value170;
    }
    {
        float value173 = 0;
        msg->Get(value173);
        this->m_speed = value173;
    }
    if (guid[4]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[4] ^= value177;
    }

    if (this->m_status.m_facing) {
        float value183 = 0;
        msg->Get(value183);
        this->m_status.m_facing = value183;
    }

    if (guid[2]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[2] ^= value189;
    }

    if (guid[3]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[3] ^= value195;
    }

    if (guid[0]) {
        uint8_t value201 = 0;
        msg->Get(value201);
        guid[0] ^= value201;
    }

    if (guid[6]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[6] ^= value207;
    }

    if (guid[1]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[1] ^= value213;
    }

    if (guid[5]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[5] ^= value219;
    }

    if (this->m_status.m_pitch) {
        float value225 = 0;
        msg->Get(value225);
        this->m_status.m_pitch = value225;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006A5AD0/0x140353A30
bool ClientMoveUpdateSwimSpeed::Handle() {
    if (!ClientMoveUpdateSwimSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateSwimSpeed::s_handler(this);
}

// PROVEN: 231d119f IDENTICAL/NONE
// OFFSET: 0x006D9110/0x1403975E0
ClientMoveUpdateTeleport::ClientMoveUpdateTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 543cae81 IDENTICAL/NONE
// OFFSET: 0x006A2EF0/0x140350400
ClientMoveUpdateTeleport::~ClientMoveUpdateTeleport() {
}

// OFFSET: 0x006CBA10/0x140384210
void ClientMoveUpdateTeleport::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.y = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    JamBitReader bits(msg);
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[1] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
    }

    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[1] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value54 = 0;
        msg->Get(value54);
        guid[7] ^= value54;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[3]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[3] ^= value61;
        }

        if (transportGuid[4]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[4] ^= value67;
        }

        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value76 = 0;
            msg->Get(value76);
            this->m_status.m_transport.m_vehicleId = value76;
        }

        if (transportGuid[1]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[1] ^= value82;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value88 = 0;
            msg->Get(value88);
            this->m_status.m_transport.m_time2 = value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        if (transportGuid[0]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[0] ^= value103;
        }

        if (transportGuid[6]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[6] ^= value109;
        }

        if (transportGuid[5]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[5] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        {
            uint8_t value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_seat = value126;
        }
        {
            uint32_t value129 = 0;
            msg->Get(value129);
            this->m_status.m_transport.m_time = value129;
        }
        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        {
            float value135 = 0;
            msg->Get(value135);
            this->m_status.m_transport.m_position.x = value135;
        }
    }

    if (guid[6]) {
        uint8_t value141 = 0;
        msg->Get(value141);
        guid[6] ^= value141;
    }

    if (this->m_status.m_pitch) {
        float value147 = 0;
        msg->Get(value147);
        this->m_status.m_pitch = value147;
    }

    if (this->m_status.m_splineElevation) {
        float value153 = 0;
        msg->Get(value153);
        this->m_status.m_splineElevation = value153;
    }

    if (this->m_status.m_facing) {
        float value159 = 0;
        msg->Get(value159);
        this->m_status.m_facing = value159;
    }

    if (guid[2]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[2] ^= value165;
    }

    if (guid[3]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[3] ^= value171;
    }

    if (guid[1]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[1] ^= value177;
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value183 = 0;
            msg->Get(value183);
            this->m_status.m_fall.m_time = value183;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value187 = 0;
                msg->Get(value187);
                this->m_status.m_fall.m_direction.m_xySpeed = value187;
            }
            {
                float value190 = 0;
                msg->Get(value190);
                this->m_status.m_fall.m_direction.m_cosAngle = value190;
            }
            {
                float value193 = 0;
                msg->Get(value193);
                this->m_status.m_fall.m_direction.m_sinAngle = value193;
            }
        }

        {
            float value198 = 0;
            msg->Get(value198);
            this->m_status.m_fall.m_zSpeed = value198;
        }
    }

    if (guid[5]) {
        uint8_t value204 = 0;
        msg->Get(value204);
        guid[5] ^= value204;
    }

    if (guid[4]) {
        uint8_t value210 = 0;
        msg->Get(value210);
        guid[4] ^= value210;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value216 = 0;
        msg->Get(value216);
        this->m_status.m_gameTime = value216;
    }

    if (guid[0]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[0] ^= value222;
    }
}

// PROVEN: 90089e2b IDENTICAL/NONE
// OFFSET: 0x006D7780
bool ClientMoveUpdateTeleport::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C9140
bool ClientMoveUpdateTeleport::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_status.m_position.z = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.y = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    JamBitReader bits(msg);
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[1] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
    }

    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    guid[1] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value54 = 0;
        msg->Get(value54);
        guid[7] ^= value54;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[3]) {
            uint8_t value61 = 0;
            msg->Get(value61);
            transportGuid[3] ^= value61;
        }

        if (transportGuid[4]) {
            uint8_t value67 = 0;
            msg->Get(value67);
            transportGuid[4] ^= value67;
        }

        {
            float value72 = 0;
            msg->Get(value72);
            this->m_status.m_transport.m_facing = value72;
        }
        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value76 = 0;
            msg->Get(value76);
            this->m_status.m_transport.m_vehicleId = value76;
        }

        if (transportGuid[1]) {
            uint8_t value82 = 0;
            msg->Get(value82);
            transportGuid[1] ^= value82;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value88 = 0;
            msg->Get(value88);
            this->m_status.m_transport.m_time2 = value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        if (transportGuid[7]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[7] ^= value97;
        }

        if (transportGuid[0]) {
            uint8_t value103 = 0;
            msg->Get(value103);
            transportGuid[0] ^= value103;
        }

        if (transportGuid[6]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[6] ^= value109;
        }

        if (transportGuid[5]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[5] ^= value115;
        }

        if (transportGuid[2]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[2] ^= value121;
        }

        {
            uint8_t value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_seat = value126;
        }
        {
            uint32_t value129 = 0;
            msg->Get(value129);
            this->m_status.m_transport.m_time = value129;
        }
        {
            float value132 = 0;
            msg->Get(value132);
            this->m_status.m_transport.m_position.y = value132;
        }
        {
            float value135 = 0;
            msg->Get(value135);
            this->m_status.m_transport.m_position.x = value135;
        }
    }

    if (guid[6]) {
        uint8_t value141 = 0;
        msg->Get(value141);
        guid[6] ^= value141;
    }

    if (this->m_status.m_pitch) {
        float value147 = 0;
        msg->Get(value147);
        this->m_status.m_pitch = value147;
    }

    if (this->m_status.m_splineElevation) {
        float value153 = 0;
        msg->Get(value153);
        this->m_status.m_splineElevation = value153;
    }

    if (this->m_status.m_facing) {
        float value159 = 0;
        msg->Get(value159);
        this->m_status.m_facing = value159;
    }

    if (guid[2]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[2] ^= value165;
    }

    if (guid[3]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[3] ^= value171;
    }

    if (guid[1]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[1] ^= value177;
    }

    if (this->m_status.m_hasFall) {
        {
            uint32_t value183 = 0;
            msg->Get(value183);
            this->m_status.m_fall.m_time = value183;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value187 = 0;
                msg->Get(value187);
                this->m_status.m_fall.m_direction.m_xySpeed = value187;
            }
            {
                float value190 = 0;
                msg->Get(value190);
                this->m_status.m_fall.m_direction.m_cosAngle = value190;
            }
            {
                float value193 = 0;
                msg->Get(value193);
                this->m_status.m_fall.m_direction.m_sinAngle = value193;
            }
        }

        {
            float value198 = 0;
            msg->Get(value198);
            this->m_status.m_fall.m_zSpeed = value198;
        }
    }

    if (guid[5]) {
        uint8_t value204 = 0;
        msg->Get(value204);
        guid[5] ^= value204;
    }

    if (guid[4]) {
        uint8_t value210 = 0;
        msg->Get(value210);
        guid[4] ^= value210;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value216 = 0;
        msg->Get(value216);
        this->m_status.m_gameTime = value216;
    }

    if (guid[0]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[0] ^= value222;
    }

    return msg->IsRead() && this->m_status.IsValid();
}

// OFFSET: 0x006BC810/0x140370CD0
bool ClientMoveUpdateTeleport::Handle() {
    if (!ClientMoveUpdateTeleport::s_handler) {
        return false;
    }

    return ClientMoveUpdateTeleport::s_handler(this);
}

// PROVEN: 5990ff89 IDENTICAL/NONE
// OFFSET: 0x006D90D0/0x140397590
ClientMoveUpdateTurnRate::ClientMoveUpdateTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f43c63fd IDENTICAL/NONE
// OFFSET: 0x006A1970/0x14034ED10
ClientMoveUpdateTurnRate::~ClientMoveUpdateTurnRate() {
}

// OFFSET: 0x006D1680/0x14038C780
void ClientMoveUpdateTurnRate::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_rate = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.y = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[5] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[3] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[2] ^= value58;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value64 = 0;
            msg->Get(value64);
            this->m_status.m_transport.m_vehicleId = value64;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value70 = 0;
            msg->Get(value70);
            this->m_status.m_transport.m_time2 = value70;
        }

        {
            uint32_t value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_time = value75;
        }
        if (transportGuid[6]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[6] ^= value79;
        }

        if (transportGuid[1]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[1] ^= value85;
        }

        if (transportGuid[7]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[7] ^= value91;
        }

        if (transportGuid[0]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[0] ^= value97;
        }

        {
            float value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_position.x = value102;
        }
        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_facing = value105;
        }
        if (transportGuid[3]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[3] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.z = value114;
        }
        {
            uint8_t value117 = 0;
            msg->Get(value117);
            this->m_status.m_transport.m_seat = value117;
        }
        if (transportGuid[4]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[4] ^= value121;
        }

        {
            float value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_position.y = value126;
        }
        if (transportGuid[5]) {
            uint8_t value130 = 0;
            msg->Get(value130);
            transportGuid[5] ^= value130;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value138 = 0;
        msg->Get(value138);
        this->m_status.m_splineElevation = value138;
    }

    if (guid[2]) {
        uint8_t value144 = 0;
        msg->Get(value144);
        guid[2] ^= value144;
    }

    if (this->m_status.m_facing) {
        float value150 = 0;
        msg->Get(value150);
        this->m_status.m_facing = value150;
    }

    if (this->m_status.m_pitch) {
        float value156 = 0;
        msg->Get(value156);
        this->m_status.m_pitch = value156;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_sinAngle = value163;
            }
            {
                float value166 = 0;
                msg->Get(value166);
                this->m_status.m_fall.m_direction.m_cosAngle = value166;
            }
            {
                float value169 = 0;
                msg->Get(value169);
                this->m_status.m_fall.m_direction.m_xySpeed = value169;
            }
        }

        {
            uint32_t value174 = 0;
            msg->Get(value174);
            this->m_status.m_fall.m_time = value174;
        }
        {
            float value177 = 0;
            msg->Get(value177);
            this->m_status.m_fall.m_zSpeed = value177;
        }
    }

    if (guid[7]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[7] ^= value183;
    }

    if (guid[3]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[3] ^= value189;
    }

    if (guid[6]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[6] ^= value195;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value201 = 0;
        msg->Get(value201);
        this->m_status.m_gameTime = value201;
    }

    if (guid[4]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[4] ^= value207;
    }

    if (guid[5]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[5] ^= value213;
    }

    if (guid[0]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[0] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }
}

// PROVEN: f3a52abe IDENTICAL/NONE
// OFFSET: 0x006D46B0
bool ClientMoveUpdateTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D2000
bool ClientMoveUpdateTurnRate::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_rate = value0;
    }
    {
        float value3 = 0;
        msg->Get(value3);
        this->m_status.m_position.y = value3;
    }
    {
        float value6 = 0;
        msg->Get(value6);
        this->m_status.m_position.x = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_status.m_position.z = value9;
    }
    JamBitReader bits(msg);
    this->m_status.m_hasSpline = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[0] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
        transportGuid[6] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
    }

    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    guid[5] = bits.ReadBit();
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[3] = bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[2] ^= value58;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value64 = 0;
            msg->Get(value64);
            this->m_status.m_transport.m_vehicleId = value64;
        }

        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value70 = 0;
            msg->Get(value70);
            this->m_status.m_transport.m_time2 = value70;
        }

        {
            uint32_t value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_time = value75;
        }
        if (transportGuid[6]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[6] ^= value79;
        }

        if (transportGuid[1]) {
            uint8_t value85 = 0;
            msg->Get(value85);
            transportGuid[1] ^= value85;
        }

        if (transportGuid[7]) {
            uint8_t value91 = 0;
            msg->Get(value91);
            transportGuid[7] ^= value91;
        }

        if (transportGuid[0]) {
            uint8_t value97 = 0;
            msg->Get(value97);
            transportGuid[0] ^= value97;
        }

        {
            float value102 = 0;
            msg->Get(value102);
            this->m_status.m_transport.m_position.x = value102;
        }
        {
            float value105 = 0;
            msg->Get(value105);
            this->m_status.m_transport.m_facing = value105;
        }
        if (transportGuid[3]) {
            uint8_t value109 = 0;
            msg->Get(value109);
            transportGuid[3] ^= value109;
        }

        {
            float value114 = 0;
            msg->Get(value114);
            this->m_status.m_transport.m_position.z = value114;
        }
        {
            uint8_t value117 = 0;
            msg->Get(value117);
            this->m_status.m_transport.m_seat = value117;
        }
        if (transportGuid[4]) {
            uint8_t value121 = 0;
            msg->Get(value121);
            transportGuid[4] ^= value121;
        }

        {
            float value126 = 0;
            msg->Get(value126);
            this->m_status.m_transport.m_position.y = value126;
        }
        if (transportGuid[5]) {
            uint8_t value130 = 0;
            msg->Get(value130);
            transportGuid[5] ^= value130;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value138 = 0;
        msg->Get(value138);
        this->m_status.m_splineElevation = value138;
    }

    if (guid[2]) {
        uint8_t value144 = 0;
        msg->Get(value144);
        guid[2] ^= value144;
    }

    if (this->m_status.m_facing) {
        float value150 = 0;
        msg->Get(value150);
        this->m_status.m_facing = value150;
    }

    if (this->m_status.m_pitch) {
        float value156 = 0;
        msg->Get(value156);
        this->m_status.m_pitch = value156;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value163 = 0;
                msg->Get(value163);
                this->m_status.m_fall.m_direction.m_sinAngle = value163;
            }
            {
                float value166 = 0;
                msg->Get(value166);
                this->m_status.m_fall.m_direction.m_cosAngle = value166;
            }
            {
                float value169 = 0;
                msg->Get(value169);
                this->m_status.m_fall.m_direction.m_xySpeed = value169;
            }
        }

        {
            uint32_t value174 = 0;
            msg->Get(value174);
            this->m_status.m_fall.m_time = value174;
        }
        {
            float value177 = 0;
            msg->Get(value177);
            this->m_status.m_fall.m_zSpeed = value177;
        }
    }

    if (guid[7]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[7] ^= value183;
    }

    if (guid[3]) {
        uint8_t value189 = 0;
        msg->Get(value189);
        guid[3] ^= value189;
    }

    if (guid[6]) {
        uint8_t value195 = 0;
        msg->Get(value195);
        guid[6] ^= value195;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value201 = 0;
        msg->Get(value201);
        this->m_status.m_gameTime = value201;
    }

    if (guid[4]) {
        uint8_t value207 = 0;
        msg->Get(value207);
        guid[4] ^= value207;
    }

    if (guid[5]) {
        uint8_t value213 = 0;
        msg->Get(value213);
        guid[5] ^= value213;
    }

    if (guid[0]) {
        uint8_t value219 = 0;
        msg->Get(value219);
        guid[0] ^= value219;
    }

    if (guid[1]) {
        uint8_t value225 = 0;
        msg->Get(value225);
        guid[1] ^= value225;
    }

    return msg->IsRead() && _finite(this->m_rate) && this->m_status.IsValid();
}

// OFFSET: 0x006C09B0/0x140375D80
bool ClientMoveUpdateTurnRate::Handle() {
    if (!ClientMoveUpdateTurnRate::s_handler) {
        return false;
    }

    return ClientMoveUpdateTurnRate::s_handler(this);
}

// PROVEN: b881f9d0 IDENTICAL/NONE
// OFFSET: 0x006DACF0/0x1403994F0
ClientMoveUpdateWalkSpeed::ClientMoveUpdateWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9e5d4024 IDENTICAL/NONE
// OFFSET: 0x006A1F20/0x14034F3A0
ClientMoveUpdateWalkSpeed::~ClientMoveUpdateWalkSpeed() {
}

// OFFSET: 0x006D30C0/0x14038EFE0
void ClientMoveUpdateWalkSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[6] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasSpline = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (this->m_status.m_pitch) {
        float value45 = 0;
        msg->Get(value45);
        this->m_status.m_pitch = value45;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[6]) {
            uint8_t value52 = 0;
            msg->Get(value52);
            transportGuid[6] ^= value52;
        }

        if (transportGuid[0]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[0] ^= value58;
        }

        if (transportGuid[4]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[4] ^= value64;
        }

        if (transportGuid[2]) {
            uint8_t value70 = 0;
            msg->Get(value70);
            transportGuid[2] ^= value70;
        }

        {
            float value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_position.x = value75;
        }
        if (transportGuid[7]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[7] ^= value79;
        }

        {
            uint32_t value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_time = value84;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value88 = 0;
            msg->Get(value88);
            this->m_status.m_transport.m_time2 = value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        {
            uint8_t value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_seat = value96;
        }
        if (transportGuid[5]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[5] ^= value100;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value106 = 0;
            msg->Get(value106);
            this->m_status.m_transport.m_vehicleId = value106;
        }

        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_facing = value111;
        }
        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_position.y = value120;
        }
        if (transportGuid[3]) {
            uint8_t value124 = 0;
            msg->Get(value124);
            transportGuid[3] ^= value124;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value132 = 0;
        msg->Get(value132);
        this->m_status.m_splineElevation = value132;
    }

    if (this->m_status.m_hasFall) {
        {
            float value138 = 0;
            msg->Get(value138);
            this->m_status.m_fall.m_zSpeed = value138;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_xySpeed = value142;
            }
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_sinAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_cosAngle = value148;
            }
        }

        {
            uint32_t value153 = 0;
            msg->Get(value153);
            this->m_status.m_fall.m_time = value153;
        }
    }

    if (guid[1]) {
        uint8_t value159 = 0;
        msg->Get(value159);
        guid[1] ^= value159;
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (guid[2]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[2] ^= value171;
    }

    if (guid[6]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[6] ^= value177;
    }

    if (guid[7]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[7] ^= value183;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value189 = 0;
        msg->Get(value189);
        this->m_status.m_gameTime = value189;
    }

    if (this->m_status.m_facing) {
        float value195 = 0;
        msg->Get(value195);
        this->m_status.m_facing = value195;
    }

    {
        float value200 = 0;
        msg->Get(value200);
        this->m_status.m_position.y = value200;
    }
    if (guid[0]) {
        uint8_t value204 = 0;
        msg->Get(value204);
        guid[0] ^= value204;
    }

    {
        float value209 = 0;
        msg->Get(value209);
        this->m_status.m_position.z = value209;
    }
    {
        float value212 = 0;
        msg->Get(value212);
        this->m_status.m_position.x = value212;
    }
    if (guid[3]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[3] ^= value216;
    }

    if (guid[5]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[5] ^= value222;
    }

    {
        float value227 = 0;
        msg->Get(value227);
        this->m_speed = value227;
    }
}

// PROVEN: af938b5f IDENTICAL/NONE
// OFFSET: 0x006D3060
bool ClientMoveUpdateWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CDEB0
bool ClientMoveUpdateWalkSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_status.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&this->m_status.m_transport.m_guid);
    this->m_status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
    this->m_status.m_heightChangeFailed = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
    guid[2] = bits.ReadBit();
    this->m_status.m_hasTransport = bits.ReadBit();
    if (this->m_status.m_hasTransport) {
        transportGuid[6] = bits.ReadBit();
        transportGuid[3] = bits.ReadBit();
        transportGuid[2] = bits.ReadBit();
        transportGuid[0] = bits.ReadBit();
        transportGuid[4] = bits.ReadBit();
        this->m_status.m_transport.m_hasTime2 = bits.ReadBit();
        transportGuid[7] = bits.ReadBit();
        transportGuid[1] = bits.ReadBit();
        transportGuid[5] = bits.ReadBit();
        this->m_status.m_transport.m_hasVehicleId = bits.ReadBit();
    }

    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_status.m_hasFall = bits.ReadBit();
    guid[0] = bits.ReadBit();
    this->m_status.m_moveExtraFlags = !bits.ReadBit();
    this->m_status.m_gameTime = !bits.ReadBit();
    this->m_status.m_moveFlags = !bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (this->m_status.m_hasFall) {
        this->m_status.m_fall.m_hasDirection = bits.ReadBit();
    }

    if (this->m_status.m_moveExtraFlags) {
        this->m_status.m_moveExtraFlags = bits.ReadBits<12>();
    }

    if (this->m_status.m_moveFlags) {
        this->m_status.m_moveFlags = bits.ReadBits<30>();
    }

    this->m_status.m_hasSpline = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (this->m_status.m_pitch) {
        float value45 = 0;
        msg->Get(value45);
        this->m_status.m_pitch = value45;
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[6]) {
            uint8_t value52 = 0;
            msg->Get(value52);
            transportGuid[6] ^= value52;
        }

        if (transportGuid[0]) {
            uint8_t value58 = 0;
            msg->Get(value58);
            transportGuid[0] ^= value58;
        }

        if (transportGuid[4]) {
            uint8_t value64 = 0;
            msg->Get(value64);
            transportGuid[4] ^= value64;
        }

        if (transportGuid[2]) {
            uint8_t value70 = 0;
            msg->Get(value70);
            transportGuid[2] ^= value70;
        }

        {
            float value75 = 0;
            msg->Get(value75);
            this->m_status.m_transport.m_position.x = value75;
        }
        if (transportGuid[7]) {
            uint8_t value79 = 0;
            msg->Get(value79);
            transportGuid[7] ^= value79;
        }

        {
            uint32_t value84 = 0;
            msg->Get(value84);
            this->m_status.m_transport.m_time = value84;
        }
        if (this->m_status.m_transport.m_hasTime2) {
            uint32_t value88 = 0;
            msg->Get(value88);
            this->m_status.m_transport.m_time2 = value88;
        }

        {
            float value93 = 0;
            msg->Get(value93);
            this->m_status.m_transport.m_position.z = value93;
        }
        {
            uint8_t value96 = 0;
            msg->Get(value96);
            this->m_status.m_transport.m_seat = value96;
        }
        if (transportGuid[5]) {
            uint8_t value100 = 0;
            msg->Get(value100);
            transportGuid[5] ^= value100;
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            uint32_t value106 = 0;
            msg->Get(value106);
            this->m_status.m_transport.m_vehicleId = value106;
        }

        {
            float value111 = 0;
            msg->Get(value111);
            this->m_status.m_transport.m_facing = value111;
        }
        if (transportGuid[1]) {
            uint8_t value115 = 0;
            msg->Get(value115);
            transportGuid[1] ^= value115;
        }

        {
            float value120 = 0;
            msg->Get(value120);
            this->m_status.m_transport.m_position.y = value120;
        }
        if (transportGuid[3]) {
            uint8_t value124 = 0;
            msg->Get(value124);
            transportGuid[3] ^= value124;
        }
    }

    if (this->m_status.m_splineElevation) {
        float value132 = 0;
        msg->Get(value132);
        this->m_status.m_splineElevation = value132;
    }

    if (this->m_status.m_hasFall) {
        {
            float value138 = 0;
            msg->Get(value138);
            this->m_status.m_fall.m_zSpeed = value138;
        }
        if (this->m_status.m_fall.m_hasDirection) {
            {
                float value142 = 0;
                msg->Get(value142);
                this->m_status.m_fall.m_direction.m_xySpeed = value142;
            }
            {
                float value145 = 0;
                msg->Get(value145);
                this->m_status.m_fall.m_direction.m_sinAngle = value145;
            }
            {
                float value148 = 0;
                msg->Get(value148);
                this->m_status.m_fall.m_direction.m_cosAngle = value148;
            }
        }

        {
            uint32_t value153 = 0;
            msg->Get(value153);
            this->m_status.m_fall.m_time = value153;
        }
    }

    if (guid[1]) {
        uint8_t value159 = 0;
        msg->Get(value159);
        guid[1] ^= value159;
    }

    if (guid[4]) {
        uint8_t value165 = 0;
        msg->Get(value165);
        guid[4] ^= value165;
    }

    if (guid[2]) {
        uint8_t value171 = 0;
        msg->Get(value171);
        guid[2] ^= value171;
    }

    if (guid[6]) {
        uint8_t value177 = 0;
        msg->Get(value177);
        guid[6] ^= value177;
    }

    if (guid[7]) {
        uint8_t value183 = 0;
        msg->Get(value183);
        guid[7] ^= value183;
    }

    if (this->m_status.m_gameTime) {
        uint32_t value189 = 0;
        msg->Get(value189);
        this->m_status.m_gameTime = value189;
    }

    if (this->m_status.m_facing) {
        float value195 = 0;
        msg->Get(value195);
        this->m_status.m_facing = value195;
    }

    {
        float value200 = 0;
        msg->Get(value200);
        this->m_status.m_position.y = value200;
    }
    if (guid[0]) {
        uint8_t value204 = 0;
        msg->Get(value204);
        guid[0] ^= value204;
    }

    {
        float value209 = 0;
        msg->Get(value209);
        this->m_status.m_position.z = value209;
    }
    {
        float value212 = 0;
        msg->Get(value212);
        this->m_status.m_position.x = value212;
    }
    if (guid[3]) {
        uint8_t value216 = 0;
        msg->Get(value216);
        guid[3] ^= value216;
    }

    if (guid[5]) {
        uint8_t value222 = 0;
        msg->Get(value222);
        guid[5] ^= value222;
    }

    {
        float value227 = 0;
        msg->Get(value227);
        this->m_speed = value227;
    }

    return msg->IsRead() && _finite(this->m_speed) && this->m_status.IsValid();
}

// OFFSET: 0x006BCD10/0x1403712E0
bool ClientMoveUpdateWalkSpeed::Handle() {
    if (!ClientMoveUpdateWalkSpeed::s_handler) {
        return false;
    }

    return ClientMoveUpdateWalkSpeed::s_handler(this);
}

// PROVEN: 891d7d49 IDENTICAL/NONE
// OFFSET: 0x006B12A0/0x1403623B0
ClientPlaySpellVisual::ClientPlaySpellVisual(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9ed6cc67 IDENTICAL/NONE
// OFFSET: 0x006A1E20/0x14034F290
ClientPlaySpellVisual::~ClientPlaySpellVisual() {
}

// OFFSET: 0x006A5F00/0x140354010
void ClientPlaySpellVisual::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    uint8_t* targetGuid = reinterpret_cast<uint8_t*>(&this->m_targetGuid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_targetPosition.z = value0;
    }
    {
        uint32_t value3 = 0;
        msg->Get(value3);
        this->m_spellVisualId = value3;
    }
    {
        uint16_t value6 = 0;
        msg->Get(value6);
        this->m_reflectStatus = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_travelSpeed = value9;
    }
    {
        float value12 = 0;
        msg->Get(value12);
        this->m_targetPosition.x = value12;
    }
    {
        uint16_t value15 = 0;
        msg->Get(value15);
        this->m_missReason = value15;
    }
    {
        float value18 = 0;
        msg->Get(value18);
        this->m_targetPosition.y = value18;
    }
    JamBitReader bits(msg);
    targetGuid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    targetGuid[2] = bits.ReadBit();
    targetGuid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    targetGuid[6] = bits.ReadBit();
    targetGuid[0] = bits.ReadBit();
    this->m_speedAsTime = bits.ReadBit();
    guid[6] = bits.ReadBit();
    targetGuid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    targetGuid[3] = bits.ReadBit();
    targetGuid[4] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[7] ^= value40;
    }

    if (guid[4]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[4] ^= value46;
    }

    if (targetGuid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        targetGuid[7] ^= value52;
    }

    if (guid[1]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[1] ^= value58;
    }

    if (guid[3]) {
        uint8_t value64 = 0;
        msg->Get(value64);
        guid[3] ^= value64;
    }

    if (guid[0]) {
        uint8_t value70 = 0;
        msg->Get(value70);
        guid[0] ^= value70;
    }

    if (guid[6]) {
        uint8_t value76 = 0;
        msg->Get(value76);
        guid[6] ^= value76;
    }

    if (targetGuid[0]) {
        uint8_t value82 = 0;
        msg->Get(value82);
        targetGuid[0] ^= value82;
    }

    if (targetGuid[4]) {
        uint8_t value88 = 0;
        msg->Get(value88);
        targetGuid[4] ^= value88;
    }

    if (guid[5]) {
        uint8_t value94 = 0;
        msg->Get(value94);
        guid[5] ^= value94;
    }

    if (targetGuid[1]) {
        uint8_t value100 = 0;
        msg->Get(value100);
        targetGuid[1] ^= value100;
    }

    if (targetGuid[5]) {
        uint8_t value106 = 0;
        msg->Get(value106);
        targetGuid[5] ^= value106;
    }

    if (targetGuid[6]) {
        uint8_t value112 = 0;
        msg->Get(value112);
        targetGuid[6] ^= value112;
    }

    if (targetGuid[2]) {
        uint8_t value118 = 0;
        msg->Get(value118);
        targetGuid[2] ^= value118;
    }

    if (guid[2]) {
        uint8_t value124 = 0;
        msg->Get(value124);
        guid[2] ^= value124;
    }

    if (targetGuid[3]) {
        uint8_t value130 = 0;
        msg->Get(value130);
        targetGuid[3] ^= value130;
    }
}

// PROVEN: ee100563 IDENTICAL/NONE
// OFFSET: 0x006B1690
bool ClientPlaySpellVisual::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006A9470
bool ClientPlaySpellVisual::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    uint8_t* targetGuid = reinterpret_cast<uint8_t*>(&this->m_targetGuid);
    {
        float value0 = 0;
        msg->Get(value0);
        this->m_targetPosition.z = value0;
    }
    {
        uint32_t value3 = 0;
        msg->Get(value3);
        this->m_spellVisualId = value3;
    }
    {
        uint16_t value6 = 0;
        msg->Get(value6);
        this->m_reflectStatus = value6;
    }
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_travelSpeed = value9;
    }
    {
        float value12 = 0;
        msg->Get(value12);
        this->m_targetPosition.x = value12;
    }
    {
        uint16_t value15 = 0;
        msg->Get(value15);
        this->m_missReason = value15;
    }
    {
        float value18 = 0;
        msg->Get(value18);
        this->m_targetPosition.y = value18;
    }
    JamBitReader bits(msg);
    targetGuid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    targetGuid[2] = bits.ReadBit();
    targetGuid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    targetGuid[6] = bits.ReadBit();
    targetGuid[0] = bits.ReadBit();
    this->m_speedAsTime = bits.ReadBit();
    guid[6] = bits.ReadBit();
    targetGuid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    targetGuid[3] = bits.ReadBit();
    targetGuid[4] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[7] ^= value40;
    }

    if (guid[4]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[4] ^= value46;
    }

    if (targetGuid[7]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        targetGuid[7] ^= value52;
    }

    if (guid[1]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[1] ^= value58;
    }

    if (guid[3]) {
        uint8_t value64 = 0;
        msg->Get(value64);
        guid[3] ^= value64;
    }

    if (guid[0]) {
        uint8_t value70 = 0;
        msg->Get(value70);
        guid[0] ^= value70;
    }

    if (guid[6]) {
        uint8_t value76 = 0;
        msg->Get(value76);
        guid[6] ^= value76;
    }

    if (targetGuid[0]) {
        uint8_t value82 = 0;
        msg->Get(value82);
        targetGuid[0] ^= value82;
    }

    if (targetGuid[4]) {
        uint8_t value88 = 0;
        msg->Get(value88);
        targetGuid[4] ^= value88;
    }

    if (guid[5]) {
        uint8_t value94 = 0;
        msg->Get(value94);
        guid[5] ^= value94;
    }

    if (targetGuid[1]) {
        uint8_t value100 = 0;
        msg->Get(value100);
        targetGuid[1] ^= value100;
    }

    if (targetGuid[5]) {
        uint8_t value106 = 0;
        msg->Get(value106);
        targetGuid[5] ^= value106;
    }

    if (targetGuid[6]) {
        uint8_t value112 = 0;
        msg->Get(value112);
        targetGuid[6] ^= value112;
    }

    if (targetGuid[2]) {
        uint8_t value118 = 0;
        msg->Get(value118);
        targetGuid[2] ^= value118;
    }

    if (guid[2]) {
        uint8_t value124 = 0;
        msg->Get(value124);
        guid[2] ^= value124;
    }

    if (targetGuid[3]) {
        uint8_t value130 = 0;
        msg->Get(value130);
        targetGuid[3] ^= value130;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A9040/0x1403580F0
bool ClientPlaySpellVisual::Handle() {
    if (!ClientPlaySpellVisual::s_handler) {
        return false;
    }

    return ClientPlaySpellVisual::s_handler(this);
}

// PROVEN: debfda3a IDENTICAL/NONE
// OFFSET: 0x006C98B0/0x1403812F0
ClientSetFootstepSoundsSuppressed::ClientSetFootstepSoundsSuppressed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d5fc10db IDENTICAL/NONE
// OFFSET: 0x006A2370/0x14034F890
ClientSetFootstepSoundsSuppressed::~ClientSetFootstepSoundsSuppressed() {
}

// PROVEN: bc868ba6 IDENTICAL/NONE
// OFFSET: 0x006BE600/0x140373390
void ClientSetFootstepSoundsSuppressed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    this->m_suppressed = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[1] ^= value11;
    }

    if (guid[7]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[7] ^= value17;
    }

    if (guid[0]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[0] ^= value23;
    }

    if (guid[2]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[2] ^= value29;
    }

    if (guid[6]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[6] ^= value35;
    }

    if (guid[4]) {
        uint8_t value41 = 0;
        msg->Get(value41);
        guid[4] ^= value41;
    }

    if (guid[3]) {
        uint8_t value47 = 0;
        msg->Get(value47);
        guid[3] ^= value47;
    }

    if (guid[5]) {
        uint8_t value53 = 0;
        msg->Get(value53);
        guid[5] ^= value53;
    }
}

// PROVEN: 3bc04411 IDENTICAL/NONE
// OFFSET: 0x006B7650
bool ClientSetFootstepSoundsSuppressed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 75cae6f1 IDENTICAL/NONE
// OFFSET: 0x006AE260
bool ClientSetFootstepSoundsSuppressed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    this->m_suppressed = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[1] ^= value11;
    }

    if (guid[7]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[7] ^= value17;
    }

    if (guid[0]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[0] ^= value23;
    }

    if (guid[2]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[2] ^= value29;
    }

    if (guid[6]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[6] ^= value35;
    }

    if (guid[4]) {
        uint8_t value41 = 0;
        msg->Get(value41);
        guid[4] ^= value41;
    }

    if (guid[3]) {
        uint8_t value47 = 0;
        msg->Get(value47);
        guid[3] ^= value47;
    }

    if (guid[5]) {
        uint8_t value53 = 0;
        msg->Get(value53);
        guid[5] ^= value53;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B9030/0x14036C4C0
bool ClientSetFootstepSoundsSuppressed::Handle() {
    if (!ClientSetFootstepSoundsSuppressed::s_handler) {
        return false;
    }

    return ClientSetFootstepSoundsSuppressed::s_handler(this);
}

// PROVEN: df10bd3f IDENTICAL/NONE
// OFFSET: 0x006C6070/0x14037C490
ClientSplineMoveCollisionDisable::ClientSplineMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 92d9a16f IDENTICAL/NONE
// OFFSET: 0x006A2270/0x14034F770
ClientSplineMoveCollisionDisable::~ClientSplineMoveCollisionDisable() {
}

// PROVEN: f6b924d5 IDENTICAL/NONE
// OFFSET: 0x006BC430/0x1403707D0
void ClientSplineMoveCollisionDisable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }
}

// PROVEN: 69b75a97 IDENTICAL/NONE
// OFFSET: 0x006C78C0
bool ClientSplineMoveCollisionDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e93d9869 IDENTICAL/NONE
// OFFSET: 0x006B6B10
bool ClientSplineMoveCollisionDisable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C1500/0x140376B20
bool ClientSplineMoveCollisionDisable::Handle() {
    if (!ClientSplineMoveCollisionDisable::s_handler) {
        return false;
    }

    return ClientSplineMoveCollisionDisable::s_handler(this);
}

// PROVEN: 2f4563d4 IDENTICAL/NONE
// OFFSET: 0x006C1360/0x140376910
ClientSplineMoveCollisionEnable::ClientSplineMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a0161cb2 IDENTICAL/NONE
// OFFSET: 0x006A2320/0x14034F850
ClientSplineMoveCollisionEnable::~ClientSplineMoveCollisionEnable() {
}

// PROVEN: 0912e50e IDENTICAL/NONE
// OFFSET: 0x006AA190/0x140359790
void ClientSplineMoveCollisionEnable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[0]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[0] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[4]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[4] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }
}

// PROVEN: c5fea94d IDENTICAL/NONE
// OFFSET: 0x006C0A40
bool ClientSplineMoveCollisionEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9c0f91c0 IDENTICAL/NONE
// OFFSET: 0x006AEC70
bool ClientSplineMoveCollisionEnable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[0]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[0] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[4]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[4] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006C1730/0x140376DE0
bool ClientSplineMoveCollisionEnable::Handle() {
    if (!ClientSplineMoveCollisionEnable::s_handler) {
        return false;
    }

    return ClientSplineMoveCollisionEnable::s_handler(this);
}

// PROVEN: 204ee9b4 IDENTICAL/NONE
// OFFSET: 0x006B5000/0x1403671E0
ClientSplineMoveGravityDisable::ClientSplineMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9557d82e IDENTICAL/NONE
// OFFSET: 0x006A2110/0x14034F560
ClientSplineMoveGravityDisable::~ClientSplineMoveGravityDisable() {
}

// PROVEN: cb471656 IDENTICAL/NONE
// OFFSET: 0x006B20E0/0x140363620
void ClientSplineMoveGravityDisable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }
}

// PROVEN: 15b5e0e8 IDENTICAL/NONE
// OFFSET: 0x006BF4A0
bool ClientSplineMoveGravityDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1aaec67b IDENTICAL/NONE
// OFFSET: 0x006B7BA0
bool ClientSplineMoveGravityDisable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BF130/0x140374100
bool ClientSplineMoveGravityDisable::Handle() {
    if (!ClientSplineMoveGravityDisable::s_handler) {
        return false;
    }

    return ClientSplineMoveGravityDisable::s_handler(this);
}

// PROVEN: 4658b421 IDENTICAL/NONE
// OFFSET: 0x006B8430/0x14036B5C0
ClientSplineMoveGravityEnable::ClientSplineMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4cbf6d11 IDENTICAL/NONE
// OFFSET: 0x006A28A0/0x14034FD20
ClientSplineMoveGravityEnable::~ClientSplineMoveGravityEnable() {
}

// PROVEN: d994e280 IDENTICAL/NONE
// OFFSET: 0x006A92E0/0x140358480
void ClientSplineMoveGravityEnable::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }
}

// PROVEN: 6fd27692 IDENTICAL/NONE
// OFFSET: 0x006CDE90
bool ClientSplineMoveGravityEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 115f75a0 IDENTICAL/NONE
// OFFSET: 0x006B90B0
bool ClientSplineMoveGravityEnable::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[5]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[5] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AFA20/0x1403604C0
bool ClientSplineMoveGravityEnable::Handle() {
    if (!ClientSplineMoveGravityEnable::s_handler) {
        return false;
    }

    return ClientSplineMoveGravityEnable::s_handler(this);
}

// PROVEN: 0aca9f61 IDENTICAL/NONE
// OFFSET: 0x006B9070/0x14036C520
ClientSplineMoveRoot::ClientSplineMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5bc67e6e IDENTICAL/NONE
// OFFSET: 0x006A0FE0/0x14034E240
ClientSplineMoveRoot::~ClientSplineMoveRoot() {
}

// PROVEN: d6e714d2 IDENTICAL/NONE
// OFFSET: 0x006A8B60/0x140357AE0
void ClientSplineMoveRoot::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[2] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[3]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[3] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }
}

// PROVEN: f3fd3b47 IDENTICAL/NONE
// OFFSET: 0x006B4100
bool ClientSplineMoveRoot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: db96f44a IDENTICAL/NONE
// OFFSET: 0x006AF250
bool ClientSplineMoveRoot::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[2] ^= value10;
    }

    if (guid[1]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[1] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[3]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[3] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BE120/0x140372D20
bool ClientSplineMoveRoot::Handle() {
    if (!ClientSplineMoveRoot::s_handler) {
        return false;
    }

    return ClientSplineMoveRoot::s_handler(this);
}

// PROVEN: 244ffb16 IDENTICAL/NONE
// OFFSET: 0x006C78D0/0x14037E700
ClientSplineMoveSetFeatherFall::ClientSplineMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c168da68 IDENTICAL/NONE
// OFFSET: 0x006A2E30/0x140350390
ClientSplineMoveSetFeatherFall::~ClientSplineMoveSetFeatherFall() {
}

// PROVEN: 794cb1d1 IDENTICAL/NONE
// OFFSET: 0x006BC8E0/0x140370DE0
void ClientSplineMoveSetFeatherFall::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }
}

// PROVEN: 166c60c5 IDENTICAL/NONE
// OFFSET: 0x006C8140
bool ClientSplineMoveSetFeatherFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 77557831 IDENTICAL/NONE
// OFFSET: 0x006C4D60
bool ClientSplineMoveSetFeatherFall::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AAD00/0x14035A590
bool ClientSplineMoveSetFeatherFall::Handle() {
    if (!ClientSplineMoveSetFeatherFall::s_handler) {
        return false;
    }

    return ClientSplineMoveSetFeatherFall::s_handler(this);
}

// PROVEN: 6a9c3b7f IDENTICAL/NONE
// OFFSET: 0x006C4F10/0x14037ABD0
ClientSplineMoveSetFlightBackSpeed::ClientSplineMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ce720705 IDENTICAL/NONE
// OFFSET: 0x006A19E0/0x14034EE20
ClientSplineMoveSetFlightBackSpeed::~ClientSplineMoveSetFlightBackSpeed() {
}

// PROVEN: ac9c85fe IDENTICAL/NONE
// OFFSET: 0x006BBDF0/0x14036FF90
void ClientSplineMoveSetFlightBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_speed = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[4]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[4] ^= value55;
    }
}

// PROVEN: 7eea3b7d IDENTICAL/NONE
// OFFSET: 0x006BE7E0
bool ClientSplineMoveSetFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 8791e403 IDENTICAL/NONE
// OFFSET: 0x006BB0C0
bool ClientSplineMoveSetFlightBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_speed = value15;
    }
    if (guid[6]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[6] ^= value19;
    }

    if (guid[1]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[1] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[2]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[2] ^= value37;
    }

    if (guid[3]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[3] ^= value43;
    }

    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[4]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[4] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006BF4B0/0x140374550
bool ClientSplineMoveSetFlightBackSpeed::Handle() {
    if (!ClientSplineMoveSetFlightBackSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetFlightBackSpeed::s_handler(this);
}

// PROVEN: 27cfb260 IDENTICAL/NONE
// OFFSET: 0x006B83E0/0x14036B570
ClientSplineMoveSetFlightSpeed::ClientSplineMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 141ae6d0 IDENTICAL/NONE
// OFFSET: 0x006A2690/0x14034FB70
ClientSplineMoveSetFlightSpeed::~ClientSplineMoveSetFlightSpeed() {
}

// PROVEN: fae55185 IDENTICAL/NONE
// OFFSET: 0x006B1340/0x140362460
void ClientSplineMoveSetFlightSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    {
        float value57 = 0;
        msg->Get(value57);
        this->m_speed = value57;
    }
}

// PROVEN: b11baa6d IDENTICAL/NONE
// OFFSET: 0x006C7940
bool ClientSplineMoveSetFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: be7f70ec IDENTICAL/NONE
// OFFSET: 0x006BB5A0
bool ClientSplineMoveSetFlightSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    {
        float value57 = 0;
        msg->Get(value57);
        this->m_speed = value57;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006AB3F0/0x14035AED0
bool ClientSplineMoveSetFlightSpeed::Handle() {
    if (!ClientSplineMoveSetFlightSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetFlightSpeed::s_handler(this);
}

// PROVEN: 62ab9ec6 IDENTICAL/NONE
// OFFSET: 0x006BC240/0x140370570
ClientSplineMoveSetFlying::ClientSplineMoveSetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f639615e IDENTICAL/NONE
// OFFSET: 0x006A1E10/0x14034F280
ClientSplineMoveSetFlying::~ClientSplineMoveSetFlying() {
}

// PROVEN: 8b3e98ee IDENTICAL/NONE
// OFFSET: 0x006AE5D0/0x14035EC40
void ClientSplineMoveSetFlying::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[5]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[5] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }
}

// PROVEN: 0beef25a IDENTICAL/NONE
// OFFSET: 0x006BC850
bool ClientSplineMoveSetFlying::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f34d8c94 IDENTICAL/NONE
// OFFSET: 0x006BB990
bool ClientSplineMoveSetFlying::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[5]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[5] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[2]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[2] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BF380/0x1403743E0
bool ClientSplineMoveSetFlying::Handle() {
    if (!ClientSplineMoveSetFlying::s_handler) {
        return false;
    }

    return ClientSplineMoveSetFlying::s_handler(this);
}

// PROVEN: 3227cbbe IDENTICAL/NONE
// OFFSET: 0x006C17F0/0x140376E40
ClientSplineMoveSetHover::ClientSplineMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: be553cba IDENTICAL/NONE
// OFFSET: 0x006A25D0/0x14034FB20
ClientSplineMoveSetHover::~ClientSplineMoveSetHover() {
}

// PROVEN: cea3fa0f IDENTICAL/NONE
// OFFSET: 0x006AB4D0/0x14035AFF0
void ClientSplineMoveSetHover::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[2] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }
}

// PROVEN: cf565dbf IDENTICAL/NONE
// OFFSET: 0x006BBAF0
bool ClientSplineMoveSetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 869e18aa IDENTICAL/NONE
// OFFSET: 0x006A3350
bool ClientSplineMoveSetHover::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    if (guid[2]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[2] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AAF20/0x14035A870
bool ClientSplineMoveSetHover::Handle() {
    if (!ClientSplineMoveSetHover::s_handler) {
        return false;
    }

    return ClientSplineMoveSetHover::s_handler(this);
}

// PROVEN: 2f613032 IDENTICAL/NONE
// OFFSET: 0x006C87A0/0x14037F870
ClientSplineMoveSetLandWalk::ClientSplineMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8cc2c1e3 IDENTICAL/NONE
// OFFSET: 0x006A2D80/0x1403502E0
ClientSplineMoveSetLandWalk::~ClientSplineMoveSetLandWalk() {
}

// PROVEN: a54d70e8 IDENTICAL/NONE
// OFFSET: 0x006BE350/0x140373010
void ClientSplineMoveSetLandWalk::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }
}

// PROVEN: fdc249e6 IDENTICAL/NONE
// OFFSET: 0x006C4560
bool ClientSplineMoveSetLandWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d7f46ac1 IDENTICAL/NONE
// OFFSET: 0x006B9E40
bool ClientSplineMoveSetLandWalk::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[1]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[1] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[0]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[0] ^= value46;
    }

    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BF6F0/0x140374820
bool ClientSplineMoveSetLandWalk::Handle() {
    if (!ClientSplineMoveSetLandWalk::s_handler) {
        return false;
    }

    return ClientSplineMoveSetLandWalk::s_handler(this);
}

// PROVEN: 1a1d9a28 IDENTICAL/NONE
// OFFSET: 0x006C8320/0x14037F320
ClientSplineMoveSetNormalFall::ClientSplineMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c3ab0e0f IDENTICAL/NONE
// OFFSET: 0x006A2D60/0x1403502C0
ClientSplineMoveSetNormalFall::~ClientSplineMoveSetNormalFall() {
}

// PROVEN: 3ec36359 IDENTICAL/NONE
// OFFSET: 0x006BEA10/0x140373810
void ClientSplineMoveSetNormalFall::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }
}

// PROVEN: dc8bc00c IDENTICAL/NONE
// OFFSET: 0x006CCA90
bool ClientSplineMoveSetNormalFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9c33d84e IDENTICAL/NONE
// OFFSET: 0x006B5280
bool ClientSplineMoveSetNormalFall::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[3]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[3] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006AEED0/0x14035F7F0
bool ClientSplineMoveSetNormalFall::Handle() {
    if (!ClientSplineMoveSetNormalFall::s_handler) {
        return false;
    }

    return ClientSplineMoveSetNormalFall::s_handler(this);
}

// PROVEN: a1f35500 IDENTICAL/NONE
// OFFSET: 0x006B0860/0x1403616B0
ClientSplineMoveSetPitchRate::ClientSplineMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ea60a0f8 IDENTICAL/NONE
// OFFSET: 0x006A1280/0x14034E550
ClientSplineMoveSetPitchRate::~ClientSplineMoveSetPitchRate() {
}

// PROVEN: c9acd03f IDENTICAL/NONE
// OFFSET: 0x006AA7C0/0x140359EF0
void ClientSplineMoveSetPitchRate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    {
        float value51 = 0;
        msg->Get(value51);
        this->m_rate = value51;
    }
    if (guid[4]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[4] ^= value55;
    }
}

// PROVEN: d6abb68b IDENTICAL/NONE
// OFFSET: 0x006CDE40
bool ClientSplineMoveSetPitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a31b4b36 IDENTICAL/NONE
// OFFSET: 0x006BB260
bool ClientSplineMoveSetPitchRate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    {
        float value51 = 0;
        msg->Get(value51);
        this->m_rate = value51;
    }
    if (guid[4]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[4] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_rate);
}

// OFFSET: 0x006AD0C0/0x14035D150
bool ClientSplineMoveSetPitchRate::Handle() {
    if (!ClientSplineMoveSetPitchRate::s_handler) {
        return false;
    }

    return ClientSplineMoveSetPitchRate::s_handler(this);
}

// PROVEN: b6bf78ca IDENTICAL/NONE
// OFFSET: 0x006D3070/0x14038EFA0
ClientSplineMoveSetRunBackSpeed::ClientSplineMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a816d7f6 IDENTICAL/NONE
// OFFSET: 0x006A17B0/0x14034EB30
ClientSplineMoveSetRunBackSpeed::~ClientSplineMoveSetRunBackSpeed() {
}

// PROVEN: 951fe937 IDENTICAL/NONE
// OFFSET: 0x006C11C0/0x1403766E0
void ClientSplineMoveSetRunBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_speed = value15;
    }
    if (guid[2]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[2] ^= value19;
    }

    if (guid[4]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[4] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[6]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[6] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }
}

// PROVEN: 7023e344 IDENTICAL/NONE
// OFFSET: 0x006B39A0
bool ClientSplineMoveSetRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 28137f22 IDENTICAL/NONE
// OFFSET: 0x006B0900
bool ClientSplineMoveSetRunBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[1]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[1] ^= value10;
    }

    {
        float value15 = 0;
        msg->Get(value15);
        this->m_speed = value15;
    }
    if (guid[2]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[2] ^= value19;
    }

    if (guid[4]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[4] ^= value25;
    }

    if (guid[0]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[0] ^= value31;
    }

    if (guid[3]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[3] ^= value37;
    }

    if (guid[6]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[6] ^= value43;
    }

    if (guid[5]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[5] ^= value49;
    }

    if (guid[7]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[7] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B7B60/0x14036AA50
bool ClientSplineMoveSetRunBackSpeed::Handle() {
    if (!ClientSplineMoveSetRunBackSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetRunBackSpeed::s_handler(this);
}

// PROVEN: 32f0890d IDENTICAL/NONE
// OFFSET: 0x006BC860/0x140370D40
ClientSplineMoveSetRunMode::ClientSplineMoveSetRunMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9c2fec97 IDENTICAL/NONE
// OFFSET: 0x006A17A0/0x14034EB20
ClientSplineMoveSetRunMode::~ClientSplineMoveSetRunMode() {
}

// PROVEN: 0d8b454b IDENTICAL/NONE
// OFFSET: 0x006B4CD0/0x140366DF0
void ClientSplineMoveSetRunMode::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }
}

// PROVEN: e09d74a2 IDENTICAL/NONE
// OFFSET: 0x006B4350
bool ClientSplineMoveSetRunMode::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4a0e7628 IDENTICAL/NONE
// OFFSET: 0x006AEB10
bool ClientSplineMoveSetRunMode::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[4]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[4] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[2]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[2] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B5230/0x1403674B0
bool ClientSplineMoveSetRunMode::Handle() {
    if (!ClientSplineMoveSetRunMode::s_handler) {
        return false;
    }

    return ClientSplineMoveSetRunMode::s_handler(this);
}

// PROVEN: 77c394c1 IDENTICAL/NONE
// OFFSET: 0x006D4670/0x1403910D0
ClientSplineMoveSetRunSpeed::ClientSplineMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 677f5624 IDENTICAL/NONE
// OFFSET: 0x006A0EF0/0x14034DFD0
ClientSplineMoveSetRunSpeed::~ClientSplineMoveSetRunSpeed() {
}

// PROVEN: 7b1aa95e IDENTICAL/NONE
// OFFSET: 0x006C13A0/0x140376950
void ClientSplineMoveSetRunSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[2]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[2] ^= value49;
    }

    if (guid[1]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[1] ^= value55;
    }
}

// PROVEN: 5b9cc9a0 IDENTICAL/NONE
// OFFSET: 0x006D4FA0
bool ClientSplineMoveSetRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 40d5de4e IDENTICAL/NONE
// OFFSET: 0x006CA8D0
bool ClientSplineMoveSetRunSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[6]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[6] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[2]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[2] ^= value49;
    }

    if (guid[1]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[1] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B8F00/0x14036C350
bool ClientSplineMoveSetRunSpeed::Handle() {
    if (!ClientSplineMoveSetRunSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetRunSpeed::s_handler(this);
}

// PROVEN: 52a7b75b IDENTICAL/NONE
// OFFSET: 0x006B0AA0/0x140361940
ClientSplineMoveSetSwimBackSpeed::ClientSplineMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: bcd6c464 IDENTICAL/NONE
// OFFSET: 0x006A2400/0x14034F910
ClientSplineMoveSetSwimBackSpeed::~ClientSplineMoveSetSwimBackSpeed() {
}

// PROVEN: 6c822d6e IDENTICAL/NONE
// OFFSET: 0x006AADC0/0x14035A6A0
void ClientSplineMoveSetSwimBackSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }
}

// PROVEN: a196ce27 IDENTICAL/NONE
// OFFSET: 0x006D45C0
bool ClientSplineMoveSetSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1ba1f28c IDENTICAL/NONE
// OFFSET: 0x006C1590
bool ClientSplineMoveSetSwimBackSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[7]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[7] ^= value34;
    }

    if (guid[6]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[6] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[4]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[4] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006BC7D0/0x140370C70
bool ClientSplineMoveSetSwimBackSpeed::Handle() {
    if (!ClientSplineMoveSetSwimBackSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetSwimBackSpeed::s_handler(this);
}

// PROVEN: 3549e454 IDENTICAL/NONE
// OFFSET: 0x006C8720/0x14037F7F0
ClientSplineMoveSetSwimSpeed::ClientSplineMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: eae6c780 IDENTICAL/NONE
// OFFSET: 0x006A1130/0x14034E3D0
ClientSplineMoveSetSwimSpeed::~ClientSplineMoveSetSwimSpeed() {
}

// PROVEN: 358e7f60 IDENTICAL/NONE
// OFFSET: 0x006B60A0/0x140368730
void ClientSplineMoveSetSwimSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }
}

// PROVEN: fdfb8d2d IDENTICAL/NONE
// OFFSET: 0x006CA480
bool ClientSplineMoveSetSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d608e29c IDENTICAL/NONE
// OFFSET: 0x006B87B0
bool ClientSplineMoveSetSwimSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[7]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[7] ^= value49;
    }

    if (guid[3]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[3] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006BF8D0/0x140374A90
bool ClientSplineMoveSetSwimSpeed::Handle() {
    if (!ClientSplineMoveSetSwimSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetSwimSpeed::s_handler(this);
}

// PROVEN: 4d15eaf7 IDENTICAL/NONE
// OFFSET: 0x006C8660/0x14037F7A0
ClientSplineMoveSetTurnRate::ClientSplineMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 68169eae IDENTICAL/NONE
// OFFSET: 0x006A27B0/0x14034FCB0
ClientSplineMoveSetTurnRate::~ClientSplineMoveSetTurnRate() {
}

// PROVEN: 5879419e IDENTICAL/NONE
// OFFSET: 0x006B2A70/0x1403641D0
void ClientSplineMoveSetTurnRate::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_rate = value9;
    }
    if (guid[1]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[1] ^= value13;
    }

    if (guid[5]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[5] ^= value19;
    }

    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[7]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[7] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[6]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[6] ^= value49;
    }

    if (guid[0]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[0] ^= value55;
    }
}

// PROVEN: cc5f3ea6 IDENTICAL/NONE
// OFFSET: 0x006D0D20
bool ClientSplineMoveSetTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a66529ef IDENTICAL/NONE
// OFFSET: 0x006B9CA0
bool ClientSplineMoveSetTurnRate::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    {
        float value9 = 0;
        msg->Get(value9);
        this->m_rate = value9;
    }
    if (guid[1]) {
        uint8_t value13 = 0;
        msg->Get(value13);
        guid[1] ^= value13;
    }

    if (guid[5]) {
        uint8_t value19 = 0;
        msg->Get(value19);
        guid[5] ^= value19;
    }

    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[2]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[2] ^= value31;
    }

    if (guid[7]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[7] ^= value37;
    }

    if (guid[4]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[4] ^= value43;
    }

    if (guid[6]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[6] ^= value49;
    }

    if (guid[0]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[0] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_rate);
}

// OFFSET: 0x006B35A0/0x140365070
bool ClientSplineMoveSetTurnRate::Handle() {
    if (!ClientSplineMoveSetTurnRate::s_handler) {
        return false;
    }

    return ClientSplineMoveSetTurnRate::s_handler(this);
}

// PROVEN: ebe7f92c IDENTICAL/NONE
// OFFSET: 0x006CB4C0/0x140383AC0
ClientSplineMoveSetWalkMode::ClientSplineMoveSetWalkMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 48fff467 IDENTICAL/NONE
// OFFSET: 0x006A1FF0/0x14034F460
ClientSplineMoveSetWalkMode::~ClientSplineMoveSetWalkMode() {
}

// PROVEN: 33b89b23 IDENTICAL/NONE
// OFFSET: 0x006BA310/0x14036DCA0
void ClientSplineMoveSetWalkMode::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }
}

// PROVEN: 51df3a33 IDENTICAL/NONE
// OFFSET: 0x006AE820
bool ClientSplineMoveSetWalkMode::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ac54b698 IDENTICAL/NONE
// OFFSET: 0x006AA340
bool ClientSplineMoveSetWalkMode::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[6]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[6] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[3]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[3] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BE880/0x140373630
bool ClientSplineMoveSetWalkMode::Handle() {
    if (!ClientSplineMoveSetWalkMode::s_handler) {
        return false;
    }

    return ClientSplineMoveSetWalkMode::s_handler(this);
}

// PROVEN: 13d04964 IDENTICAL/NONE
// OFFSET: 0x006D0CE0/0x14038B8B0
ClientSplineMoveSetWalkSpeed::ClientSplineMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1a926bb3 IDENTICAL/NONE
// OFFSET: 0x006A1DF0/0x14034F260
ClientSplineMoveSetWalkSpeed::~ClientSplineMoveSetWalkSpeed() {
}

// PROVEN: f271b2a0 IDENTICAL/NONE
// OFFSET: 0x006BC660/0x140370A90
void ClientSplineMoveSetWalkSpeed::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[6]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[6] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }
}

// PROVEN: 69ada395 IDENTICAL/NONE
// OFFSET: 0x006C4F50
bool ClientSplineMoveSetWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d2fe45a9 IDENTICAL/NONE
// OFFSET: 0x006ADFC0
bool ClientSplineMoveSetWalkSpeed::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[4]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[4] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[1]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[1] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[3]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[3] ^= value40;
    }

    {
        float value45 = 0;
        msg->Get(value45);
        this->m_speed = value45;
    }
    if (guid[6]) {
        uint8_t value49 = 0;
        msg->Get(value49);
        guid[6] ^= value49;
    }

    if (guid[2]) {
        uint8_t value55 = 0;
        msg->Get(value55);
        guid[2] ^= value55;
    }

    return msg->IsRead() && _finite(this->m_speed);
}

// OFFSET: 0x006B31B0/0x140364BB0
bool ClientSplineMoveSetWalkSpeed::Handle() {
    if (!ClientSplineMoveSetWalkSpeed::s_handler) {
        return false;
    }

    return ClientSplineMoveSetWalkSpeed::s_handler(this);
}

// PROVEN: 2bc2fc1e IDENTICAL/NONE
// OFFSET: 0x006C4860/0x14037A7A0
ClientSplineMoveSetWaterWalk::ClientSplineMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 17714037 IDENTICAL/NONE
// OFFSET: 0x006A26B0/0x14034FB90
ClientSplineMoveSetWaterWalk::~ClientSplineMoveSetWaterWalk() {
}

// PROVEN: bc1968fb IDENTICAL/NONE
// OFFSET: 0x006B99D0/0x14036D100
void ClientSplineMoveSetWaterWalk::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }
}

// PROVEN: 1fb8e29e IDENTICAL/NONE
// OFFSET: 0x006B20D0
bool ClientSplineMoveSetWaterWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1556365e IDENTICAL/NONE
// OFFSET: 0x006AE840
bool ClientSplineMoveSetWaterWalk::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    if (guid[0]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[0] ^= value10;
    }

    if (guid[6]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[6] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[7]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[7] ^= value28;
    }

    if (guid[4]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[4] ^= value34;
    }

    if (guid[2]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[2] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006A6D20/0x140355260
bool ClientSplineMoveSetWaterWalk::Handle() {
    if (!ClientSplineMoveSetWaterWalk::s_handler) {
        return false;
    }

    return ClientSplineMoveSetWaterWalk::s_handler(this);
}

// PROVEN: b138a1cd IDENTICAL/NONE
// OFFSET: 0x006C4760/0x14037A670
ClientSplineMoveStartSwim::ClientSplineMoveStartSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3e6e6ef6 IDENTICAL/NONE
// OFFSET: 0x006A19C0/0x14034EDF0
ClientSplineMoveStartSwim::~ClientSplineMoveStartSwim() {
}

// PROVEN: 489e74f5 IDENTICAL/NONE
// OFFSET: 0x006A6250/0x140354450
void ClientSplineMoveStartSwim::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }
}

// PROVEN: e7468ea3 IDENTICAL/NONE
// OFFSET: 0x006BEB60
bool ClientSplineMoveStartSwim::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 6ac04444 IDENTICAL/NONE
// OFFSET: 0x006B39B0
bool ClientSplineMoveStartSwim::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[3] ^= value10;
    }

    if (guid[7]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[7] ^= value16;
    }

    if (guid[2]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[2] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[6]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[6] ^= value34;
    }

    if (guid[4]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[4] ^= value40;
    }

    if (guid[1]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[1] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BB560/0x14036F480
bool ClientSplineMoveStartSwim::Handle() {
    if (!ClientSplineMoveStartSwim::s_handler) {
        return false;
    }

    return ClientSplineMoveStartSwim::s_handler(this);
}

// PROVEN: 514eb121 IDENTICAL/NONE
// OFFSET: 0x006C48C0/0x14037A800
ClientSplineMoveStopSwim::ClientSplineMoveStopSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 6780ea66 IDENTICAL/NONE
// OFFSET: 0x006A0E20/0x14034DEC0
ClientSplineMoveStopSwim::~ClientSplineMoveStopSwim() {
}

// PROVEN: cd1ac6e9 IDENTICAL/NONE
// OFFSET: 0x006B7E80/0x14036AE40
void ClientSplineMoveStopSwim::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }
}

// PROVEN: e6d4bd03 IDENTICAL/NONE
// OFFSET: 0x006D59B0
bool ClientSplineMoveStopSwim::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 838ca1a4 IDENTICAL/NONE
// OFFSET: 0x006C4F60
bool ClientSplineMoveStopSwim::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    if (guid[7]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[7] ^= value22;
    }

    if (guid[2]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[2] ^= value28;
    }

    if (guid[3]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[3] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[5]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[5] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B1AA0/0x140362E10
bool ClientSplineMoveStopSwim::Handle() {
    if (!ClientSplineMoveStopSwim::s_handler) {
        return false;
    }

    return ClientSplineMoveStopSwim::s_handler(this);
}

// PROVEN: 14b17a7b IDENTICAL/NONE
// OFFSET: 0x006BDF60/0x140372AD0
ClientSplineMoveUnroot::ClientSplineMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ffb9a264 IDENTICAL/NONE
// OFFSET: 0x006A22C0/0x14034F790
ClientSplineMoveUnroot::~ClientSplineMoveUnroot() {
}

// PROVEN: 710d5c37 IDENTICAL/NONE
// OFFSET: 0x006B3E50/0x140365BB0
void ClientSplineMoveUnroot::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }
}

// PROVEN: 25438ab4 IDENTICAL/NONE
// OFFSET: 0x006D46C0
bool ClientSplineMoveUnroot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 28d1cf04 IDENTICAL/NONE
// OFFSET: 0x006C1830
bool ClientSplineMoveUnroot::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    if (guid[6]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[6] ^= value10;
    }

    if (guid[3]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[3] ^= value16;
    }

    if (guid[1]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[1] ^= value22;
    }

    if (guid[5]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[5] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[0]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[0] ^= value40;
    }

    if (guid[7]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[7] ^= value46;
    }

    if (guid[4]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[4] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BBD60/0x14036FED0
bool ClientSplineMoveUnroot::Handle() {
    if (!ClientSplineMoveUnroot::s_handler) {
        return false;
    }

    return ClientSplineMoveUnroot::s_handler(this);
}

// PROVEN: f157fc14 IDENTICAL/NONE
// OFFSET: 0x006B2830/0x140363EE0
ClientSplineMoveUnsetFlying::ClientSplineMoveUnsetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8895cb33 IDENTICAL/NONE
// OFFSET: 0x006A0F60/0x14034E1C0
ClientSplineMoveUnsetFlying::~ClientSplineMoveUnsetFlying() {
}

// PROVEN: 42121fa3 IDENTICAL/NONE
// OFFSET: 0x006A90D0/0x1403581B0
void ClientSplineMoveUnsetFlying::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }
}

// PROVEN: 6e1dc233 IDENTICAL/NONE
// OFFSET: 0x006CDEA0
bool ClientSplineMoveUnsetFlying::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 42a27b68 IDENTICAL/NONE
// OFFSET: 0x006B6560
bool ClientSplineMoveUnsetFlying::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[7]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[7] ^= value10;
    }

    if (guid[2]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[2] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[4]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[4] ^= value28;
    }

    if (guid[5]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[5] ^= value34;
    }

    if (guid[1]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[1] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[0]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[0] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006BAF00/0x14036EC60
bool ClientSplineMoveUnsetFlying::Handle() {
    if (!ClientSplineMoveUnsetFlying::s_handler) {
        return false;
    }

    return ClientSplineMoveUnsetFlying::s_handler(this);
}

// PROVEN: f78c2fdb IDENTICAL/NONE
// OFFSET: 0x006C5FF0/0x14037C410
ClientSplineMoveUnsetHover::ClientSplineMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 412abe25 IDENTICAL/NONE
// OFFSET: 0x006A26A0/0x14034FB80
ClientSplineMoveUnsetHover::~ClientSplineMoveUnsetHover() {
}

// PROVEN: 91ad102c IDENTICAL/NONE
// OFFSET: 0x006B2C10/0x140364400
void ClientSplineMoveUnsetHover::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[7]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[7] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }
}

// PROVEN: a2603f0f IDENTICAL/NONE
// OFFSET: 0x006D7200
bool ClientSplineMoveUnsetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3f30408c IDENTICAL/NONE
// OFFSET: 0x006CABC0
bool ClientSplineMoveUnsetHover::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[6] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    if (guid[4]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[4] ^= value10;
    }

    if (guid[5]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[5] ^= value16;
    }

    if (guid[3]) {
        uint8_t value22 = 0;
        msg->Get(value22);
        guid[3] ^= value22;
    }

    if (guid[0]) {
        uint8_t value28 = 0;
        msg->Get(value28);
        guid[0] ^= value28;
    }

    if (guid[2]) {
        uint8_t value34 = 0;
        msg->Get(value34);
        guid[2] ^= value34;
    }

    if (guid[7]) {
        uint8_t value40 = 0;
        msg->Get(value40);
        guid[7] ^= value40;
    }

    if (guid[6]) {
        uint8_t value46 = 0;
        msg->Get(value46);
        guid[6] ^= value46;
    }

    if (guid[1]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[1] ^= value52;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006ABC30/0x14035B940
bool ClientSplineMoveUnsetHover::Handle() {
    if (!ClientSplineMoveUnsetHover::s_handler) {
        return false;
    }

    return ClientSplineMoveUnsetHover::s_handler(this);
}

// PROVEN: 5ab5a016 IDENTICAL/NONE
// OFFSET: 0x006BADE0/0x14036EA80
ClientUnit30A6::ClientUnit30A6(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 90aa562d IDENTICAL/NONE
// OFFSET: 0x006A2D70/0x1403502D0
ClientUnit30A6::~ClientUnit30A6() {
}

// PROVEN: 41300871 IDENTICAL/NONE
// OFFSET: 0x006AAA40/0x14035A210
void ClientUnit30A6::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_value = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[3] ^= value11;
    }

    if (guid[2]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[2] ^= value17;
    }

    if (guid[1]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[1] ^= value23;
    }

    if (guid[7]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[7] ^= value29;
    }

    if (guid[0]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[0] ^= value35;
    }

    if (guid[5]) {
        uint8_t value41 = 0;
        msg->Get(value41);
        guid[5] ^= value41;
    }

    if (guid[4]) {
        uint8_t value47 = 0;
        msg->Get(value47);
        guid[4] ^= value47;
    }

    if (guid[6]) {
        uint8_t value53 = 0;
        msg->Get(value53);
        guid[6] ^= value53;
    }
}

// PROVEN: 76dd07e0 IDENTICAL/NONE
// OFFSET: 0x006ACE60
bool ClientUnit30A6::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: dcb2e615 IDENTICAL/NONE
// OFFSET: 0x006A86A0
bool ClientUnit30A6::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_value = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[3]) {
        uint8_t value11 = 0;
        msg->Get(value11);
        guid[3] ^= value11;
    }

    if (guid[2]) {
        uint8_t value17 = 0;
        msg->Get(value17);
        guid[2] ^= value17;
    }

    if (guid[1]) {
        uint8_t value23 = 0;
        msg->Get(value23);
        guid[1] ^= value23;
    }

    if (guid[7]) {
        uint8_t value29 = 0;
        msg->Get(value29);
        guid[7] ^= value29;
    }

    if (guid[0]) {
        uint8_t value35 = 0;
        msg->Get(value35);
        guid[0] ^= value35;
    }

    if (guid[5]) {
        uint8_t value41 = 0;
        msg->Get(value41);
        guid[5] ^= value41;
    }

    if (guid[4]) {
        uint8_t value47 = 0;
        msg->Get(value47);
        guid[4] ^= value47;
    }

    if (guid[6]) {
        uint8_t value53 = 0;
        msg->Get(value53);
        guid[6] ^= value53;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B45A0/0x140366520
bool ClientUnit30A6::Handle() {
    if (!ClientUnit30A6::s_handler) {
        return false;
    }

    return ClientUnit30A6::s_handler(this);
}

// PROVEN: 216d8e8e IDENTICAL/NONE
// OFFSET: 0x006C99C0/0x140381410
ClientUnitClearSlotValue::ClientUnitClearSlotValue(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 39e9d352 IDENTICAL/NONE
// OFFSET: 0x006A18E0/0x14034EC90
ClientUnitClearSlotValue::~ClientUnitClearSlotValue() {
}

// PROVEN: 6370717b IDENTICAL/NONE
// OFFSET: 0x006B81F0/0x14036B2D0
void ClientUnitClearSlotValue::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    {
        uint8_t value21 = 0;
        msg->Get(value21);
        this->m_index = value21;
    }
    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[7]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[7] ^= value31;
    }

    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[2]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[2] ^= value43;
    }

    {
        uint8_t value48 = 0;
        msg->Get(value48);
        this->m_slot = value48;
    }
    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    if (guid[1]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[1] ^= value58;
    }
}

// PROVEN: 82913a0b IDENTICAL/NONE
// OFFSET: 0x006A8CF0
bool ClientUnitClearSlotValue::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5289626a IDENTICAL/NONE
// OFFSET: 0x006A57F0
bool ClientUnitClearSlotValue::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[2] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    if (guid[5]) {
        uint8_t value10 = 0;
        msg->Get(value10);
        guid[5] ^= value10;
    }

    if (guid[0]) {
        uint8_t value16 = 0;
        msg->Get(value16);
        guid[0] ^= value16;
    }

    {
        uint8_t value21 = 0;
        msg->Get(value21);
        this->m_index = value21;
    }
    if (guid[3]) {
        uint8_t value25 = 0;
        msg->Get(value25);
        guid[3] ^= value25;
    }

    if (guid[7]) {
        uint8_t value31 = 0;
        msg->Get(value31);
        guid[7] ^= value31;
    }

    if (guid[4]) {
        uint8_t value37 = 0;
        msg->Get(value37);
        guid[4] ^= value37;
    }

    if (guid[2]) {
        uint8_t value43 = 0;
        msg->Get(value43);
        guid[2] ^= value43;
    }

    {
        uint8_t value48 = 0;
        msg->Get(value48);
        this->m_slot = value48;
    }
    if (guid[6]) {
        uint8_t value52 = 0;
        msg->Get(value52);
        guid[6] ^= value52;
    }

    if (guid[1]) {
        uint8_t value58 = 0;
        msg->Get(value58);
        guid[1] ^= value58;
    }

    return msg->IsRead() != 0;
}

// OFFSET: 0x006B0820/0x140361650
bool ClientUnitClearSlotValue::Handle() {
    if (!ClientUnitClearSlotValue::s_handler) {
        return false;
    }

    return ClientUnitClearSlotValue::s_handler(this);
}

// OFFSET: 0x006A2C10
bool JamMoveKnockBack::IsValid() const {
    if (std::fabs(this->m_horizontalSpeed) < 2.3841858e-7f && std::fabs(this->m_verticalSpeed) < 2.3841858e-7f) {
        return false;
    }

    if (!_finite(this->m_verticalSpeed) || !_finite(this->m_horizontalSpeed)) {
        return false;
    }

    if (std::fabs(this->m_direction.y * this->m_direction.y + this->m_direction.x * this->m_direction.x - 1.0f) >= 1.0e-6f && std::fabs(this->m_direction.y * this->m_direction.y + this->m_direction.x * this->m_direction.x) >= 1.0e-6f) {
        return false;
    }

    return _finite(this->m_direction.y) && _finite(this->m_direction.x);
}

// STATIC: 0x00E4D834
bool (*ClientMoveSetCompoundState::s_handler)(ClientMoveSetCompoundState* msg);

// PROVEN: 087b8667 IDENTICAL/NONE
// OFFSET: 0x006F07E0/0x1403B61B0
ClientMoveSetCompoundState::ClientMoveSetCompoundState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3da56775 IDENTICAL/NONE
// OFFSET: 0x006E1F90/0x1403B1980
ClientMoveSetCompoundState::~ClientMoveSetCompoundState() {
}

// OFFSET: 0x006E93C0/0x1403AC2C0
void ClientMoveSetCompoundState::Unpack(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    JamBitReader bits(msg);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_changes.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];
        change.m_hasVehicleRecId = bits.ReadBit();
        change.m_hasSpeed = bits.ReadBit();
        change.m_hasKnockBack = bits.ReadBit();
        change.m_hasCollisionHeight = bits.ReadBit();

        if (change.m_hasCollisionHeight) {
            change.m_collisionHeight.m_reason = bits.ReadBits<2>();
        }
    }

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];

        if (change.m_hasCollisionHeight) {
            float value = 0;
            msg->Get(value);
            change.m_collisionHeight.m_height = value;
        }

        if (change.m_hasKnockBack) {
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_horizontalSpeed = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_direction.x = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_direction.y = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_verticalSpeed = value;
            }
        }

        if (change.m_hasVehicleRecId) {
            uint32_t value = 0;
            msg->Get(value);
            change.m_vehicleRecId = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            change.m_ackIndex = value;
        }

        if (change.m_hasSpeed) {
            float value = 0;
            msg->Get(value);
            change.m_speed = value;
        }

        {
            uint16_t value = 0;
            msg->Get(value);
            change.m_messageId = value;
        }
    }

    if (guid[2]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[2] ^= value;
    }

    if (guid[1]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[1] ^= value;
    }

    if (guid[4]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[4] ^= value;
    }

    if (guid[5]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[5] ^= value;
    }

    if (guid[6]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[6] ^= value;
    }

    if (guid[7]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[7] ^= value;
    }

    if (guid[0]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[0] ^= value;
    }

    if (guid[3]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[3] ^= value;
    }
}

// PROVEN: 0153e8a9 IDENTICAL/NONE
// OFFSET: 0x006C4EC0
bool ClientMoveSetCompoundState::IsValid() {
    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];

        if (change.m_hasCollisionHeight && change.m_collisionHeight.m_reason > 2) {
            return false;
        }

        if (change.m_hasKnockBack && !change.m_knockBack.IsValid()) {
            return false;
        }
    }

    return true;
}

// PROVEN: b989a3ad IDENTICAL/NONE
// OFFSET: 0x006ECD60
bool ClientMoveSetCompoundState::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E8760
bool ClientMoveSetCompoundState::Read(CDataStore* msg, bool full) {
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    JamBitReader bits(msg);
    guid[5] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[4] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    guid[7] = bits.ReadBit();
    guid[6] = bits.ReadBit();
    guid[2] = bits.ReadBit();
    guid[3] = bits.ReadBit();
    this->m_changes.SetCount(bits.ReadBits<22>());

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];
        change.m_hasVehicleRecId = bits.ReadBit();
        change.m_hasSpeed = bits.ReadBit();
        change.m_hasKnockBack = bits.ReadBit();
        change.m_hasCollisionHeight = bits.ReadBit();

        if (change.m_hasCollisionHeight) {
            change.m_collisionHeight.m_reason = bits.ReadBits<2>();
        }
    }

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];

        if (change.m_hasCollisionHeight) {
            float value = 0;
            msg->Get(value);
            change.m_collisionHeight.m_height = value;
        }

        if (change.m_hasKnockBack) {
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_horizontalSpeed = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_direction.x = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_direction.y = value;
            }
            {
                float value = 0;
                msg->Get(value);
                change.m_knockBack.m_verticalSpeed = value;
            }
        }

        if (change.m_hasVehicleRecId) {
            uint32_t value = 0;
            msg->Get(value);
            change.m_vehicleRecId = value;
        }

        {
            uint32_t value = 0;
            msg->Get(value);
            change.m_ackIndex = value;
        }

        if (change.m_hasSpeed) {
            float value = 0;
            msg->Get(value);
            change.m_speed = value;
        }

        {
            uint16_t value = 0;
            msg->Get(value);
            change.m_messageId = value;
        }
    }

    if (guid[2]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[2] ^= value;
    }

    if (guid[1]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[1] ^= value;
    }

    if (guid[4]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[4] ^= value;
    }

    if (guid[5]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[5] ^= value;
    }

    if (guid[6]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[6] ^= value;
    }

    if (guid[7]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[7] ^= value;
    }

    if (guid[0]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[0] ^= value;
    }

    if (guid[3]) {
        uint8_t value = 0;
        msg->Get(value);
        guid[3] ^= value;
    }

    return msg->IsRead() && this->IsValid();
}

// OFFSET: 0x006AA140/0x140359730
bool ClientMoveSetCompoundState::Handle() {
    if (!ClientMoveSetCompoundState::s_handler) {
        return false;
    }

    return ClientMoveSetCompoundState::s_handler(this);
}
