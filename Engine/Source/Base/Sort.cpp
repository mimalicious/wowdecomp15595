#include <cstdint>
#include "Engine/Source/Base/Sort.h"

// INLINED
static void swap(char* a, char* b, uint32_t width) {
    if (a != b) {
        while (width--) {
            char tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
}

// OFFSET: #0x0047B770
static void shortsort(char* lo, char* hi, uint32_t width, int32_t (*comp)(const void*, const void*, void*), void* context) {
    while (hi > lo) {
        char* max = lo;

        for (char* p = lo + width; p <= hi; p += width) {
            if (comp(p, max, context) > 0) {
                max = p;
            }
        }

        swap(max, hi, width);
        hi -= width;
    }
}

