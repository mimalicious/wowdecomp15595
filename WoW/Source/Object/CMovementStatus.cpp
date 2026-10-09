#include "WoW/Source/Object/CMovementStatus.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/Types.h"

// OFFSET: 0x00696B10
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

// OFFSET: 0x00696E20
bool CMovementStatus::FallInfo::IsValid() const {
    return static_cast<int32_t>(this->m_time) >= 0 && _finite(this->m_zSpeed) && (!this->m_hasDirection || this->m_direction.IsValid());
}

// OFFSET: 0x00697240
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

// OFFSET: 0x00697340
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

// PROVEN: 3c392ee9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
CMovementStatus::~CMovementStatus() {
}
