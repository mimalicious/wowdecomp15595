#include <cstdarg>
#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

// INLINED
static int32_t ISMemGetAllocator(va_list arglist) {
    *va_arg(arglist, uint32_t*) = 0;
    return 1;
}

// PROVEN: 86e7dd27 IDENTICAL/NONE
// OFFSET: 0x004B2C60
int32_t STORMAPI ISMemGenerateReport(va_list arglist) {
    return va_arg(arglist, int32_t) == 0;
}

// INLINED: [0x0077EC65->0x0077EC67]
static int32_t ISMemMarkAllHeaps(va_list arglist) {
    return 0;
}

// OFFSET: 0x004C33D0
static int32_t ISStrI64ToString(va_list arglist) {
    int64_t num = va_arg(arglist, int64_t);
    char* buf = va_arg(arglist, char*);
    uint32_t destsize = va_arg(arglist, uint32_t);
    Int64ToString(num, buf, destsize);
    return 1;
}

// OFFSET: 0x004C3400
int32_t StormCallService(int32_t selector, ...) {
    va_list arglist;
    va_start(arglist, selector);

    int32_t result = 0;

    switch (selector) {
    case STORM_SERVICE_GETALLOCATOR:
        result = ISMemGetAllocator(arglist);
        break;
    case STORM_SERVICE_GENERATEREPORT:
        result = ISMemGenerateReport(arglist);
        break;
    case STORM_SERVICE_I64TOSTRING:
        result = ISStrI64ToString(arglist);
        break;
    case STORM_SERVICE_MARKALLHEAPS:
        result = ISMemMarkAllHeaps(arglist);
        break;
    }

    va_end(arglist);
    return result;
}
