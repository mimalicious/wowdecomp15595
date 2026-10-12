#include "Engine/Source/ComSat/ComSatBitPacker.h"
#include <cstring>

// OFFSET: 0x00A38EF0/0x1407E5430
ComSat::BitPacker::BitPacker(uint8_t* buffer, uint32_t bitLength) {
    this->m_buffer = buffer;
    this->m_bitLength = bitLength;
    this->m_byteOffset = 0;
    this->m_bitOffset = 0;
}

// OFFSET: 0x00A38F20/0x1407E5450
ComSat::BitPacker::~BitPacker() {
}

// PROVEN: 398fe42b IDENTICAL/DIFFERENT
// OFFSET: 0x00A390A0/0x1407E53F0
uint32_t ComSat::BitPacker::Finalize() {
    if (this->m_bitOffset) {
        this->m_buffer[this->m_byteOffset] &= (1 << this->m_bitOffset) - 1;
        this->m_byteOffset++;
        this->m_bitOffset = 0;
    }
    return this->m_byteOffset;
}

// OFFSET: 0x00A38F30/0x1407E5210
bool ComSat::BitPacker::PackBits(const uint8_t* data, uint32_t bits) {
    if (this->m_bitLength < bits + this->m_bitOffset + 8 * this->m_byteOffset) {
        return false;
    }
    int32_t bytes = bits >> 3;
    uint32_t remainder = bits & 7;
    int32_t i;
    if (this->m_bitOffset) {
        for (i = 0; i < bytes; i++) {
            this->m_buffer[this->m_byteOffset] &= (1 << this->m_bitOffset) - 1;
            this->m_buffer[this->m_byteOffset] |= data[i] << this->m_bitOffset;
            this->m_buffer[this->m_byteOffset + 1] &= -1 << this->m_bitOffset;
            this->m_buffer[this->m_byteOffset + 1] |= data[i] >> (8 - this->m_bitOffset);
            this->m_byteOffset++;
        }
    } else {
        memcpy(&this->m_buffer[this->m_byteOffset], data, bytes);
        this->m_byteOffset += bytes;
        i = bytes;
    }
    if (remainder) {
        uint8_t value = data[i] & ((1 << remainder) - 1);
        this->m_buffer[this->m_byteOffset] &= (0xFF >> (8 - this->m_bitOffset)) | (-1 << (this->m_bitOffset + remainder));
        this->m_buffer[this->m_byteOffset] |= value << this->m_bitOffset;
        if (this->m_bitOffset + remainder >= 8) {
            this->m_buffer[this->m_byteOffset + 1] &= -1 << this->m_bitOffset;
            this->m_buffer[this->m_byteOffset + 1] |= value >> (8 - this->m_bitOffset);
            this->m_byteOffset++;
            this->m_bitOffset += remainder - 8;
            return true;
        }
        this->m_bitOffset += remainder;
    }
    return true;
}

// OFFSET: 0x00A38EF0/0x1407E5430
ComSat::BitUnpacker::BitUnpacker(const uint8_t* buffer, uint32_t bitLength) {
    this->m_buffer = buffer;
    this->m_bitLength = bitLength;
    this->m_byteOffset = 0;
    this->m_bitOffset = 0;
}

// OFFSET: 0x00A39290
ComSat::BitUnpacker::~BitUnpacker() {
}

// OFFSET: 0x00A390D0/0x1407E5460
bool ComSat::BitUnpacker::UnpackBits(uint8_t* data, uint32_t bits) {
    if (this->m_bitLength < this->m_bitOffset + bits + 8 * this->m_byteOffset) {
        return false;
    }
    int32_t bytes = bits >> 3;
    int32_t remainder = bits & 7;
    memset(data, 0, remainder ? bytes + 1 : bytes);
    int32_t i;
    if (this->m_bitOffset) {
        for (i = 0; i < bytes; i++) {
            data[i] = this->m_buffer[this->m_byteOffset] >> this->m_bitOffset;
            data[i] |= (((1 << this->m_bitOffset) - 1) & this->m_buffer[this->m_byteOffset++ + 1]) << (8 - this->m_bitOffset);
        }
    } else {
        memcpy(data, &this->m_buffer[this->m_byteOffset], bytes);
        this->m_byteOffset += bytes;
        i = bytes;
    }
    if (remainder) {
        if (!this->m_bitOffset) {
            data[i] = ((1 << remainder) - 1) & this->m_buffer[this->m_byteOffset];
            this->m_bitOffset = remainder;
            return true;
        }
        if (remainder > static_cast<int32_t>(8 - this->m_bitOffset)) {
            data[i] = this->m_buffer[this->m_byteOffset] >> this->m_bitOffset;
            this->m_byteOffset++;
            int32_t bitOffset = remainder + this->m_bitOffset - 8;
            data[i] |= (((1 << bitOffset) - 1) & this->m_buffer[this->m_byteOffset]) << (8 - this->m_bitOffset);
            this->m_bitOffset = bitOffset;
            return true;
        }
        data[i] = ((1 << remainder) - 1) & (this->m_buffer[this->m_byteOffset] >> this->m_bitOffset);
        this->m_bitOffset = (remainder + this->m_bitOffset) & 7;
    }
    return true;
}
