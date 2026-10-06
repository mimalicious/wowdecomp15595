#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"

// OFFSET: #0x004294C0
char* GetComponentVersionData(uint32_t* size) {
    *size = 0;

    HRSRC resource = FindResourceExA(NULL, "BLIZZARDCOMPONENT", "BLIZZARDCOMPONENT", 0);

    if (!resource) {
        return NULL;
    }

    HGLOBAL handle = LoadResource(NULL, resource);

    if (!handle) {
        return NULL;
    }

    const void* source = LockResource(handle);
    *size = SizeofResource(NULL, resource);

    char* data = new char[*size + 1];
    data[*size] = '\0';
    memcpy(data, source, *size);

    return data;
}

#endif
