#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_JOBS_WORKERPOOL_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_JOBS_WORKERPOOL_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Jobs.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/ThreadSafeQueue.h"

namespace Blizzard {
namespace Thread {
class ThreadPoolRecord;
}
}

namespace Blizzard {
namespace Jobs {

class WorkerPool {
    public:
    struct Impl {
        /* 0000 */ int32_t m_pending;
        /* 0004 */ bool m_shutdown;
        /* 0008 */ Blizzard::Thread::ThreadPoolRecord* m_pool;
        /* 000C */ Blizzard::Util::ThreadSafeQueue<RefPtr<Job> > m_queued;
        /* 0048 */ Blizzard::Util::ThreadSafeQueue<Job*> m_completed;

        Impl();
    };

    struct JobParam {
        /* 0000 */ Job* m_job;
        /* 0004 */ Blizzard::Util::ThreadSafeQueue<Job*>* m_completed;
    };

    static uint32_t RunJobProc(void* param);

    /* 0004 */ Impl* m_pImpl;
    /* 0008 */ int32_t m_threads;

    WorkerPool(int32_t threads);
    virtual ~WorkerPool();
    virtual bool RunOnThread(uint32_t (*function)(void*), void* param);
    RefPtr<Job> GetCompletedJob();
    bool Pump();
    void QueueJob(const RefPtr<Job>& job);
    bool RunJob(RefPtr<Job> job);
};

}
}

#endif
