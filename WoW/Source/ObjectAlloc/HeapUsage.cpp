#include <cstddef>
#include <cstdint>
#include "WoW/Source/ObjectAlloc/HeapUsage.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

// OFFSET: %0x0048F0F0
void HeapUsageDestroy() {
    ConsoleCommandUnregister("HeapUsage");
}

