#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "Engine/Source/Base/CDataStore.h"

// PROVEN: e5cd50f7 IDENTICAL/NONE
// OFFSET: [0x00670D50]
PlayerChangeSeatsOnControlledVehicle::PlayerChangeSeatsOnControlledVehicle() {
}

// PROVEN: edd593d9 IDENTICAL/NONE
// OFFSET: [0x00673BA0]
PlayerChangeSeatsOnControlledVehicle::~PlayerChangeSeatsOnControlledVehicle() {
}

// PROVEN: b042741a IDENTICAL/NONE
// OFFSET: 0x00678A60
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

// PROVEN: 605ff5b6 IDENTICAL/NONE
// OFFSET: [0x006749F0]
PlayerDismissControlledVehicle::PlayerDismissControlledVehicle() {
}

// PROVEN: 69b9fa50 IDENTICAL/NONE
// OFFSET: [0x006749A0]
PlayerDismissControlledVehicle::~PlayerDismissControlledVehicle() {
}

// PROVEN: 3d9a643e IDENTICAL/NONE
// OFFSET: 0x00675860
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

// PROVEN: 116cde1b IDENTICAL/NONE
// OFFSET: [0x006744C0]
PlayerMoveChangeTransport::PlayerMoveChangeTransport() {
}

// PROVEN: 47ddceaf IDENTICAL/NONE
// OFFSET: [0x00670E00]
PlayerMoveChangeTransport::~PlayerMoveChangeTransport() {
}

// PROVEN: d5cce449 IDENTICAL/NONE
// OFFSET: 0x00677150
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

// PROVEN: 7be1db6a IDENTICAL/NONE
// OFFSET: [0x006747B0]
PlayerMoveFallLand::PlayerMoveFallLand() {
}

// PROVEN: 30e4dd41 IDENTICAL/NONE
// OFFSET: [0x006733D0]
PlayerMoveFallLand::~PlayerMoveFallLand() {
}

// PROVEN: 881ca18d IDENTICAL/NONE
// OFFSET: 0x00686660
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

// PROVEN: 7d572095 IDENTICAL/NONE
// OFFSET: [0x00673350]
PlayerMoveFallReset::PlayerMoveFallReset() {
}

// PROVEN: 80b2885e IDENTICAL/NONE
// OFFSET: [0x00673860]
PlayerMoveFallReset::~PlayerMoveFallReset() {
}

// PROVEN: fbd27a27 IDENTICAL/NONE
// OFFSET: 0x00681120
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

// PROVEN: 55338a2d IDENTICAL/NONE
// OFFSET: [0x00673B30]
PlayerMoveFeatherFallAck::PlayerMoveFeatherFallAck() {
}

// PROVEN: 87687411 IDENTICAL/NONE
// OFFSET: [0x00670DB0]
PlayerMoveFeatherFallAck::~PlayerMoveFeatherFallAck() {
}

// PROVEN: fbb42c02 IDENTICAL/NONE
// OFFSET: 0x00684370
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

// PROVEN: 517c3934 IDENTICAL/NONE
// OFFSET: [0x00670E90]
PlayerMoveFlightBackSpeedChangeAck::PlayerMoveFlightBackSpeedChangeAck() {
}

// PROVEN: 513008af IDENTICAL/NONE
// OFFSET: [0x00673E20]
PlayerMoveFlightBackSpeedChangeAck::~PlayerMoveFlightBackSpeedChangeAck() {
}

// PROVEN: 52e9db2a IDENTICAL/NONE
// OFFSET: 0x0067A620
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

// PROVEN: 135174f3 IDENTICAL/NONE
// OFFSET: [0x00674A90]
PlayerMoveFlightSpeedChangeAck::PlayerMoveFlightSpeedChangeAck() {
}

// PROVEN: 01f4d0df IDENTICAL/NONE
// OFFSET: [0x00674B50]
PlayerMoveFlightSpeedChangeAck::~PlayerMoveFlightSpeedChangeAck() {
}

// PROVEN: d5deed71 IDENTICAL/NONE
// OFFSET: 0x00685310
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

// PROVEN: d042563f IDENTICAL/NONE
// OFFSET: [0x006749B0]
PlayerMoveGravityDisableAck::PlayerMoveGravityDisableAck() {
}

// PROVEN: fb0f45ef IDENTICAL/NONE
// OFFSET: [0x00673D00]
PlayerMoveGravityDisableAck::~PlayerMoveGravityDisableAck() {
}

// PROVEN: 05cb3bf4 IDENTICAL/NONE
// OFFSET: 0x006780D0
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

// PROVEN: 0a4fa1ba IDENTICAL/NONE
// OFFSET: [0x00674E90]
PlayerMoveGravityEnableAck::PlayerMoveGravityEnableAck() {
}

// PROVEN: f919bf8e IDENTICAL/NONE
// OFFSET: [0x00673E70]
PlayerMoveGravityEnableAck::~PlayerMoveGravityEnableAck() {
}

// PROVEN: c848ce85 IDENTICAL/NONE
// OFFSET: 0x00684850
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

// PROVEN: 18317eef IDENTICAL/NONE
// OFFSET: [0x00673190/0x14031B570]
PlayerMoveHeartbeat::PlayerMoveHeartbeat() {
}

// PROVEN: ddd160a5 IDENTICAL/NONE
// OFFSET: [0x00674D00/0x140318CD0]
PlayerMoveHeartbeat::~PlayerMoveHeartbeat() {
}

// PROVEN: afe8114f IDENTICAL/NONE
// OFFSET: 0x00687AB0
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

// PROVEN: 50996e41 IDENTICAL/NONE
// OFFSET: [0x00670E60]
PlayerMoveHoverAck::PlayerMoveHoverAck() {
}

// PROVEN: 22ab5a41 IDENTICAL/NONE
// OFFSET: [0x00673890]
PlayerMoveHoverAck::~PlayerMoveHoverAck() {
}

// PROVEN: f3989829 IDENTICAL/NONE
// OFFSET: 0x0067E070
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

// PROVEN: 0dba7945 IDENTICAL/NONE
// OFFSET: [0x00673E80]
PlayerMoveJump::PlayerMoveJump() {
}

// PROVEN: 985e933e IDENTICAL/NONE
// OFFSET: [0x00670D90]
PlayerMoveJump::~PlayerMoveJump() {
}

// PROVEN: 92a7f02c IDENTICAL/NONE
// OFFSET: 0x006857F0
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

// PROVEN: f718a1b1 IDENTICAL/NONE
// OFFSET: [0x00674AF0]
PlayerMoveKnockBackAck::PlayerMoveKnockBackAck() {
}

// PROVEN: 67651730 IDENTICAL/NONE
// OFFSET: [0x00673D70]
PlayerMoveKnockBackAck::~PlayerMoveKnockBackAck() {
}

// PROVEN: 8eafb13d IDENTICAL/NONE
// OFFSET: 0x00680B20
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

// PROVEN: c7893b7d IDENTICAL/NONE
// OFFSET: [0x00673EA0]
PlayerMoveNotActiveMover::PlayerMoveNotActiveMover() {
}

// PROVEN: 936de125 IDENTICAL/NONE
// OFFSET: [0x006747E0]
PlayerMoveNotActiveMover::~PlayerMoveNotActiveMover() {
}

// PROVEN: 48c2d1d4 IDENTICAL/NONE
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

// PROVEN: 12d93b20 IDENTICAL/NONE
// OFFSET: [0x00673B50]
PlayerMovePitchRateChangeAck::PlayerMovePitchRateChangeAck() {
}

// PROVEN: 5a3e76a4 IDENTICAL/NONE
// OFFSET: [0x00673E30]
PlayerMovePitchRateChangeAck::~PlayerMovePitchRateChangeAck() {
}

// PROVEN: 86e51f38 IDENTICAL/NONE
// OFFSET: 0x0067DB90
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

// PROVEN: e6d7515e IDENTICAL/NONE
// OFFSET: [0x00673160]
PlayerMoveRootAck::PlayerMoveRootAck() {
}

// PROVEN: ad1a769a IDENTICAL/NONE
// OFFSET: [0x00673450]
PlayerMoveRootAck::~PlayerMoveRootAck() {
}

// PROVEN: afbd1427 IDENTICAL/NONE
// OFFSET: 0x00686FF0
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

// PROVEN: f5fc1a74 IDENTICAL/NONE
// OFFSET: [0x006739F0]
PlayerMoveRunBackSpeedChangeAck::PlayerMoveRunBackSpeedChangeAck() {
}

// PROVEN: 374d14d3 IDENTICAL/NONE
// OFFSET: [0x006743C0]
PlayerMoveRunBackSpeedChangeAck::~PlayerMoveRunBackSpeedChangeAck() {
}

// PROVEN: 93e55c44 IDENTICAL/NONE
// OFFSET: 0x00683E80
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

// PROVEN: 4b3f1991 IDENTICAL/NONE
// OFFSET: [0x00670F20]
PlayerMoveRunSpeedChangeAck::PlayerMoveRunSpeedChangeAck() {
}

// PROVEN: 56f282b5 IDENTICAL/NONE
// OFFSET: [0x00670DA0]
PlayerMoveRunSpeedChangeAck::~PlayerMoveRunSpeedChangeAck() {
}

// PROVEN: 536582b0 IDENTICAL/NONE
// OFFSET: 0x00679180
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

// PROVEN: 67168d91 IDENTICAL/NONE
// OFFSET: [0x00674CE0]
PlayerMoveSetCanFlyAck::PlayerMoveSetCanFlyAck() {
}

// PROVEN: 7f80300b IDENTICAL/NONE
// OFFSET: [0x00673DE0]
PlayerMoveSetCanFlyAck::~PlayerMoveSetCanFlyAck() {
}

// PROVEN: ef336828 IDENTICAL/NONE
// OFFSET: 0x0067A140
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

// PROVEN: 2e20c305 IDENTICAL/NONE
// OFFSET: [0x00673AD0]
PlayerMoveSetCanTransitionBetweenSwimAndFlyAck::PlayerMoveSetCanTransitionBetweenSwimAndFlyAck() {
}

// PROVEN: c8b6d2a7 IDENTICAL/NONE
// OFFSET: [0x00674D10]
PlayerMoveSetCanTransitionBetweenSwimAndFlyAck::~PlayerMoveSetCanTransitionBetweenSwimAndFlyAck() {
}

// PROVEN: d6979db2 IDENTICAL/NONE
// OFFSET: 0x0067F6A0
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

// PROVEN: 930d6b44 IDENTICAL/NONE
// OFFSET: [0x00673140]
PlayerMoveSetCollisionHeightAck::PlayerMoveSetCollisionHeightAck() {
}

// PROVEN: 56fad513 IDENTICAL/NONE
// OFFSET: [0x006749E0]
PlayerMoveSetCollisionHeightAck::~PlayerMoveSetCollisionHeightAck() {
}

// PROVEN: fbc1b136 IDENTICAL/NONE
// OFFSET: 0x0067E540
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

// PROVEN: e8caea3c IDENTICAL/NONE
// OFFSET: [0x00673370]
PlayerMoveSetFacing::PlayerMoveSetFacing() {
}

// PROVEN: 6b4a8f60 IDENTICAL/NONE
// OFFSET: [0x006733F0]
PlayerMoveSetFacing::~PlayerMoveSetFacing() {
}

// PROVEN: 7ae133bf IDENTICAL/NONE
// OFFSET: 0x006785A0
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

// PROVEN: 7a130cff IDENTICAL/NONE
// OFFSET: [0x00673A70]
PlayerMoveSetFly::PlayerMoveSetFly() {
}

// PROVEN: eaf6606e IDENTICAL/NONE
// OFFSET: [0x006749D0]
PlayerMoveSetFly::~PlayerMoveSetFly() {
}

// PROVEN: 660b32a6 IDENTICAL/NONE
// OFFSET: 0x00682DE0
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

// PROVEN: ad5174bd IDENTICAL/NONE
// OFFSET: [0x00673120]
PlayerMoveSetPitch::PlayerMoveSetPitch() {
}

// PROVEN: 41a52060 IDENTICAL/NONE
// OFFSET: [0x00673670]
PlayerMoveSetPitch::~PlayerMoveSetPitch() {
}

// PROVEN: c37fe413 IDENTICAL/NONE
// OFFSET: 0x0067D210
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

// PROVEN: 82fb8dcd IDENTICAL/NONE
// OFFSET: [0x00673D80]
PlayerMoveSetRunMode::PlayerMoveSetRunMode() {
}

// PROVEN: f67ec3a1 IDENTICAL/NONE
// OFFSET: [0x00674410]
PlayerMoveSetRunMode::~PlayerMoveSetRunMode() {
}

// PROVEN: aa6cb35c IDENTICAL/NONE
// OFFSET: 0x0067B430
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

// PROVEN: 6396cafd IDENTICAL/NONE
// OFFSET: [0x00670D30]
PlayerMoveSetVehicleRecIdAck::PlayerMoveSetVehicleRecIdAck() {
}

// PROVEN: 3951ea39 IDENTICAL/NONE
// OFFSET: [0x00673850]
PlayerMoveSetVehicleRecIdAck::~PlayerMoveSetVehicleRecIdAck() {
}

// PROVEN: ce83a869 IDENTICAL/NONE
// OFFSET: 0x00685CB0
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

// PROVEN: f0fe7468 IDENTICAL/NONE
// OFFSET: [0x006736A0]
PlayerMoveSetWalkMode::PlayerMoveSetWalkMode() {
}

// PROVEN: f6c3445c IDENTICAL/NONE
// OFFSET: [0x006738A0]
PlayerMoveSetWalkMode::~PlayerMoveSetWalkMode() {
}

// PROVEN: 131c3610 IDENTICAL/NONE
// OFFSET: 0x0067BDC0
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

// PROVEN: add30e9d IDENTICAL/NONE
// OFFSET: [0x00673E40/0x14031C480]
PlayerMoveSplineDone::PlayerMoveSplineDone() {
}

// PROVEN: 021393d3 IDENTICAL/NONE
// OFFSET: [0x006736E0/0x14031BB70]
PlayerMoveSplineDone::~PlayerMoveSplineDone() {
}

// PROVEN: 08e288b7 IDENTICAL/NONE
// OFFSET: 0x00681F60
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

// PROVEN: a6c82d50 IDENTICAL/NONE
// OFFSET: [0x00674B10]
PlayerMoveStartAscend::PlayerMoveStartAscend() {
}

// PROVEN: b19152d2 IDENTICAL/NONE
// OFFSET: [0x00673640]
PlayerMoveStartAscend::~PlayerMoveStartAscend() {
}

// PROVEN: 8a088aef IDENTICAL/NONE
// OFFSET: 0x00679B40
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

// PROVEN: 8421f0c4 IDENTICAL/NONE
// OFFSET: [0x006736C0]
PlayerMoveStartBackward::PlayerMoveStartBackward() {
}

// PROVEN: 1a75475a IDENTICAL/NONE
// OFFSET: [0x00673EC0]
PlayerMoveStartBackward::~PlayerMoveStartBackward() {
}

// PROVEN: 379ee25c IDENTICAL/NONE
// OFFSET: 0x00682910
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

// PROVEN: f8e56b09 IDENTICAL/NONE
// OFFSET: [0x00670C70]
PlayerMoveStartDescend::PlayerMoveStartDescend() {
}

// PROVEN: e7a35b47 IDENTICAL/NONE
// OFFSET: [0x006743D0]
PlayerMoveStartDescend::~PlayerMoveStartDescend() {
}

// PROVEN: 5d237ef8 IDENTICAL/NONE
// OFFSET: 0x00686B20
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

// PROVEN: fba6efa1 IDENTICAL/NONE
// OFFSET: [0x00670F00]
PlayerMoveStartForward::PlayerMoveStartForward() {
}

// PROVEN: 9384453d IDENTICAL/NONE
// OFFSET: [0x006747F0]
PlayerMoveStartForward::~PlayerMoveStartForward() {
}

// PROVEN: 14a63388 IDENTICAL/NONE
// OFFSET: 0x00684D20
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

// PROVEN: e8912854 IDENTICAL/NONE
// OFFSET: [0x00670DD0]
PlayerMoveStartPitchDown::PlayerMoveStartPitchDown() {
}

// PROVEN: b30318d1 IDENTICAL/NONE
// OFFSET: [0x00673F30]
PlayerMoveStartPitchDown::~PlayerMoveStartPitchDown() {
}

// PROVEN: 2b7c5c5a IDENTICAL/NONE
// OFFSET: 0x0067C280
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

// PROVEN: fb424d10 IDENTICAL/NONE
// OFFSET: [0x00673D40]
PlayerMoveStartPitchUp::PlayerMoveStartPitchUp() {
}

// PROVEN: eb85b8e2 IDENTICAL/NONE
// OFFSET: [0x00673D60]
PlayerMoveStartPitchUp::~PlayerMoveStartPitchUp() {
}

// PROVEN: 5a1a0515 IDENTICAL/NONE
// OFFSET: 0x00677AE0
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

// PROVEN: 1a4d0848 IDENTICAL/NONE
// OFFSET: [0x00670CB0]
PlayerMoveStartStrafeLeft::PlayerMoveStartStrafeLeft() {
}

// PROVEN: cec6b0ad IDENTICAL/NONE
// OFFSET: [0x00674790]
PlayerMoveStartStrafeLeft::~PlayerMoveStartStrafeLeft() {
}

// PROVEN: a833f47e IDENTICAL/NONE
// OFFSET: 0x00680500
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

// PROVEN: 531081da IDENTICAL/NONE
// OFFSET: [0x00674CC0]
PlayerMoveStartStrafeRight::PlayerMoveStartStrafeRight() {
}

// PROVEN: 424103ed IDENTICAL/NONE
// OFFSET: [0x00673A60]
PlayerMoveStartStrafeRight::~PlayerMoveStartStrafeRight() {
}

// PROVEN: d823f16a IDENTICAL/NONE
// OFFSET: 0x0067ED20
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

// PROVEN: 75b1ad98 IDENTICAL/NONE
// OFFSET: [0x006731B0]
PlayerMoveStartSwim::PlayerMoveStartSwim() {
}

// PROVEN: 33842982 IDENTICAL/NONE
// OFFSET: [0x00673D10]
PlayerMoveStartSwim::~PlayerMoveStartSwim() {
}

// PROVEN: 4693e5e7 IDENTICAL/NONE
// OFFSET: 0x0067CD40
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

// PROVEN: f658d23f IDENTICAL/NONE
// OFFSET: [0x00674AB0]
PlayerMoveStartTurnLeft::PlayerMoveStartTurnLeft() {
}

// PROVEN: 4eed0b78 IDENTICAL/NONE
// OFFSET: [0x006747D0]
PlayerMoveStartTurnLeft::~PlayerMoveStartTurnLeft() {
}

// PROVEN: 2ffd0450 IDENTICAL/NONE
// OFFSET: 0x0067C740
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

// PROVEN: 23b5332d IDENTICAL/NONE
// OFFSET: [0x00673F10]
PlayerMoveStartTurnRight::PlayerMoveStartTurnRight() {
}

// PROVEN: 3701457f IDENTICAL/NONE
// OFFSET: [0x00674990]
PlayerMoveStartTurnRight::~PlayerMoveStartTurnRight() {
}

// PROVEN: 10c71b9b IDENTICAL/NONE
// OFFSET: 0x0067B8F0
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

// PROVEN: 72f070f3 IDENTICAL/NONE
// OFFSET: [0x00670B90]
PlayerMoveStop::PlayerMoveStop() {
}

// PROVEN: 7343b53f IDENTICAL/NONE
// OFFSET: [0x00673E60]
PlayerMoveStop::~PlayerMoveStop() {
}

// PROVEN: 86d882db IDENTICAL/NONE
// OFFSET: 0x00676200
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

// PROVEN: 4c735107 IDENTICAL/NONE
// OFFSET: [0x00674B30]
PlayerMoveStopAscend::PlayerMoveStopAscend() {
}

// PROVEN: 711fd700 IDENTICAL/NONE
// OFFSET: [0x006731D0]
PlayerMoveStopAscend::~PlayerMoveStopAscend() {
}

// PROVEN: 6e706f11 IDENTICAL/NONE
// OFFSET: 0x0067FB70
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

// PROVEN: 3722f506 IDENTICAL/NONE
// OFFSET: [0x00670B50]
PlayerMoveStopPitch::PlayerMoveStopPitch() {
}

// PROVEN: 0ec3fc0a IDENTICAL/NONE
// OFFSET: [0x00674CB0]
PlayerMoveStopPitch::~PlayerMoveStopPitch() {
}

// PROVEN: f65d5e39 IDENTICAL/NONE
// OFFSET: 0x0067D6D0
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

// PROVEN: 69a8c9cb IDENTICAL/NONE
// OFFSET: [0x006735F0]
PlayerMoveStopStrafe::PlayerMoveStopStrafe() {
}

// PROVEN: d8c7e9d1 IDENTICAL/NONE
// OFFSET: [0x00674620]
PlayerMoveStopStrafe::~PlayerMoveStopStrafe() {
}

// PROVEN: c896dec0 IDENTICAL/NONE
// OFFSET: 0x00677620
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

// PROVEN: a52c51bd IDENTICAL/NONE
// OFFSET: [0x00673DC0]
PlayerMoveStopSwim::PlayerMoveStopSwim() {
}

// PROVEN: e0aaa2fe IDENTICAL/NONE
// OFFSET: [0x00673340]
PlayerMoveStopSwim::~PlayerMoveStopSwim() {
}

// PROVEN: 31d3bc91 IDENTICAL/NONE
// OFFSET: 0x006766C0
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

// PROVEN: b758bb7c IDENTICAL/NONE
// OFFSET: [0x00670C90]
PlayerMoveStopTurn::PlayerMoveStopTurn() {
}

// PROVEN: 460fd371 IDENTICAL/NONE
// OFFSET: [0x00670DC0]
PlayerMoveStopTurn::~PlayerMoveStopTurn() {
}

// PROVEN: b6b845af IDENTICAL/NONE
// OFFSET: 0x00681AA0
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

// PROVEN: ea76337a IDENTICAL/NONE
// OFFSET: [0x00674800]
PlayerMoveSwimBackSpeedChangeAck::PlayerMoveSwimBackSpeedChangeAck() {
}

// PROVEN: 57c16a4d IDENTICAL/NONE
// OFFSET: [0x00670EB0]
PlayerMoveSwimBackSpeedChangeAck::~PlayerMoveSwimBackSpeedChangeAck() {
}

// PROVEN: 79bb3661 IDENTICAL/NONE
// OFFSET: 0x006832A0
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

// PROVEN: 8e2cf920 IDENTICAL/NONE
// OFFSET: [0x00673650]
PlayerMoveSwimSpeedChangeAck::PlayerMoveSwimSpeedChangeAck() {
}

// PROVEN: 458eaa53 IDENTICAL/NONE
// OFFSET: [0x00673420]
PlayerMoveSwimSpeedChangeAck::~PlayerMoveSwimSpeedChangeAck() {
}

// PROVEN: f1bc4c15 IDENTICAL/NONE
// OFFSET: 0x00675D20
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

// PROVEN: 6ff5e793 IDENTICAL/NONE
// OFFSET: [0x00670600]
PlayerMoveTeleportAck::PlayerMoveTeleportAck() {
}

// PROVEN: 0ed39f64 IDENTICAL/NONE
// OFFSET: [0x00670930]
PlayerMoveTeleportAck::~PlayerMoveTeleportAck() {
}

// PROVEN: 9eca6c8f IDENTICAL/NONE
// OFFSET: 0x00673BB0
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

// PROVEN: f4847976 IDENTICAL/NONE
// OFFSET: [0x006709C0/0x140318A80]
PlayerMoveTimeSkipped::PlayerMoveTimeSkipped() {
}

// PROVEN: 626f8db9 IDENTICAL/NONE
// OFFSET: [0x00670960/0x140318940]
PlayerMoveTimeSkipped::~PlayerMoveTimeSkipped() {
}

// PROVEN: 5299da66 IDENTICAL/NONE
// OFFSET: 0x00674EB0
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

// PROVEN: 1679f711 IDENTICAL/NONE
// OFFSET: [0x00673320]
PlayerMoveToggleCollisionCheat::PlayerMoveToggleCollisionCheat() {
}

// PROVEN: 11ce69b7 IDENTICAL/NONE
// OFFSET: [0x006747A0]
PlayerMoveToggleCollisionCheat::~PlayerMoveToggleCollisionCheat() {
}

// PROVEN: 44ac6577 IDENTICAL/NONE
// OFFSET: 0x00686190
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

// PROVEN: 829a0ec6 IDENTICAL/NONE
// OFFSET: [0x00673400]
PlayerMoveTurnRateChangeAck::PlayerMoveTurnRateChangeAck() {
}

// PROVEN: 62f34d21 IDENTICAL/NONE
// OFFSET: [0x00673DF0]
PlayerMoveTurnRateChangeAck::~PlayerMoveTurnRateChangeAck() {
}

// PROVEN: cec6cc19 IDENTICAL/NONE
// OFFSET: 0x00682430
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

// PROVEN: ebb983d4 IDENTICAL/NONE
// OFFSET: [0x00674AD0]
PlayerMoveUnrootAck::PlayerMoveUnrootAck() {
}

// PROVEN: bbefc9c9 IDENTICAL/NONE
// OFFSET: [0x00673E00]
PlayerMoveUnrootAck::~PlayerMoveUnrootAck() {
}

// PROVEN: afb0d0be IDENTICAL/NONE
// OFFSET: 0x00676B90
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

// PROVEN: e61f7159 IDENTICAL/NONE
// OFFSET: [0x00673D20]
PlayerMoveWalkSpeedChangeAck::PlayerMoveWalkSpeedChangeAck() {
}

// PROVEN: 2b0217ea IDENTICAL/NONE
// OFFSET: [0x00673E10]
PlayerMoveWalkSpeedChangeAck::~PlayerMoveWalkSpeedChangeAck() {
}

// PROVEN: 9561ae1a IDENTICAL/NONE
// OFFSET: 0x00679660
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

// PROVEN: a8863742 IDENTICAL/NONE
// OFFSET: [0x00674A10]
PlayerMoveWaterWalkAck::PlayerMoveWaterWalkAck() {
}

// PROVEN: ed74fa78 IDENTICAL/NONE
// OFFSET: [0x00673B20]
PlayerMoveWaterWalkAck::~PlayerMoveWaterWalkAck() {
}

// PROVEN: 334838d4 IDENTICAL/NONE
// OFFSET: 0x00680030
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

// PROVEN: 3a0270ed IDENTICAL/NONE
// OFFSET: [0x00670BB0]
PlayerSetActiveMover::PlayerSetActiveMover() {
}

// PROVEN: 0d2e09b1 IDENTICAL/NONE
// OFFSET: [0x00670850]
PlayerSetActiveMover::~PlayerSetActiveMover() {
}

// PROVEN: e18a79b1 IDENTICAL/NONE
// OFFSET: 0x00673710
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

// OFFSET: 0x00683780
PlayerMoveUpdateMissileTrajectory::PlayerMoveUpdateMissileTrajectory() : m_hasStatus(false) {
}

// PROVEN: 2066f20e IDENTICAL/NONE
// OFFSET: 0x00684840
PlayerMoveUpdateMissileTrajectory::~PlayerMoveUpdateMissileTrajectory() {
}

// PROVEN: c5c496b4 IDENTICAL/NONE
// OFFSET: 0x0067AB00
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

// OFFSET: 0x00670AE0
PlayerMoveTeleportCheat::PlayerMoveTeleportCheat(const C3Vector& position, float facing) {
    this->m_position = position;
    this->m_facing = facing;
}

// OFFSET: 0x00670570
PlayerMoveTeleportCheat::~PlayerMoveTeleportCheat() {
}

// OFFSET: 0x00674A30
void PlayerMoveTeleportCheat::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x3A10));
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.y);
    msg->Put(this->m_facing);
    msg->Put(this->m_position.z);
}

// OFFSET: 0x006740C0
PlayerMoveSetRawPosition::PlayerMoveSetRawPosition(const C3Vector& position, float facing) {
    this->m_facing = facing;
    this->m_position = position;
}

// OFFSET: 0x00670990
PlayerMoveSetRawPosition::~PlayerMoveSetRawPosition() {
}

// OFFSET: 0x00674420
void PlayerMoveSetRawPosition::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x311A));
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.z);
    msg->Put(this->m_facing);
    msg->Put(this->m_position.y);
}

// OFFSET: 0x00673A10
PlayerMoveCharmPortCheat::PlayerMoveCharmPortCheat(const C3Vector& position, float facing) {
    this->m_facing = facing;
    this->m_position = position;
}

// OFFSET: 0x006709F0
PlayerMoveCharmPortCheat::~PlayerMoveCharmPortCheat() {
}

// OFFSET: 0x00670A80
void PlayerMoveCharmPortCheat::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x7A08));
    msg->Put(this->m_facing);
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.x);
    msg->Put(this->m_position.z);
}
