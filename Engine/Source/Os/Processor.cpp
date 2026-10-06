#include <cstdint>
#include "Engine/Source/Os/Processor.h"

// PROVEN: 2c8f7d78 IDENTICAL
// OFFSET: 0x00875F50
uint32_t OsGetProcessorFeatures() {
    int32_t vendor;
    return OsGetProcessorFeaturesEx(vendor);
}
