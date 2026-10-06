#include "HttpDownload.h"
#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// INLINED
DownloadFileCallback::DownloadFileCallback() {
    this->m_stop = false;
}

// OFFSET: %0x00A6F1C0
void DownloadFileCallback::Completed(Blizzard::URL::DownloadCallback::Status status) {
    if (this->m_state) {
        this->m_state->m_done = true;
    }

    delete this;
}

// OFFSET: %0x00A6F1E0
void DownloadFileCallback::GetContentLength(int64_t length) {
    if (this->m_state) {
        this->m_state->m_size = length;
    }
}

// OFFSET: %0x00A6F200
bool HttpDownload::Done() {
    return !this->m_requested || this->m_state->m_done;
}

// OFFSET: #0x008A1390
bool HttpDownload::Cancel() {
    if (!this->m_requested || this->m_state->m_done) {
        return true;
    }

    this->m_callback->m_stop = true;
    return false;
}

// OFFSET: %0x00A6F240
uint8_t* HttpDownload::GetBytes() {
    return this->m_state->m_data;
}

// OFFSET: %0x00A6F250
int64_t HttpDownload::Size() {
    if (this->m_state->m_done) {
        this->m_state->m_size = this->m_state->m_received;
    }

    if (this->m_state->m_size < this->m_state->m_received) {
        this->m_state->m_size = 2 * this->m_state->m_received;
    }

    return this->m_state->m_size;
}

// OFFSET: %0x00A6F2A0
int64_t HttpDownload::Progress() {
    return this->m_state->m_received;
}

// OFFSET: %0x004E0850
bool DownloadFileCallback::WantData() {
    return true;
}

// OFFSET: %0x00A6F2B0
bool DownloadFileCallback::Stop() {
    return this->m_stop;
}

// OFFSET: %0x005A4510
bool DownloadFileCallback::ShouldBlock() {
    return false;
}

// OFFSET: #0x008A1440
bool DownloadFileCallback::ReceivedData(const void* data, int32_t size) {
    if (!this->m_state) {
        return true;
    }

    this->m_state->m_receiving = true;

    if (!this->m_state->m_data) {
        this->m_state->m_capacity = static_cast<int32_t>(this->m_state->m_size);

        if (this->m_state->m_capacity < 0) {
            this->m_state->m_capacity = size;
        }

        this->m_state->m_data = static_cast<uint8_t*>(Blizzard::Memory::Allocate(this->m_state->m_capacity));
    }

    if (this->m_state->m_received + size > this->m_state->m_capacity) {
        this->m_state->m_capacity *= 2;

        if (this->m_state->m_capacity < this->m_state->m_received + size) {
            this->m_state->m_capacity = this->m_state->m_received + size;
        }

        uint8_t* grown = static_cast<uint8_t*>(Blizzard::Memory::Allocate(this->m_state->m_capacity));
        Blizzard::String::MemCopy(grown, this->m_state->m_data, this->m_state->m_received);
        Blizzard::Memory::Free(this->m_state->m_data);
        this->m_state->m_data = grown;
    }

    Blizzard::String::MemCopy(this->m_state->m_data + this->m_state->m_received, data, size);
    this->m_state->m_received += size;
    this->m_state->m_receiving = false;

    return true;
}

// OFFSET: %0x00A6F3E0
const char* HttpDownload::TargetName() {
    return this->m_state->m_targetName.c_str();
}

// OFFSET: #0x008A1550
HttpDownload::~HttpDownload() {
    if (!this->m_state->m_done) {
        while (this->m_state->m_receiving) {
            Blizzard::Thread::Sleep(0);
        }

        this->m_callback->m_state = NULL;
    }

    Blizzard::Memory::Free(this->m_state->m_data);
    delete this->m_state;
}

// OFFSET: #0x008A15E0
HttpDownload::HttpDownload(const char* url, const char* targetName) {
    this->m_state = new HttpDownloadState;
    this->m_state->m_receiving = false;
    this->m_state->m_done = false;
    this->m_state->m_size = -1;
    this->m_state->m_data = NULL;
    this->m_state->m_received = 0;

    this->m_callback = new DownloadFileCallback;
    this->m_callback->m_state = this->m_state;
    this->m_state->m_url = url;

    if (targetName) {
        this->m_state->m_targetName = targetName;
    }

    this->m_requested = Blizzard::URL::DownloadURL(url, this->m_callback);
}
