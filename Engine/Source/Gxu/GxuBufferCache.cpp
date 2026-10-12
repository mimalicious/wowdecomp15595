#include "Engine/Source/Gxu/GxuBufferCache.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"

// PROVEN: 5232c6f7 DIFFERENT/IDENTICAL
// OFFSET: 0x00622100/0x1402B2EC0
void GxuBufferCache::ReleaseCache() {
    for (uint32_t i = 0; i < this->m_entries.m_count; i++) {
        BufferEntry* entry = &this->m_entries.m_data[i];
        CGxBuf* buf = entry->m_buf;

        if (buf && buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }

        entry->m_buf = NULL;

        if (entry->m_pool) {
            g_theGxDevicePtr->PoolDestroy(entry->m_pool);
        }

        entry->m_pool = NULL;
    }
}

// OFFSET: 0x006221A0/0x1402B2F80
bool GxuBufferCache::Exists(uint32_t handle) {
    if (handle == 0xFFFF || handle >= this->m_entries.m_count) {
        return false;
    }

    BufferEntry* entry = &this->m_entries.m_data[handle];
    CGxBuf* buf = entry->m_buf;

    if (!buf || !entry->m_pool) {
        return false;
    }

    if (!GxBufIsReady(entry->m_buf)) {
        GxBufDestroy(buf);
        entry->m_buf = NULL;
        GxPoolDestroy(entry->m_pool);
        entry->m_pool = NULL;
        return false;
    }

    return true;
}

// OFFSET: 0x00622210
CGxBuf* GxuBufferCache::GetBuffer(uint32_t handle) {
    if (handle == 0xFFFF || handle >= this->m_entries.m_count) {
        return NULL;
    }

    return this->m_entries.m_data[handle].m_buf;
}

// OFFSET: 0x00622240/0x1402B3050
CGxBuf* GxuBufferCache::AcquireBuffer(uint32_t handle) {
    this->m_entries.m_data[handle].m_refCount++;
    return this->m_entries.m_data[handle].m_buf;
}

// PROVEN: 70b40631 DIFFERENT/IDENTICAL
// OFFSET: 0x00622260/0x1402B3070
void GxuBufferCache::ReleaseBuffer(uint32_t handle) {
    this->m_entries.m_data[handle].m_refCount--;
}

// OFFSET: 0x00622400/0x1402B32D0
uint32_t GxuBufferCache::CreateBuffer(uint32_t handle, EGxPoolTarget target, uint32_t itemSize, uint32_t itemCount) {
    if (handle == 0xFFFF) {
        BufferEntry entry;
        entry.m_target = target;
        entry.m_itemSize = itemSize;
        entry.m_itemCount = itemCount;
        entry.m_pool = g_theGxDevicePtr->PoolCreate(target, GxPoolUsage_Static, itemCount * itemSize, GxPoolHintBit_Unk0, NULL);
        entry.m_buf = g_theGxDevicePtr->BufCreate(entry.m_pool, itemSize, itemCount, 0);
        entry.m_refCount = 0;
        this->m_entries.New(entry);
        return this->m_entries.m_count - 1;
    }

    BufferEntry* entry = &this->m_entries.m_data[handle];

    if (!entry->m_buf || !entry->m_pool) {
        entry->m_pool = g_theGxDevicePtr->PoolCreate(target, GxPoolUsage_Static, itemCount * itemSize, GxPoolHintBit_Unk0, NULL);
        entry->m_buf = g_theGxDevicePtr->BufCreate(entry->m_pool, itemSize, itemCount, 0);
    }

    return handle;
}
