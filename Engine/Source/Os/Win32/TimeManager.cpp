#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include <cstdlib>
#include <new>
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C0FBB8
static OsTimeManager s_OsTimeMgr;

// INLINED
OsTimeManager::OsTimeManager() {
    this->m_scaleToMs = 1.0;
    this->m_timingMethod = SystemMethod1;
    this->m_timingTestError = 0;
    this->m_performanceFrequency = 0;
    this->m_timeBegin = 0.0;
}

// OFFSET: %0x007DB940
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
    DWORD startTick;
    do {
        startTick = GetTickCount();
    } while (tick == startTick);

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

    if (!this->m_timingTestError) {
        DWORD now;
        do {
            now = GetTickCount();
        } while (now - startTick < 250);

        DWORD endTick;
        do {
            endTick = GetTickCount();
        } while (now == endTick);

        LARGE_INTEGER endCount;
        QueryPerformanceCounter(&endCount);

        int64_t elapsed = static_cast<int64_t>(static_cast<double>(endCount.QuadPart - startCount.QuadPart) / static_cast<double>(this->m_performanceFrequency) * 1000.0);
        if (abs(static_cast<int32_t>(endTick - static_cast<uint32_t>(elapsed) - startTick)) >= 5) {
            this->m_timingTestError = 3;
        }
    }

    SetPriorityClass(process, priorityClass);
    SetThreadPriority(thread, threadPriority);
    OsSleep(0);

    return this->m_timingTestError ? SystemMethod1 : SystemMethod2;
}

// OFFSET: %0x007DBB60
uint32_t OsTimeGetTestError() {
    return s_OsTimeMgr.m_timingTestError;
}

// PROVEN: 53bd9f74 UNPROVEN/IDENTICAL
// OFFSET: 0x00879430/0x1405B23E0
OsTimeManager::TimingMethod OsTimeGetTimingMethod() {
    return s_OsTimeMgr.m_timingMethod;
}

// PROVEN: 6a504ed0 DIFFERENT/IDENTICAL
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

// OFFSET: %0x007DBBD0
uint64_t OsTimeManager::Snapshot() {
    switch (this->m_timingMethod) {
        case SystemMethod1:
            break;

        case SystemMethod2: {
            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);
            return static_cast<int64_t>(static_cast<double>(count.QuadPart) * this->m_scaleToMs + this->m_timeBegin);
        }

        default:
            break;
    }

    int64_t ticks = GetTickCount();
    return static_cast<int64_t>(static_cast<double>(ticks) * this->m_scaleToMs + this->m_timeBegin);
}

// OFFSET: %0x007DBC30
uint32_t OsGetAsyncTimeMs() {
    return static_cast<uint32_t>(s_OsTimeMgr.Snapshot());
}

// OFFSET: %0x007DBC40
uint64_t OsGetAsyncTimeClocks() {
    switch (s_OsTimeMgr.m_timingMethod) {
        case OsTimeManager::SystemMethod1:
            break;

        case OsTimeManager::SystemMethod2: {
            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);
            return count.QuadPart;
        }

        default:
            break;
    }

    return GetTickCount();
}

// OFFSET: %0x007DBC80
int64_t OsGetAsyncClocksPerSecond() {
    switch (s_OsTimeMgr.m_timingMethod) {
        case OsTimeManager::SystemMethod1:
            break;

        case OsTimeManager::SystemMethod2:
            return s_OsTimeMgr.m_performanceFrequency;

        default:
            break;
    }

    return 1000;
}

// OFFSET: %0x007DBCB0
OsTimeManager::OsTimeManager(OsTimeManager::TimingMethod method) {
    this->m_timingMethod = NotSet;

    OsTimeManager::TimingMethod calibrated = this->Calibrate();
    if (method != BestAvailable && calibrated != method) {
        if (method == SystemMethod2 && calibrated == SystemMethod1) {
            this->m_timingTestError = 5;
        }

        calibrated = method;
    }

    this->m_timingMethod = calibrated;

    int64_t clocksPerSecond;
    switch (calibrated) {
        case SystemMethod1:
            clocksPerSecond = 1000;
            break;

        case SystemMethod2:
            clocksPerSecond = this->m_performanceFrequency;
            break;

        default:
            clocksPerSecond = 1000;
            break;
    }

    this->m_scaleToMs = 1000.0 / static_cast<double>(clocksPerSecond);
    this->m_timeBegin = 0.0;
}

// OFFSET: %0x007DBD30
void OsTimeManagerCreate(OsTimeManager::TimingMethod method) {
    new (&s_OsTimeMgr) OsTimeManager(method);
}

// PROVEN: d3396581 IDENTICAL/NONE
// OFFSET: 0x008755D0
void OsSleep(uint32_t milliseconds) {
    Sleep(milliseconds);
}

#endif
