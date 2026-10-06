#include <cstdint>
#include <new>
#include "Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007B16A0
CGxDevice* CreateCGxDeviceD3d11() {
    void* mem = SMemAlloc(sizeof(CGxDeviceD3d11), __FILE__, __LINE__, 0);

    if (!mem) {
        return NULL;
    }

    return new (mem) CGxDeviceD3d11();
}
