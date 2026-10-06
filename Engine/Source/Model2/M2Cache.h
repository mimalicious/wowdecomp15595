#ifndef ENGINE_SOURCE_MODEL2_M2CACHE_H
#define ENGINE_SOURCE_MODEL2_M2CACHE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"

class CM2Scene;

class CM2ModelHeap {
    public:
    /* 0000 */ uint32_t m_heapId;

    void Delete(CM2Model* model);
    CM2Model* New();
};

class CM2Cache {
    public:
    static uint32_t s_processorFeatures;

    static void OnShaderModelChanged(void* param);
    static uint32_t STORMAPI ThreadProc(void* param);

    /* 0000 */ CM2Scene* m_sceneList;
    /* 0004 */ int32_t m_initialized;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint32_t m_requestedFlags;
    /* 0010 */ CM2Shared* m_freeList;
    /* 0014 */ CM2Shared** m_freeListTail;
    /* 0018 */ CM2Shared* m_shared [1021];
    /* 100C */ SThread m_thread;
    /* 1010 */ SEvent m_threadStart;
    /* 1014 */ SEvent m_threadDone;
    /* 1018 */ HPROPCONTEXT m_propContext;
    /* 101C */ void (*m_threadFunc)(void*);
    /* 1020 */ void* m_threadParam;
    /* 1024 */ char unk_1024 [64];
    /* 1064 */ char unk_1064 [64];
    /* 10A4 */ uint32_t m_updateTime;
    /* 10A8 */ TSExplicitList<CM2Shared, offsetof(CM2Shared, m_updateLink)> m_sharedUpdateList;

    CM2Cache();
    ~CM2Cache();
    void BeginThread(void (*func)(void*), void* param);
    void ClearSharedHash();
    CM2Shared* CreateShared(const char* name, uint32_t flags);
    void Destroy();
    void GarbageCollect(int32_t force);
    int32_t Initialize(uint32_t flags);
    void LinkToSharedUpdateList(CM2Shared* shared);
    CM2Shared* ReloadShared(const char* name);
    void UpdateShaderFlags();
    void UpdateShared();
    void WaitThread();
};

extern CM2ModelHeap* g_modelPool;

CM2Scene* M2CreateScene();
void M2Destroy();
uint32_t M2GetCacheFlags();
void M2Initialize(uint32_t flags, uint32_t modelCount);
void M2SetCacheFlags(uint32_t flags);
void M2SetGlobalOptFlags(uint32_t flags);

#endif
