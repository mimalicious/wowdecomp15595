#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/BitArray.h"

// OFFSET: %0x007D8060
CBitArray::CBitArray() {
    this->m_array = NULL;
    this->m_size = 0;
    this->unk_0008 = false;
}

// OFFSET: %0x007D8070
int32_t CBitArray::Get(uint32_t bitIdx) const {
    return (this->m_array[bitIdx >> 3] & (1 << (bitIdx & 7))) != 0;
}

// OFFSET: %0x007D80A0
void CBitArray::Set(uint32_t bitIdx, bool value) {
    char* byte = &this->m_array[bitIdx >> 3];
    char mask = 1 << (bitIdx & 7);

    if (value) {
        *byte |= mask;
    } else {
        *byte &= ~mask;
    }
}

// OFFSET: #0x0095DA80
CBitArray::~CBitArray() {
    if (this->m_array && this->unk_0008) {
        delete[] this->m_array;
    }
}

// OFFSET: %0x007D80F0
void CBitArray::SetRawData(char* data, uint32_t size, bool owned) {
    if (this->m_array && this->unk_0008) {
        delete[] this->m_array;
    }

    this->m_array = data;
    this->m_size = size;
    this->unk_0008 = owned;
}
