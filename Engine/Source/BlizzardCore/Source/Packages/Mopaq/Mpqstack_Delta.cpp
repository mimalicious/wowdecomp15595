#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <vector>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_MD5Verify.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Patch/Patch.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

class MpqPatchStream {
    public:
    /* 0000 */ Blizzard::Mopaq::MpqHandleRecord* m_handle;
    /* 0004 */ Blizzard::Mopaq::Mpqsystem* m_system;

    bool Read(uint32_t offset, void* buffer, uint32_t size);
};

// STATIC: %0x00C0C354
System_Mopaq::DeltaData::ApplyPatchFunc System_Mopaq::DeltaData::ApplyPatch = Blizzard::Patch::Apply;

// STATIC: %0x00D86B9C
System_Mopaq::DeltaData::PatchCompletionFunc System_Mopaq::DeltaData::s_onPatchCompletion;

// INLINED
static void ComputeMD5(Blizzard::Util::Digest<16>* digest, const void* buffer, uint32_t size) {
    Blizzard::Util::MD5 md5;
    md5.Process(buffer, size);
    md5.Finish(digest->m_data);
}

// OFFSET: %0x00714A70
bool MpqPatchStream::Read(uint32_t offset, void* buffer, uint32_t size) {
    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_count = size;
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
    parms.m_offset = offset;
    parms.m_buffer = buffer;
    parms.m_handle = this->m_handle;

    if (!this->m_system) {
        return false;
    }

    return this->m_system->m_read(this->m_system, &parms);
}

// OFFSET: %0x00712EC0
bool Blizzard::Patch::MemoryStream::Read(uint32_t offset, void* buffer, uint32_t size) {
    if (offset + size > this->m_size) {
        return false;
    }

    memmove(buffer, this->m_data + offset, size);
    return true;
}

// PROVEN: 816b3f0a IDENTICAL/DIFFERENT
// OFFSET: 0x007AB220/0x1404A1370
System_Mopaq::DeltaData::DeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, const MpqDeltaMetadata* metadata) {
    this->m_deltaSize = metadata ? metadata->m_deltaSize : 0;
    this->m_fileSize = handle->m_blockEntry.m_fileSize;
    this->m_handle = handle;
    this->m_headerProcessed = false;
    this->m_appliedBuffer = NULL;
    this->m_appliedSize = 0;
    this->m_baseFile = NULL;
    this->m_offset = handle->m_blockEntry.m_offset;
    this->m_sizeBefore = 0;
    this->m_sizeAfter = 0;
    this->m_sourceMd5 = System_SFile::s_emptyMd5;
    this->m_targetMd5 = System_SFile::s_emptyMd5;
    this->m_md5 = metadata ? metadata->m_md5 : System_SFile::s_emptyMd5;
}

// OFFSET: %0x0072DD50
System_Mopaq::DeltaData::~DeltaData() {
    Blizzard::Memory::Free(this->m_appliedBuffer);

    if (this->m_baseFile) {
        if (this->m_baseFile->m_handle && !this->m_baseFile->m_handle->Release()) {
            this->m_baseFile->m_handle = NULL;
        }

        if (this->m_baseFile) {
            if (this->m_baseFile->m_handle && !this->m_baseFile->m_handle->Release()) {
                this->m_baseFile->m_handle = NULL;
            }

            delete this->m_baseFile;
        }
    }
}

// OFFSET: %0x0074E170
bool System_Mopaq::DeltaData::AdjustHandleBlockEntry() {
    if (!this->m_deltaSize) {
        Blizzard::String::QuickFormat<1024> message("%s - m_deltaSize is 0", "System_Mopaq::DeltaData::AdjustHandleBlockEntry");
        Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
        return false;
    }

    this->m_handle->m_blockEntry.m_offset = this->m_offset + 28;

    if (this->m_handle->m_handler) {
        this->m_handle->m_handler->UpdateBaseOffset();
    }

    this->m_handle->m_blockEntry.m_compressedSize -= 28;
    this->m_handle->m_blockEntry.m_fileSize = this->m_deltaSize;
    return true;
}

// OFFSET: %0x0074E240
bool System_Mopaq::InitializeDeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, const MpqDeltaMetadata* metadata) {
    void* memory = Blizzard::Memory::Allocate(sizeof(DeltaData));
    handle->m_delta = static_cast<DeltaData*>(memory);
    handle->m_delta = memory ? new (memory) DeltaData(handle, metadata) : NULL;

    if (metadata) {
        return handle->m_delta->AdjustHandleBlockEntry();
    }

    return true;
}

// OFFSET: %0x0070E1A0
bool System_Mopaq::DeltaData::IsLargeDelta(uint32_t sectorSize) {
    uint32_t deltaSize = this->m_deltaSize;
    return deltaSize > this->m_fileSize && deltaSize >= sectorSize && deltaSize / sectorSize > static_cast<uint32_t>(this->m_fileSize) / static_cast<uint64_t>(sectorSize);
}

// OFFSET: %0x00720CB0
bool System_Mopaq::DeltaData::InitMd5s(Blizzard::Mopaq::Mpqsystem* system) {
    if (this->m_sourceMd5 != System_SFile::s_emptyMd5) {
        return true;
    }

    MpqPatchStream stream;
    stream.m_handle = this->m_handle;
    stream.m_system = system->m_next;

    Blizzard::Patch::PatchStream<MpqPatchStream> patch;
    patch.m_stream = &stream;

    Blizzard::Patch::MD5Chunk chunk;
    chunk.m_tag = Blizzard::Patch::CHUNK_MD5;
    chunk.m_size = 40;

    uint32_t offset = 0;

    if (patch.Search(Blizzard::Patch::CHUNK_MD5, offset, chunk) && chunk.m_before != chunk.m_after) {
        this->m_sourceMd5 = chunk.m_before;
        this->m_targetMd5 = chunk.m_after;
        return true;
    }

    return false;
}

// OFFSET: %0x00759F10
bool System_Mopaq::DeltaData::OpenSrcFile() {
    if (this->m_baseFile) {
        return true;
    }

    System_SFile::ARCHIVEREC* archive = this->m_handle->m_logicalArchive;

    if (!archive) {
        archive = this->m_handle->m_archive;
    }

    if (Blizzard::Mopaq::SFileOpenFileEx(archive, this->m_handle->m_name, this->m_sourceMd5, 0x1000, &this->m_baseFile)) {
        return true;
    }

    Blizzard::String::QuickFormat<1024> message("%s - fail to open ", "System_Mopaq::DeltaData::OpenSrcFile");
    Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);
    return false;
}

// OFFSET: %0x0074E200
bool System_Mopaq::DeltaData::InitializeBaseMetadata() {
    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_baseFile->m_handle;

    if (!handle) {
        return true;
    }

    while (ReadAndInitializeMetadata(handle)) {
        DeltaData* delta = handle->m_delta;

        if (!delta) {
            return true;
        }

        Blizzard::Mopaq::HSFILERecord* base = delta->m_baseFile;

        if (!base) {
            break;
        }

        handle = base->m_handle;

        if (!handle) {
            return true;
        }
    }

    return false;
}

// OFFSET: %0x00714BF0
bool System_Mopaq::DeltaData::VerifyMd5(const void* buffer, uint32_t size, int32_t which) {
    Blizzard::Util::Digest<16> md5;
    ComputeMD5(&md5, buffer, size);

    switch (which) {
        case 0:
            return md5 == this->m_sourceMd5;
        case 1:
            return md5 == this->m_targetMd5;
        case 2:
            return md5 == this->m_md5;
    }

    return false;
}

// OFFSET: %0x0072D550
bool System_Mopaq::CloseFileRecord(Blizzard::Mopaq::HSFILERecord* file) {
    if (file) {
        if (file->m_handle && !file->m_handle->Release()) {
            file->m_handle = NULL;
        }

        file->m_handle = NULL;

        if (file->m_handle && !file->m_handle->Release()) {
            file->m_handle = NULL;
        }

        delete file;
    }

    return true;
}

// OFFSET: %0x007531B0
bool System_Mopaq::RepairFile(Blizzard::Mopaq::HSFILERecord* file) {
    if (!file->m_handle) {
        return false;
    }

    if (file->Repair()) {
        file->m_handle->m_signatureFailed = true;
        return true;
    }

    if (!PurgeCacheFile(file)) {
        return false;
    }

    Blizzard::Mopaq::MpqHandleRecord* handle = file->m_handle;
    Blizzard::Mopaq::HSFILERecord* fresh = NULL;
    const char* name = handle ? handle->m_name : "";

    if (!Blizzard::Mopaq::SFileOpenFileEx(handle->m_archive, name, handle->m_blockEntry.m_md5, 0x1000, &fresh)) {
        return false;
    }

    Blizzard::Mopaq::MpqHandleRecord* swap = fresh->m_handle;
    fresh->m_handle = file->m_handle;
    file->m_handle = swap;
    CloseFileRecord(fresh);
    file->m_handle->m_signatureFailed = true;
    return true;
}

// OFFSET: %0x00753980
bool System_Mopaq::DeltaData::ReadSource(void* buffer, uint32_t size) {
    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_baseFile->m_handle;

    if (handle) {
        MpqParams parms;
        memset(&parms, 0, sizeof(parms));
        parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
        parms.m_handle = handle;
        parms.m_buffer = buffer;
        parms.m_offset = 0;
        parms.m_count = size;

        if (handle->m_stack && handle->m_stack->m_read(handle->m_stack, &parms)) {
            Blizzard::Util::Digest<16> md5;
            ComputeMD5(&md5, buffer, size);

            if (md5 == this->m_sourceMd5) {
                return true;
            }
        }
    }

    Blizzard::String::QuickFormat<1024> message("%s - Read source error %u", "System_Mopaq::DeltaData::ReadSource", size);
    Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

    return RepairFile(this->m_baseFile) && this->m_baseFile->Read(buffer, 0, size, NULL) && this->VerifyMd5(buffer, size, 0);
}

// OFFSET: %0x007536A0
bool System_Mopaq::DeltaData::PatchLevel(Blizzard::Mopaq::Mpqsystem* system, uint8_t* dest, uint8_t* source) {
    uint8_t* delta = static_cast<uint8_t*>(Blizzard::Memory::Allocate(this->m_deltaSize));

    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_buffer = delta;
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
    parms.m_count = this->m_deltaSize;
    parms.m_offset = 0;
    parms.m_handle = this->m_handle;

    bool valid = false;

    if (system && system->m_read(system, &parms)) {
        Blizzard::Util::Digest<16> md5;
        ComputeMD5(&md5, parms.m_buffer, parms.m_count);
        valid = md5 == this->m_md5;
    }

    if (!valid) {
        Blizzard::String::QuickFormat<1024> message("%s - Read delta error %lld %u", "System_Mopaq::DeltaData::PatchLevel", this->m_offset, this->m_deltaSize);
        Blizzard::File::AddToLastErrorStack(104, message.m_buffer, 4);

        Blizzard::Mopaq::HSFILERecord file;
        file.m_handle = this->m_handle;

        if (file.m_handle) {
            file.m_handle->m_refCount.Increment();
        }

        if (RepairFile(&file) && system && (*reinterpret_cast<Blizzard::Mopaq::Mpqfunc*>(reinterpret_cast<uint8_t*>(system) + parms.m_verb))(system, &parms)) {
            Blizzard::Util::Digest<16> md5;
            ComputeMD5(&md5, parms.m_buffer, parms.m_count);
            valid = md5 == this->m_md5;
        }

        if (file.m_handle) {
            file.m_handle->Release();
        }

        if (!valid) {
            Blizzard::Memory::Free(delta);
            return false;
        }
    }

    uint32_t sourceSize = static_cast<uint32_t>(this->m_baseFile->GetFileSize());
    Blizzard::Mopaq::MpqHandleRecord* baseHandle = this->m_baseFile->m_handle;
    uint32_t baseCrc = baseHandle ? baseHandle->m_blockEntry.m_crc : 0;

    bool result = ApplyPatch(delta, this->m_deltaSize, dest, static_cast<uint32_t>(this->m_fileSize), source, sourceSize, baseCrc);

    if (result) {
        Blizzard::Util::Digest<16> md5;
        ComputeMD5(&md5, dest, static_cast<uint32_t>(this->m_fileSize));
        result = md5 == this->m_targetMd5;
    }

    Blizzard::Memory::Free(delta);
    return result;
}

// OFFSET: %0x0073A700
void System_Mopaq::DeltaData::CollectLevels(std::vector<DeltaData*>& levels) {
    levels.push_back(this);

    Blizzard::Mopaq::MpqHandleRecord* handle = this->m_baseFile->m_handle;

    while (handle) {
        DeltaData* delta = handle->m_delta;

        if (!delta) {
            break;
        }

        levels.push_back(delta);
        handle->m_blockEntry.m_crc = 0;

        handle = delta->m_baseFile->m_handle;
    }
}

// OFFSET: %0x00753AA0
bool System_Mopaq::DeltaData::PatchIntoBuffer(Blizzard::Mopaq::Mpqsystem* system, uint8_t* dest) {
    std::vector<DeltaData*> levels;
    this->CollectLevels(levels);

    void* source = NULL;
    bool result;

    if (levels.size() > 1) {
        uint8_t* patched = NULL;

        for (std::vector<DeltaData*>::iterator it = levels.end(); it != levels.begin(); ) {
            if (patched) {
                Blizzard::Memory::Free(source);
                source = patched;
            } else {
                DeltaData* last = *(it - 1);
                uint32_t size = static_cast<uint32_t>(last->m_baseFile->GetFileSize());
                source = Blizzard::Memory::Allocate(size);

                if (!last->ReadSource(source, size)) {
                    Blizzard::Memory::Free(NULL);
                    Blizzard::Memory::Free(source);
                    return false;
                }
            }

            --it;
            DeltaData* level = *it;
            patched = static_cast<uint8_t*>(Blizzard::Memory::Allocate(static_cast<uint32_t>(level->m_fileSize)));

            if (!level->PatchLevel(system, patched, static_cast<uint8_t*>(source))) {
                Blizzard::String::QuickFormat<1024> message("%s - PatchLevel error", "System_Mopaq::DeltaData::PatchIntoBuffer");
                Blizzard::File::AddToLastErrorStack(131, message.m_buffer, 4);
                Blizzard::Memory::Free(patched);
                Blizzard::Memory::Free(source);
                return false;
            }
        }

        memmove(dest, patched, static_cast<uint32_t>(this->m_fileSize));
        result = true;
        Blizzard::Memory::Free(patched);
    } else {
        uint32_t size = static_cast<uint32_t>(this->m_baseFile->GetFileSize());
        source = Blizzard::Memory::Allocate(size);

        if (!this->ReadSource(source, size)) {
            Blizzard::Memory::Free(source);
            return false;
        }

        result = this->PatchLevel(system, dest, static_cast<uint8_t*>(source));

        if (!result) {
            Blizzard::Memory::Free(source);
            return result;
        }
    }

    if (s_onPatchCompletion) {
        Blizzard::Mopaq::HSFILERecord file;
        file.m_handle = this->m_handle;

        if (file.m_handle) {
            file.m_handle->m_refCount.Increment();
        }

        s_onPatchCompletion(dest, this->m_fileSize, &file, this->m_targetMd5);

        if (file.m_handle) {
            file.m_handle->Release();
        }
    }

    Blizzard::Memory::Free(source);
    return result;
}

// OFFSET: %0x00714B70
bool System_Mopaq::DeltaData::CopyFromApplied(void* buffer, int64_t offset, int32_t count) {
    if (offset + count > static_cast<int32_t>(this->m_appliedSize) || offset < 0) {
        return false;
    }

    memmove(buffer, this->m_appliedBuffer + offset, count);
    return true;
}

// OFFSET: %0x007569D0
bool System_Mopaq::DeltaData::Read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (parms->m_handle->m_archive->m_flags & 0x2000) {
        if (!system) {
            return false;
        }

        return (*reinterpret_cast<Blizzard::Mopaq::Mpqfunc*>(reinterpret_cast<uint8_t*>(system) + parms->m_verb))(system, parms);
    }

    if (!this->m_appliedBuffer) {
        if (!parms->m_offset && static_cast<uint32_t>(parms->m_count) == static_cast<uint32_t>(this->m_fileSize)) {
            return this->PatchIntoBuffer(system, static_cast<uint8_t*>(parms->m_buffer));
        }

        this->m_appliedSize = static_cast<uint32_t>(this->m_fileSize);
        this->m_appliedBuffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(this->m_appliedSize));

        if (!this->PatchIntoBuffer(system, this->m_appliedBuffer)) {
            Blizzard::String::QuickFormat<1024> message("%s - PatchIntoAppliedBuffer error %lld %d %llu", "System_Mopaq::DeltaData::Read", parms->m_offset, parms->m_count, this->m_fileSize);
            Blizzard::File::AddToLastErrorStack(131, message.m_buffer, 4);
            return false;
        }
    }

    return this->CopyFromApplied(parms->m_buffer, parms->m_offset, parms->m_count);
}

// OFFSET: %0x00714AE0
void System_Mopaq::DeltaData::ReserveDeltaSpace(int64_t size) {
    if (this->m_offset < 0) {
        this->m_offset = this->m_handle->m_archive->RequestWriteReservation(size + 28, false);

        Blizzard::Mopaq::MpqHandleRecord* base = this->m_handle->m_handler->m_base.m_handle;

        if (base && base->m_md5Verify) {
            base->m_md5Verify->m_baseOffset = this->m_offset;
        }
    }

    this->m_handle->m_blockEntry.m_offset = this->m_offset + 28;

    if (this->m_handle->m_handler) {
        this->m_handle->m_handler->UpdateBaseOffset();
    }
}

// OFFSET: %0x00720D60
bool System_Mopaq::DeltaData::ProcessPatchHeader(const void* buffer, int32_t size) {
    Blizzard::Patch::MemoryStream stream;
    stream.m_data = static_cast<const uint8_t*>(buffer);
    stream.m_size = size;

    Blizzard::Patch::PatchStream<Blizzard::Patch::MemoryStream> patch;
    patch.m_stream = &stream;

    Blizzard::Patch::ChunkHeader header;
    header.m_tag = Blizzard::Patch::CHUNK_PTCH;
    header.m_size = 16;

    uint32_t sizes[2];
    sizes[0] = 0;
    sizes[1] = 0;

    uint32_t offset = 0;

    if (!patch.FindChunk(Blizzard::Patch::CHUNK_PTCH, offset, header)) {
        return false;
    }

    if (!stream.Read(offset + 8, sizes, sizeof(sizes))) {
        return false;
    }

    Blizzard::Patch::MD5Chunk chunk;
    chunk.m_tag = Blizzard::Patch::CHUNK_MD5;
    chunk.m_size = 40;
    offset = 0;

    if (!patch.Search(Blizzard::Patch::CHUNK_MD5, offset, chunk)) {
        return false;
    }

    this->m_sizeBefore = sizes[0];
    this->m_sizeAfter = sizes[1];
    this->m_sourceMd5 = chunk.m_before;
    this->m_targetMd5 = chunk.m_after;
    this->ReserveDeltaSpace(size);
    return true;
}

// OFFSET: %0x00731CA0
bool System_Mopaq::ReleaseDeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, System_SFile::ArchivePtr& archive) {
    DeltaData* delta = handle->m_delta;

    if (delta->m_headerProcessed && archive.m_archive && archive.m_archive->m_file) {
        MpqDeltaMetadata metadata;
        memset(&metadata, 0, sizeof(metadata));
        metadata.m_size = sizeof(MpqDeltaMetadata);
        metadata.m_extendedFlags = 0x80000000;
        metadata.m_deltaSize = static_cast<uint32_t>(handle->m_blockEntry.m_fileSize);
        metadata.m_md5 = handle->m_blockEntry.m_md5;

        if (!handle->m_handler->m_base.Write(&metadata, delta->m_offset, sizeof(metadata))) {
            return false;
        }

        handle->m_blockEntry.m_offset = handle->m_delta->m_offset;
        handle->m_handler->SetSizeFileBeforeDelta(handle->m_blockEntry.m_fileSize);

        DeltaData* data = handle->m_delta;
        handle->m_blockEntry.m_fileSize = data->m_sizeAfter;
        handle->m_blockEntry.m_md5 = data->m_targetMd5;
        handle->m_blockEntry.m_compressedSize += 28;
        handle->m_blockEntry.m_patch = true;

        archive.m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
        archive.m_archive->ReturnWriteReservation(handle->m_blockEntry.m_offset, metadata.m_deltaSize + 28, handle->m_blockEntry.m_compressedSize, false);
    }

    handle->m_delta->~DeltaData();
    Blizzard::Memory::Free(handle->m_delta);
    handle->m_delta = NULL;
    return true;
}

// OFFSET: %0x00759F80
bool System_Mopaq::delta_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    bool result;

    if (!parms->m_buffer) {
        result = system->m_next ? CallNext(system, parms) : false;

        if (result) {
            void* memory = Blizzard::Memory::Allocate(sizeof(DeltaData));
            handle->m_delta = static_cast<DeltaData*>(memory);
            handle->m_delta = memory ? new (memory) DeltaData(handle, NULL) : NULL;
            return true;
        }

        Blizzard::String::QuickFormat<1024> message("%s - stack open error", "System_Mopaq::delta_open");
        Blizzard::File::AddToLastErrorStack(131, message.m_buffer, 4);
    } else {
        result = InitializeDeltaData(handle, static_cast<const MpqDeltaMetadata*>(parms->m_buffer));

        if (!result) {
            Blizzard::String::QuickFormat<1024> message("%s - InitializeDeltaData error", "System_Mopaq::delta_open");
            Blizzard::File::AddToLastErrorStack(131, message.m_buffer, 4);
        } else {
            if (handle->m_delta->IsLargeDelta(handle->m_archive->m_sectorSize)) {
                parms->m_flags |= 0x20000;
            }

            IOHandler* previous = handle->m_handler;
            result = system->m_next ? CallNext(system, parms) : false;

            if (previous) {
                handle->m_handler->m_base.Assign(previous->m_base);
                delete previous;
            }

            if (result) {
                if (handle->m_archive->m_flags & 0x2000) {
                    return result;
                }

                DeltaData* delta = handle->m_delta;

                if (delta->InitMd5s(system) && delta->OpenSrcFile()) {
                    return true;
                }

                result = false;
            }
        }
    }

    Blizzard::String::QuickFormat<1024> message("%s - error and close", "System_Mopaq::delta_open");
    Blizzard::File::AddToLastErrorStack(131, message.m_buffer, 4);

    MpqParams close;
    memset(&close, 0, sizeof(close));
    close.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_close);
    close.m_handle = parms->m_handle;

    if (close.m_handle->m_stack) {
        close.m_handle->m_stack->m_close(close.m_handle->m_stack, &close);
    }

    parms->m_handle->m_stack = FindIOStackManager(0);
    return result;
}

// OFFSET: %0x00753EF0
bool System_Mopaq::delta_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    System_SFile::ArchivePtr archive(System_SFile::GetArchivePtr(parms->m_handle->m_archive, true));
    bool result = false;

    if (ReleaseDeltaData(parms->m_handle, archive)) {
        result = system->m_next ? CallNext(system, parms) : false;
    }

    return result;
}

// OFFSET: %0x0075A1A0
bool System_Mopaq::delta_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    DeltaData* delta = parms->m_handle->m_delta;

    if (!delta->InitMd5s(system) || !delta->OpenSrcFile()) {
        return false;
    }

    if (!delta->InitializeBaseMetadata()) {
        CloseFileRecord(delta->m_baseFile);
        delta->m_baseFile = NULL;
        return false;
    }

    return parms->m_handle->m_delta->Read(system->m_next, parms);
}

// OFFSET: %0x00720E50
bool System_Mopaq::delta_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    DeltaData* delta = parms->m_handle->m_delta;

    if (!delta->m_headerProcessed && !delta->ProcessPatchHeader(parms->m_buffer, parms->m_count)) {
        return false;
    }

    bool result = system->m_next ? CallNext(system, parms) : false;
    delta->m_headerProcessed = result;
    return result;
}

// OFFSET: %0x0070E200
bool System_Mopaq::delta_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    DeltaData* delta = handle->m_delta;

    if (handle->m_archive->m_flags & 0x2000) {
        parms->m_offset = delta->m_deltaSize;
        return true;
    }

    parms->m_offset = delta->m_fileSize;
    return true;
}

// PROVEN: d04a1684 IDENTICAL/NONE
// OFFSET: 0x007AB380
void Blizzard::Mopaq::SetPatchCompleteCallback(System_Mopaq::DeltaData::PatchCompletionFunc callback) {
    System_Mopaq::DeltaData::s_onPatchCompletion = callback;
}
