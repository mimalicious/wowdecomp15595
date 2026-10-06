#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/FileNotification.h"
#include "Engine/Source/Services/GfxSingletonManager.h"
#include "Engine/Source/Services/ModelBlob.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"

// STATIC: %0x00D88D38
CM2ModelHeap* g_modelPool;

// STATIC: %0x00D88D3C
uint32_t CM2Cache::s_processorFeatures;

// STATIC: %0x00D88D40
static CM2Cache s_cache;

// STATIC: %0x00ECDEA0
static float s_randomTable[128];

// OFFSET: %0x007789B0
void CM2Cache::ClearSharedHash() {
    for (uint32_t i = 0; i < sizeof(this->m_shared) / sizeof(this->m_shared[0]); i++) {
        CM2Shared* shared = this->m_shared[i];

        while (shared) {
            CM2Shared* next = shared->m_next;
            shared->m_previous = NULL;
            shared->m_next = NULL;
            shared = next;
        }

        this->m_shared[i] = NULL;
    }
}

// OFFSET: %0x00778A00
void CM2Cache::BeginThread(void (*func)(void*), void* param) {
    this->m_threadFunc = func;
    this->m_threadParam = param;
    this->m_threadStart.Set();
}

// OFFSET: %0x00778A30
void CM2Cache::WaitThread() {
    this->m_threadDone.Wait(0xFFFFFFFF);
}

// OFFSET: %0x00778A40
uint32_t STORMAPI CM2Cache::ThreadProc(void* param) {
    CM2Cache* cache = static_cast<CM2Cache*>(param);

    PropSelectContext(cache->m_propContext);
    cache->m_flags |= 0x400;
    cache->m_threadStart.Wait(0xFFFFFFFF);

    while (!(cache->m_flags & 0x800)) {
        cache->m_threadFunc(cache->m_threadParam);
        cache->m_threadDone.Set();
        cache->m_threadStart.Wait(0xFFFFFFFF);
    }

    cache->m_flags &= ~0x400u;

    return 0;
}

// OFFSET: %0x00778AC0
void M2SetGlobalOptFlags(uint32_t flags) {
    s_cache.m_flags |= flags & 0xE000;
}

// OFFSET: %0x00778AE0
CM2Scene* M2CreateScene() {
    CM2Scene* scene = new (SMemAlloc(sizeof(CM2Scene), __FILE__, __LINE__, 0)) CM2Scene(&s_cache);
    scene->m_scenePrev = &s_cache.m_sceneList;
    scene->m_sceneNext = s_cache.m_sceneList;
    s_cache.m_sceneList = scene;

    if (scene->m_sceneNext) {
        scene->m_sceneNext->m_scenePrev = &scene->m_sceneNext;
    }

    return scene;
}

// OFFSET: %0x00778B30
uint32_t M2GetCacheFlags() {
    return s_cache.m_flags;
}

// OFFSET: %0x00778B40
void M2SetCacheFlags(uint32_t flags) {
    s_cache.m_flags = flags;
}

// OFFSET: %0x00778B50
void CM2Cache::UpdateShaderFlags() {
    uint32_t requested = this->m_requestedFlags;

    if ((requested & 8) && g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        this->m_flags |= 8;
    } else {
        this->m_flags &= ~8u;
    }

    if ((this->m_flags & 8) || (CM2Cache::s_processorFeatures & 4)) {
        this->m_flags &= ~0x40u;
    } else {
        this->m_flags |= 0x40;
    }

    if ((requested & 2) && (g_theGxDevicePtr->m_caps.m_maxClipPlanes || (g_theGxDevicePtr->m_caps.unk_02BC && (this->m_flags & 8)))) {
        this->m_flags |= 2;
    } else {
        this->m_flags &= ~2u;
    }
}

// OFFSET: %0x00779000
CM2Shared* CM2Cache::ReloadShared(const char* name) {
    for (uint32_t i = 0; i < sizeof(this->m_shared) / sizeof(this->m_shared[0]); i++) {
        for (CM2Shared* shared = this->m_shared[i]; shared; ) {
            CM2Shared* next = shared->m_next;

            if (!_stricmp(shared->m_fileNameWithoutPath, name)) {
                if (shared->m_previous) {
                    *shared->m_previous = shared->m_next;
                }

                if (shared->m_next) {
                    shared->m_next->m_previous = shared->m_previous;
                }

                shared->m_previous = NULL;
                shared->m_next = NULL;

                if (shared == this->m_shared[i]) {
                    this->m_shared[i] = next;
                }

                return this->CreateShared(shared->m_fileName, 0);
            }

            shared = next;
        }
    }

    return NULL;
}

// OFFSET: %0x007790C0
static void ReloadModelCallback(const char* name) {
    SFile::RebuildHash();

    CM2Shared* shared = s_cache.ReloadShared(name);

    if (!shared) {
        return;
    }

    for (CM2Scene* scene = s_cache.m_sceneList; scene; scene = scene->m_sceneNext) {
        for (CM2Model* model = scene->m_modelList; model; ) {
            CM2Model* next = model->m_sceneNext;

            if (!_stricmp(model->m_shared->m_fileNameWithoutPath, name)) {
                scene->ReloadModel(model, shared);
            }

            model = next;
        }
    }

    shared->Release();
}

// OFFSET: %0x00779150
void CM2Cache::OnShaderModelChanged(void* param) {
    CM2Cache* cache = static_cast<CM2Cache*>(param);
    cache->UpdateShaderFlags();
    cache->ClearSharedHash();

    for (CM2Scene* scene = cache->m_sceneList; scene; scene = scene->m_sceneNext) {
        for (CM2Model* model = scene->m_modelList; model; ) {
            CM2Model* next = model->m_sceneNext;
            CM2Shared* shared = cache->CreateShared(model->m_shared->m_fileName, 0);
            scene->ReloadModel(model, shared);
            shared->Release();
            model = next;
        }
    }
}

// OFFSET: %0x007791D0
int32_t CM2Cache::Initialize(uint32_t flags) {
    if (this->m_initialized) {
        DebugLog("Model2: M2Initialize called more than once\n");
        return 1;
    }

    this->m_requestedFlags = flags;
    CM2Cache::s_processorFeatures = OsGetProcessorFeatures();

    if (OsGetNumberOfProcessors() > 1) {
        this->m_flags |= flags & 4;
    }

    if (this->m_flags & 4) {
        this->m_flags &= ~0xC00u;
        this->m_threadStart.Reset();
        this->m_threadDone.Reset();
        this->m_propContext = PropGetSelectedContext();
        SThread::Create(CM2Cache::ThreadProc, this, this->m_thread, const_cast<char*>("Model2"), 0);
    }

    if (g_theGxDevicePtr->m_caps.m_colorWrite) {
        this->m_flags |= flags & 1;
    }

    this->UpdateShaderFlags();
    this->m_flags |= flags & 0x21A0;

    uint32_t high = rand() << 16;
    CRndSeed seed(high | static_cast<uint16_t>(rand()));

    for (uint32_t i = 0; i < sizeof(s_randomTable) / sizeof(s_randomTable[0]); i++) {
        uint32_t bits = (CRandom::uint32_(seed) & 0x7FFFFF) | 0x3F800000;
        s_randomTable[i] = *reinterpret_cast<float*>(&bits) - 1.0f;
    }

    CParticleEmitter2::Init();
    FileNotification::RegisterDeferredFile("m2");
    FileNotification::RegisterDeferredFile("skin");
    FileNotification::RegisterDeferredFile("anim");
    FileNotification::RegisterManifestFile("lst", ReloadModelCallback);
    g_theGxDevicePtr->AddShaderModelChangeCallback(CM2Cache::OnShaderModelChanged, this);
    this->m_initialized = 1;

    return 1;
}

// OFFSET: %0x00778BC0
void CM2Cache::GarbageCollect(int32_t force) {
    uint32_t now = OsGetAsyncTimeMs();

    while (this->m_freeList) {
        CM2Shared* shared = this->m_freeList;

        if (!force && static_cast<int32_t>(now - shared->m_freeTime) < 100) {
            break;
        }

        this->m_freeList = shared->m_freeNext;

        if (shared->m_freeNext) {
            shared->m_freeNext->m_freePrev = &this->m_freeList;
        } else {
            this->m_freeListTail = &this->m_freeList;
        }

        shared->m_freePrev = NULL;
        shared->m_freeNext = NULL;
        delete shared;
    }
}

// OFFSET: %0x00779350
void CM2Cache::Destroy() {
    g_theGxDevicePtr->RemoveShaderModelChangeCallback(CM2Cache::OnShaderModelChanged, this);

    if (this->m_flags & 4) {
        this->m_flags |= 0x800;
        this->m_threadStart.Set();
        this->m_thread.Wait(0xFFFFFFFF);
    }

    while (this->m_freeList) {
        CM2Shared* shared = this->m_freeList;

        if (shared->m_freePrev) {
            *shared->m_freePrev = shared->m_freeNext;
        }

        if (shared->m_freeNext) {
            shared->m_freeNext->m_freePrev = shared->m_freePrev;
        } else {
            this->m_freeListTail = shared->m_freePrev;
        }

        shared->m_freePrev = NULL;
        shared->m_freeNext = NULL;
        delete shared;
    }

    this->ClearSharedHash();
    CParticleEmitter2::Destroy();
    this->m_initialized = 0;
}

// OFFSET: %0x00778C30
CM2Shared* CM2Cache::CreateShared(const char* name, uint32_t flags) {
    char path[260];
    SStrCopy(path, name, sizeof(path));
    _strlwr(path);

    char* ext = OsPathFindExtensionWithDot(path);

    if (!(flags & 0x1000)) {
        if (!*ext) {
            DebugLog("Model2: Invalid file extension: %s\n", path);
            return NULL;
        }

        if (!strcmp(ext, ".mdl") || !strcmp(ext, ".mdx")) {
            strcpy(ext, ".m2");
        } else if (strcmp(ext, ".m2")) {
            DebugLog("Model2: Invalid file extension: %s\n", path);
            return NULL;
        }
    }

    if (ext) {
        *ext = '\0';
    }

    CAaBox* boundingBox = NULL;
    CAaBox box;
    box.b = C3Vector();
    box.t = C3Vector();

    if (ModelBlobQuery(path, box.b, box.t)) {
        boundingBox = &box;
    }

    if (ext) {
        *ext = '.';
    }

    uint32_t fullPath = flags & 0x10;
    char* key;

    if (fullPath) {
        key = path;
    } else {
        key = ext;

        if (ext > path) {
            while (true) {
                if (*key == '\\' || *key == '/') {
                    key++;
                    break;
                }

                if (--key <= path) {
                    break;
                }
            }
        }
    }

    uint32_t hash = 0;

    for (const char* c = key; *c; c++) {
        hash = *c + 19 * hash;
    }

    CM2Shared** link = &this->m_shared[hash % (sizeof(this->m_shared) / sizeof(this->m_shared[0]))];
    int32_t cmp = 1;

    for (CM2Shared* shared = *link; shared; link = &shared->m_next, shared = shared->m_next) {
        if (shared->m_fileNameHash < hash) {
            continue;
        }

        if (shared->m_fileNameHash > hash) {
            break;
        }

        cmp = strcmp(fullPath ? shared->m_fileName : shared->m_fileNameWithoutPath, key);

        if (cmp >= 0) {
            break;
        }
    }

    if (*link && cmp <= 0) {
        (*link)->AddRef();
        return *link;
    }

    SFileImpl* file;

    if (!SFile::OpenEx(NULL, path, (flags >> 2) & 1, &file)) {
        DebugLog("Model2: File not found: %s\n", path);
        return NULL;
    }

    CM2Shared* shared = new (SMemAlloc(sizeof(CM2Shared), __FILE__, __LINE__, 0)) CM2Shared(this);

    if (!shared) {
        SFileCloseFile(file);
        return NULL;
    }

    if (!shared->Load(file, flags & 4, boundingBox)) {
        SFileCloseFile(file);
        delete shared;
        return NULL;
    }

    strcpy(shared->m_fileName, path);
    shared->m_fileNameHash = hash;
    shared->m_fileNameWithoutPath = OsPathFindExtensionWithDot(shared->m_fileName);

    if (shared->m_fileNameWithoutPath > shared->m_fileName) {
        while (true) {
            if (*shared->m_fileNameWithoutPath == '\\' || *shared->m_fileNameWithoutPath == '/') {
                shared->m_fileNameWithoutPath++;
                break;
            }

            if (--shared->m_fileNameWithoutPath <= shared->m_fileName) {
                break;
            }
        }
    }

    if (!(flags & 8)) {
        shared->m_previous = link;
        shared->m_next = *link;

        if (*link) {
            (*link)->m_previous = &shared->m_next;
        }

        *link = shared;
    }

    if (flags & 0x40) {
        shared->m_flags |= 0x40;
    }

    return shared;
}

// OFFSET: %0x007793F0
void M2Initialize(uint32_t flags, uint32_t modelCount) {
    s_cache.Initialize(flags);

    if (!modelCount) {
        modelCount = 1024;
    }

    g_modelPool = new (SMemAlloc(sizeof(CM2ModelHeap), __FILE__, __LINE__, 0)) CM2ModelHeap;

    if (g_modelPool) {
        g_modelPool->m_heapId = ObjectAllocAddHeap(sizeof(CM2Model), modelCount, "CM2Model", true);
    }
}

// OFFSET: %0x00779460
void M2Destroy() {
    s_cache.Destroy();
    ParticleSystemManager::Destroy();
    RibbonManager::Destroy();
    delete g_modelPool;
    g_modelPool = NULL;
}

// OFFSET: %0x007794A0
void CM2Cache::UpdateShared() {
    this->m_updateTime = OsGetAsyncTimeMs();

    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11) {
        return;
    }

    CM2Shared* shared = this->m_sharedUpdateList.Head();

    while (!this->m_sharedUpdateList.IsEnd(shared)) {
        CM2Shared* next = this->m_sharedUpdateList.RawNext(shared);

        if (this->m_updateTime - shared->m_lastUsedTime < 1000) {
            break;
        }

        shared->DestroyBuffers();
        shared->m_updateLink.Unlink();
        shared = next;
    }
}

// OFFSET: %0x00779560
void CM2Cache::LinkToSharedUpdateList(CM2Shared* shared) {
    shared->m_lastUsedTime = this->m_updateTime;
    this->m_sharedUpdateList.LinkToTail(shared);
}

// INLINED
CM2Cache::~CM2Cache() {
    if (this->m_initialized) {
        DebugLog("Model2: M2Destroy never called\n");
    }
}

// OFFSET: %0x00779620
CM2Cache::CM2Cache() : m_threadStart(0, 0), m_threadDone(0, 0) {
    this->m_freeList = NULL;
    this->m_freeListTail = &this->m_freeList;
    this->m_initialized = 0;
    this->m_flags = 0;
    this->m_sceneList = NULL;
    memset(this->m_shared, 0, sizeof(this->m_shared));
    this->m_threadFunc = NULL;
    this->m_threadParam = NULL;
    this->unk_1024[0] = '\0';
    this->unk_1064[0] = '\0';
}
