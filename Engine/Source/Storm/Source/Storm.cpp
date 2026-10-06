#include <cstdint>
#include <cstdlib>
#include <process.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C9DDCC
static int32_t s_exitcode;

// STATIC: #0x00CAE94C
static int32_t s_exitcount;

// OFFSET: %0x004BF150
static void StormAtExit() {
    if (s_exitcount++ == 0) {
        _cexit();
        StormRtlDestroy();
        _exit(s_exitcode);
    }
}

// OFFSET: #0x007739E0
void STORMAPI StormInitialize() {
    SStrInitialize();
    SErrInitialize();
    SServerInitialize();
    SFile::Initialize();
}

// OFFSET: %0x004BF1A0
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

// OFFSET: %0x004BF1D0
HINSTANCE STORMAPI StormGetInstance() {
    return GetModuleHandleA(NULL);
}

// OFFSET: #0x00773A40
void StormExit(int32_t code) {
    s_exitcode = code;
    exit(code);
}

// OFFSET: %0x004BF1E0
void StormExitProcess(int32_t code) {
    s_exitcode = code;
    ExitProcess(code);
}
