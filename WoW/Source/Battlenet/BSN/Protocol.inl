// INLINED
inline BSN::FourCC::FourCC() {
    this->m_value = 0;
}

// INLINED
inline void BSN::FourCC::ToString(char* dest, uint32_t size) const {
    uint8_t c = static_cast<uint8_t>(this->m_value >> 24);

    if (c && size-- > 0) {
        *dest++ = c;
    }

    c = static_cast<uint8_t>(this->m_value >> 16);

    if (c && size-- > 0) {
        *dest++ = c;
    }

    c = static_cast<uint8_t>(this->m_value >> 8);

    if (c && size-- > 0) {
        *dest++ = c;
    }

    c = static_cast<uint8_t>(this->m_value);

    if (c && size-- > 0) {
        *dest++ = c;
    }

    if (size) {
        *dest = '\0';
    }
}
