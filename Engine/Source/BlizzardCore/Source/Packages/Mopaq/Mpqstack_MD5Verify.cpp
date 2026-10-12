#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_MD5Verify.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// INLINED
static void ComputeMD5(Blizzard::Util::Digest<16>* digest, const uint8_t* buffer, uint32_t size) {
    Blizzard::Util::MD5 md5;
    md5.Process(buffer, size);
    md5.Finish(digest->m_data);
}

// OFFSET: 0x007BAAF0
System_Mopaq::MD5VerifyData::MD5VerifyData(int32_t blockSize, int64_t fileSize, bool writing) {
    this->m_fileSize = fileSize;
    this->m_blockCount = (static_cast<int32_t>(fileSize) + blockSize - 1) / blockSize;
    this->m_blockSize = blockSize;
    this->m_baseOffset = 0;
    this->m_writing = writing;
    this->m_written = 0;

    if (writing) {
        uint32_t size = 16 * this->m_blockCount;

        if (size > this->m_md5s.m_count) {
            this->m_md5s.SetCount(size);
        }
    }
}

// OFFSET: 0x007BFB50
void System_Mopaq::CreateMD5VerifyData(Blizzard::Mopaq::MpqHandleRecord* handle, int32_t blockSize, int64_t fileSize, bool writing) {
    void* memory = Blizzard::Memory::Allocate(sizeof(MD5VerifyData));
    handle->m_md5Verify = static_cast<MD5VerifyData*>(memory);

    if (memory) {
        new (memory) MD5VerifyData(blockSize, fileSize, writing);
    }
}

// OFFSET: 0x007C3390/0x1404BD9A0
void System_Mopaq::DestroyMD5VerifyData(Blizzard::Mopaq::MpqHandleRecord* handle) {
    MD5VerifyData* data = handle->m_md5Verify;

    if (data->m_workBuffer.m_data) {
        delete[] data->m_workBuffer.m_data;
    }

    if (data->m_md5s.m_data) {
        delete[] data->m_md5s.m_data;
    }

    Blizzard::Memory::Free(handle->m_md5Verify);
    handle->m_md5Verify = NULL;

    if (handle->m_handler) {
        delete handle->m_handler;
    }

    if (handle->m_errorHandler) {
        delete handle->m_errorHandler;
    }

    handle->m_handler = NULL;
}

// OFFSET: 0x007B30F0
bool System_Mopaq::MD5VerifyData::WriteMD5Table(Blizzard::Mopaq::MpqHandleRecord* handle) {
    if (!this->m_md5s.m_count || !this->m_writing) {
        return true;
    }

    return handle->m_writeFunc(handle->m_handler, this->m_md5s.m_data, this->m_written, static_cast<int32_t>(16 * ((this->m_blockSize + this->m_written - 1) / this->m_blockSize)));
}

// OFFSET: 0x007BAE30/0x1404B2720
System_Mopaq::MD5VerifyData::IoOperationType System_Mopaq::MD5VerifyData::PrepareWorkBuffer(int64_t offset) {
    if (offset > this->m_fileSize || offset < 0) {
        return IO_INVALID;
    }

    if (!offset && this->m_size == this->m_fileSize) {
        return IO_ALIGNED;
    }

    int64_t blockSize = this->m_blockSize;
    this->m_lastBlock = false;
    this->m_unalignedStart = offset % blockSize > 0;
    this->m_unalignedEnd = false;

    int64_t end = offset + this->m_size;

    if (this->m_fileSize % blockSize && end > this->m_blockSize * (this->m_blockCount - 1)) {
        this->m_lastBlock = true;

        if (end != this->m_fileSize) {
            this->m_unalignedEnd = true;
        }
    } else if (end % blockSize) {
        this->m_unalignedEnd = true;
    }

    if (static_cast<uint32_t>(this->m_blockSize) > this->m_workBuffer.m_count) {
        this->m_workBuffer.SetCount(this->m_blockSize);
    }

    if (offset - offset % blockSize == offset + this->m_size - 1 - (offset + this->m_size - 1) % blockSize) {
        return IO_SINGLE_BLOCK;
    }

    return IO_MULTIPLE_BLOCKS;
}

// PROVEN: 08c4682a IDENTICAL/NONE
// OFFSET: 0x007AB5F0
bool System_Mopaq::MD5VerifyData::ReadAligned(int64_t offset) {
    int32_t count = this->m_size;
    return this->m_handle->m_readFunc(this->m_handle->m_handler, this->m_buffer, offset, &count) && count == this->m_size;
}

// OFFSET: 0x007B3150/0x1404A87A0
bool System_Mopaq::MD5VerifyData::ReadSingleBlock(int64_t offset) {
    int32_t size = this->m_blockSize;

    if (this->m_lastBlock) {
        size = static_cast<int32_t>(this->m_fileSize % this->m_blockSize);
    }

    int32_t count = size;
    return this->m_handle->m_readFunc(this->m_handle->m_handler, this->m_workBuffer.m_data, offset - offset % this->m_blockSize, &count) && count == size;
}

// OFFSET: 0x007BAC70/0x1404B2540
bool System_Mopaq::MD5VerifyData::ReadMultipleBlocks(int64_t offset) {
    int32_t head = 0;
    bool result = true;

    if (this->m_unalignedStart) {
        head = static_cast<int32_t>(offset % this->m_blockSize);
        int32_t expected = head;

        if (!this->m_handle->m_readFunc(this->m_handle->m_handler, this->m_workBuffer.m_data, offset - head, &head) || head != expected) {
            result = false;
        }
    }

    if (result) {
        bool read = this->ReadAligned(offset);

        if (this->m_unalignedStart) {
            memmove(this->m_workBuffer.m_data + head, this->m_buffer, this->m_blockSize - head);
        }

        result = read;
    }

    if (this->m_unalignedEnd && result) {
        uint8_t* tailBuffer;

        if (this->m_unalignedStart) {
            if (static_cast<uint32_t>(2 * this->m_blockSize) > this->m_workBuffer.m_count) {
                this->m_workBuffer.SetCount(2 * this->m_blockSize);
            }

            tailBuffer = this->m_workBuffer.m_data + this->m_blockSize;
        } else {
            tailBuffer = this->m_workBuffer.m_data;
        }

        int64_t end = offset + this->m_size;
        uint32_t tail = static_cast<uint32_t>(end % this->m_blockSize);
        int32_t count;

        if (this->m_lastBlock) {
            count = static_cast<int32_t>(this->m_fileSize % this->m_blockSize) - tail;
        } else {
            count = this->m_blockSize - tail;
        }

        int32_t expected = count;
        bool read = this->m_handle->m_readFunc(this->m_handle->m_handler, tailBuffer + tail, end, &count) && count == expected;
        memmove(tailBuffer, this->m_buffer + this->m_size - tail, tail);
        return read;
    }

    return result;
}

// OFFSET: 0x007BAC00/0x1404B24C0
bool System_Mopaq::MD5VerifyData::InitializeMD5Buffer() {
    if (this->m_md5s.m_count) {
        return true;
    }

    if (16 * this->m_blockCount) {
        this->m_md5s.SetCount(16 * this->m_blockCount);
    }

    int32_t count = this->m_md5s.m_count;
    int32_t expected = count;
    return this->m_handle->m_readFunc(this->m_handle->m_handler, this->m_md5s.m_data, this->m_fileSize, &count) && count == expected;
}

// OFFSET: 0x007E7C10/0x1404F1E10
bool System_Mopaq::MD5VerifyData::ValidateBlock(uint8_t* buffer, int32_t block, int32_t size, bool& repaired) {
    repaired = false;

    Blizzard::Util::Digest<16> expected;
    memcpy(expected.m_data, this->m_md5s.m_data + 16 * block, 16);

    if (this->m_writing && expected == System_SFile::s_emptyMd5) {
        return true;
    }

    Blizzard::Util::Digest<16> md5;
    ComputeMD5(&md5, buffer, size);

    bool result = expected == md5;

    if (!result) {
        for (int32_t tries = 0; this->m_handle->m_errorHandler && tries < 3; tries++) {
            Blizzard::Mopaq::MpqHandleRecord* handle = this->m_handle;
            result = handle->m_errorHandler->HandleMD5BlockError(handle, buffer, this->m_fileSize, block, this->m_blockSize, size, this->m_md5s.m_data + 16 * block);

            if (result) {
                memcpy(expected.m_data, this->m_md5s.m_data + 16 * block, 16);
                ComputeMD5(&md5, buffer, size);
                result = expected == md5;
                repaired = result;
            }

            Blizzard::String::QuickFormat<1024> message("%s - ValidateBlock fail %d %d %d %d", "System_Mopaq::MD5VerifyData::ValidateBlock", block, size, this->m_blockSize, tries);
            Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

            if (result) {
                break;
            }
        }
    }

    return result;
}

// OFFSET: 0x007E7E70/0x1404F24F0
bool System_Mopaq::MD5VerifyData::ValidateBlocks(uint8_t* buffer, int64_t offset, int64_t size) {
    if (size <= 0) {
        return false;
    }

    int32_t first = static_cast<int32_t>(offset / this->m_blockSize);
    int32_t count = static_cast<int32_t>(size / this->m_blockSize);
    bool repaired;
    bool result = true;

    for (int32_t i = 0; i < count; i++) {
        result = this->ValidateBlock(buffer + i * this->m_blockSize, i + first, this->m_blockSize, repaired);

        if (!result) {
            break;
        }
    }

    return result;
}

// OFFSET: 0x007E7EF0
bool System_Mopaq::MD5VerifyData::ValidateLastBlock(uint8_t* buffer, bool& repaired) {
    repaired = false;

    return this->ValidateBlock(buffer, this->m_blockCount - 1, static_cast<int32_t>(this->m_fileSize % this->m_blockSize), repaired);
}

// OFFSET: 0x007E7F30/0x1404F2580
bool System_Mopaq::MD5VerifyData::ValidateAlignedRead() {
    int32_t size = this->m_size;
    int32_t remainder = size % this->m_blockSize;
    int64_t full = size - remainder;
    bool result = true;

    if (full <= 0 || (result = this->ValidateBlocks(this->m_buffer, 0, full)) != false) {
        if (remainder) {
            bool repaired;
            return this->ValidateBlock(this->m_buffer + full, this->m_blockCount - 1, static_cast<int32_t>(this->m_fileSize % this->m_blockSize), repaired);
        }
    }

    return result;
}

// OFFSET: 0x007E8190/0x1404F2840
bool System_Mopaq::MD5VerifyData::ValidateSingleBlock(int64_t offset) {
    bool repaired;
    bool result;

    if (this->m_lastBlock) {
        result = this->ValidateBlock(this->m_workBuffer.m_data, this->m_blockCount - 1, static_cast<int32_t>(this->m_fileSize % this->m_blockSize), repaired);
    } else {
        result = this->ValidateBlock(this->m_workBuffer.m_data, static_cast<int32_t>(offset / this->m_blockSize), this->m_blockSize, repaired);
    }

    if (result) {
        memmove(this->m_buffer, this->m_workBuffer.m_data + offset % this->m_blockSize, this->m_size);
    }

    return result;
}

// OFFSET: 0x007E7FB0/0x1404F2610
bool System_Mopaq::MD5VerifyData::ValidateMultipleBlocks(int64_t offset) {
    bool repaired = false;
    int32_t headBlock = 0;
    int32_t headSize = 0;
    bool result;

    if (this->m_unalignedStart) {
        int32_t blockSize = this->m_blockSize;
        headBlock = static_cast<int32_t>(offset / blockSize);
        result = this->ValidateBlock(this->m_workBuffer.m_data, headBlock, blockSize, repaired);
        headSize = this->m_blockSize;

        if (!result) {
            return result;
        }
    }

    int32_t skip = 0;

    if (this->m_unalignedStart) {
        skip = static_cast<int32_t>(this->m_blockSize * (headBlock + 1) - offset);
    }

    if (repaired) {
        Blizzard::String::MemCopy(this->m_buffer, this->m_workBuffer.m_data + this->m_blockSize - skip, skip);
    }

    result = this->ValidateBlocks(this->m_buffer + skip, offset + skip, this->m_size - skip);

    if (!result) {
        return result;
    }

    int64_t end;

    if (this->m_lastBlock) {
        if (!this->m_unalignedEnd) {
            return this->ValidateLastBlock(this->m_buffer + this->m_size - this->m_fileSize % this->m_blockSize, repaired);
        }

        repaired = false;
        result = this->ValidateLastBlock(this->m_workBuffer.m_data + headSize, repaired);
        end = offset + this->m_size;

        if (this->m_fileSize < end) {
            end = this->m_fileSize;
        }
    } else {
        if (!this->m_unalignedEnd) {
            return result;
        }

        repaired = false;
        result = this->ValidateBlock(this->m_workBuffer.m_data + headSize, static_cast<int32_t>((offset + this->m_size - 1) / this->m_blockSize), this->m_blockSize, repaired);
        end = offset + this->m_size;
    }

    int32_t tail = static_cast<int32_t>(end % this->m_blockSize);

    if (result && repaired) {
        Blizzard::String::MemCopy(this->m_buffer + this->m_size - tail, this->m_workBuffer.m_data + headSize, tail);
    }

    return result;
}

// OFFSET: 0x007E8250/0x1404F2910
bool System_Mopaq::MD5VerifyData::ValidateRead(IoOperationType type, int64_t offset) {
    bool result = this->InitializeMD5Buffer();

    if (result) {
        switch (type) {
            case IO_ALIGNED:
                result = this->ValidateAlignedRead();
                break;
            case IO_SINGLE_BLOCK:
                result = this->ValidateSingleBlock(offset);
                break;
            case IO_MULTIPLE_BLOCKS:
                result = this->ValidateMultipleBlocks(offset);
                break;
            default:
                result = false;
                break;
        }

        if (result) {
            return result;
        }
    }

    Blizzard::String::QuickFormat<1024> message("%s - InitializeMD5Buffer fail %d %lld", "System_Mopaq::MD5VerifyData::ValidateRead", type, offset);
    Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

    if (this->m_md5s.m_data) {
        delete[] this->m_md5s.m_data;
        this->m_md5s.m_data = NULL;
    }

    this->m_md5s.m_count = 0;
    return result;
}

// OFFSET: 0x007E8310/0x1404F29E0
bool System_Mopaq::MD5VerifyData::ReadAndValidate(int64_t offset, Blizzard::Mopaq::MpqHandleRecord* handle) {
    this->m_handle = handle;
    handle->m_handler->m_baseOffset = this->m_baseOffset;

    int64_t relative = offset - this->m_baseOffset;
    IoOperationType type = this->PrepareWorkBuffer(relative);
    bool result = false;

    switch (type) {
        case IO_ALIGNED:
            result = this->ReadAligned(relative);
            break;
        case IO_SINGLE_BLOCK:
            result = this->ReadSingleBlock(relative);
            break;
        case IO_MULTIPLE_BLOCKS:
            result = this->ReadMultipleBlocks(relative);
            break;
        default:
            break;
    }

    if (result) {
        return this->ValidateRead(type, relative);
    }

    return result;
}

// OFFSET: 0x007B31D0
void System_Mopaq::MD5VerifyData::ComputeMD5s(const uint8_t* buffer, int64_t offset, int32_t size) {
    int32_t block = static_cast<int32_t>(offset / this->m_blockSize);

    while (size > 0) {
        int32_t amount = this->m_blockSize >= size ? size : this->m_blockSize;
        Blizzard::Util::Digest<16> md5;
        ComputeMD5(&md5, buffer, amount);
        memcpy(this->m_md5s.m_data + 16 * block, md5.m_data, 16);

        size -= this->m_blockSize;
        buffer += amount;
        block++;
    }
}

// OFFSET: 0x007E8390/0x1404F2A90
bool System_Mopaq::MD5VerifyData::ValidateExistingBlock(int64_t offset) {
    if (static_cast<uint32_t>(this->m_blockSize) > this->m_workBuffer.m_count) {
        this->m_workBuffer.SetCount(this->m_blockSize);
    }

    memset(this->m_workBuffer.m_data, 0, this->m_blockSize);

    int64_t available = this->m_written - offset;
    int64_t amount = available >= this->m_blockSize ? this->m_blockSize : available;
    int32_t count = amount > 0 ? static_cast<int32_t>(amount) : 0;
    bool result = true;

    if (count > 0) {
        result = this->m_handle->m_readFunc(this->m_handle->m_handler, this->m_workBuffer.m_data, offset, &count);

        if (result && count > 0) {
            bool repaired;
            return this->ValidateBlock(this->m_workBuffer.m_data, static_cast<int32_t>(offset / this->m_blockSize), count, repaired);
        }
    }

    return result;
}

// OFFSET: 0x007EAC90/0x1404F7720
bool System_Mopaq::MD5VerifyData::MD5SingleBlock(int64_t offset) {
    if (!this->m_unalignedStart && !this->m_unalignedEnd) {
        this->ComputeMD5s(this->m_buffer, offset, this->m_size);
        return true;
    }

    int64_t blockStart = offset - offset % this->m_blockSize;
    bool result = this->ValidateExistingBlock(blockStart);

    if (!result) {
        return result;
    }

    memmove(this->m_workBuffer.m_data + offset - blockStart, this->m_buffer, this->m_size);

    int32_t length = this->m_blockSize;

    if (this->m_lastBlock) {
        length = static_cast<int32_t>(this->m_fileSize % this->m_blockSize);
    }

    if (blockStart + length > this->m_written) {
        int64_t end = offset + this->m_size;

        if (end < this->m_written) {
            end = this->m_written;
        }

        length = static_cast<int32_t>(end - blockStart);
    }

    this->ComputeMD5s(this->m_workBuffer.m_data, blockStart, length);
    return result;
}

// OFFSET: 0x007EADB0/0x1404F7830
bool System_Mopaq::MD5VerifyData::MD5MultipleBlocks(int64_t offset) {
    uint8_t* source = this->m_buffer;
    int32_t remaining = this->m_size;
    int64_t position = offset;
    bool result = true;

    if (this->m_unalignedStart) {
        int64_t blockStart = offset - offset % this->m_blockSize;
        result = this->ValidateExistingBlock(blockStart);

        uint32_t head = static_cast<uint32_t>(blockStart + this->m_blockSize - offset);

        if (result) {
            memmove(this->m_workBuffer.m_data + (offset - blockStart), source, head);
            this->ComputeMD5s(this->m_workBuffer.m_data, blockStart, this->m_blockSize);
        }

        source += head;
        position = blockStart + this->m_blockSize;
        remaining -= head;

        if (!result) {
            return result;
        }
    }

    int32_t full = remaining;

    if (this->m_unalignedEnd) {
        full = remaining - remaining % this->m_blockSize;
    }

    if (full > 0) {
        this->ComputeMD5s(source, position, full);
    }

    source += full;
    position += full;
    remaining -= full;

    if (remaining > 0) {
        result = this->ValidateExistingBlock(position);

        if (!result) {
            return result;
        }

        Blizzard::String::MemCopy(this->m_workBuffer.m_data, source, remaining);

        int32_t length = this->m_blockSize;

        if (this->m_lastBlock) {
            length = static_cast<int32_t>(this->m_fileSize % this->m_blockSize);
        }

        if (position + length > this->m_written) {
            int64_t end = offset + this->m_size;

            if (end < this->m_written) {
                end = this->m_written;
            }

            length = static_cast<int32_t>(end - position);
        }

        if (position + length > this->m_fileSize) {
            length -= static_cast<int32_t>(offset);
        }

        this->ComputeMD5s(this->m_workBuffer.m_data, position, length);
    }

    return result;
}

// OFFSET: 0x007EAF70
bool System_Mopaq::MD5VerifyData::ComputeWriteMD5s(int64_t offset, IoOperationType type) {
    switch (type) {
        case IO_ALIGNED:
            this->ComputeMD5s(this->m_buffer, 0, this->m_size);
            return true;
        case IO_SINGLE_BLOCK:
            return this->MD5SingleBlock(offset);
        case IO_MULTIPLE_BLOCKS:
            return this->MD5MultipleBlocks(offset);
        default:
            return false;
    }
}

// OFFSET: 0x007BAB80/0x1404B2450
bool System_Mopaq::MD5VerifyData::EnsureMD5Table(int64_t offset, int64_t size) {
    bool resize = true;

    if (this->m_md5s.m_count) {
        if (size + offset < this->m_fileSize) {
            return true;
        }

        resize = this->m_writing;
    }

    if (resize) {
        this->m_fileSize = size + offset;
        this->m_blockCount = (this->m_blockSize + static_cast<int32_t>(size) + static_cast<int32_t>(offset) - 1) / this->m_blockSize;
    }

    uint32_t count = this->m_md5s.m_count;

    if (static_cast<uint32_t>(16 * this->m_blockCount) > count) {
        this->m_md5s.SetCount(16 * this->m_blockCount);
    }

    return this->m_md5s.m_count >= count;
}

// OFFSET: 0x007EAFC0/0x1404F7A00
bool System_Mopaq::MD5VerifyData::MD5AndWrite(int64_t offset, int64_t size, Blizzard::Mopaq::MpqHandleRecord* handle) {
    int64_t relative = offset - this->m_baseOffset;
    this->m_handle = handle;
    this->m_writing = true;

    bool result = this->EnsureMD5Table(relative, size);

    if (!result) {
        return result;
    }

    this->m_handle->m_handler->m_baseOffset = this->m_baseOffset;

    IoOperationType type = this->PrepareWorkBuffer(relative);
    bool ok = this->ComputeWriteMD5s(relative, type);

    if (relative > this->m_written) {
        memset(this->m_workBuffer.m_data, 0, this->m_blockSize);

        int64_t position = this->m_written;
        int64_t gap = relative - this->m_written;

        if (!ok) {
            return ok;
        }

        while (gap > 0) {
            int64_t amount = this->m_blockSize >= gap ? gap : this->m_blockSize;
            result = this->m_handle->m_writeFunc(this->m_handle->m_handler, this->m_workBuffer.m_data, position, static_cast<int32_t>(amount));
            this->m_written += static_cast<int32_t>(amount);
            ok = result;
            position = this->m_written;
            gap = relative - this->m_written;

            if (!result) {
                return result;
            }
        }
    }

    if (ok) {
        ok = this->m_handle->m_writeFunc(this->m_handle->m_handler, this->m_buffer, relative, this->m_size);

        if (ok) {
            int64_t end = relative + this->m_size;

            if (this->m_written < end) {
                this->m_written = end;
            }
        }
    }

    return ok;
}

// PROVEN: 2a8f4f4c IDENTICAL/DIFFERENT
// OFFSET: 0x007BFBC0/0x1404B8EF0
bool System_Mopaq::md5verify_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    const System_SFile::BLOCKENTRY* entry = parms->m_blockEntry;
    int64_t size = entry->m_compressedSize ? entry->m_compressedSize : entry->m_fileSize;

    CreateMD5VerifyData(parms->m_handle, parms->m_count, size, (parms->m_flags & 0x4) != 0);
    return true;
}

// OFFSET: 0x007C3400/0x1404BDA30
bool System_Mopaq::md5verify_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    MD5VerifyData* data = parms->m_handle->m_md5Verify;
    bool result = true;

    if (data) {
        result = data->WriteMD5Table(parms->m_handle);
    }

    DestroyMD5VerifyData(parms->m_handle);
    return result;
}

// OFFSET: 0x007EAC30/0x1404F76D0
bool System_Mopaq::md5verify_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    MD5VerifyData* data = handle->m_md5Verify;

    if (!data || (parms->m_flags & 0x1)) {
        if (!system->m_next) {
            return false;
        }

        return CallNext(system, parms);
    }

    data->m_buffer = static_cast<uint8_t*>(parms->m_buffer);
    data->m_size = parms->m_count;
    return data->ReadAndValidate(parms->m_offset, handle);
}

// OFFSET: 0x007EC390/0x1404F8890
bool System_Mopaq::md5verify_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    MD5VerifyData* data = handle->m_md5Verify;

    if (data) {
        data->m_size = parms->m_count;
        data->m_buffer = static_cast<uint8_t*>(parms->m_buffer);
        return data->MD5AndWrite(parms->m_offset, parms->m_count, handle);
    }

    if (!system->m_next) {
        return false;
    }

    return CallNext(system, parms);
}
