#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <winerror.h>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C9F7B0/0x140B3B908
static int32_t s_exitonfailure = 1;

// STATIC: 0x00C9F7B4/0x140B3B90C
static int32_t s_heaptracking = 1;

// OFFSET: 0x004B2A10/0x1400E95F0
void STORMAPI SMemSetDebugFlags(uint32_t flags, uint8_t changeMask) {
    if (changeMask & SMEM_DEBUG_NOEXITONFAILURE) {
        s_exitonfailure = (flags & SMEM_DEBUG_NOEXITONFAILURE) == 0;
    }

    if (changeMask & SMEM_DEBUG_HEAPTRACKING) {
        s_heaptracking = (flags & SMEM_DEBUG_HEAPTRACKING) != 0;
    }
}

// OFFSET: 0x004B2A50/0x1400E9620
static void FatalError(size_t bytes, const char* filename, int32_t linenumber) {
    char buffer[128];
    sprintf(buffer, "Requested %u bytes of memory", static_cast<uint32_t>(bytes));

    SErrDisplayError(STORM_ERROR_NOT_ENOUGH_MEMORY, filename, linenumber, buffer, s_exitonfailure == 0, 1, 0, 0x11111111);

    if (s_exitonfailure) {
        StormExitProcess(1);
    }
}

// OFFSET: 0x004B2AC0/0x1400E96C0
void* STORMAPI SMemAlloc(size_t bytes, const char* filename, int32_t linenumber, uint32_t flags) {
    size_t size = (bytes + 7) & ~static_cast<size_t>(7);
    void* ptr = (flags & SMEM_FLAG_ZEROMEMORY) ? calloc(1, size) : malloc(size);

    if (!ptr) {
        FatalError(bytes, filename, linenumber);
        return NULL;
    }

    _msize(ptr);

    return ptr;
}

// OFFSET: 0x004B2C20/0x1400E98A0
int32_t STORMAPI SMemFree(void* ptr, const char* filename, int32_t linenumber, uint32_t flags) {
    if (ptr) {
        _msize(ptr);
        free(ptr);
    }

    return 1;
}

// OFFSET: 0x004B2C50/0x14090EBB0
int32_t STORMAPI SMemIsValidPointer(void* ptr, size_t bytes, uint32_t flags) {
    return 1;
}

// OFFSET: 0x004B2B20/0x1400E9740
void* STORMAPI SMemReAlloc(void* ptr, size_t bytes, const char* filename, int32_t linenumber, uint32_t flags) {
    if (flags == SMEM_REALLOC_NULL) {
        return NULL;
    }

    if (!ptr) {
        return SMemAlloc(bytes, filename, linenumber, flags);
    }

    size_t oldsize = _msize(ptr);

    if (flags & SMEM_FLAG_NOREALLOC) {
        return NULL;
    }

    size_t size = (bytes + 7) & ~static_cast<size_t>(7);
    void* newptr = realloc(ptr, size);

    if (!newptr) {
        if (size) {
            FatalError(bytes, filename, linenumber);
        }

        return NULL;
    }

    size_t newsize = _msize(newptr);

    if (s_heaptracking && newptr != ptr) {
        DebugLog(-1, "[%08lx] REALLOC %10d bytes <-- [%08lx]/%d, requested=%d (%s: %d)\n", newptr, newsize, ptr, oldsize, bytes, filename, linenumber);
    }

    if (oldsize < bytes && (flags & SMEM_FLAG_ZEROMEMORY)) {
        memset(static_cast<uint8_t*>(newptr) + oldsize, 0, bytes - oldsize);
    }

    return newptr;
}

// OFFSET: 0x00401010/0x140001060
void* operator new(size_t bytes) {
    return SMemAlloc(bytes, "new", SLOG_FUNCTION, 0);
}

// OFFSET: 0x00872C50/0x140001030
void operator delete(void* ptr) {
    if (ptr) {
        SMemFree(ptr, "delete", SLOG_FUNCTION, 0);
    }
}

// OFFSET: 0x008499B0/0x140577DB0
void* operator new[](size_t bytes) {
    return SMemAlloc(bytes, "new[]", SLOG_FUNCTION, 0);
}

// OFFSET: 0x00401030/0x140001080
void operator delete[](void* ptr) {
    if (ptr) {
        SMemFree(ptr, "delete[]", SLOG_FUNCTION, 0);
    }
}

int32_t STORMAPI SMemDestroy() {
    return 1;
}

// OFFSET: 0x004B2C80/0x1400E9560
static void* StormAllocHook(size_t bytes) {
    return SMemAlloc(bytes, "blz", 0, 0);
}

// OFFSET: 0x004B2CA0
static void StormFreeHook(void* ptr, size_t bytes) {
    SMemFree(ptr, NULL, 0, 0);
}

// INLINED
void STORMAPI SMemInitialize() {
    Blizzard::Memory::s_allocHook = StormAllocHook;
    Blizzard::Memory::s_freeHook = StormFreeHook;
}
