#ifndef ENGINE_SOURCE_GX_CGXFORMAT_H
#define ENGINE_SOURCE_GX_CGXFORMAT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2iVector.h"

class CGxFormat {
    public:
    enum Format {
        Fmt_Rgb565 = 0,
        Fmt_ArgbX888 = 1,
        Fmt_Argb8888 = 2,
        Fmt_Argb2101010 = 3,
        Fmt_Ds160 = 4,
        Fmt_Ds24X = 5,
        Fmt_Ds248 = 6,
        Fmt_Ds320 = 7,
        Formats_Last = 8,
    };

    /* 0000 */ uint32_t m_apiSpecificModeID;
    /* 0004 */ bool m_hwTnL;
    /* 0005 */ bool m_hwCursor;
    /* 0006 */ int8_t m_fixLag;
    /* 0007 */ int8_t m_window;
    /* 0008 */ bool m_aspect;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ int32_t m_maximize;
    /* 0010 */ int32_t m_monitor;
    /* 0014 */ CGxFormat::Format m_depthFormat;
    /* 0018 */ C2iVector m_size;
    /* 0020 */ uint32_t m_backBufferCount;
    /* 0024 */ uint32_t m_multisampleCount;
    /* 0028 */ float m_multisampleQuality;
    /* 002C */ bool unk_002C;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ CGxFormat::Format m_colorFormat;
    /* 0034 */ uint32_t m_refreshRate;
    /* 0038 */ uint32_t m_refreshRateDenom;
    /* 003C */ uint32_t m_scanlineOrdering;
    /* 0040 */ uint32_t m_scaling;
    /* 0044 */ uint32_t m_vsync;
    /* 0048 */ bool m_stereoEnabled;
    /* 0049 */ uint8_t unk_003D[3];
    /* 004C */ int32_t m_shaderModel;
    /* 0050 */ C2iVector m_pos;

    CGxFormat();
    CGxFormat(bool window, const C2iVector& size, CGxFormat::Format colorFormat, CGxFormat::Format depthFormat, uint32_t refreshRate, uint32_t refreshRateDenom, uint32_t vsync, bool hwTnL, bool fixLag, bool hwCursor, bool aspect, int32_t maximize, int32_t monitor);
};

#endif
