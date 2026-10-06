#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: #0x00B325BB
static bool s_continueAfterAssert;

// STATIC: %0x00D82070
static Blizzard::Lock::Mutex s_dbgHelpLock;

// STATIC: %0x00D86D58
static HMODULE s_dbgHelp;

// STATIC: %0x00D86D5C
static bool s_dbgHelpInitialized;

// OFFSET: %0x004BB2F0
void Blizzard::Debug::Breakpoint() {
    __try {
        __debugbreak();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

// OFFSET: %0x00724E30
void Blizzard::Debug::DefaultCallback(const char* expression, const char* message, const char* fileName, uint32_t lineNumber) {
    char text[2048];
    char path[512];
    char canonical[256];
    char directory[512];
    Blizzard::Time::TimeRec rec;

    Blizzard::Time::BreakTime(Blizzard::Time::GetTimestamp(), rec);
    Blizzard::String::Format(text, sizeof(text), "%04d/%02d/%02d %02d:%02d:%02d  %s(%d) : Assertion failed: %s  \"%s\"\n", rec.year, rec.month, rec.day, rec.hour, rec.minute, rec.second, CanonicalStr(fileName, canonical, sizeof(canonical)), lineNumber, expression, message);
    fprintf(stderr, "%s", text);

    Blizzard::Process::GetProcessDirectory(directory, sizeof(directory));
    Blizzard::String::Append(directory, "Errors", sizeof(directory));
    Blizzard::File::CreateDirectory(directory, true);
    Blizzard::String::Append(directory, "/Assert.log", sizeof(directory));
    Blizzard::String::MakeBackslashPath(directory, path, sizeof(path));

    FILE* stream = NULL;
    fopen_s(&stream, path, "a");

    if (stream) {
        fprintf(stream, "%s", text);
        fclose(stream);
    }

    Breakpoint();

    if (!s_continueAfterAssert) {
        abort();
    }
}

// OFFSET: %0x0071DE00
void System_Debug::ShutdownProc() {
    if (s_dbgHelpInitialized && s_dbgHelp) {
        Blizzard::Lock::MutexEnter(s_dbgHelpLock);

        if (s_dbgHelp) {
            FreeLibrary(s_dbgHelp);
            s_dbgHelp = NULL;
        }

        Blizzard::Lock::MutexLeave(s_dbgHelpLock);
        Blizzard::Lock::MutexDestroy(s_dbgHelpLock);
    }
}

#endif
