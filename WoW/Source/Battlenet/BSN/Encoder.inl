// INLINED
inline int32_t BSN::Encoder::PutBits(uint32_t bits, uint32_t value) {
    uint32_t remaining = bits;

    while (true) {
        uint32_t position = this->m_position;

        if (position >= this->m_size) {
            this->Flush();
            position = this->m_position;

            if (position >= this->m_size) {
                return BSN::CODEC_OVERFLOW;
            }
        }

        uint32_t shift = position & 7;
        uint32_t count = 8 - shift;

        if (count >= remaining) {
            count = remaining;
        }

        remaining -= count;

        uint8_t mask = static_cast<uint8_t>((1 << count) - 1);
        uint8_t* byte = &this->m_data[position >> 3];
        *byte = (*byte & ~(mask << shift)) | ((mask & (value >> remaining)) << shift);
        this->m_position += count;

        if (!remaining) {
            return BSN::CODEC_VALID;
        }
    }
}

// INLINED
inline BSN::SizeEncoder::SizeEncoder() : m_size(0) {
}
