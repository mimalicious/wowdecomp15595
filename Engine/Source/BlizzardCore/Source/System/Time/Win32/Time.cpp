#if defined(WOWDECOMP_ARCH_32)

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: #0x009E56C0
const int32_t System_Time::s_daysBeforeMonth[13] = { 0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334, 365 };

// STATIC: %0x00D87190
Blizzard::Lock::Atomic System_Time::s_tickLock;

// STATIC: %0x00D87194
uint32_t System_Time::s_lastTickCount;

// STATIC: %0x00D86D40
System_Time::TimeInfo* System_Time::s_timeInfo;

// STATIC: %0x00D87198
uint64_t System_Time::s_tickCountHigh;

// OFFSET: 0x007AE260
Blizzard::Time::Timestamp Blizzard::Time::FromFileTime(uint64_t fileTime) {
    if (fileTime <= 0x0077A5D6AA8BF851) {
        return static_cast<Timestamp>(0x8000000000000000);
    }

    if (fileTime <= 0x030701FFA04E87AE) {
        return static_cast<Timestamp>(fileTime * 100 + 0x5143382561530000);
    }

    return 0x7FFFFFFFFFFFFFFF;
}

// PROVEN: a1ec127e IDENTICAL/NONE
// OFFSET: 0x007AE2E0
double System_Time::MeasureDrift(int64_t frequency, LARGE_INTEGER* counter, DWORD* tickCount) {
    DWORD tick;
    LARGE_INTEGER now;

    do {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        tick = GetTickCount();
        QueryPerformanceCounter(&now);
    } while (tick == *tickCount);

    double counterSeconds = static_cast<double>(now.QuadPart - counter->QuadPart) / frequency;
    double tickSeconds = static_cast<double>(tick - *tickCount);
    counter->QuadPart = now.QuadPart;
    *tickCount = tick;

    return fabs(counterSeconds - tickSeconds * 0.001) / counterSeconds;
}

// OFFSET: 0x007AE370/0x1404A3F80
uint64_t System_Time::GetTickCount64() {
    while (s_tickLock.Increment() != 1) {
        s_tickLock.Decrement();
        Sleep(0);
    }

    DWORD tick = GetTickCount();

    if (tick < s_lastTickCount) {
        s_tickCountHigh += 0x100000000;
    }

    s_lastTickCount = tick;
    s_tickLock.Decrement();

    return s_tickCountHigh + s_lastTickCount;
}

// OFFSET: 0x007AE3F0/0x1404A4010
void Blizzard::Time::BreakTime(Timestamp time, TimeRec& rec) {
    int32_t nanosecond = static_cast<int32_t>(time % 1000000000);

    if (nanosecond < 0) {
        nanosecond += 1000000000;
    }

    int64_t ticks = time / 100 + 125911584000000000;

    if (time < 0 && time % 100) {
        ticks = time / 100 + 125911583999999999;
    }

    FILETIME fileTime;
    fileTime.dwLowDateTime = static_cast<DWORD>(ticks);
    fileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&fileTime, &systemTime);

    rec.day = systemTime.wDay;
    rec.hour = systemTime.wHour;
    rec.minute = systemTime.wMinute;
    rec.second = systemTime.wSecond;
    rec.nanosecond = nanosecond;
    rec.weekday = systemTime.wDayOfWeek;
    rec.year = systemTime.wYear;
    rec.month = systemTime.wMonth;

    bool leap = !(rec.year % 400) || (rec.year % 100 && !(rec.year % 4));
    rec.yearday = System_Time::s_daysBeforeMonth[rec.month - 1] + systemTime.wDay - 1;

    if (leap && rec.month > 2) {
        rec.yearday++;
    }
}

// OFFSET: 0x007AE4F0/0x1404A4160
Blizzard::Time::Timestamp Blizzard::Time::MakeTime(const TimeRec& rec) {
    SYSTEMTIME systemTime;
    systemTime.wYear = static_cast<WORD>(rec.year);
    systemTime.wMonth = static_cast<WORD>(rec.month);
    systemTime.wDay = static_cast<WORD>(rec.day);
    systemTime.wHour = static_cast<WORD>(rec.hour);
    systemTime.wMinute = static_cast<WORD>(rec.minute);
    systemTime.wSecond = static_cast<WORD>(rec.second);
    systemTime.wMilliseconds = 0;

    FILETIME fileTime;
    SystemTimeToFileTime(&systemTime, &fileTime);
    Timestamp time = FromFileTime((static_cast<uint64_t>(fileTime.dwHighDateTime) << 32) | fileTime.dwLowDateTime);

    if (time != static_cast<Timestamp>(0x8000000000000000)) {
        return time + rec.nanosecond;
    }

    int64_t result = static_cast<int64_t>(rec.nanosecond) + 0x7FFFFFFFF7580E00;

    if (result > 0) {
        return static_cast<Timestamp>(0x8000000000000000);
    }

    return result;
}

// OFFSET: 0x007B6190/0x1404AC2A0
bool System_Time::CalibrateFrequency(LARGE_INTEGER* frequency) {
    if (!QueryPerformanceFrequency(frequency) || !frequency->QuadPart) {
        return false;
    }

    DWORD start = GetTickCount();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    DWORD tick;

    do {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        tick = GetTickCount();
        QueryPerformanceCounter(&counter);
    } while (tick == start);

    int32_t attempts = 0;

    while (MeasureDrift(frequency->QuadPart, &counter, &tick) >= 0.1) {
        if (++attempts >= 3) {
            return false;
        }
    }

    return true;
}

// OFFSET: #0x004337C0
bool System_Time::InitFrequency(LARGE_INTEGER* frequency) {
    OSVERSIONINFOW versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionExW(&versionInfo);

    if (versionInfo.dwMajorVersion >= 6) {
        return QueryPerformanceFrequency(frequency) != 0;
    }

    if (versionInfo.dwMajorVersion >= 5 && versionInfo.dwMinorVersion) {
        return CalibrateFrequency(frequency);
    }

    return false;
}

// OFFSET: 0x007C0A10
System_Time::TimeInfo::TimeInfo() {
    LARGE_INTEGER frequency;
    this->m_usePerformanceCounter = InitFrequency(&frequency);

    if (!this->m_usePerformanceCounter) {
        this->m_scaleCycles = 0;
        return;
    }

    this->m_scaleCycles = frequency.QuadPart;

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    this->m_absBegin = counter.QuadPart;

    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    this->m_gmBegin = Blizzard::Time::FromFileTime((static_cast<uint64_t>(fileTime.dwHighDateTime) << 32) | fileTime.dwLowDateTime);

    double scale = 1.0 / static_cast<double>(this->m_scaleCycles);
    this->m_scaleNanoseconds = 1000000000.0 * scale;
    this->m_scaleMicroseconds = 1000000.0 * scale;
    this->m_scaleMilliseconds = 1000.0 * scale;
    this->m_scaleSeconds = scale;
}

// OFFSET: #0x00435D80
Blizzard::Time::Timestamp Blizzard::Time::GetTimestamp() {
    System_Time::TimeInfo* info = System_Time::GetTimeInfo();

    if (info->m_usePerformanceCounter) {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        info = System_Time::s_timeInfo;
        uint64_t elapsed = static_cast<uint64_t>(counter.QuadPart - info->m_absBegin);
        return static_cast<Timestamp>(static_cast<double>(elapsed) * info->m_scaleNanoseconds) + info->m_gmBegin;
    }

    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    return FromFileTime((static_cast<uint64_t>(fileTime.dwHighDateTime) << 32) | fileTime.dwLowDateTime);
}

// OFFSET: #0x00435E20
int64_t Blizzard::Time::Milliseconds() {
    if (!System_Time::GetTimeInfo()->m_usePerformanceCounter) {
        return System_Time::GetTickCount64();
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return static_cast<int64_t>(static_cast<double>(counter.QuadPart) * System_Time::s_timeInfo->m_scaleMilliseconds);
}

// OFFSET: 0x007AE240
uint32_t Blizzard::Time::GetTimeElapsed(uint32_t start, uint32_t end) {
    if (start <= end) {
        return end - start;
    }

    return end - start - 1;
}

// OFFSET: 0x007C0C00/0x1404BA950
int64_t Blizzard::Time::Seconds() {
    if (!System_Time::GetTimeInfo()->m_usePerformanceCounter) {
        return System_Time::GetTickCount64() / 1000;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return static_cast<int64_t>(static_cast<double>(counter.QuadPart) * System_Time::s_timeInfo->m_scaleSeconds);
}

#endif
