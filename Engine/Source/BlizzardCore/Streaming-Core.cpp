#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

struct SFileStreamingStatus {
    /* 0000 */ uint8_t m_mfilReady : 1;
    /* 0000 */ uint8_t m_dataReady : 1;
    /* 0000 */ uint8_t m_disabled : 1;
    /* 0004 */ uint32_t m_build;
    /* 0008 */ char m_text[2048];
};

// STATIC: 0x00EB4210
static SFileStreamingStatus s_streamingStatus;

// PROVEN: e677872d IDENTICAL/NONE
// OFFSET: 0x007A96B0/0x14049F440
void SFile::SetStreamingStatus(bool mfilReady, bool dataReady, bool disabled, uint32_t build, const char* text) {
    s_streamingStatus.m_mfilReady = mfilReady;
    s_streamingStatus.m_dataReady = dataReady;
    s_streamingStatus.m_disabled = disabled;
    s_streamingStatus.m_build = build;
    Blizzard::String::Copy(s_streamingStatus.m_text, text, sizeof(s_streamingStatus.m_text));
}

// PROVEN: 0cb8d3ea IDENTICAL/NONE
// OFFSET: 0x007A9700
void SFile::GetStreamingStatus(bool* mfilReady, bool* dataReady, bool* disabled, uint32_t* build) {
    if (mfilReady) {
        *mfilReady = s_streamingStatus.m_mfilReady;
    }

    if (dataReady) {
        *dataReady = s_streamingStatus.m_dataReady;
    }

    if (disabled) {
        *disabled = s_streamingStatus.m_disabled;
    }

    if (build) {
        *build = s_streamingStatus.m_build;
    }
}

// PROVEN: c33d1850 IDENTICAL/NONE
// OFFSET: 0x007A9750/0x14049F4E0
bool SFile::IsStreamingDataReady() {
    return s_streamingStatus.m_dataReady;
}

// PROVEN: 1a789ce6 IDENTICAL/NONE
// OFFSET: 0x007A9760/0x14049F4F0
bool SFile::IsStreamingMfilReady() {
    return s_streamingStatus.m_mfilReady;
}

// PROVEN: 9fce349a IDENTICAL/NONE
// OFFSET: 0x007A9770/0x14049F500
bool SFile::IsStreamingDisabled() {
    return s_streamingStatus.m_disabled;
}

// PROVEN: 6a968d38 IDENTICAL/NONE
// OFFSET: 0x007A9780/0x14049F510
const char* SFile::GetStreamingStatusText() {
    return s_streamingStatus.m_text;
}
