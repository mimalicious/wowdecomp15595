#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/Os/OsLibrary.h"

// OFFSET: 0x0087EED0/0x1405B8050
void* OsLoadLibrary(const char* fileName) {
    return LoadLibraryA(fileName);
}

// OFFSET: 0x0087EEE0/0x1405B8060
void OsFreeLibrary(void* module) {
    FreeLibrary(static_cast<HMODULE>(module));
}

// OFFSET: 0x0087EEF0/0x1405B8070
void* OsGetLibraryEntryPoint(void* module, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(module), name));
}

#endif
