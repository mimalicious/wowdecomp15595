#ifndef ENGINE_SOURCE_OS_ATOMIC_H
#define ENGINE_SOURCE_OS_ATOMIC_H

#include <cstdint>

int64_t OsInterlockedAdd64(volatile int64_t* value, int64_t delta);

#endif
