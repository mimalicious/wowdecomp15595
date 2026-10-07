#include "WoW/Source/DayNight/DayNight.h"
#include <cmath>
#include <cstring>
#include <typeinfo>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DayNight/DNClouds.h"
#include "WoW/Source/DayNight/DNGlare.h"
#include "WoW/Source/DayNight/DNOverrideSky.h"
#include "WoW/Source/DayNight/DNPlanet.h"
#include "WoW/Source/DayNight/DNSky.h"
#include "WoW/Source/DayNight/DNStars.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LightParamsRec.h"
#include "WoW/Source/DB/Rec/LightRec.h"
#include "WoW/Source/DB/Rec/LightSkyboxRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/MapObj.h"

// STATIC: 0x00E42388
DayNight::INTERSECT_CALLBACK DayNight::g_intersectCallback;

// STATIC: 0x00E4238C
float DayNight::g_fogDistanceMax;

// STATIC: 0x00E42390
float DayNight::g_fOverrideFogRate;

// STATIC: 0x00E42394
float DayNight::g_fOverrideFogStartScalar;

// STATIC: 0x00E42398
float DayNight::g_fOverrideFogEnd;

// STATIC: 0x00E4239C
int32_t DayNight::g_bSaveShowSky;

// STATIC: 0x00E423A0
float DayNight::g_fSaveFogRate;

// STATIC: 0x00E423A4
float DayNight::g_fSaveFogStartScalar;

// STATIC: 0x00E423A8
float DayNight::g_fSaveFogEnd;

// STATIC: 0x00E423AC
int32_t DayNight::g_bOverrideFog;

// STATIC: 0x00E423B0
int32_t DayNight::g_zoneLightsDirty;

// STATIC: 0x00E423B4
int32_t DayNight::g_forceFullUpdate;

// STATIC: 0x00E423B8
int32_t DayNight::g_exteriorUnderwater;

// STATIC: 0x00E423BC
int32_t DayNight::g_initialized;

// STATIC: 0x00E423C0
DayNight::DNStars DayNight::g_stars;

// STATIC: 0x00E423DC
CImVector DayNight::g_SaveFogColor;

// STATIC: 0x00E423E0
DayNight::DNInfo DayNight::g_dnInfo;

// STATIC: 0x00E4279C
CImVector DayNight::g_OverrideFogColor;

// STATIC: 0x00E427A0
DayNight::DNClouds DayNight::g_clouds;

// STATIC: 0x00E42834
TSHashTable<DayNight::DNOverrideSky, HASHKEY_STRI> DayNight::g_overrideSky;

// STATIC: 0x00E42860
DayNight::DNPlanet DayNight::g_planets[4];

// STATIC: 0x00E428E0
DayNight::DNSky DayNight::g_sky;

// STATIC: 0x00E42910
TSGrowableArray<DayNight::AreaLightOverride> DayNight::g_areaLightOverrides;

// STATIC: 0x00E42920
TSGrowableArray<const LightRec*> DayNight::g_areaLights;

// STATIC: 0x00E42930
CDataAllocator DayNight::g_areaLightRecordCache(sizeof(LightRec), 16);

// STATIC: 0x00CEF960
DayNight::DNSunGlare DayNight::g_sunGlare;

// STATIC: 0x00CEFA18
DayNight::DNMoonGlare DayNight::g_moonGlare;

// INLINED
static CImVector BlendHalf(const CImVector& a, const CImVector& b) {
    CImVector color;
    color.b = static_cast<uint8_t>(a.b + (static_cast<uint16_t>(b.b - a.b) >> 1));
    color.g = static_cast<uint8_t>(a.g + (static_cast<uint16_t>(b.g - a.g) >> 1));
    color.r = static_cast<uint8_t>(a.r + (static_cast<uint16_t>(b.r - a.r) >> 1));
    color.a = 0;
    return color;
}

// INLINED
static CImVector Brighten(const CImVector& color) {
    uint32_t value = *reinterpret_cast<const uint32_t*>(&color);
    uint32_t sum = value + 0xFF101010;
    uint32_t carry = (sum ^ value ^ 0xFF101010) & 0x01010100;
    uint32_t result = (sum - carry) | (carry - (carry >> 8));
    return *reinterpret_cast<CImVector*>(&result);
}

// OFFSET: 0x00646DD0/0x1402E3930
void DayNight::Initialize(bool enableFogMode) {
    DayNight::g_dnInfo.m_enableFogMode = enableFogMode;
    DayNight::g_dnInfo.m_showSky = 1;
}

// PROVEN: a0f23023 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DayNight::Destroy() {
}

// OFFSET: 0x00646DF0/0x1402E3950
void DayNight::SetTime(uint32_t time, float timeOfDay, float dayFraction) {
    DayNight::g_dnInfo.m_timeOfDay = timeOfDay;
    DayNight::g_dnInfo.m_time = time;
    DayNight::g_dnInfo.m_dayFraction = dayFraction;
}

// OFFSET: 0x00646E20/0x1402E3970
const DayNight::FogInfo* DayNight::GetFogInfo() {
    if (Liquid::CameraSubmerged()) {
        return &DayNight::g_dnInfo.m_fogUW;
    }

    if (DayNight::g_exteriorUnderwater) {
        return &DayNight::g_dnInfo.m_fogUW;
    }

    return &DayNight::g_dnInfo.m_fog;
}

// OFFSET: 0x00646E40
const DayNight::FogInfo* DayNight::GetIntFogInfo() {
    if (Liquid::CameraSubmerged()) {
        return &DayNight::g_dnInfo.m_intFogUW;
    }

    if (DayNight::g_exteriorUnderwater) {
        return &DayNight::g_dnInfo.m_intFogUW;
    }

    return &DayNight::g_dnInfo.m_intFog;
}

// OFFSET: 0x00646E60
const DayNight::FogInfo* DayNight::GetFogInfoUW() {
    return &DayNight::g_dnInfo.m_fogUW;
}

// OFFSET: 0x00646E70
const DayNight::FogInfo* DayNight::GetIntFogInfoUW() {
    return &DayNight::g_dnInfo.m_intFogUW;
}

// OFFSET: 0x00646E80
const DayNight::FogInfo* DayNight::GetFogInfoAW() {
    return &DayNight::g_dnInfo.m_fog;
}

// OFFSET: 0x00646E90
void DayNight::ClearOverrideFog() {
    if (DayNight::g_bOverrideFog) {
        DayNight::g_dnInfo.m_light.m_fogEnd = DayNight::g_fSaveFogEnd;
        DayNight::g_dnInfo.m_light.m_fogStartScalar = DayNight::g_fSaveFogStartScalar;
        DayNight::g_dnInfo.m_light.m_fogRate = DayNight::g_fSaveFogRate;
        DayNight::g_dnInfo.m_light.m_colors[8] = DayNight::g_SaveFogColor;
        DayNight::g_dnInfo.m_showSky = DayNight::g_bSaveShowSky;
        DayNight::g_bOverrideFog = 0;
    }
}

// OFFSET: 0x00646EF0
void DayNight::SetOverrideFog(float startScalar, float end, CImVector color, int32_t showSky) {
    if (!DayNight::g_bOverrideFog) {
        DayNight::g_fSaveFogEnd = DayNight::g_dnInfo.m_light.m_fogEnd;
        DayNight::g_fSaveFogStartScalar = DayNight::g_dnInfo.m_light.m_fogStartScalar;
        DayNight::g_fSaveFogRate = DayNight::g_dnInfo.m_light.m_fogRate;
        DayNight::g_SaveFogColor = DayNight::g_dnInfo.m_light.m_colors[8];
        DayNight::g_bSaveShowSky = DayNight::g_dnInfo.m_showSky;
    }

    DayNight::g_fOverrideFogStartScalar = startScalar;
    DayNight::g_fOverrideFogEnd = end;
    DayNight::g_fOverrideFogRate = DayNight::CalcFogRate(startScalar * end, end);
    DayNight::g_OverrideFogColor = color;
    DayNight::g_dnInfo.m_showSky = showSky;
    DayNight::g_bOverrideFog = 1;
}

// OFFSET: 0x00646FA0
void DayNight::SetFogDistanceMax(float distance) {
    DayNight::g_fogDistanceMax = distance;
}

// OFFSET: 0x00646FC0/0x1402E3A10
const DayNight::LightInfo* DayNight::GetLightInfo() {
    if (Liquid::CameraSubmerged()) {
        return &DayNight::g_dnInfo.m_lightInfoUW;
    }

    return &DayNight::g_dnInfo.m_lightInfo;
}

// OFFSET: 0x00646FE0
const DayNight::LightInfo* DayNight::GetMinimapLightInfo() {
    return DayNight::g_dnInfo.m_minimapLight;
}

// OFFSET: 0x00646FF0
float DayNight::GetSelfIlluminatedScalar() {
    return DayNight::g_dnInfo.m_selfIlluminatedScalar;
}

// OFFSET: 0x00647000
float DayNight::GetGlow() {
    return DayNight::g_dnInfo.m_light.m_glow;
}

// OFFSET: 0x00647010
CImVector* DayNight::GetShadowColor(CImVector* color) {
    *color = DayNight::g_dnInfo.m_shadowColor;
    return color;
}

// OFFSET: 0x00647020
CImVector* DayNight::GetSpecularColor(CImVector* color) {
    const CurrentLight* light = Liquid::CameraSubmerged() ? &DayNight::g_dnInfo.m_lightUW : &DayNight::g_dnInfo.m_light;
    *color = light->m_colors[9];
    return color;
}

// OFFSET: 0x00647040
void DayNight::ClearOverrideLightParamsID() {
    DayNight::g_dnInfo.m_overrideLightParamsId = 0;
}

// OFFSET: 0x00647050
const DayNight::LiquidColors* DayNight::GetLiquidColors(uint32_t index) {
    const CurrentLight* light = Liquid::CameraSubmerged() ? &DayNight::g_dnInfo.m_lightUW : &DayNight::g_dnInfo.m_light;
    LiquidColors* colors = &DayNight::g_dnInfo.m_liquidColors;
    colors->m_color = light->m_colors[17];
    colors->m_alpha[0] = light->m_waterShallowAlpha;
    colors->m_alpha[1] = light->m_waterDeepAlpha;
    colors->m_alpha[2] = light->m_oceanShallowAlpha;
    colors->m_alpha[3] = light->m_oceanDeepAlpha;
    colors->m_shallowColor[0] = light->m_colors[2 * index + 14];
    colors->m_shallowColor[1] = light->m_colors[2 * index + 15];
    colors->m_deepColor[0] = light->m_colors[2 * index + 14];
    colors->m_deepColor[1] = light->m_colors[2 * index + 15];
    return colors;
}

// OFFSET: 0x006470E0
void DayNight::SetExteriorUnderwater(int32_t underwater) {
    DayNight::g_exteriorUnderwater = underwater;
}

// OFFSET: 0x006470F0
int32_t DayNight::ShouldClearToBlack() {
    return !DayNight::g_exteriorUnderwater && (DayNight::g_dnInfo.m_showSky || !DayNight::g_bOverrideFog);
}

// OFFSET: 0x00647120
C3Vector* DayNight::GetSunPos(C3Vector* pos) {
    *pos = DayNight::g_planets[0].m_position;
    return pos;
}

// OFFSET: 0x00647150
C3Vector* DayNight::GetMoonPos(C3Vector* pos) {
    *pos = DayNight::g_planets[1].m_position;
    return pos;
}

// OFFSET: 0x00647180
C3Vector* DayNight::GetSunGlarePos(C3Vector* pos) {
    *pos = DayNight::g_sunGlare.m_position;
    return pos;
}

// OFFSET: 0x006471B0
float DayNight::GetSunGlareOpacity() {
    return DayNight::g_sunGlare.m_visibility;
}

// OFFSET: 0x006471C0
float DayNight::GetMoonGlareOpacity() {
    return DayNight::g_moonGlare.m_visibility;
}

// OFFSET: 0x006471D0/0x1402E3B70
void DayNight::SetCloudLOD(uint32_t lod) {
    DayNight::g_clouds.SetLOD(lod, 0);
}

// OFFSET: 0x006471F0
void DayNight::SetEclipse(uint32_t lightId, float amount) {
    memcpy(DayNight::g_dnInfo.m_eclipseLightId, &lightId, sizeof(lightId));
    DayNight::g_dnInfo.m_eclipseAmount = static_cast<int32_t>(amount * 255.0f);
}

// OFFSET: 0x00647240/0x1402E3B90
void DayNight::SetIntersectCallback(INTERSECT_CALLBACK callback) {
    DayNight::g_intersectCallback = callback;
}

// OFFSET: 0x00647250
DayNight::CurrentLight::CurrentLight() {
    memset(this->m_colors, 0, sizeof(this->m_colors));
    this->m_skyboxes[0].m_skyboxId = 0;
    this->m_skyboxes[0].m_weight = 0.0f;
    this->m_skyboxes[1].m_skyboxId = 0;
    this->m_skyboxes[1].m_weight = 0.0f;
    this->m_skyboxes[2].m_skyboxId = 0;
    this->m_skyboxes[2].m_weight = 0.0f;
    this->m_cloudTypeId = 0;
    this->m_cloudWeight = 0.0f;
    this->m_fogEnd = 0.0f;
    this->m_fogStartScalar = 0.0f;
    this->m_fogRate = 0.0f;
    this->m_highlightSky = 0.0f;
    this->m_glow = 0.0f;
    this->m_floatBands[0] = 0.0f;
    this->m_floatBands[1] = 0.0f;
    this->m_floatBands[2] = 0.0f;
    this->m_floatBands[3] = 0.0f;
    this->m_waterShallowAlpha = 0.0f;
    this->m_waterDeepAlpha = 0.0f;
    this->m_oceanShallowAlpha = 0.0f;
    this->m_oceanDeepAlpha = 0.0f;
    this->m_lightParamsFlag = 0.0f;
}

// OFFSET: 0x006473F0/0x1402E4080
const DayNight::LightInfo* DayNight::GetDeathLightInfo() {
    LightParamsRec* params = g_lightParamsDB.GetRecord(3);
    DayNight::g_dnInfo.m_deathLightInfo = DayNight::g_dnInfo.m_lightInfo;

    CImVector color;
    DayNight::g_dnInfo.m_deathLightInfo.m_ambientColor = *DayNight::CalcIndividualColorFromDBRec(&color, 0, params, 1);
    DayNight::g_dnInfo.m_deathLightInfo.m_directColor = *DayNight::CalcIndividualColorFromDBRec(&color, 0, params, 0);
    return &DayNight::g_dnInfo.m_deathLightInfo;
}

// OFFSET: 0x006474D0
void DayNight::ClearOverrideAreaLight(int32_t time, int32_t lightId, int32_t duration) {
    for (uint32_t i = 0; i < DayNight::g_areaLightOverrides.m_count; i++) {
        AreaLightOverride* areaLight = &DayNight::g_areaLightOverrides.m_data[i];

        if (areaLight->m_lightId == lightId) {
            areaLight->m_time = time;
            areaLight->m_state = 2;
            areaLight->m_duration = duration ? duration : 1;
            DayNight::g_forceFullUpdate = 1;
            return;
        }
    }
}

// OFFSET: 0x00647540
void DayNight::SetOverrideLightParamsID(int32_t id, int32_t preload) {
    if (id <= 0) {
        DayNight::g_dnInfo.m_overrideLightParamsId = 0;
        return;
    }

    if (preload && DayNight::g_dnInfo.m_overrideLightParamsId != id) {
        LightParamsRec* params = g_lightParamsDB.GetRecord(id);

        if (params) {
            LightSkyboxRec* skybox = g_lightSkyboxDB.GetRecord(params->m_lightSkyboxID);

            if (skybox) {
                DayNight::DestroyOverrideSky(skybox->m_name);
            }
        }
    }

    DayNight::g_dnInfo.m_overrideLightParamsId = id;
}

// OFFSET: 0x006475B0/0x1402E4180
CImVector* DayNight::GetLiquidMinimapColor(CImVector* color, uint32_t time, const C3Vector& pos, uint8_t blend, uint32_t band) {
    CurrentLight light;
    DayNight::GetLight(time, pos, 0.5f, light, 0, 2);
    *color = light.m_colors[band + 14];

    if (blend) {
        color->InterpColor(blend, &light.m_colors[band + 15]);
    }

    const float* alpha = NULL;

    if (band) {
        if (band == 2) {
            alpha = &light.m_oceanShallowAlpha;
        }
    } else {
        alpha = &light.m_waterShallowAlpha;
    }

    color->a = static_cast<uint8_t>(CMath::fint_((((alpha[band + 1] - alpha[band]) * (blend * 0.0039215689f)) + alpha[band]) * 255.0f));
    return color;
}

// OFFSET: 0x00647780
void DayNight::UnloadMap() {
    for (DNOverrideSky* sky = DayNight::g_overrideSky.Head(), * next; sky; sky = next) {
        next = DayNight::g_overrideSky.Next(sky);

        if (sky->m_model) {
            sky->m_model->Release();
        }

        DayNight::g_overrideSky.Delete(sky);
    }

    if (DayNight::DNOverrideSky::s_scene) {
        DayNight::DNOverrideSky::s_scene->Release();
    }

    DayNight::DNOverrideSky::s_scene = NULL;
    DayNight::g_dnInfo.m_skybox = NULL;
    DayNight::g_dnInfo.m_blendSkybox[0] = NULL;
    DayNight::g_dnInfo.m_blendSkybox[1] = NULL;
    DayNight::g_dnInfo.m_blendSkybox[2] = NULL;

    for (uint32_t i = 0; i < DayNight::g_areaLightOverrides.m_count; i++) {
        DayNight::g_areaLightRecordCache.PutData(DayNight::g_areaLightOverrides.m_data[i].m_rec, 0, 0);
    }

    DayNight::g_areaLightOverrides.m_count = 0;
    DayNight::g_areaLightRecordCache.Clear(typeid(LightRec).raw_name(), -2, 1);
    DayNight::g_sky.Destroy();
    DayNight::g_clouds.Destroy();
    DayNight::g_planets[0].Destroy();
    DayNight::g_planets[1].Destroy();
    DayNight::g_planets[2].Destroy();
    DayNight::g_sunGlare.Destroy();
    DayNight::g_moonGlare.Destroy();
    DayNight::g_stars.Destroy();
    DayNight::g_initialized = 0;
}

// OFFSET: 0x006478C0
void DayNight::SetOverrideAreaLight(int32_t time, int32_t lightId, int32_t overrideId, int32_t duration) {
    LightRec* target = g_lightDB.GetRecord(lightId);
    const LightRec* source = g_lightDB.GetRecord(overrideId);

    if (!target || !source) {
        return;
    }

    LightRec* rec = static_cast<LightRec*>(DayNight::g_areaLightRecordCache.GetData(0, typeid(LightRec).raw_name(), -2));

    if (!rec) {
        return;
    }

    memcpy(rec, source, sizeof(LightRec));
    rec->m_gameCoords[0] = target->m_gameCoords[0];
    rec->m_gameCoords[1] = target->m_gameCoords[1];
    rec->m_gameCoords[2] = target->m_gameCoords[2];
    rec->m_gameFalloffStart = target->m_gameFalloffStart;
    rec->m_gameFalloffEnd = target->m_gameFalloffEnd;

    AreaLightOverride* areaLight = DayNight::g_areaLightOverrides.New();

    if (areaLight) {
        areaLight->m_lightId = lightId;
        areaLight->m_time = time;
        areaLight->m_rec = rec;
        areaLight->m_blend = 0.0f;
        areaLight->m_state = 1;
        areaLight->m_duration = duration ? duration : 1;
        DayNight::g_forceFullUpdate = 1;
    }
}

// OFFSET: 0x006479B0
void DayNight::LoadMap(int32_t mapId) {
    if (DayNight::g_initialized) {
        DayNight::UnloadMap();
    }

    DayNight::g_dnInfo.m_mapId = mapId;
    DayNight::LightLoad(DayNight::g_areaLights, mapId);
    DayNight::g_dnInfo.m_camPos = C3Vector();
    DayNight::g_dnInfo.m_camTarget = C3Vector();
    DayNight::g_dnInfo.m_camDir = C3Vector();
    DayNight::g_dnInfo.m_time = 0;
    DayNight::g_dnInfo.m_dayFraction = 0.0f;
    DayNight::g_dnInfo.m_timeOfDay = 0.0f;
    DayNight::g_dnInfo.m_facing = 0.0f;
    DayNight::g_dnInfo.m_farClip = 0.0f;
    DayNight::g_dnInfo.m_timeSec = 0.0f;
    DayNight::g_dnInfo.m_tickSec = 0.0f;
    DayNight::g_dnInfo.m_stormPercentage = 0.0f;
    DayNight::g_dnInfo.m_eclipseAmount = 0;
    memset(DayNight::g_dnInfo.m_eclipseLightId, 0, sizeof(DayNight::g_dnInfo.m_eclipseLightId));
    DayNight::g_dnInfo.m_overrideLightParamsId = 0;
    DayNight::g_dnInfo.m_skybox = NULL;
    DayNight::g_dnInfo.m_skyboxWeight = 0.0f;
    DayNight::g_dnInfo.m_blendSkybox[0] = NULL;
    DayNight::g_dnInfo.m_blendSkyboxWeight[0] = 0.0f;
    DayNight::g_dnInfo.m_blendSkyboxFlags[0] = 0;
    DayNight::g_dnInfo.m_blendSkybox[1] = NULL;
    DayNight::g_dnInfo.m_blendSkyboxWeight[1] = 0.0f;
    DayNight::g_dnInfo.m_blendSkyboxFlags[1] = 0;
    DayNight::g_dnInfo.m_blendSkybox[2] = NULL;
    DayNight::g_dnInfo.m_blendSkyboxWeight[2] = 0.0f;
    DayNight::g_dnInfo.m_blendSkyboxFlags[2] = 0;
    DayNight::g_dnInfo.m_interiorFogBlend = 0.0f;
    DayNight::GlareBase::s_masterEnable = 1;

    DayNight::g_sky.Initialize();
    DayNight::g_sky.GenSphere(1.0f);
    DayNight::g_clouds.Initialize();
    DayNight::g_clouds.SetLOD(0, 0);
    DayNight::g_clouds.GenSphere(1.0f);
    DayNight::g_clouds.SetDensity(0.60000002f);
    DayNight::g_clouds.SetSharpness(0.95999998f);
    DayNight::g_clouds.m_enabled = 1;

    DayNight::g_sunGlare.Initialize("Textures\\sunGlare.blp");
    DayNight::g_sunGlare.m_enable = 1;
    DayNight::g_sunGlare.m_baseScale = 1.0f;
    DayNight::g_sunGlare.m_fadeInRate = 4.0f;
    DayNight::g_sunGlare.m_fadeOutRate = 1.5151515f;
    DayNight::g_sunGlare.m_skipQuery = 1;
    DayNight::g_sunGlare.m_alphaCurve[0].x = 0.27083334f;
    DayNight::g_sunGlare.m_alphaCurve[0].y = 0.0f;
    DayNight::g_sunGlare.m_alphaCurve[1].x = 0.3125f;
    DayNight::g_sunGlare.m_alphaCurve[1].y = 1.0f;
    DayNight::g_sunGlare.m_alphaCurve[2].x = 0.8125f;
    DayNight::g_sunGlare.m_alphaCurve[2].y = 1.0f;
    DayNight::g_sunGlare.m_alphaCurve[3].x = 0.875f;
    DayNight::g_sunGlare.m_alphaCurve[3].y = 0.0f;
    DayNight::g_sunGlare.m_scaleMin = 3.0f;
    DayNight::g_sunGlare.m_scaleMax = 20.0f;
    DayNight::g_sunGlare.m_minDot = 0.69999999f;
    DayNight::g_sunGlare.m_alphaMin = 0.5f;
    DayNight::g_sunGlare.m_alphaMax = 1.0f;
    DayNight::g_sunGlare.m_glow = 0.0f;
    DayNight::g_sunGlare.m_planet = &DayNight::g_planets[0];

    DayNight::g_moonGlare.Initialize("Textures\\moonGlare.blp");
    DayNight::g_moonGlare.m_enable = 1;
    DayNight::g_moonGlare.m_baseScale = 2.0f;
    DayNight::g_moonGlare.m_fadeInRate = 3.030303f;
    DayNight::g_moonGlare.m_fadeOutRate = 1.5151515f;
    DayNight::g_moonGlare.m_skipQuery = 1;
    DayNight::g_moonGlare.m_alphaCurve[0].x = 0.083333336f;
    DayNight::g_moonGlare.m_alphaCurve[0].y = 1.0f;
    DayNight::g_moonGlare.m_alphaCurve[1].x = 0.13541667f;
    DayNight::g_moonGlare.m_alphaCurve[1].y = 0.0f;
    DayNight::g_moonGlare.m_alphaCurve[2].x = 0.94791669f;
    DayNight::g_moonGlare.m_alphaCurve[2].y = 0.0f;
    DayNight::g_moonGlare.m_alphaCurve[3].x = 0.99930561f;
    DayNight::g_moonGlare.m_alphaCurve[3].y = 1.0f;
    DayNight::g_moonGlare.m_scaleMin = 1.0f;
    DayNight::g_moonGlare.m_scaleMax = 1.0f;
    DayNight::g_moonGlare.m_minDot = 0.69999999f;
    DayNight::g_moonGlare.m_alphaMin = 0.1f;
    DayNight::g_moonGlare.m_alphaMax = 1.0f;
    DayNight::g_moonGlare.m_glow = 0.0f;
    DayNight::g_moonGlare.m_planet = &DayNight::g_planets[1];

    DayNight::g_planets[0].Initialize("Textures\\sunCenter.blp");
    DayNight::g_planets[0].m_baseScale = 1.0f;
    DayNight::g_planets[0].m_period = 1.0f;
    DayNight::g_planets[1].Initialize("Textures\\moon.blp");
    DayNight::g_planets[1].m_baseScale = 1.75f;
    DayNight::g_planets[1].m_period = 1.0f;
    DayNight::g_planets[2].Initialize("Textures\\moon02.blp");
    DayNight::g_planets[2].m_baseScale = 1.0f;
    DayNight::g_planets[2].m_period = 1.7f;
    DayNight::g_stars.Initialize();

    DayNight::g_initialized = 1;
    DayNight::g_zoneLightsDirty = 1;
    DayNight::g_forceFullUpdate = 1;
    DayNight::g_dnInfo.m_eclipseAmount = 0;
    DayNight::g_exteriorUnderwater = 0;
}

// OFFSET: 0x00647E40/0x1402E4C10
void DayNight::ForceFullUpdate() {
    DayNight::g_forceFullUpdate = 1;
}

// OFFSET: 0x00647E50
void DayNight::SetCamera(const C3Vector& pos, const C3Vector& dir, const C3Vector& target, uint32_t liquidStatus, float depth, float farClip) {
    DayNight::g_dnInfo.m_camPos = pos;
    DayNight::g_dnInfo.m_camDir = dir;

    float lengthSq = DayNight::g_dnInfo.m_camDir.y * DayNight::g_dnInfo.m_camDir.y + DayNight::g_dnInfo.m_camDir.z * DayNight::g_dnInfo.m_camDir.z + DayNight::g_dnInfo.m_camDir.x * DayNight::g_dnInfo.m_camDir.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        DayNight::g_dnInfo.m_camDir.x = DayNight::g_dnInfo.m_camDir.x * scale;
        DayNight::g_dnInfo.m_camDir.y = DayNight::g_dnInfo.m_camDir.y * scale;
        DayNight::g_dnInfo.m_camDir.z = scale * DayNight::g_dnInfo.m_camDir.z;
    }

    DayNight::g_dnInfo.m_camTarget = target;
    DayNight::g_dnInfo.m_camDepth = depth;
    DayNight::g_dnInfo.m_liquidStatus = liquidStatus;
    DayNight::g_dnInfo.m_farClip = farClip;
}

// OFFSET: 0x00647F40
void DayNight::SetStormPercentage(float percentage) {
    DayNight::g_dnInfo.m_stormPercentage = percentage;
}

// OFFSET: 0x00647F60
DayNight::CurrentLight& DayNight::CurrentLight::operator=(const CurrentLight& other) {
    memcpy(this->m_colors, other.m_colors, sizeof(this->m_colors));
    this->m_fogEnd = other.m_fogEnd;
    this->m_fogStartScalar = other.m_fogStartScalar;
    this->m_fogRate = other.m_fogRate;
    this->m_highlightSky = other.m_highlightSky;
    this->m_glow = other.m_glow;
    this->m_floatBands[0] = other.m_floatBands[0];
    this->m_floatBands[1] = other.m_floatBands[1];
    this->m_floatBands[2] = other.m_floatBands[2];
    this->m_floatBands[3] = other.m_floatBands[3];
    this->m_waterShallowAlpha = other.m_waterShallowAlpha;
    this->m_waterDeepAlpha = other.m_waterDeepAlpha;
    this->m_oceanShallowAlpha = other.m_oceanShallowAlpha;
    this->m_oceanDeepAlpha = other.m_oceanDeepAlpha;
    this->m_lightParamsFlag = other.m_lightParamsFlag;
    memcpy(this->m_skyboxes, other.m_skyboxes, sizeof(this->m_skyboxes));
    this->m_cloudTypeId = other.m_cloudTypeId;
    this->m_cloudWeight = other.m_cloudWeight;
    return *this;
}

// OFFSET: 0x00648090
void DayNight::CalcFog(FogInfo& fog, const float* params) {
    float end = params[0];

    if (DayNight::g_dnInfo.m_farClip <= end) {
        end = DayNight::g_dnInfo.m_farClip;
    }

    fog.m_end = end;
    fog.m_start = params[1] * end;
    fog.m_density = 1.0f;
    fog.m_color = *reinterpret_cast<const CImVector*>(&params[2]);

    if (end < 30.0f) {
        fog.m_end = 30.0f;
    }

    if (DayNight::g_dnInfo.m_fogLinear == 1) {
        fog.m_density = DayNight::CalcFogRate(fog.m_start, fog.m_end);
        fog.m_end = DayNight::g_dnInfo.m_farClip;

        if (fog.m_start < 0.0f) {
            fog.m_start = 0.0f;
        }
    }
}

// OFFSET: 0x00648190
void DayNight::UpdateFog() {
    DNInfo& info = DayNight::g_dnInfo;
    float fogEnd;
    float fogStart;
    float fogRate;
    CImVector fogColor;
    float uwEnd;
    float uwScalar;
    float uwRate;
    CImVector uwColor;

    if (DayNight::g_bOverrideFog) {
        uwEnd = DayNight::g_fOverrideFogEnd;
        fogEnd = info.m_farClip <= DayNight::g_fOverrideFogEnd ? info.m_farClip : DayNight::g_fOverrideFogEnd;
        uwColor = DayNight::g_OverrideFogColor;
        uwScalar = DayNight::g_fOverrideFogStartScalar;
        fogRate = DayNight::g_fOverrideFogRate;
        fogColor = DayNight::g_OverrideFogColor;
        fogStart = DayNight::g_fOverrideFogStartScalar * fogEnd;
        info.m_fog.m_end = fogEnd;
        info.m_fog.m_start = fogStart;
        info.m_fog.m_density = fogRate;
        info.m_fog.m_color = fogColor;

        if (info.m_farClip <= DayNight::g_fOverrideFogEnd) {
            uwEnd = info.m_farClip;
        }

        uwRate = DayNight::g_fOverrideFogRate;
    } else {
        uwEnd = info.m_farClip;
        fogEnd = info.m_light.m_fogEnd;

        if (info.m_farClip <= info.m_light.m_fogEnd) {
            fogEnd = info.m_farClip;
        }

        float maxEnd = 277.5f;

        if (DayNight::g_fogDistanceMax >= 277.5f) {
            maxEnd = DayNight::g_fogDistanceMax;
        }

        if (fogEnd > maxEnd) {
            fogEnd = maxEnd;
        }

        fogColor = info.m_light.m_colors[8];
        fogRate = info.m_light.m_fogRate;
        fogStart = info.m_light.m_fogStartScalar * fogEnd;
        info.m_fog.m_end = fogEnd;
        info.m_fog.m_start = fogStart;
        info.m_fog.m_density = fogRate;
        info.m_fog.m_color = fogColor;

        if (info.m_farClip > info.m_lightUW.m_fogEnd) {
            uwEnd = info.m_lightUW.m_fogEnd;
        }

        uwScalar = info.m_lightUW.m_fogStartScalar;
        uwRate = info.m_lightUW.m_fogRate;
        uwColor = info.m_lightUW.m_colors[8];
    }

    float uwStart = uwScalar * uwEnd;
    info.m_fogUW.m_color = uwColor;
    info.m_fogUW.m_end = uwEnd;
    info.m_fogUW.m_density = uwRate;
    info.m_fogUW.m_start = uwStart;

    float objEnd;
    float objUWStart;
    float objUWEnd;
    float objUWRate;
    int32_t copyFog = 0;

    if (info.m_interiorFogEnabled) {
        const SMOFog* interiorFog = reinterpret_cast<const SMOFog*>(info.m_interiorFog);
        LiquidTypeRec* liquidType = info.m_liquidStatus ? g_liquidTypeDB.GetRecord(info.m_liquidStatus) : NULL;
        int32_t underwater = 0;
        int32_t calc = 1;

        if (info.m_liquidStatus) {
            uint32_t flags = liquidType->m_flags;
            underwater = 1;

            if ((flags & 0x20) && !(interiorFog->m_flags & 0x100)) {
                underwater = 0;
            }

            if (!((!(flags & 0x100) || (interiorFog->m_flags & 0x10)) && underwater)) {
                calc = 0;
            }
        }

        if (calc) {
            DayNight::CalcFog(info.m_mapObjFog, &interiorFog->m_fogEnd);
            DayNight::CalcFog(info.m_mapObjFogUW, underwater ? &interiorFog->m_uwFogEnd : &interiorFog->m_fogEnd);

            if (underwater && liquidType && (liquidType->m_flags & 0x40)) {
                objUWRate = info.m_mapObjFogUW.m_density;
                objUWEnd = info.m_mapObjFogUW.m_end;
                objUWStart = info.m_mapObjFogUW.m_start;
                fogStart = info.m_fog.m_start;
                fogColor = info.m_fog.m_color;
                uwRate = info.m_mapObjFogUW.m_density;
                info.m_fogUW.m_start = info.m_mapObjFogUW.m_start;
                info.m_fogUW.m_end = info.m_mapObjFogUW.m_end;
                info.m_fogUW.m_density = info.m_mapObjFogUW.m_density;
                info.m_fogUW.m_color = info.m_mapObjFogUW.m_color;
                objEnd = info.m_mapObjFog.m_end;
            } else {
                uwRate = info.m_fogUW.m_density;
                fogStart = info.m_fog.m_start;
                fogColor = info.m_fog.m_color;
                objUWStart = info.m_mapObjFogUW.m_start;
                objUWEnd = info.m_mapObjFogUW.m_end;
                objUWRate = info.m_mapObjFogUW.m_density;
                objEnd = info.m_mapObjFog.m_end;
            }
        } else {
            copyFog = 1;
        }
    } else {
        objUWStart = info.m_mapObjFogUW.m_start;
        objUWEnd = info.m_mapObjFogUW.m_end;
        objUWRate = info.m_mapObjFogUW.m_density;
        objEnd = info.m_mapObjFog.m_end;
    }

    if (copyFog) {
        objUWRate = uwRate;
        info.m_mapObjFog.m_color = fogColor;
        info.m_mapObjFog.m_start = fogStart;
        info.m_mapObjFog.m_end = fogEnd;
        info.m_mapObjFog.m_density = fogRate;
        info.m_mapObjFogUW.m_color = info.m_fogUW.m_color;
        info.m_mapObjFogUW.m_start = uwStart;
        info.m_mapObjFogUW.m_end = uwEnd;
        info.m_mapObjFogUW.m_density = uwRate;
        objUWStart = uwStart;
        objUWEnd = uwEnd;
        objEnd = fogEnd;
    }

    float blend = info.m_interiorFogDistance * 0.039999999f;

    if (blend < 0.0f) {
        blend = 0.0f;
    } else if (blend >= 1.0f) {
        blend = 1.0f;
    }

    info.m_interiorFogBlend = blend;
    float intUWRate;

    if (info.m_interiorFogEnabled && info.m_interiorFogBlendEnabled) {
        info.m_intFog.m_end = (objEnd - info.m_fog.m_end) * blend + info.m_fog.m_end;
        info.m_intFog.m_start = (info.m_mapObjFog.m_start - fogStart) * blend + fogStart;
        info.m_intFog.m_density = (info.m_mapObjFog.m_density - info.m_fog.m_density) * blend + info.m_fog.m_density;

        uint8_t colorBlend = static_cast<uint8_t>(static_cast<int32_t>(blend * 255.0f));
        CImVector intColor = fogColor;

        if (colorBlend) {
            intColor.InterpColor(colorBlend, &info.m_mapObjFog.m_color);
        }

        info.m_intFogUW.m_end = (objUWEnd - info.m_fogUW.m_end) * blend + info.m_fogUW.m_end;
        intUWRate = (objUWRate - uwRate) * blend + uwRate;
        info.m_intFog.m_color = intColor;
        info.m_intFogUW.m_start = (objUWStart - info.m_fogUW.m_start) * blend + info.m_fogUW.m_start;
        info.m_intFogUW.m_density = intUWRate;

        CImVector intUWColor = info.m_fogUW.m_color;

        if (colorBlend) {
            intUWColor.InterpColor(colorBlend, &info.m_mapObjFogUW.m_color);
            intUWRate = info.m_intFogUW.m_density;
        }

        info.m_fog.m_end = info.m_intFog.m_end;
        info.m_fog.m_start = info.m_intFog.m_start;
        info.m_fog.m_density = info.m_intFog.m_density;
        uwRate = intUWRate;
        info.m_intFogUW.m_color = intUWColor;
        info.m_fogUW.m_start = info.m_intFogUW.m_start;
        info.m_fogUW.m_end = info.m_intFogUW.m_end;
        info.m_fogUW.m_density = intUWRate;
    } else {
        info.m_intFog.m_end = info.m_fog.m_end;
        info.m_intFog.m_density = info.m_fog.m_density;
        info.m_intFog.m_color = fogColor;
        info.m_intFogUW.m_start = info.m_fogUW.m_start;
        info.m_intFogUW.m_end = info.m_fogUW.m_end;
        intUWRate = uwRate;
        info.m_intFog.m_start = fogStart;
        info.m_intFogUW.m_color = info.m_fogUW.m_color;
        info.m_intFogUW.m_density = uwRate;
    }

    if (info.m_fogLinear == 1) {
        info.m_fogUW.m_density = uwRate * 2.0f;
        info.m_intFogUW.m_density = intUWRate * 2.0f;
    }
}

// OFFSET: 0x006486C0
void DayNight::SetDirection() {
    static C2Vector s_phiTable[4] = {
        C2Vector(0.0f, 2.2165682f),
        C2Vector(0.25f, 1.9198622f),
        C2Vector(0.5f, 2.2165682f),
        C2Vector(0.75f, 1.9198622f),
    };

    static C2Vector s_thetaTable[4] = {
        C2Vector(0.0f, 3.926991f),
        C2Vector(0.25f, 3.926991f),
        C2Vector(0.5f, 3.926991f),
        C2Vector(0.75f, 3.926991f),
    };

    float phi = DayNight::InterpTable(s_phiTable, 4, DayNight::g_dnInfo.m_dayFraction);
    float theta = DayNight::InterpTable(s_thetaTable, 4, DayNight::g_dnInfo.m_dayFraction);

    float sinPhi = CMath::sin_(phi);
    float cosTheta = CMath::cos_(theta);
    float sinTheta = CMath::sin_(theta);
    float cosPhi = CMath::cos_(phi);

    float x = cosTheta * sinPhi;
    float y = sinTheta * sinPhi;
    float z = cosPhi;

    DayNight::g_dnInfo.m_lightInfo.m_direction.x = x;
    DayNight::g_dnInfo.m_lightInfo.m_direction.y = y;
    DayNight::g_dnInfo.m_lightInfo.m_direction.z = z;
    DayNight::g_dnInfo.m_lightInfoUW.m_direction.x = x;
    DayNight::g_dnInfo.m_lightInfoUW.m_direction.y = y;
    DayNight::g_dnInfo.m_lightInfoUW.m_direction.z = z;
}

// OFFSET: 0x006489B0/0x1402E5690
void DayNight::SetPlanets() {
    static C2Vector s_sunTheta[5] = {
        C2Vector(0.22916667f, 1.7453293f),
        C2Vector(0.49652779f, 0.087266460f),
        C2Vector(0.5f, 0.087266460f),
        C2Vector(0.50347221f, 0.087266460f),
        C2Vector(0.89583331f, 1.7453293f),
    };

    static C2Vector s_sunPhi[3] = {
        C2Vector(0.22916667f, 0.78539819f),
        C2Vector(0.5f, 0.78539819f),
        C2Vector(0.89583331f, 0.78539819f),
    };

    static C2Vector s_moonTheta[5] = {
        C2Vector(0.0f, 0.61086524f),
        C2Vector(0.0034722222f, 0.61086524f),
        C2Vector(0.16666667f, 1.7453293f),
        C2Vector(0.91666669f, 1.7453293f),
        C2Vector(0.99652779f, 0.61086524f),
    };

    static C2Vector s_moonPhi[3] = {
        C2Vector(0.0f, 0.78539819f),
        C2Vector(0.16666667f, 0.78539819f),
        C2Vector(0.91666669f, 0.78539819f),
    };

    static C2Vector s_moon2Theta[5] = {
        C2Vector(0.0f, 0.61086524f),
        C2Vector(0.0034722222f, 0.61086524f),
        C2Vector(0.16666667f, 1.7453293f),
        C2Vector(0.91666669f, 1.7453293f),
        C2Vector(0.99652779f, 0.61086524f),
    };

    static C2Vector s_moon2Phi[3] = {
        C2Vector(0.0f, 2.3561945f),
        C2Vector(0.16666667f, 2.6179938f),
        C2Vector(0.91666669f, 2.8797932f),
    };

    static C2Vector s_sunScale[4] = {
        C2Vector(0.0f, 2.0f),
        C2Vector(0.28125f, 1.0f),
        C2Vector(0.84375f, 1.0f),
        C2Vector(0.875f, 2.0f),
    };

    static C2Vector s_moonScale[4] = {
        C2Vector(0.041666672f, 1.0f),
        C2Vector(0.16666667f, 1.5f),
        C2Vector(0.91666669f, 1.5f),
        C2Vector(0.99930561f, 1.0f),
    };

    DNInfo& info = DayNight::g_dnInfo;

    float theta = DayNight::InterpTable(s_sunTheta, 5, info.m_dayFraction);
    float phi = DayNight::InterpTable(s_sunPhi, 3, info.m_dayFraction);
    float sinTheta = CMath::sin_(theta);
    float cosTheta = CMath::cos_(theta);
    float sinPhi = CMath::sin_(phi);
    float cosPhi = CMath::cos_(phi);
    float x = cosPhi * sinTheta;
    float y = sinPhi * sinTheta;
    float scale = 12.0f / sqrt(x * x + cosTheta * cosTheta + y * y);

    DayNight::g_planets[0].m_position.y = info.m_camPos.y + y * scale;
    DayNight::g_planets[0].m_position.x = info.m_camPos.x + x * scale;
    DayNight::g_planets[0].m_position.z = info.m_camPos.z + cosTheta * scale;
    DayNight::g_planets[0].m_scale = DayNight::InterpTable(s_sunScale, 4, info.m_dayFraction) * DayNight::g_planets[0].m_baseScale;
    DayNight::g_sunGlare.m_position = DayNight::g_planets[0].m_position;

    theta = DayNight::InterpTable(s_moonTheta, 5, info.m_dayFraction);
    phi = DayNight::InterpTable(s_moonPhi, 3, info.m_dayFraction);
    sinTheta = CMath::sin_(theta);
    cosTheta = CMath::cos_(theta);
    sinPhi = CMath::sin_(phi);
    cosPhi = CMath::cos_(phi);
    x = cosPhi * sinTheta;
    y = sinPhi * sinTheta;
    scale = 12.0f / sqrt(cosTheta * cosTheta + y * y + x * x);

    DayNight::g_planets[1].m_position.x = x * scale + info.m_camPos.x;
    DayNight::g_planets[1].m_position.y = y * scale + info.m_camPos.y;
    DayNight::g_planets[1].m_position.z = cosTheta * scale + info.m_camPos.z;

    float moonScale = DayNight::InterpTable(s_moonScale, 4, info.m_dayFraction);
    DayNight::g_moonGlare.m_position = DayNight::g_planets[1].m_position;
    DayNight::g_planets[1].m_scale = DayNight::g_planets[1].m_baseScale * moonScale;
    DayNight::g_moonGlare.m_scaleMin = DayNight::g_planets[1].m_baseScale * moonScale;
    DayNight::g_moonGlare.m_scaleMax = DayNight::g_planets[1].m_baseScale * moonScale;

    uint32_t now = static_cast<uint32_t>(info.m_dayFraction * 65536.0f) + static_cast<uint32_t>(info.m_timeOfDay * 65536.0f);
    float invPeriod = 1.0f / DayNight::g_planets[2].m_period;
    uint32_t start = static_cast<uint32_t>(floor(invPeriod * (info.m_dayFraction + info.m_timeOfDay)) * DayNight::g_planets[2].m_period * 65536.0);

    if (start >= now) {
        start = now;
    }

    float t = static_cast<float>(now - start) * 0.000015258789f * invPeriod;

    theta = DayNight::InterpTable(s_moon2Theta, 5, t);
    phi = DayNight::InterpTable(s_moon2Phi, 3, t);
    sinTheta = CMath::sin_(theta);
    cosTheta = CMath::cos_(theta);
    sinPhi = CMath::sin_(phi);
    cosPhi = CMath::cos_(phi);
    x = cosPhi * sinTheta;
    y = sinPhi * sinTheta;
    scale = 12.0f / sqrt(y * y + x * x + cosTheta * cosTheta);

    DayNight::g_planets[2].m_position.x = x * scale + info.m_camPos.x;
    DayNight::g_planets[2].m_position.y = y * scale + info.m_camPos.y;
    DayNight::g_planets[2].m_position.z = scale * cosTheta + info.m_camPos.z;
    DayNight::g_planets[2].m_scale = DayNight::InterpTable(s_moonScale, 4, t) * DayNight::g_planets[2].m_baseScale;

    float dayFraction = info.m_dayFraction;

    if (dayFraction >= 0.22916667f && dayFraction < 0.5f) {
        info.m_celestialFade = (dayFraction - 0.22916667f) * 3.6923077f;
    } else if (dayFraction >= 0.5f && dayFraction < 0.89583331f) {
        info.m_celestialFade = 1.0f - (dayFraction - 0.5f) * 2.5263159f;
    } else if (dayFraction >= 0.91666669f && dayFraction < 1.0f) {
        info.m_celestialFade = (dayFraction - 0.91666669f) * 12.000003f;
    } else if (dayFraction >= 0.0f && dayFraction < 0.16666667f) {
        info.m_celestialFade = 1.0f - dayFraction * 6.0f;
    } else {
        info.m_celestialFade = 0.0f;
    }
}

// OFFSET: 0x00649640
void DayNight::SetLightColors() {
    LightInfo& info = DayNight::g_dnInfo.m_lightInfo;
    LightInfo& infoUW = DayNight::g_dnInfo.m_lightInfoUW;
    const CurrentLight& light = DayNight::g_dnInfo.m_light;
    const CurrentLight& lightUW = DayNight::g_dnInfo.m_lightUW;

    info.m_ambientColor = light.m_colors[0];
    infoUW.m_ambientColor = lightUW.m_colors[0];
    infoUW.m_directColor = lightUW.m_colors[1];
    info.m_directColor = light.m_colors[1];

    info.m_midColor1 = BlendHalf(light.m_colors[0], light.m_colors[1]);
    info.m_midColor1 = Brighten(info.m_midColor1);
    info.m_midColor0 = BlendHalf(light.m_colors[1], light.m_colors[0]);

    DayNight::g_dnInfo.m_minimapLight[0].m_direction = info.m_direction;
    DayNight::g_dnInfo.m_minimapLight[0].m_directColor = light.m_colors[1];

    infoUW.m_midColor1 = BlendHalf(lightUW.m_colors[0], lightUW.m_colors[1]);
    infoUW.m_midColor1 = Brighten(infoUW.m_midColor1);
    infoUW.m_midColor0 = BlendHalf(lightUW.m_colors[1], lightUW.m_colors[0]);

    DayNight::g_dnInfo.m_minimapLight[0].m_ambientColor = light.m_colors[0];
    DayNight::g_dnInfo.m_minimapLight[0].m_midColor0 = info.m_midColor0;
    DayNight::g_dnInfo.m_minimapLight[0].m_midColor1 = info.m_midColor1;
    memcpy(DayNight::g_dnInfo.m_minimapLight[0].m_desaturate, info.m_desaturate, sizeof(info.m_desaturate));
    DayNight::g_dnInfo.m_minimapLight[1].m_direction = infoUW.m_direction;
    DayNight::g_dnInfo.m_minimapLight[1].m_midColor1 = infoUW.m_midColor1;
    DayNight::g_dnInfo.m_minimapLight[1].m_desaturate[0] = infoUW.m_desaturate[0];
    DayNight::g_dnInfo.m_minimapLight[1].m_desaturate[2] = infoUW.m_desaturate[2];
    DayNight::g_dnInfo.m_minimapLight[1].m_directColor = lightUW.m_colors[1];
    DayNight::g_dnInfo.m_minimapLight[1].m_ambientColor = lightUW.m_colors[0];
    DayNight::g_dnInfo.m_minimapLight[1].m_desaturate[1] = infoUW.m_desaturate[1];
    DayNight::g_dnInfo.m_minimapLight[1].m_desaturate[3] = infoUW.m_desaturate[3];

    CImVector shadow = light.m_colors[0];
    shadow.b = static_cast<uint8_t>((85 * (light.m_colors[0].b + 3)) >> 8);
    shadow.g = static_cast<uint8_t>((85 * (light.m_colors[0].g + 3)) >> 8);
    shadow.r = static_cast<uint8_t>((85 * (light.m_colors[0].r + 3)) >> 8);
    shadow.a = light.m_colors[2].r;
    DayNight::g_dnInfo.m_shadowColor = shadow;

    C3Vector rgb(light.m_colors[0].r * 0.0039215689f, light.m_colors[0].g * 0.0039215689f, light.m_colors[0].b * 0.0039215689f);
    DayNight::g_dnInfo.m_minimapLight[1].m_midColor0 = infoUW.m_midColor0;

    C3Vector hsv(0.0f, 0.0f, 0.0f);
    RGBtoHSV(rgb, hsv);
    hsv.y = hsv.y * 0.33000001f;
    hsv.z = hsv.z * 1.25f;
    HSVtoRGB(hsv, rgb);

    info.m_desaturate[0] = rgb.x;
    info.m_desaturate[1] = rgb.y;
    info.m_desaturate[2] = rgb.z;
    info.m_desaturate[3] = 1.0f;
    infoUW.m_desaturate[0] = rgb.x;
    infoUW.m_desaturate[1] = rgb.y;
    infoUW.m_desaturate[2] = rgb.z;
    infoUW.m_desaturate[3] = 1.0f;
}

// OFFSET: 0x0064A580
void DayNight::SetInteriorFog(bool inside, const SMOFog& fog, bool blend, float distance) {
    DayNight::g_dnInfo.m_interiorFogEnabled = inside;
    memcpy(DayNight::g_dnInfo.m_interiorFog, &fog, sizeof(DayNight::g_dnInfo.m_interiorFog));
    DayNight::g_dnInfo.m_interiorFogBlendEnabled = blend;
    DayNight::g_dnInfo.m_interiorFogDistance = inside ? distance : 0.0f;
}

// OFFSET: 0x0064A640
void DayNight::UpdateLighting() {
    float x = DayNight::g_dnInfo.m_camDir.x;
    float y = DayNight::g_dnInfo.m_camDir.y;
    float facing;

    if (y * y + x * x <= 0.0001f) {
        facing = atan2(DayNight::g_dnInfo.m_camDir.z, DayNight::g_dnInfo.m_camDir.x);
    } else {
        facing = atan2(y, x);
    }

    DayNight::g_dnInfo.m_facing = facing;

    if (DayNight::g_dnInfo.m_facing < 0.0f) {
        DayNight::g_dnInfo.m_facing = DayNight::g_dnInfo.m_facing + 6.2831855f;
    }

    if (DayNight::g_dnInfo.m_stormPercentage * 4.0f <= 1.0f) {
        DayNight::g_dnInfo.m_stormBlend = DayNight::g_dnInfo.m_stormPercentage * 4.0f;
    } else {
        DayNight::g_dnInfo.m_stormBlend = 1.0f;
    }

    DayNight::SetColors();
    DayNight::SetDirection();
    DayNight::SetPlanets();
}

// OFFSET: 0x0064A6F0
void DayNight::Update(uint32_t timeMs, float tickSec, float timeSec) {
    DayNight::g_dnInfo.m_tickSec = tickSec;
    DayNight::g_dnInfo.m_timeSec = timeSec;
    DayNight::g_dnInfo.m_timeMs = timeMs;
    DayNight::UpdateZoneLights();

    int32_t fogMode = g_theGxDevicePtr->m_caps.m_shaderTargets[0];
    int32_t linear = 1;

    if (!DayNight::g_dnInfo.m_enableFogMode || !fogMode || fogMode == 3) {
        linear = 0;
    }

    DayNight::g_dnInfo.m_fogLinear = linear;
    DayNight::g_dnInfo.ClearZoneLights();
    DayNight::FindAndAddZoneLight();

    if (DayNight::g_forceFullUpdate) {
        DayNight::g_clouds.SetFullUpdate();
    }

    DayNight::UpdateLighting();
    DayNight::g_clouds.Update();
    DayNight::g_stars.Update();
    DayNight::UpdateFog();
    DayNight::g_forceFullUpdate = 0;
}
