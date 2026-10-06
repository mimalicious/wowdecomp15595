#include <cmath>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"

// STATIC: %0x00D95210
bool FFX::s_useRECT;

// STATIC: %0x00D95211
bool FFX::s_defaultEnabled;

// STATIC: %0x00D9521C
float FFX::s_oldMaxZ;

// STATIC: %0x00D95220
float FFX::s_oldMinZ;

// STATIC: %0x00D95224
float FFX::s_oldMaxY;

// STATIC: %0x00D95228
float FFX::s_oldMinY;

// STATIC: %0x00D9522C
float FFX::s_oldMaxX;

// STATIC: %0x00D95230
float FFX::s_oldMinX;

// STATIC: %0x00D95234
const uint16_t* FFX::s_idx;

// STATIC: %0x00D95238
const C2Vector* FFX::s_tex;

// STATIC: %0x00D9523C
const CImVector* FFX::s_col1;

// STATIC: %0x00D95240
const CImVector* FFX::s_col0;

// STATIC: %0x00D95244
const C3Vector* FFX::s_normal;

// STATIC: %0x00D95248
const C3Vector* FFX::s_geo;

// STATIC: %0x00D9524C
CGxShader* FFX::s_passThrough_PCT4;

// STATIC: %0x00D95250
CGxShader* FFX::s_passThrough_PCT3;

// STATIC: %0x00D95254
CGxShader* FFX::s_passThrough_PCT2;

// STATIC: %0x00D95258
CGxShader* FFX::s_passThrough_PCT;

// STATIC: %0x00D9525C
FFX::PortalEffect* FFX::s_portalEffect;

// STATIC: %0x00D95260
FFX::Effect* FFX::s_effect[2];

// STATIC: %0x00D95268
uint32_t FFX::s_curTimeMs;

// STATIC: %0x00D9526C
float FFX::s_tickTimeSec;

// STATIC: %0x00D95270
CGxTex* FFX::s_oldFrameBuffer;

// STATIC: %0x00D95274
CGxTex* FFX::s_oldDepthBuffer;

// STATIC: %0x00D95278
C3Vector FFX::s_camPos;

// STATIC: %0x00D95284
FFX::Tex FFX::s_waveTex;

// STATIC: %0x00D952A4
FFX::Tex FFX::s_screen;

// STATIC: %0x00D952C8
C4Vector FFX::psProcWaterConsts[15];

// STATIC: %0x00D953B8
TSGrowableArray<FFX::PortalEffect> FFX::s_portalEffects;

// STATIC: %0x00D953C8
FFX::Tex FFX::s_full[2];

// STATIC: %0x00D95408
FFX::Tex FFX::s_half[2];

// STATIC: %0x00D95448
FFX::Tex FFX::s_quart[3];

// STATIC: %0x00D954FC
static void* s_waveTexData;

// STATIC: %0x00C0FF08
static float s_viewportMaxZ = 1.0f;

// STATIC: %0x00C0FF0C
C2iVector FFX::s_wavePeriod = { 3174, 2805 };

// STATIC: %0x00C0FF14
float FFX::s_waveScaleX = 1.0f;

// STATIC: %0x00C0FF18
float FFX::s_waveScaleY = 0.88f;

// STATIC: %0x00C0FF1C
CGxBatch FFX::s_batch = { GxPrim_Triangles, 0, 0, 0, 0 };

// STATIC: %0x00C0FF30
C44Matrix FFX::s_oldProjMtxNative;

// STATIC: %0x00C0FF70
C44Matrix FFX::s_oldProjMtx;

// STATIC: %0x00C0FFB0
C44Matrix FFX::s_oldViewMtx;

// STATIC: %0x00C10028
static const uint16_t s_quadIndices[6] = { 0, 1, 2, 2, 1, 3 };

// INLINED
static EGxTexTarget GetTexTarget() {
    if (!FFX::s_cvarFFXRectangle->m_intValue) {
        return GxTex_2d;
    }

    if (g_theGxDevicePtr->m_caps.m_texNonPow2) {
        return GxTex_2d;
    }

    if (g_theGxDevicePtr->m_caps.m_texTarget[GxTex_NonPow2]) {
        return GxTex_NonPow2;
    }

    return g_theGxDevicePtr->m_caps.m_texTarget[GxTex_Rectangle] ? GxTex_Rectangle : GxTex_2d;
}

// INLINED
static void ResetTex(FFX::Tex& tex) {
    tex.m_texture = NULL;
    tex.m_depthTexture = NULL;
    tex.m_texSize.x = 0;
    tex.m_texSize.y = 0;
    tex.m_size.x = 0;
    tex.m_size.y = 0;
    tex.m_texelSize.x = 0.0f;
    tex.m_texelSize.y = 0.0f;
}

// INLINED
static void SetVertexShader(CGxShader* shader) {
    if (shader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &shader->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }
}

// INLINED
static CImVector DeviceColor(const CImVector& color) {
    CImVector result;

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        result.b = color.r;
        result.g = color.g;
        result.r = color.b;
        result.a = color.a;
    } else {
        result = color;
    }

    return result;
}

// INLINED
static C3Vector TransformPos(const C3Vector& pos, const C44Matrix& m) {
    C4Vector v;
    v.x = m.a0 * pos.x + m.b0 * pos.y + m.c0 * pos.z + m.d0;
    v.y = m.a1 * pos.x + m.b1 * pos.y + m.c1 * pos.z + m.d1;
    v.z = m.a2 * pos.x + m.b2 * pos.y + m.c2 * pos.z + m.d2;
    v.w = m.a3 * pos.x + m.b3 * pos.y + m.c3 * pos.z + m.d3;
    return C3Vector(v);
}

// INLINED
static C2Vector TransformTex(const C2Vector& tex, const C44Matrix& m) {
    C4Vector v;
    v.x = m.a0 * tex.x + m.b0 * tex.y + m.c0 * 0.0f + m.d0;
    v.y = m.a1 * tex.x + m.b1 * tex.y + m.c1 * 0.0f + m.d1;
    v.z = m.a2 * tex.x + m.b2 * tex.y + m.c2 * 0.0f + m.d2;
    v.w = m.a3 * tex.x + m.b3 * tex.y + m.c3 * 0.0f + m.d3;
    return C2Vector(v);
}

// INLINED
static float WaveCos(double x) {
    int32_t period = x <= 0.0 ? static_cast<int32_t>(static_cast<int64_t>(x)) - 1 : static_cast<int32_t>(static_cast<int64_t>(x));
    float t = static_cast<float>(x);
    float f = t - period;
    float value = 1.0f - (6.0f - f * 4.0f) * f * f;

    if (period & 1) {
        value = -value;
    }

    return value;
}

// INLINED
FFX::PortalEffect::PortalEffect() {
    this->m_batch.m_primType = GxPrim_TriangleFan;
    this->m_batch.m_start = 0;
    this->m_batch.m_count = 0;
    this->m_batch.m_minIndex = 0;
    this->m_batch.m_maxIndex = 0;
}

// PROVEN: fc004bde IDENTICAL
// OFFSET: 0x00899D40
C44Matrix* FFX::GetOldViewMtx() {
    return &FFX::s_oldViewMtx;
}

// OFFSET: %0x007FE3A0
int32_t FFX::EffectIsSet(Effect* effect) {
    for (uint32_t i = 0; i < 2; i++) {
        if (FFX::s_effect[i] == effect) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: 360fcc7c IDENTICAL
// OFFSET: 0x00899DA0
void FFX::SetEffect(uint32_t index, Effect* effect) {
    Effect* previous = FFX::s_effect[index];
    FFX::s_effect[index] = effect;

    if (previous) {
        previous->End();
    }

    if (effect) {
        effect->Begin();
    }
}

// OFFSET: %0x007FE410
FFX::Pass::Pass(Tex** inputs, uint32_t count, Tex* output, EGxBlend blend) {
    memset(this->m_inputs, 0, sizeof(this->m_inputs));
    this->m_output = NULL;

    for (uint32_t i = 0; i < count; i++) {
        this->m_inputs[i] = inputs[i];
    }

    this->m_output = output;
    this->m_blend = blend;
}

// OFFSET: %0x007FE520
static void DestroyTex(FFX::Tex& tex) {
    if (tex.m_texture) {
        TextureRelease(tex.m_texture);
    }

    if (tex.m_depthTexture) {
        TextureRelease(tex.m_depthTexture);
    }

    ResetTex(tex);
}

// OFFSET: %0x007FE590
void FFX::Destroy() {
    if (FFX::s_passThrough_PCT) {
        g_theGxDevicePtr->ShaderRelease(FFX::s_passThrough_PCT);
        FFX::s_passThrough_PCT = NULL;
    }

    if (FFX::s_passThrough_PCT2) {
        g_theGxDevicePtr->ShaderRelease(FFX::s_passThrough_PCT2);
        FFX::s_passThrough_PCT2 = NULL;
    }

    if (FFX::s_passThrough_PCT3) {
        g_theGxDevicePtr->ShaderRelease(FFX::s_passThrough_PCT3);
        FFX::s_passThrough_PCT3 = NULL;
    }

    if (FFX::s_passThrough_PCT4) {
        g_theGxDevicePtr->ShaderRelease(FFX::s_passThrough_PCT4);
        FFX::s_passThrough_PCT4 = NULL;
    }

    DestroyTex(FFX::s_waveTex);
    DestroyTex(FFX::s_full[0]);
    DestroyTex(FFX::s_full[1]);
    DestroyTex(FFX::s_half[0]);
    DestroyTex(FFX::s_half[1]);
    DestroyTex(FFX::s_quart[0]);
    DestroyTex(FFX::s_quart[1]);
    DestroyTex(FFX::s_quart[2]);
    DestroyTex(FFX::s_screen);
}

// OFFSET: %0x007FE660
void FFX::SetupGeo_Quad(const C2iVector& size, C44Matrix* matrix) {
    // STATIC: %0x00D954C8
    static const C3Vector s_quadPos[4] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 1.0f, 0.0f), C3Vector(1.0f, 0.0f, 0.0f), C3Vector(1.0f, 1.0f, 0.0f) };

    // STATIC: %0x00D954A8
    static const C2Vector s_quadTex[4] = { C2Vector(0.0f, 0.0f), C2Vector(0.0f, 1.0f), C2Vector(1.0f, 0.0f), C2Vector(1.0f, 1.0f) };

    FFX::s_batch.m_primType = GxPrim_Triangles;
    FFX::s_batch.m_start = 0;
    FFX::s_geo = s_quadPos;
    FFX::s_tex = s_quadTex;
    FFX::s_idx = s_quadIndices;
    FFX::s_batch.m_count = 6;
    FFX::s_batch.m_minIndex = 0;
    FFX::s_batch.m_maxIndex = 3;

    float x = -1.0f;
    float y = 1.0f;

    if (g_theGxDevicePtr->m_caps.m_pixelCenterOnEdge) {
        x = -1.0f - 1.0f / size.x;
        y = 1.0f / size.y + 1.0f;
    }

    matrix->Identity();
    matrix->Translate(C3Vector(x, y, 0.0f));
    matrix->Scale(C3Vector(2.0f, -2.0f, 1.0f));
}

// OFFSET: %0x007FE860
void FFX::SetupGeo_PortalEffect(C44Matrix* modelView, C44Matrix* projection, C44Matrix* model) {
    C44Matrix camera;
    camera.Translate(C3Vector(-FFX::s_camPos.x, -FFX::s_camPos.y, -FFX::s_camPos.z));

    *model = FFX::s_portalEffect->m_matrix * camera;
    *projection = FFX::s_oldProjMtxNative;
    *modelView = *model * FFX::s_oldViewMtx;

    FFX::s_geo = FFX::s_portalEffect->m_geo;
    FFX::s_normal = FFX::s_portalEffect->m_normal;
    FFX::s_col0 = FFX::s_portalEffect->m_col0;
    FFX::s_col1 = FFX::s_portalEffect->m_col1;
    FFX::s_tex = FFX::s_portalEffect->m_tex;
    FFX::s_idx = FFX::s_portalEffect->m_idx;
    FFX::s_batch = FFX::s_portalEffect->m_batch;
}

// OFFSET: %0x007FE9C0
void FFX::SetupTexMtx(const C2iVector& size, const C2iVector& texSize, C44Matrix* matrix, int32_t allowRect) {
    float width = static_cast<float>(size.x);
    float height = static_cast<float>(size.y);
    float invTexWidth = 1.0f / texSize.x;
    float invTexHeight = 1.0f / texSize.y;
    float minU = invTexWidth * 0.0f;
    float minV = invTexHeight * 0.0f;
    float scaleU = invTexWidth * width - invTexWidth * 0.0f;
    float scaleV = invTexHeight * height - invTexHeight * 0.0f;

    if (FFX::s_useRECT && allowRect) {
        scaleU = width * scaleU;
        scaleV = height * scaleV;
    }

    matrix->Identity();
    matrix->Translate(C3Vector(minU, minV, 0.0f));
    matrix->Scale(C3Vector(scaleU, scaleV, 1.0f));

    if (g_theGxDevicePtr->m_caps.m_texelCenterOnEdge) {
        C44Matrix flip;
        flip.Translate(C3Vector(0.0f, 1.0f, 0.0f));
        flip.Scale(C3Vector(1.0f, -1.0f, 1.0f));
        *matrix = flip * *matrix;
    }
}

// OFFSET: %0x007FEBC0
void FFX::SetupVB_PNCT() {
    struct Vertex {
        C3Vector p;
        C3Vector n;
        CImVector c;
        C2Vector tc;
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = FFX::s_geo[i];
        vertex->n = FFX::s_normal[i];
        vertex->c = CImVector(0xFF, 0xFF, 0xFF, 0xFF);
        vertex->tc = FFX::s_tex[i];
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PNCT);
}

// OFFSET: %0x007FECF0
void FFX::SetupVB_PNC2T_TS() {
    struct Vertex {
        C3Vector p;
        C3Vector n;
        CImVector c0;
        CImVector c1;
        C2Vector tc;
        C4Vector ts;
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = FFX::s_geo[i];
        vertex->n = FFX::s_normal[i];
        vertex->c0 = FFX::s_col0[i];
        vertex->c1 = FFX::s_col1[i];
        vertex->tc = FFX::s_tex[i];
        vertex->ts = FFX::s_portalEffect->m_tangent[i - FFX::s_batch.m_minIndex];
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PNC2T_TS);
}

// OFFSET: %0x007FEE20
void FFX::SetupVB_PT(const C44Matrix& posMtx, const C44Matrix& texMtx) {
    struct Vertex {
        C3Vector p;
        C2Vector tc;
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);
        vertex->tc = TransformTex(FFX::s_tex[i], texMtx);
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PT);
}

// OFFSET: %0x007FF0B0
void FFX::SetupVB_PCT(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& texMtx) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc;
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);
        vertex->c = DeviceColor(color);
        vertex->tc = TransformTex(FFX::s_tex[i], texMtx);
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PCT);
}

// OFFSET: %0x007FF380
void FFX::SetupVB_PCT2(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& tex0Mtx, const C44Matrix& tex1Mtx) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc[2];
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);
        vertex->c = DeviceColor(color);
        vertex->tc[0] = TransformTex(FFX::s_tex[i], tex0Mtx);
        vertex->tc[1] = TransformTex(FFX::s_tex[i], tex1Mtx);
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PCT2);
}

// OFFSET: %0x007FF730
void FFX::SetupVB_PCT3(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& tex0Mtx, const C44Matrix& tex1Mtx, const C44Matrix& tex2Mtx) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc[3];
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);
        vertex->c = DeviceColor(color);
        vertex->tc[0] = TransformTex(FFX::s_tex[i], tex0Mtx);
        vertex->tc[1] = TransformTex(FFX::s_tex[i], tex1Mtx);
        vertex->tc[2] = TransformTex(FFX::s_tex[i], tex2Mtx);
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PCT3);
}

// OFFSET: %0x007FFBE0
void FFX::SetupVB_PCT4(const CImVector& color, const C44Matrix& posMtx, const C44Matrix& texMtx, const C2Vector* offsets, const C2Vector& scale) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc[4];
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);
        vertex->c = DeviceColor(color);

        for (uint32_t j = 0; j < 4; j++) {
            C2Vector tex(FFX::s_tex[i].x + offsets[j].x * scale.x, FFX::s_tex[i].y + offsets[j].y * scale.y);
            vertex->tc[j] = TransformTex(tex, texMtx);
        }

        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PCT4);
}

// OFFSET: %0x007FFEF0
void FFX::SetupVB_PCT(const C44Matrix& posMtx, const C44Matrix& texMtx, const float* shade) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc;
    };

    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(Vertex), FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex + 1);
    Vertex* vertex = reinterpret_cast<Vertex*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_minIndex; i <= FFX::s_batch.m_maxIndex; i++) {
        vertex->p = TransformPos(FFX::s_geo[i], posMtx);

        uint8_t value = static_cast<uint8_t>(static_cast<int32_t>((shade[i] + 1.0f) * 127.5f));
        vertex->c.a = value;
        vertex->c.b = value;
        vertex->c.g = value;
        vertex->c.r = value;

        vertex->tc = TransformTex(FFX::s_tex[i], texMtx);
        vertex++;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PCT);
}

// OFFSET: %0x008001A0
void FFX::SetupIB() {
    uint32_t count = FFX::s_batch.m_count;
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), count);
    uint16_t* index = reinterpret_cast<uint16_t*>(GxBufLock(buf));

    for (uint32_t i = FFX::s_batch.m_start; i < count + FFX::s_batch.m_start; i++) {
        *index++ = FFX::s_idx[i] - FFX::s_batch.m_minIndex;
    }

    GxBufUnlock(buf, 0);
    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// OFFSET: %0x00800230
void FFX::Render() {
    CGxBatch batch;
    batch.m_primType = FFX::s_batch.m_primType;
    batch.m_count = FFX::s_batch.m_count;
    batch.m_minIndex = 0;
    batch.m_maxIndex = FFX::s_batch.m_maxIndex - FFX::s_batch.m_minIndex;
    batch.m_start = 0;
    g_theGxDevicePtr->Draw(batch);
}

// OFFSET: %0x00800280
void FFX::DoPass(uint32_t pass) {
    if (!FFX::s_cvarFFX->m_intValue) {
        return;
    }

    Effect* effect = FFX::s_effect[pass];

    if (!effect || !effect->Valid()) {
        return;
    }

    C2iVector sourcePos = { 0, 0 };
    C2iVector destPos = { 0, 0 };
    GxTexCopy(NULL, TextureGetGxTex(FFX::s_full[0].m_texture), destPos, sourcePos, FFX::s_full[0].m_size, 0, 0);

    FFX::s_effect[pass]->Execute();
}

// OFFSET: %0x00800300
C44Matrix FFX::GetWaveMtx(const C2iVector& size) {
    C44Matrix matrix;

    C3Vector move;
    move.x = static_cast<float>(static_cast<double>(FFX::s_curTimeMs % static_cast<uint32_t>(FFX::s_wavePeriod.x)) / static_cast<double>(FFX::s_wavePeriod.x));
    move.z = 0.0f;
    move.y = static_cast<float>(static_cast<double>(FFX::s_curTimeMs % static_cast<uint32_t>(FFX::s_wavePeriod.y)) / static_cast<double>(FFX::s_wavePeriod.y));
    matrix.Translate(move);

    C3Vector scale;
    scale.x = static_cast<float>(size.x) / static_cast<float>(FFX::s_waveTex.m_texSize.x) * FFX::s_waveScaleX;
    scale.y = static_cast<float>(size.y) / static_cast<float>(FFX::s_waveTex.m_texSize.y) * FFX::s_waveScaleY;
    scale.z = 1.0f;
    matrix.Scale(scale);

    matrix.RotateAroundZ(0.17453294f);

    return matrix;
}

// PROVEN: 4c8faa73 IDENTICAL
// OFFSET: 0x0089BE50
C4Vector FFX::GetWaveAmplitude(const C2Vector& texelSize, float amplitude) {
    C4Vector result;

    if (FFX::s_useRECT) {
        result.x = amplitude * 3.0f;
        result.y = 0.0f;
        result.z = 0.0f;
        result.w = amplitude * 3.0f;
    } else {
        result.x = texelSize.x * amplitude * 3.0f;
        result.y = 0.0f;
        result.z = 0.0f;
        result.w = texelSize.y * amplitude * 3.0f;
    }

    return result;
}

// OFFSET: %0x008004A0
void FFX::CreateTex(Tex& tex, const C2iVector& size, const char* name, bool depth, bool renderTarget) {
    EGxTexFormat format;

    if (g_theGxDevicePtr->m_caps.m_rttFormat[GxTex_Argb8888] && g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Argb8888]) {
        format = GxTex_Argb8888;
    } else if (g_theGxDevicePtr->m_caps.m_rttFormat[GxTex_Abgr8888] && g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Abgr8888]) {
        format = GxTex_Abgr8888;
    } else if (g_theGxDevicePtr->m_caps.m_rttFormat[GxTex_Rgb565] && g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Rgb565]) {
        format = GxTex_Rgb565;
    } else {
        format = GxTex_Unknown;
    }

    EGxTexFormat depthFormat = g_theGxDevicePtr->DeviceFormat().m_depthFormat == CGxFormat::Fmt_Ds320 ? GxTex_D32 : GxTex_D24X8;

    CGxTexFlags flags;
    flags.m_renderTarget = renderTarget;

    EGxTexTarget target = GetTexTarget();
    int32_t width = size.x;
    int32_t height = size.y;

    if (target == GxTex_2d) {
        for (height = 1; size.y > height; height *= 2) {
        }

        for (width = 1; size.x > width; width *= 2) {
        }
    }

    int32_t maxSize = static_cast<int32_t>(g_theGxDevicePtr->m_caps.m_texMaxSize[target]);

    if (height >= maxSize) {
        height = maxSize;
    }

    if (width >= maxSize) {
        width = maxSize;
    }

    height = height > 8 ? height : 8;
    width = width > 8 ? width : 8;

    if (width != tex.m_texSize.x || height != tex.m_texSize.y) {
        if (tex.m_texture) {
            TextureRelease(tex.m_texture);
            tex.m_texture = NULL;
        }

        if (tex.m_depthTexture) {
            TextureRelease(tex.m_depthTexture);
            tex.m_depthTexture = NULL;
        }

        tex.m_texture = TextureCreate(target, width, height, 0, format, format, flags, NULL, GxTexNullUserFunc, name, 0);

        if (depth) {
            tex.m_depthTexture = TextureCreate(target, width, height, 0, depthFormat, depthFormat, flags, NULL, GxTexNullUserFunc, name, 0);
        }
    }

    tex.m_size.x = size.x;
    tex.m_size.y = size.y;
    tex.m_texSize.x = width;
    tex.m_texelSize.x = 1.0f / width;
    tex.m_texSize.y = height;
    tex.m_texelSize.y = 1.0f / height;
}

// OFFSET: %0x008006C0
bool FFX::Effect::Valid() {
    if (!this->m_passes.Count()) {
        return false;
    }

    bool valid = true;

    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        valid = valid && this->m_passes[i]->Valid();
    }

    if (this->m_cvar) {
        return valid && this->m_cvar->m_intValue;
    }

    return valid;
}

// PROVEN: 38ef06f9 IDENTICAL
// OFFSET: 0x0089C100
void FFX::Effect::Execute() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        this->m_passes[i]->Execute();
    }
}

// OFFSET: %0x008008D0
void FFX::Init() {
    ResetTex(FFX::s_full[0]);
    ResetTex(FFX::s_full[1]);
    ResetTex(FFX::s_half[0]);
    ResetTex(FFX::s_half[1]);
    ResetTex(FFX::s_quart[0]);
    ResetTex(FFX::s_quart[1]);
    ResetTex(FFX::s_quart[2]);
    ResetTex(FFX::s_screen);
    ResetTex(FFX::s_waveTex);

    FFX::s_passThrough_PCT = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PassThrough_PCT");
    FFX::s_passThrough_PCT2 = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PassThrough_PCT2");
    FFX::s_passThrough_PCT3 = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PassThrough_PCT3");
    FFX::s_passThrough_PCT4 = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PassThrough_PCT4");

    FFX::s_effect[0] = NULL;
    FFX::s_effect[1] = NULL;

    FFX::s_useRECT = GetTexTarget() == GxTex_Rectangle;
}

// OFFSET: %0x00800BE0
void FFX::SetupReset() {
    g_theGxDevicePtr->RsPop();
    g_theGxDevicePtr->XformSetView(FFX::s_oldViewMtx);
    g_theGxDevicePtr->XformSetProjection(FFX::s_oldProjMtx);
    GxXformPop(GxXform_World);
    GxRenderTargetSet(GxBuffers_Color, FFX::s_oldFrameBuffer, 0);
    GxRenderTargetSet(GxBuffers_Depth, FFX::s_oldDepthBuffer, 0);
    GxXformSetViewport(FFX::s_oldMinX, FFX::s_oldMaxX, FFX::s_oldMinY, FFX::s_oldMaxY, FFX::s_oldMinZ, FFX::s_oldMaxZ);
}

// OFFSET: %0x00800D00
static void WaveTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    EGxTexFormat format = static_cast<EGxTexFormat>(reinterpret_cast<uintptr_t>(userArg));
    uint32_t bytesPerTexel = 2 * (format == GxTex_Argb8888) + 2;

    switch (command) {
        case GxTex_Lock: {
            s_waveTexData = SMemAlloc(height * width * bytesPerTexel, __FILE__, __LINE__, 0);

            if (format == GxTex_Uv88) {
                int16_t* row = static_cast<int16_t*>(s_waveTexData);

                for (uint32_t y = 0; y < height; y++) {
                    float v = static_cast<float>(static_cast<double>(y) / static_cast<double>(height) * 6.2831855 * 0.31830987 - 0.5);

                    for (uint32_t x = 0; x < width; x++) {
                        float u = WaveCos(static_cast<double>(x) / static_cast<double>(width) * 6.2831855 * 0.31830987 - 0.5) * 128.0f;

                        if (u < -128.0f) {
                            u = -128.0f;
                        } else if (u >= 127.0f) {
                            u = 127.0f;
                        }

                        float w = WaveCos(v) * 128.0f;

                        if (w < -128.0f) {
                            w = -128.0f;
                        } else if (w >= 127.0f) {
                            w = 127.0f;
                        }

                        uint8_t texel[2] = { static_cast<uint8_t>(static_cast<int32_t>(u)), static_cast<uint8_t>(static_cast<int32_t>(w)) };
                        memcpy(&row[x], texel, sizeof(texel));
                    }

                    row += width;
                }
            } else {
                uint32_t* row = static_cast<uint32_t*>(s_waveTexData);

                for (uint32_t y = 0; y < height; y++) {
                    float v = static_cast<float>(static_cast<double>(y) / static_cast<double>(height) * 6.2831855 * 0.31830987 - 0.5);

                    for (uint32_t x = 0; x < width; x++) {
                        float u = (WaveCos(static_cast<double>(x) / static_cast<double>(width) * 6.2831855 * 0.31830987 - 0.5) * 0.5f + 0.5f) * 255.0f;

                        if (u < 0.0f) {
                            u = 0.0f;
                        } else if (u >= 255.0f) {
                            u = 255.0f;
                        }

                        float w = (WaveCos(v) * 0.5f + 0.5f) * 255.0f;

                        if (w < 0.0f) {
                            w = 0.0f;
                        } else if (w >= 255.0f) {
                            w = 255.0f;
                        }

                        CImVector texel;
                        texel.b = 0;
                        texel.g = static_cast<uint8_t>(static_cast<int32_t>(w));
                        texel.r = static_cast<uint8_t>(static_cast<int32_t>(u));
                        texel.a = 0;
                        memcpy(&row[x], &texel, sizeof(texel));
                    }

                    row += width;
                }
            }

            break;
        }

        case GxTex_Latch: {
            texels = s_waveTexData;
            texelStrideInBytes = bytesPerTexel * width;
            break;
        }

        case GxTex_Unlock: {
            SMemFree(s_waveTexData, __FILE__, __LINE__, 0);
            s_waveTexData = NULL;
            break;
        }

        default:
            break;
    }
}

// OFFSET: %0x008011D0
void FFX::AllocTex() {
    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(rect);

    int32_t width = static_cast<int32_t>(rect.maxX - rect.minX);
    int32_t height = static_cast<int32_t>(rect.maxY - rect.minY);

    FFX::s_screen.m_texelSize.x = 1.0f / width;
    FFX::s_screen.m_texelSize.y = 1.0f / height;
    FFX::s_screen.m_size.x = width;
    FFX::s_screen.m_size.y = height;
    FFX::s_screen.m_texSize.x = width;
    FFX::s_screen.m_texSize.y = height;

    EGxTexTarget target = GetTexTarget();
    int32_t maxSize = static_cast<int32_t>(g_theGxDevicePtr->m_caps.m_texMaxSize[target]);

    if (height >= maxSize) {
        height = maxSize;
    }

    if (width >= maxSize) {
        width = maxSize;
    }

    height = height > 8 ? height : 8;
    width = width > 8 ? width : 8;

    C2iVector size = { width, height };
    FFX::CreateTex(FFX::s_full[0], size, "FFX Full 0", true, true);
    FFX::CreateTex(FFX::s_full[1], size, "FFX Full 1", false, true);

    size.x = width / 2;
    size.y = height / 2;
    FFX::CreateTex(FFX::s_half[0], size, "FFX Half 0", false, true);
    FFX::CreateTex(FFX::s_half[1], size, "FFX Half 1", false, true);

    size.x = width / 4;
    size.y = height / 4;
    FFX::CreateTex(FFX::s_quart[0], size, "FFX Quart 0", false, true);
    FFX::CreateTex(FFX::s_quart[1], size, "FFX Quart 1", false, true);
    FFX::CreateTex(FFX::s_quart[2], size, "FFX Quart 2", false, true);

    CGxTexFlags flags;
    flags.m_wrapU = 1;
    flags.m_wrapV = 1;

    EGxTexFormat format = g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Uv88] ? GxTex_Uv88 : GxTex_Argb8888;

    if (FFX::s_waveTex.m_texture) {
        TextureRelease(FFX::s_waveTex.m_texture);
    }

    FFX::s_waveTex.m_texture = TextureCreate(128, 128, format, format, flags, reinterpret_cast<void*>(format), WaveTexCallback, "proceduralWaveTex", 0);
    FFX::s_waveTex.m_size.x = 128;
    FFX::s_waveTex.m_texSize.x = 128;
    FFX::s_waveTex.m_size.y = 128;
    FFX::s_waveTex.m_texSize.y = 128;
    FFX::s_waveTex.m_texelSize.x = 1.0f / 128.0f;
    FFX::s_waveTex.m_texelSize.y = 1.0f / 128.0f;
}

// OFFSET: %0x008018F0
void FFX::Start() {
    if (!FFX::s_cvarFFX->m_intValue) {
        return;
    }

    bool active = false;

    for (uint32_t i = 0; i < 2; i++) {
        if (FFX::s_effect[i] && FFX::s_effect[i]->Valid()) {
            active = true;
            break;
        }
    }

    if (!active && !FFX::s_portalEffects.Count()) {
        return;
    }

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(rect);

    if (static_cast<int32_t>(rect.maxX - rect.minX) != FFX::s_screen.m_size.x || static_cast<int32_t>(rect.maxY - rect.minY) != FFX::s_screen.m_size.y) {
        FFX::AllocTex();
    }

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (rect.maxX - rect.minX > FFX::s_full[0].m_size.x) {
        scaleX = FFX::s_full[0].m_size.x / (rect.maxX - rect.minX);
    }

    if (rect.maxY - rect.minY > FFX::s_full[0].m_size.y) {
        scaleY = FFX::s_full[0].m_size.y / (rect.maxY - rect.minY);
    }

    GxXformSetViewport(0.0f, scaleX, 0.0f, scaleY, 0.0f, 1.0f);
}

// OFFSET: %0x00801A30
void FFX::FlushPortalEffects() {
    if (!FFX::s_portalEffects.Count() || !FFX::s_full[0].m_texture) {
        return;
    }

    if (g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        C2iVector sourcePos = { 0, 0 };
        C2iVector destPos = { 0, 0 };
        GxTexCopy(NULL, TextureGetGxTex(FFX::s_full[0].m_texture), destPos, sourcePos, FFX::s_full[0].m_size, 0, 0);
    }

    for (uint32_t i = 0; i < FFX::s_portalEffects.Count(); i++) {
        PortalEffect* portalEffect = &FFX::s_portalEffects[i];

        if (portalEffect->m_effect->Valid()) {
            FFX::s_portalEffect = portalEffect;
            portalEffect->m_effect->Execute();
        }
    }

    FFX::s_portalEffect = NULL;
    FFX::s_portalEffects.SetCount(0);
}

// OFFSET: %0x00801AF0
void FFX::AddPortalEffect(Effect* effect, const C44Matrix& matrix, const C3Vector* geo, const C3Vector* normal, const CImVector* col0, const CImVector* col1, const C2Vector* tex, const C4Vector* tangent, const uint16_t* idx, const CGxBatch& batch) {
    PortalEffect* portalEffect = FFX::s_portalEffects.New();
    portalEffect->m_effect = effect;
    portalEffect->m_matrix = matrix;
    portalEffect->m_normal = normal;
    portalEffect->m_col0 = col0;
    portalEffect->m_geo = geo;
    portalEffect->m_tex = tex;
    portalEffect->m_idx = idx;
    portalEffect->m_col1 = col1;
    portalEffect->m_batch = batch;
    portalEffect->m_tangent = tangent;
}

// OFFSET: %0x00801B60
FFX::Effect::Effect() : m_cvar(NULL) {
}

// OFFSET: %0x00801B80
FFX::Effect::~Effect() {
}

// OFFSET: %0x00801BD0
void FFX::SetupGx(Tex* target, int32_t depth) {
    FFX::s_oldProjMtx = g_theGxDevicePtr->m_projection;
    FFX::s_oldProjMtxNative = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        FFX::s_oldProjMtxNative.c0 = FFX::s_oldProjMtxNative.c0 * -1.0f;
        FFX::s_oldProjMtxNative.c1 = FFX::s_oldProjMtxNative.c1 * -1.0f;
        FFX::s_oldProjMtxNative.c2 = FFX::s_oldProjMtxNative.c2 * -1.0f;
        FFX::s_oldProjMtxNative.c3 = FFX::s_oldProjMtxNative.c3 * -1.0f;
    }

    GxXformView(FFX::s_oldViewMtx);

    C44Matrix projection;
    g_theGxDevicePtr->XformSetProjection(projection);

    C44Matrix view;
    g_theGxDevicePtr->XformSetView(view);

    g_theGxDevicePtr->XformPush(GxXform_World);
    g_theGxDevicePtr->m_xforms[GxXform_World].Identity();

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthTest, depth);
    GxRsSet(GxRs_DepthWrite, depth);

    g_theGxDevicePtr->DeviceGetRenderTarget(GxBuffers_Color, FFX::s_oldFrameBuffer);
    g_theGxDevicePtr->DeviceGetRenderTarget(GxBuffers_Depth, FFX::s_oldDepthBuffer);

    GxRenderTargetSet(GxBuffers_Color, target->m_texture ? TextureGetGxTex(target->m_texture) : NULL, 0);
    GxRenderTargetSet(GxBuffers_Depth, target->m_depthTexture ? TextureGetGxTex(target->m_depthTexture) : NULL, 0);

    FFX::s_oldMinX = g_theGxDevicePtr->m_viewport.m_x.m_l;
    FFX::s_oldMaxX = g_theGxDevicePtr->m_viewport.m_x.m_h;
    FFX::s_oldMinY = g_theGxDevicePtr->m_viewport.m_y.m_l;
    FFX::s_oldMaxY = g_theGxDevicePtr->m_viewport.m_y.m_h;
    FFX::s_oldMinZ = g_theGxDevicePtr->m_viewport.m_z.m_l;
    FFX::s_oldMaxZ = g_theGxDevicePtr->m_viewport.m_z.m_h;

    GxXformSetViewport(0.0f, static_cast<float>(target->m_size.x) / target->m_texSize.x, 0.0f, static_cast<float>(target->m_size.y) / target->m_texSize.y, 0.0f, s_viewportMaxZ);
}

// PROVEN: 606608f5 IDENTICAL
// OFFSET: 0x00401000
void FFX::Effect::Begin() {
}

// PROVEN: ef2a8af8 IDENTICAL
// OFFSET: 0x00401000
void FFX::Effect::End() {
}

// PROVEN: 787a0f92 IDENTICAL
// OFFSET: 0x00841AE0
void FFX::Effect::SetParameters(int32_t count, const int32_t* params) {
}
