#if defined(WOWDECOMP_ARCH_64)

#include <cstring>
#include <intrin.h>
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "Engine/Source/Storm/H/storm.h"

#define SLOCK_SPIN_NOPS() __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); __nop();

// STATIC: %0x00BB4108
static uint32_t s_spinCount = 1;

// STATIC: %0x00C9DE30
static volatile LONG s_eventCount[2];

// STATIC: %0x00C9DE38
static volatile LONG s_freeIndex[2];

// STATIC: %0x00C9DE40
static volatile LONG s_allocIndex[2];

// STATIC: %0x00C9DE48
static volatile LONG s_eventIds[2][2048];

// STATIC: %0x00CA1E48
static HANDLE s_events[2][1024];

// STATIC: #0x00CB49A0
static int32_t s_refCount;

// STATIC: #0x00CB49A4
static CInitCritSect s_refCritsect;

// STATIC: [#0x00CB49C8, #0x00CB5DC8]
template <class T>
CDebugLockEntry TDebugLock<T>::s_entries[256];

// STATIC: [#0x00CB71C8, #0x00CB71F0]
template <class T>
uint32_t TDebugLock<T>::s_freeEntry;

// STATIC: [#0x00CB71CC, #0x00CB71F4]
template <class T>
CInitCritSect TDebugLock<T>::s_critsect;

// STATIC: [#0x00CB71EC, #0x00CB7214]
template <class T>
CDebugLock* TDebugLock<T>::s_lockList;

// STATIC: %0x00CA66C0
static volatile LONG s_stallCount;

// OFFSET: 0x004BAFC0/0x1400F43C0
void SRWLock::IInitialize() {
    for (uint32_t type = 0; type < 2; type++) {
        for (uint32_t i = 0; i < 1024; i++) {
            HANDLE event = CreateEventA(NULL, type == 1, FALSE, NULL);
            s_eventIds[type][i] = (i + 1) << 21;
            s_events[type][i] = event;
        }

        s_allocIndex[type] = 2047;
        s_freeIndex[type] = 1023;
        s_eventCount[type] = 1024;
    }

    SYSTEM_INFO systemInfo = {};
    GetSystemInfo(&systemInfo);
    s_spinCount = systemInfo.dwNumberOfProcessors > 1 ? 4000 : 1;

    SServerInitialize();
}

// OFFSET: 0x004BB0B0
void SRWLock::IDestroy() {
    for (uint32_t type = 0; type < 2; type++) {
        for (uint32_t i = 0; i < 1024; i++) {
            CloseHandle(s_events[type][i]);
            s_events[type][i] = NULL;
            s_eventIds[type][i] = 0;
        }

        s_allocIndex[type] = 0;
        s_freeIndex[type] = 0;
        s_eventCount[type] = 0;
    }

    SServerDestroy();
}

// OFFSET: 0x004BB140/0x1400F4600
uint32_t SRWLock::IAllocEvent(uint32_t type) {
    if (InterlockedDecrement(&s_eventCount[type]) < 0) {
        DebugLog("IAllocEvent: too many %s event allocs", type ? "SUMREVTYPE" : "SUAREVTYPE");
    }

    volatile LONG* slot = &s_eventIds[type][InterlockedIncrement(&s_allocIndex[type]) & 0x7FF];
    uint32_t event = InterlockedExchange(slot, 0);

    while (!event) {
        uint32_t spin = s_spinCount;

        while (true) {
            event = InterlockedExchange(slot, 0);

            if (event) {
                return event;
            }

            SLOCK_SPIN_NOPS();

            if (!--spin) {
                break;
            }
        }

        Sleep(0);
    }

    return event;
}

// OFFSET: 0x004BB1F0/0x1400F46C0
void SRWLock::IFreeEvent(uint32_t event, uint32_t type, int32_t reset) {
    if (InterlockedIncrement(&s_eventCount[type]) > 1024) {
        DebugLog("IFreeEvent: too many %s event frees", type ? "SUMREVTYPE" : "SUAREVTYPE");
    }

    if (reset) {
        ResetEvent(s_events[type][(event >> 21) - 1]);
    }

    volatile LONG* slot = &s_eventIds[type][InterlockedIncrement(&s_freeIndex[type]) & 0x7FF];
    LONG value = InterlockedExchange(slot, event & 0xFFE00000);

    while (value) {
        uint32_t spin = s_spinCount;

        while (true) {
            value = InterlockedExchange(slot, value);

            if (!value) {
                return;
            }

            SLOCK_SPIN_NOPS();

            if (!--spin) {
                break;
            }
        }

        Sleep(0);
    }
}

// OFFSET: 0x004BB2D0/0x1400F47D0
uint32_t SRWLock::IAddWaiter(volatile LONG* waiters, uint32_t type, uint32_t increment) {
    uint32_t event = IAllocEvent(type);
    LONG value = InterlockedCompareExchange(waiters, event, 0);

    while (value) {
        if (InterlockedCompareExchange(waiters, value + increment, value) == value) {
            IFreeEvent(event, type, 0);
            return value + increment;
        }

        value = InterlockedCompareExchange(waiters, event, 0);
    }

    return event;
}

// OFFSET: 0x004BB330
int32_t SRWLock::IRemoveWaiter(volatile LONG* waiters, uint32_t expected, uint32_t type, uint32_t decrement) {
    LONG value = InterlockedCompareExchange(waiters, 0, expected);

    while (value != static_cast<LONG>(expected)) {
        if (InterlockedCompareExchange(waiters, value - decrement, value) == value) {
            return 0;
        }

        value = InterlockedCompareExchange(waiters, 0, expected);
    }

    IFreeEvent(expected, type, 1);
    return 1;
}

// INLINED
inline void SRWLock::SUNNLockLeave(volatile SUNNLOCK* sunnlock) {
    sunnlock->m_state = 1;

    uint32_t event = sunnlock->m_event;

    if (event) {
        SetEvent(s_events[0][(event >> 21) - 1]);
    }
}

// OFFSET: 0x004BB390/0x1400F4850
void SRWLock::SURWLockLeave(volatile SURWLOCK* surwlock, int32_t fromwriting) {
    if (fromwriting) {
        SUNNLockLeave(&surwlock->m_mutex);
    } else if (IRemoveWaiter(&surwlock->m_readerEvent, (surwlock->m_readerEvent & 0xFFE00000) + 1, 1, 2)) {
        SUNNLockLeave(&surwlock->m_mutex);
    }
}

// PROVEN: 79123baa IDENTICAL/IDENTICAL
// OFFSET: 0x004BB400/0x1400F4220
SCritSect::SCritSect() {
    InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(this->m_opaqueData));
}

// PROVEN: 58b5a765 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB410/0x1400F4240
SCritSect::~SCritSect() {
    DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(this->m_opaqueData));
}

// PROVEN: d449ea0a IDENTICAL/IDENTICAL
// OFFSET: 0x004BB420/0x1400F4250
void SCritSect::Enter() {
    EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(this->m_opaqueData));
}

// PROVEN: 03f3aaed IDENTICAL/IDENTICAL
// OFFSET: 0x004BB430/0x1400F4260
void SCritSect::Leave() {
    LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(this->m_opaqueData));
}

// PROVEN: b04316f3 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB460/0x1402C71C0
SSyncObject::SSyncObject() {
    this->m_opaqueData = NULL;
}

// PROVEN: 547093eb DIFFERENT/IDENTICAL
// OFFSET: 0x004BB440/0x1400F4970
void CSRWLock::Leave(int32_t fromwriting) {
    SRWLock::SURWLockLeave(&this->m_opaqueData, fromwriting);
}

// PROVEN: 439612b1 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB470/0x1400F4270
int32_t SSyncObject::Valid() {
    return this->m_opaqueData != NULL;
}

// PROVEN: aa98d2fe IDENTICAL/IDENTICAL
// OFFSET: 0x004BB4A0/0x1400F42B0
uint32_t SSyncObject::Wait(uint32_t timeout) {
    return WaitForSingleObject(this->m_opaqueData, timeout);
}

// OFFSET: 0x004BB4C0/0x1400F42C0
uint32_t STORMAPI SSyncObject::WaitMultiplePtr(uint32_t count, SSyncObject** objects, int32_t waitAll, uint32_t timeout) {
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];

    if (count > MAXIMUM_WAIT_OBJECTS) {
        return 0xFFFFFFFF;
    }

    uint32_t handleCount = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (objects[i] && objects[i]->m_opaqueData) {
            handles[handleCount++] = objects[i]->m_opaqueData;
        }
    }

    return WaitForMultipleObjects(handleCount, handles, waitAll, timeout);
}

// PROVEN: 9a1fa5ca IDENTICAL/IDENTICAL
// OFFSET: 0x004BB520/0x1404A31A0
int32_t SEvent::Set() {
    return SetEvent(this->m_opaqueData);
}

// PROVEN: 215eee52 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB530/0x1400F4330
int32_t SEvent::Reset() {
    return ResetEvent(this->m_opaqueData);
}

// PROVEN: 470f00f2 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB540/0x1400F4340
int32_t SThread::Create(uint32_t (STORMAPI* threadProc)(void*), void* param, SThread& thread, char* threadName, uint32_t stackSize) {
    uint32_t threadId;
    thread.m_opaqueData = SCreateThread(threadProc, param, &threadId, NULL, NULL, stackSize);
    return thread.m_opaqueData != NULL;
}

// PROVEN: ab2fa1bd DIFFERENT/IDENTICAL
// OFFSET: 0x004BB570/0x1400F4380
int32_t SThread::SetPriority(int32_t priority) {
    static const int32_t priorities[] = { THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL };

    if (priority < 6) {
        return SetThreadPriority(this->m_opaqueData, priorities[priority]);
    }

    return 0;
}

// PROVEN: 0415e2d9 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB5A0/0x1400F43A0
int32_t SSemaphore::Signal(uint32_t count) {
    return ReleaseSemaphore(this->m_opaqueData, count, NULL);
}

// OFFSET: 0x004BB5C0/0x1400F49F0
void SMutex::Create(int32_t initialOwner, const char* name) {
    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = NULL;
    }

    this->m_opaqueData = CreateMutexA(NULL, initialOwner, name);

    if (!this->m_opaqueData) {
        DebugLog("SMutex::Create: CreateMutex failed");
    }
}

// PROVEN: a7d7ca2a IDENTICAL/IDENTICAL
// OFFSET: 0x004BB610/0x1400F43B0
int32_t SMutex::Release() {
    return ReleaseMutex(this->m_opaqueData);
}

// INLINED
inline void SRWLock::IOsRWLockIncRef() {
    s_refCritsect.Enter();

    if (s_refCount++ == 0) {
        IInitialize();
    }

    s_refCritsect.Leave();
}

// INLINED
inline void SRWLock::IOsRWLockDecRef() {
    s_refCritsect.Enter();

    if (--s_refCount == 0) {
        IDestroy();
    }

    s_refCritsect.Leave();
}

// INLINED
inline void SRWLock::SURWLockInitialize(volatile SURWLOCK* surwlock) {
    surwlock->m_mutex.m_state = 1;
    surwlock->m_mutex.m_event = 0;
    surwlock->m_readerEvent = 0;
}

// INLINED
inline void SRWLock::SURWLockDelete(volatile SURWLOCK* surwlock) {
    surwlock->m_mutex.m_state = 0;

    uint32_t event = InterlockedExchange(&surwlock->m_mutex.m_event, 0);

    if (event) {
        IFreeEvent(event, 0, 1);
    }

    event = InterlockedExchange(&surwlock->m_readerEvent, 0);

    if (event) {
        IFreeEvent(event, 1, 1);
    }
}

// OFFSET: 0x004BB620/0x1400F48F0
CSRWLock::CSRWLock() {
    SRWLock::IOsRWLockIncRef();
    SRWLock::SURWLockInitialize(&this->m_opaqueData);
}

// OFFSET: 0x004BB670/0x1400F4920
CSRWLock::~CSRWLock() {
    SRWLock::SURWLockDelete(&this->m_opaqueData);
    SRWLock::IOsRWLockDecRef();
}

// PROVEN: 9fdf7b70 IDENTICAL/IDENTICAL
// OFFSET: 0x004BB480/0x1400F4280
SSyncObject::~SSyncObject() {
    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = NULL;
    }
}

void SSyncObject::Close() {
    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = NULL;
    }
}

// OFFSET: 0x004BB6E0/0x1400F4980
SEvent::SEvent(int32_t manualReset, int32_t initialState) {
    this->m_opaqueData = CreateEventA(NULL, manualReset, initialState, NULL);

    if (!this->m_opaqueData) {
        DebugLog("SEvent::SEvent: CreateEvent failed");
    }
}

// PROVEN: 7fed7c6a IDENTICAL/IDENTICAL
// OFFSET: 0x004BB720/0x1400F49C0
SSemaphore::SSemaphore(int32_t initialCount, int32_t maximumCount) {
    this->m_opaqueData = CreateSemaphoreA(NULL, initialCount, maximumCount, NULL);
}

// OFFSET: 0x004BB750/0x1400F4E30
SMutex::SMutex() {
    this->Create(0, NULL);
}

// INLINED
template <class T>
void TDebugLock<T>::LockEntries() {
    if (s_critsect.Enter()) {
        memset(s_entries, 0, sizeof(s_entries));

        for (uint32_t i = 1; i < 255; i++) {
            s_entries[i].m_next = i + 1;
        }

        s_freeEntry = 1;
    }
}

// INLINED
template <class T>
void TDebugLock<T>::DumpEntries(CDebugLock* lock) {
    CDebugLockEntry* entry = NULL;
    uint32_t now = GetTickCount();
    uint32_t index = lock->m_entry;

    while (index) {
        if (index >= 256) {
            SDebugPrintf("%s(%u) : CDebugLock:%08x: entry has bad next %u\n", __FILE__, __LINE__, lock, index);

            if (entry) {
                entry->m_next = 0;
            } else {
                lock->m_entry = 0;
            }

            return;
        }

        entry = &s_entries[index];

        char threadName[16];
        SGetThreadName(entry->m_threadId, threadName, sizeof(threadName));

        SDebugPrintf("%s(%u) : CDebugLock:%08x: tid:%03x %s %c %c t:%u\n", entry->m_file, entry->m_line & 0x3FFFFFFF, lock, entry->m_threadId, threadName, (entry->m_line & 0x40000000) ? 'W' : 'R', (entry->m_line & 0x80000000) ? 'T' : 'F', now - entry->m_time);

        index = entry->m_next;
    }
}

// INLINED
void CDebugSCritSect::DumpAllEntries() {
    LockEntries();

    SDebugPrintf("%s(%u) : CDebugSCritSect:DumpAllEntries\n", __FILE__, __LINE__);

    for (CDebugLock* lock = s_lockList; lock; lock = lock->m_next) {
        DumpEntries(lock);
    }

    s_critsect.Leave();
}

// INLINED
void CDebugSRWLock::DumpAllEntries() {
    LockEntries();

    SDebugPrintf("%s(%u) : CDebugSRWLock:DumpAllEntries\n", __FILE__, __LINE__);

    for (CDebugLock* lock = s_lockList; lock; lock = lock->m_next) {
        DumpEntries(lock);
    }

    s_critsect.Leave();
}

// OFFSET: 0x004BBA80
void SRWLock::SUNNLockEnter(volatile SUNNLOCK* sunnlock) {
    if (!sunnlock->m_event) {
        uint32_t spin = s_spinCount;

        while (true) {
            if (InterlockedExchange(&sunnlock->m_state, 0)) {
                return;
            }

            SLOCK_SPIN_NOPS();

            if (!--spin || sunnlock->m_event) {
                break;
            }
        }
    }

    uint32_t event = IAddWaiter(&sunnlock->m_event, 0, 1) & 0xFFE00000;

    while (true) {
        uint32_t spin = s_spinCount;

        while (true) {
            if (InterlockedExchange(&sunnlock->m_state, 0)) {
                IRemoveWaiter(&sunnlock->m_event, event, 0, 1);
                return;
            }

            SLOCK_SPIN_NOPS();

            if (!--spin) {
                break;
            }
        }

        if (WaitForSingleObject(s_events[0][(static_cast<uint32_t>(sunnlock->m_event) >> 21) - 1], 60000)) {
            InterlockedExchangeAdd(&s_stallCount, 1);
            CDebugSCritSect::DumpAllEntries();
            CDebugSRWLock::DumpAllEntries();
        }
    }
}

// OFFSET: 0x004BBB80
void SRWLock::SURWLockEnter(volatile SURWLOCK* surwlock, int32_t forwriting) {
    if (forwriting) {
        SUNNLockEnter(&surwlock->m_mutex);
        return;
    }

    uint32_t event = IAddWaiter(&surwlock->m_readerEvent, 1, 2);

    if (event == (event & 0xFFE00000)) {
        SUNNLockEnter(&surwlock->m_mutex);
        InterlockedExchangeAdd(&surwlock->m_readerEvent, 1);

        if (event) {
            SetEvent(s_events[1][(event >> 21) - 1]);
        }

        return;
    }

    while (true) {
        uint32_t spin = s_spinCount;

        while (true) {
            if (surwlock->m_readerEvent & 1) {
                return;
            }

            SLOCK_SPIN_NOPS();

            if (!--spin) {
                break;
            }
        }

        if (!WaitForSingleObject(s_events[1][(event >> 21) - 1], 60000)) {
            return;
        }

        InterlockedExchangeAdd(&s_stallCount, 1);
        CDebugSCritSect::DumpAllEntries();
        CDebugSRWLock::DumpAllEntries();
    }
}

// PROVEN: 1055bb99 IDENTICAL/UNPROVEN
// OFFSET: 0x004BBC60/0x1400F5150
void CSRWLock::Enter(int32_t forwriting) {
    SRWLock::SURWLockEnter(&this->m_opaqueData, forwriting);
}

#endif
