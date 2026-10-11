#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/URL/URL.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

struct SFileStreamingStatus {
    /* 0000 */ uint8_t m_mfilReady : 1;
    /* 0000 */ uint8_t m_dataReady : 1;
    /* 0000 */ uint8_t m_disabled : 1;
    /* 0004 */ uint32_t m_build;
    /* 0008 */ char m_text[2048];
};

// STATIC: 0x00D05900
const char* System_SFile2::s_downloadStatusNames[12] = { "noError", "genericError", "timedOut", "fileNotFound", "unmodified", "dataCallbackError", "forbiddenError", "nameResolveError", "cannotConnectError", "stopped", "badrequest", "outOfDiskSpace" };

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
// OFFSET: 0x007A9700/0x14049F490
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

// OFFSET: [0x007A95C0, 0x007A97F0]
System_SFile2::BufferCallback::~BufferCallback() {
    Blizzard::Memory::Free(this->m_postData, 0, ".\\Streaming-Core.cpp", 90);
    Blizzard::Lock::EventDestroy(this->m_event);
}

// PROVEN: 37933cfc IDENTICAL/NONE
// OFFSET: 0x007A9630
void System_SFile2::BufferCallback::WaitForCompletion() {
    Blizzard::Lock::EventWaitForever(this->m_event);
}

// PROVEN: 639bf0de IDENTICAL/NONE
// OFFSET: 0x007A9640/0x14049F3C0
bool System_SFile2::BufferCallback::ReceivedData(const void* data, int32_t size) {
    uint32_t length = Blizzard::String::Length(this->m_buffer);

    if (length + size + 1 < this->m_bufferSize) {
        Blizzard::String::MemCopy(this->m_buffer + length, data, size);
        this->m_buffer[length + size] = '\0';
    }

    return true;
}

// PROVEN: 74ad38df IDENTICAL/NONE
// OFFSET: 0x007A9690
void System_SFile2::BufferCallback::Completed(Blizzard::URL::DownloadCallback::Status status) {
    this->m_status = status;
    Blizzard::Lock::EventSet(this->m_event);
}

// PROVEN: 668e1246 IDENTICAL/IDENTICAL
// OFFSET: 0x00B41A60/0x1404C1350
int32_t System_SFile2::BufferCallback::GetTimeout() {
    return this->m_timeout;
}

// PROVEN: 0267456b IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool System_SFile2::BufferCallback::ShouldBlock() {
    return true;
}

// PROVEN: f12eadb5 IDENTICAL/NONE
// OFFSET: 0x007B0A90
const char* System_SFile2::BufferCallback::GetPostData() {
    return this->m_postData;
}

// OFFSET: 0x007A9790
System_SFile2::BufferCallback::BufferCallback() {
    this->m_postData = NULL;
    this->m_status = Blizzard::URL::DownloadCallback::DOWNLOAD_SUCCESS;
    this->m_bufferSize = 0;
    this->m_buffer = NULL;
    this->m_timeout = 3000;
    Blizzard::Lock::EventCreate(this->m_event);
}

// OFFSET: 0x007A9820
bool SFile::DownloadURLToBuffer(const char* url, const char* postData, uint32_t timeout, char* buffer, uint32_t size, const char** status) {
    System_SFile2::BufferCallback callback;

    if (postData) {
        callback.m_postData = Blizzard::String::Duplicate(postData);
    }

    callback.m_bufferSize = size;
    callback.m_buffer = buffer;
    *buffer = '\0';
    callback.m_timeout = timeout;

    if (!Blizzard::URL::DownloadURL(url, &callback)) {
        *status = System_SFile2::s_downloadStatusNames[callback.m_status];
        return false;
    }

    *status = System_SFile2::s_downloadStatusNames[callback.m_status];

    return callback.m_status == Blizzard::URL::DownloadCallback::DOWNLOAD_SUCCESS;
}
