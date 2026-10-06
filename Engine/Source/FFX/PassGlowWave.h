#ifndef ENGINE_SOURCE_FFX_PASSGLOWWAVE_H
#define ENGINE_SOURCE_FFX_PASSGLOWWAVE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/FFX/PassGlow.h"

class CGxShader;

class PassGlowWave : public PassGlowBase {
    public:
    /* 0050 */ uint32_t m_permute;
    /* 0054 */ CGxShader* m_shader;
    /* 0058 */ CTexture* m_maskTex;

    virtual ~PassGlowWave();
    virtual bool Valid();
    virtual void Execute();

    PassGlowWave(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
