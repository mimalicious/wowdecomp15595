#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 4574a22c IDENTICAL
// OFFSET: 0x00872DC0
CDynamicString::CDynamicString() {
    this->m_length = 0;
    this->m_capacity = 0;
    this->m_buffer = NULL;
}

// PROVEN: d9b49376 IDENTICAL
// OFFSET: 0x00872DD0
CDynamicString::~CDynamicString() {
    if (this->m_buffer) {
        SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    }
}

// PROVEN: cfd0d636 IDENTICAL
// OFFSET: 0x00872DF0
void CDynamicString::Free() {
    this->m_length = 0;
    this->m_capacity = 0;

    if (this->m_buffer) {
        SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    }

    this->m_buffer = NULL;
}

// PROVEN: 35df8dcd IDENTICAL
// OFFSET: 0x00872E20
CDynamicString& CDynamicString::operator=(const char* str) {
    if (!str) {
        this->m_length = 0;
        this->m_capacity = 0;

        if (this->m_buffer) {
            SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
        }

        this->m_buffer = NULL;
        return *this;
    }

    this->m_length = static_cast<int32_t>(SStrLen(str));
    this->m_capacity = (3 * this->m_length >> 1) + 16;
    this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, this->m_capacity, __FILE__, __LINE__, 0));
    SStrCopy(this->m_buffer, str, STORM_MAX_STR);

    return *this;
}

// OFFSET: %0x007D8220
void CDynamicString::SetCapacity(int32_t capacity) {
    if (capacity < 1) {
        this->m_length = 0;
        this->m_capacity = 0;

        if (this->m_buffer) {
            SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
        }

        this->m_buffer = NULL;
        return;
    }

    if (this->m_buffer) {
        if (capacity <= this->m_length) {
            this->m_length = capacity - 1;
            this->m_buffer[capacity - 1] = '\0';
            this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, capacity, __FILE__, __LINE__, 0));
            this->m_capacity = capacity;
        } else {
            this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, capacity, __FILE__, __LINE__, 0));
            this->m_capacity = capacity;
        }
    } else {
        this->m_buffer = static_cast<char*>(SMemAlloc(capacity, __FILE__, __LINE__, 0));
        this->m_capacity = capacity;
        this->m_length = 0;
        this->m_buffer[0] = '\0';
    }
}

// PROVEN: 28a96b18 IDENTICAL
// OFFSET: 0x00872F70
void CDynamicString::Grow() {
    this->m_capacity = (3 * this->m_length >> 1) + 16;

    if (!this->m_buffer) {
        this->m_buffer = static_cast<char*>(SMemAlloc(this->m_capacity, __FILE__, __LINE__, 0));
        this->m_buffer[0] = '\0';
    } else {
        this->m_buffer = static_cast<char*>(SMemReAlloc(this->m_buffer, this->m_capacity, __FILE__, __LINE__, 0));
    }
}

// OFFSET: #0x0095D870
void CDynamicString::operator+=(const char* str) {
    if (str) {
        int32_t length = this->m_length;
        this->m_length += static_cast<int32_t>(SStrLen(str));

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        SStrCopy(this->m_buffer + length, str, STORM_MAX_STR);
    }
}

// OFFSET: %0x007D8380
void CDynamicString::operator+=(char c) {
    if (c) {
        this->m_length++;

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        this->m_buffer[this->m_length - 1] = c;
        this->m_buffer[this->m_length] = '\0';
    }
}

// OFFSET: #0x0095D8F0
void CDynamicString::operator+=(int32_t value) {
    if (this->m_length + 15 >= this->m_capacity) {
        this->Grow();
    }

    SStrPrintf(this->m_buffer + this->m_length, 15, "%d", value);
    this->m_length = static_cast<int32_t>(SStrLen(this->m_buffer));
}

// OFFSET: %0x007D8410
void CDynamicString::Append(const char* src, int32_t count) {
    if (src && count > 0) {
        this->m_length += count;

        if (this->m_length >= this->m_capacity) {
            this->Grow();
        }

        memcpy(this->m_buffer + this->m_length - count, src, count);
        this->m_buffer[this->m_length] = '\0';
    }
}
