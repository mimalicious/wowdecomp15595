#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/Storm/H/storm.h"

#pragma pack(push, 4)
struct PartialFileHeader {
    /* 0000 */ char m_magic[4];
    /* 0004 */ int32_t m_version;
    /* 0008 */ int32_t m_contentId;
    /* 000C */ int64_t m_fileSize;
    /* 0014 */ int32_t m_chunkSize;
};
#pragma pack(pop)

// STATIC: %0x00AF2068
static const uint8_t s_bitCount[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// STATIC: %0x00AF5030
static const uint32_t s_retryDelays[11] = { 100, 500, 1000, 2000, 3000, 5000, 7000, 10000, 12000, 15000, 1 };

// OFFSET: %0x007168B0
static bool ReadExact(Blizzard::File::StreamRecord* file, void* buffer, int64_t offset, int32_t size, int32_t flags) {
    int32_t count = size;
    return Blizzard::File::Read(file, buffer, offset, &count, flags) && count == size;
}

// OFFSET: %0x00715D30
static bool ValidateHeader(const PartialFileHeader* header, int64_t fileSize) {
    return header->m_chunkSize && !memcmp(header->m_magic, "ptv3", 4) && header->m_fileSize == fileSize && header->m_version == 3;
}

// OFFSET: %0x0071D7D0
static bool ValidateHeader(const PartialFileHeader* header, int64_t fileSize, int32_t contentId) {
    return header->m_chunkSize && !memcmp(header->m_magic, "ptv3", 4) && header->m_fileSize == fileSize && header->m_version == 3 && header->m_contentId == contentId;
}

// OFFSET: %0x0071D500
static bool EnsureParentDirectory(const char* path) {
    char directory[260];
    memset(directory, 0, sizeof(directory));

    if (path) {
        int32_t length = static_cast<int32_t>(Blizzard::String::FindFilename(path) - path) + 1;
        Blizzard::String::Copy(directory, path, length <= 260 ? length : 260);
    } else {
        directory[0] = '\0';
    }

    int32_t existence = Blizzard::File::Exists(directory);

    if (existence == 1) {
        return false;
    }

    if (!existence && !Blizzard::File::CreateDirectory(directory, true, true) && Blizzard::File::Exists(directory) != 2) {
        return false;
    }

    return true;
}

// OFFSET: %0x0071DA70
Blizzard::Storage::BitField::BitField(int32_t count) {
    this->m_length = (count + 7) >> 3;
    this->m_bits = static_cast<uint8_t*>(SMemAlloc(this->m_length, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
}

// OFFSET: %0x0070EB70
int32_t Blizzard::Storage::BitField::CountSet() const {
    int32_t count = 0;

    for (const uint8_t* bits = this->m_bits; bits < this->m_bits + this->m_length; bits++) {
        count += s_bitCount[*bits];
    }

    return count;
}

// OFFSET: %0x0070EAC0
bool Blizzard::Storage::BitField::IsSetUpTo(int32_t count) const {
    int32_t words = count / 64;
    int32_t rest = count % 64;
    const uint8_t* bits = this->m_bits;

    for (int32_t i = 0; i < words; i++) {
        const uint32_t* word = reinterpret_cast<const uint32_t*>(bits + 8 * i);

        if ((word[0] & word[1]) != 0xFFFFFFFF) {
            return false;
        }
    }

    if (!rest) {
        return true;
    }

    int32_t bytes = rest / 8;
    int32_t bitCount = rest % 8;

    for (int32_t i = 0; i < bytes; i++) {
        if (bits[8 * words + i] != 0xFF) {
            return false;
        }
    }

    if (!bitCount) {
        return true;
    }

    uint8_t mask = static_cast<uint8_t>(~(-1 << bitCount));

    return (mask & bits[8 * words + bytes]) == mask;
}

// OFFSET: %0x00732290
bool Blizzard::Storage::PartialBlock::SetPartial(uint32_t block, uint32_t size) {
    uint32_t complete = this->m_blockSize;

    if (this->m_lastBlockSize && block == this->m_blockCount - 1) {
        complete = this->m_lastBlockSize;
    }

    std::map<uint32_t, uint32_t>::iterator it = this->m_partial.find(block);

    if (it == this->m_partial.end()) {
        if (size == complete) {
            return true;
        }

        this->m_partial.insert(std::make_pair(block, size));
    } else {
        it->second += size;

        if (it->second == complete) {
            this->m_partial.erase(it);
            return true;
        }
    }

    return false;
}

// OFFSET: %0x0070E790
Blizzard::Storage::BackingStore::~BackingStore() {
}

// OFFSET: %0x00726F40
Blizzard::Storage::PartialFile::PartialFile() {
    this->m_chunkSize = s_Chunk_Size;
    this->m_fileSize = 0;
    this->m_contentId = 0;
    this->m_file = NULL;
    this->m_lockFile = NULL;
    this->m_readOnly = false;
    this->m_dirtyStart = -1;
    this->m_dirtyEnd = -1;
    this->m_lastFlush = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
    this->m_locality = NULL;
    this->m_partialBlocks = NULL;
    this->m_open = false;
    this->m_completed = false;
    this->m_assumedComplete = false;
    this->m_invalidateCount = 0;
}

// OFFSET: %0x0073DBD0
Blizzard::Storage::PartialFile::~PartialFile() {
    this->Close();

    if (this->m_locality) {
        SMemFree(this->m_locality->m_bits, __FILE__, __LINE__, 0);
        delete this->m_locality;
    }

    if (this->m_lockFile) {
        Blizzard::File::Close(this->m_lockFile);
        Blizzard::File::Delete(this->m_lockPath.c_str());
    }

    delete this->m_partialBlocks;
}

// PROVEN: 6b66d99c IDENTICAL/NONE
// OFFSET: 0x007B0A90
int32_t Blizzard::Storage::PartialFile::GetChunkSize() const {
    return this->m_chunkSize;
}

// OFFSET: %0x00715D80
int64_t Blizzard::Storage::PartialFile::GetTotalLocalSize() const {
    if (this->m_completed) {
        return this->m_fileSize;
    }

    BitField* locality = this->m_locality;

    if (!locality) {
        return 0;
    }

    int64_t chunkSize = this->GetChunkSize();

    return chunkSize * locality->CountSet();
}

// OFFSET: %0x0071D820
bool Blizzard::Storage::PartialFile::Read(void* buffer, int64_t offset, int32_t size) {
    if (offset + size > this->m_fileSize) {
        return false;
    }

    if (offset < 0) {
        return false;
    }

    if (!this->m_completed && !this->IsAvailable(offset, size)) {
        return false;
    }

    ReadExact(this->m_file, buffer, offset, size, 0);

    return true;
}

// OFFSET: %0x00724A10
bool Blizzard::Storage::PartialFile::Write(const void* buffer, int64_t offset, int32_t size) {
    if (offset % this->GetChunkSize()) {
        return false;
    }

    int32_t chunkSize = this->GetChunkSize();

    if (size % chunkSize && offset + size != this->m_fileSize) {
        return false;
    }

    int64_t end = offset + size;

    if (end > this->m_fileSize) {
        return false;
    }

    int32_t count = size;
    bool result = Blizzard::File::Write(this->m_file, buffer, offset, &count, 0);

    if (result) {
        int32_t first = static_cast<int32_t>(offset / this->GetChunkSize());
        int32_t last = static_cast<int32_t>((end - 1) / this->GetChunkSize());

        for (int32_t i = first; i <= last; i++) {
            this->m_locality->m_bits[i >> 3] |= 1 << (i & 7);
        }

        this->MarkDirty(first, last);

        if (static_cast<uint32_t>(Blizzard::Time::Milliseconds()) - this->m_lastFlush >= static_cast<uint32_t>(offset & 0xFFF) + 28000) {
            this->Flush();
        }
    }

    return result;
}

// OFFSET: %0x00724B60
bool Blizzard::Storage::PartialFile::Flush() {
    if (this->m_dirtyStart < 0 || this->m_dirtyEnd < 0) {
        return true;
    }

    bool result = this->WriteLocality(this->m_dirtyStart, this->m_dirtyEnd);

    if (result) {
        this->m_dirtyStart = -1;
        this->m_dirtyEnd = -1;
        this->m_lastFlush = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
    }

    return result;
}

// OFFSET: %0x0073DE00
bool Blizzard::Storage::PartialFile::WriteUnaligned(const void* buffer, int64_t offset, int32_t size) {
    int64_t length = size;

    if (offset + size > this->m_fileSize) {
        return false;
    }

    int32_t count = size;
    bool result = Blizzard::File::Write(this->m_file, buffer, offset, &count, 0);

    if (!result) {
        return result;
    }

    int64_t aligned = offset;

    if (offset % this->GetChunkSize()) {
        aligned = offset - offset % this->GetChunkSize();
    }

    int32_t block = static_cast<int32_t>(aligned / this->GetChunkSize());
    int64_t next = aligned + this->GetChunkSize();

    if (offset + size > next) {
        int64_t head = next - offset;
        int64_t remaining = size;
        int32_t first = block;

        if (head < this->GetChunkSize()) {
            if (!this->UpdatePartialBlock(block++, static_cast<uint32_t>(head))) {
                first = block;
            }

            remaining = length - head;
        }

        uint8_t mask = static_cast<uint8_t>(1 << (block & 7));

        while (remaining >= this->GetChunkSize()) {
            this->m_locality->m_bits[block++ >> 3] |= mask;
            mask = static_cast<uint8_t>((mask << 1) | (mask >> 7));
            remaining -= this->GetChunkSize();
        }

        if (remaining > 0 && !this->UpdatePartialBlock(block, static_cast<uint32_t>(remaining))) {
            block--;
        }

        if (first <= block) {
            this->MarkDirty(first, block);
        }
    } else if (this->UpdatePartialBlock(block, size)) {
        this->MarkDirty(block, block);
    }

    return result;
}

// OFFSET: %0x0073DCF0
bool Blizzard::Storage::PartialFile::UpdatePartialBlock(uint32_t block, uint32_t size) {
    if (!this->m_partialBlocks) {
        int32_t count = static_cast<int32_t>((this->m_fileSize + this->GetChunkSize() - 1) / this->GetChunkSize());
        int32_t last = static_cast<int32_t>(this->m_fileSize % this->GetChunkSize());
        PartialBlock* partialBlocks = new PartialBlock();
        partialBlocks->m_blockSize = this->GetChunkSize();
        partialBlocks->m_lastBlockSize = last;
        partialBlocks->m_blockCount = count;
        this->m_partialBlocks = partialBlocks;
    }

    if (!this->m_partialBlocks->SetPartial(block, size)) {
        return false;
    }

    this->m_locality->m_bits[static_cast<int32_t>(block) >> 3] |= 1 << (block & 7);

    return true;
}

// OFFSET: %0x00715DC0
bool Blizzard::Storage::PartialFile::Close() {
    if (this->m_open) {
        this->Flush();
        Blizzard::File::Close(this->m_file);
        this->m_open = false;
    }

    return false;
}

// OFFSET: %0x0071D8B0
bool Blizzard::Storage::PartialFile::IsAvailable(int64_t offset, int64_t size) const {
    int32_t first = static_cast<int32_t>(offset / this->GetChunkSize());
    int32_t last = static_cast<int32_t>((size + offset - 1) / this->GetChunkSize());

    if (first > last) {
        return true;
    }

    BitField* locality = this->m_locality;

    while (locality && (locality->m_bits[first >> 3] & (1 << (first & 7)))) {
        first++;

        if (first > last) {
            return true;
        }
    }

    return false;
}

// OFFSET: %0x00715E10
bool Blizzard::Storage::PartialFile::Invalidate(int64_t offset, int64_t size) {
    if (!this->m_locality) {
        return false;
    }

    if (this->m_assumedComplete && ++this->m_invalidateCount > 16) {
        size = this->m_fileSize;
        offset = 0;
        this->m_assumedComplete = false;
        this->m_invalidateCount = 0;
    }

    int32_t chunks[2];
    this->GetChunkRange(chunks, offset, size);

    for (int32_t i = chunks[0]; i <= chunks[1]; i++) {
        this->m_locality->m_bits[i >> 3] &= ~(1 << (i & 7));
    }

    this->MarkDirty(chunks[0], chunks[1]);
    this->m_completed = false;

    return true;
}

// OFFSET: %0x00726FA0
bool Blizzard::Storage::PartialFile::IsCompleted() const {
    return this->m_completed;
}

// OFFSET: %0x0071D950
bool Blizzard::Storage::PartialFile::IsMatch(const char* path, int64_t size, int32_t contentId) const {
    if (contentId != this->m_contentId || size != this->m_fileSize) {
        return false;
    }

    char name[260];
    memset(name, 0, sizeof(name));

    if (path) {
        char* dst = name;
        int32_t remaining = 260;

        while (true) {
            char c = *path++;
            remaining--;

            if (!c) {
                *dst = '\0';
                break;
            }

            *dst++ = c == '/' ? '\\' : c;

            if (!remaining) {
                dst[-1] = '\0';
                break;
            }
        }
    } else {
        name[0] = '\0';
    }

    const char* filename = this->m_file->m_pathname;

    return filename && !_stricmp(filename, name);
}

// OFFSET: %0x0070EA60
void Blizzard::Storage::PartialFile::GetChunkRange(int32_t* chunks, int64_t offset, int64_t size) {
    chunks[0] = static_cast<int32_t>(offset / this->GetChunkSize());
    chunks[1] = static_cast<int32_t>((size + offset - 1) / this->GetChunkSize());
}

// OFFSET: %0x0070EA30
void Blizzard::Storage::PartialFile::MarkDirty(int32_t first, int32_t last) {
    if (this->m_dirtyStart < 0 || this->m_dirtyStart > first) {
        this->m_dirtyStart = first;
    }

    if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < last) {
        this->m_dirtyEnd = last;
    }
}

// OFFSET: %0x0071DA10
bool Blizzard::Storage::PartialFile::WriteLocality(int32_t first, int32_t last) {
    if (this->m_readOnly) {
        return true;
    }

    int32_t count = (last >> 3) - (first >> 3) + 1;

    return Blizzard::File::Write(this->m_file, this->m_locality->m_bits + (first >> 3), this->m_fileSize + (first >> 3), &count, 0);
}

// OFFSET: %0x00715EC0
bool Blizzard::Storage::PartialFile::IsLocalityComplete() {
    int64_t fileSize = this->m_fileSize;
    this->GetChunkSize();
    int32_t count = static_cast<int32_t>((fileSize - 1) / this->GetChunkSize());

    return this->m_locality && this->m_locality->IsSetUpTo(count);
}

// OFFSET: %0x00721630
bool Blizzard::Storage::PartialFile::RebuildLocalityMap(bool complete) {
    int32_t count = static_cast<int32_t>((this->m_chunkSize + this->m_fileSize - 1) / this->m_chunkSize);
    this->m_locality = new BitField(count);

    if (complete) {
        memset(this->m_locality->m_bits, 0xFF, this->m_locality->m_length);
        this->m_completed = true;
        this->m_assumedComplete = true;
    } else {
        this->m_completed = false;
        this->m_assumedComplete = false;
    }

    return true;
}

// OFFSET: %0x007511C0
bool Blizzard::Storage::PartialFile::RebuildHeader() {
    if (this->m_readOnly) {
        return false;
    }

    PartialFileHeader header;
    memcpy(header.m_magic, "ptv3", sizeof(header.m_magic));
    header.m_version = 3;
    header.m_contentId = this->m_contentId;
    header.m_fileSize = this->m_fileSize;
    header.m_chunkSize = this->m_chunkSize;

    int32_t count = static_cast<int32_t>((this->m_fileSize + this->m_chunkSize - 1) / this->m_chunkSize);
    int64_t expected = this->m_fileSize + this->m_locality->m_length + sizeof(PartialFileHeader);

    Blizzard::File::FileInfo info;
    bool result = Blizzard::File::GetFileInfo(this->m_file, &info);

    if (result) {
        if (info.m_size != expected) {
            result = Blizzard::File::SetEOF(this->m_file, expected, 0);
            Blizzard::File::SetEOF(this->m_file, expected, 0);
        }

        if (result) {
            result = this->WriteLocality(0, count - 1);

            if (result) {
                int32_t written = sizeof(PartialFileHeader);
                result = Blizzard::File::Write(this->m_file, &header, this->m_fileSize + this->m_locality->m_length, &written, 0);
            }
        }
    }

    Blizzard::File::Flush(this->m_file);

    if (!result) {
        System_File::FileError& fileError = System_File::s_error;

        if (fileError.m_lastError == 11) {
            Blizzard::File::AddToLastErrorStack(11, "Failed to create data archive - Insufficient disk space", 22);
        }

        Blizzard::File::AddToLastErrorStack(0, "Storage::PartialFile::RebuildHeader - Failed to write to disk", 22);
    }

    return result;
}

// OFFSET: %0x00724880
bool Blizzard::Storage::PartialFile::ParseFileFromHeader(Blizzard::File::StreamRecord* file, BitField** locality, int32_t* chunkSize, int64_t* fileSize, int32_t* contentId) {
    PartialFileHeader header;
    memset(&header, 0, sizeof(header));

    Blizzard::File::FileInfo info;

    if (!Blizzard::File::GetFileInfo(file, &info)) {
        return false;
    }

    int64_t headerOffset = info.m_size - sizeof(PartialFileHeader);
    BitField* bits = NULL;
    int32_t count = sizeof(PartialFileHeader);

    if (!Blizzard::File::Read(file, &header, headerOffset, &count, 0) || count != sizeof(PartialFileHeader)) {
        return false;
    }

    bool result = ValidateHeader(&header, header.m_fileSize, header.m_contentId);

    if (!result) {
        return result;
    }

    if (locality) {
        bits = new BitField(static_cast<int32_t>((header.m_fileSize + header.m_chunkSize - 1) / header.m_chunkSize));

        if (header.m_fileSize + bits->m_length == headerOffset) {
            result = ReadExact(file, bits->m_bits, header.m_fileSize, bits->m_length, 0);
        } else {
            result = false;
        }

        if (!result) {
            SMemFree(bits->m_bits, __FILE__, __LINE__, 0);
            delete bits;
            return result;
        }
    }

    if (locality) {
        *locality = bits;
    }

    if (chunkSize) {
        *chunkSize = header.m_chunkSize;
    }

    if (fileSize) {
        *fileSize = header.m_fileSize;
    }

    if (contentId) {
        *contentId = header.m_contentId;
    }

    return result;
}

// OFFSET: %0x00754FC0
bool Blizzard::Storage::PartialFile::ParseFile() {
    Blizzard::File::FileInfo info;

    if (this->m_open || !Blizzard::File::GetFileInfo(this->m_file, &info)) {
        return false;
    }

    if (info.m_size == this->m_fileSize) {
        this->RebuildLocalityMap(true);
        return this->RebuildHeader();
    }

    BitField* locality = NULL;
    int64_t fileSize = 0;
    int32_t chunkSize = 0;
    int32_t contentId = 0;

    if (ParseFileFromHeader(this->m_file, &locality, &chunkSize, &fileSize, &contentId) && this->m_fileSize == fileSize && this->m_contentId == contentId) {
        this->m_chunkSize = chunkSize;
        this->m_locality = locality;
        this->m_completed = this->IsLocalityComplete();
        return true;
    }

    this->RebuildLocalityMap(false);

    return this->RebuildHeader();
}

// OFFSET: %0x0071D630
bool Blizzard::Storage::PartialFile::FileIsCompletePartialFile(Blizzard::File::StreamRecord* file, int64_t archiveSize, int64_t fileSize) {
    uint32_t length = static_cast<uint32_t>(fileSize - archiveSize);
    uint8_t* data = length ? new uint8_t[length] : NULL;
    int32_t count = length;

    if (!Blizzard::File::Read(file, data, archiveSize, &count, 0) || count != static_cast<int32_t>(length) || length < sizeof(PartialFileHeader)) {
        delete[] data;
        return false;
    }

    int64_t position = archiveSize;

    if (data && !strncmp(reinterpret_cast<char*>(data), "NGIS", 4)) {
        position = archiveSize + 260;
    }

    if (position == fileSize) {
        delete[] data;
        return true;
    }

    PartialFileHeader header;
    Blizzard::String::MemCopy(&header, &data[length - sizeof(PartialFileHeader)], sizeof(PartialFileHeader));

    if (!ValidateHeader(&header, position)) {
        delete[] data;
        return false;
    }

    int64_t chunks = (position + header.m_chunkSize - 1) / header.m_chunkSize;
    int32_t bits = static_cast<int32_t>(chunks) % 8;
    int32_t bytes = static_cast<int32_t>(chunks) / 8;

    if (!bits) {
        bytes++;
    }

    int32_t i = 0;

    for (; i < bytes; i++) {
        if (data[i] != 0xFF) {
            delete[] data;
            return false;
        }
    }

    if (bits) {
        uint8_t mask = static_cast<uint8_t>(~(-1 << bits));
        bool result = (mask & data[i]) == mask;
        delete[] data;
        return result;
    }

    delete[] data;

    return true;
}

// OFFSET: %0x00756200
Blizzard::Storage::PartialFile* Blizzard::Storage::PartialFile::CreateShared(Blizzard::File::Filesystem* fs, const char* path, int64_t size, int32_t contentId) {
    if (!EnsureParentDirectory(path)) {
        return NULL;
    }

    std::string lockPath = path + std::string(".lock");
    Blizzard::File::StreamRecord* lockFile = NULL;

    if (!Blizzard::File::Open(lockPath.c_str(), 1027, lockFile)) {
        lockFile = NULL;
    }

    PartialFile* partialFile = NULL;
    bool opened = false;

    for (const uint32_t* delay = s_retryDelays; delay <= &s_retryDelays[10]; delay++) {
        Blizzard::File::StreamRecord* file;
        opened = Blizzard::File::Open(fs, path, 527, file);

        if (opened) {
            partialFile = new PartialFile();
            partialFile->m_fileSize = size;
            partialFile->m_contentId = contentId;
            partialFile->m_file = file;
            partialFile->m_lockPath = lockPath;
            partialFile->m_readOnly = lockFile == NULL;
            partialFile->m_lockFile = lockFile;

            if (partialFile->ParseFile()) {
                partialFile->m_open = true;
                return partialFile;
            }

            Blizzard::File::Close(partialFile->m_file);
            partialFile->m_lockFile = NULL;
            delete partialFile;
            partialFile = NULL;
            opened = false;

            System_File::FileError& fileError = System_File::s_error;

            if (fileError.m_errorStack && fileError.m_errorStack->FindCategory(22)) {
                break;
            }
        }

        Blizzard::Process::Sleep(*delay);
    }

    if (lockFile) {
        Blizzard::File::Close(lockFile);
        Blizzard::File::Delete(lockPath.c_str());
    }

    return partialFile;
}
