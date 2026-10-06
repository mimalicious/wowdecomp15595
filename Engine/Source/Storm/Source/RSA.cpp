#include <cstdint>
#include "RSA.h"

// PROVEN: 339184ab IDENTICAL
// OFFSET: 0x004BDC10
void Crypt::RSA::Prepare(const void* modulus, uint32_t mLength, const void* exponent, uint32_t eLength) {
    this->m_modulus.FromBinary(modulus, mLength);
    this->m_exponent.FromBinary(exponent, eLength);
}

// OFFSET: %0x004C4260
void Crypt::RSA::Process(uint8_t* data, uint32_t length) {
    BigNum src;
    src.FromBinary(data, length);
    BigNum dst;
    dst.PowMod(src, this->m_exponent, this->m_modulus);
    dst.ToBinaryBuffer(data, length);
}
