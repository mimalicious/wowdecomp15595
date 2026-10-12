#ifndef ENGINE_SOURCE_GX_CGXPOOL_INL
#define ENGINE_SOURCE_GX_CGXPOOL_INL

// INLINED
inline CGxPool::CGxPool(EGxPoolTarget target, EGxPoolUsage usage, int32_t size, EGxPoolHintBits hint, const char* name) : m_target(target), m_usage(usage), m_size(size), m_apiSpecific(NULL), unk_001C(0), m_hint(hint), m_name(name) {
}

#endif
