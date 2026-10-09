#ifndef WOW_SOURCE_TYPES_H
#define WOW_SOURCE_TYPES_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"

class VideoHardwareRec;

struct DefaultSettings {
    /* 0000 */ uint32_t m_graphicsQuality;
};

struct Hardware {
    struct Device {
        /* 0000 */ uint16_t m_vendorID;
        /* 0002 */ uint16_t m_deviceID;
        /* 0004 */ uint32_t m_driverVersionHi;
        /* 0008 */ uint32_t m_driverVersionLo;
    };

    /* 0000 */ Hardware::Device m_videoDevice;
    /* 000C */ Hardware::Device m_soundDevice;
    /* 0018 */ uint32_t m_cpuIdx;
    /* 001C */ uint32_t m_videoID;
    /* 0020 */ uint32_t m_soundIdx;
    /* 0024 */ uint32_t m_memIdx;
    /* 0028 */ VideoHardwareRec* m_videoHw;
};

#endif
