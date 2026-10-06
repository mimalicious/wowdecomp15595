#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 76b0704d IDENTICAL/UNPROVEN
// OFFSET: 0x004B2CC0/0x1400E9590
void StormRtlDestroy() {
    SErrSetLogCallback(NULL);
    SErrDestroy();
    SLogDestroy();
}

// OFFSET: #0x0076E490
void StormRtlInitialize() {
    SLogInitialize();
    SMemInitialize();
}
