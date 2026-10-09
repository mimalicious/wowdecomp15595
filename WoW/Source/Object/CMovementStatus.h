#ifndef WOW_SOURCE_OBJECT_CMOVEMENTSTATUS_H
#define WOW_SOURCE_OBJECT_CMOVEMENTSTATUS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"

class CMovementStatus {
    public:
    struct TransportInfo {
        /* 0000 */ WOWGUID m_guid;
        /* 0008 */ C3Vector m_position;
        /* 0014 */ float m_facing;
        /* 0018 */ uint8_t m_seat;
        /* 001C */ uint32_t m_time;
        /* 0020 */ bool m_hasTime2;
        /* 0024 */ uint32_t m_time2;
        /* 0028 */ bool m_hasTime3;
        /* 002C */ uint32_t m_time3;
    };

    struct FallDirection {
        /* 0000 */ float m_sinAngle;
        /* 0004 */ float m_cosAngle;
        /* 0008 */ float m_xySpeed;
    };

    struct FallInfo {
        /* 0000 */ uint32_t m_time;
        /* 0004 */ float m_zSpeed;
        /* 0008 */ bool m_hasDirection;
        /* 000C */ FallDirection m_direction;
    };

    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_moveFlags;
    /* 000C */ uint16_t m_moveExtraFlags;
    /* 0010 */ uint32_t m_gameTime;
    /* 0014 */ C3Vector m_position;
    /* 0020 */ float m_facing;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ bool m_hasTransport;
    /* 0030 */ TransportInfo m_transport;
    /* 0060 */ float m_pitch;
    /* 0064 */ bool m_hasFall;
    /* 0068 */ FallInfo m_fall;
    /* 0080 */ float m_splineElevation;
    /* 0084 */ bool m_hasSpline;
    /* 0085 */ bool m_heightChangeFailed;

    CMovementStatus();
};

#endif
