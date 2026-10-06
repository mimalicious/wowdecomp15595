#ifndef ENGINE_SOURCE_FFX_PASSBOX4_H
#define ENGINE_SOURCE_FFX_PASSBOX4_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassBox4 : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;

    virtual ~PassBox4();
    virtual bool Valid();
    virtual void Execute();

    PassBox4(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
