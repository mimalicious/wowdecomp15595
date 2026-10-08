#include <algorithm>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"

// STATIC: 0x00EBA7C0
Blizzard::Storage::StorageSystem Blizzard::Storage::g_StorageSystem;

// OFFSET: 0x007BC1A0
Blizzard::Storage::StorageSystem::StorageSystem() {
    this->m_writebackLock = 0;
    Blizzard::Lock::MutexCreate(this->m_mutex);
    this->m_buffer = NULL;
    this->m_buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(s_Chunk_Size << 7));
    this->m_allocationMarkers = static_cast<uint8_t*>(Blizzard::Memory::Allocate(128));
    memset(this->m_allocationMarkers, 0, 128);
    this->m_maxEnd = 0;
    this->m_maxAllocation = 0;
    this->m_totalAllocated = 0;
    this->m_allocated = 0;
    this->m_peakAllocated = 0;
    this->m_markerCount = 128;
}

// INLINED
Blizzard::Storage::StorageSystem::~StorageSystem() {
    Blizzard::Memory::Free(this->m_allocationMarkers);
    Blizzard::Memory::Free(this->m_buffer);
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// OFFSET: 0x007B4260
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::FindFreeRun(int32_t count, int32_t start) {
    BufferHandle result;
    int32_t found = 0;

    for (int32_t i = start; i < this->m_markerCount; i++) {
        if (this->m_allocationMarkers[i]) {
            if (found) {
                break;
            }
        } else {
            if (!found) {
                result.m.start = i;
            }

            if (++found == count) {
                break;
            }
        }
    }

    int32_t limit = this->m_markerCount - 1;
    result.m.length = std::min(found, limit);

    return result;
}

// OFFSET: 0x007B42D0
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::AllocateChunks(int32_t count) {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);
    BufferHandle result;
    int32_t start = 0;

    if (count) {
        do {
            if (start + count >= this->m_markerCount) {
                break;
            }

            result = this->FindFreeRun(count, start);
            start = result.m.length + result.m.start + 1;
        } while (result.m.length != count);
    }

    int32_t length = result.m.length;

    if (length == count) {
        int32_t first = result.m.start;

        for (int32_t i = 0; i < length; i++) {
            this->m_allocationMarkers[first + i] = 1;
        }

        int32_t end = length + first;
        this->m_maxEnd = std::max(this->m_maxEnd, end);
        this->m_maxAllocation = std::max(this->m_maxAllocation, length);
        this->m_totalAllocated += length;
        this->m_allocated += length;
        this->m_peakAllocated = std::max(this->m_peakAllocated, this->m_allocated);
    } else {
        result.all = 0;
    }

    return result;
}

// OFFSET: 0x007B43C0
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::AllocateChunksUpTo(int32_t count) {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);
    BufferHandle result;
    int32_t start = 0;

    if (count) {
        while (result.m.length + start < this->m_markerCount) {
            BufferHandle run = this->FindFreeRun(count, start);

            if (run.m.length > result.m.length) {
                result = run;
            }

            start = run.m.start + run.m.length + 1;

            if (run.m.length == count) {
                break;
            }
        }
    }

    int32_t length = result.m.length;

    if (length) {
        int32_t first = result.m.start;

        for (int32_t i = 0; i < length; i++) {
            this->m_allocationMarkers[first + i] = 1;
        }

        int32_t end = length + first;
        this->m_maxEnd = std::max(this->m_maxEnd, end);
        this->m_maxAllocation = std::max(this->m_maxAllocation, length);
        this->m_totalAllocated += length;
        this->m_allocated += length;
        this->m_peakAllocated = std::max(this->m_peakAllocated, this->m_allocated);
    }

    return result;
}

// OFFSET: 0x007B44C0
Blizzard::Storage::BufferHandle Blizzard::Storage::StorageSystem::AllocateSubChunks(BufferHandle handle, int32_t offset, int32_t size) {
    int32_t count = (size + s_Chunk_Size - 1) / s_Chunk_Size;
    BufferHandle result;

    if (!count) {
        return result;
    }

    result.m.start = handle.m.start + offset / s_Chunk_Size;
    result.m.length = count;

    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    for (int32_t i = 0; i < count; i++) {
        this->m_allocationMarkers[result.m.start + i]++;
    }

    return result;
}

// OFFSET: 0x007B4550/0x1404AA100
void Blizzard::Storage::StorageSystem::ReleaseChunks(BufferHandle handle) {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    if (!handle.m.length) {
        return;
    }

    uint32_t i = 0;

    do {
        this->m_allocationMarkers[handle.m.start + i]--;

        if (!this->m_allocationMarkers[handle.m.start + i]) {
            this->m_allocated--;
        }

        i++;
    } while (i < handle.m.length);
}
