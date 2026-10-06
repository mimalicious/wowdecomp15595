#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_INL

// INLINED
inline int32_t Blizzard::Storage::GetChunkSize() {
    return s_Chunk_Size;
}

// INLINED
inline uint8_t* Blizzard::Storage::StorageSystem::GetBuffer(BufferHandle handle) {
    return this->m_buffer + s_Chunk_Size * handle.m.start;
}

// OFFSET: %0x00735A70
inline void Blizzard::Storage::Release(StorageUnit* unit) {
    if (unit && !unit->m_refCount.Decrement() && Close(unit)) {
        delete unit;
    }
}

// INLINED
inline Blizzard::Storage::Range::Range() {
}

// INLINED
inline void Blizzard::Storage::StorageSystem::EnableWriteback(bool enable) {
    Blizzard::Lock::MutexEnter(s_storageMutex);

    if (enable) {
        if (!--this->m_writebackLock) {
            Blizzard::Lock::EventSet(s_cmdEvent);
        }
    } else {
        if (!++this->m_writebackLock) {
            Blizzard::Lock::EventSet(s_cmdEvent);
        }
    }

    Blizzard::Lock::MutexLeave(s_storageMutex);
}

#endif
