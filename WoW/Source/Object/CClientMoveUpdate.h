#ifndef WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H
#define WOW_SOURCE_OBJECT_CCLIENTMOVEUPDATE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"

class CClientMoveTransport {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ float m_facing;
    /* 0018 */ uint8_t m_seat;
    /* 001C */ uint32_t m_time;
    /* 0020 */ bool m_hasTime2;
    /* 0024 */ uint32_t m_time2;
    /* 0028 */ bool m_hasTime3;
    /* 002C */ uint32_t m_time3;

    CClientMoveTransport();
    bool IsValid() const;
};

class CClientMoveFallDirection {
    public:
    /* 0000 */ C2Vector m_direction;
    /* 0008 */ float m_speed;

    bool IsValid() const;
};

class CClientMoveFall {
    public:
    /* 0000 */ uint32_t m_fallTime;
    /* 0004 */ float m_jumpVelocity;
    /* 0008 */ bool m_hasDirection;
    /* 000C */ CClientMoveFallDirection m_direction;

    CClientMoveFall();
    bool IsValid() const;
};

class CClientMoveStatus {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_moveFlags;
    /* 000C */ uint16_t m_moveExtraFlags;
    /* 0010 */ uint32_t m_time;
    /* 0014 */ C3Vector m_position;
    /* 0020 */ float m_facing;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ bool m_hasTransport;
    /* 0030 */ CClientMoveTransport m_transport;
    /* 0060 */ float m_pitch;
    /* 0064 */ bool m_hasFall;
    /* 0068 */ CClientMoveFall m_fall;
    /* 0080 */ float m_splineElevation;
    /* 0084 */ bool unk_0084;
    /* 0085 */ bool unk_0085;

    CClientMoveStatus();
    bool IsValid() const;
};

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
    /* 0000 */ CClientMoveStatus m_status;
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
