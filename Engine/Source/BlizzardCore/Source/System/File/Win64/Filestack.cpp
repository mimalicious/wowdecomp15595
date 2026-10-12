#if defined(WOWDECOMP_ARCH_64)

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <sddl.h>
#include <winioctl.h>
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"
#include "Engine/Source/Storm/H/storm.h"

#undef CreateDirectory
#undef GetFreeSpace
#undef RemoveDirectory

// STATIC: 0x00D071DC/0x140BB7AA8
System_File::IOFunc System_File::read_func = System_File::read_first;

// STATIC: 0x00D07268/0x140BB7BC0
System_File::IOFunc System_File::write_func = System_File::write_first;

// STATIC: 0x00EBAC40
bool System_File::s_read_init;

// STATIC: 0x00EBAC44
Blizzard::Lock::Atomic System_File::s_readInitCount;

// STATIC: 0x00EBAC48
bool System_File::s_write_init;

// STATIC: 0x00EBAC4C
Blizzard::Lock::Atomic System_File::s_writeInitCount;

// STATIC: 0x00EB5858/0x140DA1C88
Blizzard::Lock::Mutex System_File::s_ioMutex;

// STATIC: 0x00EB56C4/0x140DA1DB8
Blizzard::Lock::Mutex* System_File::s_ioLock;

// OFFSET: 0x007ACEC0
int32_t System_File::ToNativeAttributes(int32_t attributes) {
    int32_t result = (attributes & 0x1) != 0 ? FILE_ATTRIBUTE_READONLY : 0;

    if (attributes & 0x2) {
        result |= FILE_ATTRIBUTE_HIDDEN;
    }

    if (attributes & 0x4) {
        result |= FILE_ATTRIBUTE_SYSTEM;
    }

    if (attributes & 0x8) {
        result |= FILE_ATTRIBUTE_ARCHIVE;
    }

    if (attributes & 0x10) {
        result |= FILE_ATTRIBUTE_TEMPORARY;
    }

    if (attributes & 0x20) {
        result |= FILE_ATTRIBUTE_NORMAL;
    }

    if (attributes & 0x40) {
        result |= FILE_ATTRIBUTE_DIRECTORY;
    }

    return result;
}

// OFFSET: 0x007ACF10
int32_t System_File::FromNativeAttributes(int32_t attributes) {
    int32_t result = (attributes & FILE_ATTRIBUTE_READONLY) != 0 ? 0x1 : 0;

    if (attributes & FILE_ATTRIBUTE_HIDDEN) {
        result |= 0x2;
    }

    if (attributes & FILE_ATTRIBUTE_SYSTEM) {
        result |= 0x4;
    }

    if (attributes & FILE_ATTRIBUTE_ARCHIVE) {
        result |= 0x8;
    }

    if (attributes & FILE_ATTRIBUTE_TEMPORARY) {
        result |= 0x10;
    }

    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        result |= 0x40;
    }

    if ((attributes & FILE_ATTRIBUTE_NORMAL) || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        result |= 0x20;
    }

    return result;
}

// OFFSET: 0x007B4B00/0x1404AA650
bool System_File::ReadP(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    OVERLAPPED overlapped;
    overlapped.Offset = static_cast<DWORD>(parms->m_pos);
    overlapped.OffsetHigh = static_cast<DWORD>(parms->m_pos >> 32);
    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.hEvent = NULL;

    return read_func(parms->m_stream, parms->m_buffer, &overlapped, &parms->m_amount);
}

// OFFSET: 0x007B4B90/0x1404AA6C0
bool System_File::WriteP(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    OVERLAPPED overlapped;
    overlapped.Offset = static_cast<DWORD>(parms->m_pos);
    overlapped.OffsetHigh = static_cast<DWORD>(parms->m_pos >> 32);
    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.hEvent = NULL;

    return write_func(parms->m_stream, parms->m_buffer, &overlapped, &parms->m_amount);
}

// PROVEN: 336cd3c4 IDENTICAL/DIFFERENT
// OFFSET: 0x007BC640/0x1404B46A0
bool System_File::Close(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    Blizzard::File::StreamRecord* stream = parms->m_stream;

    if (stream->m_fileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(stream->m_fileHandle);
    }

    SMemFree(stream, __FILE__, __LINE__, 0);

    return true;
}

// OFFSET: 0x007BC6C0/0x1404B4730
bool System_File::Stacked::GetRootChars(System_File::Stacked::FileParms* parms) {
    const char* path = parms->m_path;
    Blizzard::Util::Buffer<256, char> buffer((path ? static_cast<uint32_t>(strlen(path)) : 0) + 1);

    if (buffer.m_count < 2) {
        parms->m_pos = 0;
        return true;
    }

    Blizzard::String::MakeUnivPath(path, buffer.m_data, buffer.m_count);
    char* univ = buffer.m_data;

    if (univ[1] == ':') {
        parms->m_pos = (univ[2] == '/') + 2;
    } else if (univ[0] == '/' && univ[1] == '/') {
        const char* separator = univ + 1;
        int32_t count = 0;

        for (;;) {
            separator = strchr(separator + 1, '/');

            if (!separator) {
                parms->m_pos = buffer.m_count;
                break;
            }

            if (++count >= 2) {
                parms->m_pos = separator - univ + 1;
                break;
            }
        }
    } else {
        parms->m_pos = 0;
    }

    return true;
}

// PROVEN: f099edff IDENTICAL/IDENTICAL
// OFFSET: 0x007BFF70/0x1404B93D0
bool System_File::GetRootChars(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::GetRootChars(parms);
}

// OFFSET: 0x007C0030/0x1404B94B0
void System_File::GetFileInfoByFile(Blizzard::File::StreamRecord* file) {
    if (file->m_infoValid && !(file->m_mode & 0x8)) {
        return;
    }

    file->m_infoValid = false;
    DWORD sizeHigh;
    DWORD sizeLow = GetFileSize(file->m_fileHandle, &sizeHigh);

    if (sizeLow == INVALID_FILE_SIZE && GetLastError()) {
        return;
    }

    if (!file->m_streaming || !file->m_streaming->m_unit) {
        file->m_info.m_size = (static_cast<int64_t>(sizeHigh) << 32) | sizeLow;
    }

    DWORD compressedHigh;
    DWORD compressedLow;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(file->m_info.m_path);
        compressedLow = GetCompressedFileSizeW(reinterpret_cast<LPCWSTR>(name.m_data), &compressedHigh);
    }

    if (compressedLow == INVALID_FILE_SIZE && GetLastError()) {
        return;
    }

    file->m_info.m_compressedSize = (static_cast<int64_t>(compressedHigh) << 32) | compressedLow;

    WIN32_FILE_ATTRIBUTE_DATA data;
    BOOL found;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(file->m_pathname);
        found = GetFileAttributesExW(reinterpret_cast<LPCWSTR>(name.m_data), GetFileExInfoStandard, &data);
    }

    if (found) {
        file->m_info.m_createTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftCreationTime.dwHighDateTime) << 32) | data.ftCreationTime.dwLowDateTime);
        file->m_info.m_modTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftLastWriteTime.dwHighDateTime) << 32) | data.ftLastWriteTime.dwLowDateTime);
        file->m_info.m_accessTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftLastAccessTime.dwHighDateTime) << 32) | data.ftLastAccessTime.dwLowDateTime);
        int32_t attributes = FromNativeAttributes(data.dwFileAttributes);
        file->m_info.m_attributes = attributes;
        file->m_info.m_regularFile = (attributes & 0x20) != 0;

        if (data.dwFileAttributes == INVALID_FILE_ATTRIBUTES) {
            file->m_info.m_existence = 0;
        } else {
            file->m_info.m_existence = ((attributes & 0x40) != 0) + 1;
        }
    }

    file->m_infoValid = true;
}

// OFFSET: 0x007C0200/0x1404B98A0
static int32_t GetPathState(const char* path) {
    DWORD attributes;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(path);
        attributes = GetFileAttributesW(reinterpret_cast<LPCWSTR>(name.m_data));
    }

    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return 0;
    }

    return ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) + 1;
}

// OFFSET: 0x007C0260/0x1404B9970
bool System_File::Stacked::Exists(System_File::Stacked::FileParms* parms) {
    char path[1024];
    Blizzard::String::MakeBackslashPath(parms->m_path, path, sizeof(path));
    parms->m_info->m_existence = GetPathState(path);

    return true;
}

// PROVEN: 09004686 DIFFERENT/IDENTICAL
// OFFSET: 0x007C4020/0x1404BE8A0
bool System_File::Exists(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::Exists(parms);
}

// OFFSET: 0x007C02D0/0x1404B99F0
bool System_File::Stacked::MakeAbsolutePath(System_File::Stacked::FileParms* parms) {
    wchar_t buffer[1024];

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
        _wfullpath(buffer, reinterpret_cast<const wchar_t*>(name.m_data), 1024);
    }

    Blizzard::Unicode::ConvertUTF16to8(parms->m_cwdbuf, parms->m_cwdbuflen, reinterpret_cast<const uint16_t*>(buffer), -1, NULL, NULL);
    Blizzard::String::MakeUnivPath(parms->m_cwdbuf, parms->m_cwdbuf, parms->m_cwdbuflen);

    const char* path = parms->m_path;

    if (path[(path ? strlen(path) : 0) - 1] == '/') {
        Blizzard::String::ForceTrailingSeparator(parms->m_cwdbuf, parms->m_cwdbuflen, '/');
    }

    return true;
}

// PROVEN: c7792b6e IDENTICAL/IDENTICAL
// OFFSET: 0x007C4040/0x1404BE8B0
bool System_File::MakeAbsolutePath(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::MakeAbsolutePath(parms);
}

// OFFSET: 0x007C0450/0x1404B9CE0
bool System_File::Stacked::Move(System_File::Stacked::FileParms* parms) {
    char src[1024];
    char dst[1024];
    Blizzard::String::MakeBackslashPath(parms->m_path, src, sizeof(src));
    Blizzard::String::MakeBackslashPath(parms->m_newpath, dst, sizeof(dst));

    Blizzard::StringConversion_CodepageToUTF8::ToNative newName(dst);
    Blizzard::StringConversion_CodepageToUTF8::ToNative oldName(src);

    return MoveFileW(reinterpret_cast<LPCWSTR>(oldName.m_data), reinterpret_cast<LPCWSTR>(newName.m_data)) != 0;
}

// PROVEN: b1c28d38 IDENTICAL/IDENTICAL
// OFFSET: 0x007C4060/0x1404BE8C0
bool System_File::Move(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::Move(parms);
}

// OFFSET: 0x007C0570/0x1404B9EF0
bool System_File::Stacked::SupportsSparse(System_File::Stacked::FileParms* parms) {
    wchar_t volumeName[MAX_PATH];
    wchar_t fileSystemName[MAX_PATH];
    DWORD serialNumber;
    DWORD maxComponentLength;
    DWORD flags;
    BOOL result;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
        result = GetVolumeInformationW(reinterpret_cast<LPCWSTR>(name.m_data), volumeName, MAX_PATH, &serialNumber, &maxComponentLength, &flags, fileSystemName, MAX_PATH);
    }

    parms->m_supportsSparse = (flags & FILE_SUPPORTS_SPARSE_FILES) != 0;

    return result != 0;
}

// PROVEN: 711edcb5 IDENTICAL/IDENTICAL
// OFFSET: 0x007C4000/0x1404BE890
bool System_File::SupportsSparse(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SupportsSparse(parms);
}

// OFFSET: 0x007C0600/0x1404BA010
bool System_File::Stacked::Delete(System_File::Stacked::FileParms* parms) {
    char path[1024];
    Blizzard::String::MakeBackslashPath(parms->m_path, path, sizeof(path));

    Blizzard::StringConversion_CodepageToUTF8::ToNative name(path);

    return DeleteFileW(reinterpret_cast<LPCWSTR>(name.m_data)) != 0;
}

// PROVEN: f226fdf0 IDENTICAL/DIFFERENT
// OFFSET: 0x007C4080/0x1404BE8D0
bool System_File::Delete(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::Delete(parms);
}

// OFFSET: 0x007ACF60/0x1404A2B30
bool System_File::Stacked::SetSparse(System_File::Stacked::FileParms* parms) {
    DWORD bytesReturned = 0;
    LARGE_INTEGER size;

    if (!GetFileSizeEx(parms->m_stream->m_fileHandle, &size)) {
        return false;
    }

    BOOL result = DeviceIoControl(parms->m_stream->m_fileHandle, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &bytesReturned, NULL);

    if (parms->m_zeroFillAll && result) {
        FILE_ZERO_DATA_INFORMATION zero;
        zero.FileOffset.QuadPart = 0;
        zero.BeyondFinalZero = size;
        result = DeviceIoControl(parms->m_stream->m_fileHandle, FSCTL_SET_ZERO_DATA, &zero, sizeof(zero), NULL, 0, &bytesReturned, NULL);
    }

    return result != 0;
}

// PROVEN: 29aa2632 IDENTICAL/IDENTICAL
// OFFSET: 0x007B4B50/0x1404AA6A0
bool System_File::SetSparse(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SetSparse(parms);
}

// OFFSET: 0x007AD000/0x1404A2C00
bool System_File::Stacked::ZeroRange(System_File::Stacked::FileParms* parms) {
    DWORD bytesReturned = 0;
    BY_HANDLE_FILE_INFORMATION info;

    if (!GetFileInformationByHandle(parms->m_stream->m_fileHandle, &info) || !(info.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)) {
        return false;
    }

    FILE_ZERO_DATA_INFORMATION zero;
    zero.FileOffset.QuadPart = parms->m_pos;
    zero.BeyondFinalZero.QuadPart = parms->m_pos + parms->m_length;

    return DeviceIoControl(parms->m_stream->m_fileHandle, FSCTL_SET_ZERO_DATA, &zero, sizeof(zero), NULL, 0, &bytesReturned, NULL) != 0;
}

// PROVEN: 70bce816 IDENTICAL/IDENTICAL
// OFFSET: 0x007B4B70/0x1404AA6B0
bool System_File::ZeroRange(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::ZeroRange(parms);
}

// OFFSET: 0x007E4960/0x1404ED6D0
bool System_File::read_overlapped(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    if (file->m_fileHandle == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(8);
        Blizzard::File::AddToLastErrorStack(8, Blizzard::String::QuickFormat<1024>("Win32 Execute - %s", file->m_pathname).m_buffer, 1);
        return false;
    }

    int32_t requested = *count;

    if (!requested) {
        return true;
    }

    DWORD bytesRead;
    BOOL result = ReadFile(file->m_fileHandle, buffer, requested, &bytesRead, overlapped);
    *count = bytesRead;

    if (result) {
        return true;
    }

    LARGE_INTEGER size;

    if (GetLastError() == ERROR_HANDLE_EOF && GetFileSizeEx(file->m_fileHandle, &size) && ((static_cast<int64_t>(overlapped->OffsetHigh) << 32) | overlapped->Offset) <= size.QuadPart) {
        Blizzard::File::SetLastError(7);
        Blizzard::File::AddToLastErrorStack(7, Blizzard::String::QuickFormat<1024>("Win32 Read - End of File - %s", file->m_pathname).m_buffer, 1);
        return true;
    }

    Blizzard::File::SetLastError(5);
    Blizzard::File::AddToLastErrorStack(5, Blizzard::String::QuickFormat<1024>("Win32 Read - %s %u %u", file->m_pathname, requested, bytesRead).m_buffer, 1);

    return false;
}

// OFFSET: 0x007E4A90/0x1404ED840
bool System_File::write_overlapped(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    if (file->m_fileHandle == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    if (!*count) {
        return true;
    }

    file->m_infoValid = false;
    DWORD bytesWritten;
    BOOL result = WriteFile(file->m_fileHandle, buffer, *count, &bytesWritten, overlapped);
    *count = bytesWritten;

    if (result) {
        return true;
    }

    Blizzard::File::SetLastError(6);
    Blizzard::File::AddToLastErrorStack(6, Blizzard::String::QuickFormat<1024>("Win32 Write - %s", file->m_pathname).m_buffer, 1);

    return false;
}

// OFFSET: 0x007E4B30/0x1404EB6A0
bool System_File::ToCreateFlags(uint32_t flags, DWORD* sharemode, DWORD* desiredaccess, DWORD* creationdisposition, DWORD* flagsandattributes) {
    *sharemode = (flags >> 2) & 3;
    *desiredaccess = ((flags * 4) | (flags & 2)) << 29;
    *creationdisposition = 0;

    if ((flags & 0x100) || (flags & 0x400)) {
        *creationdisposition = (flags & 0x800) ? CREATE_NEW : CREATE_ALWAYS;
    } else if (flags & 0x800) {
        *creationdisposition = CREATE_NEW;
    } else {
        *creationdisposition = OPEN_ALWAYS - ((flags & 0x1000) != 0);
    }

    *flagsandattributes = (2 * ((flags & 0x80) | 0x40)) | ((flags & 0x40) ? (FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING) : 0);

    return true;
}

// OFFSET: 0x007C01B0/0x1404B97E0
bool System_File::Stacked::SetWorkingDirectory(System_File::Stacked::FileParms* parms) {
    Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);

    return SetCurrentDirectoryW(reinterpret_cast<LPCWSTR>(name.m_data)) != 0;
}

// OFFSET: 0x007E99F0/0x1404F5330
bool System_File::SetWorkingDirectory(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    if (!parms->m_path) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    return System_File::Stacked::SetWorkingDirectory(parms);
}

// OFFSET: 0x007E9A20/0x1404F5360
bool System_File::Create(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    Blizzard::File::SetLastError(9);

    return false;
}

// PROVEN: 9ad2d1a6 IDENTICAL/DIFFERENT
// OFFSET: 0x007BC670/0x1404B46E0
bool System_File::Stacked::GetWorkingDirectory(System_File::Stacked::FileParms* parms) {
    wchar_t buffer[1024];
    GetCurrentDirectoryW(1024, buffer);
    Blizzard::Unicode::ConvertUTF16to8(parms->m_cwdbuf, parms->m_cwdbuflen, reinterpret_cast<const uint16_t*>(buffer), -1, NULL, NULL);

    return true;
}

// OFFSET: 0x007E9A40/0x1404F5380
bool System_File::GetWorkingDirectory(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    if (!parms->m_cwdbuf || parms->m_cwdbuflen <= 0) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    *parms->m_cwdbuf = '\0';

    return System_File::Stacked::GetWorkingDirectory(parms);
}

// OFFSET: 0x007E4BE0/0x1404ED910
bool System_File::Stacked::ProcessDirFast(System_File::Stacked::FileParms* parms) {
    void* cookie = parms->m_cookie;
    const char* path = parms->m_path;
    Blizzard::File::ProcessDirCallback callback = parms->m_dircallback;

    char pattern[512];
    Blizzard::String::Format(pattern, sizeof(pattern), "%s", path);
    Blizzard::String::ForceTrailingSeparator(pattern, sizeof(pattern), '\0');
    Blizzard::String::Format(pattern, sizeof(pattern), "%s*", pattern);

    WIN32_FIND_DATAW findData;
    HANDLE find;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(pattern);
        find = FindFirstFileW(reinterpret_cast<LPCWSTR>(name.m_data), &findData);
    }

    if (find == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    Blizzard::File::ProcessDirParms dirParms;
    dirParms.m_pathRoot = path;
    dirParms.m_cookie = cookie;

    do {
        if (findData.cFileName[0] != '.' || (findData.cFileName[1] && (findData.cFileName[1] != '.' || findData.cFileName[2]))) {
            bool isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            char filename[1024];
            Blizzard::Unicode::ConvertUTF16to8(filename, sizeof(filename), reinterpret_cast<const uint16_t*>(findData.cFileName), -1, NULL, NULL);
            dirParms.m_filename = filename;
            dirParms.m_isDir = isDir;

            if (!callback(dirParms)) {
                FindClose(find);
                return false;
            }
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);

    return true;
}

// PROVEN: e4d61db0 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9A80/0x1404F53D0
bool System_File::ProcessDirFast(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::ProcessDirFast(parms);
}

// OFFSET: 0x007E9AA0/0x1404F53E0
bool System_File::Flush(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    if (parms->m_stream->m_fileHandle == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    return FlushFileBuffers(parms->m_stream->m_fileHandle) != 0;
}

// OFFSET: 0x007E4DC0/0x1404EDB90
bool System_File::Stacked::GetFileInfo(System_File::Stacked::FileParms* parms) {
    if (parms->m_path) {
        WIN32_FILE_ATTRIBUTE_DATA data;
        BOOL found;

        {
            Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
            found = GetFileAttributesExW(reinterpret_cast<LPCWSTR>(name.m_data), GetFileExInfoStandard, &data);
        }

        if (!found) {
            Blizzard::File::SetLastError(8);
            return false;
        }

        parms->m_info->m_size = (static_cast<int64_t>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
        parms->m_info->m_createTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftCreationTime.dwHighDateTime) << 32) | data.ftCreationTime.dwLowDateTime);
        parms->m_info->m_modTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftLastWriteTime.dwHighDateTime) << 32) | data.ftLastWriteTime.dwLowDateTime);
        parms->m_info->m_accessTime = Blizzard::Time::FromFileTime((static_cast<uint64_t>(data.ftLastAccessTime.dwHighDateTime) << 32) | data.ftLastAccessTime.dwLowDateTime);
        parms->m_info->m_path = NULL;
        parms->m_info->m_attributes = FromNativeAttributes(data.dwFileAttributes);
        parms->m_info->m_regularFile = (parms->m_info->m_attributes & 0x20) != 0;

        if (data.dwFileAttributes == INVALID_FILE_ATTRIBUTES) {
            parms->m_info->m_existence = 0;
        } else {
            parms->m_info->m_existence = ((parms->m_info->m_attributes & 0x40) != 0) + 1;
        }

        DWORD compressedHigh;
        DWORD compressedLow;

        {
            Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
            compressedLow = GetCompressedFileSizeW(reinterpret_cast<LPCWSTR>(name.m_data), &compressedHigh);
        }

        if (compressedLow == INVALID_FILE_SIZE && GetLastError()) {
            return false;
        }

        parms->m_info->m_compressedSize = (static_cast<int64_t>(compressedHigh) << 32) | compressedLow;

        return true;
    }

    if (parms->m_stream) {
        GetFileInfoByFile(parms->m_stream);
        Blizzard::File::StreamRecord* stream = parms->m_stream;
        parms->m_info = &stream->m_info;

        if (!stream->m_info.m_path) {
            stream->m_info.m_path = stream->m_pathname;
        }

        return parms->m_stream->m_infoValid;
    }

    Blizzard::File::SetLastError(8);

    return false;
}

// PROVEN: 5ba0aeed IDENTICAL/IDENTICAL
// OFFSET: 0x007E9AE0/0x1404F5420
bool System_File::GetFileInfo(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::GetFileInfo(parms);
}

// OFFSET: 0x007E4F90/0x1404EDEF0
bool System_File::Stacked::GetFreeSpace(System_File::Stacked::FileParms* parms) {
    const char* path = parms->m_path;

    if (path && *path) {
        char directory[MAX_PATH];
        Blizzard::String::Copy(directory, path, std::min<int32_t>(static_cast<int32_t>(Blizzard::String::FindFilename(path) - path + 1), MAX_PATH));

        ULARGE_INTEGER freeBytes;
        ULARGE_INTEGER totalBytes;
        freeBytes.QuadPart = 0;
        totalBytes.QuadPart = 0;

        wchar_t shortPath[MAX_PATH];
        DWORD shortLength;

        {
            Blizzard::StringConversion_CodepageToUTF8::ToNative name(directory);
            shortLength = GetShortPathNameW(reinterpret_cast<LPCWSTR>(name.m_data), shortPath, MAX_PATH);
        }

        if (shortLength && shortLength < MAX_PATH) {
            if (!GetDiskFreeSpaceExW(shortPath, &freeBytes, &totalBytes, NULL)) {
                Blizzard::File::SetLastError(8);
                return false;
            }

            parms->m_pos = freeBytes.QuadPart;
            return true;
        }

        BOOL result;

        {
            Blizzard::StringConversion_CodepageToUTF8::ToNative name(directory);
            result = GetDiskFreeSpaceExW(reinterpret_cast<LPCWSTR>(name.m_data), &freeBytes, &totalBytes, NULL);
        }

        if (result) {
            parms->m_pos = freeBytes.QuadPart;
            return true;
        }
    }

    Blizzard::File::SetLastError(8);

    return false;
}

// PROVEN: 47fc02f4 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9B00/0x1404F5430
bool System_File::GetFreeSpace(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::GetFreeSpace(parms);
}

// OFFSET: 0x007E50F0/0x1404EE1D0
bool System_File::Stacked::GetPos(System_File::Stacked::FileParms* parms) {
    Blizzard::File::StreamRecord* stream = parms->m_stream;
    LONG high;
    DWORD low;

    if (stream->m_fileHandle == INVALID_HANDLE_VALUE || (high = 0, low = SetFilePointer(stream->m_fileHandle, 0, &high, FILE_CURRENT), low == INVALID_SET_FILE_POINTER && GetLastError())) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    parms->m_pos = (static_cast<int64_t>(high) << 32) | low;

    return true;
}

// PROVEN: 256f0de7 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9B20/0x1404F5440
bool System_File::GetPos(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::GetPos(parms);
}

// OFFSET: 0x007E5170/0x1404EE280
bool System_File::Stacked::IsAbsolutePath(System_File::Stacked::FileParms* parms) {
    const char* path = parms->m_path;

    if (((path[0] == '\\' || path[0] == '/') && (path[1] == '\\' || path[1] == '/') && path[2]) || (isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/') && path[3])) {
        return true;
    }

    Blizzard::File::SetLastError(8);

    return false;
}

// PROVEN: ced7a509 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9B40/0x1404F5450
bool System_File::IsAbsolutePath(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::IsAbsolutePath(parms);
}

// OFFSET: 0x007E51E0/0x1404EE2F0
bool System_File::Stacked::IsReadOnly(System_File::Stacked::FileParms* parms) {
    DWORD attributes;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
        attributes = GetFileAttributesW(reinterpret_cast<LPCWSTR>(name.m_data));
    }

    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return attributes & FILE_ATTRIBUTE_READONLY;
    }

    Blizzard::File::SetLastError(8);

    return false;
}

// PROVEN: ffed721d IDENTICAL/DIFFERENT
// OFFSET: 0x007E9B60/0x1404F5460
bool System_File::IsReadOnly(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::IsReadOnly(parms);
}

// OFFSET: 0x007C03E0/0x1404B9BE0
static bool CreateOneDirectory(const char* path, SECURITY_ATTRIBUTES* attributes) {
    BOOL created;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(path);
        created = CreateDirectoryW(reinterpret_cast<LPCWSTR>(name.m_data), attributes);
    }

    if (created) {
        return true;
    }

    DWORD error = GetLastError();
    return error == ERROR_ALREADY_EXISTS || error == ERROR_ACCESS_DENIED;
}

// OFFSET: 0x007E5250/0x1404EE3E0
bool System_File::Stacked::CreateDirectory(System_File::Stacked::FileParms* parms) {
    bool recursive = parms->m_recursive;
    const char* path = parms->m_path;
    bool grantEveryone = parms->m_grantEveryone;

    if (!path) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    Blizzard::Util::Buffer<MAX_PATH, char> buffer(static_cast<uint32_t>(strlen(path)) + 1);
    Blizzard::String::MakeBackslashPath(path, buffer.m_data, buffer.m_count);

    SECURITY_ATTRIBUTES attributes;
    memset(&attributes, 0, sizeof(attributes));
    attributes.nLength = sizeof(attributes);
    attributes.bInheritHandle = FALSE;

    if (grantEveryone) {
        ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;OICI;GA;;;BU)(A;OICI;GA;;;BA)(A;OICI;GA;;;CO)", SDDL_REVISION_1, &attributes.lpSecurityDescriptor, NULL);
    }

    if (!recursive) {
        return CreateOneDirectory(buffer.m_data, grantEveryone ? &attributes : NULL);
    }

    Blizzard::Util::Buffer<MAX_PATH, char> partial(buffer.m_count);
    memmove(partial.m_data, buffer.m_data, buffer.m_count);

    bool result = true;
    bool found = false;

    while (partial.m_data) {
        char* last = NULL;

        for (char* curr = partial.m_data; curr != partial.m_data + partial.m_count && *curr; curr++) {
            if (*curr == '/' || *curr == '\\') {
                last = curr;
            }
        }

        if (!last) {
            break;
        }

        *last = 0;
        int32_t state = GetPathState(partial.m_data);

        if (state == 2) {
            found = true;
            break;
        }

        if (state == 1) {
            result = false;
            found = true;
            break;
        }
    }

    if (!found) {
        if (Blizzard::File::IsAbsolutePath(buffer.m_data)) {
            result = false;
        } else {
            result = CreateOneDirectory(partial.m_data, grantEveryone ? &attributes : NULL);
        }
    }

    char separator = '\\';

    for (const char* curr = buffer.m_data; curr && *curr; curr++) {
        if (*curr == '/' || *curr == '\\') {
            separator = *curr;
            break;
        }
    }

    if (result) {
        for (;;) {
            uint32_t length = partial.m_data ? static_cast<uint32_t>(strlen(partial.m_data)) : 0;

            if (length >= buffer.m_count - 1) {
                break;
            }

            partial.m_data[length] = separator;
            result = CreateOneDirectory(partial.m_data, grantEveryone ? &attributes : NULL);

            if (!result) {
                break;
            }
        }
    }

    if (!result) {
        Blizzard::File::SetLastError(8);
    }

    return result;
}

// PROVEN: c09a0209 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9B80/0x1404F5470
bool System_File::CreateDirectory(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::CreateDirectory(parms);
}

// OFFSET: 0x007E9BA0/0x1404F5480
bool System_File::Copy(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    bool overwrite = parms->m_overwrite;
    char src[1024];
    char dst[1024];
    Blizzard::String::MakeBackslashPath(parms->m_path, src, sizeof(src));
    Blizzard::String::MakeBackslashPath(parms->m_newpath, dst, sizeof(dst));

    Blizzard::File::StreamRecord* input;

    if (!Blizzard::File::Open(src, 0x1005, input)) {
        Blizzard::File::SetLastError(2);
        return false;
    }

    Blizzard::File::StreamRecord* output;

    if (!Blizzard::File::Open(dst, (overwrite ? 0x400 : 0xC00) | 0x2, output)) {
        Blizzard::File::Close(input);
        Blizzard::File::SetLastError(4);
        return false;
    }

    int64_t size = Blizzard::File::GetFileInfo(input)->m_size;
    int64_t offset = 0;
    bool result = true;
    void* buffer = Blizzard::Memory::Allocate(size >= 0xA00000 ? 0xA00000 : static_cast<uint32_t>(size));

    if (size > 0) {
        for (;;) {
            int32_t count = size - offset >= 0xA00000 ? 0xA00000 : static_cast<int32_t>(size - offset);

            if (!Blizzard::File::Read(input, buffer, offset, &count, 0)) {
                break;
            }

            if (!Blizzard::File::Write(output, buffer, offset, &count, 0)) {
                break;
            }

            offset += count;

            if (offset >= size) {
                goto done;
            }
        }

        result = false;
    }

done:
    Blizzard::Memory::Free(buffer);
    Blizzard::File::Close(input);
    Blizzard::File::Close(output);

    if (!result) {
        Blizzard::File::Delete(dst);
    }

    return result;
}

// OFFSET: 0x007E5570/0x1404EE760
bool System_File::Stacked::Open(System_File::Stacked::FileParms* parms) {
    int32_t mode = parms->m_mode;
    char path[1024];
    Blizzard::String::MakeBackslashPath(parms->m_path, path, sizeof(path));

    DWORD sharemode;
    DWORD desiredaccess;
    DWORD creationdisposition;
    DWORD flagsandattributes;

    if (!ToCreateFlags(mode, &sharemode, &desiredaccess, &creationdisposition, &flagsandattributes)) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    HANDLE handle;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(path);
        handle = CreateFileW(reinterpret_cast<LPCWSTR>(name.m_data), desiredaccess, sharemode, NULL, creationdisposition, flagsandattributes, NULL);
    }

    if (handle == INVALID_HANDLE_VALUE) {
        Blizzard::File::AddToLastErrorStack(4, Blizzard::String::QuickFormat<1024>("Win32 Open - %s, accessMode:0x%x, shareMode:0x%x, createMode:0x%x, flags:0x%x, lastError:%d", path, desiredaccess, sharemode, creationdisposition, flagsandattributes, GetLastError()).m_buffer, 1);
        Blizzard::File::SetLastError(4);
        return false;
    }

    Blizzard::File::StreamRecord* stream = parms->m_stream;

    if (!stream) {
        uint32_t length = static_cast<uint32_t>(strlen(path));
        stream = static_cast<Blizzard::File::StreamRecord*>(SMemAlloc(sizeof(Blizzard::File::StreamRecord) + length + 1, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
        stream->m_pathname = reinterpret_cast<char*>(stream + 1);
        Blizzard::String::Copy(stream->m_pathname, path, length + 1);
    }

    stream->m_fileHandle = handle;
    stream->m_mode = mode;
    stream->m_infoValid = false;
    stream->m_info.m_path = stream->m_pathname;
    GetFileInfoByFile(stream);

    if (mode & 0x200) {
        Blizzard::File::SetPos(stream, stream->m_info.m_size, 0);
    }

    parms->m_stream = stream;

    return true;
}

// PROVEN: 3a7091b8 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9DE0/0x1404F5870
bool System_File::Open(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::Open(parms);
}

// PROVEN: 2b74ce0b IDENTICAL/DIFFERENT
// OFFSET: 0x007E9E00/0x1404F5880
bool System_File::Read(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return read_overlapped(parms->m_stream, parms->m_buffer, NULL, &parms->m_amount);
}

// OFFSET: 0x007E5730/0x1404EEA70
bool System_File::Stacked::RemoveDirectory(System_File::Stacked::FileParms* parms) {
    const char* path = parms->m_path;

    if (parms->m_recursive) {
        return Blizzard::File::RemoveDirectoryAndContents(path, false);
    }

    if (!path) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    Blizzard::Util::Buffer<MAX_PATH, char> buffer(static_cast<uint32_t>(strlen(path)) + 1);
    Blizzard::String::MakeBackslashPath(path, buffer.m_data, buffer.m_count);

    Blizzard::StringConversion_CodepageToUTF8::ToNative name(buffer.m_data);

    return RemoveDirectoryW(reinterpret_cast<LPCWSTR>(name.m_data)) != 0;
}

// PROVEN: c4253d44 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9E20/0x1404F58A0
bool System_File::RemoveDirectory(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::RemoveDirectory(parms);
}

// OFFSET: 0x007E5850/0x1404EEC20
bool System_File::Stacked::SetCacheMode(System_File::Stacked::FileParms* parms) {
    Blizzard::File::StreamRecord* stream = parms->m_stream;
    stream->m_mode = parms->m_mode | (stream->m_mode & ~0x40);

    DWORD sharemode;
    DWORD desiredaccess;
    DWORD creationdisposition;
    DWORD flagsandattributes;

    if (!ToCreateFlags(stream->m_mode, &sharemode, &desiredaccess, &creationdisposition, &flagsandattributes)) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    CloseHandle(stream->m_fileHandle);

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative name(stream->m_info.m_path);
        stream->m_fileHandle = CreateFileW(reinterpret_cast<LPCWSTR>(name.m_data), desiredaccess, sharemode, NULL, creationdisposition, flagsandattributes, NULL);
    }

    if (stream->m_fileHandle == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(4);
        Blizzard::File::AddToLastErrorStack(4, "Win32 Close", 1);
        return false;
    }

    return true;
}

// PROVEN: 0c2c0359 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9E40/0x1404F58B0
bool System_File::SetCacheMode(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SetCacheMode(parms);
}

// OFFSET: 0x007E5930/0x1404EEE20
bool System_File::Stacked::SetEOF(System_File::Stacked::FileParms* parms) {
    int64_t pos = parms->m_pos;
    Blizzard::File::StreamRecord* stream = parms->m_stream;
    int32_t whence = parms->m_posMode;
    int64_t offset;

    if (!Blizzard::File::GetPos(stream, &offset) || !Blizzard::File::SetPos(stream, pos, whence)) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    BOOL result = SetEndOfFile(stream->m_fileHandle);

    if (!result) {
        DWORD error = GetLastError();

        if (error == ERROR_HANDLE_DISK_FULL || error == ERROR_DISK_FULL) {
            Blizzard::File::SetLastError(11);
        } else {
            Blizzard::File::SetLastError(8);
        }
    }

    Blizzard::File::SetPos(stream, offset, 0);
    stream->m_infoValid = false;

    return result != 0;
}

// PROVEN: 1fe0fe85 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9E60/0x1404F58C0
bool System_File::SetEOF(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SetEOF(parms);
}

// OFFSET: 0x007E59F0/0x1404EEF90
bool System_File::Stacked::SetAttributes(System_File::Stacked::FileParms* parms) {
    if (parms->m_has.m_all & 0x10) {
        Blizzard::Time::Timestamp time = parms->m_info->m_modTime;
        int64_t ticks = time / 100 + 125911584000000000;

        if (time < 0 && time % 100) {
            ticks = time / 100 + 125911583999999999;
        }

        FILETIME fileTime;
        fileTime.dwLowDateTime = static_cast<DWORD>(ticks);
        fileTime.dwHighDateTime = static_cast<DWORD>(ticks >> 32);

        if (!SetFileTime(parms->m_stream->m_fileHandle, NULL, NULL, &fileTime)) {
            Blizzard::File::SetLastError(8);
            return false;
        }

        parms->m_stream->m_info.m_modTime = time;
        parms->m_has.m_all &= ~0x10u;
    }

    if (parms->m_has.m_all & 0x4) {
        int32_t attributes = ToNativeAttributes(parms->m_info->m_attributes);
        BOOL result;

        {
            Blizzard::StringConversion_CodepageToUTF8::ToNative name(parms->m_path);
            result = SetFileAttributesW(reinterpret_cast<LPCWSTR>(name.m_data), attributes);
        }

        if (!result) {
            Blizzard::File::SetLastError(8);
            return false;
        }

        parms->m_has.m_all &= ~0x4u;
    }

    return parms->m_has.m_all == 0;
}

// PROVEN: f4a53b78 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9E80/0x1404F58D0
bool System_File::SetAttributes(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SetAttributes(parms);
}

// OFFSET: 0x007E5AF0/0x1404EF1A0
bool System_File::Stacked::SetPos(System_File::Stacked::FileParms* parms) {
    Blizzard::File::StreamRecord* stream = parms->m_stream;
    LONG low = static_cast<LONG>(parms->m_pos);
    DWORD method = parms->m_posMode;

    if (stream->m_fileHandle == INVALID_HANDLE_VALUE) {
        Blizzard::File::SetLastError(8);
        return false;
    }

    LONG high = static_cast<LONG>(parms->m_pos >> 32);

    if (method) {
        method = (method != 1) + 1;
    }

    DWORD result = SetFilePointer(stream->m_fileHandle, low, &high, method);
    DWORD error = GetLastError();

    if (result == INVALID_SET_FILE_POINTER && error) {
        if (error == ERROR_HANDLE_DISK_FULL || error == ERROR_DISK_FULL) {
            Blizzard::File::SetLastError(11);
        } else {
            Blizzard::File::SetLastError(8);
        }

        return false;
    }

    return true;
}

// PROVEN: caf2fc39 IDENTICAL/IDENTICAL
// OFFSET: 0x007E9EA0/0x1404F58E0
bool System_File::SetPos(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return System_File::Stacked::SetPos(parms);
}

// PROVEN: 7d1f9700 IDENTICAL/DIFFERENT
// OFFSET: 0x007E9EC0/0x1404F58F0
bool System_File::Write(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return write_overlapped(parms->m_stream, parms->m_buffer, NULL, &parms->m_amount);
}

// OFFSET: 0x007E6110
bool System_File::read_locked(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    Blizzard::Lock::ScopedMutex lock(*s_ioLock);

    if (!Blizzard::File::SetPos(file, (static_cast<int64_t>(overlapped->OffsetHigh) << 32) | overlapped->Offset, 0)) {
        *count = 0;
        Blizzard::File::SetLastError(9);
        return false;
    }

    return read_overlapped(file, buffer, NULL, count);
}

// OFFSET: 0x007E6200
bool System_File::write_locked(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    Blizzard::Lock::ScopedMutex lock(*s_ioLock);

    if (!Blizzard::File::SetPos(file, (static_cast<int64_t>(overlapped->OffsetHigh) << 32) | overlapped->Offset, 0)) {
        *count = 0;
        Blizzard::File::SetLastError(9);
        return false;
    }

    return write_overlapped(file, buffer, NULL, count);
}

// OFFSET: 0x007EA600/0x1404F5B90
void System_File::detect_io_mode() {
    OSVERSIONINFOA info;
    info.dwOSVersionInfoSize = sizeof(info);
    memset(&info.dwMajorVersion, 0, sizeof(info) - sizeof(info.dwOSVersionInfoSize));

    if (GetVersionExA(&info) && info.dwMajorVersion >= 5) {
        read_func = read_overlapped;
        write_func = write_overlapped;
    } else {
        Blizzard::Lock::MutexCreate(s_ioMutex);
        s_ioLock = &s_ioMutex;
        read_func = read_locked;
        write_func = write_locked;
    }
}

// OFFSET: 0x007EC0B0/0x1404F8410
bool System_File::read_first(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    if (!s_read_init) {
        if (s_readInitCount.Increment() == 1) {
            detect_io_mode();
            s_read_init = true;
        } else {
            do {
                Sleep(1);
            } while (!s_read_init);
        }
    }

    return read_func(file, buffer, overlapped, count);
}

// OFFSET: 0x007EC100/0x1404F84A0
bool System_File::write_first(Blizzard::File::StreamRecord* file, void* buffer, OVERLAPPED* overlapped, int32_t* count) {
    if (!s_write_init) {
        if (s_writeInitCount.Increment() == 1) {
            detect_io_mode();
            s_write_init = true;
        } else {
            do {
                Sleep(1);
            } while (!s_write_init);
        }
    }

    return write_func(file, buffer, overlapped, count);
}

// OFFSET: 0x007BD3F0/0x1404B5C40
void System_File::FromNativeName(char* buffer, uint32_t buffersize, const char* str) {
    uint32_t length = str ? static_cast<uint32_t>(strlen(str)) : 0;
    Blizzard::Util::Buffer<300, uint16_t> wide(length + 1);
    wide.m_data[MultiByteToWideChar(CP_ACP, 0, str, length, reinterpret_cast<wchar_t*>(wide.m_data), wide.m_count)] = 0;
    Blizzard::Unicode::ConvertUTF16to8(buffer, buffersize, wide.m_data, -1, NULL, NULL);
}

#endif
