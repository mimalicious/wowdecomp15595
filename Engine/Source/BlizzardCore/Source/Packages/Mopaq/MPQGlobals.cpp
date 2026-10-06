#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"

// STATIC: %0x00D829D8
Blizzard::Lock::ConstructOnce<System_MPQGlobals::Globals> System_MPQGlobals::s_globals;

// STATIC: #0x00B382C8
static System_MPQGlobals::RegisterGlobalStorage s_registerGlobalStorage;

// INLINED
bool System_MPQGlobals::ArchiveLess::operator()(const System_SFile::ARCHIVEREC* a, const System_SFile::ARCHIVEREC* b) const {
    if (a->m_priority == b->m_priority) {
        return a->m_id > b->m_id;
    }

    return a->m_priority > b->m_priority;
}

// OFFSET: %0x00745750
System_MPQGlobals::Globals::Globals() {
    this->m_mopaq.m_directAccess = 0;

    if (!Blizzard::Process::GetProcessDirectory(this->m_mopaq.m_basePath, sizeof(this->m_mopaq.m_basePath))) {
        this->m_mopaq.m_basePath[0] = '\0';
    }

    this->m_mopaq.m_md5Mode = 0;
    this->m_mopaq.m_disableAttributes = false;
    this->m_mopaq.m_disableRawRead = false;
    this->m_mopaq.m_disableRepair = false;
}

// OFFSET: %0x00748670
System_MPQGlobals::ArchiveList* System_MPQGlobals::GetArchiveList() {
    return &s_globals.Get()->m_archives;
}

// OFFSET: %0x007486D0
Blizzard::Lock::RMutex* System_MPQGlobals::GetArchiveLock() {
    return &s_globals.Get()->m_archiveLock;
}

// OFFSET: %0x00748730
System_MPQGlobals::MopaqGlobals* System_MPQGlobals::GetMopaqGlobals() {
    return &s_globals.Get()->m_mopaq;
}

// OFFSET: %0x00748790
Blizzard::Lock::RMutex* System_MPQGlobals::GetMopaqGlobalsLock() {
    return &s_globals.Get()->m_mopaqLock;
}

// OFFSET: %0x00749FB0
System_SFile::ARCHIVEREC* Blizzard::Mopaq::SFileGetArchiveHandle(const char* name) {
    System_MPQGlobals::ArchiveList* archives = System_MPQGlobals::GetArchiveList();

    for (System_MPQGlobals::ArchiveList::iterator it = archives->begin(); it != archives->end(); ++it) {
        if (name && *it && !_stricmp(name, (*it)->m_name)) {
            return *it;
        }
    }

    return NULL;
}

// OFFSET: %0x00745500
System_MPQGlobals::Globals::~Globals() {
}

// OFFSET: %0x00745160
System_MPQGlobals::WorkerPool::~WorkerPool() {
    for (std::vector<Worker*>::iterator it = this->m_workers.begin(); it != this->m_workers.end(); ++it) {
        delete *it;
    }
}

// OFFSET: %0x00742460
System_MPQGlobals::Worker::~Worker() {
    this->m_proc = NULL;
    Blizzard::Lock::EventSet(this->m_start.m_event);
    Blizzard::Thread::WaitForever(this->m_thread);
    Blizzard::Thread::Close(this->m_thread);
}

// PROVEN: b0ef04b2 IDENTICAL/DIFFERENT
// OFFSET: 0x007EC360/0x1404F8860
void System_MPQGlobals::RegisterGlobalStorage::Shutdown() {
    if (s_globals.m_constructed) {
        reinterpret_cast<Globals*>(s_globals.m_storage)->~Globals();
        s_globals.m_constructed = false;
        s_globals.m_count.m_value = 0;
    }
}

// OFFSET: #0x00451C90
System_MPQGlobals::RegisterGlobalStorage::RegisterGlobalStorage() : Blizzard::Using::Use("Blizzard::Mopaq") {
    System_Using::Intern(this->m_name)->shutdownproc = System_MPQGlobals::RegisterGlobalStorage::Shutdown;
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::Memory");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::File");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::ThreadLocalStorage");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "System_Mopaq::IOStackManager");
    Blizzard::Using::DependsOn("Blizzard::ThreadLocalStorage", "Blizzard::Memory");
}
