#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <ctime>
#include <windows.h>
#include "Engine/Source/Os/Time.h"

// OFFSET: %0x007DE2E0
int32_t OsGetTimeZone() {
    TIME_ZONE_INFORMATION information;

    if (GetTimeZoneInformation(&information) == TIME_ZONE_ID_INVALID) {
        return 0;
    }

    return -information.Bias;
}

// PROVEN: d8340f34 IDENTICAL/DIFFERENT
// OFFSET: 0x0087E930/0x1405B7910
void OsGetTimeStamp(char* buffer, uint32_t size) {
    __time32_t now;
    _time32(&now);
    strftime(buffer, size, "%m%d%y_%H%M%S", _localtime32(&now));
}

#endif
