#include <cstddef>
#include <cstdint>
#include <new>
#include "WoW/Common/TextureCache.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/BLPFile/blp.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00E4DDF4
static HASHKEY_NONE s_cacheKey;

// STATIC: 0x00E4DDF8/0x140D37100
static TSObjectAllocPool<CACHEENTRY>* s_entryPool;

// STATIC: 0x00E4DDFC/0x140D37108
static int32_t s_loadCount;

// STATIC: 0x00E4DE00
static TSHashTable<CACHEENTRY, HASHKEY_NONE> s_cacheTable;

// OFFSET: 0x00703BC0/0x1403D0630
static void FreeRequest(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00703BF0/0x1403D0670
static void LoadSuccessCallback(CAsyncObject* object) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(object->m_userArg);

    AsyncFileReadDestroyObject(entry->m_asyncObject);
    BLPHeader* header = entry->m_data;
    entry->m_asyncObject = NULL;
    CBLPFile::ValidateHeader(*header);

    entry->m_info.m_alphaSize = header->m_alphaSize;
    entry->m_info.m_opaque = header->m_alphaSize == 0;
    entry->m_info.m_width = header->m_width;
    entry->m_info.m_height = header->m_height;
    entry->m_info.m_mipCount = TextureCalcMipCount(entry->m_info.m_width, entry->m_info.m_height);
}

// OFFSET: 0x00703C60/0x1403D0700
static void LoadFailedCallback(CAsyncObject* object) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(object->m_userArg);
    void* buffer = entry->m_asyncObject->m_buffer;

    AsyncFileReadDestroyObject(entry->m_asyncObject);
    entry->m_asyncObject = NULL;
    SMemFree(buffer, __FILE__, __LINE__, 0);
    entry->m_data = NULL;
    entry->m_size = 0;
    entry->m_loadFailed = 1;
}

// OFFSET: 0x00703CC0/0x1403D0770
int32_t CACHEENTRY::LoadTexture() {
    SFileImpl* file = NULL;

    if (!SFile::OpenEx(NULL, this->m_fileName, 0, &file)) {
        char alternate[260];
        TEXFILETYPE type = TextureDiscoverFileType(this->m_fileName);
        TexturePickAlternateFilename(this->m_fileName, type, alternate, sizeof(alternate));
        SFile::OpenEx(NULL, alternate, 0, &file);
    }

    if (!file) {
        this->m_loadFailed = 1;
        return 0;
    }

    this->m_size = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<BLPHeader*>(SMemAlloc(this->m_size, __FILE__, __LINE__, 0));
    AsyncFileReadCreateObject(file, 3, 0, &this->m_asyncObject, this->m_data, this, &LoadSuccessCallback, &LoadFailedCallback, 0, 0, 0);

    return 1;
}

// OFFSET: 0x00703DA0/0x1403D08A0
void* TextureCacheCopyTexture(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return entry;
    }

    entry->m_refCount++;

    return entry;
}

// OFFSET: 0x00703DC0/0x1403D08C0
void* TextureCacheGetMip(void* handle, uint32_t mipLevel) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry || entry->m_loadFailed || !entry->m_data || mipLevel >= entry->m_info.m_mipCount) {
        return NULL;
    }

    return reinterpret_cast<uint8_t*>(entry->m_data) + entry->m_data->m_mipOffsets[mipLevel];
}

// OFFSET: 0x00703E00/0x1403D0900
BlpPalPixel* TextureCacheGetPal(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry || entry->m_loadFailed || !entry->m_data || entry->m_data->m_colorEncoding != 1) {
        return NULL;
    }

    return entry->m_data->m_extended.m_palette;
}

// OFFSET: 0x00703E40/0x1403D0930
int32_t TextureCacheHasMips(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    return entry && entry->m_data && !entry->m_loadFailed;
}

// OFFSET: 0x00703E70/0x1403D0960
int32_t TextureCacheGetInfo(void* handle, TCTEXTUREINFO& info, int32_t wait) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return 0;
    }

    if (!entry->m_loadFailed) {
        if (!entry->m_data) {
            entry->LoadTexture();
        }

        if (!entry->m_loadFailed) {
            if (!entry->m_info.m_width && entry->m_asyncObject) {
                if (!wait) {
                    return 0;
                }

                DebugLog("TextureCacheGetInfo() Wait: %s\n", entry->m_fileName);
                AsyncFileReadWait(entry->m_asyncObject);
            }

            info = entry->m_info;

            return 1;
        }
    }

    info = entry->m_info;

    return 1;
}

// OFFSET: 0x00703F10/0x1403D0A00
void TextureCacheIncreasePriority(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (entry && !entry->m_loadFailed && !entry->m_info.m_width && entry->m_asyncObject) {
        AsyncFileReadEnterQueueLock();
        AsyncFileReadLinkToPriority(entry->m_asyncObject, 1);
        AsyncFileReadLeaveQueueLock();
    }
}

// OFFSET: 0x00703F60/0x1403D0A50
void TextureCacheResetLoadCount() {
    s_loadCount = 0;
}

// OFFSET: 0x00703F70/0x1403D0A60
void TextureCacheComputeLoadProgress(void* handle, int64_t& local, int64_t& total) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (!entry) {
        return;
    }

    if (!entry->m_loadFailed && !entry->m_data) {
        entry->LoadTexture();
    }

    SFile::FileGetIsLocalAmount(entry->m_fileName, local, total);

    if (local != total) {
        total++;

        if (!entry->m_asyncObject) {
            local++;
        }
    }
}

// OFFSET: 0x00704070/0x1403D0AD0
CACHEENTRY::~CACHEENTRY() {
    int32_t freeData = 1;

    if (this->m_asyncObject) {
        freeData = AsyncFileReadCancel(this->m_asyncObject, &FreeRequest);
        this->m_asyncObject = NULL;
    }

    if (this->m_data && freeData) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
    }

    this->m_size = 0;
    this->m_data = NULL;
    this->m_fileName[0] = '\0';
}

// OFFSET: 0x007040E0
CACHEENTRY::CACHEENTRY() {
    this->m_info.m_mipCount = 0;
    this->m_info.m_opaque = 1;
    this->m_info.m_alphaSize = 0;
    this->m_info.m_width = 0;
    this->m_info.m_height = 0;
    this->m_size = 0;
    this->m_loadFailed = 0;
    this->m_asyncObject = NULL;
    this->m_fileName[0] = '\0';
    this->m_refCount = 0;
    this->m_data = NULL;
}

// OFFSET: 0x00704370
static void TextureCacheFreeEntry(CACHEENTRY* entry) {
    s_entryPool->Delete(entry);
}

// OFFSET: 0x007043B0/0x1403D0DE0
void TextureCacheDestroyTexture(void* handle) {
    CACHEENTRY* entry = static_cast<CACHEENTRY*>(handle);

    if (entry && --entry->m_refCount <= 0) {
        s_cacheTable.Unlink(entry);
        TextureCacheFreeEntry(entry);
    }
}

// OFFSET: 0x00704480/0x1403D0F10
static CACHEENTRY* TextureCacheAllocEntry() {
    if (!s_entryPool) {
        void* pool = SMemAlloc(sizeof(TSObjectAllocPool<CACHEENTRY>), __FILE__, __LINE__, 0);
        s_entryPool = pool ? new (pool) TSObjectAllocPool<CACHEENTRY>("TCACHEENTRY", 1024) : NULL;
    }

    return s_entryPool->New("", 0);
}

// OFFSET: 0x007044E0/0x1403D0FB0
void TextureCacheDestroy() {
    delete s_entryPool;
    s_entryPool = NULL;
}

// OFFSET: 0x00704BB0/0x1403D18A0
void* TextureCacheCreateTexture(const char* fileName) {
    uint32_t hashval = SStrHash(fileName, 0, 0);
    CACHEENTRY* entry = s_cacheTable.Ptr(hashval, s_cacheKey);

    if (entry) {
        entry->m_refCount++;
        return entry;
    }

    entry = TextureCacheAllocEntry();
    s_cacheTable.Insert(entry, hashval, s_cacheKey);
    SStrCopy(entry->m_fileName, fileName, sizeof(entry->m_fileName));
    entry->m_refCount++;

    return entry;
}
