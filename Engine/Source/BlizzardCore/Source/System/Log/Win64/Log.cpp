#if defined(WOWDECOMP_ARCH_64)

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: bae16471 IDENTICAL/DIFFERENT
// OFFSET: 0x007C0790/0x1404BA220
char* System_Log::DefaultPreamble() {
    Blizzard::Time::TimeRec rec;
    Blizzard::Time::BreakTime(s_LogGlobals->m_startTime, rec);

    uint16_t nativeName[1024];
    char computerName[256];
    DWORD size = 1024;

    if (GetComputerNameW(reinterpret_cast<LPWSTR>(nativeName), &size)) {
        Blizzard::Unicode::ConvertUTF16to8(computerName, sizeof(computerName), nativeName, -1, NULL, NULL);
    } else {
        Blizzard::String::Copy(computerName, "<unknown>", sizeof(computerName));
    }

    char* preamble = static_cast<char*>(SMemAlloc(4096, __FILE__, __LINE__, 0));
    Blizzard::String::Format(preamble, 4096, "\n#-----------------------------------------------------------\n# System started at %04d-%02d-%02d %02d:%02d:%02d.%04d\n# system: %s\n#-----------------------------------------------------------\n", rec.year, rec.month, rec.day, rec.hour, rec.minute, rec.second, rec.nanosecond / 100000, computerName);

    return preamble;
}

// PROVEN: e76629e0 IDENTICAL/DIFFERENT
// OFFSET: 0x007C4200/0x1404BEB10
char* System_Log::DefaultDayHeader() {
    Blizzard::Time::TimeRec rec;
    Blizzard::Time::BreakTime(Blizzard::Time::GetTimestamp(), rec);

    uint16_t nativeName[1024];
    char computerName[256];
    DWORD size = 1024;

    if (GetComputerNameW(reinterpret_cast<LPWSTR>(nativeName), &size)) {
        Blizzard::Unicode::ConvertUTF16to8(computerName, sizeof(computerName), nativeName, -1, NULL, NULL);
    } else {
        Blizzard::String::Copy(computerName, "<unknown>", sizeof(computerName));
    }

    char* header = static_cast<char*>(SMemAlloc(4096, __FILE__, __LINE__, 0));
    Blizzard::String::Format(header, 4096, "\n#-----------------------------------------------------------\n# %04d-%02d-%02d\n# system: %s\n#-----------------------------------------------------------\n", rec.year, rec.month, rec.day, computerName);

    return header;
}

// OFFSET: 0x007C47E0
uint32_t System_Log::LogThread(void* param) {
    uint32_t lastFlush = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    while (s_LogGlobals->m_state > 0) {
        Sleep(100);
        uint32_t now = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

        if (now - lastFlush > 5000) {
            lastFlush = now;

            if (Acquire(0)) {
                FlushLogs(false);
                Blizzard::Lock::MutexLeave(s_LogGlobals->m_mutex);
            }
        }
    }

    s_LogGlobals->m_threadDone = true;

    return 0;
}

#endif
