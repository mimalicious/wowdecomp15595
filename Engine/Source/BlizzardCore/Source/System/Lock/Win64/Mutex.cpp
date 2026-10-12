#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// PROVEN: aa0a4d23 IDENTICAL/NONE
// OFFSET: 0x007AD510/0x1404A31F0
void Blizzard::Lock::MutexCreate(Mutex& mutex) {
    InitializeCriticalSection(&mutex);
}

// PROVEN: a2f7d514 IDENTICAL/DIFFERENT
// OFFSET: 0x007AD520/0x1404ED600
void Blizzard::Lock::MutexEnter(Mutex& mutex) {
    EnterCriticalSection(&mutex);
}

// PROVEN: 381b11c5 IDENTICAL/NONE
// OFFSET: 0x007AD530/0x140495980
void Blizzard::Lock::MutexLeave(Mutex& mutex) {
    LeaveCriticalSection(&mutex);
}

// PROVEN: dee200ef IDENTICAL/IDENTICAL
// OFFSET: 0x007AD540/0x1404A3200
void Blizzard::Lock::MutexDestroy(Mutex& mutex) {
    DeleteCriticalSection(&mutex);
    mutex.DebugInfo = NULL;
}

// INLINED
Blizzard::Lock::ScopedMutex::ScopedMutex(Mutex& mutex) {
    this->m_mutex = &mutex;
    MutexEnter(mutex);
}

// INLINED
Blizzard::Lock::ScopedMutex::~ScopedMutex() {
    MutexLeave(*this->m_mutex);
}

// INLINED
Blizzard::Lock::RMutex::RMutex() {
    MutexCreate(this->m_mutex);
}

// INLINED
Blizzard::Lock::RMutex::~RMutex() {
    MutexDestroy(this->m_mutex);
}

#endif
