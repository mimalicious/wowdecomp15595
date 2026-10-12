#include "WoW/Source/LiquidSystem/Liquid.h"
#include <cmath>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DayNight/DayNight.h"

// STATIC: 0x00BC705C
static const float s_displMapScale[2] = { 1.0f, 1.0f };

// STATIC: 0x00BC7064
static const float s_perturbAmplitudeScale[2] = { 1.25f, 0.25f };

// STATIC: 0x00D00620/0x140BB0040
static uint16_t s_quadIndices[4] = { 0, 3, 1, 2 };

// STATIC: 0x00D00628
static uint16_t s_quadTriIndices[6] = { 0, 2, 1, 1, 2, 3 };

// STATIC: 0x00E4E908/0x140D37BD0
uint8_t Liquid::s_wmoWaterTexValid;

// STATIC: 0x00E4E909/0x140D37BD1
uint8_t Liquid::s_riverDepthTexValid;

// STATIC: 0x00E4E90A/0x140D37BD2
uint8_t Liquid::s_oceanDepthTexValid;

// STATIC: 0x00E4E90C/0x140D37BD8
static uint32_t s_displMapCurrent[2];

// STATIC: 0x00E4E914/0x140D37BE0
static uint32_t s_displMapPeriod[2];

// STATIC: 0x00E4E91C/0x140D37BE8
static uint32_t s_displMapTexSize[2];

// STATIC: 0x00E4E924/0x140D37BF0
static float s_displMapInterpolation[2];

// STATIC: 0x00E4E92C/0x140D37BF8
static uint32_t s_displMapLastTime[2];

// STATIC: 0x00E4E934/0x140D37C00
static CTexture* s_noiseVolTex;

// STATIC: 0x00E4E938/0x140D37C08
static CGxFrameBuffer* s_blurredReflectionFB;

// STATIC: 0x00E4E93C/0x140D37C10
static CGxFrameBuffer* s_blurredBackBufferFB;

// STATIC: 0x00E4E940/0x140D37C18
static CGxFrameBuffer* s_blurTempFB;

// STATIC: 0x00E4E944/0x140D37C28
static CTexture* s_blurredReflectionBuffer;

// STATIC: 0x00E4E948/0x140D37C28
static CTexture* s_blurredBackBuffer;

// STATIC: 0x00E4E94C/0x140D37C30
static CTexture* s_blurTemp;

// STATIC: 0x00E4E950/0x140D37C38
static CGxFrameBuffer* s_fakeReflectionFB;

// STATIC: 0x00E4E954/0x140D37C40
static CGxFrameBuffer* s_realReflectionFB;

// STATIC: 0x00E4E958/0x140D37C48
static CTexture* s_depthReflectionBuffer;

// STATIC: 0x00E4E95C/0x140D37C50
static CTexture* s_fakeReflectionBuffer;

// STATIC: 0x00E4E960/0x140D37C58
static CTexture* s_realReflectionBuffer;

// STATIC: 0x00E4E964/0x140D37C60
static CTexture* s_backBuffer;

// STATIC: 0x00E4E968/0x140D37C68
static CTexture* s_wmoWaterTex;

// STATIC: 0x00E4E96C/0x140D37C70
static CTexture* s_oceanDepthTex;

// STATIC: 0x00E4E970/0x140D37C78
static CTexture* s_riverDepthTex;

// STATIC: 0x00E4E974/0x140D37C80
uint32_t Liquid::s_kawasePingPong;

// STATIC: 0x00E4E978/0x140D37C84
uint32_t Liquid::s_displMapCount;

// STATIC: 0x00E4E97C/0x140D37C88
CTexture* Liquid::s_displMaps[4];

// STATIC: 0x00E4E98C/0x140D37CA0
CGxFrameBuffer* Liquid::s_displMapFBs[4];

// STATIC: 0x00E4E99C/0x140D37CC8
uint8_t Liquid::s_displMapDirty;

// STATIC: 0x00E4E99D/0x140D37CC9
static uint8_t s_perturbationsReserved;

// STATIC: 0x00E4E9A0/0x140D37CD0
CGxShader* Liquid::s_fourTapBlurVS;

// STATIC: 0x00E4E9A4/0x140D37CD8
CGxShader* Liquid::s_fourTapBlurPS;

// STATIC: 0x00E4E9A8/0x140D37CE0
CGxShader* Liquid::s_propagateDisplMapVS;

// STATIC: 0x00E4E9AC/0x140D37CE8
CGxShader* Liquid::s_propagateDisplMapPS;

// STATIC: 0x00E4E9B0/0x140D37CF0
CGxShader* Liquid::s_perturbDisplMapVS;

// STATIC: 0x00E4E9B4/0x140D37CF8
CGxShader* Liquid::s_perturbDisplMapPS;

// STATIC: 0x00E4E9B8/0x140D37D00
CGxShader* Liquid::s_perturbDisplMapMeshVS;

// STATIC: 0x00E4E9BC/0x140D37D08
CGxShader* Liquid::s_perturbDisplMapMeshPS;

// STATIC: 0x00E4E9C0/0x140D37D10
CGxShader* Liquid::s_clearDisplMapVS;

// STATIC: 0x00E4E9C4/0x140D37D18
CGxShader* Liquid::s_clearDisplMapPS;

// STATIC: 0x00E4E9C8/0x140D37D20
CTexture* Liquid::s_perturbTex;

// STATIC: 0x00E4E9CC/0x140D37D28
static C3Vector s_displMapPos[2];

// STATIC: 0x00E4E9E4/0x140D37D40
static C3Vector s_displMapRawPos[2];

// STATIC: 0x00E4E9FC/0x140D37D58
static C2Vector s_displMapSize[2];

// STATIC: 0x00E4EA0C/0x140D37D68
static TSGrowableArray<uint8_t> s_texBuffer;

// STATIC: 0x00E4EA1C/0x140D37D80
static CDataAllocator s_perturbationAllocator(sizeof(Liquid::DisplMapPerturbation), 64);

// STATIC: 0x00E4EA30/0x140D37DA0
static CDataAllocator s_perturbationMeshAllocator(sizeof(Liquid::DisplMapPerturbationMesh), 16);

// STATIC: 0x00E4EA48/0x140D37DC0
TSGrowableArray<Liquid::DisplMapPerturbation*> Liquid::s_eventPerturbations[2];

// STATIC: 0x00E4EA68
TSGrowableArray<Liquid::DisplMapPerturbation*> Liquid::s_perturbations[2];

// STATIC: 0x00E4EA88/0x140D37E20
TSGrowableArray<Liquid::DisplMapPerturbationMesh*> Liquid::s_eventPerturbationMeshes[2];

// STATIC: 0x00E4EAA8
TSGrowableArray<Liquid::DisplMapPerturbationMesh*> Liquid::s_perturbationMeshes[2];

static void ReserveTexBuffer() {
    if (s_texBuffer.m_count < 2048 && s_texBuffer.m_alloc < 2048) {
        s_texBuffer.Reserve(2048, 1);
    }

    s_texBuffer.m_count = 2048;
}

// OFFSET: 0x007172F0/0x1403EA8A0
static void NoiseVolTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch) {
        texels = userArg;
        texelStrideInBytes = 4 * width;
    }
}

// OFFSET: 0x00717320
static CTexture* CreateBlurBuffer(const char* name) {
    CGxTexFlags flags;
    flags.m_renderTarget = 1;
    return TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, name, 0);
}

// OFFSET: 0x00717360/0x1403EA8D0
int32_t Liquid::HasDynamicDisplMap() {
    if ((Liquid::s_displMapCount > 1 && (!Liquid::s_displMaps[2] || !Liquid::s_displMaps[3])) || !Liquid::s_displMaps[0] || !Liquid::s_displMaps[1]) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x007173A0/0x1403EA910
uint32_t Liquid::GetDynamicDisplMapCnt() {
    return Liquid::s_displMapCount;
}

// OFFSET: 0x007173B0/0x1403EA920
CGxTex* Liquid::GetDynamicDisplMap(uint32_t index) {
    return TextureGetGxTex(Liquid::s_displMaps[s_displMapCurrent[index]]);
}

// OFFSET: 0x007173D0/0x1403EA940
float Liquid::GetDynamicDisplMapInterpolation(uint32_t index) {
    return s_displMapInterpolation[index];
}

// OFFSET: 0x007173E0
void __fastcall Liquid::OnTexturesDirty() {
    Liquid::s_displMapDirty = 1;
    Liquid::SignalConfigChanged(1);
}

// OFFSET: 0x007173F0/0x1403EA960
void Liquid::CacheProceduralTextures() {
    CGxTexFlags flags;
    TextureCacheNewTexture(s_riverDepthTex);
    TextureCacheNewTexture(s_oceanDepthTex);
    TextureCacheNewTexture(s_wmoWaterTex);

    if (s_noiseVolTex) {
        TextureCacheNewTexture(s_noiseVolTex);
    }

    for (uint32_t i = 0; i < 2 * Liquid::s_displMapCount; i++) {
        if (Liquid::s_displMaps[i]) {
            TextureCacheNewTexture(Liquid::s_displMaps[i]);
        }
    }
}

// OFFSET: 0x00717470/0x1403EA9F0
void Liquid::UnCacheProceduralTextures() {
    if (s_riverDepthTex) {
        TextureCacheRemoveTexture(s_riverDepthTex);
    }

    if (s_oceanDepthTex) {
        TextureCacheRemoveTexture(s_oceanDepthTex);
    }

    if (s_wmoWaterTex) {
        TextureCacheRemoveTexture(s_wmoWaterTex);
    }

    if (s_noiseVolTex) {
        TextureCacheRemoveTexture(s_noiseVolTex);
    }

    for (uint32_t i = 0; i < 2 * Liquid::s_displMapCount; i++) {
        if (Liquid::s_displMaps[i]) {
            TextureCacheRemoveTexture(Liquid::s_displMaps[i]);
        }
    }
}

// OFFSET: 0x00717520/0x1403EAAA0
void Liquid::GrabBackBuffer() {
    if (!Liquid::NeedBackBuffer() || !s_backBuffer) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(s_backBuffer);

    if (tex) {
        GxTexStretch(NULL, tex, NULL, NULL, 0, 0);
    }
}

// OFFSET: 0x00717560/0x1403EAAE0
CTexture* Liquid::GetBackBuffer() {
    return s_backBuffer;
}

// OFFSET: 0x00717570/0x1403EAAF0
CTexture* Liquid::GetBlurredBackBuffer() {
    return s_blurredBackBuffer;
}

// OFFSET: 0x00717580/0x1403EAB00
CGxFrameBuffer* Liquid::GetBlurredBackBufferFB() {
    return s_blurredBackBufferFB;
}

// OFFSET: 0x00717590/0x1403EAB10
CTexture* Liquid::GetRealReflectionBuffer() {
    return s_realReflectionBuffer;
}

// OFFSET: 0x007175A0/0x1403EAAF0
CTexture* Liquid::GetBlurredReflectionBuffer() {
    return s_blurredReflectionBuffer;
}

// OFFSET: 0x007175B0/0x1403EAB30
CGxFrameBuffer* Liquid::GetBlurredReflectionBufferFB() {
    return s_blurredReflectionFB;
}

// OFFSET: 0x007175C0/0x1403EAB40
CTexture* Liquid::GetFakeReflectionBuffer() {
    return s_fakeReflectionBuffer;
}

// OFFSET: 0x007175D0/0x1403EAB50
CGxFrameBuffer* Liquid::GetRealReflectionFB() {
    return s_realReflectionFB;
}

// OFFSET: 0x007175E0/0x1403EAB60
CGxFrameBuffer* Liquid::GetFakeReflectionFB() {
    return s_fakeReflectionFB;
}

// OFFSET: 0x007175F0/0x1403EAB70
void Liquid::GetDisplMapInfoRaw(uint32_t index, C3Vector& pos, C2Vector& size) {
    pos = s_displMapRawPos[index];
    size = s_displMapSize[index];
}

// OFFSET: 0x00717770/0x1403EAD90
static CTexture* CreateBackBuffer() {
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    g_theGxDevicePtr->CapsWindowSize(rect);

    CGxTexFlags flags;
    flags.m_renderTarget = 1;
    return TextureCreate(GxTex_NonPow2, static_cast<uint32_t>(static_cast<int64_t>(rect.maxX)), static_cast<uint32_t>(static_cast<int64_t>(rect.maxY)), 1, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, "LiquidProc BackBufferTex", 0);
}

// OFFSET: 0x00717820/0x1403EAE40
static CTexture* CreateReflectionBuffer(const char* name) {
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    g_theGxDevicePtr->CapsWindowSize(rect);

    CGxTexFlags flags;
    flags.m_renderTarget = 1;
    uint32_t downscale = Liquid::GetReflectionDownscale();
    uint32_t width = static_cast<uint32_t>(static_cast<int64_t>(rect.maxX)) / (1 << downscale);
    uint32_t height = static_cast<uint32_t>(static_cast<int64_t>(rect.maxY)) / (1 << downscale);
    return TextureCreate(GxTex_NonPow2, width, height, 1, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, GxTexNullUserFunc, name, 0);
}

// OFFSET: 0x007178E0/0x1403EAF10
static CTexture* CreateDepthReflectionBuffer(const char* name) {
    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    g_theGxDevicePtr->CapsWindowSize(rect);

    CGxTexFlags flags;
    flags.m_renderTarget = 1;
    uint32_t scale = 1 << Liquid::GetReflectionDownscale();
    EGxTexFormat format = g_theGxDevicePtr->DeviceApi() != 1 ? GxTex_D32 : GxTex_D24X8;

    if (g_theGxDevicePtr->DeviceApi() == 3) {
        format = GxTex_D16;
    }

    uint32_t width = static_cast<uint32_t>(static_cast<int64_t>(rect.maxX)) / scale;
    uint32_t height = static_cast<uint32_t>(static_cast<int64_t>(rect.maxY)) / scale;
    return TextureCreate(GxTex_NonPow2, width, height, 1, format, format, flags, NULL, GxTexNullUserFunc, name, 0);
}

// OFFSET: 0x007179D0/0x1403EB020
void Liquid::UpdateProceduralTextures() {
    if (!Liquid::s_oceanDepthTexValid && s_oceanDepthTex) {
        CGxTex* tex = TextureGetGxTex(s_oceanDepthTex);

        if (tex) {
            CiRect rect(0, 0, tex->m_height, tex->m_width);
            GxTexUpdate(tex, rect, 0);
            Liquid::s_oceanDepthTexValid = 1;
        }
    }

    if (!Liquid::s_riverDepthTexValid && s_riverDepthTex) {
        CGxTex* tex = TextureGetGxTex(s_riverDepthTex);

        if (tex) {
            CiRect rect(0, 0, tex->m_height, tex->m_width);
            GxTexUpdate(tex, rect, 0);
            Liquid::s_riverDepthTexValid = 1;
        }
    }

    if (!Liquid::s_wmoWaterTexValid && s_wmoWaterTex) {
        CGxTex* tex = TextureGetGxTex(s_wmoWaterTex);

        if (tex) {
            CiRect rect(0, 0, tex->m_height, tex->m_width);
            GxTexUpdate(tex, rect, 0);
            Liquid::s_wmoWaterTexValid = 1;
        }
    }
}

// OFFSET: 0x00717AB0/0x1403EB110
void Liquid::SetDisplMapInfo(const C3Vector& pos, const C2Vector& size) {
    for (uint32_t i = 0; i < Liquid::s_displMapCount; i++) {
        float texSize = static_cast<float>(s_displMapTexSize[i]);
        float width = size.x * s_displMapScale[i];
        float height = s_displMapScale[i] * size.y;

        s_displMapPos[i].x = pos.x - width * 0.5f;
        s_displMapPos[i].y = pos.y - height * 0.5f;
        s_displMapPos[i].z = pos.z;
        s_displMapSize[i].x = width;
        s_displMapSize[i].y = height;

        s_displMapPos[i].x = floor(s_displMapPos[i].x * (texSize / width)) / (texSize / width);
        s_displMapPos[i].y = floor(s_displMapPos[i].y * (texSize / height)) / (texSize / height);
    }
}

// OFFSET: 0x00717BD0/0x1403EB2A0
void Liquid::ClearPerturbatorCache() {
    s_perturbationAllocator.Clear(typeid(Liquid::DisplMapPerturbation).raw_name(), SLOG_OBJECT, 1);
    s_perturbationMeshAllocator.Clear(typeid(Liquid::DisplMapPerturbationMesh).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x00717C10/0x1403EB300
C2iVector Liquid::GetDisplMapTexSize(uint32_t index) {
    C2iVector size;
    size.x = s_displMapTexSize[index];
    size.y = s_displMapTexSize[index];
    return size;
}

// OFFSET: 0x00717CF0
static void WmoWaterTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    ReserveTexBuffer();

    CImVector* buffer = reinterpret_cast<CImVector*>(s_texBuffer.m_data);

    if (command != GxTex_Latch) {
        return;
    }

    const DayNight::LiquidColors* colors = DayNight::GetLiquidColors(0);
    uint32_t alphaStart = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[0] * 255.0f));
    uint32_t alphaEnd = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[1] * 255.0f));
    int32_t step = static_cast<int32_t>((alphaEnd - alphaStart) << 8) >> 6;

    CImVector color = colors->m_color;
    CImVector white;
    white.b = 0xFF;
    white.g = 0xFF;
    white.r = 0xFF;

    uint32_t alpha = alphaStart << 8;
    CImVector* row = buffer;

    for (uint32_t y = 0; y < 64; y++) {
        color.a = static_cast<uint8_t>(alpha >> 8);
        white.a = static_cast<uint8_t>(alpha >> 8);

        row[0] = color;
        row[1] = color;
        row[2] = color;
        row[3] = color;
        row[4] = white;
        row[5] = white;
        row[6] = white;
        row[7] = white;

        alpha += step;
        row += 8;
    }

    texelStrideInBytes = 32;
    texels = buffer;
}

// OFFSET: 0x00717E60/0x1403EB730
static void ExtWaterDepthTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    ReserveTexBuffer();

    CImVector* buffer = reinterpret_cast<CImVector*>(s_texBuffer.m_data);

    if (command != GxTex_Latch) {
        return;
    }

    uint32_t index = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(userArg));
    const DayNight::LiquidColors* colors = DayNight::GetLiquidColors(index);

    uint8_t alphaStart[2];
    uint8_t alphaEnd[2];
    alphaStart[0] = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[2] * 255.0f));
    alphaStart[1] = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[0] * 255.0f));
    alphaEnd[0] = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[3] * 255.0f));
    alphaEnd[1] = static_cast<uint8_t>(CMath::fint_(colors->m_alpha[1] * 255.0f));

    CImVector colorStart[2];
    CImVector colorEnd[2];
    colorStart[0] = colors->m_deepColor[0];
    colorStart[1] = colors->m_shallowColor[0];
    colorEnd[0] = colors->m_deepColor[1];
    colorEnd[1] = colors->m_shallowColor[1];

    CImVector from = colorStart[index];
    CImVector to = colorEnd[index];

    int32_t stepA = static_cast<int32_t>((alphaEnd[index] - alphaStart[index]) << 8) >> 6;
    int32_t stepR = static_cast<int32_t>((to.r - from.r) << 8) >> 6;
    int32_t stepG = static_cast<int32_t>((to.g - from.g) << 8) >> 6;
    int32_t stepB = static_cast<int32_t>((to.b - from.b) << 8) >> 6;

    uint32_t a = alphaStart[index] << 8;
    uint32_t r = from.r << 8;
    uint32_t g = from.g << 8;
    uint32_t b = from.b << 8;

    uint32_t stride = 4 * width;
    uint8_t* row = reinterpret_cast<uint8_t*>(buffer);

    for (uint32_t y = 0; y < 64; y++) {
        CImVector color;
        color.b = static_cast<uint8_t>(b >> 8);
        color.g = static_cast<uint8_t>(g >> 8);
        color.r = static_cast<uint8_t>(r >> 8);
        color.a = static_cast<uint8_t>(a >> 8);

        if (y == 63 && !index) {
            C3Vector rgb(color);
            C3Vector hsv(0.0f, 0.0f, 0.0f);
            RGBtoHSV(rgb, hsv);
            hsv.z = hsv.z * 0.89999998f;
            HSVtoRGB(hsv, rgb);
            color = rgb;
        }

        r += stepR;
        a += stepA;

        CImVector* pixels = reinterpret_cast<CImVector*>(row);

        for (uint32_t x = 0; x < 8; x++) {
            pixels[x] = color;
        }

        b += stepB;
        g += stepG;
        row += stride;
    }

    texelStrideInBytes = 32;
    texels = buffer;
}

// OFFSET: 0x00718100/0x1403EBBC0
static CTexture* CreateNoiseVolTex() {
    CGxTexFlags flags;
    flags.m_wrapU = 1;
    flags.m_wrapV = 1;
    flags.m_wrapW = 1;

    TSGrowableArray<uint32_t> noise;
    noise.SetCount(0x40000);

    SFileImpl* file;

    if (SFileOpenFile("XTextures\\FX\\noise.blob", &file)) {
        SFile::Read(file, noise.m_data, 4 * noise.Count(), NULL);
        SFileCloseFile(file);
    }

    CTexture* texture = TextureCreate(GxTex_Volume, 128, 128, 16, GxTex_Argb8888, GxTex_Argb8888, flags, noise.m_data, NoiseVolTexCallback, "proceduralNoiseVolTex", 0);
    GxTexUpdate(TextureGetGxTex(texture), 0, 0, 0, 0, 1);
    return texture;
}

// OFFSET: 0x007181E0/0x1403EBD20
Liquid::DisplMapPerturbation* Liquid::CreatePerturbator(EPertSurface surface, int32_t force) {
    uint32_t index = surface;

    if (Liquid::s_displMapCount - 1 < index) {
        index = Liquid::s_displMapCount - 1;
    }

    if (!force && Liquid::s_perturbations[index].Count() >= 256) {
        return NULL;
    }

    DisplMapPerturbation* perturbation = static_cast<DisplMapPerturbation*>(s_perturbationAllocator.GetData(0, typeid(DisplMapPerturbation).raw_name(), SLOG_OBJECT));

    if (perturbation) {
        perturbation->m_pos = C3Vector(0.0f, 0.0f, 0.0f);
    }

    Liquid::s_perturbations[index].New(perturbation);
    return perturbation;
}

// OFFSET: 0x00718260/0x1403EBDD0
void Liquid::DestroyPerturbator(EPertSurface surface, DisplMapPerturbation* perturbation) {
    uint32_t index = Liquid::s_displMapCount - 1;

    if (index >= static_cast<uint32_t>(surface)) {
        index = surface;
    }

    TSGrowableArray<DisplMapPerturbation*>& perturbations = Liquid::s_perturbations[index];
    uint32_t count = perturbations.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (perturbations.m_data[i] == perturbation) {
            perturbations.m_data[i] = perturbations.m_data[count - 1];
            perturbations.SetCount(count - 1);
            s_perturbationAllocator.PutData(perturbation, NULL, 0);
            return;
        }
    }
}

// OFFSET: 0x00718300/0x1403EBE50
void Liquid::AddPerturbation(EPertSurface surface, const C3Vector& pos, float radius, float amplitude) {
    uint32_t index = Liquid::s_displMapCount - 1;

    if (index >= static_cast<uint32_t>(surface)) {
        index = surface;
    }

    if (Liquid::s_eventPerturbations[index].Count() >= 256) {
        return;
    }

    DisplMapPerturbation* perturbation = static_cast<DisplMapPerturbation*>(s_perturbationAllocator.GetData(0, typeid(DisplMapPerturbation).raw_name(), SLOG_OBJECT));

    if (perturbation) {
        perturbation->m_pos = C3Vector(0.0f, 0.0f, 0.0f);
    }

    Liquid::s_eventPerturbations[index].New(perturbation);
    perturbation->m_pos = pos;
    perturbation->m_radius = radius;
    perturbation->m_amplitude = amplitude;
    perturbation->m_period = 0;
}

// INLINED
Liquid::DisplMapPerturbationMesh::DisplMapPerturbationMesh() {
}

// OFFSET: 0x007183A0
static Liquid::DisplMapPerturbationMesh* NewPerturbationMesh(int32_t zero) {
    Liquid::DisplMapPerturbationMesh* mesh = static_cast<Liquid::DisplMapPerturbationMesh*>(s_perturbationMeshAllocator.GetData(zero, typeid(Liquid::DisplMapPerturbationMesh).raw_name(), SLOG_OBJECT));

    if (mesh) {
        new (mesh) Liquid::DisplMapPerturbationMesh();
    }

    return mesh;
}

// OFFSET: 0x00718440/0x1403EBF30
void Liquid::KawaseBlur(CTexture* source, CTexture* target, CGxFrameBuffer* frameBuffer, float a4) {
    if (!Liquid::NeedBackBuffer()) {
        return;
    }

    CGxTex* textures[2];
    textures[0] = TextureGetGxTex(target);
    textures[1] = TextureGetGxTex(s_blurTemp);

    CGxFrameBuffer* frameBuffers[2];
    frameBuffers[0] = frameBuffer;
    frameBuffers[1] = s_blurTempFB;

    CGxTex* sourceTex = source ? TextureGetGxTex(source) : NULL;

    CBoundingBox viewport = g_theGxDevicePtr->m_viewport;
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    GxTexStretch(sourceTex, textures[0], NULL, NULL, 0, 0);

    CGxFrameBuffer* oldFrameBuffer = g_theGxDevicePtr->FrameBufferGet();

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), 4);
    C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(vertexBuf));
    vertices[0] = C3Vector(-1.0f, -1.0f, 0.0f);
    vertices[1] = C3Vector(-1.0f, 1.0f, 0.0f);
    vertices[2] = C3Vector(1.0f, 1.0f, 0.0f);
    vertices[3] = C3Vector(1.0f, -1.0f, 0.0f);
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 4);
    GxBufData(indexBuf, s_quadIndices, sizeof(s_quadIndices), 0);

    CGxBatch batch;
    batch.m_minIndex = 0;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = 4;
    batch.m_maxIndex = 3;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_P);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_fourTapBlurVS->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_fourTapBlurPS->m_permutes[0]);

    uint32_t width;
    uint32_t height;
    TextureGetDimensions(s_blurTemp, &width, &height, 1);

    float texel[4];
    texel[0] = 1.0f / static_cast<float>(width);
    texel[1] = 1.0f / static_cast<float>(height);
    texel[2] = texel[0] * 0.5f;
    texel[3] = texel[1] * 0.5f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, texel, 1);

    s_kawasePingPong = 0;

    for (uint32_t i = 0; i < 5; i++) {
        float offset[4];
        offset[0] = texel[0] * static_cast<float>(i) * a4;
        offset[1] = static_cast<float>(i) * texel[1] * a4;
        offset[2] = 0.0f;
        offset[3] = 0.0f;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, offset, 1);

        s_kawasePingPong ^= 1;
        CGxFrameBuffer* passFrameBuffer = frameBuffers[s_kawasePingPong];
        GxRsSet(GxRs_ScissorTest, 0);
        g_theGxDevicePtr->FrameBufferSet(passFrameBuffer);
        g_theGxDevicePtr->RsSet(GxRs_Texture0, textures[(s_kawasePingPong - 1) & 1]);
        g_theGxDevicePtr->RsSet(GxRs_Texture1, textures[(s_kawasePingPong - 1) & 1]);
        g_theGxDevicePtr->RsSet(GxRs_Texture2, textures[(s_kawasePingPong - 1) & 1]);
        g_theGxDevicePtr->RsSet(GxRs_Texture3, textures[(s_kawasePingPong - 1) & 1]);
        g_theGxDevicePtr->Draw(batch);
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(viewport.m_x.m_l, viewport.m_x.m_h, viewport.m_y.m_l, viewport.m_y.m_h, viewport.m_z.m_l, viewport.m_z.m_h);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(oldFrameBuffer);
}

// OFFSET: 0x00718930/0x1403EC5A0
Liquid::DisplMapPerturbationMesh* Liquid::CreatePerturbatorMesh(EPertSurface surface) {
    uint32_t index = Liquid::s_displMapCount - 1;

    if (index >= static_cast<uint32_t>(surface)) {
        index = surface;
    }

    if (Liquid::s_perturbationMeshes[index].Count() >= 16) {
        return NULL;
    }

    DisplMapPerturbationMesh* mesh = NewPerturbationMesh(0);
    Liquid::s_perturbationMeshes[index].New(mesh);
    return mesh;
}

// OFFSET: 0x00718980/0x1403EC690
void Liquid::DestroyPerturbatorMesh(EPertSurface surface, DisplMapPerturbationMesh* mesh) {
    uint32_t index = Liquid::s_displMapCount - 1;

    if (index >= static_cast<uint32_t>(surface)) {
        index = surface;
    }

    TSGrowableArray<DisplMapPerturbationMesh*>& meshes = Liquid::s_perturbationMeshes[index];
    uint32_t count = meshes.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (meshes.m_data[i] == mesh) {
            meshes.m_data[i] = meshes.m_data[count - 1];
            meshes.SetCount(count - 1);

            if (mesh->m_indices.m_data) {
                SMemFree(mesh->m_indices.m_data, typeid(uint16_t).raw_name(), SLOG_OBJECT, 0);
            }

            if (mesh->m_vertices.m_data) {
                SMemFree(mesh->m_vertices.m_data, typeid(C3Vector).raw_name(), SLOG_OBJECT, 0);
            }

            s_perturbationMeshAllocator.PutData(mesh, NULL, 0);
            return;
        }
    }
}

// OFFSET: 0x00718A60/0x1403EC760
void Liquid::ClearEventPerturbations() {
    for (uint32_t i = 0; i < Liquid::s_displMapCount; i++) {
        for (uint32_t j = 0; j < Liquid::s_eventPerturbations[i].Count(); j++) {
            s_perturbationAllocator.PutData(Liquid::s_eventPerturbations[i].m_data[j], NULL, 0);
        }

        for (uint32_t j = 0; j < Liquid::s_eventPerturbationMeshes[i].Count(); j++) {
            DisplMapPerturbationMesh* mesh = Liquid::s_eventPerturbationMeshes[i].m_data[j];

            if (mesh->m_indices.m_data) {
                SMemFree(mesh->m_indices.m_data, typeid(uint16_t).raw_name(), SLOG_OBJECT, 0);
            }

            if (mesh->m_vertices.m_data) {
                SMemFree(mesh->m_vertices.m_data, typeid(C3Vector).raw_name(), SLOG_OBJECT, 0);
            }

            s_perturbationMeshAllocator.PutData(mesh, NULL, 0);
        }

        if (Liquid::s_eventPerturbations[i].m_data) {
            SMemFree(Liquid::s_eventPerturbations[i].m_data, typeid(DisplMapPerturbation*).raw_name(), SLOG_OBJECT, 0);
        }

        Liquid::s_eventPerturbations[i].m_alloc = 0;
        Liquid::s_eventPerturbations[i].m_count = 0;
        Liquid::s_eventPerturbations[i].m_data = NULL;

        if (Liquid::s_eventPerturbationMeshes[i].m_data) {
            SMemFree(Liquid::s_eventPerturbationMeshes[i].m_data, typeid(DisplMapPerturbationMesh*).raw_name(), SLOG_OBJECT, 0);
        }

        Liquid::s_eventPerturbationMeshes[i].m_alloc = 0;
        Liquid::s_eventPerturbationMeshes[i].m_count = 0;
        Liquid::s_eventPerturbationMeshes[i].m_data = NULL;
    }
}

// OFFSET: 0x00718BB0/0x1403EC940
void Liquid::ClearDisplMap() {
    uint16_t indices[6] = { 0, 2, 1, 1, 2, 3 };

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), 4);
    C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(vertexBuf));
    vertices[0] = C3Vector(-1.0f, 1.0f, 0.0f);
    vertices[1] = C3Vector(1.0f, 1.0f, 0.0f);
    vertices[2] = C3Vector(-1.0f, -1.0f, 0.0f);
    vertices[3] = C3Vector(1.0f, -1.0f, 0.0f);
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6);
    uint16_t* indexData = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    memcpy(indexData, indices, sizeof(indices));
    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_P);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &Liquid::s_clearDisplMapVS->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &Liquid::s_clearDisplMapPS->m_permutes[0]);

    for (uint32_t i = 0; i < 2 * Liquid::s_displMapCount; i++) {
        CGxFrameBuffer* frameBuffer = Liquid::s_displMapFBs[i];
        GxRsSet(GxRs_ScissorTest, 0);
        g_theGxDevicePtr->FrameBufferSet(frameBuffer);

        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 6;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 3;
        g_theGxDevicePtr->Draw(batch);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00718FA0/0x1403ECDF0
void Liquid::PerturbDisplMap(TSGrowableArray<DisplMapPerturbation*>& perturbations, uint32_t index, uint32_t time) {
    uint32_t count = perturbations.Count();

    if (!count) {
        return;
    }

    CGxTex* perturbTex = TextureGetGxTex(Liquid::s_perturbTex);

    if (!perturbTex) {
        return;
    }

    float texSize = static_cast<float>(s_displMapTexSize[index]);
    float texel = 1.0f / texSize;
    float texelConst[4];
    texelConst[0] = texel;
    texelConst[1] = texel;
    texelConst[2] = texel * 0.5f;
    texelConst[3] = texelConst[2];

    static uint32_t s_startTime = time;
    uint32_t elapsed = time - s_startTime;

    uint16_t quadIndices[6];
    quadIndices[0] = 0;
    quadIndices[3] = 1;
    quadIndices[2] = 1;
    quadIndices[1] = 2;
    quadIndices[4] = 2;
    quadIndices[5] = 3;

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 20, 4 * count);
    float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6 * count);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));

    for (uint32_t i = 0; i < count; i++) {
        DisplMapPerturbation* perturbation = perturbations.m_data[i];
        float amplitude = perturbation->m_amplitude;

        if (fabs(amplitude) < 0.001f) {
            continue;
        }

        float radiusY = perturbation->m_radius / s_displMapSize[index].y;
        float radiusX = perturbation->m_radius / s_displMapSize[index].x;
        float sizeY = floor(texSize * radiusY);
        sizeY = sizeY >= 0.5f ? sizeY : 0.5f;
        float sizeX = floor(texSize * radiusX);
        sizeX = sizeX >= 0.5f ? sizeX : 0.5f;
        sizeX = texel * sizeX;
        sizeY = texel * sizeY;

        float u = (perturbation->m_pos.x - s_displMapRawPos[index].x) / s_displMapSize[index].x;
        float v = (perturbation->m_pos.y - s_displMapRawPos[index].y) / s_displMapSize[index].y;
        u = floor(texSize * u) * texel;
        v = floor(texSize * v) * texel;

        if (u + sizeX < 0.0f || v + sizeY < 0.0f || u - sizeX > 1.0f || v - sizeY > 1.0f) {
            continue;
        }

        if (g_theGxDevicePtr->m_caps.m_pixelCenterOnEdge) {
            u = u - texelConst[2];
            v = v - texelConst[3];
        }

        float y = (0.5f - v) * 2.0f;
        float height = sizeY * 2.0f;
        float x = (u - 0.5f) * 2.0f;
        float width = sizeX * 2.0f;
        float z = s_perturbAmplitudeScale[index] * amplitude;

        if (z < -1.0f) {
            z = -1.0f;
        } else if (z >= 1.0f) {
            z = 1.0f;
        }

        if (perturbation->m_period) {
            z = static_cast<float>(sin(static_cast<double>(elapsed % perturbation->m_period) * 6.2831855f / static_cast<double>(perturbation->m_period))) * z;
        }

        vertices[0] = x - width;
        vertices[1] = height + y;
        vertices[2] = z;
        vertices[3] = 0.0f;
        vertices[4] = 0.0f;
        vertices[5] = width + x;
        vertices[6] = height + y;
        vertices[7] = z;
        vertices[8] = 1.0f;
        vertices[9] = 0.0f;
        vertices[10] = x - width;
        vertices[11] = y - height;
        vertices[12] = z;
        vertices[13] = 0.0f;
        vertices[14] = 1.0f;
        vertices[15] = width + x;
        vertices[16] = y - height;
        vertices[17] = z;
        vertices[18] = 1.0f;
        vertices[19] = 1.0f;

        for (uint32_t j = 0; j < 6; j++) {
            indices[j] = static_cast<uint16_t>(vertexCount + quadIndices[j]);
        }

        vertices += 20;
        indices += 6;
        vertexCount += 4;
        indexCount += 6;
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;

    if (!vertexCount) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 3);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PT);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &Liquid::s_perturbDisplMapVS->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &Liquid::s_perturbDisplMapPS->m_permutes[0]);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, texelConst, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, perturbTex);

    CGxFrameBuffer* frameBuffer = Liquid::s_displMapFBs[s_displMapCurrent[index]];
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);

    CGxBatch batch;
    batch.m_minIndex = 0;
    batch.m_count = indexCount;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount - 1);
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00719690/0x1403ED680
void Liquid::PerturbDisplMapMesh(TSGrowableArray<DisplMapPerturbationMesh*>& meshes, uint32_t index, uint32_t time) {
    uint32_t count = meshes.Count();

    if (!count) {
        return;
    }

    static uint32_t s_startTime = time;
    uint32_t elapsed = time - s_startTime;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 3);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &Liquid::s_perturbDisplMapMeshVS->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &Liquid::s_perturbDisplMapMeshPS->m_permutes[0]);
    GxFrameBufferSet(Liquid::s_displMapFBs[s_displMapCurrent[index]]);

    uint32_t start = 0;

    do {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        uint32_t end = start;

        for (; end < count; end++) {
            DisplMapPerturbationMesh* mesh = meshes.m_data[end];

            if (0.001f > fabs(mesh->m_amplitude)) {
                continue;
            }

            if (mesh->m_vertices.Count() + vertexCount >= 0x10000 || mesh->m_indices.Count() + indexCount >= 0x20000) {
                break;
            }

            vertexCount += mesh->m_vertices.Count();
            indexCount += mesh->m_indices.Count();
        }

        if (vertexCount) {
            uint32_t baseVertex = 0;
            uint32_t writtenIndices = 0;

            CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), vertexCount);
            float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));
            CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
            uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));

            for (uint32_t i = start; i < end; i++) {
                DisplMapPerturbationMesh* mesh = meshes.m_data[i];
                float amplitude = mesh->m_amplitude;

                if (fabs(amplitude) < 0.001f) {
                    continue;
                }

                float z = s_perturbAmplitudeScale[index] * amplitude;

                if (z < -1.0f) {
                    z = -1.0f;
                } else if (z >= 1.0f) {
                    z = 1.0f;
                }

                if (mesh->m_period) {
                    z = static_cast<float>(sin(static_cast<double>(elapsed % mesh->m_period) * 6.2831855f / static_cast<double>(mesh->m_period))) * z;
                }

                const C44Matrix& m = mesh->m_transform;

                for (uint32_t k = 0; k < mesh->m_vertices.Count(); k++) {
                    const C3Vector& p = mesh->m_vertices.m_data[k];
                    float x = p.z * m.c0 + p.x * m.a0 + p.y * m.b0 + m.d0;
                    float y = p.y * m.b1 + p.z * m.c1 + p.x * m.a1 + m.d1;
                    vertices[0] = ((x - s_displMapRawPos[index].x) / s_displMapSize[index].x - 0.5f) * 2.0f;
                    vertices[1] = (0.5f - (y - s_displMapRawPos[index].y) / s_displMapSize[index].y) * 2.0f;
                    vertices[2] = z;
                    vertices += 3;
                }

                for (uint32_t j = 0; j < mesh->m_indices.Count(); j++) {
                    *indices++ = static_cast<uint16_t>(baseVertex + mesh->m_indices.m_data[j]);
                }

                baseVertex += mesh->m_vertices.Count();
                writtenIndices += mesh->m_indices.Count();
            }

            g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
            vertexBuf->unk_001C = 1;
            g_theGxDevicePtr->BufUnlock(indexBuf, 0);
            indexBuf->unk_001C = 1;
            g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_P);
            g_theGxDevicePtr->PrimIndexPtr(indexBuf);

            CGxBatch batch;
            batch.m_count = writtenIndices;
            batch.m_minIndex = 0;
            batch.m_maxIndex = static_cast<uint16_t>(baseVertex - 1);
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            g_theGxDevicePtr->Draw(batch);
        }

        start = end;
    } while (start < count);

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00719CC0/0x1403EDEE0
void Liquid::PropagateDisplMap(uint32_t index) {
    uint32_t previous = s_displMapCurrent[index];
    uint32_t* current = &s_displMapCurrent[index];
    *current = ((previous - 1) & 1) + 2 * index;

    float dx = s_displMapPos[index].x - s_displMapRawPos[index].x;
    float dy = s_displMapPos[index].y - s_displMapRawPos[index].y;

    if (dy * dy + dx * dx >= 10000.0f) {
        dx = 0.0f;
        dy = 0.0f;
    }

    float texSize = static_cast<float>(s_displMapTexSize[index]);
    s_displMapRawPos[index] = s_displMapPos[index];

    float offset[4];
    offset[0] = dx / s_displMapSize[index].x;
    offset[1] = dy / s_displMapSize[index].y;
    offset[2] = 0.0f;
    offset[3] = 0.0f;

    float inv = 1.0f / texSize;
    float texel[4];
    texel[0] = inv;
    texel[1] = inv;
    texel[2] = inv * 0.5f;
    texel[3] = inv * 0.5f;

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(C3Vector), 4);
    C3Vector* vertices = reinterpret_cast<C3Vector*>(g_theGxDevicePtr->BufLock(vertexBuf));
    vertices[0] = C3Vector(-1.0f, 1.0f, 0.0f);
    vertices[1] = C3Vector(1.0f, 1.0f, 0.0f);
    vertices[2] = C3Vector(-1.0f, -1.0f, 0.0f);
    vertices[3] = C3Vector(1.0f, -1.0f, 0.0f);
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6);
    GxBufData(indexBuf, s_quadTriIndices, sizeof(s_quadTriIndices), 0);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_P);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &Liquid::s_propagateDisplMapVS->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &Liquid::s_propagateDisplMapPS->m_permutes[0]);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, texel, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, offset, 1);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(Liquid::s_displMaps[previous]));

    CGxFrameBuffer* frameBuffer = Liquid::s_displMapFBs[*current];
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);

    CGxBatch batch;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 3;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = 6;
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0071A0A0/0x1403EE340
void Liquid::UpdateDisplMap(uint32_t time) {
    if (!Liquid::NeedDynDisplMap()) {
        Liquid::ClearEventPerturbations();
        return;
    }

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, minZ, maxZ);

    CGxFrameBuffer* frameBuffer = g_theGxDevicePtr->FrameBufferGet();

    if (Liquid::s_displMapDirty) {
        Liquid::ClearDisplMap();
        Liquid::s_displMapDirty = 0;

        for (uint32_t i = 0; i < Liquid::s_displMapCount; i++) {
            s_displMapLastTime[i] = time - s_displMapPeriod[i];
        }
    }

    for (uint32_t i = 0; i < Liquid::s_displMapCount; i++) {
        Liquid::PerturbDisplMap(Liquid::s_eventPerturbations[i], i, time);
        Liquid::PerturbDisplMapMesh(Liquid::s_eventPerturbationMeshes[i], i, time);
    }

    Liquid::ClearEventPerturbations();

    for (uint32_t i = 0; i < Liquid::s_displMapCount; i++) {
        uint32_t period = s_displMapPeriod[i];
        uint32_t elapsed = time - s_displMapLastTime[i];
        uint32_t steps = elapsed / period;
        s_displMapInterpolation[i] = static_cast<float>(elapsed % period) / static_cast<float>(period);

        if (elapsed < period) {
            continue;
        }

        uint32_t count;

        if (steps > 2) {
            count = 2;
            s_displMapLastTime[i] = time;
        } else {
            count = steps;

            if (steps < 2) {
                s_displMapLastTime[i] += steps * period;
            } else {
                s_displMapLastTime[i] = time;
            }
        }

        while (count) {
            count--;
            Liquid::PerturbDisplMap(Liquid::s_perturbations[i], i, time);
            Liquid::PerturbDisplMapMesh(Liquid::s_perturbationMeshes[i], i, time);
            Liquid::PropagateDisplMap(i);
        }
    }

    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x0071A310/0x1403EE690
void Liquid::DestroyProceduralTextures(int32_t materials) {
    g_theGxDevicePtr->RemoveTextureRecreationCallback(Liquid::OnTexturesDirty);
    g_theGxDevicePtr->RemoveDeviceRestoredCallback(Liquid::OnTexturesDirty);
    Liquid::UnCacheProceduralTextures();

    if (s_realReflectionBuffer) {
        TextureRelease(s_realReflectionBuffer);
        s_realReflectionBuffer = NULL;
    }

    if (s_fakeReflectionBuffer) {
        TextureRelease(s_fakeReflectionBuffer);
        s_fakeReflectionBuffer = NULL;
    }

    if (s_depthReflectionBuffer) {
        TextureRelease(s_depthReflectionBuffer);
        s_depthReflectionBuffer = NULL;
    }

    if (s_backBuffer) {
        TextureRelease(s_backBuffer);
        s_backBuffer = NULL;
    }

    if (s_blurTemp) {
        TextureRelease(s_blurTemp);
        s_blurTemp = NULL;
    }

    if (s_blurredBackBuffer) {
        TextureRelease(s_blurredBackBuffer);
        s_blurredBackBuffer = NULL;
    }

    if (s_blurredReflectionBuffer) {
        TextureRelease(s_blurredReflectionBuffer);
        s_blurredReflectionBuffer = NULL;
    }

    if (s_realReflectionFB) {
        g_theGxDevicePtr->FrameBufferDelete(s_realReflectionFB);
    }

    if (s_fakeReflectionFB) {
        g_theGxDevicePtr->FrameBufferDelete(s_fakeReflectionFB);
    }

    if (s_blurTempFB) {
        g_theGxDevicePtr->FrameBufferDelete(s_blurTempFB);
    }

    if (s_blurredBackBufferFB) {
        g_theGxDevicePtr->FrameBufferDelete(s_blurredBackBufferFB);
    }

    if (s_blurredReflectionFB) {
        g_theGxDevicePtr->FrameBufferDelete(s_blurredReflectionFB);
    }

    if (Liquid::s_fourTapBlurVS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_fourTapBlurVS);
        Liquid::s_fourTapBlurVS = NULL;
    }

    if (Liquid::s_fourTapBlurPS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_fourTapBlurPS);
        Liquid::s_fourTapBlurPS = NULL;
    }

    if (Liquid::s_clearDisplMapVS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_clearDisplMapVS);
        Liquid::s_clearDisplMapVS = NULL;
    }

    if (Liquid::s_clearDisplMapPS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_clearDisplMapPS);
        Liquid::s_clearDisplMapPS = NULL;
    }

    if (Liquid::s_perturbDisplMapVS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_perturbDisplMapVS);
        Liquid::s_perturbDisplMapVS = NULL;
    }

    if (Liquid::s_perturbDisplMapPS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_perturbDisplMapPS);
        Liquid::s_perturbDisplMapPS = NULL;
    }

    if (Liquid::s_perturbDisplMapMeshVS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_perturbDisplMapMeshVS);
        Liquid::s_perturbDisplMapMeshVS = NULL;
    }

    if (Liquid::s_perturbDisplMapMeshPS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_perturbDisplMapMeshPS);
        Liquid::s_perturbDisplMapMeshPS = NULL;
    }

    if (Liquid::s_propagateDisplMapVS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_propagateDisplMapVS);
        Liquid::s_propagateDisplMapVS = NULL;
    }

    if (Liquid::s_propagateDisplMapPS) {
        g_theGxDevicePtr->ShaderRelease(Liquid::s_propagateDisplMapPS);
        Liquid::s_propagateDisplMapPS = NULL;
    }

    if (Liquid::s_perturbTex) {
        TextureRelease(Liquid::s_perturbTex);
        Liquid::s_perturbTex = NULL;
    }

    if (materials) {
        if (s_riverDepthTex) {
            TextureRelease(s_riverDepthTex);
            s_riverDepthTex = NULL;
        }

        if (s_oceanDepthTex) {
            TextureRelease(s_oceanDepthTex);
            s_oceanDepthTex = NULL;
        }

        if (s_wmoWaterTex) {
            TextureRelease(s_wmoWaterTex);
            s_wmoWaterTex = NULL;
        }
    }

    if (s_noiseVolTex) {
        TextureRelease(s_noiseVolTex);
        s_noiseVolTex = NULL;
    }

    for (uint32_t i = 0; i < 2 * Liquid::s_displMapCount; i++) {
        if (Liquid::s_displMaps[i]) {
            TextureRelease(Liquid::s_displMaps[i]);
            Liquid::s_displMaps[i] = NULL;
        }

        if (Liquid::s_displMapFBs[i]) {
            g_theGxDevicePtr->FrameBufferDelete(Liquid::s_displMapFBs[i]);
        }
    }

    Liquid::s_displMapCount = 0;
    Liquid::ClearEventPerturbations();

    if (s_texBuffer.m_data) {
        SMemFree(s_texBuffer.m_data, typeid(uint8_t).raw_name(), SLOG_OBJECT, 0);
    }

    s_texBuffer.m_alloc = 0;
    s_texBuffer.m_count = 0;
    s_texBuffer.m_data = NULL;
}

// OFFSET: 0x0071A6C0/0x1403EEAD0
void Liquid::CreateDynamicDisplacementMap() {
    Liquid::s_displMapCount = 1;
    s_displMapTexSize[0] = 1024;
    s_displMapPeriod[0] = 50;

    if (Liquid::GetRippleDetail() > 1) {
        Liquid::s_displMapCount = 2;
        s_displMapTexSize[0] = 1024;
        s_displMapTexSize[1] = 1024;
        s_displMapPeriod[0] = 25;
        s_displMapPeriod[1] = 50;
    }

    CRect rect(0.0f, 0.0f, 0.0f, 0.0f);
    g_theGxDevicePtr->CapsWindowSize(rect);

    CGxTexFlags flags;
    flags.m_renderTarget = 1;

    for (uint32_t i = 0; i < 2 * Liquid::s_displMapCount; i++) {
        char name[256];
        SStrPrintf(name, sizeof(name), "proceduralDisplMap%d", i);
        Liquid::s_displMaps[i] = TextureCreate(s_displMapTexSize[i >> 1], s_displMapTexSize[i >> 1], GxTex_Gr1616F, GxTex_Gr1616F, flags, NULL, GxTexNullUserFunc, name, 0);
        Liquid::s_displMapFBs[i] = GxFrameBufferCreate(NULL, TextureGetGxTex(Liquid::s_displMaps[i]));
    }

    Liquid::s_propagateDisplMapVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PropagateDisplMap");
    Liquid::s_propagateDisplMapPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "PropagateDisplMap");
    Liquid::s_perturbDisplMapVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PerturbDisplMap");
    Liquid::s_perturbDisplMapPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "PerturbDisplMap");
    Liquid::s_perturbDisplMapMeshVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "PerturbDisplMapMesh");
    Liquid::s_perturbDisplMapMeshPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "PerturbDisplMapMesh");
    Liquid::s_clearDisplMapVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ClearDisplMap");
    Liquid::s_clearDisplMapPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ClearDisplMap");

    CStatus status;
    CGxTexFlags texFlags;
    texFlags.m_filter = GxTex_LinearMipNearest;
    Liquid::s_perturbTex = TextureCreate("XTextures\\Ripples\\perturbTex", texFlags, &status, 0, NULL, 2);
    Liquid::s_displMapDirty = 1;
}

// OFFSET: 0x0071A8F0/0x1403EEDF0
void Liquid::CreateProceduralTextures(int32_t materials) {
    int32_t backBuffer = Liquid::GetWaterDetail() != 0;
    int32_t blur = Liquid::GetWaterDetail() != 0;
    int32_t noise = Liquid::GetWaterDetail() != 0;
    int32_t dynDisplMap = Liquid::GetWaterDetail() > 1 && Liquid::GetRippleDetail();

    Liquid::s_oceanDepthTexValid = 0;
    Liquid::s_riverDepthTexValid = 0;
    Liquid::s_wmoWaterTexValid = 0;

    if (materials) {
        CGxTexFlags flags;
        flags.m_bit20 = 1;
        s_riverDepthTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, flags, reinterpret_cast<void*>(1), ExtWaterDepthTexCallback, "proceduralRiverDepthTex", 0);

        CGxTexFlags oceanFlags;
        oceanFlags.m_bit20 = 1;
        s_oceanDepthTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, oceanFlags, NULL, ExtWaterDepthTexCallback, "proceduralOceanDepthTex", 0);

        CGxTexFlags wmoFlags;
        wmoFlags.m_bit20 = 1;
        s_wmoWaterTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, wmoFlags, NULL, WmoWaterTexCallback, "proceduralWmoWaterTex", 0);
    }

    if (noise) {
        s_noiseVolTex = CreateNoiseVolTex();
    }

    if (backBuffer) {
        s_backBuffer = CreateBackBuffer();
    }

    if (blur) {
        Liquid::s_fourTapBlurVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "FourTapBlur");
        Liquid::s_fourTapBlurPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FourTapBlur");

        CGxTexFlags tempFlags;
        tempFlags.m_renderTarget = 1;
        s_blurTemp = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, tempFlags, NULL, GxTexNullUserFunc, "blurTemp", 0);
        s_blurTempFB = GxFrameBufferCreate(NULL, TextureGetGxTex(s_blurTemp));

        CGxTexFlags backFlags;
        backFlags.m_renderTarget = 1;
        s_blurredBackBuffer = TextureCreate(512, 512, GxTex_Argb8888, GxTex_Argb8888, backFlags, NULL, GxTexNullUserFunc, "blurredBack", 0);
        s_blurredBackBufferFB = GxFrameBufferCreate(NULL, TextureGetGxTex(s_blurredBackBuffer));

        if (g_theGxDevicePtr->DeviceApi() == 3 || g_theGxDevicePtr->DeviceFormat().m_multisampleCount > 1) {
            s_depthReflectionBuffer = CreateDepthReflectionBuffer("LiquidProc Reflection Depth");
        }

        CGxTex* depth = s_depthReflectionBuffer ? TextureGetGxTex(s_depthReflectionBuffer) : g_theGxDevicePtr->BackBufferDepthTex();

        if (Liquid::GetWaterDetail() >= 3) {
            s_realReflectionBuffer = CreateReflectionBuffer("LiquidProc Real Reflection");
            s_realReflectionFB = GxFrameBufferCreate(depth, TextureGetGxTex(s_realReflectionBuffer));
            s_blurredReflectionBuffer = CreateBlurBuffer("blurredReflection");
            s_blurredReflectionFB = GxFrameBufferCreate(NULL, TextureGetGxTex(s_blurredReflectionBuffer));
        }

        s_fakeReflectionBuffer = CreateReflectionBuffer("LiquidProc Fake Reflection");
        s_fakeReflectionFB = GxFrameBufferCreate(depth, TextureGetGxTex(s_fakeReflectionBuffer));
    }

    if (dynDisplMap) {
        Liquid::CreateDynamicDisplacementMap();
        s_displMapCurrent[0] = 0;
        s_displMapCurrent[1] = 0;

        if (!s_perturbationsReserved) {
            if (Liquid::s_eventPerturbations[0].Count() + 96 > Liquid::s_eventPerturbations[0].m_alloc) {
                Liquid::s_eventPerturbations[0].Reserve(Liquid::s_eventPerturbations[0].Count() + 96, 0);
            }

            if (Liquid::s_perturbations[0].Count() + 64 > Liquid::s_perturbations[0].m_alloc) {
                Liquid::s_perturbations[0].Reserve(Liquid::s_perturbations[0].Count() + 64, 0);
            }

            if (Liquid::s_eventPerturbationMeshes[0].Count() > Liquid::s_eventPerturbationMeshes[0].m_alloc) {
                Liquid::s_eventPerturbationMeshes[0].Reserve(Liquid::s_eventPerturbationMeshes[0].Count(), 0);
            }

            if (Liquid::s_perturbationMeshes[0].Count() > Liquid::s_perturbationMeshes[0].m_alloc) {
                Liquid::s_perturbationMeshes[0].Reserve(Liquid::s_perturbationMeshes[0].Count(), 0);
            }

            if (Liquid::s_eventPerturbations[1].Count() > Liquid::s_eventPerturbations[1].m_alloc) {
                Liquid::s_eventPerturbations[1].Reserve(Liquid::s_eventPerturbations[1].Count(), 0);
            }

            if (Liquid::s_perturbations[1].Count() > Liquid::s_perturbations[1].m_alloc) {
                Liquid::s_perturbations[1].Reserve(Liquid::s_perturbations[1].Count(), 0);
            }

            if (Liquid::s_eventPerturbationMeshes[1].Count() > Liquid::s_eventPerturbationMeshes[1].m_alloc) {
                Liquid::s_eventPerturbationMeshes[1].Reserve(Liquid::s_eventPerturbationMeshes[1].Count(), 0);
            }

            if (Liquid::s_perturbationMeshes[1].Count() + 16 > Liquid::s_perturbationMeshes[1].m_alloc) {
                Liquid::s_perturbationMeshes[1].Reserve(Liquid::s_perturbationMeshes[1].Count() + 16, 0);
            }

            s_perturbationsReserved = 1;
        }
    }

    Liquid::CacheProceduralTextures();
    g_theGxDevicePtr->AddTextureRecreationCallback(Liquid::OnTexturesDirty);
    g_theGxDevicePtr->AddDeviceRestoredCallback(Liquid::OnTexturesDirty);
}
