#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: 0x00EBA238
std::map<std::string, Blizzard::Storage::StorageUnit*>* Blizzard::Storage::s_StorageMap;

// STATIC: 0x00EB4A20/0x140DA0940
Blizzard::Lock::Mutex Blizzard::Storage::s_storageMutex;

// STATIC: 0x00EB4A38
Blizzard::Lock::Event Blizzard::Storage::s_cmdEvent;

// STATIC: 0x00EBA7FC
bool Blizzard::Storage::s_storageReady;

// STATIC: 0x00EBA231
volatile bool Blizzard::Storage::s_threadShutdown;

// STATIC: 0x00EBA232
volatile bool Blizzard::Storage::s_threadExited;

// STATIC: 0x00EBA800
volatile long Blizzard::Storage::s_initLock;

// STATIC: 0x00D07178
int32_t Blizzard::Storage::s_Chunk_Size = 0x4000;

// OFFSET: 0x007B3970
bool Blizzard::Storage::IsInitialized() {
    while (InterlockedCompareExchange(&s_initLock, 1, 0)) {
        Blizzard::Process::Sleep(1);
    }

    bool ready = s_storageReady;
    InterlockedCompareExchange(&s_initLock, 0, 1);

    return ready;
}

// OFFSET: 0x007EB330
bool Blizzard::Storage::Initialize() {
    while (InterlockedCompareExchange(&s_initLock, 1, 0)) {
        Blizzard::Process::Sleep(1);
    }

    if (!s_storageReady) {
        s_StorageMap = new std::map<std::string, StorageUnit*>();
        Blizzard::Lock::MutexCreate(s_storageMutex);
        Blizzard::Lock::EventCreate(s_cmdEvent);
        s_threadShutdown = false;
        s_threadExited = false;
        Blizzard::Thread::CreateDetached(StorageThread, NULL, "Streaming Storage Thread");
        s_storageReady = true;
    }

    InterlockedCompareExchange(&s_initLock, 0, 1);

    return true;
}

// OFFSET: 0x007E4510/0x1404EC620
void Blizzard::Storage::Shutdown() {
    while (InterlockedCompareExchange(&s_initLock, 1, 0)) {
        Blizzard::Process::Sleep(1);
    }

    if (s_storageReady) {
        if (s_StorageMap) {
            s_threadShutdown = true;
            Blizzard::Lock::EventSet(s_cmdEvent);

            while (!s_threadExited) {
                Blizzard::Process::Sleep(10);
            }

            Blizzard::Lock::EventDestroy(s_cmdEvent);
        }

        if (!s_partialBlockMapDestroyed.m_value) {
            s_partialBlockMap.clear();
        }

        {
            Blizzard::Lock::ScopedMutex lock(s_storageMutex);
            std::map<std::string, StorageUnit*>* map = s_StorageMap;
            s_StorageMap = NULL;

            if (map) {
                if (map->size()) {
                    char buffer[1024];
                    buffer[0] = '\0';
                    uint32_t length = Blizzard::String::Format(buffer, sizeof(buffer), "Storage File Leaks !\n");

                    for (std::map<std::string, StorageUnit*>::iterator it = map->begin(); it != map->end() && length < 1023; ++it) {
                        StorageUnit* unit = it->second;
                        length += Blizzard::String::Format(&buffer[length], sizeof(buffer) - length, "%s : refcount = %d cacheEntries = %d \n", it->first.c_str(), unit->m_refCount.m_value, unit->m_cacheEntries->size());
                    }
                }

                delete map;
            }
        }

        Blizzard::Lock::MutexDestroy(s_storageMutex);
        s_storageReady = false;
    }

    InterlockedCompareExchange(&s_initLock, 0, 1);
}

// OFFSET: 0x007DF310
Blizzard::Storage::StorageUnit* Blizzard::Storage::Acquire(const std::string& name, bool existingOnly) {
    if (!IsInitialized()) {
        return NULL;
    }

    Blizzard::Lock::ScopedMutex lock(s_storageMutex);
    StorageUnit* unit;
    std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->find(name);

    if (it == s_StorageMap->end()) {
        if (existingOnly) {
            return NULL;
        }

        (*s_StorageMap)[name] = new StorageUnit();
        unit = (*s_StorageMap)[name];
    } else {
        unit = it->second;
    }

    if (unit) {
        unit->m_refCount.Increment();
    }

    return unit;
}

// OFFSET: 0x007D09E0
bool Blizzard::Storage::Close(StorageUnit* unit) {
    if (!IsInitialized()) {
        return true;
    }

    Blizzard::Lock::ScopedMutex lock(s_storageMutex);

    if (unit->m_refCount.m_value) {
        return false;
    }

    for (std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->begin(); it != s_StorageMap->end(); ++it) {
        if (it->second == unit) {
            s_StorageMap->erase(it);
            break;
        }
    }

    return true;
}

// OFFSET: 0x007C7D60/0x1404C3010
bool Blizzard::Storage::Exists(const std::string& name) {
    if (!IsInitialized()) {
        return false;
    }

    return s_StorageMap->find(name) != s_StorageMap->end();
}

// OFFSET: 0x007DF140
uint32_t Blizzard::Storage::StorageThread(void* param) {
    std::map<std::string, StorageUnit*>* map = s_StorageMap;

    if (s_threadShutdown) {
        s_threadExited = true;
        return 1;
    }

    do {
        Blizzard::Lock::EventWaitForever(s_cmdEvent);

        if (s_threadShutdown) {
            break;
        }

        while (true) {
            {
                Blizzard::Lock::ScopedMutex lock(s_storageMutex);
                std::map<std::string, StorageUnit*>::iterator it;

                for (it = map->begin(); it != map->end(); ++it) {
                    if (it->second->m_cacheEntries->size()) {
                        break;
                    }
                }

                if (it == map->end()) {
                    break;
                }

                StorageUnitRef ref(it->second);
                ref.m_unit->FlushCacheEntries(0x100000);
            }

            if (s_threadShutdown) {
                s_threadExited = true;
                return 1;
            }
        }
    } while (!s_threadShutdown);

    s_threadExited = true;

    return 1;
}

// OFFSET: 0x007C3BF0/0x1404BE310
void Blizzard::Mopaq::SFileGetTotalIsLocalAmount(int64_t& local, int64_t& total) {
    Blizzard::Lock::MutexEnter(Blizzard::Storage::s_storageMutex);

    std::map<std::string, Blizzard::Storage::StorageUnit*>* map = Blizzard::Storage::s_StorageMap;

    for (std::map<std::string, Blizzard::Storage::StorageUnit*>::iterator it = map->begin(); it != map->end(); ++it) {
        Blizzard::Storage::StorageUnit* unit = it->second;

        if (unit && unit->m_local) {
            total += unit->m_logicalFileSize;
            local += unit->m_local->GetTotalLocalSize();
        }
    }

    Blizzard::Lock::MutexLeave(Blizzard::Storage::s_storageMutex);
}
