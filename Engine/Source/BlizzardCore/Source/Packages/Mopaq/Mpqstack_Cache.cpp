#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Cache.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
bool System_Mopaq::CacheData::CacheBlock::operator<(const CacheBlock& other) const {
    return this->m_offset < other.m_offset;
}

// INLINED
System_Mopaq::CacheData::CacheData() : m_blocks(1) {
}

// OFFSET: 0x007B2E90/0x1404A8490
void System_Mopaq::Cache::AllocateBuffer(CacheData* data) {
    if (data->m_buffer) {
        return;
    }

    uint32_t count = data->m_blocks.m_count;
    data->m_buffer = SMemAlloc(count * data->m_blockSize + data->m_bufferAlign - 1, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY);
    data->m_alignedBuffer = reinterpret_cast<uint8_t*>(~static_cast<uintptr_t>(data->m_bufferAlign - 1) & (reinterpret_cast<uintptr_t>(data->m_buffer) + data->m_bufferAlign - 1));

    for (uint32_t i = 0; i < count; i++) {
        data->m_blocks.m_data[i].m_offset = -1;
        data->m_blocks.m_data[i].m_dirty = false;
        data->m_blocks.m_data[i].m_data = data->m_alignedBuffer + i * data->m_blockSize;
    }
}

// PROVEN: 0da5c4dc DIFFERENT/IDENTICAL
// OFFSET: 0x007B2F20/0x1404A8540
void System_Mopaq::Cache::MoveToBack(CacheData* data, uint32_t index) {
    uint32_t count = data->m_blocks.m_count;

    if (index == count - 1) {
        return;
    }

    CacheData::CacheBlock block = data->m_blocks.m_data[index];
    memmove(&data->m_blocks.m_data[index], &data->m_blocks.m_data[index + 1], (count - index - 1) * sizeof(CacheData::CacheBlock));
    data->m_blocks.m_data[count - 1] = block;
}

// OFFSET: 0x007BAA00/0x1404B22D0
bool System_Mopaq::Cache::CleanCacheEntry(CacheData* data, uint32_t index, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData::CacheBlock* block = &data->m_blocks.m_data[index];

    if (!block->m_dirty) {
        return true;
    }

    int64_t end = block->m_offset + data->m_blockSize;
    uint32_t count = static_cast<uint32_t>(std::min(end, data->m_size) - block->m_offset);
    count = std::min(data->m_blockSize, count);

    if (!data->m_exactSize) {
        memset(block->m_data + count, 0, data->m_blockSize - count);
        count = data->m_blockSize;
    }

    bool result = WriteToNext(system, parms, data->m_blocks.m_data[index].m_data, data->m_blocks.m_data[index].m_offset, count);

    block = &data->m_blocks.m_data[index];
    data->m_writtenSize = std::max(data->m_writtenSize, block->m_offset + static_cast<int32_t>(count));
    block->m_dirty = !result;

    return result;
}

// OFFSET: 0x007BF950/0x1404B8CC0
uint8_t* System_Mopaq::Cache::GetCachedBlock(CacheData* data, int64_t offset, bool dirty, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    AllocateBuffer(data);

    uint32_t count = data->m_blocks.m_count;
    uint32_t index = 0;

    while (index < count && data->m_blocks.m_data[index].m_offset != offset) {
        index++;
    }

    if (index >= count) {
        index = count - 1;

        if (!CleanCacheEntry(data, index, system, parms)) {
            return NULL;
        }

        CacheData::CacheBlock* block = &data->m_blocks.m_data[index];
        block->m_offset = -1;

        if (offset + data->m_blockSize > data->m_writtenSize) {
            if (offset >= data->m_writtenSize) {
                memset(block->m_data, 0, data->m_blockSize);
            } else {
                uint32_t valid = static_cast<uint32_t>(data->m_writtenSize - offset);

                if (!ReadFromNext(system, parms, data->m_blocks.m_data[index].m_data, offset, valid, NULL)) {
                    return NULL;
                }

                memset(data->m_blocks.m_data[index].m_data + valid, 0, data->m_blockSize - valid);
            }
        } else if (!ReadFromNext(system, parms, block->m_data, offset, data->m_blockSize, NULL)) {
            return NULL;
        }

        data->m_blocks.m_data[index].m_offset = offset;
    }

    if (index) {
        CacheData::CacheBlock block = data->m_blocks.m_data[index];
        memmove(&data->m_blocks.m_data[1], &data->m_blocks.m_data[0], index * sizeof(CacheData::CacheBlock));
        data->m_blocks.m_data[0] = block;
    }

    data->m_blocks.m_data[0].m_dirty |= dirty;
    return data->m_blocks.m_data[0].m_data;
}

// OFFSET: 0x007D0690/0x1404CE660
bool System_Mopaq::Cache::CleanCache(CacheData* data, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!data->m_buffer) {
        return true;
    }

    std::sort(data->m_blocks.m_data, data->m_blocks.m_data + data->m_blocks.m_count);

    bool result = true;
    uint32_t count = data->m_blocks.m_count;

    for (uint32_t i = 0; i < count; i++) {
        if (!CleanCacheEntry(data, i, system, parms)) {
            result = false;
        }
    }

    return result;
}

// OFFSET: 0x007BF810/0x1404B8B20
bool System_Mopaq::cache_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::Mpqsystem* next = system->m_next;

    if (!next || !CallNext(system, parms)) {
        return false;
    }

    MpqParams size;
    memset(&size, 0, sizeof(size));
    size.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_getfilesize);
    size.m_handle = parms->m_handle;

    if (!system->m_next || !system->m_next->m_getfilesize(system->m_next, &size)) {
        return false;
    }

    CacheData* data = new CacheData;
    data->m_blockSize = 0x100000;
    data->m_largeTransfers = true;
    data->m_exactSize = true;
    data->m_bufferAlign = 0x10000;
    data->m_skipCleanOnWrite = true;
    data->m_alignedBuffer = NULL;
    data->m_buffer = NULL;
    data->m_writtenSize = size.m_offset;
    data->m_size = size.m_offset;
    data->m_base = 0;
    data->m_blocks.Resize(4);

    for (uint32_t i = 0; i < 4; i++) {
        data->m_blocks.m_data[i].m_offset = -1;
        data->m_blocks.m_data[i].m_dirty = false;
        data->m_blocks.m_data[i].m_data = NULL;
    }

    parms->m_handle->m_cache = data;
    return true;
}

// OFFSET: 0x007D45C0/0x1404D4CF0
bool System_Mopaq::cache_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData* data = parms->m_handle->m_cache;
    bool result = Cache::CleanCache(data, system, parms);

    SMemFree(data->m_buffer, __FILE__, __LINE__, 0);
    delete data;

    Blizzard::Mopaq::Mpqsystem* next = system->m_next;
    return result && next && CallNext(system, parms);
}

// OFFSET: 0x007E78D0/0x1404F1A90
bool System_Mopaq::cache_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData* data = parms->m_handle->m_cache;
    int64_t offset = parms->m_offset;
    uint8_t* dst = static_cast<uint8_t*>(parms->m_buffer);
    uint32_t count = parms->m_count;

    if (offset < data->m_base) {
        Blizzard::File::SetLastError(10);
        Blizzard::File::AddToLastErrorStack(10, "IOAlignUnit::Read - Range Error", 1);
        return false;
    }

    if (!count) {
        parms->m_count = 0;
        return true;
    }

    if (offset >= data->m_size) {
        if (offset != data->m_size) {
            Blizzard::File::SetLastError(10);
            Blizzard::File::AddToLastErrorStack(10, "IOAlignUnit::Read - Range Error", 1);
            return false;
        }

        parms->m_count = 0;
        return true;
    }

    uint32_t available = static_cast<uint32_t>(data->m_size) - static_cast<uint32_t>(offset);
    count = std::min(available, count);

    int64_t position = data->m_base + (offset - data->m_base) / data->m_blockSize * data->m_blockSize;
    uint32_t remaining = count;

    if (position != offset) {
        uint32_t head = static_cast<uint32_t>(offset) - static_cast<uint32_t>(position);
        uint32_t amount = std::min(data->m_blockSize - head, remaining);
        uint8_t* block = Cache::GetCachedBlock(data, position, false, system, parms);

        if (!block) {
            return false;
        }

        memcpy(dst, block + head, amount);
        dst += amount;
        remaining = count - amount;
        position += data->m_blockSize;
    }

    if (!(data->m_exactSize && data->m_largeTransfers && position + remaining == data->m_size) && remaining >= data->m_blockSize) {
        uint32_t whole = data->m_blockSize * (remaining / data->m_blockSize);
        uint32_t chunk = data->m_largeTransfers ? whole : data->m_blockSize;
        int32_t blockCount = data->m_blocks.m_count;

        for (int32_t i = 0; i < blockCount; i++) {
            int64_t blockOffset = data->m_blocks.m_data[i].m_offset;

            if (blockOffset >= position && blockOffset < position + whole && !Cache::CleanCacheEntry(data, i, system, parms)) {
                return false;
            }
        }

        do {
            MpqParams read;
            memset(&read, 0, sizeof(read));
            read.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
            read.m_handle = parms->m_handle;
            read.m_buffer = dst;
            read.m_offset = position;
            read.m_count = chunk;

            if (!system->m_next || !system->m_next->m_read(system->m_next, &read)) {
                return false;
            }

            dst += chunk;
            remaining -= chunk;
            position += chunk;
        } while (remaining >= chunk);
    }

    if (data->m_exactSize && position + remaining == data->m_size) {
        if (!ReadFromNext(system, parms, dst, position, remaining, NULL)) {
            return false;
        }
    } else if (remaining) {
        uint8_t* block = Cache::GetCachedBlock(data, position, false, system, parms);

        if (!block) {
            return false;
        }

        memcpy(dst, block, remaining);
    }

    parms->m_count = count;
    return true;
}

// OFFSET: 0x007D4650/0x1404D4DA0
bool System_Mopaq::cache_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData* data = parms->m_handle->m_cache;
    int64_t offset = parms->m_offset;

    if (offset < data->m_base) {
        return false;
    }

    const uint8_t* src = static_cast<const uint8_t*>(parms->m_buffer);
    int64_t position = data->m_base + (offset - data->m_base) / data->m_blockSize * data->m_blockSize;
    uint32_t remaining = parms->m_count;

    if (position != offset) {
        uint32_t head = static_cast<uint32_t>(offset - position);
        uint32_t amount = std::min(data->m_blockSize - head, remaining);
        uint8_t* block = Cache::GetCachedBlock(data, position, true, system, parms);

        if (!block) {
            return false;
        }

        memcpy(block + head, src, amount);
        src += amount;
        remaining -= amount;
        position += data->m_blockSize;
        data->m_size = std::max(data->m_size, offset + amount);
    }

    if (remaining >= data->m_blockSize) {
        uint32_t whole = data->m_blockSize * (remaining / data->m_blockSize);

        for (int32_t i = data->m_blocks.m_count - 1; i >= 0; i--) {
            CacheData::CacheBlock* block = &data->m_blocks.m_data[i];

            if (block->m_offset >= position && block->m_offset < position + whole) {
                block->m_offset = -1;
                data->m_blocks.m_data[i].m_dirty = false;
                Cache::MoveToBack(data, i);
            }
        }

        if (!data->m_skipCleanOnWrite && !Cache::CleanCache(data, system, parms)) {
            return false;
        }

        uint32_t chunk = data->m_largeTransfers ? whole : data->m_blockSize;

        do {
            MpqParams write;
            memset(&write, 0, sizeof(write));
            write.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_write);
            write.m_handle = parms->m_handle;
            write.m_buffer = const_cast<uint8_t*>(src);
            write.m_offset = position;
            write.m_count = chunk;

            if (!system->m_next || !system->m_next->m_write(system->m_next, &write)) {
                return false;
            }

            src += chunk;
            position += chunk;
            remaining -= chunk;
        } while (remaining >= chunk);

        data->m_size = std::max(data->m_size, position);
        data->m_writtenSize = std::max(data->m_writtenSize, position);
    }

    if (!remaining) {
        return true;
    }

    uint8_t* block = Cache::GetCachedBlock(data, position, true, system, parms);

    if (!block) {
        return false;
    }

    memcpy(block, src, remaining);
    data->m_size = std::max(data->m_size, position + remaining);
    return true;
}

// OFFSET: 0x007D4980/0x1404D50A0
bool System_Mopaq::cache_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    bool result = Cache::CleanCache(parms->m_handle->m_cache, system, parms);

    Blizzard::Mopaq::Mpqsystem* next = system->m_next;
    return next && CallNext(system, parms) && result;
}

// OFFSET: 0x007AB5B0
bool System_Mopaq::cache_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = parms->m_handle->m_cache->m_size;
    return true;
}

// OFFSET: 0x007BA8C0/0x1404B2190
bool System_Mopaq::cache_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData* data = parms->m_handle->m_cache;
    int64_t size = parms->m_offset;

    if (data->m_buffer && size < data->m_size) {
        int64_t blockStart = data->m_base + (size - data->m_base - 1) / data->m_blockSize * data->m_blockSize;
        uint32_t tail = static_cast<uint32_t>(size) - static_cast<uint32_t>(blockStart);

        for (int32_t i = data->m_blocks.m_count - 1; i >= 0; i--) {
            CacheData::CacheBlock* block = &data->m_blocks.m_data[i];

            if (block->m_offset >= blockStart) {
                if (block->m_offset <= blockStart) {
                    memset(block->m_data + tail, 0, data->m_blockSize - tail);
                } else {
                    block->m_offset = -1;
                    data->m_blocks.m_data[i].m_dirty = false;
                    Cache::MoveToBack(data, i);
                }
            }
        }
    }

    data->m_size = size;
    parms->m_offset = size;

    Blizzard::Mopaq::Mpqsystem* next = system->m_next;

    if (!next) {
        return false;
    }

    return CallNext(system, parms);
}

// OFFSET: 0x007B2E30
bool System_Mopaq::cache_abandon(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    CacheData* data = parms->m_handle->m_cache;
    SMemFree(data->m_buffer, __FILE__, __LINE__, 0);
    data->m_buffer = NULL;

    Blizzard::Mopaq::Mpqsystem* next = system->m_next;
    return next && CallNext(system, parms);
}
