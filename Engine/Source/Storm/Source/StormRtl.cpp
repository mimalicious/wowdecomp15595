#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x004B9B00
void StormRtlDestroy() {
    SErrSetLogCallback(NULL);
    SMemDestroy();
    SErrDestroy();
    SLogDestroy();
}

// OFFSET: #0x0076E490
void StormRtlInitialize() {
    SLogInitialize();
    SMemInitialize();
}
