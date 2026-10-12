#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00408460
CDataStore::CDataStore() {
    this->m_data = NULL;
    this->m_base = 0;
    this->m_alloc = 0;
    this->m_size = 0;
    this->m_read = -1;
}

// OFFSET: 0x00487990
CDataStore::CDataStore(uint8_t* data, uint32_t size) {
    this->m_data = data;
    this->m_base = 0;
    this->m_alloc = -1;
    this->m_size = size;
    this->m_read = 0;
}

// OFFSET: 0x00487D00
void CDataStore::InitExternal(uint8_t* data, uint32_t size) {
    if (this->m_alloc != -1) {
        this->InternalDestroy(this->m_data, this->m_base, this->m_alloc);
    }

    this->m_alloc = -1;
    this->m_data = data;
    this->m_base = 0;
    this->m_read = 0;
    this->m_size = size;
}

// OFFSET: 0x00408480
int32_t CDataStore::FetchWrite(uint32_t pos, uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (pos >= this->m_base && pos + bytes <= this->m_base + this->m_alloc) {
        return 1;
    }

    return this->InternalFetchWrite(pos, bytes, this->m_data, this->m_base, this->m_alloc, fileName, lineNumber) ? 1 : 0;
}

// PROVEN: 606d939b IDENTICAL/IDENTICAL
// OFFSET: 0x004084E0/0x1405A7550
int32_t CDataStore::IsRead() {
    return this->m_read == this->m_size;
}

// OFFSET: 0x004084F0/0x14000A800
void CDataStore::Reset() {
    if (this->m_alloc == -1) {
        this->m_data = NULL;
        this->m_alloc = 0;
    }

    this->FetchWrite(0, 0, NULL, 0);
    this->m_read = -1;
    this->m_size = 0;
}

// PROVEN: 06a8a2cf IDENTICAL/IDENTICAL
// OFFSET: 0x00408540/0x14000A870
void CDataStore::Finalize() {
    this->m_read = 0;
}

// OFFSET: [0x00408610, 0x00408630]
CDataStore::~CDataStore() {
    if (this->m_alloc != -1) {
        this->InternalDestroy(this->m_data, this->m_base, this->m_alloc);
    }
}

// INLINED
void CDataStore::InternalInitialize(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
}

// PROVEN: 20a3ebdd IDENTICAL/IDENTICAL
// OFFSET: 0x00871680/0x1405A7550
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

// PROVEN: b02da87b IDENTICAL/IDENTICAL
// OFFSET: 0x008716B0/0x1405A7580
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

// PROVEN: 6bae70d8 IDENTICAL/DIFFERENT
// OFFSET: 0x008716F0/0x1405A75C0
void CDataStore::InternalDestroy(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
    if (alloc && data) {
        SMemFree(data, __FILE__, __LINE__, 0);
    }

    data = NULL;
    base = 0;
    alloc = 0;
}

// OFFSET: 0x00871740/0x1405A7620
int32_t CDataStore::InternalFetchWrite(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc, const char* fileName, int32_t lineNumber) {
    alloc = (pos + bytes + 255) & ~255;

    if (!fileName) {
        fileName = __FILE__;
        lineNumber = __LINE__;
    }

    data = static_cast<uint8_t*>(SMemReAlloc(data, alloc, fileName, lineNumber, 0));

    return 1;
}

// PROVEN: 65c61212 IDENTICAL/IDENTICAL
// OFFSET: 0x00871790/0x1405A7680
CDataStore& CDataStore::Set(uint32_t pos, uint8_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    this->m_data[pos - this->m_base] = val;

    return *this;
}

// PROVEN: 20c42974 IDENTICAL/IDENTICAL
// OFFSET: 0x008717E0/0x1405A7710
CDataStore& CDataStore::Set(uint32_t pos, uint16_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    *reinterpret_cast<uint16_t*>(&this->m_data[pos - this->m_base]) = val;

    return *this;
}

// PROVEN: 28f7316a IDENTICAL/IDENTICAL
// OFFSET: 0x00871830/0x1405A77A0
CDataStore& CDataStore::Set(uint32_t pos, uint32_t val) {
    this->FetchWrite(pos, sizeof(val), NULL, 0);
    *reinterpret_cast<uint32_t*>(&this->m_data[pos - this->m_base]) = val;

    return *this;
}

// PROVEN: acf51dc3 IDENTICAL/IDENTICAL
// OFFSET: 0x00871880/0x1405A7830
CDataStore& CDataStore::Put(uint8_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    this->m_data[this->m_size - this->m_base] = val;
    this->m_size += sizeof(val);

    return *this;
}

// PROVEN: 12d078c0 IDENTICAL/IDENTICAL
// OFFSET: 0x008718E0/0x1405A78B0
CDataStore& CDataStore::Put(uint16_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint16_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// PROVEN: 4cc7d2dd IDENTICAL/IDENTICAL
// OFFSET: 0x00871940/0x1405A7930
CDataStore& CDataStore::Put(uint32_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint32_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// PROVEN: 3cb7ac0b IDENTICAL/IDENTICAL
// OFFSET: 0x008719A0/0x1405A79B0
CDataStore& CDataStore::Put(uint64_t val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<uint64_t*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// PROVEN: 7d515d5d IDENTICAL/IDENTICAL
// OFFSET: 0x00871A00/0x1405A7A30
CDataStore& CDataStore::Put(float val) {
    this->FetchWrite(this->m_size, sizeof(val), NULL, 0);
    *reinterpret_cast<float*>(&this->m_data[this->m_size - this->m_base]) = val;
    this->m_size += sizeof(val);

    return *this;
}

// OFFSET: 0x00871A60/0x1405A7AC0
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

// PROVEN: 7bcb16b1 IDENTICAL/IDENTICAL
// OFFSET: 0x00871B20/0x1405A7C00
CDataStore& CDataStore::PutData(const void* data, uint32_t bytes) {
    return this->PutArray(static_cast<const uint8_t*>(data), bytes);
}

// OFFSET: 0x00871B30/0x1405A7C10
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

// OFFSET: 0x00871BA0/0x1405A7C90
CDataStore& CDataStore::PutString(const char* string) {
    if (string) {
        this->PutArray(reinterpret_cast<const uint8_t*>(string), static_cast<uint32_t>(SStrLen(string)) + 1);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }

    return *this;
}

// PROVEN: 05c1f76b IDENTICAL/IDENTICAL
// OFFSET: 0x00871BE0/0x1405A7CE0
CDataStore& CDataStore::Get(uint8_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = this->m_data[this->m_read - this->m_base];
        this->m_read += sizeof(val);
    }

    return *this;
}

// PROVEN: 6f074cef IDENTICAL/IDENTICAL
// OFFSET: 0x00871C10/0x1405A7D30
CDataStore& CDataStore::Get(uint16_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint16_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// PROVEN: 7330006c IDENTICAL/IDENTICAL
// OFFSET: 0x00871C50/0x1405A7DD0
CDataStore& CDataStore::Get(uint32_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint32_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// PROVEN: a4a5d97b IDENTICAL/IDENTICAL
// OFFSET: 0x00871C90/0x1405A7D80
CDataStore& CDataStore::Get(uint64_t& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<uint64_t*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// PROVEN: d5708ecc IDENTICAL/NONE
// OFFSET: 0x00871CD0
CDataStore& CDataStore::Get(float& val) {
    if (this->FetchRead(this->m_read, sizeof(val))) {
        val = *reinterpret_cast<float*>(&this->m_data[this->m_read - this->m_base]);
        this->m_read += sizeof(val);
    }

    return *this;
}

// OFFSET: 0x00871D10/0x1405A7E20
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

// OFFSET: 0x00871DF0/0x1405A7F90
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

// OFFSET: 0x00871E80
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

// PROVEN: 26817864 IDENTICAL/IDENTICAL
// OFFSET: 0x00871F20/0x1405A81F0
CDataStore& CDataStore::GetData(void* data, uint32_t bytes) {
    return this->GetArray(static_cast<uint8_t*>(data), bytes);
}

// PROVEN: 4974bc47 IDENTICAL/IDENTICAL
// OFFSET: 0x00871F30/0x1405A8200
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

// OFFSET: 0x004C5DB0
void* CDataStore::Alloc(uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (!bytes) {
        return NULL;
    }

    if (fileName) {
        return SMemAlloc(bytes, fileName, lineNumber, 0);
    }

    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: 0x004C5DF0
void* CDataStore::ReAlloc(void* ptr, uint32_t bytes, const char* fileName, int32_t lineNumber) {
    if (fileName) {
        return SMemReAlloc(ptr, bytes, fileName, lineNumber, 0);
    }

    return SMemReAlloc(ptr, bytes, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00401070
CDataStore& CDataStore::GetInt8(uint32_t& val) {
    uint8_t value = 0;
    this->Get(value);
    val = value;
    return *this;
}

// INLINED
uint8_t CDataStore::GetByte() {
    uint8_t val = 0;
    this->Get(val);

    return val;
}

// OFFSET: 0x0045B8C0
CDataStore& CDataStore::Get(int32_t& val) {
    uint32_t value = 0;
    this->Get(value);
    val = value;

    return *this;
}
