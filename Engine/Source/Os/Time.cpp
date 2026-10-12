#include <cstdint>
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"

// OFFSET: 0x0087E8C0/0x1405B78B0
float OsGetAsyncTimeSec() {
    return static_cast<float>(OsGetAsyncTimeMs()) * 0.001f;
}

// INLINED
void OsTimeStartup(OsTimeManager::TimingMethod method) {
    OsTimeManagerCreate(method);
}

// OFFSET: 0x00871FE0/0x140581DE0
void OsTimeShutdown() {
}
