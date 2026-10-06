#include "Engine/Source/Os/OsClipboard.h"
#include "Engine/Source/Gx/Gx.h"

// PROVEN: 5de22437 IDENTICAL/UNPROVEN
// OFFSET: 0x0087C8C0/0x1405B4D40
char* OsClipboardGetString() {
    return IOsClipboardGetString(GxDevWindow());
}

// PROVEN: 8449d367 IDENTICAL/NONE
// OFFSET: 0x0087C8D0
int32_t OsClipboardPutString(const char* string) {
    return IOsClipboardPutString(string, GxDevWindow());
}
