#ifndef ENGINE_SOURCE_SOUNDENGINE_SE3_INL
#define ENGINE_SOURCE_SOUNDENGINE_SE3_INL

// INLINED
inline SE3SoundBus::SE3SoundBus() {
    this->m_parent = SE3BUS_NONE;
    this->m_volume = 1.0f;
    this->m_muted = 0;
    this->m_enabled = 1;
    this->m_channelCount = 0;
    this->m_maxChannels = 10000;
}

// OFFSET: 0x004DB460
inline float SE3SoundBus::GetVolume() {
    if (this->m_muted || !this->m_enabled) {
        return 0.0f;
    }

    return this->m_volume;
}

// OFFSET: 0x004DB430
inline void SE3SoundBus::SetVolume(float volume) {
    if (volume < 0.0f) {
        volume = 0.0f;
    } else if (volume > 1.0f) {
        volume = 1.0f;
    }

    this->m_volume = volume;
}

// INLINED
inline SE3SoundChannel::SE3SoundChannel() {
    this->m_pFMODChannel = NULL;
    this->m_baseFrequency = 0.0f;
    this->m_fadeState = SE3FADE_NONE;
    this->m_fadeVolume = 1.0f;
    this->m_waveDataID = 0;
}

// INLINED
inline SE3SoundKitProperties::SE3SoundKitProperties() {
    this->m_nIndexOverride = -1;
    this->ResetToDefaults();
}

// INLINED
inline SE3SoundChunk::SE3SoundChunk() {
    this->m_handle = 0;
    this->m_minX = 0.0f;
    this->m_maxX = 0.0f;
    this->m_minY = 0.0f;
    this->m_maxY = 0.0f;
}

// INLINED
inline SuppressorSound::SuppressorSound() {
    this->m_scalar[0] = 1.0f;
    this->m_scalar[1] = 1.0f;
    this->m_scalar[2] = 1.0f;
    this->m_scalar[3] = 1.0f;
    this->m_duckTo[0] = 1.0f;
    this->m_duckTo[1] = 1.0f;
    this->m_duckTo[2] = 1.0f;
    this->m_duckTo[3] = 1.0f;
    this->m_uUID = 0;
    this->m_distance = 0.0f;
    this->m_timeToUnduck = 1000;
    this->m_innerRadius = 0.0f;
    this->m_outerRadius = 0.0f;
}

// OFFSET: 0x0062FA30
inline SE3EngineInitParams::SE3EngineInitParams() {
    this->m_sfxEnabled = 1;
    this->m_musicEnabled = 1;
    this->m_ambienceEnabled = 1;
    this->m_masterEnabled = 1;
    this->m_numChannels = 4;
    this->m_dspBufferSize = 4;
    this->m_sfxVolume = 1.0f;
    this->m_musicVolume = 1.0f;
    this->m_ambienceVolume = 1.0f;
    this->m_masterVolume = 1.0f;
    this->m_voiceChatVolume = 1.0f;
    this->m_hardware = 0;
    this->m_reverb = 0;
    this->m_softwareHRTF = 0;
    this->unk_0030 = 0;
    this->m_mixMode = SE3MIXMODE2_PCM16;
    this->m_sampleRate = 44100;
    this->m_millisecondsPerDay = 0;
    this->m_outputDriverIndex = 0;
    this->m_resampleMethod = 0;
    this->m_forceStereo = 0;
    this->m_maxSoftwareChannels = 128;
    this->m_outputDriverName = "System Default";
    this->m_systemDefaultName = "System Default";
    this->m_fpFilenameTranslateCallback = NULL;
    this->unk_0064 = NULL;
    this->m_fpGetDayProgression = NULL;
}

// INLINED
inline SE3StreamCallbacks::~SE3StreamCallbacks() {
}

// INLINED
inline SE3CaptureCallbacks::~SE3CaptureCallbacks() {
}

#endif
