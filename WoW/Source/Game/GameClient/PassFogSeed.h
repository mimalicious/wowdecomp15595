#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassFogSeed : public FFX::Pass {
    public:
    static int32_t s_deviceReset;

    static void __fastcall DeviceRestoredCallback();

    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;
    /* 0054 */ uint32_t m_row;
    /* 0058 */ CImVector m_color;

    virtual ~PassFogSeed();
    virtual bool Valid();
    virtual void Execute();

    PassFogSeed(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
