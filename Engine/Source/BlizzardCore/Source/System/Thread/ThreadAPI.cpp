#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// STATIC: #0x00B38334
static System_Thread::UsingThread s_usingThread;

// OFFSET: 0x007E9FE0
bool Blizzard::Thread::CreateDetached(uint32_t (*function)(void*), void* param, const char* name) {
    ThreadRecord* thread;
    bool result = Create(function, param, &thread, name, 0, 0);

    if (result) {
        Close(thread);
    }

    return result;
}

// OFFSET: 0x007E9EE0/0x1404F59C0
System_Thread::UsingThread::UsingThread() : Blizzard::Using::Use("Blizzard::Thread") {
    System_Using::Intern(this->m_name)->shutdownproc = System_Thread::ShutdownThreadSystem;
    Blizzard::Using::DependsOn("Blizzard::Thread", "Blizzard::Memory");
    Blizzard::Using::DependsOn("Blizzard::ThreadLocalStorage", "Blizzard::Thread");
    System_Thread::InitThreadSystem();
}
