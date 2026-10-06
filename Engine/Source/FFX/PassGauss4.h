#ifndef ENGINE_SOURCE_FFX_PASSGAUSS4_H
#define ENGINE_SOURCE_FFX_PASSGAUSS4_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassGauss4 : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;

    virtual ~PassGauss4();
    virtual bool Valid();
    virtual void Execute();

    PassGauss4(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
