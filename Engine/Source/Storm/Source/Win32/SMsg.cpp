#if defined(WOWDECOMP_ARCH_32)

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <windows.h>
#include "Engine/Source/Storm/Source/SMsg.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00BB41B8
static TSList<WNDREC> s_wndlist;

// STATIC: %0x00CA6904
static RECT s_defaultwindowrect;

// STATIC: %0x00CA6914
static HWND s_defaultwindow;

// INLINED
static WNDREC* FindWindow(HWND window) {
    for (WNDREC* rec = s_wndlist.Head(); !TSList<WNDREC>::IsEnd(rec); rec = s_wndlist.RawNext(rec)) {
        if (rec->window == window) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: #0x0077E820
HWND STORMAPI SMsgGetDefaultWindow() {
    return s_defaultwindow;
}

// OFFSET: %0x004C7E90
static void DeleteWindow(HWND window) {
    if (window == s_defaultwindow) {
        s_defaultwindow = NULL;
        s_defaultwindowrect.left = 0;
        s_defaultwindowrect.top = 0;
        s_defaultwindowrect.right = 0;
        s_defaultwindowrect.bottom = 0;
    }

    WNDREC* rec = FindWindow(window);

    if (rec) {
        SEvtUnregisterType('SMSG', reinterpret_cast<uintptr_t>(window));
        SEvtUnregisterType('SMSH', reinterpret_cast<uintptr_t>(window));
        SEvtUnregisterType('SMSI', reinterpret_cast<uintptr_t>(window));
        SEvtUnregisterType('SMSK', reinterpret_cast<uintptr_t>(window));
        SEvtUnregisterType('SMSJ', reinterpret_cast<uintptr_t>(window));
        s_wndlist.DeleteNode(rec);
    }
}

// PROVEN: 25c0a963 IDENTICAL/IDENTICAL
// OFFSET: 0x004C1050/0x1400FB8E0
int32_t STORMAPI SMsgDestroy() {
    WNDREC* rec;

    while ((rec = s_wndlist.Head()) != NULL) {
        DeleteWindow(rec->window);
    }

    return 1;
}

// PROVEN: 0c5f8d8e IDENTICAL/UNPROVEN
// OFFSET: 0x004C1070/0x1400FB910
void SDebugPrintf(const char* format, ...) {
    char buffer[4096];
    va_list arglist;
    va_start(arglist, format);
    _vsnprintf(buffer, sizeof(buffer), format, arglist);
    OutputDebugStringA(buffer);
}

#endif
