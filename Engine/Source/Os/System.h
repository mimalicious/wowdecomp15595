#ifndef ENGINE_SOURCE_OS_SYSTEM_H
#define ENGINE_SOURCE_OS_SYSTEM_H

#include <cstdint>

enum OsType {
    OsType_Unknown = 0,
    OsType_Win95 = 1,
    OsType_Win95OSR2 = 2,
    OsType_Win98 = 3,
    OsType_Win98SE = 4,
    OsType_WinME = 5,
    OsType_WinNT4 = 6,
    OsType_Win2000 = 7,
    OsType_WinXP = 8,
    OsType_Win2003 = 9,
    OsType_Win9X_Other = 10,
    OsType_WinNT_Other = 11,
    OsType_MacOS9 = 12,
    OsType_MacOSX = 13,
    OsType_Linux = 14,
    OsType_WinVista = 15,
    OsType_Win7 = 16,
    OsType_Last = 17,
};

struct CpuIdInfo {
    /* 0000 */ uint32_t m_maxStandard;
    /* 0004 */ char m_vendor[12];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t m_standard1a;
    /* 0018 */ uint32_t m_standard1b;
    /* 001C */ uint32_t m_standard1c;
    /* 0020 */ uint32_t m_standard1d;
    /* 0024 */ uint32_t m_standard4a;
    /* 0028 */ uint32_t m_standard4b;
    /* 002C */ uint32_t m_standard4c;
    /* 0030 */ uint32_t m_standard4d;
    /* 0034 */ uint32_t m_maxExtended;
    /* 0038 */ uint32_t m_extended1c;
    /* 003C */ uint32_t m_extended1d;
    /* 0040 */ char m_brand[48];
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t m_extended8c;
};

void* CreateNamedEventA(const char* name, int32_t manualReset);
int32_t IOsGetPowerProfFrequency();
int32_t IOsGetProcessorFeatures(CpuIdInfo* info);
int32_t IOsParseProcessorFrequency(const char* brand);
void IOsSystemCpuLog(const CpuIdInfo* info);
void OsFindTextSectionBounds();
uint32_t OsGetBackgroundSleepMs();
int32_t OsGetComputerName(char* buffer, uint32_t* size);
uint32_t OsGetNumberOfProcessors();
uint32_t OsGetOsId();
uint64_t OsGetPhysicalMemory();
uint32_t OsGetProcessorId();
int32_t OsGetUserName(char* buffer, uint32_t* size);
OsType OsGetVersion();
void OsGetVersionString(char* buffer, int32_t size);
int32_t OsIsRemoteSession();
int32_t OsLaunchURL(const char* url);
void OsSetBackgroundSleepMs(uint32_t milliseconds);
void OsSetSleepInBackground(int32_t sleep);
int32_t OsSleepInBackground();
void OsSystemEnableCpuLog();
void OsValidateFunctionPointer(const void* ptr);

#endif
