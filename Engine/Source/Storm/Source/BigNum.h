#ifndef ENGINE_SOURCE_STORM_SOURCE_BIGNUM_H
#define ENGINE_SOURCE_STORM_SOURCE_BIGNUM_H

#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

class BigNum {
    public:
    /* 0000 */ BigData* m_data;

    BigNum();
    ~BigNum();
    void FromBinary(const void* data, uint32_t bytes);
    BigNum& PowMod(const BigNum& b, const BigNum& c, const BigNum& d);
    void* ToBinaryBuffer(void* data, uint32_t bytes) const;
};

#include "Engine/Source/Storm/Source/BigNum.inl"

#endif
