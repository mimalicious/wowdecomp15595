#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_TEMPBUFFER_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_TEMPBUFFER_H

#include <cstdint>

namespace Blizzard {
namespace Memory {

template <class T, uint32_t N>
class TempBuffer {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;
    /* 0008 */ T m_buffer[N];

    TempBuffer();
    TempBuffer(uint32_t count);
    ~TempBuffer();
    void Resize(uint32_t count);
};

}
}

// OFFSET: %0x007063A0<char, 4096>
template <class T, uint32_t N>
Blizzard::Memory::TempBuffer<T, N>::TempBuffer() {
    this->m_data = this->m_buffer;
    this->m_count = N;
}

// OFFSET: #0x0042EBB0<uint8_t, 4096>
template <class T, uint32_t N>
Blizzard::Memory::TempBuffer<T, N>::TempBuffer(uint32_t count) {
    this->m_count = count;
    this->m_data = count > N ? new T[count] : this->m_buffer;
}

// OFFSET: %0x00709480
template <class T, uint32_t N>
Blizzard::Memory::TempBuffer<T, N>::~TempBuffer() {
    if (this->m_data != this->m_buffer) {
        delete[] this->m_data;
    }
}

// OFFSET: [%0x00717BB0, #0x0042EAF0, %0x007657F0]
template <class T, uint32_t N>
void Blizzard::Memory::TempBuffer<T, N>::Resize(uint32_t count) {
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
