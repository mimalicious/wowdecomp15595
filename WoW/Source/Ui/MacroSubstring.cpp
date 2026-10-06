#include "MacroSubstring.h"
#include <cstring>

// PROVEN: f1798afb IDENTICAL/NONE
// OFFSET: 0x00A0D220/0x1407AEC60
void MacroSubstring::SetText(const char* start, const char* end, char delimiter, const char** next) {
    this->m_start = start;

    while (this->m_start < end && *this->m_start == ' ') {
        this->m_start++;
    }

    if (delimiter) {
        this->m_end = this->m_start;

        while (this->m_end < end && *this->m_end != delimiter) {
            this->m_end++;
        }

        if (next) {
            *next = this->m_end;
        }
    } else {
        this->m_end = end;
    }

    while (this->m_end > this->m_start && *(this->m_end - 1) == ' ') {
        this->m_end--;
    }
}

// PROVEN: ecd0a603 IDENTICAL/NONE
// OFFSET: 0x00A0D2F0
bool MacroSubstring::operator==(const char* str) {
    const char* cursor = this->m_start;

    while (*str) {
        if (cursor == this->m_end || *str != *cursor) {
            return false;
        }

        str++;
        cursor++;
    }

    return cursor == this->m_end;
}

// OFFSET: #0x005EEAC0
void MacroSubstring::GetText(char* buffer, uint32_t size) {
    uint32_t length = size - 1;

    if (length >= static_cast<uint32_t>(this->m_end - this->m_start)) {
        length = static_cast<uint32_t>(this->m_end - this->m_start);
    }

    memcpy(buffer, this->m_start, length);
    buffer[length] = '\0';
}

// PROVEN: f39b0927 IDENTICAL/NONE
// OFFSET: 0x00A0D2C0/0x1407AED30
int32_t MacroSubstring::ToNumber() {
    int32_t value = 0;

    for (const char* c = this->m_start; c < this->m_end; c++) {
        if (*c < '0' || *c > '9') {
            break;
        }

        value = *c + 10 * value - '0';
    }

    return value;
}
