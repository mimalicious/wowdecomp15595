#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// STATIC: %0x00D86BAC
std::map<std::string, Blizzard::Storage::StorageUnit*>* Blizzard::Storage::s_StorageMap;

// STATIC: %0x00D86BB0
std::deque<Blizzard::Storage::STORAGE_CMD>* Blizzard::Storage::s_CmdQueue_Storage;

// STATIC: %0x00D81380
Blizzard::Lock::Mutex Blizzard::Storage::s_storageMutex;

// STATIC: %0x00D81398
Blizzard::Lock::Event Blizzard::Storage::s_cmdEvent;

// STATIC: %0x00D86BA5
bool Blizzard::Storage::s_storageReady;

// STATIC: %0x00D86BA6
volatile bool Blizzard::Storage::s_threadShutdown;

// STATIC: %0x00D86BA7
volatile bool Blizzard::Storage::s_threadExited;

// STATIC: %0x00C0C524
int32_t Blizzard::Storage::s_Chunk_Size = 0x4000;

// OFFSET: %0x007467A0
bool Blizzard::Storage::Initialize() {
    if (!s_storageReady) {
        s_StorageMap = new std::map<std::string, StorageUnit*>();
        s_CmdQueue_Storage = new std::deque<STORAGE_CMD>();
        Blizzard::Lock::MutexCreate(s_storageMutex);
        Blizzard::Lock::EventCreate(s_cmdEvent);
        s_threadShutdown = false;
        s_threadExited = false;
        Blizzard::Thread::CreateDetached(StorageThread, NULL, "Streaming Storage Thread");
        s_storageReady = true;
    }

    return true;
}

// OFFSET: %0x00742C80
void Blizzard::Storage::Shutdown() {
    if (s_storageReady && s_StorageMap) {
        s_threadShutdown = true;
        Blizzard::Lock::EventSet(s_cmdEvent);

        while (!s_threadExited) {
            Blizzard::Process::Sleep(10);
        }

        Blizzard::Lock::EventDestroy(s_cmdEvent);
        s_storageReady = false;
    }

    std::deque<STORAGE_CMD>* queue = s_CmdQueue_Storage;
    delete s_StorageMap;
    s_StorageMap = NULL;
    delete queue;
    s_CmdQueue_Storage = NULL;
    Blizzard::Lock::MutexDestroy(s_storageMutex);
}

// OFFSET: %0x00740C20
Blizzard::Storage::StorageUnit* Blizzard::Storage::Acquire(const std::string& name) {
    if (!s_storageReady) {
        return NULL;
    }

    Blizzard::Lock::MutexEnter(s_storageMutex);

    StorageUnit* unit;
    std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->find(name);

    if (it == s_StorageMap->end()) {
        (*s_StorageMap)[name] = new StorageUnit();
        unit = (*s_StorageMap)[name];
    } else {
        unit = it->second;
    }

    if (unit) {
        unit->m_refCount.Increment();
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return unit;
}

// OFFSET: %0x00732190
bool Blizzard::Storage::Close(StorageUnit* unit) {
    if (!s_storageReady) {
        return true;
    }

    Blizzard::Lock::MutexEnter(s_storageMutex);

    if (unit->m_refCount.m_value) {
        Blizzard::Lock::MutexLeave(s_storageMutex);
        return false;
    }

    for (std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->begin(); it != s_StorageMap->end(); ++it) {
        if (it->second == unit) {
            s_StorageMap->erase(it);
            break;
        }
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return true;
}

// OFFSET: %0x007290C0
Blizzard::Storage::StorageUnit* Blizzard::Storage::GetStorageInfo(const std::string& name) {
    if (!s_storageReady) {
        return NULL;
    }

    std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->find(name);

    if (it == s_StorageMap->end()) {
        return NULL;
    }

    return it->second;
}

// OFFSET: %0x00735E40
void Blizzard::Storage::QueueWriteCommand(StorageUnit* unit, int16_t tag, int64_t offset, int32_t size, BufferHandle handle) {
    STORAGE_CMD cmd;
    cmd.m_type = 1;
    cmd.m_unit = unit;
    cmd.m_handle = handle;
    cmd.m_tag = tag;
    cmd.m_offset = offset;
    cmd.m_length = size;

    if (unit) {
        unit->m_refCount.Increment();
    }

    s_CmdQueue_Storage->push_back(cmd);
    SetRangeAs(unit, offset, size, ChunkState::CHUNK_CACHED);
    Blizzard::Lock::EventSet(s_cmdEvent);
}

// OFFSET: %0x0073B5D0
uint32_t Blizzard::Storage::StorageThread(void* param) {
    if (s_threadShutdown) {
        s_threadExited = true;
        return 1;
    }

    do {
        std::deque<STORAGE_CMD>* queue = s_CmdQueue_Storage;
        Blizzard::Lock::EventWaitForever(s_cmdEvent);

        if (s_threadShutdown) {
            break;
        }

        while (true) {
            Blizzard::Lock::MutexEnter(s_storageMutex);

            if (s_CmdQueue_Storage->empty() || g_StorageSystem.m_writebackLock) {
                Blizzard::Lock::MutexLeave(s_storageMutex);
                break;
            }

            STORAGE_CMD& cmd = queue->front();

            if (cmd.m_type == 1) {
                if (cmd.m_unit->m_local->Write(g_StorageSystem.GetBuffer(cmd.m_handle), cmd.m_offset, cmd.m_length)) {
                    SetRangeAs(cmd.m_unit, cmd.m_offset, cmd.m_length, ChunkState::CHUNK_LOCAL);
                } else {
                    SetRangeAs(cmd.m_unit, cmd.m_offset, cmd.m_length, ChunkState::CHUNK_NONE);
                }
            }

            Release(cmd.m_unit);

            if (cmd.m_handle.all) {
                g_StorageSystem.ReleaseChunks(cmd.m_handle);
            }

            s_CmdQueue_Storage->pop_front();
            Blizzard::Lock::MutexLeave(s_storageMutex);

            if (s_threadShutdown) {
                s_threadExited = true;
                return 1;
            }
        }
    } while (!s_threadShutdown);

    s_threadExited = true;

    return 1;
}

// OFFSET: %0x00724CA0
void Blizzard::Mopaq::SFileGetTotalIsLocalAmount(int64_t& local, int64_t& total) {
    Blizzard::Lock::MutexEnter(Blizzard::Storage::s_storageMutex);

    for (std::map<std::string, Blizzard::Storage::StorageUnit*>::iterator it = Blizzard::Storage::s_StorageMap->begin(); it != Blizzard::Storage::s_StorageMap->end(); ++it) {
        Blizzard::Storage::StorageUnit* unit = it->second;

        if (unit && unit->m_local) {
            total += unit->m_logicalFileSize;
            local += unit->m_local->GetTotalLocalSize();
        }
    }

    Blizzard::Lock::MutexLeave(Blizzard::Storage::s_storageMutex);
}
