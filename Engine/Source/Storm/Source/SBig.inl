// INLINED
inline BigBuffer::BigBuffer() {
    this->m_offset = 0;
}

// INLINED
inline uint32_t& BigBuffer::operator[](uint32_t index) {
    this->GrowToFit(index);
    return this->m_data[index + this->m_offset];
}

// INLINED
inline void BigBuffer::Clear() {
    this->m_data.SetCount(this->m_offset);
}

// INLINED
inline uint32_t BigBuffer::Count() const {
    return this->m_data.Count() - this->m_offset;
}

// INLINED
inline int32_t BigBuffer::IsUsed(uint32_t index) const {
    return index + this->m_offset < this->m_data.Count();
}

// INLINED
inline void BigBuffer::SetOffset(uint32_t offset) {
    this->m_offset = offset;

    if (offset) {
        this->GrowToFit(0xFFFFFFFF);
    }
}

// INLINED
inline BigBuffer& BigStack::Alloc(uint32_t* count) {
    if (count) {
        ++*count;
    }

    return this->m_buffer[this->m_used++];
}

// INLINED
inline void BigStack::Free(uint32_t count) {
    this->m_used -= count;
}

// INLINED
inline BigBuffer& BigStack::MakeDistinct(BigBuffer& orig, int32_t required) {
    return required ? this->Alloc(NULL) : orig;
}

// INLINED
inline void BigStack::UnmakeDistinct(BigBuffer& orig, BigBuffer& distinct) {
    if (&orig != &distinct) {
        orig = distinct;
        this->Free(1);
    }
}

// INLINED
inline TSGrowableArray<uint8_t>& BigData::Output() const {
    return this->m_output;
}

// INLINED
inline BigBuffer& BigData::Primary() {
    return this->m_primary;
}

// INLINED
inline const BigBuffer& BigData::Primary() const {
    return this->m_primary;
}

// INLINED
inline BigStack& BigData::Stack() const {
    return this->m_stack;
}
