#include "MapMem.h"
#include "WoW/Source/WorldClient/MapArea.h"

// OFFSET: %0x006D6980
void* CMapArea::AllocAsyncLoadBuffer(int32_t fileSize) {
    return SMemAlloc(fileSize, __FILE__, __LINE__, 0);
}

// OFFSET: %0x006D69A0
void CMapArea::FreeAsyncLoadBuffer(void* buffer, int32_t size) {
    SMemFree(buffer, __FILE__, __LINE__, 0);
}
