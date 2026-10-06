#include "BigInteger.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x00A6D400
BigData* BigIntegerFromInt(uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    return num;
}

// OFFSET: %0x00A6D430
BigData* BigIntegerFromBytes(const void* data, uint32_t bytes) {
    BigData* num;
    SBigNew(&num);
    SBigFromBinary(num, data, bytes);

    return num;
}

// OFFSET: %0x00A6D460
uint32_t BigIntegerToBytes(BigData* a, uint8_t* data, uint32_t maxBytes) {
    uint32_t bytes;
    SBigToBinaryBuffer(a, data, maxBytes, &bytes);

    if (bytes < maxBytes) {
        memset(&data[bytes], 0, maxBytes - bytes);
    }

    return bytes;
}

// OFFSET: %0x00A6D4A0
int32_t BigIntegerCmp(BigData* a, BigData* b) {
    return SBigCompare(a, b);
}

// OFFSET: %0x00A6D4C0
int32_t BigIntegerCmpInt(BigData* a, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    int32_t result = SBigCompare(a, num);
    SBigDel(num);

    return result;
}

// OFFSET: %0x00A6D500
void BigIntegerAdd(BigData* a, BigData* b, BigData* c) {
    SBigAdd(a, b, c);
}

// OFFSET: %0x00A6D520
void BigIntegerAddInt(BigData* a, BigData* b, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);
    SBigAdd(a, b, num);
    SBigDel(num);
}

// OFFSET: %0x00A6D560
void BigIntegerSub(BigData* a, BigData* b, BigData* c) {
    SBigSub(a, b, c);
}

// OFFSET: %0x00A6D580
void BigIntegerMul(BigData* a, BigData* b, BigData* c) {
    SBigMul(a, b, c);
}

// OFFSET: %0x00A6D5A0
void BigIntegerMod(BigData* a, BigData* b, BigData* c) {
    SBigMod(a, b, c);
}

// OFFSET: %0x00A6D5C0
void BigIntegerModExp(BigData* a, BigData* b, BigData* c, BigData* d) {
    SBigPowMod(a, b, c, d);
}

// OFFSET: %0x00A6D5E0
void BigIntegerFree(BigData* a) {
    SBigDel(a);
}

// OFFSET: %0x00A6D5F0
uint32_t BigIntegerBitLen(BigData* a) {
    uint32_t bits;
    SBigBitLen(a, &bits);

    return bits;
}
