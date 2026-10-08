#include "Engine/Source/FFX/PassGlow.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"

// INLINED
PassGlowBase::PassGlowBase(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_color = CImVector(0, 0, 0, 0);
}

// OFFSET: 0x0089EA20
float PassGlowFade::GetFadeAmount() {
    float time = this->m_fadeTime;

    if (this->m_fadingOut && time > this->m_fadeOutTime) {
        time = this->m_fadeOutTime;
    }

    float amount = 0.0f;

    if (this->m_fadingOut) {
        if (this->m_fadeOutTime > 0.0f) {
            amount = time / this->m_fadeOutTime;
        }
    } else if (this->m_fadeInTime > 0.0f) {
        amount = time / this->m_fadeInTime;
    }

    return amount;
}

// PROVEN: bd74b2a8 IDENTICAL/NONE
// OFFSET: 0x0089EA90
void PassGlowFade::BeginFadeOut() {
    this->m_fadingOut = true;
}

// OFFSET: 0x0089EAC0/0x1405DF6E0
PassGlow::PassGlow(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : PassGlowBase(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXGlow");
}

// OFFSET: 0x0089EB20/0x1405DF7C0
PassGlowFade::PassGlowFade(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : PassGlowBase(inputs, count, output, blend) {
    this->m_fadeColor = CImVector(0, 0, 0, 0);
    this->m_fadingOut = false;
    this->m_fadeTime = 0.0f;
    this->m_fadeInTime = 0.0f;
    this->m_fadeOutTime = 0.0f;
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXGlowFade");
}

// OFFSET: 0x0089EB90/0x1405DF850
PassGlow::~PassGlow() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x0089EBE0/0x1405DF8C0
void PassGlow::Execute() {
    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix tex0Mtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &tex0Mtx, 1);

    C44Matrix tex1Mtx;
    FFX::SetupTexMtx(this->m_inputs[1]->m_size, this->m_inputs[1]->m_texSize, &tex1Mtx, 1);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[1]->m_texture));

    if (FFX::s_passThrough_PCT2) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT2->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    FFX::SetupVB_PCT2(this->m_color, posMtx, tex0Mtx, tex1Mtx);
    FFX::SetupIB();
    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x0089EEC0/0x1405DFC20
bool PassGlow::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x0089EEC0/0x1405DFC20
bool PassGlowFade::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x0089F2F0/0x1405E01E0
PassGlowFade::~PassGlowFade() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x0089F340/0x1405E0250
void PassGlowFade::Execute() {
    float step = FFX::s_tickTimeSec;

    if (this->m_fadingOut) {
        step = -step;
    }

    this->m_fadeTime = this->m_fadeTime - step;

    if (this->m_fadeTime < 0.0f) {
        this->m_fadeTime = 0.0f;
    }

    float time = this->m_fadeTime;
    float ratio = 0.0f;

    if (this->m_fadingOut) {
        if (time > this->m_fadeOutTime) {
            time = this->m_fadeOutTime;
        }

        if (this->m_fadeOutTime > 0.0f) {
            ratio = time / this->m_fadeOutTime;
        }
    } else if (this->m_fadeInTime > 0.0f) {
        ratio = time / this->m_fadeInTime;
    }

    CImVector color = this->m_color;
    float fade = (1.0f - ratio) * 255.0f;
    color.r = static_cast<uint8_t>(static_cast<int32_t>(fade));
    color.b = static_cast<uint8_t>(static_cast<int32_t>(this->m_color.b * ratio + fade));

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix tex0Mtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &tex0Mtx, 1);

    C44Matrix tex1Mtx;
    FFX::SetupTexMtx(this->m_inputs[1]->m_size, this->m_inputs[1]->m_texSize, &tex1Mtx, 1);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[1]->m_texture));

    if (FFX::s_passThrough_PCT2) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT2->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    CImVector fadeColor;

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        fadeColor.b = this->m_fadeColor.r;
        fadeColor.g = this->m_fadeColor.g;
        fadeColor.r = this->m_fadeColor.b;
        fadeColor.a = this->m_fadeColor.a;
    } else {
        fadeColor = this->m_fadeColor;
    }

    C4Vector constant(fadeColor);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &constant.x, 1);

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    FFX::SetupVB_PCT2(color, posMtx, tex0Mtx, tex1Mtx);
    FFX::SetupIB();
    FFX::Render();
    FFX::SetupReset();
}
