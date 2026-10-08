#include <algorithm>
#include <cstdint>
#include <deque>
#include <vector>
#include "Engine/Source/BlizzardCore/BGLoading-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// OFFSET: 0x0079FEA0/0x140495990
int32_t BGLoading::BGLoadManager::SkipLocalData(Blizzard::Mopaq::HSFILERecord* file, int64_t& offset, int32_t size) {
    int64_t fileSize = Blizzard::Mopaq::SFileGetFileSize(file, NULL);

    if (offset >= fileSize) {
        return 1;
    }

    Blizzard::File::StreamRecord* stream = file->GetFileStream();

    if (stream) {
        if (Blizzard::Streaming::IsLocal(stream, offset, size) != 1) {
            return 0;
        }

        while (true) {
            offset += size;

            if (offset >= fileSize) {
                break;
            }

            if (Blizzard::Lock::EventWait(1, this->m_stopEvent)) {
                return -1;
            }

            if (Blizzard::Streaming::IsLocal(stream, offset, size) != 1) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x007A1FD0
bool BGLoading::BGLoadManager::Shutdown() {
    if (!this->m_thread || !Blizzard::Thread::IsRunning(this->m_thread)) {
        this->CloseBackgroundFiles();
        return false;
    }

    Blizzard::Lock::EventSet(this->m_stopEvent);

    if (this->m_thread) {
        Blizzard::Thread::WaitForever(this->m_thread);
        Blizzard::Thread::Close(this->m_thread);
    }

    this->m_thread = NULL;
    this->m_rate = 0.0f;

    return true;
}

// OFFSET: 0x007A02D0/0x140495F10
bool BGLoading::BGLoadManager::WaitForNextTimeSlice(int32_t& slice) {
    int32_t remaining = 0;
    int32_t maxWait = 1000;

    while (true) {
        int64_t elapsed = Blizzard::Time::GetTimestamp() - this->m_sampleTimes[this->m_firstSample];
        this->m_rate = this->m_sampleTotal * 8000.0f / elapsed;

        int32_t wait;

        if (this->m_paused) {
            wait = 1001;
        } else if (slice >= 100 && !remaining) {
            slice = 0;
            wait = 10;
        } else {
            if (!this->m_throttle) {
                this->m_inProgress = false;
            }

            wait = static_cast<int32_t>(elapsed / -1000000) - static_cast<int32_t>(this->m_sampleTotal / (this->m_throttle * this->m_bandwidth * 0.01f + 0.00001f) * -0.008f);

            if (wait < 0) {
                wait = 0;
            }
        }

        int32_t timeout = std::min(wait, maxWait);

        if (Blizzard::Lock::EventWait(timeout, this->m_stopEvent)) {
            break;
        }

        remaining = wait - timeout;

        if (remaining <= 0) {
            return true;
        }
    }

    this->m_inProgress = false;

    return false;
}

// OFFSET: 0x007A2030
BGLoading::BGLoadManager::~BGLoadManager() {
    this->Shutdown();
    Blizzard::Lock::EventDestroy(this->m_stopEvent);
    Blizzard::Lock::MutexDestroy(this->m_mutex);
    Blizzard::Memory::Free(this->m_status);
    this->m_sampleTimes.clear();
    this->m_sampleBytes.clear();
}

// OFFSET: 0x007A1310
void BGLoading::BGLoadManager::QueueArchive(const char* name) {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    System_SFile::ARCHIVEREC* archive = Blizzard::Mopaq::SFileGetArchiveHandle(name);

    if (archive) {
        this->m_archiveQueue.push_back(archive);
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007A1390
bool BGLoading::BGLoadManager::QueueFile(const char* name) {
    Blizzard::Mopaq::HSFILERecord* file = NULL;
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    for (std::deque<Blizzard::Mopaq::HSFILERecord*>::iterator it = this->m_fileQueue.begin(); it != this->m_fileQueue.end(); ++it) {
        if (Blizzard::String::EqualI((*it)->GetFileName(), name)) {
            return false;
        }
    }

    if (!Blizzard::Mopaq::SFileOpenFileEx(NULL, name, 9, &file)) {
        return false;
    }

    this->m_fileQueue.push_back(file);

    return true;
}

// OFFSET: 0x007A14E0
BGLoading::BGLoadManager::BGLoadManager() {
    this->m_thread = NULL;
    Blizzard::Lock::EventCreate(this->m_stopEvent);
    Blizzard::Lock::MutexCreate(this->m_mutex);
    this->m_paused = false;
    this->m_inProgress = false;
    this->m_showStatus = false;
    this->m_status = static_cast<char*>(Blizzard::Memory::Allocate(30 * 260 + 1, 1));
    this->m_sampleTimes.resize(30, 0);
    this->m_sampleBytes.resize(30, 0);
    this->m_firstSample = 0;
    this->m_sample = 0;
    this->m_sampleTotal = 0;
    this->m_samplesFull = false;
    this->m_percentTime = 0;
    this->m_total = 0;
    this->m_local = 0;
    this->m_bandwidth = 0.0f;
    this->m_rate = 0.0f;
    this->m_throttle = 33;
}

// OFFSET: 0x007A1660/0x140497DE0
void BGLoading::BGLoadManager::AppendNewBGFiles() {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    while (this->m_fileQueue.size()) {
        this->m_files.push_back(this->m_fileQueue.front());
        this->m_fileQueue.pop_front();
        this->m_fileOffsets.push_back(0);
    }
}

// OFFSET: 0x007A1830
void BGLoading::BGLoadManager::AppendNewArchives() {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    while (this->m_archiveQueue.size()) {
        this->m_archives.push_back(this->m_archiveQueue.front());
        this->m_archiveQueue.pop_front();
        this->m_archiveBlocks.push_back(0);
    }
}

// OFFSET: 0x007A19F0/0x140498200
void BGLoading::BGLoadManager::DownloadBackgroundFiles() {
    Blizzard::Streaming::MakeLocalParams params;
    params.m_budget = 0x20000;
    params.m_status = 0;
    int32_t slice = 0;
    uint32_t i = 0;

    while (true) {
        uint32_t done = 0;

        if (this->m_files.size()) {
            do {
                int32_t result = this->SkipLocalData(this->m_files[i], this->m_fileOffsets[i], 0x20000);

                if (result < 0) {
                    return;
                }

                if (result) {
                    done++;
                } else {
                    this->m_sampleBytes[this->m_sample] = 0;
                    int64_t start = this->m_sampleTimes[this->m_sample] = Blizzard::Time::GetTimestamp();
                    params.m_status = 0;
                    int64_t bytes = 0x20000;
                    int64_t fileSize = Blizzard::Mopaq::SFileGetFileSize(this->m_files[i], NULL);

                    if (this->m_fileOffsets[i] + 0x20000 > fileSize) {
                        bytes = fileSize - this->m_fileOffsets[i];
                    }

                    if (Blizzard::Streaming::MakeLocalBlocking(this->m_files[i]->GetFileStream(), this->m_fileOffsets[i], bytes, &params, NULL)) {
                        this->m_fileOffsets[i] += bytes;
                    } else {
                        bytes = 0;
                    }

                    float rate = static_cast<float>(bytes) / (Blizzard::Time::GetTimestamp() - start) * 8000.0f;
                    this->m_bandwidth = this->m_bandwidth <= 0.0f ? rate : this->m_bandwidth * 0.8f + rate * 0.2f;

                    if (this->m_showStatus) {
                        char name[260];
                        Blizzard::Mopaq::SFileGetFileName(this->m_files[i], name, sizeof(name));
                        int64_t total = Blizzard::Mopaq::SFileGetFileSize(this->m_files[i], NULL);
                        char state = this->m_fileOffsets[i] == total ? 'L' : 'R';
                        Blizzard::String::Format(this->m_status + 260 * this->m_sample, 260, "%c: %3.1f%% : %s", state, static_cast<double>(this->m_fileOffsets[i]) / static_cast<double>(total) * 100.0, name);
                    }

                    this->m_sampleBytes[this->m_sample] = bytes;
                    this->m_sampleTotal += bytes;
                    this->AdvanceSample();

                    if (!this->WaitForNextTimeSlice(slice)) {
                        return;
                    }
                }
            } while (++i < this->m_files.size());

            i = 0;
        }

        this->AppendNewBGFiles();

        if (done >= this->m_files.size()) {
            return;
        }
    }
}

// OFFSET: 0x007A2790
BGLoading::BGLoadManager* BGLoading::BGLoadManager::Get() {
    // STATIC: 0x00EB3A58
    static BGLoadManager s_manager;
    return &s_manager;
}

// PROVEN: 6608aa26 IDENTICAL/NONE
// OFFSET: 0x007A1EA0
void BGLoading::BGLoadManager::CloseBackgroundFiles() {
    this->AppendNewBGFiles();

    for (std::vector<Blizzard::Mopaq::HSFILERecord*>::iterator it = this->m_files.begin(); it != this->m_files.end(); ++it) {
        Blizzard::Mopaq::SFileCloseFile(*it);
        *it = NULL;
    }

    this->m_files.clear();
    this->m_fileOffsets.clear();
}

// OFFSET: 0x007A1240
void BGLoading::BGLoadManager::QueueArchiveFiles() {
    Blizzard::Lock::ScopedMutex lock(this->m_mutex);

    for (uint32_t i = 0; i < this->m_archives.size(); i++) {
        Blizzard::Mopaq::HSFILERecord* file;
        Blizzard::Mopaq::SFileGetArchiveFile(this->m_archives[i], &file);
        this->m_fileQueue.push_back(file);
    }
}

// OFFSET: 0x007A0040
double BGLoading::BGLoadManager::GetDownloadedPercentage() {
    uint32_t now = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    if (Blizzard::Time::GetTimeElapsed(this->m_percentTime, now) > 2000) {
        this->m_total = 0;
        this->m_local = 0;

        for (uint32_t i = 0; i < this->m_archives.size(); i++) {
            char name[260];
            int64_t local = 0;
            int64_t total = 0;
            Blizzard::Mopaq::SFileGetArchiveName(this->m_archives[i], name, sizeof(name));
            Blizzard::Streaming::GetLocalAmount(name, &local, &total);
            this->m_local += local;
            this->m_total += total;
        }

        for (uint32_t i = 0; i < this->m_files.size(); i++) {
            char name[260];
            int64_t local = 0;
            int64_t total = 0;
            Blizzard::Mopaq::SFileGetFileName(this->m_files[i], name, sizeof(name));
            Blizzard::Streaming::GetLocalAmount(name, &local, &total);
            this->m_local += local;
            this->m_total += total;
        }

        this->m_percentTime = now;
    }

    if (this->m_total) {
        return static_cast<double>(this->m_local) / static_cast<double>(this->m_total) * 0.99000001f;
    }

    return 1.0;
}

// OFFSET: 0x007A2840
bool BGLoading::BGLoadThreadStarted() {
    Blizzard::Thread::ThreadRecord* thread = BGLoadManager::Get()->m_thread;
    return thread && Blizzard::Thread::IsRunning(thread);
}

// OFFSET: 0x007A2890
bool BGLoading::StopBGLoadThread() {
    if (!Blizzard::Streaming::IsStreamingMode()) {
        return false;
    }

    return BGLoadManager::Get()->Shutdown();
}

// OFFSET: 0x007A28B0/0x140499280
bool BGLoading::SetBandwidthThrottlePercentage(int32_t percent) {
    if (!Blizzard::Streaming::IsStreamingMode()) {
        return false;
    }

    BGLoadManager::Get()->m_throttle = percent;

    return true;
}

// OFFSET: %0x00762480
float BGLoading::GetBandwidth() {
    if (Blizzard::Streaming::IsStreamingMode()) {
        return BGLoadManager::Get()->m_bandwidth;
    }

    return 0.0f;
}

// OFFSET: 0x007A28D0
double BGLoading::GetDownloadedPercentage() {
    if (!Blizzard::Streaming::IsStreamingMode()) {
        return 1.0;
    }

    return BGLoadManager::Get()->GetDownloadedPercentage();
}

// OFFSET: 0x007A28F0
void BGLoading::QueueArchive(const char* name) {
    if (Blizzard::Streaming::IsStreamingMode()) {
        BGLoadManager::Get()->QueueArchive(name);
    }
}

// OFFSET: 0x007A2910
bool BGLoading::QueueFile(const char* name) {
    if (!Blizzard::Streaming::IsStreamingMode()) {
        return false;
    }

    return BGLoadManager::Get()->QueueFile(name);
}

// OFFSET: 0x007A2930
void BGLoading::Pause(bool pause) {
    if (Blizzard::Streaming::IsStreamingMode()) {
        BGLoadManager::Get()->m_paused = pause;
    }
}

// OFFSET: 0x007A2950/0x1404994A0
bool BGLoading::IsStreamingInProgress() {
    if (!Blizzard::Streaming::IsStreamingMode()) {
        return false;
    }

    return BGLoadManager::Get()->m_inProgress;
}

// OFFSET: 0x007A2270/0x140498A60
uint32_t BGLoading::BGLoadManager::BGLoadThread() {
    int32_t slice = 0;

    while (true) {
        this->m_inProgress = true;
        uint32_t done = 0;

        for (uint32_t i = 0; i < this->m_archives.size(); i++) {
            uint32_t blockCount = Blizzard::Mopaq::SFileGetArchiveBlockCount(this->m_archives[i]);
            uint32_t block = this->m_archiveBlocks[i];

            if (block >= blockCount) {
                done++;
                continue;
            }

            this->m_archiveBlocks[i]++;

            int64_t size = Blizzard::Mopaq::SFileGetStreamedFileSize(this->m_archives[i], block);
            char state = 'L';
            this->m_sampleBytes[this->m_sample] = 0;
            this->m_sampleTimes[this->m_sample] = Blizzard::Time::GetTimestamp();

            if (size > 0 && Blizzard::Mopaq::SFileFileIsLocal(this->m_archives[i], block) != 1) {
                int64_t offset = 0;
                int64_t remaining = size;
                uint32_t chunk = static_cast<uint32_t>(size);
                int64_t start = Blizzard::Time::GetTimestamp();

                while (true) {
                    if (remaining > 0x4000) {
                        chunk = 0x4000;
                    }

                    Blizzard::Mopaq::SFileMakeFileLocal(this->m_archives[i], block, offset, chunk, true);
                    offset += chunk;
                    remaining -= chunk;

                    if (Blizzard::Lock::EventWait(0, this->m_stopEvent)) {
                        goto stop;
                    }

                    if (remaining <= 0) {
                        break;
                    }

                    chunk = static_cast<uint32_t>(remaining);
                }

                float rate = static_cast<float>(size) / (Blizzard::Time::GetTimestamp() - start) * 8000.0f;
                this->m_bandwidth = this->m_bandwidth <= 0.0f ? rate : this->m_bandwidth * 0.8f + rate * 0.2f;
                state = 'R';
                this->m_sampleBytes[this->m_sample] = size;
                this->m_sampleTotal += size;
            }

            if (this->m_showStatus) {
                char name[260];
                Blizzard::Mopaq::SFileGetArchiveName(this->m_archives[i], name, sizeof(name));
                Blizzard::String::Format(this->m_status + 260 * this->m_sample, 260, "%c: File #%d (%d KB) of %s (%3.1f%%)", state, block, static_cast<uint32_t>((size + 1023) / 1024), Blizzard::String::FindFilename(name), static_cast<double>(block) / static_cast<double>(blockCount) * 100.0);
            }

            slice++;
            this->AdvanceSample();

            if (!this->WaitForNextTimeSlice(slice)) {
                goto stop;
            }
        }

        this->AppendNewArchives();

        if (done >= this->m_archives.size()) {
            this->QueueArchiveFiles();
            this->DownloadBackgroundFiles();
            break;
        }
    }

stop:
    this->m_inProgress = false;
    this->m_archives.clear();
    this->m_archiveQueue.clear();
    this->m_archiveBlocks.clear();
    this->CloseBackgroundFiles();

    return 0;
}

// PROVEN: 9b7e116c IDENTICAL/NONE
// OFFSET: 0x007A2780
uint32_t BGLoading::BGLoadManager::RunBGLoadThread(void* param) {
    return static_cast<BGLoadManager*>(param)->BGLoadThread();
}

// OFFSET: 0x007A27F0
void BGLoading::BGLoadManager::StartThread() {
    BGLoadManager* manager = Get();
    Blizzard::Lock::MutexEnter(manager->m_mutex);

    if (!manager->m_thread || !Blizzard::Thread::IsRunning(manager->m_thread)) {
        Blizzard::Thread::Create(RunBGLoadThread, manager, &manager->m_thread, "BGDataDownload", 0, 0);
    }

    Blizzard::Lock::MutexLeave(manager->m_mutex);
}
