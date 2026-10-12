#include "EffectUnderwater.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x01012A80
CTexture* PassUnderwater::s_causticsTex[32];

// STATIC: 0x01012B00
CTexture* PassUnderwater::s_screenMask;

// STATIC: 0x01012B04
int32_t PassUnderwater::s_texRef;

// OFFSET: 0x00B49CF0/0x140919CF0
EffectUnderwater::EffectUnderwater(uint32_t type) {
    FFX::Tex* inputs[] = { &FFX::s_full[0] };
    FFX::Pass* pass = new (__FILE__, __LINE__) PassUnderwater(type, inputs, 1, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// INLINED
EffectUnderwater::~EffectUnderwater() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// OFFSET: [0x00B49D90/0x140919D90, 0x00B49E40]
PassUnderwater::~PassUnderwater() {
    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }

    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }

    if (this->m_type) {
        PassUnderwater::s_texRef--;
    }

    if (PassUnderwater::s_texRef) {
        return;
    }

    for (uint32_t i = 0; i < 32; i++) {
        if (PassUnderwater::s_causticsTex[i]) {
            TextureRelease(PassUnderwater::s_causticsTex[i]);
            PassUnderwater::s_causticsTex[i] = NULL;
        }
    }

    if (PassUnderwater::s_screenMask) {
        TextureRelease(PassUnderwater::s_screenMask);
        PassUnderwater::s_screenMask = NULL;
    }
}

// OFFSET: 0x00B49E70/0x140919EE0
bool PassUnderwater::Valid() {
    int32_t valid = this->m_vertexShader ? this->m_vertexShader->m_permutes[0].Valid() : 0;

    for (uint32_t i = 0; i < 6; i++) {
        valid = valid && this->m_pixelShader && this->m_pixelShader->m_permutes[i].Valid();
    }

    return valid != 0;
}

// OFFSET: 0x00B49EC0/0x140919F50
void PassUnderwater::Execute() {
    CGxTex* caustics = NULL;

    if (this->m_type >= 4) {
        caustics = TextureGetGxTex(PassUnderwater::s_causticsTex[(FFX::s_curTimeMs / 50) & 0x1F]);

        if (!caustics) {
            return;
        }
    }

    CGxTex* screenMask = TextureGetGxTex(PassUnderwater::s_screenMask);

    if (!screenMask) {
        return;
    }

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));

    if (this->m_type >= 4) {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(DepthCache::GetDepthSR()));
        g_theGxDevicePtr->RsSet(GxRs_Texture2, caustics);
    }

    FFX::SetupVB_PT(posMtx, texMtx);
    FFX::SetupIB();

    int32_t sunShafts = SunShafts::IsEnabled();

    if (sunShafts) {
        g_theGxDevicePtr->RsSet(GxRs_Texture3, SunShafts::GetBuffer());
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture4, screenMask);

    if (this->m_vertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    int32_t permute = 0;

    if (this->m_type >= 4) {
        permute = sunShafts + 2 * DepthCache::GetDepthMode() - 2;
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[permute]);

    FFX::Tex* input = this->m_inputs[0];
    FFX::psProcWaterConsts[7].x = static_cast<float>(input->m_size.x);
    FFX::psProcWaterConsts[7].y = static_cast<float>(input->m_size.y);
    FFX::psProcWaterConsts[7].z = input->m_texelSize.x;
    FFX::psProcWaterConsts[7].w = input->m_texelSize.y;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &FFX::psProcWaterConsts[0].x, 15);

    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B4A170/0x14091A300
PassUnderwater::PassUnderwater(uint32_t type, FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_type = type;
    this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "FFXUnderWater");

    if (this->m_type < 4) {
        this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXUnderWaterLow");
    } else {
        this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXUnderWaterHigh");

        if (!PassUnderwater::s_texRef) {
            CGxTexFlags flags;
            flags.m_wrapU = 1;
            flags.m_wrapV = 1;

            CStatus status;

            for (uint32_t i = 0; i < 32; i++) {
                char name[1024];
                SStrPrintf(name, sizeof(name), "XTextures\\caustic\\caustics_%03d", i + 1);
                PassUnderwater::s_causticsTex[i] = TextureCreate(name, flags, &status, 0, NULL, 2);
            }
        }
    }

    if (this->m_type) {
        CStatus status;
        PassUnderwater::s_screenMask = TextureCreate("xtextures\\fx\\screenmask.blp", CGxTexFlags(), &status, 0, NULL, 2);
        PassUnderwater::s_texRef++;
    }
}
