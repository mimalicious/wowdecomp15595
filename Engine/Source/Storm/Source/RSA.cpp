#include <cstdint>
#include "RSA.h"

// PROVEN: 339184ab IDENTICAL/IDENTICAL
// OFFSET: 0x004BDC10/0x1400F7680
void Crypt::RSA::Prepare(const void* modulus, uint32_t mLength, const void* exponent, uint32_t eLength) {
    this->m_modulus.FromBinary(modulus, mLength);
    this->m_exponent.FromBinary(exponent, eLength);
}

// PROVEN: c8bd61fc IDENTICAL/NONE
// OFFSET: 0x004BDC40
bool Crypt::RSA::Process(uint8_t* data, uint32_t length) {
    BigNum src;
    src.FromBinary(data, length);
    bool valid = SBigCompare(src.m_data, this->m_modulus.m_data) < 0;
    BigNum dst;
    dst.PowMod(src, this->m_exponent, this->m_modulus);
    dst.ToBinaryBuffer(data, length);

    return valid;
}
