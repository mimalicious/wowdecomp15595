// INLINED
inline CHashKeyGUID::CHashKeyGUID() {
    this->m_guid.m_guid = 0;
}

// INLINED
inline CHashKeyGUID::CHashKeyGUID(uint64_t guid) {
    this->m_guid.m_guid = guid;
}

// INLINED
inline bool CHashKeyGUID::operator==(const CHashKeyGUID& key) {
    return this->m_guid == key.m_guid;
}
