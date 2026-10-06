#include <cmath>
#include <cstdint>
#include "Engine/Source/Gx/CGxGammaRamp.h"

// PROVEN: f3415e60 IDENTICAL/IDENTICAL
// OFFSET: 0x008431A0/0x14056F670
void CGxGammaRamp::Set(float gamma) {
    for (uint32_t i = 0; i < 256; i++) {
        uint16_t value = static_cast<uint16_t>(pow(i * 0.0039215689f, gamma) * 65535.0f);
        this->m_red[i] = value;
        this->m_green[i] = value;
        this->m_blue[i] = value;
    }
}
