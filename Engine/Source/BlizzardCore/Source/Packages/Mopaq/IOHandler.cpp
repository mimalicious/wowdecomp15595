#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQEncrypt.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SComp.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "ThirdParty/zlib/zlib.h"

// INLINED
static bool IsStreamedFile(System_SFile::ARCHIVEREC* archive) {
    Blizzard::Mopaq::MpqHandleRecord* archiveHandle = archive->m_file->m_handle;
    Blizzard::File::StreamRecord* file = archiveHandle ? archiveHandle->m_file : NULL;
    Blizzard::File::StreamingInfo* streaming = file->m_streaming;

    return streaming && streaming->m_unit;
}

// OFFSET: %0x00724210
int32_t Blizzard::Mopaq::MpqHandleRecord::Release() {
    int32_t result = this->m_refCount.Decrement();

    if (!result) {
        System_Mopaq::MpqParams parms;
        memset(&parms, 0, sizeof(parms));
        parms.m_verb = offsetof(Mpqsystem, m_close);
        parms.m_handle = this;

        if (this->m_stack) {
            this->m_stack->m_close(this->m_stack, &parms);
        }

        Blizzard::Memory::Free(this);
        return 0;
    }

    return result;
}

// PROVEN: e4d123e8 IDENTICAL
// OFFSET: 0x007AB010
Blizzard::File::StreamRecord* Blizzard::Mopaq::HSFILERecord::GetFileStream() {
    return this->m_handle ? this->m_handle->m_file : NULL;
}

// PROVEN: 4ab43a64 IDENTICAL
// OFFSET: 0x007AB020
const char* Blizzard::Mopaq::HSFILERecord::GetFileName() {
    return this->m_handle ? this->m_handle->m_name : "";
}

// OFFSET: %0x007144A0
bool Blizzard::Mopaq::HSFILERecord::Read(void* buffer, int64_t offset, int32_t count, int32_t* amount) {
    MpqHandleRecord* handle = this->m_handle;

    if (!handle) {
        return false;
    }

    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_buffer = buffer;
    parms.m_verb = offsetof(Mpqsystem, m_read);
    parms.m_handle = handle;
    parms.m_offset = offset;
    parms.m_count = count;

    if (!handle->m_stack) {
        return false;
    }

    bool result = handle->m_stack->m_read(handle->m_stack, &parms);

    if (result && amount) {
        *amount = parms.m_count;
    }

    return result;
}

// OFFSET: %0x00714650
int64_t Blizzard::Mopaq::HSFILERecord::GetFileSize() {
    MpqHandleRecord* handle = this->m_handle;

    if (handle) {
        System_Mopaq::MpqParams parms;
        memset(&parms, 0, sizeof(parms));
        parms.m_verb = offsetof(Mpqsystem, m_getfilesize);
        parms.m_handle = handle;

        if (handle->m_stack && handle->m_stack->m_getfilesize(handle->m_stack, &parms)) {
            return parms.m_offset;
        }
    }

    return -1;
}

// OFFSET: %0x007145E0
bool Blizzard::Mopaq::HSFILERecord::SetFileSize(int64_t size) {
    MpqHandleRecord* handle = this->m_handle;

    if (!handle) {
        return true;
    }

    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Mpqsystem, m_setfilesize);
    parms.m_handle = handle;
    parms.m_offset = size;

    if (!handle->m_stack) {
        return false;
    }

    return handle->m_stack->m_setfilesize(handle->m_stack, &parms);
}

// OFFSET: %0x00714590
bool Blizzard::Mopaq::HSFILERecord::Flush() {
    MpqHandleRecord* handle = this->m_handle;

    if (!handle) {
        return true;
    }

    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Mpqsystem, m_flush);
    parms.m_handle = handle;

    if (!handle->m_stack) {
        return false;
    }

    return handle->m_stack->m_flush(handle->m_stack, &parms);
}

// OFFSET: %0x00714520
bool Blizzard::Mopaq::HSFILERecord::Write(const void* buffer, int64_t offset, int32_t count) {
    MpqHandleRecord* handle = this->m_handle;

    if (!handle) {
        return false;
    }

    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_buffer = const_cast<void*>(buffer);
    parms.m_verb = offsetof(Mpqsystem, m_write);
    parms.m_handle = handle;
    parms.m_count = count;
    parms.m_offset = offset;

    if (!handle->m_stack) {
        return false;
    }

    return handle->m_stack->m_write(handle->m_stack, &parms);
}

// OFFSET: %0x00714470
Blizzard::Mopaq::MpqErrorHandler::~MpqErrorHandler() {
}

// OFFSET: %0x00714470
Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::~MpqRepairOnDemandErrorHandler() {
}

// INLINED
Blizzard::Mopaq::MpqStandaloneRepairErrorHandler::MpqStandaloneRepairErrorHandler() {
}

// OFFSET: %0x007353E0
Blizzard::Mopaq::MpqStandaloneRepairErrorHandler::~MpqStandaloneRepairErrorHandler() {
}

// OFFSET: %0x0073A650
bool Blizzard::Mopaq::MpqStandaloneRepairErrorHandler::HandleDecompressSectorError(uint32_t sector) {
    this->m_badSectors.push_back(sector);
    return true;
}

// OFFSET: %0x00735370
bool Blizzard::Mopaq::MpqStandaloneRepairErrorHandler::GetBadSectorList(std::vector<uint32_t>& sectors) {
    sectors = this->m_badSectors;
    return true;
}

// PROVEN: 2d286e26 IDENTICAL
// OFFSET: 0x007C3810
bool Blizzard::Mopaq::MpqErrorHandler::HandleInitSectorTableError(MpqHandleRecord* handle, uint8_t* table, int64_t offset, uint32_t size) {
    return false;
}

// PROVEN: baa60b30 IDENTICAL
// OFFSET: 0x007AAB20
bool Blizzard::Mopaq::MpqErrorHandler::HandleReadChecksumError(MpqHandleRecord* handle) {
    return false;
}

// PROVEN: 486a3d46 IDENTICAL
// OFFSET: 0x007AAB20
bool Blizzard::Mopaq::MpqErrorHandler::HandleDecompressSectorError(uint32_t sector) {
    return false;
}

// PROVEN: 42bbaaa1 IDENTICAL
// OFFSET: 0x007C3810
bool Blizzard::Mopaq::MpqErrorHandler::HandleReadMPQFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t* count) {
    return false;
}

// PROVEN: 42aabd43 IDENTICAL
// OFFSET: 0x007AB6C0
bool Blizzard::Mopaq::MpqErrorHandler::HandleWriteFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset) {
    return false;
}

// PROVEN: 42815344 IDENTICAL
// OFFSET: 0x007AAB30
bool Blizzard::Mopaq::MpqErrorHandler::HandleMD5BlockError(MpqHandleRecord* handle, uint8_t* buffer, int64_t fileSize, uint32_t block, int32_t blockSize, uint32_t size, uint8_t* md5) {
    return false;
}

// PROVEN: d5cc93b1 IDENTICAL
// OFFSET: 0x007C3810
bool Blizzard::Mopaq::MpqErrorHandler::HandleNonStreamableFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t count) {
    return false;
}

// PROVEN: 6e17c5ff IDENTICAL
// OFFSET: 0x007AAB20
bool Blizzard::Mopaq::MpqErrorHandler::GetBadSectorList(std::vector<uint32_t>& sectors) {
    return false;
}

// OFFSET: %0x00728B40
System_Mopaq::IOHandler::IOHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, Blizzard::Mopaq::HSFILERecord& base, int64_t baseOffset) {
    this->m_handle = handle;
    this->m_base.m_handle = base.m_handle;

    if (this->m_base.m_handle) {
        this->m_base.m_handle->m_refCount.Increment();
    }

    this->m_baseOffset = baseOffset;
    this->m_handle->m_readFunc = IOHandler::ReadData;
    this->m_handle->m_writeFunc = IOHandler::WriteData;
}

// OFFSET: %0x0072DE80
System_Mopaq::IOHandler::~IOHandler() {
    if (this->m_base.m_handle && !this->m_base.m_handle->Release()) {
        this->m_base.m_handle = NULL;
    }
}

// PROVEN: 535c6124 IDENTICAL
// OFFSET: 0x007AB8E0
bool System_Mopaq::IOHandler::ReadData(IOHandler* handler, void* buffer, int64_t offset, int32_t* count) {
    return handler->ReadData(buffer, offset, count);
}

// OFFSET: %0x0070E6E0
bool System_Mopaq::IOHandler::InitializeAndRead(IOHandler* handler, void* buffer, int64_t offset, int32_t* count) {
    handler->m_handle->m_readFunc = IOHandler::ReadData;
    return handler->InitializeAndRead(buffer, offset, count);
}

// OFFSET: %0x0070E710
bool System_Mopaq::IOHandler::WriteData(IOHandler* handler, const void* buffer, int64_t offset, int32_t count) {
    return handler->WriteData(static_cast<const uint8_t*>(buffer), offset, count);
}

// OFFSET: %0x00715A80
bool System_Mopaq::IOHandler::ReadData(void* buffer, int64_t offset, int32_t* count) {
    return this->m_base.Read(buffer, offset + this->m_baseOffset, *count, count);
}

// PROVEN: 11f2fd1e IDENTICAL
// OFFSET: 0x007C7A30
bool System_Mopaq::IOHandler::InitializeAndRead(void* buffer, int64_t offset, int32_t* count) {
    return this->ReadData(buffer, offset, count);
}

// OFFSET: %0x00715AB0
bool System_Mopaq::IOHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    return this->m_base.Write(buffer, offset + this->m_baseOffset, count);
}

// PROVEN: 55714c49 IDENTICAL
// OFFSET: 0x00A3AA30
bool System_Mopaq::IOHandler::Flush() {
    return true;
}

// OFFSET: %0x00715AE0
int32_t System_Mopaq::IOHandler::TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset) {
    int64_t available = static_cast<int64_t>(entry.m_fileSize) - offset;
    int32_t amount = static_cast<int32_t>(available >= count ? count : available);

    return amount > 0 ? amount : 0;
}

// PROVEN: 0a5c3582 IDENTICAL
// OFFSET: 0x007C7A50
void System_Mopaq::IOHandler::UpdateBaseOffset() {
    this->m_baseOffset = this->m_handle->m_blockEntry.m_offset;
}

// PROVEN: 2660d618 IDENTICAL
// OFFSET: 0x00A3AA30
bool System_Mopaq::IOHandler::AdjustSectorTable() {
    return true;
}

// PROVEN: 719f64cf IDENTICAL
// OFFSET: 0x00841AE0
void System_Mopaq::IOHandler::SetSizeFileBeforeDelta(uint64_t size) {
}

// OFFSET: %0x0072DDB0
System_Mopaq::MopaqReadHandler::MopaqReadHandler(Blizzard::Mopaq::MpqHandleRecord*& handle) : IOHandler(handle, *handle->m_archive->m_file, handle->m_blockEntry.m_offset) {
}

// OFFSET: %0x0074EE20
bool System_Mopaq::MopaqReadHandler::ReadData(void* buffer, int64_t offset, int32_t* count) {
    if (this->PerformRead(buffer, offset, *count)) {
        return true;
    }

    Blizzard::String::QuickFormat<1024> message("%s - PerformRead error %lld %d", "System_Mopaq::MopaqReadHandler::ReadData", offset, *count);
    Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

    if (IsStreamedFile(this->m_handle->m_archive)) {
        return false;
    }

    return this->m_handle->m_errorHandler->HandleNonStreamableFileError(this->m_handle, static_cast<uint8_t*>(buffer), offset, *count);
}

// PROVEN: 698c968f IDENTICAL
// OFFSET: 0x007AB6C0
bool System_Mopaq::MopaqReadHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    return false;
}

// OFFSET: %0x007156D0
int32_t System_Mopaq::MopaqReadHandler::TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset) {
    int64_t available = static_cast<int64_t>(entry.m_compressedSize) - offset;
    int32_t amount = static_cast<int32_t>(available >= count ? count : available);

    return amount > 0 ? amount : 0;
}

// OFFSET: %0x00715650
bool System_Mopaq::MopaqReadHandler::PerformRead(void* buffer, int64_t offset, int32_t count) {
    return this->m_base.Read(buffer, offset + this->m_baseOffset, count, NULL);
}

// OFFSET: %0x0071B520
System_Mopaq::SectorReadHandler::SectorCache::SectorCache(uint32_t size) {
    this->m_data = NULL;
    this->m_sector = -1;
    this->m_data = static_cast<uint8_t*>(Blizzard::Memory::Allocate(size));
}

// OFFSET: %0x0074EED0
System_Mopaq::SectorReadHandler::SectorReadHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory) : MopaqReadHandler(handle) {
    this->m_sectorSize = handle->m_archive->m_sectorSize;
    this->m_key = 0;
    this->m_sectorTable = static_cast<uint32_t*>(sectorTable);
    this->m_checksums = NULL;
    this->m_firstSector = 0;
    this->m_lastSector = 0;
    this->m_sectorCount = 0;
    this->m_sectorCache = NULL;
    this->m_sectorTableState = sharedMemory ? 1 : 2;

    uint32_t flags = this->m_handle->m_blockEntry.m_flags;

    if (flags & 0x1000000) {
        this->m_sectorTableState = 2;
        this->m_sectorCount = 1;
        this->m_sectorTable[0] = 0;
        this->m_sectorTable[1] = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_compressedSize);
        this->m_sectorSize = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize);
    } else {
        this->m_handle->m_readFunc = IOHandler::InitializeAndRead;
    }

    if (flags & 0xF0000) {
        Blizzard::Mopaq::MpqHandleRecord* record = this->m_handle;
        this->m_transform = DecryptDecompress;

        uint32_t key = System_MPQEncrypt::Hash(Blizzard::String::FindFilename(handle->m_name), 3);

        if (record->m_blockEntry.m_flags & 0x20000) {
            key = static_cast<uint32_t>(record->m_blockEntry.m_fileSize) ^ (key + static_cast<uint32_t>(record->m_blockEntry.m_offset) - static_cast<uint32_t>(handle->m_archive->m_archiveOffset));
        }

        this->m_key = key;
    } else {
        this->m_transform = Decompress;
    }

    if ((flags & 0xFF00) == 0x200) {
        this->m_decompress = System_SComp::Decompress;
    } else {
        this->m_decompress = PkwareDecompressWrapper;
    }
}

// OFFSET: %0x0072DE10
System_Mopaq::SectorReadHandler::~SectorReadHandler() {
    delete[] this->m_checksums;

    if (this->m_sectorCache) {
        Blizzard::Memory::Free(this->m_sectorCache->m_data);
        delete this->m_sectorCache;
    }

    if (!this->m_sectorTableState) {
        delete[] this->m_sectorTable;
        this->m_sectorTable = NULL;
    }
}

// OFFSET: %0x00715730
void System_Mopaq::SectorReadHandler::AllocateSectorTable() {
    if (!this->m_sectorTableState) {
        delete[] this->m_sectorTable;
    }

    this->m_sectorTable = new uint32_t[this->m_sectorCount + 2];
    memset(this->m_sectorTable, 0, 4 * this->m_sectorCount + 8);
    this->m_sectorTableState = 0;
}

// OFFSET: %0x0074F180
bool System_Mopaq::SectorReadHandler::ValidateSectorTable() {
    uint32_t count = this->m_sectorCount;
    uint64_t compressedSize = this->m_handle->m_blockEntry.m_compressedSize;

    if (!count) {
        return true;
    }

    for (uint32_t i = 1; i <= count; i++) {
        if (this->m_sectorTable[i] <= this->m_sectorTable[i - 1] || this->m_sectorTable[i] > compressedSize) {
            System_SFile::BLOCKENTRY& entry = this->m_handle->m_blockEntry;
            Blizzard::String::QuickFormat<1024> message("%s - fail to validate %u %u %u entry: %lld %llu %llu %u %d", "System_Mopaq::SectorReadHandler::ValidateSectorTable", i, this->m_sectorTable[i], this->m_sectorTable[i - 1], entry.m_offset, entry.m_compressedSize, entry.m_fileSize, entry.m_flags, this->m_handle->m_blockIndex);
            Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);
            return false;
        }
    }

    return true;
}

// OFFSET: %0x0074EFE0
bool System_Mopaq::SectorReadHandler::InitializeChecksumTable() {
    uint32_t tableSize = 4 * this->m_sectorCount;

    if (this->m_sectorTable[0] - (tableSize + 4) < 4) {
        return true;
    }

    uint32_t start = this->m_sectorTable[tableSize / 4];
    uint32_t size = this->m_sectorTable[tableSize / 4 + 1] - start;
    uint32_t decompressedSize = tableSize;

    if (!size) {
        return true;
    }

    if (size > tableSize) {
        Blizzard::String::QuickFormat<1024> message("%s - fail to validate checksum %u %u", "System_Mopaq::SectorReadHandler::InitializeChecksumTable", size, tableSize);
        Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

        if (!this->m_handle->m_errorHandler->HandleReadChecksumError(this->m_handle)) {
            return false;
        }
    }

    this->m_checksums = new uint32_t[this->m_sectorCount];

    if (!this->PerformRead(this->m_checksums, start, size)) {
        delete[] this->m_checksums;
        this->m_checksums = NULL;

        Blizzard::String::QuickFormat<1024> message("%s - fail to read checksum %u %u", "System_Mopaq::SectorReadHandler::InitializeChecksumTable", start, size);
        Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

        if (!this->m_handle->m_errorHandler->HandleReadChecksumError(this->m_handle)) {
            return false;
        }
    }

    if (!System_SComp::Decompress(this->m_checksums, &decompressedSize, this->m_checksums, size) || tableSize != decompressedSize) {
        delete[] this->m_checksums;
        this->m_checksums = NULL;

        Blizzard::String::QuickFormat<1024> message("%s - fail to decompress checksum %u", "System_Mopaq::SectorReadHandler::InitializeChecksumTable", size);
        Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

        if (!this->m_handle->m_errorHandler->HandleReadChecksumError(this->m_handle)) {
            return false;
        }
    }

    if (this->m_handle->m_blockEntry.m_flags & 0xF0000) {
        this->m_transform = DecryptChecksumDecompress;
        return true;
    }

    this->m_transform = ChecksumDecompress;
    return true;
}

// OFFSET: %0x00754750
bool System_Mopaq::SectorReadHandler::InitializeSectorTable() {
    this->m_sectorCount = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);

    if (this->m_sectorTableState == 1) {
        this->AllocateSectorTable();
    }

    uint32_t size = 4 * this->m_sectorCount + 8;

    if (!this->PerformRead(this->m_sectorTable, 0, size)) {
        bool streamed = IsStreamedFile(this->m_handle->m_archive);

        Blizzard::String::QuickFormat<1024> message("%s - fail to read sector table of size:%u, streaming:%d", "System_Mopaq::SectorReadHandler::InitializeSectorTable", size, streamed);
        Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

        if (!streamed || !this->m_handle->m_errorHandler->HandleInitSectorTableError(this->m_handle, reinterpret_cast<uint8_t*>(this->m_sectorTable), 0, size)) {
            return false;
        }
    }

    if (this->m_handle->m_blockEntry.m_flags & 0xF0000) {
        System_MPQEncrypt::Decrypt(this->m_sectorTable, size, this->m_key - 1);
    }

    if (!this->ValidateSectorTable()) {
        return false;
    }

    return this->InitializeChecksumTable();
}

// OFFSET: %0x0074F280
bool System_Mopaq::SectorReadHandler::ProcessSectors(uint8_t* source, uint8_t* dest, const int32_t* range) {
    for (uint32_t sector = this->m_firstSector; sector < this->m_lastSector + 1; sector++) {
        uint32_t sourceSize = this->m_sectorTable[sector + 1] - this->m_sectorTable[sector];
        uint32_t destSize = this->m_sectorSize;

        if (sector == this->m_sectorCount - 1) {
            destSize = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize % this->m_sectorSize);

            if (!destSize) {
                destSize = this->m_sectorSize;
            }
        }

        if (!this->m_transform(this, source, dest, sourceSize, destSize, sector)) {
            System_SFile::BLOCKENTRY& entry = this->m_handle->m_blockEntry;
            Blizzard::String::QuickFormat<1024> message("%s - transform error %u, %u %u entry: %lld %llu %llu %u %d", "System_Mopaq::SectorReadHandler::ProcessSectors", sector, sourceSize, destSize, entry.m_offset, entry.m_compressedSize, entry.m_fileSize, entry.m_flags, this->m_handle->m_blockIndex);
            Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

            if (!this->m_handle->m_errorHandler->HandleDecompressSectorError(sector)) {
                return false;
            }
        }

        source += sourceSize;
        dest += destSize;
    }

    return true;
}

// OFFSET: %0x0074F3D0
bool System_Mopaq::SectorReadHandler::ReadAndDecompressData(uint8_t* dest, const int32_t* range) {
    uint32_t sectorSize = this->m_sectorSize;
    this->m_firstSector = range[0] / sectorSize;
    this->m_lastSector = (range[1] + range[0] - 1) / sectorSize;

    uint32_t start = this->m_sectorTable[this->m_firstSector];
    uint32_t size = this->m_sectorTable[this->m_lastSector + 1] - start;
    uint32_t total = sectorSize;

    if (this->m_lastSector == this->m_sectorCount - 1 && this->m_handle->m_blockEntry.m_fileSize % sectorSize) {
        total = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize % sectorSize);
    }

    uint32_t sectors = this->m_lastSector - this->m_firstSector;

    if (sectors) {
        total += sectorSize * sectors;
    }

    uint8_t* source = dest + total - size;

    if (this->PerformRead(source, start, size)) {
        return this->ProcessSectors(source, dest, range);
    }

    System_SFile::BLOCKENTRY& entry = this->m_handle->m_blockEntry;
    Blizzard::String::QuickFormat<1024> message("%s - PerformRead error %u %u %u %u entry: %lld %llu %llu %u %d", "System_Mopaq::SectorReadHandler::ReadAndDecompressData", range[0], start, size, total, entry.m_offset, entry.m_compressedSize, entry.m_fileSize, entry.m_flags, this->m_handle->m_blockIndex);
    Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);
    return false;
}

// OFFSET: %0x0074F4F0
bool System_Mopaq::SectorReadHandler::UnalignedRead(uint8_t* dest, const int32_t* range) {
    uint32_t sectorSize = this->m_sectorSize;
    uint32_t head = range[0] % sectorSize;
    uint32_t remaining = range[1];
    uint32_t end = range[0] + remaining;
    bool toEnd = (end == this->m_handle->m_blockEntry.m_fileSize) || !(end % sectorSize);

    if (!this->m_sectorCache) {
        this->m_sectorCache = new SectorCache(this->m_sectorSize);
        memset(this->m_sectorCache->m_data, 0, this->m_sectorSize);
    }

    uint32_t position = range[0] - head;
    int32_t read[2];
    read[0] = position;
    read[1] = remaining;

    bool result = true;

    if (head) {
        uint32_t size = this->m_sectorSize;
        int32_t sector = position / size;

        if (sector == static_cast<int32_t>(this->m_sectorCount - 1) && this->m_handle->m_blockEntry.m_fileSize % size) {
            size = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize % size);
        }

        read[1] = size;

        if (this->m_sectorCache->m_sector != sector) {
            this->m_sectorCache->m_sector = sector;
            result = this->ReadAndDecompressData(this->m_sectorCache->m_data, read);
        }

        uint32_t amount = static_cast<int32_t>(size - head) >= static_cast<int32_t>(remaining) ? remaining : size - head;
        memcpy(dest, this->m_sectorCache->m_data + head, amount);
        remaining -= amount;
        dest += amount;
        position += head + amount;
        read[0] = position;

        if (!result) {
            return result;
        }
    }

    if (static_cast<int32_t>(remaining) >= static_cast<int32_t>(this->m_sectorSize)) {
        uint32_t size = remaining;

        if (!toEnd) {
            size = this->m_sectorSize * (remaining / this->m_sectorSize);
        }

        read[1] = size;
        result = this->ReadAndDecompressData(dest, read);
        remaining -= size;
        dest += size;
        position += size;
        read[0] = position;
    }

    if (result && remaining) {
        uint32_t sectorSize = this->m_sectorSize;
        int32_t sector = position / sectorSize;

        if (sector != static_cast<int32_t>(this->m_sectorCount - 1) || !(read[1] = static_cast<int32_t>(this->m_handle->m_blockEntry.m_fileSize % sectorSize))) {
            read[1] = sectorSize;
        }

        if (this->m_sectorCache->m_sector != sector) {
            this->m_sectorCache->m_sector = sector;
            result = this->ReadAndDecompressData(this->m_sectorCache->m_data, read);
        }

        memcpy(dest, this->m_sectorCache->m_data, remaining);
        return result;
    }

    return result;
}

// OFFSET: %0x0074F6F0
bool System_Mopaq::SectorReadHandler::ReadData(void* buffer, int64_t offset, int32_t* count) {
    int32_t range[2];
    range[0] = static_cast<int32_t>(offset);
    range[1] = *count;

    uint32_t end = static_cast<uint32_t>(offset) + *count;
    bool result;

    if (static_cast<uint32_t>(offset) % this->m_sectorSize || (end != this->m_handle->m_blockEntry.m_fileSize && end % this->m_sectorSize)) {
        result = this->UnalignedRead(static_cast<uint8_t*>(buffer), range);
    } else {
        result = this->ReadAndDecompressData(static_cast<uint8_t*>(buffer), range);
    }

    if (result) {
        return true;
    }

    if (!IsStreamedFile(this->m_handle->m_archive)) {
        if (this->m_handle->m_errorHandler->HandleNonStreamableFileError(this->m_handle, static_cast<uint8_t*>(buffer), offset, *count)) {
            return true;
        }

        Blizzard::String::QuickFormat<1024> message("%s - fail to repair non-streamed file offset %lld amount %d", "System_Mopaq::SectorReadHandler::ReadData", offset, *count);
        Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
    }

    return false;
}

// OFFSET: %0x00754870
bool System_Mopaq::SectorReadHandler::InitializeAndRead(void* buffer, int64_t offset, int32_t* count) {
    if (this->InitializeSectorTable() && this->ReadData(buffer, offset, count)) {
        return true;
    }

    this->m_handle->m_readFunc = IOHandler::InitializeAndRead;

    Blizzard::String::QuickFormat<1024> message("%s - fail to repair offset %lld amount %d", "System_Mopaq::SectorReadHandler::InitializeAndRead", offset, *count);
    Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
    return false;
}

// OFFSET: %0x00715790
int32_t System_Mopaq::SectorReadHandler::TrimRead(int32_t count, System_SFile::BLOCKENTRY& entry, int64_t offset) {
    int64_t available = static_cast<int64_t>(entry.m_fileSize) - offset;
    int64_t amount = available >= count ? count : available;

    return amount > 0 ? static_cast<int32_t>(amount) : 0;
}

// OFFSET: %0x0070E4B0
bool System_Mopaq::SectorReadHandler::DecryptChecksumDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector) {
    System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(source), sourceSize & ~3u, sector + handler->m_key);

    uint32_t expected = handler->m_checksums[sector];

    if (expected) {
        uint32_t checksum = adler32(0, source, sourceSize);

        if (!checksum) {
            checksum = 0xFFFFFFFF;
        }

        if (checksum != expected) {
            return false;
        }
    }

    uint32_t size = destSize;
    return handler->m_decompress(dest, &size, source, sourceSize) && size == destSize;
}

// OFFSET: %0x0070E530
bool System_Mopaq::SectorReadHandler::DecryptDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector) {
    System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(source), sourceSize & ~3u, sector + handler->m_key);

    uint32_t size = destSize;
    return handler->m_decompress(dest, &size, source, sourceSize) && size == destSize;
}

// OFFSET: %0x0070E590
bool System_Mopaq::SectorReadHandler::ChecksumDecompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector) {
    uint32_t expected = handler->m_checksums[sector];

    if (expected) {
        uint32_t checksum = adler32(0, source, sourceSize);

        if (!checksum) {
            checksum = 0xFFFFFFFF;
        }

        if (checksum != expected) {
            return false;
        }
    }

    uint32_t size = destSize;
    return handler->m_decompress(dest, &size, source, sourceSize) && size == destSize;
}

// OFFSET: %0x0070E600
bool System_Mopaq::SectorReadHandler::Decompress(SectorReadHandler* handler, uint8_t* source, uint8_t* dest, uint32_t sourceSize, uint32_t destSize, uint32_t sector) {
    uint32_t size = destSize;
    return handler->m_decompress(dest, &size, source, sourceSize) && size == destSize;
}

// OFFSET: %0x0070E640
bool System_Mopaq::SectorReadHandler::PkwareDecompressWrapper(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize) {
    if (sourceSize != *destSize) {
        return System_SComp::PkwareDecompress(dest, destSize, source, sourceSize) != 0;
    }

    if (dest != source) {
        memcpy(dest, source, sourceSize);
    }

    return true;
}

// OFFSET: %0x00726D50
void Blizzard::Mopaq::HSFILERecord::Assign(const HSFILERecord& other) {
    MpqHandleRecord* handle = other.m_handle;
    MpqHandleRecord* old = this->m_handle;
    this->m_handle = handle;

    if (handle) {
        handle->m_refCount.Increment();
    }

    if (old) {
        old->Release();
    }
}
