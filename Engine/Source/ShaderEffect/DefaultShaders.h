#ifndef ENGINE_SOURCE_SHADEREFFECT_DEFAULTSHADERS_H
#define ENGINE_SOURCE_SHADEREFFECT_DEFAULTSHADERS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"

class CImVector;

namespace DefaultShaders {
    void Destroy();
    void GetFogColor(CImVector& color);
    void Initialize();
    int32_t SetDefaultShaders(EGxVertexBufferFormat format, uint32_t textureCount);
    void SetFogEnabled(int32_t enabled);
    void SetFogParams(float start, float end, float density, const CImVector& color);
    int32_t SetPixelShader(EGxVertexBufferFormat format, uint32_t textureCount);
}

#endif
