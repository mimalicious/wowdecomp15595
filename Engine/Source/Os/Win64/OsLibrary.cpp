#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/Os/OsLibrary.h"

// PROVEN: 7c87b7e5 IDENTICAL/IDENTICAL
// OFFSET: 0x0087EED0/0x1405B8050
void* OsLoadLibrary(const char* fileName) {
    return LoadLibraryA(fileName);
}

// PROVEN: dc5ea56e IDENTICAL/NONE
// OFFSET: 0x0087EEE0
void OsFreeLibrary(void* module) {
    FreeLibrary(static_cast<HMODULE>(module));
}

// PROVEN: f455e7fb IDENTICAL/NONE
// OFFSET: 0x0087EEF0
void* OsGetLibraryEntryPoint(void* module, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
}

#endif
