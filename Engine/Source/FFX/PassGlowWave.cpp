#include "Engine/Source/FFX/PassGlowWave.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"

// OFFSET: %0x00803580
PassGlowWave::~PassGlowWave() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }

    if (this->m_maskTex) {
        TextureRelease(this->m_maskTex);
        this->m_maskTex = NULL;
    }
}

// OFFSET: %0x008035F0
bool PassGlowWave::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: %0x00803620
void PassGlowWave::Execute() {
    CGxTex* mask = TextureGetGxTex(this->m_maskTex);

    if (!mask) {
        return;
    }

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix waveMtx;
    FFX::SetupTexMtx(FFX::s_waveTex.m_size, FFX::s_waveTex.m_texSize, &waveMtx, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(FFX::s_waveTex.m_texture));
    waveMtx = waveMtx * FFX::GetWaveMtx(this->m_output->m_size);

    C44Matrix tex0Mtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &tex0Mtx, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[0]->m_texture));

    C44Matrix tex1Mtx;
    FFX::SetupTexMtx(this->m_inputs[1]->m_size, this->m_inputs[1]->m_texSize, &tex1Mtx, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(this->m_inputs[1]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, mask);

    FFX::SetupVB_PCT3(this->m_color, posMtx, waveMtx, tex0Mtx, tex1Mtx);
    FFX::SetupIB();

    if (FFX::s_passThrough_PCT3) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT3->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    C4Vector amplitude0 = FFX::GetWaveAmplitude(this->m_inputs[0]->m_texelSize, 1.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &amplitude0.x, 1);

    C4Vector amplitude1 = FFX::GetWaveAmplitude(this->m_inputs[1]->m_texelSize, 0.25f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, &amplitude1.x, 1);

    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: %0x00803DF0
PassGlowWave::PassGlowWave(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : PassGlowBase(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    if (!g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Uv88]) {
        this->m_permute += 2;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXGlowWave");

    CStatus status;
    this->m_maskTex = TextureCreate("xtextures\\fx\\screenmask.blp", CGxTexFlags(), &status, 0, NULL, 2);
}
