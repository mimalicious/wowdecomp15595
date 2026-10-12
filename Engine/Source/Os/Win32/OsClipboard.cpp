#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Os/OsClipboard.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0087C8F0/0x1405B4D80
char* IOsClipboardGetString(void* window) {
    if (!OpenClipboard(static_cast<HWND>(window))) {
        return NULL;
    }

    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (!data) {
        CloseClipboard();
        return NULL;
    }

    const uint16_t* wide = static_cast<const uint16_t*>(GlobalLock(data));
    if (!wide) {
        CloseClipboard();
        return NULL;
    }

    uint32_t length = static_cast<uint32_t>(wcslen(reinterpret_cast<const wchar_t*>(wide)));
    char* string = static_cast<char*>(SMemAlloc(3 * length + 1, __FILE__, __LINE__, 0));
    uint32_t written;
    ConvertUTF16toUTF8(string, 3 * length, wide, length, &written, NULL);
    string[written] = '\0';

    GlobalUnlock(data);
    CloseClipboard();
    return string;
}

// OFFSET: 0x0087C990/0x1405B4E90
void OsClipboardFreeString(char* string) {
    if (string) {
        SMemFree(string, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x0087C9B0/0x1405B4EC0
int32_t IOsClipboardPutString(const char* string, void* window) {
    uint32_t size = static_cast<uint32_t>(strlen(string) + 1);

    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DDESHARE, size * sizeof(uint16_t));
    if (!memory) {
        return 0;
    }

    uint16_t* data = static_cast<uint16_t*>(GlobalLock(memory));
    if (!data) {
        CloseClipboard();
        return 0;
    }

    ConvertUTF8toUTF16(data, size, string, 0x7FFFFFFF, NULL, NULL);
    GlobalUnlock(memory);

    if (!OpenClipboard(static_cast<HWND>(window))) {
        GlobalFree(memory);
        return 0;
    }

    if (!EmptyClipboard()) {
        CloseClipboard();
        GlobalFree(memory);
        return 0;
    }

    SetClipboardData(CF_UNICODETEXT, memory);
    CloseClipboard();
    return 1;
}

#endif
