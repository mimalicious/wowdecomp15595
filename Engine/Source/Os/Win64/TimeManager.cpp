#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D0A8C8/0x140BBC8F0
static OsTimeManager s_OsTimeMgr;

// INLINED
static int64_t OsTimeReadClocks(OsTimeManager::TimingMethod method) {
    switch (method) {
        case OsTimeManager::SystemMethod1:
            return GetTickCount();

        case OsTimeManager::SystemMethod2: {
            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);
            return count.QuadPart;
        }

        default:
            return GetTickCount();
    }
}

// INLINED
static int64_t OsTimeClocksPerSecond(OsTimeManager::TimingMethod method, int64_t performanceFrequency) {
    switch (method) {
        case OsTimeManager::SystemMethod1:
            return 1000;

        case OsTimeManager::SystemMethod2:
            return performanceFrequency;

        default:
            return 1000;
    }
}

// INLINED
OsTimeManager::OsTimeManager() {
    this->m_timingMethod = SystemMethod1;
    this->m_timingTestError = 0;
    this->m_performanceFrequency = 1000;
    this->m_scaleToMs = 1.0;
    this->m_timeBegin = 0.0;
    this->m_lastTime = 0;
}

// PROVEN: fab6fdc8 IDENTICAL/DIFFERENT
// OFFSET: 0x00879290/0x1405B2210
OsTimeManager::TimingMethod OsTimeManager::Calibrate() {
    if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&this->m_performanceFrequency))) {
        this->m_timingTestError = 1;
        return SystemMethod1;
    }

    if (!this->m_performanceFrequency) {
        this->m_timingTestError = 2;
        return SystemMethod1;
    }

    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    DWORD priorityClass = GetPriorityClass(process);
    int32_t threadPriority = GetThreadPriority(thread);
    SetPriorityClass(process, HIGH_PRIORITY_CLASS);
    SetThreadPriority(thread, THREAD_PRIORITY_TIME_CRITICAL);
    OsSleep(0);

    this->m_timingTestError = 0;

    DWORD tick = GetTickCount();
    while (tick == GetTickCount()) {
    }

    LARGE_INTEGER startCount;
    QueryPerformanceCounter(&startCount);

    uint32_t processors = OsGetNumberOfProcessors();
    if (processors > 1) {
        int64_t lastCount = startCount.QuadPart;

        DWORD_PTR processAffinity;
        DWORD_PTR systemAffinity;
        GetProcessAffinityMask(process, &processAffinity, &systemAffinity);

        for (uint32_t i = 0; i < 512; i++) {
            SetThreadAffinityMask(thread, 1 << (i % processors));
            OsSleep(0);

            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);

            if (count.QuadPart <= lastCount) {
                this->m_timingTestError = 4;
                break;
            }

            lastCount = count.QuadPart;
        }

        SetThreadAffinityMask(thread, processAffinity);
        OsSleep(0);
    }

    SetPriorityClass(process, priorityClass);
    SetThreadPriority(thread, threadPriority);
    OsSleep(0);

    return this->m_timingTestError ? SystemMethod1 : SystemMethod2;
}

// PROVEN: 369eb92f IDENTICAL/UNPROVEN
// OFFSET: 0x00879420/0x1405B23D0
uint32_t OsTimeGetTestError() {
    return s_OsTimeMgr.m_timingTestError;
}

// PROVEN: 53bd9f74 IDENTICAL/UNPROVEN
// OFFSET: 0x00879430/0x1405B23E0
OsTimeManager::TimingMethod OsTimeGetTimingMethod() {
    return s_OsTimeMgr.m_timingMethod;
}

// PROVEN: 6a504ed0 IDENTICAL/IDENTICAL
// OFFSET: 0x00879440/0x1405B23F0
const char* GetTimingMethodName(OsTimeManager::TimingMethod method) {
    switch (method) {
        case OsTimeManager::NotSet:
            return "[Not Set]";

        case OsTimeManager::BestAvailable:
            return "[Best Available]";

        case OsTimeManager::SystemMethod1:
            return "GetTickCount";

        case OsTimeManager::SystemMethod2:
            return "QueryPerformanceCounter";

        default:
            return "[Unknown]";
    }
}

// OFFSET: 0x00879490
uint32_t OsTimeManager::SnapshotPrecise() {
    this->m_lock.Enter();
    OsTimeManager::TimingMethod method = this->m_timingMethod;
    double scaleToMs = this->m_scaleToMs;
    double timeBegin = this->m_timeBegin;
    this->m_lock.Leave();

    uint32_t current = static_cast<uint32_t>(static_cast<double>(OsTimeReadClocks(method)) * scaleToMs + timeBegin);
    if (static_cast<int32_t>(current - this->m_lastTime) < 0) {
        return this->m_lastTime;
    }

    return current;
}

// PROVEN: 93d71ede IDENTICAL/UNPROVEN
// OFFSET: 0x00879530/0x1405B2430
uint32_t OsGetAsyncTimeMsPrecise() {
    return s_OsTimeMgr.SnapshotPrecise();
}

// OFFSET: 0x00879540/0x1405B24D0
int64_t OsGetAsyncClocksPerSecond() {
    return OsTimeClocksPerSecond(s_OsTimeMgr.m_timingMethod, s_OsTimeMgr.m_performanceFrequency);
}

// OFFSET: 0x00879570
uint32_t OsTimeManager::Snapshot() {
    uint32_t current = static_cast<uint32_t>(static_cast<double>(OsTimeReadClocks(this->m_timingMethod)) * this->m_scaleToMs + this->m_timeBegin);
    if (static_cast<int32_t>(current - this->m_lastTime) < 0) {
        return this->m_lastTime;
    }

    this->m_lastTime = current;
    return current;
}

// OFFSET: 0x008795F0
void OsTimeManager::SetTimingMethod(OsTimeManager::TimingMethod method) {
    this->m_lock.Enter();
    this->m_timingMethod = method;
    this->m_scaleToMs = 1000.0 / static_cast<double>(OsTimeClocksPerSecond(method, this->m_performanceFrequency));
    this->m_timeBegin = 0.0;
    this->m_lastTime = static_cast<uint32_t>(static_cast<double>(OsTimeReadClocks(method)) * this->m_scaleToMs);
    this->m_lock.Leave();
}

// OFFSET: 0x008796B0
void OsTimeManager::ScaleTime(float scale) {
    double now = static_cast<double>(OsTimeReadClocks(this->m_timingMethod));
    double scaleToMs = scale * this->m_scaleToMs;
    double timeBegin = now * this->m_scaleToMs + this->m_timeBegin + this->m_timeBegin - (now * scaleToMs + this->m_timeBegin);

    this->m_lock.Enter();
    this->m_timeBegin = timeBegin;
    this->m_scaleToMs = scaleToMs;
    this->m_lock.Leave();
}

// PROVEN: 7ee1a284 IDENTICAL/DIFFERENT
// OFFSET: 0x00879740/0x1405B2500
uint32_t OsGetAsyncTimeMs() {
    return s_OsTimeMgr.Snapshot();
}

// PROVEN: 83b6cd6f IDENTICAL/UNPROVEN
// OFFSET: 0x00879750/0x1405B2570
uint64_t OsGetAsyncTimeClocks() {
    return OsTimeReadClocks(s_OsTimeMgr.m_timingMethod);
}

// PROVEN: 2b3e040f IDENTICAL/NONE
// OFFSET: 0x00879790
void OsTimeScaleTime(float scale) {
    s_OsTimeMgr.ScaleTime(scale);
}

// OFFSET: 0x008797B0/0x1405B2670
double OsTimeGetCurrentScale() {
    return static_cast<double>(OsTimeClocksPerSecond(s_OsTimeMgr.m_timingMethod, s_OsTimeMgr.m_performanceFrequency)) * s_OsTimeMgr.m_scaleToMs * 0.001;
}

// PROVEN: edd0240f IDENTICAL/DIFFERENT
// OFFSET: 0x00879810/0x1405B26B0
void OsTimeManagerCreate(OsTimeManager::TimingMethod method) {
    s_OsTimeMgr.m_timingMethod = OsTimeManager::NotSet;

    OsTimeManager::TimingMethod calibrated = s_OsTimeMgr.Calibrate();
    if (method != OsTimeManager::BestAvailable && calibrated != method) {
        if (method == OsTimeManager::SystemMethod2 && calibrated == OsTimeManager::SystemMethod1) {
            s_OsTimeMgr.m_timingTestError = 5;
        }

        calibrated = method;
    }

    s_OsTimeMgr.SetTimingMethod(calibrated);
}

#endif
