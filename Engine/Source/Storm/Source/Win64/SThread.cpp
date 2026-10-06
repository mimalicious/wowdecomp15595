#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <new>
#include <windows.h>
#include "Engine/Source/Storm/Source/SThread.h"
#include "Engine/Source/Storm/H/storm.h"

#define MS_VC_EXCEPTION 0x406D1388

struct THREADNAME_INFO {
    /* 0000 */ DWORD dwType;
    /* 0004 */ LPCSTR szName;
    /* 0008 */ DWORD dwThreadID;
    /* 000C */ DWORD dwFlags;
};

struct PROCESSCOMPLETION {
    /* 0000 */ void (*callback)(void*);
    /* 0004 */ void* data;
    /* 0008 */ HANDLE process;
};

struct THREADLAUNCH {
    /* 0000 */ uint32_t (STORMAPI* threadProc)(void*);
    /* 0004 */ void* param;
    /* 0008 */ HANDLE threadH;
};

namespace S_Thread {
    // STATIC: #0x00CAC7B0
    SThreadTrack s_threads[s_maxthreads];

    // STATIC: %0x00C9DDA8
    int32_t s_numthreads;

    // STATIC: %0x00C9DDAC
    CInitCritSect s_threadCrit;
}

// PROVEN: f0a6838f IDENTICAL/IDENTICAL
// OFFSET: 0x004B7600/0x1400EF690
uint32_t STORMAPI SGetCurrentThreadId() {
    return GetCurrentThreadId();
}

// OFFSET: #0x0076FDC0
uint32_t STORMAPI SGetCurrentProcessId() {
    return GetCurrentProcessId();
}

// PROVEN: c23c7efc IDENTICAL/IDENTICAL
// OFFSET: 0x004B7620/0x1400EF6B0
int32_t STORMAPI SGetCurrentThreadPriority() {
    return GetThreadPriority(GetCurrentThread());
}

// PROVEN: df7db9ec IDENTICAL/IDENTICAL
// OFFSET: 0x004B7630/0x1400EF6D0
int32_t STORMAPI SSetCurrentThreadPriority(int32_t priority) {
    return SetThreadPriority(GetCurrentThread(), priority);
}

// PROVEN: 9009f299 IDENTICAL/NONE
// OFFSET: 0x004B7650/0x1400EF6F0
int32_t STORMAPI SSetCurrentProcessAffinityMask(size_t mask) {
    return SetProcessAffinityMask(GetCurrentProcess(), mask);
}

// OFFSET: %0x004BE450
static void SetThreadName(uint32_t threadId, const char* threadName) {
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = threadId;
    info.dwFlags = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
    } __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
}

// OFFSET: %0x004BE4D0
int32_t CInitCritSect::Enter() {
    int32_t result = 0;

    if (!this->m_critsect) {
        while (InterlockedExchange(&this->m_spinLock, 1)) {
        }

        if (!this->m_critsect) {
            this->m_critsect = reinterpret_cast<CCritSect*>(this->m_critsectData);
            new (this->m_critsectData) CCritSect();
            result = 1;
        }

        this->m_spinLock = 0;
    }

    this->m_critsect->Enter();
    return result;
}

// INLINED
void CInitCritSect::Leave() {
    this->m_critsect->Leave();
}

// OFFSET: 0x004B7740/0x1400EF750
static uint32_t STORMAPI ProcessCompletionCallbackThread(void* vdata) {
    PROCESSCOMPLETION* completion = static_cast<PROCESSCOMPLETION*>(vdata);
    WaitForSingleObject(completion->process, INFINITE);
    completion->callback(completion->data);
    CloseHandle(completion->process);
    delete completion;
    return 0;
}

// OFFSET: %0x004BE560
uint32_t STORMAPI S_Thread::s_SLaunchThread(void* threadParam) {
    s_threadCrit.Enter();

    THREADLAUNCH* launch = static_cast<THREADLAUNCH*>(threadParam);
    uint32_t (STORMAPI* threadProc)(void*) = launch->threadProc;
    void* param = launch->param;
    uint32_t threadId = GetCurrentThreadId();
    const char* threadName = NULL;

    for (int32_t i = 0; i < s_numthreads; i++) {
        if (s_threads[i].threadId == threadId) {
            threadName = s_threads[i].name;
            break;
        }
    }

    SMemFree(launch, __FILE__, __LINE__, 0);
    SetThreadName(threadId, threadName);
    s_threadCrit.Leave();

    uint32_t threadVal = threadProc(param);

    s_threadCrit.Enter();

    int32_t numthreads = s_numthreads;

    for (int32_t i = 0; i < numthreads; i++) {
        if (s_threads[i].live && s_threads[i].threadId == threadId) {
            memcpy(&s_threads[i], &s_threads[i + 1], (s_maxthreads - 1 - i) * sizeof(SThreadTrack));
            numthreads--;
        }
    }

    s_numthreads = numthreads;
    s_threadCrit.Leave();

    return threadVal;
}

// OFFSET: #0x007700A0
HANDLE SCreateThread(uint32_t dwStackSize, uint32_t (STORMAPI* lpStartAddress)(void*), void* lpParameter, uint32_t dwCreationFlags, uint32_t* lpThreadId, char* threadName) {
    if (!threadName) {
        threadName = const_cast<char*>("");
    }

    S_Thread::s_threadCrit.Enter();

    if (!S_Thread::s_numthreads) {
        S_Thread::SThreadTrack* track = &S_Thread::s_threads[S_Thread::s_numthreads];
        track->threadId = GetCurrentThreadId();
        track->threadH = NULL;
        track->live = 1;
        track->suspended = 0;
        SStrCopy(track->name, "main", sizeof(track->name));
        S_Thread::s_numthreads++;
    }

    S_Thread::s_threadCrit.Leave();

    THREADLAUNCH* launch = static_cast<THREADLAUNCH*>(SMemAlloc(sizeof(THREADLAUNCH), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    launch->threadProc = lpStartAddress;
    launch->param = lpParameter;

    S_Thread::s_threadCrit.Enter();

    HANDLE hThread = CreateThread(NULL, dwStackSize, reinterpret_cast<LPTHREAD_START_ROUTINE>(S_Thread::s_SLaunchThread), launch, dwCreationFlags, reinterpret_cast<LPDWORD>(lpThreadId));
    launch->threadH = hThread;

    S_Thread::SThreadTrack* track = &S_Thread::s_threads[S_Thread::s_numthreads];
    track->threadId = *lpThreadId;
    track->threadH = launch->threadH;
    track->live = 1;
    track->suspended = (dwCreationFlags & CREATE_SUSPENDED) != 0;

    if (threadName) {
        SStrCopy(track->name, threadName, sizeof(track->name));
    }

    S_Thread::s_numthreads++;
    S_Thread::s_threadCrit.Leave();

    return hThread;
}

// OFFSET: #0x007701C0
int32_t STORMAPI SGetThreadName(uint32_t threadId, char* buffer, int32_t bufferchars) {
    bool found = false;

    if (!buffer || bufferchars < 1) {
        return 0;
    }

    if (bufferchars > static_cast<int32_t>(sizeof(S_Thread::s_threads[0].name))) {
        bufferchars = sizeof(S_Thread::s_threads[0].name);
    }

    *buffer = 0;

    S_Thread::s_threadCrit.Enter();

    for (int32_t i = 0; i < S_Thread::s_numthreads; i++) {
        if (S_Thread::s_threads[i].threadId == threadId) {
            SStrCopy(buffer, S_Thread::s_threads[i].name, bufferchars);
            found = true;
            break;
        }
    }

    S_Thread::s_threadCrit.Leave();

    return found;
}

// PROVEN: b3edfb36 DIFFERENT/IDENTICAL
// OFFSET: 0x004B7B00/0x1400EFDA0
HANDLE STORMAPI SCreateThread(uint32_t (STORMAPI* lpStartAddress)(void*), void* lpParameter, uint32_t* lpThreadId, void* linuxData, char* threadName, uint32_t stackSize) {
    return SCreateThread(stackSize, lpStartAddress, lpParameter, 0, lpThreadId, threadName);
}

// OFFSET: 0x004B7B30/0x1400EFDD0
int32_t STORMAPI SCreateProcess(const char* appName, char* commandLine, void (*callbackWhenProcessCompletes)(void*), void* callbackData, void** processHandle, uint32_t* processId) {
    STARTUPINFOW startInfo;
    memset(&startInfo, 0, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);

    PROCESS_INFORMATION processInfo = {};

    uint16_t appNameW[1040];
    uint16_t commandLineW[1040];

    if (appName) {
        SUniConvertUTF8to16(appNameW, sizeof(appNameW) / sizeof(appNameW[0]), appName, STORM_MAX_STR, NULL, NULL);
    }

    SUniConvertUTF8to16(commandLineW, sizeof(commandLineW) / sizeof(commandLineW[0]), commandLine, STORM_MAX_STR, NULL, NULL);

    if (!CreateProcessW(appName ? reinterpret_cast<LPCWSTR>(appNameW) : NULL, reinterpret_cast<LPWSTR>(commandLineW), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startInfo, &processInfo)) {
        return 0;
    }

    if (callbackWhenProcessCompletes) {
        if (WaitForInputIdle(processInfo.hProcess, 10000)) {
            return 0;
        }

        PROCESSCOMPLETION* completion = static_cast<PROCESSCOMPLETION*>(SMemAlloc(sizeof(PROCESSCOMPLETION), __FILE__, __LINE__, 0));
        completion->callback = callbackWhenProcessCompletes;
        completion->data = callbackData;

        HANDLE currentProcess = GetCurrentProcess();

        if (!DuplicateHandle(currentProcess, processInfo.hProcess, currentProcess, &completion->process, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            TerminateProcess(processInfo.hProcess, 0xC0000001);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
            return 0;
        }

        uint32_t threadId;
        SCreateThread(ProcessCompletionCallbackThread, completion, &threadId, NULL, NULL, 0);

        if (processHandle) {
            *processHandle = processInfo.hProcess;
        } else {
            CloseHandle(NULL);
        }
    } else if (processHandle) {
        *processHandle = processInfo.hProcess;
    } else {
        CloseHandle(processInfo.hProcess);
        CloseHandle(NULL);
    }

    if (processId) {
        *processId = processInfo.dwProcessId;
    }

    ResumeThread(processInfo.hThread);
    CloseHandle(processInfo.hThread);

    return 1;
}

#endif
