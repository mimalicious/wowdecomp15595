#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_MD5Verify.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00D86B98
MpqSystemList::Implementation* MpqSystemList::s_list;

// OFFSET: 0x007B2080
MpqSystemList::Node::Node(int32_t flags) {
    this->m_flags = flags;
    this->m_next = NULL;
    memcpy(&this->m_system, &System_Mopaq::s_nullstack, sizeof(Blizzard::Mopaq::Mpqsystem));
}

// OFFSET: 0x007BF320/0x1404B81C0
MpqSystemList::Node::~Node() {
    if (this->m_next) {
        this->m_next->~Node();
        Blizzard::Memory::Free(this->m_next);
        this->m_next = NULL;
    }

    Blizzard::Mopaq::Mpqsystem* layer = this->m_system.m_next;

    if (layer) {
        do {
            Blizzard::Mopaq::Mpqsystem* next = layer->m_next;
            Blizzard::Memory::Free(layer);
            layer = next;
        } while (layer);

        this->m_system.m_next = NULL;
    }
}

// OFFSET: 0x007BF370/0x1404B8240
void MpqSystemList::Implementation::InitializeNewStackAtHead(Node* after, int32_t flags) {
    void* memory = SMemAlloc(sizeof(Node), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY);
    Node* node = memory ? new (memory) Node(flags) : NULL;

    System_Mopaq::InitStack(node->m_system, node->m_flags);

    Blizzard::Lock::MutexEnter(this->m_mutex);

    if (!after) {
        after = this->m_head;
    }

    if (!after) {
        node->m_next = NULL;
        this->m_head = node;
        Blizzard::Lock::MutexLeave(this->m_mutex);
        return;
    }

    for (Node* existing = after->m_next; existing; existing = existing->m_next) {
        if (existing->m_flags == flags) {
            Blizzard::Memory::Free(node);
            Blizzard::Lock::MutexLeave(this->m_mutex);
            return;
        }
    }

    after->m_next = node;
    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007C3260/0x1404BD810
Blizzard::Mopaq::Mpqsystem* MpqSystemList::Implementation::GetStack(int32_t flags) {
    if (!this->m_head) {
        this->InitializeNewStackAtHead(NULL, flags);
    }

    Node* node = this->m_head;

    if (node->m_flags == flags) {
        return &node->m_system;
    }

    while (node->m_next) {
        node = node->m_next;

        if (node->m_flags == flags) {
            return &node->m_system;
        }
    }

    this->InitializeNewStackAtHead(node, flags);

    while (node->m_flags != flags) {
        node = node->m_next;

        if (!node) {
            return NULL;
        }
    }

    return &node->m_system;
}

// OFFSET: 0x007CC420/0x1404C8890
MpqSystemList::MpqSystemList() : Blizzard::Using::Use("System_Mopaq::IOStackManager") {
    System_Using::Intern(this->m_name)->shutdownproc = MpqSystemList::ShutdownProc;

    Implementation* list = static_cast<Implementation*>(SMemAlloc(sizeof(Implementation), __FILE__, __LINE__, 0));
    s_list = list;

    if (list) {
        list->m_head = NULL;
        Blizzard::Using::StartUsing("Blizzard::Memory");
        Blizzard::Lock::MutexCreate(list->m_mutex);
        s_list = list;
    } else {
        s_list = NULL;
    }
}

// OFFSET: 0x007CC3B0/0x1404C8800
void MpqSystemList::ShutdownProc() {
    Implementation* list = s_list;
    list->m_head->~Node();
    Blizzard::Memory::Free(list->m_head);

    Blizzard::Lock::MutexDestroy(list->m_mutex);
    Blizzard::Using::StopUsing("Blizzard::Memory");

    SMemFree(s_list, __FILE__, __LINE__, SMEM_FLAG_4000000);
    s_list = NULL;
}

// OFFSET: 0x007CC4A0
Blizzard::Mopaq::Mpqsystem* System_Mopaq::FindIOStackManager(int32_t flags) {
    static MpqSystemList s_systemList;
    return MpqSystemList::s_list->GetStack(flags);
}

namespace System_Mopaq {
namespace MpqHandleRecordPrealloc {

enum PreallocPart {
    PREALLOC_NAME = 1,
    PREALLOC_HANDLER = 2,
    PREALLOC_SECTOR_TABLE = 3,
    PREALLOC_ERROR_HANDLER = 4,
};

}
}

// OFFSET: 0x007AB050
static uint32_t GetSectorTableSize(System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY* entry, uint32_t flags) {
    uint32_t size = 0;

    if (entry->m_flags & 0xFF00) {
        uint32_t count;

        if (entry->m_flags & 0x1000000) {
            count = 1;
        } else {
            count = static_cast<uint32_t>((entry->m_fileSize - 1) / archive->m_sectorSize + 1);
        }

        size = 4 * count + 8;
    }

    if ((flags & 0x20000) && size > 12) {
        return 0;
    }

    return size;
}

// OFFSET: 0x007AB0B0
static uint32_t GetHandlerSize(System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY* entry, uint32_t flags) {
    if (flags & 0x200) {
        if ((entry->m_flags & 0xFF00) && !(flags & 0x400)) {
            return sizeof(System_Mopaq::SectorInPlaceReadWriteHandler);
        }

        return sizeof(System_Mopaq::MopaqInPlaceReadWriteHandler);
    }

    if ((archive->m_flags & 0x10) && (flags & 0x4)) {
        if ((entry->m_flags & 0xFF00) && !(flags & 0x400)) {
            return sizeof(System_Mopaq::SectorReadWriteHandler);
        }

        return sizeof(System_Mopaq::MopaqReadWriteHandler);
    }

    if (!(entry->m_flags & 0xFF00)) {
        return sizeof(System_Mopaq::MopaqReadHandler);
    }

    if (flags & 0x400) {
        return sizeof(System_Mopaq::MopaqReadHandler);
    }

    return sizeof(System_Mopaq::SectorReadHandler);
}

// OFFSET: 0x007EEF50
static Blizzard::Mopaq::MpqHandleRecord* GetPrealloc(const char* name, System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY* entry, uint32_t flags) {
    uint32_t nameLength = name ? strlen(name) : 0;
    uint32_t errorHandlerSize = ((flags >> 11) & 1) ? sizeof(Blizzard::Mopaq::MpqStandaloneRepairErrorHandler) : sizeof(Blizzard::Mopaq::MpqErrorHandler);
    uint32_t handlerSize = GetHandlerSize(archive, entry, flags);
    uint32_t sectorTableSize = GetSectorTableSize(archive, entry, flags);

    return static_cast<Blizzard::Mopaq::MpqHandleRecord*>(SMemAlloc(nameLength + handlerSize + sectorTableSize + errorHandlerSize + sizeof(Blizzard::Mopaq::MpqHandleRecord) + 1, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
}

// OFFSET: 0x007B2650
void System_Mopaq::InitializeHandle(Blizzard::Mopaq::MpqHandleRecord* handle, MpqParams* parms) {
    if (parms->m_flags & 0x10000) {
        return;
    }

    if (parms->m_blockEntry) {
        handle->m_blockEntry = *parms->m_blockEntry;
    }

    handle->m_position = 0;
    handle->m_blockIndex = parms->m_blockIndex;

    char* name = reinterpret_cast<char*>(handle + 1);
    handle->m_name = name;

    uint32_t size = (parms->m_name ? strlen(parms->m_name) : 0) + 1;
    memmove(name, parms->m_name, size);
}

// OFFSET: 0x007EABA0
static void CreateHandler(bool sharedMemory, bool compressed, Blizzard::Mopaq::MpqHandleRecord* handle, void* memory, void* sectorTable, bool writable, bool inPlace) {
    if (inPlace) {
        if (compressed) {
            System_Mopaq::SectorInPlaceReadWriteHandler* handler = memory ? new (memory) System_Mopaq::SectorInPlaceReadWriteHandler(handle, sectorTable, sharedMemory) : NULL;
            handle->m_handler = handler;
            System_SFile::ARCHIVEREC* archive = handle->m_archive;

            if (archive->m_compression.size()) {
                handler->m_compression.insert(handler->m_compression.end(), archive->m_compression.begin(), archive->m_compression.end());
            } else {
                Blizzard::Mopaq::CompressionAlgorithm algorithm;
                algorithm.m_type = 2;
                algorithm.m_level = 0;
                handler->m_compression.push_back(algorithm);
            }
        } else if (memory) {
            handle->m_handler = new (memory) System_Mopaq::MopaqInPlaceReadWriteHandler(handle);
        } else {
            handle->m_handler = NULL;
        }
    } else if (writable) {
        if (compressed) {
            System_Mopaq::SectorReadWriteHandler* handler = memory ? new (memory) System_Mopaq::SectorReadWriteHandler(handle, sectorTable, sharedMemory) : NULL;
            handle->m_handler = handler;
            System_SFile::ARCHIVEREC* archive = handle->m_archive;

            if (archive->m_compression.size()) {
                handler->m_compression.insert(handler->m_compression.end(), archive->m_compression.begin(), archive->m_compression.end());
            } else {
                Blizzard::Mopaq::CompressionAlgorithm algorithm;
                algorithm.m_type = 2;
                algorithm.m_level = 0;
                handler->m_compression.push_back(algorithm);
            }
        } else if (memory) {
            handle->m_handler = new (memory) System_Mopaq::MopaqReadWriteHandler(handle);
        } else {
            handle->m_handler = NULL;
        }
    } else if (compressed) {
        handle->m_handler = memory ? new (memory) System_Mopaq::SectorReadHandler(handle, sectorTable, sharedMemory) : NULL;
    } else {
        handle->m_handler = memory ? new (memory) System_Mopaq::MopaqReadHandler(handle) : NULL;
    }
}

// OFFSET: 0x007D8F80
static void CreateErrorHandler(Blizzard::Mopaq::MpqHandleRecord* handle, void* memory, bool standalone, bool repairOnDemand) {
    if (standalone) {
        handle->m_errorHandler = memory ? new (memory) Blizzard::Mopaq::MpqStandaloneRepairErrorHandler() : NULL;
    } else if (repairOnDemand) {
        handle->m_errorHandler = memory ? new (memory) Blizzard::Mopaq::MpqRepairOnDemandErrorHandler() : NULL;
    } else {
        handle->m_errorHandler = memory ? new (memory) Blizzard::Mopaq::MpqErrorHandler() : NULL;
    }
}

// OFFSET: 0x007EEFE0
bool System_Mopaq::mopaq_open(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    InitializeHandle(handle, parms);

    uint32_t nameLength = handle->m_name ? strlen(handle->m_name) : 0;
    uint32_t flags = parms->m_flags;
    uint8_t* sectorTable = reinterpret_cast<uint8_t*>(handle) + sizeof(Blizzard::Mopaq::MpqHandleRecord) + 1 + nameLength;
    uint8_t* handler = sectorTable + GetSectorTableSize(handle->m_archive, &handle->m_blockEntry, flags);

    bool compressed = (handle->m_blockEntry.m_flags & 0xFF00) && !(flags & 0x400);
    bool writable = (flags & 0x4) && (handle->m_archive->m_flags & 0x10);
    CreateHandler(sectorTable == handler, compressed, handle, handler, sectorTable, writable, (flags & 0x200) != 0);

    System_SFile::ARCHIVEREC* archive = handle->m_archive;
    uint32_t handlerSize = GetHandlerSize(archive, &handle->m_blockEntry, parms->m_flags);
    uint32_t sectorTableSize = GetSectorTableSize(archive, &handle->m_blockEntry, parms->m_flags);
    nameLength = handle->m_name ? strlen(handle->m_name) : 0;
    uint8_t* errorHandler = reinterpret_cast<uint8_t*>(handle) + sizeof(Blizzard::Mopaq::MpqHandleRecord) + 1 + nameLength + handlerSize + sectorTableSize;
    CreateErrorHandler(handle, errorHandler, (parms->m_flags & 0x800) != 0, archive->m_header.m_rawChunkSize > 0 && !System_MPQGlobals::GetMopaqGlobals()->m_disableRepair);

    archive = handle->m_archive;

    if (archive && !(parms->m_flags & 0x10000)) {
        archive->m_internalRefs.Increment();
    }

    return true;
}

// OFFSET: 0x007F1BC0/0x1404FEAB0
bool System_Mopaq::mopaq_close(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    bool result = true;

    if (handle->m_handler) {
        result = handle->m_handler->Flush();
        parms->m_handle->m_handler->~IOHandler();
        parms->m_handle->m_handler = NULL;
    }

    handle = parms->m_handle;

    if (handle->m_errorHandler) {
        handle->m_errorHandler->~MpqErrorHandler();
        parms->m_handle->m_errorHandler = NULL;
    }

    handle = parms->m_handle;
    System_SFile::ARCHIVEREC* archive = handle->m_archive;

    if (archive) {
        archive->RemoveInternalArchiveRef();
        handle->m_archive = NULL;
    }

    return result;
}

// OFFSET: 0x007E76B0/0x1404F1800
bool System_Mopaq::mopaq_read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    int64_t offset = parms->m_offset;

    if (static_cast<uint64_t>(offset) > handle->m_blockEntry.m_fileSize || offset < 0) {
        Blizzard::String::QuickFormat<1024> message("%s - offset:%lld out of range (0, %llu)", "System_Mopaq::mopaq_read", offset, handle->m_blockEntry.m_fileSize);
        Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
        return false;
    }

    handle->m_position = offset;

    int32_t count = handle->m_handler->TrimRead(parms->m_count, handle->m_blockEntry, handle->m_position);
    parms->m_count = count;

    if (!count) {
        return true;
    }

    bool result = handle->m_readFunc(handle->m_handler, parms->m_buffer, handle->m_position, &parms->m_count);

    if (result) {
        handle->m_position += parms->m_count;
        return result;
    }

    if (!handle->m_errorHandler->HandleReadMPQFileError(handle, static_cast<uint8_t*>(parms->m_buffer), handle->m_position, reinterpret_cast<uint32_t*>(&parms->m_count))) {
        Blizzard::String::QuickFormat<1024> message("%s - fail to repair offset %lld amount %d", "System_Mopaq::mopaq_read", handle->m_position, parms->m_count);
        Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
    }

    return false;
}

// OFFSET: 0x007AB130/0x1404A12A0
bool System_Mopaq::mopaq_write(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    bool result = handle->m_writeFunc(handle->m_handler, parms->m_buffer, handle->m_position, parms->m_count);

    if (result) {
        handle->m_position += parms->m_count;
        return result;
    }

    if (!handle->m_errorHandler->HandleWriteFileError(handle, static_cast<uint8_t*>(parms->m_buffer), handle->m_position)) {
        return false;
    }

    return result;
}

// PROVEN: af674117 IDENTICAL/IDENTICAL
// OFFSET: 0x007AB190/0x1404A1320
bool System_Mopaq::mopaq_flush(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    return parms->m_handle->m_handler->Flush();
}

// PROVEN: 35a1195f IDENTICAL/DIFFERENT
// OFFSET: 0x007AB1B0/0x1404A1330
bool System_Mopaq::mopaq_getfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    parms->m_offset = parms->m_handle->m_blockEntry.m_fileSize;
    return true;
}

// OFFSET: 0x007AB1D0/0x1404A1340
bool System_Mopaq::mopaq_setfilesize(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms) {
    Blizzard::Mopaq::MpqHandleRecord* handle = parms->m_handle;
    uint64_t old = handle->m_blockEntry.m_fileSize;
    handle->m_blockEntry.m_fileSize = parms->m_offset;

    if (static_cast<uint64_t>(parms->m_offset) > old) {
        return parms->m_handle->m_handler->AdjustSectorTable();
    }

    return true;
}

// OFFSET: 0x007CC4E0
static bool OpenMD5VerificationHandle(const System_SFile::BLOCKENTRY* entry, int32_t chunkSize, uint32_t flags, Blizzard::Mopaq::MpqHandleRecord** result) {
    Blizzard::Mopaq::MpqHandleRecord* handle = static_cast<Blizzard::Mopaq::MpqHandleRecord*>(SMemAlloc(sizeof(Blizzard::Mopaq::MpqHandleRecord), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (handle) {
        handle->m_baseOffset = 0;
        handle->m_refCount.m_value = 0;
        handle->m_signatureFailed = false;
    }

    *result = handle;

    System_Mopaq::MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_handle = handle;
    parms.m_blockEntry = entry;
    parms.m_count = chunkSize;

    handle->m_stack = System_Mopaq::FindIOStackManager(0x4000);
    parms.m_flags = flags;

    bool opened;

    if (handle->m_stack) {
        opened = handle->m_stack->m_open(handle->m_stack, &parms);

        if (opened) {
            return opened;
        }
    } else {
        opened = false;
    }

    if (*result) {
        Blizzard::Memory::Free(*result);
    }

    *result = NULL;
    return opened;
}

// OFFSET: 0x007EED00
bool PrepareHandleForMD5Verification(Blizzard::Mopaq::MpqHandleRecord* handle, int32_t chunkSize, uint32_t flags) {
    Blizzard::Mopaq::MpqHandleRecord* verifyHandle = NULL;

    if (!OpenMD5VerificationHandle(&handle->m_blockEntry, chunkSize, flags, &verifyHandle)) {
        return false;
    }

    if (!handle->m_handler) {
        handle->m_handler = new System_Mopaq::IOHandler(handle, *handle->m_archive->m_file, handle->m_blockEntry.m_offset);
    }

    verifyHandle->m_handler = new System_Mopaq::IOHandler(verifyHandle, handle->m_handler->m_base, handle->m_blockEntry.m_offset);
    if (System_MPQGlobals::GetMopaqGlobals()->m_disableRepair) {
        verifyHandle->m_errorHandler = new Blizzard::Mopaq::MpqErrorHandler();
    } else {
        verifyHandle->m_errorHandler = new Blizzard::Mopaq::MpqRepairOnDemandErrorHandler();
    }

    if (!verifyHandle->m_archive) {
        verifyHandle->m_archive = handle->m_archive;
    }

    if (!verifyHandle->m_name) {
        verifyHandle->m_name = handle->m_name;
    }

    verifyHandle->m_refCount.Increment();

    Blizzard::Mopaq::HSFILERecord& base = handle->m_handler->m_base;
    Blizzard::Mopaq::MpqHandleRecord* old = base.m_handle;
    base.m_handle = verifyHandle;
    verifyHandle->m_refCount.Increment();

    if (old) {
        old->Release();
    }

    verifyHandle->Release();

    Blizzard::Mopaq::MpqHandleRecord* baseHandle = handle->m_handler->m_base.m_handle;

    if (baseHandle && baseHandle->m_md5Verify) {
        baseHandle->m_md5Verify->m_baseOffset = handle->m_blockEntry.m_offset;
    }

    return true;
}

// INLINED
static Blizzard::Mopaq::HSFILERecord* WrapHandle(Blizzard::Mopaq::MpqHandleRecord* handle) {
    Blizzard::Mopaq::HSFILERecord* record = new Blizzard::Mopaq::HSFILERecord;

    if (record) {
        record->m_handle = handle;

        if (handle) {
            handle->m_refCount.Increment();
        }
    }

    return record;
}

// OFFSET: 0x007F7530
void System_Mopaq::OpenArchiveFile(const char* name, System_SFile::ARCHIVEREC* logicalArchive, System_SFile::ARCHIVEREC* archive, const System_SFile::BLOCKENTRY& entry, uint32_t index, uint32_t flags, Blizzard::Mopaq::HSFILERecord** result) {
    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_blockEntry = &entry;
    parms.m_blockIndex = index;
    parms.m_name = name;
    parms.m_path = NULL;
    parms.m_flags = flags;

    Blizzard::Mopaq::MpqHandleRecord* handle = GetPrealloc(name, archive, &entry, flags);

    if (handle) {
        handle->m_baseOffset = 0;
        handle->m_refCount.m_value = 0;
        handle->m_signatureFailed = false;
    }

    parms.m_handle = handle;

    int32_t stack = (flags & 0x20) ? 18 : 2;

    if (parms.m_blockEntry->m_md5Check && System_MPQGlobals::GetMopaqGlobals()->m_md5Mode && !(flags & 0x400)) {
        stack += 8;
    }

    if (flags & 0x200) {
        if (archive->m_attributeFlags & 0x5) {
            stack += 8;
        }

        System_SFile::PrepareArchiveForWriting(archive);
    }

    if (entry.m_flags & 0xF00000) {
        stack += 512;
    }

    parms.m_handle->m_stack = FindIOStackManager(stack);
    parms.m_handle->m_archive = archive;
    parms.m_handle->m_logicalArchive = logicalArchive;

    handle = parms.m_handle;

    if (handle->m_stack) {
        bool opened = handle->m_stack->m_open(handle->m_stack, &parms);

        if (opened && !(entry.m_flags & 0xF00000)) {
            int32_t chunkSize = archive->m_header.m_rawChunkSize;

            if (chunkSize > 0) {
                opened = PrepareHandleForMD5Verification(parms.m_handle, chunkSize, flags);
            }
        }

        if (opened) {
            *result = WrapHandle(parms.m_handle);
            return;
        }

        handle = parms.m_handle;
    }

    *result = NULL;

    if (handle) {
        Blizzard::Memory::Free(handle);
    }
}

// OFFSET: 0x007F31A0
void System_Mopaq::CreateArchiveFile(const char* name, System_SFile::ARCHIVEREC* archive, uint32_t flags, const System_SFile::BLOCKENTRY& entry, uint32_t index, Blizzard::Mopaq::HSFILERecord** result) {
    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_blockIndex = index;
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_blockEntry = &entry;
    parms.m_name = name;
    parms.m_flags = flags | 0x4;

    Blizzard::Mopaq::MpqHandleRecord* handle = GetPrealloc(name, archive, &entry, flags | 0x4);

    if (handle) {
        handle->m_baseOffset = 0;
        handle->m_refCount.m_value = 0;
        handle->m_signatureFailed = false;
    }

    parms.m_handle = handle;

    int32_t stack = 2;

    if ((archive->m_attributeFlags & 0x5) && !(flags & 0x400)) {
        stack = 10;
    }

    if (flags & 0x20) {
        stack += 16;
    }

    if (entry.m_flags & 0x100000) {
        stack += 1024;
    }

    parms.m_handle->m_stack = FindIOStackManager(stack);
    parms.m_handle->m_archive = archive;

    handle = parms.m_handle;

    if (handle->m_stack) {
        bool opened = handle->m_stack->m_open(handle->m_stack, &parms);

        if (opened) {
            int32_t chunkSize = archive->m_header.m_rawChunkSize;

            if (chunkSize > 0 && (entry.m_compressedSize || entry.m_fileSize)) {
                opened = PrepareHandleForMD5Verification(parms.m_handle, chunkSize, flags);
            }
        }

        if (opened) {
            *result = WrapHandle(parms.m_handle);
            return;
        }

        handle = parms.m_handle;
    }

    *result = NULL;

    if (handle) {
        Blizzard::Memory::Free(handle);
    }
}

// OFFSET: 0x007CC5E0/0x1404C8AF0
void System_Mopaq::OpenDiskFile(const char* name, uint32_t flags, const char* path, System_SFile::ARCHIVEREC* archive, Blizzard::Mopaq::HSFILERecord** result) {
    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_name = name;
    parms.m_path = path;

    Blizzard::Mopaq::MpqHandleRecord* handle = static_cast<Blizzard::Mopaq::MpqHandleRecord*>(SMemAlloc(sizeof(Blizzard::Mopaq::MpqHandleRecord), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (handle) {
        handle->m_baseOffset = 0;
        handle->m_refCount.m_value = 0;
        handle->m_signatureFailed = false;
    }

    parms.m_handle = handle;
    parms.m_flags = (flags & 0x40) | ((flags & 0x4) ? 0x202 : 0x1000) | ((flags & 0x8000 | 0x5000) >> 12);

    int32_t stack = (flags & 0x20) ? 17 : 1;

    if (flags & 0x40) {
        stack += 192;
    }

    if (flags & 0x80) {
        stack += 256;
    }

    parms.m_handle->m_stack = FindIOStackManager(stack);
    handle = parms.m_handle;

    if (handle->m_stack && handle->m_stack->m_open(handle->m_stack, &parms)) {
        *result = WrapHandle(parms.m_handle);
        return;
    }

    handle = parms.m_handle;
    *result = NULL;

    if (handle) {
        Blizzard::Memory::Free(handle);
    }
}

// OFFSET: 0x007CC750/0x1404C8CC0
void System_Mopaq::CreateDiskFile(const char* name, uint32_t flags, const char* path, System_SFile::ARCHIVEREC* archive, Blizzard::Mopaq::HSFILERecord** result) {
    MpqParams parms;
    memset(&parms, 0, sizeof(parms));
    parms.m_verb = offsetof(Blizzard::Mopaq::Mpqsystem, m_open);
    parms.m_name = name;
    parms.m_path = NULL;

    Blizzard::Mopaq::MpqHandleRecord* handle = static_cast<Blizzard::Mopaq::MpqHandleRecord*>(SMemAlloc(sizeof(Blizzard::Mopaq::MpqHandleRecord), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (handle) {
        handle->m_baseOffset = 0;
        handle->m_refCount.m_value = 0;
        handle->m_signatureFailed = false;
    }

    parms.m_handle = handle;
    parms.m_flags = ((!(flags & 0x1)) << 11) | 0x407;

    int32_t stack = (flags & 0x20) ? 17 : 1;

    parms.m_handle->m_stack = FindIOStackManager(stack);
    handle = parms.m_handle;

    if (handle->m_stack && handle->m_stack->m_open(handle->m_stack, &parms)) {
        *result = WrapHandle(parms.m_handle);
        return;
    }

    handle = parms.m_handle;
    *result = NULL;

    if (handle) {
        Blizzard::Memory::Free(handle);
    }
}
