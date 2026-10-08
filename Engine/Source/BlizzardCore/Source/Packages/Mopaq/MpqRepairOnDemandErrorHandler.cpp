#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/RepairCache.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// INLINED
static bool IsStreamedFile(Blizzard::Mopaq::HSFILERecord* file) {
    Blizzard::File::StreamRecord* stream = file->m_handle ? file->m_handle->m_file : NULL;

    return stream && stream->m_streaming && stream->m_streaming->m_unit;
}

// OFFSET: 0x007EE330
static bool MarkOffsetRemote(Blizzard::Mopaq::MpqHandleRecord* handle, int64_t offset, uint32_t size) {
    System_Mopaq::IOHandler* handler = handle->m_handler;
    Blizzard::Mopaq::MpqHandleRecord* base = handler->m_base.m_handle;

    if (!base || !base->m_file) {
        return false;
    }

    int64_t position = offset + handler->m_baseOffset;

    if (!Blizzard::Streaming::MakeRemote(base->m_file, position, size)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to mark data (offset = %lld, length = %d) remote \n", position, size);
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s -  Failed to mark data (offset = %lld, length = %d) remote", "MarkOffsetRemote", position, size).m_buffer, 4);
        return false;
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Succeeded to mark data (offset = %lld, length = %d) remote \n", position, size);

    return true;
}

// OFFSET: 0x007EE3E0
static bool RepairChunk(Blizzard::Mopaq::HSFILERecord* file, int64_t offset, uint32_t size) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected bad section: offset = %lld, length = %d\n", offset, size);
    Blizzard::Repair::ReportRepairCorruption("Section_corrupted", offset, size);

    bool result = Blizzard::Streaming::MakeRemote(file->m_handle ? file->m_handle->m_file : NULL, offset, size);

    Blizzard::Mopaq::MpqHandleRecord* handle = file->m_handle;
    char name[260];
    Blizzard::String::Format(name, sizeof(name), "S-A-%s;F-%s;", handle->m_archive ? handle->m_archive->m_name : "", handle->m_name);

    if (result) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Repaired bad section: offset = %lld, length = %d\n", offset, size);
        Blizzard::Repair::ReportRepairSuccess(name, offset, size);
    } else {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to repair bad section: offset = %lld, length = %d\n", offset, size);
        Blizzard::Repair::ReportRepairFail(name, offset, size, "make_remote_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to repair bad section: offset = %lld, length = %d", "RepairChunk", offset, size).m_buffer, 4);
    }

    return result;
}

// OFFSET: 0x007DBE90
static bool MakeSectionRemote(Blizzard::Mopaq::MpqHandleRecord* handle, int64_t offset, uint32_t size) {
    Blizzard::Mopaq::MpqHandleRecord* archiveHandle = handle->m_archive->m_file->m_handle;
    Blizzard::File::StreamRecord* stream = archiveHandle ? archiveHandle->m_file : NULL;

    if (!Blizzard::Streaming::MakeRemote(stream, offset + handle->m_blockEntry.m_offset, size)) {
        return false;
    }

    uint64_t compressedSize = handle->m_blockEntry.m_compressedSize;

    if (!compressedSize) {
        compressedSize = handle->m_blockEntry.m_fileSize;
    }

    int64_t chunks = static_cast<int64_t>(compressedSize - 1) / handle->m_archive->m_header.m_rawChunkSize + 1;

    return Blizzard::Streaming::MakeRemote(stream, handle->m_blockEntry.m_offset + compressedSize, static_cast<uint32_t>(16 * chunks)) != 0;
}

// OFFSET: 0x007F9E10
static bool RepairCacheFile(Blizzard::Mopaq::MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t size) {
    bool deleted = Blizzard::Mopaq::SFileDeleteFile(handle->m_archive, handle->m_name, 0) != 0;
    const char* fileName = handle->m_name;

    if (fileName) {
        uint32_t length = strlen(fileName);

        if (length > 3) {
            const char* extension = fileName + length - 4;

            if (extension == ".wav" || (extension && !_stricmp(extension, ".wav"))) {
                if (!deleted) {
                    Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s -  Failed to delete %s", "RepairCacheFile", handle->m_name).m_buffer, 4);
                    return false;
                }

                Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Deleted and skipped reloading cached WAV file %s\n", handle->m_name);
                return false;
            }
        }
    }

    char name[260];
    Blizzard::String::Format(name, sizeof(name), "A-%s;F-%s;", handle->m_archive->m_name, handle->m_name);

    if (!deleted) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to delete %s\n", handle->m_name);
        Blizzard::Repair::ReportRepairFail(name, offset, size, "delete_file_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s -  Error in deleting %s", "RepairCacheFile", handle->m_name).m_buffer, 4);
        return false;
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully deleted %s\n", handle->m_name);

    if (!Blizzard::Mopaq::SFileLoadFile(NULL, handle->m_name, buffer, &size, 0, 0)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to reload %s\n", handle->m_name);
        Blizzard::Repair::ReportRepairFail(name, offset, size, "open_file_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s -  Failed to reload %s ", "RepairCacheFile", handle->m_name).m_buffer, 4);
        return false;
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully reloaded %s\n", handle->m_name);
    Blizzard::Repair::ReportRepairSuccess(name, offset, size);

    return true;
}

// OFFSET: 0x007FA010
static bool RepairFile(Blizzard::Mopaq::MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t size) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected corruption of %s in section: offset = %lld, length = %d\n", handle->m_name, offset, size);
    Blizzard::Repair::ReportRepairCorruption("Section_corrupted", offset, size);

    if (!MakeSectionRemote(handle, offset, size)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to make remote the section: offset = %lld, length = %d\n", offset, size);
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to make remote the section: offset = %lld, length = %d", "RepairFile", offset, size).m_buffer, 4);
        return RepairCacheFile(handle, buffer, offset, size);
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully made remote the section: offset = %lld, length = %d\n", offset, size);

    char name[260];
    Blizzard::String::Format(name, sizeof(name), "S-A-%s;F-%s;", handle->m_archive->m_name, handle->m_name);

    for (int32_t attempt = 1;; attempt++) {
        System_Mopaq::IOHandler* handler = handle->m_handler;

        if (handler->m_base.Read(buffer, offset + handler->m_baseOffset, size, NULL)) {
            Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully re-read the bad section: offset = %lld, length = %d\n", offset, size);
            Blizzard::Repair::ReportRepairSuccess(name, offset, size);
            return true;
        }

        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to re-read the bad section: offset = %lld, length = %d", "RepairFile", offset, size).m_buffer, 4);
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to re-read the bad section: offset = %lld, length = %d\n", offset, size);

        if (attempt >= 3) {
            Blizzard::Repair::ReportRepairFail(name, offset, size, "read_error");
            return false;
        }
    }
}

// OFFSET: 0x007EE710
bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleLoadAndVerify(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected Mopaq B/H table error: offset = %lld, length = %d\n", offset, size);

    if (!IsStreamedFile(file)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Could not repair because the storage file is not streamed.\n");
        return false;
    }

    if (!RepairChunk(file, offset, size)) {
        return false;
    }

    bool result;
    int32_t attempt = 1;

    do {
        result = file->Read(buffer, offset, size, NULL);

        if (result) {
            Blizzard::Util::MD5 hash;
            hash.Process(buffer, size);
            Blizzard::Util::Digest<16> actual;
            hash.Finish(actual.m_data);

            if (md5 == actual) {
                Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully re-read and verify the bad section: offset = %lld, length = %d\n", offset, size);
                return result;
            }

            char expectedHex[33];
            md5.ToHexString(expectedHex, sizeof(expectedHex));
            char actualHex[33];
            actual.ToHexString(actualHex, sizeof(actualHex));
            Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - fail md5 verification %s %s", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleLoadAndVerify", expectedHex, actualHex).m_buffer, 4);
            result = false;
        }

        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to re-read and verify the bad section: offset = %lld, length = %d\n", offset, size);
    } while (!result && attempt++ < 3);

    return result;
}

// OFFSET: 0x007EE520
bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMPQHeaderV3Error(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected Mopaq v3 header error: offset = %lld, length = %d\n", offset, size);

    if (size < sizeof(System_SFile::MPQHEADER)) {
        return false;
    }

    if (!IsStreamedFile(file)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Could not repair because the storage file is not streamed.\n");
        return false;
    }

    if (!RepairChunk(file, offset, size)) {
        return false;
    }

    for (int32_t attempt = 1;; attempt++) {
        if (file->Read(buffer, offset, size, NULL)) {
            System_SFile::MPQHEADER header;
            memcpy(&header, buffer, sizeof(header));

            if (header.m_formatVersion < 2 || header.m_headerSize < sizeof(System_SFile::MPQHEADER)) {
                if (attempt >= 3) {
                    return false;
                }

                continue;
            }

            Blizzard::Util::Digest<16> md5;
            Blizzard::Util::HashData(md5.m_data, &header, offsetof(System_SFile::MPQHEADER, m_headerMd5));

            if (md5 == header.m_headerMd5) {
                Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Successfully re-read and verify the bad mpq header: offset = %lld, length = %d\n", offset, size);
                return true;
            }
        }

        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to re-read and verify the bad mpq header: offset = %lld, length = %d", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMPQHeaderV3Error", offset, size).m_buffer, 4);
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to re-read and verify the bad mpq header: offset = %lld, length = %d\n", offset, size);

        if (attempt >= 3) {
            return false;
        }
    }
}

// OFFSET: 0x007EE960
bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError(MpqHandleRecord* handle, uint8_t* buffer, int64_t md5Offset, uint32_t block, int32_t blockSize, uint32_t size, uint8_t* md5) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected corruption for MD5 block #%d, amount = %d\n", block, size);
    Blizzard::Repair::ReportRepairCorruption("MD5block_corrupted", block, size);

    if (!handle || !handle->m_handler) {
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - %s (block #%d, amount = %d)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", handle ? "handle->ioHandler is NULL" : "handle is NULL", block, size).m_buffer, 4);
        return false;
    }

    if (handle->m_archive && handle->m_archive->m_name && handle->m_name) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Corruption in archive = %s, file = %s\n", handle->m_archive->m_name, handle->m_name);
        Blizzard::Repair::ReportRepairCorruption("MD5block_corrupted", block, size);
    }

    Blizzard::Mopaq::MpqHandleRecord* base = handle->m_handler->m_base.m_handle;

    if (!base || !base->m_file || !base->m_file->m_streaming || !base->m_file->m_streaming->m_unit) {
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - file is not streamed(block #%d, amount = %d)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", block, size).m_buffer, 4);
        return false;
    }

    int64_t offset = block * blockSize;
    char name[260];
    Blizzard::String::Format(name, sizeof(name), "M-A-%s;F-%s;", handle->m_archive->m_name, handle->m_name);

    if (!MarkOffsetRemote(handle, offset, size)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "MarkOffsetRemote failed to mark the data as remote (block #%d, amount = %d)\n", block, size);
        Blizzard::Repair::ReportRepairFail(name, block, size, "make_remote_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - MarkOffsetRemote error (block #%d, amount = %d)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", block, size).m_buffer, 4);
        return false;
    }

    int32_t count = size;

    if (!handle->m_readFunc(handle->m_handler, buffer, offset, &count)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to re-read data (offset = %lld, length = %u)\n", offset, count);
        Blizzard::Repair::ReportRepairFail(name, offset, count, "read_data_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to re-read data (offset = %lld, length = %u)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", offset, count).m_buffer, 4);
        return false;
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Succeeded to re-read data (offset = %lld, length = %u)\n", offset, count);
    Blizzard::Repair::ReportRepairSuccess(name, block, count);

    int64_t position = md5Offset + 16 * block;
    count = 16;

    if (!MarkOffsetRemote(handle, position, 16)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "MarkOffsetRemote failed to mark the MD5 as remote (block #%d, amount = %d)\n", block, count);
        Blizzard::Repair::ReportRepairFail(name, block, count, "make_remote_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to mark the MD5 as remote (block #%d, amount = %d)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", block, count).m_buffer, 4);
        return false;
    }

    bool result = handle->m_readFunc(handle->m_handler, md5, position, &count);

    if (!result) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Failed to re-read MD5 (offset = %lld, length = %ud)\n", position, count);
        Blizzard::Repair::ReportRepairFail(name, position, count, "read_md5_error");
        Blizzard::File::AddToLastErrorStack(1006, Blizzard::String::QuickFormat<1024>("%s - Failed to re-read data (offset = %lld, length = %u)", "Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError", position, count).m_buffer, 4);
        return false;
    }

    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Succeeded to re-read MD5 (offset = %lld, length = %u)\n", position, count);
    Blizzard::Repair::ReportRepairSuccess("MD5block_repaired", block, count);

    return result;
}

// OFFSET: 0x007FA280
bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleInitSectorTableError(MpqHandleRecord* handle, uint8_t* table, int64_t offset, uint32_t size) {
    Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Detected MPQ sector table error: offset = %lld, length = %d\n", offset, size);

    if (!handle || !handle->m_archive || !handle->m_archive->m_file) {
        return false;
    }

    if (!IsStreamedFile(handle->m_archive->m_file)) {
        Blizzard::Log::Printf(0, "Logs/repair-on-demand.log", "Could not repair because the storage file is not streamed.\n");
        return false;
    }

    return RepairFile(handle, table, offset, size);
}

// OFFSET: 0x007FA310
bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleNonStreamableFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t count) {
    return RepairCacheFile(handle, buffer, offset, count);
}

// OFFSET: 0x007EEE80
bool Blizzard::Mopaq::HSFILERecord::Repair() {
    MpqHandleRecord* handle = this->m_handle;

    if (!handle) {
        return false;
    }

    Blizzard::File::StreamRecord* stream = handle->m_file;

    if (!stream) {
        System_SFile::ARCHIVEREC* archive = handle->m_archive;

        if (!archive || !archive->m_file || !archive->m_file->m_handle || !(stream = archive->m_file->m_handle->m_file)) {
            return false;
        }
    }

    const char* status = Blizzard::Streaming::MakeRemote(stream, handle->m_blockEntry.m_offset, static_cast<uint32_t>(handle->m_blockEntry.m_compressedSize)) ? "success" : "failed";
    handle = this->m_handle;
    Blizzard::Repair::LogRepair("Repair of %s: %s, %lld, %lld\n", handle ? handle->m_name : "", status, handle ? handle->m_blockEntry.m_offset : 0, handle ? handle->m_blockEntry.m_compressedSize : static_cast<uint64_t>(-1));

    return false;
}

// OFFSET: 0x007F3340
bool PurgeCacheFile(Blizzard::Mopaq::HSFILERecord* file) {
    Blizzard::Mopaq::MpqHandleRecord* handle = file->m_handle;

    if (!handle) {
        return false;
    }

    System_SFile::ARCHIVEREC* archive = handle->m_logicalArchive;

    if (!archive) {
        archive = handle->m_archive;

        if (!archive) {
            return false;
        }
    }

    bool result = false;
    System_SFile::ARCHIVEREC* ptr = System_SFile::GetArchivePtr(archive, true);

    if (ptr && (ptr->m_flags & 0x10010)) {
        result = Blizzard::Mopaq::SFileDeleteFile(archive, file->m_handle ? file->m_handle->m_name : "", 0) != 0;
        Blizzard::Repair::LogRepair("Cache Purge of %s: %s\n", file->m_handle ? file->m_handle->m_name : "", result ? "success" : "failed");
    }

    if (ptr) {
        ptr->RemoveInternalArchiveRef();
    }

    return result;
}
