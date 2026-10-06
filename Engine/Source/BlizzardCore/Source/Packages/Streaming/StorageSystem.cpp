#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D86DE8
Blizzard::Storage::StorageSystem Blizzard::Storage::g_StorageSystem;

// OFFSET: %0x0071DD90
Blizzard::Storage::StorageSystem::StorageSystem() {
    this->m_writebackLock = 0;
    Blizzard::Lock::MutexCreate(this->m_mutex);
    this->m_buffer = NULL;
    this->m_buffer = static_cast<uint8_t*>(SMemAlloc(s_Chunk_Size << 7, __FILE__, __LINE__, 0));
    this->m_allocationMarkers = static_cast<uint8_t*>(SMemAlloc(128, __FILE__, __LINE__, 0));
    memset(this->m_allocationMarkers, 0, 128);
    this->m_maxEnd = 0;
    this->m_maxAllocation = 0;
    this->m_totalAllocated = 0;
    this->m_allocated = 0;
    this->m_peakAllocated = 0;
    this->m_markerCount = 128;
}

// OFFSET: %0x00AB1450
Blizzard::Storage::StorageSystem::~StorageSystem() {
    SMemFree(this->m_allocationMarkers, __FILE__, __LINE__, 0);
    SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// OFFSET: %0x00716160
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::AllocateChunks(int32_t size) {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    int32_t count = (s_Chunk_Size + size - 1) / s_Chunk_Size;
    int32_t run = 0;
    int32_t start = -1;
    BufferHandle result;
    result.all = 0;

    for (int32_t i = 0; i < this->m_markerCount; i++) {
        if (this->m_allocationMarkers[i]) {
            start = -1;
            run = 0;
        } else {
            if (start == -1) {
                start = i;
            }

            if (++run == count) {
                break;
            }
        }
    }

    if (run == count && start != -1) {
        result.m.start = start;
        result.m.length = run;

        for (int32_t i = 0; i < run; i++) {
            this->m_allocationMarkers[start + i] = 1;
        }

        if (this->m_maxEnd < start + run) {
            this->m_maxEnd = start + run;
        }

        if (this->m_maxAllocation < run) {
            this->m_maxAllocation = run;
        }

        int32_t peak = this->m_peakAllocated < this->m_allocated ? this->m_allocated : this->m_peakAllocated;
        this->m_totalAllocated += run;
        this->m_allocated += run;
        this->m_peakAllocated = peak;
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);

    return result;
}

// OFFSET: %0x00716280
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::AllocateSubChunks(BufferHandle handle, int32_t offset, int32_t size) {
    BufferHandle result;
    int32_t count = (size + s_Chunk_Size - 1) / s_Chunk_Size;

    if (!count) {
        result.all = 0;
        return result;
    }

    result.all = ((count & 0x7F) << 7) | ((handle.m.start + offset / s_Chunk_Size) & 0x7F);

    Blizzard::Lock::MutexEnter(this->m_mutex);

    for (int32_t i = 0; i < count; i++) {
        this->m_allocationMarkers[result.m.start + i]++;
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);

    return result;
}

// OFFSET: %0x00716310
void Blizzard::Storage::StorageSystem::ReleaseChunks(BufferHandle handle) {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    if (handle.m.length) {
        for (uint32_t i = 0; i < handle.m.length; i++) {
            this->m_allocationMarkers[handle.m.start + i]--;

            if (!this->m_allocationMarkers[handle.m.start + i]) {
                this->m_allocated--;
            }
        }
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: %0x007366C0
bool Blizzard::Storage::StorageSystem::RecoverMemory(StorageUnit* unit, int32_t level) {
    if (!level) {
        unit->FlushCacheEntries(0);
        return true;
    }

    if (level == 1) {
        Blizzard::Lock::MutexEnter(s_storageMutex);

        for (std::map<std::string, StorageUnit*>::iterator it = s_StorageMap->begin(); it != s_StorageMap->end(); ++it) {
            it->second->FlushCacheEntries(0);
        }
    } else {
        if (level != 2) {
            return false;
        }

        Blizzard::Lock::MutexEnter(s_storageMutex);

        std::deque<STORAGE_CMD>* queue = s_CmdQueue_Storage;
        int32_t count = static_cast<int32_t>(queue->size());

        for (int32_t i = 0; i < count; i++) {
            STORAGE_CMD& cmd = (*queue)[i];

            if (cmd.m_handle.all) {
                g_StorageSystem.ReleaseChunks(cmd.m_handle);
            }

            Release(cmd.m_unit);
        }

        queue->clear();
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return true;
}

// OFFSET: %0x00736870
Blizzard::Storage::BufferHandle Blizzard::Storage::AllocateChunksRecovering(StorageUnit* unit, int32_t size) {
    BufferHandle handle;
    int32_t level = 0;

    do {
        handle = g_StorageSystem.AllocateChunks(size);

        if (handle.all) {
            break;
        }
    } while (g_StorageSystem.RecoverMemory(unit, level++));

    return handle;
}
