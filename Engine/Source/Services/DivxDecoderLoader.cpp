#include "Engine/Source/Services/DivxDecoderLoader.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Os/OsLibrary.h"

// STATIC: %0x00CD2844
DIVXDECODE g_divxDecode;

// STATIC: %0x00CD2848
INITIALIZEDIVXDECODER g_initializeDivxDecoder;

// STATIC: %0x00CD284C
SETOUTPUTFORMAT g_setOutputFormat;

// STATIC: %0x00CD2850
UNINITIALIZEDIVXDECODER g_unInitializeDivxDecoder;

// STATIC: %0x00CD2854
static void* s_divxModule;

// OFFSET: 0x00657240
void UnloadDivxDecoder() {
    if (s_divxModule) {
        OsFreeLibrary(s_divxModule);
        g_initializeDivxDecoder = NULL;
        g_setOutputFormat = NULL;
        g_divxDecode = NULL;
        g_unInitializeDivxDecoder = NULL;
        s_divxModule = NULL;
    }
}

// PROVEN: debfaccf IDENTICAL/NONE
// OFFSET: 0x00657280/0x1402FA910
void* DivxDecoderAlloc(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// PROVEN: 8c8ce177 IDENTICAL/NONE
// OFFSET: 0x006572A0
void DivxDecoderFree(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: 0x006572C0
int32_t LoadDivxDecoder() {
    s_divxModule = OsLoadLibrary("DivxDecoder.dll");

    if (!s_divxModule) {
        return 0;
    }

    g_initializeDivxDecoder = reinterpret_cast<INITIALIZEDIVXDECODER>(OsGetLibraryEntryPoint(s_divxModule, "InitializeDivxDecoder"));
    g_setOutputFormat = reinterpret_cast<SETOUTPUTFORMAT>(OsGetLibraryEntryPoint(s_divxModule, "SetOutputFormat"));
    g_divxDecode = reinterpret_cast<DIVXDECODE>(OsGetLibraryEntryPoint(s_divxModule, "DivxDecode"));
    g_unInitializeDivxDecoder = reinterpret_cast<UNINITIALIZEDIVXDECODER>(OsGetLibraryEntryPoint(s_divxModule, "UnInitializeDivxDecoder"));

    if (g_initializeDivxDecoder && g_setOutputFormat && g_divxDecode && g_unInitializeDivxDecoder) {
        return 1;
    }

    UnloadDivxDecoder();
    return 0;
}
