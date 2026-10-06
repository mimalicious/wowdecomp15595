#include <cstdint>
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"

// PROVEN: 1a813b49 IDENTICAL
// OFFSET: 0x0087E8C0
float OsGetAsyncTimeSec() {
    return static_cast<float>(OsGetAsyncTimeMs()) * 0.001f;
}

// OFFSET: %0x007DE2C0
void OsTimeStartup(OsTimeManager::TimingMethod method) {
    OsTimeManagerCreate(method);
}

// PROVEN: 6d280427 IDENTICAL
// OFFSET: 0x00871FE0
void OsTimeShutdown() {
}
