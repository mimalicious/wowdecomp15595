// INLINED
inline fixed16::fixed16() {
}

// INLINED
inline fixed16::fixed16(float value) {
    if (value < 1.0f) {
        if (value > -1.0f) {
            this->m_value = static_cast<int16_t>(value * 32767.0f + 0.5f);
        } else {
            this->m_value = -32767;
        }
    } else {
        this->m_value = 0x7FFF;
    }
}
