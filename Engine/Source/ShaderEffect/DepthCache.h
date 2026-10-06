#ifndef ENGINE_SOURCE_SHADEREFFECT_DEPTHCACHE_H
#define ENGINE_SOURCE_SHADEREFFECT_DEPTHCACHE_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"

class CTexture;

namespace DepthCache {
    enum EDepthMode {
        DM_Unknown = 0,
        DM_Texture = 3,
    };

    void AddRef();
    EDepthMode GetDepthMode();
    CTexture* GetDepthSR();
    int32_t IsSupported(EDepthMode* mode, EGxTexFormat* format, EGxTexFormat* resolveFormat);
    bool IsValid();
    void Release();
    void __fastcall Reset();
    void Resolve();
    void Validate();
}

#endif
