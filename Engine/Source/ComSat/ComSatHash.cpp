#include "Engine/Source/ComSat/ComSatHash.h"

// OFFSET: %0x0097AF10
uint32_t ComSat::Hash32(const void* data, uint32_t length, uint32_t seed) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t hash = seed;
    for (; length; length--) {
        hash = (hash ^ *bytes) * 0x1000193;
        bytes++;
    }
    return hash;
}
