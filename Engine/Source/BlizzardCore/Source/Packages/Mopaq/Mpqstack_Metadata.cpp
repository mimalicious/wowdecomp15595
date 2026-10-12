#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// OFFSET: 0x007EF150/0x1404FB3A0
bool System_Mopaq::ReadAndInitializeMetadata(Blizzard::Mopaq::MpqHandleRecord* handle) {
    if (!(handle->m_stack->m_flags & 0x200)) {
        return true;
    }

    int32_t chunkSize = handle->m_archive->m_header.m_rawChunkSize;

    if (chunkSize > 0) {
        PrepareHandleForMD5Verification(handle, chunkSize, 0);
    }

    MpqDeltaMetadata metadata;
    int32_t count = sizeof(metadata);

    if (handle->m_readFunc) {
        if (!handle->m_readFunc(handle->m_handler, &metadata, 0, &count)) {
            Blizzard::String::QuickFormat<1024> message("%s - fail in readFunc ", "System_Mopaq::ReadAndInitializeMetadata");
            Blizzard::File::AddToLastErrorStack(STORM_ERROR_FILE_READ, message.m_buffer, 4);
            return false;
        }
    } else if (!handle->m_archive->m_file->Read(&metadata, handle->m_blockEntry.m_offset, sizeof(metadata), &count)) {
        Blizzard::String::QuickFormat<1024> message("%s - fail in Read", "System_Mopaq::ReadAndInitializeMetadata");
        Blizzard::File::AddToLastErrorStack(STORM_ERROR_FILE_READ, message.m_buffer, 4);
        return false;
    }

    if (count < 8) {
        Blizzard::String::QuickFormat<1024> message("%s - read incomplete %d", "System_Mopaq::ReadAndInitializeMetadata", count);
        Blizzard::File::AddToLastErrorStack(STORM_ERROR_FILE_READ, message.m_buffer, 4);
        return false;
    }

    if (metadata.m_version || metadata.m_extendedFlags != 0x80000000) {
        Blizzard::String::QuickFormat<1024> message("%s - meta corrupted %u %d", "System_Mopaq::ReadAndInitializeMetadata", metadata.m_extendedFlags, metadata.m_version);
        Blizzard::File::AddToLastErrorStack(STORM_ERROR_FILE_CORRUPTED, message.m_buffer, 4);
        return false;
    }

    Blizzard::Mopaq::Mpqsystem* stack = FindIOStackManager(handle->m_stack->m_next->m_flags + 0x400);
    handle->m_stack = stack;

    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_flags |= 0x10000;
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_handle = handle;
    parms.m_buffer = &metadata;

    if (!stack) {
        return false;
    }

    return stack->m_open(stack, &parms);
}

// OFFSET: 0x007EF300/0x1404FB610
bool System_Mopaq::metadata_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    InitializeHandle(handle, parms);

    System_SFile::ARCHIVEREC* archive = handle->m_archive;

    if (archive && !(parms->m_flags & 0x10000)) {
        archive->m_internalRefs.Increment();
    }

    if (!(parms->m_flags & 0x2000) && !(parms->m_handle->m_archive->m_flags & 0x2000)) {
        return true;
    }

    return ReadAndInitializeMetadata(parms->m_handle);
}

// OFFSET: 0x007EF370/0x1404FB690
bool System_Mopaq::metadata_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!ReadAndInitializeMetadata(parms->m_handle)) {
        return false;
    }

    Blizzard::Mopaq::Mpqsystem* stack = parms->m_handle->m_stack;

    if (!stack) {
        return false;
    }

    return (*reinterpret_cast<Blizzard::Mopaq::Mpqfunc*>(reinterpret_cast<uint8_t*>(stack) + parms->m_verb))(stack, parms);
}
