#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// PROVEN: 09ef75de IDENTICAL/DIFFERENT
// OFFSET: 0x007DF450/0x1404E4320
bool Blizzard::Thread::Close(ThreadRecord* thread) {
    if (thread->m_refCount.Decrement() > 0) {
        return true;
    }

    if (thread->m_handle) {
        CloseHandle(thread->m_handle);
        thread->m_handle = NULL;
    }

    System_Thread::RemoveFromRegistry(thread);
    free(thread);

    return true;
}

// OFFSET: %0x00717660
bool Blizzard::Thread::WaitForever(ThreadRecord* thread) {
    if (!thread || thread->m_state < 0) {
        return true;
    }

    System_Thread::ThreadWorker* worker = thread->m_worker;

    if (worker && thread != worker->m_thread) {
        if (thread == worker->m_task) {
            Blizzard::Lock::EventWaitForever(worker->m_readyEvent.m_event);
            Blizzard::Lock::EventSet(worker->m_readyEvent.m_event);
        }

        return true;
    }

    return WaitForSingleObject(thread->m_handle, INFINITE) == WAIT_OBJECT_0;
}

// OFFSET: %0x00716F60
int32_t System_Thread::GetProcessorCount() {
    SYSTEM_INFO info;
    memset(&info, 0, sizeof(info));
    GetSystemInfo(&info);

    int32_t count = info.dwNumberOfProcessors ? static_cast<int32_t>(info.dwNumberOfProcessors) : 1;
    System_Hardware::ReadLogicalProcessorInformation();

    return count >= static_cast<int32_t>(System_Hardware::s_processorFeatures.m_processorCount) ? count : static_cast<int32_t>(System_Hardware::s_processorFeatures.m_processorCount);
}

// OFFSET: %0x00717750
bool Blizzard::Thread::SetAffinity(ThreadRecord* thread, const ThreadAffinity& affinity) {
    if (affinity.m_mode == 0) {
        if (affinity.m_processor < 0 || affinity.m_processor >= System_Thread::GetProcessorCount()) {
            return false;
        }
    } else if (affinity.m_mode != 1) {
        return false;
    }

    DWORD_PTR mask = 1;

    if (affinity.m_mode == 0) {
        mask = static_cast<DWORD_PTR>(1) << affinity.m_processor;
    } else if (affinity.m_mode == 1) {
        mask = System_Thread::GetProcessorCount() < 32 ? (static_cast<DWORD_PTR>(1) << System_Thread::GetProcessorCount()) - 1 : static_cast<DWORD_PTR>(-1);
    }

    if (!SetThreadAffinityMask(thread->m_handle, mask)) {
        return false;
    }

    thread->m_affinity = affinity;
    return true;
}

// OFFSET: %0x007177E0
bool Blizzard::Thread::GetAffinity(ThreadRecord* thread, ThreadAffinity* affinity) {
    if (thread->m_affinity.m_mode == 0) {
        if (thread->m_affinity.m_processor < 0 || thread->m_affinity.m_processor >= System_Thread::GetProcessorCount()) {
            return false;
        }
    } else if (thread->m_affinity.m_mode != 1) {
        return false;
    }

    *affinity = thread->m_affinity;
    return true;
}

#endif
