#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// STATIC: 0x00BCD74C
static const uint32_t s_repairDelays[5] = { 100, 200, 500, 1000, 1000 };

// OFFSET: 0x007AB030
void Blizzard::Mopaq::HSFILERecord::SetLogicalArchive(System_SFile::ARCHIVEREC* archive) {
    if (this->m_handle) {
        this->m_handle->m_logicalArchive = archive;
    }
}

// OFFSET: 0x007F7700/0x140506750
int32_t Blizzard::Mopaq::SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, HSFILERecord** result) {
    *result = NULL;

    System_SFile::ARCHIVEREC* record = NULL;
    System_SFile::ARCHIVEREC* archive = NULL;
    int32_t opened = 0;
    uint32_t error = STORM_ERROR_FILE_READ;
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

// OFFSET: 0x007F7880/0x140506970
int32_t Blizzard::Mopaq::SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, HSFILERecord** result) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    return SFileOpenFileEx(handle, name, md5, flags, result);
}

// OFFSET: 0x007CC360/0x1404C87A0
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

// OFFSET: 0x007F4880/0x140502330
bool Blizzard::Mopaq::SFileReadFile(HSFILERecord* file, void* buffer, uint32_t count, uint32_t* bytesRead) {
    int32_t read = 0;
    int64_t position = file->m_handle ? file->m_handle->m_position : -1;
    bool result = file->Read(buffer, position, count, &read);

    if (!result) {
        if (System_Mopaq::RepairFile(file)) {
            const uint32_t* delay = s_repairDelays;

            do {
                result = file->Read(buffer, position, count, &read);

                if (!result) {
                    Blizzard::Process::Sleep(*delay);
                }

                delay++;
            } while (delay < s_repairDelays + 5 && !result && System_Mopaq::RepairFile(file));
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

// OFFSET: 0x007BA190/0x1404B1920
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

// OFFSET: 0x007BA110/0x1404B1890
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

// OFFSET: 0x007BA180/0x1404B1910
int64_t Blizzard::Mopaq::SFileGetFileSize(HSFILERecord* file, uint32_t* sizeHigh) {
    return file->GetFileSize();
}

// OFFSET: 0x007B1B30/0x1404A6C30
int32_t Blizzard::Mopaq::SFileGetFilePointer(HSFILERecord* file) {
    if (!file->m_handle) {
        return -1;
    }

    return static_cast<int32_t>(file->m_handle->m_position);
}

// OFFSET: 0x007B1B60
uint32_t Blizzard::Mopaq::SFileGetFileCompressedSize(HSFILERecord* file, uint32_t* sizeHigh) {
    if (!file->m_handle) {
        return 0xFFFFFFFF;
    }

    return static_cast<uint32_t>(file->m_handle->m_blockEntry.m_compressedSize);
}

// OFFSET: 0x007B1C50
bool Blizzard::Mopaq::SFileIsFileCompressed(HSFILERecord* file) {
    return file->m_handle && (file->m_handle->m_blockEntry.m_flags & 0x200) == 0x200;
}

// OFFSET: 0x007B1C20/0x1404A6CE0
bool Blizzard::Mopaq::SFileGetFileName(HSFILERecord* file, char* name, uint32_t size) {
    Blizzard::String::Copy(name, file->m_handle ? file->m_handle->m_name : "", size);
    return true;
}

// OFFSET: 0x007B1B90
bool Blizzard::Mopaq::SFileGetLogicalArchive(HSFILERecord* file, System_SFile::ARCHIVEREC** archive) {
    MpqHandleRecord* handle = file->m_handle;

    if (!handle) {
        *archive = NULL;
        return false;
    }

    *archive = handle->m_logicalArchive ? handle->m_logicalArchive : handle->m_archive;

    return *archive != NULL;
}

// OFFSET: 0x007B1BE0/0x1404A6C90
bool Blizzard::Mopaq::SFileGetFileMD5(HSFILERecord* file, Blizzard::Util::Digest<16>* md5) {
    *md5 = file->m_handle ? file->m_handle->m_blockEntry.m_md5 : System_SFile::s_emptyMd5;
    return *md5 != System_SFile::s_emptyMd5;
}

// OFFSET: 0x007B2910/0x1404C87A0
bool Blizzard::Mopaq::SFileGetDeltaDigests(HSFILERecord* file, Blizzard::Util::Digest<16>& source, Blizzard::Util::Digest<16>& target) {
    if (!file || !file->m_handle || !file->m_handle->m_delta) {
        return false;
    }

    source = file->m_handle->m_delta->m_sourceMd5;
    target = file->m_handle->m_delta->m_targetMd5;

    return true;
}

// OFFSET: 0x007FB2D0/0x14049AE00
int32_t Blizzard::Mopaq::SFileFileIsLocal(HSFILERecord* file, bool ignorePatch) {
    if (!file) {
        return 1;
    }

    MpqHandleRecord* handle = file->m_handle;

    if (!ignorePatch && handle && handle->m_blockEntry.m_patch) {
        return Blizzard::Mopaq::SFileFileIsLocal(handle->m_name, 0);
    }

    if (!handle) {
        return 1;
    }

    int64_t offset = handle->m_blockEntry.m_offset;
    int64_t size = handle->m_blockEntry.m_compressedSize;
    System_SFile::ARCHIVEREC* archive = handle->m_archive;
    Blizzard::File::StreamRecord* stream = NULL;

    if (archive && archive->m_file && archive->m_file->m_handle) {
        stream = archive->m_file->m_handle->m_file;
    }

    if (stream && stream->m_streaming && stream->m_streaming->m_unit) {
        return Blizzard::Streaming::IsLocal(stream, offset, GetStreamedSize(archive, size));
    }

    Blizzard::File::StreamRecord* local = handle->m_file;

    if (local && local->m_streaming && local->m_streaming->m_unit) {
        return Blizzard::Streaming::IsLocal(local, offset, size);
    }

    return 1;
}

// OFFSET: 0x007AC110/0x1404A1F50
bool Blizzard::Mopaq::SFileIsPatchFile(HSFILERecord* file) {
    if (!file->m_handle) {
        return false;
    }

    return file->m_handle->m_blockEntry.m_patch;
}
