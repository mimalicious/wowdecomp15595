#include "BigInteger.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00B3AC00/0x140907900
BigData* BigIntegerFromInt(uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    return num;
}

// OFFSET: 0x00B3AC30/0x140907930
BigData* BigIntegerFromBytes(const void* data, uint32_t bytes) {
    BigData* num;
    SBigNew(&num);
    SBigFromBinary(num, data, bytes);

    return num;
}

// OFFSET: 0x00B3AC60/0x140907970
uint32_t BigIntegerToBytes(BigData* a, uint8_t* data, uint32_t maxBytes) {
    uint32_t bytes;
    SBigToBinaryBuffer(a, data, maxBytes, &bytes);

    if (bytes < maxBytes) {
        memset(&data[bytes], 0, maxBytes - bytes);
    }

    return bytes;
}

// OFFSET: 0x00B3ACA0/0x1409079C0
int32_t BigIntegerCmp(BigData* a, BigData* b) {
    return SBigCompare(a, b);
}

// OFFSET: 0x00B3ACC0/0x1409079D0
int32_t BigIntegerCmpInt(BigData* a, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);

    int32_t result = SBigCompare(a, num);
    SBigDel(num);

    return result;
}

// OFFSET: 0x00B3AD00/0x140907A20
void BigIntegerAdd(BigData* a, BigData* b, BigData* c) {
    SBigAdd(a, b, c);
}

// OFFSET: 0x00B3AD20/0x140907A30
void BigIntegerAddInt(BigData* a, BigData* b, uint32_t value) {
    BigData* num;
    SBigNew(&num);
    SBigFromUnsigned(num, value);
    SBigAdd(a, b, num);
    SBigDel(num);
}

// OFFSET: 0x00B3AD60/0x140907A90
void BigIntegerSub(BigData* a, BigData* b, BigData* c) {
    SBigSub(a, b, c);
}

// OFFSET: 0x00B3AD80/0x140907AA0
void BigIntegerMul(BigData* a, BigData* b, BigData* c) {
    SBigMul(a, b, c);
}

// OFFSET: 0x00B3ADA0/0x140907AB0
void BigIntegerMod(BigData* a, BigData* b, BigData* c) {
    SBigMod(a, b, c);
}

// OFFSET: 0x00B3ADC0/0x140907AC0
void BigIntegerModExp(BigData* a, BigData* b, BigData* c, BigData* d) {
    SBigPowMod(a, b, c, d);
}

// OFFSET: 0x00B3ADE0/0x140907AD0
void BigIntegerFree(BigData* a) {
    SBigDel(a);
}

// OFFSET: 0x00B3ADF0/0x140907AE0
uint32_t BigIntegerBitLen(BigData* a) {
    uint32_t bits;
    SBigBitLen(a, &bits);

    return bits;
}
