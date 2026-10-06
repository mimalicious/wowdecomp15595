#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 76b0704d IDENTICAL
// OFFSET: 0x004B2CC0
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
