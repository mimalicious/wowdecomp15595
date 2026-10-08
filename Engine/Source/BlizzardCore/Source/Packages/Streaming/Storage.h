#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <intrin.h>
#include <map>
#include <string>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

namespace Blizzard {
namespace File {
struct Filesystem;
struct StreamRecord;
}
}

namespace Blizzard {
namespace Streaming {
class CompletionCallback;
}
}

namespace Blizzard {
namespace Storage {

class Request;
class StorageUnit;

union BufferHandle {
    /* 0000 */ uint32_t all;
    struct {
        uint32_t start : 7;
        uint32_t length : 7;
    } m;

    BufferHandle();
};

struct ChunkRange {
    /* 0000 */ int64_t m_offset;
    /* 0008 */ int64_t m_length;
    /* 0010 */ int32_t m_chunkCount;
    /* 0014 */ bool m_complete;
};

struct ReadSegment {
    /* 0000 */ int64_t m_offset;
    /* 0008 */ int64_t m_length;
    /* 0010 */ int32_t m_chunkSize;
    /* 0014 */ int32_t m_source;

    void Set(int64_t offset, int64_t length, const ChunkRange* missing);
};

struct STORAGE_CMD {
    /* 0000 */ Request* m_request;
    /* 0004 */ BufferHandle m_handle;
    /* 0008 */ int64_t m_offset;
    /* 0010 */ int32_t m_length;
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
    BufferHandle AllocateChunks(int32_t count);
    BufferHandle AllocateChunksUpTo(int32_t count);
    BufferHandle AllocateSubChunks(BufferHandle handle, int32_t offset, int32_t size);
    BufferHandle FindFreeRun(int32_t count, int32_t start);
    uint8_t* GetBuffer(BufferHandle handle);
    void ReleaseChunks(BufferHandle handle);
};

class BitField {
    public:
    /* 0000 */ uint8_t* m_bits;
    /* 0004 */ int32_t m_length;
    /* 0008 */ void* m_block;
    /* 000C */ void (*m_free)(void* block);

    void Clear(int32_t index);
    int64_t CountSet() const;
    bool IsSetUpTo(int32_t count) const;
    void Set(int32_t index);
};

class BitFieldAllocator {
    public:
    virtual ~BitFieldAllocator();
    virtual BitField* Attach(const char* name, int64_t bits, BitField* field) = 0;
    virtual BitField* Create(int64_t bits) = 0;
    virtual void Destroy(BitField* field) = 0;
};

class DefaultBitFieldAllocator : public BitFieldAllocator {
    public:
    BitField* Attach(const char* name, int64_t bits, BitField* field) override;
    BitField* Create(int64_t bits) override;
    void Destroy(BitField* field) override;
};

class SharedBitFieldAllocator : public DefaultBitFieldAllocator {
    public:
    BitField* Attach(const char* name, int64_t bits, BitField* field) override;
};

class PartialBlock {
    public:
    /* 0000 */ uint32_t m_lastBlockSize;
    /* 0004 */ uint32_t m_blockSize;
    /* 0008 */ uint32_t m_blockCount;
    /* 000C */ std::map<uint32_t, uint32_t> m_partial;
    /* 002C */ std::string m_name;

    PartialBlock(uint32_t blockSize, uint32_t blockCount, uint32_t lastBlockSize, std::string name);
    ~PartialBlock();
    bool SetPartial(uint32_t block, uint32_t size);
};

class PartialBlockMap : public std::map<std::string, std::map<uint32_t, uint32_t> > {
    public:
    ~PartialBlockMap();
};

class LockFile {
    public:
    /* 0000 */ Blizzard::File::StreamRecord* m_file;
    /* 0004 */ std::string m_path;

    LockFile();
    bool Acquire();
    bool IsStale(uint32_t seconds);
};

class BackingStore {
    public:
    /* 0004 */ uint8_t m_type;

    BackingStore(uint8_t type);
    virtual ~BackingStore();
    virtual bool Read(void* buffer, int64_t offset, int64_t size) = 0;
    virtual bool FindMissing(int64_t offset, int64_t size, ChunkRange* range) = 0;
    virtual bool GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) = 0;
    virtual void AlignRange(ChunkRange* range) = 0;
};

class PartialFile : public BackingStore {
    public:
    static PartialFile* CreateShared(Blizzard::File::Filesystem* fs, const char* path, int64_t size, int32_t contentId);
    static bool FileIsCompletePartialFile(Blizzard::File::StreamRecord* file, int64_t archiveSize, int64_t fileSize);
    static bool ParseFileFromHeader(Blizzard::File::StreamRecord* file, BitField** locality, int32_t* chunkSize, int64_t* fileSize, int32_t* contentId);

    /* 0008 */ int32_t m_chunkSize;
    /* 0010 */ int64_t m_fileSize;
    /* 0018 */ int32_t m_contentId;
    /* 001C */ Blizzard::File::StreamRecord* m_file;
    /* 0020 */ LockFile m_lock;
    /* 0040 */ int32_t m_dirtyStart;
    /* 0044 */ int32_t m_dirtyEnd;
    /* 0048 */ uint32_t m_lastFlush;
    /* 004C */ BitField* m_locality;
    /* 0050 */ uint8_t m_localityHash[16];
    /* 0060 */ PartialBlock* m_partialBlocks;
    /* 0064 */ bool m_open;
    /* 0065 */ bool m_completed;
    /* 0066 */ bool m_assumedComplete;
    /* 0068 */ int32_t m_invalidateCount;

    PartialFile();
    ~PartialFile();
    bool Read(void* buffer, int64_t offset, int64_t size) override;
    bool FindMissing(int64_t offset, int64_t size, ChunkRange* range) override;
    bool GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) override;
    void AlignRange(ChunkRange* range) override;
    virtual int32_t GetChunkSize() const;
    virtual int64_t GetTotalLocalSize() const;
    virtual bool Write(const void* buffer, int64_t offset, int32_t size);
    virtual bool Flush();
    virtual bool Close();
    virtual bool WriteUnaligned(const void* buffer, int64_t offset, int32_t size);
    virtual void UpdateUnaligned(int64_t offset, int32_t size);
    virtual bool IsAvailable(int64_t offset, int64_t size) const;
    virtual int64_t GetLocalBytes(int64_t offset, int64_t size) const;
    virtual bool Invalidate(int64_t offset, int64_t size);
    virtual bool IsCompleted() const;
    virtual bool IsReadOnly() const;
    virtual bool GetMissingRange(int64_t offset, int64_t size, ChunkRange* range) const;
    virtual bool IsMatch(const char* path, int64_t size, int32_t contentId) const;
    void GetChunkRange(int32_t* chunks, int64_t offset, int64_t size);
    bool IsLocalityComplete();
    void MakeSparse();
    bool ParseFile();
    bool RebuildHeader();
    bool RebuildLocalityMap(bool complete);
    bool UpdatePartialBlock(uint32_t block, uint32_t size);
    bool WriteLocality(int32_t first, int32_t last);
};

class CacheSource : public BackingStore {
    public:
    CacheSource();
    bool Read(void* buffer, int64_t offset, int64_t size) override;
    bool FindMissing(int64_t offset, int64_t size, ChunkRange* range) override;
    bool GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) override;
    void AlignRange(ChunkRange* range) override;
};

class HttpSource : public BackingStore {
    public:
    /* 0008 */ std::string m_path;
    /* 0028 */ int64_t m_size;

    HttpSource(std::string path, int64_t size);
    ~HttpSource();
    bool Read(void* buffer, int64_t offset, int64_t size) override;
    bool FindMissing(int64_t offset, int64_t size, ChunkRange* range) override;
    bool GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) override;
    void AlignRange(ChunkRange* range) override;
};

class StorageUnit {
    public:
    /* 0000 */ bool m_valid;
    /* 0004 */ std::string m_name;
    /* 0020 */ bool m_flushed;
    /* 0028 */ int64_t m_logicalFileSize;
    /* 0030 */ int32_t m_chunkSize;
    /* 0034 */ Blizzard::Lock::Atomic m_refCount;
    /* 0038 */ PartialFile* m_local;
    /* 003C */ BackingStore* m_cache;
    /* 0040 */ BackingStore* m_http;
    /* 0044 */ std::deque<STORAGE_CMD>* m_cacheEntries;

    StorageUnit();
    ~StorageUnit();
    bool AddCacheEntry(Request* request, int64_t offset, int64_t size, BufferHandle handle);
    int32_t FetchRequest(Request* request, ChunkRange* range);
    bool FindMissingRange(int64_t offset, int64_t size, ChunkRange* range);
    bool FlushCacheEntries(int64_t maxBytes);
    bool IsAvailable(int64_t offset, int64_t size);
    bool QueueWrite(Request* request, BufferHandle handle, int64_t offset, int64_t size);
    bool Read(int64_t offset, int64_t size, void* buffer);
    bool ReadRemote(void* buffer, int64_t offset, int32_t size);
    bool ReadRemoteUnaligned(void* buffer, int64_t offset, int32_t size);
    bool ReadToCache(ChunkRange* range, BufferHandle* handle);
    void SetCacheSource();
    void SetHttpSource(const std::string& path, int64_t size);
    void SetLocal(PartialFile* local, int64_t size);
};

class Request {
    public:
    enum State {
        STATE_NONE = 0,
        STATE_LOCAL = 1,
        STATE_PENDING = 2,
        STATE_CANCELLED = 3,
        STATE_NO_UNIT = 4,
        STATE_FAILED = 5
    };

    /* 0000 */ StorageUnit* m_unit;
    /* 0008 */ int64_t m_offset;
    /* 0010 */ int64_t m_size;
    /* 0018 */ Blizzard::Lock::Event m_event;
    /* 001C */ int32_t m_state;
    /* 0020 */ bool m_cancelled;
    /* 0024 */ Blizzard::Streaming::CompletionCallback* m_callback;
    /* 0028 */ const int64_t* m_chunk;
    /* 002C */ Blizzard::Lock::Atomic m_refCount;
    /* 0030 */ Blizzard::Lock::Atomic m_pendingWrites;

    static Request* Create(StorageUnit* unit, int64_t offset, int64_t size);

    Request(StorageUnit* unit, int64_t offset, int64_t size);
    ~Request();
    void AddRef();
    void Cancel();
    void CheckLocal();
    void OnWriteComplete(bool failed);
    void Release();
};

class StorageUnitRef {
    public:
    /* 0000 */ StorageUnit* m_unit;

    StorageUnitRef(StorageUnit* unit);
    ~StorageUnitRef();
};

extern StorageSystem g_StorageSystem;
extern PartialBlockMap s_partialBlockMap;
extern Blizzard::Lock::Atomic s_partialBlockMapDestroyed;
extern std::map<std::string, StorageUnit*>* s_StorageMap;
extern Blizzard::Lock::Mutex s_storageMutex;
extern Blizzard::Lock::Event s_cmdEvent;
extern volatile bool s_threadShutdown;
extern volatile bool s_threadExited;
extern bool s_storageReady;
extern volatile long s_initLock;
extern int32_t s_Chunk_Size;
extern BitFieldAllocator* s_bitFieldAllocator;

StorageUnit* Acquire(const std::string& name, bool existingOnly);
bool Close(StorageUnit* unit);
bool Exists(const std::string& name);
int32_t GetChunkSize();
bool Initialize();
void InitializeBitFieldAllocator(int32_t mode);
bool IsInitialized();
void Release(StorageUnit* unit);
void Shutdown();
void ShutdownBitFieldAllocator();
uint32_t StorageThread(void* param);

}
}

namespace Blizzard {
namespace Mopaq {

void SFileGetTotalIsLocalAmount(int64_t& local, int64_t& total);

}
}

#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.inl"

#endif
