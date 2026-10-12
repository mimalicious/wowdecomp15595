#include <algorithm>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/SharedMemory.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

#pragma pack(push, 4)
struct PartialFileHeader {
    /* 0000 */ char m_magic[4];
    /* 0004 */ int32_t m_version;
    /* 0008 */ int32_t m_contentId;
    /* 000C */ int64_t m_fileSize;
    /* 0014 */ int32_t m_chunkSize;
};
#pragma pack(pop)

// STATIC: 0x00BCA138/0x1409B8970
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

// STATIC: 0x00BCD184
static const uint32_t s_retryDelays[11] = { 100, 500, 1000, 2000, 3000, 5000, 7000, 10000, 12000, 15000, 1 };

// STATIC: 0x00D07424/0x140BB7F30
static Blizzard::Storage::DefaultBitFieldAllocator s_defaultBitFieldAllocator;

// STATIC: 0x00D07184/0x140BB7A30
Blizzard::Storage::BitFieldAllocator* Blizzard::Storage::s_bitFieldAllocator = &s_defaultBitFieldAllocator;

// STATIC: 0x00EBA768
Blizzard::Storage::PartialBlockMap Blizzard::Storage::s_partialBlockMap;

// STATIC: 0x00EBA23C
Blizzard::Lock::Atomic Blizzard::Storage::s_partialBlockMapDestroyed;

// OFFSET: 0x007B3D00
static bool ValidateHeader(const PartialFileHeader* header, int64_t fileSize) {
    return header->m_chunkSize && !memcmp(header->m_magic, "ptv3", 4) && header->m_fileSize == fileSize && header->m_version == 3;
}

// OFFSET: 0x007BB920
static bool ValidateHeader(const PartialFileHeader* header, int64_t fileSize, int32_t contentId) {
    return header->m_chunkSize && !memcmp(header->m_magic, "ptv3", 4) && header->m_fileSize == fileSize && header->m_version == 3 && header->m_contentId == contentId;
}

// OFFSET: 0x007BB680
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

// OFFSET: 0x007E9860/0x1404F5070
static Blizzard::File::StreamRecord* CreateLockFile(const char* path) {
    Blizzard::File::StreamRecord* file = NULL;

    if (Blizzard::File::Open(path, 1027, file)) {
        return file;
    }

    Blizzard::String::QuickFormat<1024> message("%s - Fail to create lock: %s", "CreateLockFile", path);
    Blizzard::File::AddToLastErrorStack(3, message.m_buffer, 2);
    System_File::FileError& fileError = System_File::s_error;
    fileError.m_lastError = 3;

    return NULL;
}

// OFFSET: 0x007AC1F0/0x1404A2010
int64_t Blizzard::Storage::BitField::CountSet() const {
    int64_t count = 0;

    for (const uint8_t* bits = this->m_bits; bits < this->m_bits + this->m_length; bits++) {
        count += s_bitCount[*bits];
    }

    return count;
}

// OFFSET: 0x007AC140/0x1404A1F70
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

// OFFSET: 0x007AB9E0/0x1404A20A0
Blizzard::Storage::BitFieldAllocator::~BitFieldAllocator() {
}

// OFFSET: 0x007AC130/0x1404A1F60
Blizzard::Storage::BitField* Blizzard::Storage::DefaultBitFieldAllocator::Attach(const char* name, int64_t bits, BitField* field) {
    return field;
}

// OFFSET: 0x007BFE50/0x1404B9230
Blizzard::Storage::BitField* Blizzard::Storage::DefaultBitFieldAllocator::Create(int64_t bits) {
    BitField* field = new BitField;

    if (!field) {
        return NULL;
    }

    int32_t length = (static_cast<int32_t>(bits) + 7) >> 3;
    field->m_length = length;
    void* block = Blizzard::Memory::Allocate(length, 1);
    field->m_bits = static_cast<uint8_t*>(block);
    field->m_block = block;
    field->m_free = Blizzard::Memory::Free;

    return field;
}

// OFFSET: 0x007BB650/0x1404AA190
void Blizzard::Storage::DefaultBitFieldAllocator::Destroy(BitField* field) {
    if (field) {
        field->m_free(field->m_block);
        delete field;
    }
}

// OFFSET: 0x007CCC50/0x1404C9190
Blizzard::Storage::BitField* Blizzard::Storage::SharedBitFieldAllocator::Attach(const char* name, int64_t bits, BitField* field) {
    uint32_t bytes = static_cast<uint32_t>((bits + 7) >> 3);
    char path[260];

    if (!Blizzard::File::MakeAbsolutePath(name, path, sizeof(path), false)) {
        DefaultBitFieldAllocator::Destroy(field);
        return NULL;
    }

    Blizzard::Memory::SharedMemory* memory;
    int32_t status = Blizzard::Memory::OpenSharedMemory(path, bytes, &memory);

    if (!status) {
        DefaultBitFieldAllocator::Destroy(field);
        return NULL;
    }

    void* data = memory->m_data;

    if (status == 1 && field) {
        Blizzard::String::MemCopy(data, field->m_bits, bytes);
    }

    BitField* shared = new BitField;

    if (shared) {
        shared->m_bits = static_cast<uint8_t*>(data);
        shared->m_length = (static_cast<int32_t>(bits) + 7) >> 3;
        shared->m_block = memory;
        shared->m_free = Blizzard::Memory::CloseSharedMemory;
    }

    DefaultBitFieldAllocator::Destroy(field);

    return shared;
}

// OFFSET: 0x007AC290/0x1404A20D0
void Blizzard::Storage::ShutdownBitFieldAllocator() {
    if (s_bitFieldAllocator != &s_defaultBitFieldAllocator && s_bitFieldAllocator) {
        delete s_bitFieldAllocator;
    }

    s_bitFieldAllocator = NULL;
}

// OFFSET: 0x007BC210/0x1404B4040
void Blizzard::Storage::InitializeBitFieldAllocator(int32_t mode) {
    if (mode == 1) {
        return;
    }

    BitFieldAllocator* allocator = new SharedBitFieldAllocator();

    if (s_bitFieldAllocator != &s_defaultBitFieldAllocator && s_bitFieldAllocator) {
        delete s_bitFieldAllocator;
    }

    s_bitFieldAllocator = allocator;
}

// OFFSET: 0x007E1550
Blizzard::Storage::PartialBlock::PartialBlock(uint32_t blockSize, uint32_t blockCount, uint32_t lastBlockSize, std::string name) {
    this->m_lastBlockSize = lastBlockSize;
    this->m_blockSize = blockSize;
    this->m_blockCount = blockCount;
    this->m_name = name;

    PartialBlockMap::iterator it = s_partialBlockMap.find(this->m_name);

    if (it != s_partialBlockMap.end()) {
        this->m_partial = it->second;
        s_partialBlockMap.erase(it);
    }
}

// OFFSET: 0x007E4760/0x1404EC920
Blizzard::Storage::PartialBlock::~PartialBlock() {
    if (!this->m_partial.empty()) {
        s_partialBlockMap[this->m_name] = this->m_partial;
    }
}

// OFFSET: 0x007D0AE0
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

// OFFSET: 0x007EB720
Blizzard::Storage::PartialBlockMap::~PartialBlockMap() {
    if (s_partialBlockMapDestroyed.Increment() == 1) {
        this->clear();
    }
}

// OFFSET: 0x007E98D0
bool Blizzard::Storage::LockFile::Acquire() {
    if (this->m_file) {
        return true;
    }

    this->m_file = CreateLockFile(this->m_path.c_str());

    return this->m_file != NULL;
}

// OFFSET: 0x007C3980/0x1404BDFC0
bool Blizzard::Storage::LockFile::IsStale(uint32_t seconds) {
    if (this->m_file) {
        return false;
    }

    Blizzard::File::FileInfo info;
    Blizzard::File::GetFileInfo(this->m_path.c_str(), &info);
    uint32_t modified = static_cast<uint32_t>(info.m_modTime / 1000 / 1000);
    uint32_t now = static_cast<uint32_t>(Blizzard::Time::Seconds());

    if (modified > now) {
        return seconds < now - modified - 1;
    }

    return seconds < now - modified;
}

// OFFSET: 0x007AB9B0
Blizzard::Storage::BackingStore::~BackingStore() {
}

// OFFSET: 0x007C6220/0x1404C12B0
Blizzard::Storage::PartialFile::PartialFile() : BackingStore(3), m_chunkSize(s_Chunk_Size), m_fileSize(0), m_contentId(0), m_file(NULL) {
    this->m_dirtyStart = -1;
    this->m_dirtyEnd = -1;
    this->m_lastFlush = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
    this->m_locality = NULL;
    this->m_partialBlocks = NULL;
    this->m_open = false;
    this->m_completed = false;
    this->m_assumedComplete = false;
    this->m_invalidateCount = 0;
    memset(this->m_localityHash, 0, sizeof(this->m_localityHash));
}

// OFFSET: [0x007EB5D0/0x1404F8220, 0x007EDB20/0x1404F9FF0]
Blizzard::Storage::PartialFile::~PartialFile() {
    this->Close();
    s_bitFieldAllocator->Destroy(this->m_locality);
    this->m_locality = NULL;

    if (this->m_lock.m_file) {
        Blizzard::File::Close(this->m_lock.m_file);
        Blizzard::File::Delete(this->m_lock.m_path.c_str());
    }

    delete this->m_partialBlocks;
}

// OFFSET: 0x007BBB00/0x1404B38D0
bool Blizzard::Storage::PartialFile::Read(void* buffer, int64_t offset, int64_t size) {
    if (offset + size > this->m_fileSize || offset < 0) {
        return false;
    }

    if (!this->m_completed && !this->IsAvailable(offset, size)) {
        return false;
    }

    Blizzard::File::ReadExact(this->m_file, buffer, offset, static_cast<int32_t>(size), 0);

    return true;
}

// OFFSET: 0x007ABD90/0x1404A1CC0
bool Blizzard::Storage::PartialFile::FindMissing(int64_t offset, int64_t size, ChunkRange* range) {
    int64_t step = this->m_chunkSize - range->m_offset % this->m_chunkSize;
    int64_t end = offset + size;

    while (range->m_offset < end) {
        if (!this->IsAvailable(range->m_offset, 1)) {
            return true;
        }

        range->m_offset += step;
        step = this->m_chunkSize;
    }

    range->m_offset = end < range->m_offset ? end : range->m_offset;

    return false;
}

// OFFSET: 0x007ABE50/0x1404A1D50
bool Blizzard::Storage::PartialFile::GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) {
    int64_t end = offset + size;
    int64_t position = range->m_offset;

    if (position < end) {
        int64_t length = this->m_chunkSize - position % this->m_chunkSize;

        while (position + length < end && length < range->m_length) {
            if (this->IsAvailable(range->m_offset + length, 1)) {
                break;
            }

            length += this->m_chunkSize;
        }

        range->m_length = length;
        return true;
    }

    range->m_length = 0;

    return false;
}

// OFFSET: 0x007BBF30/0x1404B3D30
void Blizzard::Storage::PartialFile::AlignRange(ChunkRange* range) {
    if (!range->m_length) {
        range->m_chunkCount = 0;
        return;
    }

    int64_t chunkSize = this->m_chunkSize;
    int64_t end = range->m_length + (range->m_offset + chunkSize) - 1;
    end = std::min(end - end % chunkSize, this->m_fileSize);
    int64_t start = range->m_offset - range->m_offset % this->m_chunkSize;
    range->m_offset = start;
    range->m_length = end - start;
    range->m_chunkCount = static_cast<int32_t>((range->m_length + this->m_chunkSize - 1) / this->m_chunkSize);
}

// OFFSET: 0x00B41A60/0x1404C1350
int32_t Blizzard::Storage::PartialFile::GetChunkSize() const {
    return this->m_chunkSize;
}

// OFFSET: 0x007B3E00/0x1404A9610
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

// OFFSET: 0x007C3A00/0x1404BE130
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
            this->m_locality->Set(i);
        }

        if (this->m_dirtyStart < 0 || this->m_dirtyStart > first) {
            this->m_dirtyStart = first;
        }

        if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < last) {
            this->m_dirtyEnd = last;
        }

        if (static_cast<uint32_t>(Blizzard::Time::Milliseconds()) - this->m_lastFlush >= static_cast<uint32_t>(offset & 0xFFF) + 28000) {
            this->Flush();
        }
    }

    return result;
}

// OFFSET: 0x007EB6E0/0x1404F8340
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

// OFFSET: 0x007B3E40
bool Blizzard::Storage::PartialFile::Close() {
    if (this->m_open) {
        Blizzard::Util::MD5 md5;
        uint8_t digest[16];
        md5.Process(this->m_locality->m_bits, this->m_locality->m_length);
        md5.Finish(digest);

        if (memcmp(digest, this->m_localityHash, sizeof(digest))) {
            int64_t fileSize = this->m_fileSize;
            this->GetChunkSize();
            int32_t last = static_cast<int32_t>((fileSize - 1) / this->GetChunkSize());

            if (this->m_dirtyStart < 0 || this->m_dirtyStart > 0) {
                this->m_dirtyStart = 0;
            }

            if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < last) {
                this->m_dirtyEnd = last;
            }

            this->Flush();
        }

        Blizzard::File::Close(this->m_file);
        this->m_open = false;
    }

    return false;
}

// OFFSET: 0x007BBB90/0x1404B3960
bool Blizzard::Storage::PartialFile::WriteUnaligned(const void* buffer, int64_t offset, int32_t size) {
    int32_t length = size;

    if (offset + size > this->m_fileSize) {
        return false;
    }

    int32_t count = size;
    bool result = Blizzard::File::Write(this->m_file, buffer, offset, &count, 0);

    if (result) {
        this->UpdateUnaligned(offset, length);
    }

    return result;
}

// OFFSET: 0x007E3150/0x1404E9D10
void Blizzard::Storage::PartialFile::UpdateUnaligned(int64_t offset, int32_t size) {
    int64_t aligned = offset;

    if (offset % this->GetChunkSize()) {
        aligned = offset - offset % this->GetChunkSize();
    }

    int32_t block = static_cast<int32_t>(aligned / this->GetChunkSize());
    int64_t next = aligned + this->GetChunkSize();

    if (offset + size <= next) {
        if (!this->UpdatePartialBlock(block, size)) {
            return;
        }

        if (this->m_dirtyStart < 0 || this->m_dirtyStart > block) {
            this->m_dirtyStart = block;
        }

        if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < block) {
            this->m_dirtyEnd = block;
        }

        return;
    }

    int64_t remaining = size;
    int32_t first = block;
    int32_t head = static_cast<int32_t>(next - offset);

    if (head < this->GetChunkSize()) {
        if (!this->UpdatePartialBlock(block++, head)) {
            first = block;
        }

        remaining = size - head;
    }

    while (remaining >= this->GetChunkSize()) {
        this->m_locality->Set(block++);
        remaining -= this->GetChunkSize();
    }

    if (remaining > 0 && !this->UpdatePartialBlock(block, static_cast<uint32_t>(remaining))) {
        block--;
    }

    if (first <= block) {
        if (this->m_dirtyStart < 0 || this->m_dirtyStart > first) {
            this->m_dirtyStart = first;
        }

        if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < block) {
            this->m_dirtyEnd = block;
        }
    }
}

// OFFSET: 0x007BBC00/0x1404B3A20
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

// OFFSET: 0x007BBDE0/0x1404B3C00
int64_t Blizzard::Storage::PartialFile::GetLocalBytes(int64_t offset, int64_t size) const {
    int32_t first = static_cast<int32_t>(offset / this->GetChunkSize());
    int32_t last = static_cast<int32_t>((offset + size - 1) / this->GetChunkSize());
    int32_t i = first;
    int64_t total = 0;

    if (this->m_locality && (this->m_locality->m_bits[first >> 3] & (1 << (first & 7)))) {
        if (first == last) {
            total = size;
        } else {
            total = this->m_chunkSize - offset % this->m_chunkSize;
        }

        i = first + 1;
    }

    uint8_t mask = static_cast<uint8_t>(1 << (i & 7));

    for (; i < last; i++) {
        if (this->m_locality && (mask & this->m_locality->m_bits[i >> 3])) {
            total += this->m_chunkSize;
        }

        mask = static_cast<uint8_t>((mask << 1) | (mask >> 7));
    }

    if (i == last && this->m_locality && (this->m_locality->m_bits[last >> 3] & (1 << (last & 7)))) {
        return (offset + size) % this->m_chunkSize + total;
    }

    return total;
}

// OFFSET: 0x007B3F50/0x1404A99A0
bool Blizzard::Storage::PartialFile::Invalidate(int64_t offset, int64_t size) {
    if (!this->m_locality) {
        return false;
    }

    int32_t chunks[2];
    this->GetChunkRange(chunks, offset, size);

    for (int32_t i = chunks[0]; i <= chunks[1]; i++) {
        this->m_locality->Clear(i);
    }

    if (this->m_dirtyStart < 0 || this->m_dirtyStart > chunks[0]) {
        this->m_dirtyStart = chunks[0];
    }

    if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < chunks[1]) {
        this->m_dirtyEnd = chunks[1];
    }

    this->m_completed = false;

    return true;
}

// OFFSET: 0x007C6290/0x1404C1360
bool Blizzard::Storage::PartialFile::IsCompleted() const {
    return this->m_completed;
}

// OFFSET: 0x007C62A0/0x1404C1370
bool Blizzard::Storage::PartialFile::IsReadOnly() const {
    return this->m_lock.m_file == NULL;
}

// OFFSET: 0x007BBCA0/0x1404B3AD0
bool Blizzard::Storage::PartialFile::GetMissingRange(int64_t offset, int64_t size, ChunkRange* range) const {
    int32_t first = static_cast<int32_t>(offset / this->GetChunkSize());
    int32_t last = static_cast<int32_t>((offset + size - 1) / this->GetChunkSize());
    int32_t i = first;
    range->m_chunkCount = 0;
    range->m_offset = 0;
    range->m_length = 0;

    if (i <= last) {
        while (this->m_locality && (this->m_locality->m_bits[i >> 3] & (1 << (i & 7)))) {
            if (++i > last) {
                goto done;
            }
        }

        range->m_offset = static_cast<int64_t>(i) * this->m_chunkSize;
        range->m_chunkCount = 1;
        range->m_length = this->m_chunkSize;
        i++;
    }

done:
    uint8_t mask = static_cast<uint8_t>(1 << (i & 7));

    while (i <= last) {
        if (this->m_locality && (mask & this->m_locality->m_bits[i >> 3])) {
            break;
        }

        range->m_chunkCount++;
        range->m_length += this->m_chunkSize;
        i++;
        mask = static_cast<uint8_t>((mask << 1) | (mask >> 7));
    }

    int64_t remaining = this->m_fileSize - range->m_offset;
    range->m_length = remaining >= range->m_length ? range->m_length : remaining;

    return i > last;
}

// OFFSET: 0x007BC000/0x1404B3DB0
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

// OFFSET: 0x007ABD30
void Blizzard::Storage::PartialFile::GetChunkRange(int32_t* chunks, int64_t offset, int64_t size) {
    chunks[0] = static_cast<int32_t>(offset / this->GetChunkSize());
    chunks[1] = static_cast<int32_t>((size + offset - 1) / this->GetChunkSize());
}

// OFFSET: 0x007B3FE0/0x1404A9A80
bool Blizzard::Storage::PartialFile::IsLocalityComplete() {
    int64_t fileSize = this->m_fileSize;
    this->GetChunkSize();
    int32_t count = static_cast<int32_t>((fileSize - 1) / this->GetChunkSize());

    return this->m_locality && this->m_locality->IsSetUpTo(count);
}

// OFFSET: 0x007B3B50
void Blizzard::Storage::PartialFile::MakeSparse() {
    Blizzard::File::FileInfo info;
    Blizzard::File::GetFileInfo(this->m_file, &info);

    if (info.m_size > 0 && info.m_compressedSize < info.m_size) {
        return;
    }

    char directory[260];
    Blizzard::File::GetWorkingDirectory(directory, sizeof(directory));

    char root[4];
    Blizzard::String::Copy(root, directory, sizeof(root));

    if (!Blizzard::File::SupportsSparse(root)) {
        return;
    }

    Blizzard::File::SetSparse(this->m_file, false);

    int64_t offset = 0;
    int64_t remaining = this->m_fileSize;

    while (remaining > 0) {
        ChunkRange range;

        if (this->GetMissingRange(offset, remaining, &range)) {
            break;
        }

        offset = this->m_chunkSize + range.m_length + range.m_offset;
        Blizzard::File::ZeroRange(this->m_file, range.m_offset, range.m_length);
        remaining = this->m_fileSize - offset;
    }
}

// OFFSET: 0x007EDB50/0x1404FA020
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
        this->m_locality = locality;

        Blizzard::Util::MD5 md5;
        md5.Process(locality->m_bits, locality->m_length);
        md5.Finish(this->m_localityHash);

        this->m_chunkSize = chunkSize;
        this->m_completed = this->IsLocalityComplete();
        return true;
    }

    s_bitFieldAllocator->Destroy(locality);
    this->RebuildLocalityMap(false);

    return this->RebuildHeader();
}

// OFFSET: 0x007EB430/0x1404F7F20
bool Blizzard::Storage::PartialFile::RebuildHeader() {
    if (this->IsReadOnly()) {
        return false;
    }

    PartialFileHeader header;
    header.m_version = 3;
    Blizzard::String::MemCopy(header.m_magic, "ptv3", sizeof(header.m_magic));
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

// OFFSET: 0x007B3D50
bool Blizzard::Storage::PartialFile::RebuildLocalityMap(bool complete) {
    Blizzard::File::FileInfo info;

    if (!Blizzard::File::GetFileInfo(this->m_file, &info)) {
        return false;
    }

    int32_t count = static_cast<int32_t>((this->m_chunkSize + this->m_fileSize - 1) / this->m_chunkSize);
    this->m_locality = s_bitFieldAllocator->Create(count);

    if (complete) {
        memset(this->m_locality->m_bits, 0xFF, this->m_locality->m_length);
        this->m_completed = true;
        this->m_assumedComplete = true;
    } else {
        this->m_completed = false;
        this->m_assumedComplete = false;
    }

    this->m_locality = s_bitFieldAllocator->Attach(info.m_path, count, this->m_locality);

    return true;
}

// OFFSET: 0x007E3000
bool Blizzard::Storage::PartialFile::UpdatePartialBlock(uint32_t block, uint32_t size) {
    if (!this->m_partialBlocks) {
        int32_t count = static_cast<int32_t>((this->m_fileSize + this->GetChunkSize() - 1) / this->GetChunkSize());
        int32_t chunkSize = this->GetChunkSize();
        int32_t last = static_cast<int32_t>(this->m_fileSize % chunkSize);
        this->m_partialBlocks = new PartialBlock(this->GetChunkSize(), count, last, this->m_lock.m_path);
    }

    if (!this->m_partialBlocks->SetPartial(block, size)) {
        return false;
    }

    if (this->m_dirtyStart < 0 || this->m_dirtyStart > static_cast<int32_t>(block)) {
        this->m_dirtyStart = block;
    }

    if (this->m_dirtyEnd < 0 || this->m_dirtyEnd < static_cast<int32_t>(block)) {
        this->m_dirtyEnd = block;
    }

    this->m_locality->Set(block);

    return true;
}

// OFFSET: 0x007E9910/0x1404F5120
bool Blizzard::Storage::PartialFile::WriteLocality(int32_t first, int32_t last) {
    if (this->IsReadOnly() && this->m_lock.IsStale(50)) {
        this->m_lock.Acquire();
    }

    if (this->IsReadOnly()) {
        return true;
    }

    int32_t count = (last >> 3) - (first >> 3) + 1;
    bool result = Blizzard::File::Write(this->m_file, this->m_locality->m_bits + (first >> 3), this->m_fileSize + (first >> 3), &count, 0);

    if (this->m_lock.m_file) {
        Blizzard::File::SetModificationTime(this->m_lock.m_file, Blizzard::Time::GetTimestamp());
    }

    return result;
}

// OFFSET: 0x007BB970
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
        int32_t bitCount = static_cast<int32_t>((header.m_fileSize + header.m_chunkSize - 1) / header.m_chunkSize);
        bits = s_bitFieldAllocator->Create(bitCount);

        if (header.m_fileSize + bits->m_length == headerOffset) {
            result = Blizzard::File::ReadExact(file, bits->m_bits, header.m_fileSize, bits->m_length, 0);

            if (result) {
                bits = s_bitFieldAllocator->Attach(info.m_path, bitCount, bits);
            }
        } else {
            result = false;
        }

        if (!result) {
            s_bitFieldAllocator->Destroy(bits);
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

// OFFSET: 0x007BB750
bool Blizzard::Storage::PartialFile::FileIsCompletePartialFile(Blizzard::File::StreamRecord* file, int64_t archiveSize, int64_t fileSize) {
    uint32_t length = static_cast<uint32_t>(fileSize - archiveSize);
    uint8_t* data = length ? new uint8_t[length] : NULL;
    int32_t count = length;

    if (!Blizzard::File::Read(file, data, archiveSize, &count, 0) || count != static_cast<int32_t>(length) || length < sizeof(PartialFileHeader)) {
        delete[] data;
        return false;
    }

    PartialFileHeader header;
    Blizzard::String::MemCopy(&header, &data[length - sizeof(PartialFileHeader)], sizeof(PartialFileHeader));

    if (!ValidateHeader(&header, header.m_fileSize)) {
        delete[] data;
        return false;
    }

    if (header.m_fileSize == fileSize) {
        delete[] data;
        return true;
    }

    if (header.m_fileSize > fileSize) {
        delete[] data;
        return false;
    }

    int32_t chunks = static_cast<int32_t>((header.m_fileSize + header.m_chunkSize - 1) / header.m_chunkSize);
    int32_t bits = chunks % 8;
    int32_t bytes = chunks / 8;
    int32_t total = bits ? bytes + 1 : bytes;
    int32_t i = length - total - sizeof(PartialFileHeader);
    int32_t end = i + bytes;

    for (; i < end; i++) {
        if (data[i] != 0xFF) {
            delete[] data;
            return false;
        }
    }

    if (!bits) {
        delete[] data;
        return true;
    }

    uint8_t mask = static_cast<uint8_t>(~(-1 << bits));
    bool result = (mask & data[i]) == mask;
    delete[] data;

    return result;
}

// OFFSET: 0x007F0490/0x1404FCDC0
Blizzard::Storage::PartialFile* Blizzard::Storage::PartialFile::CreateShared(Blizzard::File::Filesystem* fs, const char* path, int64_t size, int32_t contentId) {
    if (!EnsureParentDirectory(path)) {
        return NULL;
    }

    std::string lockPath = path + std::string(".lock");
    Blizzard::File::StreamRecord* lockFile = CreateLockFile(lockPath.c_str());
    PartialFile* partialFile = NULL;
    bool result;

    for (const uint32_t* delay = s_retryDelays; ; ) {
        Blizzard::File::StreamRecord* file;
        result = Blizzard::File::Open(fs, path, 527, file);

        if (result) {
            partialFile = new PartialFile();
            partialFile->m_fileSize = size;
            partialFile->m_contentId = contentId;
            partialFile->m_file = file;
            partialFile->m_lock.m_path = lockPath;
            partialFile->m_lock.m_file = lockFile;

            Blizzard::File::FileInfo info;
            result = Blizzard::File::GetFileInfo(file, &info);

            if (result && !info.m_compressedSize && lockFile) {
                Blizzard::File::SetSparse(file, true);
            }

            if (partialFile->ParseFile()) {
                if (result) {
                    partialFile->m_open = true;
                    break;
                }
            } else {
                Blizzard::File::Close(partialFile->m_file);
                partialFile->m_lock.m_file = NULL;
                delete partialFile;
                partialFile = NULL;
                result = false;

                System_File::FileError& fileError = System_File::s_error;

                if (fileError.m_errorStack && fileError.m_errorStack->FindCategory(22)) {
                    break;
                }
            }
        }

        Blizzard::Process::Sleep(*delay);

        if (++delay > &s_retryDelays[10]) {
            break;
        }
    }

    if (result) {
        if (partialFile && lockFile) {
            partialFile->MakeSparse();
        }
    } else if (lockFile) {
        Blizzard::File::Close(lockFile);
        Blizzard::File::Delete(lockPath.c_str());
    }

    return partialFile;
}

// OFFSET: 0x007C3810/0x140317F90
bool Blizzard::Storage::CacheSource::Read(void* buffer, int64_t offset, int64_t size) {
    return false;
}

// OFFSET: 0x007C3810/0x140317F90
bool Blizzard::Storage::CacheSource::FindMissing(int64_t offset, int64_t size, ChunkRange* range) {
    return false;
}

// OFFSET: 0x007C3810/0x140317F90
bool Blizzard::Storage::CacheSource::GetMissingLength(int64_t offset, int64_t size, ChunkRange* range) {
    return false;
}

// OFFSET: 0x00847950/0x1407D7E80
void Blizzard::Storage::CacheSource::AlignRange(ChunkRange* range) {
}
