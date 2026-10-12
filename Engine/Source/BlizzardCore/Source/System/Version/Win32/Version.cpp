#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"
#include "Engine/Source/BlizzardCore/Source/System/Version/Version.h"

typedef DWORD (WINAPI* GETFILEVERSIONINFOSIZEWPROC)(LPCWSTR filename, LPDWORD handle);
typedef BOOL (WINAPI* GETFILEVERSIONINFOWPROC)(LPCWSTR filename, DWORD handle, DWORD size, LPVOID data);
typedef BOOL (WINAPI* VERQUERYVALUEWPROC)(LPCVOID block, LPCWSTR subBlock, LPVOID* buffer, PUINT length);

// OFFSET: 0x007AE6E0
DWORD OsGetFileVersionInfoSizeW(LPCWSTR filename, LPDWORD handle) {
    HMODULE module = LoadLibraryW(L"version.dll");

    if (!module) {
        return 0;
    }

    DWORD result = 0;
    GETFILEVERSIONINFOSIZEWPROC proc = reinterpret_cast<GETFILEVERSIONINFOSIZEWPROC>(GetProcAddress(module, "GetFileVersionInfoSizeW"));

    if (proc) {
        result = proc(filename, handle);
    } else {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    }

    FreeLibrary(module);
    return result;
}

// OFFSET: 0x007AE740/0x1404A4330
BOOL OsGetFileVersionInfoW(LPCWSTR filename, DWORD handle, DWORD size, LPVOID data) {
    HMODULE module = LoadLibraryW(L"version.dll");

    if (!module) {
        return FALSE;
    }

    BOOL result = FALSE;
    GETFILEVERSIONINFOWPROC proc = reinterpret_cast<GETFILEVERSIONINFOWPROC>(GetProcAddress(module, "GetFileVersionInfoW"));

    if (proc) {
        result = proc(filename, handle, size, data);
    } else {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    }

    FreeLibrary(module);
    return result;
}

// OFFSET: 0x007AE7B0
BOOL OsVerQueryValueW(LPCVOID block, LPCWSTR subBlock, LPVOID* buffer, PUINT length) {
    HMODULE module = LoadLibraryW(L"version.dll");

    if (!module) {
        return FALSE;
    }

    BOOL result = FALSE;
    VERQUERYVALUEWPROC proc = reinterpret_cast<VERQUERYVALUEWPROC>(GetProcAddress(module, "VerQueryValueW"));

    if (proc) {
        result = proc(block, subBlock, buffer, length);
    } else {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    }

    FreeLibrary(module);
    return result;
}

// OFFSET: 0x007BD6A0
static bool GetVersionInfo(const char* path, Blizzard::Version::FileVersion& version, int32_t type) {
    version.m_major = 0;
    version.m_minor = 0;
    version.m_build = 0;
    version.m_revision = 0;

    if (Blizzard::File::Exists(path) != 1) {
        return false;
    }

    Blizzard::StringConversion_CodepageToUTF8::ToNative nativePath(path);

    DWORD handle;
    DWORD size = OsGetFileVersionInfoSizeW(reinterpret_cast<LPCWSTR>(nativePath.m_data), &handle);
    uint8_t* data = NULL;

    if (size) {
        data = new uint8_t[size];
    }

    if (!OsGetFileVersionInfoW(reinterpret_cast<LPCWSTR>(nativePath.m_data), 0, size, data)) {
        if (data) {
            delete[] data;
        }

        return false;
    }

    VS_FIXEDFILEINFO* info;
    UINT length;

    if (!OsVerQueryValueW(data, L"\\", reinterpret_cast<LPVOID*>(&info), &length) || length < sizeof(VS_FIXEDFILEINFO)) {
        if (data) {
            delete[] data;
        }

        return false;
    }

    if (type == 0) {
        version.m_major = HIWORD(info->dwProductVersionMS);
        version.m_minor = LOWORD(info->dwProductVersionMS);
        version.m_build = HIWORD(info->dwProductVersionLS);
        version.m_revision = LOWORD(info->dwProductVersionLS);
    } else if (type == 1) {
        version.m_major = HIWORD(info->dwFileVersionMS);
        version.m_minor = LOWORD(info->dwFileVersionMS);
        version.m_build = HIWORD(info->dwFileVersionLS);
        version.m_revision = LOWORD(info->dwFileVersionLS);
    }

    if (data) {
        delete[] data;
    }

    return true;
}

// OFFSET: 0x007BD840/0x1404B60A0
bool Blizzard::Version::GetFileVersion(const char* path, FileVersion& version) {
    return GetVersionInfo(path, version, 1);
}

#endif
