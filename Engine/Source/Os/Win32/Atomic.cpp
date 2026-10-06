#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <intrin.h>
#include "Engine/Source/Os/Atomic.h"

// OFFSET: 0x004C5E30
int64_t OsInterlockedAdd64(volatile int64_t* value, int64_t delta) {
    int64_t old;

    do {
        old = *value;
    } while (_InterlockedCompareExchange64(value, old + delta, old) != old);

    return old;
}

#endif
