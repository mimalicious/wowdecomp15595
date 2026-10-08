#include "WoW/Source/Object/CClientMoveUpdate.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/Types.h"

// INLINED
CClientMoveTransport::CClientMoveTransport() {
    this->m_hasTime2 = false;
    this->m_hasTime3 = false;
}

// OFFSET: 0x00697240
bool CClientMoveTransport::IsValid() const {
    uint32_t low = static_cast<uint32_t>(this->m_guid.m_guid);
    uint32_t high = static_cast<uint32_t>(this->m_guid.m_guid >> 32);

    bool guidValid = !this->m_guid.m_guid
        || (high & 0xF0F00000) == 0xF0500000
        || ((high & 0xF0000000) == 0 && ((high & 0xF07FFFFF) || low))
        || ((high & 0xF0000000) == 0x10000000 && (high & 0x0FF00000) == 0x0FC00000);

    return guidValid
        && (!this->m_hasTime3 || this->m_time3)
        && this->m_facing >= 0.0f
        && this->m_facing < 6.2831864f
        && _finite(this->m_facing)
        && _finite(this->m_position.z)
        && _finite(this->m_position.y)
        && _finite(this->m_position.x);
}

// OFFSET: 0x00696B10
bool CClientMoveFallDirection::IsValid() const {
    if (this->m_speed < 0.0f || !_finite(this->m_speed)) {
        return false;
    }

    float length = this->m_direction.x * this->m_direction.x + this->m_direction.y * this->m_direction.y;

    if (fabs(length - 1.0f) >= 0.000001f && fabs(length) >= 0.000001f) {
        return false;
    }

    return _finite(this->m_direction.y) && _finite(this->m_direction.x);
}

// INLINED
CClientMoveFall::CClientMoveFall() {
    this->m_hasDirection = false;
    this->m_direction.m_direction.x = 0.0f;
    this->m_direction.m_direction.y = 0.0f;
}

// OFFSET: 0x00696E20
bool CClientMoveFall::IsValid() const {
    return static_cast<int32_t>(this->m_fallTime) >= 0 && _finite(this->m_jumpVelocity) && (!this->m_hasDirection || this->m_direction.IsValid());
}

// INLINED
CClientMoveStatus::CClientMoveStatus() {
    this->m_moveFlags = 0;
    this->m_moveExtraFlags = 0;
    this->m_time = 0;
    this->m_facing = 0.0f;
    this->m_hasTransport = false;
    this->m_pitch = 0.0f;
    this->m_hasFall = false;
    this->m_splineElevation = 0.0f;
    this->unk_0084 = false;
    this->unk_0085 = false;
}

// OFFSET: 0x00697340
bool CClientMoveStatus::IsValid() const {
    if (!_finite(this->m_splineElevation) || (this->m_splineElevation != 0.0f && !(this->m_moveFlags & 0x2000000))) {
        return false;
    }

    if ((this->m_moveFlags & 0x800) && (!this->m_hasFall || !this->m_fall.m_hasDirection)) {
        return false;
    }

    if (this->m_pitch >= 3.1415937f || this->m_pitch <= -3.1415937f || !_finite(this->m_pitch)) {
        return false;
    }

    if (this->m_facing < 0.0f || this->m_facing >= 6.2831864f || !_finite(this->m_facing) || !_finite(this->m_position.z) || !_finite(this->m_position.y) || !_finite(this->m_position.x)) {
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

// INLINED
CClientMoveSplineData::CClientMoveSplineData() {
    this->m_hasVerticalAcceleration = false;
    this->m_hasEffectStartTime = false;
}

// OFFSET: 0x00696D60
bool CClientMoveSplineData::IsValid() const {
    return (!this->m_hasVerticalAcceleration || _finite(this->m_verticalAcceleration))
        && _finite(this->m_durationModNext)
        && _finite(this->m_durationMod)
        && (this->m_faceType != 1 || (_finite(this->m_facingPoint.x) && _finite(this->m_facingPoint.y) && _finite(this->m_facingPoint.z)))
        && (this->m_faceType != 0 || _finite(this->m_facingAngle))
        && this->m_faceType <= 3;
}

// INLINED
CClientMoveSpline::CClientMoveSpline() {
    this->m_hasData = false;
}

// OFFSET: 0x006974E0
bool CClientMoveSpline::IsValid() const {
    return _finite(this->m_endPoint.z)
        && _finite(this->m_endPoint.y)
        && _finite(this->m_endPoint.x)
        && (!this->m_hasData || this->m_data.IsValid());
}

// OFFSET: 0x00699DF0
CClientMoveUpdate::CClientMoveUpdate() {
    this->m_hasSpline = false;
}

// OFFSET: 0x00697EF0
bool CClientMoveUpdate::IsValid() const {
    if (this->m_status.unk_0084 && !this->m_hasSpline) {
        return false;
    }

    return fnotequal(this->m_pitchRate, 0.0f)
        && fnotequal(this->m_turnRate, 0.0f)
        && fnotequal(this->m_flightBackSpeed, 0.0f)
        && fnotequal(this->m_flightSpeed, 0.0f)
        && fnotequal(this->m_swimBackSpeed, 0.0f)
        && fnotequal(this->m_swimSpeed, 0.0f)
        && fnotequal(this->m_walkSpeed, 0.0f)
        && fnotequal(this->m_runBackSpeed, 0.0f)
        && fnotequal(this->m_runSpeed, 0.0f)
        && _finite(this->m_pitchRate)
        && _finite(this->m_turnRate)
        && _finite(this->m_flightBackSpeed)
        && _finite(this->m_flightSpeed)
        && _finite(this->m_swimBackSpeed)
        && _finite(this->m_swimSpeed)
        && _finite(this->m_walkSpeed)
        && _finite(this->m_runBackSpeed)
        && _finite(this->m_runSpeed)
        && this->m_status.IsValid()
        && (!this->m_hasSpline || this->m_spline.IsValid());
}
