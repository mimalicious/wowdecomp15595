#ifndef ENGINE_SOURCE_OS_TIMEMANAGER_H
#define ENGINE_SOURCE_OS_TIMEMANAGER_H

#include <cstdint>
#include "Engine/Source/Storm/Source/SLock.h"

class OsTimeManager {
    public:
    enum TimingMethod {
        NotSet = -1,
        BestAvailable = 0,
        SystemMethod1 = 1,
        SystemMethod2 = 2,
    };

    /* 0000 */ OsTimeManager::TimingMethod m_timingMethod;
    /* 0004 */ uint32_t m_timingTestError;
    /* 0008 */ int64_t m_performanceFrequency;
    /* 0010 */ double m_scaleToMs;
    /* 0018 */ double m_timeBegin;
    /* 0020 */ SCritSect m_lock;
    /* 0048 */ uint32_t m_lastTime;

    OsTimeManager();
    OsTimeManager::TimingMethod Calibrate();
    uint32_t SnapshotPrecise();
    uint32_t Snapshot();
    void SetTimingMethod(OsTimeManager::TimingMethod method);
    void ScaleTime(float scale);
};

uint32_t OsTimeGetTestError();
OsTimeManager::TimingMethod OsTimeGetTimingMethod();
const char* GetTimingMethodName(OsTimeManager::TimingMethod method);
uint32_t OsGetAsyncTimeMsPrecise();
int64_t OsGetAsyncClocksPerSecond();
uint32_t OsGetAsyncTimeMs();
uint64_t OsGetAsyncTimeClocks();
void OsTimeScaleTime(float scale);
double OsTimeGetCurrentScale();
void OsTimeManagerCreate(OsTimeManager::TimingMethod method);
void OsSleep(uint32_t milliseconds);

#endif
