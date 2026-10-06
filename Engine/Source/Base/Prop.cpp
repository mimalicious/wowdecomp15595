#include <cstdint>
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Os/Tls.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D9332C
static uint32_t s_tlsIndex;

// PROVEN: c94601bd IDENTICAL/UNPROVEN
// OFFSET: 0x00872B80/0x1405A91A0
void PropInitialize() {
    s_tlsIndex = OsTlsAlloc();
    OsTlsSetValue(s_tlsIndex, NULL);
}

// PROVEN: 607e63f1 IDENTICAL/NONE
// OFFSET: 0x00872BA0/0x1405A91C0
void PropDestroy() {
    OsTlsFree(s_tlsIndex);
}

// PROVEN: 746ee1dd IDENTICAL/IDENTICAL
// OFFSET: 0x00872BB0/0x1405A91D0
void PropSelectContext(HPROPCONTEXT context) {
    OsTlsSetValue(s_tlsIndex, context);
}

// PROVEN: d1adbcd8 IDENTICAL/IDENTICAL
// OFFSET: 0x00872BD0/0x1405A91E0
HPROPCONTEXT PropGetSelectedContext() {
    return static_cast<HPROPCONTEXT>(OsTlsGetValue(s_tlsIndex));
}

// PROVEN: 5d86df0c IDENTICAL/IDENTICAL
// OFFSET: 0x00872BE0/0x1405A91F0
void* PropGet(PROPERTY id) {
    void** context = static_cast<void**>(OsTlsGetValue(s_tlsIndex));
    return context ? context[id] : NULL;
}

// PROVEN: b70e4a28 IDENTICAL/IDENTICAL
// OFFSET: 0x00872C10/0x1405A9220
void PropSet(PROPERTY id, void* value) {
    void** context = static_cast<void**>(OsTlsGetValue(s_tlsIndex));

    if (context) {
        context[id] = value;
    }
}

// PROVEN: fc1a4f8d IDENTICAL/DIFFERENT
// OFFSET: 0x00872C30/0x1405A9250
HPROPCONTEXT PropCreateContext() {
    return static_cast<HPROPCONTEXT>(SMemAlloc(PROPERTIES * sizeof(void*), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
}

void PropDeleteContext(HPROPCONTEXT context) {
    delete context;
}
