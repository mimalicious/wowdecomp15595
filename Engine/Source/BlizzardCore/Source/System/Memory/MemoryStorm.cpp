#include <cstdint>
#include <cstdlib>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

static System_Memory::UsingMemory s_usingMemory;

// PROVEN: 96e55dd0 IDENTICAL/NONE
// OFFSET: 0x00B392B0
static void* DefaultAllocHook(size_t bytes) {
    return malloc(bytes);
}

// PROVEN: a77d3da9 IDENTICAL/NONE
// OFFSET: 0x00A30330
static void DefaultFreeHook(void* ptr, size_t bytes) {
    free(ptr);
}

// STATIC: 0x00D1C918
void* (*Blizzard::Memory::s_allocHook)(size_t bytes) = DefaultAllocHook;

// STATIC: 0x00D1C91C
void (*Blizzard::Memory::s_freeHook)(void* ptr, size_t bytes) = DefaultFreeHook;

// INLINED
inline uint32_t ToStormFlags(uint32_t flags) {
    uint32_t stormFlags = (flags & 1) ? SMEM_FLAG_ZEROMEMORY : 0;

    if (flags & 2) {
        stormFlags |= SMEM_FLAG_4000000;
    }

    return stormFlags;
}

// OFFSET: 0x007B5810/0x1404AB7A0
void* Blizzard::Memory::Allocate(uint32_t bytes) {
    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007B5830
void* Blizzard::Memory::Allocate(uint32_t bytes, uint32_t flags) {
    return SMemAlloc(bytes, __FILE__, __LINE__, ToStormFlags(flags));
}

// OFFSET: 0x007B5890/0x1404AB840
void* operator new(size_t bytes, uint32_t flags, const char* fileName, int32_t lineNumber) {
    return Blizzard::Memory::Allocate(static_cast<uint32_t>(bytes), flags | 0x80, fileName, lineNumber);
}

// INLINED
void operator delete(void* ptr, uint32_t flags, const char* fileName, int32_t lineNumber) {
    if (ptr) {
        SMemFree(ptr, fileName, lineNumber, (flags & 1) ? SMEM_FLAG_ZEROMEMORY : 0);
    }
}

// OFFSET: 0x007AD670/0x1404A32F0
void* Blizzard::Memory::Allocate(uint32_t bytes, uint32_t flags, const char* fileName, int32_t lineNumber) {
    return SMemAlloc(bytes, fileName, lineNumber, ToStormFlags(flags));
}

// PROVEN: a77536bd IDENTICAL/DIFFERENT
// OFFSET: 0x007B5870/0x1404AB820
void Blizzard::Memory::Free(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007AD6B0/0x1404A3340
void Blizzard::Memory::Free(void* ptr, uint32_t flags, const char* fileName, int32_t lineNumber) {
    SMemFree(ptr, fileName, lineNumber, ToStormFlags(flags));
}

// PROVEN: 343a02cf IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void System_Memory::ShutdownProc() {
}

// INLINED
System_Memory::UsingMemory::UsingMemory() {
    Blizzard::Using::StartUsing("Blizzard::Memory");
    System_Using::Intern("Blizzard::Memory")->shutdownproc = System_Memory::ShutdownProc;
}

// INLINED
System_Memory::UsingMemory::~UsingMemory() {
    Blizzard::Using::StopUsing("Blizzard::Memory");
}
