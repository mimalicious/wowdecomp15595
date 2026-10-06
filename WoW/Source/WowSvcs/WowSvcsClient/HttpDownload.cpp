#include "HttpDownload.h"
#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// INLINED
DownloadFileCallback::DownloadFileCallback() {
    this->m_stop = false;
}

// PROVEN: b4813b58 IDENTICAL/NONE
// OFFSET: 0x00B3CF20
void DownloadFileCallback::Completed(Blizzard::URL::DownloadCallback::Status status) {
    if (this->m_state) {
        this->m_state->m_done = true;
    }

    delete this;
}

// PROVEN: 7905a82a IDENTICAL/NONE
// OFFSET: 0x00B3CF40
void DownloadFileCallback::GetContentLength(int64_t length) {
    if (this->m_state) {
        this->m_state->m_size = length;
    }
}

// PROVEN: d8570886 IDENTICAL/NONE
// OFFSET: 0x00B3CF60/0x140909EB0
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

// PROVEN: 59d0958c IDENTICAL/NONE
// OFFSET: 0x00B3CFA0/0x140909EF0
uint8_t* HttpDownload::GetBytes() {
    return this->m_state->m_data;
}

// PROVEN: ba25c1b3 IDENTICAL/NONE
// OFFSET: 0x00B3CFB0/0x140909F00
int64_t HttpDownload::Size() {
    if (this->m_state->m_done) {
        this->m_state->m_size = this->m_state->m_received;
    }

    if (this->m_state->m_size < this->m_state->m_received) {
        this->m_state->m_size = 2 * this->m_state->m_received;
    }

    return this->m_state->m_size;
}

// PROVEN: 96cdb03d IDENTICAL/NONE
// OFFSET: 0x00B3D000/0x140909F40
int64_t HttpDownload::Progress() {
    return this->m_state->m_received;
}

// PROVEN: 49878bdd IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool DownloadFileCallback::WantData() {
    return true;
}

// PROVEN: 7d316299 IDENTICAL/NONE
// OFFSET: 0x00B3D010/0x140909F50
bool DownloadFileCallback::Stop() {
    return this->m_stop;
}

// PROVEN: 67004f4a IDENTICAL/NONE
// OFFSET: 0x00B06D20
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

// PROVEN: 6ad6c62a IDENTICAL/NONE
// OFFSET: 0x00B3D110/0x14090A0B0
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
