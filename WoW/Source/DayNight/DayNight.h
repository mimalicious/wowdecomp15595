#ifndef WOW_SOURCE_DAYNIGHT_DAYNIGHT_H
#define WOW_SOURCE_DAYNIGHT_DAYNIGHT_H

#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/DayNight/DNOverrideSky.h"

class C44Matrix;
class LightFloatBandRec;
class LightIntBandRec;
class LightParamsRec;
class LightRec;
class ZoneLightPointRec;
struct SMOFog;

namespace DayNight {
    typedef bool (*INTERSECT_CALLBACK)(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist);

    class DNClouds;
    class DNMoonGlare;
    class DNPlanet;
    class DNSky;
    class DNStars;
    class DNSunGlare;

    class FogInfo {
        public:
        /* 0000 */ CImVector m_color;
        /* 0004 */ float m_start;
        /* 0008 */ float m_end;
        /* 000C */ float m_density;
    };

    class LiquidColors {
        public:
        /* 0000 */ CImVector m_color;
        /* 0004 */ float m_alpha[4];
        /* 0014 */ CImVector m_shallowColor[2];
        /* 001C */ CImVector m_deepColor[2];
    };

    class LightInfo {
        public:
        /* 0000 */ C3Vector m_direction;
        /* 000C */ CImVector m_directColor;
        /* 0010 */ CImVector m_ambientColor;
        /* 0014 */ CImVector m_midColor0;
        /* 0018 */ CImVector m_midColor1;
        /* 001C */ float m_desaturate[4];
    };

    class SkyboxBlend {
        public:
        /* 0000 */ int32_t m_skyboxId;
        /* 0004 */ float m_weight;
    };

    class CurrentLight {
        public:
        /* 0000 */ CImVector m_colors[18];
        /* 0048 */ float m_fogEnd;
        /* 004C */ float m_fogStartScalar;
        /* 0050 */ float m_fogRate;
        /* 0054 */ float m_highlightSky;
        /* 0058 */ float m_glow;
        /* 005C */ float m_floatBands[4];
        /* 006C */ float m_waterShallowAlpha;
        /* 0070 */ float m_waterDeepAlpha;
        /* 0074 */ float m_oceanShallowAlpha;
        /* 0078 */ float m_oceanDeepAlpha;
        /* 007C */ float m_lightParamsFlag;
        /* 0080 */ SkyboxBlend m_skyboxes[3];
        /* 0098 */ int32_t m_cloudTypeId;
        /* 009C */ float m_cloudWeight;

        CurrentLight();
        CurrentLight& operator=(const CurrentLight& other);
    };

    class LightQE {
        public:
        /* 0000 */ float m_distSq;
        /* 0004 */ const LightRec* const* m_light;
    };

    class AreaLightOverride {
        public:
        /* 0000 */ int32_t m_lightId;
        /* 0004 */ LightRec* m_rec;
        /* 0008 */ float m_blend;
        /* 000C */ uint32_t m_state;
        /* 0010 */ int32_t m_time;
        /* 0014 */ int32_t m_duration;
    };

    class CZoneLight {
        public:
        /* 0000 */ int32_t m_id;
        /* 0004 */ uint32_t m_pointCount;
        /* 0008 */ C2Vector m_min;
        /* 0010 */ C2Vector m_max;
        /* 0018 */ TSGrowableArray<const ZoneLightPointRec*> m_points;

        CZoneLight();
    };

    class DNInfo {
        public:
        /* 0000 */ int32_t m_mapId;
        /* 0004 */ uint32_t m_time;
        /* 0008 */ float m_dayFraction;
        /* 000C */ float m_timeOfDay;
        /* 0010 */ C3Vector m_camPos;
        /* 001C */ C3Vector m_camTarget;
        /* 0028 */ C3Vector m_camDir;
        /* 0034 */ uint32_t m_liquidStatus;
        /* 0038 */ float m_camDepth;
        /* 003C */ float m_facing;
        /* 0040 */ float m_farClip;
        /* 0044 */ float m_timeSec;
        /* 0048 */ float m_tickSec;
        /* 004C */ uint32_t m_timeMs;
        /* 0050 */ float m_stormPercentage;
        /* 0054 */ uint8_t m_eclipseAmount;
        /* 0055 */ uint8_t m_eclipseLightId[4];
        /* 0059 */ uint8_t unk_0059[3];
        /* 005C */ int32_t m_overrideLightParamsId;
        /* 0060 */ DNOverrideSky* m_skybox;
        /* 0064 */ float m_skyboxWeight;
        /* 0068 */ DNOverrideSky* m_blendSkybox[3];
        /* 0074 */ float m_blendSkyboxWeight[3];
        /* 0080 */ uint32_t m_blendSkyboxFlags[3];
        /* 008C */ float m_stormBlend;
        /* 0090 */ FogInfo m_fog;
        /* 00A0 */ FogInfo m_fogUW;
        /* 00B0 */ float m_interiorFogBlend;
        /* 00B4 */ FogInfo m_intFog;
        /* 00C4 */ FogInfo m_intFogUW;
        /* 00D4 */ FogInfo m_mapObjFog;
        /* 00E4 */ FogInfo m_mapObjFogUW;
        /* 00F4 */ CurrentLight m_light;
        /* 0194 */ CurrentLight m_lightUW;
        /* 0234 */ LightInfo m_minimapLight[2];
        /* 028C */ LightInfo m_lightInfo;
        /* 02B8 */ LightInfo m_lightInfoUW;
        /* 02E4 */ LightInfo m_deathLightInfo;
        /* 0310 */ LiquidColors m_liquidColors;
        /* 0334 */ CImVector m_shadowColor;
        /* 0338 */ int32_t m_showSky;
        /* 033C */ C2Vector m_sunCloudUV;
        /* 0344 */ float m_celestialFade;
        /* 0348 */ float m_selfIlluminatedScalar;
        /* 034C */ int32_t m_zoneLightCount;
        /* 0350 */ uint32_t unk_0350;
        /* 0354 */ const LightRec* m_zoneLights[5];
        /* 0368 */ float m_zoneLightBlend[5];
        /* 037C */ int32_t m_fogLinear;
        /* 0380 */ uint8_t m_enableFogMode;
        /* 0381 */ uint8_t m_interiorFogEnabled;
        /* 0382 */ uint8_t unk_0382[2];
        /* 0384 */ uint32_t m_interiorFog[12];
        /* 03B4 */ uint8_t m_interiorFogBlendEnabled;
        /* 03B5 */ uint8_t unk_03B5[3];
        /* 03B8 */ float m_interiorFogDistance;

        void AddZoneLight(int32_t lightId, float blend);
        void ClearZoneLights();
    };

    extern DNInfo g_dnInfo;
    extern INTERSECT_CALLBACK g_intersectCallback;
    extern float g_fogDistanceMax;
    extern float g_fOverrideFogRate;
    extern float g_fOverrideFogStartScalar;
    extern float g_fOverrideFogEnd;
    extern int32_t g_bSaveShowSky;
    extern float g_fSaveFogRate;
    extern float g_fSaveFogStartScalar;
    extern float g_fSaveFogEnd;
    extern int32_t g_bOverrideFog;
    extern int32_t g_zoneLightsDirty;
    extern int32_t g_forceFullUpdate;
    extern int32_t g_exteriorUnderwater;
    extern int32_t g_initialized;
    extern int32_t g_areaLightSortByDistance;
    extern float g_areaLightSortRadius;
    extern CImVector g_SaveFogColor;
    extern CImVector g_OverrideFogColor;
    extern DNStars g_stars;
    extern DNClouds g_clouds;
    extern DNPlanet g_planets[4];
    extern DNSky g_sky;
    extern DNSunGlare g_sunGlare;
    extern DNMoonGlare g_moonGlare;
    extern TSHashTable<DNOverrideSky, HASHKEY_STRI> g_overrideSky;
    extern TSGrowableArray<AreaLightOverride> g_areaLightOverrides;
    extern TSGrowableArray<const LightRec*> g_areaLights;
    extern CDataAllocator g_areaLightRecordCache;
    extern TSGrowableArray<CZoneLight> g_zoneLights;

    void BlendAreaLight(CurrentLight& out, float weight, const LightRec* rec, int32_t index, int32_t stormIndex);
    void BlendAreaLightColors(CurrentLight& light, const LightRec* rec, int32_t index, int32_t stormIndex);
    void BlendColors(CurrentLight& out, const CurrentLight& in, float t);
    void BlendOutputs(CurrentLight& out, const CurrentLight& in, float t);
    void Billboard(const C3Vector& dir, C44Matrix& mat);
    void CalcColors(int32_t time, CurrentLight& light, const LightParamsRec* params);
    void CalcFog(FogInfo& fog, const float* params);
    float CalcFogRate(float start, float end);
    CImVector* CalcIndividualColorFromDBRec(CImVector* out, const LightIntBandRec* band, int32_t time);
    CImVector* CalcIndividualColorFromDBRec(CImVector* out, int32_t time, const LightParamsRec* params, int32_t band);
    void ClearOverrideAreaLight(int32_t time, int32_t lightId, int32_t duration);
    void ClearOverrideFog();
    void ClearOverrideLightParamsID();
    CImVector* DarkenColor(CImVector* out, const CImVector* color, float scale);
    void Destroy();
    void DestroyOverrideSky(const char* name);
    void DoAreaLights(uint32_t time, CurrentLight& light, int32_t index, int32_t stormIndex);
    void DrawSky(DNOverrideSky* sky, float weight);
    void FindAndAddZoneLight();
    void ForceFullUpdate();
    const LightInfo* GetDeathLightInfo();
    const FogInfo* GetFogInfo();
    const FogInfo* GetFogInfoAW();
    const FogInfo* GetFogInfoUW();
    float GetGlow();
    const FogInfo* GetIntFogInfo();
    const FogInfo* GetIntFogInfoUW();
    void GetLight(uint32_t time, const C3Vector& pos, float dayFraction, CurrentLight& light, int32_t index, int32_t stormIndex);
    const LightInfo* GetLightInfo();
    const LightParamsRec* GetLightParams(const LightRec* light, int32_t index);
    const LiquidColors* GetLiquidColors(uint32_t index);
    CImVector* GetLiquidMinimapColor(CImVector* color, uint32_t time, const C3Vector& pos, uint8_t blend, uint32_t band);
    const LightInfo* GetMinimapLightInfo();
    C3Vector* GetMoonPos(C3Vector* pos);
    float GetMoonGlareOpacity();
    DNOverrideSky* GetOverrideSky(uint32_t timeMs, const char* name, uint32_t flags);
    float GetSelfIlluminatedScalar();
    CImVector* GetShadowColor(CImVector* color);
    CImVector* GetSpecularColor(CImVector* color);
    C3Vector* GetSunGlarePos(C3Vector* pos);
    float GetSunGlareOpacity();
    C3Vector* GetSunPos(C3Vector* pos);
    void Initialize(bool enableFogMode);
    CImVector* InterpColor(CImVector* out, const CImVector* a, const CImVector* b, float t);
    float InterpTable(const C2Vector* table, uint32_t count, float t);
    int32_t IsAllLoaded();
    void LightLoad(TSGrowableArray<const LightRec*>& lights, int32_t mapId);
    void LoadMap(int32_t mapId);
    int32_t PointInZone(const CZoneLight& zone, const C2Vector& point, float& dist);
    void Preload(const C3Vector& pos);
    void RenderGlares();
    void RenderSky(const char* skyboxName, bool interior);
    DNOverrideSky* SetBlendSky(uint32_t timeMs, uint32_t index, const char* name, uint32_t flags, float weight);
    void SetCamera(const C3Vector& pos, const C3Vector& dir, const C3Vector& target, uint32_t liquidStatus, float depth, float farClip);
    void SetCloudLOD(uint32_t lod);
    void SetColors();
    void SetDirection();
    void SetEclipse(uint32_t lightId, float amount);
    void SetExteriorUnderwater(int32_t underwater);
    void SetFogDistanceMax(float distance);
    void SetInteriorFog(bool inside, const SMOFog& fog, bool blend, float distance);
    void SetIntersectCallback(INTERSECT_CALLBACK callback);
    void SetLightColors();
    void SetOverrideAreaLight(int32_t time, int32_t lightId, int32_t overrideId, int32_t duration);
    void SetOverrideFog(float startScalar, float end, CImVector color, int32_t showSky);
    void SetOverrideLightParamsID(int32_t id, int32_t preload);
    void SetPlanets();
    void SetStormPercentage(float percentage);
    void SetTime(uint32_t time, float timeOfDay, float dayFraction);
    int32_t ShouldClearToBlack();
    void StreamingUpdate();
    void SyncSkyAnimWithTime(DNOverrideSky* sky, int32_t minute);
    void UnloadMap();
    void Update(uint32_t timeMs, float tickSec, float timeSec);
    void UpdateFog();
    void UpdateLighting();
    void UpdateZoneLights();
}

#include "WoW/Source/DayNight/DayNight.inl"

#endif
