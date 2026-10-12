#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"

// STATIC: 0x00EB6060
Blizzard::Lock::ConstructOnce<System_MPQGlobals::Globals> System_MPQGlobals::s_globals;

static System_MPQGlobals::RegisterGlobalStorage s_registerGlobalStorage;

// INLINED
bool System_MPQGlobals::ArchiveLess::operator()(const System_SFile::ARCHIVEREC* a, const System_SFile::ARCHIVEREC* b) const {
    if (a->m_priority == b->m_priority) {
        return a->m_id > b->m_id;
    }

    return a->m_priority > b->m_priority;
}

// OFFSET: 0x007E6060
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

// OFFSET: 0x007EE190/0x1404FAB90
System_MPQGlobals::ArchiveList* System_MPQGlobals::GetArchiveList() {
    return &s_globals.Get()->m_archives;
}

// OFFSET: 0x007EE1F0/0x1404FAC20
Blizzard::Lock::RMutex* System_MPQGlobals::GetArchiveLock() {
    return &s_globals.Get()->m_archiveLock;
}

// OFFSET: 0x007EE250/0x1404FACB0
System_MPQGlobals::MopaqGlobals* System_MPQGlobals::GetMopaqGlobals() {
    return &s_globals.Get()->m_mopaq;
}

// OFFSET: 0x007EE2B0/0x1404FAD40
Blizzard::Lock::RMutex* System_MPQGlobals::GetMopaqGlobalsLock() {
    return &s_globals.Get()->m_mopaqLock;
}

// OFFSET: 0x007F07A0
bool Blizzard::Mopaq::SFileEnableRawRead(bool enable) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
    System_MPQGlobals::GetMopaqGlobals()->m_disableRawRead = !enable;

    return true;
}

// OFFSET: 0x007F1440/0x1404FE140
System_SFile::ARCHIVEREC* Blizzard::Mopaq::SFileGetArchiveHandle(const char* name) {
    System_MPQGlobals::ArchiveList* archives = System_MPQGlobals::GetArchiveList();

    for (System_MPQGlobals::ArchiveList::iterator it = archives->begin(); it != archives->end(); ++it) {
        if (name && *it && !_stricmp(name, (*it)->m_name)) {
            return *it;
        }
    }

    return NULL;
}

// OFFSET: 0x007E5E10/0x1404EF5C0
System_MPQGlobals::Globals::~Globals() {
}

// OFFSET: 0x007D35B0
System_MPQGlobals::WorkerPool::WorkerPool() {
    Blizzard::Hardware::GetProcessorFeatures();
    System_Hardware::ReadLogicalProcessorInformation();
    this->m_maxWorkers = System_Hardware::s_processorFeatures.m_coreCount;
}

// OFFSET: 0x007E4050/0x1404EBFA0
System_MPQGlobals::WorkerPool::~WorkerPool() {
    for (std::vector<Worker*>::iterator it = this->m_workers.begin(); it != this->m_workers.end(); ++it) {
        delete *it;
    }
}

// OFFSET: 0x007E0E30/0x1404E6EA0
System_MPQGlobals::Worker::~Worker() {
    this->m_proc = NULL;
    Blizzard::Lock::EventSet(this->m_start.m_event);
    Blizzard::Thread::WaitForever(this->m_thread);
    Blizzard::Thread::Close(this->m_thread);
}

// OFFSET: 0x007EC360/0x1404F8860
void System_MPQGlobals::RegisterGlobalStorage::Shutdown() {
    if (s_globals.m_constructed) {
        reinterpret_cast<Globals*>(s_globals.m_storage)->~Globals();
        s_globals.m_constructed = false;
        s_globals.m_count.m_value = 0;
    }
}

// OFFSET: 0x007EE110/0x1404FA8E0
System_MPQGlobals::RegisterGlobalStorage::RegisterGlobalStorage() : Blizzard::Using::Use("Blizzard::Mopaq") {
    System_Using::Intern(this->m_name)->shutdownproc = System_MPQGlobals::RegisterGlobalStorage::Shutdown;
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::Memory");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::File");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "Blizzard::ThreadLocalStorage");
    Blizzard::Using::DependsOn("Blizzard::Mopaq", "System_Mopaq::IOStackManager");
    Blizzard::Using::DependsOn("Blizzard::ThreadLocalStorage", "Blizzard::Memory");
}
