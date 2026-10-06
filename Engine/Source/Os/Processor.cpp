#include <cstdint>
#include "Engine/Source/Os/Processor.h"

// OFFSET: %0x007DB8F0
uint32_t OsGetProcessorFeatures() {
    int32_t vendor;
    return OsGetProcessorFeaturesEx(vendor);
}
