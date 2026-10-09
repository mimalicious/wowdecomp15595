#ifndef WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H
#define WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/WGUID.h"

class CClientMoveSplineData {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_faceType;
    /* 0008 */ uint32_t m_timePassed;
    /* 000C */ uint32_t m_duration;
    /* 0010 */ float m_durationMod;
    /* 0014 */ float m_durationModNext;
    /* 0018 */ bool m_hasVerticalAcceleration;
    /* 001C */ float m_verticalAcceleration;
    /* 0020 */ bool m_hasEffectStartTime;
    /* 0024 */ uint32_t m_effectStartTime;
    /* 0028 */ TSGrowableArray<C3Vector> m_points;
    /* 0038 */ uint8_t m_mode;
    /* 003C */ float m_facingAngle;
    /* 0040 */ WOWGUID m_facingTarget;
    /* 0048 */ C3Vector m_facingPoint;

    CClientMoveSplineData();
    bool IsValid() const;
};

class CClientMoveSpline {
    public:
    /* 0000 */ uint32_t m_id;
    /* 0004 */ C3Vector m_endPoint;
    /* 0010 */ bool m_hasData;
    /* 0018 */ CClientMoveSplineData m_data;

    CClientMoveSpline();
    bool IsValid() const;
};

class CClientMoveUpdate {
    public:
    /* 0000 */ CMovementStatus m_status;
    /* 0088 */ float m_walkSpeed;
    /* 008C */ float m_runSpeed;
    /* 0090 */ float m_runBackSpeed;
    /* 0094 */ float m_swimSpeed;
    /* 0098 */ float m_swimBackSpeed;
    /* 009C */ float m_flightSpeed;
    /* 00A0 */ float m_flightBackSpeed;
    /* 00A4 */ float m_turnRate;
    /* 00A8 */ float m_pitchRate;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ bool m_hasSpline;
    /* 00B8 */ CClientMoveSpline m_spline;

    CClientMoveUpdate();
    bool IsValid() const;
};

#endif
