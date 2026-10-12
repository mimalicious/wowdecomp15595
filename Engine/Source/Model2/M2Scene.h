#ifndef ENGINE_SOURCE_MODEL2_M2SCENE_H
#define ENGINE_SOURCE_MODEL2_M2SCENE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/Types.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"

class C3Vector;
class CAsyncObject;
class CAaBox;
class CImVector;
class CM2Cache;
class CM2Light;
class CM2Lighting;
class CM2Model;
class CM2Scene;
class CParticleEmitter2;
class CShaderEffect;
class CM2Shared;
class CTexture;

class M2HitRec {
    public:
    /* 0000 */ CM2Model* m_model;
    /* 0004 */ float m_tNear;
    /* 0008 */ float m_tFar;
    /* 000C */ uint32_t m_priority;
};

class M2Element {
    public:
    /* 0000 */ uint32_t m_typeKey;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ float m_alpha;
    /* 0010 */ float m_sortDepth;
    /* 0014 */ float m_depth;
    union {
        /* 0018 */ int32_t m_index;
        /* 0018 */ CParticleEmitter2* m_emitter;
    };
    /* 001C */ uint32_t m_doodadKey;
    /* 0020 */ int32_t m_priorityPlane;
    /* 0024 */ M2Batch* m_batch;
    /* 0028 */ M2SkinSection* m_skinSection;
    /* 002C */ CShaderEffect* m_effect;
    /* 0030 */ uint32_t m_vertexPermute;
    /* 0034 */ uint32_t m_hullPermute;
    /* 0038 */ uint32_t m_domainPermute;
    /* 003C */ uint32_t m_pixelPermute;
    /* 0040 */ uint32_t m_shadowValue;
    /* 0044 */ uint32_t m_sortKey;
};

class M2PreloadRecord {
    public:
    /* 0000 */ CAsyncObject* m_object;
    /* 0004 */ CM2Scene* m_scene;
    /* 0008 */ uint64_t m_key;
    /* 0010 */ char m_fileName [260];
    /* 0114 */ TSGrowableArray<CAsyncObject*>* m_waitList;

    void Set(CM2Scene* scene, const char* fileName, const uint64_t& key, TSGrowableArray<CAsyncObject*>* waitList);
};

class M2PreloadMap {
    public:
    struct Node {
        /* 0000 */ Node* m_next;
        /* 0008 */ uint64_t m_key;
        /* 0010 */ M2PreloadRecord m_record;
    };

    struct Value {
        /* 0000 */ uint64_t m_key;
        /* 0008 */ M2PreloadRecord m_record;
    };

    struct Iterator {
        /* 0000 */ Node* m_node;
        /* 0004 */ Node** m_bucket;
        /* 0008 */ Node** m_end;

        void Next();
    };

    struct InsertResult {
        /* 0000 */ Iterator m_where;
        /* 000C */ bool m_inserted;
    };

    /* 0000 */ size_t m_bucketCount;
    /* 0004 */ Node** m_buckets;
    /* 0008 */ size_t m_count;
    /* 000C */ float m_maxLoadFactor;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;

    M2PreloadMap();
    ~M2PreloadMap();
    Iterator Begin();
    Iterator End();
    Iterator Erase(Iterator where);
    int32_t Erase(const uint64_t& key);
    Iterator Find(const uint64_t& key);
    InsertResult Insert(const Value& value);
    void Rehash(uint32_t bucketCount);
};

class CM2Scene {
    public:
    /* 0000 */ CM2Scene** m_scenePrev;
    /* 0004 */ CM2Scene* m_sceneNext;
    /* 0008 */ int32_t m_refCount;
    /* 000C */ CM2Cache* m_cache;
    /* 0010 */ M2PreloadMap m_preloads;
    /* 0028 */ CM2Model* m_modelList;
    /* 002C */ CM2Model* m_recycleList;
    /* 0030 */ uint32_t m_time;
    /* 0034 */ uint32_t m_timeDelta;
    /* 0038 */ uint32_t m_frameStamp;
    /* 003C */ float m_float18;
    /* 0040 */ uint32_t m_flags;
    /* 0044 */ CM2Light* m_lightList;
    /* 0048 */ CM2Light** m_lightGrid;
    /* 004C */ CM2Model* m_animateList;
    /* 0050 */ CM2Model* m_drawList;
    /* 0054 */ CM2Model* m_particleList;
    /* 0058 */ TSGrowableArray<M2Element> m_elements;
    /* 0068 */ TSGrowableArray<uint32_t> m_passElements [6];
    /* 00C8 */ TSGrowableArray<uint32_t> m_doodadElements;
    /* 00D8 */ TSGrowableArray<uint32_t> m_doodadCounts;
    /* 00E8 */ C44Matrix m_view;
    /* 0128 */ C44Matrix m_viewInv;
    /* 0168 */ void (*m_projectTextureCallback)(const CAaBox&, const CImVector&, int32_t, void*, int32_t);
    /* 016C */ void* m_projectTextureParam;
    /* 0170 */ int32_t (*m_projectPositionCallback)(const C3Vector&, float&, void*);
    /* 0174 */ void* m_projectPositionParam;
    /* 0178 */ CM2Model* m_hitTestList;
    /* 017C */ M2HitRec* m_hitRecs;
    /* 0180 */ uint32_t* m_hitOrder;
    /* 0184 */ uint32_t m_hitCapacity;
    /* 0188 */ C3Vector* m_hitVerts;
    /* 018C */ uint32_t m_hitVertCapacity;
    /* 0190 */ M2HitRec m_lastHit;
    /* 01A0 */ void* m_lastHitOwner;
    /* 01A4 */ uint32_t m_liquidTypeId;
    /* 01A8 */ uint32_t m_shadowValue;
    /* 01AC */ uint32_t m_passMask;
    /* 01B0 */ uint32_t m_drawing;
    /* 01B4 */ CTexture* m_litSphereTexture;
    /* 01B8 */ CShaderEffect* m_litSphereEffect;

    static uint32_t s_optFlags;

    static int32_t AsyncLoad(const char* fileName, uint8_t priority, int32_t allocBuffer, TSGrowableArray<CAsyncObject*>* waitList, void (*succeeded)(CAsyncObject*), void (*failed)(CAsyncObject*), CM2Scene* scene);
    static int32_t GetSkinProfileName(uint32_t numSkinProfiles, uint32_t useShaders, const char* fileName, char* result, uint32_t size);
    static void ModelLoadSucceededCallback(CAsyncObject* object);
    static void PreloadCommonCallback(CAsyncObject* object);
    static int32_t SortAdditiveParticles(uint32_t a, uint32_t b, const void* param);
    static int32_t SortDoodad(uint32_t a, uint32_t b, const void* param);
    static int32_t SortDoodadKey(uint32_t a, uint32_t b, const void* param);
    static int32_t SortHitNear(uint32_t a, uint32_t b, const void* param);
    static int32_t SortOpaque(uint32_t a, uint32_t b, const void* param);
    static int32_t SortOpaqueGeoBatches(M2Element* a, M2Element* b);
    static int32_t SortOpaqueParticles(M2Element* a, M2Element* b);
    static int32_t SortOpaqueRibbons(M2Element* a, M2Element* b);
    static int32_t SortSortKey(uint32_t a, uint32_t b, const void* param);
    static int32_t SortTransparent(uint32_t a, uint32_t b, const void* param);

    CM2Scene(CM2Cache* cache);
    ~CM2Scene();
    void AddParticleElement(CParticleEmitter2* emitter, CM2Model* model, float depth, float alpha, int32_t above, int32_t below, uint32_t& index, uint32_t& additiveCount);
    void AdvanceTime(uint32_t delta);
    void AllocateSpaceForHitList();
    void Animate(const C3Vector& cameraPos);
    int32_t BeginDraw();
    void BeginHitTest();
    void ClearHitTestList();
    void ComputeElementShaders(M2Element* element);
    int32_t ComputeRayDirAndLen(const C3Vector& start, const C3Vector& end, float t, float* length, C3Vector* dir);
    CM2Model* CreateModel(const char* fileName, uint32_t flags, uint8_t priority);
    void Draw(M2PASS pass);
    CM2Model* DuplicateModel(CM2Model* model, uint32_t flags);
    void EndDraw();
    void* EndHitTest(const C3Vector& start, const C3Vector& end, float& t, int32_t twoPass);
    void* EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float& t);
    void GarbageCollectModels();
    M2HitRec* HitTestCollision(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best);
    M2HitRec* HitTestGeometry(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best);
    M2HitRec* IntersectHitTestTriangles(const uint16_t* indices, const uint16_t* end, uint32_t vertexStart, const C3Vector& start, uint32_t pass, M2HitRec* rec, float& t, M2HitRec* best);
    int32_t PrefetchModel(const char* fileName, uint8_t priority);
    void PreloadModel(const char* fileName, uint8_t priority, int32_t wait);
    void PurgeModelCache();
    int32_t Release();
    void ReloadModel(CM2Model* model, CM2Shared* shared);
    void SelectLights(CM2Lighting* lighting);
    void SetFloat18(float value);
    void SetProjectPositionCallback(int32_t (*callback)(const C3Vector&, float&, void*), void* param);
    void SetProjectTextureCallback(void (*callback)(const CAaBox&, const CImVector&, int32_t, void*, int32_t), void* param);
    void SetTime(uint32_t time);
    void SortAdditiveParticleElements(uint32_t pass);
    uint32_t SphereTestModels(const C3Vector& start, const C3Vector& dir, float length, int32_t frameCheck);
};

extern const uint32_t s_blendModes[6][7];
extern const uint32_t s_localLighting[7];

uint64_t M2HashFileName(const char* fileName, uint32_t length);
uint32_t ParticleBlendToM2Blend(uint32_t blend);
uint32_t ParticleFlagsToM2Flags(uint32_t flags);

#endif
