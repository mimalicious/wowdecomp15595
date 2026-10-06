#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_NEWZEROFILL_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_NEWZEROFILL_H

#include <new>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x007DEB20
template <class T>
T* NewZerofill() {
    void* memory = SMemAlloc(sizeof(T), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY);

    if (!memory) {
        return NULL;
    }

    return new (memory) T();
}

#endif
