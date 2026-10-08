#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"

// OFFSET: 0x007C3D00
Blizzard::Storage::Request::Request(StorageUnit* unit, int64_t offset, int64_t size) {
    this->m_offset = offset;
    this->m_unit = NULL;
    this->m_size = size;
    Blizzard::Lock::EventCreate(this->m_event);
    this->m_state = STATE_NONE;
    this->m_cancelled = false;
    this->m_callback = NULL;
    this->m_chunk = NULL;
    this->m_refCount.m_value = 0;
    this->m_pendingWrites.m_value = 0;

    if (unit) {
        unit->m_refCount.Increment();
    }

    this->m_unit = unit;
    this->CheckLocal();
}

// OFFSET: 0x007D52D0
Blizzard::Storage::Request::~Request() {
    if (this->m_callback) {
        this->m_callback->m_success = this->m_state == STATE_LOCAL;
        this->m_callback->Release();
    }

    Blizzard::Storage::Release(this->m_unit);
    Blizzard::Lock::EventDestroy(this->m_event);
}

// OFFSET: 0x007C6390
Blizzard::Storage::Request* Blizzard::Storage::Request::Create(StorageUnit* unit, int64_t offset, int64_t size) {
    Request* request = new Request(unit, offset, size);

    if (request->m_state == STATE_PENDING) {
        request->m_refCount.Increment();
        request->m_pendingWrites.Increment();
    } else {
        request->m_refCount.Increment();
    }

    return request;
}

// OFFSET: 0x007AC2E0
void Blizzard::Storage::Request::CheckLocal() {
    if (this->m_unit) {
        if (this->m_unit->m_local && this->m_unit->m_local->IsAvailable(this->m_offset, this->m_size)) {
            this->m_state = STATE_LOCAL;
        } else {
            this->m_state = STATE_PENDING;
        }
    } else {
        this->m_state = STATE_NO_UNIT;
    }
}

// PROVEN: 648d692b IDENTICAL/NONE
// OFFSET: 0x007B4640/0x1404AA2A0
void Blizzard::Storage::Request::AddRef() {
    this->m_refCount.Increment();
}

// OFFSET: 0x007B4650
void Blizzard::Storage::Request::Cancel() {
    this->m_cancelled = true;
    this->m_state = STATE_CANCELLED;
    Blizzard::Lock::EventSet(this->m_event);
}

// OFFSET: 0x007DA2E0/0x1404DCA30
void Blizzard::Storage::Request::Release() {
    if (!this->m_refCount.Decrement()) {
        delete this;
    }
}

// OFFSET: 0x007DA310/0x1404DCA70
void Blizzard::Storage::Request::OnWriteComplete(bool failed) {
    if (failed) {
        this->m_state = STATE_FAILED;
    }

    if (!this->m_pendingWrites.Decrement()) {
        if (this->m_unit && this->m_unit->m_local && this->m_unit->m_local->IsAvailable(this->m_offset, this->m_size)) {
            this->m_state = STATE_LOCAL;
        }

        Blizzard::Lock::EventSet(this->m_event);
    }

    if (!this->m_refCount.Decrement()) {
        delete this;
    }
}
