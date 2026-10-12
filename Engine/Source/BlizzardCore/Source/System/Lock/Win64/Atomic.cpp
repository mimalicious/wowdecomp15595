#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// OFFSET: 0x0054D5E0/0x1401A7610
int32_t Blizzard::Lock::Atomic::Increment() {
    return InterlockedIncrement(reinterpret_cast<volatile LONG*>(&this->m_value));
}

// OFFSET: 0x007AD480/0x1404A3180
int32_t Blizzard::Lock::Atomic::Decrement() {
    return InterlockedDecrement(reinterpret_cast<volatile LONG*>(&this->m_value));
}

#endif
