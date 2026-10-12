#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "Engine/Source/Base/CDataStore.h"

// OFFSET: [0x00670D50/0x140319050]
PlayerChangeSeatsOnControlledVehicle::PlayerChangeSeatsOnControlledVehicle() {
}

// OFFSET: [0x00673BA0]
PlayerChangeSeatsOnControlledVehicle::~PlayerChangeSeatsOnControlledVehicle() {
}

// OFFSET: 0x00678A60/0x140321210
void PlayerChangeSeatsOnControlledVehicle::Put(CDataStore* msg) {
    const uint8_t* vehicleGuid = reinterpret_cast<const uint8_t*>(&this->m_vehicleGuid);
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7310));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_seat);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(vehicleGuid[2] != 0);
    bits.WriteBit(vehicleGuid[4] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(vehicleGuid[7] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(vehicleGuid[6] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(vehicleGuid[5] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(vehicleGuid[0] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(vehicleGuid[1] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(vehicleGuid[3] != 0);
    bits.WriteBit(guid[3] != 0);

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    bits.Flush();

    if (vehicleGuid[6]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (vehicleGuid[1]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[1] ^ 1));
    }

    if (vehicleGuid[2]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (vehicleGuid[5]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[5] ^ 1));
    }

    if (vehicleGuid[3]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[3] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (vehicleGuid[0]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[0] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (vehicleGuid[4]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[4] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (vehicleGuid[7]) {
        msg->Put(static_cast<uint8_t>(vehicleGuid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x006749F0]
PlayerDismissControlledVehicle::PlayerDismissControlledVehicle() {
}

// OFFSET: [0x006749A0]
PlayerDismissControlledVehicle::~PlayerDismissControlledVehicle() {
}

// OFFSET: 0x00675860/0x14031D8A0
void PlayerDismissControlledVehicle::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3218));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[5] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x006744C0/0x140318450]
PlayerMoveChangeTransport::PlayerMoveChangeTransport() {
}

// OFFSET: [0x00670E00/0x140319150]
PlayerMoveChangeTransport::~PlayerMoveChangeTransport() {
}

// OFFSET: 0x00677150/0x14031F5D0
void PlayerMoveChangeTransport::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3102));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x006747B0/0x1403185B0]
PlayerMoveFallLand::PlayerMoveFallLand() {
}

// OFFSET: [0x006733D0/0x14031B820]
PlayerMoveFallLand::~PlayerMoveFallLand() {
}

// OFFSET: 0x00686660/0x140331080
void PlayerMoveFallLand::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x380A));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[7] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673350/0x14031C520]
PlayerMoveFallReset::PlayerMoveFallReset() {
}

// OFFSET: [0x00673860/0x140318600]
PlayerMoveFallReset::~PlayerMoveFallReset() {
}

// OFFSET: 0x00681120/0x14032B160
void PlayerMoveFallReset::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x310A));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00673B30]
PlayerMoveFeatherFallAck::PlayerMoveFeatherFallAck() {
}

// OFFSET: [0x00670DB0]
PlayerMoveFeatherFallAck::~PlayerMoveFeatherFallAck() {
}

// OFFSET: 0x00684370/0x14032E700
void PlayerMoveFeatherFallAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3110));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[4] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[0] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00670E90]
PlayerMoveFlightBackSpeedChangeAck::PlayerMoveFlightBackSpeedChangeAck() {
}

// OFFSET: [0x00673E20]
PlayerMoveFlightBackSpeedChangeAck::~PlayerMoveFlightBackSpeedChangeAck() {
}

// OFFSET: 0x0067A620/0x1403231D0
void PlayerMoveFlightBackSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x310E));
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_speed);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_facing);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);
        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);
        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_data.m_status.m_hasFall) {
        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
        msg->Put(this->m_data.m_status.m_fall.m_time);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }
}

// OFFSET: [0x00674A90]
PlayerMoveFlightSpeedChangeAck::PlayerMoveFlightSpeedChangeAck() {
}

// OFFSET: [0x00674B50]
PlayerMoveFlightSpeedChangeAck::~PlayerMoveFlightSpeedChangeAck() {
}

// OFFSET: 0x00685310/0x14032F990
void PlayerMoveFlightSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7314));
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_speed);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[1] != 0);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[4] != 0);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
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

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_facing);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);
        msg->Put(this->m_data.m_status.m_transport.m_time);
        msg->Put(this->m_data.m_status.m_transport.m_position.y);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_hasFall) {
        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_data.m_status.m_fall.m_time);
        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }
}

// OFFSET: [0x006749B0]
PlayerMoveGravityDisableAck::PlayerMoveGravityDisableAck() {
}

// OFFSET: [0x00673D00]
PlayerMoveGravityDisableAck::~PlayerMoveGravityDisableAck() {
}

// OFFSET: 0x006780D0/0x1403206C0
void PlayerMoveGravityDisableAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3118));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[1] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00674E90]
PlayerMoveGravityEnableAck::PlayerMoveGravityEnableAck() {
}

// OFFSET: [0x00673E70]
PlayerMoveGravityEnableAck::~PlayerMoveGravityEnableAck() {
}

// OFFSET: 0x00684850/0x14032ECA0
void PlayerMoveGravityEnableAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x700A));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[0] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.y);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00673190/0x14031B570]
PlayerMoveHeartbeat::PlayerMoveHeartbeat() {
}

// OFFSET: [0x00674D00/0x140318CD0]
PlayerMoveHeartbeat::~PlayerMoveHeartbeat() {
}

// OFFSET: 0x00687AB0/0x140332790
void PlayerMoveHeartbeat::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3914));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00670E60]
PlayerMoveHoverAck::PlayerMoveHoverAck() {
}

// OFFSET: [0x00673890]
PlayerMoveHoverAck::~PlayerMoveHoverAck() {
}

// OFFSET: 0x0067E070/0x140327410
void PlayerMoveHoverAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3318));
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[3] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00673E80/0x14031C4F0]
PlayerMoveJump::PlayerMoveJump() {
}

// OFFSET: [0x00670D90/0x140319090]
PlayerMoveJump::~PlayerMoveJump() {
}

// OFFSET: 0x006857F0/0x14032FF40
void PlayerMoveJump::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7A06));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00674AF0]
PlayerMoveKnockBackAck::PlayerMoveKnockBackAck() {
}

// OFFSET: [0x00673D70]
PlayerMoveKnockBackAck::~PlayerMoveKnockBackAck() {
}

// OFFSET: 0x00680B20/0x14032A500
void PlayerMoveKnockBackAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x721C));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    bits.Flush();

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_seat);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673EA0]
PlayerMoveNotActiveMover::PlayerMoveNotActiveMover() {
}

// OFFSET: [0x006747E0/0x140318600]
PlayerMoveNotActiveMover::~PlayerMoveNotActiveMover() {
}

// OFFSET: 0x006815E0
void PlayerMoveNotActiveMover::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7A1A));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveFlags == 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[3] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
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

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673B50]
PlayerMovePitchRateChangeAck::PlayerMovePitchRateChangeAck() {
}

// OFFSET: [0x00673E30]
PlayerMovePitchRateChangeAck::~PlayerMovePitchRateChangeAck() {
}

// OFFSET: 0x0067DB90/0x140326E50
void PlayerMovePitchRateChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3100));
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (this->m_data.m_status.m_hasTransport) {
        msg->Put(this->m_data.m_status.m_transport.m_facing);
        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);
        msg->Put(this->m_data.m_status.m_transport.m_time);
        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_hasFall) {
        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_data.m_status.m_fall.m_time);
        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }
}

// OFFSET: [0x00673160]
PlayerMoveRootAck::PlayerMoveRootAck() {
}

// OFFSET: [0x00673450]
PlayerMoveRootAck::~PlayerMoveRootAck() {
}

// OFFSET: 0x00686FF0/0x140331BD0
void PlayerMoveRootAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x701E));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[5] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_seat);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x006739F0]
PlayerMoveRunBackSpeedChangeAck::PlayerMoveRunBackSpeedChangeAck() {
}

// OFFSET: [0x006743C0]
PlayerMoveRunBackSpeedChangeAck::~PlayerMoveRunBackSpeedChangeAck() {
}

// OFFSET: 0x00683E80/0x14032E110
void PlayerMoveRunBackSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3216));
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_data.m_status.m_hasFall) {
        msg->Put(this->m_data.m_status.m_fall.m_time);

        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
    }

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);
        msg->Put(this->m_data.m_status.m_transport.m_facing);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }
}

// OFFSET: [0x00670F20]
PlayerMoveRunSpeedChangeAck::PlayerMoveRunSpeedChangeAck() {
}

// OFFSET: [0x00670DA0]
PlayerMoveRunSpeedChangeAck::~PlayerMoveRunSpeedChangeAck() {
}

// OFFSET: 0x00679180/0x1403219D0
void PlayerMoveRunSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7818));
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(guid[3] != 0);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_data.m_status.m_hasTransport) {
        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);
        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_facing);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_data.m_status.m_hasFall) {
        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);

        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_data.m_status.m_fall.m_time);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }
}

// OFFSET: [0x00674CE0]
PlayerMoveSetCanFlyAck::PlayerMoveSetCanFlyAck() {
}

// OFFSET: [0x00673DE0]
PlayerMoveSetCanFlyAck::~PlayerMoveSetCanFlyAck() {
}

// OFFSET: 0x0067A140/0x140322C20
void PlayerMoveSetCanFlyAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x790C));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00673AD0]
PlayerMoveSetCanTransitionBetweenSwimAndFlyAck::PlayerMoveSetCanTransitionBetweenSwimAndFlyAck() {
}

// OFFSET: [0x00674D10]
PlayerMoveSetCanTransitionBetweenSwimAndFlyAck::~PlayerMoveSetCanTransitionBetweenSwimAndFlyAck() {
}

// OFFSET: 0x0067F6A0/0x140328CB0
void PlayerMoveSetCanTransitionBetweenSwimAndFlyAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3014));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[7] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00673140/0x14031B510]
PlayerMoveSetCollisionHeightAck::PlayerMoveSetCollisionHeightAck() {
}

// OFFSET: [0x006749E0/0x1403187E0]
PlayerMoveSetCollisionHeightAck::~PlayerMoveSetCollisionHeightAck() {
}

// OFFSET: 0x0067E540/0x1403279D0
void PlayerMoveSetCollisionHeightAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7114));
    msg->Put(this->m_height);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBits<2>(this->m_reason);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[0] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_position.z);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00673370/0x14031B7B0]
PlayerMoveSetFacing::PlayerMoveSetFacing() {
}

// OFFSET: [0x006733F0/0x14031B840]
PlayerMoveSetFacing::~PlayerMoveSetFacing() {
}

// OFFSET: 0x006785A0/0x140320C80
void PlayerMoveSetFacing::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7914));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00673A70/0x14031BF40]
PlayerMoveSetFly::PlayerMoveSetFly() {
}

// OFFSET: [0x006749D0/0x140318750]
PlayerMoveSetFly::~PlayerMoveSetFly() {
}

// OFFSET: 0x00682DE0/0x14032CDE0
void PlayerMoveSetFly::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x720E));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673120/0x14031B4E0]
PlayerMoveSetPitch::PlayerMoveSetPitch() {
}

// OFFSET: [0x00673670/0x14031BAF0]
PlayerMoveSetPitch::~PlayerMoveSetPitch() {
}

// OFFSET: 0x0067D210/0x140326300
void PlayerMoveSetPitch::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7312));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[5] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673D80]
PlayerMoveSetRunMode::PlayerMoveSetRunMode() {
}

// OFFSET: [0x00674410]
PlayerMoveSetRunMode::~PlayerMoveSetRunMode() {
}

// OFFSET: 0x0067B430/0x140324130
void PlayerMoveSetRunMode::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x791A));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00670D30/0x140319020]
PlayerMoveSetVehicleRecIdAck::PlayerMoveSetVehicleRecIdAck() {
}

// OFFSET: [0x00673850]
PlayerMoveSetVehicleRecIdAck::~PlayerMoveSetVehicleRecIdAck() {
}

// OFFSET: 0x00685CB0/0x1403304F0
void PlayerMoveSetVehicleRecIdAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3108));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_vehicleRecId);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[4] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x006736A0]
PlayerMoveSetWalkMode::PlayerMoveSetWalkMode() {
}

// OFFSET: [0x006738A0]
PlayerMoveSetWalkMode::~PlayerMoveSetWalkMode() {
}

// OFFSET: 0x0067BDC0/0x140324C80
void PlayerMoveSetWalkMode::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7002));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673E40/0x14031C480]
PlayerMoveSplineDone::PlayerMoveSplineDone() {
}

// OFFSET: [0x006736E0/0x14031BB70]
PlayerMoveSplineDone::~PlayerMoveSplineDone() {
}

// OFFSET: 0x00681F60/0x14032BCC0
void PlayerMoveSplineDone::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x790E));
    msg->Put(this->m_splineId);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[7] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00674B10]
PlayerMoveStartAscend::PlayerMoveStartAscend() {
}

// OFFSET: [0x00673640]
PlayerMoveStartAscend::~PlayerMoveStartAscend() {
}

// OFFSET: 0x00679B40/0x140322570
void PlayerMoveStartAscend::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x390A));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[4] != 0);

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x006736C0/0x14031BB40]
PlayerMoveStartBackward::PlayerMoveStartBackward() {
}

// OFFSET: [0x00673EC0/0x14031C550]
PlayerMoveStartBackward::~PlayerMoveStartBackward() {
}

// OFFSET: 0x00682910/0x14032C830
void PlayerMoveStartBackward::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x330A));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00670C70]
PlayerMoveStartDescend::PlayerMoveStartDescend() {
}

// OFFSET: [0x006743D0]
PlayerMoveStartDescend::~PlayerMoveStartDescend() {
}

// OFFSET: 0x00686B20/0x140331630
void PlayerMoveStartDescend::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3800));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00670F00/0x1403192A0]
PlayerMoveStartForward::PlayerMoveStartForward() {
}

// OFFSET: [0x006747F0/0x140318650]
PlayerMoveStartForward::~PlayerMoveStartForward() {
}

// OFFSET: 0x00684D20/0x14032F2D0
void PlayerMoveStartForward::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7814));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00670DD0/0x140319110]
PlayerMoveStartPitchDown::PlayerMoveStartPitchDown() {
}

// OFFSET: [0x00673F30/0x1403181E0]
PlayerMoveStartPitchDown::~PlayerMoveStartPitchDown() {
}

// OFFSET: 0x0067C280/0x140325230
void PlayerMoveStartPitchDown::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3908));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00673D40/0x14031C2C0]
PlayerMoveStartPitchUp::PlayerMoveStartPitchUp() {
}

// OFFSET: [0x00673D60/0x14031C2F0]
PlayerMoveStartPitchUp::~PlayerMoveStartPitchUp() {
}

// OFFSET: 0x00677AE0/0x140320130
void PlayerMoveStartPitchUp::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3304));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[7] != 0);

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00670CB0/0x140318F80]
PlayerMoveStartStrafeLeft::PlayerMoveStartStrafeLeft() {
}

// OFFSET: [0x00674790/0x140318510]
PlayerMoveStartStrafeLeft::~PlayerMoveStartStrafeLeft() {
}

// OFFSET: 0x00680500/0x140329DD0
void PlayerMoveStartStrafeLeft::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3A16));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[0] != 0);

        if ((((((((((3 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00674CC0/0x140318BA0]
PlayerMoveStartStrafeRight::PlayerMoveStartStrafeRight() {
}

// OFFSET: [0x00673A60/0x14031BF20]
PlayerMoveStartStrafeRight::~PlayerMoveStartStrafeRight() {
}

// OFFSET: 0x0067ED20/0x140328260
void PlayerMoveStartStrafeRight::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3A02));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[0] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x006731B0/0x14031B5A0]
PlayerMoveStartSwim::PlayerMoveStartSwim() {
}

// OFFSET: [0x00673D10/0x14031C270]
PlayerMoveStartSwim::~PlayerMoveStartSwim() {
}

// OFFSET: 0x0067CD40/0x140325D50
void PlayerMoveStartSwim::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3206));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[2] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00674AB0/0x1403188F0]
PlayerMoveStartTurnLeft::PlayerMoveStartTurnLeft() {
}

// OFFSET: [0x006747D0/0x1403185E0]
PlayerMoveStartTurnLeft::~PlayerMoveStartTurnLeft() {
}

// OFFSET: 0x0067C740/0x1403257D0
void PlayerMoveStartTurnLeft::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x700C));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[6] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);

        if (((((((((((2 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00673F10/0x1403181B0]
PlayerMoveStartTurnRight::PlayerMoveStartTurnRight() {
}

// OFFSET: [0x00674990/0x1403186C0]
PlayerMoveStartTurnRight::~PlayerMoveStartTurnRight() {
}

// OFFSET: 0x0067B8F0/0x1403246D0
void PlayerMoveStartTurnRight::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7000));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00670B90/0x14031B5A0]
PlayerMoveStop::PlayerMoveStop() {
}

// OFFSET: [0x00673E60/0x14031C270]
PlayerMoveStop::~PlayerMoveStop() {
}

// OFFSET: 0x00676200/0x140325D50
void PlayerMoveStop::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x320A));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }
}

// OFFSET: [0x00674B30]
PlayerMoveStopAscend::PlayerMoveStopAscend() {
}

// OFFSET: [0x006731D0]
PlayerMoveStopAscend::~PlayerMoveStopAscend() {
}

// OFFSET: 0x0067FB70/0x140329260
void PlayerMoveStopAscend::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7B00));
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[5] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[0] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_position.z);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00670B50/0x140318D20]
PlayerMoveStopPitch::PlayerMoveStopPitch() {
}

// OFFSET: [0x00674CB0/0x140318B20]
PlayerMoveStopPitch::~PlayerMoveStopPitch() {
}

// OFFSET: 0x0067D6D0/0x1403268B0
void PlayerMoveStopPitch::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7216));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveFlags == 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_hasTransport) {
        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x006735F0/0x14031BA30]
PlayerMoveStopStrafe::PlayerMoveStopStrafe() {
}

// OFFSET: [0x00674620/0x140318490]
PlayerMoveStopStrafe::~PlayerMoveStopStrafe() {
}

// OFFSET: 0x00677620/0x14031FB80
void PlayerMoveStopStrafe::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3002));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }
}

// OFFSET: [0x00673DC0/0x140318DC0]
PlayerMoveStopSwim::PlayerMoveStopSwim() {
}

// OFFSET: [0x00673340/0x14031C4B0]
PlayerMoveStopSwim::~PlayerMoveStopSwim() {
}

// OFFSET: 0x006766C0/0x14031E400
void PlayerMoveStopSwim::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3802));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasFall);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_zSpeed);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }
}

// OFFSET: [0x00670C90/0x140318F50]
PlayerMoveStopTurn::PlayerMoveStopTurn() {
}

// OFFSET: [0x00670DC0/0x1403190F0]
PlayerMoveStopTurn::~PlayerMoveStopTurn() {
}

// OFFSET: 0x00681AA0/0x14032B710
void PlayerMoveStopTurn::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x331E));
    msg->Put(this->m_status.m_position.x);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[7] != 0);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);
        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        msg->Put(this->m_status.m_fall.m_time);

        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00674800]
PlayerMoveSwimBackSpeedChangeAck::PlayerMoveSwimBackSpeedChangeAck() {
}

// OFFSET: [0x00670EB0]
PlayerMoveSwimBackSpeedChangeAck::~PlayerMoveSwimBackSpeedChangeAck() {
}

// OFFSET: 0x006832A0/0x14032D390
void PlayerMoveSwimBackSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7A16));
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_ackIndex);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (this->m_data.m_status.m_hasFall) {
        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
        msg->Put(this->m_data.m_status.m_fall.m_time);
    }

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);
        msg->Put(this->m_data.m_status.m_transport.m_facing);
        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }
}

// OFFSET: [0x00673650]
PlayerMoveSwimSpeedChangeAck::PlayerMoveSwimSpeedChangeAck() {
}

// OFFSET: [0x00673420]
PlayerMoveSwimSpeedChangeAck::~PlayerMoveSwimSpeedChangeAck() {
}

// OFFSET: 0x00675D20/0x14031DE40
void PlayerMoveSwimSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7A10));
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_ackIndex);
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_status.m_position.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(guid[0] != 0);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);
        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.z);
        msg->Put(this->m_data.m_status.m_transport.m_facing);
        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_data.m_status.m_hasFall) {
        msg->Put(this->m_data.m_status.m_fall.m_time);

        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }
}

// OFFSET: [0x00670600/0x140317F50]
PlayerMoveTeleportAck::PlayerMoveTeleportAck() {
}

// OFFSET: [0x00670930/0x1403187D0]
PlayerMoveTeleportAck::~PlayerMoveTeleportAck() {
}

// OFFSET: 0x00673BB0/0x14031C100
void PlayerMoveTeleportAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_guid);
    msg->Put(static_cast<uint32_t>(0x390C));
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_time);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.Flush();

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }
}

// OFFSET: [0x006709C0/0x140318A80]
PlayerMoveTimeSkipped::PlayerMoveTimeSkipped() {
}

// OFFSET: [0x00670960/0x140318940]
PlayerMoveTimeSkipped::~PlayerMoveTimeSkipped() {
}

// OFFSET: 0x00674EB0/0x14031D0B0
void PlayerMoveTimeSkipped::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_guid);
    msg->Put(static_cast<uint32_t>(0x7A0A));
    msg->Put(this->m_timeSkipped);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }
}

// OFFSET: [0x00673320]
PlayerMoveToggleCollisionCheat::PlayerMoveToggleCollisionCheat() {
}

// OFFSET: [0x006747A0]
PlayerMoveToggleCollisionCheat::~PlayerMoveToggleCollisionCheat() {
}

// OFFSET: 0x00686190/0x140330AD0
void PlayerMoveToggleCollisionCheat::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7B04));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(this->m_status.m_heightChangeFailed);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[6] != 0);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);
        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }
}

// OFFSET: [0x00673400]
PlayerMoveTurnRateChangeAck::PlayerMoveTurnRateChangeAck() {
}

// OFFSET: [0x00673DF0]
PlayerMoveTurnRateChangeAck::~PlayerMoveTurnRateChangeAck() {
}

// OFFSET: 0x00682430/0x14032C260
void PlayerMoveTurnRateChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7316));
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_ackIndex);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[1] != 0);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_data.m_status.m_hasTransport) {
        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_facing);
        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_hasFall) {
        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);
        msg->Put(this->m_data.m_status.m_fall.m_time);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }
}

// OFFSET: [0x00674AD0]
PlayerMoveUnrootAck::PlayerMoveUnrootAck() {
}

// OFFSET: [0x00673E00]
PlayerMoveUnrootAck::~PlayerMoveUnrootAck() {
}

// OFFSET: 0x00676B90/0x14031EF50
void PlayerMoveUnrootAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7808));
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[6] != 0);

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[1] != 0);

        if ((((((((((3 + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) + 1) == 7) {
        }
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    bits.Flush();

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
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

    if (this->m_status.m_hasTransport) {
        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00673D20]
PlayerMoveWalkSpeedChangeAck::PlayerMoveWalkSpeedChangeAck() {
}

// OFFSET: [0x00673E10]
PlayerMoveWalkSpeedChangeAck::~PlayerMoveWalkSpeedChangeAck() {
}

// OFFSET: 0x00679660/0x140321FA0
void PlayerMoveWalkSpeedChangeAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_data.m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x7210));
    msg->Put(this->m_data.m_status.m_position.z);
    msg->Put(this->m_data.m_status.m_position.y);
    msg->Put(this->m_data.m_speed);
    msg->Put(this->m_data.m_status.m_position.x);
    msg->Put(this->m_data.m_ackIndex);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveExtraFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_gameTime == 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_data.m_status.m_moveFlags == 0);
    bits.WriteBit(this->m_data.m_status.m_hasFall);
    bits.WriteBit(this->m_data.m_status.m_facing == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasSpline);
    bits.WriteBit(this->m_data.m_status.m_heightChangeFailed);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_data.m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_data.m_status.m_hasTransport);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_data.m_status.m_pitch == 0.0f);

    if (this->m_data.m_status.m_hasTransport) {
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(this->m_data.m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[3] != 0);
    }

    if (this->m_data.m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_data.m_status.m_moveFlags);
    }

    if (this->m_data.m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_data.m_status.m_moveExtraFlags);
    }

    if (this->m_data.m_status.m_hasFall) {
        bits.WriteBit(this->m_data.m_status.m_fall.m_hasDirection);
    }

    bits.Flush();

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (this->m_data.m_status.m_hasFall) {
        msg->Put(this->m_data.m_status.m_fall.m_zSpeed);

        if (this->m_data.m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_data.m_status.m_fall.m_direction.m_sinAngle);
        }

        msg->Put(this->m_data.m_status.m_fall.m_time);
    }

    if (this->m_data.m_status.m_hasTransport) {
        msg->Put(this->m_data.m_status.m_transport.m_position.z);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.x);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_time);

        if (this->m_data.m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_data.m_status.m_transport.m_vehicleId);
        }

        msg->Put(this->m_data.m_status.m_transport.m_position.y);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_data.m_status.m_transport.m_facing);
        msg->Put(this->m_data.m_status.m_transport.m_seat);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (this->m_data.m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_data.m_status.m_transport.m_time2);
        }
    }

    if (this->m_data.m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_data.m_status.m_splineElevation);
    }

    if (this->m_data.m_status.m_gameTime) {
        msg->Put(this->m_data.m_status.m_gameTime);
    }

    if (this->m_data.m_status.m_facing != 0.0f) {
        msg->Put(this->m_data.m_status.m_facing);
    }

    if (this->m_data.m_status.m_pitch != 0.0f) {
        msg->Put(this->m_data.m_status.m_pitch);
    }
}

// OFFSET: [0x00674A10]
PlayerMoveWaterWalkAck::PlayerMoveWaterWalkAck() {
}

// OFFSET: [0x00673B20]
PlayerMoveWaterWalkAck::~PlayerMoveWaterWalkAck() {
}

// OFFSET: 0x00680030/0x140329810
void PlayerMoveWaterWalkAck::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x3B00));
    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);
    msg->Put(this->m_ackIndex);
    msg->Put(this->m_status.m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_status.m_gameTime == 0);
    bits.WriteBit(this->m_status.m_pitch == 0.0f);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_status.m_facing == 0.0f);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_status.m_moveFlags == 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(this->m_status.m_hasTransport);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(this->m_status.m_hasFall);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_status.m_heightChangeFailed);
    bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
    bits.WriteBit(this->m_status.m_hasSpline);

    if (this->m_status.m_hasTransport) {
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[3] != 0);
        bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[4] != 0);
    }

    if (this->m_status.m_moveFlags) {
        bits.WriteBits<30>(this->m_status.m_moveFlags);
    }

    if (this->m_status.m_hasFall) {
        bits.WriteBit(this->m_status.m_fall.m_hasDirection);
    }

    if (this->m_status.m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.x);

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_facing);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }
}

// OFFSET: [0x00670BB0/0x140318DF0]
PlayerSetActiveMover::PlayerSetActiveMover() {
}

// OFFSET: [0x00670850/0x140318480]
PlayerSetActiveMover::~PlayerSetActiveMover() {
}

// OFFSET: 0x00673710/0x14031BB90
void PlayerSetActiveMover::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_guid);
    msg->Put(static_cast<uint32_t>(0x3314));

    JamBitWriter bits(msg);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.Flush();

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }
}

// OFFSET: 0x00683780/0x14031CEF0
PlayerMoveUpdateMissileTrajectory::PlayerMoveUpdateMissileTrajectory() : m_hasStatus(false) {
}

// OFFSET: 0x00684840/0x14031D090
PlayerMoveUpdateMissileTrajectory::~PlayerMoveUpdateMissileTrajectory() {
}

// OFFSET: 0x0067AB00/0x140323790
void PlayerMoveUpdateMissileTrajectory::Put(CDataStore* msg) {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_guid);
    const uint8_t* moverGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_status.m_transport.m_guid);
    msg->Put(static_cast<uint32_t>(0x781E));
    msg->Put(this->m_firePos.z);
    msg->Put(this->m_firePos.x);
    msg->Put(this->m_pitch);
    msg->Put(this->m_impactPos.x);
    msg->Put(this->m_firePos.y);
    msg->Put(this->m_speed);
    msg->Put(this->m_impactPos.y);
    msg->Put(this->m_moveMsgID);
    msg->Put(this->m_spellID);
    msg->Put(this->m_impactPos.z);

    JamBitWriter bits(msg);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[0] != 0);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_hasStatus);

    if (this->m_hasStatus) {
        bits.WriteBit(moverGuid[2] != 0);
        bits.WriteBit(this->m_status.m_heightChangeFailed);
        bits.WriteBit(this->m_status.m_moveFlags == 0);
        bits.WriteBit(this->m_status.m_hasSpline);
        bits.WriteBit(moverGuid[4] != 0);

        if (this->m_status.m_moveFlags) {
            bits.WriteBits<30>(this->m_status.m_moveFlags);
        }

        bits.WriteBit(moverGuid[3] != 0);
        bits.WriteBit(this->m_status.m_pitch == 0.0f);
        bits.WriteBit(this->m_status.m_moveExtraFlags == 0);
        bits.WriteBit(this->m_status.m_hasTransport);
        bits.WriteBit(moverGuid[7] != 0);

        if (this->m_status.m_hasTransport) {
            bits.WriteBit(transportGuid[0] != 0);
            bits.WriteBit(transportGuid[7] != 0);
            bits.WriteBit(transportGuid[2] != 0);
            bits.WriteBit(transportGuid[4] != 0);
            bits.WriteBit(this->m_status.m_transport.m_hasVehicleId);
            bits.WriteBit(transportGuid[5] != 0);
            bits.WriteBit(transportGuid[3] != 0);
            bits.WriteBit(transportGuid[6] != 0);
            bits.WriteBit(this->m_status.m_transport.m_hasTime2);
            bits.WriteBit(transportGuid[1] != 0);
        }

        bits.WriteBit(moverGuid[6] != 0);
        bits.WriteBit(this->m_status.m_facing == 0.0f);
        bits.WriteBit(this->m_status.m_gameTime == 0);
        bits.WriteBit(moverGuid[5] != 0);
        bits.WriteBit(this->m_status.m_splineElevation == 0.0f);
        bits.WriteBit(moverGuid[1] != 0);
        bits.WriteBit(this->m_status.m_hasFall);
        bits.WriteBit(moverGuid[0] != 0);

        if (this->m_status.m_hasFall) {
            bits.WriteBit(this->m_status.m_fall.m_hasDirection);
        }

        if (this->m_status.m_moveExtraFlags) {
            bits.WriteBits<12>(this->m_status.m_moveExtraFlags);
        }
    }

    bits.Flush();

    if (guid[2]) {
        msg->Put(static_cast<uint8_t>(guid[2] ^ 1));
    }

    if (guid[1]) {
        msg->Put(static_cast<uint8_t>(guid[1] ^ 1));
    }

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
    }

    if (guid[7]) {
        msg->Put(static_cast<uint8_t>(guid[7] ^ 1));
    }

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (guid[4]) {
        msg->Put(static_cast<uint8_t>(guid[4] ^ 1));
    }

    if (guid[0]) {
        msg->Put(static_cast<uint8_t>(guid[0] ^ 1));
    }

    if (guid[5]) {
        msg->Put(static_cast<uint8_t>(guid[5] ^ 1));
    }

    if (!this->m_hasStatus) {
        return;
    }

    if (this->m_status.m_hasFall) {
        if (this->m_status.m_fall.m_hasDirection) {
            msg->Put(this->m_status.m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_cosAngle);
            msg->Put(this->m_status.m_fall.m_direction.m_xySpeed);
        }

        msg->Put(this->m_status.m_fall.m_zSpeed);
        msg->Put(this->m_status.m_fall.m_time);
    }

    if (this->m_status.m_facing != 0.0f) {
        msg->Put(this->m_status.m_facing);
    }

    if (this->m_status.m_hasTransport) {
        msg->Put(this->m_status.m_transport.m_position.z);

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.x);
        msg->Put(this->m_status.m_transport.m_facing);

        if (this->m_status.m_transport.m_hasTime2) {
            msg->Put(this->m_status.m_transport.m_time2);
        }

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_time);

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_position.y);

        if (this->m_status.m_transport.m_hasVehicleId) {
            msg->Put(this->m_status.m_transport.m_vehicleId);
        }

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        msg->Put(this->m_status.m_transport.m_seat);

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }
    }

    if (this->m_status.m_splineElevation != 0.0f) {
        msg->Put(this->m_status.m_splineElevation);
    }

    if (moverGuid[4]) {
        msg->Put(static_cast<uint8_t>(moverGuid[4] ^ 1));
    }

    if (this->m_status.m_pitch != 0.0f) {
        msg->Put(this->m_status.m_pitch);
    }

    if (moverGuid[7]) {
        msg->Put(static_cast<uint8_t>(moverGuid[7] ^ 1));
    }

    if (moverGuid[2]) {
        msg->Put(static_cast<uint8_t>(moverGuid[2] ^ 1));
    }

    if (this->m_status.m_gameTime) {
        msg->Put(this->m_status.m_gameTime);
    }

    msg->Put(this->m_status.m_position.x);

    if (moverGuid[5]) {
        msg->Put(static_cast<uint8_t>(moverGuid[5] ^ 1));
    }

    if (moverGuid[3]) {
        msg->Put(static_cast<uint8_t>(moverGuid[3] ^ 1));
    }

    if (moverGuid[1]) {
        msg->Put(static_cast<uint8_t>(moverGuid[1] ^ 1));
    }

    if (moverGuid[0]) {
        msg->Put(static_cast<uint8_t>(moverGuid[0] ^ 1));
    }

    msg->Put(this->m_status.m_position.y);
    msg->Put(this->m_status.m_position.z);

    if (moverGuid[6]) {
        msg->Put(static_cast<uint8_t>(moverGuid[6] ^ 1));
    }
}

// OFFSET: 0x00670AE0/0x140318C40
PlayerMoveTeleportCheat::PlayerMoveTeleportCheat(const C3Vector& position, float facing) {
    this->m_position = position;
    this->m_facing = facing;
}

// OFFSET: 0x00670570/0x140317EB0
PlayerMoveTeleportCheat::~PlayerMoveTeleportCheat() {
}

// OFFSET: 0x00674A30/0x140318860
void PlayerMoveTeleportCheat::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3A10));
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.y);
    msg->Put(this->m_facing);
    msg->Put(this->m_position.z);
}

// OFFSET: 0x006740C0/0x140318240
PlayerMoveSetRawPosition::PlayerMoveSetRawPosition(const C3Vector& position, float facing) {
    this->m_facing = facing;
    this->m_position = position;
}

// OFFSET: 0x00670990/0x140318A10
PlayerMoveSetRawPosition::~PlayerMoveSetRawPosition() {
}

// OFFSET: 0x00674420/0x1403183B0
void PlayerMoveSetRawPosition::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x311A));
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.z);
    msg->Put(this->m_facing);
    msg->Put(this->m_position.y);
}

// OFFSET: 0x00673A10/0x14031BEE0
PlayerMoveCharmPortCheat::PlayerMoveCharmPortCheat(const C3Vector& position, float facing) {
    this->m_facing = facing;
    this->m_position = position;
}

// OFFSET: 0x006709F0/0x140318AD0
PlayerMoveCharmPortCheat::~PlayerMoveCharmPortCheat() {
}

// OFFSET: 0x00670A80/0x140318BE0
void PlayerMoveCharmPortCheat::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7A08));
    msg->Put(this->m_facing);
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.z);
}
