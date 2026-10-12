#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <intrin.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// OFFSET: 0x0054D5E0/0x1401A7610
int32_t Blizzard::Lock::Atomic::Increment() {
    return _InterlockedIncrement(reinterpret_cast<volatile long*>(&this->m_value));
}

// OFFSET: 0x007AD480/0x1404A3180
int32_t Blizzard::Lock::Atomic::Decrement() {
    return _InterlockedDecrement(reinterpret_cast<volatile long*>(&this->m_value));
}

#endif
