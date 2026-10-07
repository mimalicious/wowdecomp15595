#ifndef WOW_SOURCE_WORLDCLIENT_WORLDPARAM_H
#define WOW_SOURCE_WORLDCLIENT_WORLDPARAM_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"

struct CVar;

class CWorldParam {
    public:
    struct DefaultSetting {
        /* 0000 */ const char* m_name;
        /* 0004 */ int32_t m_value;
    };

    static CVar* cvar_detailDoodadInstancing;
    static CVar* cvar_terrainMipLevel;
    static CVar* cvar_farClip;
    static CVar* cvar_nearClip;
    static CVar* cvar_particleDensity;
    static CVar* cvar_waterDetail;
    static CVar* cvar_rippleDetail;
    static CVar* cvar_reflectionMode;
    static CVar* cvar_reflectionDownscale;
    static CVar* cvar_sunShafts;
    static CVar* cvar_baseMip;
    static CVar* cvar_horizonFarClipScale;
    static CVar* cvar_horizonNearClipScale;
    static CVar* cvar_bspCache;
    static CVar* cvar_worldPoolUsage;
    static CVar* cvar_terrainAlphaBitDepth;
    static CVar* cvar_groundEffectDensity;
    static CVar* cvar_groundEffectFade;
    static CVar* cvar_groundEffectDist;
    static CVar* cvar_objectFade;
    static CVar* cvar_objectFadeZFill;
    static CVar* cvar_environmentDetail;
    static CVar* cvar_hwPCF;
    static CVar* cvar_projectedTextures;
    static CVar* cvar_gxTextureCacheSize;
    static CVar* cvar_shadowMode;
    static CVar* cvar_shadowTextureSize;
    static CVar* cvar_preloadStreamingDistTerrain;
    static CVar* cvar_preloadStreamingDistObject;
    static CVar* cvar_preloadLoadingDistTerrain;
    static CVar* cvar_preloadLoadingDistObject;
    static CVar* cvar_skyCloudLOD;
    static CVar* cvar_textureFilteringMode;
    static CVar* cvar_minimapPortalMax;
    static const DefaultSetting* s_defaults[5];
    static const uint32_t s_textureFilteringAnisotropy[6];
    static const EGxTexFilter s_textureFilteringModes[6];

    static bool BaseMipCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool BspCacheCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool CloudLODCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void Destroy();
    static bool DetailDoodadInstancingCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool EnvironmentDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool FarClipCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool GroundEffectDensityCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool GroundEffectDistCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool GroundEffectFadeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool GxTextureCacheSizeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool HorizonFarClipScaleCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool HorizonNearClipScaleCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool HWPCFCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void Initialize();
    static bool NearClipCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ObjectFadeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ObjectFadeZFillCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ParticleDensityCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ProjectedTexturesCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ReflectionDownscaleCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ReflectionModeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool RippleDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void SetDefaults(OPTION_PAGE page);
    static bool ShadowModeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool ShadowTextureSizeCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool SunShaftsCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool TerrainAlphaBitDepthCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool TerrainMipLevelCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool TextureFilteringCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static uint32_t ValidateTextureFilteringMode(uint32_t mode);
    static bool WaterDetailCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static bool WorldPoolUsageCallback(CVar* var, const char* oldValue, const char* value, void* arg);
};

#endif
