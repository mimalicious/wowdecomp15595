#include "WoW/Source/JamClientAutoCode/JamClientMove.h"
#include <cmath>
#include <float.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: 0x00E4D7F4/0x140D369C0
bool (*ClientAuraPointsDepleted::s_handler)(ClientAuraPointsDepleted* msg);

// STATIC: 0x00E4D540/0x140D36458
bool (*ClientMoveCollisionDisable::s_handler)(ClientMoveCollisionDisable* msg);

// STATIC: 0x00E4D808/0x140D369E8
bool (*ClientMoveCollisionEnable::s_handler)(ClientMoveCollisionEnable* msg);

// STATIC: 0x00E4D498/0x140D36308
bool (*ClientMoveGravityDisable::s_handler)(ClientMoveGravityDisable* msg);

// STATIC: 0x00E4D79C/0x140D36910
bool (*ClientMoveGravityEnable::s_handler)(ClientMoveGravityEnable* msg);

// STATIC: 0x00E4D608/0x140D365E8
bool (*ClientMoveKnockBack::s_handler)(ClientMoveKnockBack* msg);

// STATIC: 0x00E4D70C/0x140D367F0
bool (*ClientMoveRoot::s_handler)(ClientMoveRoot* msg);

// STATIC: 0x00E4D524/0x140D36420
bool (*ClientMoveSetCanFly::s_handler)(ClientMoveSetCanFly* msg);

// STATIC: 0x00E4D67C/0x140D366D0
bool (*ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler)(ClientMoveSetCanTransitionBetweenSwimAndFly* msg);

// STATIC: 0x00E4D750/0x140D36878
bool (*ClientMoveSetCollisionHeight::s_handler)(ClientMoveSetCollisionHeight* msg);

// STATIC: 0x00E4D634/0x140D36640
bool (*ClientMoveSetFeatherFall::s_handler)(ClientMoveSetFeatherFall* msg);

// STATIC: 0x00E4D64C/0x140D36670
bool (*ClientMoveSetFlightBackSpeed::s_handler)(ClientMoveSetFlightBackSpeed* msg);

// STATIC: 0x00E4D4CC/0x140D36370
bool (*ClientMoveSetFlightSpeed::s_handler)(ClientMoveSetFlightSpeed* msg);

// STATIC: 0x00E4D5EC/0x140D365B0
bool (*ClientMoveSetHover::s_handler)(ClientMoveSetHover* msg);

// STATIC: 0x00E4D6D4/0x140D36780
bool (*ClientMoveSetLandWalk::s_handler)(ClientMoveSetLandWalk* msg);

// STATIC: 0x00E4D434/0x140D36240
bool (*ClientMoveSetNormalFall::s_handler)(ClientMoveSetNormalFall* msg);

// STATIC: 0x00E4D7A8/0x140D36928
bool (*ClientMoveSetPitchRate::s_handler)(ClientMoveSetPitchRate* msg);

// STATIC: 0x00E4D484/0x140D362E0
bool (*ClientMoveSetRunBackSpeed::s_handler)(ClientMoveSetRunBackSpeed* msg);

// STATIC: 0x00E4D668/0x140D366A8
bool (*ClientMoveSetRunSpeed::s_handler)(ClientMoveSetRunSpeed* msg);

// STATIC: 0x00E4D4A4/0x140D36320
bool (*ClientMoveSetSwimBackSpeed::s_handler)(ClientMoveSetSwimBackSpeed* msg);

// STATIC: 0x00E4D62C/0x140D36630
bool (*ClientMoveSetSwimSpeed::s_handler)(ClientMoveSetSwimSpeed* msg);

// STATIC: 0x00E4D614/0x140D36600
bool (*ClientMoveSetTurnRate::s_handler)(ClientMoveSetTurnRate* msg);

// STATIC: 0x00E4D800/0x140D369D8
bool (*ClientMoveSetVehicleRecId::s_handler)(ClientMoveSetVehicleRecId* msg);

// STATIC: 0x00E4D4B0/0x140D36338
bool (*ClientMoveSetWalkSpeed::s_handler)(ClientMoveSetWalkSpeed* msg);

// STATIC: 0x00E4D7A4/0x140D36920
bool (*ClientMoveSetWaterWalk::s_handler)(ClientMoveSetWaterWalk* msg);

// STATIC: 0x00E4D788/0x140D368E8
bool (*ClientMoveSkipTime::s_handler)(ClientMoveSkipTime* msg);

// STATIC: 0x00E4D4FC/0x140D363D0
bool (*ClientMoveTeleport::s_handler)(ClientMoveTeleport* msg);

// STATIC: 0x00E4D6C8/0x140D36768
bool (*ClientMoveUnroot::s_handler)(ClientMoveUnroot* msg);

// STATIC: 0x00E4D4E0/0x140D36398
bool (*ClientMoveUnsetCanFly::s_handler)(ClientMoveUnsetCanFly* msg);

// STATIC: 0x00E4D528/0x140D36428
bool (*ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler)(ClientMoveUnsetCanTransitionBetweenSwimAndFly* msg);

// STATIC: 0x00E4D730/0x140D36838
bool (*ClientMoveUnsetHover::s_handler)(ClientMoveUnsetHover* msg);

// STATIC: 0x00E4D4D0/0x140D36378
bool (*ClientMoveUpdate::s_handler)(ClientMoveUpdate* msg);

// STATIC: 0x00E4D560/0x140D36498
bool (*ClientMoveUpdateCollisionHeight::s_handler)(ClientMoveUpdateCollisionHeight* msg);

// STATIC: 0x00E4D778/0x140D368C8
bool (*ClientMoveUpdateFlightBackSpeed::s_handler)(ClientMoveUpdateFlightBackSpeed* msg);

// STATIC: 0x00E4D490/0x140D362F8
bool (*ClientMoveUpdateFlightSpeed::s_handler)(ClientMoveUpdateFlightSpeed* msg);

// STATIC: 0x00E4D63C/0x140D36650
bool (*ClientMoveUpdateKnockBack::s_handler)(ClientMoveUpdateKnockBack* msg);

// STATIC: 0x00E4D58C/0x140D364F0
bool (*ClientMoveUpdatePitchRate::s_handler)(ClientMoveUpdatePitchRate* msg);

// STATIC: 0x00E4D700/0x140D367D8
bool (*ClientMoveUpdateRunBackSpeed::s_handler)(ClientMoveUpdateRunBackSpeed* msg);

// STATIC: 0x00E4D744/0x140D36860
bool (*ClientMoveUpdateRunSpeed::s_handler)(ClientMoveUpdateRunSpeed* msg);

// STATIC: 0x00E4D72C/0x140D36830
bool (*ClientMoveUpdateSwimBackSpeed::s_handler)(ClientMoveUpdateSwimBackSpeed* msg);

// STATIC: 0x00E4D5B8/0x140D36548
bool (*ClientMoveUpdateSwimSpeed::s_handler)(ClientMoveUpdateSwimSpeed* msg);

// STATIC: 0x00E4D708/0x140D367E8
bool (*ClientMoveUpdateTeleport::s_handler)(ClientMoveUpdateTeleport* msg);

// STATIC: 0x00E4D7D4/0x140D36980
bool (*ClientMoveUpdateTurnRate::s_handler)(ClientMoveUpdateTurnRate* msg);

// STATIC: 0x00E4D5AC/0x140D36530
bool (*ClientMoveUpdateWalkSpeed::s_handler)(ClientMoveUpdateWalkSpeed* msg);

// STATIC: 0x00E4D5FC/0x140D365D0
bool (*ClientPlaySpellVisual::s_handler)(ClientPlaySpellVisual* msg);

// STATIC: 0x00E4D794/0x140D36900
bool (*ClientSetPlayHoverAnim::s_handler)(ClientSetPlayHoverAnim* msg);

// STATIC: 0x00E4D4BC/0x140D36350
bool (*ClientSplineMoveCollisionDisable::s_handler)(ClientSplineMoveCollisionDisable* msg);

// STATIC: 0x00E4D588/0x140D364E8
bool (*ClientSplineMoveCollisionEnable::s_handler)(ClientSplineMoveCollisionEnable* msg);

// STATIC: 0x00E4D7AC/0x140D36930
bool (*ClientSplineMoveGravityDisable::s_handler)(ClientSplineMoveGravityDisable* msg);

// STATIC: 0x00E4D6E8/0x140D367A8
bool (*ClientSplineMoveGravityEnable::s_handler)(ClientSplineMoveGravityEnable* msg);

// STATIC: 0x00E4D4E4/0x140D363A0
bool (*ClientSplineMoveRoot::s_handler)(ClientSplineMoveRoot* msg);

// STATIC: 0x00E4D688/0x140D366E8
bool (*ClientSplineMoveSetFeatherFall::s_handler)(ClientSplineMoveSetFeatherFall* msg);

// STATIC: 0x00E4D81C/0x140D36A10
bool (*ClientSplineMoveSetFlightBackSpeed::s_handler)(ClientSplineMoveSetFlightBackSpeed* msg);

// STATIC: 0x00E4D68C/0x140D366F0
bool (*ClientSplineMoveSetFlightSpeed::s_handler)(ClientSplineMoveSetFlightSpeed* msg);

// STATIC: 0x00E4D604/0x140D365E0
bool (*ClientSplineMoveSetFlying::s_handler)(ClientSplineMoveSetFlying* msg);

// STATIC: 0x00E4D438/0x140D36248
bool (*ClientSplineMoveSetHover::s_handler)(ClientSplineMoveSetHover* msg);

// STATIC: 0x00E4D76C/0x140D368B0
bool (*ClientSplineMoveSetLandWalk::s_handler)(ClientSplineMoveSetLandWalk* msg);

// STATIC: 0x00E4D73C/0x140D36850
bool (*ClientSplineMoveSetNormalFall::s_handler)(ClientSplineMoveSetNormalFall* msg);

// STATIC: 0x00E4D638/0x140D36648
bool (*ClientSplineMoveSetPitchRate::s_handler)(ClientSplineMoveSetPitchRate* msg);

// STATIC: 0x00E4D5C4/0x140D36560
bool (*ClientSplineMoveSetRunBackSpeed::s_handler)(ClientSplineMoveSetRunBackSpeed* msg);

// STATIC: 0x00E4D57C/0x140D364D0
bool (*ClientSplineMoveSetRunMode::s_handler)(ClientSplineMoveSetRunMode* msg);

// STATIC: 0x00E4D740/0x140D36858
bool (*ClientSplineMoveSetRunSpeed::s_handler)(ClientSplineMoveSetRunSpeed* msg);

// STATIC: 0x00E4D678/0x140D366C8
bool (*ClientSplineMoveSetSwimBackSpeed::s_handler)(ClientSplineMoveSetSwimBackSpeed* msg);

// STATIC: 0x00E4D69C/0x140D36710
bool (*ClientSplineMoveSetSwimSpeed::s_handler)(ClientSplineMoveSetSwimSpeed* msg);

// STATIC: 0x00E4D6B0/0x140D36738
bool (*ClientSplineMoveSetTurnRate::s_handler)(ClientSplineMoveSetTurnRate* msg);

// STATIC: 0x00E4D44C/0x140D36270
bool (*ClientSplineMoveSetWalkMode::s_handler)(ClientSplineMoveSetWalkMode* msg);

// STATIC: 0x00E4D82C/0x140D36A30
bool (*ClientSplineMoveSetWalkSpeed::s_handler)(ClientSplineMoveSetWalkSpeed* msg);

// STATIC: 0x00E4D660/0x140D36698
bool (*ClientSplineMoveSetWaterWalk::s_handler)(ClientSplineMoveSetWaterWalk* msg);

// STATIC: 0x00E4D828/0x140D36A28
bool (*ClientSplineMoveStartSwim::s_handler)(ClientSplineMoveStartSwim* msg);

// STATIC: 0x00E4D6BC/0x140D36750
bool (*ClientSplineMoveStopSwim::s_handler)(ClientSplineMoveStopSwim* msg);

// STATIC: 0x00E4D5C8/0x140D36568
bool (*ClientSplineMoveUnroot::s_handler)(ClientSplineMoveUnroot* msg);

// STATIC: 0x00E4D550/0x140D36478
bool (*ClientSplineMoveUnsetFlying::s_handler)(ClientSplineMoveUnsetFlying* msg);

// STATIC: 0x00E4D628/0x140D36628
bool (*ClientSplineMoveUnsetHover::s_handler)(ClientSplineMoveUnsetHover* msg);

// STATIC: 0x00E4D790/0x140D368F8
bool (*ClientSuppressNPCGreetings::s_handler)(ClientSuppressNPCGreetings* msg);

// PROVEN: 465ad810 IDENTICAL/IDENTICAL
// OFFSET: 0x006C99C0/0x140381410
ClientAuraPointsDepleted::ClientAuraPointsDepleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 42684abe IDENTICAL/IDENTICAL
// OFFSET: 0x006A18E0/0x14034EC90
ClientAuraPointsDepleted::~ClientAuraPointsDepleted() {
}

// PROVEN: 6a517d52 IDENTICAL/DIFFERENT
// OFFSET: 0x006B81F0/0x14036B2D0
void ClientAuraPointsDepleted::Unpack(CDataStore* msg, bool full) {
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
        this->m_effectIndex = value21;
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

// PROVEN: 48e55031 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8CF0/0x140357CF0
bool ClientAuraPointsDepleted::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 86616813 IDENTICAL/DIFFERENT
// OFFSET: 0x006A57F0/0x140353680
bool ClientAuraPointsDepleted::Read(CDataStore* msg, bool full) {
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
        this->m_effectIndex = value21;
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

// PROVEN: d351670f IDENTICAL/UNPROVEN
// OFFSET: 0x006B0820/0x140361650
bool ClientAuraPointsDepleted::Handle() {
    if (ClientAuraPointsDepleted::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEAD0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEAD0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientAuraPointsDepleted*)>(reinterpret_cast<uintptr_t>(ClientAuraPointsDepleted::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 14d411e4 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2410/0x14034F920
ClientMoveCollisionDisable::ClientMoveCollisionDisable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 80e7cadf IDENTICAL/IDENTICAL
// OFFSET: 0x006C6EC0/0x14037D910
ClientMoveCollisionDisable::ClientMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7beb2e40 IDENTICAL/IDENTICAL
// OFFSET: 0x006A19A0/0x14034ED60
ClientMoveCollisionDisable::~ClientMoveCollisionDisable() {
}

// PROVEN: b3085b98 IDENTICAL/DIFFERENT
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

// PROVEN: 917e205c IDENTICAL/UNPROVEN
// OFFSET: 0x006AB620/0x14035B1A0
bool ClientMoveCollisionDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a40c148e IDENTICAL/IDENTICAL
// OFFSET: 0x006A7E80/0x140356A80
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

// PROVEN: 7407b20a IDENTICAL/UNPROVEN
// OFFSET: 0x006BFB60/0x140374CF0
bool ClientMoveCollisionDisable::Handle() {
    if (ClientMoveCollisionDisable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE07C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE07C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveCollisionDisable*)>(reinterpret_cast<uintptr_t>(ClientMoveCollisionDisable::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: e2bd4f00 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1CE0/0x14034F130
ClientMoveCollisionEnable::ClientMoveCollisionEnable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 40047fe6 IDENTICAL/IDENTICAL
// OFFSET: 0x006C54C0/0x14037B280
ClientMoveCollisionEnable::ClientMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 0feb1c12 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2ED0/0x1403503E0
ClientMoveCollisionEnable::~ClientMoveCollisionEnable() {
}

// PROVEN: 9e621a40 IDENTICAL/DIFFERENT
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

// PROVEN: fda8a59d IDENTICAL/UNPROVEN
// OFFSET: 0x006C50C0/0x14037ADF0
bool ClientMoveCollisionEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ba58f97a IDENTICAL/IDENTICAL
// OFFSET: 0x006AE9A0/0x14035F120
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

// PROVEN: 2bd8c1df IDENTICAL/UNPROVEN
// OFFSET: 0x006C0D40/0x1403761D0
bool ClientMoveCollisionEnable::Handle() {
    if (ClientMoveCollisionEnable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDFCC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDFCC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveCollisionEnable*)>(reinterpret_cast<uintptr_t>(ClientMoveCollisionEnable::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 1200b753 IDENTICAL/IDENTICAL
// OFFSET: 0x006A29B0/0x14034FE50
ClientMoveGravityDisable::ClientMoveGravityDisable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 4a25db8c IDENTICAL/IDENTICAL
// OFFSET: 0x006C6F10/0x14037D960
ClientMoveGravityDisable::ClientMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ede2728 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2FB0/0x1403504B0
ClientMoveGravityDisable::~ClientMoveGravityDisable() {
}

// PROVEN: f5d72615 IDENTICAL/DIFFERENT
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

// PROVEN: 9b5862e9 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF730/0x140374880
bool ClientMoveGravityDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 03b81940 IDENTICAL/IDENTICAL
// OFFSET: 0x006B0B60/0x140361A40
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

// PROVEN: 2191920e IDENTICAL/UNPROVEN
// OFFSET: 0x006A34B0/0x140350CA0
bool ClientMoveGravityDisable::Handle() {
    if (ClientMoveGravityDisable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFED68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFED68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveGravityDisable*)>(reinterpret_cast<uintptr_t>(ClientMoveGravityDisable::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: b3144733 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1FA0/0x14034F420
ClientMoveGravityEnable::ClientMoveGravityEnable(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: eda89b6a IDENTICAL/IDENTICAL
// OFFSET: 0x006B9430/0x14036C9F0
ClientMoveGravityEnable::ClientMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: e8833812 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F20/0x14034E000
ClientMoveGravityEnable::~ClientMoveGravityEnable() {
}

// PROVEN: 8b8fb955 IDENTICAL/DIFFERENT
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

// PROVEN: 7eb5ee86 IDENTICAL/UNPROVEN
// OFFSET: 0x006C8710/0x14037F7E0
bool ClientMoveGravityEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 0401652d IDENTICAL/IDENTICAL
// OFFSET: 0x006B5A20/0x140367EE0
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

// PROVEN: cddd60d7 IDENTICAL/UNPROVEN
// OFFSET: 0x006A8B20/0x140357A80
bool ClientMoveGravityEnable::Handle() {
    if (ClientMoveGravityEnable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF718.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF718.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveGravityEnable*)>(reinterpret_cast<uintptr_t>(ClientMoveGravityEnable::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 8ad49aaf IDENTICAL/IDENTICAL
// OFFSET: 0x006A2F50/0x140350460
ClientMoveKnockBack::ClientMoveKnockBack(WOWGUID guid, uint32_t ackIndex, const C2Vector& direction, float horizontalSpeed, float verticalSpeed) {
    this->m_guid = guid;
    this->m_horizontalSpeed = horizontalSpeed;
    this->m_ackIndex = ackIndex;
    this->m_verticalSpeed = verticalSpeed;
    this->m_direction = direction;
}

// PROVEN: b90b78bf IDENTICAL/IDENTICAL
// OFFSET: 0x006ADBE0/0x14035E010
ClientMoveKnockBack::ClientMoveKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: abab2fa5 IDENTICAL/IDENTICAL
// OFFSET: 0x006A23E0/0x14034F8F0
ClientMoveKnockBack::~ClientMoveKnockBack() {
}

// PROVEN: 4a099e15 IDENTICAL/DIFFERENT
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

// PROVEN: b2ddde43 IDENTICAL/UNPROVEN
// OFFSET: 0x006C87F0/0x14037F8C0
bool ClientMoveKnockBack::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 5e34d84f IDENTICAL/DIFFERENT
// OFFSET: 0x006B3620/0x140365110
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

// OFFSET: 0x006A28D0/0x14034FD50
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

// PROVEN: 7f4c8502 IDENTICAL/UNPROVEN
// OFFSET: 0x006A7540/0x140355FE0
bool ClientMoveKnockBack::Handle() {
    if (ClientMoveKnockBack::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF8FC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF8FC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveKnockBack*)>(reinterpret_cast<uintptr_t>(ClientMoveKnockBack::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 619c80be IDENTICAL/IDENTICAL
// OFFSET: 0x006A3C50/0x1403514E0
ClientMoveRoot::ClientMoveRoot(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 76e7e259 IDENTICAL/IDENTICAL
// OFFSET: 0x006C05B0/0x140375880
ClientMoveRoot::ClientMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 7c524104 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1B70/0x14034F030
ClientMoveRoot::~ClientMoveRoot() {
}

// PROVEN: cb8cd089 IDENTICAL/DIFFERENT
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

// PROVEN: 35921c4e IDENTICAL/UNPROVEN
// OFFSET: 0x006BB400/0x14036F2C0
bool ClientMoveRoot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 77c70b26 IDENTICAL/IDENTICAL
// OFFSET: 0x006A51F0/0x140353190
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

// PROVEN: 80d50a48 IDENTICAL/UNPROVEN
// OFFSET: 0x006B3C10/0x1403658A0
bool ClientMoveRoot::Handle() {
    if (ClientMoveRoot::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEDD0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEDD0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveRoot*)>(reinterpret_cast<uintptr_t>(ClientMoveRoot::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 927e999c IDENTICAL/IDENTICAL
// OFFSET: 0x006A10C0/0x14034E300
ClientMoveSetCanFly::ClientMoveSetCanFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: fd6f3024 IDENTICAL/IDENTICAL
// OFFSET: 0x006C5F60/0x14037C380
ClientMoveSetCanFly::ClientMoveSetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5433cb5b IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E00/0x14034F270
ClientMoveSetCanFly::~ClientMoveSetCanFly() {
}

// PROVEN: 501b94c6 IDENTICAL/DIFFERENT
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

// PROVEN: 98a7ea07 IDENTICAL/UNPROVEN
// OFFSET: 0x006C07B0/0x140375AF0
bool ClientMoveSetCanFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e7de905f IDENTICAL/IDENTICAL
// OFFSET: 0x006BBBB0/0x14036FC90
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

// PROVEN: 4ae2e678 IDENTICAL/UNPROVEN
// OFFSET: 0x006B2080/0x1403635B0
bool ClientMoveSetCanFly::Handle() {
    if (ClientMoveSetCanFly::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF490.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF490.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetCanFly*)>(reinterpret_cast<uintptr_t>(ClientMoveSetCanFly::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 151b66cb IDENTICAL/IDENTICAL
// OFFSET: 0x006A1CA0/0x14034F100
ClientMoveSetCanTransitionBetweenSwimAndFly::ClientMoveSetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: d3d29216 IDENTICAL/IDENTICAL
// OFFSET: 0x006C9A50/0x1403814B0
ClientMoveSetCanTransitionBetweenSwimAndFly::ClientMoveSetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a0b81e8a IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F10/0x14034DFF0
ClientMoveSetCanTransitionBetweenSwimAndFly::~ClientMoveSetCanTransitionBetweenSwimAndFly() {
}

// PROVEN: 89b2c708 IDENTICAL/DIFFERENT
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

// PROVEN: 78ee688d IDENTICAL/UNPROVEN
// OFFSET: 0x006CFCA0/0x14038A230
bool ClientMoveSetCanTransitionBetweenSwimAndFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d374ed1d IDENTICAL/IDENTICAL
// OFFSET: 0x006C7F90/0x14037EED0
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

// PROVEN: b5ef1662 IDENTICAL/UNPROVEN
// OFFSET: 0x006A4890/0x1403523E0
bool ClientMoveSetCanTransitionBetweenSwimAndFly::Handle() {
    if (ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEB00.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEB00.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetCanTransitionBetweenSwimAndFly*)>(reinterpret_cast<uintptr_t>(ClientMoveSetCanTransitionBetweenSwimAndFly::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: c9cc0e19 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1050/0x14034E2A0
ClientMoveSetCollisionHeight::ClientMoveSetCollisionHeight(WOWGUID guid, uint32_t ackIndex, float height, uint32_t reason) {
    this->m_guid = guid;
    this->m_height = height;
    this->m_ackIndex = ackIndex;
    this->m_reason = reason;
}

// PROVEN: 01adc0df IDENTICAL/IDENTICAL
// OFFSET: 0x006C0E20/0x140376310
ClientMoveSetCollisionHeight::ClientMoveSetCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 73ea8b6d IDENTICAL/IDENTICAL
// OFFSET: 0x006A1690/0x14034E9D0
ClientMoveSetCollisionHeight::~ClientMoveSetCollisionHeight() {
}

// PROVEN: a601d573 IDENTICAL/DIFFERENT
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

// PROVEN: ee4fea34 IDENTICAL/UNPROVEN
// OFFSET: 0x006B72C0/0x14036A030
bool ClientMoveSetCollisionHeight::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4b3b74ea IDENTICAL/DIFFERENT
// OFFSET: 0x006B6ED0/0x140369B40
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

// PROVEN: 2c668440 IDENTICAL/UNPROVEN
// OFFSET: 0x006A5BB0/0x140353B50
bool ClientMoveSetCollisionHeight::Handle() {
    if (ClientMoveSetCollisionHeight::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD04C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD04C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetCollisionHeight*)>(reinterpret_cast<uintptr_t>(ClientMoveSetCollisionHeight::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: a60416d8 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2AA0/0x14034FF80
ClientMoveSetFeatherFall::ClientMoveSetFeatherFall(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 1b8328c4 IDENTICAL/IDENTICAL
// OFFSET: 0x006C6F90/0x14037D9E0
ClientMoveSetFeatherFall::ClientMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3851f197 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1FE0/0x14034F450
ClientMoveSetFeatherFall::~ClientMoveSetFeatherFall() {
}

// PROVEN: bc8e3f8c IDENTICAL/DIFFERENT
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

// PROVEN: 92fbbad0 IDENTICAL/UNPROVEN
// OFFSET: 0x006C5120/0x14037AE60
bool ClientMoveSetFeatherFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1ad03a79 IDENTICAL/IDENTICAL
// OFFSET: 0x006AEF70/0x14035F850
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

// PROVEN: 1cae558d IDENTICAL/UNPROVEN
// OFFSET: 0x006AE7D0/0x14035EEE0
bool ClientMoveSetFeatherFall::Handle() {
    if (ClientMoveSetFeatherFall::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF440.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF440.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetFeatherFall*)>(reinterpret_cast<uintptr_t>(ClientMoveSetFeatherFall::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: d0d84d45 IDENTICAL/IDENTICAL
// OFFSET: 0x006A7210/0x14034E020
ClientMoveSetFlightBackSpeed::ClientMoveSetFlightBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 2b01b4ec IDENTICAL/IDENTICAL
// OFFSET: 0x006C6030/0x14037C450
ClientMoveSetFlightBackSpeed::ClientMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5bc26de3 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2380/0x14034F8A0
ClientMoveSetFlightBackSpeed::~ClientMoveSetFlightBackSpeed() {
}

// PROVEN: 959c8edf IDENTICAL/DIFFERENT
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

// PROVEN: 282fb0b7 IDENTICAL/UNPROVEN
// OFFSET: 0x006D0460/0x14038AE60
bool ClientMoveSetFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f29ecf4f IDENTICAL/DIFFERENT
// OFFSET: 0x006BF540/0x140374610
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

// PROVEN: 7d1694c5 IDENTICAL/UNPROVEN
// OFFSET: 0x006B1A60/0x140362DB0
bool ClientMoveSetFlightBackSpeed::Handle() {
    if (ClientMoveSetFlightBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE1A4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE1A4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetFlightBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetFlightBackSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 85f56437 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2BA0/0x1403500E0
ClientMoveSetFlightSpeed::ClientMoveSetFlightSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 3a9e9e88 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8410/0x14037F440
ClientMoveSetFlightSpeed::ClientMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: af9ead83 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2B10/0x140350050
ClientMoveSetFlightSpeed::~ClientMoveSetFlightSpeed() {
}

// PROVEN: 05482758 IDENTICAL/DIFFERENT
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

// PROVEN: ec443e5f IDENTICAL/UNPROVEN
// OFFSET: 0x006C48B0/0x14037A7F0
bool ClientMoveSetFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b4c03838 IDENTICAL/DIFFERENT
// OFFSET: 0x006C0600/0x1403758D0
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

// PROVEN: bf7036dd IDENTICAL/UNPROVEN
// OFFSET: 0x006B8160/0x14036B1F0
bool ClientMoveSetFlightSpeed::Handle() {
    if (ClientMoveSetFlightSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFBCC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFBCC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetFlightSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetFlightSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 1cc6263a IDENTICAL/IDENTICAL
// OFFSET: 0x006A2340/0x14034F870
ClientMoveSetHover::ClientMoveSetHover(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 9526e212 IDENTICAL/IDENTICAL
// OFFSET: 0x006C7F40/0x14037EE80
ClientMoveSetHover::ClientMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 88c63a6f IDENTICAL/IDENTICAL
// OFFSET: 0x006A2450/0x14034F950
ClientMoveSetHover::~ClientMoveSetHover() {
}

// PROVEN: b43b45fe IDENTICAL/IDENTICAL
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

// PROVEN: 296e79fe IDENTICAL/UNPROVEN
// OFFSET: 0x006C5F50/0x14037C370
bool ClientMoveSetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: def8fea0 IDENTICAL/IDENTICAL
// OFFSET: 0x006AB630/0x14035B1B0
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

// PROVEN: d6e5bf37 IDENTICAL/UNPROVEN
// OFFSET: 0x006B6450/0x140368C00
bool ClientMoveSetHover::Handle() {
    if (ClientMoveSetHover::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3AC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3AC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetHover*)>(reinterpret_cast<uintptr_t>(ClientMoveSetHover::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: eb38b2c3 IDENTICAL/IDENTICAL
// OFFSET: 0x006A68A0/0x14034DFA0
ClientMoveSetLandWalk::ClientMoveSetLandWalk(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 63302a1b IDENTICAL/IDENTICAL
// OFFSET: 0x006CB670/0x140383D30
ClientMoveSetLandWalk::ClientMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 499bf972 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1620/0x14034E920
ClientMoveSetLandWalk::~ClientMoveSetLandWalk() {
}

// PROVEN: 2bda9e81 IDENTICAL/IDENTICAL
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

// PROVEN: 208bd6e1 IDENTICAL/UNPROVEN
// OFFSET: 0x006C6F00/0x14037D950
bool ClientMoveSetLandWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9b78ef13 IDENTICAL/IDENTICAL
// OFFSET: 0x006C3E10/0x140379DB0
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

// PROVEN: b9d3fecb IDENTICAL/UNPROVEN
// OFFSET: 0x006ABE50/0x14035BB80
bool ClientMoveSetLandWalk::Handle() {
    if (ClientMoveSetLandWalk::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF3C8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF3C8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetLandWalk*)>(reinterpret_cast<uintptr_t>(ClientMoveSetLandWalk::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: ccfccc94 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2DF0/0x140350360
ClientMoveSetNormalFall::ClientMoveSetNormalFall(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: fea953ca IDENTICAL/IDENTICAL
// OFFSET: 0x006B5D90/0x140368330
ClientMoveSetNormalFall::ClientMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 94ad6c9e IDENTICAL/IDENTICAL
// OFFSET: 0x006A1EF0/0x14034F370
ClientMoveSetNormalFall::~ClientMoveSetNormalFall() {
}

// PROVEN: 58ae70ce IDENTICAL/IDENTICAL
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

// PROVEN: 72e3ff7c IDENTICAL/UNPROVEN
// OFFSET: 0x006D3860/0x14038FC30
bool ClientMoveSetNormalFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 198c9053 IDENTICAL/IDENTICAL
// OFFSET: 0x006CB8A0/0x140384030
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

// PROVEN: 1c86cde8 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4560/0x1403664C0
bool ClientMoveSetNormalFall::Handle() {
    if (ClientMoveSetNormalFall::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDFA0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDFA0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetNormalFall*)>(reinterpret_cast<uintptr_t>(ClientMoveSetNormalFall::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 9cb17202 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1000/0x14034E260
ClientMoveSetPitchRate::ClientMoveSetPitchRate(WOWGUID guid, uint32_t ackIndex, float rate) {
    this->m_guid = guid;
    this->m_rate = rate;
    this->m_ackIndex = ackIndex;
}

// PROVEN: dfd40978 IDENTICAL/IDENTICAL
// OFFSET: 0x006B47A0/0x1403667C0
ClientMoveSetPitchRate::ClientMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 05bc15af IDENTICAL/IDENTICAL
// OFFSET: 0x006A0EB0/0x14034DF70
ClientMoveSetPitchRate::~ClientMoveSetPitchRate() {
}

// PROVEN: c726a058 IDENTICAL/DIFFERENT
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

// PROVEN: 3c8b2523 IDENTICAL/UNPROVEN
// OFFSET: 0x006D30B0/0x140373560
bool ClientMoveSetPitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2761618b IDENTICAL/DIFFERENT
// OFFSET: 0x006C41E0/0x140352440
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

// PROVEN: 037b14d7 IDENTICAL/UNPROVEN
// OFFSET: 0x006AB890/0x14035B4E0
bool ClientMoveSetPitchRate::Handle() {
    if (ClientMoveSetPitchRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF9AC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF9AC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetPitchRate*)>(reinterpret_cast<uintptr_t>(ClientMoveSetPitchRate::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 42afe1bc IDENTICAL/IDENTICAL
// OFFSET: 0x006A1240/0x14034E520
ClientMoveSetRunBackSpeed::ClientMoveSetRunBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 7d68c2af IDENTICAL/IDENTICAL
// OFFSET: 0x006C3DC0/0x140379D50
ClientMoveSetRunBackSpeed::ClientMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 612185b7 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2330/0x14034F860
ClientMoveSetRunBackSpeed::~ClientMoveSetRunBackSpeed() {
}

// PROVEN: 5d67cb88 IDENTICAL/DIFFERENT
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

// PROVEN: b6ae2303 IDENTICAL/UNPROVEN
// OFFSET: 0x006D7720/0x14037ADE0
bool ClientMoveSetRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: db34f697 IDENTICAL/DIFFERENT
// OFFSET: 0x006CF0E0/0x140368CC0
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

// PROVEN: 39a8f899 IDENTICAL/UNPROVEN
// OFFSET: 0x006ACE70/0x14035CEF0
bool ClientMoveSetRunBackSpeed::Handle() {
    if (ClientMoveSetRunBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD8D4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD8D4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetRunBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetRunBackSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b45b2acd IDENTICAL/IDENTICAL
// OFFSET: 0x006A15E0/0x14034E8F0
ClientMoveSetRunSpeed::ClientMoveSetRunSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: 00e52bb6 IDENTICAL/IDENTICAL
// OFFSET: 0x006CA860/0x140382930
ClientMoveSetRunSpeed::ClientMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8350f0e2 IDENTICAL/IDENTICAL
// OFFSET: 0x006A24C0/0x14034F9E0
ClientMoveSetRunSpeed::~ClientMoveSetRunSpeed() {
}

// PROVEN: 087d219b IDENTICAL/DIFFERENT
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

// PROVEN: 4515e61c IDENTICAL/UNPROVEN
// OFFSET: 0x006C5FA0/0x14037C3C0
bool ClientMoveSetRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 14099eae IDENTICAL/DIFFERENT
// OFFSET: 0x006AD190/0x14035D270
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

// PROVEN: d681f80a IDENTICAL/UNPROVEN
// OFFSET: 0x006AE220/0x14035E770
bool ClientMoveSetRunSpeed::Handle() {
    if (ClientMoveSetRunSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE300.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE300.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetRunSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetRunSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: f9ea59f2 IDENTICAL/IDENTICAL
// OFFSET: 0x006A45A0/0x140352030
ClientMoveSetSwimBackSpeed::ClientMoveSetSwimBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 34247677 IDENTICAL/IDENTICAL
// OFFSET: 0x006C0C40/0x1403760B0
ClientMoveSetSwimBackSpeed::ClientMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4ac7260c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2FC0/0x1403504C0
ClientMoveSetSwimBackSpeed::~ClientMoveSetSwimBackSpeed() {
}

// PROVEN: cdb81759 IDENTICAL/DIFFERENT
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

// PROVEN: 455fbda2 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF830/0x1403749D0
bool ClientMoveSetSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 47223aa0 IDENTICAL/DIFFERENT
// OFFSET: 0x006ACBD0/0x14035CBA0
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

// PROVEN: c8192a16 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5400/0x1403676F0
bool ClientMoveSetSwimBackSpeed::Handle() {
    if (ClientMoveSetSwimBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE634.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE634.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetSwimBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetSwimBackSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 347a5d30 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2390/0x14034F8B0
ClientMoveSetSwimSpeed::ClientMoveSetSwimSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_speed = speed;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 2a3981d0 IDENTICAL/IDENTICAL
// OFFSET: 0x006C46D0/0x14037A5D0
ClientMoveSetSwimSpeed::ClientMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ecfa107f IDENTICAL/IDENTICAL
// OFFSET: 0x006A28C0/0x14034FD40
ClientMoveSetSwimSpeed::~ClientMoveSetSwimSpeed() {
}

// PROVEN: 6785133b IDENTICAL/DIFFERENT
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

// PROVEN: 32617c54 IDENTICAL/UNPROVEN
// OFFSET: 0x006C46C0/0x14037A5C0
bool ClientMoveSetSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3d06b3bc IDENTICAL/DIFFERENT
// OFFSET: 0x006C43B0/0x14037A200
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

// PROVEN: ddbd1031 IDENTICAL/UNPROVEN
// OFFSET: 0x006A6690/0x140354A20
bool ClientMoveSetSwimSpeed::Handle() {
    if (ClientMoveSetSwimSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD91C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD91C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetSwimSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetSwimSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 7e220f79 IDENTICAL/IDENTICAL
// OFFSET: 0x006A7960/0x14034E0D0
ClientMoveSetTurnRate::ClientMoveSetTurnRate(WOWGUID guid, uint32_t ackIndex, float rate) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_rate = rate;
}

// PROVEN: 8a1f649b IDENTICAL/IDENTICAL
// OFFSET: 0x006AC0E0/0x14035BEE0
ClientMoveSetTurnRate::ClientMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 339b9420 IDENTICAL/IDENTICAL
// OFFSET: 0x006A10F0/0x14034E320
ClientMoveSetTurnRate::~ClientMoveSetTurnRate() {
}

// PROVEN: 352c18f5 IDENTICAL/DIFFERENT
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

// PROVEN: fc687abb IDENTICAL/UNPROVEN
// OFFSET: 0x006C9930/0x140381370
bool ClientMoveSetTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e94ff991 IDENTICAL/DIFFERENT
// OFFSET: 0x006C5130/0x14037AE70
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

// PROVEN: cb89a52f IDENTICAL/UNPROVEN
// OFFSET: 0x006B2E50/0x140364740
bool ClientMoveSetTurnRate::Handle() {
    if (ClientMoveSetTurnRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD9EC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD9EC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetTurnRate*)>(reinterpret_cast<uintptr_t>(ClientMoveSetTurnRate::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 7462773e IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E80/0x14034F2F0
ClientMoveSetVehicleRecId::ClientMoveSetVehicleRecId(WOWGUID guid, uint32_t ackIndex, uint32_t vehicleRecId) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_vehicleRecId = vehicleRecId;
}

// PROVEN: 62b3150b IDENTICAL/IDENTICAL
// OFFSET: 0x006B7D00/0x14036AC70
ClientMoveSetVehicleRecId::ClientMoveSetVehicleRecId(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 30d9a879 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1110/0x14034E340
ClientMoveSetVehicleRecId::~ClientMoveSetVehicleRecId() {
}

// PROVEN: 1d5a817d IDENTICAL/IDENTICAL
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

// PROVEN: 657e45ec IDENTICAL/UNPROVEN
// OFFSET: 0x006C61A0/0x14037C5D0
bool ClientMoveSetVehicleRecId::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ef1818f4 IDENTICAL/IDENTICAL
// OFFSET: 0x006B74D0/0x14036A2C0
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

// PROVEN: c415bf0e IDENTICAL/UNPROVEN
// OFFSET: 0x006ABE10/0x14035BAD0
bool ClientMoveSetVehicleRecId::Handle() {
    if (ClientMoveSetVehicleRecId::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF964.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF964.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetVehicleRecId*)>(reinterpret_cast<uintptr_t>(ClientMoveSetVehicleRecId::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 8d56423c IDENTICAL/IDENTICAL
// OFFSET: 0x006A2F00/0x140350420
ClientMoveSetWalkSpeed::ClientMoveSetWalkSpeed(WOWGUID guid, uint32_t ackIndex, float speed) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
    this->m_speed = speed;
}

// PROVEN: c9830330 IDENTICAL/IDENTICAL
// OFFSET: 0x006AA530/0x140359BC0
ClientMoveSetWalkSpeed::ClientMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: d56cecdc IDENTICAL/IDENTICAL
// OFFSET: 0x006A2A10/0x14034FF00
ClientMoveSetWalkSpeed::~ClientMoveSetWalkSpeed() {
}

// PROVEN: 7ee3ea6a IDENTICAL/DIFFERENT
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

// PROVEN: 1f0ebf53 IDENTICAL/UNPROVEN
// OFFSET: 0x006C0E60/0x140376350
bool ClientMoveSetWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f02af77e IDENTICAL/DIFFERENT
// OFFSET: 0x006B7310/0x14036A0A0
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

// PROVEN: fbbbbb7a IDENTICAL/UNPROVEN
// OFFSET: 0x006BD9D0/0x140372390
bool ClientMoveSetWalkSpeed::Handle() {
    if (ClientMoveSetWalkSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6D4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6D4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetWalkSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveSetWalkSpeed::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 2d20c8fe IDENTICAL/IDENTICAL
// OFFSET: 0x006A1370/0x14034E660
ClientMoveSetWaterWalk::ClientMoveSetWaterWalk(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 6468136f IDENTICAL/IDENTICAL
// OFFSET: 0x006BBFA0/0x1403701C0
ClientMoveSetWaterWalk::ClientMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: fed7fe41 IDENTICAL/IDENTICAL
// OFFSET: 0x006A14F0/0x14034E7C0
ClientMoveSetWaterWalk::~ClientMoveSetWaterWalk() {
}

// PROVEN: 2e7f5ad9 IDENTICAL/DIFFERENT
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

// PROVEN: b0255e79 IDENTICAL/UNPROVEN
// OFFSET: 0x006C9130/0x140380730
bool ClientMoveSetWaterWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 197b37da IDENTICAL/IDENTICAL
// OFFSET: 0x006B4360/0x140366220
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

// PROVEN: f3f427ec IDENTICAL/UNPROVEN
// OFFSET: 0x006AA080/0x140359660
bool ClientMoveSetWaterWalk::Handle() {
    if (ClientMoveSetWaterWalk::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD320.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD320.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetWaterWalk*)>(reinterpret_cast<uintptr_t>(ClientMoveSetWaterWalk::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 85cfaf5f IDENTICAL/IDENTICAL
// OFFSET: 0x006AC7E0/0x14035C670
ClientMoveSkipTime::ClientMoveSkipTime(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 96060234 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1DA0/0x14034F220
ClientMoveSkipTime::~ClientMoveSkipTime() {
}

// PROVEN: e13494af IDENTICAL/DIFFERENT
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

// PROVEN: 63363caa IDENTICAL/UNPROVEN
// OFFSET: 0x006D95F0/0x140397A30
bool ClientMoveSkipTime::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: edb7f816 IDENTICAL/IDENTICAL
// OFFSET: 0x006CEEE0/0x140389020
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

// PROVEN: c4b1938d IDENTICAL/UNPROVEN
// OFFSET: 0x006A7250/0x140355C70
bool ClientMoveSkipTime::Handle() {
    if (ClientMoveSkipTime::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF6A4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF6A4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSkipTime*)>(reinterpret_cast<uintptr_t>(ClientMoveSkipTime::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 76553b1c IDENTICAL/DIFFERENT
// OFFSET: 0x006C6110/0x14037C530
ClientMoveTeleport::ClientMoveTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_hasTransport = false;
    this->m_hasVehicle = false;
    this->Unpack(msg, true);
}

// PROVEN: 21fcefc8 IDENTICAL/IDENTICAL
// OFFSET: 0x006CCA20/0x140385B70
ClientMoveTeleport::~ClientMoveTeleport() {
}

// PROVEN: 98cc8f21 IDENTICAL/DIFFERENT
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

// PROVEN: 159579e8 IDENTICAL/UNPROVEN
// OFFSET: 0x006D64C0/0x140393A00
bool ClientMoveTeleport::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a24ffb00 IDENTICAL/DIFFERENT
// OFFSET: 0x006D3870/0x14038FC40
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

// OFFSET: 0x006BCC80/0x140371260
bool ClientMoveTeleport::IsValid() const {
    return this->m_facing >= 0.0f && this->m_facing < 6.2831864f && _finite(this->m_facing) && _finite(this->m_position.z) && _finite(this->m_position.y) && IsFinite(this->m_position.x);
}

// PROVEN: 288c63d7 IDENTICAL/UNPROVEN
// OFFSET: 0x006AC120/0x14035BF20
bool ClientMoveTeleport::Handle() {
    if (ClientMoveTeleport::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEB28.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEB28.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveTeleport*)>(reinterpret_cast<uintptr_t>(ClientMoveTeleport::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: e1a44e1c IDENTICAL/IDENTICAL
// OFFSET: 0x006A1580/0x14034E880
ClientMoveUnroot::ClientMoveUnroot(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: dfd197c2 IDENTICAL/IDENTICAL
// OFFSET: 0x006C5FB0/0x14037C3D0
ClientMoveUnroot::ClientMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9a12c140 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2800/0x14034FCC0
ClientMoveUnroot::~ClientMoveUnroot() {
}

// PROVEN: 698067f4 IDENTICAL/DIFFERENT
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

// PROVEN: fcee716f IDENTICAL/UNPROVEN
// OFFSET: 0x006CB510/0x140383B70
bool ClientMoveUnroot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3d9b1b60 IDENTICAL/IDENTICAL
// OFFSET: 0x006B45F0/0x140366580
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

// PROVEN: 1203c95d IDENTICAL/UNPROVEN
// OFFSET: 0x006C1540/0x140376B80
bool ClientMoveUnroot::Handle() {
    if (ClientMoveUnroot::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF538.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF538.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUnroot*)>(reinterpret_cast<uintptr_t>(ClientMoveUnroot::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 4623cb4c IDENTICAL/IDENTICAL
// OFFSET: 0x006A16A0/0x14034E9F0
ClientMoveUnsetCanFly::ClientMoveUnsetCanFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 6872bcda IDENTICAL/IDENTICAL
// OFFSET: 0x006C4950/0x14037A8A0
ClientMoveUnsetCanFly::ClientMoveUnsetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5f7ce380 IDENTICAL/IDENTICAL
// OFFSET: 0x006A23D0/0x14034F8E0
ClientMoveUnsetCanFly::~ClientMoveUnsetCanFly() {
}

// PROVEN: d68112b0 IDENTICAL/DIFFERENT
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

// PROVEN: abd7ce75 IDENTICAL/UNPROVEN
// OFFSET: 0x006CA840/0x140382910
bool ClientMoveUnsetCanFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2a8ecefa IDENTICAL/IDENTICAL
// OFFSET: 0x006C8150/0x14037F0D0
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

// PROVEN: 451bd6f2 IDENTICAL/UNPROVEN
// OFFSET: 0x006B1A20/0x140362D50
bool ClientMoveUnsetCanFly::Handle() {
    if (ClientMoveUnsetCanFly::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE634.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE634.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUnsetCanFly*)>(reinterpret_cast<uintptr_t>(ClientMoveUnsetCanFly::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 750dd881 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2BE0/0x140350110
ClientMoveUnsetCanTransitionBetweenSwimAndFly::ClientMoveUnsetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 23a70d57 IDENTICAL/IDENTICAL
// OFFSET: 0x006CAB80/0x140382D00
ClientMoveUnsetCanTransitionBetweenSwimAndFly::ClientMoveUnsetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 240e4cfb IDENTICAL/IDENTICAL
// OFFSET: 0x006A21B0/0x14034F6B0
ClientMoveUnsetCanTransitionBetweenSwimAndFly::~ClientMoveUnsetCanTransitionBetweenSwimAndFly() {
}

// PROVEN: dce4dd1b IDENTICAL/DIFFERENT
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

// PROVEN: b067469c IDENTICAL/UNPROVEN
// OFFSET: 0x006D72F0/0x140394F70
bool ClientMoveUnsetCanTransitionBetweenSwimAndFly::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: b6e6423d IDENTICAL/IDENTICAL
// OFFSET: 0x006C5330/0x14037B080
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

// PROVEN: d8ab744b IDENTICAL/UNPROVEN
// OFFSET: 0x006A5C40/0x140353C00
bool ClientMoveUnsetCanTransitionBetweenSwimAndFly::Handle() {
    if (ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3E0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3E0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUnsetCanTransitionBetweenSwimAndFly*)>(reinterpret_cast<uintptr_t>(ClientMoveUnsetCanTransitionBetweenSwimAndFly::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: a832d2be IDENTICAL/IDENTICAL
// OFFSET: 0x006A2A60/0x14034FF50
ClientMoveUnsetHover::ClientMoveUnsetHover(WOWGUID guid, uint32_t ackIndex) {
    this->m_guid = guid;
    this->m_ackIndex = ackIndex;
}

// PROVEN: 1733a632 IDENTICAL/IDENTICAL
// OFFSET: 0x006BCC40/0x140371220
ClientMoveUnsetHover::ClientMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 66456d9d IDENTICAL/IDENTICAL
// OFFSET: 0x006A0DD0/0x14034DE70
ClientMoveUnsetHover::~ClientMoveUnsetHover() {
}

// PROVEN: a760bdfb IDENTICAL/IDENTICAL
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

// PROVEN: 2e207e16 IDENTICAL/UNPROVEN
// OFFSET: 0x006C9A00/0x140381450
bool ClientMoveUnsetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4162dc96 IDENTICAL/IDENTICAL
// OFFSET: 0x006BB820/0x14036F800
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

// PROVEN: 2dcece11 IDENTICAL/UNPROVEN
// OFFSET: 0x006AB850/0x14035B480
bool ClientMoveUnsetHover::Handle() {
    if (ClientMoveUnsetHover::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3BC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3BC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUnsetHover*)>(reinterpret_cast<uintptr_t>(ClientMoveUnsetHover::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 142eadda IDENTICAL/IDENTICAL
// OFFSET: 0x006D4540/0x140390F20
ClientMoveUpdate::ClientMoveUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: dd173323 IDENTICAL/UNPROVEN
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

// PROVEN: 89d467ad IDENTICAL/UNPROVEN
// OFFSET: 0x006CEED0/0x140389010
bool ClientMoveUpdate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C6FD0/0x14037DA20
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

// PROVEN: 8e3778d2 IDENTICAL/UNPROVEN
// OFFSET: 0x006ABA40/0x14035B710
bool ClientMoveUpdate::Handle() {
    if (ClientMoveUpdate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE6E8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE6E8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdate*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdate::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 14e401e5 IDENTICAL/IDENTICAL
// OFFSET: 0x006DA970/0x1403990B0
ClientMoveUpdateCollisionHeight::ClientMoveUpdateCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 02bb83d0 IDENTICAL/UNPROVEN
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

// PROVEN: 5175528c IDENTICAL/UNPROVEN
// OFFSET: 0x006CA8B0/0x140382980
bool ClientMoveUpdateCollisionHeight::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C35A0/0x140379070
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

// PROVEN: 8fdc7d1f IDENTICAL/UNPROVEN
// OFFSET: 0x006BED60/0x140373C70
bool ClientMoveUpdateCollisionHeight::Handle() {
    if (ClientMoveUpdateCollisionHeight::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFBFC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFBFC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateCollisionHeight*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateCollisionHeight::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 8e8d61e2 IDENTICAL/IDENTICAL
// OFFSET: 0x006D4FD0/0x140391E10
ClientMoveUpdateFlightBackSpeed::ClientMoveUpdateFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1157518d IDENTICAL/UNPROVEN
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

// PROVEN: acdf1f57 IDENTICAL/UNPROVEN
// OFFSET: 0x006DAEF0/0x1403996E0
bool ClientMoveUpdateFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5010/0x140391E60
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

// PROVEN: 0da129b8 IDENTICAL/UNPROVEN
// OFFSET: 0x006A76E0/0x140356200
bool ClientMoveUpdateFlightBackSpeed::Handle() {
    if (ClientMoveUpdateFlightBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE6D0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE6D0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateFlightBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateFlightBackSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: e725f957 IDENTICAL/IDENTICAL
// OFFSET: 0x006D8670/0x140396770
ClientMoveUpdateFlightSpeed::ClientMoveUpdateFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1ebbbcdf IDENTICAL/UNPROVEN
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

// PROVEN: 92efb285 IDENTICAL/UNPROVEN
// OFFSET: 0x006CA8A0/0x140382970
bool ClientMoveUpdateFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C9B40/0x1403815E0
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

// PROVEN: 45dd11e4 IDENTICAL/UNPROVEN
// OFFSET: 0x006B8950/0x14036BC10
bool ClientMoveUpdateFlightSpeed::Handle() {
    if (ClientMoveUpdateFlightSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF164.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF164.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateFlightSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateFlightSpeed::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: d9eb91d8 IDENTICAL/IDENTICAL
// OFFSET: 0x006D9280/0x140397690
ClientMoveUpdateKnockBack::ClientMoveUpdateKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 49046eeb IDENTICAL/UNPROVEN
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

// PROVEN: df179b92 IDENTICAL/UNPROVEN
// OFFSET: 0x006D1FF0/0x14038D620
bool ClientMoveUpdateKnockBack::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CD700/0x140386B30
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

// PROVEN: 4bc3cce2 IDENTICAL/UNPROVEN
// OFFSET: 0x006BECD0/0x140373BA0
bool ClientMoveUpdateKnockBack::Handle() {
    if (ClientMoveUpdateKnockBack::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3AC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3AC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateKnockBack*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateKnockBack::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 8313458e IDENTICAL/IDENTICAL
// OFFSET: 0x006CB470/0x140383A60
ClientMoveUpdatePitchRate::ClientMoveUpdatePitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a1f2bed7 IDENTICAL/UNPROVEN
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

// PROVEN: 2121f2b1 IDENTICAL/UNPROVEN
// OFFSET: 0x006D4FC0/0x140391E00
bool ClientMoveUpdatePitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CC150/0x140384D80
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

// PROVEN: 633fa99b IDENTICAL/UNPROVEN
// OFFSET: 0x006B6410/0x140368BA0
bool ClientMoveUpdatePitchRate::Handle() {
    if (ClientMoveUpdatePitchRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF3B4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF3B4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdatePitchRate*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdatePitchRate::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 31a9754c IDENTICAL/IDENTICAL
// OFFSET: 0x006D9700/0x140397B80
ClientMoveUpdateRunBackSpeed::ClientMoveUpdateRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3c9118d8 IDENTICAL/UNPROVEN
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

// PROVEN: 56b9abd6 IDENTICAL/UNPROVEN
// OFFSET: 0x006D92F0/0x1403976E0
bool ClientMoveUpdateRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D46D0/0x140391130
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

// PROVEN: 261408c9 IDENTICAL/UNPROVEN
// OFFSET: 0x006AA000/0x1403595A0
bool ClientMoveUpdateRunBackSpeed::Handle() {
    if (ClientMoveUpdateRunBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE800.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE800.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateRunBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateRunBackSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 1823e0e4 IDENTICAL/IDENTICAL
// OFFSET: 0x006D72B0/0x140394F20
ClientMoveUpdateRunSpeed::ClientMoveUpdateRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 90cc337b IDENTICAL/UNPROVEN
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

// PROVEN: f605d254 IDENTICAL/UNPROVEN
// OFFSET: 0x006D8D50/0x140397150
bool ClientMoveUpdateRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D5C20/0x140392FB0
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

// PROVEN: 9d29ebfb IDENTICAL/UNPROVEN
// OFFSET: 0x006AC050/0x14035BE40
bool ClientMoveUpdateRunSpeed::Handle() {
    if (ClientMoveUpdateRunSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3D4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3D4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateRunSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateRunSpeed::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 469341d5 IDENTICAL/IDENTICAL
// OFFSET: 0x006D4580/0x140390F70
ClientMoveUpdateSwimBackSpeed::ClientMoveUpdateSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8a6c429f IDENTICAL/UNPROVEN
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

// PROVEN: 6121481c IDENTICAL/UNPROVEN
// OFFSET: 0x006D9340/0x140397740
bool ClientMoveUpdateSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CE690/0x140388350
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

// PROVEN: ea503908 IDENTICAL/UNPROVEN
// OFFSET: 0x006B2E90/0x1403647A0
bool ClientMoveUpdateSwimBackSpeed::Handle() {
    if (ClientMoveUpdateSwimBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCF68.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCF68.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateSwimBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateSwimBackSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 4f7545f6 IDENTICAL/IDENTICAL
// OFFSET: 0x006D64D0/0x140393A10
ClientMoveUpdateSwimSpeed::ClientMoveUpdateSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5a22da8c IDENTICAL/UNPROVEN
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

// PROVEN: a22945c6 IDENTICAL/UNPROVEN
// OFFSET: 0x006D96F0/0x140397B70
bool ClientMoveUpdateSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CF350/0x1403892F0
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

// PROVEN: 64125e8c IDENTICAL/UNPROVEN
// OFFSET: 0x006A5AD0/0x140353A30
bool ClientMoveUpdateSwimSpeed::Handle() {
    if (ClientMoveUpdateSwimSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFC74.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFC74.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateSwimSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateSwimSpeed::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 231d119f IDENTICAL/IDENTICAL
// OFFSET: 0x006D9110/0x1403975E0
ClientMoveUpdateTeleport::ClientMoveUpdateTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 543cae81 IDENTICAL/UNPROVEN
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

// PROVEN: 90089e2b IDENTICAL/UNPROVEN
// OFFSET: 0x006D7780/0x140395430
bool ClientMoveUpdateTeleport::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006C9140/0x140380740
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

// PROVEN: 5b12440e IDENTICAL/UNPROVEN
// OFFSET: 0x006BC810/0x140370CD0
bool ClientMoveUpdateTeleport::Handle() {
    if (ClientMoveUpdateTeleport::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD57C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD57C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateTeleport*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateTeleport::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 5990ff89 IDENTICAL/IDENTICAL
// OFFSET: 0x006D90D0/0x140397590
ClientMoveUpdateTurnRate::ClientMoveUpdateTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f43c63fd IDENTICAL/UNPROVEN
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

// PROVEN: f3a52abe IDENTICAL/UNPROVEN
// OFFSET: 0x006D46B0/0x140391110
bool ClientMoveUpdateTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006D2000/0x14038D630
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

// PROVEN: 78d59a45 IDENTICAL/UNPROVEN
// OFFSET: 0x006C09B0/0x140375D80
bool ClientMoveUpdateTurnRate::Handle() {
    if (ClientMoveUpdateTurnRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD3E0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD3E0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateTurnRate*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateTurnRate::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b881f9d0 IDENTICAL/IDENTICAL
// OFFSET: 0x006DACF0/0x1403994F0
ClientMoveUpdateWalkSpeed::ClientMoveUpdateWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9e5d4024 IDENTICAL/UNPROVEN
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

// PROVEN: af938b5f IDENTICAL/UNPROVEN
// OFFSET: 0x006D3060/0x14038EF90
bool ClientMoveUpdateWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006CDEB0/0x1403876C0
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

// PROVEN: 88841251 IDENTICAL/UNPROVEN
// OFFSET: 0x006BCD10/0x1403712E0
bool ClientMoveUpdateWalkSpeed::Handle() {
    if (ClientMoveUpdateWalkSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE9B8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE9B8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveUpdateWalkSpeed*)>(reinterpret_cast<uintptr_t>(ClientMoveUpdateWalkSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 891d7d49 IDENTICAL/IDENTICAL
// OFFSET: 0x006B12A0/0x1403623B0
ClientPlaySpellVisual::ClientPlaySpellVisual(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9ed6cc67 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E20/0x14034F290
ClientPlaySpellVisual::~ClientPlaySpellVisual() {
}

// PROVEN: e55159e6 DIFFERENT/IDENTICAL
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

// PROVEN: ee100563 IDENTICAL/UNPROVEN
// OFFSET: 0x006B1690/0x1403628C0
bool ClientPlaySpellVisual::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1abee5af DIFFERENT/IDENTICAL
// OFFSET: 0x006A9470/0x1403586C0
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

// PROVEN: 3eeda410 IDENTICAL/UNPROVEN
// OFFSET: 0x006A9040/0x1403580F0
bool ClientPlaySpellVisual::Handle() {
    if (ClientPlaySpellVisual::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF728.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF728.m_left->Get();
        return reinterpret_cast<bool (*)(ClientPlaySpellVisual*)>(reinterpret_cast<uintptr_t>(ClientPlaySpellVisual::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 00e37547 IDENTICAL/IDENTICAL
// OFFSET: 0x006BADE0/0x14036EA80
ClientSetPlayHoverAnim::ClientSetPlayHoverAnim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 21c339e1 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2D70/0x1403502D0
ClientSetPlayHoverAnim::~ClientSetPlayHoverAnim() {
}

// PROVEN: cbac5179 IDENTICAL/IDENTICAL
// OFFSET: 0x006AAA40/0x14035A210
void ClientSetPlayHoverAnim::Unpack(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_playHoverAnim = bits.ReadBit();
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

// PROVEN: 3aac09d2 IDENTICAL/UNPROVEN
// OFFSET: 0x006ACE60/0x14035CEE0
bool ClientSetPlayHoverAnim::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 2693fc07 IDENTICAL/IDENTICAL
// OFFSET: 0x006A86A0/0x1403574C0
bool ClientSetPlayHoverAnim::Read(CDataStore* msg, bool full) {
    JamBitReader bits(msg);
    uint8_t* guid = reinterpret_cast<uint8_t*>(&this->m_guid);
    guid[4] = bits.ReadBit();
    guid[0] = bits.ReadBit();
    guid[1] = bits.ReadBit();
    this->m_playHoverAnim = bits.ReadBit();
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

// PROVEN: 29d303c0 IDENTICAL/UNPROVEN
// OFFSET: 0x006B45A0/0x140366520
bool ClientSetPlayHoverAnim::Handle() {
    if (ClientSetPlayHoverAnim::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE008.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE008.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSetPlayHoverAnim*)>(reinterpret_cast<uintptr_t>(ClientSetPlayHoverAnim::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: df10bd3f IDENTICAL/IDENTICAL
// OFFSET: 0x006C6070/0x14037C490
ClientSplineMoveCollisionDisable::ClientSplineMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 92d9a16f IDENTICAL/IDENTICAL
// OFFSET: 0x006A2270/0x14034F770
ClientSplineMoveCollisionDisable::~ClientSplineMoveCollisionDisable() {
}

// PROVEN: f6b924d5 IDENTICAL/IDENTICAL
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

// PROVEN: 69b75a97 IDENTICAL/UNPROVEN
// OFFSET: 0x006C78C0/0x14037E6F0
bool ClientSplineMoveCollisionDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e93d9869 IDENTICAL/IDENTICAL
// OFFSET: 0x006B6B10/0x140369670
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

// PROVEN: b2069e12 IDENTICAL/UNPROVEN
// OFFSET: 0x006C1500/0x140376B20
bool ClientSplineMoveCollisionDisable::Handle() {
    if (ClientSplineMoveCollisionDisable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE8F4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE8F4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveCollisionDisable*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveCollisionDisable::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 2f4563d4 IDENTICAL/IDENTICAL
// OFFSET: 0x006C1360/0x140376910
ClientSplineMoveCollisionEnable::ClientSplineMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a0161cb2 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2320/0x14034F850
ClientSplineMoveCollisionEnable::~ClientSplineMoveCollisionEnable() {
}

// PROVEN: 0912e50e IDENTICAL/IDENTICAL
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

// PROVEN: c5fea94d IDENTICAL/UNPROVEN
// OFFSET: 0x006C0A40/0x140375E40
bool ClientSplineMoveCollisionEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9c0f91c0 IDENTICAL/IDENTICAL
// OFFSET: 0x006AEC70/0x14035F4C0
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

// PROVEN: eec5858f IDENTICAL/UNPROVEN
// OFFSET: 0x006C1730/0x140376DE0
bool ClientSplineMoveCollisionEnable::Handle() {
    if (ClientSplineMoveCollisionEnable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEDA0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEDA0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveCollisionEnable*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveCollisionEnable::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 204ee9b4 IDENTICAL/IDENTICAL
// OFFSET: 0x006B5000/0x1403671E0
ClientSplineMoveGravityDisable::ClientSplineMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9557d82e IDENTICAL/IDENTICAL
// OFFSET: 0x006A2110/0x14034F560
ClientSplineMoveGravityDisable::~ClientSplineMoveGravityDisable() {
}

// PROVEN: cb471656 IDENTICAL/IDENTICAL
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

// PROVEN: 15b5e0e8 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF4A0/0x140374540
bool ClientSplineMoveGravityDisable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1aaec67b IDENTICAL/IDENTICAL
// OFFSET: 0x006B7BA0/0x14036AAB0
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

// PROVEN: b9d2dec5 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF130/0x140374100
bool ClientSplineMoveGravityDisable::Handle() {
    if (ClientSplineMoveGravityDisable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFECE8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFECE8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveGravityDisable*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveGravityDisable::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 4658b421 IDENTICAL/IDENTICAL
// OFFSET: 0x006B8430/0x14036B5C0
ClientSplineMoveGravityEnable::ClientSplineMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 4cbf6d11 IDENTICAL/IDENTICAL
// OFFSET: 0x006A28A0/0x14034FD20
ClientSplineMoveGravityEnable::~ClientSplineMoveGravityEnable() {
}

// PROVEN: d994e280 IDENTICAL/IDENTICAL
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

// PROVEN: 6fd27692 IDENTICAL/UNPROVEN
// OFFSET: 0x006CDE90/0x1403876A0
bool ClientSplineMoveGravityEnable::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 115f75a0 IDENTICAL/IDENTICAL
// OFFSET: 0x006B90B0/0x14036C560
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

// PROVEN: 02b21b9c IDENTICAL/UNPROVEN
// OFFSET: 0x006AFA20/0x1403604C0
bool ClientSplineMoveGravityEnable::Handle() {
    if (ClientSplineMoveGravityEnable::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFA40.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFA40.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveGravityEnable*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveGravityEnable::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 0aca9f61 IDENTICAL/IDENTICAL
// OFFSET: 0x006B9070/0x14036C520
ClientSplineMoveRoot::ClientSplineMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 5bc67e6e IDENTICAL/IDENTICAL
// OFFSET: 0x006A0FE0/0x14034E240
ClientSplineMoveRoot::~ClientSplineMoveRoot() {
}

// PROVEN: d6e714d2 IDENTICAL/IDENTICAL
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

// PROVEN: f3fd3b47 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4100/0x140365F20
bool ClientSplineMoveRoot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: db96f44a IDENTICAL/IDENTICAL
// OFFSET: 0x006AF250/0x14035FBB0
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

// PROVEN: c6e6ec4f IDENTICAL/UNPROVEN
// OFFSET: 0x006BE120/0x140372D20
bool ClientSplineMoveRoot::Handle() {
    if (ClientSplineMoveRoot::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF3C8.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF3C8.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveRoot*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveRoot::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 244ffb16 IDENTICAL/IDENTICAL
// OFFSET: 0x006C78D0/0x14037E700
ClientSplineMoveSetFeatherFall::ClientSplineMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c168da68 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2E30/0x140350390
ClientSplineMoveSetFeatherFall::~ClientSplineMoveSetFeatherFall() {
}

// PROVEN: 794cb1d1 IDENTICAL/IDENTICAL
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

// PROVEN: 166c60c5 IDENTICAL/UNPROVEN
// OFFSET: 0x006C8140/0x14037F0C0
bool ClientSplineMoveSetFeatherFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 77557831 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4D60/0x14037AA10
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

// PROVEN: d428e8f3 IDENTICAL/UNPROVEN
// OFFSET: 0x006AAD00/0x14035A590
bool ClientSplineMoveSetFeatherFall::Handle() {
    if (ClientSplineMoveSetFeatherFall::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE04C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE04C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetFeatherFall*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetFeatherFall::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 6a9c3b7f IDENTICAL/IDENTICAL
// OFFSET: 0x006C4F10/0x14037ABD0
ClientSplineMoveSetFlightBackSpeed::ClientSplineMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ce720705 IDENTICAL/IDENTICAL
// OFFSET: 0x006A19E0/0x14034EE20
ClientSplineMoveSetFlightBackSpeed::~ClientSplineMoveSetFlightBackSpeed() {
}

// PROVEN: ac9c85fe IDENTICAL/DIFFERENT
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

// PROVEN: 7eea3b7d IDENTICAL/UNPROVEN
// OFFSET: 0x006BE7E0/0x140373570
bool ClientSplineMoveSetFlightBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 8791e403 IDENTICAL/DIFFERENT
// OFFSET: 0x006BB0C0/0x14036EEC0
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

// PROVEN: 13b67573 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF4B0/0x140374550
bool ClientSplineMoveSetFlightBackSpeed::Handle() {
    if (ClientSplineMoveSetFlightBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFED40.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFED40.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetFlightBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetFlightBackSpeed::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 27cfb260 IDENTICAL/IDENTICAL
// OFFSET: 0x006B83E0/0x14036B570
ClientSplineMoveSetFlightSpeed::ClientSplineMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 141ae6d0 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2690/0x14034FB70
ClientSplineMoveSetFlightSpeed::~ClientSplineMoveSetFlightSpeed() {
}

// PROVEN: fae55185 IDENTICAL/IDENTICAL
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

// PROVEN: b11baa6d IDENTICAL/UNPROVEN
// OFFSET: 0x006C7940/0x14037E7C0
bool ClientSplineMoveSetFlightSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: be7f70ec IDENTICAL/IDENTICAL
// OFFSET: 0x006BB5A0/0x14036F4E0
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

// PROVEN: d177ced7 IDENTICAL/UNPROVEN
// OFFSET: 0x006AB3F0/0x14035AED0
bool ClientSplineMoveSetFlightSpeed::Handle() {
    if (ClientSplineMoveSetFlightSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCF04.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCF04.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetFlightSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetFlightSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 62ab9ec6 IDENTICAL/IDENTICAL
// OFFSET: 0x006BC240/0x140370570
ClientSplineMoveSetFlying::ClientSplineMoveSetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f639615e IDENTICAL/IDENTICAL
// OFFSET: 0x006A1E10/0x14034F280
ClientSplineMoveSetFlying::~ClientSplineMoveSetFlying() {
}

// PROVEN: 8b3e98ee IDENTICAL/IDENTICAL
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

// PROVEN: 0beef25a IDENTICAL/UNPROVEN
// OFFSET: 0x006BC850/0x140370D30
bool ClientSplineMoveSetFlying::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: f34d8c94 IDENTICAL/IDENTICAL
// OFFSET: 0x006BB990/0x14036F9E0
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

// PROVEN: ed917b1a IDENTICAL/UNPROVEN
// OFFSET: 0x006BF380/0x1403743E0
bool ClientSplineMoveSetFlying::Handle() {
    if (ClientSplineMoveSetFlying::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDEB4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDEB4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetFlying*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetFlying::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 3227cbbe IDENTICAL/IDENTICAL
// OFFSET: 0x006C17F0/0x140376E40
ClientSplineMoveSetHover::ClientSplineMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: be553cba IDENTICAL/IDENTICAL
// OFFSET: 0x006A25D0/0x14034FB20
ClientSplineMoveSetHover::~ClientSplineMoveSetHover() {
}

// PROVEN: cea3fa0f IDENTICAL/IDENTICAL
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

// PROVEN: cf565dbf IDENTICAL/UNPROVEN
// OFFSET: 0x006BBAF0/0x14036FBA0
bool ClientSplineMoveSetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 869e18aa IDENTICAL/IDENTICAL
// OFFSET: 0x006A3350/0x140350AE0
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

// PROVEN: 24e435d0 IDENTICAL/UNPROVEN
// OFFSET: 0x006AAF20/0x14035A870
bool ClientSplineMoveSetHover::Handle() {
    if (ClientSplineMoveSetHover::s_handler) {
        uint32_t right = g_jamHandlerKey_CFCFE0.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFCFE0.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetHover*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetHover::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 2f613032 IDENTICAL/IDENTICAL
// OFFSET: 0x006C87A0/0x14037F870
ClientSplineMoveSetLandWalk::ClientSplineMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8cc2c1e3 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2D80/0x1403502E0
ClientSplineMoveSetLandWalk::~ClientSplineMoveSetLandWalk() {
}

// PROVEN: a54d70e8 IDENTICAL/IDENTICAL
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

// PROVEN: fdc249e6 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4560/0x14037A410
bool ClientSplineMoveSetLandWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d7f46ac1 IDENTICAL/IDENTICAL
// OFFSET: 0x006B9E40/0x14036D6A0
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

// PROVEN: 4df74643 IDENTICAL/UNPROVEN
// OFFSET: 0x006BF6F0/0x140374820
bool ClientSplineMoveSetLandWalk::Handle() {
    if (ClientSplineMoveSetLandWalk::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD434.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD434.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetLandWalk*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetLandWalk::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 1a1d9a28 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8320/0x14037F320
ClientSplineMoveSetNormalFall::ClientSplineMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: c3ab0e0f IDENTICAL/IDENTICAL
// OFFSET: 0x006A2D60/0x1403502C0
ClientSplineMoveSetNormalFall::~ClientSplineMoveSetNormalFall() {
}

// PROVEN: 3ec36359 IDENTICAL/IDENTICAL
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

// PROVEN: dc8bc00c IDENTICAL/UNPROVEN
// OFFSET: 0x006CCA90/0x140385BE0
bool ClientSplineMoveSetNormalFall::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 9c33d84e IDENTICAL/IDENTICAL
// OFFSET: 0x006B5280/0x140367510
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

// PROVEN: 3fa2c3ca IDENTICAL/UNPROVEN
// OFFSET: 0x006AEED0/0x14035F7F0
bool ClientSplineMoveSetNormalFall::Handle() {
    if (ClientSplineMoveSetNormalFall::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDC44.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDC44.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetNormalFall*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetNormalFall::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: a1f35500 IDENTICAL/IDENTICAL
// OFFSET: 0x006B0860/0x1403616B0
ClientSplineMoveSetPitchRate::ClientSplineMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ea60a0f8 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1280/0x14034E550
ClientSplineMoveSetPitchRate::~ClientSplineMoveSetPitchRate() {
}

// PROVEN: c9acd03f IDENTICAL/DIFFERENT
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

// PROVEN: d6abb68b IDENTICAL/UNPROVEN
// OFFSET: 0x006CDE40/0x140387690
bool ClientSplineMoveSetPitchRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a31b4b36 IDENTICAL/DIFFERENT
// OFFSET: 0x006BB260/0x14036F0C0
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

// PROVEN: fe57aaaf IDENTICAL/UNPROVEN
// OFFSET: 0x006AD0C0/0x14035D150
bool ClientSplineMoveSetPitchRate::Handle() {
    if (ClientSplineMoveSetPitchRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEFF4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEFF4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetPitchRate*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetPitchRate::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: b6bf78ca IDENTICAL/IDENTICAL
// OFFSET: 0x006D3070/0x14038EFA0
ClientSplineMoveSetRunBackSpeed::ClientSplineMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: a816d7f6 IDENTICAL/IDENTICAL
// OFFSET: 0x006A17B0/0x14034EB30
ClientSplineMoveSetRunBackSpeed::~ClientSplineMoveSetRunBackSpeed() {
}

// PROVEN: 951fe937 IDENTICAL/DIFFERENT
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

// PROVEN: 7023e344 IDENTICAL/UNPROVEN
// OFFSET: 0x006B39A0/0x140365580
bool ClientSplineMoveSetRunBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 28137f22 IDENTICAL/DIFFERENT
// OFFSET: 0x006B0900/0x140361740
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

// PROVEN: 57ab81fc IDENTICAL/UNPROVEN
// OFFSET: 0x006B7B60/0x14036AA50
bool ClientSplineMoveSetRunBackSpeed::Handle() {
    if (ClientSplineMoveSetRunBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD200.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD200.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetRunBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetRunBackSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 32f0890d IDENTICAL/IDENTICAL
// OFFSET: 0x006BC860/0x140370D40
ClientSplineMoveSetRunMode::ClientSplineMoveSetRunMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 9c2fec97 IDENTICAL/IDENTICAL
// OFFSET: 0x006A17A0/0x14034EB20
ClientSplineMoveSetRunMode::~ClientSplineMoveSetRunMode() {
}

// PROVEN: 0d8b454b IDENTICAL/IDENTICAL
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

// PROVEN: e09d74a2 IDENTICAL/UNPROVEN
// OFFSET: 0x006B4350/0x140366210
bool ClientSplineMoveSetRunMode::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 4a0e7628 IDENTICAL/IDENTICAL
// OFFSET: 0x006AEB10/0x14035F300
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

// PROVEN: 7c071132 IDENTICAL/UNPROVEN
// OFFSET: 0x006B5230/0x1403674B0
bool ClientSplineMoveSetRunMode::Handle() {
    if (ClientSplineMoveSetRunMode::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEF10.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEF10.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetRunMode*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetRunMode::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 77c394c1 IDENTICAL/IDENTICAL
// OFFSET: 0x006D4670/0x1403910D0
ClientSplineMoveSetRunSpeed::ClientSplineMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 677f5624 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0EF0/0x14034DFD0
ClientSplineMoveSetRunSpeed::~ClientSplineMoveSetRunSpeed() {
}

// PROVEN: 7b1aa95e IDENTICAL/DIFFERENT
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

// PROVEN: 5b9cc9a0 IDENTICAL/UNPROVEN
// OFFSET: 0x006D4FA0/0x140391DE0
bool ClientSplineMoveSetRunSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 40d5de4e IDENTICAL/DIFFERENT
// OFFSET: 0x006CA8D0/0x1403829A0
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

// PROVEN: ddb6bc83 IDENTICAL/UNPROVEN
// OFFSET: 0x006B8F00/0x14036C350
bool ClientSplineMoveSetRunSpeed::Handle() {
    if (ClientSplineMoveSetRunSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF000.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF000.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetRunSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetRunSpeed::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 52a7b75b IDENTICAL/IDENTICAL
// OFFSET: 0x006B0AA0/0x140361940
ClientSplineMoveSetSwimBackSpeed::ClientSplineMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: bcd6c464 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2400/0x14034F910
ClientSplineMoveSetSwimBackSpeed::~ClientSplineMoveSetSwimBackSpeed() {
}

// PROVEN: 6c822d6e IDENTICAL/DIFFERENT
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

// PROVEN: a196ce27 IDENTICAL/UNPROVEN
// OFFSET: 0x006D45C0/0x140390FC0
bool ClientSplineMoveSetSwimBackSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1ba1f28c IDENTICAL/DIFFERENT
// OFFSET: 0x006C1590/0x140376BE0
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

// PROVEN: f6ca63ef IDENTICAL/UNPROVEN
// OFFSET: 0x006BC7D0/0x140370C70
bool ClientSplineMoveSetSwimBackSpeed::Handle() {
    if (ClientSplineMoveSetSwimBackSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF204.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF204.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetSwimBackSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetSwimBackSpeed::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 3549e454 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8720/0x14037F7F0
ClientSplineMoveSetSwimSpeed::ClientSplineMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: eae6c780 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1130/0x14034E3D0
ClientSplineMoveSetSwimSpeed::~ClientSplineMoveSetSwimSpeed() {
}

// PROVEN: 358e7f60 IDENTICAL/DIFFERENT
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

// PROVEN: fdfb8d2d IDENTICAL/UNPROVEN
// OFFSET: 0x006CA480/0x140382450
bool ClientSplineMoveSetSwimSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d608e29c IDENTICAL/DIFFERENT
// OFFSET: 0x006B87B0/0x14036BA10
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

// PROVEN: 46cdd8ed IDENTICAL/UNPROVEN
// OFFSET: 0x006BF8D0/0x140374A90
bool ClientSplineMoveSetSwimSpeed::Handle() {
    if (ClientSplineMoveSetSwimSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC7C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC7C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetSwimSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetSwimSpeed::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 4d15eaf7 IDENTICAL/IDENTICAL
// OFFSET: 0x006C8660/0x14037F7A0
ClientSplineMoveSetTurnRate::ClientSplineMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 68169eae IDENTICAL/IDENTICAL
// OFFSET: 0x006A27B0/0x14034FCB0
ClientSplineMoveSetTurnRate::~ClientSplineMoveSetTurnRate() {
}

// PROVEN: 5879419e IDENTICAL/IDENTICAL
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

// PROVEN: cc5f3ea6 IDENTICAL/UNPROVEN
// OFFSET: 0x006D0D20/0x14038B8F0
bool ClientSplineMoveSetTurnRate::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: a66529ef IDENTICAL/IDENTICAL
// OFFSET: 0x006B9CA0/0x14036D4A0
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

// PROVEN: ac933970 IDENTICAL/UNPROVEN
// OFFSET: 0x006B35A0/0x140365070
bool ClientSplineMoveSetTurnRate::Handle() {
    if (ClientSplineMoveSetTurnRate::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD23C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD23C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetTurnRate*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetTurnRate::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: ebe7f92c IDENTICAL/IDENTICAL
// OFFSET: 0x006CB4C0/0x140383AC0
ClientSplineMoveSetWalkMode::ClientSplineMoveSetWalkMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 48fff467 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1FF0/0x14034F460
ClientSplineMoveSetWalkMode::~ClientSplineMoveSetWalkMode() {
}

// PROVEN: 33b89b23 IDENTICAL/IDENTICAL
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

// PROVEN: 51df3a33 IDENTICAL/UNPROVEN
// OFFSET: 0x006AE820/0x14035EF40
bool ClientSplineMoveSetWalkMode::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: ac54b698 IDENTICAL/IDENTICAL
// OFFSET: 0x006AA340/0x140359940
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

// PROVEN: 2fa46f1a IDENTICAL/UNPROVEN
// OFFSET: 0x006BE880/0x140373630
bool ClientSplineMoveSetWalkMode::Handle() {
    if (ClientSplineMoveSetWalkMode::s_handler) {
        uint32_t right = g_jamHandlerKey_CFD934.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFD934.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetWalkMode*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetWalkMode::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 13d04964 IDENTICAL/IDENTICAL
// OFFSET: 0x006D0CE0/0x14038B8B0
ClientSplineMoveSetWalkSpeed::ClientSplineMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 1a926bb3 IDENTICAL/IDENTICAL
// OFFSET: 0x006A1DF0/0x14034F260
ClientSplineMoveSetWalkSpeed::~ClientSplineMoveSetWalkSpeed() {
}

// PROVEN: f271b2a0 IDENTICAL/DIFFERENT
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

// PROVEN: 69ada395 IDENTICAL/UNPROVEN
// OFFSET: 0x006C4F50/0x14037AC10
bool ClientSplineMoveSetWalkSpeed::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: d2fe45a9 IDENTICAL/DIFFERENT
// OFFSET: 0x006ADFC0/0x14035E510
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

// PROVEN: 5249a1e9 IDENTICAL/UNPROVEN
// OFFSET: 0x006B31B0/0x140364BB0
bool ClientSplineMoveSetWalkSpeed::Handle() {
    if (ClientSplineMoveSetWalkSpeed::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC6C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC6C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetWalkSpeed*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetWalkSpeed::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 2bc2fc1e IDENTICAL/IDENTICAL
// OFFSET: 0x006C4860/0x14037A7A0
ClientSplineMoveSetWaterWalk::ClientSplineMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 17714037 IDENTICAL/IDENTICAL
// OFFSET: 0x006A26B0/0x14034FB90
ClientSplineMoveSetWaterWalk::~ClientSplineMoveSetWaterWalk() {
}

// PROVEN: bc1968fb IDENTICAL/IDENTICAL
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

// PROVEN: 1fb8e29e IDENTICAL/UNPROVEN
// OFFSET: 0x006B20D0/0x140363610
bool ClientSplineMoveSetWaterWalk::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 1556365e IDENTICAL/IDENTICAL
// OFFSET: 0x006AE840/0x14035EF60
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

// PROVEN: 2446107d IDENTICAL/UNPROVEN
// OFFSET: 0x006A6D20/0x140355260
bool ClientSplineMoveSetWaterWalk::Handle() {
    if (ClientSplineMoveSetWaterWalk::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEA54.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEA54.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveSetWaterWalk*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveSetWaterWalk::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: b138a1cd IDENTICAL/IDENTICAL
// OFFSET: 0x006C4760/0x14037A670
ClientSplineMoveStartSwim::ClientSplineMoveStartSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3e6e6ef6 IDENTICAL/IDENTICAL
// OFFSET: 0x006A19C0/0x14034EDF0
ClientSplineMoveStartSwim::~ClientSplineMoveStartSwim() {
}

// PROVEN: 489e74f5 IDENTICAL/IDENTICAL
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

// PROVEN: e7468ea3 IDENTICAL/UNPROVEN
// OFFSET: 0x006BEB60/0x1403739C0
bool ClientSplineMoveStartSwim::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 6ac04444 IDENTICAL/IDENTICAL
// OFFSET: 0x006B39B0/0x140365590
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

// PROVEN: 9974b8dd IDENTICAL/UNPROVEN
// OFFSET: 0x006BB560/0x14036F480
bool ClientSplineMoveStartSwim::Handle() {
    if (ClientSplineMoveStartSwim::s_handler) {
        uint32_t right = g_jamHandlerKey_CFFCD4.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFFCD4.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveStartSwim*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveStartSwim::s_handler) - (right ^ left))(this);
    }

    return false;
}

// PROVEN: 514eb121 IDENTICAL/IDENTICAL
// OFFSET: 0x006C48C0/0x14037A800
ClientSplineMoveStopSwim::ClientSplineMoveStopSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 6780ea66 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0E20/0x14034DEC0
ClientSplineMoveStopSwim::~ClientSplineMoveStopSwim() {
}

// PROVEN: cd1ac6e9 IDENTICAL/IDENTICAL
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

// PROVEN: e6d4bd03 IDENTICAL/UNPROVEN
// OFFSET: 0x006D59B0/0x140392CE0
bool ClientSplineMoveStopSwim::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 838ca1a4 IDENTICAL/IDENTICAL
// OFFSET: 0x006C4F60/0x14037AC20
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

// PROVEN: f8c142c7 IDENTICAL/UNPROVEN
// OFFSET: 0x006B1AA0/0x140362E10
bool ClientSplineMoveStopSwim::Handle() {
    if (ClientSplineMoveStopSwim::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE9DC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE9DC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveStopSwim*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveStopSwim::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 14b17a7b IDENTICAL/IDENTICAL
// OFFSET: 0x006BDF60/0x140372AD0
ClientSplineMoveUnroot::ClientSplineMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: ffb9a264 IDENTICAL/IDENTICAL
// OFFSET: 0x006A22C0/0x14034F790
ClientSplineMoveUnroot::~ClientSplineMoveUnroot() {
}

// PROVEN: 710d5c37 IDENTICAL/IDENTICAL
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

// PROVEN: 25438ab4 IDENTICAL/UNPROVEN
// OFFSET: 0x006D46C0/0x140391120
bool ClientSplineMoveUnroot::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 28d1cf04 IDENTICAL/IDENTICAL
// OFFSET: 0x006C1830/0x140376E80
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

// PROVEN: 729e8102 IDENTICAL/UNPROVEN
// OFFSET: 0x006BBD60/0x14036FED0
bool ClientSplineMoveUnroot::Handle() {
    if (ClientSplineMoveUnroot::s_handler) {
        uint32_t right = g_jamHandlerKey_CFE198.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFE198.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveUnroot*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveUnroot::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: f157fc14 IDENTICAL/IDENTICAL
// OFFSET: 0x006B2830/0x140363EE0
ClientSplineMoveUnsetFlying::ClientSplineMoveUnsetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 8895cb33 IDENTICAL/IDENTICAL
// OFFSET: 0x006A0F60/0x14034E1C0
ClientSplineMoveUnsetFlying::~ClientSplineMoveUnsetFlying() {
}

// PROVEN: 42121fa3 IDENTICAL/IDENTICAL
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

// PROVEN: 6e1dc233 IDENTICAL/UNPROVEN
// OFFSET: 0x006CDEA0/0x1403876B0
bool ClientSplineMoveUnsetFlying::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 42a27b68 IDENTICAL/IDENTICAL
// OFFSET: 0x006B6560/0x140368F30
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

// PROVEN: c6c6c13b IDENTICAL/UNPROVEN
// OFFSET: 0x006BAF00/0x14036EC60
bool ClientSplineMoveUnsetFlying::Handle() {
    if (ClientSplineMoveUnsetFlying::s_handler) {
        uint32_t right = g_jamHandlerKey_CFDE64.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFDE64.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveUnsetFlying*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveUnsetFlying::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: f78c2fdb IDENTICAL/IDENTICAL
// OFFSET: 0x006C5FF0/0x14037C410
ClientSplineMoveUnsetHover::ClientSplineMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 412abe25 IDENTICAL/IDENTICAL
// OFFSET: 0x006A26A0/0x14034FB80
ClientSplineMoveUnsetHover::~ClientSplineMoveUnsetHover() {
}

// PROVEN: 91ad102c IDENTICAL/IDENTICAL
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

// PROVEN: a2603f0f IDENTICAL/UNPROVEN
// OFFSET: 0x006D7200/0x140394E70
bool ClientSplineMoveUnsetHover::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3f30408c IDENTICAL/IDENTICAL
// OFFSET: 0x006CABC0/0x140382D40
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

// PROVEN: 58f46d00 IDENTICAL/UNPROVEN
// OFFSET: 0x006ABC30/0x14035B940
bool ClientSplineMoveUnsetHover::Handle() {
    if (ClientSplineMoveUnsetHover::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF8FC.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF8FC.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSplineMoveUnsetHover*)>(reinterpret_cast<uintptr_t>(ClientSplineMoveUnsetHover::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 3cf6ca4e IDENTICAL/IDENTICAL
// OFFSET: 0x006C98B0/0x1403812F0
ClientSuppressNPCGreetings::ClientSuppressNPCGreetings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 00f1f479 IDENTICAL/IDENTICAL
// OFFSET: 0x006A2370/0x14034F890
ClientSuppressNPCGreetings::~ClientSuppressNPCGreetings() {
}

// PROVEN: 8d515cf1 IDENTICAL/IDENTICAL
// OFFSET: 0x006BE600/0x140373390
void ClientSuppressNPCGreetings::Unpack(CDataStore* msg, bool full) {
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

// PROVEN: 1e0b8949 IDENTICAL/UNPROVEN
// OFFSET: 0x006B7650/0x14036A4C0
bool ClientSuppressNPCGreetings::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: e24bafa7 IDENTICAL/IDENTICAL
// OFFSET: 0x006AE260/0x14035E7D0
bool ClientSuppressNPCGreetings::Read(CDataStore* msg, bool full) {
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

// PROVEN: c20325d9 IDENTICAL/UNPROVEN
// OFFSET: 0x006B9030/0x14036C4C0
bool ClientSuppressNPCGreetings::Handle() {
    if (ClientSuppressNPCGreetings::s_handler) {
        uint32_t right = g_jamHandlerKey_CFEC9C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFEC9C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientSuppressNPCGreetings*)>(reinterpret_cast<uintptr_t>(ClientSuppressNPCGreetings::s_handler) - (left - right))(this);
    }

    return false;
}

// OFFSET: 0x006A2C10/0x140350130
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

// STATIC: 0x00E4D834/0x140D36A40
bool (*ClientMoveSetCompoundState::s_handler)(ClientMoveSetCompoundState* msg);

// PROVEN: 087b8667 IDENTICAL/IDENTICAL
// OFFSET: 0x006F07E0/0x1403B61B0
ClientMoveSetCompoundState::ClientMoveSetCompoundState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: 3da56775 IDENTICAL/IDENTICAL
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
    this->m_changes.SetCount(bits.ReadBits<23>());

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];
        change.m_hasVehicleRecId = bits.ReadBit() != 0;
        change.m_hasSpeed = bits.ReadBit() != 0;
        change.m_hasKnockBack = bits.ReadBit() != 0;
        change.m_hasCollisionHeight = bits.ReadBit() != 0;

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

// PROVEN: b989a3ad IDENTICAL/UNPROVEN
// OFFSET: 0x006ECD60/0x1403B1170
bool ClientMoveSetCompoundState::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// OFFSET: 0x006E8760/0x1403AAFB0
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
    uint32_t count = bits.ReadBits<23>();

    if (msg->m_size - msg->m_read < ((4 * count + 7) >> 3) + 6 * count) {
        return false;
    }

    this->m_changes.SetCount(count);

    for (uint32_t i = 0; i < this->m_changes.Count(); i++) {
        JamMoveStateChange& change = this->m_changes[i];
        change.m_hasVehicleRecId = bits.ReadBit() != 0;
        change.m_hasSpeed = bits.ReadBit() != 0;
        change.m_hasKnockBack = bits.ReadBit() != 0;
        change.m_hasCollisionHeight = bits.ReadBit() != 0;

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

// PROVEN: a67a8836 IDENTICAL/UNPROVEN
// OFFSET: 0x006AA140/0x140359730
bool ClientMoveSetCompoundState::Handle() {
    if (ClientMoveSetCompoundState::s_handler) {
        uint32_t right = g_jamHandlerKey_CFF304.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFF304.m_left->Get();
        return reinterpret_cast<bool (*)(ClientMoveSetCompoundState*)>(reinterpret_cast<uintptr_t>(ClientMoveSetCompoundState::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: ab482013 IDENTICAL/NONE
// OFFSET: 0x006C0480
void JamMoveStateChange::operator=(const JamMoveStateChange& source) {
    this->m_hasCollisionHeight = source.m_hasCollisionHeight;
    this->m_hasKnockBack = source.m_hasKnockBack;

    if (this->m_hasCollisionHeight) {
        this->m_collisionHeight.m_height = source.m_collisionHeight.m_height;
        this->m_collisionHeight.m_reason = source.m_collisionHeight.m_reason;
    }

    this->m_ackIndex = source.m_ackIndex;

    if (this->m_hasKnockBack) {
        this->m_knockBack.m_direction = source.m_knockBack.m_direction;
        this->m_knockBack.m_horizontalSpeed = source.m_knockBack.m_horizontalSpeed;
        this->m_knockBack.m_verticalSpeed = source.m_knockBack.m_verticalSpeed;
    }

    this->m_hasVehicleRecId = source.m_hasVehicleRecId;
    this->m_messageId = source.m_messageId;
    this->m_hasSpeed = source.m_hasSpeed;

    if (this->m_hasSpeed) {
        this->m_speed = source.m_speed;
    }

    if (this->m_hasVehicleRecId) {
        this->m_vehicleRecId = source.m_vehicleRecId;
    }
}
