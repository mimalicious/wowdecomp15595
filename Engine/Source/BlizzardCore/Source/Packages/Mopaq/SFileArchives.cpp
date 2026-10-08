#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/ArchiveCheckpoint.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQEncrypt.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/NewZerofill.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SComp.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileSignatures.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SBig.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C0C254
Blizzard::Lock::Atomic System_SFile::s_nextArchiveId = { static_cast<int32_t>(0x80000000) };

// STATIC: %0x00D81308
uint32_t System_SFile::s_lastError;

// STATIC: %0x00D87268
Blizzard::Util::Digest<16> System_SFile::s_emptyMd5;

// STATIC: %0x00C0C250
bool System_SFile::s_threadSafe = true;

// PROVEN: 0078f922 IDENTICAL/IDENTICAL
// OFFSET: 0x007AAB40/0x1404A0E70
uint32_t Blizzard::Mopaq::SFileGetLastError() {
    return System_SFile::s_lastError;
}

// OFFSET: 0x007AAB80/0x1404A0EA0
void System_SFile::BitPacked::ReadBits(uint32_t start, uint32_t bits, uint8_t* out, uint32_t outSize) const {
    uint32_t byte = start >> 3;
    uint8_t shift = start & 7;
    uint32_t next = byte + 1;
    uint32_t full = 0;

    if (bits >> 3) {
        full = bits >> 3;

        for (uint32_t i = 0; i < full; i++) {
            if (shift) {
                out[i] = (this->m_data[byte] >> shift) | (this->m_data[next] << (8 - shift));
            } else {
                out[i] = this->m_data[byte];
            }

            byte++;
            next++;
        }
    }

    uint8_t rest = bits & 7;

    if (rest) {
        uint8_t value = this->m_data[byte] >> shift;
        out[full] = value;

        if (8 - shift < rest) {
            out[full] = value | (this->m_data[next] << (8 - shift));
        }

        out[full] &= (1 << rest) - 1;
    }
}

// OFFSET: 0x007AAC60/0x1404A0FB0
bool System_SFile::BitPacked::WriteBits(uint32_t start, uint32_t bits, const uint8_t* in, uint32_t inSize) {
    if (start + bits > this->m_bitCount) {
        return false;
    }

    uint8_t shift = start & 7;
    uint32_t byte = start >> 3;
    uint32_t next = byte + 1;
    uint32_t full = 0;

    if (bits >> 3) {
        full = bits >> 3;

        for (uint32_t i = 0; i < full; i++) {
            if (shift) {
                uint8_t mask = (1 << shift) - 1;
                this->m_data[byte] &= mask;
                this->m_data[byte] |= in[i] << shift;
                this->m_data[next] &= ~mask;
                this->m_data[next] |= in[i] >> (8 - shift);
            } else {
                this->m_data[byte] = in[i];
            }

            byte++;
            next++;
        }
    }

    uint32_t rest = bits & 7;

    if (rest) {
        uint8_t value = in[full];
        uint32_t end = shift + rest;

        if (end > 8) {
            this->m_data[byte] &= (1 << shift) - 1;
            this->m_data[byte] |= value << shift;
            this->m_data[next] &= ~((1 << (end - 8)) - 1);
            this->m_data[next] |= value >> (8 - shift);
            return true;
        }

        this->m_data[byte] &= ((1 << shift) - 1) | ~((1 << end) - 1);
        this->m_data[byte] |= value << shift;
    }

    return true;
}

// OFFSET: 0x007AADE0
void System_SFile::HashEntryTable::SetHashBits(int32_t bits) {
    this->m_hashBits = bits;
    this->m_hashMask = (bits == 64 ? 0 : static_cast<uint64_t>(1) << bits) - 1;
    this->m_hashTopBit = static_cast<uint64_t>(1) << (bits - 1);
}

// OFFSET: 0x007B0E60/0x1404A5C80
bool System_SFile::BlockEntryTable::UnpackEntry(uint32_t index, BLOCKENTRY* entry) {
    if (!this->m_entries) {
        return false;
    }

    memset(entry, 0, sizeof(BLOCKENTRY));

    uint32_t base = index * this->m_entryBits;
    this->m_entries->ReadBits(base + this->m_offsetBit, this->m_offsetBits, reinterpret_cast<uint8_t*>(&entry->m_offset), 8);
    this->m_entries->ReadBits(base + this->m_fileSizeBit, this->m_fileSizeBits, reinterpret_cast<uint8_t*>(&entry->m_fileSize), 8);
    this->m_entries->ReadBits(base + this->m_compressedSizeBit, this->m_compressedSizeBits, reinterpret_cast<uint8_t*>(&entry->m_compressedSize), 8);

    size_t flagCount = this->m_flagTable.size();

    if (!flagCount) {
        return false;
    }

    if (flagCount == 1) {
        entry->m_flags = this->m_flagTable[0];
    } else {
        this->m_entries->ReadBits(base + this->m_flagIndexBit, this->m_flagIndexBits, reinterpret_cast<uint8_t*>(&entry->m_flags), 4);

        if (entry->m_flags >= this->m_flagTable.size()) {
            return false;
        }

        entry->m_flags = this->m_flagTable[entry->m_flags];
    }

    if (this->m_tableFlags & 0x200) {
        uint8_t md5Check = 0;
        this->m_md5Checks->ReadBits(this->m_md5CheckBit + index * this->m_md5CheckEntryBits, this->m_md5CheckBits, &md5Check, 1);
        entry->m_md5Check = md5Check != 0;
    }

    return true;
}

// OFFSET: 0x007B0FC0
uint64_t System_SFile::HashEntryTable::CalculateHash(const char* name) {
    uint8_t buffer[260];
    uint32_t length = 0;

    if (name) {
        uint8_t* dst = buffer;

        do {
            char c = *name++;

            if (!c) {
                break;
            }

            if (static_cast<uint8_t>(c - 'A') > 25) {
                if (c == '/') {
                    c = '\\';
                }
            } else {
                c += 32;
            }

            *dst++ = c;
        } while (dst < buffer + 259);

        *dst = 0;
        length = static_cast<uint32_t>(dst - buffer);
    }

    uint32_t high = 1;
    uint32_t low = 2;
    Blizzard::Util::hashlittle2(buffer, length, &low, &high);

    return low + (static_cast<uint64_t>(high) << 32);
}

// OFFSET: 0x007B1060
uint32_t System_SFile::HashEntryTable::GetEntry(const uint64_t& hash, BlockEntryTable* blocks) {
    uint64_t key = (hash & this->m_hashMask) | this->m_hashTopBit;
    uint32_t start = static_cast<uint32_t>(key % this->m_slotCount);
    uint8_t tag = static_cast<uint8_t>(key >> (this->m_hashBits - 8));
    uint64_t check = key & (this->m_hashMask >> 8);
    uint32_t slot = start;

    do {
        uint8_t current = this->m_tags[slot];

        if (!current) {
            break;
        }

        if (current == tag) {
            uint32_t index = 0;
            this->m_indices->ReadBits(slot * this->m_entryBits, this->m_indexBits, reinterpret_cast<uint8_t*>(&index), 4);

            uint64_t stored = 0;
            blocks->m_hashes->ReadBits(index * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<uint8_t*>(&stored), 8);

            if (stored == check) {
                return index;
            }
        }

        slot = (slot + 1) % this->m_slotCount;
    } while (slot != start);

    return -1;
}

// OFFSET: 0x007B8DA0/0x1404AFAC0
System_SFile::BitPacked::BitPacked(uint32_t bitCount, uint8_t fill) {
    this->m_data = static_cast<uint8_t*>(Blizzard::Memory::Allocate((bitCount + 7) / 8));
    memset(this->m_data, fill, (bitCount + 7) / 8);
    this->m_bitCount = bitCount;
}

// INLINED
System_SFile::BitPacked::~BitPacked() {
    Blizzard::Memory::Free(this->m_data);
}

// OFFSET: 0x007B9140/0x1404AFEF0
void System_SFile::BlockEntryTable::InitializeHashCheckTable(int32_t bits) {
    int32_t hashBits = bits - 8;

    if (hashBits == this->m_hashBits) {
        return;
    }

    if (hashBits > 0) {
        this->m_hashBits = hashBits;
        this->m_hashExtraBits = 0;
        this->m_hashEntryBits = hashBits;
    }

    delete this->m_hashes;
    this->m_hashes = new BitPacked(this->m_hashEntryBits * this->m_count, 0);
}

// OFFSET: 0x007B9390/0x1404B02C0
System_SFile::HashEntryTable::HashEntryTable(uint32_t count, int32_t hashBits) {
    this->unk_0004 = 0;
    this->m_indexBits = 0;
    this->m_slotCount = 100 * count / 75;
    this->m_entryBits = 0;
    this->m_tags = NULL;
    this->m_indices = NULL;
    this->m_count = count;

    uint32_t bits = 0;

    for (uint32_t i = count; i; i >>= 1) {
        bits++;
    }

    this->m_indexBits = bits;
    this->m_entryBits = this->m_indexBits;
    this->unk_0004 = 0;
    this->m_tags = static_cast<uint8_t*>(SMemAlloc(this->m_slotCount, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    this->SetHashBits(hashBits);
}

// OFFSET: 0x007B9410/0x1404B0380
System_SFile::HashEntryTable::~HashEntryTable() {
    if (this->m_indices) {
        delete this->m_indices;
        this->m_indices = NULL;
    }

    memset(this->m_tags, 0, this->m_slotCount);

    if (this->m_tags) {
        Blizzard::Memory::Free(this->m_tags);
        this->m_tags = NULL;
    }
}

// OFFSET: 0x007B9470
bool System_SFile::HashEntryTable::AddEntry(const char* name, uint32_t blockIndex, BlockEntryTable* blocks) {
    if (!this->m_indices) {
        this->m_indices = new BitPacked(this->m_slotCount * this->m_entryBits, 0xFF);
    }

    uint64_t key = (CalculateHash(name) & this->m_hashMask) | this->m_hashTopBit;
    uint32_t slot = static_cast<uint32_t>(key % this->m_slotCount);
    uint32_t bit = slot * this->m_entryBits;
    uint32_t start = slot;
    uint8_t tag = static_cast<uint8_t>(key >> (this->m_hashBits - 8));
    uint8_t* tags = this->m_tags;
    uint64_t check = key & (this->m_hashMask >> 8);

    if (tags[slot]) {
        for (;;) {
            uint32_t index = 0;
            this->m_indices->ReadBits(bit, this->m_indexBits, reinterpret_cast<uint8_t*>(&index), 4);

            uint64_t stored = 0;
            blocks->m_hashes->ReadBits(index * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<uint8_t*>(&stored), 8);

            if (tags[slot] == tag && stored == check) {
                return false;
            }

            slot = (slot + 1) % this->m_slotCount;

            if (slot == start) {
                return false;
            }

            bit = slot * this->m_entryBits;

            if (!tags[slot]) {
                break;
            }
        }
    }

    if (!this->m_indices->WriteBits(bit, this->m_indexBits, reinterpret_cast<const uint8_t*>(&blockIndex), 4)) {
        return false;
    }

    this->m_tags[slot] = tag;

    return blocks->m_hashes->WriteBits(blockIndex * blocks->m_hashEntryBits, blocks->m_hashBits, reinterpret_cast<const uint8_t*>(&check), 8);
}

// OFFSET: 0x007CC130
System_SFile::BlockEntryTable::~BlockEntryTable() {
    delete this->m_entries;
    this->m_entries = NULL;
    delete this->m_hashes;
    this->m_hashes = NULL;
    delete this->m_md5Checks;
    this->m_md5Checks = NULL;
}

// OFFSET: 0x007C2F70/0x1404BD420
System_SFile::ArchiveBuffers::~ArchiveBuffers() {
    memset(this->unk_0000.m_data, 0, this->unk_0000.m_count);
    memset(this->unk_000C.m_data, 0, this->unk_000C.m_count);
    memset(this->unk_0114.m_data, 0, this->unk_0114.m_count);
}

// OFFSET: 0x007D8870
System_SFile::ARCHIVEREC::~ARCHIVEREC() {
    delete this->m_blockEntryTable;
    delete this->m_hashEntryTable;

    Blizzard::Mopaq::HSFILERecord* file = this->m_file;

    if (file) {
        if (this->m_closeCallback) {
            char path[261];
            memset(path, 0, sizeof(path));
            Blizzard::String::Copy(path, file->m_handle ? file->m_handle->m_name : "", 261);

            if (file->m_handle && !file->m_handle->Release()) {
                file->m_handle = NULL;
            }

            delete file;
            ArchiveCheckpoint::DeleteRestorePoint(path);
        } else {
            if (file->m_handle && !file->m_handle->Release()) {
                file->m_handle = NULL;
            }

            delete file;
        }

        this->m_file = NULL;
    }

    delete this->m_listfile;
}

// OFFSET: 0x007DBD60
System_SFile::ARCHIVEREC::ARCHIVEREC() {
    this->m_sectorSize = 0x4000;
    this->unk_011C = 0;
    this->m_compressTables = false;
    this->unk_0121 = false;
    this->m_chunkSize = 0;
    this->m_setFileSize = true;
    this->m_minFileSize = 0;
    this->m_blockEntryTable = NULL;
    this->m_hashEntryTable = NULL;
    this->m_listfile = NULL;
    this->m_writeReservationEnd = -1;
    this->m_header.m_rawChunkSize = 0;
}

// OFFSET: 0x007F0800
bool Blizzard::Mopaq::SFileEnableDirectAccess(uint32_t enable) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
    System_MPQGlobals::GetMopaqGlobals()->m_directAccess = enable;
    return true;
}

// OFFSET: 0x007F0980/0x1404FD3F0
bool System_SFile::ARCHIVEREC::RemoveInternalArchiveRef() {
    bool result = true;

    if (this->m_internalRefs.Decrement()) {
        return true;
    }

    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);

    if (this->m_internalRefs.m_value) {
        Blizzard::String::QuickFormat<1024> message("Potential race condition in client code for archive \"%s\"!\n", this->m_name);
        fputs(message.m_buffer, stderr);
        return true;
    }

    this->m_internalRefs.m_value = 1;

    if (this->m_closeCallback && !this->m_closeCallback(this)) {
        result = false;
    }

    if (this->m_buffers) {
        result = false;
        delete this->m_buffers;
    }

    System_MPQGlobals::GetArchiveList()->erase(this);
    this->~ARCHIVEREC();
    Blizzard::Memory::Free(this);

    return result;
}

// OFFSET: 0x007F1210
bool System_SFile::OpenDirectPathAsArchive(const char* path, int32_t priority, uint32_t flags, ARCHIVEREC** result) {
    ARCHIVEREC* archive = NewZerofill<ARCHIVEREC>();
    archive->m_priority = priority;
    archive->m_id = s_nextArchiveId.Increment() - 1;
    archive->m_flags = flags;
    archive->m_externalRefs.Increment();
    archive->m_internalRefs.Increment();

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        System_MPQGlobals::GetArchiveList()->insert(archive);
    }

    archive->m_name[0] = '\0';
    archive->m_file = NULL;
    archive->m_parent = NULL;
    archive->unk_02E0 = 0;

    char directory[260];
    Blizzard::String::MakeBackslashPath(path, directory, sizeof(directory));

    if (directory[0]) {
        Blizzard::String::ForceTrailingSeparator(directory, sizeof(directory), '\\');
    }

    char absolute[260];
    Blizzard::File::MakeAbsolutePath(directory, absolute, sizeof(absolute), false);

    archive->m_directPath = absolute;
    *result = archive;

    return true;
}

// OFFSET: 0x007F19A0
void System_SFile::ConvertRelativePathName(const char* path, char* result) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);

    char joined[260];
    Blizzard::String::JoinPath(joined, sizeof(joined), System_MPQGlobals::GetMopaqGlobals()->m_basePath, path);
    Blizzard::File::MakeAbsolutePath(joined, result, 260, false);
}

// OFFSET: 0x007F0860/0x1404FD270
bool Blizzard::Mopaq::SFileSetBasePath(const char* path) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetMopaqGlobalsLock()->m_mutex);
    System_MPQGlobals::MopaqGlobals* globals = System_MPQGlobals::GetMopaqGlobals();

    if (!*path) {
        if (!Blizzard::Process::GetProcessDirectory(globals->m_basePath, sizeof(globals->m_basePath))) {
            globals->m_basePath[0] = '\0';
        }

        return true;
    }

    if (strlen(path) + 1 >= sizeof(globals->m_basePath)) {
        System_SFile::s_lastError = 161;
        Blizzard::String::QuickFormat<1024> message("SFileSetBasePath - SFILE_ERROR_BAD_PATHNAME");
        Blizzard::File::AddToLastErrorStack(161, message.m_buffer, 4);
        return false;
    }

    Blizzard::String::Copy(globals->m_basePath, path, sizeof(globals->m_basePath));
    Blizzard::String::ForceTrailingSeparator(globals->m_basePath, sizeof(globals->m_basePath), '\0');

    return true;
}

// OFFSET: 0x007F10F0/0x1404FDCD0
System_SFile::ARCHIVEREC* System_SFile::GetArchivePtr(ARCHIVEREC* handle, bool addRef) {
    ARCHIVEREC* archive = NULL;

    if (!handle) {
        return NULL;
    }

    bool threadSafe = s_threadSafe;
    Blizzard::Lock::RMutex* mutex = System_MPQGlobals::GetArchiveLock();

    if (threadSafe) {
        Blizzard::Lock::MutexEnter(mutex->m_mutex);
    }

    System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();
    System_MPQGlobals::ArchiveList::iterator it = list->find(handle);

    if (it == list->end()) {
        s_lastError = 6;
        Blizzard::String::QuickFormat<1024> message("GetArchivePtr - SFILE_ERROR_INVALID_HANDLE");
        Blizzard::File::AddToLastErrorStack(6, message.m_buffer, 4);
    } else {
        archive = *it;

        if (addRef) {
            archive->m_internalRefs.Increment();
        }
    }

    if (threadSafe) {
        Blizzard::Lock::MutexLeave(mutex->m_mutex);
    }

    return archive;
}

// PROVEN: 717688b9 IDENTICAL/DIFFERENT
// OFFSET: 0x007F13D0/0x1404FE0C0
bool Blizzard::Mopaq::SFileCloseArchive(System_SFile::ARCHIVEREC* handle) {
    bool result = true;
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, false);

    if (!archive) {
        return false;
    }

    if (archive->m_parent) {
        System_SFile::ARCHIVEREC* parent = System_SFile::GetArchivePtr(archive->m_parent, false);
        result = parent ? parent->RemoveInternalArchiveRef() : false;
    }

    archive->m_externalRefs.Decrement();

    return archive->RemoveInternalArchiveRef() && result;
}

// OFFSET: 0x007B16E0
static bool ValidateHeaderMD5(const System_SFile::MPQHEADER* header) {
    Blizzard::Util::MD5 md5;
    md5.Process(header, offsetof(System_SFile::MPQHEADER, m_headerMd5));

    Blizzard::Util::Digest<16> digest;
    md5.Finish(digest.m_data);

    return digest == header->m_headerMd5;
}

// OFFSET: 0x007B1740
static void FixupOldHeader(System_SFile::MPQHEADER* header) {
    if (header->m_formatVersion == 0) {
        header->m_hiBlockTablePos = 0;
        header->m_hashTablePosHi = 0;
        header->m_blockTablePosHi = 0;
    }

    if (header->m_formatVersion >= 2) {
        return;
    }

    int64_t hashPos = (static_cast<int64_t>(header->m_hashTablePosHi) << 32) | header->m_hashTablePos;
    int64_t blockPos = (static_cast<int64_t>(header->m_blockTablePosHi) << 32) | header->m_blockTablePos;

    header->m_rawChunkSize = 0;
    header->m_hashTableSize64 = blockPos - hashPos;

    if (header->m_hiBlockTablePos) {
        header->m_blockTableSize64 = header->m_hiBlockTablePos - blockPos;
        header->m_hiBlockTableSize64 = 2 * header->m_blockTableSize;
        header->m_archiveSize64 = header->m_hiBlockTablePos + header->m_hiBlockTableSize64;
    } else {
        header->m_blockTableSize64 = 16 * header->m_blockTableSize;
        int64_t blockEnd = blockPos + static_cast<int64_t>(header->m_blockTableSize64);

        if (blockEnd >= static_cast<int64_t>(header->m_archiveSize)) {
            header->m_archiveSize64 = header->m_archiveSize;
        } else {
            header->m_archiveSize64 = blockEnd;
        }
    }
}

// OFFSET: 0x007B1840
static bool ValidateHeader(System_SFile::ARCHIVEREC* archive, int64_t fileSize) {
    System_SFile::MPQHEADER* header = &archive->m_header;

    if (header->m_magic != 0x1A51504D) {
        return false;
    }

    bool valid;

    switch (header->m_formatVersion) {
    case 0:
        valid = header->m_headerSize == 32;
        break;
    case 1:
        valid = header->m_headerSize == 44;
        break;
    case 2:
        valid = header->m_headerSize == 68 || header->m_headerSize == 44;
        header->m_formatVersion = 1;
        break;
    default:
        valid = header->m_headerSize >= 0xD0;
        break;
    }

    if (!valid) {
        return false;
    }

    FixupOldHeader(header);

    int64_t available = fileSize;
    int64_t hashPos = (static_cast<int64_t>(header->m_hashTablePosHi) << 32) | header->m_hashTablePos;
    int64_t blockPos = (static_cast<int64_t>(header->m_blockTablePosHi) << 32) | header->m_blockTablePos;
    int64_t hiBlockPos = static_cast<int64_t>(header->m_hiBlockTablePos);

    if (!hashPos) {
        return false;
    }

    if (hashPos > available) {
        return false;
    }

    if (blockPos < hashPos) {
        return false;
    }

    if (blockPos > hashPos + 16 * header->m_hashTableSize) {
        return false;
    }

    if (blockPos > available) {
        return false;
    }

    if (hiBlockPos && hiBlockPos < blockPos) {
        return false;
    }

    if (hiBlockPos > blockPos + 16 * header->m_blockTableSize
        || hiBlockPos > available
        || hiBlockPos + 2 * header->m_blockTableSize > available
        || (header->m_formatVersion >= 2 && available < static_cast<int64_t>(header->m_archiveSize64))) {
        return false;
    }

    archive->m_archiveEnd = archive->m_archiveOffset + header->m_archiveSize64;

    return true;
}

// INLINED
static bool HasHttpError() {
    System_File::FileError& fileError = System_File::s_error;
    Blizzard::Debug::ErrorStackRecord* stack = fileError.m_errorStack;

    return stack && stack->FindCategory(21);
}

// OFFSET: 0x007F8620/0x140507C60
bool System_SFile::s_OpenArchive(ARCHIVEREC* archive, uint32_t flags, ARCHIVEREC** result) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);

    int64_t bufferPos = 0;
    uint32_t bufferSize = 0;
    uint32_t readSize = 0;
    int64_t candidate = 0;
    bool haveCandidate = false;
    bool scan = (flags & 0x20) != 0;
    uint32_t repairs = 0;
    int64_t fileSize = archive->m_file->GetFileSize();
    uint8_t buffer[4096];
    uint8_t* header;
    bool valid;
    const char* error;

    for (;;) {
        for (;;) {
            if (archive->m_archiveOffset >= bufferPos + bufferSize) {
                bufferPos = archive->m_archiveOffset;
                readSize = 0;

                int32_t read = 0;

                if (archive->m_file->m_handle && archive->m_file->Read(buffer, archive->m_archiveOffset, sizeof(buffer), &read)) {
                    readSize = read;

                    if (readSize >= 0x2C && archive->m_archiveOffset + 44 <= fileSize) {
                        bufferSize = readSize;
                        goto parse;
                    }
                } else if (HasHttpError()) {
                    s_lastError = STORM_ERROR_NETWORK_READ;

                    Blizzard::String::QuickFormat<1024> message("OpenArchive - HTTP error - %s", archive->m_name);
                    Blizzard::File::AddToLastErrorStack(STORM_ERROR_NETWORK_READ, message.m_buffer, 4);

                    goto fail;
                }

                if (!(flags & 0x20) || !haveCandidate) {
                    if (HasHttpError()) {
                        s_lastError = STORM_ERROR_NETWORK_READ;
                        error = "OpenArchive -  MPQ signature not found and HTTP error - %s";
                    } else {
                        error = "OpenArchive - MPQ signature not found - %s";
                        s_lastError = STORM_ERROR_NOT_ARCHIVE;
                    }

                    goto report;
                }

                archive->m_archiveOffset = candidate;
                bufferPos = 0;
                bufferSize = 0;
                scan = false;
                continue;
            }

        parse:
            header = buffer + static_cast<uint32_t>(archive->m_archiveOffset - bufferPos);
            uint32_t magic = *reinterpret_cast<uint32_t*>(header);
            valid = false;

            if (magic == 0x1A51504D) {
                break;
            }

            if (magic != 0x1B51504D) {
                goto next;
            }

            archive->m_userDataOffset = archive->m_archiveOffset + 12;
            archive->m_userDataSize = reinterpret_cast<uint32_t*>(header)[1];
            archive->m_archiveOffset += reinterpret_cast<uint32_t*>(header)[2];
        }

        {
            MPQHEADER copy;
            memcpy(&copy, header, sizeof(copy));

            if (copy.m_formatVersion > 2 && copy.m_headerSize >= 0xD0) {
                MPQHEADER check;
                memcpy(&check, header, sizeof(check));

                if (!ValidateHeaderMD5(&check)) {
                    Blizzard::String::QuickFormat<1024> message("%s - Failed to validate archive header hash - %s", "System_SFile::s_OpenArchive", archive->m_name);
                    Blizzard::File::AddToLastErrorStack(STORM_ERROR_NOT_ARCHIVE, message.m_buffer, 4);

                    if (System_MPQGlobals::GetMopaqGlobals()->m_disableRepair || !Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMPQHeaderV3Error(archive->m_file, header, bufferPos, readSize)) {
                        valid = false;
                        goto next;
                    }
                }
            }

            memcpy(&archive->m_header, header, sizeof(MPQHEADER));

            valid = ValidateHeader(archive, fileSize);

            if (!valid) {
                goto next;
            }

            haveCandidate = true;
            candidate = archive->m_archiveOffset;

            if (!scan) {
                break;
            }

            archive->m_archiveOffset = (archive->m_archiveEnd + 511) & ~static_cast<int64_t>(511);
        }

    next:
        if (flags & 8) {
            if (repairs++ >= 3) {
                goto missing;
            }

            Blizzard::Mopaq::HSFILERecord* file = archive->m_file;
            Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected Mopaq signature error: offset = %lld, length = %d\n", bufferPos, 4096);

            Blizzard::Mopaq::MpqHandleRecord* handle = file->m_handle;
            Blizzard::File::StreamRecord* stream = handle ? handle->m_file : NULL;

            if (!stream || !stream->m_streaming || !stream->m_streaming->m_unit) {
                Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Could not repair because the storage file is not streamed.\n");
                goto missing;
            }

            if (!Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleArchiveSignatureError(file, bufferPos)) {
                goto missing;
            }

            bufferPos = 0;
            archive->m_archiveOffset = 0;
            bufferSize = 0;
        } else if (!valid) {
            archive->m_archiveOffset += 512;
        }
    }

    archive->m_sectorSize = 512 << archive->m_header.m_sectorSizeShift;

    if (flags & 0x4000) {
        archive->m_setFileSize = false;
    }

    archive->m_attributeFlags = 5;

    {
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        archive->m_compression.push_back(algorithm);
    }

    archive->m_patchMetadataLoaded = false;

    {
        Blizzard::Lock::ScopedMutex listLock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        System_MPQGlobals::GetArchiveList()->insert(archive);
    }

    archive->m_file->m_handle->m_baseOffset = archive->m_archiveOffset;

    if (!(flags & 0x800) || archive->InitializeTables()) {
        *result = archive;
        return true;
    }

    if (HasHttpError()) {
        s_lastError = STORM_ERROR_NETWORK_READ;
        error = "OpenArchive - INIT TABLE ERROR - %s with HTTP error";
    } else {
        s_lastError = STORM_ERROR_NOT_ARCHIVE;
        error = "OpenArchive - INIT TABLE ERROR - %s";
    }

    goto report;

missing:
    if (HasHttpError()) {
        s_lastError = STORM_ERROR_NETWORK_READ;
        error = "OpenArchive - MPQ signature missing from the beginning of %s with HTTP error";
    } else {
        s_lastError = STORM_ERROR_NOT_ARCHIVE;
        error = "OpenArchive - MPQ signature missing from the beginning of %s";
    }

report:
    {
        Blizzard::String::QuickFormat<1024> message(error, archive->m_name);
        Blizzard::File::AddToLastErrorStack(STORM_ERROR_NOT_ARCHIVE, message.m_buffer, 4);
    }

fail:
    archive->m_externalRefs.Decrement();
    archive->RemoveInternalArchiveRef();

    return false;
}

// OFFSET: 0x007F8D40
bool Blizzard::Mopaq::SFileOpenArchive(HSFILERecord* file, const char* name, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result) {
    System_SFile::ARCHIVEREC* archive = NewZerofill<System_SFile::ARCHIVEREC>();
    archive->m_priority = priority;
    archive->m_id = System_SFile::s_nextArchiveId.Increment() - 1;
    archive->m_flags = flags;
    Blizzard::String::Copy(archive->m_name, name, sizeof(archive->m_name));

    HSFILERecord* copy = new HSFILERecord(*file);

    archive->m_file = copy;
    archive->m_externalRefs.Increment();
    archive->m_internalRefs.Increment();
    archive->m_archiveOffset = 0;
    archive->m_archiveEnd = 0;
    archive->unk_02E0 = 0;

    return System_SFile::s_OpenArchive(archive, flags, result);
}

// OFFSET: 0x007F8E10/0x140508670
bool Blizzard::Mopaq::SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, System_SFile::ARCHIVEREC** result) {
    const char* name = path;
    char converted[260];

    *result = NULL;

    int32_t exists = Blizzard::File::Exists(System_File::Stacked::s_manager, path);

    if (!exists && !Blizzard::File::GetRootChars(path)) {
        System_SFile::ConvertRelativePathName(path, converted);
        name = converted;
        exists = Blizzard::File::Exists(System_File::Stacked::s_manager, converted);
    }

    if (exists == 1) {
        HSFILERecord* file = NULL;
        System_Mopaq::OpenDiskFile(name, (flags & 0x8000) | (((flags & 0x10) | 0x24) >> 2), name, NULL, &file);

        if (file) {
            ArchiveCheckpoint::ApplyRestorePoint(file);
            bool opened = SFileOpenArchive(file, name, priority, flags, result);

            if (file->m_handle && !file->m_handle->Release()) {
                file->m_handle = NULL;
            }

            delete file;
            return opened;
        }

        System_File::FileError& fileError = System_File::s_error;

        if (fileError.m_errorStack->FindCategoryCode(1, 4)) {
            System_SFile::s_lastError = 5;
            Blizzard::String::QuickFormat<1024> message("SFileOpenArchive - STORM_ERROR_ACCESS_DENIED - %s", name);
            Blizzard::File::AddToLastErrorStack(2, message.m_buffer, 4);
            return false;
        }

        if (fileError.m_errorStack->FindCategoryCode(22, 11)) {
            System_SFile::s_lastError = 112;
            Blizzard::String::QuickFormat<1024> message("SFileOpenArchive - STORM_ERROR_DISK_FULL - %s", name);
            Blizzard::File::AddToLastErrorStack(2, message.m_buffer, 4);
            return false;
        }
    } else if (exists == 2) {
        return System_SFile::OpenDirectPathAsArchive(name, priority, flags, result);
    }

    System_SFile::s_lastError = 2;
    Blizzard::String::QuickFormat<1024> message("SFileOpenArchive - SFILE_ERROR_FILE_NOT_FOUND - %s", name);
    Blizzard::File::AddToLastErrorStack(2, message.m_buffer, 4);

    return false;
}

// INLINED
void System_SFile::BlockEntryTable::UpdateLayout() {
    this->m_entryBits = this->m_offsetBits;
    this->m_offsetBit = 0;
    this->m_fileSizeBit = this->m_offsetBits;
    this->m_entryBits += this->m_fileSizeBits;
    this->m_compressedSizeBit = this->m_fileSizeBit + this->m_fileSizeBits;
    this->m_entryBits += this->m_compressedSizeBits;
    this->m_flagIndexBit = this->m_compressedSizeBit + this->m_compressedSizeBits;
    this->m_entryBits += this->m_flagIndexBits;
    this->m_md5Bit = this->m_flagIndexBit + this->m_flagIndexBits;
    this->m_entryBits += this->m_md5Bits;
}

// INLINED
static uint32_t BitLength(uint64_t value) {
    uint32_t bits = 0;

    for (uint32_t high = static_cast<uint32_t>(value >> 32); high; high >>= 1) {
        bits++;
    }

    if (bits) {
        return bits + 32;
    }

    for (uint32_t low = static_cast<uint32_t>(value); low; low >>= 1) {
        bits++;
    }

    return bits;
}

// OFFSET: 0x007B8F20
void System_SFile::BlockEntryTable::SetLayout(BLOCKENTRY& maxima) {
    this->m_offsetBits = BitLength(maxima.m_offset);
    this->UpdateLayout();

    this->m_fileSizeBits = BitLength(maxima.m_fileSize);
    this->UpdateLayout();

    this->m_compressedSizeBits = BitLength(maxima.m_compressedSize);
    this->UpdateLayout();

    uint32_t flagCount = static_cast<uint32_t>(this->m_flagTable.size());

    if (flagCount > 1) {
        uint32_t bits = 0;

        for (uint32_t value = flagCount; value; value >>= 1) {
            bits++;
        }

        this->m_flagIndexBits = bits;
        this->UpdateLayout();
    }

    if (this->m_tableFlags & 0x200) {
        this->m_md5CheckBits = 1;
        this->m_md5CheckBit = 0;
        this->m_md5CheckEntryBits = 1;
        this->m_md5Checks = new BitPacked(this->m_count * this->m_md5CheckEntryBits, 0);
    }
}

// OFFSET: 0x007B8DF0
bool System_SFile::BlockEntryTable::PackEntry(uint32_t index, const BLOCKENTRY* entry) {
    if (!this->m_entries) {
        this->m_entries = new BitPacked(this->m_entryBits * this->m_count, 0);
    }

    uint32_t base = index * this->m_entryBits;

    if (!this->m_entries->WriteBits(base + this->m_offsetBit, this->m_offsetBits, reinterpret_cast<const uint8_t*>(&entry->m_offset), 8)
        || !this->m_entries->WriteBits(base + this->m_fileSizeBit, this->m_fileSizeBits, reinterpret_cast<const uint8_t*>(&entry->m_fileSize), 8)
        || !this->m_entries->WriteBits(base + this->m_compressedSizeBit, this->m_compressedSizeBits, reinterpret_cast<const uint8_t*>(&entry->m_compressedSize), 8)) {
        return false;
    }

    if (this->m_flagTable.size() <= 1) {
        return true;
    }

    uint32_t flagIndex = 0;

    if (this->m_flagTable.size()) {
        while (flagIndex < this->m_flagTable.size() && this->m_flagTable[flagIndex] != entry->m_flags) {
            flagIndex++;
        }
    }

    if (flagIndex == this->m_flagTable.size()) {
        return false;
    }

    return this->m_entries->WriteBits(base + this->m_flagIndexBit, this->m_flagIndexBits, reinterpret_cast<const uint8_t*>(&flagIndex), 4);
}

// INLINED
bool System_SFile::BlockEntryTable::SetMD5Check(uint32_t index, bool enabled) {
    if (!(this->m_tableFlags & 0x200)) {
        return false;
    }

    uint8_t bit = enabled;
    return this->m_md5Checks->WriteBits(this->m_md5CheckBit + index * this->m_md5CheckEntryBits, this->m_md5CheckBits, &bit, 1);
}

// PROVEN: 4421cabe IDENTICAL/UNPROVEN
// OFFSET: 0x007C2EC0/0x1404BD340
void System_SFile::ARCHIVEREC::PackBlockTable() {
    uint32_t index = 0;

    for (std::vector<BLOCKENTRY>::iterator it = this->m_blockTable.begin(); it != this->m_blockTable.end(); ++it) {
        this->m_blockEntryTable->PackEntry(index++, &*it);
    }
}

// OFFSET: 0x007D81B0
System_SFile::BlockEntryTable::BlockEntryTable(uint32_t count, uint32_t flags) {
    this->m_entryBits = 0;
    this->m_offsetBit = 0;
    this->m_fileSizeBit = 0;
    this->m_compressedSizeBit = 0;
    this->m_flagIndexBit = 0;
    this->m_md5Bit = 0;
    this->m_offsetBits = 0;
    this->m_fileSizeBits = 0;
    this->m_compressedSizeBits = 0;
    this->m_flagIndexBits = 0;
    this->m_md5Bits = 0;
    this->m_hashEntryBits = 0;
    this->m_hashExtraBits = 0;
    this->m_hashBits = 0;
    this->m_md5CheckEntryBits = 0;
    this->m_md5CheckBit = 0;
    this->m_md5CheckBits = 0;
    this->m_md5Checks = NULL;
    this->m_hashes = NULL;
    this->m_entries = NULL;
    this->m_count = count;
    this->m_tableFlags = flags;

    if (flags & 0x80) {
        this->m_crcs.resize(this->m_count, 0);
    }

    if (this->m_tableFlags & 0x40) {
        this->m_md5s.resize(this->m_count);
    }

    if (this->m_tableFlags & 0x100) {
        this->m_fileTimes.resize(this->m_count, 0);
    }

    if (this->m_tableFlags & 0x1000) {
        this->m_patchBits.resize((this->m_count + 7) >> 3, 0);
    }
}

// OFFSET: 0x007D8300
void System_SFile::BlockEntryTable::AddFlags(uint32_t flags) {
    for (std::vector<uint32_t>::iterator it = this->m_flagTable.begin(); it != this->m_flagTable.end(); ++it) {
        if (*it == flags) {
            return;
        }
    }

    this->m_flagTable.push_back(flags);
}

// OFFSET: 0x007D83F0
System_SFile::BLOCKENTRY* System_SFile::BlockEntryTable::GetMaxima(BLOCKENTRY* maxima, const std::vector<BLOCKENTRY>& entries) {
    maxima->m_offset = 0;
    maxima->m_compressedSize = 0;
    maxima->m_fileSize = 0;
    maxima->m_md5Check = false;

    for (std::vector<BLOCKENTRY>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        if (it->m_offset > maxima->m_offset) {
            maxima->m_offset = it->m_offset;
        }

        if (it->m_compressedSize > maxima->m_compressedSize) {
            maxima->m_compressedSize = it->m_compressedSize;
        }

        if (it->m_fileSize > maxima->m_fileSize) {
            maxima->m_fileSize = it->m_fileSize;
        }

        this->AddFlags(it->m_flags);
    }

    return maxima;
}

// OFFSET: 0x007D8B20
void System_SFile::ARCHIVEREC::ConvertToNewBlockTableFormat(bool clear) {
    this->m_blockEntryTable = new BlockEntryTable(static_cast<uint32_t>(this->m_blockTable.size()), this->m_flags);

    BLOCKENTRY maxima;
    this->m_blockEntryTable->GetMaxima(&maxima, this->m_blockTable);
    this->m_blockEntryTable->SetLayout(maxima);
    this->PackBlockTable();

    if (clear) {
        std::vector<BLOCKENTRY>().swap(this->m_blockTable);
    }
}

// OFFSET: 0x007D8E70
bool System_SFile::ClearTables(ARCHIVEREC* archive, uint32_t hashCount) {
    std::vector<BLOCKENTRY>().swap(archive->m_blockTable);
    delete archive->m_blockEntryTable;
    delete archive->m_hashEntryTable;
    archive->m_header.m_blockTableSize = 0;

    HASHENTRY empty;
    memset(&empty, 0xFF, sizeof(empty));

    std::vector<HASHENTRY>().swap(archive->m_hashTable);
    archive->m_hashTable.insert(archive->m_hashTable.end(), hashCount, empty);
    archive->m_header.m_hashTableSize = hashCount;
    archive->m_blockKeys.clear();
    archive->m_blockKeysBuilt = true;
    archive->m_tablesInitialized = true;

    return true;
}

// PROVEN: 2def239b IDENTICAL/DIFFERENT
// OFFSET: 0x007CC320/0x1404C8750
void System_SFile::ClearNewBlockHashTables(ARCHIVEREC* archive) {
    delete archive->m_blockEntryTable;
    delete archive->m_hashEntryTable;
}

// PROVEN: 9d2f041b IDENTICAL/NONE
// OFFSET: 0x007F2300
System_SFile::ArchivePtr::ArchivePtr(ARCHIVEREC* handle) {
    this->m_archive = GetArchivePtr(handle, true);
    this->m_locked = false;
}

// OFFSET: 0x007F2390/0x1404FF4E0
System_SFile::ArchivePtr::~ArchivePtr() {
    if (this->m_locked && this->m_archive) {
        Blizzard::Lock::MutexLeave(this->m_archive->m_mutex.m_mutex);
    }

    this->m_locked = false;

    if (this->m_archive) {
        this->m_archive->RemoveInternalArchiveRef();
    }
}

// OFFSET: 0x007F5510/0x140503620
void System_SFile::ARCHIVEREC::ConvertToNewHashTableFormat(char* listfile, int32_t size, bool clear) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (!blocks) {
        return;
    }

    uint32_t count = blocks->m_count;
    blocks->InitializeHashCheckTable(64);
    this->m_hashEntryTable = new HashEntryTable(count, 64);

    if (this->FillNewHashTable(64, listfile, size)) {
        if (clear) {
            std::vector<HASHENTRY>().swap(this->m_hashTable);
        }
    } else {
        delete this->m_hashEntryTable;
        this->m_hashEntryTable = NULL;
    }
}

// OFFSET: 0x007D85C0
System_SFile::BlockEntryTable* System_SFile::BlockEntryTable::CreateBlockTableFromBuffer(const char* buffer, uint32_t size, uint32_t flags) {
    if (!buffer || size < sizeof(BlockTableHeader)) {
        return NULL;
    }

    BlockTableHeader header;
    memcpy(&header, buffer, sizeof(header));

    if (header.m_tableSize > size) {
        return NULL;
    }

    const char* data = buffer + sizeof(header);
    BlockEntryTable* table = new BlockEntryTable(header.m_count, flags);
    Blizzard::String::MemCopy(&table->m_entryBits, &header.m_entryBits, 0x2C);
    table->m_flagTable.resize(header.m_flagCount);

    for (uint32_t i = 0; i < header.m_flagCount; i++) {
        table->m_flagTable[i] = reinterpret_cast<const uint32_t*>(data)[i];
    }

    data += header.m_flagCount * sizeof(uint32_t);

    table->m_entries = new BitPacked(table->m_entryBits * table->m_count, 0);
    memcpy(table->m_entries->m_data, data, (table->m_entries->m_bitCount + 7) >> 3);
    data += (table->m_entries->m_bitCount + 7) >> 3;

    table->m_hashEntryBits = header.m_hashEntryBits;
    table->m_hashExtraBits = header.m_hashExtraBits;
    table->m_hashBits = header.m_hashBits;

    table->m_hashes = new BitPacked(table->m_hashEntryBits * table->m_count, 0);
    memcpy(table->m_hashes->m_data, data, header.m_hashesSize);

    if (flags & 0x200) {
        table->m_md5CheckBits = 1;
        table->m_md5CheckBit = 0;
        table->m_md5CheckEntryBits = table->m_md5CheckBits;
        table->m_md5Checks = new BitPacked(table->m_md5CheckEntryBits * table->m_count, 0);
    }

    return table;
}

// OFFSET: 0x007B9770
System_SFile::HashEntryTable* System_SFile::HashEntryTable::CreateHashEntryTableFromBuffer(const char* buffer, uint32_t size) {
    if (!buffer || size < sizeof(HashTableHeader)) {
        return NULL;
    }

    HashTableHeader header;
    memcpy(&header, buffer, sizeof(header));

    if (header.m_tableSize > size) {
        return NULL;
    }

    const char* data = buffer + sizeof(header);
    HashEntryTable* table = new HashEntryTable(header.m_count, header.m_hashBits);
    Blizzard::String::MemCopy(table, &header.m_entryBits, 0xC);
    Blizzard::String::MemCopy(table->m_tags, data, header.m_tagsSize);
    data += header.m_tagsSize;

    table->m_indices = new BitPacked(table->m_entryBits * table->m_slotCount, 0xFF);
    Blizzard::String::MemCopy(table->m_indices->m_data, data, header.m_indicesSize);

    return table;
}

// OFFSET: 0x007F0AA0
bool System_SFile::ARCHIVEREC::ReadTable(void* data, int64_t offset, uint32_t size, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Mopaq::HSFILERecord* file = this->m_file;

    if (!file->m_handle) {
        return false;
    }

    if (!file->Read(data, offset, size, NULL)) {
        return false;
    }

    if (this->m_header.m_rawChunkSize <= 0) {
        return true;
    }

    Blizzard::Util::MD5 hasher;
    hasher.Process(data, size);

    Blizzard::Util::Digest<16> digest;
    hasher.Finish(digest.m_data);

    if (digest == md5) {
        return true;
    }

    if (System_MPQGlobals::GetMopaqGlobals()->m_disableRepair) {
        return false;
    }

    return Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleLoadAndVerify(this->m_file, static_cast<uint8_t*>(data), offset, size, md5);
}

// OFFSET: 0x007F0B80
bool System_SFile::ARCHIVEREC::LoadOldTable(ARCHIVEREC* archive, void* table, Blizzard::Util::Buffer<4096, char>& buffer, uint32_t diskSize, uint32_t size, int64_t offset, uint32_t key, const Blizzard::Util::Digest<16>& md5) {
    if (diskSize == size) {
        bool result = archive->ReadTable(table, offset, size, md5);

        if (result) {
            System_MPQEncrypt::Decrypt(static_cast<uint32_t*>(table), size, key);
        }

        return result;
    }

    if (diskSize > buffer.m_count) {
        buffer.Resize(diskSize);
    }

    if (!archive->ReadTable(buffer.m_data, offset, diskSize, md5)) {
        return false;
    }

    System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(buffer.m_data), diskSize, key);

    return System_SComp::Decompress(table, &size, buffer.m_data, diskSize);
}

// OFFSET: 0x007F0C20
bool System_SFile::ARCHIVEREC::LoadOldHashTable(Blizzard::Util::Buffer<4096, char>& buffer) {
    uint32_t diskSize = this->m_header.m_blockTablePos - this->m_header.m_hashTablePos;
    uint32_t size = 16 * this->m_header.m_hashTableSize;
    int64_t offset = (static_cast<int64_t>(this->m_header.m_hashTablePosHi) << 32) | this->m_header.m_hashTablePos;

    if (!LoadOldTable(this, &this->m_hashTable[0], buffer, diskSize, size, offset, 0xC3AF3770, this->m_header.m_hashTableMd5)) {
        return false;
    }

    if (diskSize != size) {
        this->m_compressTables = true;
    }

    int32_t count = this->m_header.m_hashTableSize;

    for (int32_t i = 0; i < count; i++) {
        this->m_hashTable[i].unk_000B = 0;

        if (this->m_hashTable[i].m_blockIndex != -1 && this->m_hashTable[i].m_blockIndex >= this->m_header.m_blockTableSize) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x007F0D60
bool System_SFile::ARCHIVEREC::LoadOldBlockTable(Blizzard::Util::Buffer<4096, char>& buffer) {
    uint32_t diskSize;

    if (this->m_header.m_hiBlockTablePos) {
        diskSize = static_cast<uint32_t>(this->m_header.m_hiBlockTablePos) - this->m_header.m_blockTablePos;
    } else {
        diskSize = static_cast<uint32_t>(this->m_archiveEnd) - this->m_header.m_blockTablePos - static_cast<uint32_t>(this->m_archiveOffset);
    }

    uint32_t size = 16 * this->m_header.m_blockTableSize;
    int64_t offset = (static_cast<int64_t>(this->m_header.m_blockTablePosHi) << 32) | this->m_header.m_blockTablePos;

    if (!LoadOldTable(this, &this->m_blockTable[0], buffer, diskSize, size, offset, 0xEC83B3A3, this->m_header.m_blockTableMd5)) {
        return false;
    }

    const uint32_t* raw = reinterpret_cast<const uint32_t*>(reinterpret_cast<const uint8_t*>(&this->m_blockTable[0]) + 16 * this->m_header.m_blockTableSize - 16);
    BLOCKENTRY* first = &this->m_blockTable[0];

    for (BLOCKENTRY* entry = &this->m_blockTable[this->m_header.m_blockTableSize - 1]; entry >= first; entry--, raw -= 4) {
        uint32_t fileSize = raw[2];
        uint32_t flags = raw[3];
        uint32_t offsetLo = raw[0];
        entry->m_compressedSize = raw[1];
        entry->m_offset = offsetLo;
        entry->m_fileSize = fileSize;
        entry->m_flags = flags;
        entry->m_fileTime = 0;
        entry->m_crc = 0;
        entry->m_md5 = s_emptyMd5;
        entry->m_md5Check = false;
        entry->m_patch = false;
    }

    return true;
}

// OFFSET: 0x007F0F40
bool System_SFile::ARCHIVEREC::LoadTable(ARCHIVEREC* archive, Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, int64_t offset, uint32_t size, char** data, uint32_t* dataSize, uint32_t key, uint32_t signature, const Blizzard::Util::Digest<16>& md5) {
    if (size > buffer.m_count) {
        buffer.Resize(size);
    }

    if (!archive->ReadTable(buffer.m_data, offset, size, md5)) {
        return false;
    }

    ExtTableHeader header;
    memcpy(&header, buffer.m_data, sizeof(header));

    if (header.m_signature != signature) {
        return false;
    }

    uint32_t encryptedSize = size - sizeof(header);
    char* encrypted = buffer.m_data + sizeof(header);
    System_MPQEncrypt::Decrypt(reinterpret_cast<uint32_t*>(encrypted), encryptedSize, key);

    if (header.m_dataSize == encryptedSize) {
        *data = encrypted;
        *dataSize = encryptedSize;
        return true;
    }

    uint32_t outputSize = header.m_dataSize;

    if (header.m_dataSize > output.m_count) {
        output.Resize(header.m_dataSize);
    }

    if (!System_SComp::Decompress(output.m_data, &outputSize, encrypted, encryptedSize)) {
        return false;
    }

    *data = output.m_data;
    *dataSize = outputSize;

    return true;
}

// OFFSET: 0x007F10B0
bool System_SFile::ARCHIVEREC::LoadBlockEntryTable(Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, char** data, uint32_t* dataSize) {
    return LoadTable(this, buffer, output, this->m_header.m_betTablePos, static_cast<uint32_t>(this->m_header.m_betTableSize64), data, dataSize, 0xEC83B3A3, 0x1A544542, this->m_header.m_betTableMd5);
}

// PROVEN: 86eec255 IDENTICAL/NONE
// OFFSET: 0x007F1040
bool System_SFile::ARCHIVEREC::LoadHashEntryTable(Blizzard::Util::Buffer<4096, char>& buffer, Blizzard::Util::Buffer<4096, char>& output, char** data, uint32_t* dataSize) {
    uint32_t size = static_cast<uint32_t>(this->m_header.m_hetTableSize64);

    if (!LoadTable(this, buffer, output, this->m_header.m_hetTablePos, size, data, dataSize, 0xC3AF3770, 0x1A544548, this->m_header.m_hetTableMd5)) {
        return false;
    }

    if (*dataSize != size - sizeof(ExtTableHeader)) {
        this->m_compressTables = true;
    }

    return true;
}

// OFFSET: 0x007F8130/0x140507470
bool System_SFile::ARCHIVEREC::InitializeTables() {
    if (this->m_tablesInitialized) {
        return true;
    }

    ArchivePtr archive(this);

    if (archive.m_archive) {
        Blizzard::Lock::MutexEnter(archive.m_archive->m_mutex.m_mutex);
    }

    archive.m_locked = true;

    if (this->m_parent) {
        ArchivePtr parent(this->m_parent);
        this->m_tablesInitialized = parent.m_archive->InitializeTables();

        return this->m_tablesInitialized;
    }

    if (this->m_hashTable.size() || this->m_blockEntryTable) {
        return true;
    }

    if (!this->m_header.m_hashTableSize || !this->m_header.m_blockTableSize) {
        Blizzard::String::QuickFormat<1024> message("Unable to read archive hash/block table: \"%s\"\n", this->m_name);
        Blizzard::Debug::StderrOutput(message.m_buffer);
        ClearTables(this, 1);
        return false;
    }

    Blizzard::Util::Buffer<4096, char> buffers[2];
    this->m_hashTable.resize(this->m_header.m_hashTableSize);
    this->m_blockTable.resize(this->m_header.m_blockTableSize);

    if (this->m_header.m_formatVersion >= 2 && (this->m_flags & 0x410) == 0x400) {
        char* blockData = NULL;
        char* hashData = NULL;
        uint32_t hashSize = 0;
        uint32_t blockSize = 0;

        if (archive.m_archive->LoadBlockEntryTable(buffers[0], buffers[1], &blockData, &blockSize)) {
            this->m_blockEntryTable = BlockEntryTable::CreateBlockTableFromBuffer(blockData, blockSize, this->m_flags);

            if (this->m_blockEntryTable && archive.m_archive->LoadHashEntryTable(buffers[0], buffers[1], &hashData, &hashSize)) {
                this->m_hashEntryTable = HashEntryTable::CreateHashEntryTableFromBuffer(hashData, hashSize);

                if (this->m_hashEntryTable) {
                    std::vector<BLOCKENTRY>().swap(this->m_blockTable);
                    std::vector<HASHENTRY>().swap(this->m_hashTable);
                    goto attributes;
                }
            }
        }

        ClearNewBlockHashTables(this);
    }

    if (!archive.m_archive->LoadOldHashTable(buffers[0])) {
        Blizzard::String::QuickFormat<1024> message("Unable to read archive hash table: \"%s\"\n", this->m_name);
        Blizzard::Debug::StderrOutput(message.m_buffer);
        ClearTables(this, 1);
        return false;
    }

    if (!archive.m_archive->LoadOldBlockTable(buffers[0])) {
        Blizzard::String::QuickFormat<1024> message("Unable to read archive block table: \"%s\"\n", this->m_name);
        Blizzard::Debug::StderrOutput(message.m_buffer);
        ClearTables(this, 1);
        return false;
    }

    if (this->m_header.m_hiBlockTablePos) {
        HeapArray<uint16_t> hiBlockTable(this->m_header.m_blockTableSize);

        if (!this->ReadTable(hiBlockTable.m_data, this->m_header.m_hiBlockTablePos, 2 * this->m_header.m_blockTableSize, this->m_header.m_hiBlockTableMd5)) {
            Blizzard::String::QuickFormat<1024> message("Unable to read extended block table: \"%s\"\n", this->m_name);
            Blizzard::Debug::StderrOutput(message.m_buffer);
            ClearTables(this, 1);
            return false;
        }

        for (uint32_t i = 0; i < this->m_header.m_blockTableSize; i++) {
            this->m_blockTable[i].m_offset += static_cast<uint64_t>(hiBlockTable.m_data[i]) << 32;
        }
    }

    if ((this->m_flags & 0x400) && !(this->m_flags & 0x10)) {
        this->ConvertToNewBlockTableFormat(true);
        this->ConvertToNewHashTableFormat(NULL, 0, true);
    }

attributes:
    if (!System_MPQGlobals::GetMopaqGlobals()->m_disableAttributes && !this->ReadAdditionalAttributes()) {
        Blizzard::String::QuickFormat<1024> message("Unable to read extra attributes: \"%s\"\n", this->m_name);
        Blizzard::Debug::StderrOutput(message.m_buffer);
    }

    this->m_blockKeysBuilt = false;
    this->m_tablesInitialized = true;

    return true;
}

// OFFSET: 0x007B11D0/0x1404A5FD0
System_SFile::BLOCKENTRY System_SFile::ARCHIVEREC::GetBlockEntry(uint32_t index) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (!blocks) {
        return this->m_blockTable[index];
    }

    BLOCKENTRY entry;
    blocks->UnpackEntry(index, &entry);

    if (this->m_flags & 0x80) {
        entry.m_crc = this->m_blockEntryTable->m_crcs[index];
    }

    if (this->m_flags & 0x40) {
        entry.m_md5 = this->m_blockEntryTable->m_md5s[index];
    }

    if (this->m_flags & 0x100) {
        entry.m_fileTime = this->m_blockEntryTable->m_fileTimes[index];
    }

    if (this->m_flags & 0x1000) {
        static const uint8_t s_bits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
        entry.m_patch = (this->m_blockEntryTable->m_patchBits[index >> 3] & s_bits[index & 7]) != 0;
    }

    return entry;
}

// OFFSET: 0x007B19F0
uint32_t System_SFile::SearchHashTable(ARCHIVEREC* archive, const char* name, bool matchNeutral) {
    archive->InitializeTables();

    std::vector<HASHENTRY>& table = archive->m_hashTable;
    uint32_t mask = static_cast<uint32_t>(table.size()) - 1;
    uint16_t locale = archive->m_locale;
    uint8_t platform = archive->m_platform;
    uint32_t index = System_MPQEncrypt::Hash(name, 0) & mask;
    uint32_t found = -1;
    HASHENTRY* entry = &table[index];

    if (entry->m_blockIndex == -1) {
        return -1;
    }

    uint32_t hashA = System_MPQEncrypt::Hash(name, 1);
    uint32_t hashB = System_MPQEncrypt::Hash(name, 2);
    uint32_t start = index;

    while (true) {
        if (entry->m_hashA == hashA && entry->m_hashB == hashB && entry->m_blockIndex != -2) {
            if (entry->m_locale == locale && entry->m_platform == platform) {
                return index;
            }

            if (matchNeutral && (!entry->m_locale || entry->m_locale == locale) && (!entry->m_platform || entry->m_platform == platform)) {
                found = index;
            }
        }

        index = (index + 1) & mask;

        if (index == start) {
            return found;
        }

        entry = &table[index];

        if (entry->m_blockIndex == -1) {
            return found;
        }
    }
}

// OFFSET: 0x007F1A50
bool System_SFile::CheckFileExistsOnDisk(const char* name, uint32_t flags, char* result) {
    char buffer[260];

    if (!(flags & 3)) {
        return false;
    }

    if (!result) {
        result = buffer;
    }

    if (flags & 2) {
        ConvertRelativePathName(Blizzard::String::FindFilename(name), result);

        if (Blizzard::File::Exists(System_File::Stacked::s_manager, result) == 1) {
            return true;
        }
    }

    if (!(flags & 1)) {
        return false;
    }

    if (Blizzard::File::GetRootChars(name)) {
        Blizzard::String::Copy(result, name, 260);
    } else {
        ConvertRelativePathName(name, result);
    }

    return Blizzard::File::Exists(System_File::Stacked::s_manager, result) == 1;
}

// OFFSET: 0x007F2510/0x1404FF700
bool Blizzard::Mopaq::SFileSetChecksumFlags(System_SFile::ARCHIVEREC* handle, uint32_t flags) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (archive) {
        Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);
    }

    if (!archive) {
        return false;
    }

    archive->m_attributeFlags = flags;
    Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);
    archive->RemoveInternalArchiveRef();

    return true;
}

// OFFSET: 0x007B9FA0
static bool WriteUserDataHeader(System_SFile::ARCHIVEREC** archive, uint32_t userDataSize) {
    if (userDataSize && (*archive)->m_header.m_rawChunkSize > 0 && (*archive)->m_header.m_rawChunkSize < 512) {
        return false;
    }

    uint32_t size = userDataSize;
    (*archive)->m_userDataOffset = 12;
    (*archive)->m_userDataSize = userDataSize;
    (*archive)->m_archiveOffset = (userDataSize + 523) & 0xFFFFFE00;

    int64_t chunkSize = (*archive)->m_header.m_rawChunkSize;

    if (chunkSize > 0 && (*archive)->m_archiveOffset > chunkSize) {
        size = userDataSize + static_cast<uint32_t>(16 * (((*archive)->m_archiveOffset - 1) / chunkSize)) + 16;
        (*archive)->m_archiveOffset = (size + 523) & 0xFFFFFE00;
    }

    (*archive)->m_archiveEnd = (*archive)->m_archiveOffset + (*archive)->m_header.m_headerSize;

    uint32_t header[3];
    header[0] = 0x1B51504D;
    header[1] = size;
    header[2] = static_cast<uint32_t>((*archive)->m_archiveOffset);

    return (*archive)->m_file->Write(header, 0, 12);
}

// OFFSET: 0x007B9A90
bool System_SFile::ARCHIVEREC::WriteArchiveHeader() {
    uint32_t size;

    switch (this->m_header.m_formatVersion) {
    case 0:
        size = 32;
        break;
    case 1:
        size = 44;
        break;
    case 3:
        size = 208;
        break;
    default:
        size = 68;
        break;
    }

    MPQHEADER header;
    memcpy(&header, &this->m_header, sizeof(header));

    uint16_t shift = 0;

    for (uint32_t sectors = this->m_sectorSize >> 10; sectors; sectors >>= 1) {
        shift++;
    }

    header.m_magic = 0x1A51504D;
    header.m_headerSize = size;
    header.m_archiveSize = static_cast<uint32_t>(this->m_header.m_archiveSize64);
    header.m_sectorSizeShift = shift;
    header.m_hiBlockTablePos = this->m_header.m_hiBlockTablePos;
    header.m_archiveSize64 = this->m_header.m_archiveSize64;
    header.m_hetTablePos = this->m_header.m_hetTablePos;
    header.m_betTablePos = this->m_header.m_betTablePos;

    if (header.m_formatVersion >= 2) {
        Blizzard::Util::MD5 md5;
        md5.Process(&header, offsetof(MPQHEADER, m_headerMd5));
        md5.Finish(header.m_headerMd5.m_data);
    }

    return this->m_file->Write(&header, 0, size);
}

// PROVEN: 5b840513 IDENTICAL/DIFFERENT
// OFFSET: 0x007AAEE0/0x1404A6D50
static int64_t AddMD5Overhead(int32_t chunkSize, int64_t size) {
    if (chunkSize <= 0 || size <= 0) {
        return size;
    }

    return size + 16 * ((size - 1) / chunkSize + 1);
}

// OFFSET: 0x007B1EE0
int64_t System_SFile::ARCHIVEREC::RequestWriteReservation(int64_t size, bool raw) {
    Blizzard::Lock::MutexEnter(this->m_writeLock.m_mutex);

    int64_t offset = this->m_writeReservationEnd;
    int64_t reserved = raw ? size : AddMD5Overhead(this->m_header.m_rawChunkSize, size);
    this->m_writeReservationPending -= reserved;
    this->m_writeReservationEnd = offset + reserved;

    Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);

    return offset;
}

// OFFSET: 0x007B1E40/0x1404A6E90
void System_SFile::ARCHIVEREC::ReturnWriteReservation(int64_t offset, int64_t size, int64_t newSize, bool raw) {
    Blizzard::Lock::MutexEnter(this->m_writeLock.m_mutex);

    if (!raw) {
        int32_t chunkSize = this->m_header.m_rawChunkSize;
        size = AddMD5Overhead(chunkSize, size);
        newSize = AddMD5Overhead(chunkSize, newSize);
    }

    if (this->m_writeReservationEnd == size + offset) {
        this->m_writeReservationEnd = newSize + offset;
    }

    Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);
}

// OFFSET: 0x007B1F70/0x1404A6FF0
void System_SFile::ARCHIVEREC::CompleteWriteReservation(int64_t used, int64_t reserved) {
    int32_t chunkSize = this->m_header.m_rawChunkSize;
    this->m_writeReservationEnd += AddMD5Overhead(chunkSize, used);
    this->m_writeReservationPending -= AddMD5Overhead(chunkSize, reserved);

    Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);
}

// OFFSET: 0x007C3020
void System_SFile::ARCHIVEREC::InitializeWriteReservations(int32_t headerSize) {
    Blizzard::Lock::MutexEnter(this->m_writeLock.m_mutex);

    if (this->m_writeReservationEnd != -1) {
        Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);
        return;
    }

    this->m_writeReservationEnd = this->m_archiveOffset + headerSize;
    this->m_writeReservationPending = 0;

    if (this->m_blockEntryTable) {
        for (uint32_t i = 0; i < this->m_blockEntryTable->m_count; i++) {
            BLOCKENTRY entry;
            this->m_blockEntryTable->UnpackEntry(i, &entry);

            if ((entry.m_flags & 0x80000000) && (entry.m_flags & 0xFDFFFFFF) && entry.m_offset >= this->m_writeReservationEnd) {
                this->m_writeReservationEnd = entry.m_offset + AddMD5Overhead(this->m_header.m_rawChunkSize, entry.m_compressedSize);
            }
        }
    } else {
        for (std::vector<BLOCKENTRY>::iterator it = this->m_blockTable.begin(); it != this->m_blockTable.end(); ++it) {
            if ((it->m_flags & 0x80000000) && (it->m_flags & 0xFDFFFFFF) && it->m_offset >= this->m_writeReservationEnd) {
                this->m_writeReservationEnd = it->m_offset + AddMD5Overhead(this->m_header.m_rawChunkSize, it->m_compressedSize);
            }
        }
    }

    Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);
}

// OFFSET: 0x007F6B40
bool Blizzard::Mopaq::SFileCreateArchive(HSFILERecord* file, const char* name, int32_t priority, uint32_t userDataSize, System_SFile::ARCHIVEREC** result, const ArchiveOptions* options) {
    Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);

    System_SFile::ARCHIVEREC* archive = NewZerofill<System_SFile::ARCHIVEREC>();
    archive->m_priority = priority;
    archive->m_id = System_SFile::s_nextArchiveId.Increment() - 1;
    archive->m_flags = 16;
    archive->m_externalRefs.Increment();
    archive->m_internalRefs.Increment();

    {
        Blizzard::Lock::ScopedMutex listLock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        System_MPQGlobals::GetArchiveList()->insert(archive);
    }

    Blizzard::String::Copy(archive->m_name, name, sizeof(archive->m_name));

    HSFILERecord* copy = new HSFILERecord(*file);

    archive->m_file = copy;
    archive->m_header.m_formatVersion = 1;
    archive->unk_02E0 = 0;
    archive->m_header.m_rawChunkSize = 0;

    if (options) {
        memcpy(&archive->m_sectorSize, options, sizeof(ArchiveOptions));

        if (archive->m_chunkSize > 0) {
            archive->m_header.m_rawChunkSize = archive->m_chunkSize;
        }
    } else {
        archive->m_sectorSize = 4096;
        archive->unk_011C = 0;
        archive->m_compressTables = false;
        archive->unk_0121 = false;
        archive->m_chunkSize = 0;
        archive->m_setFileSize = true;
        archive->m_minFileSize = 0;
    }

    if (archive->unk_0121 || archive->m_chunkSize > 0) {
        archive->m_header.m_formatVersion = 3;
    } else {
        archive->m_header.m_formatVersion = 1;
    }

    archive->m_attributeFlags = 5;

    {
        CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        archive->m_compression.push_back(algorithm);
    }

    uint32_t headerSize = 44;

    if (archive->m_header.m_formatVersion > 1) {
        if (archive->m_header.m_formatVersion == 2) {
            return false;
        }

        headerSize = 208;
    }

    archive->m_header.m_headerSize = headerSize;

    if (userDataSize) {
        if (!WriteUserDataHeader(&archive, userDataSize)) {
            archive->m_externalRefs.Decrement();
            archive->RemoveInternalArchiveRef();
            return false;
        }
    } else {
        archive->m_archiveEnd = headerSize;
    }

    archive->m_header.m_archiveSize64 = archive->m_archiveEnd - archive->m_archiveOffset;
    archive->m_header.m_blockTablePos = static_cast<uint32_t>(archive->m_archiveEnd);
    archive->m_header.m_hashTablePos = static_cast<uint32_t>(archive->m_archiveEnd);
    archive->m_header.m_blockTablePosHi = 0;
    archive->m_header.m_hashTablePosHi = 0;
    archive->m_header.m_hiBlockTablePos = 0;
    archive->WriteArchiveHeader();
    ArchiveCheckpoint::CreateRestorePoint(archive);
    archive->m_closeCallback = System_SFile::ARCHIVEREC::WriteArchiveChanges;
    archive->m_listfile = new System_SFile::ListFile();
    archive->m_listfileLoaded = true;
    archive->m_listfileRefcount = 1;
    archive->m_patchMetadataLoaded = false;
    System_SFile::ClearTables(archive, 0x10000);
    archive->InitializeWriteReservations(headerSize);
    *result = archive;

    return true;
}

// OFFSET: 0x007F6E70/0x140505BB0
bool Blizzard::Mopaq::SFileCreateArchive(const char* path, int32_t priority, uint32_t userDataSize, System_SFile::ARCHIVEREC** result, const ArchiveOptions* options) {
    *result = NULL;

    HSFILERecord* file = NULL;
    System_Mopaq::CreateDiskFile(path, 0, path, NULL, &file);

    if (!file) {
        System_SFile::s_lastError = 110;
        Blizzard::String::QuickFormat<1024> message("SFileCreateArchive - SFILE_ERROR_INVALID_PARAMETERS - %s", path);
        Blizzard::File::AddToLastErrorStack(110, message.m_buffer, 4);
        return false;
    }

    bool created = SFileCreateArchive(file, path, priority, userDataSize, result, options);

    if (file->m_handle && !file->m_handle->Release()) {
        file->m_handle = NULL;
    }

    delete file;

    if (created) {
        return true;
    }

    System_SFile::s_lastError = 110;
    Blizzard::String::QuickFormat<1024> message("SFileCreateArchive - SFILE_ERROR_INVALID_PARAMETERS - %s", path);
    Blizzard::File::AddToLastErrorStack(110, message.m_buffer, 4);
    Blizzard::File::Delete(path);

    return false;
}

// OFFSET: 0x007BA0C0
static bool PrefixDirectPath(const std::string& directPath, const char* name, char* buffer, int32_t size) {
    int32_t length = 0;
    bool prefixed = false;

    if (!directPath.empty()) {
        length = Blizzard::String::Copy(buffer, directPath.c_str(), size);
        prefixed = true;
    }

    Blizzard::String::Copy(&buffer[length], name, size - length);

    return prefixed;
}

// OFFSET: 0x007F1AF0
static int32_t CheckDirectPath(System_SFile::ARCHIVEREC* archive, const char* name, bool prefixed, uint32_t flags, System_SFile::FileLookup* lookup) {
    if (archive->m_file) {
        if (prefixed && (flags & 1) && System_SFile::CheckFileExistsOnDisk(name, 1, lookup->m_path)) {
            return 3;
        }

        return 1;
    }

    if (!lookup->m_checkedDisk || prefixed) {
        if (System_SFile::CheckFileExistsOnDisk(name, 1, lookup->m_path)) {
            lookup->m_archive = archive;
            return 2;
        }

        if (!prefixed) {
            lookup->m_checkedDisk = true;
        }
    }

    return 0;
}

// OFFSET: 0x007F92D0
int32_t FindFileInOneArchive(System_SFile::ARCHIVEREC* archive, const char* name, System_SFile::FileLookup* lookup, const uint64_t& hash, const Blizzard::Util::Digest<16>& md5, uint32_t flags) {
    if (!(flags & 0x10) && (archive->m_externalRefs.m_value <= 0 || static_cast<int32_t>(archive->unk_02E0) > 0)) {
        return 0;
    }

    if (name && lookup->m_blockIndex == -1) {
        char path[260];
        bool prefixed = PrefixDirectPath(archive->m_directPath, name, path, sizeof(path));
        int32_t result = CheckDirectPath(archive, path, prefixed, flags, lookup);

        if (result != 1) {
            return result;
        }

        if (archive->m_parent) {
            archive = archive->m_parent;
        }

        archive->InitializeTables();

        if (archive->m_hashTable.size()) {
            uint32_t hashIndex = System_SFile::SearchHashTable(archive, path, true);

            if (hashIndex != -1) {
                lookup->m_blockIndex = archive->m_hashTable[hashIndex].m_blockIndex;
            }
        } else {
            uint64_t key = prefixed ? System_SFile::HashEntryTable::CalculateHash(path) : hash;
            lookup->m_blockIndex = archive->m_hashEntryTable->GetEntry(key, archive->m_blockEntryTable);
        }
    }

    bool deleted = false;

    if (lookup->m_blockIndex != -1) {
        System_SFile::BLOCKENTRY entry = archive->GetBlockEntry(lookup->m_blockIndex);
        deleted = (entry.m_flags & 0x2000000) != 0;

        if (md5 != System_SFile::s_emptyMd5 && entry.m_md5 != md5) {
            lookup->m_blockIndex = -1;
        }

        if (!(entry.m_flags & 0x80000000)) {
            lookup->m_blockIndex = -1;
        }
    }

    if (lookup->m_blockIndex == -1) {
        return 0;
    }

    lookup->m_archive = archive;

    return deleted ? 4 : 1;
}

// OFFSET: 0x007F9450/0x140508EB0
int32_t GetFileBlockEntry(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut, System_SFile::ARCHIVEREC** recordOut, uint32_t* blockIndex, char* path) {
    int32_t result = 0;

    if (archiveOut) {
        *archiveOut = NULL;
    }

    if (recordOut) {
        *recordOut = NULL;
    }

    if (blockIndex) {
        *blockIndex = -1;
    }

    System_SFile::ArchivePtr archive(handle);

    if (handle && !archive.m_archive) {
        Blizzard::String::QuickFormat<1024> message("%s - Null archive handle", "GetFileBlockEntry");
        Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);
        return 0;
    }

    System_SFile::FileLookup lookup;
    lookup.m_checkedDisk = false;
    lookup.m_archive = NULL;
    lookup.m_blockIndex = -1;
    lookup.m_path = path;
    int32_t found = 0;

    if (flags & 3) {
        lookup.m_checkedDisk = true;

        if (System_SFile::CheckFileExistsOnDisk(name, flags, path)) {
            found = 3;
        }

        result = found;
    }

    if (flags & 8) {
        return result;
    }

    if (!result) {
        uint64_t hash = System_SFile::HashEntryTable::CalculateHash(name);

        if (!archive.m_archive || (flags & 0x1000)) {
            bool threadSafe = System_SFile::s_threadSafe;
            Blizzard::Lock::RMutex* mutex = System_MPQGlobals::GetArchiveLock();

            if (threadSafe) {
                Blizzard::Lock::MutexEnter(mutex->m_mutex);
            }

            System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();
            System_MPQGlobals::ArchiveList::iterator it;

            if (flags & 0x1000) {
                it = list->find(archive.m_archive);

                if (it != list->end()) {
                    ++it;
                }
            } else {
                it = list->begin();
            }

            for (; it != list->end(); ++it) {
                found = FindFileInOneArchive(*it, name, &lookup, hash, md5, flags);

                if (found) {
                    handle = *it;
                    break;
                }
            }

            if (threadSafe) {
                Blizzard::Lock::MutexLeave(mutex->m_mutex);
            }
        } else {
            found = FindFileInOneArchive(archive.m_archive, name, &lookup, hash, md5, flags);
        }

        result = found;
    }

    if (result == 1 || result == 2 || result == 4) {
        if (recordOut) {
            lookup.m_archive->m_internalRefs.Increment();
            *recordOut = lookup.m_archive;
        }

        if (archiveOut) {
            *archiveOut = handle;
        }

        if (result == 1 && blockIndex) {
            *blockIndex = lookup.m_blockIndex;
        }
    }

    return result;
}

// OFFSET: 0x007AAE80/0x1404A1190
int32_t Blizzard::Mopaq::SFileFileExists(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    int32_t result = GetFileBlockEntry(handle, name, md5, flags, archiveOut, NULL, NULL, NULL);

    if (result == 4) {
        return 0;
    }

    return result;
}

// OFFSET: 0x007F9810
bool Blizzard::Mopaq::SFileGetFileDetails(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, System_SFile::ARCHIVEREC** archiveOut, System_SFile::ARCHIVEREC*& record, System_SFile::BLOCKENTRY& entry) {
    uint32_t blockIndex;

    if (GetFileBlockEntry(handle, name, md5, flags, archiveOut, &record, &blockIndex, NULL) == 1) {
        entry = record->GetBlockEntry(blockIndex);
        return true;
    }

    if (record) {
        record->RemoveInternalArchiveRef();
    }

    return false;
}

// PROVEN: 9468c9b0 IDENTICAL/UNPROVEN
// OFFSET: 0x007F9710/0x140509310
bool Blizzard::Mopaq::SFileHasHigherPriority(const char* name, const char* other) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    System_SFile::ARCHIVEREC* record = NULL;

    if (!GetFileBlockEntry(NULL, name, md5, 0, NULL, &record, NULL, NULL) || !record) {
        return false;
    }

    int64_t priority = (static_cast<int64_t>(record->m_priority) << 32) + record->m_id;
    record->RemoveInternalArchiveRef();
    record = NULL;

    if (!GetFileBlockEntry(NULL, other, md5, 0, NULL, &record, NULL, NULL) || !record) {
        return true;
    }

    int64_t otherPriority = (static_cast<int64_t>(record->m_priority) << 32) + record->m_id;
    record->RemoveInternalArchiveRef();

    return priority > otherPriority;
}

// OFFSET: 0x007AC0B0
int64_t Blizzard::Mopaq::AddChunkOverhead(int64_t size, System_SFile::ARCHIVEREC* archive) {
    if (!archive) {
        return size;
    }

    if (archive->m_parent) {
        archive = archive->m_parent;
    }

    int32_t chunkSize = archive->m_header.m_rawChunkSize;

    if (chunkSize > 0) {
        return size + 16 * ((size + chunkSize - 1) / chunkSize);
    }

    return size;
}

// OFFSET: 0x007F4460
int64_t Blizzard::Mopaq::GetStreamedSize(System_SFile::ARCHIVEREC* handle, int64_t size) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);
    int64_t result = AddChunkOverhead(size, archive);

    if (archive) {
        archive->RemoveInternalArchiveRef();
    }

    return result;
}

// OFFSET: 0x007F44A0/0x140502000
int32_t Blizzard::Mopaq::SFileFileIsLocal(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex) {
    System_SFile::ArchivePtr archive(handle);

    if (!archive.m_archive) {
        return 1;
    }

    System_SFile::BLOCKENTRY entry = archive.m_archive->GetBlockEntry(blockIndex);
    Blizzard::Mopaq::MpqHandleRecord* fileHandle = archive.m_archive->m_file->m_handle;
    Blizzard::File::StreamRecord* file = fileHandle ? fileHandle->m_file : NULL;

    return Blizzard::Streaming::IsLocal(file, entry.m_offset, AddChunkOverhead(entry.m_compressedSize, archive.m_archive));
}

// OFFSET: 0x007F4680/0x140502170
bool Blizzard::Mopaq::SFileMakeFileLocal(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex, int64_t offset, int64_t size, bool wait) {
    System_SFile::ArchivePtr archive(handle);

    if (!archive.m_archive) {
        return true;
    }

    System_SFile::BLOCKENTRY entry = archive.m_archive->GetBlockEntry(blockIndex);
    int64_t start = offset + entry.m_offset;
    int64_t streamed = GetStreamedSize(handle, entry.m_compressedSize);

    if (size > streamed) {
        size = streamed;
    }

    Blizzard::Mopaq::MpqHandleRecord* fileHandle = archive.m_archive->m_file->m_handle;
    Blizzard::File::StreamRecord* file = fileHandle ? fileHandle->m_file : NULL;

    if (wait) {
        return Blizzard::Streaming::MakeLocalBlocking(file, start, size, NULL, NULL);
    }

    return Blizzard::Streaming::MakeLocal(file, start, size, NULL, NULL);
}

// OFFSET: 0x007E3720
static void AddIsLocalAmount(System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY& entry, int64_t& local, int64_t& total) {
    int64_t size = Blizzard::Mopaq::AddChunkOverhead(entry.m_compressedSize, archive);

    if (Blizzard::Streaming::IsStreamedFile(archive->m_name)) {
        local += Blizzard::Streaming::GetIsLocalAmount(archive->m_name, entry.m_offset, size);
    } else {
        local += size;
    }

    total += size;
}

// OFFSET: 0x007FA8B0/0x14050A860
bool Blizzard::Mopaq::SFileGetIsLocalAmount(const char* name, int64_t& local, int64_t& total) {
    System_SFile::ARCHIVEREC* record = NULL;
    uint32_t blockIndex;
    int32_t result = GetFileBlockEntry(NULL, name, System_SFile::s_emptyMd5, 0, NULL, &record, &blockIndex, NULL);

    if (result == 1) {
        System_SFile::BLOCKENTRY entry = record->GetBlockEntry(blockIndex);
        AddIsLocalAmount(record, entry, local, total);
    }

    if (record) {
        record->RemoveInternalArchiveRef();
    }

    return result == 1;
}

// OFFSET: 0x007F4790
void Blizzard::Mopaq::SFileGetIsLocalAmount(System_SFile::ARCHIVEREC* handle, int64_t& local, int64_t& total) {
    local = 0;
    total = 0;

    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (archive) {
        local = Blizzard::Streaming::GetIsLocalAmount(archive->m_name, 0, archive->m_header.m_archiveSize64);
        total = archive->m_header.m_archiveSize64;
        archive->RemoveInternalArchiveRef();
    }
}

// OFFSET: 0x007F4830
bool Blizzard::Mopaq::IsArchiveStreamingComplete(System_SFile::ARCHIVEREC* handle) {
    int64_t local;
    int64_t total;
    SFileGetIsLocalAmount(handle, local, total);

    return total > 0 && local == total;
}

// OFFSET: 0x007E37A0/0x1404EA4E0
void Blizzard::Mopaq::SFileGetIsLocalAmount(HSFILERecord* file, int64_t& local, int64_t& total) {
    MpqHandleRecord* handle = file->m_handle;
    System_SFile::ARCHIVEREC* archive = handle->m_archive;

    if (archive) {
        System_SFile::BLOCKENTRY entry = archive->GetBlockEntry(handle->m_blockIndex);
        AddIsLocalAmount(archive, entry, local, total);
    }
}

struct ArchiveStreamInfo {
    /* 0000 */ System_SFile::ARCHIVEREC* m_archive;
    /* 0004 */ Blizzard::File::StreamRecord* m_file;
    /* 0008 */ System_SFile::BLOCKENTRY m_entry;
};

class DeltaCompletionCallback : public Blizzard::Streaming::CompletionCallback {
    public:
    /* 000C */ ArchiveStreamInfo m_info;
    /* 0050 */ std::string m_name;

    DeltaCompletionCallback(const ArchiveStreamInfo& info, const std::string& name);
    virtual ~DeltaCompletionCallback();
    virtual void Complete();
};

// OFFSET: 0x007B41F0
static bool GetArchiveStreamInfo(Blizzard::Mopaq::HSFILERecord* file, ArchiveStreamInfo* info) {
    Blizzard::Mopaq::MpqHandleRecord* handle = file->m_handle;

    if (handle) {
        info->m_archive = handle->m_logicalArchive ? handle->m_logicalArchive : handle->m_archive;
    } else {
        info->m_archive = NULL;
    }

    if (handle && handle->m_archive && handle->m_archive->m_file && handle->m_archive->m_file->m_handle) {
        info->m_file = handle->m_archive->m_file->m_handle->m_file;
    } else {
        info->m_file = NULL;
    }

    if (!info->m_file) {
        info->m_file = handle ? handle->m_file : NULL;
    }

    info->m_entry = handle->m_blockEntry;

    return true;
}

// OFFSET: 0x007FA340
static bool GetArchiveStreamInfo(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, ArchiveStreamInfo* info) {
    System_SFile::ARCHIVEREC* record = NULL;

    if (!Blizzard::Mopaq::SFileGetFileDetails(handle, name, md5, flags, &info->m_archive, record, info->m_entry) || !record) {
        return false;
    }

    if (!info->m_archive) {
        info->m_archive = record;
    }

    if (record->m_file) {
        info->m_file = record->m_file->m_handle ? record->m_file->m_handle->m_file : NULL;
    }

    record->RemoveInternalArchiveRef();

    return true;
}

// OFFSET: 0x007FA3C0
static int32_t NextDeltaBase(ArchiveStreamInfo* info, const char* name) {
    if (!info->m_entry.m_patch) {
        return 0;
    }

    Blizzard::Mopaq::HSFILERecord* file;

    if (Blizzard::Mopaq::SFileOpenFileEx(info->m_archive, name, info->m_entry.m_md5, 0x2000, &file) != 1) {
        return 2;
    }

    Blizzard::Util::Digest<16> source;
    Blizzard::Util::Digest<16> target;
    bool found = Blizzard::Mopaq::SFileGetDeltaDigests(file, source, target);
    Blizzard::Mopaq::SFileCloseFile(file);

    if (!found || source == target) {
        return 2;
    }

    return 2 - (GetArchiveStreamInfo(info->m_archive, name, source, 0x1000, info) != 0);
}

// OFFSET: 0x007FA460
static int32_t FileIsLocal(const char* name) {
    ArchiveStreamInfo info;

    if (!GetArchiveStreamInfo(NULL, name, System_SFile::s_emptyMd5, 0, &info)) {
        return -1;
    }

    int32_t result;

    while (true) {
        int64_t size = Blizzard::Mopaq::GetStreamedSize(info.m_archive, info.m_entry.m_compressedSize);
        result = Blizzard::Streaming::IsLocal(info.m_file, info.m_entry.m_offset, size);

        if (result != 1 || !info.m_entry.m_patch) {
            break;
        }

        if (NextDeltaBase(&info, name) != 1) {
            return -1;
        }
    }

    return result;
}

// OFFSET: %0x0075CAC0
int32_t Blizzard::Mopaq::SFileFileIsLocal(const char* name, int32_t option) {
    return FileIsLocal(name);
}

// OFFSET: 0x007C8150
DeltaCompletionCallback::DeltaCompletionCallback(const ArchiveStreamInfo& info, const std::string& name) {
    this->m_info = info;
    this->m_name = name;
}

// OFFSET: 0x007C81D0
DeltaCompletionCallback::~DeltaCompletionCallback() {
}

// OFFSET: 0x007FA550
void DeltaCompletionCallback::Complete() {
    if (this->m_success && NextDeltaBase(&this->m_info, this->m_name.c_str()) == 1) {
        DeltaCompletionCallback* callback = new DeltaCompletionCallback(this->m_info, this->m_name);
        int64_t size = Blizzard::Mopaq::GetStreamedSize(this->m_info.m_archive, this->m_info.m_entry.m_compressedSize);
        Blizzard::Streaming::MakeLocal(this->m_info.m_file, this->m_info.m_entry.m_offset, size, callback, NULL);
    }

    delete this;
}

// OFFSET: 0x007FA640
static bool MakeArchiveStreamLocal(ArchiveStreamInfo* info, bool wait, const char* name, Blizzard::Streaming::MakeLocalParams* params, const int64_t* chunk) {
    int64_t size = Blizzard::Mopaq::GetStreamedSize(info->m_archive, info->m_entry.m_compressedSize);

    if (!wait) {
        return Blizzard::Streaming::MakeLocal(info->m_file, info->m_entry.m_offset, size, new DeltaCompletionCallback(*info, std::string(name)), chunk);
    }

    while (true) {
        bool result = Blizzard::Streaming::MakeLocalBlocking(info->m_file, info->m_entry.m_offset, size, params, chunk);

        if (!result || (params && params->m_status == 1)) {
            return result;
        }

        int32_t next = NextDeltaBase(info, name);
        size = Blizzard::Mopaq::GetStreamedSize(info->m_archive, info->m_entry.m_compressedSize);

        if (next != 1) {
            return next == 0;
        }
    }
}

// OFFSET: 0x007DC580/0x14050A7D0
bool Blizzard::Mopaq::SFileMakeFileRangeLocal(HSFILERecord* file, int64_t offset, int64_t size, Blizzard::Streaming::MakeLocalParams* params, const int64_t* chunk) {
    MpqHandleRecord* handle = file->m_handle;
    System_SFile::BLOCKENTRY entry = handle->m_blockEntry;

    if (!handle) {
        return false;
    }

    int64_t total = entry.m_compressedSize;

    if (total <= 0) {
        return false;
    }

    int64_t available = total - offset;
    size = size >= available ? available : size;

    System_SFile::ARCHIVEREC* archive = handle->m_archive;
    Blizzard::File::StreamRecord* stream = NULL;

    if (archive && archive->m_file && archive->m_file->m_handle) {
        stream = archive->m_file->m_handle->m_file;
    }

    return Blizzard::Streaming::MakeLocalBlocking(stream, offset + entry.m_offset, size, params, chunk);
}

// OFFSET: %0x0075CE10
bool Blizzard::Mopaq::SFileMakeFileLocal(const char* name, int32_t option, bool wait, Blizzard::Streaming::MakeLocalParams* params) {
    ArchiveStreamInfo info;

    if (!GetArchiveStreamInfo(NULL, name, System_SFile::s_emptyMd5, 0, &info)) {
        return false;
    }

    return MakeArchiveStreamLocal(&info, wait, name, params, NULL);
}

// OFFSET: 0x007FA830
bool Blizzard::Mopaq::SFileMakeFileLocal(HSFILERecord* file, bool wait, Blizzard::Streaming::MakeLocalParams* params, const int64_t* chunk) {
    if (!file) {
        return false;
    }

    ArchiveStreamInfo info;

    if (!GetArchiveStreamInfo(file, &info)) {
        return false;
    }

    if (!info.m_file || !info.m_entry.m_compressedSize) {
        return true;
    }

    return MakeArchiveStreamLocal(&info, wait, file->m_handle ? file->m_handle->m_name : "", params, chunk);
}

// OFFSET: 0x007B14A0
void System_SFile::ARCHIVEREC::SetMD5Check(uint32_t index, bool enabled) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x200) {
            blocks->SetMD5Check(index, enabled);
        }
    } else {
        this->m_blockTable[index].m_md5Check = enabled;
    }
}

// OFFSET: 0x007B9870/0x1404B0940
void System_SFile::ARCHIVEREC::SetBlockEntry(uint32_t index, const BLOCKENTRY& entry) {
    this->m_blockTable[index] = entry;
}

// PROVEN: 4e4e93b8 IDENTICAL/IDENTICAL
// OFFSET: 0x007B1350/0x1404A6210
void System_SFile::ARCHIVEREC::SetBlockFileTime(uint32_t index, uint64_t fileTime) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x100) {
            blocks->m_fileTimes[index] = fileTime;
        }
    } else {
        this->m_blockTable[index].m_fileTime = fileTime;
    }
}

// PROVEN: 41f84187 IDENTICAL/IDENTICAL
// OFFSET: 0x007B13E0/0x1404A62D0
void System_SFile::ARCHIVEREC::SetBlockMd5(uint32_t index, const Blizzard::Util::Digest<16>& md5) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x40) {
            blocks->m_md5s[index] = md5;
        }
    } else {
        this->m_blockTable[index].m_md5 = md5;
    }
}

// PROVEN: ffa6d034 IDENTICAL/IDENTICAL
// OFFSET: 0x007B1540/0x1404A6480
void System_SFile::ARCHIVEREC::SetBlockCrc(uint32_t index, uint32_t crc) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (this->m_flags & 0x80) {
            blocks->m_crcs[index] = crc;
        }
    } else {
        this->m_blockTable[index].m_crc = crc;
    }
}

// PROVEN: 7e5fbb5c UNPROVEN/IDENTICAL
// OFFSET: 0x007B15D0/0x1404A6540
void System_SFile::ARCHIVEREC::SetBlockPatch(uint32_t index, bool patch) {
    BlockEntryTable* blocks = this->m_blockEntryTable;

    if (blocks) {
        if (patch && (this->m_flags & 0x1000)) {
            static const uint8_t s_bits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
            blocks->m_patchBits[index >> 3] |= s_bits[index & 7];
        }
    } else {
        this->m_blockTable[index].m_patch = patch;
    }
}

// OFFSET: 0x007B98C0
bool System_SFile::ARCHIVEREC::IncreaseHashBits(int32_t* hashBits) {
    if (*hashBits == 64) {
        return false;
    }

    HashEntryTable* table = this->m_hashEntryTable;

    if (table->m_indices) {
        delete table->m_indices;
        table->m_indices = NULL;
    }

    memset(table->m_tags, 0, table->m_slotCount);
    this->m_hashEntryTable->SetHashBits(++*hashBits);
    this->m_blockEntryTable->InitializeHashCheckTable(*hashBits);

    return true;
}

// OFFSET: 0x007F7E10/0x140507070
bool System_SFile::ARCHIVEREC::ReadAdditionalAttributes() {
    Blizzard::Util::Digest<16> md5 = s_emptyMd5;
    Blizzard::Mopaq::HSFILERecord* file;

    if (!Blizzard::Mopaq::SFileOpenFileEx(this, "(attributes)", md5, 0x10, &file)) {
        return Blizzard::Mopaq::SFileFileExists(this, "(attributes)", 0x10, NULL) == 0;
    }

    uint32_t count = this->m_header.m_blockTableSize;
    uint32_t size = static_cast<uint32_t>(file->GetFileSize());

    if (size < 8 || size > (count << 9) + 8) {
        Blizzard::Mopaq::SFileCloseFile(file);
        return false;
    }

    HeapArray<uint8_t> data(size);
    uint32_t read;

    if (!Blizzard::Mopaq::SFileReadFile(file, data.m_data, size, &read)) {
        Blizzard::Mopaq::SFileCloseFile(file);
        return false;
    }

    Blizzard::Mopaq::SFileCloseFile(file);

    const uint32_t* header = reinterpret_cast<const uint32_t*>(data.m_data);
    uint32_t flags = header[1];
    uint32_t remaining = size - 8;
    const uint8_t* p = data.m_data + 8;
    uint32_t needed = 0;
    uint32_t unknown = flags;

    if (flags & 1) {
        needed = 4 * count;
        unknown &= ~1u;
    }

    if (flags & 2) {
        needed += 8 * count;
        unknown &= ~2u;
    }

    if (flags & 4) {
        needed += 16 * count;
        unknown &= ~4u;
    }

    if (flags & 8) {
        if (remaining == needed + ((count + 6) >> 3)) {
            needed += (count + 6) >> 3;
        } else {
            needed += 4 * count;
        }

        unknown &= ~8u;
    }

    if (remaining < needed || (!unknown && remaining != needed)) {
        return false;
    }

    if (flags & 1) {
        for (uint32_t i = 0; i < count; i++, p += 4) {
            this->SetBlockCrc(i, *reinterpret_cast<const uint32_t*>(p));
        }
    }

    if (flags & 2) {
        for (uint32_t i = 0; i < count; i++, p += 8) {
            this->SetBlockFileTime(i, *reinterpret_cast<const uint64_t*>(p));
        }
    }

    if (flags & 4) {
        for (uint32_t i = 0; i < count; i++, p += 16) {
            this->SetBlockMd5(i, *reinterpret_cast<const Blizzard::Util::Digest<16>*>(p));
        }
    }

    if (flags & 8) {
        static const uint8_t s_bits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

        if (count != 1) {
            uint32_t i = 0;

            do {
                this->SetBlockPatch(i, (s_bits[i & 7] & *p) != 0);

                if ((i & 7) == 7) {
                    p++;
                }
            } while (++i < count - 1);
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        this->SetMD5Check(i, false);
    }

    return true;
}

// OFFSET: 0x007F9000/0x140508920
bool Blizzard::Mopaq::SFileOpenArchiveView(System_SFile::ARCHIVEREC* handle, const char* name, int32_t priority, System_SFile::ARCHIVEREC** result) {
    *result = NULL;

    System_SFile::ArchivePtr archive(handle);

    if (!archive.m_archive) {
        return false;
    }

    char path[260];

    if (!archive.m_archive->m_file) {
        Blizzard::String::JoinPath(path, sizeof(path), archive.m_archive->m_directPath.c_str(), name);
        return SFileOpenArchive(path, priority, archive.m_archive->m_flags, result);
    }

    if (archive.m_archive->m_parent) {
        Blizzard::String::JoinPath(path, sizeof(path), archive.m_archive->m_directPath.c_str(), name);
        return SFileOpenArchiveView(archive.m_archive->m_parent, path, priority, result);
    }

    System_SFile::ARCHIVEREC* view = NewZerofill<System_SFile::ARCHIVEREC>();
    view->m_priority = priority;
    view->m_id = System_SFile::s_nextArchiveId.Increment() - 1;
    view->m_flags = archive.m_archive->m_flags;
    view->m_externalRefs.Increment();
    view->m_internalRefs.Increment();

    {
        Blizzard::Lock::ScopedMutex lock(System_MPQGlobals::GetArchiveLock()->m_mutex);
        archive.m_archive->m_internalRefs.Increment();
        System_MPQGlobals::GetArchiveList()->insert(view);
    }

    Blizzard::String::Copy(view->m_name, archive.m_archive->m_name, sizeof(view->m_name));

    HSFILERecord* file = new HSFILERecord(*archive.m_archive->m_file);

    view->m_file = file;
    view->m_parent = handle;
    view->unk_02E0 = 0;

    char directory[260];
    Blizzard::String::MakeBackslashPath(name, directory, sizeof(directory));

    if (directory[0]) {
        Blizzard::String::ForceTrailingSeparator(directory, sizeof(directory), '\\');
    }

    view->m_directPath.assign(directory, strlen(directory));
    *result = view;

    return true;
}

// OFFSET: 0x007F98E0/0x1405094F0
void System_SFile::ClearAllFileFlags() {
    bool threadSafe = s_threadSafe;
    Blizzard::Lock::RMutex* mutex = System_MPQGlobals::GetArchiveLock();

    if (threadSafe) {
        Blizzard::Lock::MutexEnter(mutex->m_mutex);
    }

    System_MPQGlobals::ArchiveList* list = System_MPQGlobals::GetArchiveList();

    for (System_MPQGlobals::ArchiveList::iterator it = list->begin(); it != list->end(); ++it) {
        ARCHIVEREC* archive = *it;

        if (archive->m_parent) {
            continue;
        }

        Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);
        archive->InitializeTables();

        for (uint32_t i = 0; i < archive->m_header.m_blockTableSize; i++) {
            archive->SetMD5Check(i, false);
        }

        Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);
    }

    if (threadSafe) {
        Blizzard::Lock::MutexLeave(mutex->m_mutex);
    }
}

// PROVEN: 621e9913 IDENTICAL/IDENTICAL
// OFFSET: 0x007F1B80/0x1404FEA70
void Blizzard::Mopaq::SetSignatureCheckMode(uint32_t mode) {
    System_MPQGlobals::GetMopaqGlobals()->m_md5Mode = mode;
}

// PROVEN: a7561424 IDENTICAL/IDENTICAL
// OFFSET: 0x007F1BA0/0x1404FEA90
void Blizzard::Mopaq::SetSignatureErrorCallback(void (*callback)(const char* name, const char* message)) {
    System_MPQGlobals::GetMopaqGlobals()->m_signatureErrorCallback = callback;
}

// OFFSET: 0x007F9B60/0x140509810
bool Blizzard::Mopaq::SetFileSignature(const char* name, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Util::Digest<16> empty = System_SFile::s_emptyMd5;
    System_SFile::ARCHIVEREC* record = NULL;
    uint32_t blockIndex;
    int32_t found = GetFileBlockEntry(NULL, name, empty, 0, NULL, &record, &blockIndex, NULL);

    if (found != 1) {
        Blizzard::String::QuickFormat<1024> message("%s - GetFileBlockEntry returns %d", "Blizzard::Mopaq::SetFileSignature", found);
        Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);

        if (record) {
            record->RemoveInternalArchiveRef();
        }

        return false;
    }

    System_SFile::ArchivePtr archive(record);

    if (archive.m_archive) {
        Blizzard::Lock::MutexEnter(archive.m_archive->m_mutex.m_mutex);
    }

    archive.m_locked = true;

    bool result = System_SFile::s_signatures.AddEntry(name, md5);

    if (!result) {
        result = System_SFile::s_signatures.DelEntry(name);

        if (!result) {
            Blizzard::String::QuickFormat<1024> message("%s - DelEntry fails", "Blizzard::Mopaq::SetFileSignature");
            Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);
        } else {
            result = System_SFile::s_signatures.AddEntry(name, md5);

            if (!result) {
                Blizzard::String::QuickFormat<1024> message("%s - AddEntry fails", "Blizzard::Mopaq::SetFileSignature");
                Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);
            }
        }
    }

    if (result) {
        archive.m_archive->SetMD5Check(blockIndex, true);
    }

    if (record) {
        record->RemoveInternalArchiveRef();
    }

    return result;
}

// PROVEN: 9a199608 IDENTICAL/DIFFERENT
// OFFSET: 0x007B5A40/0x1404ABA60
char* System_SFile::BackslashPath(const char* path, char* buffer, uint32_t size) {
    Blizzard::String::MakeBackslashPath(path, buffer, size);
    return buffer;
}

// OFFSET: 0x007B1CC0
bool System_SFile::ListFileEntry::operator<(const ListFileEntry& other) const {
    int32_t compare;

    if (this->m_name == other.m_name) {
        compare = 0;
    } else if (!this->m_name) {
        compare = -1;
    } else if (!other.m_name) {
        compare = 1;
    } else {
        compare = _stricmp(this->m_name, other.m_name);
    }

    return compare < 0 || (compare <= 0 && this->m_blockIndex < other.m_blockIndex);
}

// INLINED
void System_SFile::ARCHIVEREC::ReleaseListfile() {
    if (--this->m_listfileRefcount <= 0) {
        this->m_listfileEntries.clear();
        this->m_listfileLoaded = false;
    }
}

// OFFSET: 0x007F2330/0x1404FF520
void System_SFile::ArchivePtr::Reset(ARCHIVEREC* handle) {
    bool locked = this->m_locked;

    if (locked) {
        if (this->m_archive) {
            Blizzard::Lock::MutexLeave(this->m_archive->m_mutex.m_mutex);
        }

        this->m_locked = false;
    }

    if (this->m_archive) {
        this->m_archive->RemoveInternalArchiveRef();
    }

    this->m_archive = GetArchivePtr(handle, true);

    if (locked) {
        if (!this->m_locked && this->m_archive) {
            Blizzard::Lock::MutexEnter(this->m_archive->m_mutex.m_mutex);
        }

        this->m_locked = true;
    }
}

struct BlockHash {
    /* 0000 */ uint32_t m_index;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_hash;
    /* 0010 */ bool m_listed;
    /* 0011 */ uint8_t unk_0011[7];

    bool operator<(const BlockHash& other) const;
};

// INLINED
bool BlockHash::operator<(const BlockHash& other) const {
    return this->m_hash < other.m_hash;
}

struct NameHash {
    /* 0000 */ uint64_t m_hash;
    /* 0008 */ const char* m_name;
    /* 000C */ uint32_t unk_000C;

    bool operator<(const NameHash& other) const;
};

// INLINED
bool NameHash::operator<(const NameHash& other) const {
    return this->m_hash < other.m_hash;
}

// INLINED
static uint64_t HashName(const char* name) {
    return System_MPQEncrypt::Hash(name, 1) | static_cast<uint64_t>(System_MPQEncrypt::Hash(name, 2)) << 32;
}

// OFFSET: 0x007D3CC0/0x1404D3B40
void System_SFile::ARCHIVEREC::AcquireListfileUncompressedBlockHash() {
    std::vector<BlockHash> hashes;
    uint32_t count = static_cast<uint32_t>(this->m_hashTable.size());
    hashes.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_hashTable[i].m_blockIndex < 0xFFFFFFFE) {
            BlockHash entry;
            entry.m_index = i;
            entry.m_hash = this->m_hashTable[i].m_hashA | static_cast<uint64_t>(this->m_hashTable[i].m_hashB) << 32;
            entry.m_listed = false;
            hashes.push_back(entry);
        }
    }

    std::sort(hashes.begin(), hashes.end());

    uint32_t unlisted = static_cast<uint32_t>(hashes.size());
    this->m_listfileEntries.reserve(4);

    const char* special[4];
    special[0] = "(listfile)";
    special[1] = "(attributes)";
    special[2] = "(signature)";
    special[3] = "(patch_metadata)";

    for (uint32_t i = 0; i < 4; i++) {
        const char* name = special[i];
        BlockHash probe;
        probe.m_index = -1;
        probe.m_hash = HashName(name);
        probe.m_listed = false;

        std::vector<BlockHash>::iterator it = std::lower_bound(hashes.begin(), hashes.end(), probe);

        if (it != hashes.end() && it->m_hash == probe.m_hash) {
            ListFileEntry entry;
            entry.m_name = name;
            entry.m_blockIndex = -1;
            this->m_listfileEntries.push_back(entry);

            do {
                if (!it->m_listed) {
                    unlisted--;
                    it->m_listed = true;
                }

                ++it;
            } while (it != hashes.end() && it->m_hash == probe.m_hash);
        }
    }

    this->m_listfileEntries.reserve(this->m_listfile->m_names.size());

    for (std::vector<const char*>::iterator name = this->m_listfile->m_names.begin(); name != this->m_listfile->m_names.end(); ++name) {
        BlockHash probe;
        probe.m_index = -1;
        probe.m_hash = HashName(*name);
        probe.m_listed = false;

        std::vector<BlockHash>::iterator it = std::lower_bound(hashes.begin(), hashes.end(), probe);

        if (it != hashes.end() && it->m_hash == probe.m_hash) {
            ListFileEntry entry;
            entry.m_name = *name;
            entry.m_blockIndex = this->m_hashTable[it->m_index].m_blockIndex;
            this->m_listfileEntries.push_back(entry);

            do {
                if (!it->m_listed) {
                    unlisted--;
                    it->m_listed = true;
                }

                ++it;
            } while (it != hashes.end() && it->m_hash == probe.m_hash);
        } else {
            Blizzard::String::QuickFormat<1024> message("Listfile references unknown file \"%s\"!\n", *name);
            fputs(message.m_buffer, stderr);
        }
    }

    std::sort(this->m_listfileEntries.begin(), this->m_listfileEntries.end());

    if (unlisted) {
        Blizzard::String::QuickFormat<1024> message("Archive \"%s\" has an incomplete listfile!\n", this->m_name);
        fputs(message.m_buffer, stderr);
        this->m_listfileLoaded = false;
    } else {
        this->m_listfileLoaded = true;
    }
}

// OFFSET: 0x007D4430
void System_SFile::ARCHIVEREC::AcquireListfileCompressedBlockHash() {
    ListFileParser parser(this->m_listfile);

    while (!parser.Next()) {
        if (parser.m_isSpecial) {
            continue;
        }

        uint64_t hash = HashEntryTable::CalculateHash(parser.m_token);
        ListFileEntry entry;
        entry.m_name = parser.m_token;
        entry.m_blockIndex = this->m_hashEntryTable->GetEntry(hash, this->m_blockEntryTable);
        this->m_listfileEntries.push_back(entry);
    }

    this->m_listfileLoaded = true;
}

// OFFSET: 0x007F4BE0/0x140502820
bool System_SFile::ARCHIVEREC::AcquireListfile() {
    if (this->m_listfileRefcount++ <= 0) {
        this->InitializeTables();

        if (!this->m_listfile) {
            this->m_listfile = new ListFile();
            this->m_listfile->Load(this);
        }

        if (!this->m_hashTable.empty() || !this->m_hashEntryTable) {
            this->AcquireListfileUncompressedBlockHash();
        } else {
            this->AcquireListfileCompressedBlockHash();
        }
    }

    return this->m_listfileLoaded;
}

// OFFSET: 0x007F4DF0
bool System_SFile::RebuildHashTable(ARCHIVEREC* archive, uint32_t hashSize) {
    if (!archive->AcquireListfile()) {
        archive->ReleaseListfile();
        return false;
    }

    std::vector<NameHash> names;
    names.reserve(archive->m_listfileEntries.size());

    for (std::vector<ListFileEntry>::iterator it = archive->m_listfileEntries.begin(); it != archive->m_listfileEntries.end(); ++it) {
        const char* name = it->m_name;
        NameHash entry;
        entry.m_hash = HashName(name);
        entry.m_name = name;
        names.push_back(entry);
    }

    std::sort(names.begin(), names.end());

    HASHENTRY empty;
    memset(&empty, 0xFF, sizeof(empty));

    std::vector<HASHENTRY> table(hashSize, empty);

    for (std::vector<HASHENTRY>::iterator it = archive->m_hashTable.begin(); it != archive->m_hashTable.end(); ++it) {
        if (it->m_blockIndex >= 0xFFFFFFFE) {
            continue;
        }

        NameHash probe;
        probe.m_hash = it->m_hashA | static_cast<uint64_t>(it->m_hashB) << 32;
        probe.m_name = NULL;

        std::vector<NameHash>::iterator found = std::lower_bound(names.begin(), names.end(), probe);

        if (found == names.end() || found->m_hash != probe.m_hash) {
            continue;
        }

        uint32_t start = System_MPQEncrypt::Hash(found->m_name, 0) & (hashSize - 1);
        uint32_t slot = start;

        while (table[slot].m_blockIndex < 0xFFFFFFFE) {
            slot = (slot + 1) & (hashSize - 1);

            if (slot == start) {
                return false;
            }
        }

        table[slot] = *it;
    }

    archive->m_hashTable.swap(table);
    archive->ReleaseListfile();
    archive->m_header.m_hashTableSize = hashSize;

    return true;
}

// INLINED
static void DeleteHashEntry(System_SFile::ARCHIVEREC* archive, uint32_t hashIndex, const char* name) {
    for (size_t i = archive->m_listfileEntries.size(); i;) {
        i--;

        if (archive->m_listfileEntries[i].m_name && name && !_stricmp(archive->m_listfileEntries[i].m_name, name)) {
            std::swap(archive->m_listfileEntries[i], archive->m_listfileEntries.back());

            if (!archive->m_listfileEntries.empty()) {
                archive->m_listfileEntries.pop_back();
            }
        }
    }

    System_SFile::HASHENTRY& hash = archive->m_hashTable[hashIndex];
    System_SFile::BLOCKENTRY entry = archive->GetBlockEntry(hash.m_blockIndex);
    uint32_t mask = static_cast<uint32_t>(archive->m_hashTable.size()) - 1;
    System_SFile::HASHENTRY& next = archive->m_hashTable[(hashIndex + 1) & mask];
    entry.m_flags &= ~0x80000000;
    archive->m_blockTable[hash.m_blockIndex] = entry;
    hash.m_blockIndex = next.m_blockIndex == -1 ? -1 : -2;
    archive->ReturnWriteReservation(entry.m_offset, entry.m_compressedSize, 0, false);
}

// OFFSET: 0x007F6F60
bool System_SFile::PrepareArchiveForWriting(ARCHIVEREC* archive) {
    if (!(archive->m_flags & 0x10)) {
        return false;
    }

    if (archive->m_file && !archive->m_closeCallback) {
        archive->InitializeTables();
        ArchiveCheckpoint::CreateRestorePoint(archive);
        archive->InitializeWriteReservations(archive->m_header.m_headerSize);
        archive->AcquireListfile();

        uint32_t hashIndex = SearchHashTable(archive, "(attributes)", true);

        if (hashIndex != -1) {
            DeleteHashEntry(archive, hashIndex, "(attributes)");
        }

        hashIndex = SearchHashTable(archive, "(signature)", true);

        if (hashIndex != -1) {
            DeleteHashEntry(archive, hashIndex, "(signature)");
        }

        hashIndex = SearchHashTable(archive, "(listfile)", true);

        if (hashIndex != -1) {
            DeleteHashEntry(archive, hashIndex, "(listfile)");
        }

        uint32_t zero = 0;
        archive->m_file->Write(&zero, archive->m_archiveOffset, 4);
        archive->m_closeCallback = ARCHIVEREC::WriteArchiveChanges;
    }

    return true;
}

// OFFSET: 0x007B1D20
int64_t System_SFile::ARCHIVEREC::GetWriteReservationLimit() {
    if (this->m_blockEntryTable) {
        return -1;
    }

    uint32_t high = this->m_header.m_formatVersion ? 0xFFFF : 0;

    return ((static_cast<int64_t>(high) << 32) | 0xFFFFFFFF)
        - static_cast<uint32_t>(sizeof(BLOCKENTRY) * this->m_blockTable.size())
        - static_cast<uint32_t>(sizeof(HASHENTRY) * this->m_hashTable.size())
        - static_cast<uint32_t>(262 * this->m_listfileEntries.size())
        - this->m_writeReservationPending
        - this->m_writeReservationEnd
        - 0x10000;
}

// OFFSET: 0x007B1DD0
bool System_SFile::ARCHIVEREC::PreRequestWriteReservation(int64_t size) {
    Blizzard::Lock::MutexEnter(this->m_writeLock.m_mutex);

    int64_t reserved = AddMD5Overhead(this->m_header.m_rawChunkSize, size);

    if (this->GetWriteReservationLimit() >= reserved) {
        this->m_writeReservationPending += reserved;
        Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);
        return true;
    }

    Blizzard::Lock::MutexLeave(this->m_writeLock.m_mutex);

    return false;
}

// OFFSET: 0x007D0380
bool System_Mopaq::CreateDiskFile(Blizzard::Mopaq::HSFILERecord** result, const char* path, uint32_t flags) {
    char directory[260];

    if (path) {
        int32_t length = static_cast<int32_t>(Blizzard::String::FindFilename(path) - path) + 1;
        Blizzard::String::Copy(directory, path, std::min(length, 260));
    } else {
        directory[0] = '\0';
    }

    Blizzard::File::CreateDirectory(directory, true);
    CreateDiskFile(path, flags, NULL, NULL, result);

    if (*result) {
        return true;
    }

    System_SFile::s_lastError = 1006;

    return false;
}

// OFFSET: 0x007F5840/0x140503AB0
bool Blizzard::Mopaq::SetupHashEntry(System_SFile::ArchivePtr& archive, const char* name, const System_SFile::BLOCKENTRY& entry, int32_t blockIndex) {
    uint32_t hash = System_MPQEncrypt::Hash(name, 0);
    bool rebuilt = false;

    while (true) {
        uint32_t size = static_cast<uint32_t>(archive.m_archive->m_hashTable.size());
        uint32_t slot = hash & (size - 1);
        uint32_t tries = 0;

        if (size) {
            while (true) {
                System_SFile::HASHENTRY& candidate = archive.m_archive->m_hashTable[slot];

                if (candidate.m_blockIndex >= 0xFFFFFFFE) {
                    uint32_t hashA = System_MPQEncrypt::Hash(name, 1);
                    candidate.m_hashB = System_MPQEncrypt::Hash(name, 2);
                    candidate.m_hashA = hashA;
                    candidate.m_locale = archive.m_archive->m_locale;
                    candidate.m_platform = archive.m_archive->m_platform;
                    candidate.m_blockIndex = blockIndex;
                    candidate.unk_000B = 0;
                    archive.m_archive->m_blockTable.push_back(entry);
                    return true;
                }

                slot = (size - 1) & (slot + 1);

                if (++tries >= size) {
                    break;
                }
            }
        }

        if (rebuilt) {
            break;
        }

        uint32_t newSize = std::max(2 * size, 0x10000u);

        if (newSize >= 0x100000) {
            fputs("SFileCreateFile: warning - hash table about to exceed 2^24 items!\n", stderr);
        }

        if (!System_SFile::RebuildHashTable(archive.m_archive, newSize)) {
            break;
        }

        rebuilt = true;
    }

    fputs("SFileCreateFile: unable to insert an entry into the hash table!\n", stderr);

    return false;
}

// OFFSET: 0x007F59C0/0x140503D10
bool Blizzard::Mopaq::SFileCreateFile(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t size, uint32_t flags, HSFILERecord** result) {
    *result = NULL;

    for (const char* c = name; *c; c++) {
        if (*c == '\n' || *c == '\r') {
            return false;
        }
    }

    System_SFile::ArchivePtr archive(handle);

    if (archive.m_archive) {
        Blizzard::Lock::MutexEnter(archive.m_archive->m_mutex.m_mutex);
    }

    archive.m_locked = true;

    const char* path = name;
    char buffer[260];

    if (flags & 2) {
        if (!Blizzard::File::GetRootChars(name)) {
            System_SFile::ConvertRelativePathName(name, buffer);
            path = buffer;
        }

        return System_Mopaq::CreateDiskFile(result, path, flags);
    }

    if (!archive.m_archive) {
        return false;
    }

    if (!archive.m_archive->m_directPath.empty()) {
        Blizzard::String::JoinPath(buffer, sizeof(buffer), archive.m_archive->m_directPath.c_str(), name);
        path = buffer;
    }

    if (archive.m_archive->m_parent) {
        archive.Reset(archive.m_archive->m_parent);
    }

    if (!archive.m_archive->m_file) {
        return System_Mopaq::CreateDiskFile(result, path, flags);
    }

    uint32_t createFlags = flags & 0x424;
    uint32_t fileFlags = createFlags ^ flags;

    if (!System_SFile::PrepareArchiveForWriting(archive.m_archive)) {
        return false;
    }

    char fileName[260];
    System_SFile::BackslashPath(path, fileName, sizeof(fileName));

    if (fileFlags & 1) {
        uint32_t hashIndex = System_SFile::SearchHashTable(archive.m_archive, fileName, true);

        if (hashIndex != -1) {
            DeleteHashEntry(archive.m_archive, hashIndex, fileName);
        }

        fileFlags &= ~1u;
    } else if (System_SFile::SearchHashTable(archive.m_archive, fileName, false) != -1) {
        return false;
    }

    if (!archive.m_archive->PreRequestWriteReservation(size)) {
        return false;
    }

    System_SFile::BLOCKENTRY entry;
    entry.m_offset = -1;
    entry.m_compressedSize = 0;
    entry.m_fileSize = size;
    entry.m_flags = fileFlags;
    entry.m_fileTime = 0;
    entry.m_crc = 0;
    entry.m_md5 = System_SFile::s_emptyMd5;
    entry.m_md5Check = false;
    entry.m_patch = false;

    if ((fileFlags & 0x1000200) == 0x200) {
        entry.m_flags = fileFlags | 0x4000000;
    }

    uint32_t blockIndex = static_cast<uint32_t>(archive.m_archive->m_blockTable.size());

    if (!SetupHashEntry(archive, fileName, entry, blockIndex)) {
        return false;
    }

    if (!size || !(fileFlags & 0xFF00)) {
        entry.m_flags &= ~0x1000000u;
    }

    System_Mopaq::CreateArchiveFile(fileName, archive.m_archive, createFlags, entry, blockIndex, result);

    if (*result) {
        System_SFile::ListFileEntry listed;
        listed.m_name = archive.m_archive->m_listfile->Add(fileName);
        listed.m_blockIndex = blockIndex;
        archive.m_archive->m_listfileEntries.push_back(listed);
        return true;
    }

    System_SFile::s_lastError = 1006;
    Blizzard::String::QuickFormat<1024> message("SFileCreateFile - SFILE_ERROR_FILE_INVALID");
    Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

    return false;
}

// OFFSET: 0x007D8BE0
void System_SFile::ARCHIVEREC::CompactBlockTable() {
    std::map<uint32_t, uint32_t> moved;
    uint32_t count = static_cast<uint32_t>(this->m_blockTable.size());
    uint32_t kept = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_blockTable[i].m_flags & 0x80000000) {
            if (i != kept) {
                this->m_blockTable[kept] = this->m_blockTable[i];
                moved.insert(std::pair<const uint32_t, uint32_t>(i, kept));
            }

            kept++;
        }
    }

    this->m_blockTable.resize(kept);

    uint32_t hashCount = static_cast<uint32_t>(this->m_hashTable.size());

    for (uint32_t i = 0; i < hashCount; i++) {
        uint32_t blockIndex = this->m_hashTable[i].m_blockIndex;

        if (blockIndex <= count) {
            std::map<uint32_t, uint32_t>::iterator it = moved.find(blockIndex);

            if (it != moved.end()) {
                this->m_hashTable[i].m_blockIndex = it->second;
            }
        }
    }
}

// PROVEN: c2cadee7 IDENTICAL/NONE
// OFFSET: 0x007B1680
static uint32_t GetHashTableSize(System_SFile::ARCHIVEREC* archive) {
    uint32_t count = static_cast<uint32_t>(archive->m_blockTable.size());

    if (count - 0x8000 <= 0x7FFF) {
        return 0x10000;
    }

    uint32_t target = 4 * count / 3;
    uint32_t size;

    for (size = 4; size < target; size *= 2) {
    }

    return size;
}

// OFFSET: 0x007F65A0/0x140504F80
static bool WriteListfile(std::vector<char>& data, System_SFile::ARCHIVEREC* archive) {
    bool result = true;
    std::sort(archive->m_listfileEntries.begin(), archive->m_listfileEntries.end());
    data.reserve(260 * archive->m_listfileEntries.size());

    for (std::vector<System_SFile::ListFileEntry>::iterator it = archive->m_listfileEntries.begin(); it != archive->m_listfileEntries.end(); ++it) {
        if (it->m_blockIndex == -1) {
            continue;
        }

        data.insert(data.end(), it->m_name, it->m_name + (it->m_name ? strlen(it->m_name) : 0));
        data.push_back('\r');
        data.push_back('\n');
    }

    uint32_t size = static_cast<uint32_t>(data.size());
    Blizzard::Mopaq::HSFILERecord* file;

    if (!Blizzard::Mopaq::SFileCreateFile(archive, "(listfile)", size, 0x200, &file)) {
        fputs("*** WriteArchiveChanges: creating listfile failed\n", stderr);
        return false;
    }

    if (!data.empty() && !file->Write(&data[0], 0, size)) {
        fputs("*** WriteArchiveChanges: writing listfile failed\n", stderr);
        result = false;
    }

    Blizzard::Mopaq::SFileCloseFile(file);

    return result;
}

// OFFSET: 0x007F5FF0/0x140504540
static bool WriteAttributeFile(System_SFile::ARCHIVEREC* archive, bool result) {
    uint32_t count = static_cast<uint32_t>(archive->m_blockTable.size());
    uint32_t header[2];
    header[0] = 100;
    header[1] = 0;

    std::vector<uint32_t> crcs(count + 1);
    std::vector<uint64_t> fileTimes(count + 1);
    std::vector<Blizzard::Util::Digest<16> > md5s(count + 1);
    std::vector<uint8_t> patchBits((count + 7) >> 3);
    memset(&patchBits[0], 0, (count + 7) >> 3);

    for (uint32_t i = 0; i < count; i++) {
        crcs[i] = archive->m_blockTable[i].m_crc;

        if (crcs[i]) {
            header[1] |= 1;
        }

        fileTimes[i] = archive->m_blockTable[i].m_fileTime;

        if (fileTimes[i]) {
            header[1] |= 2;
        }

        md5s[i] = archive->m_blockTable[i].m_md5;

        if (md5s[i] != System_SFile::s_emptyMd5) {
            header[1] |= 4;
        }

        if (archive->m_blockTable[i].m_patch) {
            static const uint8_t s_bits[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
            patchBits[i >> 3] |= s_bits[i & 7];
            header[1] |= 8;
        }
    }

    crcs.back() = 0;
    fileTimes.back() = 0;
    md5s.back() = System_SFile::s_emptyMd5;

    if (!header[1]) {
        return result;
    }

    uint32_t size = static_cast<uint32_t>(patchBits.size() + 4 * (crcs.size() + 2 * (fileTimes.size() + 2 * md5s.size()) + 2));
    Blizzard::Mopaq::HSFILERecord* file;

    if (!Blizzard::Mopaq::SFileCreateFile(archive, "(attributes)", size, 0x200, &file)) {
        fputs("*** WriteAttributeFile: creating attributes file failed\n", stderr);
        return false;
    }

    int64_t position = file->m_handle ? file->m_handle->m_position : -1;

    if (file->Write(header, position, 8)) {
        if (file->m_handle) {
            file->m_handle->m_position = position + 8;
        }
    } else {
        fputs("*** WriteAttributeFile: writing attributes file header failed\n", stderr);
        result = false;
    }

    if ((header[1] & 1) && !Blizzard::Mopaq::SFileWriteFile(file, &crcs[0], static_cast<uint32_t>(4 * crcs.size()))) {
        fputs("*** WriteAttributeFile: writing attributes file CRC block failed\n", stderr);
        result = false;
    }

    if ((header[1] & 2) && !Blizzard::Mopaq::SFileWriteFile(file, &fileTimes[0], static_cast<uint32_t>(8 * fileTimes.size()))) {
        fputs("*** WriteAttributeFile: writing attributes file time block failed\n", stderr);
        result = false;
    }

    if ((header[1] & 4) && !Blizzard::Mopaq::SFileWriteFile(file, &md5s[0], static_cast<uint32_t>(16 * md5s.size()))) {
        fputs("*** WriteAttributeFile: writing attributes file MD5 block failed\n", stderr);
        result = false;
    }

    if ((header[1] & 8) && !Blizzard::Mopaq::SFileWriteFile(file, &patchBits[0], static_cast<uint32_t>(patchBits.size()))) {
        fputs("*** WriteAttributeFile: writing attributes file deltaFlag block failed\n", stderr);
        result = false;
    }

    Blizzard::Mopaq::SFileCloseFile(file);

    return result;
}

// OFFSET: 0x007B9C20
static bool WriteTableMD5s(Blizzard::Mopaq::HSFILERecord* file, Blizzard::Util::Buffer<1024, char>& buffer, int64_t offset, int64_t size, int64_t chunkSize) {
    int32_t chunks = static_cast<int32_t>((size - 1) / static_cast<int32_t>(chunkSize)) + 1;
    int32_t remaining = static_cast<int32_t>(size);

    for (int32_t i = 0; i < chunks; i++) {
        int32_t length = static_cast<int32_t>(chunkSize) >= remaining ? remaining : static_cast<int32_t>(chunkSize);

        Blizzard::Util::MD5 md5;
        md5.Process(buffer.m_data + i * static_cast<int32_t>(chunkSize), length);

        Blizzard::Util::Digest<16> digest;
        md5.Finish(digest.m_data);
        memcpy(buffer.m_data + 16 * i, digest.m_data, 16);

        remaining -= length;
    }

    return file->Write(buffer.m_data, offset, 16 * chunks);
}

// OFFSET: 0x007B0F70
uint32_t System_SFile::BlockEntryTable::GetBufferSize() {
    uint32_t bitsSize;

    if (this->m_entries) {
        bitsSize = ((this->m_entries->m_bitCount + 7) >> 3) + ((this->m_hashes->m_bitCount + 7) >> 3);
    } else {
        bitsSize = (this->m_hashes->m_bitCount + 7) >> 3;
    }

    return 4 * static_cast<uint32_t>(this->m_flagTable.size()) + sizeof(BlockTableHeader) + bitsSize;
}

// OFFSET: 0x007B91C0
bool System_SFile::BlockEntryTable::SaveBlockTableToBuffer(char* buffer, uint32_t size) {
    if (!buffer || size < this->GetBufferSize()) {
        return false;
    }

    BlockTableHeader header;
    header.m_tableSize = this->GetBufferSize();
    header.unk_0008 = this->m_tableFlags;
    memcpy(&header.m_entryBits, &this->m_entryBits, 0x2C);
    header.m_count = this->m_count;
    header.m_flagCount = static_cast<uint32_t>(this->m_flagTable.size());
    header.m_hashesSize = (this->m_hashes->m_bitCount + 7) >> 3;
    header.m_hashEntryBits = this->m_hashEntryBits;
    header.m_hashExtraBits = this->m_hashExtraBits;
    header.m_hashBits = this->m_hashBits;
    memcpy(buffer, &header, sizeof(header));

    char* data = buffer + sizeof(header);

    for (uint32_t i = 0; i < this->m_flagTable.size(); i++) {
        *reinterpret_cast<uint32_t*>(data) = this->m_flagTable[i];
        data += 4;
    }

    memcpy(data, this->m_entries->m_data, (this->m_entries->m_bitCount + 7) >> 3);
    data += (this->m_entries->m_bitCount + 7) >> 3;
    memcpy(data, this->m_hashes->m_data, (this->m_hashes->m_bitCount + 7) >> 3);

    return true;
}

// OFFSET: 0x007B9620
bool System_SFile::HashEntryTable::SaveHashEntryTableToBuffer(char* buffer, uint32_t size) {
    if (!buffer) {
        return false;
    }

    uint32_t needed = ((this->m_indices->m_bitCount + 7) >> 3) + this->m_slotCount + sizeof(HashTableHeader);

    if (size < needed) {
        return false;
    }

    HashTableHeader header;
    header.m_tableSize = needed;
    header.m_hashBits = this->m_hashBits;
    memcpy(&header.m_entryBits, this, 0xC);
    header.m_indicesSize = (this->m_indices->m_bitCount + 7) >> 3;
    header.m_count = this->m_count;
    header.m_tagsSize = this->m_slotCount;
    memcpy(buffer, &header, sizeof(header));

    char* data = buffer + sizeof(header);
    memcpy(data, this->m_tags, this->m_slotCount);
    data += this->m_slotCount;
    memcpy(data, this->m_indices->m_data, (this->m_indices->m_bitCount + 7) >> 3);

    return true;
}

// OFFSET: 0x007E6AF0/0x1404F08A0
bool System_SFile::ARCHIVEREC::WriteBitPackedHashTable() {
    Blizzard::Util::Buffer<1024, char> buffer;
    HashEntryTable* table = this->m_hashEntryTable;
    uint32_t size = ((table->m_indices->m_bitCount + 7) >> 3) + table->m_slotCount + sizeof(HashTableHeader);
    bool result = true;

    ExtTableHeader header;
    header.m_signature = 0x1A544548;
    header.m_version = 1;
    header.m_dataSize = size;

    buffer.Resize(size + sizeof(header));
    uint32_t dataSize = size;

    if (!table->SaveHashEntryTableToBuffer(buffer.m_data + sizeof(header), size)) {
        fputs("*** WriteArchiveChanges: SaveHashEntryTableToBuffer failed.\n", stderr);
        result = false;
    }

    if (this->m_compressTables) {
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        bool checksum;
        System_SComp::Compress(buffer.m_data + sizeof(header), &dataSize, &checksum, buffer.m_data + sizeof(header), size, 1, &algorithm);
        size = dataSize;
    }

    System_MPQEncrypt::Encrypt(reinterpret_cast<uint32_t*>(buffer.m_data + sizeof(header)), size, 0xC3AF3770);

    uint32_t total = size + sizeof(header);
    this->m_header.m_hetTablePos = this->RequestWriteReservation(total, false);
    this->m_header.m_hetTableSize64 = total;
    memcpy(buffer.m_data, &header, sizeof(header));

    if (!this->m_file->Write(buffer.m_data, this->m_header.m_hetTablePos, total)) {
        fputs("*** WriteArchiveChangesNew: writing hash table failed\n", stderr);
        result = false;
    }

    Blizzard::Util::MD5 md5;
    md5.Process(buffer.m_data, total);
    md5.Finish(this->m_header.m_hetTableMd5.m_data);

    if (result && this->m_header.m_rawChunkSize > 0) {
        result = WriteTableMD5s(this->m_file, buffer, this->m_header.m_hetTableSize64 + this->m_header.m_hetTablePos, this->m_header.m_hetTableSize64, this->m_header.m_rawChunkSize);
    }

    return result;
}

// OFFSET: 0x007E6D40/0x1404F0BD0
bool System_SFile::ARCHIVEREC::WriteBitPackedBlockTable() {
    Blizzard::Util::Buffer<1024, char> buffer;
    BlockEntryTable* table = this->m_blockEntryTable;
    bool result = true;
    uint32_t entriesSize = table->m_entries ? (table->m_entries->m_bitCount + 7) >> 3 : 0;
    uint32_t size = 4 * static_cast<uint32_t>(table->m_flagTable.size()) + sizeof(BlockTableHeader) + entriesSize + ((table->m_hashes->m_bitCount + 7) >> 3);

    ExtTableHeader header;
    header.m_signature = 0x1A544542;
    header.m_version = 1;
    header.m_dataSize = size;

    buffer.Resize(size + sizeof(header));
    uint32_t dataSize = size;

    if (!table->SaveBlockTableToBuffer(buffer.m_data + sizeof(header), size)) {
        fputs("*** WriteArchiveChanges: SaveBlockTableToBuffer failed.\n", stderr);
        result = false;
    }

    if (this->m_compressTables) {
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        bool checksum;
        System_SComp::Compress(buffer.m_data + sizeof(header), &dataSize, &checksum, buffer.m_data + sizeof(header), size, 1, &algorithm);
        size = dataSize;
    }

    System_MPQEncrypt::Encrypt(reinterpret_cast<uint32_t*>(buffer.m_data + sizeof(header)), size, 0xEC83B3A3);

    uint32_t total = size + sizeof(header);
    this->m_header.m_betTablePos = this->RequestWriteReservation(total, false);
    this->m_header.m_betTableSize64 = total;
    memcpy(buffer.m_data, &header, sizeof(header));

    if (!this->m_file->Write(buffer.m_data, this->m_header.m_betTablePos, total)) {
        fputs("*** WriteArchiveChangesNew: writing block table failed\n", stderr);
        result = false;
    }

    Blizzard::Util::MD5 md5;
    md5.Process(buffer.m_data, total);
    md5.Finish(this->m_header.m_betTableMd5.m_data);

    if (result && this->m_header.m_rawChunkSize > 0) {
        result = WriteTableMD5s(this->m_file, buffer, this->m_header.m_betTableSize64 + this->m_header.m_betTablePos, this->m_header.m_betTableSize64, this->m_header.m_rawChunkSize);
    }

    return result;
}

// OFFSET: 0x007E6FB0/0x1404F0F20
bool System_SFile::ARCHIVEREC::WriteHashTable() {
    uint32_t count = static_cast<uint32_t>(this->m_hashTable.size());
    uint32_t size = 16 * count;
    int64_t reserved = size;
    bool result = true;
    int64_t position = this->RequestWriteReservation(size, true);
    HASHENTRY* table = static_cast<HASHENTRY*>(Blizzard::Memory::Allocate(size));

    for (uint32_t i = 0; i < count; i++) {
        table[i].m_hashA = this->m_hashTable[i].m_hashA;
        table[i].m_hashB = this->m_hashTable[i].m_hashB;
        table[i].m_locale = this->m_hashTable[i].m_locale;
        table[i].m_platform = this->m_hashTable[i].m_platform;
        table[i].unk_000B = this->m_hashTable[i].unk_000B;
        table[i].m_blockIndex = this->m_hashTable[i].m_blockIndex;
    }

    if (this->m_compressTables) {
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        bool checksum;
        System_SComp::Compress(table, &size, &checksum, table, size, 1, &algorithm);
    }

    System_MPQEncrypt::Encrypt(reinterpret_cast<uint32_t*>(table), size, 0xC3AF3770);

    if (this->m_file->Write(table, position, size)) {
        Blizzard::Util::MD5 md5;
        md5.Process(table, size);
        md5.Finish(this->m_header.m_hashTableMd5.m_data);
        this->ReturnWriteReservation(position, reserved, size, true);

        int64_t relative = position - this->m_archiveOffset;
        this->m_header.m_hashTablePos = static_cast<uint32_t>(relative);
        this->m_header.m_hashTableSize = count;
        this->m_header.m_hashTablePosHi = static_cast<uint16_t>(relative >> 32);
        this->m_header.m_hashTableSize64 = size;
    } else {
        fputs("*** WriteArchiveChanges: writing hash table failed\n", stderr);
        result = false;
    }

    Blizzard::Memory::Free(table);

    return result;
}

// OFFSET: 0x007E7310/0x1404F1320
bool System_SFile::ARCHIVEREC::WriteBlockTable(bool& extended, bool reserve) {
    extended = false;

    uint32_t count = static_cast<uint32_t>(this->m_blockTable.size());
    uint32_t size = 16 * count;
    int64_t reserved = size;
    bool result = true;
    int64_t position = (static_cast<int64_t>(this->m_header.m_blockTablePosHi) << 32) | this->m_header.m_blockTablePos;

    if (reserve) {
        position = this->RequestWriteReservation(size, true);
    }

    uint32_t* table = static_cast<uint32_t*>(Blizzard::Memory::Allocate(size));

    for (uint32_t i = 0; i < count; i++) {
        int64_t offset = this->m_blockTable[i].m_offset - this->m_archiveOffset;

        if (offset > 0xFFFFFFFFLL) {
            extended = true;
        }

        table[4 * i + 0] = static_cast<uint32_t>(offset);
        table[4 * i + 1] = static_cast<uint32_t>(this->m_blockTable[i].m_compressedSize);
        table[4 * i + 2] = static_cast<uint32_t>(this->m_blockTable[i].m_fileSize);
        table[4 * i + 3] = this->m_blockTable[i].m_flags;
    }

    if (this->m_compressTables) {
        Blizzard::Mopaq::CompressionAlgorithm algorithm;
        algorithm.m_type = 2;
        algorithm.m_level = 0;
        bool checksum;
        System_SComp::Compress(table, &size, &checksum, table, size, 1, &algorithm);
    }

    System_MPQEncrypt::Encrypt(table, size, 0xEC83B3A3);

    if (reserve || static_cast<int64_t>(size) <= static_cast<int64_t>(this->m_header.m_blockTableSize64)) {
        if (this->m_file->Write(table, position + this->m_archiveOffset, size)) {
            if (reserve) {
                Blizzard::Util::MD5 md5;
                md5.Process(table, size);
                md5.Finish(this->m_header.m_blockTableMd5.m_data);
                this->ReturnWriteReservation(position, reserved, size, true);

                int64_t relative = position - this->m_archiveOffset;
                this->m_header.m_blockTablePos = static_cast<uint32_t>(relative);
                this->m_header.m_blockTableSize = count;
                this->m_header.m_blockTableSize64 = size;
                this->m_header.m_blockTablePosHi = static_cast<uint16_t>(relative >> 32);
                this->m_header.m_archiveSize64 = this->m_archiveOffset + ((static_cast<uint64_t>(this->m_header.m_blockTablePosHi) << 32) | size) + this->m_header.m_blockTablePos;
            }
        } else {
            fputs("*** WriteArchiveChanges: writing block table failed\n", stderr);
            result = false;
        }
    } else {
        result = false;
    }

    Blizzard::Memory::Free(table);

    if (result && extended && !this->m_header.m_formatVersion) {
        fputs("*** WriteBlockTable: archive is unable to support 64 bit offsets\n", stderr);
        return false;
    }

    return result;
}

// OFFSET: 0x007B9D70/0x1404B13D0
bool System_SFile::ARCHIVEREC::WriteExtendedBlockTable(bool reserve) {
    uint32_t count = static_cast<uint32_t>(this->m_blockTable.size());
    uint32_t size = 2 * count;
    bool result = true;

    if (reserve) {
        this->m_header.m_hiBlockTablePos = this->RequestWriteReservation(size, true);
    }

    uint16_t* table = static_cast<uint16_t*>(Blizzard::Memory::Allocate(size));

    for (uint32_t i = 0; i < count; i++) {
        table[i] = static_cast<uint16_t>(static_cast<int64_t>(this->m_blockTable[i].m_offset) >> 32);
    }

    int64_t available = static_cast<int64_t>(this->m_header.m_hiBlockTableSize64);

    if (reserve || available >> 32 > 0 || (available >= 0 && size <= static_cast<uint32_t>(available))) {
        if (this->m_file->Write(table, this->m_header.m_hiBlockTablePos, size)) {
            if (reserve) {
                Blizzard::Util::MD5 md5;
                md5.Process(table, size);
                md5.Finish(this->m_header.m_hiBlockTableMd5.m_data);
                this->ReturnWriteReservation(this->m_header.m_hiBlockTablePos, size, size, true);
                this->m_header.m_hiBlockTableSize64 = size;
                this->m_header.m_archiveSize64 = this->m_header.m_hiBlockTablePos + size;
            }
        } else {
            fputs("*** WriteArchiveChanges: writing extended block table failed\n", stderr);
            result = false;
        }
    } else {
        result = false;
    }

    Blizzard::Memory::Free(table);

    return result;
}

// OFFSET: 0x007F68D0/0x1405054A0
bool System_SFile::ARCHIVEREC::WriteArchiveChanges(ARCHIVEREC* archive) {
    archive->m_locale = 0;
    archive->m_platform = 0;
    archive->m_attributeFlags = 1;
    archive->CompactBlockTable();

    std::vector<char> listfile;
    bool result = WriteListfile(listfile, archive);
    result = WriteAttributeFile(archive, result);
    RebuildHashTable(archive, GetHashTableSize(archive));

    if (archive->m_header.m_formatVersion >= 2 && !listfile.empty()) {
        archive->ConvertToNewBlockTableFormat(false);
        archive->ConvertToNewHashTableFormat(&listfile[0], static_cast<int32_t>(listfile.size()), false);

        if (result) {
            if (archive->m_hashEntryTable) {
                result = archive->WriteBitPackedHashTable();
            }

            if (result && archive->m_blockEntryTable) {
                result = archive->WriteBitPackedBlockTable();
            }
        }
    }

    if (result) {
        result = archive->WriteHashTable();
    }

    bool extended = false;

    if (result) {
        result = archive->WriteBlockTable(extended, true);

        if (result && extended) {
            result = archive->WriteExtendedBlockTable(true);
        }
    }

    if (!archive->WriteArchiveHeader()) {
        fputs("*** WriteArchiveChanges: writing archive header failed\n", stderr);
        result = false;
    }

    if (archive->m_setFileSize) {
        int64_t end = archive->m_header.m_archiveSize64 + archive->m_archiveOffset;

        if (archive->m_minFileSize > end) {
            end = archive->m_minFileSize;
        }

        result = archive->m_file->SetFileSize(end);
    }

    if (archive->m_file->m_handle && !archive->m_file->Flush()) {
        fputs("*** WriteArchiveChanges: flushing archive failed\n", stderr);
        result = false;
    }

    archive->ReleaseListfile();

    if (archive->m_listfileRefcount > 0) {
        fputs("*** WriteArchiveChanges: listfile was Acquired more than Released\n", stderr);
    }

    if (archive->m_header.m_formatVersion >= 2) {
        std::vector<BLOCKENTRY>().swap(archive->m_blockTable);
        std::vector<HASHENTRY>().swap(archive->m_hashTable);
    }

    return result;
}

// OFFSET: 0x007F9D00/0x1405099F0
bool System_SFile::ARCHIVEREC::InitPatchMetadata() {
    Blizzard::Lock::ScopedMutex lock(this->m_patchLock.m_mutex);

    if (!this->m_patchMetadataLoaded) {
        this->InitializeTables();

        Blizzard::Util::Digest<16> md5 = s_emptyMd5;
        Blizzard::Mopaq::HSFILERecord* file = NULL;

        if (Blizzard::Mopaq::SFileOpenFileEx(this, "(patch_metadata)", md5, 16, &file)) {
            uint32_t size = static_cast<uint32_t>(file->GetFileSize());
            this->m_patchMetadata.SetCount(size + 1);

            if (Blizzard::Mopaq::SFileReadFile(file, this->m_patchMetadata.m_data, size, NULL)) {
                this->m_patchMetadata.m_data[size] = 0;
                this->m_patchMetadataLoaded = true;
            } else {
                this->m_patchMetadata.SetCount(0);
            }

            System_Mopaq::CloseFileRecord(file);
        }
    }

    return this->m_patchMetadataLoaded;
}

// PROVEN: 154b4f84 IDENTICAL/IDENTICAL
// OFFSET: 0x007AAB50/0x1404A0E80
Blizzard::Mopaq::ArchiveOptions::ArchiveOptions() {
    this->m_sectorSize = 0x4000;
    this->unk_0004 = 0;
    this->m_compressTables = false;
    this->unk_0009 = false;
    this->m_rawChunkSize = 0;
    this->m_setFileSize = true;
    this->m_minFileSize = 0;
}

// OFFSET: 0x007F3140/0x140500450
uint32_t Blizzard::Mopaq::SFileGetArchiveBlockCount(System_SFile::ARCHIVEREC* handle) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (!archive) {
        return 0;
    }

    uint32_t count = archive->m_header.m_blockTableSize;
    archive->RemoveInternalArchiveRef();

    return count;
}

// OFFSET: 0x007F2590
bool Blizzard::Mopaq::SFileGetArchiveFile(System_SFile::ARCHIVEREC* handle, HSFILERecord** file) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (!archive) {
        return false;
    }

    HSFILERecord* record = new HSFILERecord;

    if (record) {
        MpqHandleRecord* mpq = archive->m_file->m_handle;
        record->m_handle = mpq;

        if (mpq) {
            mpq->m_refCount.Increment();
        }
    }

    *file = record;
    archive->RemoveInternalArchiveRef();

    return true;
}

// OFFSET: 0x007F3010/0x140500340
int64_t Blizzard::Mopaq::SFileGetStreamedFileSize(System_SFile::ARCHIVEREC* handle, uint32_t blockIndex) {
    System_SFile::ArchivePtr archive(handle);

    if (!archive.m_archive) {
        return 0;
    }

    while (archive.m_archive->m_parent) {
        archive.Reset(archive.m_archive->m_parent);
    }

    System_SFile::BLOCKENTRY entry = archive.m_archive->GetBlockEntry(blockIndex);
    int32_t chunkSize = archive.m_archive->m_header.m_rawChunkSize;
    uint32_t size = static_cast<uint32_t>(entry.m_compressedSize);

    if (chunkSize > 0) {
        size += 16 * ((size + chunkSize - 1) / chunkSize);
    }

    return size;
}

// OFFSET: 0x007F4560/0x1405020C0
bool Blizzard::Mopaq::IsArchiveComplete(System_SFile::ARCHIVEREC* handle) {
    System_SFile::ArchivePtr archive(handle);

    if (!archive.m_archive) {
        return false;
    }

    int64_t fileSize = archive.m_archive->m_file->GetFileSize();
    int64_t archiveSize = static_cast<int64_t>(archive.m_archive->m_header.m_archiveSize64);

    if (archiveSize == fileSize) {
        return true;
    }

    if (archiveSize > fileSize) {
        return false;
    }

    if (archive.m_archive->m_flags & 0x10) {
        return true;
    }

    MpqHandleRecord* fileHandle = archive.m_archive->m_file->m_handle;
    Blizzard::File::StreamRecord* stream = fileHandle ? fileHandle->m_file : NULL;

    return Blizzard::Storage::PartialFile::FileIsCompletePartialFile(stream, archiveSize, fileSize);
}

// OFFSET: 0x007FA940
bool Blizzard::Mopaq::SFileGetPatchMetadata(System_SFile::ARCHIVEREC* handle, Blizzard::Util::Buffer<4096, char>& metadata) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);
    bool result = false;

    if (archive) {
        result = archive->InitPatchMetadata();

        if (result) {
            metadata.Resize(archive->m_patchMetadata.m_count);
            memcpy(metadata.m_data, archive->m_patchMetadata.m_data, metadata.m_count);
        }
    }

    if (archive) {
        archive->RemoveInternalArchiveRef();
    }

    return result;
}

// OFFSET: 0x007F2C80/0x1404FFEA0
int32_t Blizzard::Mopaq::SFileDeleteFile(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags) {
    if ((!handle && !(flags & 8)) || (flags & 2)) {
        System_SFile::s_lastError = 87;
        return 0;
    }

    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    System_SFile::ARCHIVEREC* archiveOut = NULL;
    System_SFile::ARCHIVEREC* record = NULL;
    int32_t result = 0;
    char path[260];

    switch (GetFileBlockEntry(handle, name, md5, flags, &archiveOut, &record, NULL, path)) {
    case 0:
    case 4:
    case 5:
        System_SFile::s_lastError = 2;
        result = 0;
        break;
    case 1: {
        System_SFile::ArchivePtr archive(archiveOut);
        char joined[260];
        const char* fileName = name;

        if (archive.m_archive && !archive.m_archive->m_directPath.empty()) {
            Blizzard::String::JoinPath(joined, sizeof(joined), archive.m_archive->m_directPath.c_str(), name);
            fileName = joined;
        }

        uint32_t hashIndex;

        if (!System_SFile::PrepareArchiveForWriting(record) || (hashIndex = System_SFile::SearchHashTable(record, fileName, true)) == -1) {
            result = 0;
            System_SFile::s_lastError = 104;
        } else {
            DeleteHashEntry(record, hashIndex, fileName);
            result = 1;
        }

        break;
    }
    case 2:
    case 3:
        if (Blizzard::File::Delete(path)) {
            result = 2;
        } else {
            result = 0;
            System_SFile::s_lastError = 104;
        }

        break;
    default:
        break;
    }

    if (record) {
        record->RemoveInternalArchiveRef();
    }

    return result;
}
