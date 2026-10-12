#if defined(WOWDECOMP_ARCH_32)

#include <windows.h>
#include "Engine/Source/Os/Error.h"

// OFFSET: 0x0087BC70/0x1405B3240
char* OsGetLastErrorStr() {
    char* buffer;
    DWORD error = GetLastError();
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<char*>(&buffer), 0, NULL);
    return buffer;
}

// OFFSET: 0x0087BCB0/0x1405B3290
void OsFreeLastErrorStr(char* str) {
    LocalFree(str);
}

#endif
