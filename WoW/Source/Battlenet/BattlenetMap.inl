// INLINED
inline Battlenet::String::String() {
    this->m_block = NULL;
    this->m_string = NULL;
}

// INLINED
inline Battlenet::String::String(const String& other) {
    this->m_block = other.m_block;
    this->m_string = other.m_string;

    if (this->m_block) {
        this->m_block->m_refCount++;
    }
}

// INLINED
inline Battlenet::String::~String() {
    this->Release();
}

// INLINED
inline void Battlenet::String::Release() {
    if (this->m_block) {
        if (this->m_block->m_refCount-- == 1) {
            Battlenet::g_allocator->Free(this->m_block);
        }

        this->m_block = NULL;
    }

    this->m_string = NULL;
}

// INLINED
inline Battlenet::String& Battlenet::String::operator=(const String& other) {
    this->Release();
    this->m_block = other.m_block;
    this->m_string = other.m_string;

    if (this->m_block) {
        this->m_block->m_refCount++;
    }

    return *this;
}

// INLINED
inline const char* Battlenet::String::GetString() const {
    return this->m_string ? this->m_string : "";
}

// INLINED
inline Battlenet::MapKey::MapKey() {
    this->m_hash = 0;
}

// INLINED
inline bool Battlenet::MapKey::operator==(const MapKey& other) const {
    return this->m_hash == other.m_hash && !strcmp(this->m_value.m_string, other.m_value.m_string);
}
