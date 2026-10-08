#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_BUFFER_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_UTIL_BUFFER_H

#include <cstdint>

namespace Blizzard {
namespace Util {

template <uint32_t N, class T>
class Buffer {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;
    /* 0008 */ T m_buffer[N];

    Buffer();
    Buffer(uint32_t count);
    ~Buffer();
    void Resize(uint32_t count);
};

}
}

// OFFSET: 0x007A37C0/0x1404AC9E0<4096, char>
template <uint32_t N, class T>
Blizzard::Util::Buffer<N, T>::Buffer() {
    this->m_data = this->m_buffer;
    this->m_count = N;
}

// OFFSET: 0x007B67A0<4096, uint8_t>
template <uint32_t N, class T>
Blizzard::Util::Buffer<N, T>::Buffer(uint32_t count) {
    this->m_count = count;
    this->m_data = count > N ? new T[count] : this->m_buffer;
}

// PROVEN: 2949f53e IDENTICAL/IDENTICAL
// OFFSET: 0x007B6800/0x1404AC7A0
template <uint32_t N, class T>
Blizzard::Util::Buffer<N, T>::~Buffer() {
    if (this->m_data != this->m_buffer) {
        delete[] this->m_data;
    }
}

// OFFSET: [0x007B6410<4096, char>, 0x007B65E0<1024, char>, 0x007B66A0/0x1404ACB30<1, System_Mopaq::CacheData::CacheBlock>, 0x007B64A0<16384, uint8_t>, 0x007FEB50<4194304, uint8_t>]
template <uint32_t N, class T>
void Blizzard::Util::Buffer<N, T>::Resize(uint32_t count) {
    if (count > N) {
        if (count != this->m_count) {
            T* data = new T[count];
            uint32_t copy = this->m_count >= count ? count : this->m_count;

            for (uint32_t i = 0; i < copy; i++) {
                data[i] = this->m_data[i];
            }

            if (this->m_data != this->m_buffer) {
                delete[] this->m_data;
            }

            this->m_data = data;
        }
    } else if (this->m_data != this->m_buffer) {
        uint32_t copy = this->m_count >= count ? count : this->m_count;

        for (uint32_t i = 0; i < copy; i++) {
            this->m_buffer[i] = this->m_data[i];
        }

        delete[] this->m_data;
        this->m_data = this->m_buffer;
    }

    this->m_count = count;
}

#endif
