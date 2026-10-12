#include "WoW/Source/WorldClient/WorldParam.h"
#include <cstdio>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/GfxSingletonManager.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Common/AaBsp.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00E8901C/0x140D74C58
CVar* CWorldParam::cvar_minimapPortalMax;

// STATIC: 0x00E89020/0x140D74C60
CVar* CWorldParam::cvar_textureFilteringMode;

// STATIC: 0x00E89024/0x140D74C68
CVar* CWorldParam::cvar_skyCloudLOD;

// STATIC: 0x00E89028/0x140D74C70
CVar* CWorldParam::cvar_preloadLoadingDistObject;

// STATIC: 0x00E8902C/0x140D74C78
CVar* CWorldParam::cvar_preloadLoadingDistTerrain;

// STATIC: 0x00E89030/0x140D74C80
CVar* CWorldParam::cvar_preloadStreamingDistObject;

// STATIC: 0x00E89034/0x140D74C88
CVar* CWorldParam::cvar_preloadStreamingDistTerrain;

// STATIC: 0x00E89038/0x140D74C90
CVar* CWorldParam::cvar_shadowTextureSize;

// STATIC: 0x00E8903C/0x140D74C98
CVar* CWorldParam::cvar_shadowMode;

// STATIC: 0x00E89040/0x140D74CA0
CVar* CWorldParam::cvar_detailDoodadInstancing;

// STATIC: 0x00E89044/0x140D74CA8
CVar* CWorldParam::cvar_gxTextureCacheSize;

// STATIC: 0x00E89048/0x140D74CB0
CVar* CWorldParam::cvar_projectedTextures;

// STATIC: 0x00E8904C/0x140D74CB8
CVar* CWorldParam::cvar_hwPCF;

// STATIC: 0x00E89050/0x140D74CC0
CVar* CWorldParam::cvar_environmentDetail;

// STATIC: 0x00E89054/0x140D74CC8
CVar* CWorldParam::cvar_objectFadeZFill;

// STATIC: 0x00E89058/0x140D74CD0
CVar* CWorldParam::cvar_objectFade;

// STATIC: 0x00E8905C/0x140D74CD8
CVar* CWorldParam::cvar_terrainAlphaBitDepth;

// STATIC: 0x00E89060/0x140D74CE0
CVar* CWorldParam::cvar_groundEffectDist;

// STATIC: 0x00E89064/0x140D74CE8
CVar* CWorldParam::cvar_groundEffectFade;

// STATIC: 0x00E89068/0x140D74CF0
CVar* CWorldParam::cvar_groundEffectDensity;

// STATIC: 0x00E8906C/0x140D74CF8
CVar* CWorldParam::cvar_worldPoolUsage;

// STATIC: 0x00E89070/0x140D74D00
CVar* CWorldParam::cvar_bspCache;

// STATIC: 0x00E89074/0x140D74D08
CVar* CWorldParam::cvar_horizonNearClipScale;

// STATIC: 0x00E89078/0x140D74D10
CVar* CWorldParam::cvar_horizonFarClipScale;

// STATIC: 0x00E8907C/0x140D74D18
CVar* CWorldParam::cvar_baseMip;

// STATIC: 0x00E89080/0x140D74D20
CVar* CWorldParam::cvar_sunShafts;

// STATIC: 0x00E89084/0x140D74D28
CVar* CWorldParam::cvar_reflectionDownscale;

// STATIC: 0x00E89088/0x140D74D30
CVar* CWorldParam::cvar_reflectionMode;

// STATIC: 0x00E8908C/0x140D74D38
CVar* CWorldParam::cvar_rippleDetail;

// STATIC: 0x00E89090/0x140D74D40
CVar* CWorldParam::cvar_waterDetail;

// STATIC: 0x00E89098/0x140D74D50
CVar* CWorldParam::cvar_particleDensity;

// STATIC: 0x00E8909C/0x140D74D58
CVar* CWorldParam::cvar_nearClip;

// STATIC: 0x00E890A0/0x140D74D60
CVar* CWorldParam::cvar_farClip;

// STATIC: 0x00E890A4/0x140D74D68
CVar* CWorldParam::cvar_terrainMipLevel;

// OFFSET: 0x0076B620/0x140453710
bool CWorldParam::TerrainMipLevelCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t level = SStrToInt(value);

    if (level > 1) {
        ConsoleWrite("Terrain mip level must be in range 0 - 1.", DEFAULT_COLOR);
        return false;
    }

    ConsoleWriteA("Terrain mip level changed to %d.", DEFAULT_COLOR, level);
    return true;
}

// OFFSET: 0x0076B660/0x140453750
bool CWorldParam::FarClipCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    World::SetFarClip(SStrToFloat(value));
    return true;
}

// OFFSET: 0x0076B680/0x140453770
bool CWorldParam::NearClipCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    World::SetNearClip(SStrToFloat(value));
    return true;
}

// OFFSET: 0x0076B6A0/0x140453790
bool CWorldParam::HorizonFarClipScaleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    float scale = SStrToFloat(value);

    if (scale < 3.0f) {
        scale = 3.0f;
    } else if (scale > 6.0f) {
        scale = 6.0f;
    }

    World::SetHorizonFarClipScale(scale);
    return true;
}

// OFFSET: 0x0076B700/0x1404537D0
bool CWorldParam::HorizonNearClipScaleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    float scale = SStrToFloat(value);

    if (scale < 0.0099999998f) {
        scale = 0.0099999998f;
    } else if (scale > 1.0f) {
        scale = 1.0f;
    }

    World::SetHorizonNearClipScale(scale);
    return true;
}

// OFFSET: 0x0076B760/0x140453810
bool CWorldParam::ParticleDensityCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t density = SStrToInt(value);

    if (static_cast<uint32_t>(density - 10) <= 90) {
        ParticleSystemManager::SetScaler(static_cast<float>(density) * 0.0099999998f);
        return true;
    }

    ConsoleWrite("Value must be between 10 and 100.", DEFAULT_COLOR);
    return false;
}

// OFFSET: 0x0076B7B0/0x140453860
bool CWorldParam::WaterDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t detail = SStrToInt(value);

    if (!Liquid::SetWaterDetail(detail)) {
        ConsoleWrite("Unsupported water detail mode.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Water detail changed to %d", detail);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076B820/0x1404538D0
bool CWorldParam::RippleDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t detail = SStrToInt(value);

    if (!Liquid::SetRippleDetail(detail)) {
        ConsoleWrite("Unsupported ripple detail mode.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Ripple detail changed to %d", detail);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076B890/0x140453940
bool CWorldParam::ReflectionModeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t mode = SStrToInt(value);

    if (!Liquid::SetReflectionMode(mode)) {
        ConsoleWrite("Unsupported reflection mode (0 = screen space, 1 = sky, 2 = sky/terrain, 3 = sky/terrain/wmo).", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Reflection mode changed to %d", mode);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076B900/0x1404539B0
bool CWorldParam::ReflectionDownscaleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t downscale = SStrToInt(value);

    if (!Liquid::SetReflectionDownscale(downscale)) {
        ConsoleWrite("Unsupported ripple downscale (choose a value from 0-4).", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Reflection downscale changed to %d", downscale);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076B970/0x140453A20
bool CWorldParam::SunShaftsCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t mode = SStrToInt(value);

    if (!SunShafts::SetMode(mode)) {
        ConsoleWrite("Unsupported sunshafts detail mode.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Sunshafts quality changed to %d", mode);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076B9E0/0x140453A90
bool CWorldParam::WorldPoolUsageCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (!SStrCmpI(value, "dynamic", STORM_MAX_STR)) {
        ConsoleWrite("WorldPoolUsage set on restart.", DEFAULT_COLOR);
        return true;
    }

    ConsoleWrite("WorldPoolUsage must be stream, dynamic or static.", DEFAULT_COLOR);
    return false;
}

// OFFSET: 0x0076BA20/0x140453AD0
bool CWorldParam::TerrainAlphaBitDepthCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t depth = SStrToInt(value);

    if (depth == 4 || depth == 8) {
        char text[256];
        sprintf(text, "Alpha map bit depth set to %dbit on restart.", depth);
        ConsoleWrite(text, DEFAULT_COLOR);
        return true;
    }

    ConsoleWrite("Alpha map bit depth must be 4 or 8.", DEFAULT_COLOR);
    return false;
}

// OFFSET: 0x0076BA80/0x140453B30
bool CWorldParam::GroundEffectFadeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    World::SetDetailDoodadDensityFade(static_cast<float>(static_cast<uint32_t>(SStrToInt(value))) * 0.0099999998f);
    return true;
}

// OFFSET: 0x0076BAC0/0x140453B60
bool CWorldParam::ObjectFadeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);

    if (enable) {
        ConsoleWrite("Object distance fade enabled.", DEFAULT_COLOR);
    } else {
        ConsoleWrite("Object distance fade disabled.", DEFAULT_COLOR);
    }

    World::SetEnables(0x2000, enable);
    return true;
}

// OFFSET: 0x0076BB00/0x140453BA0
bool CWorldParam::ObjectFadeZFillCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);

    if (enable) {
        ConsoleWrite("Object distance fade ZFill pass enabled.", DEFAULT_COLOR);
    } else {
        ConsoleWrite("Object distance fade ZFill pass disabled.", DEFAULT_COLOR);
    }

    World::SetEnables(0x4000, enable);
    return true;
}

// OFFSET: 0x0076BB40/0x140453BE0
bool CWorldParam::EnvironmentDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t detail = SStrToInt(value);

    if (detail < 50) {
        detail = 50;
    } else if (detail > 150) {
        detail = 150;
    }

    CWorldView::ScaleFadeDist(static_cast<float>(detail) * 0.0099999998f);
    return true;
}

// OFFSET: 0x0076BB90/0x140453C20
bool CWorldParam::ShadowModeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t mode = SStrToInt(value);

    if (!CShadowCache::SetShadowMode(static_cast<EShadowMode>(mode))) {
        ConsoleWrite("Unsupported shadow mode.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Shadow mode changed to %s", CShadowCache::GetShadowModeDesc(mode));
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076BC00/0x140453C90
bool CWorldParam::ShadowTextureSizeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t size = SStrToInt(value);

    if (!CShadowCache::SetTextureSize(size)) {
        ConsoleWrite("Unsupported shadow texture size.", DEFAULT_COLOR);
        return false;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), "Shadow texture size changed to %d.", size);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076BC70/0x140453D00
bool CWorldParam::ProjectedTexturesCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);

    if (enable) {
        ConsoleWrite("Projected textures enabled.", DEFAULT_COLOR);
    } else {
        ConsoleWrite("Projected textures disabled.", DEFAULT_COLOR);
    }

    World::SetEnables(0x800, enable);
    return true;
}

// OFFSET: 0x0076BCB0/0x140453D40
bool CWorldParam::DetailDoodadInstancingCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    bool instanced = SStrToInt(value) != 0;
    CDetailDoodad::SetInstanced(instanced);

    if (CDetailDoodad::IsInstanced()) {
        ConsoleWrite("Detail doodad instancing enabled.", DEFAULT_COLOR);
    } else {
        ConsoleWrite("Detail doodad instancing disabled.", DEFAULT_COLOR);
    }

    return true;
}

// OFFSET: 0x0076BD00/0x140453D80
bool CWorldParam::CloudLODCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t lod = SStrToInt(value);

    if (lod < 0) {
        lod = 0;
    } else if (lod > 3) {
        lod = 3;
    }

    DayNight::SetCloudLOD(lod);

    char text[64];
    SStrPrintf(text, sizeof(text), "SkyCloudLOD set to %i", lod);
    ConsoleWrite(text, DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x0076BD60/0x140453DE0
bool CWorldParam::TextureFilteringCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t mode = SStrToInt(value);

    if (mode > 5) {
        char text[256];
        SStrPrintf(text, sizeof(text), "Texture filtering mode must be in range 0 to %d.", 6);
        ConsoleWrite(text, DEFAULT_COLOR);
        return false;
    }

    TextureSetFilterMode(CWorldParam::s_textureFilteringModes[mode]);
    TextureSetMaxAnisotropy(CWorldParam::s_textureFilteringAnisotropy[mode]);
    GxTexSetFilterMode(TextureGetFilterMode(), TextureGetMaxAnisotropy());
    return true;
}

// OFFSET: 0x0076BDF0/0x140453E90
bool CWorldParam::BaseMipCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t level = SStrToInt(value);

    if (level == g_theGxDevicePtr->DeviceBaseMipLevel()) {
        return true;
    }

    if (level <= 1) {
        g_theGxDevicePtr->DeviceSetBaseMipLevel(level);
        TextureNotifyGxRestart();

        char text[256];
        SStrPrintf(text, sizeof(text), "BaseMip level changed to %d", level);
        ConsoleWrite(text, DEFAULT_COLOR);
        return true;
    }

    ConsoleWrite("BaseMip must be 0 or 1", DEFAULT_COLOR);
    return false;
}

// OFFSET: 0x0076BE80/0x140453F20
bool CWorldParam::BspCacheCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (SStrToInt(value)) {
        if (g_BspDigestCache) {
            ConsoleWrite("Enabling BSP node cache (already enabled, so clearing content.)", DEFAULT_COLOR);
            g_BspDigestCache->Reset();
            return true;
        }

        ConsoleWrite("Enabling BSP node cache (first time - starting up)", DEFAULT_COLOR);
        void* mem = SMemAlloc(sizeof(CAaBspDigestCache), __FILE__, __LINE__, 0);
        g_BspDigestCache = mem ? new (mem) CAaBspDigestCache() : NULL;
        return true;
    }

    if (!g_BspDigestCache) {
        ConsoleWrite("BSP node cache already disabled.", DEFAULT_COLOR);
        return true;
    }

    ConsoleWrite("Disabling BSP node cache.", DEFAULT_COLOR);
    CAaBspDigestCache* cache = g_BspDigestCache;

    if (cache) {
        cache->~CAaBspDigestCache();
        SMemFree(cache, "delete", -1, 0);
    }

    g_BspDigestCache = NULL;
    return true;
}

// OFFSET: 0x0076BF60/0x140454030
bool CWorldParam::GroundEffectDensityCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t minDensity;
    uint32_t maxDensity;
    CDetailDoodad::GetMinMaxDensity(minDensity, maxDensity);

    uint32_t density = SStrToInt(value);
    uint32_t range = maxDensity - minDensity;

    if (density - minDensity > range) {
        density = minDensity + (range & ~(static_cast<int32_t>(density - minDensity) >> 31));
    }

    World::SetDetailDoodadDensity(density);
    return true;
}

// OFFSET: 0x0076BFB0/0x140454090
bool CWorldParam::GroundEffectDistCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    float minDist;
    float maxDist;
    CDetailDoodad::GetMinMaxDist(minDist, maxDist);

    float dist = SStrToFloat(value);

    if (minDist > dist) {
        dist = minDist;
    } else if (maxDist < dist) {
        dist = maxDist;
    }

    World::SetDetailDoodadDist(dist);
    return true;
}

// OFFSET: 0x0076C010/0x140454100
bool CWorldParam::HWPCFCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    uint32_t enables = World::GetEnables();

    if (enable) {
        if (g_theGxDevicePtr->m_caps.m_pcfFormat[GxTex_D24X8]) {
            ConsoleWrite("Hardware PCF enabled.", DEFAULT_COLOR);
            World::SetEnables(0x10000, 1);
        } else {
            ConsoleWrite("Hardware PCF not supported by this graphics card.", DEFAULT_COLOR);
            return false;
        }
    } else {
        ConsoleWrite("Hardware PCF disabled.", DEFAULT_COLOR);
        World::SetEnables(0x10000, 0);
    }

    if (enables ^ World::GetEnables()) {
        CShaderEffect::SetUsePcfFiltering((World::GetEnables() >> 16) & 1);
        CShadowCache::NotifyTexturesRecreate();
    }

    return true;
}

// OFFSET: 0x0076C0B0/0x1404541B0
bool CWorldParam::GxTextureCacheSizeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t size = SStrToInt(value);
    char text[256];

    if (size) {
        World::s_texCacheOverride = size << 20;
        g_theGxDevicePtr->TexSetCacheSize(size << 20);
        sprintf(text, "Texture cache size set to %dMB.", size);
        ConsoleWrite(text, DEFAULT_COLOR);
    } else {
        World::s_texCacheOverride = 0;
        ConsoleWrite("Texture cache size set to default.", DEFAULT_COLOR);
    }

    return true;
}

// OFFSET: 0x0076C170/0x140454280
void CWorldParam::Initialize() {
    CWorldParam::cvar_detailDoodadInstancing = CVar::Register("detailDoodadInstancing", "Detail doodad instancing", 1, "1", CWorldParam::DetailDoodadInstancingCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_terrainMipLevel = CVar::Register("terrainMipLevel", "Terrain blend map mip level", 1, "1", CWorldParam::TerrainMipLevelCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_farClip = CVar::Register("farclip", "Far clip plane distance", 1, "350", CWorldParam::FarClipCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_nearClip = CVar::Register("nearclip", "Near clip plane distance", 1, "0.2", CWorldParam::NearClipCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_particleDensity = CVar::Register("particleDensity", "Video option: Particle density", 1, "100", CWorldParam::ParticleDensityCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_waterDetail = CVar::Register("waterDetail", "Water surface detail", 1, "0", CWorldParam::WaterDetailCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_rippleDetail = CVar::Register("rippleDetail", "Ripple surface detail", 1, "2", CWorldParam::RippleDetailCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_reflectionMode = CVar::Register("reflectionMode", "Reflection mode", 1, "3", CWorldParam::ReflectionModeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_reflectionDownscale = CVar::Register("reflectionDownscale", "Reflection downscale", 1, "0", CWorldParam::ReflectionDownscaleCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_sunShafts = CVar::Register("sunShafts", "SunShafts", 1, "0", CWorldParam::SunShaftsCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_baseMip = CVar::Register("baseMip", "base mipmap level", 1, "0", CWorldParam::BaseMipCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_horizonFarClipScale = CVar::Register("horizonFarclipScale", "Far clip plane scale for horizon", 1, "4.0", CWorldParam::HorizonFarClipScaleCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_horizonNearClipScale = CVar::Register("horizonNearclipScale", "Near clip plane scale for horizon", 1, "0.7", CWorldParam::HorizonNearClipScaleCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_bspCache = CVar::Register("bspcache", "BSP node caching", 1, "1", CWorldParam::BspCacheCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_worldPoolUsage = CVar::Register("worldPoolUsage", "Usage static/dynamic/stream", 1, "static", CWorldParam::WorldPoolUsageCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_terrainAlphaBitDepth = CVar::Register("terrainAlphaBitDepth", "Terrain alpha map bit depth", 1, "8", CWorldParam::TerrainAlphaBitDepthCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_groundEffectDensity = CVar::Register("groundEffectDensity", "Ground effect density", 1, "16", CWorldParam::GroundEffectDensityCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_groundEffectFade = CVar::Register("groundEffectFade", "Ground effect fade", 1, "70", CWorldParam::GroundEffectFadeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_groundEffectDist = CVar::Register("groundEffectDist", "Ground effect dist", 1, "70", CWorldParam::GroundEffectDistCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_objectFade = CVar::Register("objectFade", "Fade objects into view", 1, "1", CWorldParam::ObjectFadeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_objectFadeZFill = CVar::Register("objectFadeZFill", "Fade objects using ZFill pass ", 1, "0", CWorldParam::ObjectFadeZFillCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_environmentDetail = CVar::Register("environmentDetail", "Environment detail", 1, "100", CWorldParam::EnvironmentDetailCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_hwPCF = CVar::Register("hwPCF", "Hardware PCF Filtering", 1, "1", CWorldParam::HWPCFCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_projectedTextures = CVar::Register("projectedTextures", "Projected Textures", 1, "0", CWorldParam::ProjectedTexturesCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_gxTextureCacheSize = CVar::Register("gxTextureCacheSize", "GX Texture Cache Size", 1, "0", CWorldParam::GxTextureCacheSizeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_shadowMode = CVar::Register("shadowMode", "Quality of shadows (0-3)", 1, "0", CWorldParam::ShadowModeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_shadowTextureSize = CVar::Register("shadowTextureSize", "Shadow texture size (1024-2048)", 1, "1024", CWorldParam::ShadowTextureSizeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_preloadStreamingDistTerrain = CVar::Register("preloadStreamingDistTerrain", "Terrain preload distance when streaming", 1, "256", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CWorldParam::cvar_preloadStreamingDistObject = CVar::Register("preloadStreamingDistObject", "Object preload distance when streaming", 1, "64", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CWorldParam::cvar_preloadLoadingDistTerrain = CVar::Register("preloadLoadingDistTerrain", "Terrain preload distance when loading", 1, "1024", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CWorldParam::cvar_preloadLoadingDistObject = CVar::Register("preloadLoadingDistObject", "Object preload distance when loading", 1, "512", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CWorldParam::cvar_skyCloudLOD = CVar::Register("SkyCloudLOD", "Texture resolution for clouds", 1, "0", CWorldParam::CloudLODCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_textureFilteringMode = CVar::Register("textureFilteringMode", "Texture filtering mode", 1, "1", CWorldParam::TextureFilteringCallback, CATEGORY_GRAPHICS, false, NULL, false);
    CWorldParam::cvar_minimapPortalMax = CVar::Register("minimapPortalMax", "Max Number of Portals to traverse for minimap", 32, "99", NULL, CATEGORY_GAME, false, NULL, false);

    CCharacterComponent::InitCVars();
    Weather::InitializeCVars();
    FFX::InitCVars();
    AddConsoleDeviceDefaultCallback(CWorldParam::SetDefaults);

    int32_t version = 0;
    CVar* videoOptionsVersion = CVar::Lookup("videoOptionsVersion");

    if (videoOptionsVersion) {
        version = videoOptionsVersion->m_intValue;
    }

    if (ConsoleDeviceHardwareChanged() || version < 4) {
        for (uint32_t page = OPTION_PAGE_0; page < NUM_OPTION_PAGES; page++) {
            CWorldParam::SetDefaults(static_cast<OPTION_PAGE>(page));
        }
    }
}
