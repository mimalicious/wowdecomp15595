#ifndef WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H
#define WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H

#include <cstdint>

class CHMMgr;
struct CVar;
class WeatherRec;

class Weather {
    public:
    static CVar* s_cvarUseWeatherShaders;
    static float s_densityScale;

    static bool DensityCallback(CVar* var, const char* oldValue, const char* value, void* arg);
    static void Destroy();
    static void Initialize();
    static void InitializeCVars();

    /* 0000 */ uint32_t unk_0000[5];
    /* 0014 */ float m_intensity;
    /* 0018 */ uint8_t unk_0018[12];
    /* 0024 */ uint8_t m_densityChanged;
    /* 0025 */ uint8_t unk_0025[291];
    /* 0148 */ CHMMgr* m_hmMgr;
    /* 014C */ uint8_t unk_014C[64];

    Weather();
    ~Weather();
    void Render();
    void Update();
    void SetType(int32_t type, float intensity, const WeatherRec* rec, bool a4, float a5);
};

#endif
