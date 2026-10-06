#include <cstdint>
#include "Engine/Source/Os/Processor.h"

// PROVEN: 2c8f7d78 IDENTICAL/UNPROVEN
// OFFSET: 0x00875F50/0x1405ACE90
uint32_t OsGetProcessorFeatures() {
    int32_t vendor;
    return OsGetProcessorFeaturesEx(vendor);
}
