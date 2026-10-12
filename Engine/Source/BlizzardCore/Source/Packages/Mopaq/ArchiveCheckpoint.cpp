#include <cstdint>
#include <cstring>
#include <string>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/ArchiveCheckpoint.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Buffer.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"

static const char* s_extension = ".writeCacheBackup";

struct RestorePointHeader {
    /* 0000 */ int64_t m_headerOffset;
    /* 0008 */ uint32_t m_headerSize;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ int64_t m_tailOffset;
    /* 0018 */ uint32_t m_tailSize;
    /* 001C */ uint32_t unk_001C;
};

struct RestorePointFile {
    /* 0000 */ Blizzard::Mopaq::HSFILERecord* m_archive;
    /* 0004 */ Blizzard::File::StreamRecord* m_file;

    RestorePointFile(Blizzard::Mopaq::HSFILERecord* archive, Blizzard::File::StreamRecord* file);
    bool Read(void* buffer, int64_t offset, uint32_t size);
    bool Write(const void* buffer, int64_t offset, uint32_t size);
};

struct RestorePointBuffer : public RestorePointFile {
    /* 0008 */ Blizzard::Util::Buffer<4096, char> m_buffer;

    RestorePointBuffer(Blizzard::File::StreamRecord* file);
    RestorePointBuffer(Blizzard::Mopaq::HSFILERecord* archive, Blizzard::File::StreamRecord* file);
    bool ComputeMD5(uint32_t size, Blizzard::Util::Digest<16>& md5);
    bool VerifyMD5(uint32_t size, const Blizzard::Util::Digest<16>& md5);
};

struct RestorePointCopier {
    /* 0000 */ RestorePointFile m_source;
    /* 0008 */ RestorePointBuffer m_target;

    RestorePointCopier(Blizzard::Mopaq::HSFILERecord* source, Blizzard::File::StreamRecord* target);
    RestorePointCopier(Blizzard::File::StreamRecord* source, Blizzard::Mopaq::HSFILERecord* target);
    bool Copy(int64_t source, int64_t target, uint32_t size);
};

// OFFSET: 0x007B8800
bool RestorePointFile::Read(void* buffer, int64_t offset, uint32_t size) {
    int32_t count = size;
    bool result;

    if (this->m_file) {
        result = Blizzard::File::Read(this->m_file, buffer, offset, &count, 0);
    } else {
        result = this->m_archive->Read(buffer, offset, size, &count);
    }

    return result && count == static_cast<int32_t>(size);
}

// OFFSET: 0x007B87A0
bool RestorePointFile::Write(const void* buffer, int64_t offset, uint32_t size) {
    if (this->m_archive) {
        return this->m_archive->Write(buffer, offset, size);
    }

    int32_t count = size;

    return Blizzard::File::Write(this->m_file, buffer, offset, &count, 0) && count == static_cast<int32_t>(size);
}

// INLINED
RestorePointFile::RestorePointFile(Blizzard::Mopaq::HSFILERecord* archive, Blizzard::File::StreamRecord* file) : m_archive(archive), m_file(file) {
}

// OFFSET: 0x007B88F0
RestorePointBuffer::RestorePointBuffer(Blizzard::File::StreamRecord* file) : RestorePointFile(NULL, file) {
}

// INLINED
RestorePointBuffer::RestorePointBuffer(Blizzard::Mopaq::HSFILERecord* archive, Blizzard::File::StreamRecord* file) : RestorePointFile(archive, file) {
}

// OFFSET: 0x007B8920
bool RestorePointBuffer::ComputeMD5(uint32_t size, Blizzard::Util::Digest<16>& md5) {
    Blizzard::Util::MD5 context;
    this->m_buffer.Resize(size);

    if (!this->Read(this->m_buffer.m_data, 0, size)) {
        return false;
    }

    context.Process(this->m_buffer.m_data, size);
    context.Finish(md5.m_data);

    return true;
}

// OFFSET: 0x007B89A0
bool RestorePointBuffer::VerifyMD5(uint32_t size, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Util::Digest<16> computed;

    if (!this->ComputeMD5(size, computed)) {
        return false;
    }

    return md5 == computed;
}

// INLINED
RestorePointCopier::RestorePointCopier(Blizzard::Mopaq::HSFILERecord* source, Blizzard::File::StreamRecord* target) : m_source(source, NULL), m_target(NULL, target) {
}

// OFFSET: 0x007B8860
RestorePointCopier::RestorePointCopier(Blizzard::File::StreamRecord* source, Blizzard::Mopaq::HSFILERecord* target) : m_source(NULL, source), m_target(target, NULL) {
}

// OFFSET: 0x007B8890
bool RestorePointCopier::Copy(int64_t source, int64_t target, uint32_t size) {
    bool result = false;
    this->m_target.m_buffer.Resize(size);

    if (this->m_source.Read(this->m_target.m_buffer.m_data, source, size)) {
        result = this->m_target.Write(this->m_target.m_buffer.m_data, target, size);
    }

    return result;
}

// OFFSET: 0x007B9950/0x1404B0AD0
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

// OFFSET: 0x007B9A00/0x1404B0BA0
static int64_t GetTailOffset(System_SFile::ARCHIVEREC* archive) {
    int64_t attributes = GetSpecialFileOffset("(attributes)", archive);
    int64_t listfile = GetSpecialFileOffset("(listfile)", archive);
    int64_t offset = listfile >= attributes ? attributes : listfile;
    int64_t signature = GetSpecialFileOffset("(signature)", archive);

    return signature >= offset ? offset : signature;
}

// OFFSET: 0x007CBA90
bool ArchiveCheckpoint::ApplyRestorePoint(Blizzard::Mopaq::HSFILERecord* file) {
    bool result = false;
    std::string path(file->m_handle ? file->m_handle->m_name : "");
    path.append(s_extension, strlen(s_extension));

    if (Blizzard::File::Exists(System_File::Stacked::s_manager, path.c_str())) {
        Blizzard::File::StreamRecord* stream;

        if (Blizzard::File::Open(System_File::Stacked::s_manager, path.c_str(), 0x1001, stream)) {
            RestorePointHeader header;
            memset(&header, 0, sizeof(header));
            int32_t count = sizeof(header);

            if (Blizzard::File::Read(stream, &header, 0, &count, 0)) {
                Blizzard::File::FileInfo info;
                Blizzard::File::GetFileInfo(stream, &info);

                int32_t size = header.m_tailSize + header.m_headerSize + sizeof(header);
                Blizzard::Util::Digest<16> md5;

                if (info.m_size == size + 16 && Blizzard::File::ReadExact(stream, &md5, size, 16, 0)) {
                    RestorePointBuffer verifier(stream);

                    if (verifier.VerifyMD5(size, md5)) {
                        RestorePointCopier copier(stream, file);

                        if (copier.Copy(header.m_headerSize + sizeof(header), header.m_tailOffset, header.m_tailSize)
                            && copier.Copy(sizeof(header), header.m_headerOffset, header.m_headerSize)
                            && file->SetFileSize(header.m_tailOffset + header.m_tailSize)) {
                            result = file->Flush();
                        }
                    }
                }
            }

            Blizzard::File::Close(stream);
        }
    }

    if (Blizzard::File::Exists(System_File::Stacked::s_manager, path.c_str()) == 1) {
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

        RestorePointCopier copier(archive->m_file, stream);
        uint32_t size = 0;

        if (copier.Copy(header.m_headerOffset, sizeof(header), header.m_headerSize)) {
            size = header.m_headerSize;

            if (copier.Copy(header.m_tailOffset, header.m_headerSize + sizeof(header), header.m_tailSize)) {
                int32_t count = sizeof(header);
                result = Blizzard::File::Write(stream, &header, 0, &count, 0);
                size += header.m_tailSize + sizeof(header);
            }
        }

        Blizzard::Util::Digest<16> md5;

        if (result && copier.m_target.ComputeMD5(size, md5)) {
            int32_t count = sizeof(md5);
            result = Blizzard::File::Write(stream, &md5, size, &count, 0);
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
