#include <algorithm>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/ScopeGuard.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// OFFSET: 0x007C6100
Blizzard::Storage::StorageUnit::StorageUnit() {
    this->m_valid = true;
    this->m_flushed = false;
    this->m_logicalFileSize = 0;
    this->m_chunkSize = s_Chunk_Size;
    this->m_refCount.m_value = 0;
    this->m_local = NULL;
    this->m_cache = NULL;
    this->m_http = NULL;
    this->m_cacheEntries = new std::deque<STORAGE_CMD>();
}

// OFFSET: 0x007C7DB0/0x1404CEA40
Blizzard::Storage::StorageUnit::~StorageUnit() {
    while (this->m_cacheEntries && this->m_cacheEntries->size()) {
        g_StorageSystem.ReleaseChunks(this->m_cacheEntries->front().m_handle);
        this->m_cacheEntries->pop_front();
    }

    delete this->m_cacheEntries;
    delete this->m_local;
    delete this->m_cache;
    delete this->m_http;
}

// OFFSET: 0x007ABBA0/0x1404A1AF0
void Blizzard::Storage::StorageUnit::SetLocal(PartialFile* local, int64_t size) {
    delete this->m_local;
    this->m_logicalFileSize = size;
    this->m_local = local;
}

// OFFSET: 0x007ABBD0/0x1404A1B50
void Blizzard::Storage::StorageUnit::SetCacheSource() {
    delete this->m_cache;
    this->m_cache = new CacheSource();
}

// OFFSET: 0x007C7F60
void Blizzard::Storage::StorageUnit::SetHttpSource(const std::string& path, int64_t size) {
    delete this->m_http;
    this->m_http = new HttpSource(path, size);
}

// OFFSET: 0x007ABC10
bool Blizzard::Storage::StorageUnit::IsAvailable(int64_t offset, int64_t size) {
    if (this->m_local) {
        return this->m_local->IsAvailable(offset, size);
    }

    return false;
}

// OFFSET: 0x007ABC40/0x1404A1BB0
bool Blizzard::Storage::StorageUnit::FindMissingRange(int64_t offset, int64_t size, ChunkRange* range) {
    range->m_complete = false;

    if (!this->m_local) {
        return false;
    }

    BackingStore* sources[2];
    sources[0] = this->m_local;
    uint32_t count = 1;

    if (this->m_cache) {
        sources[1] = this->m_cache;
        count = 2;
    }

    for (uint32_t i = 0; i < count; i++) {
        if (sources[i]->FindMissing(offset, size, range) && i) {
            i = 0;
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        sources[i]->GetMissingLength(offset, size, range);
    }

    this->m_local->AlignRange(range);

    if (range->m_offset + range->m_length < offset + size) {
        range->m_complete = false;
        return false;
    }

    range->m_complete = true;

    return true;
}

// OFFSET: 0x007B39C0
void Blizzard::Storage::ReadSegment::Set(int64_t offset, int64_t length, const ChunkRange* missing) {
    this->m_length = length;
    this->m_source = 0;
    this->m_offset = offset;

    int64_t start = missing->m_offset;

    if (offset < start) {
        this->m_source = 1;
        int64_t available = missing->m_offset - offset;
        this->m_length = std::min(this->m_length, available);
        return;
    }

    int64_t remaining = missing->m_length - offset + start;
    this->m_length = std::min(remaining, this->m_length);
    int64_t chunkSize = this->m_chunkSize;

    if (!(offset % chunkSize) && this->m_length >= chunkSize) {
        this->m_source = 2;
        this->m_length -= this->m_length % chunkSize;
        return;
    }

    this->m_source = 3;
    int64_t next = chunkSize + offset - (chunkSize + offset) % chunkSize - offset;
    this->m_length = std::min(next, this->m_length);
}

// OFFSET: 0x007D5060/0x1404D5A00
bool Blizzard::Storage::StorageUnit::Read(int64_t offset, int64_t size, void* buffer) {
    if (!buffer) {
        return false;
    }

    if (offset >= this->m_logicalFileSize) {
        return false;
    }

    if (offset + size > this->m_logicalFileSize) {
        size = this->m_logicalFileSize - offset;
    }

    if (!this->m_local) {
        return this->m_http && this->m_http->Read(buffer, offset, size);
    }

    int64_t remaining = size;
    int64_t position = offset;
    int64_t bufferOffset = 0;
    ChunkRange missing;
    missing.m_offset = -1;
    missing.m_length = 0;
    missing.m_chunkCount = 0;
    missing.m_complete = false;
    ReadSegment segment;
    segment.m_chunkSize = this->m_chunkSize;
    bool result = true;

    while (remaining) {
        if (missing.m_offset + missing.m_length <= position) {
            missing.m_offset = position;
            missing.m_length = remaining;
            this->FindMissingRange(position, remaining, &missing);
        }

        segment.Set(position, remaining, &missing);

        switch (segment.m_source) {
            case 1:
                if (!this->m_local) {
                    return false;
                }

                result = this->m_local->Read(static_cast<uint8_t*>(buffer) + bufferOffset, segment.m_offset, segment.m_length);
                break;

            case 3:
                result = this->ReadRemoteUnaligned(static_cast<uint8_t*>(buffer) + bufferOffset, segment.m_offset, static_cast<int32_t>(segment.m_length));
                break;

            case 2:
                result = this->ReadRemote(static_cast<uint8_t*>(buffer) + bufferOffset, segment.m_offset, static_cast<int32_t>(segment.m_length));
                break;

            default:
                return false;
        }

        if (!result) {
            return result;
        }

        position += segment.m_length;
        bufferOffset += segment.m_length;
        remaining -= segment.m_length;
    }

    return result;
}

// OFFSET: 0x007D4F30
bool Blizzard::Storage::StorageUnit::ReadRemote(void* buffer, int64_t offset, int32_t size) {
    if (!this->m_http) {
        return false;
    }

    int64_t length = size;

    if (!this->m_http->Read(buffer, offset, length)) {
        return false;
    }

    BufferHandle handle = g_StorageSystem.AllocateChunks((this->m_chunkSize + size - 1) / this->m_chunkSize);

    if (!handle.all) {
        this->m_local->Write(buffer, offset, size);
        return true;
    }

    Blizzard::Util::ScopeGuard guard = Blizzard::Util::MakeObjGuard(g_StorageSystem, &StorageSystem::ReleaseChunks, handle);
    Blizzard::String::MemCopy(g_StorageSystem.GetBuffer(handle), buffer, size);
    this->AddCacheEntry(NULL, offset, length, handle);

    return true;
}

// OFFSET: 0x007D4DE0
bool Blizzard::Storage::StorageUnit::ReadRemoteUnaligned(void* buffer, int64_t offset, int32_t size) {
    if (!this->m_http) {
        return false;
    }

    ChunkRange range;
    range.m_offset = offset;
    range.m_length = size;
    range.m_chunkCount = 0;
    range.m_complete = false;

    if (this->m_local) {
        this->m_local->AlignRange(&range);
    }

    BufferHandle handle = g_StorageSystem.AllocateChunksUpTo(1);

    if (!handle.m.length) {
        return false;
    }

    Blizzard::Util::ScopeGuard guard = Blizzard::Util::MakeObjGuard(g_StorageSystem, &StorageSystem::ReleaseChunks, handle);
    uint8_t* data = g_StorageSystem.GetBuffer(handle);
    bool result = this->m_http->Read(data, range.m_offset, static_cast<int32_t>(range.m_length));

    if (result) {
        Blizzard::String::MemCopy(buffer, data + (offset - range.m_offset), size);
        this->QueueWrite(NULL, handle, range.m_offset, static_cast<int32_t>(range.m_length));
    }

    return result;
}

// OFFSET: 0x007C3850
bool Blizzard::Storage::StorageUnit::ReadToCache(ChunkRange* range, BufferHandle* handle) {
    if (!this->m_http) {
        return false;
    }

    handle->all = 0;
    bool result = false;
    *handle = g_StorageSystem.AllocateChunksUpTo(range->m_chunkCount);

    if (!handle->all) {
        return result;
    }

    uint32_t count = handle->m.length;

    if (count < static_cast<uint32_t>(range->m_chunkCount)) {
        range->m_length = count * this->m_chunkSize;
        range->m_complete = false;
    }

    if (!handle->all) {
        return result;
    }

    Blizzard::Util::ScopeGuard guard = Blizzard::Util::MakeObjGuard(g_StorageSystem, &StorageSystem::ReleaseChunks, *handle);
    uint8_t* data = g_StorageSystem.GetBuffer(*handle);

    if (this->m_http) {
        result = this->m_http->Read(data, range->m_offset, range->m_length);

        if (result) {
            guard.Dismiss();
        }
    } else {
        result = false;
    }

    return result;
}

// OFFSET: 0x007D4CD0/0x1404D54F0
int32_t Blizzard::Storage::StorageUnit::FetchRequest(Request* request, ChunkRange* range) {
    if (request->m_offset + request->m_size > this->m_logicalFileSize) {
        return 2;
    }

    if (!this->m_local) {
        return 2;
    }

    this->FindMissingRange(request->m_offset, request->m_size, range);

    if (range->m_length) {
        BufferHandle handle;

        if (!this->ReadToCache(range, &handle)) {
            return 2;
        }

        Blizzard::Util::ScopeGuard guard = Blizzard::Util::MakeObjGuard(g_StorageSystem, &StorageSystem::ReleaseChunks, handle);

        if (!this->QueueWrite(request, handle, range->m_offset, range->m_length)) {
            return 2;
        }
    }

    return range->m_complete == false;
}

// OFFSET: 0x007D4B60
bool Blizzard::Storage::StorageUnit::AddCacheEntry(Request* request, int64_t offset, int64_t size, BufferHandle handle) {
    if (request) {
        request->m_refCount.Increment();
        request->m_pendingWrites.Increment();
    }

    STORAGE_CMD cmd;
    cmd.m_request = request;
    cmd.m_handle = g_StorageSystem.AllocateSubChunks(handle, 0, static_cast<int32_t>(size));
    cmd.m_offset = offset;
    cmd.m_length = static_cast<int32_t>(size);

    Blizzard::Lock::ScopedMutex lock(s_storageMutex);
    this->m_cacheEntries->push_back(cmd);
    Blizzard::Lock::EventSet(s_cmdEvent);

    return true;
}

// OFFSET: 0x007D4C20
bool Blizzard::Storage::StorageUnit::QueueWrite(Request* request, BufferHandle handle, int64_t offset, int64_t size) {
    Blizzard::Lock::MutexEnter(s_storageMutex);
    Blizzard::Streaming::AddStreamed(size);
    bool result = this->AddCacheEntry(request, offset, size, handle);
    Blizzard::Lock::MutexLeave(s_storageMutex);

    return result;
}

// OFFSET: 0x007DC0A0
bool Blizzard::Storage::StorageUnit::FlushCacheEntries(int64_t maxBytes) {
    Blizzard::Lock::ScopedMutex lock(s_storageMutex);
    int64_t written = 0;

    while (this->m_cacheEntries->size()) {
        if (maxBytes < written) {
            break;
        }

        STORAGE_CMD& cmd = this->m_cacheEntries->front();
        bool result;

        if (this->m_local) {
            result = this->m_local->Write(g_StorageSystem.GetBuffer(cmd.m_handle), cmd.m_offset, cmd.m_length);
        } else {
            result = false;
        }

        written += cmd.m_length;

        if (cmd.m_request) {
            cmd.m_request->OnWriteComplete(!result);
        }

        if (cmd.m_handle.all) {
            g_StorageSystem.ReleaseChunks(cmd.m_handle);
        }

        this->m_cacheEntries->pop_front();
    }

    return this->m_cacheEntries->size() == 0;
}

// OFFSET: 0x007D9560
Blizzard::Storage::StorageUnitRef::~StorageUnitRef() {
    Release(this->m_unit);
}

// OFFSET: 0x007C7ED0
Blizzard::Storage::HttpSource::HttpSource(std::string path, int64_t size) : BackingStore(0) {
    this->m_path = path;
    this->m_size = size;
}

// OFFSET: 0x007C61C0
Blizzard::Storage::HttpSource::~HttpSource() {
}

// OFFSET: 0x007C3810/0x140317F90
bool Blizzard::Storage::HttpSource::FindMissing(int64_t offset, int64_t size, ChunkRange* range) {
    return false;
}

// OFFSET: 0x007C3810/0x140317F90
bool Blizzard::Storage::HttpSource::GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) {
    return false;
}

// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::Storage::HttpSource::AlignRange(ChunkRange* range) {
}
