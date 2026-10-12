#include "FFXEffects.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/FFX/PassGlow.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/Game/GameClient/PassColorShift.h"
#include "WoW/Source/Game/GameClient/PassDeath.h"
#include "WoW/Source/Game/GameClient/PassFogCombine.h"
#include "WoW/Source/Game/GameClient/PassFogSeed.h"
#include "WoW/Source/Game/GameClient/PassNetherBlur.h"
#include "WoW/Source/Game/GameClient/PassPropagateFog.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "Engine/Source/Event/FrameTime.h"

static float s_twoPi = 6.2831855f;

static float s_netherCombineConstants[4] = { 0.6f, 0.6f, 0.78f, 0.0f };

static int32_t s_propagateFogRate = 30;

static uint32_t* s_seedTexels;

static uint32_t s_propagateFogElapsed;

static float s_netherBlurPhase;

// INLINED
static float NoiseValue(CRndSeed& seed) {
    uint32_t bits = (CRandom::uint32_(seed) & 0x7FFFFF) | 0x3F800000;
    float value = *reinterpret_cast<float*>(&bits) - 1.0f;
    return static_cast<float>(fmod(static_cast<double>(value + value - 1.0f), 1.0));
}

// INLINED
static C2Vector PolarCoord(float u, float v) {
    float dx = (u - 0.5f) * 1.4142137f;
    float dy = (v - 0.5f) * 1.4142137f;
    float radius = sqrt(dx * dx + dy * dy);
    float c = 1.0f;

    if (radius != 0.0f) {
        c = dy * (1.0f / radius);
    }

    return C2Vector(static_cast<float>(acos(c) * 0.31830987f), radius);
}

// OFFSET: 0x00B0B7A0
bool EffectUnconscious::FadingOut() const {
    return this->m_fadingOut;
}

// OFFSET: 0x00B0C630
PassPropagateFog::PassPropagateFog(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, const C2Vector* params, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_strength = 0.0f;
    this->m_permute = FFX::s_useRECT != 0;
    memcpy(this->m_params, params, sizeof(this->m_params));
    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXPropagateFog");
}

// OFFSET: 0x00B0B830/0x1408CF6E0
PassFogCombine::PassFogCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_lastSize.x = 0;
    this->m_lastSize.y = 0;
    this->m_density = 0.0f;
    this->m_fadeTime = 0.0f;
    this->m_vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Static, 34848, GxPoolHintBit_Unk0, "Pass Fog Combine Vertex Pool");
    this->m_indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Static, 12288, GxPoolHintBit_Unk0, "Pass Fog Combine Index Pool");
    this->m_vertexBuf = g_theGxDevicePtr->BufCreate(this->m_vertexPool, 32, 1089, 0);
    this->m_indexBuf = g_theGxDevicePtr->BufCreate(this->m_indexPool, 2, 6144, 0);
    this->m_permute = FFX::s_useRECT != 0;
    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXFogCombine");
}

// OFFSET: [0x00B0B910/0x1408CF810, 0x00B0CE60/0x1408D1150]
PassFogCombine::~PassFogCombine() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }

    CGxBuf* vertexBuf = this->m_vertexBuf;

    if (vertexBuf && vertexBuf != g_theGxDevicePtr->BufStream(vertexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(vertexBuf);
    }

    CGxBuf* indexBuf = this->m_indexBuf;

    if (indexBuf && indexBuf != g_theGxDevicePtr->BufStream(indexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(indexBuf);
    }

    if (this->m_vertexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_vertexPool);
    }

    if (this->m_indexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_indexPool);
    }
}

// OFFSET: 0x00B0B9E0/0x1408CF920
void PassFogCombine::BuildCircularMappingVertices(const FFX::Tex& tex0, const FFX::Tex& tex1) {
    struct Vertex {
        C3Vector p;
        CImVector c;
        C2Vector tc[2];
    };

    C44Matrix tex0Mtx;
    C44Matrix tex1Mtx;
    FFX::SetupTexMtx(tex0.m_size, tex0.m_texSize, &tex0Mtx, 1);
    FFX::SetupTexMtx(tex1.m_size, tex1.m_texSize, &tex1Mtx, 1);

    Vertex* vertex = reinterpret_cast<Vertex*>(g_theGxDevicePtr->BufLock(this->m_vertexBuf));

    for (uint32_t y = 0; y <= 32; y++) {
        float v = y * 0.03125f;
        float py = -(v + v - 1.0f);

        for (uint32_t x = 0; x <= 32; x++) {
            float u = x * 0.03125f;
            vertex->p.x = u + u - 1.0f;
            vertex->p.y = py;
            vertex->p.z = 0.0f;

            C2Vector polar = PolarCoord(u, v);
            C4Vector t0;
            t0.x = tex0Mtx.a0 * polar.x + tex0Mtx.b0 * polar.y + tex0Mtx.c0 * 0.0f + tex0Mtx.d0;
            t0.y = tex0Mtx.a1 * polar.x + tex0Mtx.b1 * polar.y + tex0Mtx.c1 * 0.0f + tex0Mtx.d1;
            t0.z = tex0Mtx.a2 * polar.x + tex0Mtx.b2 * polar.y + tex0Mtx.c2 * 0.0f + tex0Mtx.d2;
            t0.w = tex0Mtx.a3 * polar.x + tex0Mtx.b3 * polar.y + tex0Mtx.c3 * 0.0f + tex0Mtx.d3;
            vertex->tc[0] = C2Vector(t0);

            C4Vector t1;
            t1.x = tex1Mtx.a0 * u + tex1Mtx.b0 * v + tex1Mtx.c0 * 0.0f + tex1Mtx.d0;
            t1.y = tex1Mtx.a1 * u + tex1Mtx.b1 * v + tex1Mtx.c1 * 0.0f + tex1Mtx.d1;
            t1.z = tex1Mtx.a2 * u + tex1Mtx.b2 * v + tex1Mtx.c2 * 0.0f + tex1Mtx.d2;
            t1.w = tex1Mtx.a3 * u + tex1Mtx.b3 * v + tex1Mtx.c3 * 0.0f + tex1Mtx.d3;
            vertex->tc[1] = C2Vector(t1);

            vertex->c = CImVector(0xFF, 0xFF, 0xFF, 0xFF);
            vertex++;
        }
    }

    CGxBuf* buf = this->m_vertexBuf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x00B0BE10/0x1408CFED0
void PassFogCombine::BuildCircularMappingIndices() {
    uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(this->m_indexBuf));

    for (uint32_t y = 0; y < 32; y++) {
        for (uint32_t x = 0; x < 32; x++) {
            uint16_t base = static_cast<uint16_t>(33 * y + x);
            index[0] = base + 1;
            index[1] = base + 33;
            index[2] = base;
            index[3] = base + 1;
            index[4] = base + 34;
            index[5] = base + 33;
            index += 6;
        }
    }

    CGxBuf* buf = this->m_indexBuf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x00B0BEB0
PassDeath::PassDeath(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_shader = NULL;
    this->m_nvts2dShader = NULL;
    this->m_nvtsRectShader = NULL;

    int32_t profile = g_theGxDevicePtr->m_caps.m_shaderTargets[GxSh_Pixel];

    if (profile == GxShProfile_Fp20 || profile == GxShProfile_Fp25) {
        this->m_nvts2dShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXDeath_nvts_2d");
        this->m_nvtsRectShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXDeath_nvts_rect");
    } else {
        this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXDeath");
    }

    this->m_permute = FFX::s_useRECT != 0;
}

// OFFSET: [0x00B0BF50/0x1408CFFD0, 0x00B0D1D0]
PassDeath::~PassDeath() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }

    if (this->m_nvts2dShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_nvts2dShader);
        this->m_nvts2dShader = NULL;
    }

    if (this->m_nvtsRectShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_nvtsRectShader);
        this->m_nvtsRectShader = NULL;
    }
}

// OFFSET: 0x00B0BFD0/0x1408D0060
PassNetherBlur::PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_seed.SetSeed(0);
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXNetherBlur");
    this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "FFXNetherBlur");
    this->m_seed.SetSeed(0xABCDEF01);

    for (uint32_t i = 0; i < 36; i++) {
        this->m_noise[0][i] = NoiseValue(this->m_seed);
    }

    for (uint32_t i = 0; i < 36; i++) {
        this->m_noise[1][i] = NoiseValue(this->m_seed);
    }

    for (uint32_t i = 0; i < 36; i++) {
        this->m_noise[2][i] = NoiseValue(this->m_seed);
    }

    this->m_noiseTables[0] = this->m_noise[0];
    this->m_noiseTables[1] = this->m_noise[1];
    this->m_noiseTables[2] = this->m_noise[2];
    this->m_blend = 0.0f;
}

// OFFSET: [0x00B0C150/0x1408D0280, 0x00B0D790/0x1408D1D20]
PassNetherBlur::~PassNetherBlur() {
    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }

    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }
}

// OFFSET: 0x00B0C1B0
PassNetherCombine::PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXNetherCombine");
    this->m_fade = 0.0f;
}

// OFFSET: 0x00B0C2B0/0x1408D02F0
void EffectDeath::SetParameters(int32_t count, const int32_t* params) {
    if (g_theGxDevicePtr->m_caps.m_shaderModel >= 2) {
        static_cast<PassDeath*>(this->m_passes[2])->m_alpha = static_cast<uint8_t>(params[0]);
    }
}

// OFFSET: 0x00B0C2E0/0x1408D0320
void EffectNetherWorld::End() {
    if (this->m_passes.Count() >= 2) {
        static_cast<PassNetherCombine*>(this->m_passes[1])->m_fade = 0.0f;
    }
}

// OFFSET: 0x00B0C300/0x1408D0340
static void SeedTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    static float s_scaleX = 64.0f / width;
    static float s_scaleY = 64.0f / height;

    if (command == GxTex_Lock) {
        s_seedTexels = static_cast<uint32_t*>(SMemAlloc(4 * height * width, __FILE__, __LINE__, 0));

        for (uint32_t y = 0; y < height; y++) {
            float fy = y * s_scaleY;
            uint8_t* texel = reinterpret_cast<uint8_t*>(s_seedTexels) + 4 * width * y;

            for (uint32_t x = 0; x < width; x++) {
                float noise = PerlinNoise2D(x * s_scaleX, fy, 5);
                texel[2] = 0xFF;
                texel[1] = 0xFF;
                texel[0] = 0xFF;
                texel[3] = static_cast<uint8_t>((noise + 3.0f) * 0.25f * 255.0f + 0.5f);
                texel += 4;
            }
        }
    } else if (command == GxTex_Latch) {
        texels = s_seedTexels;
        texelStrideInBytes = 4 * width;
    } else if (command == GxTex_Unlock) {
        SMemFree(s_seedTexels, __FILE__, __LINE__, 0);
        s_seedTexels = NULL;
    }
}

// OFFSET: 0x00B0C4C0/0x1408D0580
void EffectSwirlingFog::SetParameters(int32_t count, const int32_t* params) {
    float strength = params[1] * 0.0039215689f * 0.89999998f;
    float density = params[2] * 0.0099999998f;
    static_cast<PassFogSeed*>(this->m_passes[0])->m_color = *reinterpret_cast<const CImVector*>(&params[0]);
    static_cast<PassPropagateFog*>(this->m_passes[1])->m_strength = strength;
    PassFogCombine* combine = static_cast<PassFogCombine*>(this->m_passes[2]);
    combine->m_density = density;
    combine->m_fadeTime = 3.0f;
}

// OFFSET: 0x00B0C520/0x1408D05F0
void EffectUnconscious::Begin() {
    this->m_fadingOut = false;
    PassGlowFade* pass = static_cast<PassGlowFade*>(this->m_passes[2]);
    pass->m_fadeTime = 5.0f;
    pass->m_fadeInTime = 5.0f;
    pass->m_fadeOutTime = 5.0f;
    pass->m_fadingOut = false;
}

// OFFSET: 0x00B0C550/0x1408D0630
void EffectUnconscious::SetParameters(int32_t count, const int32_t* params) {
    float time = static_cast<float>(params[1]);
    float fadeOutTime = static_cast<float>(params[2]);
    this->m_reportFade = params[3] & 1;
    PassGlowFade* pass = static_cast<PassGlowFade*>(this->m_passes[2]);
    pass->m_fadeColor = *reinterpret_cast<const CImVector*>(&params[0]);
    pass->m_fadeTime = time;
    pass->m_fadeInTime = time;
    pass->m_fadeOutTime = fadeOutTime;
}

// OFFSET: 0x00B0C590/0x1408D0680
float EffectUnconscious::GetFadeAmount() {
    if (!this->m_reportFade) {
        return 1.0f;
    }

    return static_cast<PassGlowFade*>(this->m_passes[2])->GetFadeAmount();
}

// OFFSET: 0x00B0C5B0/0x1408D06A0
void EffectUnconscious::SetGlowBlur(uint8_t glow, uint8_t blur) {
    static_cast<PassGlowFade*>(this->m_passes[2])->m_color = CImVector(blur, blur, blur, glow);
}

// OFFSET: 0x00B0C5E0/0x1408D06D0
bool EffectUnconscious::FadedOut() {
    if (!this->m_fadingOut) {
        static_cast<PassGlowFade*>(this->m_passes[2])->BeginFadeOut();
        this->m_fadingOut = true;
    }

    PassGlowFade* pass = static_cast<PassGlowFade*>(this->m_passes[2]);
    return pass->m_fadingOut && pass->m_fadeTime > pass->m_fadeOutTime;
}

// OFFSET: 0x00B0C6C0/0x1408D0730
PassPropagateFog::~PassPropagateFog() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x00B0C710/0x1408D07E0
void PassPropagateFog::Execute() {
    s_propagateFogElapsed += FrameTime::s_tickTimeMs;

    if (s_propagateFogElapsed < static_cast<uint32_t>(1000 / s_propagateFogRate)) {
        return;
    }

    s_propagateFogElapsed = 0;

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    FFX::SetupVB_PCT4(white, posMtx, texMtx, this->m_params, this->m_inputs[0]->m_texelSize);
    FFX::SetupIB();

    CGxTex* texture = TextureGetGxTex(this->m_inputs[0]->m_texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture2, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture3, texture);

    for (int32_t state = GxRs_Texture4; state <= GxRs_Texture15; state++) {
        GxRsSet(static_cast<EGxRenderState>(state), 0);
    }

    if (FFX::s_passThrough_PCT4) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT4->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    float constants[4];
    constants[0] = this->m_strength;
    constants[1] = constants[0];
    constants[2] = constants[0];
    constants[3] = constants[0];
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, constants, 1);

    GxRsSet(GxRs_BlendingMode, this->m_blend);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B0CE90/0x1408D07A0
bool PassPropagateFog::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// INLINED
bool PassFogCombine::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x00B0CEC0/0x1408D11A0
void PassFogCombine::Execute() {
    FFX::SetupGx(this->m_output, 0);

    bool rebuildVertices = !(this->m_vertexBuf->unk_001C && this->m_vertexBuf->unk_001D);
    bool rebuildIndices = !(this->m_indexBuf->unk_001C && this->m_indexBuf->unk_001D);
    const C2iVector& size = this->m_output->m_size;

    if (this->m_lastSize.x != size.x || this->m_lastSize.y != size.y) {
        this->m_lastSize.x = size.x;
        this->m_lastSize.y = size.y;
        this->BuildCircularMappingVertices(*this->m_inputs[0], *this->m_inputs[1]);
    } else if (rebuildVertices) {
        this->BuildCircularMappingVertices(*this->m_inputs[0], *this->m_inputs[1]);
    }

    if (rebuildIndices) {
        this->BuildCircularMappingIndices();
    }

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[1]->m_texture));

    for (int32_t state = GxRs_Texture2; state <= GxRs_Texture15; state++) {
        GxRsSet(static_cast<EGxRenderState>(state), 0);
    }

    if (FFX::s_passThrough_PCT2) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT2->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);

    float density;
    float minimum;

    if (this->m_fadeTime <= 0.0f) {
        density = this->m_density;
        minimum = 0.1f;
    } else {
        float fade = 1.0f - this->m_fadeTime * 0.33333334f;
        this->m_fadeTime = this->m_fadeTime - FrameTime::s_tickTimeSec;

        if (this->m_fadeTime < 0.0f) {
            this->m_fadeTime = 0.0f;
        }

        density = this->m_density * fade;
        minimum = fade * 0.1f;
    }

    float densityConstants[4] = { density, density, density, density };
    float minimumConstants[4] = { minimum, minimum, minimum, minimum };
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, densityConstants, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, minimumConstants, 1);

    g_theGxDevicePtr->PrimVertexPtr(this->m_vertexBuf, GxVBF_PCT2);
    g_theGxDevicePtr->PrimIndexPtr(this->m_indexBuf);
    GxRsSet(GxRs_BlendingMode, this->m_blend);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = 6144;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 1088;
    g_theGxDevicePtr->Draw(batch);

    FFX::SetupReset();
}

// OFFSET: 0x00B0D200/0x1408D15E0
bool PassDeath::Valid() {
    int32_t profile = g_theGxDevicePtr->m_caps.m_shaderTargets[GxSh_Pixel];

    if (profile == GxShProfile_Fp20 || profile == GxShProfile_Fp25) {
        CGxShader* shader = this->m_permute ? this->m_nvtsRectShader : this->m_nvts2dShader;
        return shader && shader->m_permutes[0].Valid();
    }

    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x00B0D260/0x1408D1670
void PassDeath::Execute() {
    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix tex0Mtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &tex0Mtx, 1);

    C44Matrix tex1Mtx;
    FFX::SetupTexMtx(this->m_inputs[1]->m_size, this->m_inputs[1]->m_texSize, &tex1Mtx, 1);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[1]->m_texture));

    CImVector color;
    color.a = this->m_alpha;
    color.b = 0xA8;
    color.g = 0x93;
    color.r = 0x53;
    FFX::SetupVB_PCT2(color, posMtx, tex0Mtx, tex1Mtx);
    FFX::SetupIB();

    if (FFX::s_passThrough_PCT2) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT2->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    int32_t profile = g_theGxDevicePtr->m_caps.m_shaderTargets[GxSh_Pixel];

    if (profile != GxShProfile_Fp20 && profile != GxShProfile_Fp25) {
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);
    } else if (!this->m_permute) {
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_nvts2dShader->m_permutes[0]);
    } else {
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_nvtsRectShader->m_permutes[this->m_permute]);
    }

    FFX::Render();
    FFX::SetupReset();
}

// INLINED
PassDeathNVRC::PassDeathNVRC(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXDeath_nvrc");
}

// OFFSET: 0x00B0D510/0x1408D19E0
PassDeathNVRC::~PassDeathNVRC() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x00B0D560/0x1408D1A50
bool PassDeathNVRC::Valid() {
    return this->m_shader && this->m_shader->m_permutes[0].Valid();
}

// OFFSET: 0x00B0D580/0x1408D1A80
void PassDeathNVRC::Execute() {
    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    CImVector color(0xA8, 0x93, 0x53, 0xFF);
    FFX::SetupVB_PCT(color, posMtx, texMtx);
    FFX::SetupIB();

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));

    if (FFX::s_passThrough_PCT) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[0]);
    GxRsSet(GxRs_BlendingMode, this->m_blend);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B0D7C0/0x1408D1D70
bool PassNetherBlur::Valid() {
    return this->m_vertexShader && this->m_vertexShader->m_permutes[0].Valid() && this->m_pixelShader && this->m_pixelShader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x00B0D800/0x1408D1DD0
void PassNetherBlur::Execute() {
    float elapsed = FrameTime::s_tickTimeSec;
    float scale = 8.0f;
    s_netherBlurPhase = static_cast<float>(fmod(static_cast<double>(elapsed + elapsed + s_netherBlurPhase), 6.2831854820251465));

    const C44Matrix* view = FFX::GetOldViewMtx();
    float z = (view->c2 + view->b2) * 0.0f + view->a2;
    float x = (view->c0 + view->b0) * 0.0f + view->a0;
    float y = (view->c1 + view->b1) * 0.0f + view->a1;

    if (fabs(z) > 0.000099999997f) {
        float inv = 1.0f / z;
        x = inv * x;
        y = inv * y;
    }

    float invLength = 1.0f / sqrt(y * y + x * x);
    float angle = static_cast<float>(acos(invLength * x));

    if (invLength * y < 0.0f) {
        angle = s_twoPi - angle;
    }

    angle = static_cast<float>(cos(s_netherBlurPhase) * 0.5f + angle);

    if (this->m_blend > 1.0f) {
        this->m_blend = static_cast<float>(fmod(static_cast<double>(this->m_blend), 1.0));
        float* oldest = this->m_noiseTables[0];
        this->m_noiseTables[0] = this->m_noiseTables[1];
        this->m_noiseTables[1] = this->m_noiseTables[2];
        this->m_noiseTables[2] = oldest;

        for (uint32_t i = 0; i < 36; i++) {
            this->m_noiseTables[2][i] = NoiseValue(this->m_seed);
        }
    }

    for (uint32_t i = 0; i < 36; i++) {
        this->m_shade[i] = (1.0f - this->m_blend) * this->m_noiseTables[0][i] + this->m_noiseTables[1][i] * this->m_blend;
    }

    this->m_blend = elapsed * 1.5f + this->m_blend;

    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    C44Matrix texMtx;

    if (!FFX::s_useRECT) {
        FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);
        FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);
        FFX::SetupVB_PCT(posMtx, texMtx, this->m_shade);
    }

    FFX::SetupIB();
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[this->m_permute]);

    float constants[4];
    constants[0] = angle;
    constants[1] = 0.0f;
    constants[2] = 0.0f;
    constants[3] = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, constants, 1);

    if (FFX::s_useRECT) {
        constants[0] = 4.0f;
        constants[1] = 4.0f;
        scale = 4.0f;
    } else {
        constants[0] = this->m_inputs[0]->m_texelSize.x * 8.0f;
        constants[1] = this->m_inputs[0]->m_texelSize.y * 8.0f;
    }

    constants[2] = 0.0f;
    constants[3] = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, constants, 1);

    float convergence = g_theGxDevicePtr->StereoGetConvergence();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 2, &convergence, 1);

    CGxTex* target = TextureGetGxTex(FFX::s_quart[1].m_texture);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, target, 0);
    GxXformSetViewport(0.0f, static_cast<float>(FFX::s_quart[1].m_size.x) / FFX::s_quart[1].m_texSize.x, 0.0f, static_cast<float>(FFX::s_quart[1].m_size.y) / FFX::s_quart[1].m_texSize.y, 0.0f, 1.0f);
    GxSceneClear(15, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(this->m_inputs[0]->m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(this->m_inputs[0]->m_texture));

    if (FFX::s_useRECT) {
        FFX::SetupGeo_Quad(FFX::s_quart[1].m_size, &posMtx);
        FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);
        FFX::SetupVB_PCT(posMtx, texMtx, this->m_shade);
    }

    FFX::Render();

    target = TextureGetGxTex(this->m_output->m_texture);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, target, 0);
    GxXformSetViewport(0.0f, static_cast<float>(this->m_output->m_size.x) / this->m_output->m_texSize.x, 0.0f, static_cast<float>(this->m_output->m_size.y) / this->m_output->m_texSize.y, 0.0f, 1.0f);
    GxSceneClear(15, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(FFX::s_quart[1].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(FFX::s_quart[1].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(FFX::s_quart[1].m_texture));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(FFX::s_quart[1].m_texture));

    if (FFX::s_useRECT) {
        constants[0] = 0.0f;
        constants[1] = 0.0f;
        constants[2] = 0.0f;
        constants[3] = 0.0f;
        constants[0] = this->m_inputs[0]->m_texelSize.x / FFX::s_quart[1].m_texelSize.x * scale;
        constants[1] = this->m_inputs[0]->m_texelSize.y / FFX::s_quart[1].m_texelSize.y * scale;
        constants[2] = 0.0f;
        constants[3] = 0.0f;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, constants, 1);

        FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);
        FFX::SetupTexMtx(FFX::s_quart[1].m_size, FFX::s_quart[1].m_texSize, &texMtx, 1);
        FFX::SetupVB_PCT(posMtx, texMtx, this->m_shade);
    }

    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B0E1C0/0x1408D2B10
PassNetherCombine::~PassNetherCombine() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: [0x0089F840/0x1405E08C0]
bool PassNetherCombine::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x00B0E210/0x1408D2B80
void PassNetherCombine::Execute() {
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

    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);

    if (this->m_fade < 0.75f) {
        float fade = this->m_fade + FrameTime::s_tickTimeSec;
        this->m_fade = fade;
        this->m_fade = fade >= 0.75f ? 0.75f : fade;
    }

    s_netherCombineConstants[3] = this->m_fade;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, s_netherCombineConstants, 1);

    FFX::SetupVB_PCT2(white, posMtx, tex0Mtx, tex1Mtx);
    FFX::SetupIB();
    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B0E790/0x1408D32D0
EffectDeath::EffectDeath() {
    this->m_cvar = CVar::Register("ffxDeath", "full screen death effect", 1, FFX::s_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, NULL, false);

    int32_t shaderModel = g_theGxDevicePtr->m_caps.m_shaderModel;

    if (shaderModel == 1) {
        FFX::Tex* inputs[] = { &FFX::s_full[0] };
        FFX::Pass* pass = new (__FILE__, __LINE__) PassDeathNVRC(inputs, 1, &FFX::s_screen, GxBlend_Opaque);
        this->m_passes.New(pass);
    } else if (shaderModel >= 2) {
        FFX::Tex* inputs[] = { &FFX::s_full[0], &FFX::s_quart[0] };

        FFX::Pass* pass = new (__FILE__, __LINE__) PassBox4(&inputs[0], 1, inputs[1], GxBlend_Opaque);
        this->m_passes.New(pass);

        pass = new (__FILE__, __LINE__) PassGauss4(&inputs[1], 1, inputs[1], GxBlend_Opaque);
        this->m_passes.New(pass);

        pass = new (__FILE__, __LINE__) PassDeath(&inputs[0], 2, &FFX::s_screen, GxBlend_Opaque);
        this->m_passes.New(pass);
    }
}

// OFFSET: 0x00B0E950
EffectDeath::~EffectDeath() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// OFFSET: 0x00B0E9A0/0x1408D36B0
EffectNetherWorld::EffectNetherWorld() {
    this->m_cvar = CVar::Register("ffxNetherWorld", "full screen nether world effect (for invisibility)", 1, "1", FFX::CVarCallback, 1, false, NULL, false);

    if (g_theGxDevicePtr->m_caps.m_shaderModel >= 2) {
        FFX::Tex* inputs[] = { &FFX::s_full[0], &FFX::s_quart[0] };

        FFX::Pass* pass = new (__FILE__, __LINE__) PassNetherBlur(&inputs[0], 1, inputs[1], GxBlend_Opaque);
        this->m_passes.New(pass);

        pass = new (__FILE__, __LINE__) PassNetherCombine(&inputs[0], 2, &FFX::s_screen, GxBlend_Opaque);
        this->m_passes.New(pass);
    }
}

// OFFSET: [0x00B0EAB0, 0x00B0F0E0]
EffectNetherWorld::~EffectNetherWorld() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// OFFSET: 0x00B0EB00/0x1408D3910
EffectSwirlingFog::EffectSwirlingFog() {
    this->m_fogTex.m_texture = NULL;
    this->m_fogTex.m_frameBuffer = NULL;
    this->m_fogTex.m_texSize.x = 0;
    this->m_fogTex.m_texSize.y = 0;
    this->m_fogTex.m_size.x = 0;
    this->m_fogTex.m_size.y = 0;
    this->m_fogTex.m_texelSize.x = 0.0f;
    this->m_fogTex.m_texelSize.y = 0.0f;
    this->m_seedTex.m_texture = NULL;
    this->m_seedTex.m_frameBuffer = NULL;
    this->m_seedTex.m_texSize.x = 0;
    this->m_seedTex.m_texSize.y = 0;
    this->m_seedTex.m_size.x = 0;
    this->m_seedTex.m_size.y = 0;
    this->m_seedTex.m_texelSize.x = 0.0f;
    this->m_seedTex.m_texelSize.y = 0.0f;

    C2iVector seedSize = { 256, 256 };
    C2iVector fogSize = { 256, 128 };
    FFX::CreateTex(this->m_seedTex, seedSize, "EffectSwirlingFog Noise", false, false);
    FFX::CreateTex(this->m_fogTex, fogSize, "EffectSwirlingFog Magic Fog", false, true);
    TextureSetUserData(this->m_seedTex.m_texture, SeedTextureCallback, NULL);

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 0;
    rect.maxX = 0;
    GxTexUpdate(TextureGetGxTex(this->m_seedTex.m_texture), rect, 1);

    this->m_cvar = CVar::Register("ffxSpecial", "full screen test effect", 1, FFX::s_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, NULL, false);

    static C2Vector s_propagateParams[4] = {
        C2Vector(-1.0f, 1.0f),
        C2Vector(0.0f, 1.0f),
        C2Vector(1.0f, 1.0f),
        C2Vector(0.0f, 2.0f),
    };

    FFX::Tex* seedInputs[] = { &this->m_seedTex };
    FFX::Tex* fogInputs[] = { &this->m_fogTex, &FFX::s_full[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassFogSeed(seedInputs, 1, &this->m_fogTex, GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassPropagateFog(fogInputs, 1, &this->m_fogTex, s_propagateParams, GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassFogCombine(fogInputs, 2, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// OFFSET: [0x00B0ED60/0x1408D3E10, 0x00B0F110/0x1408D4400]
EffectSwirlingFog::~EffectSwirlingFog() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }

    if (this->m_fogTex.m_texture) {
        TextureRelease(this->m_fogTex.m_texture);
    }

    if (this->m_fogTex.m_frameBuffer) {
        g_theGxDevicePtr->FrameBufferDelete(this->m_fogTex.m_frameBuffer);
    }

    this->m_fogTex.m_texSize.y = 0;
    this->m_fogTex.m_size.y = 0;
    this->m_fogTex.m_texture = NULL;
    this->m_fogTex.m_texSize.x = 0;
    this->m_fogTex.m_size.x = 0;
    this->m_fogTex.m_texelSize.x = 0.0f;
    this->m_fogTex.m_texelSize.y = 0.0f;
    this->m_fogTex.m_frameBuffer = NULL;

    if (this->m_seedTex.m_texture) {
        TextureRelease(this->m_seedTex.m_texture);
    }

    if (this->m_seedTex.m_frameBuffer) {
        g_theGxDevicePtr->FrameBufferDelete(this->m_seedTex.m_frameBuffer);
    }

    this->m_seedTex.m_texSize.x = 0;
    this->m_seedTex.m_texSize.y = 0;
    this->m_seedTex.m_size.x = 0;
    this->m_seedTex.m_size.y = 0;
    this->m_seedTex.m_texture = NULL;
    this->m_seedTex.m_frameBuffer = NULL;
    this->m_seedTex.m_texelSize.x = 0.0f;
    this->m_seedTex.m_texelSize.y = 0.0f;
}

// OFFSET: 0x00B0EE80/0x1408D3F80
EffectUnconscious::EffectUnconscious() {
    this->m_fadingOut = false;

    FFX::Tex* fullInputs[] = { &FFX::s_full[0] };
    FFX::Tex* quartInputs[] = { &FFX::s_quart[0] };
    FFX::Tex* glowInputs[] = { &FFX::s_full[0], &FFX::s_quart[1] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassBox4(fullInputs, 1, &FFX::s_quart[0], GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGauss4(quartInputs, 1, &FFX::s_quart[1], GxBlend_Opaque);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGlowFade(glowInputs, 2, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// OFFSET: [0x00B0EF90/0x1408D40E0, 0x00B0F140/0x1408D4450]
EffectUnconscious::~EffectUnconscious() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// OFFSET: 0x00B0B750/0x1408CF670
void PassColorShift::SetParameters(int32_t count, const float* params) {
    if (count != 9) {
        return;
    }

    this->m_matrix[0].x = params[0];
    this->m_matrix[0].y = params[1];
    this->m_matrix[0].z = params[2];
    this->m_matrix[1].x = params[3];
    this->m_matrix[1].y = params[4];
    this->m_matrix[1].z = params[5];
    this->m_matrix[2].x = params[6];
    this->m_matrix[2].y = params[7];
    this->m_matrix[2].z = params[8];
}

// OFFSET: 0x00B0C210
PassColorShift::PassColorShift(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend) : FFX::Pass(inputs, count, output, blend) {
    this->m_permute = 0;

    if (FFX::s_useRECT) {
        this->m_permute = 1;
    }

    this->m_matrix[0] = C4Vector(1.0f, 0.0f, 0.0f, 1.0f);
    this->m_matrix[1] = C4Vector(0.0f, 1.0f, 0.0f, 1.0f);
    this->m_matrix[2] = C4Vector(0.0f, 0.0f, 1.0f, 1.0f);
    this->m_shader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FFXColorShift");
}

// OFFSET: 0x00B0C610/0x1408D0710
void EffectColorShift::SetParameters(int32_t count, const int32_t* params) {
    if (count == 9) {
        static_cast<PassColorShift*>(this->m_passes[0])->SetParameters(9, reinterpret_cast<const float*>(params));
    }
}

// OFFSET: [0x00B0E500/0x1408D2F30, 0x00B0E500/0x1408D2F30]
PassColorShift::~PassColorShift() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderRelease(this->m_shader);
        this->m_shader = NULL;
    }
}

// OFFSET: 0x00B0E550/0x1408D2FA0
bool PassColorShift::Valid() {
    return this->m_shader && this->m_shader->m_permutes[this->m_permute].Valid();
}

// OFFSET: 0x00B0E580/0x1408D2FE0
void PassColorShift::Execute() {
    FFX::SetupGx(this->m_output, 0);

    C44Matrix posMtx;
    FFX::SetupGeo_Quad(this->m_output->m_size, &posMtx);

    C44Matrix texMtx;
    FFX::SetupTexMtx(this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, &texMtx, 1);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture));

    if (FFX::s_passThrough_PCT) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &FFX::s_passThrough_PCT->m_permutes[0]);
    } else {
        GxRsSet(GxRs_VertexShader, 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_shader->m_permutes[this->m_permute]);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &this->m_matrix[0].x, 4);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 1, &this->m_matrix[1].x, 4);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 2, &this->m_matrix[2].x, 4);

    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    FFX::SetupVB_PCT(white, posMtx, texMtx);
    FFX::SetupIB();
    FFX::Render();
    FFX::SetupReset();
}

// OFFSET: 0x00B0EFE0/0x1408D4180
EffectColorShift::EffectColorShift() {
    FFX::Tex* inputs[] = { &FFX::s_full[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassColorShift(inputs, 1, &FFX::s_screen, GxBlend_Opaque);
    this->m_passes.New(pass);
}

// OFFSET: [0x00B0F060/0x1408D42C0, 0x00B0F170/0x1408D44A0]
EffectColorShift::~EffectColorShift() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}
