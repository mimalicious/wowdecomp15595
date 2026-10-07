#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

// STATIC: %0x00D86D3C
Blizzard::Thread::ThreadPoolRecord* System_Thread::s_defaultPool;

// OFFSET: #0x00438AD0
System_Thread::ThreadWorker::ThreadWorker() {
    this->m_thread = NULL;
    this->m_pool = NULL;
    this->m_task = NULL;
    this->m_taskFunction = NULL;
    this->m_taskParam = NULL;
    this->m_affinity.m_mode = 0;
    this->m_affinity.m_processor = 0;
}

// OFFSET: #0x00450DF0
Blizzard::Thread::ThreadPoolRecord::ThreadPoolRecord(uint32_t count, uint32_t maxWorkers, int64_t unk, const ThreadAffinity& affinity) {
    this->m_affinity = affinity;
    Blizzard::Lock::ScopedMutex lock(this->m_mutex.m_mutex);
    this->m_count = count;
    this->m_maxWorkers = maxWorkers;

    for (uint32_t i = 0; i < count; i++) {
        System_Thread::ThreadWorker* worker = new System_Thread::ThreadWorker();

        if (!worker) {
            break;
        }

        worker->Start(this, 0, 0, affinity);
        this->m_workers.push_back(worker);
        this->m_idle.push_back(worker);
    }
}

// OFFSET: #0x0044CFD0
Blizzard::Thread::ThreadPoolRecord::~ThreadPoolRecord() {
    {
        Blizzard::Lock::ScopedMutex lock(this->m_mutex.m_mutex);
        this->m_idle.erase(this->m_idle.begin(), this->m_idle.end());
        this->m_count = 0;
    }

    for (std::vector<System_Thread::ThreadWorker*>::iterator it = this->m_workers.begin(); it != this->m_workers.end(); ++it) {
        if (*it) {
            delete *it;
        }
    }

    {
        Blizzard::Lock::ScopedMutex lock(this->m_mutex.m_mutex);
        this->m_workers.erase(this->m_workers.begin(), this->m_workers.end());
    }
}

// OFFSET: #0x00450EF0
System_Thread::ThreadWorker* Blizzard::Thread::ThreadPoolRecord::GetWorker(int32_t flags, int32_t stackSize) {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex.m_mutex);

    if (!this->m_count) {
        return NULL;
    }

    if (!this->m_idle.empty()) {
        System_Thread::ThreadWorker* worker = this->m_idle.back();
        this->m_idle.pop_back();

        if (worker) {
            return worker;
        }
    }

    if (this->m_maxWorkers && this->m_workers.size() >= this->m_maxWorkers) {
        return NULL;
    }

    System_Thread::ThreadWorker* worker = new System_Thread::ThreadWorker();

    if (worker->Start(this, 0, stackSize, this->m_affinity)) {
        this->m_workers.push_back(worker);
    } else {
        delete worker;
        worker = NULL;
    }

    return worker;
}

// OFFSET: #0x00451040
bool System_Thread::CreateDefaultPool() {
    Blizzard::Thread::ThreadAffinity affinity;
    affinity.m_mode = 0;
    affinity.m_processor = 0;
    s_defaultPool = new Blizzard::Thread::ThreadPoolRecord(1, 0, 0, affinity);

    return s_defaultPool != NULL;
}

// OFFSET: 0x007EA4B0/0x1404EAFF0
bool Blizzard::Thread::CreateFromPool(ThreadPoolRecord* pool, uint32_t (*function)(void*), void* param, ThreadRecord** thread, int32_t flags, int32_t stackSize) {
    if (!pool) {
        if (!System_Thread::s_defaultPool && !System_Thread::CreateDefaultPool()) {
            return false;
        }

        pool = System_Thread::s_defaultPool;
    }

    System_Thread::ThreadWorker* worker = pool->GetWorker(flags, stackSize);

    if (!worker) {
        return false;
    }

    Blizzard::Lock::EventWaitForever(worker->m_readyEvent.m_event);
    worker->m_task = System_Thread::NewThread(function, param, NULL);
    worker->m_task->m_state = (flags & 1) == 0;
    worker->m_task->m_worker = worker;
    worker->m_thread->m_worker = worker;
    worker->m_taskFunction = function;
    worker->m_taskParam = param;
    *thread = worker->m_task;

    if (worker->m_task->m_state == 1) {
        Blizzard::Lock::EventSet(worker->m_wakeEvent.m_event);
    }

    return true;
}

// OFFSET: 0x007EA550
bool Blizzard::Thread::CreateDetachedFromPool(ThreadPoolRecord* pool, uint32_t (*function)(void*), void* param, int32_t flags, int32_t stackSize) {
    ThreadRecord* thread;
    bool result = CreateFromPool(pool, function, param, &thread, flags, stackSize);

    if (result) {
        Close(thread);
    }

    return result;
}

// OFFSET: 0x007E5B90/0x1404EF380
void System_Thread::ShutdownThreadSystem() {
    if (s_mainThread) {
        RunTLSDestructors();
        Blizzard::Thread::ThreadRecord* thread = s_mainThread;
        RemoveFromRegistry(thread);
        Blizzard::Memory::Free(thread, 0, __FILE__, __LINE__);
        s_mainThread = NULL;
    }

    if (s_defaultPool) {
        delete s_defaultPool;
        s_defaultPool = NULL;
    }
}
