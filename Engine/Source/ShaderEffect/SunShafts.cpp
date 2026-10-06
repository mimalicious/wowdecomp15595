#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"

// STATIC: %0x00C2040C
static const uint16_t s_quadIndices[4] = { 0, 3, 1, 2 };

// STATIC: %0x00E457D8
static CGxShader* s_propagatePixelShader;

// STATIC: %0x00E457DC
static CGxShader* s_propagateVertexShader;

// STATIC: %0x00E457E0
static CGxShader* s_maskPixelShader;

// STATIC: %0x00E457E4
static CGxShader* s_maskVertexShader;

// STATIC: %0x00E457E8
static CGxTex* s_gxTex[2];

// STATIC: %0x00E457F0
static CTexture* s_tex[2];

// STATIC: %0x00E457F8
static uint32_t s_mode;

// PROVEN: 2201f9d8 IDENTICAL
// OFFSET: 0x00A2ED00
int32_t SunShafts::IsEnabled() {
    return s_mode > 0;
}

// PROVEN: a4c1905c IDENTICAL
// OFFSET: 0x00A2ED10
CGxTex* SunShafts::GetBuffer() {
    return s_gxTex[1];
}

// OFFSET: %0x00970F40
void SunShafts::Create() {
    CGxTexFlags flags;
    flags.m_renderTarget = 1;

    s_tex[0] = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, "sunShaftsTex0", 0);
    s_tex[1] = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, "sunShaftsTex1", 0);
    s_gxTex[0] = TextureGetGxTex(s_tex[0]);
    s_gxTex[1] = TextureGetGxTex(s_tex[1]);
    s_maskVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "SunShaftsMask");
    s_maskPixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "SunShaftsMask");
    s_propagateVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "SunShaftsPropagate");
    s_propagatePixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "SunShaftsPropagate");
    DepthCache::AddRef();
}

// OFFSET: %0x00971040
void SunShafts::Destroy() {
    for (uint32_t i = 0; i < 2; i++) {
        if (s_tex[i]) {
            TextureRelease(s_tex[i]);
            s_tex[i] = NULL;
            s_gxTex[i] = NULL;
        }
    }

    if (s_maskVertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_maskVertexShader);
        s_maskVertexShader = NULL;
    }

    if (s_maskPixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_maskPixelShader);
        s_maskPixelShader = NULL;
    }

    if (s_propagateVertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_propagateVertexShader);
        s_propagateVertexShader = NULL;
    }

    if (s_propagatePixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_propagatePixelShader);
        s_propagatePixelShader = NULL;
    }

    DepthCache::Release();
}

// OFFSET: %0x009710F0
uint32_t SunShafts::ValidateMode(uint32_t mode) {
    if (!mode) {
        return 0;
    }

    if (mode > 2) {
        return 2;
    }

    if (g_theGxDevicePtr->m_caps.m_shaderModel < 4) {
        return 64;
    }

    return DepthCache::IsSupported(NULL, NULL, NULL) ? 0 : 8;
}

// OFFSET: %0x00971140
int32_t SunShafts::SetMode(uint32_t mode) {
    if (!mode || (mode <= 2 && g_theGxDevicePtr->m_caps.m_shaderModel >= 4 && DepthCache::IsSupported(NULL, NULL, NULL))) {
        if (SunShafts::IsEnabled() != (mode > 0)) {
            if (mode) {
                SunShafts::Create();
            } else {
                SunShafts::Destroy();
            }
        }

        s_mode = mode;
        return 1;
    }

    return 0;
}

// OFFSET: %0x009711C0
void SunShafts::Update(const C3Vector& eye, const C3Vector& sun) {
    if (!s_mode) {
        return;
    }

    if (!s_tex[0]) {
        SunShafts::Create();
    }

    CGxTex* savedTargets[GxBuffers_Last] = {};

    for (int32_t i = 0; i < GxBuffers_Last; i++) {
        g_theGxDevicePtr->DeviceGetRenderTarget(static_cast<EGxBuffer>(i), savedTargets[i]);
        GxRsSet(GxRs_ScissorTest, 0);
        g_theGxDevicePtr->DeviceSetRenderTarget(static_cast<EGxBuffer>(i), NULL, 0);
    }

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), 4);
    C3Vector* vertex = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(vertexBuf));
    vertex[0] = C3Vector(-1.0f, -1.0f, 0.0f);
    vertex[1] = C3Vector(-1.0f, 1.0f, 0.0f);
    vertex[2] = C3Vector(1.0f, 1.0f, 0.0f);
    vertex[3] = C3Vector(1.0f, -1.0f, 0.0f);
    GxBufUnlock(vertexBuf, 0);

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 4);
    GxBufData(indexBuf, s_quadIndices, sizeof(s_quadIndices), 0);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = 4;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 3;

    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_P);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

    CGxTex* depthTex = TextureGetGxTex(DepthCache::GetDepthSR());
    float width = static_cast<float>(depthTex->m_width);
    float height = static_cast<float>(depthTex->m_height);

    C4Vector texel;
    texel.x = 1.0f / width;
    texel.y = 1.0f / height;
    texel.z = texel.x * 0.5f;
    texel.w = texel.y * 0.5f;

    C4Vector viewParams = ViewSettings::CalculateParams();

    C44Matrix viewProj;
    GxXformViewProj(viewProj);

    C3Vector dir = sun - eye;
    C4Vector pos;
    pos.x = viewProj.a0 * dir.x + viewProj.b0 * dir.y + viewProj.c0 * dir.z + viewProj.d0;
    pos.y = viewProj.a1 * dir.x + viewProj.b1 * dir.y + viewProj.c1 * dir.z + viewProj.d1;
    pos.z = viewProj.a2 * dir.x + viewProj.b2 * dir.y + viewProj.c2 * dir.z + viewProj.d2;
    pos.w = viewProj.a3 * dir.x + viewProj.b3 * dir.y + viewProj.c3 * dir.z + viewProj.d3;

    float invW = 1.0f / pos.w;
    C4Vector sunPos;
    sunPos.x = pos.x * invW;
    sunPos.y = pos.y * invW;
    sunPos.z = pos.z * invW;
    sunPos.w = invW * pos.w;

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_maskVertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_maskPixelShader->m_permutes[s_mode + 2 * DepthCache::GetDepthMode() - 3]);

    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, s_gxTex[0], 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, depthTex);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &texel.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 28, &viewParams.x, 1);
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_propagateVertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_propagatePixelShader->m_permutes[0]);

    width = static_cast<float>(s_gxTex[0]->m_width);
    height = static_cast<float>(s_gxTex[0]->m_height);
    texel.x = 1.0f / width;
    texel.y = 1.0f / height;
    texel.z = texel.x * 0.5f;
    texel.w = texel.y * 0.5f;

    C4Vector flip;
    flip.x = 1.0f;
    flip.y = g_theGxDevicePtr->m_caps.m_texelCenterOnEdge ? 1.0f : -1.0f;
    flip.z = 0.0f;
    flip.w = 0.0f;

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &texel.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, &flip.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &texel.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, &sunPos.x, 1);

    uint32_t target = 0;

    for (uint32_t i = 0; i < 3; i++) {
        target ^= 1;
        GxRsSet(GxRs_ScissorTest, 0);
        g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, s_gxTex[target], 0);

        C4Vector offset;
        offset.x = texel.z + i * texel.x;
        offset.y = i * texel.x + texel.w;
        offset.z = 0.0f;
        offset.w = 0.0f;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, &offset.x, 1);

        CGxTex* source = s_gxTex[(target - 1) & 1];
        g_theGxDevicePtr->RsSet(GxRs_Texture0, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture2, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture3, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture4, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture5, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture6, source);
        g_theGxDevicePtr->RsSet(GxRs_Texture7, source);
        g_theGxDevicePtr->Draw(batch);
    }

    g_theGxDevicePtr->RsPop();

    for (int32_t i = 0; i < GxBuffers_Last; i++) {
        GxRsSet(GxRs_ScissorTest, 0);
        g_theGxDevicePtr->DeviceSetRenderTarget(static_cast<EGxBuffer>(i), savedTargets[i], 0);
    }
}
