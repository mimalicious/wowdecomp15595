#ifndef ENGINE_SOURCE_GX_CGXBUF_INL
#define ENGINE_SOURCE_GX_CGXBUF_INL

#include "Engine/Source/Gx/CGxPool.h"

// INLINED
inline CGxBuf::CGxBuf(CGxPool* pool, uint32_t itemSize, uint32_t itemCount, uint32_t index) : m_pool(pool), m_itemSize(itemSize), m_itemCount(itemCount), m_size(itemSize * itemCount), m_index(index), unk_001C(0), unk_001D(1), unk_001E(0), unk_001F(0) {
}

#endif
