#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"

// OFFSET: 0x0089FCE0/0x1405E0DA0
PassBox4::PassBox4(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXBox4");
}

// OFFSET: 0x0089FD40/0x1405E0E10
PassBox4::~PassBox4() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x0089F840/0x1405E08C0
bool PassBox4::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x0089FD90/0x1405E0E80
void PassBox4::Execute() {
    // STATIC: %0x00D9558C
    static const C2Vector s_offsets[4] = { C2Vector(-1.5f, -1.5f), C2Vector(0.5f, -1.5f), C2Vector(0.5f, 0.5f), C2Vector(-1.5f, 0.5f) };

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    FFX::SetupVB_PCT4(white, posMtx, texMtx, s_offsets, this->m_inputs[0]->m_texelSize);
    FFX::SetupIB();

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(this->m_inputs[0]->m_texture));

    if (FFX::s_passThrough_PCT4) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT4->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    FFX::Render();
    FFX::SetupReset();
}
