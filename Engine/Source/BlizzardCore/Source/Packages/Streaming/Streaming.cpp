#include <cstdint>
#include <cctype>
#include <cstring>
#include <deque>
#include <string>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

// STATIC: #0x00AB93E8
static Blizzard::Using::Use s_useFile("Blizzard::File");

// STATIC: 0x00D06E3A
bool Blizzard::Streaming::s_throttleProgress = true;

// STATIC: 0x00EBA233/0x140DAACD3
bool Blizzard::Streaming::s_streamingReady;

// STATIC: 0x00EBA230
bool Blizzard::Streaming::s_streamingWrite;

// STATIC: %0x00D86BB5
bool Blizzard::Streaming::s_isStreamingTrial;

// STATIC: 0x00EBA240
Blizzard::Streaming::RequestHandler* Blizzard::Streaming::s_requestHandler;

// STATIC: 0x00EBA818
Blizzard::Streaming::DownloadProgress Blizzard::Streaming::s_progress;

// STATIC: #0x00B38038
Blizzard::Time::Timestamp Blizzard::Streaming::s_elapsedSinceStart;

// OFFSET: 0x007A7F30/0x14049D7B0
bool Blizzard::Streaming::ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms) {
    System_SFile2::ScanDirCookie* cookie = static_cast<System_SFile2::ScanDirCookie*>(parms.m_cookie);

    char path[512];
    Blizzard::String::Format(path, sizeof(path), "%s%s%s", cookie->m_root, cookie->m_prefix, parms.m_filename);
    char* relative = &path[cookie->m_rootLength];

    if (parms.m_isDir) {
        if (Blizzard::String::EqualI(parms.m_filename, ".svn")) {
            return true;
        }

        Blizzard::String::ForceTrailingSeparator(path, sizeof(path), '\\');

        if (System_SFile2::s_dataPath[0] && Blizzard::String::Equal(&path[cookie->m_rootLength], System_SFile2::s_dataPath)) {
            return true;
        }

        const char* prefix = cookie->m_prefix;
        cookie->m_prefix = &path[cookie->m_rootLength];
        Blizzard::File::ProcessDirFast(path, cookie, Blizzard::Streaming::ScanDirectoryForManifest, false);
        cookie->m_prefix = prefix;

        return true;
    }

    const char* extension = Blizzard::String::FindExtensionWithLeadingDot(path);
    char* fullPath = Blizzard::String::Duplicate(path);

    if (*extension && Blizzard::String::EqualI(extension, ".MPQ") && System_SFile2::s_fileHash.find(std::string(relative)) == System_SFile2::s_fileHash.end()) {
        System_SFile2::FileHashEntry entry;
        entry.m_path = fullPath;
        entry.m_type = 2;
        System_SFile2::s_fileHash.insert(System_SFile2::FileHash::value_type(std::string(relative), entry));
    } else if (System_SFile2::s_fileHash.find(std::string(relative)) == System_SFile2::s_fileHash.end()) {
        System_SFile2::FileHashEntry entry;
        entry.m_path = fullPath;
        entry.m_type = 0;
        System_SFile2::s_fileHash.insert(System_SFile2::FileHash::value_type(std::string(relative), entry));
    }

    Blizzard::Memory::Free(fullPath);

    return true;
}

// PROVEN: 14c027df IDENTICAL/IDENTICAL
// OFFSET: 0x007ABF30/0x1404A1E00
bool Blizzard::Streaming::IsStreamingMode() {
    return Blizzard::Streaming::s_streamingReady;
}

// INLINED
Blizzard::Streaming::DownloadProgress::DownloadProgress() {
    this->ResetTime();
}

// INLINED
Blizzard::Streaming::DownloadProgress::~DownloadProgress() {
    this->m_lastTime = 0;
    this->m_callback = NULL;
    memset(this->m_samples, 0, sizeof(this->m_samples));
}

// INLINED
void Blizzard::Streaming::DownloadProgress::ResetTime() {
    this->m_lastTime = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
}

// PROVEN: 9c1c3fbf IDENTICAL/NONE
// OFFSET: 0x007ABF50
void Blizzard::Streaming::DownloadProgress::Sample(uint32_t time) {
    if (++this->m_index >= 51) {
        this->m_index = 0;
    }

    this->m_samples[this->m_index].m_time = 0;
    this->m_samples[this->m_index].m_bytes = this->m_streamed;
    this->m_samples[this->m_index].m_time = time;
}

// OFFSET: 0x007ABFB0/0x1404A1E20
int32_t Blizzard::Streaming::DownloadProgress::GetRate() {
    int32_t index = this->m_index;
    int32_t first;

    if (index == 50) {
        first = 0;
    } else {
        first = index + 1;

        if (!index) {
            index = 51;
        }
    }

    int32_t last = index - 1;

    while (!this->m_samples[first].m_time) {
        if (first == last) {
            break;
        }

        if (static_cast<uint32_t>(++first) > 50) {
            first = 0;
        }
    }

    for (; !this->m_samples[last].m_time; last--) {
        if (first == last) {
            break;
        }

        if (!last) {
            last = 51;
        }
    }

    int64_t bytes = this->m_samples[last].m_bytes - this->m_samples[first].m_bytes;
    int32_t time = this->m_samples[last].m_time - this->m_samples[first].m_time;

    if (first == last) {
        int32_t next = last + 1;

        if (next == 51) {
            next = 0;
        }

        if (this->m_samples[next].m_time) {
            bytes = this->m_samples[next].m_bytes;
            time = this->m_samples[next].m_time - this->m_lastTime;
        }
    }

    if (bytes > 0 && time) {
        return static_cast<int32_t>(1000 * (bytes / static_cast<uint32_t>(time)));
    }

    return 0;
}

// OFFSET: 0x007C3B60
void Blizzard::Streaming::DownloadProgress::FireCallback() {
    if (this->m_enabled && (!s_throttleProgress || static_cast<uint32_t>(Blizzard::Time::Milliseconds()) - this->m_lastTime >= 5000)) {
        PROGRESSCALLBACK callback = this->m_callback;

        if (callback) {
            callback(static_cast<uint32_t>(this->m_total), static_cast<uint32_t>(this->m_total >> 32), static_cast<uint32_t>(this->m_downloaded), static_cast<uint32_t>(this->m_downloaded >> 32), static_cast<uint32_t>(this->m_streamed), static_cast<uint32_t>(this->m_streamed >> 32), this->GetRate());
        }
    }
}

// OFFSET: 0x007C8000/0x1404C3390
std::string Blizzard::Streaming::GetBandwidthString() {
    return std::string(Blizzard::String::QuickFormat<1024>("b,%d", s_progress.GetRate()).m_buffer);
}

// OFFSET: 0x007B4090
Blizzard::Streaming::ScopedWorkingDirectory::ScopedWorkingDirectory() {
    this->m_path[0] = '\0';
    this->m_saved = Blizzard::File::GetWorkingDirectory(this->m_path, sizeof(this->m_path));
    this->m_changed = false;
}

// PROVEN: 1cead704 IDENTICAL/NONE
// OFFSET: 0x007E99C0/0x1404F52D0
Blizzard::Streaming::ScopedWorkingDirectory::~ScopedWorkingDirectory() {
    if (this->m_saved && this->m_changed && !Blizzard::File::SetWorkingDirectory(this->m_path)) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to restore working directory", 1);
    }
}

// OFFSET: 0x007B40F0
bool Blizzard::Streaming::ScopedWorkingDirectory::ChangeToDataPath(const char* path) {
    if (!path) {
        return false;
    }

    char absolute[260];

    if (!Blizzard::File::MakeAbsolutePath(path, absolute, sizeof(absolute), false) || Blizzard::File::Exists(absolute) != 2) {
        return false;
    }

    this->m_changed = Blizzard::File::SetWorkingDirectory(absolute);

    return this->m_changed;
}

// OFFSET: 0x007BC0C0
bool Blizzard::Streaming::ScopedWorkingDirectory::ChangeToManifestPath(const char* manifest) {
    if (!manifest || (s_httpPrefix && !_strnicmp(manifest, s_httpPrefix, strlen(s_httpPrefix)))) {
        return true;
    }

    char absolute[260];

    if (!Blizzard::File::MakeAbsolutePath(manifest, absolute, sizeof(absolute), false)) {
        return false;
    }

    char directory[260];
    int32_t length = static_cast<int32_t>(Blizzard::String::FindFilename(absolute) - absolute) + 1;
    Blizzard::String::Copy(directory, absolute, length <= 260 ? length : 260);
    this->m_changed = Blizzard::File::SetWorkingDirectory(directory);

    return this->m_changed;
}

// OFFSET: 0x007A5E50/0x14049ACA0
Blizzard::Streaming::RequestHandler::~RequestHandler() {
}

// OFFSET: 0x007C6310/0x1404C14E0
Blizzard::Streaming::RequestProcessor::RequestProcessor() {
    this->m_paused = false;
    this->m_shutdown = false;
    Blizzard::Lock::EventCreate(this->m_event);
    this->m_thread = NULL;
    Blizzard::Lock::MutexCreate(this->m_mutex);
}

// INLINED
Blizzard::Streaming::RequestProcessor::~RequestProcessor() {
    Blizzard::Lock::MutexDestroy(this->m_mutex);
    Blizzard::Lock::EventDestroy(this->m_event);
}

// PROVEN: ac69aa15 IDENTICAL/NONE
// OFFSET: 0x007E1680/0x1404E79E0
uint32_t Blizzard::Streaming::RequestProcessor::ThreadProc(void* param) {
    RequestProcessor* processor = static_cast<RequestProcessor*>(param);

    while (!processor->m_shutdown) {
        if (processor->HasWork()) {
            processor->ProcessNext();
        }
    }

    return 0;
}

// OFFSET: 0x007EB7A0/0x1404EC9E0
bool Blizzard::Streaming::RequestProcessor::StartThread() {
    if (this->m_thread && this->m_thread->m_state >= 0) {
        return true;
    }

    this->m_shutdown = false;
    Blizzard::Thread::Create(ThreadProc, this, &this->m_thread, "StreamingProcessThread", 0, 0);

    return true;
}

// OFFSET: 0x007B45B0/0x1404AA1C0
bool Blizzard::Streaming::RequestProcessor::HasWork() {
    if (this->m_shutdown) {
        return false;
    }

    Blizzard::Lock::MutexEnter(this->m_mutex);
    bool empty = this->m_queue.size() == 0;
    Blizzard::Lock::MutexLeave(this->m_mutex);

    if (!empty && !this->m_paused) {
        return true;
    }

    while ((!Blizzard::Lock::EventWait(100, this->m_event) || this->m_paused) && !this->m_shutdown) {
    }

    if (this->m_shutdown) {
        return false;
    }

    Blizzard::Lock::MutexEnter(this->m_mutex);
    empty = this->m_queue.size() == 0;
    Blizzard::Lock::MutexLeave(this->m_mutex);

    return !empty;
}

// OFFSET: 0x007BFEA0
void Blizzard::Streaming::RequestProcessor::PopFront() {
    Blizzard::Lock::MutexEnter(this->m_mutex);
    this->m_queue.pop_front();
    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007DC630
bool Blizzard::Streaming::RequestProcessor::ProcessNext() {
    Blizzard::Lock::MutexEnter(this->m_mutex);
    Blizzard::Storage::Request* request = this->m_queue.front();
    Blizzard::Lock::MutexLeave(this->m_mutex);

    if (request->m_cancelled) {
        Blizzard::Lock::MutexEnter(this->m_mutex);
        this->m_queue.pop_front();
        Blizzard::Lock::MutexLeave(this->m_mutex);

        if (!request->m_refCount.Decrement()) {
            delete request;
        }

        return true;
    }

    int32_t result;

    if (request->m_unit->m_local) {
        Blizzard::Storage::ChunkRange range;
        range.m_offset = request->m_offset;
        range.m_length = request->m_size;
        result = request->m_unit->FetchRequest(request, &range);
    } else {
        result = 2;
    }

    if (!result) {
        this->PopFront();
        request->OnWriteComplete(false);
    } else if (result == 2) {
        request->m_state = Blizzard::Storage::Request::STATE_NO_UNIT;
        Blizzard::Lock::EventSet(request->m_event);
        return false;
    }

    return true;
}

// OFFSET: 0x007E16B0
void Blizzard::Streaming::RequestProcessor::Shutdown() {
    this->m_shutdown = true;
    Blizzard::Thread::WaitForever(this->m_thread);

    if (this->m_thread) {
        Blizzard::Thread::Close(this->m_thread);
        this->m_thread = NULL;
    }

    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    for (std::deque<Blizzard::Storage::Request*>::iterator it = this->m_queue.begin(); it != this->m_queue.end(); ++it) {
        Blizzard::Storage::Request* request = *it;

        if (!request->m_refCount.Decrement()) {
            delete request;
        }
    }

    this->m_queue.clear();
}

// INLINED
Blizzard::Streaming::DefaultRequestHandler::DefaultRequestHandler() {
    this->m_processor = NULL;
    Blizzard::Lock::MutexCreate(this->m_mutex);
}

// OFFSET: [0x007E37E0, 0x007E4810/0x1404EC9B0]
Blizzard::Streaming::DefaultRequestHandler::~DefaultRequestHandler() {
    this->Stop();
    delete this->m_processor;
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// OFFSET: 0x007D5250/0x1404D5C60
void Blizzard::Streaming::DefaultRequestHandler::Queue(Blizzard::Storage::Request* request) {
    request->m_refCount.Increment();
    Blizzard::Lock::Mutex& mutex = this->m_processor->m_mutex;
    Blizzard::Lock::MutexEnter(mutex);
    this->m_processor->m_queue.push_back(request);

    if (!this->m_processor->m_paused) {
        Blizzard::Lock::EventSet(this->m_processor->m_event);
    }

    Blizzard::Lock::MutexLeave(mutex);
}

// OFFSET: 0x007EB820/0x1404ECA80
bool Blizzard::Streaming::DefaultRequestHandler::Start(bool paused) {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    if (!this->m_processor) {
        this->m_processor = new RequestProcessor();
    }

    if (this->m_processor->StartThread()) {
        this->m_processor->m_paused = paused;

        if (!this->m_processor->m_paused) {
            Blizzard::Lock::EventSet(this->m_processor->m_event);
        }

        return true;
    }

    delete this->m_processor;

    return false;
}

// OFFSET: 0x007E17C0/0x1404E7BF0
void Blizzard::Streaming::DefaultRequestHandler::Stop() {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);
    this->m_processor->Shutdown();
}

// PROVEN: 34134552 IDENTICAL/NONE
// OFFSET: 0x007AC2C0/0x1404A2110
void Blizzard::Streaming::DefaultRequestHandler::Pause() {
    this->m_processor->m_paused = true;
}

// PROVEN: 7da30b81 IDENTICAL/NONE
// OFFSET: 0x007AC2D0/0x1404A2120
void Blizzard::Streaming::DefaultRequestHandler::Resume() {
    this->m_processor->m_paused = false;
}

// OFFSET: 0x007EDC60
void Blizzard::Streaming::SetDefaultRequestHandler() {
    // STATIC: 0x00EBAC50
    static DefaultRequestHandler s_defaultHandler;

    s_defaultHandler.Start(false);
    s_requestHandler = &s_defaultHandler;
}

// OFFSET: 0x007F42B0/0x140501C40
bool Blizzard::Streaming::InitializeStreaming(const char* manifest, const char* dataPath, RequestHandler* handler) {
    if (s_streamingReady) {
        return true;
    }

    ScopedWorkingDirectory directory;

    if (!directory.ChangeToDataPath(dataPath) && !directory.ChangeToManifestPath(manifest)) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to change working directory", 1);
        return false;
    }

    if (!ManifestInfo::Initialize(manifest)) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Could not load manifest", 20);
        return false;
    }

    if (!Blizzard::Storage::Initialize()) {
        ManifestInfo::Get()->Destroy();
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Could not initialize storage", 20);
        return false;
    }

    if (handler) {
        s_requestHandler = handler;
    } else {
        SetDefaultRequestHandler();
    }

    System_File::Stacked::Push(&System_File::Stacked::s_streamstack);
    Blizzard::Mopaq::SFileEnableRawRead(false);
    s_streamingReady = true;

    return true;
}

// OFFSET: 0x007F0750/0x1404FD170
void Blizzard::Streaming::ShutdownStreaming() {
    RequestHandler* handler = s_requestHandler;

    if (handler) {
        s_streamingReady = false;
        s_requestHandler = NULL;
        handler->Stop();
    }

    Blizzard::Storage::Shutdown();
    ManifestInfo::Get()->Destroy();
}

// OFFSET: 0x007E3310/0x1404E9EA0
bool Blizzard::Streaming::IsStreamedFile(const char* path) {
    if (!s_streamingReady || !path) {
        return false;
    }

    std::string key = System_File::GetInode(std::string(path));

    return Blizzard::Storage::Exists(key);
}

// OFFSET: 0x007EDDD0
bool Blizzard::Streaming::IsInManifest(const char* path) {
    if (!s_streamingReady || !path) {
        return false;
    }

    std::string key = System_File::GetInode(std::string(path));

    return ManifestInfo::Get()->GetFileItem(key) != NULL;
}

// OFFSET: 0x007EDCC0
bool Blizzard::Streaming::GetStreamedFileSize(const char* path, int64_t* size) {
    if (size) {
        *size = 0;
    }

    if (!s_streamingReady || !path) {
        return false;
    }

    std::string key = System_File::GetInode(std::string(path));
    Manifests::FileItem* item = ManifestInfo::Get()->GetFileItem(key);

    if (!item) {
        return false;
    }

    if (size) {
        *size = item->m_size;
    }

    return true;
}

// OFFSET: 0x007E3400
int64_t Blizzard::Streaming::GetIsLocalAmount(const char* path, int64_t offset, int64_t size) {
    if (!path) {
        return size;
    }

    std::string key = System_File::GetInode(std::string(path));
    Blizzard::Storage::StorageUnit* unit = Blizzard::Storage::Acquire(key, false);

    if (!unit) {
        return size;
    }

    int64_t local = unit->m_local ? unit->m_local->GetLocalBytes(offset, size) : 0;
    Blizzard::Storage::Release(unit);

    return local;
}

// OFFSET: 0x007E3570
void Blizzard::Streaming::GetLocalAmount(const char* path, int64_t* local, int64_t* total) {
    *local = 0;
    *total = 0;

    Blizzard::Lock::MutexEnter(Blizzard::Storage::s_storageMutex);
    std::string key = System_File::GetInode(std::string(path));

    if (Blizzard::Storage::IsInitialized() && Blizzard::Storage::s_StorageMap->find(key) != Blizzard::Storage::s_StorageMap->end()) {
        std::map<std::string, Blizzard::Storage::StorageUnit*>::iterator it = Blizzard::Storage::s_StorageMap->find(key);

        if (it != Blizzard::Storage::s_StorageMap->end() && it->second->m_local) {
            *local = it->second->m_local->GetTotalLocalSize();
            *total = it->second->m_logicalFileSize;
        }
    }

    Blizzard::Lock::MutexLeave(Blizzard::Storage::s_storageMutex);
}

// OFFSET: 0x007DA020/0x1404DC7D0
int32_t Blizzard::Streaming::IsLocal(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size) {
    if (!s_streamingReady) {
        return 1;
    }

    Blizzard::Storage::StorageUnitRef ref(AcquireStorageUnit(file));

    if (ref.m_unit && size) {
        return ref.m_unit->IsAvailable(offset, size) ? 1 : 0;
    }

    return 1;
}

// OFFSET: 0x007DA140
int64_t Blizzard::Streaming::GetIsLocalAmount(Blizzard::File::StreamRecord* file) {
    Blizzard::Storage::StorageUnit* unit = AcquireStorageUnit(file);

    if (!unit) {
        return -1;
    }

    int64_t local = unit->m_local ? unit->m_local->GetLocalBytes(0, unit->m_logicalFileSize) : 0;
    Blizzard::Storage::Release(unit);

    return local;
}

// OFFSET: 0x007DA210
bool Blizzard::Streaming::MakeRemote(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size) {
    Blizzard::Storage::StorageUnit* unit = AcquireStorageUnit(file);

    if (!unit) {
        return false;
    }

    bool result = unit->m_local ? unit->m_local->Invalidate(offset, size) : false;
    Blizzard::Storage::Release(unit);

    return result;
}

// OFFSET: 0x007DC200
bool Blizzard::Streaming::MakeLocal(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size, CompletionCallback* callback, const int64_t* chunk) {
    if (!s_requestHandler) {
        if (callback) {
            callback->m_success = false;
            callback->Release();
        }

        return false;
    }

    Blizzard::Storage::StorageUnit* unit = AcquireStorageUnit(file);
    Blizzard::Storage::Request* request = Blizzard::Storage::Request::Create(unit, offset, size);
    Blizzard::Storage::Release(unit);

    if (request->m_callback) {
        request->m_callback->m_success = false;
        request->m_callback->Release();
    }

    request->m_callback = callback;

    if (callback) {
        callback->m_refCount.Increment();
    }

    request->m_chunk = chunk;

    if (request->m_state != Blizzard::Storage::Request::STATE_LOCAL) {
        if (request->m_state != Blizzard::Storage::Request::STATE_PENDING) {
            request->Release();
            return unit == NULL;
        }

        s_requestHandler->Queue(request);
    }

    request->Release();

    return true;
}

// OFFSET: 0x007DC3A0
bool Blizzard::Streaming::MakeLocalBlocking(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size, MakeLocalParams* params, const int64_t* chunk) {
    int32_t retries = 0;

    if (!s_requestHandler) {
        return false;
    }

    Blizzard::Storage::StorageUnit* unit = AcquireStorageUnit(file);
    Blizzard::Storage::Request* request = Blizzard::Storage::Request::Create(unit, offset, size);
    Blizzard::Storage::Release(unit);
    request->m_chunk = chunk;

    if (request->m_state == Blizzard::Storage::Request::STATE_LOCAL) {
        request->Release();
        return true;
    }

    bool result;

    if (request->m_state == Blizzard::Storage::Request::STATE_PENDING) {
        s_requestHandler->Queue(request);

        while (!Blizzard::Lock::EventWait(3600000, request->m_event) || request->m_state != Blizzard::Storage::Request::STATE_LOCAL) {
            int32_t state = request->m_state;

            if (state == Blizzard::Storage::Request::STATE_FAILED || state == Blizzard::Storage::Request::STATE_CANCELLED) {
                break;
            }

            if (retries++ >= 3) {
                request->Cancel();
                break;
            }
        }

        if (params) {
            int32_t state = request->m_state;

            if (state == Blizzard::Storage::Request::STATE_LOCAL) {
                params->m_status = 0;
            } else {
                params->m_status = (static_cast<uint32_t>(state - 2) > 1) + 1;
            }
        }

        int32_t state = request->m_state;
        result = state == Blizzard::Storage::Request::STATE_LOCAL || state == Blizzard::Storage::Request::STATE_CANCELLED;
    } else {
        if (params) {
            params->m_status = unit != NULL ? 3 : 0;
        }

        result = unit == NULL;
    }

    request->Release();

    return result;
}
