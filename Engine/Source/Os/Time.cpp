#include <cstdint>
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"

// OFFSET: %0x007DE290
float OsGetAsyncTimeSec() {
    return static_cast<float>(OsGetAsyncTimeMs()) * 0.001f;
}

// OFFSET: %0x007DE2C0
void OsTimeStartup(OsTimeManager::TimingMethod method) {
    OsTimeManagerCreate(method);
}

// OFFSET: %0x007DE2D0
void OsTimeShutdown() {
}
