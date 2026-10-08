// INLINED
inline BSN::FourCC::FourCC() {
    this->m_value = 0;
}

// INLINED
inline void BSN::FourCC::ToString(char* dest, uint32_t size) const {
    for (int32_t shift = 24; shift >= 0; shift -= 8) {
        uint8_t c = static_cast<uint8_t>(this->m_value >> shift);

        if (c && size--) {
            *dest++ = c;
        }
    }

    if (size) {
        *dest = '\0';
    }
}
