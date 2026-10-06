#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include "Engine/Source/Os/Error.h"

// OFFSET: %0x007E3E70
char* OsGetLastErrorStr() {
    char* buffer;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&buffer), 0, NULL);
    return buffer;
}

// OFFSET: %0x007E3EA0
void OsFreeLastErrorStr(char* str) {
    LocalFree(str);
}

#endif
