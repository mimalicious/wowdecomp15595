#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/Os/OsLibrary.h"

// OFFSET: %0x007DE250
void* OsLoadLibrary(const char* fileName) {
    return LoadLibraryA(fileName);
}

// OFFSET: %0x007DE260
void OsFreeLibrary(void* module) {
    FreeLibrary(static_cast<HMODULE>(module));
}

// OFFSET: %0x007DE270
void* OsGetLibraryEntryPoint(void* module, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
}

#endif
