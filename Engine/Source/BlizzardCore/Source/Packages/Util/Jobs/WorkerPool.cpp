#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Jobs/WorkerPool.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// PROVEN: f620d273 IDENTICAL/IDENTICAL
// OFFSET: 0x007AA740/0x1404A0B80
bool Blizzard::Jobs::Job::IsReady() const {
    for (int32_t i = 0; i < this->m_dependencyCount; i++) {
        if (!this->m_dependencies[i].Get()) {
            return false;
        }
    }

    return true;
}

// OFFSET: %0x00712A60
uint32_t Blizzard::Jobs::WorkerPool::RunJobProc(void* param) {
    JobParam* jobParam = static_cast<JobParam*>(param);
    Job* job = jobParam->m_job;
    job->m_result = job->m_cancelled ? false : job->Execute();
    jobParam->m_completed->Push(jobParam->m_job);
    Blizzard::Thread::Sleep(0);
    delete jobParam;

    return 0;
}

// OFFSET: %0x00712AB0
void Blizzard::Jobs::WorkerPool::QueueJob(const RefPtr<Job>& job) {
    if (!job->m_cancelled) {
        this->m_pImpl->m_queued.Push(job);
        return;
    }

    job->AddRef();
    Job* cancelled = job.Get();
    cancelled->m_result = cancelled->m_cancelled ? false : cancelled->Execute();
    this->m_pImpl->m_completed.Push(cancelled);
    this->m_pImpl->m_pending++;
}

// OFFSET: %0x00719770
Blizzard::Jobs::Job::Job(int32_t dependencyCount) {
    this->m_dependencyCount = dependencyCount;
    this->m_result = true;
    this->m_cancelled = false;

    if (this->m_dependencyCount > 4) {
        this->m_dependencyCount = 4;
    }
}

// OFFSET: %0x007197B0
Blizzard::Jobs::Job::~Job() {
}

// OFFSET: %0x00719850
bool Blizzard::Jobs::WorkerPool::RunJob(RefPtr<Job> job) {
    if (this->m_pImpl->m_shutdown) {
        return false;
    }

    JobParam* param = new JobParam;
    param->m_job = job.Get();
    param->m_completed = &this->m_pImpl->m_completed;
    job->AddRef();

    if (this->RunOnThread(RunJobProc, param)) {
        this->m_pImpl->m_pending++;
        return true;
    }

    job->Release();
    delete param;

    return false;
}

// OFFSET: %0x00720760
Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> Blizzard::Jobs::WorkerPool::GetCompletedJob() {
    Job* completed;

    if (!this->m_pImpl->m_completed.Pop(completed)) {
        return RefPtr<Job>();
    }

    this->m_pImpl->m_pending--;
    RefPtr<Job> job(completed);
    completed->Release();

    RefPtr<Job> result;

    if (!this->m_pImpl->m_shutdown) {
        result = job;
    }

    return result;
}

// OFFSET: %0x00720850
bool Blizzard::Jobs::WorkerPool::Pump() {
    if (this->m_pImpl->m_shutdown) {
        while (this->GetCompletedJob().Get()) {
        }
    }

    for (int32_t i = 0; i < this->m_threads; i++) {
        if (!this->m_pImpl->m_queued.Size()) {
            break;
        }

        RefPtr<Job> job = this->m_pImpl->m_queued.Front();

        if (this->RunJob(job)) {
            this->m_pImpl->m_queued.Pop();
        }
    }

    return this->m_pImpl->m_pending || this->m_pImpl->m_queued.Size() || this->m_pImpl->m_completed.Size();
}

// OFFSET: %0x00731940
Blizzard::Jobs::WorkerPool::Impl::Impl() {
    this->m_pending = 0;
    this->m_shutdown = false;
}

// OFFSET: %0x007450B0
Blizzard::Jobs::WorkerPool::~WorkerPool() {
    if (this->m_pImpl->m_pool) {
        delete this->m_pImpl->m_pool;
    }

    bool more;

    do {
        more = this->GetCompletedJob().Get() != NULL;
    } while (more);

    delete this->m_pImpl;
}

// OFFSET: %0x007466B0
Blizzard::Jobs::WorkerPool::WorkerPool(int32_t threads) {
    this->m_pImpl = new Impl;
    this->m_threads = threads;

    Blizzard::Thread::ThreadAffinity affinity;
    affinity.m_mode = 1;
    affinity.m_processor = 0;
    this->m_pImpl->m_pool = new Blizzard::Thread::ThreadPoolRecord(threads, threads, 0, affinity);
}

// OFFSET: %0x00746760
bool Blizzard::Jobs::WorkerPool::RunOnThread(uint32_t (*function)(void*), void* param) {
    return Blizzard::Thread::CreateDetachedFromPool(this->m_pImpl->m_pool, function, param, 0, 0);
}
