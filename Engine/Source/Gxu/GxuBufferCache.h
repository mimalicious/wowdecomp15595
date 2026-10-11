#ifndef ENGINE_SOURCE_GXU_GXUBUFFERCACHE_H
#define ENGINE_SOURCE_GXU_GXUBUFFERCACHE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxBuf;
struct CGxPool;

class GxuBufferCache {
    public:
    struct BufferEntry {
        /* 0000 */ EGxPoolTarget m_target;
        /* 0004 */ uint32_t m_itemSize;
        /* 0008 */ uint32_t m_itemCount;
        /* 000C */ CGxPool* m_pool;
        /* 0010 */ CGxBuf* m_buf;
        /* 0014 */ int32_t m_refCount;
    };

    /* 0000 */ TSGrowableArray<BufferEntry> m_entries;

    void ReleaseCache();
    bool Exists(uint32_t handle);
    CGxBuf* GetBuffer(uint32_t handle);
    CGxBuf* AcquireBuffer(uint32_t handle);
    void ReleaseBuffer(uint32_t handle);
    uint32_t CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount);
};

#endif
