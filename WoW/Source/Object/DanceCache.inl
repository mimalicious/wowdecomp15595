#ifndef WOW_SOURCE_OBJECT_DANCECACHE_INL
#define WOW_SOURCE_OBJECT_DANCECACHE_INL

// INLINED
inline DanceCache::DanceCache() {
    this->m_id = 0;
    this->m_guid = WOWGUID();
    this->m_name[0] = '\0';
    this->m_data = NULL;
    this->m_dataCount = 0;
    this->unk_0098 = 0;
}

// INLINED
inline DanceCache::~DanceCache() {
    delete[] this->m_data;
}

#endif
