#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSCOLORSHIFT_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSCOLORSHIFT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Tempest/C4Vector.h"

class CGxShader;

class PassColorShift : public FFX::Pass {
    public:
    /* 004C */ uint32_t m_permute;
    /* 0050 */ C4Vector m_matrix[3];
    /* 0080 */ CGxShader* m_shader;

    virtual ~PassColorShift();
    virtual bool Valid();
    virtual void Execute();
    virtual void SetParameters(int32_t count, const float* params);

    PassColorShift(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
