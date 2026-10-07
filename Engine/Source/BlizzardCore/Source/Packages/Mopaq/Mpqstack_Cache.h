#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_CACHE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_CACHE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"

namespace System_Mopaq {

struct CacheData {
    struct CacheBlock {
        /* 0000 */ int64_t m_offset;
        /* 0008 */ bool m_dirty;
        /* 0009 */ uint8_t unk_0009[3];
        /* 000C */ uint8_t* m_data;

        bool operator<(const CacheBlock& other) const;
    };

    /* 0000 */ int64_t m_base;
    /* 0008 */ int64_t m_size;
    /* 0010 */ int64_t m_writtenSize;
    /* 0018 */ uint32_t m_blockSize;
    /* 001C */ bool m_largeTransfers;
    /* 001D */ bool m_exactSize;
    /* 001E */ uint8_t unk_001E[2];
    /* 0020 */ uint32_t m_bufferAlign;
    /* 0024 */ bool m_skipCleanOnWrite;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ Blizzard::Util::Buffer<1, CacheBlock> m_blocks;
    /* 0040 */ uint8_t* m_alignedBuffer;
    /* 0044 */ void* m_buffer;

    CacheData();
};

namespace Cache {

void AllocateBuffer(CacheData* data);
bool CleanCache(CacheData* data, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool CleanCacheEntry(CacheData* data, uint32_t index, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
uint8_t* GetCachedBlock(CacheData* data, int64_t offset, bool dirty, Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
void MoveToBack(CacheData* data, uint32_t index);

}

}

#endif
