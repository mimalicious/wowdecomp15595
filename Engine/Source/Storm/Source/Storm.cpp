#include <cstdint>
#include <cstdlib>
#include <process.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C9DDCC
static int32_t s_exitcode;

// STATIC: 0x00D9D540/0x140C7F664
static int32_t s_exitcount;

// PROVEN: 7422ad48 IDENTICAL/IDENTICAL
// OFFSET: 0x004B8950/0x1400F1000
static void StormAtExit() {
    if (s_exitcount++ == 0) {
        _cexit();
        StormRtlDestroy();
        _exit(s_exitcode);
    }
}

// OFFSET: 0x004B8980
void STORMAPI StormInitialize() {
    SStrInitialize();
    SErrInitialize();
    SServerInitialize();
    SFile::Initialize();
}

// OFFSET: 0x004B89A0/0x1400F1070
void STORMAPI StormDestroy() {
    SRgnDestroy();
    SMsgDestroy();
    SEvtDestroy();
    SCmdDestroy();
    SFile::Destroy();
    SCompDestroy();
    SStrDestroy();
    atexit(StormAtExit);
}

// PROVEN: 6ac4be0e IDENTICAL/IDENTICAL
// OFFSET: 0x004B89E0/0x1400F10B0
HINSTANCE STORMAPI StormGetInstance() {
    return GetModuleHandleA(NULL);
}

// PROVEN: 1a673566 DIFFERENT/IDENTICAL
// OFFSET: 0x004B89F0/0x1400F10C0
void StormExitProcess(int32_t code) {
    s_exitcode = code;
    ExitProcess(code);
}
