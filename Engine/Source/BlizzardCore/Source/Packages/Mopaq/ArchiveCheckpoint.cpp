#include <cstdint>
#include <cstring>
#include <string>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/ArchiveCheckpoint.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"

// STATIC: %0x00C0AD4C
static const char* s_extension = ".writeCacheBackup";

struct RestorePointHeader {
    /* 0000 */ int64_t m_headerOffset;
    /* 0008 */ uint32_t m_headerSize;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ int64_t m_tailOffset;
    /* 0018 */ uint32_t m_tailSize;
    /* 001C */ uint32_t unk_001C;
};

struct RestorePointCopier {
    /* 0000 */ Blizzard::Mopaq::HSFILERecord* m_sourceArchive;
    /* 0004 */ Blizzard::File::StreamRecord* m_sourceFile;
    /* 0008 */ Blizzard::Mopaq::HSFILERecord* m_targetArchive;
    /* 000C */ Blizzard::File::StreamRecord* m_targetFile;
    /* 0010 */ Blizzard::Util::Buffer<4096, char> m_buffer;

    bool Copy(int64_t source, int64_t target, uint32_t size);
    bool Read(void* buffer, int64_t offset, uint32_t size);
    bool Write(const void* buffer, int64_t offset, uint32_t size);
};

// PROVEN: 6b5ea1be IDENTICAL/NONE
// OFFSET: 0x007B8800
bool RestorePointCopier::Read(void* buffer, int64_t offset, uint32_t size) {
    int32_t count = size;
    bool result;

    if (this->m_sourceFile) {
        result = Blizzard::File::Read(this->m_sourceFile, buffer, offset, &count, 0);
    } else {
        result = this->m_sourceArchive->Read(buffer, offset, size, &count);
    }

    return result && count == static_cast<int32_t>(size);
}

// OFFSET: 0x007B87A0
bool RestorePointCopier::Write(const void* buffer, int64_t offset, uint32_t size) {
    if (this->m_targetArchive) {
        return this->m_targetArchive->Write(buffer, offset, size);
    }

    int32_t count = size;

    return Blizzard::File::Write(this->m_targetFile, buffer, offset, &count, 0) && count == static_cast<int32_t>(size);
}

// OFFSET: 0x007B8890
bool RestorePointCopier::Copy(int64_t source, int64_t target, uint32_t size) {
    this->m_buffer.Resize(size);

    if (!this->Read(this->m_buffer.m_data, source, size)) {
        return false;
    }

    return this->Write(this->m_buffer.m_data, target, size);
}

// OFFSET: 0x007B9950
static int64_t GetSpecialFileOffset(const char* name, System_SFile::ARCHIVEREC* archive) {
    uint32_t blockIndex;

    if (archive->m_hashTable.size()) {
        uint32_t hashIndex = System_SFile::SearchHashTable(archive, name, true);

        if (hashIndex == -1) {
            return 0x7FFFFFFFFFFFFFFFLL;
        }

        blockIndex = archive->m_hashTable[hashIndex].m_blockIndex;
    } else {
        if (!archive->m_hashEntryTable) {
            return 0x7FFFFFFFFFFFFFFFLL;
        }

        uint64_t hash = System_SFile::HashEntryTable::CalculateHash(name);
        blockIndex = archive->m_hashEntryTable->GetEntry(hash, archive->m_blockEntryTable);
    }

    if (blockIndex == -1) {
        return 0x7FFFFFFFFFFFFFFFLL;
    }

    return archive->GetBlockEntry(blockIndex).m_offset;
}

// OFFSET: 0x007B9A00
static int64_t GetTailOffset(System_SFile::ARCHIVEREC* archive) {
    int64_t attributes = GetSpecialFileOffset("(attributes)", archive);
    int64_t listfile = GetSpecialFileOffset("(listfile)", archive);
    int64_t offset = listfile >= attributes ? attributes : listfile;
    int64_t signature = GetSpecialFileOffset("(signature)", archive);

    return signature >= offset ? offset : signature;
}

// OFFSET: %0x0072CDD0
bool ArchiveCheckpoint::ApplyRestorePoint(Blizzard::Mopaq::HSFILERecord* file) {
    bool result = false;
    std::string path(file->m_handle ? file->m_handle->m_name : "");
    path.append(s_extension, strlen(s_extension));

    Blizzard::File::StreamRecord* stream;

    if (!Blizzard::File::Open(System_File::Stacked::s_manager, path.c_str(), 0x1001, stream)) {
        return false;
    }

    RestorePointHeader header;
    memset(&header, 0, sizeof(header));
    int32_t count = sizeof(header);

    if (Blizzard::File::Read(stream, &header, 0, &count, 0)) {
        RestorePointCopier copier;
        copier.m_sourceArchive = NULL;
        copier.m_sourceFile = stream;
        copier.m_targetArchive = file;
        copier.m_targetFile = NULL;

        if (copier.Copy(header.m_headerSize + sizeof(header), header.m_tailOffset, header.m_tailSize)
            && copier.Copy(sizeof(header), header.m_headerOffset, header.m_headerSize)
            && file->SetFileSize(header.m_tailOffset + header.m_tailSize)) {
            result = file->Flush();
        }
    }

    Blizzard::File::Close(stream);

    if (result) {
        Blizzard::File::Delete(path.c_str());
    }

    return result;
}

// OFFSET: 0x007CBD60
bool ArchiveCheckpoint::CreateRestorePoint(const System_SFile::ARCHIVEREC* archive) {
    Blizzard::Mopaq::HSFILERecord* file = archive->m_file;

    if (!file || archive->m_closeCallback || (file->m_handle && !file->m_handle->m_name) || !(archive->m_flags & 0x10)) {
        return false;
    }

    std::string path(file->m_handle ? file->m_handle->m_name : "");
    path.append(s_extension, strlen(s_extension));

    Blizzard::File::StreamRecord* stream = NULL;
    bool result = false;

    if (Blizzard::File::Open(System_File::Stacked::s_manager, path.c_str(), 0xC03, stream)) {
        RestorePointHeader header;
        header.m_tailOffset = 0;
        header.m_tailSize = 0;

        if (!archive->m_header.m_formatVersion) {
            header.m_headerSize = 32;
        } else if (archive->m_header.m_formatVersion == 1) {
            header.m_headerSize = 44;
        } else {
            header.m_headerSize = 208;
        }

        header.m_headerOffset = archive->m_archiveOffset;
        header.m_tailOffset = GetTailOffset(const_cast<System_SFile::ARCHIVEREC*>(archive));

        if (header.m_tailOffset == 0x7FFFFFFFFFFFFFFFLL) {
            header.m_tailOffset = archive->m_archiveEnd;
        }

        header.m_tailSize = static_cast<uint32_t>(archive->m_archiveEnd) - static_cast<uint32_t>(header.m_tailOffset);

        RestorePointCopier copier;
        copier.m_sourceArchive = archive->m_file;
        copier.m_sourceFile = NULL;
        copier.m_targetArchive = NULL;
        copier.m_targetFile = stream;

        if (copier.Copy(header.m_headerOffset, sizeof(header), header.m_headerSize) && copier.Copy(header.m_tailOffset, header.m_headerSize + sizeof(header), header.m_tailSize)) {
            int32_t count = sizeof(header);
            result = Blizzard::File::Write(stream, &header, 0, &count, 0);
        }

        Blizzard::File::Close(stream);

        if (!result) {
            Blizzard::File::Delete(path.c_str());
        }
    }

    return result;
}

// OFFSET: 0x007CC050
void ArchiveCheckpoint::DeleteRestorePoint(const char* path) {
    std::string backup(path);
    backup.append(s_extension, strlen(s_extension));
    Blizzard::File::Delete(backup.c_str());
}
