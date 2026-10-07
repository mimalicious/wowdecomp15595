#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007359A0
Blizzard::Storage::StorageUnit::StorageUnit() {
    this->m_valid = true;
    this->m_flushed = false;
    this->m_logicalFileSize = 0;
    this->m_chunkSize = s_Chunk_Size;
    this->m_refCount.m_value = 0;
    this->m_local = NULL;
    this->m_cache = NULL;
    this->m_http = NULL;
    this->m_cacheTag = 0;
    this->m_cacheEntries = new std::deque<STORAGE_CMD>();
}

// OFFSET: %0x0072E230
Blizzard::Storage::StorageUnit::~StorageUnit() {
    delete this->m_cacheEntries;
    delete this->m_local;
    delete this->m_cache;
    delete this->m_http;
}

// OFFSET: 0x007ABBD0
void Blizzard::Storage::StorageUnit::SetCacheSource() {
    delete this->m_cache;
    this->m_cache = new CacheSource(this);
}

// OFFSET: %0x007291D0
void Blizzard::Storage::StorageUnit::SetHttpSource(const std::string& path, int64_t size) {
    delete this->m_http;
    this->m_http = new HttpSource(path, size);
}

// OFFSET: %0x0073BAF0
void Blizzard::Storage::StorageUnit::ReserveChunks() {
    uint32_t count = static_cast<uint32_t>((this->m_logicalFileSize + this->m_chunkSize - 1) / this->m_chunkSize);
    this->m_chunkStates.resize(count);
    memset(&this->m_chunkStates[0], 0, count);
    this->m_flushed = false;
}

// OFFSET: 0x007ABBA0
void Blizzard::Storage::StorageUnit::SetLocal(BackingStore* local, int64_t size) {
    delete this->m_local;
    this->m_logicalFileSize = size;
    this->ReserveChunks();
    this->m_local = local;
}

// OFFSET: %0x0071CD40
bool Blizzard::Storage::StorageUnit::ReadLocal(void* buffer, int64_t offset, int32_t size) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    if (!this->m_local) {
        Blizzard::Lock::MutexLeave(s_storageMutex);
        return false;
    }

    int32_t end = static_cast<int32_t>((offset + size) / this->m_chunkSize);
    uint32_t index = static_cast<uint32_t>(offset / this->m_chunkSize);

    if (index != end) {
        do {
            if (!(this->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK) && this->m_local->IsAvailable(static_cast<int64_t>(static_cast<int32_t>(index)) * this->m_chunkSize, this->m_chunkSize)) {
                this->m_chunkStates[index] = (this->m_chunkStates[index] & ChunkState::CHUNK_CACHE_ENTRY) | ChunkState::CHUNK_LOCAL;
            }

            if ((this->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK) != ChunkState::CHUNK_LOCAL) {
                Blizzard::Lock::MutexLeave(s_storageMutex);
                return false;
            }
        } while (++index != end);
    }

    bool result = this->m_local->Read(buffer, offset, size);
    Blizzard::Lock::MutexLeave(s_storageMutex);

    return result;
}

// OFFSET: %0x00735EB0
void Blizzard::Storage::StorageUnit::FlushCacheEntries(int16_t tag) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    if (tag == this->m_cacheTag) {
        this->m_cacheTag = 0;
    }

    if (this->m_cacheEntries) {
        std::deque<STORAGE_CMD>::iterator it = this->m_cacheEntries->begin();

        while (it != this->m_cacheEntries->end()) {
            if (tag && tag != it->m_tag) {
                ++it;
                continue;
            }

            g_StorageSystem.ReleaseChunks(it->m_handle);
            Release(it->m_unit);

            if (tag) {
                it->m_unit->m_chunkStates[static_cast<uint32_t>(it->m_offset / this->m_chunkSize)] &= ~ChunkState::CHUNK_CACHE_ENTRY;
                it = this->m_cacheEntries->erase(it);
            } else {
                ++it;
            }
        }

        if (!tag) {
            this->m_cacheEntries->clear();
        }
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);
}

// OFFSET: %0x007360F0
void Blizzard::Storage::StorageUnit::AddCacheEntry(int16_t tag, int64_t offset, int32_t size, BufferHandle handle) {
    if (!tag && this->m_cacheTag) {
        g_StorageSystem.ReleaseChunks(handle);
        return;
    }

    if (this) {
        this->m_refCount.Increment();
    }

    STORAGE_CMD entry;
    entry.m_type = 1;
    entry.m_unit = this;
    entry.m_handle = handle;
    entry.m_tag = tag;
    entry.m_offset = offset;
    entry.m_length = size;
    this->m_cacheEntries->push_back(entry);

    uint32_t index = static_cast<uint32_t>(offset / this->m_chunkSize);
    this->m_chunkStates[index] = (this->m_chunkStates[index] & ChunkState::CHUNK_CACHE_ENTRY) | ChunkState::CHUNK_CACHED;
    this->m_chunkStates[index] |= ChunkState::CHUNK_CACHE_ENTRY;
}

// OFFSET: %0x0073BB90
bool Blizzard::Storage::StorageUnit::FetchDeferData(Range* range, int16_t tag) {
    int32_t count = 0;
    Range* ranges = GetRange(this, range->m_chunkOffset, range->m_chunkLength, &count, true);

    if (!ranges || !count) {
        return false;
    }

    bool fetch = false;
    ChunkState::State errorState = ChunkState::CHUNK_LOCAL;

    Blizzard::Lock::MutexEnter(s_storageMutex);

    for (int32_t i = 0; i < count; i++) {
        if (ranges[i].m_state == 2) {
            errorState = ChunkState::CHUNK_NONE;
        } else if (ranges[i].m_state == 3) {
            errorState = ChunkState::CHUNK_REQUESTED;
        } else {
            continue;
        }

        SetRangeAs(this, ranges[i].m_chunkOffset, ranges[i].m_chunkLength, ChunkState::CHUNK_DOWNLOADING);
        fetch = true;
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);
    SMemFree(ranges, __FILE__, __LINE__, 0);

    if (!fetch) {
        return true;
    }

    BufferHandle handle = AllocateChunksRecovering(this, range->m_chunkLength);

    if (!handle.all) {
        FetchError(this, range->m_chunkOffset, range->m_chunkLength, errorState);
        return false;
    }

    bool result = false;

    if (!this->m_http || !(result = this->m_http->Read(g_StorageSystem.GetBuffer(handle), range->m_chunkOffset, range->m_chunkLength)) || !(result = Write(this, handle, range->m_chunkOffset, range->m_chunkLength, tag))) {
        FetchError(this, range->m_chunkOffset, range->m_chunkLength, errorState);
    }

    g_StorageSystem.ReleaseChunks(handle);

    return result;
}

// OFFSET: %0x0071CA40
bool Blizzard::Storage::Read(StorageUnit* unit, void* buffer, int64_t offset, int32_t size) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    bool result = false;
    uint32_t index = static_cast<uint32_t>(offset / unit->m_chunkSize);
    uint8_t state = unit->m_chunkStates[index];

    if ((state & ChunkState::CHUNK_STATE_MASK) == ChunkState::CHUNK_DOWNLOADING || (state & ChunkState::CHUNK_STATE_MASK) == ChunkState::CHUNK_REQUESTED) {
        while ((unit->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK) == ChunkState::CHUNK_DOWNLOADING || (unit->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK) == ChunkState::CHUNK_REQUESTED) {
            Blizzard::Lock::MutexLeave(s_storageMutex);
            Blizzard::Process::Sleep(1);
            Blizzard::Lock::MutexEnter(s_storageMutex);
        }

        state = unit->m_chunkStates[index];
    }

    if (!(state & ChunkState::CHUNK_STATE_MASK) && unit->m_local && unit->m_local->IsAvailable(offset, size)) {
        state = (state & ChunkState::CHUNK_CACHE_ENTRY) | ChunkState::CHUNK_LOCAL;
        unit->m_chunkStates[index] = state;
    }

    if (!(state & ChunkState::CHUNK_CACHE_ENTRY) || !(result = unit->m_cache->Read(buffer, offset, size))) {
        switch (state & ChunkState::CHUNK_STATE_MASK) {
        case ChunkState::CHUNK_NONE:
            if (unit->m_http) {
                result = unit->m_http->Read(buffer, offset, size);
            }

            break;

        case ChunkState::CHUNK_DOWNLOADING:
            result = false;
            break;

        case ChunkState::CHUNK_CACHED:
            if (unit->m_cache) {
                result = unit->m_cache->Read(buffer, offset, size);
            }

            if (!result && unit->m_http) {
                result = unit->m_http->Read(buffer, offset, size);
            }

            break;

        case ChunkState::CHUNK_LOCAL:
            result = unit->m_local->Read(buffer, offset, size);
            break;

        default:
            if (unit->m_local && unit->m_local->IsAvailable(offset, size) && unit->m_local->Read(buffer, offset, size)) {
                unit->m_chunkStates[index] = (unit->m_chunkStates[index] & ChunkState::CHUNK_CACHE_ENTRY) | ChunkState::CHUNK_LOCAL;
                result = true;
            } else {
                result = false;
            }

            break;
        }
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return result;
}

// OFFSET: %0x0073B850
bool Blizzard::Storage::Write(StorageUnit* unit, BufferHandle handle, int64_t offset, int32_t size, int16_t tag) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    Blizzard::Streaming::AddStreamed(size);

    uint32_t index = static_cast<uint32_t>(offset / unit->m_chunkSize);
    int32_t remaining = size;
    int32_t position = 0;
    int32_t runStart = 0;
    int32_t runLength = 0;

    if (size > 0) {
        do {
            int32_t state = unit->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK;
            int32_t length = s_Chunk_Size >= remaining ? remaining : s_Chunk_Size;

            if (state == ChunkState::CHUNK_LOCAL || state == ChunkState::CHUNK_CACHED) {
                if (runLength) {
                    QueueWriteCommand(unit, tag, offset + runStart, runLength, g_StorageSystem.AllocateSubChunks(handle, runStart, runLength));
                }

                runLength = 0;
                runStart = length + position;
            } else {
                runLength += length;

                if (tag) {
                    unit->AddCacheEntry(tag, offset + position, length, g_StorageSystem.AllocateSubChunks(handle, position, 1));
                }
            }

            position += length;
            index++;
            remaining -= length;
        } while (remaining > 0);

        if (runLength) {
            QueueWriteCommand(unit, tag, offset + runStart, runLength, g_StorageSystem.AllocateSubChunks(handle, runStart, runLength));
        }
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return true;
}

// OFFSET: %0x0071CCA0
void Blizzard::Storage::FetchError(StorageUnit* unit, int64_t offset, int32_t size, ChunkState::State state) {
    int64_t chunkSize = unit->m_chunkSize;
    int32_t first = static_cast<int32_t>(offset / chunkSize);
    int32_t last = size <= 0 ? first : static_cast<int32_t>((offset + size - 1) / chunkSize);

    Blizzard::Lock::MutexEnter(s_storageMutex);

    for (uint32_t i = first; static_cast<int32_t>(i) <= last; i++) {
        unit->m_chunkStates[i] = (state & ChunkState::CHUNK_STATE_MASK) | (unit->m_chunkStates[i] & ChunkState::CHUNK_CACHE_ENTRY);
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);
}

// OFFSET: %0x0071D3F0
bool Blizzard::Storage::SetRangeAs(StorageUnit* unit, int64_t offset, int32_t size, ChunkState::State state) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    int64_t length = size;
    int32_t last = static_cast<int32_t>((offset + size - 1) / unit->m_chunkSize);

    for (uint32_t i = static_cast<uint32_t>(offset / unit->m_chunkSize); static_cast<int32_t>(i) <= last; i++) {
        if (state && (unit->m_chunkStates[i] & ChunkState::CHUNK_STATE_MASK) >= state) {
            continue;
        }

        unit->m_chunkStates[i] ^= (state ^ unit->m_chunkStates[i]) & ChunkState::CHUNK_STATE_MASK;
    }

    if (!state) {
        unit->m_local->Invalidate(offset, length);
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);

    return true;
}

// OFFSET: %0x0071D2A0
Blizzard::Storage::Range* Blizzard::Storage::GetRange(StorageUnit* unit, int64_t offset, int32_t size, int32_t* count, bool lock) {
    int64_t start = offset >= 0 ? offset : 0;

    if (unit->m_logicalFileSize < start) {
        start = unit->m_logicalFileSize;
    }

    int64_t end = start + size;

    if (end < 0) {
        end = 0;
    }

    if (unit->m_logicalFileSize < end) {
        end = unit->m_logicalFileSize;
    }

    int32_t length = static_cast<int32_t>(end) - static_cast<int32_t>(start);

    if (static_cast<int32_t>(end) == static_cast<int32_t>(start)) {
        *count = 0;
        return NULL;
    }

    if (lock) {
        Blizzard::Lock::MutexEnter(s_storageMutex);
    }

    int64_t chunkSize = unit->m_chunkSize;
    Range* ranges = static_cast<Range*>(SMemAlloc(static_cast<uint32_t>(sizeof(Range) * ((start + length - 1) / chunkSize - start / chunkSize + 4)), __FILE__, __LINE__, 0));
    *count = GetRandomRange(unit, start, length, ranges);

    if (lock) {
        Blizzard::Lock::MutexLeave(s_storageMutex);
    }

    return ranges;
}

// INLINED
Blizzard::Storage::CacheSource::CacheSource(StorageUnit* unit) {
    this->m_unit = unit;
}

// OFFSET: %0x00715D00
Blizzard::Storage::CacheSource::~CacheSource() {
}

// OFFSET: %0x00735AB0
bool Blizzard::Storage::CacheSource::Read(void* buffer, int64_t offset, int32_t size) {
    StorageUnit* unit = this->m_unit;
    int64_t chunkSize = unit->m_chunkSize;
    int32_t index = static_cast<int32_t>(offset / chunkSize);
    int64_t start = index * chunkSize;
    int64_t end = static_cast<int32_t>((offset + size - 1) / chunkSize + 1) * chunkSize;
    std::deque<STORAGE_CMD>* entries = unit->m_cacheEntries;

    for (std::deque<STORAGE_CMD>::iterator it = entries->begin(); it != entries->end(); ++it) {
        if (start >= it->m_offset && end <= it->m_offset + it->m_length) {
            uint8_t* source = g_StorageSystem.GetBuffer(it->m_handle) + (offset - it->m_offset);

            if (static_cast<uint8_t*>(buffer) >= source + size || static_cast<uint8_t*>(buffer) + size <= source) {
                memcpy(buffer, source, size);
            } else {
                memmove(buffer, source, size);
            }

            it->m_unit->m_chunkStates[index] &= ~ChunkState::CHUNK_CACHE_ENTRY;
            g_StorageSystem.ReleaseChunks(it->m_handle);
            Release(it->m_unit);
            entries->erase(it);

            return true;
        }
    }

    std::deque<STORAGE_CMD>* queue = s_CmdQueue_Storage;
    int32_t count = static_cast<int32_t>(queue->size());

    for (int32_t i = 0; i < count; i++) {
        STORAGE_CMD& cmd = (*s_CmdQueue_Storage)[i];

        if (this->m_unit == cmd.m_unit && start >= cmd.m_offset && end <= cmd.m_offset + cmd.m_length) {
            uint8_t* source = g_StorageSystem.GetBuffer(cmd.m_handle) + (offset - cmd.m_offset);

            if (static_cast<uint8_t*>(buffer) >= source + size || static_cast<uint8_t*>(buffer) + size <= source) {
                memcpy(buffer, source, size);
            } else {
                memmove(buffer, source, size);
            }

            return true;
        }
    }

    return false;
}

// OFFSET: %0x00729140
Blizzard::Storage::HttpSource::HttpSource(std::string path, int64_t size) {
    this->m_path = path;
    this->m_size = size;
}

// OFFSET: %0x00726EF0
Blizzard::Storage::HttpSource::~HttpSource() {
}
