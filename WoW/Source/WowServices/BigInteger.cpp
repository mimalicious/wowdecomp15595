#include "BigInteger.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 5f8f4ae3 IDENTICAL/NONE
// OFFSET: 0x00B3AC00
BigData* BigIntegerFromInt(uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    return num;
}

// PROVEN: 40ef6b16 IDENTICAL/NONE
// OFFSET: 0x00B3AC30
BigData* BigIntegerFromBytes(const void* data, uint32_t bytes) {
    BigData* num;
    SBigNew(&num);
    SBigFromBinary(num, data, bytes);

    return num;
}

// PROVEN: 052f2bd8 IDENTICAL/NONE
// OFFSET: 0x00B3AC60
uint32_t BigIntegerToBytes(BigData* a, uint8_t* data, uint32_t maxBytes) {
    uint32_t bytes;
    SBigToBinaryBuffer(a, data, maxBytes, &bytes);

    if (bytes < maxBytes) {
        memset(&data[bytes], 0, maxBytes - bytes);
    }

    return bytes;
}

// PROVEN: b25209d7 IDENTICAL/NONE
// OFFSET: 0x00B3ACA0
int32_t BigIntegerCmp(BigData* a, BigData* b) {
    return SBigCompare(a, b);
}

// PROVEN: c4e96369 IDENTICAL/NONE
// OFFSET: 0x00B3ACC0
int32_t BigIntegerCmpInt(BigData* a, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    int32_t result = SBigCompare(a, num);
    SBigDel(num);

    return result;
}

// PROVEN: 9029ae02 IDENTICAL/NONE
// OFFSET: 0x00B3AD00
void BigIntegerAdd(BigData* a, BigData* b, BigData* c) {
    SBigAdd(a, b, c);
}

// PROVEN: 70ca5ff7 IDENTICAL/NONE
// OFFSET: 0x00B3AD20
void BigIntegerAddInt(BigData* a, BigData* b, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);
    SBigAdd(a, b, num);
    SBigDel(num);
}

// PROVEN: 5354d0f5 IDENTICAL/NONE
// OFFSET: 0x00B3AD60
void BigIntegerSub(BigData* a, BigData* b, BigData* c) {
    SBigSub(a, b, c);
}

// PROVEN: f5e27b17 IDENTICAL/NONE
// OFFSET: 0x00B3AD80
void BigIntegerMul(BigData* a, BigData* b, BigData* c) {
    SBigMul(a, b, c);
}

// PROVEN: 37c96a45 IDENTICAL/NONE
// OFFSET: 0x00B3ADA0
void BigIntegerMod(BigData* a, BigData* b, BigData* c) {
    SBigMod(a, b, c);
}

// PROVEN: 574f0641 IDENTICAL/NONE
// OFFSET: 0x00B3ADC0
void BigIntegerModExp(BigData* a, BigData* b, BigData* c, BigData* d) {
    SBigPowMod(a, b, c, d);
}

// PROVEN: 92851f68 IDENTICAL/NONE
// OFFSET: 0x00B3ADE0
void BigIntegerFree(BigData* a) {
    SBigDel(a);
}

// PROVEN: e6a6ea0a IDENTICAL/NONE
// OFFSET: 0x00B3ADF0
uint32_t BigIntegerBitLen(BigData* a) {
    uint32_t bits;
    SBigBitLen(a, &bits);

    return bits;
}
