#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSPROPAGATEFOG_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSPROPAGATEFOG_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CGxShader;

class PassPropagateFog : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ CGxShader* m_shader;
    /* 0054 */ C2Vector m_params[4];
    /* 0074 */ float m_strength;

    virtual ~PassPropagateFog();
    virtual bool Valid();
    virtual void Execute();

    PassPropagateFog(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, const C2Vector* params, EGxBlend blend);
};

#endif
