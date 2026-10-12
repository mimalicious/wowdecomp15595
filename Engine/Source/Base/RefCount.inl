#ifndef ENGINE_SOURCE_BASE_REFCOUNT_INL
#define ENGINE_SOURCE_BASE_REFCOUNT_INL

// INLINED
inline TRefCnt::TRefCnt() {
    this->m_refcnt = 0;
}

#endif
