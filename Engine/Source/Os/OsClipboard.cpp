#include "Engine/Source/Os/OsClipboard.h"
#include "Engine/Source/Gx/Gx.h"

// OFFSET: 0x0087C8C0/0x1405B4D40
char* OsClipboardGetString() {
    return IOsClipboardGetString(GxDevWindow());
}

// OFFSET: 0x0087C8D0/0x1405B4D60
int32_t OsClipboardPutString(const char* string) {
    return IOsClipboardPutString(string, GxDevWindow());
}
