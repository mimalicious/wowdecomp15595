#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSDEATH_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSDEATH_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassDeath : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;
    /* 0054 */ CGxShader* m_nvts2dShader;
    /* 0058 */ CGxShader* m_nvtsRectShader;
    /* 005C */ uint8_t m_alpha;

    virtual ~PassDeath();
    virtual bool Valid();
    virtual void Execute();

    PassDeath(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

class PassDeathNVRC : public FFX::Pass {
    public:
    /* 004C */ CGxShader* m_shader;

    virtual ~PassDeathNVRC();
    virtual bool Valid();
    virtual void Execute();

    PassDeathNVRC(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
