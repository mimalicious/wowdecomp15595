#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_INL

// INLINED
inline Blizzard::Storage::BufferHandle::BufferHandle() {
    this->all = 0;
}

// INLINED
inline void Blizzard::Storage::BitField::Clear(int32_t index) {
    _InterlockedAnd8(reinterpret_cast<volatile char*>(&this->m_bits[index >> 3]), static_cast<char>(~(1 << (index & 7))));
}

// INLINED
inline void Blizzard::Storage::BitField::Set(int32_t index) {
    _InterlockedOr8(reinterpret_cast<volatile char*>(&this->m_bits[index >> 3]), static_cast<char>(1 << (index & 7)));
}

// INLINED
inline int32_t Blizzard::Storage::GetChunkSize() {
    return s_Chunk_Size;
}

// INLINED
inline uint8_t* Blizzard::Storage::StorageSystem::GetBuffer(BufferHandle handle) {
    return this->m_buffer + s_Chunk_Size * handle.m.start;
}

// INLINED
inline Blizzard::Storage::BackingStore::BackingStore(uint8_t type) {
    this->m_type = type;
}

// INLINED
inline Blizzard::Storage::LockFile::LockFile() : m_file(NULL) {
}

// INLINED
inline Blizzard::Storage::CacheSource::CacheSource() : BackingStore(4) {
}

// OFFSET: 0x007D4B20
inline void Blizzard::Storage::Release(StorageUnit* unit) {
    if (unit && !unit->m_refCount.Decrement() && Close(unit)) {
        delete unit;
    }
}

// INLINED
inline Blizzard::Storage::StorageUnitRef::StorageUnitRef(StorageUnit* unit) {
    if (unit) {
        unit->m_refCount.Increment();
    }

    this->m_unit = unit;
}

#endif
