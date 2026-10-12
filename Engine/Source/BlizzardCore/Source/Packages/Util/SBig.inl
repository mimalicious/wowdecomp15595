#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_SBIG_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_SBIG_INL

// INLINED
inline uint32_t Blizzard::BigBuffer::operator[](uint32_t index) const {
    return this->IsUsed(index) ? this->m_data[index + this->m_offset] : 0;
}

#endif
