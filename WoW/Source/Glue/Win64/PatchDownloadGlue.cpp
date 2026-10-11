#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include <shellapi.h>
#include <cstdint>
#include "WoW/Source/Glue/CGlueMgr.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"

// OFFSET: 0x007A9460/0x14049F1F0
static bool ShellExecuteRunAs(const char* path, const char* arguments) {
    if (!arguments) {
        arguments = "";
    }

    Blizzard::StringConversion_CodepageToUTF8::ToNative nativeArguments(arguments);
    Blizzard::StringConversion_CodepageToUTF8::ToNative nativePath(path);
    Blizzard::StringConversion_CodepageToUTF8::ToNative verb("runas");

    return reinterpret_cast<uintptr_t>(ShellExecuteW(NULL, reinterpret_cast<LPCWSTR>(verb.m_data), reinterpret_cast<LPCWSTR>(nativePath.m_data), reinterpret_cast<LPCWSTR>(nativeArguments.m_data), NULL, SW_SHOWNORMAL)) > 32;
}

// OFFSET: 0x007A9550/0x14049F2E0
bool CGlueMgr::PatchDownloadExecuteCommand(const char* path, const char* arguments) {
    OSVERSIONINFOA versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionExA(&versionInfo);

    if (versionInfo.dwMajorVersion < 6) {
        return Blizzard::Process::LaunchChildProcessWithArguments(path, arguments, NULL, NULL);
    }

    return ShellExecuteRunAs(path, arguments);
}

#endif
