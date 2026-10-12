#if defined(WOWDECOMP_ARCH_32)

#include <windows.h>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DA609C
static BOOL (WINAPI* s_tryEnterPtr)(LPCRITICAL_SECTION);

static HMODULE s_kernel32lib;

// OFFSET: 0x004C1770/0x1400FC020
void SServerInitialize() {
    if (!s_tryEnterPtr) {
        s_kernel32lib = LoadLibraryA("kernel32.dll");

        if (s_kernel32lib) {
            s_tryEnterPtr = reinterpret_cast<BOOL (WINAPI*)(LPCRITICAL_SECTION)>(GetProcAddress(s_kernel32lib, "TryEnterCriticalSection"));
        }
    }
}

// OFFSET: 0x004C17A0/0x1400FC070
void SServerDestroy() {
    if (s_kernel32lib) {
        FreeLibrary(s_kernel32lib);
    }
}

#endif
