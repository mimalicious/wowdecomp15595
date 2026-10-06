#if defined(WOWDECOMP_ARCH_32)

#include <windows.h>
#include "Engine/Source/Os/OsLibrary.h"

// PROVEN: 7c87b7e5 IDENTICAL
// OFFSET: 0x0087EED0
void* OsLoadLibrary(const char* fileName) {
    return LoadLibraryA(fileName);
}

// PROVEN: dc5ea56e IDENTICAL
// OFFSET: 0x0087EEE0
void OsFreeLibrary(void* module) {
    FreeLibrary(static_cast<HMODULE>(module));
}

// OFFSET: %0x007DE270
void* OsGetLibraryEntryPoint(void* module, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
}

#endif
