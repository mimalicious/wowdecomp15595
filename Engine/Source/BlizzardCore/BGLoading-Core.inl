#ifndef ENGINE_SOURCE_BLIZZARDCORE_BGLOADING_CORE_INL
#define ENGINE_SOURCE_BLIZZARDCORE_BGLOADING_CORE_INL

// INLINED
inline void BGLoading::BGLoadManager::AdvanceSample() {
    if (++this->m_sample == 30) {
        this->m_samplesFull = true;
        this->m_sample = 0;
    }

    if (this->m_samplesFull) {
        this->m_sampleTotal -= this->m_sampleBytes[this->m_sample];
        this->m_firstSample = (this->m_sample + 1) % 30;
    }
}

// OFFSET: 0x007A2870
inline void BGLoading::StartBGLoadThread() {
    if (Blizzard::Streaming::IsStreamingMode()) {
        BGLoadManager::Get();
        BGLoadManager::StartThread();
    }
}

#endif
