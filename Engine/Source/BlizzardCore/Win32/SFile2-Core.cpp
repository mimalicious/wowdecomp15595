#if defined(WOWDECOMP_ARCH_32)

#include <windows.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

// STATIC: %0x00D73938
static HANDLE s_handle0;

// STATIC: %0x00D7A520
static HANDLE s_handle1;

// OFFSET: %0x007090E0
void SFile::Destroy() {
    if (s_handle0) {
        CloseHandle(s_handle0);
        s_handle0 = NULL;
    }

    if (s_handle1) {
        CloseHandle(s_handle1);
        s_handle1 = NULL;
    }
}

#endif
