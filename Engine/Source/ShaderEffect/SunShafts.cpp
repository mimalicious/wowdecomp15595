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

// STATIC: 0x00D1C8FC
static const uint16_t s_quadIndices[4] = { 0, 3, 1, 2 };

// STATIC: 0x00F803A0
static CGxShader* s_propagatePixelShader;

// STATIC: 0x00F803A4
static CGxShader* s_propagateVertexShader;

// STATIC: 0x00F803A8
static CGxShader* s_maskPixelShader;

// STATIC: 0x00F803AC
static CGxShader* s_maskVertexShader;

// STATIC: 0x00F803B0
static CGxFrameBuffer* s_frameBuffer[2];

// STATIC: 0x00F803B8/0x140E78228
static CGxTex* s_gxTex[2];

// STATIC: 0x00F803C0
static CTexture* s_tex[2];

// STATIC: 0x00F803C8
static uint32_t s_mode;

// PROVEN: 2201f9d8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A2ED00/0x1407D86E0
int32_t SunShafts::IsEnabled() {
    return s_mode > 0;
}

// PROVEN: a4c1905c IDENTICAL/IDENTICAL
// OFFSET: 0x00A2ED10/0x1407D86F0
CGxTex* SunShafts::GetBuffer() {
    return s_gxTex[1];
}

// OFFSET: 0x00A2ED20
void SunShafts::Create() {
    CGxTexFlags flags;
    flags.m_renderTarget = 1;

    s_tex[0] = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, "sunShaftsTex0", 0);
    s_tex[1] = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, "sunShaftsTex1", 0);
    s_gxTex[0] = TextureGetGxTex(s_tex[0]);
    s_gxTex[1] = TextureGetGxTex(s_tex[1]);
    s_frameBuffer[0] = GxFrameBufferCreate(NULL, s_gxTex[0]);
    s_frameBuffer[1] = GxFrameBufferCreate(NULL, s_gxTex[1]);
    s_maskVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "SunShaftsMask");
    s_maskPixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "SunShaftsMask");
    s_propagateVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "SunShaftsPropagate");
    s_propagatePixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "SunShaftsPropagate");
    DepthCache::AddRef();
}

// OFFSET: 0x00A2EE40
void SunShafts::Destroy() {
    for (uint32_t i = 0; i < 2; i++) {
        if (s_tex[i]) {
            TextureRelease(s_tex[i]);
            s_tex[i] = NULL;
        }

        s_gxTex[i] = NULL;

        if (s_frameBuffer[i]) {
            g_theGxDevicePtr->FrameBufferDelete(s_frameBuffer[i]);
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

// PROVEN: 1fac55c6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EF10/0x1407D8990
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

// OFFSET: 0x00A2EF60
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

// OFFSET: 0x00A2EFE0
void SunShafts::Update(const C3Vector& eye, const C3Vector& sun) {
    if (!s_mode) {
        return;
    }

    if (!s_tex[0]) {
        SunShafts::Create();
    }

    C44Matrix viewProj;
    GxXformViewProj(viewProj);

    C3Vector dir = sun - eye;
    C4Vector pos;
    pos.x = viewProj.a0 * dir.x + viewProj.b0 * dir.y + viewProj.c0 * dir.z + viewProj.d0;
    pos.y = viewProj.a1 * dir.x + viewProj.b1 * dir.y + viewProj.c1 * dir.z + viewProj.d1;
    pos.z = viewProj.a2 * dir.x + viewProj.b2 * dir.y + viewProj.c2 * dir.z + viewProj.d2;
    pos.w = viewProj.a3 * dir.x + viewProj.b3 * dir.y + viewProj.c3 * dir.z + viewProj.d3;

    C4Vector sunPos = pos;

    if (pos.w < 0.00000011920929f) {
        return;
    }

    float invW = 1.0f / pos.w;
    sunPos.y = sunPos.y * invW;
    sunPos.x = pos.x * invW;
    sunPos.z = sunPos.z * invW;
    sunPos.w = invW * sunPos.w;

    CGxFrameBuffer* savedFrameBuffer = g_theGxDevicePtr->FrameBufferGet();

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

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

    CGxTex* depthTex = TextureGetGxTex(DepthCache::GetDepthSR());
    float width = static_cast<float>(depthTex->m_width);
    float height = static_cast<float>(depthTex->m_height);

    C4Vector texel;
    texel.x = 1.0f / width;
    texel.y = 1.0f / height;
    texel.z = texel.x * 0.5f;
    texel.w = texel.y * 0.5f;

    C4Vector viewParams = ViewSettings::CalculateParams();
    C4Vector homToScreen = ViewSettings::CalculateHomToScreen(texel.z, texel.w);

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_maskVertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_maskPixelShader->m_permutes[s_mode + 2 * DepthCache::GetDepthMode() - 3]);

    GxFrameBufferSet(s_frameBuffer[0]);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, depthTex);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);

    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &texel.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 28, &viewParams.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 29, &homToScreen.x, 1);
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_propagateVertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_propagatePixelShader->m_permutes[0]);

    width = static_cast<float>(s_gxTex[0]->m_width);
    height = static_cast<float>(s_gxTex[0]->m_height);
    texel.x = 1.0f / width;
    texel.z = texel.x * 0.5f;
    texel.y = 1.0f / height;
    texel.w = texel.y * 0.5f;

    C4Vector flip;
    flip.x = 1.0f;
    flip.y = -1.0f;
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
        g_theGxDevicePtr->FrameBufferSet(s_frameBuffer[target]);

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
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(savedFrameBuffer);
}
