#include <cstring>
#include "lmemPool.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C1C594
static const uint32_t s_blocksizes[] = { 16, 24, 32, 40, 64, 80, 128, 160, 256 };

// OFFSET: 0x00A2E330/0x1407D71F0
MemChunk::MemChunk(uint32_t blockSize, uint32_t size) {
    this->m_blockSize = blockSize;
    this->m_numBlocks = size / blockSize;
    this->m_size = blockSize * this->m_numBlocks;
    this->m_memblocks = SMemAlloc(this->m_size, __FILE__, __LINE__, 0);

    char* block = static_cast<char*>(this->m_memblocks);
    this->m_free = block;

    while (block < static_cast<char*>(this->m_memblocks) + this->m_size - this->m_blockSize) {
        *reinterpret_cast<void**>(block) = block + this->m_blockSize;
        block += this->m_blockSize;
    }

    *reinterpret_cast<void**>(block) = NULL;
}

// INLINED
MemChunk::~MemChunk() {
    SMemFree(this->m_memblocks, __FILE__, __LINE__, 0);
}

// INLINED
MemPool::MemPool() {
}

// OFFSET: 0x00A2E440
int32_t MemPool::MemFree(void* ptr) {
    for (uint32_t i = 0; i < this->m_chunks.Count(); i++) {
        MemChunk* chunk = this->m_chunks[i];

        if (ptr >= chunk->m_memblocks && ptr < static_cast<char*>(chunk->m_memblocks) + chunk->m_size) {
            *static_cast<void**>(ptr) = chunk->m_free;
            chunk->m_numBlocks++;
            chunk->m_free = ptr;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A2E4B0
MemPool::~MemPool() {
    for (uint32_t i = this->m_chunks.Count(); i; i--) {
        delete this->m_chunks[i - 1];
    }
}

// OFFSET: 0x00A2E520/0x1407D7430
void* luaM_initPool() {
    MemPool** pools = new (__FILE__, __LINE__) MemPool*[sizeof(s_blocksizes) / sizeof(s_blocksizes[0])];

    for (uint32_t i = 0; i < sizeof(s_blocksizes) / sizeof(s_blocksizes[0]); i++) {
        pools[i] = new (__FILE__, __LINE__) MemPool();
        pools[i]->m_blockSize = s_blocksizes[i];
        pools[i]->m_numBlocks = (s_blocksizes[i] * 1024) / s_blocksizes[i];
        pools[i]->m_chunkSize = s_blocksizes[i] * pools[i]->m_numBlocks;
    }

    return pools;
}

// OFFSET: 0x00A2E5B0
void luaM_freePool(void* ud) {
    MemPool** pools = static_cast<MemPool**>(ud);

    if (!pools) {
        return;
    }

    for (uint32_t i = 0; i < sizeof(s_blocksizes) / sizeof(s_blocksizes[0]); i++) {
        delete pools[i];
    }

    SMemFree(pools, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00A2E600/0x1407D7590
void* MemPool::MemAlloc() {
    uint32_t count = this->m_chunks.Count();

    for (uint32_t i = 0; i < count; i++) {
        MemChunk* chunk = this->m_chunks[i];

        if (chunk->m_free) {
            void* block = chunk->m_free;
            chunk->m_numBlocks--;
            chunk->m_free = *static_cast<void**>(block);
            return block;
        }
    }

    this->m_chunks.SetCount(count + 1);
    this->m_chunks[count] = new (__FILE__, __LINE__) MemChunk(this->m_blockSize, this->m_chunkSize);

    MemChunk* chunk = this->m_chunks[count];
    void* block = NULL;

    if (chunk->m_free) {
        block = chunk->m_free;
        chunk->m_numBlocks--;
        chunk->m_free = *static_cast<void**>(block);
    }

    return block;
}

// OFFSET: 0x00A2E6C0/0x1407D76C0
void* luaM_reallocPool(void* ud, void* ptr, size_t osize, size_t nsize) {
    MemPool** pools = static_cast<MemPool**>(ud);
    int32_t oldpool = -1;
    int32_t newpool = -1;

    if (ptr) {
        for (int32_t i = 0; i < static_cast<int32_t>(sizeof(s_blocksizes) / sizeof(s_blocksizes[0])); i++) {
            if (osize <= s_blocksizes[i]) {
                oldpool = i;
                break;
            }
        }
    }

    if (!nsize) {
        if (ptr) {
            if (oldpool >= 0) {
                pools[oldpool]->MemFree(ptr);
                return NULL;
            }

            SMemFree(ptr, __FILE__, __LINE__, 0);
        }

        return NULL;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(s_blocksizes) / sizeof(s_blocksizes[0])); i++) {
        if (nsize <= s_blocksizes[i]) {
            newpool = i;
            break;
        }
    }

    if (oldpool == newpool) {
        if (oldpool < 0) {
            return SMemReAlloc(ptr, nsize, __FILE__, __LINE__, 0);
        }

        return ptr;
    }

    void* newptr = newpool < 0 ? SMemAlloc(nsize, __FILE__, __LINE__, 0) : pools[newpool]->MemAlloc();

    if (ptr) {
        memcpy(newptr, ptr, osize < nsize ? osize : nsize);

        if (oldpool >= 0) {
            pools[oldpool]->MemFree(ptr);
        } else {
            SMemFree(ptr, __FILE__, __LINE__, 0);
        }
    }

    return newptr;
}
