#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <new>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"

// STATIC: %0x00C0C252
static bool s_StreamingFlush = true;

// STATIC: %0x00C0C4A8
Blizzard::File::Filesystem System_File::Stacked::s_streamstack = {
    &System_File::Stacked::s_streamstack,
    NULL,
    {
        System_File::Stacked::PassThrough,
        System_File::Stacked::stream_close,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::stream_dirwalk,
        System_File::Stacked::stream_exists,
        System_File::Stacked::stream_flush,
        System_File::Stacked::stream_getfileinfo,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::stream_open,
        System_File::Stacked::stream_read,
        System_File::Stacked::stream_readp,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::PassThrough,
        System_File::Stacked::stream_unlink,
        System_File::Stacked::PassThrough,
        System_File::Stacked::stream_writep,
        System_File::Stacked::stream_shutdown
    }
};

// STATIC: %0x00D86BA0
System_File::Stacked::LockAndMap* System_File::Stacked::LockAndMap::s_instance;

// STATIC: %0x00D86BA4
bool System_File::Stacked::g_callerGuaranteesStreamingWriteHasProperData;

// STATIC: %0x00D86BF0
static std::map<std::string, std::string>* s_inodes;

// STATIC: %0x00D86C10
int64_t System_File::Stacked::size_local_read;

// STATIC: %0x00D86DA0
static Blizzard::Lock::RMutex s_inodeLock;

// INLINED
FileStack_Streaming::FileStack_Streaming(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) : m_fs(fs), m_parms(parms), m_key(System_File::GetInode(std::string(parms->m_path))), m_unit(NULL) {
}

// OFFSET: %0x0070E820
bool System_File::Stacked::stream_read(Blizzard::File::Filesystem* fs, FileParms* parms) {
    Blizzard::File::StreamingInfo* streaming = parms->m_stream->m_streaming;

    if ((streaming && streaming->m_unit) || !fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x0071C650
void System_File::Stacked::PushStreaming() {
    Blizzard::File::Filesystem* fs = static_cast<Blizzard::File::Filesystem*>(malloc(sizeof(Blizzard::File::Filesystem)));
    Blizzard::String::MemCopy(fs, &s_streamstack, sizeof(Blizzard::File::Filesystem));
    fs->m_next = s_manager;
    s_manager = fs;
    HoistAll();
}

// OFFSET: %0x00726D80
Blizzard::File::StreamRecord* STORMAPI System_File::Stacked::AllocStreamingRecord(const char* path) {
    uint32_t length = path ? strlen(path) : 0;

    Blizzard::File::StreamRecord* record = static_cast<Blizzard::File::StreamRecord*>(Blizzard::Memory::Allocate(sizeof(Blizzard::File::StreamRecord) + sizeof(Blizzard::File::StreamingInfo) + length + 1, 8, __FILE__, __LINE__));
    record->m_streaming = reinterpret_cast<Blizzard::File::StreamingInfo*>(record + 1);
    record->m_pathname = reinterpret_cast<char*>(record->m_streaming + 1);
    new (record->m_streaming) Blizzard::File::StreamingInfo;
    Blizzard::String::Copy(record->m_pathname, path, length + 1);

    return record;
}

// OFFSET: %0x0073AD00
bool System_File::Stacked::stream_flush(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (!parms->m_stream) {
        return true;
    }

    Blizzard::File::StreamingInfo* streaming = parms->m_stream->m_streaming;

    if (!streaming || !streaming->m_unit) {
        return true;
    }

    Blizzard::Storage::StorageUnit* unit = streaming->m_unit;

    if (unit) {
        unit->m_refCount.Increment();
        Blizzard::Storage::Release(unit);
    }

    return true;
}

// OFFSET: %0x0073AD60
FileStack_Streaming::~FileStack_Streaming() {
    Blizzard::Storage::Release(this->m_unit);
}

// OFFSET: %0x0073ADF0
bool System_File::Stacked::stream_writep(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (!parms->m_stream->m_streaming) {
        if (!fs->m_next) {
            return false;
        }

        return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
    }

    bool result = false;

    if (g_callerGuaranteesStreamingWriteHasProperData) {
        Blizzard::Storage::StorageUnit* unit = Blizzard::Streaming::AcquireStorageUnit(parms->m_stream);

        if (unit->m_local) {
            result = unit->m_local->WriteUnaligned(parms->m_buffer, parms->m_pos, parms->m_amount);
        }

        Blizzard::Storage::Release(unit);
    }

    return result;
}

// OFFSET: %0x00740A40
bool StreamDirWalk::Callback(const Blizzard::File::ProcessDirParms& parms) {
    static_cast<StreamDirWalk*>(parms.m_cookie)->m_files[std::string(parms.m_filename)] = parms.m_isDir;
    return true;
}

// OFFSET: %0x00742740
System_File::Stacked::LockAndMap* System_File::Stacked::LockAndMap::Get() {
    if (!s_instance) {
        s_instance = new LockAndMap();
    }

    return s_instance;
}

// OFFSET: %0x007427B0
Blizzard::File::StreamRecord* System_File::Stacked::LockAndMap::GetRecord(const std::string& key, const char* path) {
    Blizzard::Lock::Mutex& lock = Get()->m_mutex.m_mutex;
    Blizzard::Lock::MutexEnter(lock);

    std::map<std::string, Blizzard::File::StreamRecord*>& records = Get()->m_map;
    std::map<std::string, Blizzard::File::StreamRecord*>::iterator it = records.find(key);
    Blizzard::File::StreamRecord* record;

    if (it == records.end()) {
        record = AllocStreamingRecord(path);
        record->m_streaming->m_refCount = 0;
        Blizzard::Lock::MutexCreate(record->m_streaming->m_lock);
        records.insert(std::make_pair(key, record));
    } else {
        record = it->second;
    }

    Blizzard::Lock::MutexLeave(lock);

    return record;
}

// OFFSET: %0x00742920
bool System_File::Stacked::stream_close(Blizzard::File::Filesystem* fs, FileParms* parms) {
    Blizzard::File::StreamRecord* file = parms->m_stream;

    if (s_StreamingFlush) {
        FileParms flush;
        flush.m_verb = offsetof(Blizzard::File::Filesystem, m_verbs.m_flush);
        flush.m_stream = file;

        if (s_manager) {
            s_manager->m_verbs.m_flush(s_manager, &flush);
        }
    }

    if (file->m_streaming && file->m_streaming->m_unit) {
        Blizzard::Lock::ScopedMutex mapLock(LockAndMap::Get()->m_mutex.m_mutex);
        Blizzard::Lock::ScopedMutex fileLock(file->m_streaming->m_lock);

        if (--file->m_streaming->m_refCount > 0) {
            return true;
        }

        LockAndMap::Get()->m_map.erase(file->m_streaming->m_name);
        file->m_streaming->m_unit->FlushCacheEntries(0);
        Blizzard::Storage::Release(file->m_streaming->m_unit);
        file->m_streaming->m_unit = NULL;
    }

    if (file->m_streaming) {
        file->m_streaming->~StreamingInfo();
        Blizzard::Memory::Free(file, 0, __FILE__, __LINE__);
        return true;
    }

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x00742AE0
Blizzard::File::StreamRecord* FileStack_Streaming::InitRecord(Blizzard::File::StreamRecord* record, int64_t size) {
    Blizzard::String::MemCopy(&record->m_streaming->m_info, &record->m_info, sizeof(Blizzard::File::FileInfo));
    record->m_info.m_path = record->m_pathname;
    record->m_info.m_size = size;
    record->m_info.m_createTime = 0;
    record->m_info.m_modTime = 0;
    record->m_info.m_accessTime = 0;
    record->m_info.m_regularFile = true;
    record->m_info.m_attributes = 32;
    record->m_infoValid = true;

    Blizzard::Storage::StorageUnit* unit = Blizzard::Storage::Acquire(this->m_key);
    this->m_unit = unit;
    record->m_streaming->m_unit = unit;
    record->m_streaming->m_name = this->m_key;
    record->m_streaming->m_refCount++;

    return record;
}

// OFFSET: %0x00742D40
std::string System_File::GetInode(const std::string& path) {
    Blizzard::Lock::MutexEnter(s_inodeLock.m_mutex);

    if (!s_inodes) {
        s_inodes = new std::map<std::string, std::string>();
    }

    std::string result;

    if ((*s_inodes)[path].size()) {
        result = (*s_inodes)[path];
    } else {
        char absolute[260];

        if (Blizzard::File::MakeAbsolutePath(path.c_str(), absolute, 260, false)) {
            char normalized[260];
            char* dst = normalized;
            const char* src = absolute;
            uint32_t left = 260;

            while (true) {
                char c = *src++;
                left--;

                if (!c) {
                    *dst = '\0';
                    break;
                }

                *dst++ = c == '/' ? '\\' : c;

                if (!left) {
                    dst[-1] = '\0';
                    break;
                }
            }

            StringToLower(normalized);
            (*s_inodes)[path].assign(normalized, strlen(normalized));
            result = (*s_inodes)[path];
        }
    }

    Blizzard::Lock::MutexLeave(s_inodeLock.m_mutex);

    return result;
}

// OFFSET: %0x00743C90
void System_File::Stacked::LockAndMap::Release() {
    delete s_instance;
    s_instance = NULL;
}

// OFFSET: %0x00745260
bool System_File::Stacked::stream_unlink(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (Blizzard::Streaming::IsStreamedFile(parms->m_path)) {
        return true;
    }

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x007488F0
bool StreamDirWalk::Walk(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) {
    char buffer[1024];

    System_File::Stacked::FileParms walk = *parms;
    walk.m_cookie = this;
    walk.m_dircallback = Callback;

    if (fs->m_next) {
        (*reinterpret_cast<System_File::Stacked::FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + walk.m_verb))(fs->m_next, &walk);
    }

    std::vector<std::string> files = Blizzard::Streaming::FileManifest::GetFileList(parms->m_path);

    for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
        if (m_files.find(*it) == m_files.end()) {
            m_files[*it] = false;
        }
    }

    Blizzard::File::ProcessDirParms dir;
    dir.m_pathRoot = parms->m_path;
    dir.m_cookie = parms->m_cookie;

    for (std::map<std::string, bool>::iterator it = m_files.begin(); it != m_files.end(); ++it) {
        Blizzard::Unicode::ConvertUTF16to8(buffer, sizeof(buffer), Blizzard::StringConversion_CodepageToUTF8::ToNative(it->first.c_str()).m_data, -1, NULL, NULL);
        dir.m_filename = buffer;
        dir.m_isDir = it->second;

        if (!parms->m_dircallback(dir)) {
            return false;
        }
    }

    return true;
}

// OFFSET: %0x00748C40
bool System_File::Stacked::stream_exists(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (!parms->m_path) {
        parms->m_info->m_existence = 0;
        return true;
    }

    std::string key = System_File::GetInode(std::string(parms->m_path));
    Blizzard::Streaming::Manifests::FileItem* item = Blizzard::Streaming::ManifestInfo::Get()->GetFileItem(key);

    if (item) {
        parms->m_info->m_existence = item->m_size ? 1 : 2;
        return true;
    }

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x00748D90
bool System_File::Stacked::stream_getfileinfo(Blizzard::File::Filesystem* fs, FileParms* parms) {
    if (!parms->m_path) {
        if (parms->m_stream) {
            Blizzard::Streaming::Manifests::FileItem* item = NULL;

            if (parms->m_stream->m_streaming) {
                item = Blizzard::Streaming::ManifestInfo::Get()->GetFileItem(parms->m_stream->m_streaming->m_name);
            }

            if (!fs->m_next || !(*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms)) {
                return false;
            }

            if (item) {
                parms->m_info->m_size = item->m_size;
            }
        }

        return true;
    }

    std::string key = System_File::GetInode(std::string(parms->m_path));
    Blizzard::Streaming::Manifests::FileItem* item = Blizzard::Streaming::ManifestInfo::Get()->GetFileItem(key);

    if (item) {
        parms->m_info->m_size = item->m_size;
        parms->m_info->m_path = parms->m_path;
        parms->m_info->m_createTime = 0;
        parms->m_info->m_modTime = 0;
        parms->m_info->m_accessTime = 0;
        parms->m_info->m_regularFile = true;
        parms->m_info->m_existence = item->m_size > 0 ? 1 : 2;
        parms->m_info->m_attributes = 32;
        return true;
    }

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x0074A410
bool System_File::Stacked::stream_dirwalk(Blizzard::File::Filesystem* fs, FileParms* parms) {
    StreamDirWalk walk;
    return walk.Walk(fs, parms);
}

// OFFSET: %0x0074A480
void System_File::Stacked::StreamingShutdown() {
    Blizzard::Streaming::ShutdownStreaming();

    Blizzard::Lock::ScopedMutex lock(LockAndMap::Get()->m_mutex.m_mutex);

    std::map<std::string, Blizzard::File::StreamRecord*>& records = LockAndMap::Get()->m_map;

    for (std::map<std::string, Blizzard::File::StreamRecord*>::iterator it = records.begin(); it != records.end(); ++it) {
        it->second->m_streaming->~StreamingInfo();
        Blizzard::Memory::Free(it->second);
    }
}

// OFFSET: %0x0074A5A0
bool System_File::Stacked::stream_shutdown(Blizzard::File::Filesystem* fs, FileParms* parms) {
    StreamingShutdown();
    LockAndMap::Release();

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}

// OFFSET: %0x0074FD50
bool System_File::Stacked::stream_readp(Blizzard::File::Filesystem* fs, FileParms* parms) {
    Blizzard::File::StreamRecord* file = parms->m_stream;

    if (!file->m_streaming || !file->m_streaming->m_unit) {
        if (!fs->m_next) {
            return false;
        }

        return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
    }

    void* buffer = parms->m_buffer;
    int64_t offset = parms->m_pos;
    Blizzard::Storage::BackingStore* local = file->m_streaming->m_unit->m_local;

    if (local && local->Read(buffer, offset, parms->m_amount)) {
        return true;
    }

    Blizzard::Streaming::StreamRequest request(file, offset, parms->m_amount);

    if (!request.GetLocality()) {
        Blizzard::File::AddToLastErrorStack(2, Blizzard::String::QuickFormat<1024>("%s - fail in GetLocality %s %lld %u ", "System_File::Stacked::stream_readp", file->m_pathname, offset, parms->m_amount).m_buffer, 20);
        return false;
    }

    Blizzard::Storage::g_StorageSystem.EnableWriteback(false);

    if (!request.PrefetchData()) {
        Blizzard::Storage::g_StorageSystem.EnableWriteback(true);
        Blizzard::File::AddToLastErrorStack(2, Blizzard::String::QuickFormat<1024>("%s - fail in PrefetchData %s %lld %u ", "System_File::Stacked::stream_readp", file->m_pathname, offset, parms->m_amount).m_buffer, 20);
        return false;
    }

    if (!request.ReadLocalChunks(buffer)) {
        Blizzard::Storage::g_StorageSystem.EnableWriteback(true);
        Blizzard::File::AddToLastErrorStack(2, Blizzard::String::QuickFormat<1024>("%s - fail in ReadLocalChunks %s %lld %u ", "System_File::Stacked::stream_readp", file->m_pathname, offset, parms->m_amount).m_buffer, 20);
        return false;
    }

    Blizzard::Storage::g_StorageSystem.EnableWriteback(true);

    if (!request.ReadRemoteChunks(buffer)) {
        Blizzard::File::AddToLastErrorStack(104, Blizzard::String::QuickFormat<1024>("%s - fail in ReadRemoteChunks %s %lld %u ", "System_File::Stacked::stream_readp", file->m_pathname, offset, parms->m_amount).m_buffer, 20);
        return false;
    }

    return true;
}

// OFFSET: %0x00756AD0
Blizzard::File::StreamRecord* FileStack_Streaming::StreamingOpen() {
    Blizzard::Streaming::Manifests::FileItem* item = Blizzard::Streaming::ManifestInfo::Get()->GetFileItem(this->m_key);

    if (!item) {
        return NULL;
    }

    Blizzard::File::StreamRecord* record = System_File::Stacked::LockAndMap::GetRecord(this->m_key, this->m_parms->m_path);
    Blizzard::Lock::Mutex& lock = record->m_streaming->m_lock;
    Blizzard::Lock::MutexEnter(lock);

    if (record->m_streaming->m_refCount > 0) {
        record->m_streaming->m_refCount++;
        Blizzard::Lock::MutexLeave(lock);
        return record;
    }

    this->m_unit = Blizzard::Storage::Acquire(this->m_key);

    if (!this->m_unit->m_local || !this->m_unit->m_local->IsMatch(this->m_parms->m_path, item->m_size, atoi(item->m_fileVersion.c_str()))) {
        Blizzard::Storage::PartialFile* local = Blizzard::Storage::PartialFile::CreateShared(this->m_fs->m_next, this->m_parms->m_path, item->m_size, item->GetContentId());

        if (!local) {
            Blizzard::File::AddToLastErrorStack(0, "FileStack_Streaming::StreamingOpen - Unable to open the storage file", 20);
            Blizzard::Lock::MutexLeave(lock);
            return NULL;
        }

        this->m_unit->SetLocal(local, item->m_size);
        this->m_unit->SetCacheSource();
        this->m_unit->SetHttpSource(this->m_key, item->m_size);
    }

    Blizzard::File::StreamRecord* result = this->InitRecord(record, item->m_size);
    Blizzard::Lock::MutexLeave(lock);

    return result;
}

// OFFSET: %0x007580C0
bool System_File::Stacked::stream_open(Blizzard::File::Filesystem* fs, FileParms* parms) {
    FileStack_Streaming stack(fs, parms);
    Blizzard::File::StreamRecord* file = stack.StreamingOpen();

    if (file || stack.m_unit) {
        parms->m_stream = file;
        return file != NULL;
    }

    if (!fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}
