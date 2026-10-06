#include <cstdint>
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Os/Tls.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D9332C
static uint32_t s_tlsIndex;

// OFFSET: %0x007D7750
void PropInitialize() {
    s_tlsIndex = OsTlsAlloc();
    OsTlsSetValue(s_tlsIndex, NULL);
}

// OFFSET: #0x0047CBE0
void PropDestroy() {
    OsTlsFree(s_tlsIndex);
}

// OFFSET: %0x007D7780
void PropSelectContext(HPROPCONTEXT context) {
    OsTlsSetValue(s_tlsIndex, context);
}

// OFFSET: %0x007D77A0
HPROPCONTEXT PropGetSelectedContext() {
    return static_cast<HPROPCONTEXT>(OsTlsGetValue(s_tlsIndex));
}

// OFFSET: %0x007D77B0
void* PropGet(PROPERTY id) {
    void** context = static_cast<void**>(OsTlsGetValue(s_tlsIndex));
    return context ? context[id] : NULL;
}

// OFFSET: %0x007D77E0
void PropSet(PROPERTY id, void* value) {
    void** context = static_cast<void**>(OsTlsGetValue(s_tlsIndex));

    if (context) {
        context[id] = value;
    }
}

// OFFSET: %0x007D7800
HPROPCONTEXT PropCreateContext() {
    return static_cast<HPROPCONTEXT>(SMemAlloc(PROPERTIES * sizeof(void*), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
}

void PropDeleteContext(HPROPCONTEXT context) {
    delete context;
}
