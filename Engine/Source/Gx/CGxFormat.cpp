#include <cstdint>
#include "Engine/Source/Gx/CGxFormat.h"

// OFFSET: 0x00840780/0x14056C4E0
CGxFormat::CGxFormat() {
    this->m_hwTnL = true;
    this->m_hwCursor = true;
    this->m_fixLag = 0;
    this->m_window = 1;
    this->m_aspect = true;
    this->m_maximize = 0;
    this->m_monitor = 0;
    this->m_depthFormat = Fmt_Ds24X;
    this->m_size.x = 800;
    this->m_size.y = 600;
    this->m_backBufferCount = 1;
    this->m_multisampleCount = 1;
    this->m_multisampleQuality = 0.0f;
    this->m_colorFormat = Fmt_ArgbX888;
    this->m_refreshRate = 60;
    this->m_refreshRateDenom = 1;
    this->m_vsync = 1;
    this->m_stereoEnabled = false;
    this->m_shaderModel = -1;
    this->m_pos.x = 0;
    this->m_pos.y = 0;
}

// OFFSET: 0x008407F0/0x14056C560
CGxFormat::CGxFormat(bool window, const C2iVector& size, CGxFormat::Format colorFormat, CGxFormat::Format depthFormat, uint32_t refreshRate, uint32_t refreshRateDenom, uint32_t vsync, bool hwTnL, bool fixLag, bool hwCursor, bool aspect, int32_t maximize, int32_t monitor) {
    this->m_hwTnL = hwTnL;
    this->m_fixLag = fixLag;
    this->m_hwCursor = hwCursor;
    this->m_aspect = aspect;
    this->m_window = window;
    this->m_monitor = monitor;
    this->m_maximize = maximize;
    this->m_depthFormat = depthFormat;
    this->m_size = size;
    this->m_backBufferCount = 1;
    this->m_multisampleCount = 1;
    this->m_refreshRate = refreshRate;
    this->m_colorFormat = colorFormat;
    this->m_vsync = vsync;
    this->m_multisampleQuality = 0.0f;
    this->m_refreshRateDenom = refreshRateDenom;
    this->m_stereoEnabled = false;
    this->m_shaderModel = -1;
    this->m_pos.x = 0;
    this->m_pos.y = 0;
}
