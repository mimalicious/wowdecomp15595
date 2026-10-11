#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_TIME_TIME_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_TIME_TIME_H

#include <cstdint>
#include <cstdlib>
#include <new>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"
#include "Engine/Source/Storm/H/storm.h"

namespace System_Time {

class UsingTime : public Blizzard::Using::Use {
    public:
    UsingTime();
};

class TimeInfo {
    public:
    /* 0000 */ double m_scaleNanoseconds;
    /* 0008 */ double m_scaleMicroseconds;
    /* 0010 */ double m_scaleMilliseconds;
    /* 0018 */ double m_scaleSeconds;
    /* 0020 */ bool m_usePerformanceCounter;
    /* 0021 */ uint8_t unk_0021[7];
    /* 0028 */ uint64_t m_scaleCycles;
    /* 0030 */ int64_t m_absBegin;
    /* 0038 */ Blizzard::Time::Timestamp m_gmBegin;

    TimeInfo();
};

extern const int32_t s_daysBeforeMonth[13];
extern Blizzard::Lock::Atomic s_tickLock;
extern uint32_t s_lastTickCount;
extern uint64_t s_tickCountHigh;
extern TimeInfo* s_timeInfo;

bool CalibrateFrequency(LARGE_INTEGER* frequency);
TimeInfo* GetTimeInfo();
uint64_t GetTickCount64();
bool InitFrequency(LARGE_INTEGER* frequency);
double MeasureDrift(int64_t frequency, LARGE_INTEGER* counter, DWORD* tickCount);
void ShutdownProc();

}

namespace Blizzard {
namespace Time {

void BreakTime(Timestamp time, TimeRec& rec);
Timestamp FromFileTime(uint64_t fileTime);
uint32_t GetTimeElapsed(uint32_t start, uint32_t end);
Timestamp GetTimestamp();
Timestamp MakeTime(const TimeRec& rec);
int64_t Milliseconds();
int64_t Seconds();

}
}

#include "Engine/Source/BlizzardCore/Source/System/Time/Time.inl"

#endif
