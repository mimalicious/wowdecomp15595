#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJ_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJ_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

namespace World {
    struct FacetData;
    struct LiquidQueryResult;
    struct MinimapQuad;
    struct TriData;
}

class CAsyncObject;
class CFrustum;
class CMapObjDef;
class CMapObjDefGroup;
class CMapObjGroup;
class CTexture;
class CWorldView;
struct C3Segment;

struct CWorldQueryElement {
    /* 0000 */ CMapObjGroup* m_group;
    /* 0004 */ const C44Matrix* m_matrix;
};

struct SMOHeader {
    /* 0000 */ uint32_t m_textureCount;
    /* 0004 */ uint32_t m_groupCount;
    /* 0008 */ uint32_t m_portalCount;
    /* 000C */ uint32_t m_lightCount;
    /* 0010 */ uint32_t m_doodadNameCount;
    /* 0014 */ uint32_t m_doodadDefCount;
    /* 0018 */ uint32_t m_doodadSetCount;
    /* 001C */ CImVector m_ambColor;
    /* 0020 */ uint32_t m_wmoId;
    /* 0024 */ CAaBox m_bbox;
    /* 003C */ uint16_t m_flags;
    /* 003E */ uint16_t m_lodCount;
};

struct SMOGroupInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ CAaBox m_bbox;
    /* 001C */ int32_t m_nameOffset;
};

struct SMOPoly {
    /* 0000 */ uint8_t m_flags;
    /* 0001 */ uint8_t m_materialId;
};

struct SMOPortalInfo {
    /* 0000 */ uint16_t m_startVertex;
    /* 0002 */ uint16_t m_count;
    /* 0004 */ C4Plane m_plane;
};

struct SMOPortalRef {
    /* 0000 */ uint16_t m_portalIndex;
    /* 0002 */ uint16_t m_groupIndex;
    /* 0004 */ int16_t m_side;
    /* 0006 */ uint16_t m_filler;
};

struct SMOVisibleBlock {
    /* 0000 */ uint16_t m_firstVertex;
    /* 0002 */ uint16_t m_count;
};

struct SMOLight {
    /* 0000 */ uint8_t m_type;
    /* 0001 */ uint8_t m_useAtten;
    /* 0002 */ uint8_t m_pad[2];
    /* 0004 */ CImVector m_color;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ float m_intensity;
    /* 0018 */ float m_unknown[4];
    /* 0028 */ float m_attenStart;
    /* 002C */ float m_attenEnd;
};

struct SMODoodadSet {
    /* 0000 */ char m_name[20];
    /* 0014 */ uint32_t m_startIndex;
    /* 0018 */ uint32_t m_count;
    /* 001C */ uint32_t m_pad;
};

struct SMODoodadDef {
    /* 0000 */ uint32_t m_nameIndex;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ float m_rotation[4];
    /* 0020 */ float m_scale;
    /* 0024 */ CImVector m_color;
};

struct SMOFog {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_pos;
    /* 0010 */ float m_start;
    /* 0014 */ float m_end;
    /* 0018 */ float m_fogEnd;
    /* 001C */ float m_fogStartScalar;
    /* 0020 */ uint32_t m_fogColor;
    /* 0024 */ float m_uwFogEnd;
    /* 0028 */ float m_uwFogStartScalar;
    /* 002C */ uint32_t m_uwFogColor;
};

struct SMOMaterial {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_shader;
    /* 0008 */ uint32_t m_blendMode;
    /* 000C */ uint32_t m_texture1;
    /* 0010 */ CImVector m_sidnColor;
    /* 0014 */ CImVector m_frameSidnColor;
    /* 0018 */ uint32_t m_texture2;
    /* 001C */ CImVector m_diffColor;
    /* 0020 */ uint32_t m_groundType;
    /* 0024 */ uint32_t m_texture3;
    /* 0028 */ uint32_t m_color2;
    /* 002C */ uint32_t m_flags2;
    /* 0030 */ uint32_t m_runtimeData[4];
};

struct CMapObjMaterial {
    /* 0000 */ int32_t m_refCount;
    /* 0004 */ CTexture* m_textures[3];
};

class CMapObjOccluder {
    public:
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ C3Vector m_start;
    /* 0010 */ C3Vector m_end;
    /* 001C */ TSLink<CMapObjOccluder> m_link;
};

class CMapObj : public CMapBaseObj, public TSHashObject<CMapObj, HASHKEY_NONE> {
    public:
    static int32_t bPoolsDirty;
    static TSObjectAllocPool<CMapObjOccluder>* s_occluderPool;
    static HASHKEY_NONE s_mapObjKey;
    static TSHashTable<CMapObj, HASHKEY_NONE> s_mapObjHash;
    static float s_closestDistSq;

    static void AsyncCallback(CAsyncObject* object);
    static void AsyncFailedCallback(CAsyncObject* object);
    static void AsyncFreeCallback(CAsyncObject* object);
    static CMapObjOccluder* AllocOccluder();
    static void ClearCache();
    static CMapObj* Create(const char* name);
    static uint32_t CreateWmoIgnoreFlags(uint32_t flags);
    static void Delete(CMapObj* mapObj);
    static void Destroy();
    static void DestroyPools();
    static void FreeOccluder(CMapObjOccluder* occluder);
    static void Initialize();
    static void LinkToAsyncHead(void* param);
    static void RenderMapObjGroupsColor(const CWorldQueryElement* groups, uint32_t count, const C44Matrix& world, const CImVector& color);
    static void RenderMapObjGroupsShadowMap(const CWorldQueryElement* groups, uint32_t count, const C44Matrix& world, const CFrustum& frustum);
    static void Update();
    static void UpdateGroups(CMapObj* mapObj);
    static void UpdatePools();

    /* 0028 */ float m_distSq;
    /* 002C */ CAsyncObject* m_asyncObject;
    /* 0030 */ uint32_t m_mapObjGroupCount;
    /* 0034 */ CMapObjGroup* m_groups[512];
    /* 0834 */ uint32_t unk_0834;
    /* 0838 */ CAaBox m_bbox;
    /* 0850 */ char m_name[260];
    /* 0954 */ uint8_t* m_data;
    /* 0958 */ uint32_t m_dataSize;
    /* 095C */ SMOHeader* m_header;
    /* 0960 */ const char* m_textureNames;
    /* 0964 */ const char* m_groupNames;
    /* 0968 */ const char* m_skyboxName;
    /* 096C */ SMOGroupInfo* m_groupInfos;
    /* 0970 */ C3Vector* m_portalVertices;
    /* 0974 */ SMOPortalInfo* m_portalInfos;
    /* 0978 */ SMOPortalRef* m_portalRefs;
    /* 097C */ C3Vector* m_visBlockVertices;
    /* 0980 */ SMOVisibleBlock* m_visBlocks;
    /* 0984 */ SMOLight* m_lights;
    /* 0988 */ SMODoodadSet* m_doodadSets;
    /* 098C */ const char* m_doodadNames;
    /* 0990 */ SMODoodadDef* m_doodadDefs;
    /* 0994 */ SMOFog* m_fogs;
    /* 0998 */ C4Plane* m_convexVolumePlanes;
    /* 099C */ SMOMaterial* m_materials;
    /* 09A0 */ CMapObjMaterial* m_runtimeMaterials;
    /* 09A4 */ uint32_t m_textureNamesSize;
    /* 09A8 */ uint32_t m_groupNamesSize;
    /* 09AC */ uint32_t m_groupCount;
    /* 09B0 */ uint32_t m_portalVertexCount;
    /* 09B4 */ uint32_t m_portalCount;
    /* 09B8 */ uint32_t m_portalRefCount;
    /* 09BC */ uint32_t m_visBlockVertexCount;
    /* 09C0 */ uint32_t m_visBlockCount;
    /* 09C4 */ uint32_t m_lightCount;
    /* 09C8 */ uint32_t m_doodadSetCount;
    /* 09CC */ uint32_t m_doodadNamesSize;
    /* 09D0 */ uint32_t m_doodadDefCount;
    /* 09D4 */ uint32_t m_fogCount;
    /* 09D8 */ uint32_t m_convexVolumePlaneCount;
    /* 09DC */ uint32_t m_materialCount;
    /* 09E0 */ CImVector m_ambColor;
    /* 09E4 */ uint16_t m_minimapGroup;

    virtual ~CMapObj();

    CMapObj();
    void AttenTransVerts(CMapObjGroup* group);
    void CalcPortalCenterInWorldSpace(const SMOPortalInfo* portal, const C44Matrix& matrix, C3Vector& pos);
    float CalcPortalFarthestDistance(const SMOPortalInfo* portal, const C4Plane& plane);
    bool ClosestPortal(uint32_t groupNum, const C3Vector& pos, float* dist, uint32_t* closestGroup);
    void Clear();
    int32_t Create();
    void CreateMaterial(uint32_t materialId);
    void CreateMaterials();
    int32_t CreatePointers();
    void DestroyMaterial(uint32_t materialId);
    void GatherGroup(CWorldView* view, CMapObjDefGroup* defGroup);
    void GetBounds(CAaBox& box);
    bool QueryLighting(uint32_t groupNum, const C3Segment& seg, CImVector& color, bool& exterior);
    bool QueryMapObjMinimap(uint32_t groupNum, const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool interior, bool wholeObject);
    void QueryMapObjMinimapGroup(uint32_t groupNum, uint32_t fromGroup, const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool interior, uint32_t flagsMatch, uint32_t depth);
    void GetMinimapQuads(const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads);
    void QueryLiquidSounds(uint32_t groupNum, uint32_t fromGroup, uint32_t depth, uint32_t* distance, const C3Vector& pos, TSGrowableArray<World::LiquidQueryResult>& results);
    bool GetTris(const CAaBox& box, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef);
    bool GetTris(const CFrustum& frustum, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef);
    bool GetTris(const C3Segment& seg, float& t, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef);
    bool GetFacets(const CAaBox& box, uint32_t flags, World::FacetData& facets, const CMapObjDef* mapObjDef, uint32_t groupNum);
    bool Intersect(const C3Vector& start, const C3Vector& end, float& t, uint32_t flags, uint32_t ignoreFlags, uint32_t groupNum, uint32_t& faceIndex);
    bool VectorIntersect(const C3Vector& start, const C3Vector& end, uint32_t flags, uint32_t groupMask, float* t, uint16_t* faceIndex, CMapObjDef* mapObjDef, float* dot, uint32_t* groupNum);
    int32_t GetDoodadSet(uint32_t doodadIndex);
    CMapObjGroup* GetGroup(uint32_t groupNum, int32_t loaded);
    void GetGroupBounds(CAaBox& box, uint32_t groupNum);
    uint32_t GetGroupFlags(uint32_t groupNum);
    SMOGroupInfo* GetGroupInfo(uint32_t groupNum);
    SMOMaterial* GetGroupMaterial(uint32_t groupNum);
    const char* GetGroupName(uint32_t groupNum);
    void Init();
    void InitPtrs();
    int32_t IsAllLoaded();
    int32_t IsGroupLoaded(uint32_t groupNum);
    int32_t QueryLighting(uint32_t groupNum, const C3Vector& pos, uint16_t polyIndex, CImVector& color, bool& exterior);
    int32_t QueryLiquid(uint32_t ignoreFlags, int32_t requireTiles, const C3Vector& pos, uint32_t& liquidType, float& height);
    int32_t Read(const char* name);
    void ReadGroup(uint32_t groupNum);
    void ReadGroup(uint32_t groupNum, int32_t wait);
    int32_t TestBounds(const CAaBox& box);
    int32_t TestBounds(const C3Vector& pos);
    int32_t TestBounds(const C3Vector& start, const C3Vector& end);
    int32_t TestConvexVolume(const C3Vector& pos);
    int32_t TestGroupBounds(const C3Vector& start, const C3Vector& end, uint32_t groupNum);
    int32_t TestGroupBounds(const CAaBox& box, uint32_t groupNum, int32_t requireLoaded);
    int32_t TestGroupBounds(const C3Vector& pos, uint32_t groupNum);
    bool VectorIntersectPortals(uint32_t groupNum, const C3Segment& seg, float& t, uint32_t* groups);
    void DistFromClosestExtPortal(uint32_t depth, uint32_t flags, const CMapObjGroup* group, const CMapObjGroup* fromGroup, const C3Vector& pos, float& dist);
    float DistFromClosestExtPortal(CMapObjGroup* group, const C3Vector& pos, int32_t outdoor);
    void UpdateMaterials();
    void UpdateSceneMaterials(CMapObjDefGroup* defGroup, uint32_t state);
    void UpdateTextures(CMapObjGroup* group);
    void WaitLoad();
    void WaitLoadGroup(uint32_t groupNum);
};

#endif
