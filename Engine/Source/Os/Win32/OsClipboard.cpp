#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <malloc.h>
#include <windows.h>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/Os/OsClipboard.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007E1E40
char* IOsClipboardGetString(void* window) {
    if (!OpenClipboard(static_cast<HWND>(window))) {
        return NULL;
    }

    HANDLE data = GetClipboardData(CF_TEXT);
    if (!data) {
        CloseClipboard();
        return NULL;
    }

    const char* text = static_cast<const char*>(GlobalLock(data));
    if (!text) {
        CloseClipboard();
        return NULL;
    }

    int32_t length = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, text, -1, NULL, 0);
    uint16_t* wide = static_cast<uint16_t*>(_alloca(length * sizeof(uint16_t)));
    MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, text, -1, reinterpret_cast<LPWSTR>(wide), length);

    uint32_t size = length * 3;
    char* string = static_cast<char*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    uint32_t written;
    ConvertUTF16toUTF8(string, size, wide, length, &written, NULL);
    string[written] = '\0';

    GlobalUnlock(data);
    CloseClipboard();
    return string;
}

// PROVEN: c60281e2 IDENTICAL
// OFFSET: 0x0087C990
void OsClipboardFreeString(char* string) {
    if (string) {
        SMemFree(string, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x007E1F30
int32_t IOsClipboardPutString(const char* string, void* window) {
    uint32_t size = static_cast<uint32_t>(strlen(string) + 1);

    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DDESHARE, size);
    if (!memory) {
        return 0;
    }

    char* data = static_cast<char*>(GlobalLock(memory));
    if (!data) {
        CloseClipboard();
        return 0;
    }

    uint16_t* wide = static_cast<uint16_t*>(_alloca(size * sizeof(uint16_t)));
    ConvertUTF8toUTF16(wide, size, string, 0x7FFFFFFF, NULL, NULL);
    WideCharToMultiByte(GetACP(), 0, reinterpret_cast<LPCWSTR>(wide), -1, data, size, NULL, NULL);
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

    SetClipboardData(CF_TEXT, memory);
    CloseClipboard();
    return 1;
}

#endif
