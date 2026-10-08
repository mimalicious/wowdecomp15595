#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3WATER_H
#define WOW_SOURCE_SOUNDINTERFACE_SI3WATER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/SoundInterface/SI3.h"

struct SI3WaterSound {
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ int32_t m_soundKitId;
    /* 0008 */ C3Vector m_direction;
    /* 0014 */ float m_nearestDistanceSq;
    /* 0018 */ float m_distanceCutoff;
    /* 001C */ int32_t m_active;
};

#endif
