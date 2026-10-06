#include <cstdint>
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C988D8
SIMD::FunctionTable SIMD::s_functions;

// STATIC: %0x00BAEDA0
static void (*const s_binders[])(SIMD::FunctionTable*) = {
    SIMD::Init_C,
    SIMD::Init_SSE,
    SIMD::Init_SSE2,
    SIMD::Init_SSE3,
    SIMD::Init_SSSE3,
    SIMD::Init_SSE41,
    SIMD::Init_AVX,
};

// OFFSET: %0x00498E30
void SIMD::BindAll(int32_t features) {
    uint32_t bit = 1;

    for (void (*const* binder)(SIMD::FunctionTable*) = s_binders; binder < s_binders + sizeof(s_binders) / sizeof(s_binders[0]); binder++) {
        if (bit & (features | 1)) {
            (*binder)(&SIMD::s_functions);
        }

        bit = (bit << 1) | (bit >> 31);
    }
}

// OFFSET: %0x00498E70
void* SIMD::AllocA16(uint32_t size) {
    uint8_t* ptr = static_cast<uint8_t*>(SMemAlloc(size + 16, __FILE__, __LINE__, 0));

    if (!ptr) {
        return NULL;
    }

    uint8_t pad = static_cast<uint8_t>(16 - (reinterpret_cast<uintptr_t>(ptr) & 0xF));
    ptr += pad;
    ptr[-1] = pad;

    return ptr;
}

// OFFSET: %0x00498EB0
void SIMD::FreeA16(void* ptr) {
    if (ptr) {
        uint8_t* base = static_cast<uint8_t*>(ptr) - static_cast<uint8_t*>(ptr)[-1];
        delete[] base;
    }
}

// PROVEN: 3e8552cc IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SIMD::Init_SSE3(SIMD::FunctionTable* table) {
}

// PROVEN: 0b701b9a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SIMD::Init_SSSE3(SIMD::FunctionTable* table) {
}

// PROVEN: 1b111dc8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SIMD::Init_AVX(SIMD::FunctionTable* table) {
}
