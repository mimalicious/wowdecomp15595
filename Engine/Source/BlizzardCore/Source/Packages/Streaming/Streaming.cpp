#include <cstdint>
#include <cctype>
#include <cstring>
#include <deque>
#include <string>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
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

// STATIC: #0x00AB7AC8
static uint8_t s_clientInitialized = 1;

// STATIC: %0x00AF2FA8
static const int32_t s_queueOrder[6] = { 0, 1, 0, 2, 0, 1 };

// STATIC: %0x00AF2FC0
static const int32_t s_queueGroupBase[2] = { 0, 4 };

// STATIC: %0x00C0C251
bool Blizzard::Streaming::s_backgroundPreloading = true;

// STATIC: %0x00C0C253
static bool s_throttleProgress = true;

// STATIC: %0x00D8130C
static Blizzard::Lock::Event s_cmdForegroundEvent;

// STATIC: %0x00D81310
static Blizzard::Lock::Mutex s_cmdMutex;

// STATIC: %0x00D81328
static Blizzard::Lock::Event s_cmdPartnerEvent[2];

// STATIC: %0x00D81330
static Blizzard::Lock::Event s_pauseEvent;

// STATIC: %0x00D81334
static Blizzard::Streaming::FILE_THREAD_CMD* s_CmdProcess[2];

// STATIC: %0x00D8133C
static std::deque<Blizzard::Streaming::FILE_THREAD_CMD>* s_CmdQueue_Streaming[Blizzard::Streaming::MAX_CMD_QUEUE];

// STATIC: %0x00D8135C
static Blizzard::Lock::Mutex s_pauseMutex;

// STATIC: %0x00D81378
static bool s_paused;

// STATIC: %0x00D8137C
static Blizzard::Lock::Event s_cmdEvent;

// STATIC: %0x00D86BB4
bool Blizzard::Streaming::s_streamingReady;

// STATIC: %0x00D86BB5
bool Blizzard::Streaming::s_isStreamingTrial;

// STATIC: %0x00D86BB6
volatile bool Blizzard::Streaming::s_threadShutdown;

// STATIC: %0x00D86BB8
static volatile int32_t s_threadExited;

// STATIC: %0x00D86BBC
static volatile int32_t s_threadDownloading;

// STATIC: %0x00D86BC4
Blizzard::Streaming::StreamingX::DATAREQUIRECALLBACK Blizzard::Streaming::StreamingX::g_StreamingDataRequireCallBack;

// STATIC: %0x00D86BC8
Blizzard::Streaming::StreamingX::ISENABLEP2PCALLBACK Blizzard::Streaming::StreamingX::g_IsEnableP2PCallBack;

// STATIC: %0x00D86BCC
int32_t Blizzard::Streaming::StreamingX::g_lowCommandInterval;

// STATIC: %0x00D86BD0
int32_t Blizzard::Streaming::StreamingX::g_maxLowCommandInterval;

// STATIC: %0x00D86BD4
int32_t Blizzard::Streaming::StreamingX::g_stepLowCommandInterval;

// STATIC: %0x00D86BD8
Blizzard::Lock::Atomic Blizzard::Streaming::StreamRequest::globalRequestId;

// STATIC: %0x00D86BDC
Blizzard::Lock::Atomic Blizzard::Streaming::StreamRequest::concurrentRequests;

// STATIC: %0x00D86E28
Blizzard::Streaming::DownloadProgress Blizzard::Streaming::s_progress;

// STATIC: %0x00D871E0
static int32_t s_queueRotation[2];

// STATIC: #0x00B38038
Blizzard::Time::Timestamp Blizzard::Streaming::s_elapsedSinceStart;

// STATIC: #0x00B38040
int64_t Blizzard::Streaming::s_statBytesRead;

// STATIC: #0x00B38048
int64_t Blizzard::Streaming::s_statBytesDownloaded;

// STATIC: #0x00B38050
int64_t Blizzard::Streaming::s_statBytesLocal;

// STATIC: #0x00B38058
int32_t Blizzard::Streaming::s_statRetries;

// STATIC: #0x00B38060
char Blizzard::Streaming::s_statFrameTimes[260];

// OFFSET: #0x00424280
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

// OFFSET: #0x00428010
bool CheckIfStreamingAndTrial() {
    return Blizzard::Streaming::s_streamingReady && Blizzard::Streaming::s_isStreamingTrial;
}

// OFFSET: #0x00428800
void StringToLower(char* str) {
    if (!str) {
        return;
    }

    for (; *str; str++) {
        *str = tolower(*str);
    }
}

// OFFSET: #0x00437A00
void Blizzard::Time::GetElapsedSinceStart(bool start) {
    // STATIC: #0x00B38A48
    static Blizzard::Time::Timestamp s_startTime = Blizzard::Time::GetTimestamp();

    if (!start) {
        Blizzard::Streaming::s_elapsedSinceStart = Blizzard::Time::GetTimestamp() - s_startTime;
    }
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

// OFFSET: #0x00438300
void Blizzard::Streaming::DownloadProgress::ResetTime() {
    this->m_lastTime = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
}

// OFFSET: %0x0075FC10
void ShutdownStreamingIfTrial() {
    if (CheckIfStreamingAndTrial()) {
        GetStreamingManagerSingleton()->Shutdown();
    }
}

// OFFSET: #0x0041E4E0
void SetClientInitialized(uint8_t initialized) {
    s_clientInitialized = initialized;
}

// OFFSET: %0x0070EC30
int32_t Blizzard::Streaming::GetIdleThread() {
    for (int32_t thread = 0; thread < 2; thread++) {
        if (((1 << thread) & s_threadExited) && !((1 << thread) & s_threadDownloading)) {
            return thread;
        }
    }

    return -1;
}

// OFFSET: %0x0070EC60
int32_t Blizzard::Streaming::StreamingX::GetLowCommandIntervalStep() {
    int32_t step = 0;

    if (g_stepLowCommandInterval > 0) {
        int32_t left = g_maxLowCommandInterval - g_lowCommandInterval;

        if (left > 0) {
            step = g_stepLowCommandInterval;

            if (left < 70 * g_stepLowCommandInterval) {
                step = 2 * g_stepLowCommandInterval;
            }

            if (left < 50 * g_stepLowCommandInterval) {
                step += g_stepLowCommandInterval;
            }

            if (left < 30 * g_stepLowCommandInterval) {
                step += g_stepLowCommandInterval;
            }

            if (left < 10 * g_stepLowCommandInterval) {
                step += g_stepLowCommandInterval;
            }
        } else if (g_maxLowCommandInterval == g_lowCommandInterval) {
            g_lowCommandInterval = g_maxLowCommandInterval;
        }
    }

    return step;
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

// OFFSET: %0x0070ED20
int32_t Blizzard::Streaming::DownloadProgress::GetRate() {
    uint32_t last = this->m_index;
    uint32_t first;

    if (last == 50) {
        first = 0;
    } else {
        first = last + 1;

        if (!last) {
            last = 51;
        }
    }

    uint32_t end = last - 1;

    while (!this->m_samples[first].m_time) {
        if (first == end) {
            break;
        }

        if (++first > 50) {
            first = 0;
        }
    }

    for (; !this->m_samples[end].m_time; end--) {
        if (first == end) {
            break;
        }

        if (!end) {
            end = 51;
        }
    }

    int64_t bytes = this->m_samples[end].m_bytes - this->m_samples[first].m_bytes;
    int32_t time = this->m_samples[end].m_time - this->m_samples[first].m_time;

    if (first == end) {
        uint32_t next = end + 1;

        if (next == 51) {
            next = 0;
        }

        if (this->m_samples[next].m_time) {
            bytes = this->m_samples[next].m_bytes;
            time = this->m_samples[next].m_time - this->m_lastTime;
        }
    }

    if (bytes > 0) {
        return static_cast<int32_t>(1000 * (bytes / time));
    }

    return 0;
}

// OFFSET: %0x00715FD0
bool Blizzard::Streaming::IsPreloadInProgress() {
    Blizzard::Lock::MutexEnter(s_cmdMutex);

    for (int32_t i = 0; i < 2; i++) {
        if (s_CmdProcess[i] && s_CmdProcess[i]->m_type == 1) {
            Blizzard::Lock::MutexLeave(s_cmdMutex);
            return true;
        }
    }

    Blizzard::Lock::MutexLeave(s_cmdMutex);
    return false;
}

// OFFSET: [%0x00716020, %0x00716060]
void Blizzard::Streaming::WaitForLoads() {
    for (;;) {
        Blizzard::Lock::MutexEnter(s_cmdMutex);

        bool busy = false;

        for (int32_t i = 0; i < 2; i++) {
            if (s_CmdProcess[i] && s_CmdProcess[i]->m_type == 1) {
                busy = true;
                break;
            }
        }

        Blizzard::Lock::MutexLeave(s_cmdMutex);

        if (!busy) {
            return;
        }

        Blizzard::Process::Sleep(1);
    }
}

// OFFSET: %0x00716100
Blizzard::Streaming::StreamRequest::StreamRequest(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size) {
    this->m_file = file;
    this->m_offset = offset;
    this->m_size = size;
    this->m_ranges = NULL;
    this->m_count = 0;

    int16_t tag;

    do {
        tag = static_cast<int16_t>(globalRequestId.Increment());
    } while (!tag);

    concurrentRequests.Increment();
    this->m_tag = tag;
}

// OFFSET: %0x0071DAA0
int32_t Blizzard::Streaming::IsLocal(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size) {
    if (!s_streamingReady) {
        return 1;
    }

    if (!file->m_streaming || !file->m_streaming->m_unit) {
        return 1;
    }

    Blizzard::Storage::StorageUnit* unit = file->m_streaming->m_unit;

    if (!unit || (unit->m_local && unit->m_local->IsCompleted())) {
        return 1;
    }

    int32_t count;
    Blizzard::Storage::Range* ranges = Blizzard::Storage::GetRange(unit, offset, size, &count, false);

    if (!ranges || !count) {
        return 1;
    }

    int32_t result = 1;

    for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
        switch (range->m_state) {
            case 1:
            case 3:
                result = 2;
                break;

            case 2:
                result = 0;
                count = 0;
                break;
        }
    }

    Blizzard::Memory::Free(ranges);

    return result;
}

// OFFSET: %0x0071DB60
int64_t Blizzard::Streaming::GetIsLocalAmount(Blizzard::Storage::StorageUnit* unit, int64_t offset, uint32_t size) {
    uint32_t local = size;

    if (unit) {
        int32_t count;
        Blizzard::Storage::Range* ranges = Blizzard::Storage::GetRange(unit, offset, size, &count, false);

        if (ranges && count) {
            local = 0;

            for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
                if (!range->m_state) {
                    local += range->m_length;
                }
            }

            Blizzard::Memory::Free(ranges);
        }
    }

    return local;
}

// OFFSET: %0x0071DBC0
void Blizzard::Streaming::StreamRequest::ReadLocalRange(Blizzard::Storage::Range* first, Blizzard::Storage::Range* last, void* buffer) {
    if (!first) {
        return;
    }

    int32_t size = static_cast<int32_t>(last->m_offset + last->m_length - first->m_offset);

    if (this->m_file->m_streaming->m_unit->ReadLocal(static_cast<uint8_t*>(buffer) + static_cast<int32_t>(first->m_offset - this->m_offset), first->m_offset, size)) {
        System_File::Stacked::size_local_read += size;
    } else {
        for (Blizzard::Storage::Range* range = first; range <= last; range++) {
            range->m_state = 2;
        }
    }
}

// OFFSET: %0x0071DC30
bool Blizzard::Streaming::StreamRequest::ReadLocalChunks(void* buffer) {
    Blizzard::Storage::Range* first = NULL;
    Blizzard::Storage::Range* last = NULL;

    for (Blizzard::Storage::Range* range = this->m_ranges; range != this->m_ranges + this->m_count; range++) {
        if (!range->m_state) {
            if (!last || last->m_offset + last->m_length != range->m_offset || last - first >= 15) {
                this->ReadLocalRange(first, last, buffer);
                first = range;
            }

            last = range;
        }
    }

    this->ReadLocalRange(first, last, buffer);

    return true;
}

// OFFSET: %0x0071DCB0
bool Blizzard::Streaming::StreamRequest::ReadRemoteChunks(void* buffer) {
    bool result = true;
    Blizzard::Storage::Range* range = this->m_ranges;

    for (int32_t count = this->m_count; count > 0; count--, range++) {
        if (range->m_state) {
            if (!Blizzard::Storage::Read(this->m_file->m_streaming->m_unit, static_cast<uint8_t*>(buffer) + static_cast<int32_t>(range->m_offset - this->m_offset), range->m_offset, range->m_length)) {
                result = false;
            }

            if (!range->m_state) {
                System_File::Stacked::size_local_read += range->m_length;
            }
        }

        if (!result) {
            break;
        }
    }

    return result;
}

// OFFSET: %0x0071DD50
bool Blizzard::Streaming::StreamRequest::GetLocality() {
    this->m_ranges = Blizzard::Storage::GetRange(this->m_file->m_streaming->m_unit, this->m_offset, this->m_size, &this->m_count, true);
    return this->m_ranges && this->m_count;
}

// OFFSET: %0x00724BA0
Blizzard::Streaming::FileCompletionCallback::FileCompletionCallback() {
    Blizzard::Lock::EventCreate(this->m_event);
}

// OFFSET: %0x00724BF0
void Blizzard::Streaming::FileCompletionCallback::Complete() {
    Blizzard::Lock::EventSet(this->m_event);
}

// OFFSET: %0x00724C00
Blizzard::Streaming::FileCompletionCallback::~FileCompletionCallback() {
    Blizzard::Lock::EventDestroy(this->m_event);
}

// OFFSET: %0x00724C30
void Blizzard::Streaming::DownloadProgress::FireCallback() {
    if (!this->m_enabled) {
        return;
    }

    if (s_throttleProgress && Blizzard::Time::Milliseconds() - this->m_lastTime < 5000) {
        return;
    }

    PROGRESSCALLBACK callback = this->m_callback;

    if (callback) {
        callback(static_cast<uint32_t>(this->m_total), static_cast<uint32_t>(this->m_total >> 32), static_cast<uint32_t>(this->m_downloaded), static_cast<uint32_t>(this->m_downloaded >> 32), static_cast<uint32_t>(this->m_streamed), static_cast<uint32_t>(this->m_streamed >> 32), this->GetRate());
    }
}

// OFFSET: %0x00726FB0
bool Blizzard::Streaming::ResetMakeLocal(MakeLocalOption option) {
    Blizzard::Lock::MutexEnter(s_cmdMutex);

    std::deque<FILE_THREAD_CMD>& queue = *s_CmdQueue_Streaming[option];

    for (std::deque<FILE_THREAD_CMD>::iterator it = queue.begin(); it != queue.end(); ++it) {
        if (it->m_completion && it->m_type == 2) {
            it->m_completion->m_result = static_cast<int32_t>(s_CmdQueue_Streaming[option]->size());
            it->m_completion->m_done = true;
            Blizzard::Lock::EventSet(*it->m_completion->m_event);
            it->m_completion = NULL;
        }

        if (it->m_callback) {
            it->m_callback->m_success = false;
            it->m_callback->Release();
            it->m_callback = NULL;
        }

        if (it->m_type) {
            it->m_type = 0;

            if (it->m_range) {
                Blizzard::Storage::SetRangeAs(it->m_unit, it->m_range->m_chunkOffset, it->m_range->m_chunkLength, Blizzard::Storage::ChunkState::CHUNK_NONE);
            }
        }
    }

    Blizzard::Lock::MutexLeave(s_cmdMutex);

    return true;
}

// OFFSET: %0x00727170
bool Blizzard::Streaming::RemoveCompletion(Completion* completion, int32_t queue) {
    if (!s_CmdQueue_Streaming[queue]->size()) {
        return false;
    }

    Blizzard::Lock::MutexEnter(s_cmdMutex);

    std::deque<FILE_THREAD_CMD>& commands = *s_CmdQueue_Streaming[queue];

    for (std::deque<FILE_THREAD_CMD>::iterator it = commands.end(); it != commands.begin();) {
        --it;

        if (it->m_completion == completion) {
            it->m_completion = NULL;
            Blizzard::Lock::MutexLeave(s_cmdMutex);
            return true;
        }
    }

    Blizzard::Lock::MutexLeave(s_cmdMutex);
    return false;
}

// OFFSET: %0x007272A0
bool Blizzard::Streaming::PopCommand(FILE_THREAD_CMD* command, int32_t queue) {
    if (!s_CmdQueue_Streaming[queue]->size()) {
        return false;
    }

    *command = s_CmdQueue_Streaming[queue]->front();
    s_CmdQueue_Streaming[queue]->pop_front();

    return true;
}

// OFFSET: %0x00727340
bool Blizzard::Streaming::SelectQueue(FILE_THREAD_CMD* command, int32_t group, int32_t* queue) {
    if (group >= 1 && !s_backgroundPreloading) {
        return false;
    }

    int32_t base = s_queueGroupBase[group];

    for (int32_t i = 0; i < 6; i++) {
        *queue = base + s_queueOrder[s_queueRotation[group]];
        s_queueRotation[group] = (s_queueRotation[group] + 1) % 6;

        if (PopCommand(command, *queue)) {
            return true;
        }
    }

    return false;
}

// OFFSET: %0x007273C0
int32_t Blizzard::Streaming::GetNextCommand(FILE_THREAD_CMD** command, int32_t thread) {
    FILE_THREAD_CMD* next = new FILE_THREAD_CMD();
    int32_t queue = MAX_CMD_QUEUE;

    if (SelectQueue(next, 0, &queue) || PopCommand(next, queue = 3) || SelectQueue(next, 1, &queue)) {
        StreamingX::g_lowCommandInterval = 0;
    } else {
        queue = 7;

        if (!PopCommand(next, 7)) {
            delete next;
            return MAX_CMD_QUEUE;
        }
    }

    s_CmdProcess[thread] = next;
    *command = next;

    return queue;
}

// OFFSET: %0x00727490
void Blizzard::Streaming::AddTotal(int32_t size) {
    s_progress.m_total += size;
    s_progress.FireCallback();
}

// OFFSET: %0x007274B0
void Blizzard::Streaming::AddDownloaded(int32_t size) {
    s_progress.m_downloaded += size;
    s_progress.FireCallback();
}

// OFFSET: %0x00729270
std::string Blizzard::Streaming::GetBandwidthString() {
    return std::string(Blizzard::String::QuickFormat<1024>("b,%d", s_progress.GetRate()).m_buffer);
}

// OFFSET: %0x00736230
void Blizzard::Streaming::QueueMakeLocal(int16_t tag, Blizzard::File::StreamRecord* file, Blizzard::Storage::Range* range, MakeLocalOption option, Completion* completion, CompletionCallback* callback) {
    if (s_threadShutdown) {
        return;
    }

    FILE_THREAD_CMD command;
    command.m_type = 1;
    command.m_tag = tag;
    command.m_retries = 0;
    command.m_unit = AcquireStorageUnit(file);

    if (!command.m_unit) {
        return;
    }

    command.m_range = static_cast<Blizzard::Storage::Range*>(Blizzard::Memory::Allocate(sizeof(Blizzard::Storage::Range)));
    memmove(command.m_range, range, sizeof(Blizzard::Storage::Range));
    command.m_completion = completion;

    if (StreamingX::g_StreamingDataRequireCallBack) {
        StreamingX::g_StreamingDataRequireCallBack(file, range->m_chunkOffset, range->m_chunkLength);
    }

    if (callback) {
        callback->m_refCount.Increment();
    }

    command.m_callback = callback;

    Blizzard::Lock::MutexEnter(s_cmdMutex);
    s_CmdQueue_Streaming[option]->push_back(command);
    Blizzard::Lock::MutexLeave(s_cmdMutex);

    Blizzard::Storage::SetRangeAs(file->m_streaming->m_unit, range->m_chunkOffset, range->m_chunkLength, Blizzard::Storage::ChunkState::CHUNK_REQUESTED);
}

// OFFSET: %0x00736340
bool Blizzard::Streaming::MakeLocal(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size, MakeLocalOption option, CompletionCallback* callback) {
    if (!file->m_streaming || !file->m_streaming->m_unit) {
        return false;
    }

    Blizzard::Storage::StorageUnit* unit = file->m_streaming->m_unit;

    if (!unit) {
        return false;
    }

    if (unit->m_local && unit->m_local->IsCompleted()) {
        return true;
    }

    int32_t count;
    Blizzard::Storage::Range* ranges = Blizzard::Storage::GetRange(unit, offset, size, &count, true);

    if (!ranges || !count) {
        return false;
    }

    bool queued = false;

    for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
        if (static_cast<uint32_t>(range->m_state - 2) <= 1) {
            QueueMakeLocal(0, file, range, option, NULL, callback);

            if (range->m_state != 3) {
                AddTotal(range->m_chunkLength);
            }

            queued = true;
        }
    }

    if (callback) {
        callback->Release();
    }

    if (queued) {
        if (s_backgroundPreloading) {
            Blizzard::Lock::EventSet(s_cmdEvent);
        }
    } else {
        AddDownloaded(size);
    }

    Blizzard::Memory::Free(ranges);

    return true;
}

// OFFSET: %0x00736470
void Blizzard::Streaming::MakeLocalCoalescing(Blizzard::File::StreamRecord* file, Blizzard::Storage::Range* ranges, int32_t count, int16_t tag) {
    Blizzard::Storage::Range current;
    current.m_state = 0;
    int32_t merged = 0;

    for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
        int32_t state = 0;

        if (range->m_state == 3 || range->m_state == 2) {
            state = 2;
        }

        if (current.m_state == state && current.m_chunkOffset + current.m_chunkLength == range->m_chunkOffset && merged < 8) {
            current.m_chunkLength += range->m_chunkLength;
            merged++;
        } else {
            if (current.m_state == 2) {
                QueueMakeLocal(tag, file, &current, static_cast<MakeLocalOption>(0), NULL, NULL);
                Blizzard::Lock::EventSet(s_cmdForegroundEvent);
                Blizzard::Lock::EventSet(s_cmdEvent);
            }

            current = *range;
            merged = 1;
        }
    }

    if (current.m_state == 2) {
        QueueMakeLocal(tag, file, &current, static_cast<MakeLocalOption>(0), NULL, NULL);
        Blizzard::Lock::EventSet(s_cmdForegroundEvent);
        Blizzard::Lock::EventSet(s_cmdEvent);
    }
}

// OFFSET: %0x00736580
bool Blizzard::Streaming::QueueCompletion(Completion* completion, int32_t queue) {
    if (s_threadShutdown) {
        return false;
    }

    if (!s_CmdQueue_Streaming[queue]->size()) {
        completion->m_result = 0;
        completion->m_done = false;
        return false;
    }

    FILE_THREAD_CMD command;
    command.m_tag = 0;
    command.m_type = 2;
    command.m_unit = NULL;
    command.m_retries = 0;
    command.m_range = NULL;
    command.m_completion = completion;
    command.m_callback = NULL;

    Blizzard::Lock::MutexEnter(s_cmdMutex);
    s_CmdQueue_Streaming[queue]->push_back(command);
    Blizzard::Lock::MutexLeave(s_cmdMutex);
    Blizzard::Lock::EventSet(s_cmdEvent);

    return true;
}

// OFFSET: %0x00736620
void Blizzard::Streaming::ClearCommand(FILE_THREAD_CMD* command) {
    if (command->m_completion && command->m_type == 2) {
        command->m_completion->m_result = 0;
        command->m_completion->m_done = true;
        Blizzard::Lock::EventSet(*command->m_completion->m_event);
        command->m_completion = NULL;
    }

    if (command->m_callback) {
        command->m_callback->m_success = false;
        command->m_callback->Release();
        command->m_callback = NULL;
    }

    command->m_type = 0;
    Blizzard::Memory::Free(command->m_range);
    command->m_range = NULL;
    Blizzard::Storage::Release(command->m_unit);
    command->m_unit = NULL;
}

// OFFSET: %0x0073BD90
Blizzard::Streaming::StreamRequest::~StreamRequest() {
    this->m_file->m_streaming->m_unit->FlushCacheEntries(this->m_tag);
    Blizzard::Memory::Free(this->m_ranges);
    concurrentRequests.Decrement();
}

// OFFSET: %0x00744500
bool Blizzard::Streaming::IsStreamedFile(const char* path) {
    if (s_streamingReady && path) {
        std::string key = System_File::GetInode(std::string(path));

        if (Blizzard::Storage::GetStorageInfo(key)) {
            return true;
        }
    }

    return false;
}

// OFFSET: %0x007445E0
int32_t Blizzard::Streaming::IsLocal(const char* path, int64_t offset, uint32_t size) {
    if (!s_streamingReady) {
        return 1;
    }

    if (!path) {
        return -1;
    }

    std::string key = System_File::GetInode(std::string(path));
    Blizzard::Storage::StorageUnit* unit = Blizzard::Storage::GetStorageInfo(key);

    if (!unit || (unit->m_local && unit->m_local->IsCompleted())) {
        return 1;
    }

    int32_t count;
    Blizzard::Storage::Range* ranges = Blizzard::Storage::GetRange(unit, offset, size, &count, false);

    if (!ranges || !count) {
        return 1;
    }

    int32_t result = 1;

    for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
        switch (range->m_state) {
            case 1:
            case 3:
                if (result == 1) {
                    result = 2;
                }

                break;

            case 2:
                result = 0;
                count = 0;
                break;
        }

        if (!result) {
            break;
        }
    }

    Blizzard::Memory::Free(ranges);

    return result;
}

// OFFSET: %0x00744780
int64_t Blizzard::Streaming::GetIsLocalAmount(const char* path, int64_t offset, uint32_t size) {
    std::string key = System_File::GetInode(std::string(path));
    Blizzard::Storage::StorageUnit* unit = Blizzard::Storage::GetStorageInfo(key);

    if (!unit) {
        return -1;
    }

    return GetIsLocalAmount(unit, offset, size);
}

// OFFSET: %0x007361B0
int64_t Blizzard::Streaming::GetIsLocalAmount(Blizzard::File::StreamRecord* file) {
    Blizzard::Storage::StorageUnit* unit = AcquireStorageUnit(file);

    if (!unit) {
        return -1;
    }

    int64_t local = GetIsLocalAmount(unit, 0, static_cast<uint32_t>(unit->m_logicalFileSize));
    Blizzard::Storage::Release(unit);

    return local;
}

// OFFSET: %0x00744860
bool Blizzard::Streaming::MakeLocalBlocking(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size, MakeLocalOption option, MakeLocalParams* params) {
    std::string key = System_File::GetInode(std::string(file->m_pathname));
    Blizzard::Storage::StorageUnit* unit = Blizzard::Storage::GetStorageInfo(key);

    if (unit && unit->m_local && unit->m_local->IsCompleted()) {
        return true;
    }

    int32_t count;
    Blizzard::Storage::Range* ranges = unit ? Blizzard::Storage::GetRange(unit, offset, size, &count, true) : NULL;

    if (!ranges || !count) {
        if (params) {
            params->m_status = 0;
        }

        return true;
    }

    Blizzard::Storage::Range request;
    request.m_state = 0;

    FileCompletionCallback* callback = new FileCompletionCallback();

    if (params && params->m_budget < unit->m_chunkSize) {
        params->m_budget = unit->m_chunkSize;
    }

    for (Blizzard::Storage::Range* range = ranges; count > 0; count--, range++) {
        if (static_cast<uint32_t>(range->m_state - 2) > 1) {
            continue;
        }

        if (range->m_chunkLength <= 0) {
            Blizzard::Memory::Free(ranges);

            if (params) {
                params->m_status = 2;
            }

            return false;
        }

        request.m_state = 2;
        request.m_chunkOffset = range->m_chunkOffset;
        request.m_chunkLength = range->m_chunkLength;

        if (params) {
            if (params->m_budget < range->m_chunkLength) {
                params->m_status = 1;
                continue;
            }

            params->m_budget -= range->m_chunkLength;
        }

        QueueMakeLocal(0, file, &request, option, NULL, callback);
        Blizzard::Lock::EventSet(s_cmdForegroundEvent);
        Blizzard::Lock::EventSet(s_cmdEvent);
    }

    Blizzard::Memory::Free(ranges);

    if (!request.m_state) {
        delete callback;
        return true;
    }

    callback->Release();
    Blizzard::Lock::EventSet(s_cmdForegroundEvent);
    Blizzard::Lock::EventSet(s_cmdEvent);
    Blizzard::Lock::EventWaitForever(callback->m_event);

    bool success = callback->m_success;
    delete callback;

    if (!success && params) {
        params->m_status = 2;
    }

    return success;
}

// OFFSET: %0x00748220
bool Blizzard::Streaming::GetStreamedFileSize(const char* path, int64_t* size) {
    *size = 0;

    if (!s_streamingReady || !path) {
        return false;
    }

    std::string key = System_File::GetInode(std::string(path));
    Manifests::FileItem* item = ManifestInfo::Get()->GetFileItem(key);

    if (!item) {
        return false;
    }

    *size = item->m_size;

    return true;
}

// OFFSET: %0x00748320
bool Blizzard::Streaming::StreamRequest::PrefetchData() {
    Blizzard::Storage::Range* range = this->m_ranges;

    for (int32_t count = this->m_count; count > 0; count--, range++) {
        if (static_cast<uint32_t>(range->m_state) > 1) {
            if (!ManifestInfo::Get()->m_servers.size()) {
                return false;
            }

            Blizzard::Lock::MutexEnter(Blizzard::Storage::s_storageMutex);
            Blizzard::Lock::MutexLeave(Blizzard::Storage::s_storageMutex);
            MakeLocalCoalescing(this->m_file, this->m_ranges, this->m_count, this->m_tag);

            return true;
        }
    }

    return true;
}

// OFFSET: %0x00749490
void Blizzard::Streaming::ShutdownStreaming() {
    if (s_streamingReady) {
        s_progress.FireCallback();
        s_threadShutdown = true;
        Blizzard::Lock::EventSet(s_cmdEvent);
        Blizzard::Lock::EventSet(s_cmdForegroundEvent);
        s_paused = false;
        Blizzard::Lock::EventSet(s_pauseEvent);

        while (s_threadExited > 0) {
            Blizzard::Process::Sleep(10);
        }

        Blizzard::Lock::EventDestroy(s_cmdEvent);
        Blizzard::Lock::EventDestroy(s_cmdForegroundEvent);

        for (int32_t i = 0; i < 2; i++) {
            Blizzard::Lock::EventDestroy(s_cmdPartnerEvent[i]);
            delete s_CmdProcess[i];
        }

        Blizzard::Lock::MutexEnter(s_cmdMutex);

        for (int32_t i = 0; i < MAX_CMD_QUEUE; i++) {
            std::deque<FILE_THREAD_CMD>& queue = *s_CmdQueue_Streaming[i];

            for (std::deque<FILE_THREAD_CMD>::iterator it = queue.begin(); it != queue.end(); ++it) {
                ClearCommand(&*it);
            }

            queue.clear();
        }

        Blizzard::Lock::MutexLeave(s_cmdMutex);
        Blizzard::Lock::MutexDestroy(s_cmdMutex);
        Blizzard::Lock::EventDestroy(s_pauseEvent);
        Blizzard::Lock::MutexDestroy(s_pauseMutex);

        for (int32_t i = 0; i < MAX_CMD_QUEUE; i++) {
            delete s_CmdQueue_Streaming[i];
        }

        s_streamingReady = false;
    }

    Blizzard::Storage::Shutdown();

    delete ManifestInfo::Get();
    ManifestInfo::s_instance = NULL;
}

// OFFSET: %0x00751360
uint32_t Blizzard::Streaming::FileThread(void* param) {
    int32_t thread = *static_cast<int32_t*>(param);
    delete static_cast<int32_t*>(param);

    Blizzard::Lock::MutexEnter(s_cmdMutex);
    int32_t bit = 1 << thread;
    s_threadExited |= bit;
    Blizzard::Lock::MutexLeave(s_cmdMutex);

    if (!s_threadShutdown) {
        for (;;) {
            if (Blizzard::Lock::EventWait(100, s_cmdEvent) || Blizzard::Lock::EventWait(5, s_cmdPartnerEvent[thread])) {
                while (!s_threadShutdown) {
                    if (s_paused) {
                        Blizzard::Lock::EventWaitForever(s_pauseEvent);
                        Blizzard::Lock::EventSet(s_pauseEvent);
                    }

                    Blizzard::Lock::MutexEnter(s_cmdMutex);
                    s_threadDownloading |= bit;

                    FILE_THREAD_CMD* command = NULL;
                    int32_t queue = GetNextCommand(&command, thread);

                    if (queue == MAX_CMD_QUEUE) {
                        Blizzard::Lock::MutexLeave(s_cmdMutex);
                        break;
                    }

                    int32_t pending = 0;

                    for (int32_t i = 0; i < MAX_CMD_QUEUE; i++) {
                        pending += static_cast<int32_t>(s_CmdQueue_Streaming[i]->size());
                    }

                    if (pending > 0) {
                        int32_t idle = GetIdleThread();

                        if (idle >= 0) {
                            Blizzard::Lock::EventSet(s_cmdPartnerEvent[idle]);
                        }
                    }

                    Blizzard::Lock::MutexLeave(s_cmdMutex);

                    bool success = true;
                    bool requeue = false;
                    bool unwatched = !command->m_completion && !command->m_callback;

                    if (!command->m_type) {
                        success = false;
                    } else if (command->m_type == 1) {
                        if (command->m_unit->FetchDeferData(command->m_range, command->m_tag)) {
                            command->m_type = 0;
                        } else {
                            success = false;

                            if (unwatched && command->m_retries++ < 5) {
                                requeue = true;
                            }
                        }
                    } else if (command->m_type == 2) {
                        command->m_type = 0;

                        if (!pending) {
                            WaitForLoads();
                        }
                    }

                    if (!requeue) {
                        if (command->m_type == 1) {
                            Blizzard::Storage::SetRangeAs(command->m_unit, command->m_range->m_chunkOffset, command->m_range->m_chunkLength, Blizzard::Storage::ChunkState::CHUNK_NONE);
                        }

                        if (command->m_completion) {
                            command->m_completion->m_result = static_cast<int32_t>(s_CmdQueue_Streaming[queue]->size());
                            command->m_completion->m_done = false;
                            command->m_completion->m_success = success;
                            Blizzard::Lock::EventSet(*command->m_completion->m_event);
                            command->m_completion = NULL;
                        }

                        if (command->m_callback) {
                            if (!success) {
                                command->m_callback->m_success = false;
                            }

                            command->m_callback->Release();
                        }
                    }

                    Blizzard::Lock::MutexEnter(s_cmdMutex);
                    s_CmdProcess[thread] = NULL;

                    if (requeue) {
                        s_CmdQueue_Streaming[queue]->push_front(*command);
                        AddStreamed(0);
                    } else {
                        Blizzard::Memory::Free(command->m_range);
                        Blizzard::Storage::Release(command->m_unit);
                    }

                    delete command;
                    Blizzard::Lock::MutexLeave(s_cmdMutex);

                    System_File::FileError& error = System_File::s_error;

                    if (error.m_errorStack) {
                        Blizzard::Debug::FreeErrorStack(error.m_errorStack);
                    }

                    error.m_errorStack = NULL;

                    if (queue > 2) {
                        int32_t sleep;

                        if (StreamingX::g_IsEnableP2PCallBack && StreamingX::g_IsEnableP2PCallBack() && queue == 6 && StreamingX::g_stepLowCommandInterval) {
                            StreamingX::g_lowCommandInterval += StreamingX::GetLowCommandIntervalStep();
                            sleep = StreamingX::g_lowCommandInterval;
                        } else {
                            sleep = FileManifest::GetBGPreloadSleep();
                        }

                        Blizzard::Lock::EventWait(sleep, s_cmdForegroundEvent);
                    }
                }
            }

            Blizzard::Lock::MutexEnter(s_cmdMutex);
            s_threadDownloading &= ~bit;
            Blizzard::Lock::MutexLeave(s_cmdMutex);

            if (s_threadShutdown) {
                break;
            }
        }
    }

    Blizzard::Lock::MutexEnter(s_cmdMutex);
    s_threadExited &= ~bit;
    Blizzard::Lock::MutexLeave(s_cmdMutex);

    return 1;
}

// OFFSET: %0x0075A2C0
bool Blizzard::Streaming::InitializeStreaming(const char* manifest, const char* dataPath) {
    if (s_streamingReady) {
        return true;
    }

    char cwd[260];
    cwd[0] = '\0';

    if (!Blizzard::File::GetWorkingDirectory(cwd, sizeof(cwd))) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to get working directory", 20);
        return false;
    }

    char root[260];

    if (!manifest || (s_httpPrefix && !_strnicmp(manifest, s_httpPrefix, strlen(s_httpPrefix)))) {
        Blizzard::String::Copy(root, cwd, sizeof(root));
    } else {
        char absolute[260];

        if (!Blizzard::File::MakeAbsolutePath(manifest, absolute, sizeof(absolute), false)) {
            Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to make absolute manifest path", 20);
            return false;
        }

        int32_t length = static_cast<int32_t>(Blizzard::String::FindFilename(absolute) - absolute + 1);
        Blizzard::String::Copy(root, absolute, length <= 260 ? length : 260);
    }

    if (dataPath && (!Blizzard::File::MakeAbsolutePath(dataPath, root, sizeof(root), false) || !Blizzard::File::IsDirectory(dataPath))) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Invalid data path specified", 20);
    }

    if (!Blizzard::File::SetWorkingDirectory(root)) {
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to change working directory", 20);
        return false;
    }

    bool manifestLoaded = ManifestInfo::Initialize(manifest);
    s_isStreamingTrial = FileManifest::GetIsStreamingTrial();

    if (!manifestLoaded) {
        Blizzard::File::SetWorkingDirectory(cwd);
        return false;
    }

    if (!Blizzard::Storage::Initialize()) {
        Blizzard::File::SetWorkingDirectory(cwd);
        Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Could not initialize storage", 20);
        return false;
    }

    for (int32_t i = 0; i < MAX_CMD_QUEUE; i++) {
        s_CmdQueue_Streaming[i] = new std::deque<FILE_THREAD_CMD>();
    }

    s_threadExited = 0;
    s_threadShutdown = false;
    Blizzard::Lock::MutexCreate(s_cmdMutex);
    Blizzard::Lock::EventCreate(s_cmdEvent);
    Blizzard::Lock::EventCreate(s_cmdForegroundEvent);

    for (int32_t i = 0; i < 2; i++) {
        s_CmdProcess[i] = NULL;
        Blizzard::Lock::EventCreate(s_cmdPartnerEvent[i]);

        int32_t* thread = new int32_t;
        *thread = i;
        Blizzard::Thread::CreateDetached(FileThread, thread, Blizzard::String::QuickFormat<1024>("Stream BG DL %d", i).m_buffer);
    }

    s_paused = false;
    Blizzard::Lock::MutexCreate(s_pauseMutex);
    Blizzard::Lock::EventCreate(s_pauseEvent);

    System_File::Stacked::PushStreaming();

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
        System_MPQGlobals::GetMopaqGlobals()->m_disableRawRead = true;
    }

    s_streamingReady = true;

    if (Blizzard::File::SetWorkingDirectory(cwd)) {
        return true;
    }

    Blizzard::File::AddToLastErrorStack(0, "InitializeStreaming - Failed to restore working directory", 20);
    return false;
}
