#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_DIGEST_H

#include <cstdint>
#include <cstring>

namespace Blizzard {
namespace Util {

template <int N>
struct Digest {
    /* 0000 */ uint8_t m_data[N];

    Digest& FromHexString(const char* hex);
    bool operator<(const Digest& other) const;
    bool operator==(const Digest& other) const;
    bool operator!=(const Digest& other) const;
    bool ToHexString(char* buffer, int32_t size) const;
};

extern const uint8_t s_hexDigits[256];
extern const char s_hexChars[16];

}
}

// OFFSET: %0x00710F80<16>
template <int N>
Blizzard::Util::Digest<N>& Blizzard::Util::Digest<N>::FromHexString(const char* hex) {
    if (hex && strlen(hex) == 2 * N) {
        for (int32_t i = 0; i < 2 * N; i += 2) {
            this->m_data[i >> 1] = (s_hexDigits[hex[i]] << 4) | s_hexDigits[hex[i + 1]];
        }
    }

    return *this;
}

// OFFSET: #0x00429540
template <int N>
bool Blizzard::Util::Digest<N>::operator<(const Digest& other) const {
    for (int32_t i = 0; i < N; i++) {
        if (this->m_data[i] < other.m_data[i]) {
            return true;
        }

        if (this->m_data[i] > other.m_data[i]) {
            return false;
        }
    }

    return false;
}

// OFFSET: #0x00668750
template <int N>
bool Blizzard::Util::Digest<N>::operator==(const Digest& other) const {
    return memcmp(this->m_data, other.m_data, N) == 0;
}

// PROVEN: 42860e12 IDENTICAL/DIFFERENT
// OFFSET: 0x007A33A0/0x1404A7920
template <int N>
bool Blizzard::Util::Digest<N>::operator!=(const Digest& other) const {
    return memcmp(this->m_data, other.m_data, N) != 0;
}

// OFFSET: %0x00710FE0<16>
template <int N>
bool Blizzard::Util::Digest<N>::ToHexString(char* buffer, int32_t size) const {
    if (size <= 2 * N) {
        return false;
    }

    for (int32_t i = 0; i < N; i++) {
        buffer[2 * i] = s_hexChars[this->m_data[i] >> 4];
        buffer[2 * i + 1] = s_hexChars[this->m_data[i] & 0xF];
    }

    buffer[2 * N] = '\0';
    return true;
}

#endif
