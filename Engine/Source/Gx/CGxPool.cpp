#include <cstdint>
#include "Engine/Source/Gx/CGxPool.h"

// INLINED
CGxPool::CGxPool(EGxPoolTarget target, EGxPoolUsage usage, int32_t size, EGxPoolHintBits hint, const char* name) : m_target(target), m_usage(usage), m_size(size), m_apiSpecific(NULL), unk_001C(0), m_hint(hint), m_name(name) {
}

// OFFSET: %0x007A7640
CGxPool::~CGxPool() {
}

// OFFSET: %0x007AE080
void CGxPool::Invalidate() {
    for (CGxBuf* buf = this->m_bufList.Head(); buf; buf = this->m_bufList.Next(buf)) {
        buf->unk_001C = 0;
    }
}

// OFFSET: %0x007AE0B0
void CGxPool::Discard() {
    for (CGxBuf* buf = this->m_bufList.Head(); buf; buf = this->m_bufList.Next(buf)) {
        buf->unk_001C = 0;
    }

    this->unk_001C = 0;
}

// OFFSET: %0x007AA890
void CGxPool::UnlinkBuf(CGxBuf* buf) {
    this->m_bufList.UnlinkNode(buf);
    buf->m_pool = NULL;
}
