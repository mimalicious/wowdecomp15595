#include "Engine/Source/FFX/PassWaterWindow.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/Storm/H/storm.h"

static CTexture* s_bumpTex;

// STATIC: 0x00EC72A8
static CTexture* s_oceanTex[30];

// STATIC: 0x00EC7320
static int32_t s_refCount;

// OFFSET: [0x0089DFE0/0x1405DEA50, 0x0089E1D0]
PassWaterWindow::~PassWaterWindow() {
    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }

    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }

    if (--s_refCount) {
        return;
    }

    for (uint32_t i = 0; i < 30; i++) {
        if (s_oceanTex[i]) {
            TextureRelease(s_oceanTex[i]);
            s_oceanTex[i] = NULL;
        }
    }

    if (s_bumpTex) {
        TextureRelease(s_bumpTex);
        s_bumpTex = NULL;
    }
}

// OFFSET: 0x0089E080/0x1405DEB30
PassWaterWindow::VertexConstants::VertexConstants() {
    this->m_time.x = 0.0f;
    this->m_time.y = 0.0f;
    this->m_time.z = 0.0f;
    this->m_time.w = 0.0f;
}

// OFFSET: 0x0089E200/0x1405DEC90
bool PassWaterWindow::Valid() {
    if (g_theGxDevicePtr->m_caps.m_shaderModel < 4 || !this->m_type) {
        return true;
    }

    int32_t valid = this->m_vertexShader ? this->m_vertexShader->m_permutes[0].Valid() : 0;

    for (uint32_t i = 0; i < 9; i++) {
        valid = valid && this->m_pixelShader && this->m_pixelShader->m_permutes[i].Valid();
    }

    return valid != 0;
}

// OFFSET: 0x0089E270/0x1405DED20
void PassWaterWindow::Execute() {
    uint32_t time = FFX::s_curTimeMs;
    VertexConstants constants;

    if (g_theGxDevicePtr->m_caps.m_shaderModel < 4) {
        FFX::SetupGx(&FFX::s_screen, 1);
        FFX::SetupGeo_PortalEffect(&constants.m_modelView, &constants.m_projection, &constants.m_model);

        g_theGxDevicePtr->XformSetProjection(FFX::s_oldProjMtx);
        g_theGxDevicePtr->XformSetView(FFX::s_oldViewMtx);
        GxXformSet(GxXform_World, constants.m_model);
        GxXformSetViewport(FFX::s_oldMinX, FFX::s_oldMaxX, FFX::s_oldMinY, FFX::s_oldMaxY, FFX::s_oldMinZ, FFX::s_oldMaxZ);

        GxRsSet(GxRs_PixelShader, 0);
        GxRsSet(GxRs_VertexShader, 0);
        GxRsSet(GxRs_TextureShader0, 0);
        GxRsSet(GxRs_ColorOp0, 1);

        g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(s_oceanTex[time / 50 % 30]));
        GxRsSet(GxRs_Texture1, 0);
        GxRsSet(GxRs_BlendingMode, GxBlend_Add);

        FFX::SetupVB_PNCT();
        FFX::SetupIB();
        DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, 0xFFFFFFFF);
        FFX::Render();
        FFX::SetupReset();

        return;
    }

    FFX::SetupGx(this->m_output, 1);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    FFX::SetupGeo_PortalEffect(&constants.m_modelView, &constants.m_projection, &constants.m_model);
    GxXformSetViewport(FFX::s_oldMinX, FFX::s_oldMaxX, FFX::s_oldMinY, FFX::s_oldMaxY, FFX::s_oldMinZ, FFX::s_oldMaxZ);

    uint32_t type = this->m_type;

    if (type > 2) {
        type = 2;
    }

    if (!s_bumpTex) {
        type = 0;
    }

    CGxTexFlags flags;
    GxTexFlags(TextureGetGxTex(this->m_inputs[0]->m_texture), flags);

    CGxTexFlags pointFlags = flags;
    pointFlags.m_filter = 0;
    GxTexSetFlags(TextureGetGxTex(this->m_inputs[0]->m_texture), pointFlags);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));

    CGxTex* water = type ? TextureGetGxTex(s_bumpTex) : TextureGetGxTex(s_oceanTex[time / 50 % 30]);

    if (water) {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, water);

        if (type == 2 && DepthCache::IsValid()) {
            g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(DepthCache::GetDepthSR()));
        }

        FFX::SetupVB_PNC2T_TS();
        FFX::SetupIB();

        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);

        if (DepthCache::GetDepthMode()) {
            g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[DepthCache::GetDepthMode() + 3 * type - 1]);
        } else {
            g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[3 * type]);
        }

        float t = static_cast<float>(static_cast<double>(time % 100000) * 0.0000099999997);
        constants.m_time.x = t;
        constants.m_time.y = t;
        constants.m_time.z = t;
        constants.m_time.w = t;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &constants.m_modelView.a0, 13);

        FFX::psProcWaterConsts[8].x = t;
        FFX::psProcWaterConsts[8].y = t;
        FFX::psProcWaterConsts[8].z = t;
        FFX::psProcWaterConsts[8].w = t;

        FFX::Tex* input = this->m_inputs[0];
        float scaleX = static_cast<float>(input->m_texSize.x) / static_cast<float>(input->m_size.x);
        float scaleY = static_cast<float>(input->m_texSize.y) / static_cast<float>(input->m_size.y);

        FFX::psProcWaterConsts[14] = ViewSettings::CalculateHomToScreen(input->m_texelSize.x * 0.5f, input->m_texelSize.y * 0.5f);
        FFX::psProcWaterConsts[14].x = 1.0f / scaleX * FFX::psProcWaterConsts[14].x;
        FFX::psProcWaterConsts[14].y = 1.0f / scaleY * FFX::psProcWaterConsts[14].y;
        FFX::psProcWaterConsts[14].z = 1.0f / scaleX * FFX::psProcWaterConsts[14].z;
        FFX::psProcWaterConsts[14].w = 1.0f / scaleY * FFX::psProcWaterConsts[14].w;

        input = this->m_inputs[0];
        FFX::psProcWaterConsts[7].x = static_cast<float>(input->m_size.x);
        FFX::psProcWaterConsts[7].y = static_cast<float>(input->m_size.y);
        FFX::psProcWaterConsts[7].z = input->m_texelSize.x;
        FFX::psProcWaterConsts[7].w = input->m_texelSize.y;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &FFX::psProcWaterConsts[0].x, 15);

        FFX::Render();

        GxTexSetFlags(TextureGetGxTex(this->m_inputs[0]->m_texture), flags);
    }

    FFX::SetupReset();
}

// OFFSET: 0x0089E8A0/0x1405DF410
PassWaterWindow::PassWaterWindow(uint32_t type, int32_t submarine, FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_type = type;
    this->m_submarine = submarine;
    this->m_vertexShader = NULL;
    this->m_pixelShader = NULL;

    if (g_theGxDevicePtr->m_caps.m_shaderModel >= 4) {
        const char* name = submarine ? "FFXSubmarineWindow" : "FFXWaterWindow";
        this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, name);
        this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, name);
    }

    if (!s_refCount) {
        CGxTexFlags flags;
        flags.m_wrapU = 1;
        flags.m_wrapV = 1;

        CStatus status;

        if (g_theGxDevicePtr->m_caps.m_shaderModel < 4 || !this->m_type) {
            for (uint32_t i = 0; i < 30; i++) {
                char name[256];
                SStrPrintf(name, sizeof(name), "XTextures\\ocean\\ocean_h.%d.blp", i + 1);
                s_oceanTex[i] = TextureCreate(name, flags, &status, 0, NULL, 2);
            }
        }

        if (g_theGxDevicePtr->m_caps.m_shaderModel >= 4 && this->m_type) {
            s_bumpTex = TextureCreate("XTextures\\FX\\blobbybump.blp", flags, &status, 0, NULL, 2);
        }
    }

    s_refCount++;
}
