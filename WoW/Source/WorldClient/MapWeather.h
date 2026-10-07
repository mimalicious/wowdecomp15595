#ifndef WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H
#define WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/HMMgr.h"

class Rain;
class Sand;
class Snow;
struct CVar;
class WeatherRec;

class Weather {
    public:
    struct PosDelta {
        /* 0000 */ C3Vector m_delta;
        /* 000C */ uint32_t m_time;
        /* 0010 */ TSLink<PosDelta> m_link;
    };

    static CVar* s_cvarUseWeatherShaders;
    static float s_densityScale;
    static uint8_t s_weatherTypeForced;

    static bool DensityCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void Destroy();
    static void Initialize();
    static void InitializeCVars();

    /* 0000 */ float m_targetIntensity;
    /* 0004 */ float m_startIntensity;
    /* 0008 */ float m_intensity;
    /* 000C */ float m_targetFogIntensity;
    /* 0010 */ float m_startFogIntensity;
    /* 0014 */ float m_fogIntensity;
    /* 0018 */ uint32_t m_intensityStartTime;
    /* 001C */ uint32_t m_fogStartTime;
    /* 0020 */ int32_t m_pendingType;
    /* 0024 */ uint8_t m_densityChanged;
    /* 0025 */ uint8_t m_transition;
    /* 0028 */ float m_densityBlend;
    /* 002C */ float m_startDensityBlend;
    /* 0030 */ float m_targetDensityBlend;
    /* 0034 */ CImVector m_color;
    /* 0038 */ char m_precipTexture[260];
    /* 013C */ Rain* m_rain;
    /* 0140 */ Snow* m_snow;
    /* 0144 */ Sand* m_sand;
    /* 0148 */ CHMMgr* m_hmMgr;
    /* 014C */ TSExplicitList<PosDelta, offsetof(PosDelta, m_link)> m_posDeltas;
    /* 0158 */ C3Vector m_velocity;
    /* 0164 */ C3Vector m_position;
    /* 0170 */ int32_t m_onTaxi;
    /* 0174 */ float m_facing;
    /* 0178 */ float m_speed;
    /* 017C */ C3Vector m_transportVelocity;
    /* 0188 */ int32_t m_updated;

    Weather();
    ~Weather();
    void CalcPlayerVelocity(const C3Vector& position, const C3Vector& velocity);
    void Clear();
    void FillInDefaultPrecipTexturePath(int32_t type);
    int32_t GetType() const;
    void Render();
    void SetStormIntensity(float start, float target);
    void SetType(int32_t type, float intensity, const WeatherRec* rec, bool transition, float densityBlend);
    void Update();
};

#endif
