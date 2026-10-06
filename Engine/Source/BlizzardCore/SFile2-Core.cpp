#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/BackgroundCache.h"
#include "Engine/Source/BlizzardCore/BGLoading-Core.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileEnumerators.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/RepairCache.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Tracker.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"

// STATIC: %0x00D734A0
static SFileStreamingStatus s_streamingStatus;

// STATIC: %0x00CBEA80
uint32_t SFile::s_locale;

// STATIC: %0x00D737C8
char SFile::m_basePath[260];

// STATIC: %0x00D738CD
bool System_SFile2::s_trial;

// STATIC: %0x00D736C0
char System_SFile2::s_dataPath[260];

// STATIC: %0x00D735B8
char System_SFile2::s_dataPath2[260];

// STATIC: %0x00C0AD34
bool System_SFile2::s_checkDisk = true;

// STATIC: %0x00C0AD38
uint32_t System_SFile2::s_directAccess = 1;

// STATIC: %0x00D734A8
uint32_t System_SFile2::s_useOldFindFile;

// STATIC: %0x00D734B0
char System_SFile2::s_hashedBasePath[260];

// STATIC: %0x00D738CC
bool System_SFile2::s_fileHashBuilt;

// STATIC: %0x00D738D0
Blizzard::Lock::RMutex System_SFile2::s_fileHashLock;

// STATIC: %0x00D738E8
System_SFile2::FileHash System_SFile2::s_fileHash;

// STATIC: %0x00D73930
Blizzard::Thread::LocalObject<System_SFile2::SFileFatalInfo> System_SFile2::s_fatalInfo;

// OFFSET: 0x00627990/0x140693B10
void SFile::LogFileAccess(SFileImpl* file, int32_t mode, bool flag) {
}

// PROVEN: 16424743 IDENTICAL/NONE
// OFFSET: 0x007A4EC0
static void BlizzardAssertCallback(const char* expression, const char* message, const char* fileName, uint32_t lineNumber) {
    if (!*message) {
        SErrDisplayError(0, fileName, lineNumber, expression, 0, 1, 2, 0x11111111);
    } else {
        SErrDisplayError(0, fileName, lineNumber, message, 0, 1, 2, 0x11111111);
    }
}

// INLINED
int32_t SFile::Initialize() {
    Blizzard::Debug::SetAssertHandler(BlizzardAssertCallback);
    return 1;
}

// OFFSET: %0x00708010
static void SignatureErrorCallback(const char* name, const char* message) {
    SErrDisplayError(0x83, name, -4, message, 0, 1, 0, 0x11111111);
}

// PROVEN: a4589ec1 IDENTICAL/IDENTICAL
// OFFSET: 0x007A4F50/0x140499BE0
int32_t STORMAPI SFile::CloseArchive(SArchive* archive) {
    int32_t result = Blizzard::Mopaq::SFileCloseArchive(archive->m_archive);
    delete archive;
    return result;
}

// PROVEN: 69b9fa80 IDENTICAL/NONE
// OFFSET: 0x007A4F80
void SFile::DisableSFileCheckDisk() {
    System_SFile2::s_checkDisk = false;
}

// PROVEN: eacfce5b IDENTICAL/NONE
// OFFSET: 0x007A4F90
int32_t STORMAPI SFile::EnableDirectAccess(uint32_t flags) {
    System_SFile2::s_directAccess = 0;

    if (flags & 1) {
        System_SFile2::s_directAccess = 1;
    }

    if (flags & 2) {
        System_SFile2::s_directAccess |= 2;
    }

    Blizzard::Mopaq::SFileEnableDirectAccess(flags);

    return 1;
}

// PROVEN: 65080a7a IDENTICAL/IDENTICAL
// OFFSET: 0x007A4FD0/0x140499C60
void STORMAPI SFile::ConvertOggToWavInBackground(const char* name) {
    if (OggToWavConversion::s_ConvertOggCallback) {
        OggToWavConversion::s_ConvertOggCallback(name);
    }
}

// OFFSET: %0x007080E0
int32_t STORMAPI SFile::FileIsLocal(const char* name, int32_t queue) {
    int32_t result = Blizzard::Mopaq::SFileFileIsLocal(name, queue);

    if (!result) {
        BGLoading::StartBGLoadThread();
    }

    return result;
}

// OFFSET: %0x00708110
int32_t STORMAPI SFile::FileIsLocal(SFileImpl* file, int32_t queue) {
    int32_t result = 1;

    if (file->m_file) {
        result = Blizzard::Mopaq::SFileFileIsLocal(file->m_file, queue);

        if (!result) {
            BGLoading::StartBGLoadThread();
            return 0;
        }
    } else if (file->m_stream) {
        return Blizzard::Streaming::IsLocal(file->m_stream, 0, static_cast<uint32_t>(Blizzard::File::GetFileInfo(file->m_stream)->m_size));
    }

    return result;
}

// OFFSET: %0x00708180
int32_t STORMAPI SFile::FileRangeIsLocal(SFileImpl* file, int64_t offset, uint32_t size) {
    if (!file->m_stream) {
        return 1;
    }

    return Blizzard::Streaming::IsLocal(file->m_stream, offset, size);
}

// PROVEN: db1b75ff IDENTICAL/IDENTICAL
// OFFSET: 0x007A4FF0/0x140499C80
bool STORMAPI SFile::FileGetIsLocalAmount(const char* name, int64_t& local, int64_t& total) {
    return Blizzard::Mopaq::SFileGetIsLocalAmount(name, local, total);
}

// OFFSET: %0x007081D0
void STORMAPI SFile::FileGetIsLocalAmount(SFileImpl* file, int64_t& local, int64_t& total) {
    if (file->m_file) {
        Blizzard::Mopaq::SFileGetIsLocalAmount(file->m_file, local, total);
        return;
    }

    if (file->m_stream) {
        total = Blizzard::File::GetFileInfo(file->m_stream)->m_size;
        int64_t amount = Blizzard::Streaming::GetIsLocalAmount(file->m_stream);
        local = amount < 0 ? total : amount;
    }
}

// OFFSET: %0x00708240
int32_t STORMAPI SFile::MakeFileLocal(const char* name, int32_t queue, bool wait, int64_t budget, int32_t* status) {
    int32_t local = Blizzard::Mopaq::SFileFileIsLocal(name, queue);

    if (local == 0) {
        BGLoading::StartBGLoadThread();
    } else if (local == 1) {
        return 1;
    } else if (local == 2) {
        if (!wait) {
            return 1;
        }
    } else if (local == -1) {
        return 1;
    }

    Blizzard::Streaming::MakeLocalParams params;
    params.m_budget = 0x7FFFFFFFFFFFFFFFLL;
    params.m_status = 0;

    if (budget >= 0) {
        params.m_budget = budget;
    }

    int32_t result = Blizzard::Mopaq::SFileMakeFileLocal(name, queue, wait, &params);

    if (status) {
        *status = params.m_status;
    }

    return result;
}

// OFFSET: %0x007082F0
int32_t STORMAPI SFile::MakeFileLocal(SFileImpl* file, int32_t queue, bool wait, int64_t budget, int32_t* status) {
    int32_t result = SFile::FileIsLocal(file, queue);

    if (result == 1) {
        return result;
    }

    if (result == 2) {
        if (!wait) {
            return 1;
        }
    } else if (result == -1) {
        return 1;
    }

    Blizzard::Streaming::MakeLocalParams params;
    params.m_budget = 0x7FFFFFFFFFFFFFFFLL;
    params.m_status = 0;

    if (budget >= 0) {
        params.m_budget = budget;
    }

    result = 0;

    if (file->m_file) {
        result = Blizzard::Mopaq::SFileMakeFileLocal(file->m_file, queue, wait, &params);
    } else if (file->m_stream) {
        uint32_t size = static_cast<uint32_t>(Blizzard::File::GetFileInfo(file->m_stream)->m_size);

        if (wait) {
            result = Blizzard::Streaming::MakeLocalBlocking(file->m_stream, 0, size, static_cast<Blizzard::Streaming::MakeLocalOption>(queue), &params);
        } else {
            result = Blizzard::Streaming::MakeLocal(file->m_stream, 0, size, static_cast<Blizzard::Streaming::MakeLocalOption>(queue), NULL);
        }
    }

    if (status) {
        *status = params.m_status;
    }

    return result;
}

// OFFSET: %0x007083E0
int32_t STORMAPI SFile::MakeFileRangeLocal(SFileImpl* file, int32_t queue, bool wait, int64_t offset, uint32_t size) {
    int32_t result = SFile::FileIsLocal(file, queue);

    if (result == 1) {
        return 1;
    }

    if (result == 2) {
        if (!wait) {
            return 1;
        }
    } else if (result == -1) {
        return 1;
    }

    if (!file->m_stream) {
        return 0;
    }

    if (wait) {
        return Blizzard::Streaming::MakeLocalBlocking(file->m_stream, offset, size, static_cast<Blizzard::Streaming::MakeLocalOption>(queue), NULL);
    }

    return Blizzard::Streaming::MakeLocal(file->m_stream, offset, size, static_cast<Blizzard::Streaming::MakeLocalOption>(queue), NULL);
}

// OFFSET: %0x00708470
int32_t STORMAPI SFile::FileHasHigherPriority(const char* name, const char* extension, uint32_t size) {
    char buffer[1024];
    Blizzard::String::Copy(buffer, name, size);
    char* ext = const_cast<char*>(Blizzard::String::FindExtension(buffer));
    Blizzard::String::Copy(ext, extension, static_cast<uint32_t>(buffer + sizeof(buffer) - ext));

    return Blizzard::Mopaq::SFileHasHigherPriority(name, buffer) != 0;
}

// PROVEN: b04b438a IDENTICAL/DIFFERENT
// OFFSET: 0x007A5100/0x140499D90
int32_t STORMAPI SFile::GetActualFileName(SFileImpl* file, char* buffer, uint32_t size) {
    const char* name = file->m_actualName;

    if (!name) {
        name = file->m_name;
    }

    Blizzard::String::Copy(buffer, name, size);

    return 1;
}

// PROVEN: 9ab8cf17 IDENTICAL/IDENTICAL
// OFFSET: 0x007A5130/0x140499DC0
int32_t STORMAPI SFile::GetBasePath(char* path, uint32_t size) {
    Blizzard::String::Copy(path, SFile::m_basePath, size);
    return 1;
}

// OFFSET: %0x00708530
const char* SFile::GetDataPath() {
    return System_SFile2::s_dataPath;
}

// OFFSET: %0x00708540
uint32_t STORMAPI SFileGetFileSize(SFileImpl* file, uint32_t* sizeHigh) {
    if (file->m_type) {
        if (file->m_type > 1 && file->m_type <= 3) {
            return static_cast<uint32_t>(Blizzard::Mopaq::SFileGetFileSize(file->m_file, sizeHigh));
        }

        return 0;
    }

    if (!file->m_stream) {
        return 0;
    }

    Blizzard::File::FileInfo info;

    if (!Blizzard::File::GetFileInfo(file->m_stream, &info)) {
        return 0;
    }

    if (sizeHigh) {
        *sizeHigh = static_cast<uint32_t>(info.m_size >> 32);
    }

    return static_cast<uint32_t>(info.m_size);
}

// OFFSET: %0x007085B0
int32_t STORMAPI SFile::List(SArchive* archive, int32_t (*callback)(const char* fileName, void* param), void* param) {
    if (archive->unk_0000) {
        return 0;
    }

    Blizzard::Mopaq::Enumerator* enumerator = NULL;
    bool done = false;
    bool result = Blizzard::Mopaq::SFileOpenArchiveEnumerator(archive->m_archive, NULL, 3, &enumerator);

    if (result) {
        char name[512];

        while (!done) {
            int32_t status = Blizzard::Mopaq::SFileEnumerateArchive(enumerator, name, sizeof(name));

            if (status) {
                if (status != 2) {
                    result = false;
                    break;
                }

                if (Blizzard::String::EqualI("md5", Blizzard::String::FindExtension(name)) || callback(name, param)) {
                    continue;
                }
            }

            done = true;
        }
    }

    if (enumerator) {
        Blizzard::Mopaq::SFileCloseArchiveEnumerator(enumerator);
    }

    return result;
}

// OFFSET: %0x00708680
int32_t STORMAPI SFile::OpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive) {
    int32_t result = 0;
    Blizzard::File::SetLastErrorStack(NULL);

    *archive = new (0u, __FILE__, __LINE__) SArchive;
    (*archive)->m_archive = NULL;

    System_SFile::ARCHIVEREC* handle;

    if (Blizzard::String::EqualI(Blizzard::String::FindExtension(path), "mpq")) {
        (*archive)->unk_0000 = 0;
        result = Blizzard::Mopaq::SFileOpenArchive(path, priority, flags, &handle);
        (*archive)->m_archive = handle;

        if (result) {
            fprintf(stderr, "archive %s opened\n", path);
            return result;
        }
    }

    delete *archive;
    *archive = NULL;
    SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());

    return result;
}

// OFFSET: %0x00708740
int32_t STORMAPI SFile::CreateArchive(const char* path, int32_t priority, SArchive** archive) {
    Blizzard::File::SetLastErrorStack(NULL);

    Blizzard::Mopaq::ArchiveOptions options;
    options.m_sectorSize = 0x4000;
    options.m_rawChunkSize = 0x4000;
    options.m_compressTables = true;
    options.unk_0009 = true;

    System_SFile::ARCHIVEREC* handle;
    int32_t result = Blizzard::Mopaq::SFileCreateArchive(path, priority, 0, &handle, &options);

    if (result) {
        Blizzard::Mopaq::SFileSetChecksumFlags(handle, 5);
        *archive = new (0u, __FILE__, __LINE__) SArchive;
        (*archive)->m_archive = handle;
        (*archive)->unk_0000 = 0;
        fprintf(stderr, "archive %s opened for writing\n", path);
    } else {
        SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());
    }

    return result;
}

// OFFSET: %0x00708800
int32_t STORMAPI SFile::OpenPathAsArchive(SArchive* archive, const char* path, int32_t priority, uint32_t flags, SArchive** result) {
    *result = new (0u, __FILE__, __LINE__) SArchive;
    (*result)->unk_0000 = 0;
    (*result)->m_archive = NULL;

    System_SFile::ARCHIVEREC* handle;
    int32_t opened = Blizzard::Mopaq::SFileOpenArchiveView(archive->m_archive, path, priority, &handle);
    (*result)->m_archive = handle;

    if (!opened) {
        delete *result;
        *result = NULL;
    }

    return opened;
}

// OFFSET: 0x007A6550/0x14049B550
int32_t STORMAPI SFile::Read(SFileImpl* file, void* buffer, uint32_t count, uint32_t* bytesRead) {
    return SFileReadFile(file, buffer, count, bytesRead, NULL, 0);
}

// OFFSET: %0x00708880
int32_t STORMAPI SFile::ReadAtPosition(SFileImpl* file, void* buffer, uint32_t offset, uint32_t count, uint32_t* bytesRead) {
    int32_t result = 0;

    if (!count) {
        if (bytesRead) {
            *bytesRead = 0;
        }

        return 1;
    }

    uint32_t read;

    if (file->m_type) {
        if (static_cast<uint32_t>(file->m_type - 2) > 1) {
            return 0;
        }

        read = 0;
        Blizzard::Mopaq::SFileSetFilePointer(file->m_file, offset, NULL, 0);

        if (!Blizzard::Mopaq::SFileReadFile(file->m_file, buffer, count, &read)) {
            goto fail;
        }
    } else {
        read = count;

        if (!Blizzard::File::Read(file->m_stream, buffer, offset, reinterpret_cast<int32_t*>(&read), 0)) {
            goto fail;
        }
    }

    if (bytesRead) {
        *bytesRead = read;
    }

    if (read == count) {
        result = 1;
        return result;
    }

fail:
    SErrSetLastError(38);

    return result;
}

// PROVEN: ccac0e90 IDENTICAL/IDENTICAL
// OFFSET: 0x007A5590/0x14049A2D0
void STORMAPI SFile::EnableHash(bool enable) {
    System_SFile2::s_useOldFindFile = !enable;
}

// PROVEN: 8c0e90a1 IDENTICAL/NONE
// OFFSET: 0x007A55B0
int32_t STORMAPI SFile::SetBasePath(const char* path) {
    Blizzard::String::Copy(SFile::m_basePath, path, sizeof(((SFile*)0)->m_basePath));
    uint32_t length = Blizzard::String::Length(SFile::m_basePath);

    if (SFile::m_basePath[0]) {
        char last = SFile::m_basePath[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(SFile::m_basePath, "\\", sizeof(((SFile*)0)->m_basePath));
        }
    }

    Blizzard::Mopaq::SFileSetBasePath(path);

    return 1;
}

// PROVEN: d5c07e81 IDENTICAL/NONE
// OFFSET: 0x007A5620
int32_t STORMAPI SFile::SetDataPath(const char* path) {
    Blizzard::String::Copy(System_SFile2::s_dataPath, path, sizeof(System_SFile2::s_dataPath));
    uint32_t length = Blizzard::String::Length(System_SFile2::s_dataPath);

    if (System_SFile2::s_dataPath[0]) {
        char last = System_SFile2::s_dataPath[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(System_SFile2::s_dataPath, "\\", sizeof(System_SFile2::s_dataPath));
        }
    }

    return 1;
}

// PROVEN: 9e1abd79 IDENTICAL/DIFFERENT
// OFFSET: 0x007A5680/0x14049A3B0
int32_t STORMAPI SFile::SetDataPathAlternate(const char* path) {
    Blizzard::String::Copy(System_SFile2::s_dataPath2, path, sizeof(System_SFile2::s_dataPath2));
    uint32_t length = Blizzard::String::Length(System_SFile2::s_dataPath2);

    if (System_SFile2::s_dataPath2[0]) {
        char last = System_SFile2::s_dataPath2[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(System_SFile2::s_dataPath2, "\\", sizeof(System_SFile2::s_dataPath2));
        }
    }

    return 1;
}

// OFFSET: %0x00708A90
int32_t STORMAPI SFile::SetFilePointer(SFileImpl* file, int32_t distance, int32_t* distanceHigh, uint32_t method) {
    int32_t result = 0;
    Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

    if (file->m_type) {
        if (static_cast<uint32_t>(file->m_type - 2) <= 1) {
            result = static_cast<int32_t>(Blizzard::Mopaq::SFileSetFilePointer(file->m_file, distance, distanceHigh, method));
        }
    } else {
        int64_t position;

        if (method == 0) {
            position = distance;
        } else if (method == 1) {
            position = file->m_lock->m_position + distance;
        } else if (method == 2) {
            position = Blizzard::File::GetFileInfo(file->m_stream)->m_size + distance;
        } else {
            position = -1;
        }

        result = static_cast<int32_t>(position);
        file->m_lock->m_position = static_cast<uint32_t>(position);
    }

    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

    return result;
}

// PROVEN: ba8e3f5b IDENTICAL/IDENTICAL
// OFFSET: 0x007A5800/0x14049A520
int32_t STORMAPI SFile::Unload(void* data) {
    Blizzard::Memory::Free(data);
    return 1;
}

// OFFSET: %0x00708B70
int32_t System_SFile2::OldFindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type) {
    Blizzard::File::FileInfo info;

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, name, size);

    if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
        *type = 0;
        return true;
    }

    const char* filename = Blizzard::String::FindReverse(name, '\\', 0x7FFFFFFF);

    if (filename) {
        Blizzard::String::Copy(path, SFile::m_basePath, size);
        Blizzard::String::Append(path, filename + 1, size);

        if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
            *type = 0;
            return true;
        }
    }

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, s_dataPath, size);
    Blizzard::String::Append(path, name, size);

    if (Blizzard::File::GetFileInfo(path, &info)) {
        if (info.m_regularFile) {
            *type = 0;
            return true;
        }

        return false;
    }

    if (SFile::m_basePath[0]) {
        Blizzard::String::Copy(path, s_dataPath, size);
        Blizzard::String::Append(path, name, size);

        if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
            *type = 0;
            return true;
        }
    }

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, s_dataPath, size);
    Blizzard::String::Append(path, name, size);
    Blizzard::String::Append(path, ".MPQ", size);

    if (Blizzard::File::GetFileInfo(path, &info)) {
        if (info.m_regularFile) {
            *type = 2;
            return true;
        }

        return false;
    }

    if (s_dataPath2[0]) {
        Blizzard::String::Copy(path, SFile::m_basePath, size);
        Blizzard::String::Append(path, s_dataPath2, size);
        Blizzard::String::Append(path, name, size);

        if (Blizzard::File::GetFileInfo(path, &info)) {
            if (info.m_regularFile) {
                *type = 0;
                return true;
            }

            return false;
        }
    }

    if (!s_checkDisk) {
        flags &= ~3u;
    }

    if (Blizzard::Mopaq::SFileFileExists(NULL, name, flags, NULL)) {
        Blizzard::String::Copy(path, name, size);
        *type = 3;
        return true;
    }

    SErrSetLastError(2);

    return false;
}

// PROVEN: 5ebfbe9f IDENTICAL/IDENTICAL
// OFFSET: 0x007A5A40/0x14049A7F0
int32_t STORMAPI SFileAuthenticateArchiveEx(SArchive* archive, uint32_t* result, const uint8_t* modulus, uint32_t modulusSize, const uint8_t* exponent, uint32_t exponentSize) {
    Blizzard::Mopaq::AuthResult authResult;
    bool success = Blizzard::Mopaq::SFileAuthenticateArchiveEx(reinterpret_cast<System_SFile::ARCHIVEREC*>(archive), &authResult, modulus, modulusSize, exponent, exponentSize, "ARCHIVE");
    *result = authResult;

    if (!success) {
        SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());
    }

    return success != 0;
}

// PROVEN: df5c91e5 IDENTICAL/IDENTICAL
// OFFSET: 0x007A5A90/0x14049A860
int32_t STORMAPI SFileCloseArchive(SArchive* archive) {
    return Blizzard::Mopaq::SFileCloseArchive(reinterpret_cast<System_SFile::ARCHIVEREC*>(archive)) != 0;
}

// OFFSET: %0x00708E00
int32_t STORMAPI SFile::IsArchiveComplete(SArchive* archive) {
    return Blizzard::Mopaq::IsArchiveComplete(archive->m_archive);
}

// OFFSET: #0x00421FC0
int32_t STORMAPI SFile::FileExistsInArchives(const char* name) {
    int32_t result = Blizzard::Mopaq::SFileFileExists(NULL, name, 0, NULL);

    if (result) {
        SErrSetLastError(2);
    }

    return result;
}

// PROVEN: 1ededb30 IDENTICAL/IDENTICAL
// OFFSET: 0x007A5AF0/0x14049A8C0
int32_t STORMAPI SFileLoadFileEx(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags, SOVERLAPPED* overlapped) {
    bool result = Blizzard::Mopaq::SFileLoadFile(reinterpret_cast<System_SFile::ARCHIVEREC*>(archive), name, data, size, extra, flags);

    if (!result) {
        SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());
    }

    return result != 0;
}

// PROVEN: bb1edc7e IDENTICAL/IDENTICAL
// OFFSET: 0x007A5B40/0x14049A900
int32_t STORMAPI SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive) {
    System_SFile::ARCHIVEREC* result;

    if (Blizzard::Mopaq::SFileOpenArchive(path, priority, flags, &result)) {
        *archive = reinterpret_cast<SArchive*>(result);
        return 1;
    }

    SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());
    return 0;
}

// PROVEN: d10fa8cf IDENTICAL/IDENTICAL
// OFFSET: 0x007A5B90/0x14049A940
int32_t STORMAPI SFileUnloadFile(void* data) {
    return Blizzard::Mopaq::SFileUnloadFile(data) != 0;
}

// OFFSET: %0x00708EE0
void STORMAPI SFile::SetStreamingStatus(bool enabled, bool initialized, bool flag2, bool flag3, uint32_t build) {
    if (enabled) {
        s_streamingStatus.m_enabled = enabled;
    }

    if (initialized) {
        s_streamingStatus.m_initialized = initialized;
    }

    if (flag2) {
        s_streamingStatus.m_flag2 = flag2;
    }

    if (flag3) {
        s_streamingStatus.m_flag3 = flag3;
    }

    if (build) {
        s_streamingStatus.m_build = build;
    }
}

// OFFSET: %0x00708F30
int32_t STORMAPI SFile::InitializeStreaming(const char* mfilPath) {
    Blizzard::File::SetLastErrorStack(0);
    return Blizzard::Streaming::InitializeStreaming(mfilPath, NULL);
}

// OFFSET: %0x00708F50
void SFile::ShutdownStreaming() {
    Blizzard::Streaming::ShutdownStreaming();
}

// PROVEN: f6c844bf IDENTICAL/UNPROVEN
// OFFSET: 0x007A5BE0/0x14049A9A0
int32_t SFile::IsStreamingMode() {
    return Blizzard::Streaming::IsStreamingMode();
}

// OFFSET: %0x00708F70
const char* SFile::GetErrorDetails() {
    return Blizzard::Debug::ErrorStackAsText(Blizzard::File::GetLastErrorStack());
}

// OFFSET: %0x00708F80
int32_t SFile::IsBackgroundStreamingInProgress() {
    return BGLoading::IsStreamingInProgress();
}

// OFFSET: %0x00708F90
float SFile::GetStreamingBandwidth() {
    return BGLoading::GetBandwidth();
}

// OFFSET: %0x00708FA0
float SFile::GetBGStreamingDownloadedPercentage() {
    return BGLoading::GetDownloadedPercentage();
}

// OFFSET: %0x00708FB0
void STORMAPI SFile::PauseBackgroundStreaming(bool pause) {
    BGLoading::Pause(pause);
}

// OFFSET: %0x00708FD0
void STORMAPI SFile::InitializeRepair(const char* path) {
    Blizzard::Repair::SetRepairReportCallbacks(Tracker::RepairReportFail, Tracker::RepairReportSuccess);
}

// OFFSET: %0x00708FF0
int32_t SFileClearBlockTableMD5s() {
    System_SFile::ClearAllFileFlags();
    return 1;
}

// OFFSET: %0x00709000
int32_t STORMAPI SFileSetFileBlockEntryMD5(const char* name, const MD5& md5) {
    Blizzard::Util::Digest<16> digest;
    md5.ToArray(reinterpret_cast<uint8_t*>(&digest));
    return Blizzard::Mopaq::SetFileSignature(name, digest) != 0;
}

// OFFSET: %0x00709040
void STORMAPI SFileSetSignatureCallbacks(int32_t enable) {
    Blizzard::Mopaq::SetSignatureCheckMode(enable);

    if (enable) {
        Blizzard::Mopaq::SetSignatureErrorCallback(SignatureErrorCallback);
    }
}

// OFFSET: %0x00709070
int32_t STORMAPI SEnumProcessesByName(const char* name, void (*callback)(const char*, uint32_t, void*), void* param) {
    return Blizzard::Process::EnumProcessesByName(name, callback, param) != 0;
}

// PROVEN: 010054ff IDENTICAL/IDENTICAL
// OFFSET: 0x007A5D40/0x14049AB20
void STORMAPI SFile::SetTrial(bool trial) {
    System_SFile2::s_trial = trial;
}

// OFFSET: %0x007090B0
int32_t SFile::IsTrial() {
    return System_SFile2::s_trial;
}

// INLINED
System_SFile2::SFileFatalInfo::SFileFatalInfo() {
    this->m_fatal = true;
}

// OFFSET: %0x00719720
size_t System_SFile2::FileHashCompare::operator()(const std::string& key) const {
    const char* str = key.c_str();

    if (str) {
        return Blizzard::Util::Hash32I(str, static_cast<uint32_t>(strlen(str)));
    }

    return Blizzard::Util::Hash32I(NULL, 0);
}

// OFFSET: %0x007196A0
bool System_SFile2::FileHashCompare::operator()(const std::string& key1, const std::string& key2) const {
    const char* str1 = key1.c_str();
    const char* str2 = key2.c_str();

    if (str1 == str2) {
        return false;
    }

    if (!str1) {
        return true;
    }

    if (str2) {
        return _stricmp(str1, str2) < 0;
    }

    return false;
}

// OFFSET: %0x00709140
SFileImpl::SFileImpl(int32_t type) {
    this->m_lock = new (this->m_lockData) SFileLock();
    this->m_lock->unk_0018 = 0;
    this->m_lock->m_position = 0;
    this->m_stream = NULL;
    this->m_archive = NULL;
    this->m_name = NULL;
    this->m_file = NULL;
    this->m_async = 0;
    this->m_actualName = NULL;
    this->unk_001C = 0;
    this->m_type = type;
}

// INLINED
SFileImpl::~SFileImpl() {
    Blizzard::Memory::Free(this->m_actualName);
    this->m_lock->~SFileLock();
}

// INLINED
SFileLock::SFileLock() {
    Blizzard::Lock::MutexCreate(this->m_mutex);
}

// INLINED
SFileLock::~SFileLock() {
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// PROVEN: c0eb6753 IDENTICAL/UNPROVEN
// OFFSET: 0x007A6230/0x14049B1E0
void STORMAPI SFile::SetReadFatal(bool fatal) {
    static_cast<System_SFile2::SFileFatalInfo&>(System_SFile2::s_fatalInfo).m_fatal = fatal;
}

// OFFSET: %0x00709210
int32_t STORMAPI SFileReadFile(SFileImpl* file, void* buffer, uint32_t count, uint32_t* bytesRead, SOVERLAPPED* overlapped, uint32_t unused) {
    int32_t result = 0;
    Blizzard::File::SetLastErrorStack(NULL);

    if (overlapped) {
        return 0;
    }

    if (!count) {
        if (bytesRead) {
            *bytesRead = 0;
        }

        return 1;
    }

    if (file->m_type == 0) {
        int32_t read = count;
        Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

        if (Blizzard::File::Read(file->m_stream, buffer, file->m_lock->m_position, &read, 0)) {
            if (bytesRead) {
                *bytesRead = read;
            }

            if (read == static_cast<int32_t>(count)) {
                file->m_lock->m_position += read;
                result = 1;
                Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
                return result;
            }
        }

        SErrSetLastError(38);
        Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

        return result;
    }

    if (static_cast<uint32_t>(file->m_type - 2) > 1) {
        return 0;
    }

    uint32_t read = 0;
    Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);
    Blizzard::File::SetLastError(0);

    if (Blizzard::Mopaq::SFileReadFile(file->m_file, buffer, count, &read)) {
        if (bytesRead) {
            *bytesRead = read;
        }

        if (read == count) {
            Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
            return 1;
        }
    } else {
        bool fatal = static_cast<System_SFile2::SFileFatalInfo&>(System_SFile2::s_fatalInfo).m_fatal;
        Blizzard::Debug::ErrorStackRecord* stack = Blizzard::File::GetLastErrorStack();

        if (!stack || !stack->FindCategoryCode(1, 10)) {
            if (stack && stack->FindCategory(21)) {
                SErrSetLastError(2100);
                Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
                return 0;
            }

            if (fatal) {
                bool stormError = SErrGetLastError() != 0;
                char message[256];
                SErrGetErrorStr(SErrGetLastError(), message, sizeof(message));

                if (stack->FindCategory(21)) {
                    const char* format = stormError ? "Failed to read data from the network. Please check your Internet connection and try again.\n\nDebug Details:\n\n%sStorm Error Msg:%s" : "Failed to read data from the network. Please check your Internet connection and try again.\n\nDebug Details:\n\n%s";
                    SErrDisplayAppFatal(format, Blizzard::Debug::ErrorStackAsText(stack), message);
                }

                const char* format = stormError ? "Failed to read file %s.\n\nDebug Details:\n\n%sStorm Error Msg:%s" : "Failed to read file %s.\n\nDebug Details:\n\n%s";
                SErrDisplayAppFatal(format, file->m_name, Blizzard::Debug::ErrorStackAsText(stack), message);
            }
        }
    }

    SErrSetLastError(38);
    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

    return 0;
}

// OFFSET: %0x00709530
int32_t STORMAPI SFileCloseFile(SFileImpl* file) {
    if (file) {
        if (file->m_stream) {
            Blizzard::File::Close(file->m_stream);
        }

        if (file->m_file) {
            Blizzard::Mopaq::SFileCloseFile(file->m_file);
        }

        if (file->m_archive) {
            if (file->m_archive->m_archive) {
                Blizzard::Mopaq::SFileCloseArchive(file->m_archive->m_archive);
            }

            delete file->m_archive;
        }

        file->~SFileImpl();
        Blizzard::Memory::Free(file);
    }

    return 1;
}

// OFFSET: %0x0070A1B0
void SFile::RebuildHash() {
    Blizzard::Lock::MutexEnter(System_SFile2::s_fileHashLock.m_mutex);
    System_SFile2::s_fileHash.clear();
    System_SFile2::s_fileHashBuilt = false;
    Blizzard::Lock::MutexLeave(System_SFile2::s_fileHashLock.m_mutex);
}

// OFFSET: %0x0070AAD0
bool System_SFile2::ScanDirForFileSystemHash(const char* directory) {
    char path[512];
    System_SFile::BackslashPath(directory, path, sizeof(path));
    Blizzard::String::ForceTrailingSeparator(path, sizeof(path), '\\');

    ScanDirCookie cookie;
    cookie.unk_0000 = 1;
    cookie.m_root = path;
    cookie.m_rootLength = Blizzard::String::Length(path);
    cookie.m_prefix = "";

    return Blizzard::File::ProcessDirFast(path, &cookie, Blizzard::Streaming::ScanDirectoryForManifest, false);
}

// PROVEN: e855b34d IDENTICAL/DIFFERENT
// OFFSET: 0x007A8350/0x14049DDA0
void System_SFile2::BuildFileSystemHash() {
    Blizzard::String::Copy(s_hashedBasePath, SFile::m_basePath, sizeof(s_hashedBasePath));

    const char* root = SFile::m_basePath;

    if (!Blizzard::File::IsDirectory(SFile::m_basePath)) {
        root = ".";
    }

    if (Blizzard::File::IsDirectory(root)) {
        ScanDirForFileSystemHash(root);
    }

    char path[512];

    if (s_dataPath2[0]) {
        Blizzard::String::Copy(path, root, sizeof(path));
        Blizzard::String::Copy(path, s_dataPath2, sizeof(path));

        if (Blizzard::File::IsDirectory(path)) {
            ScanDirForFileSystemHash(path);
        }
    }

    Blizzard::String::Copy(path, root, sizeof(path));
    Blizzard::String::Append(path, s_dataPath, sizeof(path));

    if (Blizzard::File::IsDirectory(path)) {
        ScanDirForFileSystemHash(path);
    }

    s_fileHashBuilt = true;
    Blizzard::Log::Flush(0);
}

// OFFSET: %0x0070AC50
int32_t System_SFile2::FindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type, System_SFile::ARCHIVEREC** archive) {
    flags |= s_directAccess;

    if (archive) {
        *archive = NULL;
    }

    if (s_useOldFindFile || (name[0] == '\\' && name[1] == '\\') || name[1] == ':') {
        return OldFindFile(name, path, size, flags, type);
    }

    if (flags & 3) {
        bool found = false;
        Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);

        if (!s_fileHashBuilt) {
            Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);
            Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);
            BuildFileSystemHash();
            Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);
            Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);
        }

        FileHash::iterator it = s_fileHash.end();

        if (flags & 2) {
            it = s_fileHash.find(std::string(Blizzard::String::FindFilename(name)));
        }

        if (it == s_fileHash.end()) {
            it = s_fileHash.find(std::string(name));

            if (it == s_fileHash.end() && !Blizzard::String::Equal(s_hashedBasePath, SFile::m_basePath)) {
                char fullPath[260];
                Blizzard::String::Copy(fullPath, SFile::m_basePath, sizeof(fullPath));
                Blizzard::String::Append(fullPath, name, sizeof(fullPath));
                it = s_fileHash.find(std::string(fullPath));
            }
        }

        if (it != s_fileHash.end()) {
            Blizzard::String::Copy(path, it->second.m_path.c_str(), size);
            *type = it->second.m_type;
            found = true;
        }

        Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);

        if (found) {
            return true;
        }
    }

    if (!s_checkDisk) {
        flags &= ~3u;
    }

    System_SFile::ARCHIVEREC* found;

    if (Blizzard::Mopaq::SFileFileExists(NULL, name, flags, &found)) {
        if (archive) {
            *archive = found;
        }

        Blizzard::String::Copy(path, name, size);
        *type = 3;

        return true;
    }

    return false;
}

// OFFSET: %0x0070AFA0
int32_t STORMAPI SFile::FileExistsEx(const char* name, uint32_t flags) {
    char path[260];
    int32_t type;

    return System_SFile2::FindFile(name, path, sizeof(path), flags, &type, NULL);
}

// OFFSET: %0x0070AFE0
int32_t STORMAPI SFile::OpenEx(SArchive* archive, const char* name, uint32_t flags, SFileImpl** result) {
    Blizzard::File::SetLastErrorStack(NULL);
    *result = NULL;

    System_SFile::ARCHIVEREC* handle = NULL;
    int32_t type;
    char path[260];

    if (archive) {
        if (archive->unk_0000) {
            return 0;
        }

        type = 3;
        handle = archive->m_archive;
    } else if (!System_SFile2::FindFile(name, path, sizeof(path), flags, &type, &handle)) {
        SErrSetLastError(2);
        return 0;
    }

    void* memory = Blizzard::Memory::Allocate(Blizzard::String::Length(path) + sizeof(SFileImpl) + 1, 1);
    *result = new (memory) SFileImpl(type);

    if (type == 0) {
        if (!Blizzard::File::Open(path, 0x1005, (*result)->m_stream)) {
            goto fail;
        }

        (*result)->m_name = reinterpret_cast<char*>(*result + 1);
        Blizzard::String::MemCopy((*result)->m_name, path, Blizzard::String::Length(path) + 1);

        return 2;
    }

    if (type == 2) {
        return 0;
    }

    if (type != 3) {
        return 0;
    }

    if (!Blizzard::Mopaq::SFileOpenFileEx(handle, name, handle ? 0 : flags, &(*result)->m_file)) {
        goto fail;
    }

    (*result)->m_name = reinterpret_cast<char*>(*result + 1);
    Blizzard::String::MemCopy((*result)->m_name, path, Blizzard::String::Length(path) + 1);

    return 1;

fail:
    if (*result) {
        (*result)->~SFileImpl();
        Blizzard::Memory::Free(*result);
    }

    *result = NULL;

    return 0;
}

// OFFSET: %0x0070B200
int32_t STORMAPI SFile::FileExists(const char* name) {
    char path[260];
    int32_t type;

    return System_SFile2::FindFile(name, path, sizeof(path), 0, &type, NULL);
}

// OFFSET: %0x0070B230
int32_t STORMAPI SFile::ReadFile(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags, SOVERLAPPED* overlapped) {
    SFileImpl* file;
    int32_t result = SFile::OpenEx(archive, name, flags, &file);

    if (!result) {
        return result;
    }

    uint32_t fileSize = SFileGetFileSize(file, NULL);
    uint8_t* buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(fileSize + extra));

    if (overlapped) {
        file->m_async = 1;
    }

    if (!SFileReadFile(file, buffer, fileSize, size, overlapped, 0)) {
        Blizzard::Memory::Free(buffer, 0, __FILE__, __LINE__);
        SFileCloseFile(file);
        return 0;
    }

    if (extra) {
        Blizzard::String::MemFill(buffer + fileSize, extra, 0);
    }

    *data = buffer;

    if (size) {
        *size = fileSize;
    }

    if (!overlapped) {
        SFileCloseFile(file);
    }

    return 1;
}

// OFFSET: %0x0070B340
int32_t STORMAPI SFile::LoadFile(const char* name, void** data, uint32_t* size, uint32_t extra, SOVERLAPPED* overlapped) {
    return SFile::ReadFile(NULL, name, data, size, extra, 0, overlapped);
}

// PROVEN: 8c3d601c IDENTICAL/IDENTICAL
// OFFSET: 0x007A8C50/0x14049EA10
int32_t STORMAPI SFileOpenFile(const char* name, SFileImpl** result) {
    return SFile::OpenEx(NULL, name, 0, result);
}

