#include "WoW/Source/Battlenet/BSN/Protocol.h"

const BSN::Utf8Sequence BSN::s_utf8Sequences[9] = {
    { 1, { { 0x01, 0x7F }, { 0x00, 0x00 }, { 0x00, 0x00 }, { 0x00, 0x00 } } },
    { 2, { { 0xC2, 0xDF }, { 0x80, 0xBF }, { 0x00, 0x00 }, { 0x00, 0x00 } } },
    { 3, { { 0xE0, 0xE0 }, { 0xA0, 0xBF }, { 0x80, 0xBF }, { 0x00, 0x00 } } },
    { 3, { { 0xE1, 0xEC }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x00, 0x00 } } },
    { 3, { { 0xED, 0xED }, { 0x80, 0x9F }, { 0x80, 0xBF }, { 0x00, 0x00 } } },
    { 3, { { 0xEE, 0xEF }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x00, 0x00 } } },
    { 4, { { 0xF0, 0xF0 }, { 0x90, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } } },
    { 4, { { 0xF1, 0xF3 }, { 0x80, 0xBF }, { 0x80, 0xBF }, { 0x80, 0xBF } } },
    { 4, { { 0xF4, 0xF4 }, { 0x80, 0x8F }, { 0x80, 0xBF }, { 0x80, 0xBF } } },
};

// INLINED
BSN::FourCC::FourCC(const char* string) {
    this->m_value = 0;

    for (; *string; string++) {
        this->m_value = (this->m_value << 8) | static_cast<uint8_t>(*string);
    }
}

// OFFSET: 0x00664190/0x14030A360
uint32_t BSN::AsciiStringCopy(char* dest, const char* source, uint32_t maxLength) {
    char* out = dest;

    while (maxLength) {
        maxLength--;

        if (!*source) {
            break;
        }

        *out++ = *source++;
    }

    *out = '\0';
    return static_cast<uint32_t>(out - dest);
}

// OFFSET: 0x006641C0/0x14030A3A0
uint32_t BSN::UTF8StringCopy(char* dest, const char* source, uint32_t maxChars, uint32_t& length) {
    char* out = dest;
    uint32_t count = 0;

    for (; maxChars; maxChars--) {
        char c = *source++;
        uint32_t extra;

        if (c < 0) {
            if ((c & 0xE0) == 0xC0) {
                extra = 1;
            } else if ((c & 0xF0) == 0xE0) {
                extra = 2;
            } else if ((c & 0xF8) == 0xF0) {
                extra = 3;
            } else {
                break;
            }
        } else {
            if (!c) {
                break;
            }

            extra = 0;
        }

        char* start = out;
        *out++ = c;

        while (extra) {
            char next = *source;
            extra--;
            source++;

            if ((next & 0xC0) != 0x80) {
                out = start;
                break;
            }

            *out++ = next;
        }

        count++;
    }

    *out = '\0';
    length = static_cast<uint32_t>(out - dest);
    return count;
}

// OFFSET: 0x00665CB0/0x14030C290
void BSN::Cpp::ReadInt(const void* data, uint32_t size, bool isSigned, uint64_t& value) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);

    if (isSigned && bytes[size - 1] >= 0x80) {
        value = static_cast<uint64_t>(-1);
    } else {
        value = 0;
    }

    while (size) {
        size--;
        value = (value << 8) | bytes[size];
    }
}

// OFFSET: 0x00665D10/0x14030C2D0
void BSN::Cpp::WriteInt(uint64_t value, uint32_t size, void* data) {
    uint8_t* bytes = static_cast<uint8_t*>(data);

    for (; size; size--) {
        *bytes++ = static_cast<uint8_t>(value);
        value >>= 8;
    }
}
