#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

// STATIC: #0x00AB935C
static System_File::UsingFile s_usingFile;

// STATIC: 0x00D07270
Blizzard::File::Filesystem System_File::Stacked::s_initstack = {
    &System_File::Stacked::s_initstack,
    NULL,
    {
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init,
        System_File::Stacked::file_init
    }
};

// STATIC: 0x00D0726C
Blizzard::File::Filesystem* System_File::Stacked::s_manager = &System_File::Stacked::s_initstack;

// STATIC: 0x00D07388
Blizzard::File::Filesystem System_File::Stacked::s_nullstack = {
    &System_File::Stacked::s_nullstack,
    NULL,
    {
        System_File::Stacked::null_cd,
        System_File::Stacked::null_close,
        System_File::Stacked::null_create,
        System_File::Stacked::null_cwd,
        System_File::Stacked::null_dirwalk,
        System_File::Stacked::null_exists,
        System_File::Stacked::null_flush,
        System_File::Stacked::null_getfileinfo,
        System_File::Stacked::null_getfreespace,
        System_File::Stacked::null_getpos,
        System_File::Stacked::null_getrootchars,
        System_File::Stacked::null_isabspath,
        System_File::Stacked::null_isreadonly,
        System_File::Stacked::null_makeabspath,
        System_File::Stacked::null_mkdir,
        System_File::Stacked::null_move,
        System_File::Stacked::null_copy,
        System_File::Stacked::null_open,
        System_File::Stacked::null_read,
        System_File::Stacked::null_readp,
        System_File::Stacked::null_rmdir,
        System_File::Stacked::null_setcachemode,
        System_File::Stacked::null_seteof,
        System_File::Stacked::null_setfileinfo,
        System_File::Stacked::null_setpos,
        System_File::Stacked::null_unlink,
        System_File::Stacked::null_write,
        System_File::Stacked::null_writep,
        System_File::Stacked::null_shutdown
    }
};

// STATIC: 0x00D071E0
Blizzard::File::Filesystem System_File::Stacked::s_basestack = {
    &System_File::Stacked::s_basestack,
    NULL,
    {
        System_File::SupportsSparse,
        System_File::SetWorkingDirectory,
        System_File::Close,
        System_File::Create,
        System_File::GetWorkingDirectory,
        System_File::ProcessDirFast,
        System_File::Exists,
        System_File::Flush,
        System_File::GetFileInfo,
        System_File::GetFreeSpace,
        System_File::GetPos,
        System_File::GetRootChars,
        System_File::IsAbsolutePath,
        System_File::IsReadOnly,
        System_File::MakeAbsolutePath,
        System_File::CreateDirectory,
        System_File::Move,
        System_File::Copy,
        System_File::Open,
        System_File::Read,
        System_File::ReadP,
        System_File::RemoveDirectory,
        System_File::SetCacheMode,
        System_File::SetEOF,
        System_File::SetAttributes,
        System_File::SetPos,
        System_File::SetSparse,
        System_File::ZeroRange,
        System_File::Delete,
        System_File::Write,
        System_File::WriteP,
        System_File::Shutdown
    }
};

// STATIC: #0x009E5630
const Blizzard::File::FileInfo System_File::s_noinfo = { NULL, 0, -1 };

// STATIC: %0x00D86D94
Blizzard::Thread::LocalObject<System_File::FileError> System_File::s_error;

// STATIC: #0x00AB9130
int64_t System_File::Stacked::s_throttleRate = 56888;

// STATIC: #0x00B332C8
int64_t System_File::Stacked::s_throttleDebt;

// STATIC: #0x00B38024
int32_t System_File::Stacked::s_throttleDelay;

// STATIC: #0x00AB9138
Blizzard::File::Filesystem System_File::Stacked::s_throttlestack = {
    &System_File::Stacked::s_throttlestack,
    NULL,
    {
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::throttle_read,
        System_File::Stacked::throttle_read,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::null_shutdown
    }
};

bool System_File::Stacked::null_cd(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_close(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_create(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_cwd(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_dirwalk(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_exists(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_flush(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_getfileinfo(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_getfreespace(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_getpos(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_getrootchars(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_isabspath(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_isreadonly(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_makeabspath(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_mkdir(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_move(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_copy(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_open(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_read(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_readp(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_rmdir(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_setcachemode(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_seteof(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_setfileinfo(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_setpos(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_unlink(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_write(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_writep(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return false;
}

bool System_File::Stacked::null_shutdown(Blizzard::File::Filesystem* fs, FileParms* parms) {
    return true;
}

bool System_File::Shutdown(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    return true;
}

// INLINED
System_File::FileError::FileError() {
    this->m_errorStack = NULL;
    this->m_lastError = 0;
    this->unk_0004 = 0;
}

// INLINED
System_File::FileError::~FileError() {
    Blizzard::Debug::FreeErrorStack(this->m_errorStack);
    this->m_errorStack = NULL;
}

// PROVEN: 10e3c9cf IDENTICAL/DIFFERENT
// OFFSET: 0x007AC4A0/0x1404A2240
bool Blizzard::File::Close(StreamRecord* file) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_close);
    parms.m_stream = file;

    if (manager) {
        return manager->m_verbs.m_close(manager, &parms);
    }

    return false;
}

// PROVEN: e286eadd IDENTICAL/DIFFERENT
// OFFSET: 0x007AC530/0x1404A2280
bool Blizzard::File::Delete(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_unlink);
    parms.m_path = const_cast<char*>(path);

    if (manager) {
        return manager->m_verbs.m_unlink(manager, &parms);
    }

    return false;
}

// PROVEN: 07b945b3 IDENTICAL/NONE
// OFFSET: 0x007AC5D0
int32_t Blizzard::File::Exists(const char* path) {
    return Blizzard::File::Exists(System_File::Stacked::s_manager, path);
}

// PROVEN: 77c80729 IDENTICAL/NONE
// OFFSET: 0x007AC570
int32_t Blizzard::File::Exists(Filesystem* fs, const char* path) {
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_exists);
    parms.m_path = const_cast<char*>(path);
    parms.m_info = &parms.m_info2;

    if (path && fs && fs->m_verbs.m_exists(fs, &parms)) {
        return parms.m_info->m_existence;
    }

    return 0;
}

// OFFSET: 0x007AC5F0/0x1404A2320
bool Blizzard::File::GetFileInfo(const char* path, FileInfo* info) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getfileinfo);
    parms.m_path = const_cast<char*>(path);
    parms.m_stream = NULL;
    parms.m_has.m_all = 0;
    parms.m_want.m_all = 0xFFFFFFFF;
    parms.m_info = &parms.m_info2;

    if (manager) {
        bool result = manager->m_verbs.m_getfileinfo(manager, &parms);

        if (result) {
            *info = *parms.m_info;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007AC670/0x1404A23E0
Blizzard::File::FileInfo* Blizzard::File::GetFileInfo(StreamRecord* file) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getfileinfo);
    parms.m_path = NULL;
    parms.m_stream = file;
    parms.m_has.m_all = 0;
    parms.m_want.m_all = 0xFFFFFFFF;
    parms.m_info = NULL;

    if (manager && manager->m_verbs.m_getfileinfo(manager, &parms)) {
        return parms.m_info;
    }

    return const_cast<FileInfo*>(&System_File::s_noinfo);
}

// OFFSET: 0x007AC6E0/0x1404A2450
bool Blizzard::File::GetFileInfo(StreamRecord* file, FileInfo* info) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getfileinfo);
    parms.m_path = NULL;
    parms.m_stream = file;
    parms.m_info = NULL;

    if (manager) {
        bool result = manager->m_verbs.m_getfileinfo(manager, &parms);

        if (result) {
            *info = *parms.m_info;
        }

        return result;
    }

    return false;
}

// OFFSET: #0x00427330
bool Blizzard::File::GetFileInfo(Filesystem* fs, const char* path, FileInfo* info) {
    System_File::Stacked::FileParms parms;
    parms.m_path = const_cast<char*>(path);
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getfileinfo);
    parms.m_stream = NULL;
    parms.m_has.m_all = 0;
    parms.m_want.m_all = 0xFFFFFFFF;
    parms.m_info = &parms.m_info2;

    if (fs) {
        bool result = fs->m_verbs.m_getfileinfo(fs, &parms);

        if (result) {
            *info = *parms.m_info;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007AC750
bool Blizzard::File::GetPos(StreamRecord* file, int64_t* offset) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getpos);
    parms.m_stream = file;
    parms.m_pos = 0;
    parms.m_posMode = 1;

    if (manager) {
        bool result = manager->m_verbs.m_getpos(manager, &parms);

        if (result) {
            *offset = parms.m_pos;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007AC7C0
int32_t Blizzard::File::GetRootChars(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_getrootchars);
    parms.m_path = const_cast<char*>(path);
    parms.m_pos = 0;

    bool result = manager ? manager->m_verbs.m_getrootchars(manager, &parms) : false;

    return result ? static_cast<int32_t>(parms.m_pos) : 0;
}

// OFFSET: 0x007AC820
bool Blizzard::File::GetWorkingDirectory(char* buffer, int32_t buffersize) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_cwd);
    parms.m_cwdbuf = buffer;
    parms.m_cwdbuflen = buffersize;
    *buffer = '\0';

    if (manager) {
        return manager->m_verbs.m_cwd(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007AC870
bool Blizzard::File::IsAbsolutePath(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_isabspath);
    parms.m_path = const_cast<char*>(path);

    if (manager) {
        return manager->m_verbs.m_isabspath(manager, &parms);
    }

    return false;
}

// PROVEN: 69cdcb61 IDENTICAL/DIFFERENT
// OFFSET: 0x007AC8B0/0x1404A2640
bool Blizzard::File::MakeAbsolutePath(const char* rel, char* result, int32_t resultsize, bool canonicalize) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_path = const_cast<char*>(rel);
    parms.m_cwdbuf = result;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_makeabspath);
    parms.m_cwdbuflen = resultsize;
    parms.m_makeCanonical = canonicalize;

    if (manager) {
        return manager->m_verbs.m_makeabspath(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007AC910
bool Blizzard::File::Move(const char* src, const char* dst) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_move);
    parms.m_path = const_cast<char*>(src);
    parms.m_newpath = const_cast<char*>(dst);

    if (manager) {
        return manager->m_verbs.m_move(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007AC960
bool Blizzard::File::Copy(const char* src, const char* dst, bool overwrite) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_path = const_cast<char*>(src);
    parms.m_verb = offsetof(Filesystem, m_verbs.m_copy);
    parms.m_newpath = const_cast<char*>(dst);
    parms.m_overwrite = overwrite;

    if (manager) {
        return manager->m_verbs.m_copy(manager, &parms);
    }

    return false;
}

// INLINED
bool Blizzard::File::Open(const char* filename, int32_t flags, StreamRecord*& file) {
    return Blizzard::File::Open(System_File::Stacked::s_manager, filename, flags, file);
}

// OFFSET: 0x007AC9B0
bool Blizzard::File::Open(Filesystem* fs, const char* filename, int32_t flags, StreamRecord*& file) {
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_open);
    parms.m_stream = NULL;
    parms.m_path = const_cast<char*>(filename);
    parms.m_mode = flags;

    if (fs) {
        bool result = fs->m_verbs.m_open(fs, &parms);

        if (result) {
            file = parms.m_stream;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007ACA30/0x1404A27A0
bool Blizzard::File::ProcessDirFast(const char* path, void* cookie, ProcessDirCallback callback, bool treatSymlinksAsFiles) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_cookie = cookie;
    parms.m_path = const_cast<char*>(path);
    parms.m_verb = offsetof(Filesystem, m_verbs.m_dirwalk);
    parms.m_dircallback = callback;
    parms.m_sorted = false;
    parms.m_mode = treatSymlinksAsFiles;

    if (manager) {
        return manager->m_verbs.m_dirwalk(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007ACA90
bool Blizzard::File::Read(StreamRecord* file, void* data, int32_t* count) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_read);
    parms.m_buffer = data;
    parms.m_amount = *count;
    *count = 0;

    if (manager) {
        bool result = manager->m_verbs.m_read(manager, &parms);

        if (result) {
            *count = parms.m_amount;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007ACB00
bool Blizzard::File::Read(StreamRecord* file, void* data, int64_t offset, int32_t* count, int32_t streamFlags) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_buffer = data;
    parms.m_pos = offset;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_readp);
    parms.m_amount = *count;
    parms.m_streamFlags = streamFlags;
    *count = 0;

    if (manager) {
        bool result = manager->m_verbs.m_readp(manager, &parms);

        if (result) {
            *count = parms.m_amount;
        }

        return result;
    }

    return false;
}

// OFFSET: #0x00427790
bool Blizzard::File::Read(Filesystem* fs, StreamRecord* file, void* data, int64_t offset, int32_t* count) {
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_buffer = data;
    parms.m_pos = offset;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_readp);
    parms.m_amount = *count;

    if (fs) {
        bool result = fs->m_verbs.m_readp(fs, &parms);

        if (result) {
            *count = parms.m_amount;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007ACB80
bool Blizzard::File::RemoveDirectory(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_rmdir);
    parms.m_path = const_cast<char*>(path);
    parms.m_recursive = false;

    if (manager) {
        return manager->m_verbs.m_rmdir(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007ACBD0
bool Blizzard::File::SetAttributes(const char* path, int32_t attributes) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_path = const_cast<char*>(path);
    parms.m_verb = offsetof(Filesystem, m_verbs.m_setfileinfo);
    parms.m_has.m_all = 4;
    parms.m_info2.m_attributes = attributes;
    parms.m_info = &parms.m_info2;

    if (manager) {
        return manager->m_verbs.m_setfileinfo(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007ACC30
bool Blizzard::File::SetEOF(StreamRecord* file, int64_t offset, int32_t whence) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_pos = offset;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_seteof);
    parms.m_posMode = whence;

    if (manager) {
        return manager->m_verbs.m_seteof(manager, &parms);
    }

    return false;
}

// PROVEN: acdabe66 IDENTICAL/NONE
// OFFSET: 0x007ACC90
bool Blizzard::File::SetSparse(StreamRecord* file, bool zeroFill) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_setsparse);
    parms.m_stream = file;
    parms.m_zeroFillAll = zeroFill;

    if (manager) {
        return manager->m_verbs.m_setsparse(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007ACCE0
bool Blizzard::File::SetModificationTime(StreamRecord* file, Blizzard::Time::Timestamp time) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_setfileinfo);
    parms.m_stream = file;
    parms.m_info = &parms.m_info2;
    parms.m_info2.m_modTime = time;
    parms.m_has.m_all = 0x10;

    if (manager) {
        return manager->m_verbs.m_setfileinfo(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007AC450
bool Blizzard::File::SupportsSparse(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_supportssparse);
    parms.m_path = const_cast<char*>(path);
    parms.m_supportsSparse = false;

    bool result = manager ? manager->m_verbs.m_supportssparse(manager, &parms) : false;

    if (!result) {
        return false;
    }

    return parms.m_supportsSparse;
}

// OFFSET: 0x007ACD40
bool Blizzard::File::SetPos(StreamRecord* file, int64_t offset, int32_t whence) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_pos = offset;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_setpos);
    parms.m_posMode = whence;

    if (manager) {
        return manager->m_verbs.m_setpos(manager, &parms);
    }

    return false;
}

// PROVEN: d62e4460 IDENTICAL/DIFFERENT
// OFFSET: 0x007ACDA0/0x1404A2A80
bool Blizzard::File::SetWorkingDirectory(const char* path) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_cd);
    parms.m_path = const_cast<char*>(path);

    if (manager) {
        return manager->m_verbs.m_cd(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007ACDE0
bool Blizzard::File::Write(StreamRecord* file, const void* data, int32_t* count) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_amount = *count;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_write);
    parms.m_buffer = const_cast<void*>(data);
    parms.m_pos = -1;

    if (manager) {
        bool result = manager->m_verbs.m_write(manager, &parms);

        if (result) {
            *count = parms.m_amount;
        }

        return result;
    }

    return false;
}

// OFFSET: 0x007ACE50
bool Blizzard::File::Write(StreamRecord* file, const void* data, int64_t offset, int32_t* count, int32_t streamFlags) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_stream = file;
    parms.m_buffer = const_cast<void*>(data);
    parms.m_pos = offset;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_writep);
    parms.m_amount = *count;
    parms.m_streamFlags = streamFlags;

    if (manager) {
        bool result = manager->m_verbs.m_writep(manager, &parms);

        if (result) {
            *count = parms.m_amount;
        }

        return result;
    }

    return false;
}

// PROVEN: 7633edd1 IDENTICAL/DIFFERENT
// OFFSET: 0x007AD080/0x1404A2CD0
bool Blizzard::File::IsDirectory(const char* path) {
    return Exists(path) == 2;
}

// OFFSET: 0x007AD0B0
bool Blizzard::File::IsFile(const char* path) {
    return Exists(path) == 1;
}

// OFFSET: 0x007AD0D0/0x1404A2DB0
bool Blizzard::File::SimpleGlob(const char* name, const char* pattern) {
    char p = *pattern;

    if (!p) {
        return !*name;
    }

    for (;;) {
        char c = *name;

        if (!c && p != '*') {
            return false;
        }

        char pc = p;
        pattern++;

        if (p == '*') {
            break;
        }

        if (p == '?') {
            if (!c) {
                return false;
            }
        } else {
            if (p == '\\' && *pattern) {
                pc = *pattern++;
            }

            if (pc != c && tolower(c) != tolower(pc)) {
                return false;
            }
        }

        p = *pattern;
        name++;

        if (!p) {
            return !*name;
        }
    }

    while (*pattern == '*') {
        pattern++;
    }

    char next = *pattern;

    if (!next) {
        return true;
    }

    if (next != '?' && next != '\\') {
        if (!*name) {
            return false;
        }

        while (next != *name) {
            if (!*++name) {
                return false;
            }
        }
    }

    if (!*name) {
        return false;
    }

    for (;;) {
        if (SimpleGlob(name, pattern)) {
            return true;
        }

        if (!*++name) {
            return false;
        }
    }
}

// PROVEN: e226f43a IDENTICAL/DIFFERENT
// OFFSET: 0x007B4BE0/0x1404AA710
bool Blizzard::File::CreateDirectory(const char* path, bool recursive) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_mkdir);
    parms.m_path = const_cast<char*>(path);
    parms.m_recursive = recursive;
    parms.m_grantEveryone = false;

    if (manager) {
        return manager->m_verbs.m_mkdir(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007AC4E0
bool Blizzard::File::CreateDirectory(const char* path, bool recursive, bool grantEveryone) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_mkdir);
    parms.m_path = const_cast<char*>(path);
    parms.m_recursive = recursive;
    parms.m_grantEveryone = grantEveryone;

    if (manager) {
        return manager->m_verbs.m_mkdir(manager, &parms);
    }

    return false;
}

// INLINED
bool Blizzard::File::Flush(StreamRecord* file) {
    Filesystem* manager = System_File::Stacked::s_manager;
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Filesystem, m_verbs.m_flush);
    parms.m_stream = file;

    if (manager) {
        return manager->m_verbs.m_flush(manager, &parms);
    }

    return false;
}

// OFFSET: 0x007B4C70/0x1404AA7D0
void System_File::Shutdown() {
    System_File::Stacked::FileParms parms;
    parms.m_verb = offsetof(Blizzard::File::Filesystem, m_verbs.m_shutdown);

    if (!System_File::Stacked::s_manager) {
        return;
    }

    System_File::Stacked::s_manager->m_verbs.m_shutdown(System_File::Stacked::s_manager, &parms);

    while (System_File::Stacked::s_manager) {
        Blizzard::File::Filesystem* next = System_File::Stacked::s_manager->m_next;
        free(System_File::Stacked::s_manager);
        System_File::Stacked::s_manager = next;
    }
}

#define HOIST(verb) \
    if (!s_manager->m_verbs.verb) { \
        Blizzard::File::Filesystem* fs = s_manager; \
        while (fs->m_next) { \
            fs->m_verbs.verb = fs->m_next->m_verbs.verb; \
            fs = fs->m_next; \
        } \
        fs->m_verbs.verb = NULL; \
    }

// OFFSET: %0x00716950
void System_File::Stacked::HoistAll() {
    HOIST(m_cd);
    HOIST(m_close);
    HOIST(m_create);
    HOIST(m_cwd);
    HOIST(m_dirwalk);
    HOIST(m_exists);
    HOIST(m_flush);
    HOIST(m_getfileinfo);
    HOIST(m_getfreespace);
    HOIST(m_getpos);
    HOIST(m_getrootchars);
    HOIST(m_isabspath);
    HOIST(m_isreadonly);
    HOIST(m_makeabspath);
    HOIST(m_mkdir);
    HOIST(m_move);
    HOIST(m_copy);
    HOIST(m_open);
    HOIST(m_read);
    HOIST(m_readp);
    HOIST(m_rmdir);
    HOIST(m_setcachemode);
    HOIST(m_seteof);
    HOIST(m_setfileinfo);
    HOIST(m_setpos);
    HOIST(m_unlink);
    HOIST(m_write);
    HOIST(m_writep);
    HOIST(m_shutdown);
}

#undef HOIST

// OFFSET: #0x0042D440
bool System_File::Stacked::file_init(Blizzard::File::Filesystem* fs, FileParms* parms) {
    s_manager = static_cast<Blizzard::File::Filesystem*>(malloc(sizeof(Blizzard::File::Filesystem)));
    Blizzard::String::MemCopy(s_manager, &s_nullstack, sizeof(Blizzard::File::Filesystem));

    Blizzard::File::Filesystem* base = static_cast<Blizzard::File::Filesystem*>(malloc(sizeof(Blizzard::File::Filesystem)));
    Blizzard::String::MemCopy(base, &s_basestack, sizeof(Blizzard::File::Filesystem));
    base->m_next = s_manager;
    s_manager = base;
    HoistAll();

    if (!s_manager) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(s_manager) + parms->m_verb))(s_manager, parms);
}

// OFFSET: 0x007E48C0/0x1404ED5E0
Blizzard::Debug::ErrorStackRecord* Blizzard::File::GetLastErrorStack() {
    System_File::FileError& error = System_File::s_error;

    return error.m_errorStack;
}

// OFFSET: 0x007E48D0/0x1404ED600
void Blizzard::File::SetLastErrorStack(Blizzard::Debug::ErrorStackRecord* stack) {
    System_File::FileError& error = System_File::s_error;

    if (error.m_errorStack != stack) {
        Blizzard::Debug::FreeErrorStack(error.m_errorStack);
    }

    error.m_errorStack = stack;
}

// OFFSET: 0x007E4900/0x1404ED650
void Blizzard::File::AddToLastErrorStack(int32_t errCode, const char* errStr, int32_t category) {
    System_File::FileError& error = System_File::s_error;
    Blizzard::Debug::ErrorStackRecord* stack = Blizzard::Debug::AddErrorToStack(error.m_errorStack, errCode, errStr ? errStr : "", category);

    if (stack) {
        SetLastErrorStack(stack);
    }
}

// OFFSET: 0x007E48A0/0x1404ED5C0
void Blizzard::File::SetLastError(int32_t errorCode) {
    System_File::FileError& error = System_File::s_error;
    error.m_lastError = errorCode;
}

// OFFSET: 0x007BC5A0
bool System_File::LoadFile(Blizzard::File::StreamRecord* file, void** data, uint32_t* bytes) {
    Blizzard::File::FileInfo info;

    if (!Blizzard::File::GetFileInfo(file, &info)) {
        return false;
    }

    if (info.m_size < 0) {
        return false;
    }

    uint32_t size = static_cast<uint32_t>(info.m_size);

    if (info.m_size <= 0) {
        return false;
    }

    *data = Blizzard::Memory::Allocate(size + 1);
    int32_t count = size;

    if (Blizzard::File::Read(file, *data, 0, &count, 0) && count == size) {
        static_cast<char*>(*data)[size] = '\0';

        if (bytes) {
            *bytes = size;
        }

        return true;
    }

    Blizzard::Memory::Free(*data);

    return false;
}

// PROVEN: c6dcb0f2 IDENTICAL/NONE
// OFFSET: 0x007BFEF0
bool System_File::LoadFile(const char* filename, void** data, uint32_t* bytes, int32_t flags) {
    Blizzard::File::StreamRecord* file;

    if (!Blizzard::File::Open(filename, flags | 0x1001, file)) {
        return false;
    }

    bool result = LoadFile(file, data, bytes);
    Blizzard::File::Close(file);

    return result;
}

// OFFSET: 0x007ABA50/0x1404A2CB0
bool System_File::Stacked::PassThrough(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: 0x007C40A0/0x1404BE8E0
bool System_File::Stacked::throttle_read(Blizzard::File::Filesystem* fs, FileParms* parms) {
    Blizzard::Time::Timestamp start = Blizzard::Time::GetTimestamp();
    bool result = fs->m_next ? (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms) : false;
    Blizzard::Time::Timestamp end = Blizzard::Time::GetTimestamp();
    Throttle(parms->m_amount, end - start);

    return result;
}

// OFFSET: 0x007BC810
bool Blizzard::File::RemoveDirectoryAndContents_Internal::Callback(const ProcessDirParms& parms) {
    State* state = static_cast<State*>(parms.m_cookie);
    char path[1024];

    if (parms.m_isDir) {
        Blizzard::String::Format(path, sizeof(path), "%s%s/", parms.m_pathRoot, parms.m_filename);
        Blizzard::File::ProcessDirFast(path, state, Callback, true);

        if (!Blizzard::File::RemoveDirectory(path)) {
            state->m_success = false;
        }

        return true;
    }

    Blizzard::String::Format(path, sizeof(path), "%s%s", parms.m_pathRoot, parms.m_filename);

    if (state->m_removeReadOnly) {
        Blizzard::File::SetAttributes(path, 0x20);
    }

    if (Blizzard::Streaming::IsStreamedFile(path)) {
        return true;
    }

    if (!Blizzard::File::Delete(path)) {
        state->m_success = false;
    }

    return true;
}

// OFFSET: #0x00450970
bool Blizzard::File::RemoveDirectoryAndContents_Internal::State::Remove(const char* path, bool removeReadOnly) {
    this->m_removeReadOnly = removeReadOnly;
    this->m_success = true;

    return Blizzard::File::ProcessDirFast(path, this, Callback, true) && this->m_success;
}

// OFFSET: #0x004518C0
bool Blizzard::File::RemoveDirectoryAndContents(const char* path, bool removeReadOnly) {
    if (Exists(path) != 2) {
        return false;
    }

    char buffer[1024];
    Blizzard::String::Copy(buffer, path, sizeof(buffer));
    Blizzard::String::ForceTrailingSeparator(buffer, sizeof(buffer), Blizzard::String::FirstPathSeparator(buffer));

    RemoveDirectoryAndContents_Internal::State state = {};

    if (!state.Remove(buffer, removeReadOnly)) {
        return false;
    }

    return RemoveDirectory(buffer);
}

// INLINED
System_File::UsingFile::UsingFile() : Blizzard::Using::Use("Blizzard::File") {
    System_Using::Intern(this->m_name)->shutdownproc = System_File::Shutdown;
    Blizzard::Using::DependsOn("Blizzard::File", "Blizzard::Memory");
    Blizzard::Using::DependsOn("Blizzard::File", "Blizzard::ThreadLocalStorage");
}
