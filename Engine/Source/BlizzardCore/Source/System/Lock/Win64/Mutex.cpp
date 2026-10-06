#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// OFFSET: #0x004283A0
void Blizzard::Lock::MutexCreate(Mutex& mutex) {
    InitializeCriticalSection(&mutex);
}

// PROVEN: a2f7d514 IDENTICAL/NONE
// OFFSET: 0x007AD520
void Blizzard::Lock::MutexEnter(Mutex& mutex) {
    EnterCriticalSection(&mutex);
}

// PROVEN: 381b11c5 IDENTICAL/NONE
// OFFSET: 0x007AD530
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

// OFFSET: #0x0042B3A0
Blizzard::Lock::ScopedMutex::~ScopedMutex() {
    MutexLeave(*this->m_mutex);
}

// INLINED
Blizzard::Lock::RMutex::RMutex() {
    MutexCreate(this->m_mutex);
}

// OFFSET: #0x00421640
Blizzard::Lock::RMutex::~RMutex() {
    MutexDestroy(this->m_mutex);
}

#endif
