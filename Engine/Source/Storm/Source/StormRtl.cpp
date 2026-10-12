#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x004B2CC0/0x1400E9590
void StormRtlDestroy() {
    SErrSetLogCallback(NULL);
    SErrDestroy();
    SLogDestroy();
}

// OFFSET: 0x004B2CE0
void StormRtlInitialize() {
    SMemInitialize();
    SLogInitialize();
}
