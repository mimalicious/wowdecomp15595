#ifndef WOW_SOURCE_WORLDCLIENT_WORLDVIEW_H
#define WOW_SOURCE_WORLDCLIENT_WORLDVIEW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapObjRender.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"

class CM2Model;
class CM2Scene;
class CMapAreaLow;
class CMapBaseObjLink;
class CChunkLiquid;
class CWorldOccluder;
class CMapChunk;
class CMapDoodadDef;
class CMapEntity;
class CMapObj;
class CMapObjDef;
class CMapObjDefGroup;
class CMapObjGroup;
class CMapRenderChunk;
class CSceneMaterial;
class CShadowQuery;
class CSortEntry;
class CSortEntryChunk;
class CSortTable;
class CWorldOcclusion;
struct SMOPortal;
struct SMOPortalInfo;
struct SMOPortalRef;

class CPortalView {
    public:
    /* 0000 */ CAaRect m_rect;
    /* 0010 */ C3Vector m_pos;
    /* 001C */ float m_maxViewDepth;
    /* 0020 */ uint32_t m_vertStart;
    /* 0024 */ uint32_t m_vertCount;

    CPortalView();
    CPortalView(const CAaRect& rect, float maxViewDepth);
    void Clear();
    void Merge(const CPortalView& other);
};

struct SPortalExt {
    /* 0000 */ uint16_t m_flags;
    /* 0004 */ CAaRect m_rect;
    /* 0014 */ uint32_t m_stamp;
    /* 0018 */ uint32_t unk_0018;
};

class CWorldView {
    public:
    struct CVisRenderChunk {
        /* 0000 */ CMapRenderChunk* m_renderChunk;
        /* 0004 */ uint32_t m_batch;
    };

    /* 0000 */ int32_t m_viewType;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ CWorldView* m_parentView;
    /* 000C */ CSortTable* m_sortTable;
    /* 0010 */ CWorldOcclusion* m_occlusion;
    /* 0014 */ CShadowQuery* m_shadowQuery;
    /* 0018 */ CMapObjRender m_mapObjRender;
    /* 0068 */ int32_t m_shadowMode;
    /* 006C */ CMapObjDef* m_viewerMapObjDef;
    /* 0070 */ CMapObjDef* m_viewerMovedMapObjDef;
    /* 0074 */ TSGrowableArray<uint32_t> m_viewerGroups;
    /* 0084 */ TSGrowableArray<uint32_t> m_viewerMovedGroups;
    /* 0094 */ uint32_t m_viewStamp;
    /* 0098 */ CAaBox m_frustumBounds;
    /* 00B0 */ C3Vector m_frustumCorners[8];
    /* 0110 */ CFrustum m_frustumStack[32];
    /* 1910 */ int32_t m_frustumIndex;
    /* 1914 */ C3Vector m_camPos;
    /* 1920 */ C3Vector m_camTarget;
    /* 192C */ C3Vector m_camDir;
    /* 1938 */ float m_nearClip;
    /* 193C */ float m_farClip;
    /* 1940 */ float m_fov;
    /* 1944 */ float m_aspect;
    /* 1948 */ C4Plane m_camPlane;
    /* 1958 */ C4Plane m_horizPlane;
    /* 1968 */ C4Plane m_reflectionPlane;
    /* 1978 */ float m_viewerExteriorDepth;
    /* 197C */ CPortalView m_frustumPortalView;
    /* 19A4 */ CPortalView m_exteriorPortalView;
    /* 19CC */ int32_t m_exteriorUnderwater;
    /* 19D0 */ int32_t m_underwater;
    /* 19D4 */ TSGrowableArray<CPortalView> m_exteriorPortalViews;
    /* 19E4 */ TSGrowableArray<CPortalView> m_portalViews;
    /* 19F4 */ int32_t m_renderExterior;
    /* 19F8 */ C44Matrix m_viewProjMat;
    /* 1A38 */ C44Matrix m_viewMat;
    /* 1A78 */ C44Matrix m_projMat;
    /* 1AB8 */ float m_viewportMinX;
    /* 1ABC */ float m_viewportMinY;
    /* 1AC0 */ float m_viewportMinZ;
    /* 1AC4 */ float m_viewportMaxX;
    /* 1AC8 */ float m_viewportMaxY;
    /* 1ACC */ float m_viewportMaxZ;
    /* 1AD0 */ CFrustum m_lowDetailFrustum;
    /* 1B90 */ C44Matrix m_matrix1B90;
    /* 1BD0 */ int32_t m_quadX;
    /* 1BD4 */ int32_t m_quadY;
    /* 1BD8 */ uint32_t m_camLiquidStatus;
    /* 1BDC */ float m_camDepth;
    /* 1BE0 */ float m_smoothCamDepth;
    /* 1BE4 */ float m_smoothCamDepthVel;
    /* 1BE8 */ C4Plane m_cullPlane;
    /* 1BF8 */ TSGrowableArray<SPortalExt> m_portalExts;
    /* 1C08 */ TSGrowableArray<C3Vector> m_portalVerts;
    /* 1C18 */ TSGrowableArray<CPortalView> m_cullPortalViews;
    /* 1C28 */ TSGrowableArray<CAaRect> m_cullRects;
    /* 1C38 */ C3Vector m_localCamPos;
    /* 1C44 */ C3Vector m_localCamTarget;
    /* 1C50 */ C4Plane m_localCamPlane;
    /* 1C60 */ C44Matrix m_localViewProj;
    /* 1CA0 */ C44Matrix m_viewProj;
    /* 1CE0 */ C44Matrix m_localView;
    /* 1D20 */ C44Matrix m_localToWorld;
    /* 1D60 */ CMapObjDef* m_cullMapObjDef;
    /* 1D64 */ CMapObjDef* m_cullMapObjDefPrev;
    /* 1D68 */ uint32_t m_cullStamp;
    /* 1D6C */ int32_t m_cullFromInterior;
    /* 1D70 */ int32_t m_cullValid;
    /* 1D74 */ int32_t m_hasLiquids;
    /* 1D78 */ TSGrowableArray<CMapObjDefGroup*> m_liquidGroups;
    /* 1D88 */ TSExplicitList<CMapObjDefGroup, offsetof(CMapObjDefGroup, m_moveableLink)> m_moveableGroups;
    /* 1D94 */ TSGrowableArray<CMapAreaLow*> m_lowDetailAreas;
    /* 1DA4 */ TSGrowableArray<CMapObjDef*> m_lowDetailMapObjDefs;
    /* 1DB4 */ TSGrowableArray<CVisRenderChunk> m_renderChunks[6][6];
    /* 1FF4 */ TSGrowableArray<CMapObjDefGroup*> m_alphaGroups;
    /* 2004 */ TSExplicitList<CMapEntity, offsetof(CMapEntity, m_viewLink)> m_list2004;
    /* 2010 */ TSExplicitList<CMapEntity, offsetof(CMapEntity, m_viewLink)> m_nonVisEntities;
    /* 201C */ TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> m_occluders;
    /* 2028 */ char m_subzoneName[260];
    /* 212C */ char m_zoneName[260];

    static float s_fadeStart[5];
    static float s_skyboxMinZ;

    static void ScaleFadeDist(float scale);
    static uint32_t CalcMinSizeCatFromDistSqr(float distSqr);
    static float DistSqrToBox(const CAaBox& box, const C3Vector& pos);
    static void FrustumCalcCorners(C3Vector* dest, const C3Vector* corners, const CAaRect& rect);
    static void FrustumSet(CFrustum* frustum, const C3Vector* corners, const CAaRect& rect);
    static void RectFromVerts(CAaRect& rect, const C3Vector* verts, uint32_t count);

    CWorldView(int32_t viewType);
    ~CWorldView();
    void AddDoodadDefModelToModelScene(CMapDoodadDef* doodadDef);
    void AddDoodadDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, int32_t entry);
    void AddChunkOccluders(const CSortEntryChunk& sortChunk, int32_t a2, int32_t a3, int32_t keep);
    void AddExteriorPortal(CPortalView& portalView);
    void AddLowDetailMapArea(CMapAreaLow* area);
    void AddLowDetailMapAreaOcean(CMapAreaLow* area);
    void AddLowDetailMapObjDef(CMapObjDef* mapObjDef);
    void AddMapChunk(CMapChunk* chunk, int32_t a2);
    void AddMapChunkToRenderList(CMapChunk* chunk);
    void AddMapEntity(CMapEntity* entity);
    void AddMapObjDef(CMapObjDef* mapObjDef);
    void AddMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* group);
    void AddMoveableMapObjDefGroups();
    void AddOccluder(const C3Vector& start, const C3Vector& end);
    void AddPortalViewExterior(CMapObj* mapObj, const SMOPortalInfo* portal, const SMOPortalRef* ref, uint16_t* ext, int32_t addViewer);
    void AddPortalViewInterior(CMapObj* mapObj, const SMOPortalInfo* portal, const SMOPortalRef* ref, uint16_t* ext, TSGrowableArray<CPortalView>* views);
    void CalcLowDetailFrustum(CFrustum& frustum, const CAaRect& rect);
    void CalcProjection(C44Matrix& projection);
    void ClearLists();
    int32_t ClipPolygon(const C3Vector* verts, uint32_t count, C3Vector*& outVerts, uint32_t& outCount);
    void ClipPortal(CMapObj* mapObj, const SMOPortalInfo* portal, uint16_t* ext);
    void ClipPortalExt(CMapObj* mapObj, const SMOPortalInfo* portal, SPortalExt* ext);
    void CreateInverseExteriorPortalList();
    void Cull();
    void CullChunk(CSortEntryChunk& entry);
    void CullChunkLiquids(CMapChunk* chunk, int32_t cull);
    void CullChunkReflection(CSortEntryChunk& entry);
    void CullChunks(CSortEntry* entry, int32_t index);
    void CullDoodads(CSortTable* table, int32_t index, uint16_t minSize);
    void CullDoodadsExterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, uint16_t minSize, int32_t a3, int32_t a4);
    void CullDoodadsInterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, uint16_t minSize, int32_t a3);
    void CullEntitys(CSortEntry* entry);
    void CullEntitysInterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, int32_t a2);
    void CullLowDetail(CSortEntry* entry, const CFrustum& frustum);
    void CullMapObjDefGroup(CMapObjDefGroup* group, int32_t a2);
    void CullMapObjDefGroupContents(CMapObjDefGroup* group);
    void CullMapObjDefGroupFromExterior(CMapObjDefGroup* group, const CAaRect& rect);
    void CullMapObjDefGroupFromInterior(CMapObjDefGroup* group, const CAaRect& rect);
    void CullMapObjDefGroups(CSortEntry* entry, const CAaRect& rect, int32_t index);
    int32_t CullModel(const CAaBox& box);
    int32_t CullModel(const CAaBox& box, uint32_t mask);
    void CullModelTree(CM2Model* model);
    void CullModelTree(CM2Model* model, uint32_t boneId);
    void CullMoveableMapObjDefGroups(const CAaRect& rect);
    void CullPortals(CMapObj* mapObj, uint32_t groupNum, uint32_t prevGroup, const CAaRect& rect, uint32_t depth, int32_t fromInterior);
    void CullReflection();
    void CullSortTable(const CAaRect& rect);
    int32_t CullSphere(const C3Vector& center, float radius);
    void EnableOcclusion(int32_t enable);
    void EnableShadow(int32_t enable);
    void EnableSortTable(int32_t enable);
    void FrustumPop();
    void FrustumPush();
    void FrustumSet(const CAaRect& rect);
    int32_t FrustumCull(const CAaBox& box);
    void Gather();
    void GatherMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* group, CMapObj* mapObj, CMapObjGroup* mapObjGroup);
    void GatherMapObjDefGroupLiquids(CMapObjDef* mapObjDef, CMapObjDefGroup* group, CMapObj* mapObj, CMapObjGroup* mapObjGroup);
    void GatherMapObjDefOccluders(CMapObjDef* mapObjDef);
    void GetCameraClosestPoint(const CAaBox& box, C3Vector& point);
    int32_t IsBehindCullPlane(const CAaBox& box);
    int32_t OcclusionQueryBox(const CAaBox& box, uint32_t flags);
    int32_t OcclusionQuerySphere(const CAaSphere& sphere, uint32_t flags);
    void Render();
    void RenderAlpha();
    void RenderM2SkyboxPass(CM2Scene* scene);
    void RenderOpaque();
    void RenderSceneClear();
    void SetupMapObjDefCull(CMapObjDef* mapObjDef);
    int32_t TransformAndClipVerts(int32_t a1, const C3Vector* verts, uint32_t count, const C3Vector& offset, C3Vector*& outVerts, uint32_t& outCount);
    void Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect);
    void UpdateOcclusion(CSortEntry* entry, int32_t index, int32_t a3);
    void UpdateNonVisEntitys();
    void UpdateShadows();
    void UpdateViewMat();
    void UpdateViewer();
    void UpdateViewerLiquid();
    void UpdateViewerUnderwater();
};

#include "WoW/Source/WorldClient/WorldView.inl"

#endif
