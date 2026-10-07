#ifndef WOW_SOURCE_WORLDCLIENT_MISTS_H
#define WOW_SOURCE_WORLDCLIENT_MISTS_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CTexture;
class Weather;

class Mists {
    public:
    struct Mist {
        /* 0000 */ uint32_t m_heightCount;
        /* 0004 */ float m_heights[64];
        /* 0104 */ uint32_t m_numHeights;
        /* 0108 */ C3Vector m_pos;
        /* 0114 */ C3Vector m_velocity;
        /* 0120 */ uint32_t m_startTime;
        /* 0124 */ uint32_t m_endTime;
        /* 0128 */ float m_rise;

        Mist();
        Mist(const Mist& source);
        void SetHeights(uint32_t count, const float* heights);
    };

    /* 0000 */ Weather* m_weather;
    /* 0004 */ C3Vector m_extents;
    /* 0010 */ C3Vector unk_0010;
    /* 001C */ float m_size;
    /* 0020 */ float m_spawnRate;
    /* 0024 */ float m_angle;
    /* 0028 */ float m_angleRange;
    /* 002C */ float m_spawnAccum;
    /* 0030 */ float m_fadeTime;
    /* 0034 */ float m_speed;
    /* 0038 */ float m_speedRange;
    /* 003C */ uint8_t m_fadeOut;
    /* 0040 */ CTexture* m_texture;
    /* 0044 */ TSGrowableArray<Mist> m_mists;

    Mists(Weather* weather, float size, const C3Vector& extents, const char* texture, float angle, float angleRange, float speed, float speedRange, uint32_t count);
    ~Mists();
    bool CreateMist(Mist& mist, uint32_t time);
    bool InitMist(Mist& mist, uint32_t time);
    uint32_t NumActive();
    void Render();
};

#endif
