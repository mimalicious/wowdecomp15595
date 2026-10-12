#ifndef WOW_SOURCE_OBJECT_MOVEMENTSHARED_INL
#define WOW_SOURCE_OBJECT_MOVEMENTSHARED_INL

#include "WoW/Source/Object/CMoveSpline.h"

// INLINED
inline int32_t CMovementShared::IsOnFallingSpline() const {
    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FALLING)) {
        return 1;
    }

    return 0;
}

// INLINED
inline int32_t CMovementShared::IsSplineFlyerOrFlyingOrSwimming() const {
    if (this->IsGravityDisabled()) {
        return 1;
    }

    return (this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) != 0;
}

// INLINED
inline int32_t CMovementShared::StopStrafe(uint32_t moveFlags) {
    if (!(moveFlags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) || (this->m_flags & MOVEMENTFLAG_FALLING)) {
        return 0;
    }

    this->ForceStopStrafe();
    return 1;
}

// INLINED
inline int32_t CMovementShared::StopMove(uint32_t moveFlags) {
    if (!(moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) || (this->m_flags & MOVEMENTFLAG_FALLING)) {
        return 0;
    }

    this->ForceStopMove(1);
    return 1;
}

#endif
