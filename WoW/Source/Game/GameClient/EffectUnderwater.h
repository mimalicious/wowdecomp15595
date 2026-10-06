#ifndef WOW_SOURCE_GAME_GAMECLIENT_EFFECTUNDERWATER_H
#define WOW_SOURCE_GAME_GAMECLIENT_EFFECTUNDERWATER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;
class CTexture;

class PassUnderwater : public FFX::Pass {
    public:
    static CTexture* s_causticsTex[32];
    static CTexture* s_screenMask;
    static int32_t s_texRef;

    /* 004C */ uint32_t m_type;
    /* 0050 */ CGxShader* m_vertexShader;
    /* 0054 */ CGxShader* m_pixelShader;

    virtual ~PassUnderwater();
    virtual bool Valid();
    virtual void Execute();

    PassUnderwater(uint32_t type, FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

class EffectUnderwater : public FFX::Effect {
    public:
    virtual ~EffectUnderwater();

    EffectUnderwater(uint32_t type);
};

#endif
