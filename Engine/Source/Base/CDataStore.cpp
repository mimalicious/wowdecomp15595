#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x00401050
CDataStore::CDataStore() {
    this->m_data = NULL;
    this->m_base = 0;
    this->m_alloc = 0;
    this->m_size = 0;
    this->m_read = -1;
}

// OFFSET: %0x00490120
CDataStore::CDataStore(uint8_t* data, uint32_t size) {
    this->m_data = data;
    this->m_base = 0;
    this->m_alloc = -1;
    this->m_size = size;
    this->m_read = 0;
}

// OFFSET: %0x00401070
int32_t CDataStore::FetchWrite(uint32_t pos, uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (pos >= this->m_base && pos + bytes <= this->m_base + this->m_alloc) {
        return 1;
    }

    return this->InternalFetchWrite(pos, bytes, this->m_data, this->m_base, this->m_alloc, fileName, lineNumber) ? 1 : 0;
}

// OFFSET: %0x004010D0
int32_t CDataStore::IsRead() {
    return this->m_read == this->m_size;
}

// OFFSET: %0x004010E0
void CDataStore::Reset() {
    if (this->m_alloc == -1) {
        this->m_data = NULL;
        this->m_alloc = 0;
    }

    this->FetchWrite(0, 0, NULL, 0);
    this->m_read = -1;
    this->m_size = 0;
}

// OFFSET: %0x00701870
void CDataStore::Finalize() {
    this->m_read = 0;
}

// OFFSET: [%0x004033B0, %0x004033D0]
CDataStore::~CDataStore() {
    if (this->m_alloc != -1) {
        this->InternalDestroy(this->m_data, this->m_base, this->m_alloc);
    }
}

// INLINED
void CDataStore::InternalInitialize(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
}

// OFFSET: %0x007D6030
void CDataStore::GetBufferParams(const void** data, uint32_t* size, uint32_t* alloc) {
    if (data) {
        *data = this->m_data;
    }

    if (size) {
        *size = this->m_size;
    }

    if (alloc) {
        *alloc = this->m_alloc;
    }
}

// OFFSET: %0x007D6060
void CDataStore::DetachBuffer(void** data, uint32_t* size, uint32_t* alloc) {
    if (data) {
        *data = this->m_data;
    }

    if (size) {
        *size = this->m_size;
    }

    if (alloc) {
        *alloc = this->m_alloc;
    }

    this->m_data = NULL;
    this->m_alloc = 0;
    this->Reset();
}

// OFFSET: %0x007D60A0
void CDataStore::InternalDestroy(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
    if (alloc && data) {
        SMemFree(data, __FILE__, __LINE__, 0);
    }

    data = NULL;
    base = 0;
    alloc = 0;
}

// OFFSET: %0x007D60F0
int32_t CDataStore::InternalFetchWrite(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc, const char* fileName, int32_t lineNumber) {
    alloc = (pos + bytes + 255) & ~255;

    if (!fileName) {
        fileName = __FILE__;
        lineNumber = __LINE__;
    }

    data = static_cast<uint8_t*>(SMemReAlloc(data, alloc, fileName, lineNumber, 0));

    return 1;
}

// OFFSET: %0x007D6140
CDataStore& CDataStore::Set(uint32_t pos, uint8_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    this->m_data[pos - this->m_base] = val;

    return *this;
}

// OFFSET: %0x007D6190
CDataStore& CDataStore::Set(uint32_t pos, uint16_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    *reinterpret_cast<uint16_t*>(&this->m_data[pos - this->m_base]) = val;

    return *this;
}

// OFFSET: %0x007D61E0
CDataStore& CDataStore::Set(uint32_t pos, uint32_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    *reinterpret_cast<uint32_t*>(&this->m_data[pos - this->m_base]) = val;

    return *this;
}

// OFFSET: %0x007D6230
CDataStore& CDataStore::Put(uint8_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    this->m_data[this->m_size - this->m_base] = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: %0x007D6290
CDataStore& CDataStore::Put(uint16_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint16_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: %0x007D62F0
CDataStore& CDataStore::Put(uint32_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint32_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: %0x007D6350
CDataStore& CDataStore::Put(uint64_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint64_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: %0x007D63B0
CDataStore& CDataStore::Put(float val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<float*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: %0x007D6410
CDataStore& CDataStore::PutArray(const uint8_t* pval, uint32_t count) {
    if (!pval) {
        if (count) {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        }

        return *this;
    }

    this->FetchWrite(this->m_size, count, NULL, 0);

    while (count) {
        uint32_t bytes = count < this->m_alloc ? count : this->m_alloc;

        if (bytes <= 1) {
            bytes = 1;
        }

        this->FetchWrite(this->m_size, bytes, NULL, 0);
        uint8_t* dest = &this->m_data[this->m_size - this->m_base];

        if (dest != pval) {
            memcpy(dest, pval, bytes);
        }

        pval += bytes;
        this->m_size += bytes;
        count -= bytes;
    }

    return *this;
}

// OFFSET: %0x007D64D0
CDataStore& CDataStore::PutData(const void* data, uint32_t bytes) {
    return this->PutArray(static_cast<const uint8_t*>(data), bytes);
}

// OFFSET: %0x007D64E0
int32_t CDataStore::FetchRead(uint32_t pos, uint32_t bytes) {
    if (pos + bytes > this->m_size) {
        this->m_read = this->m_size + 1;
        return 0;
    }

    if (pos >= this->m_base && pos + bytes <= this->m_base + this->m_alloc) {
        return 1;
    }

    if (this->InternalFetchRead(pos, bytes, this->m_data, this->m_base, this->m_alloc)) {
        return 1;
    }

    this->m_read = this->m_size + 1;
    return 0;
}

// OFFSET: #0x0047B300
CDataStore& CDataStore::PutString(const char* string) {
    if (string) {
        this->PutArray(reinterpret_cast<const uint8_t*>(string), static_cast<uint32_t>(SStrLen(string)) + 1);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }

    return *this;
}

// OFFSET: %0x007D6590
CDataStore& CDataStore::Get(uint8_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = this->m_data[this->m_read - this->m_base];
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: %0x007D65C0
CDataStore& CDataStore::Get(uint16_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint16_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: %0x007D6600
CDataStore& CDataStore::Get(uint32_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint32_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: %0x007D6640
CDataStore& CDataStore::Get(uint64_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint64_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: %0x007D6680
CDataStore& CDataStore::Get(float& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<float*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: %0x007D66C0
CDataStore& CDataStore::GetString(char* string, uint32_t maxChars) {
    if (!string) {
        if (maxChars) {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        }

        return *this;
    }

    if (!maxChars) {
        return *this;
    }

    if (this->m_read <= this->m_size) {
        uint32_t copied = 0;

        if (this->FetchRead(this->m_read, 1)) {
            do {
                uint32_t end = this->m_base + this->m_alloc < this->m_size ? this->m_base + this->m_alloc : this->m_size;
                uint32_t chars = end - this->m_read < maxChars - copied ? end - this->m_read : maxChars - copied;
                uint8_t* src = &this->m_data[this->m_read - this->m_base];
                uint32_t read = 0;

                for (; chars; chars--) {
                    uint8_t c = src[read];
                    string[copied++] = c;
                    read++;

                    if (!c) {
                        break;
                    }
                }

                this->m_read += read;

                if (chars) {
                    break;
                }

                if (copied >= maxChars) {
                    this->m_read = this->m_size + 1;
                    break;
                }
            } while (this->FetchRead(this->m_read, 1));
        }
    }

    if (this->m_read > this->m_size) {
        string[0] = '\0';
    }

    return *this;
}

// OFFSET: %0x007D67A0
CDataStore& CDataStore::GetArray(uint8_t* pval, uint32_t count) {
    if (!pval && count) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return *this;
    }

    if (this->m_read <= this->m_size) {
        while (count) {
            uint32_t bytes = this->m_size - this->m_read < count ? this->m_size - this->m_read : count;
            bytes = bytes < this->m_alloc ? bytes : this->m_alloc;

            if (bytes <= 1) {
                bytes = 1;
            }

            if (!this->FetchRead(this->m_read, bytes)) {
                break;
            }

            uint8_t* src = &this->m_data[this->m_read - this->m_base];

            if (pval != src) {
                memcpy(pval, src, bytes);
            }

            pval += bytes;
            this->m_read += bytes;
            count -= bytes;
        }
    }

    return *this;
}

// OFFSET: %0x007D6830
CDataStore& CDataStore::GetArray(uint32_t* pval, uint32_t count) {
    if (!pval && count) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return *this;
    }

    if (this->m_read <= this->m_size) {
        uint32_t remaining = count * sizeof(uint32_t);

        while (remaining) {
            uint32_t bytes = this->m_size - this->m_read < remaining ? this->m_size - this->m_read : remaining;
            bytes = bytes < this->m_alloc ? bytes : this->m_alloc;

            if (bytes <= sizeof(uint32_t)) {
                bytes = sizeof(uint32_t);
            }

            bytes &= ~(sizeof(uint32_t) - 1);

            if (!this->FetchRead(this->m_read, bytes)) {
                break;
            }

            memcpy(pval, &this->m_data[this->m_read - this->m_base], bytes / sizeof(uint32_t) * sizeof(uint32_t));
            pval += bytes / sizeof(uint32_t);
            this->m_read += bytes;
            remaining -= bytes;
        }
    }

    return *this;
}

// OFFSET: %0x007D68D0
CDataStore& CDataStore::GetData(void* data, uint32_t bytes) {
    return this->GetArray(static_cast<uint8_t*>(data), bytes);
}

// OFFSET: %0x007D68E0
CDataStore& CDataStore::GetDataInSitu(void*& data, uint32_t bytes) {
    data = NULL;

    if (this->FetchRead(this->m_read, bytes)) {
        data = &this->m_data[this->m_read - this->m_base];
        this->m_read += bytes;
    }

    return *this;
}

// INLINED
int32_t CDataStore::InternalFetchRead(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc) {
    return 0;
}

// INLINED
uint8_t* CDataStore::GetHeaderSpace(uint32_t bytes) {
    return NULL;
}

// OFFSET: %0x004CC2E0
void* CDataStore::Alloc(uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (!bytes) {
        return NULL;
    }

    if (fileName) {
        return SMemAlloc(bytes, fileName, lineNumber, 0);
    }

    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: %0x004CC320
void* CDataStore::ReAlloc(void* ptr, uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (fileName) {
        return SMemReAlloc(ptr, bytes, fileName, lineNumber, 0);
    }

    return SMemReAlloc(ptr, bytes, __FILE__, __LINE__, 0);
}

// OFFSET: %0x00401130
CDataStore& CDataStore::GetInt8(uint32_t& val) {
    uint8_t value = 0;
    this->Get(value);
    val = value;
    return *this;
}

// OFFSET: [%0x00560930, #0x004D3870]
uint8_t CDataStore::GetByte() {
    uint8_t val = 0;
    this->Get(val);

    return val;
}

// OFFSET: %0x00423080
CDataStore& CDataStore::Get(int32_t& val) {
    uint32_t value = 0;
    this->Get(value);
    val = value;

    return *this;
}
