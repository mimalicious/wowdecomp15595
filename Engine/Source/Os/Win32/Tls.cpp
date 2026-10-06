#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/Tls.h"

// PROVEN: 4bd1fb00 IDENTICAL
// OFFSET: 0x0087EF10
uint32_t OsTlsAlloc() {
    return TlsAlloc();
}

// PROVEN: 75e469b8 IDENTICAL
// OFFSET: 0x0087EF20
void OsTlsFree(uint32_t index) {
    TlsFree(index);
}

// PROVEN: 69cfc0ee IDENTICAL
// OFFSET: 0x0087EF30
void* OsTlsGetValue(uint32_t index) {
    return TlsGetValue(index);
}

// PROVEN: 33f06d92 IDENTICAL
// OFFSET: 0x0087EF40
int32_t OsTlsSetValue(uint32_t index, void* value) {
    return TlsSetValue(index, value);
}

#endif
