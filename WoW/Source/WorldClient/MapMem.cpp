#include "MapMem.h"
#include "WoW/Source/WorldClient/MapArea.h"

// OFFSET: %0x006D6980
void* CMapArea::AllocAsyncLoadBuffer(int32_t fileSize) {
    return SMemAlloc(fileSize, __FILE__, __LINE__, 0);
}

// PROVEN: a8109484 IDENTICAL
// OFFSET: 0x00775920
void CMapArea::FreeAsyncLoadBuffer(void* buffer, int32_t size) {
    SMemFree(buffer, __FILE__, __LINE__, 0);
}
