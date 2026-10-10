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

// OFFSET: 0x0044E2F0
inline WOWGUID CGObject_C::GetGuildGUID() const {
    if (this->m_obj->m_dataType == 1) {
        return this->m_obj->m_data;
    }

    return WOWGUID::Make(0);
}
