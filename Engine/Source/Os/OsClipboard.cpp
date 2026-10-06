#include "Engine/Source/Os/OsClipboard.h"
#include "Engine/Source/Gx/Gx.h"

// OFFSET: %0x007E1E10
char* OsClipboardGetString() {
    return IOsClipboardGetString(GxDevWindow());
}

// OFFSET: %0x007E1E20
int32_t OsClipboardPutString(const char* string) {
    return IOsClipboardPutString(string, GxDevWindow());
}
