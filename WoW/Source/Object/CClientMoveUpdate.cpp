#include "WoW/Source/Object/CClientMoveUpdate.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/Types.h"

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
    if (this->m_status.m_hasSpline && !this->m_hasSpline) {
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
