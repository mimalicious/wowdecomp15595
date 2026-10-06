#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Os/TimeManager.h"

// STATIC: 0x00EC7498
uint32_t FrameTime::s_curTimeMs;

// STATIC: 0x00EC749C
float FrameTime::s_curTimeSec;

// STATIC: 0x00EC74A0
uint32_t FrameTime::s_tickTimeMs;

// STATIC: 0x00EC74A4
float FrameTime::s_tickTimeSec;

// STATIC: 0x00EC74AC
uint8_t FrameTime::s_unk_EC74AC;

// STATIC: 0x00EC74B0
uint32_t FrameTime::s_sessionStartMs;

// STATIC: 0x00EC74B4
uint32_t FrameTime::s_unk_EC74B4;

// PROVEN: 937890b9 IDENTICAL
// OFFSET: 0x008A09B0
uint32_t FrameTime::GetCurTimeMs() {
    return FrameTime::s_curTimeMs;
}

// PROVEN: ecb30c45 IDENTICAL
// OFFSET: 0x008A09C0
uint32_t FrameTime::GetTickTimeMs() {
    return FrameTime::s_tickTimeMs;
}

// PROVEN: 4189b633 IDENTICAL
// OFFSET: 0x008A09D0
float FrameTime::GetCurTimeSec() {
    return FrameTime::s_curTimeSec;
}

// PROVEN: 6e7e54d3 IDENTICAL
// OFFSET: 0x008A09E0
float FrameTime::GetTickTimeSec() {
    return FrameTime::s_tickTimeSec;
}

// PROVEN: 16c4d999 IDENTICAL
// OFFSET: 0x008A09F0
uint32_t FrameTime::GetSessionTimeMs() {
    return FrameTime::s_curTimeMs - FrameTime::s_sessionStartMs;
}

// PROVEN: c2318d4e IDENTICAL
// OFFSET: 0x008A0A50
void FrameTime::Update(float elapsedSec, uint32_t time) {
    FrameTime::s_curTimeMs = time;
    FrameTime::s_curTimeSec = static_cast<float>(time) * 0.001f;
    FrameTime::s_tickTimeMs = static_cast<uint32_t>(elapsedSec * 1000.0f);
    FrameTime::s_tickTimeSec = elapsedSec;
}

// PROVEN: b04cd628 IDENTICAL
// OFFSET: 0x008A0AC0
void FrameTime::Initialize(uint32_t time) {
    FrameTime::Update(0.0f, time);
    uint32_t now = OsGetAsyncTimeMs();
    FrameTime::s_sessionStartMs = now;
    FrameTime::s_unk_EC74B4 = now;
    FrameTime::s_unk_EC74AC = 0;
}
