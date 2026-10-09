#ifndef ENGINE_SOURCE_SHADEREFFECT_SUNSHAFTS_H
#define ENGINE_SOURCE_SHADEREFFECT_SUNSHAFTS_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CGxTex;

namespace SunShafts {
    void Create();
    void Destroy();
    CGxTex* GetBuffer();
    int32_t IsEnabled();
    int32_t ModeNeedsDepthCache(uint32_t mode);
    int32_t SetMode(uint32_t mode);
    void Update(const C3Vector& eye, const C3Vector& sun);
    uint32_t ValidateMode(uint32_t mode);
}

#endif
