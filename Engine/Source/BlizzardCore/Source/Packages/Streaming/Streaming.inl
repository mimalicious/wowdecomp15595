#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_INL

// INLINED
inline Blizzard::Streaming::CompletionCallback::CompletionCallback() {
    this->m_success = true;
    this->m_refCount.m_value = 1;
}

// INLINED
inline Blizzard::Streaming::CompletionCallback::~CompletionCallback() {
}

// OFFSET: %0x00715FB0
inline void Blizzard::Streaming::CompletionCallback::Release() {
    if (!this->m_refCount.Decrement()) {
        this->Complete();
    }
}

// INLINED
inline Blizzard::Storage::StorageUnit* Blizzard::Streaming::AcquireStorageUnit(Blizzard::File::StreamRecord* file) {
    if (file && file->m_streaming && file->m_streaming->m_unit) {
        file->m_streaming->m_unit->m_refCount.Increment();
        return file->m_streaming->m_unit;
    }

    return NULL;
}

// INLINED
inline void Blizzard::Streaming::AddStreamed(int32_t size) {
    s_progress.m_streamed += size;

    uint32_t now = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    if (now - s_progress.m_samples[s_progress.m_index].m_time > 100) {
        s_progress.Sample(now);
        s_progress.FireCallback();
    }
}

#endif
