#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <process.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

// STATIC: 0x00EB5C9F
bool System_Thread::s_initialized;

// STATIC: 0x00EB5CA0
Blizzard::Lock::Mutex System_Thread::s_mutex;

// STATIC: 0x00EB5EB8
Blizzard::Lock::Mutex System_Thread::s_registryMutex;

// STATIC: 0x00EBA7A0
Blizzard::Thread::TLSSlot System_Thread::s_threadRecordTLS;

// STATIC: 0x00EBA788
Blizzard::Thread::TLSSlot System_Thread::s_stackTraceEntryPointTLS;

// STATIC: 0x00EB5C98
Blizzard::Thread::ThreadRecord* System_Thread::s_mainThread;

// STATIC: 0x00EB5ED4
std::set<Blizzard::Thread::ThreadRecord*, std::less<Blizzard::Thread::ThreadRecord*>, Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord*>>* System_Thread::s_ThreadRegistry;

// STATIC: 0x00EBA63C
int32_t System_Thread::s_DefaultStackSize;

// STATIC: 0x00EB5EE0
System_Thread::ExternalTLSRecord System_Thread::s_externalTLS[32];

// STATIC: 0x00EBA640
int32_t System_Thread::s_externalTLSCount;

// PROVEN: bb135620 IDENTICAL/IDENTICAL
// OFFSET: 0x007AD6E0/0x1404A3370
bool Blizzard::Thread::Sleep(uint32_t milliseconds) {
    ::Sleep(milliseconds);
    return true;
}

// OFFSET: 0x007AE090
bool Blizzard::Thread::IsRunning(ThreadRecord* thread) {
    return thread && thread->m_state >= 0;
}

// PROVEN: 7a87b31c IDENTICAL/DIFFERENT
// OFFSET: 0x007AE0B0/0x1404A3D80
bool System_Thread::SetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t level) {
    switch (level) {
        case 0:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_IDLE) != 0;
        case 1:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_LOWEST) != 0;
        case 2:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_BELOW_NORMAL) != 0;
        case 3:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_NORMAL) != 0;
        case 4:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_ABOVE_NORMAL) != 0;
        case 5:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_HIGHEST) != 0;
        case 6:
            return SetThreadPriority(thread->m_handle, THREAD_PRIORITY_TIME_CRITICAL) != 0;
        default:
            return false;
    }
}

// PROVEN: 19237535 IDENTICAL/DIFFERENT
// OFFSET: 0x007AE180/0x1404A3E70
bool System_Thread::GetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t* level) {
    switch (GetThreadPriority(thread->m_handle)) {
        case THREAD_PRIORITY_IDLE:
            *level = 0;
            break;
        case THREAD_PRIORITY_LOWEST:
            *level = 1;
            break;
        case THREAD_PRIORITY_BELOW_NORMAL:
            *level = 2;
            break;
        case THREAD_PRIORITY_NORMAL:
            *level = 3;
            break;
        case THREAD_PRIORITY_ABOVE_NORMAL:
            *level = 4;
            break;
        case THREAD_PRIORITY_HIGHEST:
            *level = 5;
            break;
        case THREAD_PRIORITY_TIME_CRITICAL:
            *level = 6;
            break;
    }

    return false;
}

// OFFSET: 0x007B5FB0/0x1404AC000
void System_Thread::ShutdownThreadLocalStorage() {
    if (!s_initialized) {
        return;
    }

    Blizzard::Lock::MutexEnter(s_mutex);

    for (int32_t i = 0; i < s_externalTLSCount; i++) {
        ExternalTLSRecord& record = s_externalTLS[i];

        if (record.m_destructor) {
            record.m_destructor(record.m_value);
        }

        record.m_threadId = 0;
        record.m_value = NULL;
        record.m_destructor = NULL;
    }

    s_externalTLSCount = 0;
    Blizzard::Lock::MutexLeave(s_mutex);
}

// OFFSET: 0x007B60A0
void System_Thread::RunExternalTLSDestructors(uint32_t threadId) {
    Blizzard::Lock::MutexEnter(s_mutex);

    for (int32_t i = 0; i < s_externalTLSCount;) {
        ExternalTLSRecord& record = s_externalTLS[i];

        if (record.m_threadId != threadId) {
            i++;
            continue;
        }

        if (record.m_destructor) {
            record.m_destructor(record.m_value);
        }

        int32_t last = s_externalTLSCount - 1;
        record.m_threadId = 0;
        record.m_value = NULL;
        record.m_destructor = NULL;
        record = s_externalTLS[last];
        s_externalTLSCount = last;
    }

    Blizzard::Lock::MutexLeave(s_mutex);
}

// OFFSET: 0x007B6160
void NTAPI System_Thread::TLSCallback(PVOID module, DWORD reason, PVOID reserved) {
    if (s_initialized && (reason == DLL_THREAD_DETACH || reason == DLL_PROCESS_DETACH)) {
        RunExternalTLSDestructors(GetCurrentThreadId());
    }
}

#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma section(".CRT$XLB", read)

// STATIC: 0x00B870B0
__declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK s_tlsCallback = System_Thread::TLSCallback;

// OFFSET: 0x007DA3A0
void System_Thread::AddToRegistry(Blizzard::Thread::ThreadRecord* thread) {
    Blizzard::Lock::ScopedMutex lock(s_registryMutex);

    if (!s_ThreadRegistry) {
        void* memory = Blizzard::Memory::Allocate(sizeof(*s_ThreadRegistry), 0, __FILE__, __LINE__);
        s_ThreadRegistry = memory ? new (memory) std::set<Blizzard::Thread::ThreadRecord*, std::less<Blizzard::Thread::ThreadRecord*>, Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord*>>() : NULL;
    }

    s_ThreadRegistry->insert(thread);
}

// OFFSET: 0x007DCD00/0x1404E0A80
Blizzard::Thread::ThreadRecord* System_Thread::NewThread(uint32_t (*function)(void*), void* param, const char* name) {
    if (!name) {
        name = "";
    }

    uint32_t length = static_cast<uint32_t>(strlen(name));
    Blizzard::Thread::ThreadRecord* thread = static_cast<Blizzard::Thread::ThreadRecord*>(Blizzard::Memory::Allocate(sizeof(Blizzard::Thread::ThreadRecord) + length + 1, 0, __FILE__, __LINE__));
    memset(thread, 0, sizeof(Blizzard::Thread::ThreadRecord));
    thread->m_param = param;
    thread->m_function = function;
    thread->m_state = 0;
    thread->m_refCount.m_value = 2;
    Blizzard::String::Copy(thread->m_name, name, length + 1);
    AddToRegistry(thread);

    return thread;
}

// OFFSET: 0x007DCDA0
void System_Thread::RemoveFromRegistry(Blizzard::Thread::ThreadRecord* thread) {
    Blizzard::Lock::ScopedMutex lock(s_registryMutex);

    if (!s_ThreadRegistry) {
        return;
    }

    s_ThreadRegistry->erase(thread);

    if (s_ThreadRegistry->empty()) {
        typedef std::set<Blizzard::Thread::ThreadRecord*, std::less<Blizzard::Thread::ThreadRecord*>, Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord*>> Registry;
        s_ThreadRegistry->~Registry();
        Blizzard::Memory::Free(s_ThreadRegistry, 0, __FILE__, __LINE__);
        s_ThreadRegistry = NULL;
    }
}

// INLINED
void System_Thread::InitMainThread() {
    s_mainThread = NewThread(NULL, NULL, NULL);
    InternalSetLocalStorage(&s_threadRecordTLS, s_mainThread);
    s_mainThread->m_state = 1;
    s_mainThread->m_refCount.m_value = 1;
}

// OFFSET: 0x007E1B40
void System_Thread::InitThreadSystem() {
    if (s_initialized) {
        return;
    }

    s_initialized = true;
    Blizzard::Lock::MutexCreate(s_mutex);
    Blizzard::Lock::MutexCreate(s_registryMutex);
    AllocateLocalStorage(&s_threadRecordTLS, NULL);
    AllocateLocalStorage(&s_stackTraceEntryPointTLS, NULL);
    InitMainThread();
}

// OFFSET: 0x007E1BC0
void System_Thread::SetStackTraceEntryPoint(void* entryPoint) {
    InitThreadSystem();
    InternalSetLocalStorage(&s_stackTraceEntryPointTLS, entryPoint);
}

// INLINED
Blizzard::Thread::ThreadRecord* System_Thread::GetThreadRecord() {
    InitThreadSystem();

    return static_cast<Blizzard::Thread::ThreadRecord*>(TlsGetValue(~s_threadRecordTLS.m_key));
}

// OFFSET: 0x007E1BE0/0x1404E8160
bool System_Thread::AllocateTLSSlot(Blizzard::Thread::TLSSlot* slot, Blizzard::Thread::TLSDestructor destructor) {
    InitThreadSystem();
    Blizzard::Lock::ScopedMutex lock(s_mutex);

    return slot->m_key || AllocateLocalStorage(slot, destructor);
}

// OFFSET: 0x007E1C70
void System_Thread::InitStackTraceEntryPoint() {
    uintptr_t* frame = NULL;
    uintptr_t* stack = NULL;

    __asm {
        mov frame, ebp
        mov stack, esp
    }

    MEMORY_BASIC_INFORMATION info = {};
    uintptr_t* top = NULL;

    if (VirtualQuery(stack, &info, sizeof(info)) >= sizeof(info) && (info.State | info.Protect | info.Type) == (MEM_COMMIT | PAGE_READWRITE | MEM_PRIVATE)) {
        top = reinterpret_cast<uintptr_t*>(static_cast<uint8_t*>(info.BaseAddress) + info.RegionSize);
    }

    if (frame > stack && frame + 2 <= top) {
        uintptr_t* next = reinterpret_cast<uintptr_t*>(*frame);

        if (!(*frame & 3) && next > frame && next + 2 <= top) {
            SetStackTraceEntryPoint(reinterpret_cast<void*>(next[1]));
        }
    }
}

// OFFSET: 0x007E3900
uint32_t __stdcall System_Thread::ThreadStarter(void* param) {
    Blizzard::Thread::ThreadRecord* thread = static_cast<Blizzard::Thread::ThreadRecord*>(param);

    InternalSetLocalStorage(&s_threadRecordTLS, thread);
    InitStackTraceEntryPoint();

    while (!thread->m_state) {
        Sleep(1);
    }

    uint32_t result = thread->m_function(thread->m_param);
    thread->m_result = result;
    RunTLSDestructors();
    thread->m_state = -1;
    Blizzard::Thread::Close(thread);

    return result;
}

// OFFSET: 0x007E9F60
bool Blizzard::Thread::Create(uint32_t (*function)(void*), void* param, ThreadRecord** thread, const char* name, int32_t flags, int32_t stackSize) {
    System_Thread::InitThreadSystem();
    ThreadRecord* record = System_Thread::NewThread(function, param, name);
    *thread = record;

    uint32_t initFlags = (flags & 1) ? CREATE_SUSPENDED : 0;

    if (!stackSize) {
        stackSize = System_Thread::s_DefaultStackSize;
    }

    if (stackSize > 0) {
        initFlags |= STACK_SIZE_PARAM_IS_A_RESERVATION;
    }

    record->m_handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, stackSize, System_Thread::ThreadStarter, record, initFlags, &record->m_threadId));
    (*thread)->m_state = (flags & 1) == 0;

    return true;
}

// OFFSET: 0x007B6050/0x1404AC0A0
void System_Thread::RegisterExternalTLSDestructor(void* value, Blizzard::Thread::TLSDestructor destructor) {
    uint32_t threadId = GetCurrentThreadId();
    Blizzard::Lock::MutexEnter(s_mutex);

    if (s_externalTLSCount < 32) {
        ExternalTLSRecord& record = s_externalTLS[s_externalTLSCount++];
        record.m_threadId = threadId;
        record.m_value = value;
        record.m_destructor = destructor;
    }

    Blizzard::Lock::MutexLeave(s_mutex);
}

// OFFSET: 0x007BD510/0x1404BA5D0
void System_Thread::InternalSetLocalStorage(const Blizzard::Thread::TLSSlot* slot, const void* value) {
    TlsSetValue(~slot->m_key, const_cast<void*>(value));

    if (slot->m_destructor && value) {
        Blizzard::Thread::ThreadRecord* thread = GetThreadRecord();

        if (thread && thread->m_worker) {
            thread = thread->m_worker->m_task;
        }

        if (!thread) {
            RegisterExternalTLSDestructor(const_cast<void*>(value), slot->m_destructor);
        }
    }
}

#endif
