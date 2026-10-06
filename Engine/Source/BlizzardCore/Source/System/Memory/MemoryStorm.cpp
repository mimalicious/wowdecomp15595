#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

static System_Memory::UsingMemory s_usingMemory;

// INLINED
inline uint32_t ToStormFlags(uint32_t flags) {
    uint32_t stormFlags = (flags & 1) ? SMEM_FLAG_ZEROMEMORY : 0;

    if (flags & 2) {
        stormFlags |= SMEM_FLAG_4000000;
    }

    return stormFlags;
}

// OFFSET: %0x00717300
void* Blizzard::Memory::Allocate(uint32_t bytes) {
    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: %0x00717320
void* Blizzard::Memory::Allocate(uint32_t bytes, uint32_t flags) {
    return SMemAlloc(bytes, __FILE__, __LINE__, ToStormFlags(flags));
}

// OFFSET: #0x0042E3D0
void* operator new(size_t bytes, uint32_t flags, const char* fileName, int32_t lineNumber) {
    return Blizzard::Memory::Allocate(static_cast<uint32_t>(bytes), flags | 0x80, fileName, lineNumber);
}

// OFFSET: #0x004284B0
void operator delete(void* ptr, uint32_t flags, const char* fileName, int32_t lineNumber) {
    if (ptr) {
        SMemFree(ptr, fileName, lineNumber, (flags & 1) ? SMEM_FLAG_ZEROMEMORY : 0);
    }
}

// OFFSET: #0x00428450
void* Blizzard::Memory::Allocate(uint32_t bytes, uint32_t flags, const char* fileName, int32_t lineNumber) {
    return SMemAlloc(bytes, fileName, lineNumber, ToStormFlags(flags));
}

// PROVEN: a77536bd IDENTICAL/DIFFERENT
// OFFSET: 0x007B5870/0x1404AB820
void Blizzard::Memory::Free(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: #0x00428480
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
