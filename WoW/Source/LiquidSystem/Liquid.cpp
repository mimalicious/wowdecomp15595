#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00E4EADC
static int32_t s_cameraRelativeCoords;

// STATIC: 0x00D00634
static float s_worldCoordScale = 1.0f;

// STATIC: 0x00D00638
static int32_t s_shadersEnabled = 1;

// STATIC: 0x00D0063C
static int32_t s_materialsDirty = 1;

// STATIC: 0x00E4EAFC
static int32_t s_configDirty;

// STATIC: 0x00E4EAE0
static uint32_t s_waterDetail;

// STATIC: 0x00E4EAE4
static uint32_t s_rippleDetail;

// STATIC: 0x00E4EAE8
static uint32_t s_reflectionMode;

// STATIC: 0x00E4EAEC
static uint32_t s_reflectionDownscale;

// STATIC: 0x00E4EAF0
static int32_t s_cameraSubmerged;

// STATIC: 0x00E4EAF4
static int32_t s_needBackBuffer;

// STATIC: 0x00E4EAF8
static int32_t s_needDynDisplMap;

// STATIC: 0x00E4EB00
static float s_underWaterFogStart;

// STATIC: 0x00E4EB04
static float s_underWaterFogEnd;

// STATIC: 0x00E4EB08
static float s_underWaterFogDensity;

// STATIC: 0x00E4EB18
static CImVector s_underWaterFogColor;

// STATIC: 0x00E4EB0C
static float s_aboveWaterFogStart;

// STATIC: 0x00E4EB10
static float s_aboveWaterFogEnd;

// STATIC: 0x00E4EB14
static float s_aboveWaterFogDensity;

// STATIC: 0x00E4EB1C
static CImVector s_aboveWaterFogColor;

// STATIC: 0x00E4EB20
static TSGrowableArray<Liquid::CONFIG_CHANGED_CALLBACK> s_configChangedCallbacks;

// STATIC: 0x00E4EB30
WaterSurfaceGen* Liquid::s_surfaceGen;

// STATIC: 0x00E4EB7C
CDataAllocator Liquid::s_instanceAllocator(sizeof(Liquid::CInstance), 32);

// OFFSET: 0x0071ACF0
void Liquid::SignalConfigChanged(int32_t materials) {
    int32_t wasDirty = s_configDirty;
    s_configDirty = 1;

    if (!wasDirty) {
        s_materialsDirty = materials;
    } else {
        s_materialsDirty |= materials;
    }
}

// OFFSET: 0x0071AD20/0x1403EF450
void Liquid::SetUnderWaterFog(float start, float end, float density, const CImVector& color) {
    s_underWaterFogStart = start;
    s_underWaterFogEnd = end;
    s_underWaterFogDensity = density;
    s_underWaterFogColor = color;
}

// OFFSET: 0x0071AD60/0x1403EF480
void Liquid::GetUnderWaterFog(float& start, float& end, float& density, CImVector& color) {
    start = s_underWaterFogStart;
    end = s_underWaterFogEnd;
    density = s_underWaterFogDensity;
    color = s_underWaterFogColor;
}

// OFFSET: 0x0071ADA0/0x1403EF4B0
void Liquid::SetAboveWaterFog(float start, float end, float density, const CImVector& color) {
    s_aboveWaterFogStart = start;
    s_aboveWaterFogEnd = end;
    s_aboveWaterFogDensity = density;
    s_aboveWaterFogColor = color;
}

// OFFSET: 0x0071ADE0
void Liquid::GetAboveWaterFog(float& start, float& end, float& density, CImVector& color) {
    start = s_aboveWaterFogStart;
    end = s_aboveWaterFogEnd;
    density = s_aboveWaterFogDensity;
    color = s_aboveWaterFogColor;
}

// PROVEN: c9f9370b IDENTICAL/NONE
// OFFSET: 0x0071AE20/0x1403EF290
uint32_t Liquid::GetWaterDetail() {
    return s_waterDetail;
}

// PROVEN: 1b32f54a IDENTICAL/NONE
// OFFSET: 0x0071AE30/0x1403EF2A0
uint32_t Liquid::GetRippleDetail() {
    return s_rippleDetail;
}

// PROVEN: af6e86d2 IDENTICAL/NONE
// OFFSET: 0x0071AE40/0x1403EF2B0
uint32_t Liquid::GetReflectionMode() {
    return s_reflectionMode;
}

// PROVEN: 3dd13e28 IDENTICAL/NONE
// OFFSET: 0x0071AE50
uint32_t Liquid::GetReflectionDownscale() {
    return s_reflectionDownscale;
}

// PROVEN: cfbb7e77 IDENTICAL/NONE
// OFFSET: 0x0071AE60/0x1403EF2D0
int32_t Liquid::WaterDetailNeedsDepthCache(uint32_t detail) {
    return detail >= 2;
}

// PROVEN: 191c7132 IDENTICAL/NONE
// OFFSET: 0x0071AE70/0x1403EF2E0
int32_t Liquid::CameraSubmerged() {
    return s_cameraSubmerged;
}

// PROVEN: b651f811 IDENTICAL/NONE
// OFFSET: 0x0071AE80/0x1403EF2F0
void Liquid::SetCameraSubmerged(int32_t submerged) {
    s_cameraSubmerged = submerged;
}

// PROVEN: abc6bfc0 IDENTICAL/NONE
// OFFSET: 0x0071AE90/0x1403EF300
int32_t Liquid::NeedBackBuffer() {
    return s_needBackBuffer;
}

// PROVEN: 5f61cc36 IDENTICAL/NONE
// OFFSET: 0x0071AEA0/0x1403EF310
void Liquid::SetNeedBackBuffer(int32_t need) {
    s_needBackBuffer = need;
}

// PROVEN: ea9b8ac0 IDENTICAL/NONE
// OFFSET: 0x0071AEB0/0x1403EF320
int32_t Liquid::NeedDynDisplMap() {
    return s_needDynDisplMap;
}

// PROVEN: ed575564 IDENTICAL/NONE
// OFFSET: 0x0071AEC0/0x1403EF330
void Liquid::SetNeedDynDisplMap(int32_t need) {
    s_needDynDisplMap = need;
}

// PROVEN: d45ce39a IDENTICAL/NONE
// OFFSET: 0x0071AED0/0x1403EF340
int32_t Liquid::GetShadersEnabled() {
    return s_shadersEnabled;
}

// PROVEN: 1383d3e2 IDENTICAL/NONE
// OFFSET: 0x0071AEE0/0x1403EF350
int32_t Liquid::UseCameraRelativeCoords() {
    return s_cameraRelativeCoords;
}

// PROVEN: f46b1b37 IDENTICAL/NONE
// OFFSET: 0x0071AEF0/0x1403EF360
float Liquid::WorldCoordScale() {
    return s_worldCoordScale;
}

// OFFSET: 0x0071AF00
void Liquid::Destroy() {
    Liquid::CMaterialBank::Destroy();
    Liquid::CSettingsBank::Destroy();
    Liquid::CInstance::Destroy();
    Liquid::DestroySurfaceGen();
    Liquid::ClearPerturbatorCache();

    if (s_waterDetail > 1) {
        DepthCache::Release();
    }
}

// OFFSET: 0x0071AF30/0x1403EF3A0
int32_t Liquid::IsAllLoaded(uint32_t liquidType) {
    IMaterial* material = Liquid::CMaterialBank::GetMaterial(liquidType);
    return material->IsAllLoaded(Liquid::CSettingsBank::GetSettings(liquidType));
}

// OFFSET: 0x0071AF60/0x1403EF3E0
void Liquid::LinkToAsyncHead(uint32_t liquidType) {
    IMaterial* material = Liquid::CMaterialBank::GetMaterial(liquidType);
    material->LinkToAsyncHead(Liquid::CSettingsBank::GetSettings(liquidType));
}

// OFFSET: 0x0071AF90/0x1403EF420
int32_t Liquid::IsTransparent(uint32_t liquidType) {
    return Liquid::CSettingsBank::GetSettings(liquidType)->m_transparent;
}

// PROVEN: b9bf9c00 IDENTICAL/NONE
// OFFSET: 0x0071AFB0/0x1403EF440
void Liquid::PurgeMaterialBank() {
    Liquid::CMaterialBank::Purge();
}

// OFFSET: 0x0071AFC0/0x1403EF510
uint32_t Liquid::ValidateWaterDetail(uint32_t detail) {
    if (!detail) {
        return 0;
    }

    if (detail > 3) {
        return 2;
    }

    CGxDevice* device = g_theGxDevicePtr;

    if (detail == 1) {
        if (device->m_caps.m_shaderModel < 4) {
            return 64;
        }
    } else {
        if (device->m_caps.m_shaderModel < 5) {
            return 128;
        }

        if (!DepthCache::IsSupported(NULL, NULL, NULL)) {
            return 8;
        }

        device = g_theGxDevicePtr;
    }

    if (device->m_caps.m_texFmt[GxTex_Gr1616F] != 1 || !device->m_caps.m_texTarget[GxTex_NonPow2]) {
        return 8;
    }

    return device->m_caps.m_texTarget[GxTex_Volume] != 0 ? 0 : 8;
}

// OFFSET: 0x0071B050/0x1403EF5B0
int32_t Liquid::SetWaterDetail(uint32_t detail) {
    if (Liquid::ValidateWaterDetail(detail)) {
        return 0;
    }

    s_materialsDirty = 1;
    s_configDirty |= detail != s_waterDetail;

    if ((s_waterDetail >= 2) != (detail >= 2)) {
        if (detail >= 2) {
            DepthCache::AddRef();
            s_waterDetail = detail;
            return 1;
        }

        DepthCache::Release();
    }

    s_waterDetail = detail;
    return 1;
}

// PROVEN: a2627f67 IDENTICAL/NONE
// OFFSET: 0x0071B0C0/0x1403EF630
uint32_t Liquid::ValidateRippleDetail(uint32_t detail) {
    if (!detail) {
        return detail;
    }

    if (detail > 2) {
        return 2;
    }

    if (g_theGxDevicePtr->m_caps.m_shaderModel < 5) {
        return 128;
    }

    return g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Gr1616F] != 1 ? 8 : 0;
}

// OFFSET: 0x0071B100/0x1403EF670
int32_t Liquid::SetRippleDetail(uint32_t detail) {
    if (detail && (detail > 2 || g_theGxDevicePtr->m_caps.m_shaderModel < 5 || g_theGxDevicePtr->m_caps.m_texFmt[GxTex_Gr1616F] != 1)) {
        return 0;
    }

    int32_t same = detail == s_rippleDetail;
    s_rippleDetail = detail;
    s_configDirty |= !same;
    return 1;
}

// PROVEN: c74a57a9 IDENTICAL/NONE
// OFFSET: 0x0071B150/0x1403EF6C0
uint32_t Liquid::ValidateReflectionMode(uint32_t mode) {
    if (!mode) {
        return mode;
    }

    if (mode > 3) {
        return 2;
    }

    return g_theGxDevicePtr->m_caps.m_shaderModel >= 5 ? 0 : 0x80;
}

// OFFSET: 0x0071B190/0x1403EF6F0
int32_t Liquid::SetReflectionMode(uint32_t mode) {
    if (mode && (mode > 3 || g_theGxDevicePtr->m_caps.m_shaderModel < 5)) {
        return 0;
    }

    int32_t same = mode == s_reflectionMode;
    s_reflectionMode = mode;
    s_configDirty |= !same;
    return 1;
}

// PROVEN: a3b4a00d IDENTICAL/NONE
// OFFSET: 0x0071B1D0/0x1403EF730
uint32_t Liquid::ValidateReflectionDownscale(uint32_t downscale) {
    if (!downscale) {
        return downscale;
    }

    if (downscale > 4) {
        return 2;
    }

    return g_theGxDevicePtr->m_caps.m_texTarget[GxTex_NonPow2] != 0 ? 0 : 8;
}

// OFFSET: 0x0071B200/0x1403EF760
int32_t Liquid::SetReflectionDownscale(uint32_t downscale) {
    if (downscale && (downscale > 4 || !g_theGxDevicePtr->m_caps.m_texTarget[GxTex_NonPow2])) {
        return 0;
    }

    int32_t same = downscale == s_reflectionDownscale;
    s_reflectionDownscale = downscale;
    s_configDirty |= !same;
    return 1;
}

// OFFSET: 0x0071B240/0x1403EF7A0
int32_t Liquid::Initialize(int32_t cameraRelativeCoords, float worldCoordScale) {
    s_cameraRelativeCoords = cameraRelativeCoords;
    s_worldCoordScale = worldCoordScale;
    s_configDirty |= CShaderEffect::s_enableShaders != s_shadersEnabled;
    s_shadersEnabled = CShaderEffect::s_enableShaders;
    Liquid::CInstance::Initialize();
    Liquid::CSettingsBank::Create();
    Liquid::CMaterialBank::Create();
    s_configDirty = 0;
    Liquid::InitLiquidObjects();
    return 1;
}

// OFFSET: 0x0071B2A0/0x1403EF800
void Liquid::Update() {
    if (s_configDirty) {
        if (s_materialsDirty) {
            Liquid::CMaterialBank::ResetMaterials();
        }

        for (uint32_t i = 0; i < s_configChangedCallbacks.Count(); i++) {
            s_configChangedCallbacks[i]();
        }

        Liquid::DestroyProceduralTextures(s_materialsDirty);

        if (s_materialsDirty) {
            Liquid::CMaterialBank::Purge();
            Liquid::CSettingsBank::Purge();
        }

        Liquid::CreateProceduralTextures(s_materialsDirty);
        s_configDirty = 0;
        s_materialsDirty = 1;
    }

    s_needBackBuffer = 0;
    s_needDynDisplMap = 0;
    Liquid::s_oceanDepthTexValid = 0;
    Liquid::s_riverDepthTexValid = 0;
    Liquid::s_wmoWaterTexValid = 0;
}

// OFFSET: 0x0071B340
Liquid::CInstance* Liquid::CreateInstance(uint32_t a1, uint32_t liquidType, IGeomFactory* factory) {
    CInstance* instance = Liquid::CInstance::Create();
    instance->m_material = Liquid::CMaterialBank::GetMaterial(liquidType);
    instance->m_settings = Liquid::CSettingsBank::GetSettings(liquidType);
    instance->m_geomFactory = factory;

    LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(liquidType);

    if (!rec) {
        return NULL;
    }

    Object* object = Liquid::RegisterLiquidObject(a1, liquidType, rec->m_materialID);

    if (object->m_objectId && object->m_refCount == 1 && object->m_reflectionType == 1) {
        object->SetReflectionPlane(factory->GetHeight());
    }

    instance->m_object = object;
    return instance;
}

// OFFSET: 0x0071B3E0
void Liquid::AddConfigChangedCallback(CONFIG_CHANGED_CALLBACK callback) {
    s_configChangedCallbacks.New(callback);
}

// OFFSET: 0x0071B400
void Liquid::RemoveConfigChangedCallback(CONFIG_CHANGED_CALLBACK callback) {
    uint32_t count = s_configChangedCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (s_configChangedCallbacks[i] == callback) {
            s_configChangedCallbacks[i] = s_configChangedCallbacks[s_configChangedCallbacks.Count() - 1];
            s_configChangedCallbacks.SetCount(count - 1);
            return;
        }
    }
}

// OFFSET: 0x0071B490
void Liquid::DestroySurfaceGen() {
    WaterSurfaceGen* surfaceGen = s_surfaceGen;

    if (surfaceGen) {
        surfaceGen->~WaterSurfaceGen();
        SMemFree(surfaceGen, "delete", -1, 0);
    }

    s_surfaceGen = NULL;
}

// PROVEN: 86e54946 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Liquid::UpdateSurfaceGen(uint32_t time) {
}


// PROVEN: 9a41ca3e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Liquid::CInstance::Initialize() {
}

// PROVEN: 90ee0c9d IDENTICAL/NONE
// OFFSET: 0x0071BDF0
void Liquid::CInstance::AddRef() {
    this->m_refCount++;
}

// PROVEN: 259e5a12 IDENTICAL/NONE
// OFFSET: 0x0071BE00/0x1403F0B50
void Liquid::CInstance::Purge() {
    this->m_geomFactory->Purge();
}

// OFFSET: 0x0071BE10
void Liquid::CInstance::Destroy() {
    Liquid::s_instanceAllocator.Clear(typeid(Liquid::CInstance).raw_name(), SLOG_OBJECT, 1);
}

// OFFSET: 0x0071BE30
Liquid::CInstance::CInstance() {
    this->m_material = NULL;
    this->m_settings = NULL;
    this->m_geomFactory = NULL;
    this->m_environment = NULL;
    this->m_object = NULL;
    this->m_transform = C44Matrix();
    this->m_bounds.c = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_bounds.r = 0.0f;
    this->unk_0064 = 0;
    this->m_isMesh = 0;
    this->m_refCount = 1;
}

// OFFSET: 0x0071BEE0
Liquid::CInstance* Liquid::CInstance::Create() {
    return Liquid::s_instanceAllocator.New<Liquid::CInstance>(0);
}

// OFFSET: 0x0071BF10/0x1403F0C80
void Liquid::CInstance::Release() {
    if (this->m_refCount-- == 1) {
        this->m_geomFactory->Release();
        this->m_environment->Release();
        Liquid::UnregisterLiquidObject(this->m_object);
        Liquid::s_instanceAllocator.PutData(this, NULL, 0);
    }
}

// PROVEN: 02c0d807 IDENTICAL/NONE
// OFFSET: 0x0071D8D0
void WaterSurfaceGen::Release() {
    if (this->unk_0028) {
        SMemFree(this->unk_0028, "delete[]", -1, 0);
    }

    SIMD::FreeA16(this->m_alignedBuffers[0]);
    SIMD::FreeA16(this->m_alignedBuffers[1]);
    SIMD::FreeA16(this->m_alignedBuffers[2]);
    SIMD::FreeA16(this->m_alignedBuffers[3]);
    SIMD::FreeA16(this->m_alignedBuffers[4]);

    if (this->unk_0020) {
        SMemFree(this->unk_0020, "delete[]", -1, 0);
    }

    if (this->unk_0024) {
        SMemFree(this->unk_0024, "delete[]", -1, 0);
    }

    this->m_indices.Clear();

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = NULL;
}

// PROVEN: a59e00a4 IDENTICAL/NONE
// OFFSET: 0x0071D9A0
WaterSurfaceGen::~WaterSurfaceGen() {
    this->Release();
}
