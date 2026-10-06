#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/Tls.h"

// OFFSET: %0x007E3EB0
uint32_t OsTlsAlloc() {
    return TlsAlloc();
}

// OFFSET: %0x007E3EC0
void OsTlsFree(uint32_t index) {
    TlsFree(index);
}

// OFFSET: %0x007E3ED0
void* OsTlsGetValue(uint32_t index) {
    return TlsGetValue(index);
}

// OFFSET: %0x007E3EE0
int32_t OsTlsSetValue(uint32_t index, void* value) {
    return TlsSetValue(index, value);
}

#endif
