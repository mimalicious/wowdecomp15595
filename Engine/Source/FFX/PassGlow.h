#ifndef ENGINE_SOURCE_FFX_PASSGLOW_H
#define ENGINE_SOURCE_FFX_PASSGLOW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassGlowBase : public FFX::Pass {
    public:
    /* 004C */ CImVector m_color;

    PassGlowBase(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

class PassGlow : public PassGlowBase {
    public:
    /* 0050 */ uint32_t m_permute;
    /* 0054 */ CGxShader* m_shader;

    virtual ~PassGlow();
    virtual bool Valid();
    virtual void Execute();

    PassGlow(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

class PassGlowFade : public PassGlowBase {
    public:
    /* 0050 */ uint32_t m_permute;
    /* 0054 */ CGxShader* m_shader;
    /* 0058 */ CImVector m_fadeColor;
    /* 005C */ float m_fadeTime;
    /* 0060 */ float m_fadeInTime;
    /* 0064 */ float m_fadeOutTime;
    /* 0068 */ bool m_fadingOut;

    virtual ~PassGlowFade();
    virtual bool Valid();
    virtual void Execute();

    PassGlowFade(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
    void BeginFadeOut();
};

#endif
