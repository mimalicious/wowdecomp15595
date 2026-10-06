#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// OFFSET: %0x0070DEF0
void Blizzard::Mopaq::HSFILERecord::SetLogicalArchive(System_SFile::ARCHIVEREC* archive) {
    if (this->m_handle) {
        this->m_handle->m_logicalArchive = archive;
    }
}

// OFFSET: %0x00759D40
int32_t Blizzard::Mopaq::SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, HSFILERecord** result) {
    *result = NULL;

    System_SFile::ARCHIVEREC* record = NULL;
    System_SFile::ARCHIVEREC* archive = NULL;
    int32_t opened = 0;
    uint32_t error = 104;
    uint32_t blockIndex;
    char path[260];

    switch (GetFileBlockEntry(handle, name, md5, flags, &archive, &record, &blockIndex, path)) {
    case 0:
    case 4:
        error = 2;
        break;
    case 1: {
        System_SFile::BLOCKENTRY entry = record->GetBlockEntry(blockIndex);

        if (!(entry.m_flags & 0x80000000) || (!(entry.m_flags & 0x1FFF00) && entry.m_compressedSize != entry.m_fileSize)) {
            error = 1006;
            break;
        }

        System_Mopaq::OpenArchiveFile(name, archive, record, entry, blockIndex, flags, result);
        opened = 1;

        if (*result && archive != record) {
            (*result)->SetLogicalArchive(archive);
        }

        break;
    }
    case 2:
    case 3:
        System_Mopaq::OpenDiskFile(name, flags, path, record, result);
        opened = 2;
        break;
    case 5:
        error = 1006;
        break;
    default:
        break;
    }

    bool released = !record || record->RemoveInternalArchiveRef();

    if (*result && released) {
        return opened;
    }

    System_SFile::s_lastError = error;

    if (name && *name) {
        Blizzard::String::QuickFormat<1024> message("SFileOpenFileEx - %s", name);
        Blizzard::File::AddToLastErrorStack(error, message.m_buffer, 4);
    }

    SFileCloseFile(*result);

    return 0;
}

// OFFSET: %0x00759EC0
int32_t Blizzard::Mopaq::SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, HSFILERecord** result) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    return SFileOpenFileEx(handle, name, md5, flags, result);
}

// OFFSET: %0x0072D550
bool Blizzard::Mopaq::SFileCloseFile(HSFILERecord* file) {
    if (file) {
        if (file->m_handle && !file->m_handle->Release()) {
            file->m_handle = NULL;
        }

        file->m_handle = NULL;
        delete file;
    }

    return true;
}

// OFFSET: %0x00755520
bool Blizzard::Mopaq::SFileReadFile(HSFILERecord* file, void* buffer, uint32_t count, uint32_t* bytesRead) {
    int32_t read = 0;
    int64_t position = file->m_handle ? file->m_handle->m_position : -1;
    bool result = file->Read(buffer, position, count, &read);

    if (!result) {
        if (file->Repair()) {
            result = file->Read(buffer, position, count, &read);
        } else if (!System_MPQGlobals::GetMopaqGlobals()->m_disableRawRead) {
            static_cast<System_SFile::RawReadInfo&>(System_SFile::s_rawReadInfo).m_retryNative = true;
            result = file->Read(buffer, position, count, &read);
            static_cast<System_SFile::RawReadInfo&>(System_SFile::s_rawReadInfo).m_retryNative = false;
        }
    }

    if (file->m_handle) {
        file->m_handle->m_position = position + static_cast<uint32_t>(read);
    }

    if (bytesRead) {
        *bytesRead = read;
    }

    if (static_cast<uint32_t>(read) != count) {
        System_SFile::s_lastError = 38;
    }

    if (result) {
        return result;
    }

    char name[260];

    if (file->m_handle && file->m_handle->m_archive) {
        SFileGetArchiveName(file->m_handle->m_archive, name, sizeof(name));
        Blizzard::String::QuickFormat<1024> message("SFileReadFile - %s - %s", file->m_handle ? file->m_handle->m_name : "", name);
        Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);
        return false;
    }

    const char* fileName = file->m_handle ? file->m_handle->m_name : "";

    if (fileName) {
        Blizzard::String::Copy(name, fileName, sizeof(name));
    } else {
        name[0] = '\0';
    }

    Blizzard::String::QuickFormat<1024> message("SFileReadFile - %s", name);
    Blizzard::File::AddToLastErrorStack(0, message.m_buffer, 4);

    return false;
}

// OFFSET: %0x0071B4D0
bool Blizzard::Mopaq::SFileWriteFile(HSFILERecord* file, const void* data, uint32_t size) {
    int64_t position = file->m_handle ? file->m_handle->m_position : -1;

    if (!file->Write(data, position, size)) {
        return false;
    }

    if (file->m_handle) {
        file->m_handle->m_position = position + size;
    }

    return true;
}

// OFFSET: %0x0071B450
int64_t Blizzard::Mopaq::SFileSetFilePointer(HSFILERecord* file, int32_t distance, int32_t* distanceHigh, uint32_t method) {
    int64_t base = 0;

    if (method == 1) {
        base = file->m_handle ? file->m_handle->m_position : -1;
    } else if (method == 2) {
        base = file->GetFileSize();
    }

    int64_t position;

    if (distance >= 0 || base >= -distance) {
        position = base + distance;
    } else {
        position = 0;
    }

    if (file->m_handle) {
        file->m_handle->m_position = position;
    }

    return position;
}

// OFFSET: %0x0071B4C0
int64_t Blizzard::Mopaq::SFileGetFileSize(HSFILERecord* file, uint32_t* sizeHigh) {
    return file->GetFileSize();
}

// OFFSET: %0x00714080
bool Blizzard::Mopaq::SFileGetFileName(HSFILERecord* file, char* name, uint32_t size) {
    Blizzard::String::Copy(name, file->m_handle ? file->m_handle->m_name : "", size);
    return true;
}

// OFFSET: %0x00713FF0
bool Blizzard::Mopaq::SFileGetLogicalArchive(HSFILERecord* file, System_SFile::ARCHIVEREC** archive) {
    MpqHandleRecord* handle = file->m_handle;

    if (!handle) {
        *archive = NULL;
        return false;
    }

    *archive = handle->m_logicalArchive ? handle->m_logicalArchive : handle->m_archive;

    return *archive != NULL;
}

// OFFSET: %0x00714040
bool Blizzard::Mopaq::SFileGetFileMD5(HSFILERecord* file, Blizzard::Util::Digest<16>* md5) {
    *md5 = file->m_handle ? file->m_handle->m_blockEntry.m_md5 : System_SFile::s_emptyMd5;
    return *md5 != System_SFile::s_emptyMd5;
}

// OFFSET: %0x00714CA0
bool Blizzard::Mopaq::SFileGetDeltaDigests(HSFILERecord* file, Blizzard::Util::Digest<16>& source, Blizzard::Util::Digest<16>& target) {
    if (!file || !file->m_handle || !file->m_handle->m_delta) {
        return false;
    }

    source = file->m_handle->m_delta->m_sourceMd5;
    target = file->m_handle->m_delta->m_targetMd5;

    return true;
}

// OFFSET: %0x00755090
uint32_t Blizzard::Mopaq::GetStreamedSize(System_SFile::ARCHIVEREC* handle, uint32_t size) {
    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (!archive) {
        return size;
    }

    System_SFile::ARCHIVEREC* owner = archive->m_parent ? archive->m_parent : archive;
    int32_t chunkSize = owner->m_header.m_rawChunkSize;

    if (chunkSize > 0) {
        size += 16 * ((chunkSize + size - 1) / static_cast<uint32_t>(chunkSize));
    }

    archive->RemoveInternalArchiveRef();

    return size;
}

// OFFSET: %0x007550E0
int32_t Blizzard::Mopaq::SFileFileIsLocal(HSFILERecord* file, int32_t option) {
    if (!file) {
        return 1;
    }

    MpqHandleRecord* handle = file->m_handle;

    if (!handle) {
        return 1;
    }

    if (handle->m_blockEntry.m_patch) {
        return 0;
    }

    int64_t offset = handle->m_blockEntry.m_offset;
    uint32_t size = static_cast<uint32_t>(handle->m_blockEntry.m_compressedSize);

    if (handle->m_archive) {
        char name[260];
        SFileGetArchiveName(handle->m_archive, name, sizeof(name));
        return Blizzard::Streaming::IsLocal(name, offset, GetStreamedSize(handle->m_archive, size));
    }

    if (handle->m_file) {
        return Blizzard::Streaming::IsLocal(handle->m_file, offset, size);
    }

    return 1;
}
