#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace File {
struct Filesystem;
struct StreamRecord;
}
}

namespace Blizzard {
namespace Storage {

class StorageUnit;

struct ChunkState {
    enum State {
        CHUNK_NONE = 0,
        CHUNK_REQUESTED = 1,
        CHUNK_DOWNLOADING = 2,
        CHUNK_CACHED = 3,
        CHUNK_LOCAL = 4,
        CHUNK_STATE_MASK = 0x7F,
        CHUNK_CACHE_ENTRY = 0x80
    };
};

union BufferHandle {
    /* 0000 */ uint32_t all;
    struct {
        uint32_t start : 7;
        uint32_t length : 7;
    } m;
};

struct Range {
    /* 0000 */ int64_t m_offset;
    /* 0008 */ int32_t m_length;
    /* 0010 */ int64_t m_chunkOffset;
    /* 0018 */ int32_t m_chunkLength;
    /* 001C */ int32_t m_state;

    Range();
    Range(int64_t offset, int32_t length, int64_t chunkOffset, int32_t chunkLength, int32_t state);
};

struct STORAGE_CMD {
    /* 0000 */ int32_t m_type;
    /* 0004 */ StorageUnit* m_unit;
    /* 0008 */ BufferHandle m_handle;
    /* 000C */ int16_t m_tag;
    /* 0010 */ int64_t m_offset;
    /* 0018 */ int32_t m_length;
};

class StorageSystem {
    public:
    /* 0000 */ int32_t m_writebackLock;
    /* 0004 */ Blizzard::Lock::Mutex m_mutex;
    /* 001C */ uint8_t* m_buffer;
    /* 0020 */ uint8_t* m_allocationMarkers;
    /* 0024 */ int32_t m_markerCount;
    /* 0028 */ int32_t m_maxEnd;
    /* 002C */ int32_t m_maxAllocation;
    /* 0030 */ int32_t m_totalAllocated;
    /* 0034 */ int32_t m_allocated;
    /* 0038 */ int32_t m_peakAllocated;

    StorageSystem();
    ~StorageSystem();
    BufferHandle AllocateChunks(int32_t size);
    BufferHandle AllocateSubChunks(BufferHandle handle, int32_t offset, int32_t size);
    void EnableWriteback(bool enable);
    uint8_t* GetBuffer(BufferHandle handle);
    bool RecoverMemory(StorageUnit* unit, int32_t level);
    void ReleaseChunks(BufferHandle handle);
};

class BitField {
    public:
    /* 0000 */ uint8_t* m_bits;
    /* 0004 */ int32_t m_length;

    BitField(int32_t count);
    int32_t CountSet() const;
    bool IsSetUpTo(int32_t count) const;
};

class PartialBlock {
    public:
    /* 0000 */ uint32_t m_lastBlockSize;
    /* 0004 */ uint32_t m_blockSize;
    /* 0008 */ uint32_t m_blockCount;
    /* 000C */ std::map<uint32_t, uint32_t> m_partial;

    bool SetPartial(uint32_t block, uint32_t size);
};

class BackingStore {
    public:
    virtual int32_t GetChunkSize() const = 0;
    virtual int64_t GetTotalLocalSize() const = 0;
    virtual bool Read(void* buffer, int64_t offset, int32_t size) = 0;
    virtual bool Write(const void* buffer, int64_t offset, int32_t size) = 0;
    virtual bool Flush() = 0;
    virtual bool WriteUnaligned(const void* buffer, int64_t offset, int32_t size) = 0;
    virtual bool Close() = 0;
    virtual bool IsAvailable(int64_t offset, int64_t size) const = 0;
    virtual bool Invalidate(int64_t offset, int64_t size) = 0;
    virtual bool IsCompleted() const = 0;
    virtual bool IsMatch(const char* path, int64_t size, int32_t contentId) const = 0;
    virtual ~BackingStore();
};

class PartialFile : public BackingStore {
    public:
    static PartialFile* CreateShared(Blizzard::File::Filesystem* fs, const char* path, int64_t size, int32_t contentId);
    static bool FileIsCompletePartialFile(Blizzard::File::StreamRecord* file, int64_t archiveSize, int64_t fileSize);
    static bool ParseFileFromHeader(Blizzard::File::StreamRecord* file, BitField** locality, int32_t* chunkSize, int64_t* fileSize, int32_t* contentId);

    /* 0004 */ int32_t m_chunkSize;
    /* 0008 */ int64_t m_fileSize;
    /* 0010 */ int32_t m_contentId;
    /* 0014 */ Blizzard::File::StreamRecord* m_file;
    /* 0018 */ Blizzard::File::StreamRecord* m_lockFile;
    /* 001C */ std::string m_lockPath;
    /* 0038 */ bool m_readOnly;
    /* 003C */ int32_t m_dirtyStart;
    /* 0040 */ int32_t m_dirtyEnd;
    /* 0044 */ uint32_t m_lastFlush;
    /* 0048 */ BitField* m_locality;
    /* 004C */ PartialBlock* m_partialBlocks;
    /* 0050 */ bool m_open;
    /* 0051 */ bool m_completed;
    /* 0052 */ bool m_assumedComplete;
    /* 0054 */ int32_t m_invalidateCount;

    PartialFile();
    virtual int32_t GetChunkSize() const;
    virtual int64_t GetTotalLocalSize() const;
    virtual bool Read(void* buffer, int64_t offset, int32_t size);
    virtual bool Write(const void* buffer, int64_t offset, int32_t size);
    virtual bool Flush();
    virtual bool WriteUnaligned(const void* buffer, int64_t offset, int32_t size);
    virtual bool Close();
    virtual bool IsAvailable(int64_t offset, int64_t size) const;
    virtual bool Invalidate(int64_t offset, int64_t size);
    virtual bool IsCompleted() const;
    virtual bool IsMatch(const char* path, int64_t size, int32_t contentId) const;
    virtual ~PartialFile();
    void GetChunkRange(int32_t* chunks, int64_t offset, int64_t size);
    void MarkDirty(int32_t first, int32_t last);
    bool ParseFile();
    bool RebuildHeader();
    bool RebuildLocalityMap(bool complete);
    bool UpdatePartialBlock(uint32_t block, uint32_t size);
    bool WriteLocality(int32_t first, int32_t last);
    bool IsLocalityComplete();
};

class CacheSource {
    public:
    /* 0004 */ StorageUnit* m_unit;

    CacheSource(StorageUnit* unit);
    virtual ~CacheSource();
    virtual bool Read(void* buffer, int64_t offset, int32_t size);
};

class HttpSource {
    public:
    /* 0004 */ std::string m_path;
    /* 0028 */ int64_t m_size;

    HttpSource(std::string path, int64_t size);
    virtual ~HttpSource();
    virtual bool Read(void* buffer, int64_t offset, int32_t size);
};

class StorageUnit {
    public:
    /* 0000 */ bool m_valid;
    /* 0004 */ std::string m_name;
    /* 0020 */ bool m_flushed;
    /* 0028 */ int64_t m_logicalFileSize;
    /* 0030 */ int32_t m_chunkSize;
    /* 0034 */ std::vector<uint8_t> m_chunkStates;
    /* 004C */ Blizzard::Lock::Atomic m_refCount;
    /* 0050 */ BackingStore* m_local;
    /* 0054 */ CacheSource* m_cache;
    /* 0058 */ HttpSource* m_http;
    /* 005C */ std::deque<STORAGE_CMD>* m_cacheEntries;
    /* 0060 */ int16_t m_cacheTag;

    StorageUnit();
    ~StorageUnit();
    void AddCacheEntry(int16_t tag, int64_t offset, int32_t size, BufferHandle handle);
    bool FetchDeferData(Range* range, int16_t tag);
    void FlushCacheEntries(int16_t tag);
    bool ReadLocal(void* buffer, int64_t offset, int32_t size);
    void ReserveChunks();
    void SetCacheSource();
    void SetHttpSource(const std::string& path, int64_t size);
    void SetLocal(BackingStore* local, int64_t size);
};

extern StorageSystem g_StorageSystem;
extern std::map<std::string, StorageUnit*>* s_StorageMap;
extern std::deque<STORAGE_CMD>* s_CmdQueue_Storage;
extern Blizzard::Lock::Mutex s_storageMutex;
extern Blizzard::Lock::Event s_cmdEvent;
extern bool s_storageReady;
extern volatile bool s_threadShutdown;
extern volatile bool s_threadExited;
extern int32_t s_Chunk_Size;

StorageUnit* Acquire(const std::string& name);
BufferHandle AllocateChunksRecovering(StorageUnit* unit, int32_t size);
bool Close(StorageUnit* unit);
void FetchError(StorageUnit* unit, int64_t offset, int32_t size, ChunkState::State state);
int32_t GetChunkSize();
int32_t GetRandomRange(StorageUnit* unit, int64_t offset, int32_t size, Range* ranges);
Range* GetRange(StorageUnit* unit, int64_t offset, int32_t size, int32_t* count, bool lock);
StorageUnit* GetStorageInfo(const std::string& name);
bool Initialize();
void QueueWriteCommand(StorageUnit* unit, int16_t tag, int64_t offset, int32_t size, BufferHandle handle);
bool Read(StorageUnit* unit, void* buffer, int64_t offset, int32_t size);
void Release(StorageUnit* unit);
bool SetRangeAs(StorageUnit* unit, int64_t offset, int32_t size, ChunkState::State state);
void Shutdown();
uint32_t StorageThread(void* param);
bool Write(StorageUnit* unit, BufferHandle handle, int64_t offset, int32_t size, int16_t tag);

}
}

namespace Blizzard {
namespace Mopaq {

void SFileGetTotalIsLocalAmount(int64_t& local, int64_t& total);

}
}

#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.inl"

#endif
