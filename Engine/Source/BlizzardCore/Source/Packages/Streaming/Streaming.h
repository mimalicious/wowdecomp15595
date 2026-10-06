#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H

#include <cstdint>
#include <deque>
#include <string>
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/StreamingManager.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

namespace Blizzard {
namespace Streaming {

enum MakeLocalOption {
    MAX_CMD_QUEUE = 8
};

struct Completion {
    /* 0000 */ Blizzard::Lock::Event* m_event;
    /* 0004 */ int32_t m_result;
    /* 0008 */ bool m_done;
    /* 0009 */ bool m_success;
};

class CompletionCallback {
    public:
    /* 0004 */ bool m_success;
    /* 0008 */ Blizzard::Lock::Atomic m_refCount;

    CompletionCallback();
    virtual ~CompletionCallback();
    virtual void Complete() = 0;
    void Release();
};

class FileCompletionCallback : public CompletionCallback {
    public:
    /* 000C */ Blizzard::Lock::Event m_event;

    FileCompletionCallback();
    virtual ~FileCompletionCallback();
    virtual void Complete();
};

struct FILE_THREAD_CMD {
    /* 0000 */ int32_t m_type;
    /* 0004 */ Blizzard::Storage::StorageUnit* m_unit;
    /* 0008 */ Blizzard::Storage::Range* m_range;
    /* 000C */ Completion* m_completion;
    /* 0010 */ CompletionCallback* m_callback;
    /* 0014 */ int16_t m_tag;
    /* 0016 */ int16_t m_retries;
};

struct MakeLocalParams {
    /* 0000 */ int64_t m_budget;
    /* 0008 */ int32_t m_status;
};

class StreamRequest {
    public:
    static Blizzard::Lock::Atomic globalRequestId;
    static Blizzard::Lock::Atomic concurrentRequests;

    /* 0000 */ Blizzard::File::StreamRecord* m_file;
    /* 0008 */ int64_t m_offset;
    /* 0010 */ uint32_t m_size;
    /* 0014 */ Blizzard::Storage::Range* m_ranges;
    /* 0018 */ int32_t m_count;
    /* 001C */ int16_t m_tag;

    StreamRequest(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size);
    ~StreamRequest();
    bool GetLocality();
    bool PrefetchData();
    bool ReadLocalChunks(void* buffer);
    void ReadLocalRange(Blizzard::Storage::Range* first, Blizzard::Storage::Range* last, void* buffer);
    bool ReadRemoteChunks(void* buffer);
};

struct ProgressSample {
    /* 0000 */ int64_t m_bytes;
    /* 0008 */ uint32_t m_time;
};

typedef void (*PROGRESSCALLBACK)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t);

class DownloadProgress {
    public:
    /* 0000 */ int64_t m_total;
    /* 0008 */ int64_t m_downloaded;
    /* 0010 */ int64_t m_streamed;
    /* 0018 */ uint32_t m_lastTime;
    /* 0020 */ ProgressSample m_samples[51];
    /* 0350 */ uint32_t m_index;
    /* 0354 */ PROGRESSCALLBACK m_callback;
    /* 0358 */ bool m_enabled;

    DownloadProgress();
    ~DownloadProgress();
    void FireCallback();
    int32_t GetRate();
    void ResetTime();
    void Sample(uint32_t time);
};

namespace StreamingX {

typedef void (*DATAREQUIRECALLBACK)(Blizzard::File::StreamRecord* file, int64_t offset, int32_t size);
typedef bool (*ISENABLEP2PCALLBACK)();

extern DATAREQUIRECALLBACK g_StreamingDataRequireCallBack;
extern ISENABLEP2PCALLBACK g_IsEnableP2PCallBack;
extern int32_t g_lowCommandInterval;
extern int32_t g_maxLowCommandInterval;
extern int32_t g_stepLowCommandInterval;

int32_t GetLowCommandIntervalStep();

}

extern Blizzard::Time::Timestamp s_elapsedSinceStart;
extern int64_t s_statBytesRead;
extern int64_t s_statBytesDownloaded;
extern int64_t s_statBytesLocal;
extern int32_t s_statRetries;
extern char s_statFrameTimes[260];
extern bool s_streamingReady;
extern bool s_isStreamingTrial;
extern bool s_backgroundPreloading;
extern volatile bool s_threadShutdown;

extern DownloadProgress s_progress;

Blizzard::Storage::StorageUnit* AcquireStorageUnit(Blizzard::File::StreamRecord* file);
void AddDownloaded(int32_t size);
void AddStreamed(int32_t size);
void AddTotal(int32_t size);
void ClearCommand(FILE_THREAD_CMD* command);
uint32_t FileThread(void* param);
int32_t GetIdleThread();
int64_t GetIsLocalAmount(const char* path, int64_t offset, uint32_t size);
int64_t GetIsLocalAmount(Blizzard::Storage::StorageUnit* unit, int64_t offset, uint32_t size);
int64_t GetIsLocalAmount(Blizzard::File::StreamRecord* file);
int32_t GetNextCommand(FILE_THREAD_CMD** command, int32_t thread);
std::string GetBandwidthString();
bool GetStreamedFileSize(const char* path, int64_t* size);
bool InitializeStreaming(const char* manifest, const char* dataPath);
int32_t IsLocal(const char* path, int64_t offset, uint32_t size);
int32_t IsLocal(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size);
bool IsPreloadInProgress();
bool IsStreamedFile(const char* path);
bool IsStreamingMode();
bool MakeLocal(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size, MakeLocalOption option, CompletionCallback* callback);
bool MakeLocalBlocking(Blizzard::File::StreamRecord* file, int64_t offset, uint32_t size, MakeLocalOption option, MakeLocalParams* params);
void MakeLocalCoalescing(Blizzard::File::StreamRecord* file, Blizzard::Storage::Range* ranges, int32_t count, int16_t tag);
bool PopCommand(FILE_THREAD_CMD* command, int32_t queue);
bool QueueCompletion(Completion* completion, int32_t queue);
void QueueMakeLocal(int16_t tag, Blizzard::File::StreamRecord* file, Blizzard::Storage::Range* range, MakeLocalOption option, Completion* completion, CompletionCallback* callback);
bool RemoveCompletion(Completion* completion, int32_t queue);
bool ResetMakeLocal(MakeLocalOption option);
bool SelectQueue(FILE_THREAD_CMD* command, int32_t group, int32_t* queue);
void ShutdownStreaming();
void WaitForLoads();
void SetZone(int32_t areaId, int32_t subzoneId);
bool ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms);

}
}

bool CheckIfStreamingAndTrial();
void StringToLower(char* str);
StreamingManager* GetStreamingManagerSingleton();
void SetClientInitialized(uint8_t initialized);
void ShutdownStreamingIfTrial();
int32_t StreamingGetRace();
bool StreamingIsRaceDataReady(int32_t raceID);
void StreamingSetAreaOfInterest(int32_t chunkY, int32_t chunkX);
void StreamingSetMapId(int32_t mapId);
void StreamingSetRace(int32_t raceID);
void StreamingSetState(int32_t state);
char* GetComponentVersionData(uint32_t* size);
void SetActiveDataSource(bool restore);
void StartDataPreloadThread();
void StreamingSetEnabled(bool enabled);
float StreamingGetLoadFraction(int32_t raceID);

#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.inl"

#endif
