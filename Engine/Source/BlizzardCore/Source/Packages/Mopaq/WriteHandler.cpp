#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_MD5Verify.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SComp.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "ThirdParty/zlib/zlib.h"

// INLINED
static void ReserveBlockOffset(System_Mopaq::IOHandler* handler) {
    Blizzard::Mopaq::MpqHandleRecord* handle = handler->m_handle;
    handle->m_blockEntry = handle->m_archive->GetBlockEntry(handle->m_blockIndex);

    handle = handler->m_handle;
    uint64_t size = handle->m_blockEntry.m_compressedSize ? handle->m_blockEntry.m_compressedSize : handle->m_blockEntry.m_fileSize;
    handler->m_baseOffset = handle->m_archive->RequestWriteReservation(size, false);
    handler->m_handle->m_blockEntry.m_offset = handler->m_baseOffset;

    handler->m_base.SetVerifyBaseOffset(handler->m_handle->m_blockEntry.m_offset);
    handler->StoreBlockEntry();
}

// OFFSET: 0x007BAFA0
void System_Mopaq::IOHandler::StoreBlockEntry() {
    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;

    if (handle->m_blockIndex != 0xFFFFFFFE && handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
    }
}

// OFFSET: 0x007D04B0
System_Mopaq::MopaqReadWriteHandler::MopaqReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle) : MopaqReadHandler(handle) {
    this->m_written = 0;
    this->m_sizeBeforeDelta = 0;
}

// OFFSET: 0x007BFC10
void System_Mopaq::MopaqReadWriteHandler::UpdateBlockEntry() {
    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;
    handle->m_blockEntry.m_compressedSize = handle->m_blockEntry.m_fileSize;
    this->m_handle->m_blockEntry.m_flags |= 0x80000000;

    handle = this->m_handle;

    if (handle->m_blockIndex != 0xFFFFFFFE && handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
    }
}

// OFFSET: 0x007C3430/0x1404BDAA0
bool System_Mopaq::MopaqReadWriteHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    if (offset != this->m_written) {
        return false;
    }

    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;
    uint64_t limit = handle->m_blockEntry.m_compressedSize < handle->m_blockEntry.m_fileSize ? handle->m_blockEntry.m_fileSize : handle->m_blockEntry.m_compressedSize;

    if (static_cast<uint64_t>(offset + count) > limit) {
        return false;
    }

    if (this->m_baseOffset < 0 && this->m_handle->m_blockIndex != 0xFFFFFFFE) {
        ReserveBlockOffset(this);
    }

    bool result = this->m_base.Write(buffer, this->m_baseOffset + offset, count);
    this->m_written += count;

    if (this->m_handle->m_blockEntry.m_fileSize == this->m_written) {
        this->UpdateBlockEntry();
    }

    return result;
}

// OFFSET: 0x007BFC50/0x1404B8FD0
bool System_Mopaq::MopaqReadWriteHandler::Flush() {
    if (!this->m_written) {
        Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;
        handle->m_blockEntry.m_offset = 0;
        handle->m_blockEntry.m_compressedSize = handle->m_blockEntry.m_fileSize;
        this->m_handle->m_blockEntry.m_flags |= 0x80000000;

        handle = this->m_handle;

        if (handle->m_blockIndex != 0xFFFFFFFE && handle->m_archive) {
            handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
        }
    }

    uint32_t written = this->m_written;

    if (!this->m_sizeBeforeDelta || written != this->m_sizeBeforeDelta) {
        Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;

        if (written != handle->m_blockEntry.m_fileSize) {
            if (handle->m_archive) {
                handle->m_archive->ReturnWriteReservation(this->m_baseOffset, handle->m_blockEntry.m_fileSize, written, false);
            }

            handle = this->m_handle;

            if (written != handle->m_blockEntry.m_compressedSize) {
                handle->m_blockEntry.m_fileSize = written;
                this->UpdateBlockEntry();
            }
        }
    }

    return true;
}

// OFFSET: 0x007AB8A0/0x1404A19C0
void System_Mopaq::MopaqReadWriteHandler::SetSizeFileBeforeDelta(uint64_t size) {
    this->m_sizeBeforeDelta = size;
}

// OFFSET: 0x007D04F0
System_Mopaq::MopaqInPlaceReadWriteHandler::MopaqInPlaceReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle) : MopaqReadWriteHandler(handle) {
}

// OFFSET: 0x007BFD20/0x1404B90C0
bool System_Mopaq::MopaqInPlaceReadWriteHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    if (offset) {
        return false;
    }

    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;

    if (static_cast<uint64_t>(static_cast<int64_t>(count)) > handle->m_blockEntry.m_fileSize) {
        return false;
    }

    handle->m_blockEntry.m_compressedSize = static_cast<int64_t>(count);
    handle->m_blockEntry.m_fileSize = handle->m_blockEntry.m_compressedSize;

    if (this->m_baseOffset < 0) {
        ReserveBlockOffset(this);
    }

    bool result = this->m_base.Write(buffer, this->m_baseOffset, count);
    this->m_writtenInPlace += count;

    if (static_cast<uint32_t>(this->m_writtenInPlace) == this->m_handle->m_blockEntry.m_fileSize) {
        this->UpdateBlockEntry();
    }

    return result;
}

// OFFSET: 0x00A3AA30/0x1404BDD20
bool System_Mopaq::MopaqInPlaceReadWriteHandler::Flush() {
    return true;
}

// OFFSET: 0x007E8E60
System_Mopaq::SectorReadWriteHandler::SectorReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory) : SectorReadHandler(handle, sectorTable, sharedMemory) {
    this->m_written = 0;
    this->m_sizeBeforeDelta = 0;
    this->m_reserved = false;
}

// OFFSET: [0x007CCA00, 0x007D0730]
System_Mopaq::SectorReadWriteHandler::~SectorReadWriteHandler() {
}

// OFFSET: 0x007B34F0
void System_Mopaq::SectorReadWriteHandler::MarkSingleUnit() {
    this->m_sectorTable[0] = 0;
    this->m_handle->m_blockEntry.m_flags |= 0x1000000;
    this->m_handle->m_blockEntry.m_flags &= ~0x4000000u;
    this->m_handle->m_readFunc = IOHandler::ReadData;
}

// OFFSET: 0x007B3520/0x1404A8D90
void System_Mopaq::SectorReadWriteHandler::ResetSectorCount() {
    uint32_t count = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);
    this->m_sectorCount = count;

    if (count <= 1) {
        this->MarkSingleUnit();
    } else {
        this->m_sectorTable[0] = 4 * count + 8;
    }
}

// OFFSET: 0x007E8ED0/0x1404F37F0
bool System_Mopaq::SectorReadWriteHandler::PrepareSector(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t* destSize, uint32_t sector) {
    bool checksum = false;
    const Blizzard::Mopaq::CompressionAlgorithm* algorithms = this->m_compression.size() ? &this->m_compression[0] : NULL;
    bool result = System_SComp::Compress(dest, destSize, &checksum, source, sourceSize, static_cast<uint32_t>(this->m_compression.size()), algorithms);

    if (checksum && this->m_sectorCount > 1) {
        if (!this->m_checksums) {
            this->m_checksums = new uint32_t[this->m_sectorCount];
            memset(this->m_checksums, 0, 4 * this->m_sectorCount);
        }

        uint32_t adler = adler32(0, dest, *destSize);

        if (!adler) {
            adler = 0xFFFFFFFF;
        }

        this->m_checksums[sector] = adler;
    }

    return result;
}

// OFFSET: 0x007BAFC0/0x1404B2850
void System_Mopaq::SectorReadWriteHandler::UpdateBlockHashTable() {
    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;
    handle->m_blockEntry.m_compressedSize = this->m_sectorTable[static_cast<uint32_t>((handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize) + 2];
    this->m_handle->m_blockEntry.m_flags |= 0x80000000;

    handle = this->m_handle;

    if (handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);

        if (this->m_reserved) {
            this->m_handle->m_archive->CompleteWriteReservation(this->m_handle->m_blockEntry.m_compressedSize, this->m_handle->m_blockEntry.m_fileSize);
        }
    }
}

// OFFSET: 0x007BB0A0/0x1404B2930
bool System_Mopaq::SectorReadWriteHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    if (offset != this->m_written) {
        return false;
    }

    uint32_t start = static_cast<uint32_t>(offset);
    uint64_t end = static_cast<uint64_t>(start) + count;

    if (end > this->m_handle->m_blockEntry.m_fileSize) {
        return false;
    }

    if (!start) {
        this->ResetSectorCount();
    }

    uint32_t sectorSize = this->m_sectorSize;
    uint32_t head = start % sectorSize;
    int32_t remaining = count;
    uint32_t sector = start / sectorSize;
    bool atEnd = end == this->m_handle->m_blockEntry.m_fileSize;
    uint32_t done = 0;
    bool result = true;

    if (head) {
        if (head + count < sectorSize && !atEnd) {
            Blizzard::String::MemCopy(this->m_sectorCache->m_data + head, buffer, count);
            this->m_written += remaining;
            return true;
        }

        done = sectorSize - head >= static_cast<uint32_t>(count) ? count : sectorSize - head;
        remaining = count - done;
        Blizzard::String::MemCopy(this->m_sectorCache->m_data + head, buffer, done);
        result = this->CompressAndWrite(this->m_sectorCache->m_data, sector++, 1);

        if (!result) {
            this->m_written += count;
            return result;
        }
    }

    if (remaining > 0) {
        sectorSize = this->m_sectorSize;

        if (remaining >= static_cast<int32_t>(sectorSize) || atEnd) {
            uint32_t sectors = remaining / sectorSize;

            if (atEnd && remaining % sectorSize) {
                sectors++;
            }

            result = this->CompressAndWrite(buffer + done, sector, sectors);
            remaining -= sectors * this->m_sectorSize;
            done += sectors * this->m_sectorSize;
        }
    }

    if (result && remaining > 0) {
        if (!this->m_sectorCache) {
            this->m_sectorCache = new SectorCache(this->m_sectorSize);
        }

        Blizzard::String::MemCopy(this->m_sectorCache->m_data, buffer + done, remaining);
    }

    this->m_written += count;

    if (result && atEnd) {
        result = this->FinalizeWrite();
    }

    return result;
}

// OFFSET: 0x007B35C0/0x1404A8E40
bool System_Mopaq::SectorReadWriteHandler::Flush() {
    uint32_t written = this->m_written;

    if (!written) {
        this->m_handle->m_blockEntry.m_offset = 0;
        return this->FinalizeWrite();
    }

    if (this->m_sizeBeforeDelta ? written == this->m_sizeBeforeDelta : written == this->m_handle->m_blockEntry.m_fileSize) {
        return true;
    }

    this->m_handle->m_blockEntry.m_fileSize = written;

    if (!this->m_sectorCache) {
        return this->FinalizeWrite();
    }

    if (!(this->m_written % this->m_sectorSize)) {
        return this->FinalizeWrite();
    }

    uint32_t count = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);
    this->m_sectorCount = count;

    if (count == 1) {
        this->MarkSingleUnit();
    }

    if (!this->CompressAndWrite(this->m_sectorCache->m_data, this->m_written / this->m_sectorSize, 1)) {
        return false;
    }

    return this->FinalizeWrite();
}

// OFFSET: 0x007B3580/0x1404A8DF0
bool System_Mopaq::SectorReadWriteHandler::AdjustSectorTable() {
    if (this->m_written) {
        return false;
    }

    uint32_t count = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);

    if (this->m_sectorCount < count) {
        this->m_sectorCount = count;
        this->AllocateSectorTable();
    }

    return true;
}

// OFFSET: 0x007AB8C0/0x14090D710
void System_Mopaq::SectorReadWriteHandler::SetSizeFileBeforeDelta(uint64_t size) {
    this->m_sizeBeforeDelta = size;
}

// OFFSET: 0x007E8FA0/0x1404F3910
bool System_Mopaq::SectorReadWriteHandler::WriteChecksumTable() {
    uint32_t size = 0;
    uint32_t count = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);

    if (this->m_checksums && this->m_sectorCount > 1) {
        uint32_t compressedSize = 4 * count;
        bool checksum;
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 0x20;
        algorithm.m_level = 0;

        if (!System_SComp::Compress(this->m_checksums, &compressedSize, &checksum, this->m_checksums, 4 * count, 1, &algorithm)) {
            return false;
        }

        if (!this->PerformWrite(reinterpret_cast<const uint8_t*>(this->m_checksums), this->m_sectorTable[count], compressedSize)) {
            return false;
        }

        size = compressedSize;
    }

    this->m_sectorTable[count + 1] = this->m_sectorTable[count] + size;
    return true;
}

// OFFSET: 0x007BB040/0x1404B28D0
bool System_Mopaq::SectorReadWriteHandler::FinalizeWrite() {
    bool result = this->WriteChecksumTable();

    if (result) {
        if (this->m_sectorCount > 1 && !this->PerformWrite(reinterpret_cast<const uint8_t*>(this->m_sectorTable), 0, 4 * this->m_sectorCount + 8)) {
            this->UpdateBlockHashTable();
            return false;
        }

        result = true;
    }

    this->UpdateBlockHashTable();
    return result;
}

// OFFSET: 0x007B3460/0x1404A8CE0
bool System_Mopaq::SectorReadWriteHandler::PerformWrite(const uint8_t* buffer, int64_t offset, int32_t count) {
    if (this->m_baseOffset < 0) {
        System_SFile::ARCHIVEREC* archive = this->m_handle->m_archive;
        Blizzard::Lock::MutexEnter(archive->m_writeLock.m_mutex);

        this->m_baseOffset = archive->m_writeReservationEnd;
        this->m_handle->m_blockEntry.m_offset = this->m_baseOffset;

        if (this->m_base.m_handle && this->m_base.m_handle->m_md5Verify) {
            this->m_base.m_handle->m_md5Verify->m_baseOffset = this->m_handle->m_blockEntry.m_offset;
        }

        this->m_reserved = true;
    }

    return this->m_base.Write(buffer, this->m_baseOffset + offset, count);
}

// OFFSET: 0x007E9050/0x1404F39D0
bool System_Mopaq::SectorReadWriteHandler::CompressAndWrite(const uint8_t* buffer, uint32_t sector, uint32_t count) {
    uint32_t first = sector;
    Blizzard::Util::Buffer<4096, uint8_t> output(0);
    output.Resize(count * this->m_sectorSize);

    uint32_t outputSize = 0;
    uint32_t position = 0;
    uint32_t sectorSize = this->m_sectorSize;

    while (position < count * sectorSize) {
        uint32_t available = output.m_count - outputSize;

        if (sector == this->m_sectorCount - 1 && this->m_handle->m_blockEntry.m_fileSize % sectorSize) {
            sectorSize = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize % sectorSize);
        }

        if (!this->PrepareSector(buffer + position, sectorSize, output.m_data + outputSize, &available, sector)) {
            return false;
        }

        outputSize += available;
        sector++;
        this->m_sectorTable[sector] = this->m_sectorTable[sector - 1] + available;
        sectorSize = this->m_sectorSize;
        position += sectorSize;
    }

    return this->PerformWrite(output.m_data, this->m_sectorTable[first], outputSize);
}

// OFFSET: 0x007EAA80
System_Mopaq::SectorInPlaceReadWriteHandler::SectorInPlaceReadWriteHandler(Blizzard::Mopaq::MpqHandleRecord*& handle, void* sectorTable, bool sharedMemory) : SectorReadWriteHandler(handle, sectorTable, sharedMemory) {
}

// OFFSET: 0x007EA940
System_Mopaq::SectorInPlaceReadWriteHandler::~SectorInPlaceReadWriteHandler() {
}

// OFFSET: 0x007B3680/0x1404A8F30
bool System_Mopaq::SectorInPlaceReadWriteHandler::WriteData(const uint8_t* buffer, int64_t offset, int32_t count) {
    if (offset) {
        return false;
    }

    if (static_cast<uint64_t>(static_cast<int64_t>(count)) > this->m_handle->m_blockEntry.m_fileSize) {
        return false;
    }

    this->m_handle->m_blockEntry.m_fileSize = static_cast<int64_t>(count);
    this->ResetSectorCount();

    uint32_t sectors = count / this->m_sectorSize;

    if (count % this->m_sectorSize) {
        sectors++;
    }

    bool result = this->CompressAndWrite(buffer, 0, sectors);

    if (result) {
        this->m_written += count;
        return this->FinalizeWrite();
    }

    return result;
}

// OFFSET: 0x007E9310/0x1404F3CD0
bool System_Mopaq::SectorInPlaceReadWriteHandler::WriteChecksumTable() {
    uint32_t size = 0;
    uint32_t count = static_cast<uint32_t>((this->m_handle->m_blockEntry.m_fileSize - 1) / this->m_sectorSize + 1);

    if (this->m_checksums && this->m_sectorCount > 1) {
        uint32_t compressedSize = 4 * count;
        bool checksum;
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 0x20;
        algorithm.m_level = 0;

        if (!System_SComp::Compress(this->m_checksums, &compressedSize, &checksum, this->m_checksums, 4 * count, 1, &algorithm)) {
            return false;
        }

        uint32_t start = this->m_sectorTable[count];
        size = compressedSize;

        if (static_cast<uint64_t>(start + compressedSize) >= this->m_handle->m_blockEntry.m_compressedSize || !this->PerformWrite(reinterpret_cast<const uint8_t*>(this->m_checksums), start, compressedSize)) {
            return false;
        }
    }

    this->m_sectorTable[count + 1] = this->m_sectorTable[count] + size;
    return true;
}

// OFFSET: 0x007BB270/0x1404B2B40
bool System_Mopaq::SectorInPlaceReadWriteHandler::FinalizeWrite() {
    bool result = this->WriteChecksumTable();

    if (result) {
        if (this->m_sectorCount <= 1 || (result = this->PerformWrite(reinterpret_cast<const uint8_t*>(this->m_sectorTable), 0, 4 * this->m_sectorCount + 8)) != false) {
            this->UpdateBlockHashTable();
            return true;
        }
    }

    return result;
}

// OFFSET: 0x007E91A0/0x1404F3B40
bool System_Mopaq::SectorInPlaceReadWriteHandler::CompressAndWrite(const uint8_t* buffer, uint32_t sector, uint32_t count) {
    uint32_t first = sector;
    bool result = true;
    Blizzard::Util::Buffer<4096, uint8_t> output(0);
    output.Resize(count * this->m_sectorSize);

    uint32_t outputSize = 0;
    uint32_t position = 0;
    uint32_t sectorSize = this->m_sectorSize;

    while (position < count * sectorSize) {
        uint32_t available = output.m_count - outputSize;

        if (sector == this->m_sectorCount - 1 && this->m_handle->m_blockEntry.m_fileSize % sectorSize) {
            sectorSize = static_cast<uint32_t>(this->m_handle->m_blockEntry.m_fileSize % sectorSize);
        }

        result = this->PrepareSector(buffer + position, sectorSize, output.m_data + outputSize, &available, sector);

        if (!result) {
            break;
        }

        outputSize += available;
        sector++;
        this->m_sectorTable[sector] = this->m_sectorTable[sector - 1] + available;
        sectorSize = this->m_sectorSize;
        position += sectorSize;
    }

    if (this->m_sectorTable[sector] > this->m_handle->m_blockEntry.m_compressedSize) {
        return false;
    }

    if (!result) {
        return result;
    }

    return this->PerformWrite(output.m_data, this->m_sectorTable[first], outputSize);
}
