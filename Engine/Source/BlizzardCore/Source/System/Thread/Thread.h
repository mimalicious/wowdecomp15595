#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREAD_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREAD_H

#include <cstdint>
#include <functional>
#include <set>
#include <vector>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

namespace System_Thread {

class ThreadWorker;

}

namespace Blizzard {
namespace Thread {

class ThreadPoolRecord;

struct ThreadAffinity {
    /* 0000 */ int32_t m_mode;
    /* 0004 */ int32_t m_processor;
};

struct ThreadRecord {
    /* 0000 */ HANDLE m_handle;
    /* 0004 */ uint32_t m_threadId;
    /* 0008 */ ThreadAffinity m_affinity;
    /* 0010 */ void* m_param;
    /* 0014 */ uint32_t (*m_function)(void*);
    /* 0018 */ volatile int32_t m_state;
    /* 001C */ Blizzard::Lock::Atomic m_refCount;
    /* 0020 */ uint32_t m_result;
    /* 0024 */ System_Thread::ThreadWorker* m_worker;
    /* 0028 */ uint8_t unk_0028[16];
    /* 0038 */ char m_name[8];
};

class ThreadPoolRecord {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ uint32_t m_maxWorkers;
    /* 0008 */ Blizzard::Lock::RMutex m_mutex;
    /* 0020 */ std::vector<System_Thread::ThreadWorker*> m_idle;
    /* 0030 */ std::vector<System_Thread::ThreadWorker*> m_workers;
    /* 0050 */ ThreadAffinity m_affinity;

    ThreadPoolRecord(uint32_t count, uint32_t maxWorkers, int64_t unk, const ThreadAffinity& affinity);
    ~ThreadPoolRecord();
    System_Thread::ThreadWorker* GetWorker(int32_t flags, int32_t stackSize);
};

bool Close(ThreadRecord* thread);
bool Create(uint32_t (*function)(void*), void* param, ThreadRecord** thread, const char* name, int32_t flags, int32_t stackSize);
bool CreateDetached(uint32_t (*function)(void*), void* param, const char* name);
bool CreateDetachedFromPool(ThreadPoolRecord* pool, uint32_t (*function)(void*), void* param, int32_t flags, int32_t stackSize);
bool CreateFromPool(ThreadPoolRecord* pool, uint32_t (*function)(void*), void* param, ThreadRecord** thread, int32_t flags, int32_t stackSize);
bool GetAffinity(ThreadRecord* thread, ThreadAffinity* affinity);
bool IsRunning(ThreadRecord* thread);
bool SetAffinity(ThreadRecord* thread, const ThreadAffinity& affinity);
bool Sleep(uint32_t milliseconds);
bool WaitForever(ThreadRecord* thread);

}
}

namespace System_Thread {

class ThreadWorker {
    public:
    static uint32_t Executive(void* param);

    /* 0000 */ Blizzard::Thread::ThreadRecord* m_thread;
    /* 0004 */ Blizzard::Thread::ThreadPoolRecord* m_pool;
    /* 0008 */ Blizzard::Lock::EventObject m_wakeEvent;
    /* 000C */ Blizzard::Lock::EventObject m_readyEvent;
    /* 0010 */ Blizzard::Thread::ThreadRecord* m_task;
    /* 0014 */ uint32_t (*m_taskFunction)(void*);
    /* 0018 */ void* m_taskParam;
    /* 001C */ int32_t m_priority;
    /* 0020 */ Blizzard::Thread::ThreadAffinity m_affinity;

    ThreadWorker();
    ~ThreadWorker();
    bool Start(Blizzard::Thread::ThreadPoolRecord* pool, int32_t flags, int32_t stackSize, const Blizzard::Thread::ThreadAffinity& affinity);
};

class UsingThread : public Blizzard::Using::Use {
    public:
    UsingThread();
};

class UsingThreadLocalStorage : public Blizzard::Using::Use {
    public:
    UsingThreadLocalStorage();
};

struct ExternalTLSRecord {
    /* 0000 */ HANDLE m_thread;
    /* 0004 */ HANDLE m_event;
    /* 0008 */ HANDLE m_watcher;
    /* 000C */ uint32_t m_threadId;
    /* 0010 */ void* m_value;
    /* 0014 */ Blizzard::Thread::TLSDestructor m_destructor;
};

extern bool s_initialized;
extern Blizzard::Lock::Mutex s_mutex;
extern Blizzard::Lock::Mutex s_registryMutex;
extern Blizzard::Thread::TLSSlot s_threadRecordTLS;
extern Blizzard::Thread::TLSSlot s_stackTraceEntryPointTLS;
extern Blizzard::Thread::ThreadRecord* s_mainThread;
extern std::set<Blizzard::Thread::ThreadRecord*, std::less<Blizzard::Thread::ThreadRecord*>, Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord*>>* s_ThreadRegistry;
extern int32_t s_DefaultStackSize;
extern ExternalTLSRecord s_externalTLS[32];
extern int32_t s_externalTLSCount;
extern Blizzard::Thread::ThreadPoolRecord* s_defaultPool;

void AddToRegistry(Blizzard::Thread::ThreadRecord* thread);
bool CreateDefaultPool();
uint32_t ExternalTLSWatcher(void* param);
bool GetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t* level);
int32_t GetProcessorCount();
Blizzard::Thread::ThreadRecord* GetThreadRecord();
void InitMainThread();
void InitStackTraceEntryPoint();
void InitThreadSystem();
Blizzard::Thread::ThreadRecord* NewThread(uint32_t (*function)(void*), void* param, const char* name);
void RegisterExternalTLSDestructor(void* value, Blizzard::Thread::TLSDestructor destructor);
void RemoveFromRegistry(Blizzard::Thread::ThreadRecord* thread);
void RunExternalTLSDestructors(HANDLE thread);
bool SetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t level);
void SetStackTraceEntryPoint(void* entryPoint);
void ShutdownThreadLocalStorage();
void ShutdownThreadSystem();
uint32_t __stdcall ThreadStarter(void* param);

}

#endif
