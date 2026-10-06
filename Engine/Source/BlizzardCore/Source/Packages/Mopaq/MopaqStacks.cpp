#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileSignatures.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"
#include "Engine/Source/Storm/H/storm.h"
#include "ThirdParty/zlib/zlib.h"

// STATIC: %0x00C0C264
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_nullstack = {
    0,
    NULL,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func,
    System_Mopaq::null_func
};

// STATIC: %0x00C0C294
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_basestack = {
    0x1,
    NULL,
    System_Mopaq::base_open,
    System_Mopaq::base_close,
    System_Mopaq::base_read,
    System_Mopaq::base_write,
    System_Mopaq::base_flush,
    System_Mopaq::base_getfilesize,
    System_Mopaq::base_setfilesize,
    System_Mopaq::base_setposition,
    System_Mopaq::base_reload,
    NULL
};

// STATIC: %0x00C0C2C4
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_mopaqstack = {
    0x2,
    NULL,
    System_Mopaq::mopaq_open,
    System_Mopaq::mopaq_close,
    System_Mopaq::mopaq_read,
    System_Mopaq::mopaq_write,
    System_Mopaq::mopaq_flush,
    System_Mopaq::mopaq_getfilesize,
    System_Mopaq::mopaq_setfilesize,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C2F4
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_metadatastack = {
    0x200,
    NULL,
    System_Mopaq::metadata_open,
    NULL,
    System_Mopaq::metadata_read,
    NULL,
    NULL,
    System_Mopaq::mopaq_getfilesize,
    NULL,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C324
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_deltastack = {
    0x400,
    NULL,
    System_Mopaq::delta_open,
    System_Mopaq::delta_close,
    System_Mopaq::delta_read,
    System_Mopaq::delta_write,
    NULL,
    System_Mopaq::delta_getfilesize,
    NULL,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C358
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_checksumstack = {
    0x8,
    NULL,
    System_Mopaq::checksum_open,
    System_Mopaq::checksum_close,
    System_Mopaq::checksum_operation,
    System_Mopaq::checksum_operation,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C388
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_lockstack = {
    0x10,
    NULL,
    System_Mopaq::lock_open,
    System_Mopaq::lock_close,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation,
    System_Mopaq::lock_operation
};

// STATIC: %0x00C0C3B8
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_memorystack = {
    0x4,
    NULL,
    System_Mopaq::memory_open,
    System_Mopaq::memory_close,
    System_Mopaq::memory_read,
    System_Mopaq::memory_write,
    System_Mopaq::memory_flush,
    System_Mopaq::memory_getfilesize,
    System_Mopaq::memory_setfilesize,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C3E8
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_alignstack = {
    0x40,
    NULL,
    System_Mopaq::align_open,
    System_Mopaq::align_close,
    System_Mopaq::align_read,
    System_Mopaq::align_write,
    NULL,
    System_Mopaq::align_getfilesize,
    System_Mopaq::align_setfilesize,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C418
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_cachestack = {
    0x80,
    NULL,
    System_Mopaq::cache_open,
    System_Mopaq::cache_close,
    System_Mopaq::cache_read,
    System_Mopaq::cache_write,
    System_Mopaq::cache_flush,
    System_Mopaq::cache_getfilesize,
    System_Mopaq::cache_setfilesize,
    NULL,
    NULL,
    System_Mopaq::cache_abandon
};

// STATIC: %0x00C0C448
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_verifystack = {
    0x100,
    NULL,
    System_Mopaq::verify_open,
    System_Mopaq::verify_close,
    NULL,
    System_Mopaq::verify_write,
    System_Mopaq::verify_flush,
    System_Mopaq::verify_getfilesize,
    System_Mopaq::verify_setfilesize,
    NULL,
    NULL,
    NULL
};

// STATIC: %0x00C0C478
Blizzard::Mopaq::Mpqsystem System_Mopaq::s_md5verifystack = {
    0x4000,
    NULL,
    System_Mopaq::md5verify_open,
    System_Mopaq::md5verify_close,
    System_Mopaq::md5verify_read,
    System_Mopaq::md5verify_write,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

// INLINED
bool System_Mopaq::CallNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::Mpqsystem* next = system->m_next;

    if (!next) {
        return false;
    }

    return (*reinterpret_cast<Blizzard::Mopaq::Mpqfunc*>(reinterpret_cast<uint8_t*>(next) + parms->m_verb))(next, parms);
}

// PROVEN: 7cfb7190 IDENTICAL/NONE
// OFFSET: 0x00B06D20
bool System_Mopaq::null_func(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return false;
}

// OFFSET: %0x007146B0
bool System_Mopaq::HoistAll(Blizzard::Mopaq::Mpqsystem* system) {
    Blizzard::Mopaq::Mpqfunc* verbs = &system->m_open;

    for (uint32_t i = 0; i < 10; i++) {
        if (verbs[i]) {
            continue;
        }

        Blizzard::Mopaq::Mpqsystem* last = system;

        for (Blizzard::Mopaq::Mpqsystem* next = system->m_next; next; next = next->m_next) {
            (&last->m_open)[i] = (&next->m_open)[i];
            last = next;
        }

        (&last->m_open)[i] = NULL;
    }

    return true;
}

// OFFSET: %0x0071B550
bool System_Mopaq::Push(Blizzard::Mopaq::Mpqsystem& system, Blizzard::Mopaq::Mpqsystem* layer) {
    Blizzard::Mopaq::Mpqsystem* copy = static_cast<Blizzard::Mopaq::Mpqsystem*>(Blizzard::Memory::Allocate(sizeof(Blizzard::Mopaq::Mpqsystem)));
    memcpy(copy, &system, sizeof(Blizzard::Mopaq::Mpqsystem));
    memcpy(&system, layer, sizeof(Blizzard::Mopaq::Mpqsystem));
    system.m_next = copy;
    system.m_flags |= copy->m_flags;

    return HoistAll(&system);
}

// OFFSET: %0x0071B5D0
void System_Mopaq::InitStack(Blizzard::Mopaq::Mpqsystem& system, int32_t flags) {
    if (flags & 0x2) {
        Push(system, &s_mopaqstack);
    } else if (flags & 0x4) {
        Push(system, &s_memorystack);
    } else if (flags & 0x1) {
        Push(system, &s_basestack);
    }

    if (flags & 0x400) {
        Push(system, &s_deltastack);
    }

    if (flags & 0x8) {
        Push(system, &s_checksumstack);
    }

    if (flags & 0x40) {
        Push(system, &s_alignstack);
    }

    if (flags & 0x100) {
        Push(system, &s_verifystack);
        Push(system, &s_cachestack);
    }

    if (flags & 0x200) {
        Push(system, &s_metadatastack);
    }

    if (flags & 0x4000) {
        Push(system, &s_md5verifystack);
    }

    if (flags & 0x10) {
        Push(system, &s_lockstack);
    }
}

// OFFSET: %0x00715330
bool System_Mopaq::ReadFromNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms, void* buffer, int64_t offset, int32_t count, int32_t* amount) {
    MpqParams read;
    memset(&read, 0, sizeof(read));
    read.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
    read.m_handle = parms->m_handle;
    read.m_buffer = buffer;
    read.m_offset = offset;
    read.m_count = count;

    if (!system->m_next) {
        return false;
    }

    bool result = system->m_next->m_read(system->m_next, &read);

    if (result && amount) {
        *amount = read.m_count;
    }

    return result;
}

// OFFSET: %0x007153B0
bool System_Mopaq::WriteToNext(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms, const void* buffer, int64_t offset, int32_t count) {
    MpqParams write;
    memset(&write, 0, sizeof(write));
    write.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_write);
    write.m_handle = parms->m_handle;
    write.m_buffer = const_cast<void*>(buffer);
    write.m_offset = offset;
    write.m_count = count;

    if (!system->m_next) {
        return false;
    }

    return system->m_next->m_write(system->m_next, &write);
}

// OFFSET: %0x00720C20
bool System_Mopaq::base_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    const char* path = parms->m_path;
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;

    if (!path) {
        path = parms->m_name;
    }

    if (!Blizzard::File::Open(System_File::Stacked::s_manager, path, parms->m_flags, handle->m_file)) {
        return false;
    }

    Blizzard::File::FileInfo info;

    if (Blizzard::File::GetFileInfo(handle->m_file, &info)) {
        handle->m_blockEntry.m_fileSize = info.m_size;
        handle->m_blockEntry.m_compressedSize = info.m_size;
    }

    handle->m_name = System_String::Duplicate(path, __FILE__, __LINE__);
    handle->m_position = 0;

    return true;
}

// OFFSET: %0x0071B6B0
bool System_Mopaq::base_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Memory::Free(parms->m_handle->m_name);
    parms->m_handle->m_name = NULL;

    if (!Blizzard::File::Close(parms->m_handle->m_file)) {
        return false;
    }

    parms->m_handle->m_file = NULL;
    return true;
}

// OFFSET: %0x00714870
bool System_Mopaq::base_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return Blizzard::File::Read(parms->m_handle->m_file, parms->m_buffer, parms->m_offset, &parms->m_count, 0);
}

// OFFSET: %0x00714840
bool System_Mopaq::base_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return Blizzard::File::Write(parms->m_handle->m_file, parms->m_buffer, parms->m_offset, &parms->m_count, 0);
}

// OFFSET: %0x007148A0
bool System_Mopaq::base_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    System_File::Stacked::FileParms fileParms;
    fileParms.m_verb = offsetof(Blizzard::File::Filesystem, m_verbs.m_flush);
    fileParms.m_stream = parms->m_handle->m_file;

    if (!System_File::Stacked::s_manager) {
        return false;
    }

    return System_File::Stacked::s_manager->m_verbs.m_flush(System_File::Stacked::s_manager, &fileParms);
}

// PROVEN: 18e51b58 IDENTICAL/NONE
// OFFSET: 0x007B2560
bool System_Mopaq::base_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = Blizzard::File::GetFileInfo(parms->m_handle->m_file)->m_size;
    return true;
}

// OFFSET: %0x00714920
bool System_Mopaq::base_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return Blizzard::File::SetEOF(parms->m_handle->m_file, parms->m_offset, 0);
}

// OFFSET: %0x00714980
bool System_Mopaq::base_setposition(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return Blizzard::File::SetPos(parms->m_handle->m_file, parms->m_offset, 0);
}

// OFFSET: %0x00721770
static bool ReopenFile(Blizzard::File::StreamRecord*& file) {
    char* path = System_String::Duplicate(file->m_pathname, __FILE__, __LINE__);
    int32_t mode = file->m_mode;

    Blizzard::File::Close(file);
    bool result = Blizzard::File::Open(System_File::Stacked::s_manager, path, mode, file);

    Blizzard::Memory::Free(path);
    return result;
}

// OFFSET: %0x00724270
bool System_Mopaq::base_reload(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return ReopenFile(parms->m_handle->m_file);
}

// OFFSET: %0x00714DB0
bool System_Mopaq::lock_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!system->m_next) {
        return false;
    }

    bool result = CallNext(system, parms);

    if (result) {
        Blizzard::Lock::Mutex* mutex = new Blizzard::Lock::Mutex;

        if (mutex) {
            Blizzard::Lock::MutexCreate(*mutex);
        }

        parms->m_handle->m_lock = mutex;
    }

    return result;
}

// OFFSET: %0x0071B960
bool System_Mopaq::lock_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    bool result = system->m_next ? CallNext(system, parms) : false;
    Blizzard::Lock::Mutex* mutex = parms->m_handle->m_lock;

    if (mutex) {
        Blizzard::Lock::MutexDestroy(*mutex);
        delete mutex;
    }

    return result;
}

// OFFSET: %0x00714E10
bool System_Mopaq::lock_operation(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Lock::Mutex* mutex = parms->m_handle->m_lock;
    Blizzard::Lock::MutexEnter(*mutex);

    bool result = false;

    if (system->m_next) {
        result = CallNext(system, parms);
    }

    Blizzard::Lock::MutexLeave(*mutex);
    return result;
}

// OFFSET: %0x0071B9B0
System_Mopaq::MemoryData::MemoryData(Blizzard::Util::Array<uint8_t>* array) {
    this->m_array = array;
    this->m_owned = false;

    if (!array) {
        this->m_owned = true;

        Blizzard::Util::Array<uint8_t>* owned = new Blizzard::Util::Array<uint8_t>;

        if (owned) {
            owned->m_count = 0x8000;
            owned->m_data = new uint8_t[0x8000];
        }

        this->m_array = owned;
    }
}

// OFFSET: %0x0071BA30
bool System_Mopaq::memory_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_handle->m_memory = new MemoryData(static_cast<Blizzard::Util::Array<uint8_t>*>(parms->m_buffer));
    parms->m_handle->m_position = 0;
    return true;
}

// OFFSET: %0x00724290
bool System_Mopaq::memory_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    MemoryData* data = parms->m_handle->m_memory;

    if (data) {
        if (data->m_owned) {
            Blizzard::Util::Array<uint8_t>* array = data->m_array;

            if (array) {
                delete[] array->m_data;
                delete array;
            }
        }

        data->m_array = NULL;
        delete data;
    }

    return true;
}

// OFFSET: %0x00714E80
bool System_Mopaq::memory_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Util::Array<uint8_t>* array = parms->m_handle->m_memory->m_array;
    uint32_t size = array->m_count;

    if (parms->m_offset > static_cast<int64_t>(size)) {
        return false;
    }

    int32_t offset = static_cast<int32_t>(parms->m_offset);
    int32_t excess = offset + parms->m_count - static_cast<int32_t>(size);
    int32_t zero = 0;
    int32_t amount = parms->m_count - (excess > 0 ? excess : zero);

    memcpy(parms->m_buffer, array->m_data + offset, amount);
    parms->m_count = amount;
    return true;
}

// OFFSET: %0x0071BAA0
bool System_Mopaq::memory_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (parms->m_offset + parms->m_count > 0xFFFFFFFFLL) {
        return false;
    }

    Blizzard::Util::Array<uint8_t>* array = parms->m_handle->m_memory->m_array;
    uint32_t end = static_cast<uint32_t>(parms->m_offset) + parms->m_count;

    if (end > array->m_count) {
        array->SetCount(end);
    }

    memcpy(array->m_data + static_cast<uint32_t>(parms->m_offset), parms->m_buffer, parms->m_count);
    return true;
}

// PROVEN: bf8dc270 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool System_Mopaq::memory_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return true;
}

// PROVEN: 0526e835 IDENTICAL/NONE
// OFFSET: 0x007B2B70
bool System_Mopaq::memory_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = parms->m_handle->m_memory->m_array->m_count;
    return true;
}

// PROVEN: c8180205 IDENTICAL/NONE
// OFFSET: 0x007BA760
bool System_Mopaq::memory_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (parms->m_offset > 0xFFFFFFFFLL) {
        return false;
    }

    parms->m_handle->m_memory->m_array->SetCount(static_cast<uint32_t>(parms->m_offset));
    return true;
}

// OFFSET: %0x00720EB0
bool System_Mopaq::align_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!system->m_next || !CallNext(system, parms)) {
        return false;
    }

    AlignData* data = new AlignData;

    if (data) {
        memset(data, 0, sizeof(AlignData));
    }

    parms->m_handle->m_align = data;
    parms->m_handle->m_position = 0;

    Blizzard::File::FileInfo* info = Blizzard::File::GetFileInfo(parms->m_handle->m_file);
    data->m_size = info->m_size;

    char root[1024];
    Blizzard::String::Copy(root, info->m_path, Blizzard::File::GetRootChars(info->m_path) + 1);

    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD freeClusters;
    DWORD totalClusters;
    BOOL valid;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative native(root);
        valid = GetDiskFreeSpaceW(reinterpret_cast<LPCWSTR>(native.m_data), &sectorsPerCluster, &bytesPerSector, &freeClusters, &totalClusters);
    }

    if (!valid || ((bytesPerSector - 1) & bytesPerSector)) {
        data->m_alignMask = 0xFFFF;
    } else {
        data->m_alignMask = bytesPerSector - 1;
    }

    data->m_buffer = Blizzard::Memory::Allocate(data->m_alignMask + 0x100000);
    data->m_alignedBuffer = reinterpret_cast<uint8_t*>(~static_cast<uintptr_t>(data->m_alignMask) & (reinterpret_cast<uintptr_t>(data->m_buffer) + data->m_alignMask));

    return true;
}

// OFFSET: %0x0071BB40
bool System_Mopaq::align_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    AlignData* data = parms->m_handle->m_align;
    Blizzard::Memory::Free(data->m_buffer);

    Blizzard::File::FileInfo* info = Blizzard::File::GetFileInfo(parms->m_handle->m_file);
    int64_t fileSize = info->m_size;

    char path[1024];

    if (info->m_path) {
        Blizzard::String::Copy(path, info->m_path, sizeof(path));
    } else {
        path[0] = '\0';
    }

    bool result = system->m_next ? CallNext(system, parms) : false;

    if (fileSize > data->m_size) {
        if (Blizzard::File::Open(System_File::Stacked::s_manager, path, 0x100E, parms->m_handle->m_file)) {
            bool truncated = Blizzard::File::SetEOF(parms->m_handle->m_file, data->m_size, 0) && result;
            result = Blizzard::File::Close(parms->m_handle->m_file) && truncated;
        } else {
            result = false;
        }
    }

    delete parms->m_handle->m_align;
    return result;
}

// OFFSET: %0x00714F20
bool System_Mopaq::align_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    AlignData* data = parms->m_handle->m_align;
    int64_t offset = parms->m_offset;
    void* buffer = parms->m_buffer;
    uint32_t remaining = parms->m_count;

    if (!(data->m_alignMask & reinterpret_cast<uintptr_t>(buffer)) && !(data->m_alignMask & remaining)) {
        if (!system->m_next) {
            return false;
        }

        return CallNext(system, parms);
    }

    if (!data->m_alignedBuffer) {
        return false;
    }

    uint8_t* dst = static_cast<uint8_t*>(buffer);
    int64_t position = offset;
    int32_t total = 0;
    bool result = true;

    while (remaining) {
        uint32_t chunk = remaining <= 0x100000 ? remaining : 0x100000;
        uint32_t request = chunk;

        if (request & data->m_alignMask) {
            request = ~data->m_alignMask & (data->m_alignMask + request);
        }

        parms->m_buffer = data->m_alignedBuffer;
        parms->m_offset = position;
        parms->m_count = request;

        if (!system->m_next) {
            result = false;
            break;
        }

        result = CallNext(system, parms);

        if (!result) {
            break;
        }

        uint32_t amount = chunk < request ? chunk : request;
        memcpy(dst, data->m_alignedBuffer, amount);
        dst += amount;
        position += amount;
        remaining -= amount;
        total += amount;

        if (amount < chunk) {
            break;
        }
    }

    parms->m_buffer = buffer;
    parms->m_offset = offset;
    parms->m_count = total;
    return result;
}

// OFFSET: %0x00715080
bool System_Mopaq::align_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    AlignData* data = parms->m_handle->m_align;
    const uint8_t* src = static_cast<const uint8_t*>(parms->m_buffer);
    void* buffer = parms->m_buffer;
    int64_t offset = parms->m_offset;
    int32_t count = parms->m_count;
    uint32_t remaining = count;

    if (!(data->m_alignMask & reinterpret_cast<uintptr_t>(src)) && !(data->m_alignMask & remaining)) {
        if (!system->m_next) {
            return false;
        }

        return CallNext(system, parms);
    }

    if (!data->m_alignedBuffer) {
        return false;
    }

    int64_t position = offset;
    bool result = true;

    while (remaining) {
        uint32_t chunk = remaining <= 0x100000 ? remaining : 0x100000;
        memcpy(data->m_alignedBuffer, src, chunk);
        src += chunk;

        uint32_t request = chunk;

        if (request & data->m_alignMask) {
            request = ~data->m_alignMask & (data->m_alignMask + request);
        }

        parms->m_buffer = data->m_alignedBuffer;
        parms->m_offset = position;
        parms->m_count = request;

        if (!system->m_next) {
            result = false;
            break;
        }

        result = CallNext(system, parms);

        if (!result) {
            break;
        }

        position += chunk;
        remaining -= chunk;
    }

    if (position > data->m_size) {
        data->m_size = position;
    }

    parms->m_buffer = buffer;
    parms->m_offset = offset;
    parms->m_count = count;
    return result;
}

// OFFSET: %0x0070E370
bool System_Mopaq::align_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = parms->m_handle->m_align->m_size;
    return true;
}

// OFFSET: %0x0070E390
bool System_Mopaq::align_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    int64_t size = parms->m_offset;

    if (size > 0xFFFFFFFFLL) {
        return false;
    }

    AlignData* data = parms->m_handle->m_align;
    data->m_size = size;
    parms->m_offset = ~static_cast<int64_t>(data->m_alignMask) & (data->m_alignMask + size);

    if (!system->m_next) {
        parms->m_offset = size;
        return false;
    }

    bool result = CallNext(system, parms);
    parms->m_offset = size;
    return result;
}

// OFFSET: %0x0070E250
bool System_Mopaq::ChecksumData::GetRange(const void* buffer, int64_t offset, uint32_t count, const void** start, uint32_t* amount) {
    if (offset > this->m_position) {
        return false;
    }

    if (offset + count <= this->m_position) {
        return false;
    }

    const uint8_t* data = static_cast<const uint8_t*>(buffer);

    if (offset < this->m_position) {
        uint32_t skip = static_cast<uint32_t>(this->m_position - offset);
        data += skip;
        count -= skip;
    }

    *start = data;
    *amount = count;
    return true;
}

// OFFSET: %0x0070E2E0
bool System_Mopaq::ChecksumData::Update(Blizzard::Mopaq::MpqHandleRecord* handle, const void* buffer, int64_t offset, uint32_t count) {
    if (!this->GetRange(buffer, offset, count, &buffer, &count)) {
        return true;
    }

    this->m_checksum(this, buffer, count);

    if (this->m_position + static_cast<int32_t>(count) == this->m_size) {
        bool result = this->m_finish(this, handle);

        if (result) {
            this->m_position += static_cast<int32_t>(count);
        }

        return result;
    }

    this->m_position += static_cast<int32_t>(count);
    return true;
}

// OFFSET: %0x0070E2C0
void System_Mopaq::ChecksumData::ChecksumCrc(ChecksumData* data, const void* buffer, int32_t size) {
    data->m_crc = crc32(data->m_crc, static_cast<const Bytef*>(buffer), size);
}

// OFFSET: %0x00714D60
void System_Mopaq::ChecksumData::ChecksumMd5(ChecksumData* data, const void* buffer, int32_t size) {
    data->m_md5.Process(buffer, size);
}

// OFFSET: %0x00714D80
void System_Mopaq::ChecksumData::ChecksumCrcAndMd5(ChecksumData* data, const void* buffer, int32_t size) {
    data->m_crc = crc32(data->m_crc, static_cast<const Bytef*>(buffer), size);
    data->m_md5.Process(buffer, size);
}

// OFFSET: %0x0071B860
bool System_Mopaq::ChecksumData::FinishCrc(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle) {
    handle->m_blockEntry.m_crc = data->m_crc;

    if (handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
    }

    return true;
}

// OFFSET: %0x0071B810
bool System_Mopaq::ChecksumData::FinishMd5(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle) {
    Blizzard::Util::Digest<16> md5;
    data->m_md5.Finish(md5.m_data);
    handle->m_blockEntry.m_md5 = md5;

    if (handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
    }

    return true;
}

// OFFSET: %0x0071B890
bool System_Mopaq::ChecksumData::FinishCrcAndMd5(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle) {
    Blizzard::Util::Digest<16> md5;
    data->m_md5.Finish(md5.m_data);
    handle->m_blockEntry.m_md5 = md5;
    handle->m_blockEntry.m_crc = data->m_crc;

    if (handle->m_archive) {
        handle->m_archive->SetBlockEntry(handle->m_blockIndex, handle->m_blockEntry);
    }

    return true;
}

// OFFSET: %0x00753FB0
static void AppendHandleSource(Blizzard::Mopaq::MpqHandleRecord* handle, char* buffer, uint32_t size) {
    if (handle->m_logicalArchive) {
        char name[260];
        Blizzard::Mopaq::SFileGetArchiveFullName(handle->m_logicalArchive, name, sizeof(name));
        Blizzard::String::Append(buffer, name, size);
    } else {
        Blizzard::String::Append(buffer, handle->m_archive ? handle->m_archive->m_name : "<<Direct File>>", size);
    }

    Blizzard::String::Append(buffer, "\n", size);
}

// OFFSET: %0x00754030
bool System_Mopaq::ChecksumData::FinishSigning(ChecksumData* data, Blizzard::Mopaq::MpqHandleRecord* handle) {
    Blizzard::Util::Digest<16> md5;
    data->m_md5.Finish(md5.m_data);

    bool result = true;

    if (handle->m_blockEntry.m_md5 != md5) {
        if (!handle->m_signatureFailed && !data->m_position) {
            data->m_crc = 0;
            new (&data->m_md5) Blizzard::Util::MD5();
            data->m_position = 0;
            return false;
        }

        char message[4096];
        memset(message, 0, sizeof(message));
        md5.ToHexString(message, sizeof(message));

        Blizzard::String::QuickFormat<1024> line(" %lld %d\n", data->m_size, Blizzard::Streaming::IsStreamingMode());
        Blizzard::String::Append(message, line.m_buffer, sizeof(message));
        AppendHandleSource(handle, message, sizeof(message));

        for (System_Mopaq::DeltaData* delta = handle->m_delta; delta; ) {
            if (!delta->m_baseFile) {
                Blizzard::String::Append(message, "<<Broken Delta File Found>>\n", sizeof(message));
                break;
            }

            Blizzard::Mopaq::MpqHandleRecord* base = delta->m_baseFile->m_handle;
            AppendHandleSource(base, message, sizeof(message));
            delta = base->m_delta;
        }

        System_MPQGlobals::GetMopaqGlobals()->m_signatureErrorCallback(handle->m_name, message);
        result = false;
    }

    handle->m_archive->SetMD5Check(handle->m_blockIndex, false);
    return result;
}

// OFFSET: %0x0071B8E0
void System_Mopaq::ChecksumData::InitMd5() {
    new (&this->m_md5) Blizzard::Util::MD5();
    this->m_checksum = ChecksumMd5;
    this->m_finish = FinishMd5;
}

// OFFSET: %0x0071B920
void System_Mopaq::ChecksumData::InitCrcAndMd5() {
    this->m_crc = 0;
    new (&this->m_md5) Blizzard::Util::MD5();
    this->m_checksum = ChecksumCrcAndMd5;
    this->m_finish = FinishCrcAndMd5;
}

// OFFSET: %0x007541C0
void System_Mopaq::ChecksumData::InitSigning(Blizzard::Mopaq::MpqHandleRecord* handle) {
    new (&this->m_md5) Blizzard::Util::MD5();
    this->m_checksum = ChecksumMd5;
    this->m_finish = FinishSigning;
    System_SFile::s_signatures.GetEntry(handle->m_name, &handle->m_blockEntry.m_md5);
}

// OFFSET: %0x00756000
bool System_Mopaq::checksum_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    int64_t size = 0;

    if (parms->m_blockEntry) {
        size = parms->m_blockEntry->m_fileSize;
    } else if (parms->m_handle) {
        size = parms->m_handle->m_blockEntry.m_fileSize;
    }

    if (!system->m_next) {
        return false;
    }

    bool result = CallNext(system, parms);

    if (!result || !parms->m_handle) {
        return result;
    }

    ChecksumData* data = static_cast<ChecksumData*>(Blizzard::Memory::Allocate(sizeof(ChecksumData)));
    parms->m_handle->m_checksum = data;
    data->m_position = 0;
    data->m_size = size;

    if (parms->m_handle->m_blockEntry.m_md5Check && System_MPQGlobals::GetMopaqGlobals()->m_md5Mode && !(parms->m_flags & 0x4)) {
        data->InitSigning(parms->m_handle);
        return result;
    }

    System_SFile::ARCHIVEREC* archive = parms->m_handle->m_archive;

    if (archive) {
        uint32_t flags = archive->m_attributeFlags;

        if ((flags & 0x1) && (flags & 0x4)) {
            data->InitCrcAndMd5();
        } else if (flags & 0x1) {
            data->m_crc = 0;
            data->m_checksum = ChecksumData::ChecksumCrc;
            data->m_finish = ChecksumData::FinishCrc;
        } else if (flags & 0x4) {
            data->InitMd5();
        }
    }

    return result;
}

// OFFSET: %0x0071B7D0
bool System_Mopaq::checksum_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Memory::Free(parms->m_handle->m_checksum);

    if (!system->m_next) {
        return false;
    }

    return CallNext(system, parms);
}

// OFFSET: %0x00714D10
bool System_Mopaq::checksum_operation(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;

    if (!system->m_next) {
        return false;
    }

    if (!CallNext(system, parms)) {
        return false;
    }

    return handle->m_checksum->Update(handle, parms->m_buffer, parms->m_offset, parms->m_count);
}

// OFFSET: %0x0073A7B0
void System_Mopaq::VerifyData::ChecksumTable::SetChecksum(uint32_t index, uint32_t checksum) {
    uint32_t entry = index >> 5;
    uint32_t count = static_cast<uint32_t>(this->m_entries.size());

    if (entry >= count) {
        uint32_t newCount = count > 1 ? count : 1;

        while (entry >= newCount) {
            newCount *= 2;
        }

        ChecksumEntry32 empty;
        memset(&empty, 0, sizeof(empty));
        this->m_entries.resize(newCount, empty);
    }

    this->m_entries[entry].m_valid |= 0x80000000 >> (index & 0x1F);
    this->m_entries[entry].m_checksums[index & 0x1F] = checksum;
}

// PROVEN: df145a72 IDENTICAL/NONE
// OFFSET: 0x007B3090
void System_Mopaq::VerifyData::ChecksumTable::ClearChecksum(uint32_t index) {
    this->m_entries[index >> 5].m_valid &= ~(0x80000000 >> (index & 0x1F));
}

// OFFSET: %0x0073A8B0
void System_Mopaq::VerifyData::ChecksumTable::ForgetAllChecksumsAtOrBeyond(uint32_t index) {
    uint32_t entry = (index + 31) >> 5;

    ChecksumEntry32 empty;
    memset(&empty, 0, sizeof(empty));
    this->m_entries.resize(entry + 1, empty);

    this->m_entries[entry].m_valid &= ~(0xFFFFFFFF >> (index & 0x1F));
}

// OFFSET: %0x0073DAF0
bool System_Mopaq::verify_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!system->m_next || !CallNext(system, parms)) {
        return false;
    }

    parms->m_handle->m_verify = new VerifyData;

    Blizzard::File::FileInfo* info = Blizzard::File::GetFileInfo(parms->m_handle->m_file);

    if (info->m_size == -1) {
        return false;
    }

    VerifyData* data = parms->m_handle->m_verify;
    data->m_base = 0;
    data->m_blockSize = 0x100000;
    data->m_size = info->m_size;
    return true;
}

// OFFSET: %0x00735850
bool System_Mopaq::verify_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    delete parms->m_handle->m_verify;

    if (!system->m_next) {
        return false;
    }

    return CallNext(system, parms);
}

// OFFSET: %0x0073A940
bool System_Mopaq::verify_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    VerifyData* data = parms->m_handle->m_verify;
    int64_t offset = parms->m_offset;
    uint32_t remaining = parms->m_count;

    if (offset < data->m_base || (offset - data->m_base) % data->m_blockSize) {
        return false;
    }

    int64_t end = offset + remaining;

    if (end < data->m_size) {
        if (remaining % data->m_blockSize) {
            return false;
        }
    } else {
        data->m_size = end;
    }

    if (!system->m_next || !CallNext(system, parms)) {
        return false;
    }

    uint32_t blockSize = data->m_blockSize;
    uint32_t first = static_cast<uint32_t>((offset - data->m_base) / blockSize);
    uint32_t last = (blockSize + remaining - 1) / blockSize + first - 1;
    const uint8_t* src = static_cast<const uint8_t*>(parms->m_buffer);

    Blizzard::Memory::TempBuffer<uint8_t, 20480> buffer(blockSize + 4095);
    uint8_t* aligned = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(buffer.m_data + 4095) & ~static_cast<uintptr_t>(0xFFF));

    for (uint32_t index = first; index <= last; index++) {
        uint32_t amount = data->m_blockSize < remaining ? data->m_blockSize : remaining;
        uint32_t checksum = adler32(0, src, amount);
        data->m_table.SetChecksum(index, checksum);

        src += amount;
        remaining -= amount;

        int64_t position = data->m_base + static_cast<uint64_t>(index) * data->m_blockSize;
        int64_t tail = data->m_size - position;
        uint32_t readSize = static_cast<int64_t>(data->m_blockSize) >= tail ? static_cast<uint32_t>(tail) : data->m_blockSize;

        MpqParams read;
        memset(&read, 0, sizeof(read));
        read.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_read);
        read.m_handle = parms->m_handle;
        read.m_buffer = aligned;
        read.m_offset = position;
        read.m_count = readSize;

        if (!system->m_read(system, &read)) {
            return false;
        }

        if (adler32(0, aligned, readSize) != checksum) {
            return false;
        }

        data->m_table.ClearChecksum(index);
    }

    return true;
}

// OFFSET: %0x0070E7F0
bool System_Mopaq::verify_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    if (!system->m_next) {
        return false;
    }

    return CallNext(system, parms);
}

// OFFSET: %0x0070E440
bool System_Mopaq::verify_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = parms->m_handle->m_verify->m_size;
    return true;
}

// OFFSET: %0x0073ABE0
bool System_Mopaq::verify_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    VerifyData* data = parms->m_handle->m_verify;

    if (parms->m_offset == data->m_size) {
        return true;
    }

    data->m_table.ForgetAllChecksumsAtOrBeyond(static_cast<uint32_t>((parms->m_offset - data->m_base) / data->m_blockSize));
    data->m_size = parms->m_offset;

    if (!system->m_next) {
        return false;
    }

    return CallNext(system, parms);
}
