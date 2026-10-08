#if defined(WOWDECOMP_ARCH_32)

#include <windows.h>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/SharedMemory.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// OFFSET: 0x007BCEC0
bool Blizzard::Memory::MakeSharedMemoryName(const char* path, uint32_t size, char* name) {
    char buffer[260];
    Blizzard::String::Copy(buffer, path, sizeof(buffer));
    Blizzard::String::ToLower(buffer);
    Blizzard::String::MakeConsistentPath(buffer, buffer, sizeof(buffer));
    Blizzard::String::MakeBackslashPath(buffer, buffer, sizeof(buffer));
    Blizzard::String::Format(buffer, sizeof(buffer), "%s%d", buffer, size);

    Blizzard::Util::Digest<16> digest;
    Blizzard::Util::HashString(digest.m_data, buffer);
    digest.ToHexString(name, 33);

    return true;
}

// OFFSET: 0x007C8300/0x1404C3720
int32_t Blizzard::Memory::OpenSharedMemory(const char* path, uint32_t size, SharedMemory** memory) {
    int32_t created = 1;

    if (!path) {
        return 0;
    }

    *memory = new (1, __FILE__, __LINE__) SharedMemory();

    char name[33];
    MakeSharedMemoryName(path, size, name);
    (*memory)->m_mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);

    if (!(*memory)->m_mapping || (*memory)->m_mapping == INVALID_HANDLE_VALUE) {
        return 0;
    }

    int32_t result = 2;

    if (GetLastError() != ERROR_ALREADY_EXISTS) {
        result = created;
    }

    (*memory)->m_data = MapViewOfFile((*memory)->m_mapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, size);

    if (!(*memory)->m_data) {
        CloseHandle((*memory)->m_mapping);
        return 0;
    }

    (*memory)->m_size = size;
    (*memory)->m_name = name;

    return result;
}

// OFFSET: 0x007C83F0/0x1404C3860
void Blizzard::Memory::CloseSharedMemory(void* memory) {
    SharedMemory* shared = static_cast<SharedMemory*>(memory);

    if (shared && UnmapViewOfFile(shared->m_data) && CloseHandle(shared->m_mapping)) {
        shared->~SharedMemory();
        Blizzard::Memory::Free(shared, 0, __FILE__, __LINE__);
    }
}

#endif
