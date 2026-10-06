#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/Storm/H/storm.h"

// STATIC: #0x00CB74F0
static BOOL (WINAPI* s_tryEnterPtr)(LPCRITICAL_SECTION);

// STATIC: %0x00CA691C
static HMODULE s_kernel32lib;

// OFFSET: %0x004C7F80
void SServerInitialize() {
    if (!s_tryEnterPtr) {
        s_kernel32lib = LoadLibraryA("kernel32.dll");

        if (s_kernel32lib) {
            s_tryEnterPtr = reinterpret_cast<BOOL (WINAPI*)(LPCRITICAL_SECTION)>(GetProcAddress(s_kernel32lib, "TryEnterCriticalSection"));
        }
    }
}

// OFFSET: %0x004C7FB0
void SServerDestroy() {
    if (s_kernel32lib) {
        FreeLibrary(s_kernel32lib);
    }
}

#endif
