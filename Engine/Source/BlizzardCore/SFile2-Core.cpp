#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <windows.h>
#include <new>
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/BackgroundCache.h"
#include "Engine/Source/BlizzardCore/BGLoading-Core.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileEnumerators.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
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
#include "Engine/Source/BlizzardCore/Source/System/URL/URL.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"

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

// STATIC: 0x00EB3C70
int32_t (*System_SFile2::s_findFileCallback)(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type, uint32_t* fileSize, void* param);

// STATIC: 0x00EB3C6C
void* System_SFile2::s_findFileParam;

class StreamingRequestHandler : public Blizzard::Streaming::RequestHandler {
    public:
    void Queue(Blizzard::Storage::Request* request) override;
    bool Start(bool paused) override;
    void Stop() override;
    void Pause() override;
    void Resume() override;
};

// STATIC: 0x00EB4098
static StreamingRequestHandler* s_streamingHandler;

// PROVEN: 4aee979d IDENTICAL/DIFFERENT
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

// PROVEN: 157c7100 IDENTICAL/DIFFERENT
// OFFSET: 0x007A4F20/0x140499BA0
static void SignatureErrorCallback(const char* name, const char* message) {
    SErrDisplayError(STORM_ERROR_FILE_CORRUPTED, name, -4, message, 0, 1, 0, 0x11111111);
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



// OFFSET: 0x007A5FC0
int32_t STORMAPI SFile::FileIsLocal(SFileImpl* file, int64_t offset, uint32_t size) {
    if (file->m_file) {
        return Blizzard::Mopaq::SFileFileIsLocal(file->m_file, false);
    }

    if (file->m_stream) {
        if (!size) {
            size = SFileGetFileSize(file, NULL);
        }

        return Blizzard::Streaming::IsLocal(file->m_stream, offset, size);
    }

    return 1;
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

// OFFSET: 0x007A5010/0x140499C90
void STORMAPI SFile::FileGetIsLocalAmount(SFileImpl* file, int64_t& local, int64_t& total) {
    if (!file) {
        local = 0;
        total = 0;
        return;
    }

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



// OFFSET: 0x007A6020/0x14049AEE0
int32_t STORMAPI SFile::MakeFileLocal(SFileImpl* file, bool wait, int64_t chunk, int32_t* status, int64_t offset, uint32_t size) {
    int32_t result = SFile::FileIsLocal(file, 0, 0);

    switch (result) {
        case 1:
            return 1;

        case 2:
            if (!wait) {
                return 1;
            }

            break;

        case -1:
            return 1;
    }

    result = 0;
    Blizzard::Streaming::MakeLocalParams params;
    params.m_budget = 0x7FFFFFFFFFFFFFFFLL;
    params.m_status = 0;

    if (file->m_file) {
        if (size >= SFileGetFileSize(file, NULL) || Blizzard::Mopaq::SFileIsFileCompressed(file->m_file) || Blizzard::Mopaq::SFileIsPatchFile(file->m_file) || !wait) {
            result = Blizzard::Mopaq::SFileMakeFileLocal(file->m_file, wait, &params, &chunk);
        } else {
            result = Blizzard::Mopaq::SFileMakeFileRangeLocal(file->m_file, offset, size, &params, &chunk);
        }
    } else if (file->m_stream) {
        if (!size) {
            size = SFileGetFileSize(file, NULL);
        }

        if (wait) {
            result = Blizzard::Streaming::MakeLocalBlocking(file->m_stream, offset, size, &params, &chunk);
        } else {
            result = Blizzard::Streaming::MakeLocal(file->m_stream, offset, size, NULL, &chunk);
        }
    }

    if (status) {
        *status = params.m_status;
    }

    return result;
}

// OFFSET: %0x007083E0
int32_t STORMAPI SFile::MakeFileRangeLocal(SFileImpl* file, int32_t queue, bool wait, int64_t offset, uint32_t size) {
    int32_t result = SFile::FileIsLocal(file, 0, 0);

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
        return Blizzard::Streaming::MakeLocalBlocking(file->m_stream, offset, size, NULL, NULL);
    }

    return Blizzard::Streaming::MakeLocal(file->m_stream, offset, size, NULL, NULL);
}

// PROVEN: b377e194 IDENTICAL/DIFFERENT
// OFFSET: 0x007A50A0/0x140499D20
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

// PROVEN: 4c74c8bd IDENTICAL/NONE
// OFFSET: 0x007A51E0
uint32_t STORMAPI SFile::GetFileCompressedSize(SFileImpl* file, uint32_t* sizeHigh) {
    switch (file->m_type) {
        case 0:
        case 2:
            return SFileGetFileSize(file, sizeHigh);

        case 3:
            return Blizzard::Mopaq::SFileGetFileCompressedSize(file->m_file, sizeHigh);

        default:
            return 0;
    }
}

// PROVEN: 9ab8cf17 IDENTICAL/IDENTICAL
// OFFSET: 0x007A5130/0x140499DC0
int32_t STORMAPI SFile::GetBasePath(char* path, uint32_t size) {
    Blizzard::String::Copy(path, SFile::m_basePath, size);
    return 1;
}

// PROVEN: a37adce2 IDENTICAL/NONE
// OFFSET: 0x007A5160/0x140499DE0
const char* SFile::GetDataPath() {
    return System_SFile2::s_dataPath;
}

// OFFSET: 0x007A5170/0x140499DF0
uint32_t STORMAPI SFileGetFileSize(SFileImpl* file, uint32_t* sizeHigh) {
    if (file->m_type) {
        if (file->m_type > 1 && file->m_type <= 3) {
            return static_cast<uint32_t>(Blizzard::Mopaq::SFileGetFileSize(file->m_file, sizeHigh));
        }
    } else {
        if (file->m_size) {
            return file->m_size;
        }

        Blizzard::File::FileInfo info;

        if (file->m_stream && Blizzard::File::GetFileInfo(file->m_stream, &info)) {
            if (sizeHigh) {
                *sizeHigh = static_cast<uint32_t>(info.m_size >> 32);
            }

            return static_cast<uint32_t>(info.m_size);
        }
    }

    return 0;
}

// OFFSET: 0x007A5220/0x140499E90
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

// PROVEN: 2fa807a3 IDENTICAL/NONE
// OFFSET: 0x007A52F0/0x140499F90
bool STORMAPI SFile::DeleteArchive(const char* path) {
    bool result = Blizzard::File::Delete(path);

    if (result) {
        char buffer[260];
        Blizzard::String::Format(buffer, sizeof(buffer), "%s.lock", path);
        Blizzard::File::Delete(buffer);
        Blizzard::String::Format(buffer, sizeof(buffer), "%s.writeCacheBackup", path);
        result = Blizzard::File::Delete(buffer);
    }

    return result;
}

// OFFSET: 0x007A5360/0x14049A000
int32_t STORMAPI SFile::OpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive) {
    int32_t result = 0;
    Blizzard::File::SetLastErrorStack(NULL);

    *archive = new (0u, __FILE__, __LINE__) SArchive;
    (*archive)->m_archive = NULL;

    System_SFile::ARCHIVEREC* handle;

    if (Blizzard::String::EqualI(Blizzard::String::FindExtension(path), "mpq")) {
        (*archive)->unk_0000 = 0;
        result = Blizzard::Mopaq::SFileOpenArchive(path, priority, flags, &handle);

        if (!result && Blizzard::File::Exists(path)) {
            SFile::DeleteArchive(path);
            result = Blizzard::Mopaq::SFileOpenArchive(path, priority, flags, &handle);
        }

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

// OFFSET: 0x007A5450/0x14049A120
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

// OFFSET: 0x007A5510/0x14049A220
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

// OFFSET: 0x007A56E0/0x14049A410
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

// OFFSET: 0x007A57A0/0x14049A4D0
int32_t STORMAPI SFile::GetFilePointer(SFileImpl* file) {
    int32_t result = 0;
    Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

    if (file->m_type) {
        if (static_cast<uint32_t>(file->m_type - 2) <= 1) {
            result = static_cast<int32_t>(Blizzard::Mopaq::SFileGetFilePointer(file->m_file));
            Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
            return result;
        }
    } else {
        result = static_cast<int32_t>(file->m_lock->m_position);
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

// PROVEN: 3f66fe39 IDENTICAL/DIFFERENT
// OFFSET: 0x007A5820/0x14049A540
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

// PROVEN: 40c4376b IDENTICAL/NONE
// OFFSET: 0x007A5AB0/0x14049A880
int32_t STORMAPI SFile::IsArchiveComplete(SArchive* archive) {
    if (Blizzard::Streaming::IsStreamingMode()) {
        return Blizzard::Mopaq::IsArchiveStreamingComplete(archive->m_archive);
    }

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

// OFFSET: 0x007A5E80/0x14049ACD0
void StreamingRequestHandler::Queue(Blizzard::Storage::Request* request) {
    if (!request->m_size) {
        return;
    }

    int64_t chunk = -1;

    if (request->m_chunk) {
        chunk = *request->m_chunk;
    }

    request->AddRef();

    if (request->m_cancelled) {
        request->Release();
        return;
    }

    int64_t position = request->m_offset;

    while (true) {
        int64_t remaining = request->m_size + request->m_offset - position;
        int64_t length;

        if (chunk > 0) {
            length = chunk >= remaining ? remaining : chunk;
        } else {
            length = remaining;
        }

        Blizzard::Storage::ChunkRange range;
        range.m_offset = position;
        int64_t available = request->m_unit->m_logicalFileSize - position;
        range.m_length = length >= available ? available : length;
        int32_t result = request->m_unit->FetchRequest(request, &range);

        if (result == 2) {
            request->m_state = Blizzard::Storage::Request::STATE_NO_UNIT;
            break;
        }

        if (result == 1) {
            position += range.m_length;
            request->m_state = Blizzard::Storage::Request::STATE_CANCELLED;
            chunk -= range.m_length;
        } else if (!result) {
            break;
        }

        if (chunk <= 0) {
            break;
        }
    }

    request->OnWriteComplete(false);
}

// PROVEN: 5002475f IDENTICAL/NONE
// OFFSET: 0x00A332C0
bool StreamingRequestHandler::Start(bool paused) {
    return true;
}

// PROVEN: 9cc0a9d4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void StreamingRequestHandler::Stop() {
}

// PROVEN: 58fa913d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void StreamingRequestHandler::Pause() {
}

// PROVEN: 1c82bf83 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void StreamingRequestHandler::Resume() {
}

// OFFSET: 0x007A6190/0x14049B100
int32_t STORMAPI SFile::InitializeStreaming(const char* mfilPath, const char* dataPath) {
    s_streamingHandler = new StreamingRequestHandler();
    Blizzard::File::SetLastErrorStack(0);
    Blizzard::Storage::InitializeBitFieldAllocator(0);

    if (Blizzard::Streaming::InitializeStreaming(mfilPath, dataPath, s_streamingHandler)) {
        return 1;
    }

    Blizzard::Storage::ShutdownBitFieldAllocator();
    delete s_streamingHandler;
    s_streamingHandler = NULL;

    return 0;
}

// PROVEN: 5f86426a IDENTICAL/NONE
// OFFSET: 0x007A5BB0/0x14049A960
void SFile::ShutdownStreaming() {
    if (SFile::IsStreamingDataReady()) {
        Blizzard::Streaming::ShutdownStreaming();
        Blizzard::Storage::ShutdownBitFieldAllocator();
        delete s_streamingHandler;
        s_streamingHandler = NULL;
    }
}

// PROVEN: f6c844bf IDENTICAL/UNPROVEN
// OFFSET: 0x007A5BE0/0x14049A9A0
int32_t SFile::IsStreamingMode() {
    return Blizzard::Streaming::IsStreamingMode();
}

// PROVEN: 9a7a04ca IDENTICAL/NONE
// OFFSET: 0x007A5BF0/0x14049A9C0
int32_t STORMAPI SFile::IsPatchFile(SFileImpl* file) {
    if (!file->m_file) {
        return 0;
    }

    return Blizzard::Mopaq::SFileIsPatchFile(file->m_file);
}

// PROVEN: 4812a33f IDENTICAL/NONE
// OFFSET: 0x007A5C20/0x14049A9F0
const char* SFile::GetErrorDetails() {
    return Blizzard::Debug::ErrorStackAsText(Blizzard::File::GetLastErrorStack());
}

// PROVEN: b683909c IDENTICAL/NONE
// OFFSET: 0x007A5C30/0x14049AA10
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

// PROVEN: ab5e6376 IDENTICAL/UNPROVEN
// OFFSET: 0x007A5C50/0x14049AA40
void STORMAPI SFile::PauseBackgroundStreaming(bool pause) {
    BGLoading::Pause(pause);
}

// OFFSET: 0x007A5C70
void STORMAPI SFile::InitializeRepair(const char* path) {
    Blizzard::Repair::SetRepairReportCallbacks(NULL, Tracker::RepairReportFail, Tracker::RepairReportSuccess);
}

// PROVEN: 89a1fc5f IDENTICAL/NONE
// OFFSET: 0x007A5C90/0x14049AA70
int32_t SFileClearBlockTableMD5s() {
    System_SFile::ClearAllFileFlags();
    return 1;
}

// OFFSET: 0x007A5CA0/0x14049AA90
int32_t STORMAPI SFileSetFileBlockEntryMD5(const char* name, const MD5& md5) {
    Blizzard::Util::Digest<16> digest;
    md5.ToArray(reinterpret_cast<uint8_t*>(&digest));
    return Blizzard::Mopaq::SetFileSignature(name, digest) != 0;
}

// PROVEN: efe94da0 IDENTICAL/UNPROVEN
// OFFSET: 0x007A5CE0/0x14049AAD0
void STORMAPI SFileSetSignatureCallbacks(int32_t enable) {
    Blizzard::Mopaq::SetSignatureCheckMode(enable);

    if (enable) {
        Blizzard::Mopaq::SetSignatureErrorCallback(SignatureErrorCallback);
    }
}

// PROVEN: b795a6f5 IDENTICAL/NONE
// OFFSET: 0x007A5D10
int32_t STORMAPI SEnumProcessesByName(const char* name, void (*callback)(const char*, uint32_t, void*), void* param) {
    return Blizzard::Process::EnumProcessesByName(name, callback, param) != 0;
}

// PROVEN: 010054ff IDENTICAL/IDENTICAL
// OFFSET: 0x007A5D40/0x14049AB20
void STORMAPI SFile::SetTrial(bool trial) {
    System_SFile2::s_trial = trial;
}

// PROVEN: 293f67ad IDENTICAL/NONE
// OFFSET: 0x007A5D50/0x14049AB30
int32_t SFile::IsTrial() {
    return System_SFile2::s_trial;
}

// INLINED
System_SFile2::SFileFatalInfo::SFileFatalInfo() {
    this->m_fatal = true;
}

// OFFSET: 0x007B82A0/0x1404AEC30
size_t System_SFile2::FileHashCompare::operator()(const std::string& key) const {
    const char* str = key.c_str();

    if (str) {
        return Blizzard::Util::Hash32I(str, static_cast<uint32_t>(strlen(str)));
    }

    return Blizzard::Util::Hash32I(NULL, 0);
}

// OFFSET: 0x007B8220/0x1404AEBB0
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

// OFFSET: 0x007A5DD0
SFileImpl::SFileImpl(int32_t type) {
    this->m_lock = new (this->m_lockData) SFileLock();
    this->m_lock->unk_0018 = 0;
    this->m_lock->m_position = 0;
    this->m_stream = NULL;
    this->m_archive = NULL;
    this->m_name = NULL;
    this->m_file = NULL;
    this->m_actualName = NULL;
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

// OFFSET: 0x007A6260/0x14049B210
static int32_t ReadFileInternal(SFileImpl* file, void* buffer, uint32_t count, int32_t position, uint32_t* bytesRead) {
    Blizzard::File::SetLastErrorStack(NULL);

    if (!count) {
        if (bytesRead) {
            *bytesRead = 0;
        }

        return 1;
    }

    if (file->m_type == 0) {
        int32_t result = 0;
        int32_t read = count;
        Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

        int64_t offset = position < 0 ? file->m_lock->m_position : position;

        if (Blizzard::File::Read(file->m_stream, buffer, offset, &read, 0)) {
            if (bytesRead) {
                *bytesRead = read;
            }

            if (read == static_cast<int32_t>(count)) {
                if (position < 0) {
                    file->m_lock->m_position += read;
                }

                result = 1;
            } else {
                SErrSetLastError(38);
            }
        } else {
            SErrSetLastError(38);
        }

        Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

        return result;
    }

    if (static_cast<uint32_t>(file->m_type - 2) > 1) {
        return 0;
    }

    int32_t result = 0;
    uint32_t read = 0;
    Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);
    Blizzard::File::SetLastError(0);

    if (position >= 0) {
        Blizzard::Mopaq::SFileSetFilePointer(file->m_file, position, NULL, 0);
    }

    if (Blizzard::Mopaq::SFileReadFile(file->m_file, buffer, count, &read)) {
        if (bytesRead) {
            *bytesRead = read;
        }

        if (read == count) {
            result = 1;
            Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
            return result;
        }
    } else {
        bool fatal = static_cast<System_SFile2::SFileFatalInfo&>(System_SFile2::s_fatalInfo).m_fatal;
        Blizzard::Debug::ErrorStackRecord* stack = Blizzard::File::GetLastErrorStack();

        if (!stack || !stack->FindCategoryCode(1, 10)) {
            if (stack && stack->FindCategory(21)) {
                SErrSetLastError(STORM_ERROR_NETWORK_READ);
                Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
                return result;
            }

            if (fatal) {
                bool stormError = SErrGetLastError() != 0;
                char message[256];
                SErrGetErrorStr(SErrGetLastError(), message, sizeof(message));

                bool mfilReady;
                bool dataReady;
                uint32_t build;
                SFile::GetStreamingStatus(&mfilReady, &dataReady, NULL, &build);

                const char* reason;

                if (stack->FindCategory(21) && build) {
                    reason = "Failed to read file from network. Please try to check network connection.";
                } else {
                    reason = "Failed to read file from local.";
                }

                const char* dataStatus = SFile::IsStreamingDataReady() ? "Data Rdy" : "Data Not Rdy";
                const char* mfilStatus = SFile::IsStreamingMfilReady() ? "Mfil Rdy" : "Mfil Not Rdy";
                const char* enableStatus = SFile::IsStreamingDisabled() ? "Disabled" : "Enabled";

                char buffer[2048];
                Blizzard::String::Format(buffer, sizeof(buffer), "%s\n\nDetails: (Streaming Status: %s / %s / %s, Build: %d)\n%s", reason, enableStatus, mfilStatus, dataStatus, build, Blizzard::Debug::ErrorStackAsText(stack));

                if (stormError) {
                    Blizzard::String::Append(buffer, "Storm Error Msg:", sizeof(buffer));
                    Blizzard::String::Append(buffer, message, sizeof(buffer));
                }

                SErrDisplayAppFatal(buffer);
            }
        }
    }

    SErrSetLastError(38);
    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

    return result;
}

// OFFSET: 0x007A6550/0x14049B550
int32_t STORMAPI SFile::Read(SFileImpl* file, void* buffer, uint32_t count, uint32_t* bytesRead) {
    return ReadFileInternal(file, buffer, count, -1, bytesRead);
}

// OFFSET: 0x007A6580/0x14049B570
int32_t STORMAPI SFile::ReadAtPosition(SFileImpl* file, void* buffer, uint32_t offset, uint32_t count, uint32_t* bytesRead) {
    return ReadFileInternal(file, buffer, count, offset, bytesRead);
}

// PROVEN: 880ef8a1 IDENTICAL/UNPROVEN
// OFFSET: 0x007A6600/0x14049B610
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

// OFFSET: 0x007A79B0/0x14049D0A0
void SFile::RebuildHash() {
    Blizzard::Lock::MutexEnter(System_SFile2::s_fileHashLock.m_mutex);
    System_SFile2::s_fileHash.clear();
    System_SFile2::s_fileHashBuilt = false;
    Blizzard::Lock::MutexLeave(System_SFile2::s_fileHashLock.m_mutex);
}

// OFFSET: 0x007A82D0/0x14049DD10
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

// OFFSET: 0x007A8450/0x14049DFA0
int32_t System_SFile2::HashFindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type) {
    int32_t found = 0;
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

        if (it == s_fileHash.end()) {
            if (!Blizzard::String::Equal(s_hashedBasePath, SFile::m_basePath)) {
                char fullPath[260];
                Blizzard::String::Copy(fullPath, SFile::m_basePath, sizeof(fullPath));
                Blizzard::String::Append(fullPath, name, sizeof(fullPath));

                FileHash::iterator full = s_fileHash.find(std::string(fullPath));

                if (full != s_fileHash.end()) {
                    Blizzard::String::Copy(path, full->second.m_path.c_str(), size);
                    *type = full->second.m_type;
                }
            }

            Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);

            return found;
        }
    }

    Blizzard::String::Copy(path, it->second.m_path.c_str(), size);
    *type = it->second.m_type;
    found = 1;

    Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);

    return found;
}

// PROVEN: e3289ab5 IDENTICAL/NONE
// OFFSET: 0x007A8750/0x14049E3B0
int32_t System_SFile2::FindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type, uint32_t* fileSize, System_SFile::ARCHIVEREC** archive) {
    flags |= s_directAccess;

    if (archive) {
        *archive = NULL;
    }

    if (fileSize) {
        *fileSize = 0;
    }

    if (s_findFileCallback && s_findFileCallback(name, path, size, flags, type, fileSize, s_findFileParam)) {
        return *type != 6;
    }

    if (s_useOldFindFile || (name[0] == '\\' && name[1] == '\\') || name[1] == ':') {
        return OldFindFile(name, path, size, flags, type);
    }

    if ((flags & 3) && HashFindFile(name, path, size, flags, type)) {
        return 1;
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

        return 1;
    }

    return 0;
}

// PROVEN: f3094e89 IDENTICAL/NONE
// OFFSET: 0x007A8860
int32_t STORMAPI SFile::FileExistsEx(const char* name, uint32_t flags) {
    char path[260];
    int32_t type;
    uint32_t fileSize;

    return System_SFile2::FindFile(name, path, sizeof(path), flags, &type, &fileSize, NULL);
}

// OFFSET: 0x007A88A0/0x14049E4F0
int32_t STORMAPI SFile::OpenEx(SArchive* archive, const char* name, uint32_t flags, SFileImpl** result) {
    Blizzard::File::SetLastErrorStack(NULL);
    *result = NULL;

    System_SFile::ARCHIVEREC* handle = NULL;
    uint32_t fileSize = 0;
    int32_t type;
    char path[260];

    if (archive) {
        if (archive->unk_0000) {
            return 0;
        }

        type = 3;
        handle = archive->m_archive;
    } else if (!System_SFile2::FindFile(name, path, sizeof(path), flags, &type, &fileSize, &handle)) {
        SErrSetLastError(2);
        return 0;
    }

    void* memory = Blizzard::Memory::Allocate(Blizzard::String::Length(path) + sizeof(SFileImpl) + 1, 1);
    *result = memory ? new (memory) SFileImpl(type) : NULL;

    if (type == 0) {
        if (fileSize) {
            (*result)->m_name = reinterpret_cast<char*>(*result + 1);
            Blizzard::String::MemCopy((*result)->m_name, path, Blizzard::String::Length(path) + 1);
            (*result)->m_size = fileSize;

            return 2;
        }

        if (!Blizzard::File::Open(path, 0x1005, (*result)->m_stream)) {
            goto fail;
        }

        (*result)->m_name = reinterpret_cast<char*>(*result + 1);
        Blizzard::String::MemCopy((*result)->m_name, path, Blizzard::String::Length(path) + 1);
        (*result)->m_size = 0;

        return 2;
    }

    if (type == 2) {
        return 0;
    }

    if (type == 3) {
        if (Blizzard::Mopaq::SFileOpenFileEx(handle, name, handle ? 0 : flags, &(*result)->m_file)) {
            (*result)->m_name = reinterpret_cast<char*>(*result + 1);
            Blizzard::String::MemCopy((*result)->m_name, path, Blizzard::String::Length(path) + 1);

            return 1;
        }

fail:
        if (*result) {
            (*result)->~SFileImpl();
            Blizzard::Memory::Free(*result);
        }
    }

    *result = NULL;

    return 0;
}

// PROVEN: 3148e2ff IDENTICAL/DIFFERENT
// OFFSET: 0x007A8AF0/0x14049E860
int32_t STORMAPI SFile::FileExists(const char* name) {
    char path[260];
    int32_t type;
    uint32_t fileSize;

    return System_SFile2::FindFile(name, path, sizeof(path), 0, &type, &fileSize, NULL);
}

// OFFSET: 0x007A8B30/0x14049E870
int32_t STORMAPI SFile::ReadFile(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags) {
    SFileImpl* file;
    int32_t result = SFile::OpenEx(archive, name, flags, &file);

    if (!result) {
        return result;
    }

    uint32_t fileSize = SFileGetFileSize(file, NULL);
    uint8_t* buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(fileSize + extra));

    if (!ReadFileInternal(file, buffer, fileSize, -1, size)) {
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

    SFileCloseFile(file);

    return 1;
}

// PROVEN: 0f279594 IDENTICAL/DIFFERENT
// OFFSET: 0x007A8C30/0x14049E9E0
int32_t STORMAPI SFile::LoadFile(const char* name, void** data, uint32_t* size, uint32_t extra) {
    return SFile::ReadFile(NULL, name, data, size, extra, 0);
}

// PROVEN: 8c3d601c IDENTICAL/IDENTICAL
// OFFSET: 0x007A8C50/0x14049EA10
int32_t STORMAPI SFileOpenFile(const char* name, SFileImpl** result) {
    return SFile::OpenEx(NULL, name, 0, result);
}

// PROVEN: 9b884dec IDENTICAL/NONE
// OFFSET: 0x007A8C70
int32_t STORMAPI SFile::IsPatchFile(const char* name) {
    SFileImpl* file;
    int32_t result = SFile::OpenEx(NULL, name, 0, &file);

    if (!result) {
        return result;
    }

    if (!file->m_file) {
        return 0;
    }

    result = Blizzard::Mopaq::SFileIsPatchFile(file->m_file);
    SFileCloseFile(file);

    return result;
}

// OFFSET: 0x007A8CC0/0x14049EA90
int32_t STORMAPI SFile::FileIsLocal(const char* name) {
    SFileImpl* file;

    if (!SFile::OpenEx(NULL, name, 0, &file)) {
        return -1;
    }

    int32_t result = 1;

    if (file->m_file) {
        result = Blizzard::Mopaq::SFileFileIsLocal(file->m_file, false);
        SFileCloseFile(file);
        return result;
    }

    if (file->m_stream) {
        result = Blizzard::Streaming::IsLocal(file->m_stream, 0, SFileGetFileSize(file, NULL));
    }

    SFileCloseFile(file);

    return result;
}

// OFFSET: 0x0079FFD0
bool SFile::IsFileCompletelyLocal(const char* name) {
    int64_t local;
    int64_t total;
    Blizzard::Streaming::GetLocalAmount(name, &local, &total);

    return total > 0 && local == total;
}

// OFFSET: 0x007A4DA0
static char* EscapeURL(const char* src, char* dst, int32_t size) {
    char* out = dst;
    char c = *src;

    if (c) {
        for (int32_t length = 0; length < size;) {
            if (c == ' ') {
                *out++ = '+';
                length++;
            } else if ((c > 0 && c < ' ') || c == ':' || c == '/') {
                char escaped[4];
                Blizzard::String::Format(escaped, sizeof(escaped), "%%%02x", c);

                if (length < 5) {
                    break;
                }

                Blizzard::String::Copy(out, escaped, sizeof(escaped));
                length += 3;
                out += 3;

                if (c == '\n') {
                    c = '\r';
                    continue;
                }
            } else {
                *out++ = c;
                length++;
            }

            c = *++src;

            if (!c) {
                break;
            }
        }
    }

    *out = '\0';

    return dst;
}

// OFFSET: 0x007A4E40
static int32_t GetBuildFromURL(const char* url) {
    int32_t build = -1;
    char* copy = Blizzard::String::Duplicate(url);
    const char* start = Blizzard::String::FindI(copy, "streaming-wow/wow-", 0x7FFFFFFF);

    if (start) {
        char* number = const_cast<char*>(start) + Blizzard::String::Length("streaming-wow/wow-");
        char* end = const_cast<char*>(Blizzard::String::FindI(number, "-internal", 0x7FFFFFFF));

        if (end) {
            *end = '\0';
            build = atoi(number);
        }
    }

    Blizzard::Memory::Free(copy);

    return build;
}

// OFFSET: 0x007A66F0
System_SFile2::ReportCallback::ReportCallback() {
    this->m_shouldBlock = false;
    this->m_timeout = 10000;
    Blizzard::Lock::EventCreate(this->m_event);
}

// OFFSET: [0x007A6770, 0x007A6BE0]
System_SFile2::ReportCallback::~ReportCallback() {
    Blizzard::Lock::EventDestroy(this->m_event);
}

// OFFSET: 0x007FD0B0
int32_t System_SFile2::ReportCallback::GetTimeout() {
    return this->m_timeout;
}

// OFFSET: 0x007A6760
bool System_SFile2::ReportCallback::ShouldBlock() {
    return this->m_shouldBlock;
}

// OFFSET: 0x007FD560
bool System_SFile2::ReportCallback::ReceivedData(const void* data, int32_t size) {
    this->m_data.append(static_cast<const char*>(data), size);
    return true;
}

// OFFSET: 0x007A4D90
void System_SFile2::ReportCallback::Completed(Blizzard::URL::DownloadCallback::Status status) {
    Blizzard::Lock::EventSet(this->m_event);
}

// OFFSET: 0x007A67E0
void SFile::SendBuildReport(int32_t build) {
    System_SFile2::ReportCallback callback;
    callback.m_shouldBlock = true;

    char url[260];
    Blizzard::String::Format(url, sizeof(url), "http://streaming.corp.blizzard.net/wowreport/buildreport.php?build=%d", build);
    Blizzard::URL::DownloadURL(url, &callback);
    Blizzard::Lock::EventWaitForever(callback.m_event);
}

// OFFSET: 0x007A6870
void SFile::ReportManifestURL(const char* url) {
    int32_t build = GetBuildFromURL(url);

    if (build >= 0) {
        SendBuildReport(build);
    }
}

// OFFSET: 0x007A6890
void SFile::EnableBuildReports() {
    Blizzard::Streaming::s_processUrlFunc = ReportManifestURL;
}

// OFFSET: 0x007A7020
static std::string GetUserNameString() {
    char name[256];
    DWORD size = sizeof(name);

    if (!GetUserNameA(name, &size)) {
        strcpy(name, "<unknown>");
    }

    return std::string(name);
}

// OFFSET: 0x007A70B0
static std::string GetComputerNameString() {
    char name[256];
    DWORD size = sizeof(name);

    if (!GetComputerNameA(name, &size)) {
        strcpy(name, "<unknown>");
    }

    return std::string(name);
}

// OFFSET: 0x007A7140
void STORMAPI SFile::SendErrorReport(const char* error, bool block) {
    System_SFile2::ReportCallback callback;
    callback.m_shouldBlock = block;

    std::string computer = GetComputerNameString();
    std::string user = GetUserNameString();
    std::string url;
    url.append("http://streaming.corp.blizzard.net/wowreport/report.php", 55);
    url.append("?", 1);

    char escaped[2048];
    const char* part = Blizzard::String::QuickFormat<1024>("error=%s", EscapeURL(error, escaped, sizeof(escaped) - 1)).m_buffer;
    url.append(part, strlen(part));
    part = Blizzard::String::QuickFormat<1024>("&computer=%s&user=%s", computer.c_str(), user.c_str()).m_buffer;
    url.append(part, strlen(part));

    if (Blizzard::URL::DownloadURL(url.c_str(), &callback)) {
        Blizzard::Lock::EventWaitForever(callback.m_event);
    }
}
