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

