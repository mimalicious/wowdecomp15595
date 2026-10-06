#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: b5de032f IDENTICAL/IDENTICAL
// OFFSET: 0x007A8D40/0x14049EAE0
bool OsCloseFile(HOSFILE file) {
    return Blizzard::File::Close(reinterpret_cast<Blizzard::File::StreamRecord*>(file));
}

// OFFSET: 0x007A8D50/0x14049EAF0
int32_t OsFileExists(const char* fileName) {
    return Blizzard::File::IsFile(fileName) != 0;
}

// PROVEN: c55b2bfa IDENTICAL/DIFFERENT
// OFFSET: 0x007A8D70/0x14049EB10
int32_t OsDirectoryExists(const char* pathName) {
    return Blizzard::File::IsDirectory(pathName) != 0;
}

// OFFSET: 0x007A8D90/0x14049EB30
int32_t OsReadFile(HOSFILE file, void* buffer, uint32_t bytesToRead, uint32_t* bytesRead) {
    if (!buffer) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!bytesRead) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    *bytesRead = bytesToRead;
    return Blizzard::File::Read(reinterpret_cast<Blizzard::File::StreamRecord*>(file), buffer, reinterpret_cast<int32_t*>(bytesRead)) != 0;
}

// OFFSET: 0x007A8DD0/0x14049EB70
int32_t OsWriteFile(HOSFILE file, const void* buffer, uint32_t bytesToWrite, uint32_t* bytesWritten) {
    if (!buffer) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!bytesWritten) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    *bytesWritten = bytesToWrite;
    return Blizzard::File::Write(reinterpret_cast<Blizzard::File::StreamRecord*>(file), buffer, reinterpret_cast<int32_t*>(bytesWritten)) != 0;
}

// PROVEN: 3b7a53d4 IDENTICAL/DIFFERENT
// OFFSET: 0x007A8EA0/0x14049EC40
uint64_t OsGetFileSize(HOSFILE file) {
    Blizzard::File::FileInfo* info = Blizzard::File::GetFileInfo(reinterpret_cast<Blizzard::File::StreamRecord*>(file));
    return !info ? 0xFFFFFFFF : info->m_size;
}

// PROVEN: 1d127c33 IDENTICAL/DIFFERENT
// OFFSET: 0x007A8F20/0x14049ECC0
uint32_t OsGetFileAttributes(const char* fileName) {
    if (!fileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    Blizzard::File::FileInfo info;
    if (!Blizzard::File::GetFileInfo(fileName, &info)) {
        return 0xFFFFFFFF;
    }

    return info.m_attributes;
}

// OFFSET: 0x007A8F60/0x1404A2930
int32_t OsSetFileAttributes(const char* fileName, uint32_t attributes) {
    if (!fileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::SetAttributes(fileName, attributes) != 0;
}

// OFFSET: 0x007A8F90/0x14049ED00
int32_t OsMoveFile(const char* existingFileName, const char* newFileName) {
    if (!existingFileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!newFileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::Move(existingFileName, newFileName) != 0;
}

// OFFSET: 0x007A8FD0/0x14049ED40
int32_t OsCopyFile(const char* existingFileName, const char* newFileName, int32_t failIfExists) {
    if (!existingFileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!newFileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::Copy(existingFileName, newFileName, failIfExists == 0) != 0;
}

// PROVEN: 61dac228 IDENTICAL/DIFFERENT
// OFFSET: 0x007A9010/0x14049ED80
int32_t OsDeleteFile(const char* fileName) {
    if (!fileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::Delete(fileName) != 0;
}

// PROVEN: 61066313 IDENTICAL/DIFFERENT
// OFFSET: 0x007A9040/0x14049EDB0
int32_t OsCreateDirectory(const char* pathName, int32_t recursive) {
    if (!pathName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::CreateDirectory(pathName, recursive != 0) != 0;
}

// OFFSET: 0x007A9080/0x14049EDF0
int32_t OsRemoveDirectory(const char* pathName) {
    if (!pathName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::RemoveDirectory(pathName) != 0;
}

// PROVEN: c1b268a6 IDENTICAL/DIFFERENT
// OFFSET: 0x007A90B0/0x14049EE20
int32_t OsRemoveDirectoryRecurse(const char* pathName, uint32_t flags) {
    if (!pathName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::RemoveDirectoryAndContents(pathName, (flags & 0x1) != 0) != 0;
}

// PROVEN: 697341bb IDENTICAL/DIFFERENT
// OFFSET: 0x007A90F0/0x14049EE50
int32_t OsSetCurrentDirectory(const char* path) {
    if (!path) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::SetWorkingDirectory(path) != 0;
}

// OFFSET: 0x007A9120/0x14049EE80
int32_t OsGetCurrentDirectory(uint32_t bufferSize, char* buffer) {
    if (!buffer) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return Blizzard::File::GetWorkingDirectory(buffer, bufferSize) != 0;
}

// PROVEN: d753dbb6 IDENTICAL/DIFFERENT
// OFFSET: 0x007A9150/0x14049EEC0
bool OsFileList_Internal::Callback(const Blizzard::File::ProcessDirParms& parms) {
    bool result = true;

    OS_FILE_DATA data;
    Blizzard::String::Copy(data.fileName, parms.m_filename, sizeof(data.fileName));

    char path[260];
    Blizzard::String::JoinPath(path, sizeof(path), parms.m_pathRoot, parms.m_filename);

    OsFileList_Internal* list = static_cast<OsFileList_Internal*>(parms.m_cookie);

    if (Blizzard::File::SimpleGlob(parms.m_filename, list->m_pattern)) {
        Blizzard::File::FileInfo info;
        if (Blizzard::File::GetFileInfo(path, &info)) {
            data.size = static_cast<uint32_t>(info.m_size);
            data.flags = 0;

            if (info.m_attributes & 0x40) {
                data.flags = FILE_ATTRIBUTE_DIRECTORY;
            }

            if (info.m_attributes & 0x1) {
                data.flags |= FILE_ATTRIBUTE_READONLY;
            }

            if (info.m_attributes & 0x2) {
                data.flags |= FILE_ATTRIBUTE_HIDDEN;
            }

            if ((list->m_includeHidden || !(info.m_attributes & 0x2)) && list->m_callback(data, list->m_param)) {
                result = false;
            }
        }
    }

    return result;
}

// PROVEN: 6dbab928 IDENTICAL/IDENTICAL
// OFFSET: 0x007A9230/0x14049EFA0
uint32_t GetFileMode(uint32_t desiredAccess, uint32_t shareMode, uint32_t createDisposition) {
    uint32_t mode = 0;

    if (desiredAccess & GENERIC_READ) {
        mode = 0x1;
    }

    if (desiredAccess & GENERIC_WRITE) {
        mode |= 0x2;
    }

    if (shareMode & FILE_SHARE_READ) {
        mode |= 0x4;
    }

    if (shareMode & FILE_SHARE_WRITE) {
        mode |= 0x8;
    }

    switch (createDisposition) {
        case CREATE_NEW:
            mode |= 0xC00;
            break;

        case CREATE_ALWAYS:
            mode |= 0x400;
            break;

        case OPEN_EXISTING:
            mode |= 0x1000;
            break;

        case OPEN_ALWAYS:
            mode |= 0x200;
            break;

        case TRUNCATE_EXISTING:
            mode |= 0x100;
            break;
    }

    return mode;
}

// OFFSET: 0x007A9320/0x14049F070
char* OsFileGetDownloadFolder() {
    // STATIC: %0x00D81108
    static char s_downloadfolder[260];

    if (!s_downloadfolder[0]) {
        Blizzard::File::GetWorkingDirectory(s_downloadfolder, sizeof(s_downloadfolder));
    }

    return s_downloadfolder;
}

// OFFSET: 0x007A9350/0x14049F0B0
HOSFILE OsCreateFile(const char* fileName, uint32_t desiredAccess, uint32_t shareMode, uint32_t createDisposition, uint32_t flagsAndAttributes, uint32_t extendedFileType) {
    if (!fileName || !desiredAccess || createDisposition < CREATE_NEW || createDisposition > TRUNCATE_EXISTING) {
        SErrSetLastError(STORM_ERROR_BAD_ARGUMENT);
        return reinterpret_cast<HOSFILE>(INVALID_HANDLE_VALUE);
    }

    Blizzard::File::StreamRecord* file;
    if (!Blizzard::File::Open(fileName, GetFileMode(desiredAccess, shareMode, createDisposition), file)) {
        return reinterpret_cast<HOSFILE>(INVALID_HANDLE_VALUE);
    }

    if (!(flagsAndAttributes & FILE_ATTRIBUTE_NORMAL)) {
        OsSetFileAttributes(fileName, flagsAndAttributes);
    }

    return reinterpret_cast<HOSFILE>(file);
}

// PROVEN: bddbed91 IDENTICAL/DIFFERENT
// OFFSET: 0x007A93C0/0x14049F130
int32_t OsFileList(const char* dir, const char* pattern, OsFileListCallback callback, void* param, int32_t includeHidden) {
    char relative[260];
    Blizzard::String::Copy(relative, dir, sizeof(relative));

    char* separator = Blizzard::String::FindPathSeparatorReverse(relative, 0x7FFFFFFF);
    if (separator) {
        *separator = '\0';
    }

    char path[260];
    if (!Blizzard::File::MakeAbsolutePath(relative, path, sizeof(path), false)) {
        return 0;
    }

    OsFileList_Internal list;
    list.m_callback = callback;
    list.m_param = param;
    list.m_includeHidden = includeHidden;
    list.m_pattern = pattern;

    return Blizzard::File::ProcessDirFast(path, &list, OsFileList_Internal::Callback, false) != 0;
}

#endif
