#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"

// OFFSET: %0x00803EC0
PassGauss4::PassGauss4(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXGauss4");
}

// OFFSET: %0x00803F20
PassGauss4::~PassGauss4() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: %0x00803F70
bool PassGauss4::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: %0x00803FA0
void PassGauss4::Execute() {
    C2Vector horizontal[4] = { C2Vector(-2.5f, 0.0f), C2Vector(-0.5f, 0.0f), C2Vector(0.5f, 0.0f), C2Vector(2.5f, 0.0f) };
    C2Vector vertical[4] = { C2Vector(0.0f, -2.5f), C2Vector(0.0f, -0.5f), C2Vector(0.0f, 0.5f), C2Vector(0.0f, 2.5f) };

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    FFX::SetupIB();

    if (FFX::s_passThrough_PCT4) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT4->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    FFX::SetupVB_PCT4(white, posMtx, texMtx, horizontal, this->m_inputs[0]->m_texelSize);

    GxRenderTargetSet(GxBuffers_Color, FFX::s_quart[2].m_texture ? TextureGetGxTex(FFX::s_quart[2].m_texture) : NULL, 0);
    GxRenderTargetSet(GxBuffers_Depth, FFX::s_quart[2].m_depthTexture ? TextureGetGxTex(FFX::s_quart[2].m_depthTexture) : NULL, 0);
    GxXformSetViewport(0.0f, static_cast<float>(FFX::s_quart[2].m_size.x) / FFX::s_quart[2].m_texSize.x, 0.0f, static_cast<float>(FFX::s_quart[2].m_size.y) / FFX::s_quart[2].m_texSize.y, 0.0f, 1.0f);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(this->m_inputs[0]->m_texture));
    FFX::Render();

    FFX::SetupVB_PCT4(white, posMtx, texMtx, vertical, this->m_inputs[0]->m_texelSize);

    GxRenderTargetSet(GxBuffers_Color, this->m_output->m_texture ? TextureGetGxTex(this->m_output->m_texture) : NULL, 0);
    GxRenderTargetSet(GxBuffers_Depth, this->m_output->m_depthTexture ? TextureGetGxTex(this->m_output->m_depthTexture) : NULL, 0);
    GxXformSetViewport(0.0f, static_cast<float>(this->m_output->m_size.x) / this->m_output->m_texSize.x, 0.0f, static_cast<float>(this->m_output->m_size.y) / this->m_output->m_texSize.y, 0.0f, 1.0f);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(FFX::s_quart[2].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(FFX::s_quart[2].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(FFX::s_quart[2].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(FFX::s_quart[2].m_texture));
    FFX::Render();

    FFX::SetupReset();
}
