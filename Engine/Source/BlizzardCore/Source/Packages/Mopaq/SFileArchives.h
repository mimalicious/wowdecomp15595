#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Include/Blizzard/STL/CaseInsensitive.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Types.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SBig.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"

namespace Blizzard {
namespace Mopaq {
struct ArchiveOptions;
struct HSFILERecord;
}
namespace Streaming {
struct MakeLocalParams;
}
}

namespace System_SFile {

#pragma pack(push, 1)
struct BLOCKENTRY {
    /* 0000 */ int64_t m_offset;
    /* 0008 */ uint64_t m_compressedSize;
    /* 0010 */ uint64_t m_fileSize;
    /* 0018 */ uint32_t m_flags;
    /* 001C */ uint64_t m_fileTime;
    /* 0024 */ uint32_t m_crc;
    /* 0028 */ Blizzard::Util::Digest<16> m_md5;
    /* 0038 */ bool m_md5Check;
    /* 0039 */ bool m_patch;
};
#pragma pack(pop)

class BitPacked {
    public:
    /* 0000 */ uint8_t* m_data;
    /* 0004 */ uint32_t m_bitCount;

    BitPacked(uint32_t bitCount, uint8_t fill);
    ~BitPacked();
    void ReadBits(uint32_t start, uint32_t bits, uint8_t* out, uint32_t outSize) const;
    bool WriteBits(uint32_t start, uint32_t bits, const uint8_t* in, uint32_t inSize);
};

class BlockEntryTable {
    public:
    static BlockEntryTable* CreateBlockTableFromBuffer(const char* buffer, uint32_t size, uint32_t flags);

    /* 0000 */ std::vector<uint32_t> m_flagTable;
    /* 0018 */ uint32_t m_entryBits;
    /* 001C */ uint32_t m_offsetBit;
    /* 0020 */ uint32_t m_fileSizeBit;
    /* 0024 */ uint32_t m_compressedSizeBit;
    /* 0028 */ uint32_t m_flagIndexBit;
    /* 002C */ uint32_t m_md5Bit;
    /* 0030 */ uint32_t m_offsetBits;
    /* 0034 */ uint32_t m_fileSizeBits;
    /* 0038 */ uint32_t m_compressedSizeBits;
    /* 003C */ uint32_t m_flagIndexBits;
    /* 0040 */ uint32_t m_md5Bits;
    /* 0044 */ uint32_t m_hashEntryBits;
    /* 0048 */ uint32_t m_hashExtraBits;
    /* 004C */ int32_t m_hashBits;
    /* 0050 */ uint32_t m_md5CheckEntryBits;
    /* 0054 */ uint32_t m_md5CheckBit;
    /* 0058 */ uint32_t m_md5CheckBits;
    /* 005C */ BitPacked* m_md5Checks;
    /* 0060 */ BitPacked* m_hashes;
    /* 0064 */ BitPacked* m_entries;
    /* 0068 */ uint32_t m_count;
    /* 006C */ std::vector<uint64_t> m_fileTimes;
    /* 0084 */ std::vector<uint32_t> m_crcs;
    /* 009C */ std::vector<Blizzard::Util::Digest<16>> m_md5s;
    /* 00B4 */ std::vector<uint8_t> m_patchBits;
    /* 00CC */ uint32_t m_tableFlags;

    BlockEntryTable(uint32_t count, uint32_t flags);
    ~BlockEntryTable();
    void AddFlags(uint32_t flags);
    uint32_t GetBufferSize();
    BLOCKENTRY* GetMaxima(BLOCKENTRY* maxima, const std::vector<BLOCKENTRY>& entries);
    void InitializeHashCheckTable(int32_t bits);
    bool PackEntry(uint32_t index, const BLOCKENTRY* entry);
    bool SaveBlockTableToBuffer(char* buffer, uint32_t size);
    void SetLayout(BLOCKENTRY& maxima);
    bool SetMD5Check(uint32_t index, bool enabled);
    bool UnpackEntry(uint32_t index, BLOCKENTRY* entry);
    void UpdateLayout();
};

class HashEntryTable {
    public:
    static uint64_t CalculateHash(const char* name);
    static HashEntryTable* CreateHashEntryTableFromBuffer(const char* buffer, uint32_t size);

    /* 0000 */ uint32_t m_entryBits;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_indexBits;
    /* 000C */ uint8_t* m_tags;
    /* 0010 */ BitPacked* m_indices;
    /* 0014 */ uint32_t m_count;
    /* 0018 */ uint32_t m_slotCount;
    /* 001C */ int32_t m_hashBits;
    /* 0020 */ uint64_t m_hashMask;
    /* 0028 */ uint64_t m_hashTopBit;

    HashEntryTable(uint32_t count, int32_t hashBits);
    ~HashEntryTable();
    bool AddEntry(const char* name, uint32_t blockIndex, BlockEntryTable* blocks);
    uint32_t GetEntry(const uint64_t& hash, BlockEntryTable* blocks);
    bool SaveHashEntryTableToBuffer(char* buffer, uint32_t size);
    void SetHashBits(int32_t bits);
};

struct ExtTableHeader {
    /* 0000 */ uint32_t m_signature;
    /* 0004 */ uint32_t m_version;
    /* 0008 */ uint32_t m_dataSize;
};

struct BlockTableHeader {
    /* 0000 */ uint32_t m_tableSize;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t m_entryBits;
    /* 0010 */ uint32_t m_offsetBit;
    /* 0014 */ uint32_t m_fileSizeBit;
    /* 0018 */ uint32_t m_compressedSizeBit;
    /* 001C */ uint32_t m_flagIndexBit;
    /* 0020 */ uint32_t m_md5Bit;
    /* 0024 */ uint32_t m_offsetBits;
    /* 0028 */ uint32_t m_fileSizeBits;
    /* 002C */ uint32_t m_compressedSizeBits;
    /* 0030 */ uint32_t m_flagIndexBits;
    /* 0034 */ uint32_t m_md5Bits;
    /* 0038 */ uint32_t m_hashEntryBits;
    /* 003C */ uint32_t m_hashExtraBits;
    /* 0040 */ int32_t m_hashBits;
    /* 0044 */ uint32_t m_hashesSize;
    /* 0048 */ uint32_t m_flagCount;
};

struct HashTableHeader {
    /* 0000 */ uint32_t m_tableSize;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_tagsSize;
    /* 000C */ int32_t m_hashBits;
    /* 0010 */ uint32_t m_entryBits;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t m_indexBits;
    /* 001C */ uint32_t m_indicesSize;
};

struct HASHENTRY {
    /* 0000 */ uint32_t m_hashA;
    /* 0004 */ uint32_t m_hashB;
    /* 0008 */ uint16_t m_locale;
    /* 000A */ uint8_t m_platform;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint32_t m_blockIndex;
};

#pragma pack(push, 4)
struct MPQHEADER {
    /* 0000 */ uint32_t m_magic;
    /* 0004 */ uint32_t m_headerSize;
    /* 0008 */ uint32_t m_archiveSize;
    /* 000C */ uint16_t m_formatVersion;
    /* 000E */ uint16_t m_sectorSizeShift;
    /* 0010 */ uint32_t m_hashTablePos;
    /* 0014 */ uint32_t m_blockTablePos;
    /* 0018 */ uint32_t m_hashTableSize;
    /* 001C */ uint32_t m_blockTableSize;
    /* 0020 */ uint64_t m_hiBlockTablePos;
    /* 0028 */ uint16_t m_hashTablePosHi;
    /* 002A */ uint16_t m_blockTablePosHi;
    /* 002C */ uint64_t m_archiveSize64;
    /* 0034 */ uint64_t m_betTablePos;
    /* 003C */ uint64_t m_hetTablePos;
    /* 0044 */ uint64_t m_hashTableSize64;
    /* 004C */ uint64_t m_blockTableSize64;
    /* 0054 */ uint64_t m_hiBlockTableSize64;
    /* 005C */ uint64_t m_hetTableSize64;
    /* 0064 */ uint64_t m_betTableSize64;
    /* 006C */ int32_t m_rawChunkSize;
    /* 0070 */ Blizzard::Util::Digest<16> m_blockTableMd5;
    /* 0080 */ Blizzard::Util::Digest<16> m_hashTableMd5;
    /* 0090 */ Blizzard::Util::Digest<16> m_hiBlockTableMd5;
    /* 00A0 */ Blizzard::Util::Digest<16> m_betTableMd5;
    /* 00B0 */ Blizzard::Util::Digest<16> m_hetTableMd5;
    /* 00C0 */ Blizzard::Util::Digest<16> m_headerMd5;
};
#pragma pack(pop)

struct ArchiveBuffers {
    /* 0000 */ Blizzard::Util::Buffer<4, uint8_t> unk_0000;
    /* 000C */ Blizzard::Util::Buffer<256, uint8_t> unk_000C;
    /* 0114 */ Blizzard::Util::Buffer<256, uint8_t> unk_0114;
    /* 021C */ std::string unk_021C;

    ~ArchiveBuffers();
};

class ARCHIVEREC {
    public:
    typedef std::map<std::string, uint32_t, Blizzard::STL::CaseInsensitive> Listfile;

    static bool LoadOldTable(ARCHIVEREC* archive, void* table, Blizzard::Util::Buffer<4096, char>& buffer, uint32_t diskSize, uint32_t size, int64_t offset, uint32_t key, const Blizzard::Util::Digest<16>& md5);
    static bool LoadTable(ARCHIVEREC* archive, Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, int64_t offset, uint32_t size, char** data, uint32_t* dataSize, uint32_t key, uint32_t signature, const Blizzard::Util::Digest<16>& md5);
    static bool WriteArchiveChanges(ARCHIVEREC* archive);

    /* 0000 */ Blizzard::Lock::RMutex m_mutex;
    /* 0018 */ Blizzard::Mopaq::HSFILERecord* m_file;
    /* 001C */ int32_t m_priority;
    /* 0020 */ int32_t m_id;
    /* 0024 */ ARCHIVEREC* m_parent;
    /* 0028 */ MPQHEADER m_header;
    /* 00F8 */ int64_t m_archiveOffset;
    /* 0100 */ int64_t m_archiveEnd;
    /* 0108 */ int64_t m_userDataOffset;
    /* 0110 */ uint32_t m_userDataSize;
    /* 0114 */ uint32_t m_flags;
    /* 0118 */ uint32_t m_sectorSize;
    /* 011C */ uint32_t unk_011C;
    /* 0120 */ bool m_compressTables;
    /* 0121 */ bool unk_0121;
    /* 0122 */ uint8_t unk_0122[2];
    /* 0124 */ int32_t m_chunkSize;
    /* 0128 */ bool m_setFileSize;
    /* 0129 */ uint8_t unk_0129[7];
    /* 0130 */ int64_t m_minFileSize;
    /* 0138 */ char m_name[260];
    /* 023C */ std::string m_directPath;
    /* 0258 */ BlockEntryTable* m_blockEntryTable;
    /* 025C */ HashEntryTable* m_hashEntryTable;
    /* 0260 */ std::vector<BLOCKENTRY> m_blockTable;
    /* 0278 */ std::vector<HASHENTRY> m_hashTable;
    /* 0290 */ std::map<Blizzard::Util::Digest<16>, uint32_t> m_blockKeys;
    /* 02B0 */ bool m_blockKeysBuilt;
    /* 02B1 */ bool m_tablesInitialized;
    /* 02B2 */ uint16_t m_locale;
    /* 02B4 */ uint8_t m_platform;
    /* 02B5 */ uint8_t unk_02B5[3];
    /* 02B8 */ uint32_t m_attributeFlags;
    /* 02BC */ std::vector<Blizzard::Mopaq::CompressionAlgorithm> m_compression;
    /* 02D4 */ uint32_t unk_02D4;
    /* 02D8 */ ArchiveBuffers* m_buffers;
    /* 02DC */ bool (*m_closeCallback)(ARCHIVEREC* archive);
    /* 02E0 */ uint32_t unk_02E0;
    /* 02E4 */ Listfile m_listfile;
    /* 0304 */ bool m_listfileLoaded;
    /* 0305 */ uint8_t unk_0305[3];
    /* 0308 */ int32_t m_listfileRefcount;
    /* 030C */ Blizzard::Util::Array<uint8_t> m_patchMetadata;
    /* 0314 */ bool m_patchMetadataLoaded;
    /* 0315 */ uint8_t unk_0315[3];
    /* 0318 */ Blizzard::Lock::RMutex m_patchLock;
    /* 0330 */ int64_t m_writeReservationEnd;
    /* 0338 */ int64_t m_writeReservationPending;
    /* 0340 */ Blizzard::Lock::RMutex m_writeLock;
    /* 0358 */ Blizzard::Lock::Atomic m_internalRefs;
    /* 035C */ Blizzard::Lock::Atomic m_externalRefs;

    ARCHIVEREC();
    ~ARCHIVEREC();
    void CompleteWriteReservation(int64_t used, int64_t reserved);
    int64_t RequestWriteReservation(int64_t size, bool raw);
    void ReturnWriteReservation(int64_t offset, int64_t size, int64_t newSize, bool raw);
    bool AcquireListfile();
    void AcquireListfileCompressedBlockHash();
    void AcquireListfileUncompressedBlockHash();
    void CompactBlockTable();
    void ConvertToNewBlockTableFormat(bool clear);
    void ConvertToNewHashTableFormat(char* listfile, int32_t size, bool clear);
    bool FillNewHashTable(int32_t hashBits, char* listfile, int32_t size);
    BLOCKENTRY GetBlockEntry(uint32_t index);
    int64_t GetWriteReservationLimit();
    bool IncreaseHashBits(int32_t* hashBits);
    bool InitializeTables();
    bool InitPatchMetadata();
    void InitializeWriteReservations(int32_t headerSize);
    bool LoadBlockEntryTable(Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, char** data, uint32_t* dataSize);
    bool LoadHashEntryTable(Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, char** data, uint32_t* dataSize);
    bool LoadOldBlockTable(Blizzard::Util::Buffer<4096, char>& buffer);
    bool LoadOldHashTable(Blizzard::Util::Buffer<4096, char>& buffer);
    void PackBlockTable();
    bool PreRequestWriteReservation(int64_t size);
    bool ReadAdditionalAttributes();
    bool ReadTable(void* data, int64_t offset, uint32_t size, const Blizzard::Util::Digest<16>& md5);
    bool WriteArchiveHeader();
    bool WriteBitPackedBlockTable();
    bool WriteBitPackedHashTable();
    bool WriteBlockTable(bool& extended, bool reserve);
    bool WriteExtendedBlockTable(bool reserve);
    bool WriteHashTable();
    void ReleaseListfile();
    bool RemoveInternalArchiveRef();
    void SetBlockCrc(uint32_t index, uint32_t crc);
    void SetBlockEntry(uint32_t index, const BLOCKENTRY& entry);
    void SetBlockFileTime(uint32_t index, uint64_t fileTime);
    void SetBlockMd5(uint32_t index, const Blizzard::Util::Digest<16>& md5);
    void SetBlockPatch(uint32_t index, bool patch);
    void SetMD5Check(uint32_t index, bool enabled);
};

struct FileLookup {
    /* 0000 */ bool m_checkedDisk;
    /* 0001 */ uint8_t unk_0001[3];
    /* 0004 */ ARCHIVEREC* m_archive;
    /* 0008 */ uint32_t m_blockIndex;
    /* 000C */ char* m_path;
};

class ArchivePtr {
    public:
    /* 0000 */ ARCHIVEREC* m_archive;
    /* 0004 */ bool m_locked;
    /* 0005 */ uint8_t unk_0005[3];

    ArchivePtr(ARCHIVEREC* archive);
    ~ArchivePtr();
    void Reset(ARCHIVEREC* handle);
};

template <class T>
class HeapArray {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;

    HeapArray(uint32_t count);
    ~HeapArray();
};

extern Blizzard::Util::Digest<16> s_emptyMd5;
extern Blizzard::Lock::Atomic s_nextArchiveId;
extern uint32_t s_lastError;
extern bool s_threadSafe;

char* BackslashPath(const char* path, char* buffer, uint32_t size);
bool CheckFileExistsOnDisk(const char* name, uint32_t flags, char* result);
void ClearAllFileFlags();
void ClearNewBlockHashTables(ARCHIVEREC* archive);
bool ClearTables(ARCHIVEREC* archive, uint32_t hashCount);
void ConvertRelativePathName(const char* path, char* result);
ARCHIVEREC* GetArchivePtr(ARCHIVEREC* handle, bool addRef);
bool OpenDirectPathAsArchive(const char* path, int32_t priority, uint32_t flags, ARCHIVEREC** result);
bool PrepareArchiveForWriting(ARCHIVEREC* archive);
bool RebuildHashTable(ARCHIVEREC* archive, uint32_t hashSize);
bool s_OpenArchive(ARCHIVEREC* archive, uint32_t flags, ARCHIVEREC** result);
uint32_t SearchHashTable(ARCHIVEREC* archive, const char* name, bool matchNeutral);

}

int32_t FindFileInOneArchive(System_SFile::ARCHIVEREC* archive, const char* name, System_SFile::FileLookup* lookup, const uint64_t& hash, const Blizzard::Util::Digest<16>& md5, uint32_t flags);
int32_t GetFileBlockEntry(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut, System_SFile::ARCHIVEREC** recordOut, uint32_t* blockIndex, char* path);

namespace Blizzard {
namespace Mopaq {

bool SFileAuthenticateArchiveEx(System_SFile::ARCHIVEREC* handle, AuthResult* result, const uint8_t* modulus, uint32_t modulusSize, const uint8_t* exponent, uint32_t exponentSize, const char* salt);
struct ArchiveOptions {
    /* 0000 */ uint32_t m_sectorSize;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ bool m_compressTables;
    /* 0009 */ bool unk_0009;
    /* 000A */ uint8_t unk_000A[2];
    /* 000C */ int32_t m_rawChunkSize;
    /* 0010 */ bool m_setFileSize;
    /* 0011 */ uint8_t unk_0011[7];
    /* 0018 */ int64_t m_minFileSize;

    ArchiveOptions();
};

bool SFileCloseArchive(System_SFile::ARCHIVEREC* handle);
int32_t SFileDeleteFile(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags);
bool SFileCreateArchive(HSFILERecord* file, const char* name, int32_t priority, uint32_t userDataSize, System_SFile::ARCHIVEREC** result, const ArchiveOptions* options);
bool SFileCreateArchive(const char* path, int32_t priority, uint32_t userDataSize, System_SFile::ARCHIVEREC** result, const ArchiveOptions* options);
bool IsArchiveComplete(System_SFile::ARCHIVEREC* handle);
bool IsArchiveStreamingComplete(System_SFile::ARCHIVEREC* handle);
void SFileGetIsLocalAmount(System_SFile::ARCHIVEREC* handle, int64_t& local, int64_t& total);
bool SFileEnableDirectAccess(uint32_t enable);
uint32_t SFileGetArchiveBlockCount(System_SFile::ARCHIVEREC* handle);
bool SFileGetPatchMetadata(System_SFile::ARCHIVEREC* handle, Blizzard::Util::Buffer<4096, char>& metadata);
uint32_t SFileGetStreamedFileSize(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex);
System_SFile::ARCHIVEREC* SFileGetArchiveHandle(const char* name);
int32_t SFileFileExists(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut);
bool SFileGetFileDetails(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut, System_SFile::ARCHIVEREC*& record, System_SFile::BLOCKENTRY& entry);
bool SFileHasHigherPriority(const char* name, const char* other);
int32_t SFileFileIsLocal(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex);
int32_t SFileFileIsLocal(const char* name, int32_t option);
bool SFileGetIsLocalAmount(const char* name, int64_t& local, int64_t& total);
void SFileGetIsLocalAmount(HSFILERecord* file, int64_t& local, int64_t& total);
uint32_t GetStreamedSize(System_SFile::ARCHIVEREC* handle, uint32_t size);
bool SFileMakeFileLocal(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex, int64_t offset, uint32_t size, int32_t option, bool wait);
bool SFileMakeFileLocal(const char* name, int32_t option, bool wait, Blizzard::Streaming::MakeLocalParams* params);
bool SFileMakeFileLocal(HSFILERecord* file, int32_t option, bool wait, Blizzard::Streaming::MakeLocalParams* params);
uint32_t SFileGetLastError();
bool SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result);
bool SFileOpenArchive(HSFILERecord* file, const char* name, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result);
bool SFileOpenArchiveView(System_SFile::ARCHIVEREC* handle, const char* name, int32_t priority, System_SFile::ARCHIVEREC** result);
bool SFileSetBasePath(const char* path);
bool SFileSetChecksumFlags(System_SFile::ARCHIVEREC* handle, uint32_t flags);
bool SetupHashEntry(System_SFile::ArchivePtr& archive, const char* name, const System_SFile::BLOCKENTRY& entry, int32_t blockIndex);
bool SetFileSignature(const char* name, const Blizzard::Util::Digest<16>& md5);
void SetSignatureCheckMode(uint32_t mode);
void SetSignatureErrorCallback(void (*callback)(const char* name, const char* message));

}
}

// OFFSET: [%0x00717AD0, %0x00717D70]
template <class T>
System_SFile::HeapArray<T>::HeapArray(uint32_t count) {
    this->m_count = count;
    if (count <= 0) {
        this->m_data = NULL;
    } else {
        this->m_data = new T[count];
    }
}

// INLINED
template <class T>
System_SFile::HeapArray<T>::~HeapArray() {
    delete[] this->m_data;
}

#endif
