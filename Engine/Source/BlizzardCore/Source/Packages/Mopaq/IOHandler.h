#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_IOHANDLER_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_IOHANDLER_H

#include <cstdint>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Types.h"

namespace Blizzard {
namespace Mopaq {

class MpqErrorHandler {
    public:
    virtual ~MpqErrorHandler();
    virtual bool HandleInitSectorTableError(MpqHandleRecord* handle, uint8_t* table, int64_t offset, uint32_t size);
    virtual bool HandleReadChecksumError(MpqHandleRecord* handle);
    virtual bool HandleDecompressSectorError(uint32_t sector);
    virtual bool HandleReadMPQFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t* count);
    virtual bool HandleWriteFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset);
    virtual bool HandleMD5BlockError(MpqHandleRecord* handle, uint8_t* buffer, int64_t fileSize, uint32_t block, int32_t blockSize, uint32_t size, uint8_t* md5);
    virtual bool HandleNonStreamableFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t count);
    virtual bool GetBadSectorList(std::vector<uint32_t>& sectors);
};

class MpqRepairOnDemandErrorHandler : public MpqErrorHandler {
    public:
    static bool HandleLoadAndVerify(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size, const Blizzard::Util::Digest<16>& md5);
    static bool HandleMPQHeaderV3Error(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size);

    virtual ~MpqRepairOnDemandErrorHandler();
    virtual bool HandleInitSectorTableError(MpqHandleRecord* handle, uint8_t* table, int64_t offset, uint32_t size);
    virtual bool HandleMD5BlockError(MpqHandleRecord* handle, uint8_t* buffer, int64_t fileSize, uint32_t block, int32_t blockSize, uint32_t size, uint8_t* md5);
    virtual bool HandleNonStreamableFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t count);
};

class MpqStandaloneRepairErrorHandler : public MpqErrorHandler {
    public:
    /* 0004 */ std::vector<uint32_t> m_badSectors;

    MpqStandaloneRepairErrorHandler();
    virtual ~MpqStandaloneRepairErrorHandler();
    virtual bool HandleDecompressSectorError(uint32_t sector);
    virtual bool GetBadSectorList(std::vector<uint32_t>& sectors);
};

}
}

namespace System_Mopaq {

class IOHandler {
    public:
    static bool InitializeAndRead(IOHandler* handler, void* buffer, int64_t offset, int32_t* count);
    static bool ReadData(IOHandler* handler, void* buffer, int64_t offset, int32_t* count);
    static bool WriteData(IOHandler* handler, const void* buffer, int64_t offset, int32_t count);

    /* 0008 */ Blizzard::Mopaq::MpqHandleRecord* m_handle;
    /* 000C */ Blizzard::Mopaq::HSFILERecord m_base;
    /* 0010 */ int64_t m_baseOffset;

    IOHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, Blizzard::Mopaq::HSFILERecord& base, int64_t baseOffset);
    virtual ~IOHandler();
    virtual bool ReadData(void* buffer, int64_t offset, int32_t* count);
    virtual bool InitializeAndRead(void* buffer, int64_t offset, int32_t* count);
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool Flush();
    virtual int32_t TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset);
    virtual void UpdateBaseOffset();
    virtual bool AdjustSectorTable();
    virtual void SetSizeFileBeforeDelta(uint64_t size);
};

class MopaqReadHandler : public IOHandler {
    public:
    MopaqReadHandler(Blizzard::Mopaq::MpqHandleRecord*& handle);
    virtual bool ReadData(void* buffer, int64_t offset, int32_t* count);
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual int32_t TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset);
    virtual bool PerformRead(void* buffer, int64_t offset, int32_t count);
};

class MopaqReadWriteHandler : public MopaqReadHandler {
    public:
    /* 0018 */ uint32_t m_written;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ uint64_t m_sizeBeforeDelta;

    MopaqReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle);
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool Flush();
    virtual void SetSizeFileBeforeDelta(uint64_t size);

    void UpdateBlockEntry();
};

class MopaqInPlaceReadWriteHandler : public MopaqReadWriteHandler {
    public:
    /* 0028 */ int32_t m_writtenInPlace;
    /* 002C */ uint8_t unk_002C[4];

    MopaqInPlaceReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle);
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool Flush();
};

class SectorReadHandler : public MopaqReadHandler {
    public:
    typedef bool (*TransformFunc)(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector);
    typedef bool (*DecompressFunc)(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);

    struct SectorCache {
        /* 0000 */ uint8_t* m_data;
        /* 0004 */ int32_t m_sector;

        SectorCache(uint32_t size);
    };

    static bool ChecksumDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector);
    static bool Decompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector);
    static bool DecryptChecksumDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector);
    static bool DecryptDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector);
    static bool PkwareDecompressWrapper(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);

    /* 0018 */ uint32_t m_sectorSize;
    /* 001C */ uint32_t m_key;
    /* 0020 */ uint32_t* m_sectorTable;
    /* 0024 */ uint32_t* m_checksums;
    /* 0028 */ uint32_t m_firstSector;
    /* 002C */ uint32_t m_lastSector;
    /* 0030 */ uint32_t m_sectorCount;
    /* 0034 */ SectorCache* m_sectorCache;
    /* 0038 */ TransformFunc m_transform;
    /* 003C */ DecompressFunc m_decompress;
    /* 0040 */ int32_t m_sectorTableState;
    /* 0044 */ uint8_t unk_0044[4];

    SectorReadHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory);
    virtual ~SectorReadHandler();
    virtual bool ReadData(void* buffer, int64_t offset, int32_t* count);
    virtual bool InitializeAndRead(void* buffer, int64_t offset, int32_t* count);
    virtual int32_t TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset);

    void AllocateSectorTable();
    bool InitializeChecksumTable();
    bool InitializeSectorTable();
    bool ProcessSectors(uint8_t* source, uint8_t* dest, const int32_t* range);
    bool ReadAndDecompressData(uint8_t* dest, const int32_t* range);
    bool UnalignedRead(uint8_t* dest, const int32_t* range);
    bool ValidateSectorTable();
};

class SectorReadWriteHandler : public SectorReadHandler {
    public:
    /* 0048 */ uint32_t m_written;
    /* 004C */ uint8_t unk_004C[4];
    /* 0050 */ uint64_t m_sizeBeforeDelta;
    /* 0058 */ std::vector<Blizzard::Mopaq::CompressionAlgorithm> m_compression;
    /* 0070 */ bool m_reserved;
    /* 0071 */ uint8_t unk_0071[7];

    SectorReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory);
    virtual ~SectorReadWriteHandler();
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool Flush();
    virtual bool AdjustSectorTable();
    virtual void SetSizeFileBeforeDelta(uint64_t size);
    virtual bool WriteChecksumTable();
    virtual bool FinalizeWrite();
    virtual bool PerformWrite(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool CompressAndWrite(const uint8_t* buffer, uint32_t sector, uint32_t count);

    void MarkSingleUnit();
    bool PrepareSector(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t* destSize, uint32_t sector);
    void ResetSectorCount();
    void UpdateBlockHashTable();
};

class SectorInPlaceReadWriteHandler : public SectorReadWriteHandler {
    public:
    SectorInPlaceReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory);
    virtual ~SectorInPlaceReadWriteHandler();
    virtual bool WriteData(const uint8_t* buffer, int64_t offset, int32_t count);
    virtual bool WriteChecksumTable();
    virtual bool FinalizeWrite();
    virtual bool CompressAndWrite(const uint8_t* buffer, uint32_t sector, uint32_t count);
};

}

#endif
