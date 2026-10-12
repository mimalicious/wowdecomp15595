#include "WoW/Source/Object/CMovementStatus.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

// OFFSET: 0x00696B10/0x140342160
bool CMovementStatus::FallDirection::IsValid() const {
    if (this->m_xySpeed < 0.0f || !_finite(this->m_xySpeed)) {
        return false;
    }

    float length = this->m_sinAngle * this->m_sinAngle + this->m_cosAngle * this->m_cosAngle;

    if (fabs(length - 1.0f) >= 0.000001f && fabs(length) >= 0.000001f) {
        return false;
    }

    return _finite(this->m_cosAngle) && _finite(this->m_sinAngle);
}

// OFFSET: 0x00696E20/0x140342470
bool CMovementStatus::FallInfo::IsValid() const {
    return static_cast<int32_t>(this->m_time) >= 0 && _finite(this->m_zSpeed) && (!this->m_hasDirection || this->m_direction.IsValid());
}

// OFFSET: 0x00697240/0x140342830
bool CMovementStatus::TransportInfo::IsValid() const {
    uint32_t low = static_cast<uint32_t>(this->m_guid.m_guid);
    uint32_t high = static_cast<uint32_t>(this->m_guid.m_guid >> 32);

    bool guidValid = !this->m_guid.m_guid
        || (high & 0xF0F00000) == 0xF0500000
        || ((high & 0xF0000000) == 0 && ((high & 0xF07FFFFF) || low))
        || ((high & 0xF0000000) == 0x10000000 && (high & 0x0FF00000) == 0x0FC00000);

    return guidValid
        && (!this->m_hasVehicleId || this->m_vehicleId)
        && this->m_facing >= 0.0f
        && this->m_facing < 6.2831864f
        && _finite(this->m_facing)
        && _finite(this->m_position.z)
        && IsFinite(this->m_position.y)
        && IsFinite(this->m_position.x);
}

// OFFSET: 0x00697340/0x140342930
bool CMovementStatus::IsValid() const {
    if (!_finite(this->m_splineElevation) || (this->m_splineElevation != 0.0f && !(this->m_moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION))) {
        return false;
    }

    if ((this->m_moveFlags & MOVEMENTFLAG_FALLING) && (!this->m_hasFall || !this->m_fall.m_hasDirection)) {
        return false;
    }

    if (this->m_pitch >= 3.1415937f || this->m_pitch <= -3.1415937f || !_finite(this->m_pitch)) {
        return false;
    }

    if (this->m_facing < 0.0f || this->m_facing >= 6.2831864f || !IsFinite(this->m_facing) || !IsFinite(this->m_position.z) || !IsFinite(this->m_position.y) || !IsFinite(this->m_position.x)) {
        return false;
    }

    uint32_t low = static_cast<uint32_t>(this->m_guid.m_guid);
    uint32_t high = static_cast<uint32_t>(this->m_guid.m_guid >> 32);

    bool guidValid = (high & 0xF0F00000) == 0xF0300000
        || (high & 0xF0F00000) == 0xF0500000
        || ((high & 0xF0000000) == 0 && ((high & 0xF07FFFFF) || low))
        || (high & 0xF0F00000) == 0xF0400000;

    return guidValid
        && (!this->m_hasFall || this->m_fall.IsValid())
        && (!this->m_hasTransport || this->m_transport.IsValid());
}

// OFFSET: 0x00697900/0x140342EF0
void CMovementStatus::WriteEmbedded(CDataStore* msg) const {
    const uint8_t* guid = reinterpret_cast<const uint8_t*>(&this->m_guid);
    const uint8_t* transportGuid = reinterpret_cast<const uint8_t*>(&this->m_transport.m_guid);
    msg->Put(this->m_position.z);
    msg->Put(this->m_position.y);
    msg->Put(this->m_position.x);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_hasFall);
    bits.WriteBit(this->m_gameTime == 0);
    bits.WriteBit(this->m_facing == 0.0f);
    bits.WriteBit(this->m_heightChangeFailed);
    bits.WriteBit(this->m_hasSpline);
    bits.WriteBit(guid[6] != 0);
    bits.WriteBit(guid[4] != 0);
    bits.WriteBit(this->m_moveExtraFlags == 0);
    bits.WriteBit(guid[3] != 0);
    bits.WriteBit(guid[5] != 0);
    bits.WriteBit(this->m_splineElevation == 0.0f);
    bits.WriteBit(this->m_pitch == 0.0f);
    bits.WriteBit(guid[7] != 0);
    bits.WriteBit(this->m_hasTransport);
    bits.WriteBit(guid[2] != 0);
    bits.WriteBit(this->m_moveFlags == 0);
    bits.WriteBit(guid[1] != 0);
    bits.WriteBit(guid[0] != 0);

    if (this->m_hasTransport) {
        bits.WriteBit(transportGuid[6] != 0);
        bits.WriteBit(transportGuid[2] != 0);
        bits.WriteBit(transportGuid[5] != 0);
        bits.WriteBit(this->m_transport.m_hasTime2);
        bits.WriteBit(transportGuid[7] != 0);
        bits.WriteBit(transportGuid[4] != 0);
        bits.WriteBit(this->m_transport.m_hasVehicleId);
        bits.WriteBit(transportGuid[0] != 0);
        bits.WriteBit(transportGuid[1] != 0);
        bits.WriteBit(transportGuid[3] != 0);
    }

    if (this->m_moveExtraFlags) {
        bits.WriteBits<12>(this->m_moveExtraFlags);
    }

    if (this->m_moveFlags) {
        bits.WriteBits<30>(this->m_moveFlags);
    }

    if (this->m_hasFall) {
        bits.WriteBit(this->m_fall.m_hasDirection);
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

    if (guid[3]) {
        msg->Put(static_cast<uint8_t>(guid[3] ^ 1));
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

    if (guid[6]) {
        msg->Put(static_cast<uint8_t>(guid[6] ^ 1));
    }

    if (this->m_hasTransport) {
        msg->Put(this->m_transport.m_seat);
        msg->Put(this->m_transport.m_facing);
        msg->Put(this->m_transport.m_time);

        if (transportGuid[6]) {
            msg->Put(static_cast<uint8_t>(transportGuid[6] ^ 1));
        }

        if (transportGuid[5]) {
            msg->Put(static_cast<uint8_t>(transportGuid[5] ^ 1));
        }

        if (this->m_transport.m_hasVehicleId) {
            msg->Put(this->m_transport.m_vehicleId);
        }

        msg->Put(this->m_transport.m_position.x);

        if (transportGuid[4]) {
            msg->Put(static_cast<uint8_t>(transportGuid[4] ^ 1));
        }

        msg->Put(this->m_transport.m_position.z);

        if (transportGuid[2]) {
            msg->Put(static_cast<uint8_t>(transportGuid[2] ^ 1));
        }

        if (transportGuid[0]) {
            msg->Put(static_cast<uint8_t>(transportGuid[0] ^ 1));
        }

        if (this->m_transport.m_hasTime2) {
            msg->Put(this->m_transport.m_time2);
        }

        if (transportGuid[1]) {
            msg->Put(static_cast<uint8_t>(transportGuid[1] ^ 1));
        }

        if (transportGuid[3]) {
            msg->Put(static_cast<uint8_t>(transportGuid[3] ^ 1));
        }

        msg->Put(this->m_transport.m_position.y);

        if (transportGuid[7]) {
            msg->Put(static_cast<uint8_t>(transportGuid[7] ^ 1));
        }
    }

    if (this->m_facing != 0.0f) {
        msg->Put(this->m_facing);
    }

    if (this->m_splineElevation != 0.0f) {
        msg->Put(this->m_splineElevation);
    }

    if (this->m_hasFall) {
        msg->Put(this->m_fall.m_time);

        if (this->m_fall.m_hasDirection) {
            msg->Put(this->m_fall.m_direction.m_xySpeed);
            msg->Put(this->m_fall.m_direction.m_sinAngle);
            msg->Put(this->m_fall.m_direction.m_cosAngle);
        }

        msg->Put(this->m_fall.m_zSpeed);
    }

    if (this->m_gameTime) {
        msg->Put(this->m_gameTime);
    }

    if (this->m_pitch != 0.0f) {
        msg->Put(this->m_pitch);
    }
}

// OFFSET: 0x00698280/0x1403439D0
CMovementStatus::CMovementStatus() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_hasTransport = false;
    this->m_transport.m_position.x = 0.0f;
    this->m_transport.m_position.y = 0.0f;
    this->m_transport.m_position.z = 0.0f;
    this->m_transport.m_hasTime2 = false;
    this->m_transport.m_hasVehicleId = false;
    this->m_hasFall = false;
    this->m_fall.m_hasDirection = false;
    this->m_fall.m_direction.m_sinAngle = 0.0f;
    this->m_fall.m_direction.m_cosAngle = 0.0f;
    this->m_moveFlags = 0;
    this->m_moveExtraFlags = 0;
    this->m_gameTime = 0;
    this->m_facing = 0.0f;
    this->m_pitch = 0.0f;
    this->m_splineElevation = 0.0f;
    this->m_hasSpline = false;
    this->m_heightChangeFailed = false;
}

// OFFSET: 0x00401000/0x1407D7E80
CMovementStatus::~CMovementStatus() {
}
