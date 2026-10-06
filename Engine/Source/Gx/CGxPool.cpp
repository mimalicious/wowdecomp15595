#include <cstdint>
#include "Engine/Source/Gx/CGxPool.h"

// INLINED
CGxPool::CGxPool(EGxPoolTarget target, EGxPoolUsage usage, int32_t size, EGxPoolHintBits hint, const char* name) : m_target(target), m_usage(usage), m_size(size), m_apiSpecific(NULL), unk_001C(0), m_hint(hint), m_name(name) {
}

// OFFSET: 0x008415A0/0x14056D8E0
CGxPool::~CGxPool() {
}

// PROVEN: 45b12417 IDENTICAL/DIFFERENT
// OFFSET: 0x00844C40/0x140571990
void CGxPool::Invalidate() {
    for (CGxBuf* buf = this->m_bufList.Head(), * nextBuf; !this->m_bufList.IsEnd(buf); buf = nextBuf) {
        nextBuf = this->m_bufList.RawNext(buf);
        buf->unk_001C = 0;
    }
}

// PROVEN: 5caf4158 IDENTICAL/NONE
// OFFSET: 0x00844C70
void CGxPool::Discard() {
    for (CGxBuf* buf = this->m_bufList.Head(), * nextBuf; !this->m_bufList.IsEnd(buf); buf = nextBuf) {
        nextBuf = this->m_bufList.RawNext(buf);
        buf->unk_001C = 0;
    }

    this->unk_001C = 0;
}

// OFFSET: 0x00844BE0/0x140571930
void CGxPool::UnlinkBuf(CGxBuf* buf) {
    this->m_bufList.UnlinkNode(buf);
    buf->m_pool = NULL;
}
