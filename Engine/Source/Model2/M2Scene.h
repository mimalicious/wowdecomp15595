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
class CAaBox;
class CImVector;
class CM2Cache;
class CM2Light;
class CM2Lighting;
class CM2Model;
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

class CM2Scene {
    public:
    /* 0000 */ CM2Scene** m_scenePrev;
    /* 0004 */ CM2Scene* m_sceneNext;
    /* 0008 */ int32_t m_refCount;
    /* 000C */ CM2Cache* m_cache;
    /* 0010 */ CM2Model* m_modelList;
    /* 0014 */ CM2Model* m_recycleList;
    /* 0018 */ uint32_t m_time;
    /* 001C */ uint32_t m_timeDelta;
    /* 0020 */ uint32_t m_frameStamp;
    /* 0024 */ float m_float18;
    /* 0028 */ uint32_t m_flags;
    /* 002C */ CM2Light* m_lightList;
    /* 0030 */ CM2Light** m_lightGrid;
    /* 0034 */ CM2Model* m_animateList;
    /* 0038 */ CM2Model* m_drawList;
    /* 003C */ CM2Model* m_particleList;
    /* 0040 */ TSGrowableArray<M2Element> m_elements;
    /* 0050 */ TSGrowableArray<uint32_t> m_passElements [6];
    /* 00B0 */ TSGrowableArray<uint32_t> m_doodadElements;
    /* 00C0 */ TSGrowableArray<uint32_t> m_doodadCounts;
    /* 00D0 */ C44Matrix m_view;
    /* 0110 */ C44Matrix m_viewInv;
    /* 0150 */ void (*m_projectTextureCallback)(const CAaBox&, const CImVector&, int32_t, void*, int32_t);
    /* 0154 */ void* m_projectTextureParam;
    /* 0158 */ int32_t (*m_projectPositionCallback)(const C3Vector&, float&, void*);
    /* 015C */ void* m_projectPositionParam;
    /* 0160 */ CM2Model* m_hitTestList;
    /* 0164 */ M2HitRec* m_hitRecs;
    /* 0168 */ uint32_t* m_hitOrder;
    /* 016C */ uint32_t m_hitCapacity;
    /* 0170 */ C3Vector* m_hitVerts;
    /* 0174 */ uint32_t m_hitVertCapacity;
    /* 0178 */ M2HitRec m_lastHit;
    /* 0188 */ void* m_lastHitOwner;
    /* 018C */ uint32_t m_liquidTypeId;
    /* 0190 */ uint32_t m_shadowValue;
    /* 0194 */ uint32_t m_passMask;
    /* 0198 */ uint32_t m_drawing;
    /* 019C */ CTexture* m_litSphereTexture;
    /* 01A0 */ CShaderEffect* m_litSphereEffect;

    static uint32_t s_optFlags;

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
    CM2Model* CreateModel(const char* fileName, uint32_t flags);
    void Draw(M2PASS pass);
    CM2Model* DuplicateModel(CM2Model* model, uint32_t flags);
    void EndDraw();
    void* EndHitTest(const C3Vector& start, const C3Vector& end, float& t, int32_t twoPass);
    void* EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float& t);
    void GarbageCollectModels();
    M2HitRec* HitTestCollision(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best);
    M2HitRec* HitTestGeometry(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best);
    M2HitRec* IntersectHitTestTriangles(const uint16_t* indices, const uint16_t* end, uint32_t vertexStart, const C3Vector& start, uint32_t pass, M2HitRec* rec, float& t, M2HitRec* best);
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

uint32_t ParticleBlendToM2Blend(uint32_t blend);
uint32_t ParticleFlagsToM2Flags(uint32_t flags);

#endif
