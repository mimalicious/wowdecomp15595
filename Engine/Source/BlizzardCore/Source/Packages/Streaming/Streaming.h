#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H

#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

namespace Blizzard {
namespace Streaming {

enum MakeLocalOption {
    MAX_CMD_QUEUE = 8
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

struct MakeLocalParams {
    /* 0000 */ int64_t m_budget;
    /* 0008 */ int32_t m_status;
};

class RequestHandler {
    public:
    virtual ~RequestHandler();
    virtual void Queue(Blizzard::Storage::Request* request) = 0;
    virtual bool Start(bool paused) = 0;
    virtual void Stop() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
};

class RequestProcessor {
    public:
    static uint32_t ThreadProc(void* param);

    /* 0000 */ std::deque<Blizzard::Storage::Request*> m_queue;
    /* 0020 */ bool m_paused;
    /* 0021 */ bool m_shutdown;
    /* 0024 */ Blizzard::Lock::Event m_event;
    /* 0028 */ Blizzard::Thread::ThreadRecord* m_thread;
    /* 002C */ Blizzard::Lock::Mutex m_mutex;

    RequestProcessor();
    ~RequestProcessor();
    bool HasWork();
    void PopFront();
    bool ProcessNext();
    void Shutdown();
    bool StartThread();
};

class DefaultRequestHandler : public RequestHandler {
    public:
    /* 0004 */ RequestProcessor* m_processor;
    /* 0008 */ Blizzard::Lock::Mutex m_mutex;

    DefaultRequestHandler();
    ~DefaultRequestHandler();
    void Queue(Blizzard::Storage::Request* request) override;
    bool Start(bool paused) override;
    void Stop() override;
    void Pause() override;
    void Resume() override;
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

class ScopedWorkingDirectory {
    public:
    /* 0000 */ bool m_saved;
    /* 0001 */ bool m_changed;
    /* 0002 */ char m_path[260];

    ScopedWorkingDirectory();
    ~ScopedWorkingDirectory();
    bool ChangeToDataPath(const char* path);
    bool ChangeToManifestPath(const char* manifest);
};

extern Blizzard::Time::Timestamp s_elapsedSinceStart;
extern bool s_isStreamingTrial;
extern bool s_streamingReady;
extern bool s_throttleProgress;
extern RequestHandler* s_requestHandler;
extern DownloadProgress s_progress;

Blizzard::Storage::StorageUnit* AcquireStorageUnit(Blizzard::File::StreamRecord* file);
void AddStreamed(int64_t size);
std::string GetBandwidthString();
int64_t GetIsLocalAmount(const char* path, int64_t offset, int64_t size);
int64_t GetIsLocalAmount(Blizzard::File::StreamRecord* file);
void GetLocalAmount(const char* path, int64_t* local, int64_t* total);
bool GetStreamedFileSize(const char* path, int64_t* size);
bool InitializeStreaming(const char* manifest, const char* dataPath, RequestHandler* handler);
bool IsInManifest(const char* path);
int32_t IsLocal(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size);
bool IsStreamedFile(const char* path);
bool IsStreamingMode();
bool LoadManifest(const char* path);
bool MakeLocal(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size, CompletionCallback* callback, const int64_t* chunk);
bool MakeLocalBlocking(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size, MakeLocalParams* params, const int64_t* chunk);
bool MakeRemote(Blizzard::File::StreamRecord* file, int64_t offset, int64_t size);
void SetDefaultRequestHandler();
void ShutdownStreaming();
void UnloadManifest();
bool ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms);
void SetZone(int32_t areaId, int32_t subzoneId);

}
}

char* GetComponentVersionData(uint32_t* size);

#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.inl"

#endif
