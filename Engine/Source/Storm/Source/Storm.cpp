#include <cstdint>
#include <cstdlib>
#include <process.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D9D53C/0x140C7F660
static int32_t s_exitcode;

// STATIC: 0x00D9D540/0x140C7F664
static int32_t s_exitcount;

// OFFSET: 0x004B8950/0x1400F1000
static void StormAtExit() {
    if (s_exitcount++ == 0) {
        _cexit();
        StormRtlDestroy();
        _exit(s_exitcode);
    }
}

// OFFSET: 0x004B8980/0x1400F1040
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

// OFFSET: 0x004B89E0/0x1400F10B0
HINSTANCE STORMAPI StormGetInstance() {
    return GetModuleHandleA(NULL);
}

// OFFSET: 0x004B89F0/0x1400F10C0
void StormExitProcess(int32_t code) {
    s_exitcode = code;
    ExitProcess(code);
}
