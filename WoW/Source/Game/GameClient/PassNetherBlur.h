#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Tempest/CRandom.h"

class CGxShader;

class PassNetherBlur : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ float m_noise[3][36];
    /* 0200 */ float m_shade[36];
    /* 0290 */ float* m_noiseTables[3];
    /* 029C */ float m_blend;
    /* 02A0 */ CRndSeed m_seed;
    /* 02A8 */ CGxShader* m_vertexShader;
    /* 02AC */ CGxShader* m_pixelShader;

    virtual ~PassNetherBlur();
    virtual bool Valid();
    virtual void Execute();

    PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

class PassNetherCombine : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;
    /* 0054 */ float m_fade;

    virtual ~PassNetherCombine();
    virtual bool Valid();
    virtual void Execute();

    PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
