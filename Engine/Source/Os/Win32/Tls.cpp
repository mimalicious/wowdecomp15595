#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/Tls.h"

// OFFSET: 0x0087EF10/0x1405B8080
uint32_t OsTlsAlloc() {
    return TlsAlloc();
}

// OFFSET: 0x0087EF20/0x1405B8090
void OsTlsFree(uint32_t index) {
    TlsFree(index);
}

// OFFSET: 0x0087EF30/0x1405B80A0
void* OsTlsGetValue(uint32_t index) {
    return TlsGetValue(index);
}

// OFFSET: 0x0087EF40/0x1405B80B0
int32_t OsTlsSetValue(uint32_t index, void* value) {
    return TlsSetValue(index, value);
}

#endif
