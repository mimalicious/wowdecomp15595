#ifndef ENGINE_SOURCE_FFX_PASSWATERWINDOW_H
#define ENGINE_SOURCE_FFX_PASSWATERWINDOW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class CGxShader;

class PassWaterWindow : public FFX::Pass {
    public:
    struct VertexConstants {
        /* 0000 */ C44Matrix m_modelView;
        /* 0040 */ C44Matrix m_projection;
        /* 0080 */ C44Matrix m_model;
        /* 00C0 */ C4Vector m_time;

        VertexConstants();
    };

    /* 004C */ int32_t m_submarine;
    /* 0050 */ uint32_t m_type;
    /* 0054 */ CGxShader* m_vertexShader;
    /* 0058 */ CGxShader* m_pixelShader;

    virtual ~PassWaterWindow();
    virtual bool Valid();
    virtual void Execute();

    PassWaterWindow(uint32_t type, int32_t submarine, FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend);
};

#endif
