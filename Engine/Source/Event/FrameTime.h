#ifndef ENGINE_SOURCE_EVENT_FRAMETIME_H
#define ENGINE_SOURCE_EVENT_FRAMETIME_H

#include <cstddef>
#include <cstdint>

class FrameTime {
    public:
    static uint32_t s_curTimeMs;
    static float s_curTimeSec;
    static uint32_t s_tickTimeMs;
    static float s_tickTimeSec;
    static uint8_t s_unk_EC74AC;
    static uint32_t s_sessionStartMs;
    static uint32_t s_unk_EC74B4;

    static uint32_t GetCurTimeMs();
    static uint32_t GetTickTimeMs();
    static float GetCurTimeSec();
    static float GetTickTimeSec();
    static uint32_t GetSessionTimeMs();
    static void Update(float elapsedSec, uint32_t time);
    static void Initialize(uint32_t time);
};

#endif
