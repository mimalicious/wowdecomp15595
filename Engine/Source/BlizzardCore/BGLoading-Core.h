#ifndef ENGINE_SOURCE_BLIZZARDCORE_BGLOADING_CORE_H
#define ENGINE_SOURCE_BLIZZARDCORE_BGLOADING_CORE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace Mopaq {
struct HSFILERecord;
}
namespace Streaming {
bool IsStreamingMode();
}
namespace Thread {
struct ThreadRecord;
}
}

namespace System_SFile {
class ARCHIVEREC;
}

namespace BGLoading {

class BGLoadManager {
    public:
    static BGLoadManager* Get();
    static uint32_t RunBGLoadThread(void* param);
    static void StartThread();

    /* 0000 */ Blizzard::Thread::ThreadRecord* m_thread;
    /* 0004 */ Blizzard::Lock::Event m_stopEvent;
    /* 0008 */ Blizzard::Lock::Mutex m_mutex;
    /* 0020 */ bool m_paused;
    /* 0021 */ bool m_inProgress;
    /* 0022 */ bool m_showStatus;
    /* 0024 */ uint32_t m_firstSample;
    /* 0028 */ uint32_t m_sample;
    /* 002C */ bool m_samplesFull;
    /* 0030 */ char* m_status;
    /* 0034 */ std::vector<int64_t> m_sampleTimes;
    /* 004C */ std::vector<int64_t> m_sampleBytes;
    /* 0064 */ float m_bandwidth;
    /* 0068 */ float m_rate;
    /* 0070 */ int64_t m_sampleTotal;
    /* 0078 */ std::vector<System_SFile::ARCHIVEREC*> m_archives;
    /* 0090 */ std::vector<uint32_t> m_archiveBlocks;
    /* 00A8 */ std::deque<System_SFile::ARCHIVEREC*> m_archiveQueue;
    /* 00C8 */ uint32_t m_percentTime;
    /* 00D0 */ int64_t m_total;
    /* 00D8 */ int64_t m_local;
    /* 00E0 */ int32_t m_throttle;
    /* 00E4 */ std::vector<Blizzard::Mopaq::HSFILERecord*> m_files;
    /* 00FC */ std::deque<Blizzard::Mopaq::HSFILERecord*> m_fileQueue;
    /* 011C */ std::vector<int64_t> m_fileOffsets;

    BGLoadManager();
    ~BGLoadManager();
    void AdvanceSample();
    void AppendNewArchives();
    void AppendNewBGFiles();
    uint32_t BGLoadThread();
    void CloseBackgroundFiles();
    void DownloadBackgroundFiles();
    double GetDownloadedPercentage();
    void QueueArchive(const char* name);
    void QueueArchiveFiles();
    bool QueueFile(const char* name);
    bool Shutdown();
    int32_t SkipLocalData(Blizzard::Mopaq::HSFILERecord* file, int64_t& offset, int32_t size);
    bool WaitForNextTimeSlice(int32_t& slice);
};

bool BGLoadThreadStarted();
float GetBandwidth();
double GetDownloadedPercentage();
bool IsStreamingInProgress();
void Pause(bool pause);
void QueueArchive(const char* name);
bool QueueFile(const char* name);
bool SetBandwidthThrottlePercentage(int32_t percent);
void StartBGLoadThread();
bool StopBGLoadThread();

}

#include "Engine/Source/BlizzardCore/BGLoading-Core.inl"

#endif
