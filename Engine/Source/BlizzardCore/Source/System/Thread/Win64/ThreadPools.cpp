#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

// OFFSET: 0x007DF4A0/0x1404E4470
System_Thread::ThreadWorker::~ThreadWorker() {
    Blizzard::Lock::EventSet(this->m_wakeEvent.m_event);
    Blizzard::Lock::EventWaitForever(this->m_readyEvent.m_event);

    if (this->m_thread) {
        Blizzard::Lock::EventSet(this->m_wakeEvent.m_event);
        WaitForSingleObject(this->m_thread->m_handle, INFINITE);
        Blizzard::Thread::Close(this->m_thread);
        this->m_thread = NULL;
    }
}

// OFFSET: 0x007DF530/0x1404E4530
uint32_t System_Thread::ThreadWorker::Executive(void* param) {
    ThreadWorker* worker = static_cast<ThreadWorker*>(param);

    Blizzard::Lock::EventSet(worker->m_readyEvent.m_event);
    Blizzard::Lock::EventWaitForever(worker->m_wakeEvent.m_event);

    while (worker->m_taskFunction) {
        worker->m_task->m_state = 1;
        worker->m_task->m_result = worker->m_taskFunction(worker->m_taskParam);

        if (worker->m_thread) {
            SetPriorityLevel(worker->m_thread, worker->m_priority);
        }

        if (worker->m_thread) {
            Blizzard::Thread::SetAffinity(worker->m_thread, worker->m_affinity);
        }

        worker->m_task->m_state = -1;
        Blizzard::Thread::Close(worker->m_task);
        worker->m_task = NULL;
        worker->m_taskFunction = NULL;
        worker->m_taskParam = NULL;

        {
            Blizzard::Lock::ScopedMutex lock(worker->m_pool->m_mutex.m_mutex);
            worker->m_pool->m_idle.push_back(worker);
        }

        Blizzard::Lock::EventSet(worker->m_readyEvent.m_event);
        Blizzard::Lock::EventWaitForever(worker->m_wakeEvent.m_event);
    }

    Blizzard::Lock::EventSet(worker->m_readyEvent.m_event);

    return 0;
}

// OFFSET: 0x007EA040/0x1404EA980
bool System_Thread::ThreadWorker::Start(Blizzard::Thread::ThreadPoolRecord* pool, int32_t flags, int32_t stackSize, const Blizzard::Thread::ThreadAffinity& affinity) {
    if (!Blizzard::Thread::Create(Executive, this, &this->m_thread, "Blizzard Thread Pool Worker", flags, stackSize)) {
        return false;
    }

    Blizzard::Thread::SetAffinity(this->m_thread, affinity);

    if (this->m_thread) {
        GetPriorityLevel(this->m_thread, &this->m_priority);
    }

    if (this->m_thread) {
        Blizzard::Thread::GetAffinity(this->m_thread, &this->m_affinity);
    }

    this->m_pool = pool;

    return true;
}

#endif
