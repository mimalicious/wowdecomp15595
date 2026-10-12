#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_STACKEDMOPAQ_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_STACKEDMOPAQ_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SBig.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

namespace System_Mopaq {
struct CacheData;
struct ChecksumData;
struct DeltaData;
struct MD5VerifyData;
struct MemoryData;
struct AlignData;
struct VerifyData;
class IOHandler;
struct MpqParams;
}

namespace Blizzard {
namespace Mopaq {

class MpqErrorHandler;
struct Mpqsystem;

typedef bool (*Mpqfunc)(Mpqsystem* system, System_Mopaq::MpqParams* parms);
typedef bool (*HandlerReadFunc)(System_Mopaq::IOHandler* handler, void* buffer, int64_t offset, int32_t* count);
typedef bool (*HandlerWriteFunc)(System_Mopaq::IOHandler* handler, const void* buffer, int64_t offset, int32_t count);

struct Mpqsystem {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ Mpqsystem* m_next;
    /* 0008 */ Mpqfunc m_open;
    /* 000C */ Mpqfunc m_close;
    /* 0010 */ Mpqfunc m_read;
    /* 0014 */ Mpqfunc m_write;
    /* 0018 */ Mpqfunc m_flush;
    /* 001C */ Mpqfunc m_getfilesize;
    /* 0020 */ Mpqfunc m_setfilesize;
    /* 0024 */ Mpqfunc m_setposition;
    /* 0028 */ Mpqfunc m_reload;
    /* 002C */ Mpqfunc m_abandon;
};

class MpqHandleRecord {
    public:
    /* 0000 */ System_SFile::BLOCKENTRY m_blockEntry;
    /* 003C */ uint32_t m_blockIndex;
    /* 0040 */ int64_t m_position;
    /* 0048 */ Blizzard::File::StreamRecord* m_file;
    /* 004C */ char* m_name;
    /* 0050 */ System_Mopaq::ChecksumData* m_checksum;
    /* 0054 */ Blizzard::Lock::Mutex* m_lock;
    /* 0058 */ System_Mopaq::IOHandler* m_handler;
    /* 005C */ MpqErrorHandler* m_errorHandler;
    /* 0060 */ HandlerReadFunc m_readFunc;
    /* 0064 */ HandlerWriteFunc m_writeFunc;
    /* 0068 */ Mpqsystem* m_stack;
    /* 006C */ System_SFile::ARCHIVEREC* m_archive;
    /* 0070 */ System_SFile::ARCHIVEREC* m_logicalArchive;
    /* 0074 */ void* unk_0074;
    /* 0078 */ Blizzard::Lock::Atomic m_refCount;
    /* 007C */ System_Mopaq::MemoryData* m_memory;
    /* 0080 */ System_Mopaq::AlignData* m_align;
    /* 0084 */ System_Mopaq::CacheData* m_cache;
    /* 0088 */ System_Mopaq::VerifyData* m_verify;
    /* 008C */ System_Mopaq::DeltaData* m_delta;
    /* 0090 */ System_Mopaq::MD5VerifyData* m_md5Verify;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ int64_t m_baseOffset;
    /* 00A0 */ bool m_signatureFailed;

    int32_t Release();
};

struct HSFILERecord {
    /* 0000 */ MpqHandleRecord* m_handle;

    HSFILERecord();
    HSFILERecord(const HSFILERecord& other);
    void Assign(const HSFILERecord& other);
    bool Flush();
    const char* GetFileName();
    Blizzard::File::StreamRecord* GetFileStream();
    int64_t GetFileSize();
    bool Read(void* buffer, int64_t offset, int32_t count, int32_t* amount);
    bool Repair();
    void SetLogicalArchive(System_SFile::ARCHIVEREC* archive);
    void SetVerifyBaseOffset(int64_t offset);
    bool SetFileSize(int64_t size);
    bool Write(const void* buffer, int64_t offset, int32_t count);
};

}
}

namespace System_Mopaq {

struct MpqParams {
    /* 0000 */ uint32_t m_verb;
    /* 0004 */ Blizzard::Mopaq::MpqHandleRecord* m_handle;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_path;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ const System_SFile::BLOCKENTRY* m_blockEntry;
    /* 0020 */ uint32_t m_blockIndex;
    /* 0024 */ void* m_buffer;
    /* 0028 */ int64_t m_offset;
    /* 0030 */ int32_t m_count;
};

struct MemoryData {
    /* 0000 */ Blizzard::Util::Array<uint8_t>* m_array;
    /* 0004 */ bool m_owned;
    /* 0005 */ uint8_t unk_0005[3];

    MemoryData(Blizzard::Util::Array<uint8_t>* array);
};

struct AlignData {
    /* 0000 */ void* m_buffer;
    /* 0004 */ uint8_t* m_alignedBuffer;
    /* 0008 */ uint32_t m_alignMask;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ int64_t m_size;
};

struct ChecksumData {
    typedef void (*ChecksumFunc)(ChecksumData* data, const void* buffer, int32_t size);
    typedef bool (*FinishFunc)(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle);

    static void ChecksumCrc(ChecksumData* data, const void* buffer, int32_t size);
    static void ChecksumCrcAndMd5(ChecksumData* data, const void* buffer, int32_t size);
    static void ChecksumMd5(ChecksumData* data, const void* buffer, int32_t size);
    static bool FinishCrc(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle);
    static bool FinishCrcAndMd5(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle);
    static bool FinishMd5(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle);
    static bool FinishSigning(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle);

    /* 0000 */ uint32_t m_crc;
    /* 0004 */ Blizzard::Util::MD5 m_md5;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ int64_t m_position;
    /* 0068 */ int64_t m_size;
    /* 0070 */ ChecksumFunc m_checksum;
    /* 0074 */ FinishFunc m_finish;

    bool GetRange(const void* buffer, int64_t offset, uint32_t count, const void** start, uint32_t* amount);
    void InitCrc();
    void InitCrcAndMd5();
    void InitMd5();
    void InitSigning(Blizzard::Mopaq::MpqHandleRecord* handle);
    bool Update(Blizzard::Mopaq::MpqHandleRecord* handle, const void* buffer, int64_t offset, uint32_t count);
};

struct VerifyData {
    struct ChecksumTable {
        struct ChecksumEntry32 {
            /* 0000 */ uint32_t m_valid;
            /* 0004 */ uint32_t m_checksums[32];
        };

        /* 0000 */ std::vector<ChecksumEntry32> m_entries;

        void ClearChecksum(uint32_t index);
        void ForgetAllChecksumsAtOrBeyond(uint32_t index);
        void SetChecksum(uint32_t index, uint32_t checksum);
    };

    /* 0000 */ ChecksumTable m_table;
    /* 0018 */ int64_t m_base;
    /* 0020 */ uint32_t m_blockSize;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ int64_t m_size;
};

void CreateArchiveFile(const char* name, System_SFile::ARCHIVEREC* archive, uint32_t flags, const System_SFile::BLOCKENTRY& entry, uint32_t index, Blizzard::Mopaq::HSFILERecord** result);
bool CreateDiskFile(Blizzard::Mopaq::HSFILERecord** result, const char* path, uint32_t flags);
void CreateDiskFile(const char* name, uint32_t flags, const char* path, System_SFile::ARCHIVEREC* archive, Blizzard::Mopaq::HSFILERecord** result);
void OpenArchiveFile(const char* name, System_SFile::ARCHIVEREC* logicalArchive, System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY& entry, uint32_t index, uint32_t flags, Blizzard::Mopaq::HSFILERecord** result);
void OpenDiskFile(const char* name, uint32_t flags, const char* path, System_SFile::ARCHIVEREC* archive, Blizzard::Mopaq::HSFILERecord** result);
bool HoistAll(Blizzard::Mopaq::Mpqsystem* system);
void InitStack(Blizzard::Mopaq::Mpqsystem& system, int32_t flags);
bool Push(Blizzard::Mopaq::Mpqsystem& system, Blizzard::Mopaq::Mpqsystem* layer);
Blizzard::Mopaq::Mpqsystem* FindIOStackManager(int32_t flags);
void InitializeHandle(Blizzard::Mopaq::MpqHandleRecord* handle, MpqParams* parms);
bool ReadAndInitializeMetadata(Blizzard::Mopaq::MpqHandleRecord* handle);

bool CallNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool ReadFromNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms, void* buffer, int64_t offset, int32_t count, int32_t* amount);
bool WriteToNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms, const void* buffer, int64_t offset, int32_t count);

bool null_func(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_setposition(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool base_reload(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool mopaq_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool metadata_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool metadata_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool delta_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool delta_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool delta_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool delta_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool delta_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool checksum_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool checksum_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool checksum_operation(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool checksum_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool lock_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool lock_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool lock_operation(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool memory_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool align_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool cache_abandon(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool verify_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool md5verify_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool md5verify_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool md5verify_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool md5verify_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool offset_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool offset_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool offset_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
bool offset_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);

extern Blizzard::Mopaq::Mpqsystem s_nullstack;
extern Blizzard::Mopaq::Mpqsystem s_basestack;
extern Blizzard::Mopaq::Mpqsystem s_mopaqstack;
extern Blizzard::Mopaq::Mpqsystem s_metadatastack;
extern Blizzard::Mopaq::Mpqsystem s_deltastack;
extern Blizzard::Mopaq::Mpqsystem s_checksumstack;
extern Blizzard::Mopaq::Mpqsystem s_lockstack;
extern Blizzard::Mopaq::Mpqsystem s_memorystack;
extern Blizzard::Mopaq::Mpqsystem s_alignstack;
extern Blizzard::Mopaq::Mpqsystem s_cachestack;
extern Blizzard::Mopaq::Mpqsystem s_verifystack;
extern Blizzard::Mopaq::Mpqsystem s_md5verifystack;
extern Blizzard::Mopaq::Mpqsystem s_offsetstack;

}

bool PurgeCacheFile(Blizzard::Mopaq::HSFILERecord* file);
bool PrepareHandleForMD5Verification(Blizzard::Mopaq::MpqHandleRecord* handle, int32_t chunkSize, uint32_t flags);

class MpqSystemList : public Blizzard::Using::Use {
    public:
    struct Node {
        /* 0000 */ int32_t m_flags;
        /* 0004 */ Blizzard::Mopaq::Mpqsystem m_system;
        /* 0034 */ Node* m_next;

        Node(int32_t flags);
        ~Node();
    };

    struct Implementation {
        /* 0000 */ Node* m_head;
        /* 0004 */ Blizzard::Lock::Mutex m_mutex;

        Blizzard::Mopaq::Mpqsystem* GetStack(int32_t flags);
        void InitializeNewStackAtHead(Node* after, int32_t flags);
    };

    static void ShutdownProc();

    static Implementation* s_list;

    MpqSystemList();
};

#endif
