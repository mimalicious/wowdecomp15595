#include <cfloat>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/TimeManager.h"

// STATIC: %0x00AFB958
const uint32_t CGxDevice::s_texFormatBitDepth[GxTexFormats_Last] = { 0, 32, 32, 16, 16, 16, 16, 4, 8, 8, 16, 32, 64, 32, 32, 128, 32, 32, 16, 32, 32, 32, 32, 32 };

// STATIC: %0x00AFB9B8
const uint32_t CGxDevice::s_texFormatBlockSize[GxTexFormats_Last] = { 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// STATIC: 0x00BD40B8
const uint32_t CGxDevice::s_texFormatIsDepth[GxTexFormats_Last] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1 };

// STATIC: %0x00AFBAD8
const float CGxDevice::s_polygonOffsetValues[3] = { 0.0f, 3.05180438e-05f, 3.81469727e-06f };

// STATIC: %0x00AFBA18
const uint32_t CGxDevice::s_texFormatBytesPerBlock[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 8, 16, 16, 2, 4, 8, 4, 4, 16, 4, 4, 2, 4, 4, 4, 4, 4 };

// STATIC: %0x00C0E2C8
static uint32_t s_primVertsPerPrim[GxPrims_Last] = { 1, 2, 1, 3, 1, 1 };

// STATIC: %0x00C0E2DC
static uint32_t s_primVertsExtra[GxPrims_Last] = { 0, 0, 1, 0, 2, 2 };

// STATIC: #0x00AD87C4
const char* s_formatToInt[CGxFormat::Formats_Last] = { "16", "24", "24", "30", "16", "24", "24", "32" };

// STATIC: %0x00C0D87C
const char* CGxDevice::s_shaderTargetNames[GxShTargets_Last] = { "Vertex", "Hull", "Domain", "Geometry", "Pixel", "Compute" };

// STATIC: %0x00C0D894
const char* CGxDevice::s_shaderModelNames[8] = { "ff", "nvrc", "ff_ps", "1_1", "2_0", "3_0", "4_0", "5_0" };

// STATIC: %0x00C0D8C0
const EGxShTarget CGxDevice::s_shaderProfileTarget[GxShProfiles_Last] = { GxShTargets_Last, GxSh_Vertex, GxSh_Pixel, GxSh_Vertex, GxSh_Vertex, GxSh_Vertex, GxSh_Vertex, GxSh_Vertex, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Geometry, GxSh_Geometry, GxSh_Hull, GxSh_Domain, GxSh_Compute, GxSh_Compute, GxSh_Vertex, GxSh_Vertex, GxSh_Pixel, GxSh_Vertex, GxSh_Pixel, GxSh_Compute, GxSh_Vertex, GxSh_Vertex, GxSh_Vertex, GxSh_Vertex, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Pixel, GxSh_Geometry, GxSh_Compute };

// STATIC: %0x00C0D958
const char* CGxDevice::s_shaderProfileNames[GxShProfiles_Last] = { "none", "vs_ff", "ps_ff", "vs_1_1", "vs_2_0", "vs_3_0", "vs_4_0", "vs_5_0", "ps_1_1", "ps_1_4", "ps_2_0", "ps_3_0", "ps_4_0", "ps_5_0", "gs_4_0", "gs_5_0", "hs_5_0", "ds_5_0", "cs_4_0", "cs_5_0", "arbvp1", "arbvp1_cg12", "arbfp1", "glslv", "glslf", "opencl", "vp20", "vp30", "vp40", "gp4vp", "fp10", "fp20", "fp25", "fp30", "fp40", "gp4fp", "gp4gp", "cuda" };

// STATIC: %0x00C0DB40
const char* CGxDevice::s_texFormatNames[GxTexFormats_Last] = { "Unknown", "Abgr8888", "Argb8888", "Argb4444", "Argb1555", "Rgb565", "Uv88", "Dxt1", "Dxt3", "Dxt5", "R16_UNORM", "Gr1616F", "Abgr16161616F", "R24_UNORM_X8", "R32F", "Abgr32323232F", "NULL", "D24X8", "D16", "D32", "INTZ", "RAWZ", "RESZ", "ATI_D24" };

// STATIC: %0x00C0E2F0
uint32_t CGxDevice::s_alphaRef[12] = { 0, 128, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };

// STATIC: %0x00C0E324
int32_t s_streamVertexPoolSize = 0x2C0000;

// STATIC: %0x00C0E328
int32_t s_streamIndexPoolSize = 0x40000;

// STATIC: #0x00AD8BB4
C3Vector CGxDevice::s_pointScaleAttenuation(1.0f, 0.0f, 0.0f);

// STATIC: 0x00D09028/0x140BBAD30
static const char* s_formatNames[CGxFormat::Formats_Last] = { "Rgb565", "ArgbX888", "Argb8888", "Argb2101010", "Ds160", "Ds24X", "Ds248", "Ds320" };

// STATIC: %0x00C0E360
static C2iVector kWinMinSize = { 320, 240 };

// STATIC: #0x00C5DF88
CGxDevice* g_theGxDevicePtr;

// STATIC: %0x00D8BCEC
uint32_t CGxDevice::m_logBytes;

// STATIC: %0x00D91D20
HSLOG CGxDevice::m_log;

// STATIC: %0x00D92E88
C2iVector CGxDevice::s_clientAdjust;

// STATIC: 0x00EC6678
static uint32_t s_nextFrameTime;

// STATIC: %0x00C0E4CC
const char* CGxDevice::s_shaderPaths[GxShTargets_Last] = { "Shaders\\Vertex", "Shaders\\Hull", "Shaders\\Domain", "Shaders\\Geometry", "Shaders\\Pixel", "Shaders\\Compute" };

// STATIC: %0x00C0E32C
static float kWinAspect;

// STATIC: %0x00D92D30
static int32_t kWinAspectEnabled;

// STATIC: %0x00D8BCE8
CGxShader* CGxDevice::s_uiVertexShader;

// STATIC: %0x00D92D34
CGxShader* CGxDevice::s_uiPixelShader;

// STATIC: 0x00EBF4A0
ShadowConstants CGxDevice::s_shadowConstants[GxShTargets_Last];

// STATIC: %0x00D92D80
TSHashTable<CGxShader, HASHKEY_STRI> CGxDevice::s_shaderList[GxShTargets_Last];

// STATIC: %0x00D92D3C
TSGrowableArray<GxDeviceCallback> CGxDevice::s_deviceRestoredCallbacks;

// STATIC: %0x00D92D4C
TSGrowableArray<GxDeviceCallback> CGxDevice::s_textureRecreationCallbacks;

// STATIC: %0x00D92D5C
TSGrowableArray<GxDisplayChangeCallback> CGxDevice::s_displayChangeCallbacks;

// STATIC: %0x00D92D6C
TSGrowableArray<CGxDevice::ShaderModelCall> CGxDevice::s_shaderModelChangeCallbacks;

// STATIC: %0x00C0E338
TSList<CGxPool, TSGetLink<CGxPool> > CGxDevice::s_poolList;

// STATIC: %0x00C0E33C
TSList<CGxBuf, TSGetLink<CGxBuf> > CGxDevice::s_bufList;

// STATIC: %0x00C0E348
TSExplicitList<CGxTex, offsetof(CGxTex, m_link)> CGxDevice::s_textureList;

// STATIC: %0x00D92D2C
int32_t CGxDevice::s_cursorVisible;

// STATIC: %0x00D92D38
int32_t CGxDevice::s_monitorsInit;

// STATIC: %0x00D92E90
TSGrowableArray<void*> CGxDevice::s_monitors;

// STATIC: 0x00EC6500/0x140DB7578
uint32_t CGxDevice::s_cursorHotspotX;

// STATIC: 0x00EBF49C/0x140DB150C
uint32_t CGxDevice::s_cursorHotspotY;

// STATIC: %0x00D91D28
uint32_t CGxDevice::s_cursor[1024];

// STATIC: %0x00C0D8B4
static const int32_t s_shaderProfileSetC0D8B4[] = { GxShProfile_VsFf, GxShProfile_PsFf, 0 };

// STATIC: %0x00C0D9F0
static const int32_t s_shaderProfileSetC0D9F0[] = { GxShProfile_VsFf, GxShProfile_Fp10, 0 };

// STATIC: %0x00C0D9FC
static const int32_t s_shaderProfileSetC0D9FC[] = { GxShProfile_VsFf, GxShProfile_Ps20, 0 };

// STATIC: %0x00C0DA08
static const int32_t s_shaderProfileSetC0DA08[] = { GxShProfile_VsFf, GxShProfile_Arbfp1, 0 };

// STATIC: %0x00C0DA14
static const int32_t s_shaderProfileSetC0DA14[] = { GxShProfile_Vs11, GxShProfile_Ps11, 0 };

// STATIC: %0x00C0DA20
static const int32_t s_shaderProfileSetC0DA20[] = { GxShProfile_Vp20, GxShProfile_Fp20, 0 };

// STATIC: %0x00C0DA2C
static const int32_t s_shaderProfileSetC0DA2C[] = { GxShProfile_Arbvp1, GxShProfile_Fp20, 0 };

// STATIC: %0x00C0DA38
static const int32_t s_shaderProfileSetC0DA38[] = { GxShProfile_Vs20, GxShProfile_Ps20, 0 };

// STATIC: %0x00C0DA44
static const int32_t s_shaderProfileSetC0DA44[] = { GxShProfile_Arbvp1, GxShProfile_Arbfp1, 0 };

// STATIC: %0x00C0DA50
static const int32_t s_shaderProfileSetC0DA50[] = { GxShProfile_Arbvp1Cg12, GxShProfile_Arbfp1, 0 };

// STATIC: %0x00C0DA5C
static const int32_t s_shaderProfileSetC0DA5C[] = { GxShProfile_Vp30, GxShProfile_Fp30, 0 };

// STATIC: %0x00C0DA68
static const int32_t s_shaderProfileSetC0DA68[] = { GxShProfile_Vs30, GxShProfile_Ps30, 0 };

// STATIC: %0x00C0DA74
static const int32_t s_shaderProfileSetC0DA74[] = { GxShProfile_Vp40, GxShProfile_Fp40, 0 };

// STATIC: %0x00C0DA80
static const int32_t s_shaderProfileSetC0DA80[] = { GxShProfile_Vs40, GxShProfile_Gs40, GxShProfile_Ps40, GxShProfile_Cs40, 0 };

// STATIC: %0x00C0DA94
static const int32_t s_shaderProfileSetC0DA94[] = { GxShProfile_Gp4vp, GxShProfile_Gp4gp, GxShProfile_Gp4fp, 0 };

// STATIC: %0x00C0DAA4
static const int32_t s_shaderProfileSetC0DAA4[] = { GxShProfile_Vs50, GxShProfile_Hs50, GxShProfile_Ds50, GxShProfile_Gs50, GxShProfile_Ps50, GxShProfile_Cs50, 0 };

// STATIC: %0x00C0DAC0
static const int32_t* const s_shaderModel0Sets[] = { s_shaderProfileSetC0D8B4, NULL };

// STATIC: %0x00C0DAC8
static const int32_t* const s_shaderModel1Sets[] = { s_shaderProfileSetC0D9F0, NULL };

// STATIC: %0x00C0DAD0
static const int32_t* const s_shaderModel2Sets[] = { s_shaderProfileSetC0D9FC, s_shaderProfileSetC0DA08, NULL };

// STATIC: %0x00C0DADC
static const int32_t* const s_shaderModel3Sets[] = { s_shaderProfileSetC0DA14, s_shaderProfileSetC0DA20, s_shaderProfileSetC0DA2C, NULL };

// STATIC: %0x00C0DAEC
static const int32_t* const s_shaderModel4Sets[] = { s_shaderProfileSetC0DA38, s_shaderProfileSetC0DA44, s_shaderProfileSetC0DA50, s_shaderProfileSetC0DA5C, NULL };

// STATIC: %0x00C0DB00
static const int32_t* const s_shaderModel5Sets[] = { s_shaderProfileSetC0DA68, s_shaderProfileSetC0DA74, NULL };

// STATIC: %0x00C0DB0C
static const int32_t* const s_shaderModel6Sets[] = { s_shaderProfileSetC0DA80, s_shaderProfileSetC0DA94, NULL };

// STATIC: %0x00C0DB18
static const int32_t* const s_shaderModel7Sets[] = { s_shaderProfileSetC0DAA4, NULL };

// STATIC: %0x00C0DB20
static const int32_t* const* const s_shaderModelProfiles[8] = { s_shaderModel0Sets, s_shaderModel1Sets, s_shaderModel2Sets, s_shaderModel3Sets, s_shaderModel4Sets, s_shaderModel5Sets, s_shaderModel6Sets, s_shaderModel7Sets };

void CGxDevice::DeviceEvictResources() {
}

void CGxDevice::DeviceResolveDepthBuffer(CGxTex* texture) {
}

void CGxDevice::DeviceCopyTex(CGxTex* source, uint32_t sourcePlane, CGxTex* dest, uint32_t destPlane, uint32_t level) {
}

// OFFSET: 0x00841930
CGxFrameBuffer* CGxDevice::FrameBufferCreate() {
    return new (SMemAlloc(sizeof(CGxFrameBuffer), __FILE__, __LINE__, 0)) CGxFrameBuffer();
}

// PROVEN: a989595e IDENTICAL/NONE
// OFFSET: 0x00841960
void CGxDevice::FrameBufferDelete(CGxFrameBuffer*& frameBuffer) {
    if (frameBuffer) {
        SMemFree(frameBuffer, "delete", -1, 0);
    }

    frameBuffer = NULL;
}

// PROVEN: 004f5c49 IDENTICAL/NONE
// OFFSET: 0x00841990
int32_t CGxDevice::FrameBufferAttach(CGxFrameBuffer* frameBuffer, EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    return frameBuffer->Attach(buffer, texture, plane);
}

// PROVEN: a8e0bac9 IDENTICAL/NONE
// OFFSET: 0x008419B0
int32_t CGxDevice::FrameBufferFinalize(CGxFrameBuffer* frameBuffer) {
    CGxTex* depth = frameBuffer->m_targets[GxBuffers_Depth].m_texture;
    CGxTex* color = frameBuffer->m_targets[GxBuffers_Color].m_texture;

    if ((!depth || !color || (!depth->m_width && !depth->m_height) || (depth->m_width >= color->m_width && depth->m_height >= color->m_height)) && frameBuffer->Finalize()) {
        return 1;
    }

    return 0;
}

// PROVEN: 70b6b2d3 IDENTICAL/NONE
// OFFSET: 0x00841A00
CGxFrameBuffer* CGxDevice::FrameBufferGet() {
    return this->m_frameBuffer;
}

// PROVEN: 4d27eae8 IDENTICAL/NONE
// OFFSET: 0x00841900
CGxTex* CGxDevice::BackBufferColorTex() {
    return this->m_backBufferColorTex;
}

// PROVEN: 8e58a0a4 IDENTICAL/NONE
// OFFSET: 0x00841910
CGxTex* CGxDevice::BackBufferDepthTex() {
    return this->m_backBufferDepthTex;
}

// PROVEN: afd1fd5e IDENTICAL/NONE
// OFFSET: 0x00841920
CGxFrameBuffer* CGxDevice::BackBuffer() {
    return this->m_backBuffer;
}

void CGxDevice::LogCrashInfo(char* buffer, uint32_t size) {
}

void CGxDevice::SceneClear(uint32_t mask, CImVector color) {
}

void CGxDevice::SceneClearToViewport(uint32_t mask, CImVector color) {
}

void CGxDevice::SetInstanceCount(uint32_t count) {
}

void CGxDevice::ResetInstancing() {
}

void CGxDevice::BufData(CGxBuf* buf, const void* data, uint32_t size, uint32_t offset) {
}

int32_t CGxDevice::TexResolve(CGxTex* dest, CGxTex* source) {
    return 0;
}

void CGxDevice::TexSetCacheSize(int32_t size) {
}

int32_t CGxDevice::ISetCursor() {
    return 0;
}

// PROVEN: 36fb51ed IDENTICAL/NONE
// OFFSET: 0x00841850/0x14056DBD0
int32_t CGxDevice::DeviceCreate(GxWindowProc windowProc, const CGxFormat& format) {
    this->m_windowProc = windowProc;
    return this->DeviceSetFormat(format);
}

// PROVEN: ced9af73 IDENTICAL/UNPROVEN
// OFFSET: 0x00841870/0x14056DBF0
int32_t CGxDevice::DeviceCreate(void* window, const CGxFormat& format) {
    this->m_format = format;
    return 1;
}

// PROVEN: d5c3217b IDENTICAL/IDENTICAL
// OFFSET: 0x00841890/0x14056DC10
void CGxDevice::DeviceDestroy() {
    this->m_windowProc = NULL;
}

// PROVEN: e6e6e01b IDENTICAL/IDENTICAL
// OFFSET: 0x008418A0/0x14056DC20
void CGxDevice::DeviceSetBaseMipLevel(uint32_t level) {
    this->m_baseMipLevel = level;
}

// PROVEN: adcee6d1 IDENTICAL/IDENTICAL
// OFFSET: 0x008418E0/0x14056DC60
void CGxDevice::DeviceTakeScreenShot() {
    this->m_scrShotClick = 1;
}

// OFFSET: 0x00843630
void CGxDevice::ScenePresent(void* window) {
    this->m_frameCount++;
    this->m_scrShotClick = 0;
    this->m_intF5C = 0;

    for (uint32_t i = GxRs_VertexTexture0; i <= GxRs_VertexTexture3; i++) {
        if (this->m_hwRenderStates[i].m_data.p) {
            this->m_hwRenderStates[i].m_data.p = reinterpret_cast<void*>(-1);
        }
    }

    for (uint32_t i = GxRs_Texture0; i <= GxRs_Texture15; i += 4) {
        if (this->m_hwRenderStates[i].m_data.p) {
            this->m_hwRenderStates[i].m_data.p = reinterpret_cast<void*>(-1);
        }

        if (this->m_hwRenderStates[i + 1].m_data.p) {
            this->m_hwRenderStates[i + 1].m_data.p = reinterpret_cast<void*>(-1);
        }

        if (this->m_hwRenderStates[i + 2].m_data.p) {
            this->m_hwRenderStates[i + 2].m_data.p = reinterpret_cast<void*>(-1);
        }

        if (this->m_hwRenderStates[i + 3].m_data.p) {
            this->m_hwRenderStates[i + 3].m_data.p = reinterpret_cast<void*>(-1);
        }
    }
}

// OFFSET: 0x00841BD0
uint32_t CGxDevice::IVertexAttribToShAttrib(EGxVertexAttrib attrib) {
    switch (attrib) {
        case 1:
            return 1;

        case 2:
            return 2;

        case 3:
            return 3;

        case 4:
            return 9;

        case 5:
            return 10;

        case 6:
            return 11;

        case 7:
            return 12;

        case 8:
            return 13;

        case 9:
            return 14;

        case 10:
            return 15;

        case 11:
            return 16;

        case 12:
            return 17;

        case 13:
            return 18;

        default:
            return 0;
    }
}

// PROVEN: 2ba31541 IDENTICAL/IDENTICAL
// OFFSET: 0x00841E30/0x14056E180
char* CGxDevice::BufLock(CGxBuf* buf) {
    this->m_bufLocked[buf->m_pool->m_target] = buf;
    return NULL;
}

// PROVEN: adf5d0f8 IDENTICAL/IDENTICAL
// OFFSET: 0x00841E50/0x14056E1A0
int32_t CGxDevice::BufUnlock(CGxBuf* buf, uint32_t size) {
    this->m_bufLocked[buf->m_pool->m_target] = NULL;
    return 1;
}

// PROVEN: dfcff1e2 IDENTICAL/IDENTICAL
// OFFSET: 0x00842230/0x14056E520
void CGxDevice::CursorSetVisible(int32_t visible) {
    CGxDevice::s_cursorVisible = visible;
}

// PROVEN: 9027fb46 IDENTICAL/NONE
// OFFSET: 0x00842240
uint32_t* CGxDevice::CursorLock() {
    return CGxDevice::s_cursor;
}

// OFFSET: 0x00844FD0/0x140571E70
int32_t CGxDevice::DeviceSetFormat(const CGxFormat& format) {
    this->IRsForceUpdate();
    this->m_format = format;

    if (this->m_format.m_size.x < kWinMinSize.x && this->m_format.m_size.y < kWinMinSize.y) {
        this->m_format.m_size = kWinMinSize;
    }

    kWinAspect = static_cast<float>(this->m_format.m_size.x) / static_cast<float>(this->m_format.m_size.y);
    kWinAspectEnabled = this->m_format.m_aspect;

    return 1;
}

// PROVEN: d79daa47 IDENTICAL/IDENTICAL
// OFFSET: 0x00843220/0x14056F710
void CGxDevice::DeviceSetGamma(float gamma) {
    this->m_gammaRamp.Set(gamma);
}

// OFFSET: 0x00843240/0x14056F720
void CGxDevice::DeviceSetGamma(const CGxGammaRamp& ramp) {
    this->m_gammaRamp = ramp;
}

// OFFSET: 0x00843300
void CGxDevice::DeviceReadScreenShot(uint32_t& width, uint32_t& height, const CImVector*& pixels) {
    width = this->m_scrShotWidth;
    height = this->m_scrShotHeight;
    pixels = this->m_scrShotPixels.Count() ? this->m_scrShotPixels.Ptr() : NULL;
}

// OFFSET: 0x00843340/0x14056F7F0
void CGxDevice::DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, uint32_t plane) {
    CGxDevice::TextureTarget& target = this->m_textureTarget[buffer];
    target.m_plane = plane;
    target.m_texture = texture;

    if (texture && (this->m_textureTarget[GxBuffers_Color].m_texture || this->m_textureTarget[GxBuffers_Depth].m_texture)) {
        this->m_curWindowRect.minY = 0.0f;
        this->m_curWindowRect.minX = 0.0f;
        this->m_curWindowRect.maxY = static_cast<float>(texture->m_height);
        this->m_curWindowRect.maxX = static_cast<float>(texture->m_width);
    } else {
        this->m_curWindowRect = this->m_defWindowRect;
    }
}

// PROVEN: c3f242b0 IDENTICAL/UNPROVEN
// OFFSET: 0x00843540/0x14056FA30
void CGxDevice::NotifyOnDeviceRestored() {
    for (uint32_t i = 0; i < CGxDevice::s_deviceRestoredCallbacks.Count(); i++) {
        CGxDevice::s_deviceRestoredCallbacks[i]();
    }
}

// PROVEN: 48c49a9a IDENTICAL/UNPROVEN
// OFFSET: 0x00843570/0x14056FA80
void CGxDevice::NotifyOnTextureRecreation() {
    for (uint32_t i = 0; i < CGxDevice::s_textureRecreationCallbacks.Count(); i++) {
        CGxDevice::s_textureRecreationCallbacks[i]();
    }
}

// PROVEN: e53110bc IDENTICAL/UNPROVEN
// OFFSET: 0x008435A0/0x14056FAD0
void CGxDevice::NotifyOnDisplayChange(uint32_t value) {
    for (uint32_t i = 0; i < CGxDevice::s_displayChangeCallbacks.Count(); i++) {
        CGxDevice::s_displayChangeCallbacks[i](value);
    }
}

// OFFSET: %0x007A9420
void CGxDevice::NotifyOnShaderModelChange() {
    for (uint32_t i = 0; i < CGxDevice::s_shaderModelChangeCallbacks.Count(); i++) {
        CGxDevice::ShaderModelCall& call = CGxDevice::s_shaderModelChangeCallbacks[i];
        call.m_callback(call.m_arg);
    }

    this->ShaderReloadAll();
}

// OFFSET: 0x00843AF0/0x14056FFC0
void CGxDevice::ITexMarkAsUpdated(CGxTex* texId) {
    if (texId->m_needsUpdate) {
        texId->m_updateRect.minY = texId->m_height;
        texId->m_updateRect.minX = texId->m_width;
        texId->m_updateRect.maxY = 0;
        texId->m_updateRect.maxX = 0;
        texId->m_needsUpdate = 0;
    }
}

// OFFSET: 0x00844600/0x1405711A0
void CGxDevice::AddDeviceRestoredCallback(GxDeviceCallback callback) {
    CGxDevice::s_deviceRestoredCallbacks.New(callback);
}

// OFFSET: 0x00844620/0x1405711C0
void CGxDevice::RemoveDeviceRestoredCallback(GxDeviceCallback callback) {
    uint32_t count = CGxDevice::s_deviceRestoredCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (CGxDevice::s_deviceRestoredCallbacks[i] == callback) {
            CGxDevice::s_deviceRestoredCallbacks[i] = CGxDevice::s_deviceRestoredCallbacks[count - 1];
            CGxDevice::s_deviceRestoredCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// OFFSET: 0x008446B0/0x140571280
void CGxDevice::AddTextureRecreationCallback(GxDeviceCallback callback) {
    CGxDevice::s_textureRecreationCallbacks.New(callback);
}

// OFFSET: 0x008446D0/0x1405712A0
void CGxDevice::RemoveTextureRecreationCallback(GxDeviceCallback callback) {
    uint32_t count = CGxDevice::s_textureRecreationCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (CGxDevice::s_textureRecreationCallbacks[i] == callback) {
            CGxDevice::s_textureRecreationCallbacks[i] = CGxDevice::s_textureRecreationCallbacks[count - 1];
            CGxDevice::s_textureRecreationCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// OFFSET: 0x00844760
void CGxDevice::AddDisplayChangeCallback(GxDisplayChangeCallback callback) {
    CGxDevice::s_displayChangeCallbacks.New(callback);
}

// OFFSET: 0x00844780/0x140571380
void CGxDevice::RemoveDisplayChangeCallback(GxDisplayChangeCallback callback) {
    uint32_t count = CGxDevice::s_displayChangeCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (CGxDevice::s_displayChangeCallbacks[i] == callback) {
            CGxDevice::s_displayChangeCallbacks[i] = CGxDevice::s_displayChangeCallbacks[count - 1];
            CGxDevice::s_displayChangeCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// OFFSET: 0x00844810/0x140571440
void CGxDevice::AddShaderModelChangeCallback(GxShaderModelCallback callback, void* arg) {
    CGxDevice::ShaderModelCall* call = CGxDevice::s_shaderModelChangeCallbacks.New();
    call->m_callback = callback;
    call->m_arg = arg;
}

// OFFSET: 0x00844880
void CGxDevice::RemoveShaderModelChangeCallback(GxShaderModelCallback callback, void* arg) {
    uint32_t count = CGxDevice::s_shaderModelChangeCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        CGxDevice::ShaderModelCall& call = CGxDevice::s_shaderModelChangeCallbacks[i];

        if (call.m_callback == callback && call.m_arg == arg) {
            call = CGxDevice::s_shaderModelChangeCallbacks[count - 1];
            CGxDevice::s_shaderModelChangeCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// OFFSET: 0x00847090
void CGxDevice::QueryCreate(CGxQuery*& query, EGxQueryType type) {
    query = new (SMemAlloc(sizeof(CGxQuery), __FILE__, __LINE__, 0)) CGxQuery(type);
    this->m_queryList.LinkToTail(query);
}

// OFFSET: 0x00847B00/0x140574F90
void CGxDevice::QueryDestroy(CGxQuery*& query) {
    query->m_link.Unlink();
    delete query;
    query = NULL;
}

// OFFSET: 0x00847BA0/0x140575060
void CGxDevice::ICursorDraw() {
    if (!CGxDevice::s_cursorVisible || this->m_hwCursor) {
        return;
    }

    int32_t x;
    int32_t y;
    OsInputGetMousePosition(&x, &y);

    if (x < 0 || y < 0 || !(x < this->m_curWindowRect.maxX) || !(y < this->m_curWindowRect.maxY)) {
        return;
    }

    g_theGxDevicePtr->RsPush();

    GxRsSet(GxRs_PolygonOffset, 0);
    GxRsSet(GxRs_NormalizeNormals, 0);
    GxRsSet(GxRs_BlendingMode, 1);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_ColorWrite, 15);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_ClipPlaneMask, 0);
    GxRsSet(GxRs_Texture0, this->m_cursorTexture);
    GxRsSet(GxRs_Texture1, NULL);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_TexGen0, 0);
    GxRsSet(GxRs_TextureShader0, 0);

    C44Matrix world;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    C44Matrix view;
    GxXformView(view);
    GxXformSetView(C44Matrix());

    C44Matrix projection;
    GxXformProjection(projection);

    bool stereo = this->StereoEnabled();
    float depth = 1.0f;

    if (s_uiVertexShader && s_uiVertexShader->m_permutes[stereo ? 1 : 0].Valid() && s_uiPixelShader && s_uiPixelShader->m_permutes[0].Valid()) {
        if (stereo) {
            depth = this->m_cursorDepth;
        }

        float minX;
        float maxX;
        float minY;
        float maxY;
        float minZ;
        float maxZ;
        GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

        C44Matrix ortho;
        ortho.a0 = 2.0f / (maxX - minX);
        ortho.a1 = 0.0f;
        ortho.a2 = 0.0f;
        ortho.a3 = 0.0f;
        ortho.b0 = 0.0f;
        ortho.b1 = 2.0f / (maxY - minY);
        ortho.b2 = 0.0f;
        ortho.b3 = 0.0f;
        ortho.c0 = 0.0f;
        ortho.c1 = 0.0f;
        ortho.d0 = -((minX + maxX) / (maxX - minX));
        ortho.d1 = -((minY + maxY) / (maxY - minY));

        if (stereo) {
            ortho.c2 = 1.00008f;
            ortho.c3 = 1.0f;
            ortho.d2 = -0.400016f;
            ortho.d3 = 0.0f;
        } else {
            ortho.c2 = 0.00039999999f;
            ortho.c3 = 0.0f;
            ortho.d2 = -1.00008f;
            ortho.d3 = 1.0f;
        }

        if (this->m_caps.m_texelCenterOnEdge) {
            ortho.b1 *= -1.0f;
            ortho.d1 *= -1.0f;
        }

        GxXformSetProjection(ortho);
        GxRsSet(GxRs_VertexShader, s_uiVertexShader, stereo != 0);
        GxRsSet(GxRs_PixelShader, s_uiPixelShader, 0);

        C44Matrix viewProj;
        GxXformViewProjNativeTranspose(viewProj);
        GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&viewProj), 4);

        float alphaRef[4] = { 0.0f, 0.0f, 0.0f, static_cast<float>(CGxDevice::s_alphaRef[GxBlend_AlphaKey]) * (1.0f / 255.0f) };
        GxShaderConstantsSet(GxSh_Pixel, 0, alphaRef, 1);
    } else {
        if (this->m_caps.m_texelCenterOnEdge) {
            C44Matrix flipped = projection;
            flipped.b1 *= -1.0f;
            flipped.d1 *= -1.0f;
            GxXformSetProjection(flipped);
        }

        GxRsSet(GxRs_VertexShader, 0);
        GxRsSet(GxRs_PixelShader, 0);
    }

    CGxBuf* buf = GxBufStream(GxPoolTarget_Vertex, 24, 4);
    float* v = reinterpret_cast<float*>(GxBufLock(buf));

    if (v) {
        float invWidth = this->m_curWindowRect.maxX > 0.0f ? 1.0f / this->m_curWindowRect.maxX : 0.0f;
        float invHeight = this->m_curWindowRect.maxY > 0.0f ? 1.0f / this->m_curWindowRect.maxY : 0.0f;

        float left = static_cast<float>(static_cast<uint32_t>(x - CGxDevice::s_cursorHotspotX)) * invWidth;
        float right = static_cast<float>(static_cast<uint32_t>(x - CGxDevice::s_cursorHotspotX + 32)) * invWidth;
        float top = 1.0f - static_cast<float>(static_cast<uint32_t>(y - CGxDevice::s_cursorHotspotY)) * invHeight;
        float bottom = 1.0f - static_cast<float>(static_cast<uint32_t>(y - CGxDevice::s_cursorHotspotY + 32)) * invHeight;

        if (this->m_caps.m_pixelCenterOnEdge) {
            left -= invWidth * 0.5f;
            right -= invWidth * 0.5f;
            top += invHeight * 0.5f;
            bottom += invHeight * 0.5f;
        }

        uint32_t* vertices = reinterpret_cast<uint32_t*>(v);

        v[0] = left;
        v[1] = top;
        v[2] = depth;
        vertices[3] = 0xFFFFFFFF;
        v[4] = 0.0f;
        v[5] = 0.0f;

        v[6] = left;
        v[7] = bottom;
        v[8] = depth;
        vertices[9] = 0xFFFFFFFF;
        v[10] = 0.0f;
        v[11] = 1.0f;

        v[12] = right;
        v[13] = top;
        v[14] = depth;
        vertices[15] = 0xFFFFFFFF;
        v[16] = 1.0f;
        v[17] = 0.0f;

        v[18] = right;
        v[19] = bottom;
        v[20] = depth;
        vertices[21] = 0xFFFFFFFF;
        v[22] = 1.0f;
        v[23] = 1.0f;

        GxBufUnlock(buf, 0);
        GxPrimVertexPtr(buf, GxVBF_PCT);

        CGxBatch batch;
        batch.m_primType = GxPrim_TriangleStrip;
        batch.m_start = 0;
        batch.m_count = 4;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 3;
        GxDrawNonIndexed(batch);
    }

    GxXformPop(GxXform_World);
    GxXformSetView(view);
    GxXformSetProjection(projection);
    g_theGxDevicePtr->RsPop();
}

// PROVEN: eb6be8fe IDENTICAL/NONE
// OFFSET: 0x00847960
void CGxDevice::XformSetProjection(const C44Matrix& matrix) {
    this->m_projection = matrix;
}

// OFFSET: 0x004E05A0
void CGxDevice::XformPush(EGxXform xf) {
    CGxMatrixStack& stack = this->m_xforms[xf];
    stack.m_level++;

    if (stack.m_level >= 3) {
        stack.m_level = 3;
    }

    stack.m_mtx[stack.m_level] = stack.m_mtx[stack.m_level - 1];
    stack.m_flags[stack.m_level] = stack.m_flags[stack.m_level - 1];
    stack.m_dirty = 1;
}

// PROVEN: 64320f83 IDENTICAL/IDENTICAL
// OFFSET: 0x008418D0/0x14056DC50
EGxApi CGxDevice::DeviceApi() {
    return this->m_api;
}

// OFFSET: 0x00841A50
int32_t CGxDevice::CapsIsWindowVisible(int32_t visible) {
    if (visible < 0) {
        return this->m_windowVisible;
    }

    this->m_windowVisible = visible > 0;
    return visible > 0;
}

// OFFSET: 0x00841AB0/0x14056DE30
int32_t CGxDevice::CapsHasContext(int32_t context) {
    if (context < 0) {
        return this->m_context;
    }

    this->m_context = context > 0;
    return context > 0;
}

// PROVEN: 8834e4ed IDENTICAL/IDENTICAL
// OFFSET: 0x00841E00/0x14056E170
int32_t CGxDevice::MasterEnable(EGxMasterEnables state) {
    return ((1 << state) & this->m_appMasterEnables) != 0;
}

// PROVEN: 4c06cb18 IDENTICAL/UNPROVEN
// OFFSET: 0x008443E0/0x140570EB0
void CGxDevice::DeviceClearScreenShot() {
    this->m_scrShotHeight = 0;
    this->m_scrShotWidth = 0;
    this->m_scrShotPixels.Clear();
}

// OFFSET: 0x00903F10
CGxCaps& CGxDevice::Caps() {
    return this->m_caps;
}

// OFFSET: 0x00481C10/0x1400A09C0
void CGxDevice::XformPushAndSet(EGxXform xf, const C44Matrix& matrix) {
    this->XformPush(xf);

    CGxMatrixStack& stack = this->m_xforms[xf];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;
}

// OFFSET: 0x008417F0/0x14056DB70
void CGxDevice::InitLights() {
    for (uint32_t i = 0; i < 4; i++) {
        this->m_lights[i].m_flags = 0xFF;
        this->m_lights[i].unk_0046 = 0xFF;
        this->m_lights[i].m_enable = 0;
    }
}

// PROVEN: 0ba88b1a IDENTICAL/IDENTICAL
// OFFSET: 0x008418C0/0x14056DC40
uint32_t CGxDevice::DeviceBaseMipLevel() {
    return this->m_baseMipLevel;
}

// OFFSET: 0x008418F0/0x14056DC70
int32_t CGxDevice::IDevIsWindowed() {
    return this->m_format.m_window;
}

// PROVEN: a2009c37 IDENTICAL/IDENTICAL
// OFFSET: 0x008418B0/0x14056DC30
const CGxFormat& CGxDevice::DeviceFormat() {
    return this->m_format;
}

// PROVEN: db8a7e2a IDENTICAL/IDENTICAL
// OFFSET: 0x00841A10/0x14056DDB0
const CRect& CGxDevice::DeviceCurWindow() {
    return this->m_curWindowRect;
}

// OFFSET: %0x0065DDB0
const CRect& CGxDevice::DeviceDefWindow() {
    return this->m_defWindowRect;
}

// PROVEN: 4a55d6d2 IDENTICAL/IDENTICAL
// OFFSET: 0x00841A20/0x14056DDD0
void CGxDevice::DeviceSetCurWindow(const CRect& rect) {
    this->m_curWindowRect = rect;
}

// OFFSET: 0x00841A80
int32_t CGxDevice::CapsWindowHasFocus(int32_t focus) {
    if (focus < 0) {
        return this->m_windowFocus;
    }

    this->m_windowFocus = focus > 0;
    return focus > 0;
}

// PROVEN: 10d672b6 IDENTICAL/NONE
// OFFSET: 0x00841AF0/0x14056DE50
void CGxDevice::SetScissor(const CRect& rect) {
    this->m_scissorRectDirty = 1;
    this->m_scissorRect = rect;
}

// PROVEN: 31097c48 IDENTICAL/IDENTICAL
// OFFSET: 0x00841BC0/0x14056DEF0
void CGxDevice::PrimIndexPtr(CGxBuf* buf) {
    this->m_primIndexBuf = buf;
}

// PROVEN: 9b1501f1 IDENTICAL/NONE
// OFFSET: 0x00841B30
EGxVertexBufferFormat CGxDevice::PrimVertexFormat() {
    return this->m_primVertexFormat;
}

// PROVEN: b27b0f1d IDENTICAL/DIFFERENT
// OFFSET: 0x00841C80/0x14056DFB0
uint32_t CGxDevice::PrimCalcCount(EGxPrim primType, uint32_t indexCount) {
    uint32_t perPrim = s_primVertsPerPrim[primType];
    return (perPrim == 1 ? indexCount : indexCount / perPrim) - s_primVertsExtra[primType];
}

// PROVEN: 8a2f07f2 IDENTICAL/IDENTICAL
// OFFSET: 0x00841D60/0x14056E0E0
void CGxDevice::LightEnable(uint32_t index, int32_t enable) {
    CGxApiLight& light = this->m_lights[index];

    if (light.m_enable != enable) {
        light.m_flags |= 0x1;
        light.m_enable = enable;
    }
}

// PROVEN: d4ee8d6f IDENTICAL/IDENTICAL
// OFFSET: 0x00841D90/0x14056E110
int32_t CGxDevice::GetLightEnabled(uint32_t index) {
    return this->m_lights[index].m_enable;
}

// OFFSET: 0x00841DB0/0x14056E120
int32_t CGxDevice::NeedsUpdate(uint32_t app, uint32_t hw, uint32_t appDisables, uint32_t hwDisables, uint32_t flagPos, int32_t& enable) {
    enable = (app & (1 << flagPos) & ~appDisables) != 0;
    return enable != ((hw & (1 << flagPos) & ~hwDisables) != 0);
}

// PROVEN: 9ab6648d IDENTICAL/IDENTICAL
// OFFSET: 0x00841E70/0x14056E1C0
void CGxDevice::BufSizeSet(CGxBuf* buf, uint32_t itemSize, uint32_t itemCount) {
    buf->m_itemSize = itemSize;
    buf->m_itemCount = itemCount;
    buf->m_size = itemCount * itemSize;
    buf->unk_001C = 0;
}

// PROVEN: f539d674 IDENTICAL/NONE
// OFFSET: 0x00841E90
int32_t CGxDevice::TexIsReadyNow(CGxTex* texId) {
    return texId->m_apiSpecificData && !texId->m_needsUpdate && !texId->m_needsCreation;
}

// PROVEN: 8f95cf3c IDENTICAL/NONE
// OFFSET: 0x00841EC0
int32_t CGxDevice::TexCanUpdate(CGxTex* texId) {
    return texId->m_userFunc != NULL;
}

// PROVEN: 39c1e56c IDENTICAL/NONE
// OFFSET: 0x00842140
void CGxDevice::TexSetCannotUpdate(CGxTex* texId) {
    texId->m_userFunc = NULL;
    texId->m_userArg = NULL;
}

// PROVEN: 0c5033e6 IDENTICAL/NONE
// OFFSET: 0x00842160
void CGxDevice::TexSetDataFormat(CGxTex* texId, EGxTexFormat dataFormat) {
    texId->m_dataFormat = dataFormat;
}

// PROVEN: ec5df957 IDENTICAL/NONE
// OFFSET: 0x00842170
void CGxDevice::TexParameters(const CGxTex* texId, CGxTexParms& parms) {
    parms.m_target = texId->m_target;
    parms.m_width = texId->m_width;
    parms.m_height = texId->m_height;
    parms.m_depth = texId->m_depth;
    parms.m_format = texId->m_format;
    parms.m_dataFormat = texId->m_dataFormat;
    parms.m_flags = texId->m_flags;
    parms.m_userArg = texId->m_userArg;
    parms.m_userFunc = texId->m_userFunc;
}

// OFFSET: 0x00848BB0/0x1405766F0
void CGxDevice::ShaderConstantsClear() {
    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        ShadowConstants& shadow = CGxDevice::s_shadowConstants[target];

        for (uint32_t i = 0; i < 256 * 4; i++) {
            shadow.constants[i] = FLT_MAX;
        }

        shadow.dirtyMin = 255;
        shadow.dirtyMax = 0;
    }
}

// OFFSET: 0x00848BF0
void CGxDevice::ShaderConstantsSet(EGxShTarget target, uint32_t index, const float* constants, uint32_t count) {
    ShadowConstants& shadow = CGxDevice::s_shadowConstants[target];
    float* dest = &shadow.constants[index * 4];

    for (uint32_t i = index; count; i++, count--) {
        int32_t changed = 0;

        if (dest[0] != constants[0]) {
            dest[0] = constants[0];
            changed = 1;
        }

        if (dest[1] != constants[1]) {
            dest[1] = constants[1];
            changed = 1;
        }

        if (dest[2] != constants[2]) {
            dest[2] = constants[2];
            changed = 1;
        }

        if (dest[3] != constants[3]) {
            dest[3] = constants[3];
            changed = 1;
        }

        if (changed) {
            if (shadow.dirtyMin > i) {
                shadow.dirtyMin = i;
            }

            if (shadow.dirtyMax < i) {
                shadow.dirtyMax = i;
            }
        }

        dest += 4;
        constants += 4;
    }
}

// PROVEN: 0378b82d IDENTICAL/UNPROVEN
// OFFSET: 0x00848CD0/0x140576820
float* CGxDevice::ShaderConstantsLock(EGxShTarget target) {
    return CGxDevice::s_shadowConstants[target].constants;
}

// PROVEN: 2cd027af IDENTICAL/UNPROVEN
// OFFSET: 0x00848CF0/0x140576840
void CGxDevice::ShaderConstantsUnlock(EGxShTarget target, uint32_t index, uint32_t count) {
    ShadowConstants& shadow = CGxDevice::s_shadowConstants[target];

    if (shadow.dirtyMin > index) {
        shadow.dirtyMin = index;
    }

    if (shadow.dirtyMax < index + count - 1) {
        shadow.dirtyMax = index + count - 1;
    }
}

// PROVEN: 2886939b IDENTICAL/IDENTICAL
// OFFSET: 0x008421E0/0x14056E4C0
void CGxDevice::ICursorDestroy() {
    if (this->m_cursorTexture) {
        GxTexDestroy(this->m_cursorTexture);
        this->m_cursorTexture = NULL;
    }
}

// PROVEN: 0474d4fb IDENTICAL/IDENTICAL
// OFFSET: 0x00842210/0x14056E4F0
void CGxDevice::ICursorUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch) {
        texelStrideInBytes = 32 * sizeof(uint32_t);
        texels = CGxDevice::s_cursor;
    }
}

// PROVEN: 67ff5500 IDENTICAL/IDENTICAL
// OFFSET: 0x00842250/0x14056E540
void CGxDevice::CursorSetDepth(float depth) {
    this->m_cursorDepth = depth;
}

// OFFSET: 0x00842F90
void CGxDevice::ClampRectLeft(CiRect& windowRect) {
    if (windowRect.maxX - windowRect.minX - s_clientAdjust.x < kWinMinSize.x) {
        windowRect.minX = windowRect.maxX - s_clientAdjust.x - kWinMinSize.x;
    }
}

// OFFSET: 0x00842FC0
void CGxDevice::ClampRectRight(CiRect& windowRect) {
    if (windowRect.maxX - windowRect.minX - s_clientAdjust.x < kWinMinSize.x) {
        windowRect.maxX = kWinMinSize.x + s_clientAdjust.x + windowRect.minX;
    }
}

// OFFSET: 0x00842FF0
void CGxDevice::ClampRectTop(CiRect& windowRect) {
    if (windowRect.maxY - windowRect.minY - s_clientAdjust.y < kWinMinSize.y) {
        windowRect.minY = windowRect.maxY - s_clientAdjust.y - kWinMinSize.y;
    }
}

// OFFSET: 0x00843020
void CGxDevice::ClampRectBottom(CiRect& windowRect) {
    if (windowRect.maxY - windowRect.minY - s_clientAdjust.y < kWinMinSize.y) {
        windowRect.maxY = kWinMinSize.y + s_clientAdjust.y + windowRect.minY;
    }
}

// OFFSET: 0x00843260
void CGxDevice::DeviceGamma(CGxGammaRamp& ramp) {
    ramp = this->m_systemGammaRamp;
}

// OFFSET: 0x00843280/0x14056F770
void CGxDevice::DeviceScreenShot() {
    this->m_scrShotWidth = static_cast<uint32_t>(this->m_curWindowRect.maxX);
    this->m_scrShotHeight = static_cast<uint32_t>(this->m_curWindowRect.maxY);

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = this->m_scrShotHeight;
    rect.maxX = this->m_scrShotWidth;
    this->DeviceReadPixels(rect, this->m_scrShotPixels);
}

// PROVEN: c13a4fa4 IDENTICAL/IDENTICAL
// OFFSET: 0x008434A0/0x14056F9A0
void CGxDevice::DeviceSetDefWindow(const CRect& rect) {
    this->m_defWindowRect = rect;
    this->m_curWindowRect = rect;
}

// OFFSET: 0x00843720/0x14056FC10
void CGxDevice::ClipPlaneSet(uint32_t index, const C4Plane& plane) {
    C4Plane& current = this->m_clipPlane[index];

    if (plane.n.x != current.n.x || plane.n.y != current.n.y || plane.n.z != current.n.z || plane.d != current.d) {
        this->m_clipPlaneMask |= 1 << index;
        current.n.x = plane.n.x;
        current.n.y = plane.n.y;
        current.n.z = plane.n.z;
        current.d = plane.d;
    }
}

// OFFSET: 0x008439A0/0x14056FE50
void CGxDevice::LightSet(uint32_t index, const CGxLight& light, const C3Vector& pos) {
    CGxApiLight& apiLight = this->m_lights[index];
    apiLight = light;

    if ((light.m_flags & 0x2) && (pos.x != 0.0f || pos.y != 0.0f || pos.z != 0.0f)) {
        apiLight.m_flags |= 0x2;
        apiLight.m_dir.x = apiLight.m_dir.x - pos.x;
        apiLight.m_dir.y = apiLight.m_dir.y - pos.y;
        apiLight.m_dir.z = apiLight.m_dir.z - pos.z;
    }
}

// PROVEN: e8da82fb IDENTICAL/IDENTICAL
// OFFSET: 0x00843A30/0x14056FEE0
CGxBuf* CGxDevice::BufStream(EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount) {
    CGxBuf* buf = this->m_streamBufs[target];
    CGxPool* pool = buf->m_pool;

    if (pool && static_cast<uint32_t>(pool->m_size) < itemCount * itemSize) {
        this->PoolSizeSet(pool, itemCount * itemSize);
    }

    buf->m_itemSize = itemSize;
    buf->m_size = itemCount * itemSize;
    buf->m_itemCount = itemCount;
    buf->unk_001C = 0;
    return buf;
}

// OFFSET: 0x00843A80/0x14056FF40
void CGxDevice::TexMarkForUpdate(CGxTex* texId, const CiRect& updateRect, int32_t immediate) {
    texId->m_needsUpdate = 1;

    if (updateRect.minY >= updateRect.maxY || updateRect.minX >= updateRect.maxX) {
        texId->m_updateRect.minY = 0;
        texId->m_updateRect.minX = 0;
        texId->m_updateRect.maxY = texId->m_height;
        texId->m_updateRect.maxX = texId->m_width;
    } else {
        texId->m_updateRect = updateRect;
    }

    texId->m_updatePlaneMin = 0;
    texId->m_updatePlaneMax = texId->m_depth;

    if (immediate) {
        this->ITexMarkAsUpdated(texId);
    }
}

// OFFSET: 0x00843B20
void CGxDevice::TexSetUserData(CGxTex* texId, GxTexUserFunc userFunc, void* userArg) {
    texId->m_userFunc = userFunc;
    texId->m_userArg = userArg;
    texId->m_updateRect.minY = 0;
    texId->m_updateRect.minX = 0;
    texId->m_updateRect.maxY = texId->m_height;
    texId->m_updateRect.maxX = texId->m_width;
    texId->m_needsUpdate = 1;
    texId->m_updatePlaneMin = 0;
    texId->m_updatePlaneMax = texId->m_depth;
}

// PROVEN: 2672507e IDENTICAL/UNPROVEN
// OFFSET: 0x00843CD0/0x140570210
void CGxDevice::ICursorCreate(const CGxFormat& format) {
    this->m_hwCursor = format.m_hwCursor && this->m_caps.m_hwCursor;

    if (!this->m_hwCursor && !this->m_cursorTexture) {
        CGxTexFlags flags;
        flags.m_filter = GxTex_Linear;
        flags.m_wrapU = 0;
        flags.m_wrapV = 0;
        flags.m_renderTarget = 0;
        GxTexCreate(32, 32, GxTex_Argb8888, flags, NULL, CGxDevice::ICursorUpdate, this->m_cursorTexture);
    }
}

// PROVEN: 26910fc1 IDENTICAL/IDENTICAL
// OFFSET: 0x00843D40/0x1405702A0
void CGxDevice::CursorUnlock(uint32_t x, uint32_t y) {
    CGxDevice::s_cursorHotspotX = x;
    CGxDevice::s_cursorHotspotY = y;

    if (this->m_cursorTexture) {
        CiRect rect;
        rect.minY = 0;
        rect.minX = 0;
        rect.maxY = 32;
        rect.maxX = 32;
        GxTexUpdate(this->m_cursorTexture, rect, 0);
    }
}

// OFFSET: 0x00844CA0
int32_t CGxDevice::TexCreate(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name, CGxTex*& texId) {
    texId = new (SMemAlloc(sizeof(CGxTex), __FILE__, __LINE__, 0)) CGxTex(target, width, height, depth, format, dataFormat, flags, userArg, userFunc, name);
    CGxDevice::s_textureList.LinkToHead(texId);
    return 1;
}

// OFFSET: 0x00847020/0x140574210
void CGxDevice::TexDestroy(CGxTex* texId) {
    texId->m_link.Unlink();
    delete texId;
}

// OFFSET: 0x00848500/0x140575DE0
void CGxDevice::BufDestroy(CGxBuf*& buf) {
    if (!buf) {
        return;
    }

    if (buf->m_pool) {
        buf->m_pool->UnlinkBuf(buf);
    }

    buf->m_link.Unlink();
    delete buf;

    if (this->m_primIndexBuf == buf) {
        this->m_primIndexBuf = NULL;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (this->m_primVertexBufs[i] == buf) {
            this->m_primVertexBufs[i] = NULL;
        }
    }

    buf = NULL;
}

// PROVEN: 5816835d IDENTICAL/NONE
// OFFSET: 0x00841B40/0x14056DE90
void CGxDevice::PrimVertexPtr(CGxBuf* buf, EGxVertexBufferFormat format) {
    this->m_primVertexBufs[0] = buf;
    this->m_primVertexBufs[1] = NULL;
    this->m_primVertexFormat = format;
    this->m_primVertexFormats[0] = format;
    this->m_primVertexFormats[1] = GxVertexBufferFormats_Last;
}

// PROVEN: 37979e58 IDENTICAL/NONE
// OFFSET: 0x00841B80/0x14056DEC0
void CGxDevice::PrimVertexPtrs(CGxBuf** bufs, const EGxVertexBufferFormat* formats, EGxVertexBufferFormat format) {
    this->m_primVertexBufs[0] = bufs[0];
    this->m_primVertexBufs[1] = bufs[1];
    this->m_primVertexFormat = format;
    this->m_primVertexFormats[0] = formats[0];
    this->m_primVertexFormats[1] = formats[1];
}

// OFFSET: 0x0040BDD0/0x14000E7D0
void CGxDevice::RsPush() {
    uint32_t offset = this->m_pushedStates.Count();
    *this->m_stackOffsets.New() = offset;
}

// OFFSET: 0x00844920/0x140571540
void CGxDevice::IRsDirty(EGxRenderState which) {
    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (!state.m_dirty) {
        *this->m_dirtyStates.New() = which;
        state.m_dirty = 1;
    }

    if (state.m_stackDepth != this->m_stackOffsets.Count()) {
        CGxPushedRenderState* pushed = this->m_pushedStates.New();
        pushed->m_which = which;
        pushed->m_value = state.m_value;
        pushed->m_stackDepth = state.m_stackDepth;
        state.m_stackDepth = this->m_stackOffsets.Count();
    }
}

// OFFSET: 0x00844990/0x1405715E0
void CGxDevice::IRsForceUpdate(EGxRenderState which) {
    if (!this->m_context) {
        return;
    }

    uint32_t count = this->m_dirtyStates.Count();
    this->m_dirtyStates.SetCount(count + 1);
    this->m_dirtyStates[count] = which;

    CGxAppRenderState& state = this->m_appRenderStates[which];
    state.m_dirty = 1;

    CGxStateBom& hw = this->m_hwRenderStates[which];
    hw.m_data.i[0] = ~state.m_value.m_data.i[0];
    hw.m_data.i[1] = ~state.m_value.m_data.i[1];
    hw.m_data.i[2] = ~state.m_value.m_data.i[2];
    hw.m_filler = ~state.m_value.m_filler;
}

// OFFSET: 0x00844A20/0x1405716B0
void CGxDevice::IRsForceUpdate() {
    for (uint32_t which = 0; which < GxRenderStates_Last; which++) {
        *this->m_dirtyStates.New() = static_cast<EGxRenderState>(which);

        CGxAppRenderState& state = this->m_appRenderStates[which];
        state.m_dirty = 1;

        CGxStateBom& hw = this->m_hwRenderStates[which];
        hw.m_data.i[0] = ~state.m_value.m_data.i[0];
        hw.m_data.i[1] = ~state.m_value.m_data.i[1];
        hw.m_data.i[2] = ~state.m_value.m_data.i[2];
        hw.m_filler = ~state.m_value.m_filler;
    }
}

// OFFSET: 0x00844B10/0x140571810
void CGxDevice::IRsSync(int32_t force) {
    if (force) {
        this->IRsForceUpdate();
    }

    for (uint32_t i = this->m_dirtyStates.Count(); i; i--) {
        EGxRenderState which = this->m_dirtyStates[i - 1];
        CGxAppRenderState& state = this->m_appRenderStates[which];
        CGxStateBom& hw = this->m_hwRenderStates[which];

        if (state.m_dirty) {
            if ((state.m_value.m_data.i[0] - hw.m_data.i[0]) | (state.m_value.m_data.i[1] - hw.m_data.i[1]) | (state.m_value.m_data.i[2] - hw.m_data.i[2]) | (state.m_value.m_filler - hw.m_filler)) {
                this->IRsSendToHw(which);
            }
        }

        hw = state.m_value;
        state.m_dirty = 0;
    }

    this->m_dirtyStates.SetCount(0);
}

// PROVEN: 05f53f52 IDENTICAL/DIFFERENT
// OFFSET: 0x00844D10/0x140571AC0
void CGxDevice::TexSetFlags(CGxTex* texId, CGxTexFlags flags) {
    if (!(texId->m_flags != flags)) {
        return;
    }

    if (texId->m_flags.m_filter != flags.m_filter) {
        texId->m_char5D = 0;
    }

    texId->m_flags = flags;
    texId->m_needsFlagUpdate = 1;

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_hwRenderStates[GxRs_VertexTexture0 + i].m_data.p == texId) {
            this->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_VertexTexture0 + i));
        }
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (this->m_hwRenderStates[GxRs_Texture0 + i].m_data.p == texId) {
            this->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_Texture0 + i));
        }
    }
}

// PROVEN: 9ebd758f IDENTICAL/DIFFERENT
// OFFSET: 0x00844DB0/0x140571B80
void CGxDevice::TexSetWrap(CGxTex* texId, EGxTexWrapMode wrapU, EGxTexWrapMode wrapV) {
    if (texId->m_flags.m_wrapU == static_cast<uint32_t>(wrapU) && texId->m_flags.m_wrapV == static_cast<uint32_t>(wrapV)) {
        return;
    }

    texId->m_flags.m_wrapU = wrapU;
    texId->m_flags.m_wrapV = wrapV;
    texId->m_needsFlagUpdate = 1;

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_hwRenderStates[GxRs_VertexTexture0 + i].m_data.p == texId) {
            this->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_VertexTexture0 + i));
        }
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (this->m_hwRenderStates[GxRs_Texture0 + i].m_data.p == texId) {
            this->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_Texture0 + i));
        }
    }
}

// PROVEN: 7b50f304 IDENTICAL/NONE
// OFFSET: 0x00845040
void CGxDevice::MasterEnableSet(EGxMasterEnables state, int32_t enable) {
    this->m_appMasterEnables = ((enable & 1) << state) | (this->m_appMasterEnables & ~(1 << state));

    switch (state) {
        case 0:
            this->IRsForceUpdate(GxRs_Lighting);
            break;

        case 1:
            this->IRsForceUpdate(GxRs_Fog);
            break;

        case 2:
            this->IRsForceUpdate(GxRs_DepthTest);
            break;

        case 3:
            this->IRsForceUpdate(GxRs_DepthWrite);
            break;

        case 4:
            this->IRsForceUpdate(GxRs_ColorWrite);
            break;

        case 5:
            this->IRsForceUpdate(GxRs_Culling);
            break;

        default:
            break;
    }
}

// PROVEN: df7294db IDENTICAL/DIFFERENT
// OFFSET: 0x008450E0/0x140571FC0
void CGxDevice::RsSet(EGxRenderState which, void* value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (state.m_value.m_data.p == value) {
        return;
    }

    this->IRsDirty(which);
    state.m_value.m_data.p = value;

    if (value && which >= GxRs_Texture0 && which <= GxRs_Texture15) {
        CGxTex* texId = static_cast<CGxTex*>(value);

        if (texId->m_flags.m_renderTarget && texId->m_needsUpdate) {
            this->ITexMarkAsUpdated(texId);
        }
    }
}

// OFFSET: 0x00845150/0x140572050
void CGxDevice::RsPop() {
    uint32_t offset = this->m_stackOffsets[this->m_stackOffsets.Count() - 1];

    for (uint32_t i = this->m_pushedStates.Count(); i > offset; i--) {
        CGxPushedRenderState& pushed = this->m_pushedStates[i - 1];
        CGxAppRenderState& state = this->m_appRenderStates[pushed.m_which];

        if (!state.m_dirty) {
            *this->m_dirtyStates.New() = pushed.m_which;
            state.m_dirty = 1;
        }

        state.m_value = pushed.m_value;
        state.m_stackDepth = pushed.m_stackDepth;
    }

    this->m_pushedStates.SetCount(offset);
    this->m_stackOffsets.SetCount(this->m_stackOffsets.Count() - 1);
}

// OFFSET: 0x008452C0
void CGxDevice::IRsInit() {
    this->m_appRenderStates.SetCount(GxRenderStates_Last);
    this->m_hwRenderStates.SetCount(GxRenderStates_Last);
    memset(this->m_appRenderStates.Ptr(), 0, sizeof(CGxAppRenderState) * GxRenderStates_Last);
    memset(this->m_hwRenderStates.Ptr(), 0, sizeof(CGxStateBom) * GxRenderStates_Last);

    this->m_appRenderStates[GxRs_PolygonOffset].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_MatDiffuse].m_value.m_data.i[0] = -1;
    this->m_appRenderStates[GxRs_MatEmissive].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_MatSpecular].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_MatSpecularExp].m_value.m_data.f[0] = 0.0f;
    this->m_appRenderStates[GxRs_NormalizeNormals].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_FogStart].m_value.m_data.f[0] = 1.0f;
    this->m_appRenderStates[GxRs_FogEnd].m_value.m_data.f[0] = 0.0f;
    this->m_appRenderStates[GxRs_FogColor].m_value.m_data.i[0] = 0xFF808080;
    this->m_appRenderStates[GxRs_Lighting].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_Fog].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_DepthTest].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_DepthFunc].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_DepthWrite].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_ColorWrite].m_value.m_data.i[0] = 15;
    this->m_appRenderStates[GxRs_Culling].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_ClipPlaneMask].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_Multisample].m_value.m_data.i[0] = 1;
    this->m_appRenderStates[GxRs_ScissorTest].m_value.m_data.i[0] = 0;

    for (uint32_t i = 0; i < 8; i++) {
        this->m_appRenderStates[GxRs_ColorOp0 + i].m_value.m_data.i[0] = 0;
        this->m_appRenderStates[GxRs_AlphaOp0 + i].m_value.m_data.i[0] = 0;
        this->m_appRenderStates[GxRs_TexGen0 + i].m_value.m_data.i[0] = 0;
        this->m_appRenderStates[GxRs_TextureShader0 + i].m_value.m_data.i[0] = 0;
        this->m_appRenderStates[GxRs_TextureCoord0 + i].m_value.m_data.i[0] = i;
    }

    this->m_appRenderStates[GxRs_VertexTexture0].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_VertexTexture1].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_VertexTexture2].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_VertexTexture3].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_Unk41].m_value.m_data.i[0] = 0;

    for (uint32_t i = GxRs_Texture0; i <= GxRs_Texture15; i++) {
        this->m_appRenderStates[i].m_value.m_data.i[0] = 0;
    }

    this->m_appRenderStates[GxRs_VertexShader].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_HullShader].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_DomainShader].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_GeometryShader].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_PixelShader].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_PointScale].m_value.m_data.f[0] = 1.0f;
    this->m_appRenderStates[GxRs_PointScaleAttenuation].m_value.m_data.f[0] = s_pointScaleAttenuation.x;
    this->m_appRenderStates[GxRs_PointScaleAttenuation].m_value.m_data.f[1] = s_pointScaleAttenuation.y;
    this->m_appRenderStates[GxRs_PointScaleAttenuation].m_value.m_data.f[2] = s_pointScaleAttenuation.z;
    this->m_appRenderStates[GxRs_PointScaleMin].m_value.m_data.f[0] = 0.0f;
    this->m_appRenderStates[GxRs_PointScaleMax].m_value.m_data.f[0] = 1.0f;
    this->m_appRenderStates[GxRs_PointSprite].m_value.m_data.i[0] = 0;
    this->m_appRenderStates[GxRs_LineWidth].m_value.m_data.f[0] = 1.0f;
    this->m_appRenderStates[GxRs_BlendFactor].m_value.m_data.f[0] = 0.0f;
    this->m_appRenderStates[GxRs_ColorMaterial].m_value.m_data.i[0] = 0;
}

// OFFSET: 0x00847970
void CGxDevice::XformSetView(const C44Matrix& matrix) {
    CGxMatrixStack& stack = this->m_xforms[GxXform_View];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = matrix;

    for (uint32_t i = 0; i < 8; i++) {
        if (static_cast<uint32_t>(this->m_appRenderStates[GxRs_TexGen0 + i].m_value.m_data.i[0] - 1) <= 1) {
            this->IRsForceUpdate(static_cast<EGxRenderState>(GxRs_TexGen0 + i));
        }
    }
}

// PROVEN: 4ea4330d IDENTICAL/IDENTICAL
// OFFSET: 0x00842270/0x14056E550
void CGxDevice::LogOpen() {
    if (!CGxDevice::m_log) {
        OsCreateDirectory("Logs", 0);
        SLogCreate("Logs\\gx.log", 0, &CGxDevice::m_log);
        CGxDevice::m_logBytes = 0;
    }
}

// PROVEN: e356c77a IDENTICAL/UNPROVEN
// OFFSET: 0x008422B0/0x14056E590
void CGxDevice::LogClose() {
    if (CGxDevice::m_log) {
        SLogClose(CGxDevice::m_log);
        CGxDevice::m_log = NULL;
    }
}

// OFFSET: 0x00842DF0
void CGxDevice::ClampRectToWindow(CiRect& rect) {
    CiRect window;
    window.minY = static_cast<int32_t>(this->m_curWindowRect.minY);
    window.minX = static_cast<int32_t>(this->m_curWindowRect.minX);
    window.maxY = static_cast<int32_t>(this->m_curWindowRect.maxY);
    window.maxX = static_cast<int32_t>(this->m_curWindowRect.maxX);
    rect = rect & window;
}

// OFFSET: 0x00842E50
void CGxDevice::ClampRectTopByAspect(CiRect& windowRect) {
    if (kWinAspectEnabled) {
        windowRect.minY = windowRect.maxY - static_cast<int32_t>(static_cast<float>(windowRect.maxX - windowRect.minX - s_clientAdjust.x) / kWinAspect + 0.5f + s_clientAdjust.y);
    }
}

// OFFSET: 0x00842EA0
void CGxDevice::ClampRectBottomByAspect(CiRect& windowRect) {
    if (kWinAspectEnabled) {
        windowRect.maxY = windowRect.minY + static_cast<int32_t>(static_cast<float>(windowRect.maxX - windowRect.minX - s_clientAdjust.x) / kWinAspect + 0.5f + s_clientAdjust.y);
    }
}

// OFFSET: 0x00842EF0
void CGxDevice::ClampRectLeftByAspect(CiRect& windowRect) {
    if (kWinAspectEnabled) {
        windowRect.minX = windowRect.maxX - static_cast<int32_t>(kWinAspect * static_cast<float>(windowRect.maxY - windowRect.minY - s_clientAdjust.y) + 0.5f + s_clientAdjust.x);
    }
}

// OFFSET: 0x00842F40
void CGxDevice::ClampRectRightByAspect(CiRect& windowRect) {
    if (kWinAspectEnabled) {
        windowRect.maxX = windowRect.minX + static_cast<int32_t>(kWinAspect * static_cast<float>(windowRect.maxY - windowRect.minY - s_clientAdjust.y) + 0.5f + s_clientAdjust.x);
    }
}

// PROVEN: 0a9f0e4f IDENTICAL/NONE
// OFFSET: 0x008422D0/0x14056E5C0
void CGxDevice::LogV(const char* format, va_list args) {
    if (!CGxDevice::m_log) {
        return;
    }

    char buffer[2048];
    _vsnprintf(buffer, sizeof(buffer), format, args);

    if (CGxDevice::m_logBytes < 0x200000) {
        SLogWrite(CGxDevice::m_log, buffer);
        CGxDevice::m_logBytes += static_cast<uint32_t>(strlen(buffer));
    }
}

// PROVEN: 6a94f89b IDENTICAL/DIFFERENT
// OFFSET: 0x00842350/0x14056E640
void CGxDevice::Log(const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (CGxDevice::m_log) {
        CGxDevice::LogV(format, args);
    }

    va_end(args);
}

// OFFSET: 0x00842370/0x14056E680
void CGxDevice::Log(const CGxCaps& caps) const {
    CGxDevice::Log("Caps:");
    CGxDevice::Log("\tVendor ID: %x", caps.m_vendorId);
    CGxDevice::Log("\tDevice ID: %x", caps.m_deviceId);
    CGxDevice::Log("\tnumTmus: %d", caps.m_numTmus);
    CGxDevice::Log("\ttexFilterAnisotropic: %d, %d", caps.m_texFilterTrilinear, caps.m_maxTexAnisotropy);
    CGxDevice::Log("\trttFormat: %d, %d", caps.m_rttFormat[GxTex_Argb8888], caps.m_rttFormat[GxTex_Rgb565]);
    CGxDevice::Log("\tvertexShaderConstants: %d", caps.m_shaderConstants[GxSh_Vertex]);
    CGxDevice::Log("\tnumStreams: %d", caps.m_numStreams);
    CGxDevice::Log("\tstereoAvailable: %d", caps.m_stereoAvailable);
    CGxDevice::Log("Format Support:");

    for (uint32_t i = 0; i < GxTexFormats_Last; i++) {
        if (caps.m_vertexTexFmt[i] || caps.m_texFmt[i] || caps.m_rttFormat[i] || caps.m_pcfFormat[i]) {
            CGxDevice::Log(" %s:%s%s%s%s", CGxDevice::s_texFormatNames[i], caps.m_vertexTexFmt[i] ? " vertex" : "", caps.m_texFmt[i] ? " pixel" : "", caps.m_rttFormat[i] ? " rtt" : "", caps.m_pcfFormat[i] ? " pcf" : "");
        }
    }

    CGxDevice::Log("Shader Profile Support:");

    for (uint32_t i = 0; i < GxShProfiles_Last; i++) {
        if (this->m_caps.m_shaderProfiles[i]) {
            CGxDevice::Log("\t%s", CGxDevice::s_shaderProfileNames[i]);
        }
    }

    CGxDevice::Log("Texture Target Support:");
    CGxDevice::Log("\tGxTex_2d - %s", this->m_caps.m_texTarget[GxTex_2d] ? "true" : "false");
    CGxDevice::Log("\tGxTex_CubeMap - %s", this->m_caps.m_texTarget[GxTex_CubeMap] ? "true" : "false");
    CGxDevice::Log("\tGxTex_Rectangle - %s", this->m_caps.m_texTarget[GxTex_Rectangle] ? "true" : "false");
    CGxDevice::Log("\tGxTex_NonPow2 - %s", this->m_caps.m_texTarget[GxTex_NonPow2] ? "true" : "false");
    CGxDevice::Log("\tGxTex_Volume - %s", this->m_caps.m_texTarget[GxTex_Volume] ? "true" : "false");
    CGxDevice::Log("\tNPTNoDependentReads - %s", this->m_caps.m_texNonPow2 ? "true" : "false");
}

// PROVEN: ffa278b7 IDENTICAL/IDENTICAL
// OFFSET: 0x008425F0/0x14056E910
void CGxDevice::Log(const CGxFormat& format) const {
    if (format.m_window) {
        CGxDevice::Log("\tFormat: %d x %d Window, %s, multisample %d", format.m_size.x, format.m_size.y, s_formatNames[format.m_depthFormat], format.m_multisampleCount);
    } else {
        CGxDevice::Log("\tFormat %d x %d @ %.1f Fullscreen, %s, %s, multisample %d", format.m_size.x, format.m_size.y, static_cast<double>(static_cast<float>(format.m_refreshRate) / static_cast<float>(format.m_refreshRateDenom)), s_formatNames[format.m_colorFormat], s_formatNames[format.m_depthFormat], format.m_multisampleCount);
    }
}

// OFFSET: 0x008470F0/0x140574300
CGxDevice::CGxDevice() : m_defWindowRect(), m_curWindowRect(), m_viewport(), m_scissorRect(), m_appMasterEnables(0), m_hwMasterEnables(0) {
    this->IRsInit();

    for (uint32_t format = 0; format < GxVertexBufferFormats_Last; format++) {
        VertexBufDesc& desc = Buffer::s_vertexBufDesc[format];

        for (uint32_t i = 0; i < desc.attribCount; i++) {
            const CGxVertexAttrib* attrib = &desc.attribs[i];
            desc.attribPtrs[attrib->m_attrib] = attrib;
            desc.mask |= 1 << attrib->m_attrib;
        }
    }

    this->m_frameCount = 0;
    this->m_api = GxApis_Last;
    this->m_windowProc = NULL;
    this->m_cpuFeatures = OsGetProcessorFeatures();
    memset(&this->m_caps, 0, sizeof(this->m_caps));
    this->m_caps.m_shaderModel = -1;
    this->m_baseMipLevel = 0;
    this->m_caps.m_maxTexAnisotropy = 1;
    this->m_caps.m_numRenderTargets = 1;
    this->m_viewport.m_x.m_l = 0.0f;
    this->m_viewport.m_x.m_h = 1.0f;
    this->m_viewport.m_y.m_l = 0.0f;
    this->m_viewport.m_y.m_h = 1.0f;
    this->m_viewport.m_z.m_l = 0.0f;
    this->m_viewport.m_z.m_h = 1.0f;
    this->m_viewportDirty = 1;
    this->InitLights();
    this->m_scrShotClick = 0;
    this->m_scrShotWidth = 0;
    this->m_scrShotHeight = 0;
    this->m_appMasterEnables = 0x1FF;
    this->m_hwMasterEnables = 0x1FF;
    this->m_bufLocked[0] = NULL;
    this->m_bufLocked[1] = NULL;
    this->m_vertexPool = NULL;
    this->m_indexPool = NULL;
    this->m_streamBufs[0] = NULL;
    this->m_streamBufs[1] = NULL;
    this->m_clipPlaneMask = 0;
    memset(this->m_clipPlane, 0, sizeof(this->m_clipPlane));
    this->m_scissorRect.minY = 0.0f;
    this->m_scissorRect.minX = 0.0f;
    this->m_scissorRect.maxY = 0.0f;
    this->m_scissorRect.maxX = 0.0f;
    this->m_scissorRectDirty = 1;
    this->m_primVertexFormat = GxVertexBufferFormats_Last;
    this->m_primVertexFormats[0] = GxVertexBufferFormats_Last;
    this->m_primVertexFormats[1] = GxVertexBufferFormats_Last;
    this->m_primIndexBuf = NULL;
    this->m_primVertexBufs[0] = NULL;
    this->m_primVertexBufs[1] = NULL;
    this->m_gammaRamp.Set(1.0f);
    this->m_systemGammaRamp.Set(1.0f);
    memset(this->m_textureTarget, 0, sizeof(this->m_textureTarget));
    this->m_hwCursor = 0;
    this->m_cursorTexture = NULL;
    this->m_cursorDepth = 1.0f;
    this->m_context = 0;
    this->m_intF5C = 0;
    this->m_windowVisible = 0;
    this->m_windowFocus = 0;
    this->m_backBufferColorTex = NULL;
    this->m_backBufferDepthTex = NULL;
    this->m_backBuffer = NULL;
    this->m_frameBuffer = NULL;
    this->m_frameBufferVersion = 0;
    this->ShaderConstantsClear();
}

// PROVEN: 33d6dde8 IDENTICAL/NONE
// OFFSET: 0x00847950
void CGxDevice::IShaderBind(CGxShaderPermute* permute) {
}

// PROVEN: 36b582c1 IDENTICAL/DIFFERENT
// OFFSET: 0x00844430/0x140570F20
void CGxDevice::ICreateBackBufferFB(uint32_t width, uint32_t height, void* colorApiData, void* depthApiData) {
    CGxTexFlags flags;
    flags.m_renderTarget = 1;

    if (!this->m_backBufferColorTex) {
        this->m_backBufferColorTex = new (SMemAlloc(sizeof(CGxTex), __FILE__, __LINE__, 0)) CGxTex(GxTex_5, width, height, 1, GxTex_Abgr8888, GxTex_Abgr8888, flags, NULL, NULL, "backBufferColorTex");
    }

    this->m_backBufferColorTex->m_width = width;
    this->m_backBufferColorTex->m_height = height;
    this->m_backBufferColorTex->m_apiSpecificData = colorApiData;
    this->m_backBufferColorTex->m_needsCreation = 0;

    if (!this->m_backBufferDepthTex) {
        this->m_backBufferDepthTex = new (SMemAlloc(sizeof(CGxTex), __FILE__, __LINE__, 0)) CGxTex(GxTex_5, width, height, 1, GxTex_D24X8, GxTex_D24X8, flags, NULL, NULL, "backBufferDepthTex");
    }

    this->m_backBufferDepthTex->m_width = width;
    this->m_backBufferDepthTex->m_height = height;
    this->m_backBufferDepthTex->m_apiSpecificData = depthApiData;
    this->m_backBufferDepthTex->m_needsCreation = 0;

    if (!this->m_backBuffer) {
        this->m_backBuffer = GxFrameBufferCreate(this->m_backBufferDepthTex, this->m_backBufferColorTex);
    }

    this->m_frameBufferVersion = 0;
    this->m_frameBuffer = this->m_backBuffer;
}

// PROVEN: 70cf3799 IDENTICAL/IDENTICAL
// OFFSET: 0x00847A20/0x140574E60
void CGxDevice::IDeleteBackBufferFB(int32_t deleteTextures) {
    if (deleteTextures) {
        CGxTex* colorTex = this->m_backBufferColorTex;

        if (colorTex) {
            colorTex->~CGxTex();
            SMemFree(colorTex, "delete", -1, 0);
        }

        this->m_backBufferColorTex = NULL;
        CGxTex* depthTex = this->m_backBufferDepthTex;

        if (depthTex) {
            depthTex->~CGxTex();
            SMemFree(depthTex, "delete", -1, 0);
        }

        this->m_backBufferDepthTex = NULL;
    }

    if (this->m_backBuffer) {
        this->m_backBuffer->m_bound = 0;

        if (this->m_backBuffer) {
            g_theGxDevicePtr->FrameBufferDelete(this->m_backBuffer);
        }
    }
}

// OFFSET: [%0x007AE400, %0x007AE5A0]
CGxDevice::~CGxDevice() {
    this->IDeleteBackBufferFB(1);
    this->m_frameBuffer = NULL;
    this->BufDestroy(this->m_streamBufs[0]);
    this->BufDestroy(this->m_streamBufs[1]);
    this->PoolDestroy(this->m_vertexPool);
    this->PoolDestroy(this->m_indexPool);
}

// OFFSET: 0x00846F30/0x140574080
CGxBuf* CGxDevice::BufCreate(CGxPool* pool, uint32_t itemSize, uint32_t itemCount, uint32_t index) {
    CGxBuf* buf = new (SMemAlloc(sizeof(CGxBuf), __FILE__, __LINE__, 0)) CGxBuf(pool, itemSize, itemCount, index);
    pool->m_bufList.LinkToTail(buf);
    buf->m_pool = pool;
    return buf;
}

// OFFSET: 0x00846FB0/0x140574110
CGxPool* CGxDevice::PoolCreate(EGxPoolTarget target, EGxPoolUsage usage, int32_t size, EGxPoolHintBits hint, const char* name) {
    CGxPool* pool = new (SMemAlloc(sizeof(CGxPool), __FILE__, __LINE__, 0)) CGxPool(target, usage, size, hint, name);
    CGxDevice::s_poolList.LinkToTail(pool);
    return pool;
}

// OFFSET: 0x008485E0/0x140575F00
void CGxDevice::PoolDestroy(CGxPool* pool) {
    if (pool) {
        CGxDevice::s_poolList.UnlinkNode(pool);
        delete pool;
    }
}

// PROVEN: 7da99d49 IDENTICAL/NONE
// OFFSET: 0x008479E0
void CGxDevice::DeviceCreateStreamBufs() {
    this->m_streamBufs[0] = this->BufCreate(this->m_vertexPool, 0, 0, 0);
    this->m_streamBufs[1] = this->BufCreate(this->m_indexPool, 0, 0, 0);
}

// PROVEN: dea72ba5 DIFFERENT/IDENTICAL
// OFFSET: 0x00847AC0/0x140574F20
void CGxDevice::DeviceCreatePools() {
    this->m_vertexPool = this->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Stream, s_streamVertexPoolSize, GxPoolHintBit_Unk3, "Stream_vtx");
    this->m_indexPool = this->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Stream, s_streamIndexPoolSize, GxPoolHintBit_Unk3, "Stream_idx");
}

// PROVEN: 3eb9bc67 IDENTICAL/NONE
// OFFSET: 0x008433F0/0x14056F8A0
void CGxDevice::FrameBufferSet(CGxFrameBuffer* frameBuffer) {
    if (this->m_frameBuffer == frameBuffer && this->m_frameBufferVersion == frameBuffer->m_version) {
        return;
    }

    int32_t oldCount = this->m_frameBuffer->m_count;
    int32_t count = frameBuffer->m_count;
    int32_t i;

    for (i = 0; i < count; i++) {
        this->DeviceSetRenderTarget(static_cast<EGxBuffer>(i), frameBuffer->m_targets[i].m_texture, frameBuffer->m_targets[i].m_plane);
    }

    for (i = count; i < oldCount; i++) {
        this->DeviceSetRenderTarget(static_cast<EGxBuffer>(i), NULL, 0);
    }

    this->m_frameBuffer->m_bound = 0;
    this->m_frameBuffer = frameBuffer;
    frameBuffer->m_bound = 1;
    this->m_frameBufferVersion = this->m_frameBuffer->m_version;
}

// PROVEN: d3395b75 IDENTICAL/IDENTICAL
// OFFSET: 0x008434E0/0x14056F9D0
void CGxDevice::DeviceOverride(EGxOverride override, uint32_t value) {
    CGxDevice::Log("CGxDevice::DeviceOverride(): %d set to %d", override, value);

    if (override == GxOverride_Unk8) {
        int32_t enable = value > 1;
        this->m_caps.m_pointParameters = enable;
        this->m_caps.m_pointSprite = enable;
        this->m_caps.m_pointScaleMax = static_cast<float>(value);
    }
}

// PROVEN: 5e767dc5 IDENTICAL/DIFFERENT
// OFFSET: 0x00842670/0x14056E9B0
void CGxDevice::WaitForFPSCap() {
    uint32_t maxFps = GxGetMaxFPS() ? GxGetMaxFPS() : 0xFFFFFFFF;
    uint32_t maxFpsBk = GxGetMaxFPSBk() ? GxGetMaxFPSBk() : 0xFFFFFFFF;

    if (!this->m_windowFocus && maxFps >= maxFpsBk) {
        maxFps = maxFpsBk;
    }

    if (maxFps <= 8) {
        maxFps = 8;
    } else if (maxFps == 0xFFFFFFFF) {
        return;
    }

    uint32_t frameMs = 1000 / maxFps;
    uint32_t now = OsGetAsyncTimeMs();

    if (s_nextFrameTime - now < 1000) {
        OsSleep(s_nextFrameTime - now);
        now = OsGetAsyncTimeMs();
    }

    s_nextFrameTime = now + frameMs;
}

// OFFSET: 0x00842060/0x14056E370
void CGxDevice::ITexWHDStartEnd(CGxTex* texId, uint32_t& width, uint32_t& height, uint32_t& depth, uint32_t& baseMip, uint32_t& mipCount) {
    width = texId->m_width;
    height = texId->m_height;
    depth = texId->m_depth;

    if (texId->m_flags.m_filter < GxTex_NearestMipNearest) {
        baseMip = 0;
        mipCount = 1;
        return;
    }

    if (texId->m_format == GxTex_Dxt1 || texId->m_format == GxTex_Dxt3 || texId->m_format == GxTex_Dxt5) {
        uint32_t size = texId->m_height < texId->m_width ? texId->m_height : texId->m_width;
        mipCount = 1;

        while (size > 4) {
            size >>= 1;
            mipCount++;
        }
    } else {
        uint32_t size = texId->m_width > texId->m_height ? texId->m_width : texId->m_height;
        mipCount = 1;

        while (size != 1) {
            size >>= 1;
            mipCount++;
        }
    }

    baseMip = this->m_baseMipLevel < mipCount - 1 ? this->m_baseMipLevel : mipCount - 1;
    width >>= baseMip;
    height >>= baseMip;
}

// OFFSET: %0x007A7950
void CGxDevice::DeviceGetRenderTarget(EGxBuffer buffer, CGxTex*& texture) {
    texture = this->m_textureTarget[buffer].m_texture;
}

// INLINED
void CGxDevice::RsSet(EGxRenderState which, int32_t value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (state.m_value.m_data.p != reinterpret_cast<void*>(static_cast<intptr_t>(value))) {
        this->IRsDirty(which);
        state.m_value.m_data.p = reinterpret_cast<void*>(static_cast<intptr_t>(value));
    }
}

// INLINED
void CGxDevice::RsSet(EGxRenderState which, float value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (value != state.m_value.m_data.f[0]) {
        this->IRsDirty(which);
        state.m_value.m_data.f[0] = value;
    }
}

// INLINED
void CGxDevice::RsSet(EGxRenderState which, const C3Vector& value) {
    if (!this->m_context) {
        return;
    }

    CGxAppRenderState& state = this->m_appRenderStates[which];

    if (value.x != state.m_value.m_data.f[0] || value.y != state.m_value.m_data.f[1] || value.z != state.m_value.m_data.f[2]) {
        this->IRsDirty(which);
        state.m_value.m_data.f[0] = value.x;
        state.m_value.m_data.f[1] = value.y;
        state.m_value.m_data.f[2] = value.z;
    }
}

// OFFSET: 0x00841CB0/0x14056DFF0
void CGxDevice::Light(uint32_t index, CGxLight& light) {
    CGxApiLight& apiLight = this->m_lights[index];
    light.m_flags ^= (light.m_flags ^ apiLight.m_enable) & 0x1;

    if (light.m_flags & 0x1) {
        light.m_flags ^= (light.m_flags ^ (2 * (apiLight.m_dir.w != 0.0f))) & 0x2;
        light.m_dir.x = apiLight.m_dir.x;
        light.m_dir.y = apiLight.m_dir.y;
        light.m_dir.z = apiLight.m_dir.z;
        light.m_ambientColor = apiLight.m_ambColor;
        light.m_dirColor = apiLight.m_dirColor;
        light.m_specularColor = apiLight.m_specColor;
        light.m_constantAttenuation = apiLight.m_constantAttenuation;
        light.m_linearAttenuation = apiLight.m_linearAttenuation;
        light.m_quadraticAttenuation = apiLight.m_quadraticAttenuation;
    }
}

// OFFSET: 0x00841EE0/0x14056E210
uint32_t CGxDevice::ITexBitsSize(CGxTex* texId, uint32_t width, uint32_t height, uint32_t depth) {
    if (width == 0xFFFFFFFF) {
        width = texId->m_width;

        if (texId->m_flags.m_filter > GxTex_Linear) {
            width = texId->m_width >> this->m_baseMipLevel ? texId->m_width >> this->m_baseMipLevel : 1;
        }
    }

    if (height == 0xFFFFFFFF) {
        height = texId->m_height;

        if (texId->m_flags.m_filter > GxTex_Linear) {
            height = texId->m_height >> this->m_baseMipLevel ? texId->m_height >> this->m_baseMipLevel : 1;
        }
    }

    if (depth == 0xFFFFFFFF) {
        depth = texId->m_depth;

        if (texId->m_flags.m_filter > GxTex_Linear) {
            depth >>= this->m_baseMipLevel;

            if (!depth) {
                depth = 1;
            }
        }
    }

    uint32_t blockSize = s_texFormatBlockSize[texId->m_format];
    uint32_t size = depth * s_texFormatBytesPerBlock[texId->m_format] * (blockSize + width - 1) / blockSize * ((blockSize + height - 1) / blockSize);

    if (texId->m_flags.m_filter > GxTex_Linear) {
        while (width != 1 || height != 1 || depth != 1) {
            if (width > 1) {
                width >>= 1;
            }

            if (height > 1) {
                height >>= 1;
            }

            if (depth > 1) {
                depth >>= 1;
            }

            blockSize = s_texFormatBlockSize[texId->m_format];
            size += depth * s_texFormatBytesPerBlock[texId->m_format] * (blockSize + width - 1) / blockSize * ((blockSize + height - 1) / blockSize);
        }
    }

    if (texId->m_target == GxTex_CubeMap) {
        return 6 * size;
    }

    return size;
}

// PROVEN: 169494b5 IDENTICAL/IDENTICAL
// OFFSET: 0x00843050/0x14056F520
int32_t CGxDevice::FormatExists(TSGrowableArray<CGxFormat>& formats, const CGxFormat& format) {
    for (uint32_t i = 0; i < formats.Count(); i++) {
        CGxFormat& entry = formats[i];

        if (entry.m_colorFormat == format.m_colorFormat && entry.m_depthFormat == format.m_depthFormat && entry.m_multisampleCount == format.m_multisampleCount) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00843E90/0x140570440
void CGxDevice::GetSurveyModeInfo(uint32_t& desktopWidth, uint32_t& desktopHeight, uint32_t& desktopBpp, uint32_t& width, uint32_t& height, uint32_t& bpp) {
    int32_t monitor = this->m_format.m_monitor;
    CGxMonitorMode mode;
    mode.m_size.x = 0;
    mode.m_size.y = 0;

    if (CGxDevice::AdapterDesktopMode(monitor, mode)) {
        desktopWidth = mode.m_size.x;
        desktopHeight = mode.m_size.y;
        desktopBpp = mode.m_bpp;
    }

    width = this->m_format.m_size.x;
    height = this->m_format.m_size.y;

    if (this->m_format.m_window) {
        bpp = desktopBpp;
    } else if (this->m_format.m_colorFormat < CGxFormat::Formats_Last && s_formatToInt[this->m_format.m_colorFormat]) {
        bpp = SStrToUnsigned(s_formatToInt[this->m_format.m_colorFormat]);
    }
}

// OFFSET: 0x00845770
void CGxDevice::RsInit() {
    this->RsSet(GxRs_PolygonOffset, 0);
    this->RsSet(GxRs_MatDiffuse, -1);
    this->RsSet(GxRs_MatEmissive, 0.0f);
    this->RsSet(GxRs_MatSpecular, 0.0f);
    this->RsSet(GxRs_NormalizeNormals, 0);
    this->RsSet(GxRs_BlendingMode, 0);
    this->RsSet(GxRs_FogStart, 1.0f);
    this->RsSet(GxRs_FogEnd, 0.0f);
    this->RsSet(GxRs_FogColor, static_cast<int32_t>(0xFFFF0000));
    this->RsSet(GxRs_Lighting, 1);
    this->RsSet(GxRs_Fog, 1);
    this->RsSet(GxRs_DepthTest, 1);
    this->RsSet(GxRs_DepthFunc, 0);
    this->RsSet(GxRs_Culling, 1);
    this->RsSet(GxRs_ClipPlaneMask, 0);
    this->RsSet(GxRs_ScissorTest, 0);

    for (uint32_t tmu = 0; tmu < 8; tmu++) {
        this->RsSet(static_cast<EGxRenderState>(GxRs_ColorOp0 + tmu), 0);
        this->RsSet(static_cast<EGxRenderState>(GxRs_AlphaOp0 + tmu), 0);
        this->RsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + tmu), 0);
        this->RsSet(static_cast<EGxRenderState>(GxRs_TextureShader0 + tmu), 0);
        this->RsSet(static_cast<EGxRenderState>(GxRs_TextureCoord0 + tmu), static_cast<int32_t>(tmu));
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->RsSet(static_cast<EGxRenderState>(GxRs_VertexTexture0 + i), 0);
    }

    this->RsSet(GxRs_Unk41, NULL);

    for (uint32_t tmu = 0; tmu < 16; tmu++) {
        this->RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + tmu), NULL);
    }

    this->RsSet(GxRs_VertexShader, NULL);
    this->RsSet(GxRs_HullShader, NULL);
    this->RsSet(GxRs_DomainShader, NULL);
    this->RsSet(GxRs_GeometryShader, NULL);
    this->RsSet(GxRs_PixelShader, NULL);
    this->RsSet(GxRs_PointScale, 1.0f);
    this->RsSet(GxRs_PointScaleAttenuation, C3Vector(1.0f, 0.0f, 0.0f));
    this->RsSet(GxRs_PointScaleMin, 0.0f);
    this->RsSet(GxRs_PointScaleMax, 1.0f);
    this->RsSet(GxRs_PointSprite, 0);
    this->RsSet(GxRs_LineWidth, 1.0f);
    this->RsSet(GxRs_BlendFactor, 0.0f);
    this->RsSet(GxRs_ColorMaterial, 0);
}

// INLINED
CGxMonitorMode::CGxMonitorMode() {
    this->m_size.x = 0;
    this->m_size.y = 0;
}

// OFFSET: 0x00848F10/0x140576B50
bool CGxDevice::IShaderLoad(CGxShader* shader, EGxShTarget target, const char* name) {
    int32_t profile = this->m_caps.m_shaderTargets[target];
    const char* path = CGxDevice::s_shaderPaths[target];

    while (true) {
        char filename[260];
        SStrPrintf(filename, sizeof(filename), "%s\\%s\\%s.bls", path, CGxDevice::s_shaderProfileNames[profile], name);

        bool loaded = false;
        SFileImpl* file;
        SFileOpenFile(filename, &file);

        if (file) {
            uint32_t size = SFileGetFileSize(file, NULL);
            uint8_t* data = static_cast<uint8_t*>(SMemAlloc(size, "CGxShader.cpp", 203, 0));

            if (SFile::Read(file, data, size, NULL) && shader->Load(data, size)) {
                loaded = true;
            }

            SFileCloseFile(file);

            if (loaded) {
                return true;
            }
        }

        if (profile == GxShProfile_Ps14) {
            profile = GxShProfile_Ps11;
        } else if (profile == GxShProfile_Fp25) {
            profile = GxShProfile_Fp20;
        } else {
            return false;
        }
    }
}

// OFFSET: 0x00849000/0x140576CA0
void CGxDevice::ShaderRelease(CGxShader* shader) {
    if (--shader->m_refCount) {
        return;
    }

    for (uint32_t i = 0; i < shader->m_permutes.Count(); i++) {
        this->IShaderDestroy(&shader->m_permutes[i]);
    }

    shader->m_permutes.Clear();
    CGxDevice::s_shaderList[shader->m_target].Delete(shader);
}

// PROVEN: c4349663 IDENTICAL/NONE
// OFFSET: 0x00849110
bool CGxDevice::ShaderReload(CGxShader* shader, const char* name) {
    for (uint32_t i = 0; i < shader->m_permutes.Count(); i++) {
        this->IShaderDestroy(&shader->m_permutes[i]);
    }

    SMemFree(shader->m_data, "CGxShader.cpp", 288, 0);
    shader->m_data = NULL;

    return this->IShaderLoad(shader, static_cast<EGxShTarget>(shader->m_target), name);
}

// OFFSET: 0x00849220/0x140576F80
void CGxDevice::ShaderReloadAll() {
    for (uint32_t target = 0; target < GxShTargets_Last; target++) {
        CGxShader* next;

        for (CGxShader* shader = CGxDevice::s_shaderList[target].Head(); shader; shader = next) {
            next = CGxDevice::s_shaderList[target].Next(shader);
            this->ShaderReload(shader, shader->m_key.m_str);
        }
    }
}

// OFFSET: 0x00849830/0x140577920
CGxShader* CGxDevice::ShaderCreate(EGxShTarget target, const char* name) {
    TSHashTable<CGxShader, HASHKEY_STRI>& list = CGxDevice::s_shaderList[target];
    CGxShader* shader = list.Ptr(name);

    if (shader) {
        shader->m_refCount++;
        return shader;
    }

    shader = list.New(name, 0, 0);
    shader->m_refCount++;
    shader->m_target = target;

    if (this->IShaderLoad(shader, target, name)) {
        return shader;
    }

    list.Delete(shader);
    return NULL;
}

// OFFSET: 0x00843BC0/0x1405700C0
void CGxDevice::ChooseShaderModel(int32_t shaderModel) {
    while (true) {
        for (int32_t model = 7; model >= 0; model--) {
            if (shaderModel != -1 && model > shaderModel) {
                continue;
            }

            const int32_t* const* sets = s_shaderModelProfiles[model];

            for (const int32_t* set = *sets; set; set = *++sets) {
                const int32_t* profile = set;

                while (*profile && this->m_caps.m_shaderProfiles[*profile]) {
                    profile++;
                }

                if (*profile) {
                    continue;
                }

                CGxDevice::Log("Shader Model: %s", CGxDevice::s_shaderModelNames[model]);
                this->m_caps.m_shaderModel = model;

                for (profile = set; *profile; profile++) {
                    EGxShTarget profileTarget = CGxDevice::s_shaderProfileTarget[*profile];
                    this->m_caps.m_shaderTargets[profileTarget] = *profile;
                    CGxDevice::Log(" %s: %s", CGxDevice::s_shaderTargetNames[profileTarget], CGxDevice::s_shaderProfileNames[*profile]);
                }

                return;
            }
        }

        if (shaderModel == -1) {
            return;
        }

        shaderModel = -1;
    }
}

// PROVEN: f9f1b9a4 IDENTICAL/NONE
// OFFSET: 0x008421C0
void CGxDevice::TexFlags(const CGxTex* texId, CGxTexFlags& flags) {
    flags = texId->m_flags;
}

// OFFSET: 0x00842D70/0x14056F1C0
int32_t CGxDevice::AdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes) {
    EGxApi api = g_theGxDevicePtr ? g_theGxDevicePtr->m_api : GxGetRequestedApi();

    switch (api) {
        case GxApi_OpenGl:
        case GxApi_D3d9:
            return CGxDevice::WinAdapterMonitorModes(monitor, modes);

        case GxApi_D3d11:
            return CGxDevice::D3d11AdapterMonitorModes(monitor, modes);

        default:
            return 0;
    }
}

// PROVEN: f80de932 IDENTICAL/NONE
// OFFSET: 0x00841730
void CGxDevice::ApiQuery(EGxApi api, int32_t& supported, int32_t& selectable) {
    supported = 0;
    selectable = 0;

    switch (api) {
        case GxApi_OpenGl:
            CGxDevice::OpenGlApiQuery(supported, selectable);
            break;

        case GxApi_D3d9:
            supported = 1;
            selectable = 1;
            break;

        case GxApi_D3d11:
            CGxDevice::D3d11ApiQuery(supported, selectable);
            break;

        default:
            break;
    }
}

// OFFSET: 0x00842DB0/0x14056F230
int32_t CGxDevice::AdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode) {
    EGxApi api = g_theGxDevicePtr ? g_theGxDevicePtr->m_api : GxGetRequestedApi();

    switch (api) {
        case GxApi_OpenGl:
        case GxApi_D3d9:
            return CGxDevice::WinAdapterDesktopMode(monitor, mode);

        case GxApi_D3d11:
            return CGxDevice::D3d11AdapterDesktopMode(monitor, mode);

        default:
            return 0;
    }
}

// OFFSET: 0x00843B60
void CGxDevice::TexSetFilterMode(EGxTexFilter filter, uint32_t maxAnisotropy) {
    for (CGxTex* texId = CGxDevice::s_textureList.Head(); texId; texId = CGxDevice::s_textureList.Link(texId)->Next()) {
        if (texId->m_flags.m_bit19) {
            texId->m_flags.m_filter = filter;
            texId->m_flags.m_maxAnisotropy = maxAnisotropy;
        }
    }
}

// PROVEN: 9954181c IDENTICAL/UNPROVEN
// OFFSET: 0x00843D90/0x1405702F0
uint32_t CGxDevice::AdapterMonitorCount() {
    if (!CGxDevice::s_monitorsInit) {
        CGxDevice::AdapterInitMonitors();
    }

    return CGxDevice::s_monitors.Count();
}

// OFFSET: 0x00843DB0/0x140570310
void* CGxDevice::AdapterGetMonitorHandle(uint32_t monitor) {
    if (!CGxDevice::s_monitorsInit) {
        CGxDevice::AdapterInitMonitors();
    }

    return monitor < CGxDevice::s_monitors.Count() ? CGxDevice::s_monitors[monitor] : NULL;
}

// OFFSET: 0x00844EF0/0x140571D50
int32_t CGxDevice::AdapterFormats(TSGrowableArray<CGxFormat>& formats) {
    formats.SetCount(0);
    formats.m_chunk = 64;
    formats.Reserve(256, 0);

    EGxApi api = g_theGxDevicePtr ? g_theGxDevicePtr->m_api : GxGetRequestedApi();

    switch (api) {
        case GxApi_OpenGl:
            CGxDevice::OpenGlAdapterFormats(formats);
            break;

        case GxApi_D3d9:
            CGxDevice::D3dAdapterFormats(formats);
            break;

        case GxApi_D3d11:
            CGxDevice::D3d11AdapterFormats(formats);
            break;

        default:
            break;
    }

    GxLog("GxAdapterFormats():");

    for (uint32_t i = 0; i < formats.Count(); i++) {
        GxLog("\t%i: cf = %d, df = %d, ms = %d", i, formats[i].m_colorFormat, formats[i].m_depthFormat, formats[i].m_multisampleCount);
    }

    return formats.Count() != 0;
}

// PROVEN: 8b253a62 IDENTICAL/IDENTICAL
// OFFSET: 0x00841780/0x14056DB00
int32_t CGxDevice::AdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo) {
    return CGxDevice::DeviceAdapterID(vendorID, deviceID, driverVersionHi, driverVersionLo) != 0;
}

// OFFSET: 0x008417B0/0x14056DB20
int32_t CGxDevice::AdapterInfer(uint16_t& deviceID) {
    return CGxDevice::D3d11AdapterInfer(deviceID) || CGxDevice::DeviceAdapterInfer(deviceID) != 0;
}

// PROVEN: 02beb802 IDENTICAL/IDENTICAL
// OFFSET: 0x00844E60/0x140571C60
void CGxDevice::AdapterResetMonitors() {
    CGxDevice::s_monitorsInit = 0;
    CGxDevice::s_monitors.m_count = 0;
}
