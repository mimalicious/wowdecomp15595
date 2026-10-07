#include "WorldView.h"
#include <cmath>
#include <cstring>
#include <new>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjRender.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Particulate.h"
#include "WoW/Source/WorldClient/ShadowQuery.h"
#include "WoW/Source/WorldClient/SortTable.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include "WoW/Source/WorldClient/WorldScene.h"

struct SFadeDistances {
    /* 0000 */ float m_minSize[5];
    /* 0014 */ float m_fadeEnd[5];
    /* 0028 */ float m_fadeRange[5];
};

// STATIC: 0x00D018BC
static SFadeDistances s_baseFadeDist = {
    { 1.0f, 4.0f, 15.0f, 100.0f, 100000.0f },
    { 30.0f, 100.0f, 200.0f, 750.0f, 2050.0f },
    { 5.0f, 10.0f, 15.0f, 20.0f, 50.0f },
};

// STATIC: 0x00D018F8
float CWorldView::s_fadeStart[5];

// STATIC: 0x00D0190C
static float s_fadeInner[5];

// STATIC: 0x00D01920
static float s_fadeInnerSqr[5];

// STATIC: 0x00D01934
static float s_fadeOuter[5];

// STATIC: 0x00D01948
static float s_fadeOuterSqr[5];

// STATIC: 0x00D01964
float CWorldView::s_skyboxMinZ = 0.99902344f;

// STATIC: 0x00D0195C
static uint32_t s_maxPortalDepth = 12;

// STATIC: 0x00D018B8
static int32_t s_entityCullDone = 1;

// STATIC: 0x00E75780
static C3Vector s_clipVerts[2][16];

// STATIC: 0x00E75700
static int32_t s_clipSide[17];

// STATIC: 0x00E75740
static float s_clipDist[17];

// STATIC: 0x00E75908
static C3Vector s_transformVerts[12];

// OFFSET: 0x00753360
void CWorldView::ScaleFadeDist(float scale) {
    for (uint32_t i = 0; i < 5; i++) {
        float outer = s_baseFadeDist.m_fadeEnd[i];

        if (i >= 1 && i <= 3) {
            outer = s_baseFadeDist.m_fadeEnd[i] * scale;
        }

        s_fadeStart[i] = s_baseFadeDist.m_minSize[i];
        s_fadeOuter[i] = outer;
        s_fadeOuterSqr[i] = outer * outer;
        s_fadeInner[i] = outer - s_baseFadeDist.m_fadeRange[i];
        s_fadeInnerSqr[i] = s_fadeInner[i] * s_fadeInner[i];
    }
}

// OFFSET: 0x00753510
uint32_t CWorldView::CalcMinSizeCatFromDistSqr(float distSqr) {
    if (distSqr < 0.0f || s_fadeOuterSqr[0] > distSqr) {
        return 0;
    }

    if (s_fadeOuterSqr[1] > distSqr) {
        return 1;
    }

    if (s_fadeOuterSqr[2] > distSqr) {
        return 2;
    }

    if (s_fadeOuterSqr[3] <= distSqr) {
        return 4;
    }

    return 3;
}

// OFFSET: 0x00753580
void CWorldView::Gather() {
    CWorldMap::Gather(this);
}

// OFFSET: 0x00753590
void CWorldView::CalcProjection(C44Matrix& projection) {
    float nearClip = this->m_farClip - 50.0f;
    GxuXformCreateProjection_Exact(this->m_fov * 0.60000002f, this->m_aspect, nearClip, World::GetHorizonFarClip(), projection);
}

// OFFSET: 0x007535F0
void CWorldView::FrustumPop() {
    this->m_frustumIndex--;
}

// OFFSET: 0x00753600
int32_t CWorldView::IsBehindCullPlane(const CAaBox& box) {
    const C3Vector* corners[2] = { &box.t, &box.b };
    const C4Plane& plane = this->m_cullPlane;

    float x = corners[*reinterpret_cast<const uint32_t*>(&plane.n.x) >> 31]->x;
    float y = corners[*reinterpret_cast<const uint32_t*>(&plane.n.y) >> 31]->y;
    float z = corners[*reinterpret_cast<const uint32_t*>(&plane.n.z) >> 31]->z;

    return x * plane.n.x + z * plane.n.z + y * plane.n.y + plane.d < -0.000099999997f;
}

// OFFSET: 0x00753690
void CWorldView::RectFromVerts(CAaRect& rect, const C3Vector* verts, uint32_t count) {
    rect.minY = 3.402823466e38f;
    rect.maxY = -3.402823466e38f;
    rect.minX = 3.402823466e38f;
    rect.maxX = -3.402823466e38f;

    for (uint32_t i = 0; i < count; i++) {
        if (rect.minY > verts[i].x) {
            rect.minY = verts[i].x;
        }

        if (verts[i].x > rect.maxY) {
            rect.maxY = verts[i].x;
        }

        if (rect.minX > verts[i].y) {
            rect.minX = verts[i].y;
        }

        if (verts[i].y > rect.maxX) {
            rect.maxX = verts[i].y;
        }
    }
}

// OFFSET: 0x00753AB0
float CWorldView::DistSqrToBox(const CAaBox& box, const C3Vector& pos) {
    float x = pos.x < box.b.x ? box.b.x : (pos.x <= box.t.x ? pos.x : box.t.x);
    float y = pos.y < box.b.y ? box.b.y : (pos.y <= box.t.y ? pos.y : box.t.y);
    float z = box.b.z <= pos.z && pos.z <= box.t.z ? pos.z : (box.b.z <= pos.z ? box.t.z : box.b.z);

    return (z - pos.z) * (z - pos.z) + (y - pos.y) * (y - pos.y) + (x - pos.x) * (x - pos.x);
}

// OFFSET: 0x00753B70
CPortalView::CPortalView(const CAaRect& rect, float maxViewDepth) {
    this->m_pos = C3Vector();
    this->m_rect = rect;
    this->m_maxViewDepth = maxViewDepth;
    this->m_vertStart = 0;
    this->m_vertCount = 0;
}

// OFFSET: 0x00753BD0
void CPortalView::Clear() {
    this->m_rect.minX = 0.0f;
    this->m_rect.minY = 0.0f;
    this->m_rect.maxX = 1.0f;
    this->m_rect.maxY = 1.0f;
    this->m_pos = C3Vector();
    this->m_maxViewDepth = 0.0f;
    this->m_vertStart = 0;
    this->m_vertCount = 0;
}

// OFFSET: 0x00753C40
void CPortalView::Merge(const CPortalView& other) {
    float maxY = this->m_rect.maxY <= other.m_rect.maxY ? other.m_rect.maxY : this->m_rect.maxY;
    float maxX = this->m_rect.maxX <= other.m_rect.maxX ? other.m_rect.maxX : this->m_rect.maxX;
    float minY = other.m_rect.minY > this->m_rect.minY ? this->m_rect.minY : other.m_rect.minY;
    float minX = other.m_rect.minX <= this->m_rect.minX ? other.m_rect.minX : this->m_rect.minX;

    this->m_rect.minX = minX;
    this->m_rect.minY = minY;
    this->m_rect.maxX = maxX;
    this->m_rect.maxY = maxY;

    if (this->m_maxViewDepth <= other.m_maxViewDepth) {
        this->m_maxViewDepth = other.m_maxViewDepth;
    }

    this->m_pos = other.m_pos;
    this->m_vertStart = 0;
    this->m_vertCount = 0;
}

// OFFSET: 0x00753D00
void CWorldView::GetCameraClosestPoint(const CAaBox& box, C3Vector& point) {
    point.x = this->m_camPos.x <= this->m_camTarget.x ? box.t.x : box.b.x;
    point.y = this->m_camPos.y <= this->m_camTarget.y ? box.t.y : box.b.y;
    point.z = this->m_camPos.z <= this->m_camTarget.z ? box.t.z : box.b.z;
}

// OFFSET: 0x00753D70
int32_t CWorldView::OcclusionQuerySphere(const CAaSphere& sphere, uint32_t flags) {
    if (!this->m_occlusion) {
        return 0;
    }

    int32_t occluded = 0;

    if (flags & 0x8000) {
        occluded = this->m_occlusion->QueryBuffer(sphere, flags);
    }

    if (flags & 0x4000) {
        return this->m_occlusion->QueryVolumes(sphere) | occluded;
    }

    return occluded;
}

// OFFSET: 0x00753DC0
int32_t CWorldView::OcclusionQueryBox(const CAaBox& box, uint32_t flags) {
    if (!this->m_occlusion) {
        return 0;
    }

    int32_t occluded = 0;

    if (flags & 0x8000) {
        occluded = this->m_occlusion->QueryBuffer(box, flags);
    }

    if (flags & 0x4000) {
        return this->m_occlusion->QueryVolumes(box) | occluded;
    }

    return occluded;
}

// OFFSET: 0x00753EC0
void CWorldView::EnableSortTable(int32_t enable) {
    if (enable) {
        if (!this->m_sortTable) {
            void* mem = SMemAlloc(sizeof(CSortTable), __FILE__, __LINE__, 0);
            this->m_sortTable = mem ? new (mem) CSortTable() : NULL;
        }
    } else if (this->m_sortTable) {
        this->m_sortTable->~CSortTable();
        SMemFree(this->m_sortTable, "delete", -1, 0);
        this->m_sortTable = NULL;
    }
}

// OFFSET: 0x00753F30
void CWorldView::EnableShadow(int32_t enable) {
    if (enable) {
        if (!this->m_shadowQuery) {
            void* mem = SMemAlloc(sizeof(CShadowQuery), __FILE__, __LINE__, 0);
            this->m_shadowQuery = mem ? new (mem) CShadowQuery() : NULL;
            this->m_shadowQuery->m_worldView = this;
        }
    } else if (this->m_shadowQuery) {
        this->m_shadowQuery->~CShadowQuery();
        SMemFree(this->m_shadowQuery, "delete", -1, 0);
        this->m_shadowQuery = NULL;
    }
}

// OFFSET: 0x00753FB0
void CWorldView::UpdateViewMat() {
    C44Matrix view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    view.Translate(C3Vector(-this->m_camPos.x, -this->m_camPos.y, -this->m_camPos.z));
}

// OFFSET: 0x00754030
void CWorldView::UpdateViewerUnderwater() {
    this->m_underwater = 0;

    if (this->m_exteriorUnderwater) {
        return;
    }

    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    uint32_t liquidType;
    float height;

    if (!worldMap->QueryLiquid(this->m_frustumPortalView.m_pos, 0, liquidType, height) || height + 0.0099999998f <= this->m_frustumPortalView.m_pos.z) {
        return;
    }

    if (this->m_viewerMovedMapObjDef || (this->m_viewerMapObjDef && (this->m_viewerMapObjDef->m_flags & 0x200))) {
        Liquid::SetCameraSubmerged(1);
        ViewSettings::SetSubmerged(1);
        this->m_exteriorUnderwater = 1;

        CMapObjDef* mapObjDef = this->m_viewerMovedMapObjDef ? this->m_viewerMovedMapObjDef : this->m_viewerMapObjDef;

        if (mapObjDef) {
            mapObjDef->m_flags |= 0x20000;
        }
    }

    this->m_underwater = 1;
}

// OFFSET: 0x007540F0
void CWorldView::UpdateShadows() {
    if (!this->m_shadowQuery) {
        return;
    }

    if (CWorldScene::camTargEntity) {
        CMapEntity* entity = reinterpret_cast<CMapEntity*>(CWorldScene::camTargEntity);
        C3Vector pos(entity->m_position.x, entity->m_position.y, entity->m_position.z + 2.0f);
        this->m_shadowQuery->SetEntityCutoff(pos, this->m_camPos);
    } else {
        this->m_shadowQuery->SetEntityCutoff(this->m_camPos, this->m_camPos);
    }

    CShadowQuery::s_activeShadowQuery = this->m_shadowQuery;
    this->m_shadowQuery->Update();
    CShadowQuery::s_activeShadowQuery = NULL;
}

// OFFSET: 0x00754170
void CWorldView::RenderM2SkyboxPass(CM2Scene* scene) {
    const CBoundingBox& viewport = g_theGxDevicePtr->m_viewport;
    float minX = viewport.m_x.m_l;
    float maxX = viewport.m_x.m_h;
    float minY = viewport.m_y.m_l;
    float maxY = viewport.m_y.m_h;
    float minZ = viewport.m_z.m_l;
    float maxZ = viewport.m_z.m_h;

    GxXformSetViewport(minX, maxX, minY, maxY, CWorldView::s_skyboxMinZ, 0.99912107f);

    float nearClip = World::GetNearClip();
    float farClip = World::GetHorizonFarClip();

    C44Matrix savedProjection;
    C44Matrix projection;
    GxuXformCreateProjection_Exact(this->m_fov * 0.60000002f, this->m_aspect, nearClip, farClip, projection);

    savedProjection = g_theGxDevicePtr->m_projection;
    g_theGxDevicePtr->XformSetProjection(projection);
    scene->Draw(M2PASS_4);
    g_theGxDevicePtr->XformSetProjection(savedProjection);

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x00754380
void CWorldView::RenderSceneClear() {
    if (!g_theGxDevicePtr->MasterEnable(GxMasterEnable_PolygonFill)) {
        CImVector black;
        *reinterpret_cast<uint32_t*>(&black) = 0xFF000000;
        GxSceneClear(0x1F, black);
        return;
    }

    if (this->m_exteriorPortalView.m_maxViewDepth < 0.0f) {
        GxSceneClear(0x1F, DayNight::GetIntFogInfo()->m_color);
        return;
    }

    if ((this->m_flags & 0x8000) && (!Liquid::CameraSubmerged() || Liquid::GetWaterDetail()) && DayNight::ShouldClearToBlack()) {
        CImVector clear;
        *reinterpret_cast<uint32_t*>(&clear) = 0;
        GxSceneClear(0x1F, clear);
        return;
    }

    GxSceneClear(0x1F, DayNight::GetFogInfo()->m_color);
}

// OFFSET: 0x00754410
void CWorldView::FrustumPush() {
    this->m_frustumIndex++;
    this->m_frustumStack[this->m_frustumIndex] = this->m_frustumStack[this->m_frustumIndex - 1];
}

// OFFSET: 0x00754440
void CWorldView::FrustumCalcCorners(C3Vector* dest, const C3Vector* corners, const CAaRect& rect) {
    for (uint32_t i = 0; i < 2; i++) {
        const C3Vector* src = &corners[4 * i];
        C3Vector* dst = &dest[4 * i];

        C3Vector a0 = src[1] + (src[2] - src[1]) * rect.minY;
        C3Vector a1 = src[1] + (src[2] - src[1]) * rect.maxY;
        C3Vector b0 = src[0] + (src[3] - src[0]) * rect.minY;
        C3Vector b1 = src[0] + (src[3] - src[0]) * rect.maxY;

        dst[0] = b0 + (a0 - b0) * rect.minX;
        dst[1] = b0 + (a0 - b0) * rect.maxX;
        dst[2] = b1 + (a1 - b1) * rect.maxX;
        dst[3] = b1 + (a1 - b1) * rect.minX;
    }
}

// OFFSET: 0x00754700
void CWorldView::FrustumSet(const CAaRect& rect) {
    C3Vector corners[8];
    CWorldView::FrustumCalcCorners(corners, this->m_frustumCorners, rect);
    this->m_frustumStack[this->m_frustumIndex].CalcPlanesFromCorners(corners);
}

// OFFSET: 0x007547C0
void CWorldView::FrustumSet(CFrustum* frustum, const C3Vector* srcCorners, const CAaRect& rect) {
    C3Vector corners[8];
    CWorldView::FrustumCalcCorners(corners, srcCorners, rect);
    frustum->CalcPlanesFromCorners(corners);
}

// OFFSET: 0x00754880
int32_t CWorldView::CullSphere(const C3Vector& center, float radius) {
    if ((this->m_flags & 0x200000) && -radius > center.z * this->m_cullPlane.n.z + center.y * this->m_cullPlane.n.y + center.x * this->m_cullPlane.n.x + this->m_cullPlane.d) {
        return 1;
    }

    CAaSphere sphere;
    sphere.c = center;
    sphere.r = radius;
    return this->m_frustumStack[this->m_frustumIndex].Cull(sphere) == 0;
}

// OFFSET: 0x00754930
int32_t CWorldView::CullModel(const CAaBox& box) {
    if ((this->m_flags & 0x200000) && this->IsBehindCullPlane(box)) {
        return 1;
    }

    return this->m_frustumStack[this->m_frustumIndex].Cull(box) == 0;
}

// OFFSET: 0x007549F0
int32_t CWorldView::CullModel(const CAaBox& box, uint32_t mask) {
    if ((this->m_flags & 0x200000) && this->IsBehindCullPlane(box)) {
        return 1;
    }

    return this->m_frustumStack[this->m_frustumIndex].Cull(box, mask) == 0;
}

// OFFSET: 0x00754AB0
void CWorldView::CullChunk(CSortEntryChunk& entry) {
    uint32_t outside = 0;

    if (!this->m_frustumStack[this->m_frustumIndex].Cull(entry.m_box, &outside)) {
        entry.m_flags |= 0x3;
        return;
    }

    if (this->m_occlusion && (this->m_occlusion->QueryBuffer(entry.m_box, 0xC000) | this->m_occlusion->QueryVolumes(entry.m_box))) {
        entry.m_flags |= 0x18;
        return;
    }

    entry.m_box.b.z = entry.m_minZ;
    entry.m_box.t.z = entry.m_maxZ;

    if (outside == 63) {
        entry.m_flags |= 0x4;
    } else if (this->CullModel(entry.m_box)) {
        entry.m_flags |= 0x2;
    }

    if (this->m_occlusion && (this->m_occlusion->QueryBuffer(entry.m_box, 0xC000) | this->m_occlusion->QueryVolumes(entry.m_box))) {
        entry.m_flags |= 0x10;
    }
}

// OFFSET: 0x00754B80
void CWorldView::CullChunkReflection(CSortEntryChunk& entry) {
    entry.m_box.b.z = entry.m_minZ;
    entry.m_box.t.z = entry.m_maxZ;

    if (this->IsBehindCullPlane(entry.m_box)) {
        entry.m_flags |= 0x1B;
        return;
    }

    if (this->CullModel(entry.m_box)) {
        entry.m_flags |= 0x3;
    }

    if (this->m_occlusion && (this->m_occlusion->QueryBuffer(entry.m_box, 0xC000) | this->m_occlusion->QueryVolumes(entry.m_box))) {
        entry.m_flags |= 0x18;
    }
}

// OFFSET: 0x00754BF0
void CWorldView::CullModelTree(CM2Model* model) {
    if (model->m_fFlags & 0x2000000) {
        model->m_fFlags &= ~0x1000u;
    } else {
        int32_t culled = 1;

        if (model->IsDrawable()) {
            CAaBox box = model->GetCurrentBoundingBox();
            culled = this->CullModel(box);
        }

        model->m_fFlags ^= (model->m_fFlags ^ (culled << 12)) & 0x1000;
    }

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        this->CullModelTree(attached);
    }
}

// OFFSET: 0x00754C70
void CWorldView::CullModelTree(CM2Model* model, uint32_t boneId) {
    if (model->m_fFlags & 0x2000000) {
        model->m_fFlags &= ~0x1000u;
    } else {
        int32_t culled = 1;

        if (model->IsDrawable()) {
            CAaBox box = model->GetSplitBodyBoundingBox(boneId);
            culled = this->CullModel(box);
        }

        model->m_fFlags ^= (model->m_fFlags ^ (culled << 12)) & 0x1000;
    }

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        this->CullModelTree(attached);
    }
}

// OFFSET: 0x00754CF0
void CWorldView::ClipPortal(CMapObj* mapObj, const SMOPortalInfo* portal, uint16_t* ext) {
    float dist = this->m_localCamPos.z * portal->m_plane.n.z + this->m_localCamPos.y * portal->m_plane.n.y + this->m_localCamPos.x * portal->m_plane.n.x + portal->m_plane.d;

    if (dist > -0.0099999998f && dist < 0.0099999998f) {
        C3Vector* verts = &mapObj->m_portalVertices[portal->m_startVertex];

        if (NTempest::Intersect(this->m_localCamPos, verts, portal->m_count, portal->m_plane.n.MajorAxis())) {
            *ext |= 0x2;
        }
    }
}

// OFFSET: 0x00754D80
int32_t CWorldView::ClipPolygon(const C3Vector* verts, uint32_t count, C3Vector*& outVerts, uint32_t& outCount) {
    C3Vector* buffers[2] = { s_clipVerts[0], s_clipVerts[1] };
    uint32_t counts[2] = { count, 0 };

    for (uint32_t i = 0; i < count; i++) {
        s_clipVerts[0][i] = verts[i];
    }

    int32_t src = 0;
    int32_t dst = 1;
    const CFrustum& frustum = this->m_lowDetailFrustum;

    for (uint32_t planeIndex = 0; planeIndex < 5; planeIndex++) {
        const C4Plane& plane = frustum.planes[planeIndex];
        uint32_t srcCount = counts[src];
        C3Vector* srcVerts = buffers[src];
        bool allInside = true;
        bool allOutside = true;
        uint32_t v = 0;

        for (; v < srcCount; v++) {
            float dist = srcVerts[v].y * plane.n.y + srcVerts[v].x * plane.n.x + srcVerts[v].z * plane.n.z + plane.d;
            s_clipDist[v] = dist;

            if (dist > 0.000099999997f) {
                s_clipSide[v] = 1;
                allOutside = false;
            } else if (dist < -0.000099999997f) {
                s_clipSide[v] = 2;
                allInside = false;
            } else {
                s_clipSide[v] = 0;
                allOutside = false;
            }
        }

        s_clipSide[v] = s_clipSide[0];
        s_clipDist[v] = s_clipDist[0];

        if (allOutside) {
            outVerts = NULL;
            outCount = 0;
            return 1;
        }

        if (allInside) {
            continue;
        }

        uint32_t newCount = 0;
        C3Vector* dstVerts = buffers[dst];

        for (uint32_t j = 0; j < srcCount; j++) {
            if (s_clipSide[j] != 2) {
                if (newCount >= 16) {
                    outVerts = buffers[dst];
                    outCount = counts[dst];
                    return 0;
                }

                dstVerts[newCount++] = srcVerts[j];
            }

            if ((s_clipSide[j] == 1 && s_clipSide[j + 1] == 2) || (s_clipSide[j] == 2 && s_clipSide[j + 1] == 1)) {
                if (newCount >= 16) {
                    outVerts = buffers[dst];
                    outCount = counts[dst];
                    return 0;
                }

                uint32_t next = (j + 1) % srcCount;
                float t = s_clipDist[j] / (s_clipDist[j] - s_clipDist[next]);
                dstVerts[newCount++] = srcVerts[j] + (srcVerts[next] - srcVerts[j]) * t;
            }

            counts[dst] = newCount;
        }

        int32_t swap = src;
        src = dst;
        dst = swap;
    }

    outCount = counts[src];
    outVerts = buffers[src];
    return 1;
}

// OFFSET: 0x00755440
void CWorldView::UpdateViewerLiquid() {
    uint32_t prevStatus = this->m_camLiquidStatus;
    this->m_camLiquidStatus = 0;
    this->m_camDepth = 0.0f;

    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    uint32_t liquidType = 0;
    float height;
    this->m_exteriorUnderwater = worldMap->m_exteriorUnderwater;

    CMapObjDef* mapObjDef = this->m_viewerMovedMapObjDef;
    uint32_t* groups = this->m_viewerMovedGroups.m_data;

    if (!mapObjDef) {
        mapObjDef = this->m_viewerMapObjDef;
        groups = this->m_viewerGroups.m_data;
    }

    if (mapObjDef) {
        C3Vector localPos = this->m_camPos * mapObjDef->m_invMat;
        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(groups[0], 0);
        float liquidType2;

        if (group && group->QueryLiquid(localPos, 0, &liquidType2, &height) && height + 0.0099999998f > localPos.z) {
            liquidType = *reinterpret_cast<uint32_t*>(&liquidType2);
            this->m_camDepth = height - localPos.z;
        }
    } else {
        float type;

        if (worldMap->QueryLiquidTerrain(this->m_camPos, *reinterpret_cast<uint32_t*>(&type), height) && height + 0.0099999998f > this->m_camPos.z) {
            liquidType = *reinterpret_cast<uint32_t*>(&type);
            this->m_camDepth = height - this->m_camPos.z;
        }
    }

    if (liquidType && liquidType != prevStatus && (World::GetEnables() & 0x400)) {
        Particulate* particulate = worldMap->m_particulate;

        if (particulate) {
            LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(liquidType);

            if (rec) {
                particulate->m_enabled = (rec->m_flags & 0x8) != 0;

                if (particulate->m_enabled) {
                    particulate->SetLiquidType(liquidType);
                    particulate->SetScale(rec->m_particleScale * 0.027777778f);
                }
            } else {
                SysMsgPrintf(STATUS_ERROR, 2, "Particulates: Liquid type [%d] not found, defaulting to no particulates!", liquidType);
                particulate->m_enabled = 0;
            }
        }
    }

    this->m_camLiquidStatus = liquidType;

    if (this->m_camDepth > 100000.0f) {
        this->m_camDepth = 100000.0f;
    }

    if (this->m_camDepth < -100000.0f) {
        this->m_camDepth = -100000.0f;
    }

    float tick = FrameTime::GetTickTimeSec();
    float t = tick * 2.0f;
    float damp = 1.0f / (t * t * t * 0.235f + t * t * 0.47999999f + t + 1.0f);
    float delta = this->m_smoothCamDepth - this->m_camDepth;

    if (fabs(delta) > 0.00000011920929f) {
        float change = (delta * 2.0f + this->m_smoothCamDepthVel) * tick;
        this->m_smoothCamDepth = (change + delta) * damp + this->m_camDepth;
        this->m_smoothCamDepthVel = (this->m_smoothCamDepthVel - change * 2.0f) * damp;
    } else {
        this->m_smoothCamDepth = this->m_camDepth;
        this->m_smoothCamDepthVel = 0.0f;
    }
}

// OFFSET: 0x007557D0
void CWorldView::RenderOpaque() {
    if (this->m_flags & 0x10) {
        DepthCache::Validate();
    }

    if (this->m_flags & 0x2000) {
        CWorldScene::RenderMapObjOpaque(this);
    }

    if ((this->m_flags & 0x800) && CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->Draw(M2PASS_0);
    }

    if (this->m_flags & 0x1000) {
        CWorldScene::RenderChunks(this);
    }

    if (this->m_flags & 0x10000) {
        CWorldScene::RenderLowDetail(this);
    }

    if (this->m_flags & 0x8000) {
        int32_t clipEnabled = g_theGxDevicePtr->m_appRenderStates[GxRs_ClipPlaneMask].m_value.m_data.i[0];
        CShaderEffect::SetClipEnabled(0);

        if (this->m_exteriorPortalView.m_maxViewDepth >= 0.0f) {
            int32_t interior = this->m_flags & 0x8;

            if (!interior) {
                CWorldScene::RenderPortalLists(this);
            }

            DayNight::RenderSky(CWorldScene::renderSkyboxName, interior != 0);
        }

        if (clipEnabled) {
            CShaderEffect::SetClipEnabled(1);
        }
    }

    if (this->m_flags & 0x20000) {
        CWorldScene::RenderEntitys(this);
    }

    if (this->m_flags & 0x200) {
        Liquid::RendererFlush(FrameTime::GetCurTimeMs(), this->m_camPos, 0);
    }

    if (this->m_flags & 0x400) {
        CWorldScene::CallRenderCallbacks(1);
    }

    if (Liquid::HasFakeReflection() && (this->m_flags & 0x20)) {
        Liquid::KawaseBlur(0, Liquid::GetBlurredBackBuffer(), Liquid::GetBlurredBackBufferFB(), 0.25f);
    }

    if ((this->m_flags & 0x800) && CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->Draw(M2PASS_1);
    }

    if (this->m_flags & 0x40) {
        g_scene.Render();
        GxuSortDrawList(GxuCat_0, GxuSort_VtxStream, this->m_camPos);
        GxuFlushDrawList(GxuCat_0, this->m_camPos);
    }
}

// OFFSET: 0x00755960
void CWorldView::RenderAlpha() {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (this->m_flags & 0x2000) {
        CWorldScene::RenderMapObjAlpha(this);
    }

    if ((this->m_flags & 0x40000) && worldMap && (World::GetEnables() & 0x40)) {
        worldMap->RenderFootprints(this);
    }

    if (this->m_flags & 0x4000) {
        CWorldScene::RenderDetailDoodads(this);
    }

    if (this->m_flags & 0x100) {
        CWorldScene::RenderOccluders(this);
        CWorldScene::RenderClipVolumes(this);
        CWorldScene::RenderFrustum(this);
    }

    int32_t exteriorUnderwater = this->m_exteriorUnderwater;

    if (worldMap) {
        exteriorUnderwater = exteriorUnderwater && !worldMap->m_exteriorUnderwater;
    }

    CM2Scene* scene = CWorldScene::s_m2Scene;

    if (this->m_camLiquidStatus || exteriorUnderwater) {
        if (this->m_flags & 0x100000) {
            CWorldScene::RenderBarriers(this);
        }

        if ((this->m_flags & 0x800) && scene) {
            this->RenderM2SkyboxPass(scene);
            scene->Draw(M2PASS_2);
        }

        if (this->m_flags & 0x10) {
            if (DepthCache::IsValid()) {
                DepthCache::Resolve();
            }

            C3Vector sunDir;
            SunShafts::Update(this->m_camPos, *DayNight::GetSunGlarePos(&sunDir));
        }

        if (this->m_flags & 0x20) {
            Liquid::RenderUnderwater();

            if (Liquid::GetWaterDetail() >= 3) {
                Liquid::KawaseBlur(Liquid::GetRealReflectionBuffer(), Liquid::GetBlurredReflectionBuffer(), Liquid::GetBlurredReflectionBufferFB(), 0.25f);
            }
        }

        if (this->m_flags & 0x200) {
            CWorldScene::RenderAlphaWater(this);

            if (this->m_flags & 0x200) {
                CWorldScene::RenderAlphaWaterFog(this);
            }
        }

        if ((this->m_flags & 0x800) && scene) {
            scene->Draw(M2PASS_3);
        }

        if (this->m_flags & 0x80) {
            FFX::FlushPortalEffects();
        }

        if ((this->m_flags & 0x800) && scene) {
            scene->Draw(M2PASS_5);
        }

        if (this->m_flags & 0x80) {
            FFX::DoPass(0);
        }
    } else {
        if (this->m_flags & 0x10) {
            if (DepthCache::IsValid()) {
                DepthCache::Resolve();
            }

            C3Vector sunDir;
            SunShafts::Update(this->m_camPos, *DayNight::GetSunGlarePos(&sunDir));
        }

        if (this->m_flags & 0x20) {
            Liquid::RenderUnderwater();
        }

        if ((this->m_flags & 0x200) && Liquid::GetWaterDetail() > 1) {
            CWorldScene::RenderAlphaWater(this);
        }

        if ((this->m_flags & 0x800) && scene) {
            scene->Draw(M2PASS_3);
        }

        if (this->m_flags & 0x20) {
            Liquid::RenderUnderwater();

            if (Liquid::GetWaterDetail() >= 3) {
                Liquid::KawaseBlur(Liquid::GetRealReflectionBuffer(), Liquid::GetBlurredReflectionBuffer(), Liquid::GetBlurredReflectionBufferFB(), 0.25f);
            }
        }

        if (this->m_flags & 0x80) {
            FFX::FlushPortalEffects();
        }

        if (this->m_flags & 0x200) {
            CWorldScene::RenderAlphaWaterFog(this);
        }

        if (this->m_flags & 0x100000) {
            CWorldScene::RenderBarriers(this);
        }

        if ((this->m_flags & 0x800) && scene) {
            this->RenderM2SkyboxPass(scene);
            scene->Draw(M2PASS_2);
            scene->Draw(M2PASS_5);
        }

        if (this->m_flags & 0x80000) {
            CWorldScene::RenderWeather();
        }
    }

    if (DepthCache::DepthRangeDumpEnabled() && DepthCache::IsValid()) {
        DepthCache::Resolve();
    }

    if (this->m_flags & 0x400) {
        CWorldScene::CallRenderCallbacks(2);
    }

    if (this->m_flags & 0x40) {
        GxuSortDrawList(GxuCat_1, GxuSort_B2F, this->m_camPos);
        GxuFlushDrawList(GxuCat_1, this->m_camPos);
    }
}

// OFFSET: 0x00755C80
void CWorldView::CalcLowDetailFrustum(CFrustum& frustum, const CAaRect& rect) {
    C44Matrix projection;
    float nearClip = this->m_farClip - 50.0f;
    GxuXformCreateProjection_Exact(this->m_fov * 0.60000002f, this->m_aspect, nearClip, World::GetHorizonFarClip(), projection);

    C44Matrix view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    C3Vector corners[8];
    CWorldScene::GetFrustumCornersInWorldSpace(view, projection, this->m_camPos, corners);
    CWorldView::FrustumSet(&frustum, corners, rect);
}

// OFFSET: 0x00755F90
void CWorldView::AddDoodadDefModelToModelScene(CMapDoodadDef* doodadDef) {
    float alpha = 1.0f;

    if ((World::GetEnables() & 0x2000) && !(doodadDef->m_flags & 0x400)) {
        C3Vector delta = doodadDef->m_sphere.c - this->m_camPos;
        float distSqr = delta.z * delta.z + delta.y * delta.y + delta.x * delta.x;
        uint32_t category = doodadDef->m_sizeCategory;

        if (distSqr > s_fadeOuterSqr[category]) {
            return;
        }

        if (distSqr > s_fadeInnerSqr[category]) {
            alpha = 1.0f - (sqrt(distSqr) - s_fadeInner[category]) / s_baseFadeDist.m_fadeRange[category];

            if (alpha > 0.99000001f) {
                alpha = 1.0f;
            } else if (alpha < 0.0099999998f) {
                alpha = 0.0f;
            }
        }

        if (doodadDef->m_fade < 0xFF00) {
            alpha = static_cast<float>(doodadDef->m_fade) * alpha * 0.000015258789f;
        }

        if (alpha <= 0.0099999998f) {
            return;
        }
    }

    doodadDef->m_model->SetAnimating(1);

    CM2Model* model = doodadDef->m_model;

    if (model->m_attachParent) {
        model->m_fFlags |= 0x40;
        model->m_fFlags |= 0x10000;
    } else {
        model->m_fFlags |= 0x8;
        model->m_fFlags |= 0x8000;
    }

    doodadDef->m_model->m_fadeAlpha = alpha;
}

// OFFSET: 0x00756130
int32_t CWorldView::TransformAndClipVerts(int32_t ignoreOcclusion, const C3Vector* verts, uint32_t count, const C3Vector& offset, C3Vector*& outVerts, uint32_t& outCount) {
    if (count > 12) {
        count = 12;
    }

    for (uint32_t i = 0; i < count; i++) {
        C3Vector v = verts[i] + offset;
        s_transformVerts[i] = v * this->m_localToWorld;
    }

    if (this->m_occlusion && this->m_occlusion->m_volumes.Count() && !ignoreOcclusion && this->m_occlusion->QueryVolumes(s_transformVerts, count)) {
        outVerts = NULL;
        outCount = 0;
        return 1;
    }

    this->ClipPolygon(s_transformVerts, count, outVerts, outCount);

    if (outCount < 3) {
        outVerts = NULL;
        outCount = 0;
        return 1;
    }

    for (uint32_t i = 0; i < outCount; i++) {
        C3Vector v = outVerts[i] - this->m_camPos;
        const C44Matrix& m = this->m_viewProj;
        float x = m.a0 * v.x + m.b0 * v.y + m.c0 * v.z + m.d0;
        float y = m.a1 * v.x + m.b1 * v.y + m.c1 * v.z + m.d1;
        float z = m.c2 * v.z + m.a2 * v.x + v.y * m.b2 + m.d2;
        float w = m.b3 * v.y + m.c3 * v.z + v.x * m.a3 + m.d3;

        if (w < 0.000099999997f) {
            w = 0.000099999997f;
        }

        outVerts[i].x = 1.0f / w * x;
        outVerts[i].y = 1.0f / w * y;
        outVerts[i].z = z;
    }

    return 0;
}

// OFFSET: 0x00757130
void CWorldView::ClearLists() {
    for (uint32_t i = 0; i < 6; i++) {
        for (uint32_t j = 0; j < 6; j++) {
            this->m_renderChunks[i][j].m_count = 0;
        }
    }

    this->m_lowDetailAreas.m_count = 0;
    this->m_lowDetailMapObjDefs.m_count = 0;
}

// OFFSET: 0x00757180
void CWorldView::UpdateNonVisEntitys() {
    CMapEntity* entity = this->m_nonVisEntities.Head();

    while (entity) {
        CMapEntity* next = this->m_nonVisEntities.Next(entity);
        int32_t flags = 0;
        int32_t outside = 0;

        entity->m_viewLink.Unlink();

        if (entity->m_model) {
            int32_t visible = 0;

            if (!(entity->m_entityFlags & 0x80)) {
                if (entity->m_entityFlags & 0x100) {
                    visible = 1;
                }

                C3Vector delta = entity->m_sphere.c - this->m_camPos;

                if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x < 1111.1111f) {
                    visible = 1;
                    flags = 2;
                }

                CM2Model* model = entity->m_model;

                if (model->m_attachParent) {
                    model->m_fFlags &= ~0x40u;
                    model->m_fFlags &= ~0x10000u;
                } else {
                    model->m_fFlags &= ~0x8u;
                    model->m_fFlags &= ~0x8000u;
                }
            }

            entity->m_model->SetAnimating(visible);
        }

        if (entity->m_updateCallback) {
            if ((entity->m_entityFlags & 0x60000) != 0x60000) {
                outside = 4;
            }

            entity->m_updateCallback(entity->m_updateParam, flags | outside, entity->m_param64, entity->m_param32);
        }

        entity = next;
    }
}

// OFFSET: 0x00757300
void CWorldView::AddMapChunkToRenderList(CMapChunk* chunk) {
    CMapRenderChunk* renderChunk = chunk->m_renderChunk;

    if (!renderChunk || renderChunk->m_stamp == this->m_viewStamp) {
        return;
    }

    renderChunk->m_stamp = this->m_viewStamp;
    renderChunk->RenderPrep();

    for (uint32_t i = 0; i < renderChunk->m_batchCount; i++) {
        CMapRenderChunkBatch& batch = renderChunk->m_batches[i];

        if (!batch.m_batch.m_count) {
            continue;
        }

        uint32_t layerCount = batch.m_layerCount;
        uint32_t shaderType = batch.m_shaderType;

        if (!(renderChunk->m_flags & 0x2)) {
            layerCount = 0;
            shaderType = 0;
        }

        CVisRenderChunk entry;
        entry.m_renderChunk = renderChunk;
        entry.m_batch = i;
        this->m_renderChunks[layerCount][shaderType].Add(1, &entry);
    }
}

// OFFSET: 0x007574A0
void CWorldView::CullLowDetail(CSortEntry* entry, const CFrustum& frustum) {
    for (uint32_t i = 0; i < entry->m_lowDetailOceans.Count(); i++) {
        CMapAreaLow* area = entry->m_lowDetailOceans.m_data[i];

        if (!this->CullModel(area->m_bbox) && !(this->OcclusionQueryBox(area->m_bbox, 0xC030))) {
            area->RenderPrep();
            Liquid::RendererDraw(area->m_oceanInstance);
        }
    }

    entry->m_lowDetailOceans.m_count = 0;

    if (!entry->m_lowDetailAreas.Count() && !entry->m_lowDetailMapObjDefs.Count()) {
        return;
    }

    this->FrustumPush();
    this->m_frustumStack[this->m_frustumIndex] = frustum;

    for (uint32_t i = 0; i < entry->m_lowDetailAreas.Count(); i++) {
        CMapAreaLow* area = entry->m_lowDetailAreas.m_data[i];

        if (!this->CullModel(area->m_bbox) && !this->OcclusionQueryBox(area->m_bbox, 0xC030)) {
            this->m_lowDetailAreas.Add(1, &area);
        }
    }

    entry->m_lowDetailAreas.m_count = 0;

    for (uint32_t i = 0; i < entry->m_lowDetailMapObjDefs.Count(); i++) {
        CMapObjDef* mapObjDef = entry->m_lowDetailMapObjDefs.m_data[i];

        if (!this->CullModel(mapObjDef->m_bbox) && !this->OcclusionQueryBox(mapObjDef->m_bbox, 0xC030)) {
            this->m_lowDetailMapObjDefs.Add(1, &mapObjDef);
        }
    }

    entry->m_lowDetailMapObjDefs.m_count = 0;
    this->FrustumPop();
}

// OFFSET: 0x00757660
void CWorldView::CullEntitys(CSortEntry* entry) {
    s_entityCullDone = 0;
    CMapEntity* entity = entry->m_entities.Head();

    while (entity) {
        CMapEntity* next = entry->m_entities.Next(entity);
        entity->m_viewLink.Unlink();
        entity->m_cullStamp = this->m_viewStamp;

        if (this->CullSphere(entity->m_sphere.c, entity->m_sphere.r) || (this->m_occlusion && (this->m_occlusion->QueryBuffer(entity->m_sphere, 0) | this->m_occlusion->QueryVolumes(entity->m_sphere)))) {
            entity->m_entityFlags = (entity->m_entityFlags & 0xFFF9FFFF) | 0x40000;
            this->m_nonVisEntities.LinkToTail(entity);
        } else {
            entity->m_entityFlags = (entity->m_entityFlags & 0xFFF9FFFF) | 0x20000;

            if (entity->m_model) {
                entity->m_model->SetAnimating(1);
                CM2Model* model = entity->m_model;

                if (model->m_attachParent) {
                    model->m_fFlags |= 0x40;
                    model->m_fFlags |= 0x10000;
                } else {
                    model->m_fFlags |= 0x8;
                    model->m_fFlags |= 0x8000;
                }
            }

            if (!entity->m_updateCallback || entity->m_updateCallback(entity->m_updateParam, 5, entity->m_param64, entity->m_param32)) {
                this->m_list2004.LinkToTail(entity);
            }
        }

        entity = next;
    }

    s_entityCullDone = 1;
}

// OFFSET: 0x00757850
void CWorldView::CullEntitysInterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, int32_t interior) {
    if (!(this->m_flags & 0x20000)) {
        return;
    }

    s_entityCullDone = 0;

    for (CMapBaseObjLink* link = list.Head(); link; link = list.Next(link)) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->m_owner);

        if ((entity->m_flags & 0x10000) || (entity->m_entityFlags & 0x60000) == 0x20000 || entity->m_cullStamp == this->m_viewStamp) {
            continue;
        }

        entity->m_entityFlags = (entity->m_entityFlags & 0xFFF9FFFF) | 0x40000;

        if (this->CullSphere(entity->m_sphere.c, entity->m_sphere.r)) {
            continue;
        }

        entity->m_viewLink.Unlink();
        entity->m_cullStamp = this->m_viewStamp;
        entity->m_entityFlags = (entity->m_entityFlags & 0xFFF9FFFF) | 0x20000;

        if (interior) {
            entity->m_flags |= 0x4;
        } else {
            entity->m_flags &= ~0x4u;
        }

        if (entity->m_model) {
            entity->m_model->SetAnimating(1);
            CM2Model* model = entity->m_model;

            if (model->m_attachParent) {
                model->m_fFlags |= 0x40;
                model->m_fFlags |= 0x10000;
            } else {
                model->m_fFlags |= 0x8;
                model->m_fFlags |= 0x8000;
            }
        }

        if (entity->m_updateCallback) {
            entity->m_updateCallback(entity->m_updateParam, 5, entity->m_param64, entity->m_param32);
        }

        this->m_list2004.LinkToTail(entity);
    }

    s_entityCullDone = 1;
}

// OFFSET: 0x007579C0
void CWorldView::CullDoodads(CSortTable* table, int32_t index, uint16_t minSize) {
    CSortEntry& entry = table->m_entries.m_data[index];

    if (!(World::GetEnables() & 0x1)) {
        entry.m_doodadDefHead = -1;
        return;
    }

    if (entry.m_doodadDefHead != -1) {
        CSortEntryDoodadDef* node = &table->m_doodadDefs.m_data[entry.m_doodadDefHead];

        while (node) {
            CMapDoodadDef* doodadDef = node->m_doodadDef;

            if (doodadDef->m_sizeCategory >= minSize && !(doodadDef->m_flags & 0x10010)) {
                if (this->CullSphere(doodadDef->m_sphere.c, doodadDef->m_sphere.r) || this->OcclusionQuerySphere(doodadDef->m_sphere, 0xC000)) {
                    int32_t visible = 1;

                    if (!(doodadDef->m_entityFlags & 0x100)) {
                        C3Vector delta = doodadDef->m_sphere.c - this->m_camPos;

                        if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x >= 100.0f) {
                            visible = 0;
                        }
                    }

                    doodadDef->m_model->SetAnimating(visible);
                } else {
                    this->AddDoodadDefModelToModelScene(doodadDef);
                }
            }

            if (node->m_next == -1) {
                break;
            }

            node = &table->m_doodadDefs.m_data[node->m_next];
        }
    }

    entry.m_doodadDefHead = -1;
}

// OFFSET: 0x00757B10
void CWorldView::CullDoodadsExterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, uint16_t minSize, int32_t sorted, int32_t cull) {
    if (!(World::GetEnables() & 0x1) || !(this->m_flags & 0x800)) {
        return;
    }

    for (CMapBaseObjLink* link = list.Head(); link; link = list.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->m_sizeCategory < minSize) {
            if (sorted) {
                return;
            }

            continue;
        }

        if (doodadDef->m_cullStamp == this->m_viewStamp) {
            continue;
        }

        doodadDef->m_cullStamp = this->m_viewStamp;

        if ((doodadDef->m_flags & 0x10010) || !(doodadDef->m_flags & 0x40)) {
            continue;
        }

        if ((cull && this->CullSphere(doodadDef->m_sphere.c, doodadDef->m_sphere.r)) || this->OcclusionQuerySphere(doodadDef->m_sphere, 0xC000)) {
            C3Vector delta = doodadDef->m_sphere.c - this->m_camPos;

            if ((doodadDef->m_entityFlags & 0x100) || delta.z * delta.z + delta.y * delta.y + delta.x * delta.x < 100.0f) {
                doodadDef->m_model->SetAnimating(1);
            } else {
                doodadDef->m_model->SetAnimating(0);
            }
        } else {
            this->AddDoodadDefModelToModelScene(doodadDef);
        }
    }
}

// OFFSET: 0x00757C70
void CWorldView::CullDoodadsInterior(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, uint16_t minSize, int32_t interior) {
    if (!(World::GetEnables() & 0x1) || !(this->m_flags & 0x800)) {
        return;
    }

    for (CMapBaseObjLink* link = list.Head(); link; link = list.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->m_sizeCategory < minSize || doodadDef->m_cullStamp == this->m_viewStamp) {
            continue;
        }

        if ((doodadDef->m_flags & 0x10) || !(doodadDef->m_flags & 0x40)) {
            continue;
        }

        if (this->CullSphere(doodadDef->m_sphere.c, doodadDef->m_sphere.r)) {
            C3Vector delta = doodadDef->m_sphere.c - this->m_camPos;

            if ((doodadDef->m_entityFlags & 0x100) || delta.z * delta.z + delta.y * delta.y + delta.x * delta.x < 100.0f) {
                doodadDef->m_model->SetAnimating(1);
            } else {
                doodadDef->m_model->SetAnimating(0);
            }
        } else {
            doodadDef->m_cullStamp = this->m_viewStamp;

            if (interior) {
                doodadDef->m_flags |= 0x4;
            } else {
                doodadDef->m_flags &= ~0x4u;
            }

            this->AddDoodadDefModelToModelScene(doodadDef);
        }
    }
}

// OFFSET: 0x00757DE0
void CWorldView::CullMapObjDefGroupContents(CMapObjDefGroup* group) {
    float distSqr = CWorldView::DistSqrToBox(group->m_bbox, this->m_camPos);
    uint16_t minSize;

    if (distSqr < 0.0f || s_fadeOuterSqr[0] > distSqr) {
        minSize = 0;
    } else if (s_fadeOuterSqr[1] > distSqr) {
        minSize = 1;
    } else if (s_fadeOuterSqr[2] > distSqr) {
        minSize = 2;
    } else if (s_fadeOuterSqr[3] > distSqr) {
        minSize = 3;
    } else {
        minSize = 4;
    }

    int32_t interior = group->m_flags & 0x4;
    this->CullDoodadsInterior(group->m_doodadLinks, minSize, interior);
    this->CullEntitysInterior(group->m_entityLinks, interior);
}

// OFFSET: 0x00757E90
void CWorldView::GatherMapObjDefGroupLiquids(CMapObjDef* mapObjDef, CMapObjDefGroup* group, CMapObj* mapObj, CMapObjGroup* mapObjGroup) {
    if (!group->m_liquidInstance) {
        uint32_t liquidType = mapObjGroup->GetLiquidType();
        uint32_t liquidId = liquidType;
        LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(liquidType);
        int32_t noSurface = !(group->m_flags & 0x2);

        if (mapObjGroup->m_groupFlags & 0x48) {
            noSurface = 1;
        }

        if (rec->m_flags & 0x200) {
            noSurface = 1;
        } else if (!noSurface && liquidType < 21 && liquidType && !((liquidType - 1) & 3)) {
            liquidId = 17;
        }

        CClientLiquidEnvironment* environment = CClientLiquidEnvironment::Create(0, !noSurface);
        CMeshGeomFactory* factory = CMeshGeomFactory::Create(mapObj, mapObjGroup);
        float intensity = 0.0f;
        uint32_t lightId = 0xFFFFFFFF;

        if (!noSurface) {
            SMOMaterial* material = mapObj->GetGroupMaterial(group->m_groupNum);

            if (material) {
                lightId = material->m_color2;
            }

            intensity = 1.0f;
        }

        factory->SetDiffuseColor(CImVector(lightId));
        factory->SetTexCoord1ValueX(intensity);

        Liquid::CInstance* instance = Liquid::CreateInstance(NULL, liquidId, factory);
        group->m_liquidInstance = instance;
        instance->m_environment = environment;
        instance->SetTransform(mapObjDef->m_mat);

        const CAaBox& box = mapObjDef->m_bbox;
        instance->m_bounds.c = C3Vector((box.t.x + box.b.x) * 0.5f, (box.t.y + box.b.y) * 0.5f, (box.t.z + box.b.z) * 0.5f);
        C3Vector extent(box.t.x - box.b.x, box.t.y - box.b.y, box.t.z - box.b.z);
        instance->m_bounds.r = sqrt(extent.x * extent.x + extent.y * extent.y + extent.z * extent.z) * 0.5f;
        instance->m_isMesh = factory->GetWorldSpace() == 0;
    }

    if (group->m_liquidInstance->m_environment) {
        group->m_liquidInstance->m_environment->SetInteriorFog((this->m_flags >> 2) & 1);
    }

    Liquid::RendererDraw(group->m_liquidInstance);

    Liquid::Object* object = group->m_liquidInstance->m_object;

    if (object) {
        if (object->m_reflectionType != 2) {
            this->m_hasLiquids = 1;
        }

        if (!(mapObjGroup->m_groupFlags & 0x8) && object->m_reflectionType == 1) {
            this->m_liquidGroups.Add(1, &group);
        }
    }
}

// OFFSET: 0x007580D0
void CWorldView::SetupMapObjDefCull(CMapObjDef* mapObjDef) {
    this->m_cullMapObjDef = mapObjDef;

    if (mapObjDef != this->m_cullMapObjDefPrev) {
        this->m_cullStamp++;
        this->m_cullMapObjDefPrev = mapObjDef;
        this->m_portalExts.SetCount(mapObjDef->m_mapObj->m_portalCount);
    }

    C44Matrix view;
    view.Translate(C3Vector(-this->m_camPos.x, -this->m_camPos.y, -this->m_camPos.z));
    view = mapObjDef->m_mat * view;
    GxXformSet(GxXform_World, view);

    this->m_localCamPos = this->m_camPos * mapObjDef->m_invMat;
    this->m_localCamTarget = this->m_camTarget * mapObjDef->m_invMat;

    C3Vector dir = this->m_localCamTarget - this->m_localCamPos;

    if (dir.z * dir.z + dir.y * dir.y + dir.x * dir.x > 0.000099999997f) {
        dir *= 1.0f / sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
    }

    this->m_localCamPlane.n = dir;
    this->m_localCamPlane.d = -(this->m_localCamPos.z * dir.z + this->m_localCamPos.y * dir.y + this->m_localCamPos.x * dir.x);

    GxXformViewProj(this->m_viewProj);

    C44Matrix deviceView;
    deviceView = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    this->m_localView = view * deviceView;
    this->m_localViewProj = view * this->m_viewProj;
    this->m_localToWorld = mapObjDef->m_mat;

    this->m_cullValid = 1;
}

// OFFSET: 0x007585A0
void CWorldView::ClipPortalExt(CMapObj* mapObj, const SMOPortalInfo* portal, SPortalExt* ext) {
    this->ClipPortal(mapObj, portal, &ext->m_flags);

    C3Vector* verts = NULL;
    uint32_t count = 0;

    if (!(ext->m_flags & 0x2)) {
        C3Vector offset(0.0f, 0.0f, 0.0f);
        ext->m_flags |= this->TransformAndClipVerts((ext->m_flags >> 4) & 1, &mapObj->m_portalVertices[portal->m_startVertex], portal->m_count, offset, verts, count);
    }

    if (ext->m_flags & 0x2) {
        ext->m_rect.minY = -1.0f;
        ext->m_rect.maxY = 1.0f;
        ext->m_rect.minX = -1.0f;
        ext->m_rect.maxX = 1.0f;
    } else if (ext->m_flags & 0x1) {
        ext->m_rect.minY = 3.402823466e38f;
        ext->m_rect.maxY = -3.402823466e38f;
        ext->m_rect.minX = 3.402823466e38f;
        ext->m_rect.maxX = -3.402823466e38f;
    } else {
        CWorldView::RectFromVerts(ext->m_rect, verts, count);
    }
}

// OFFSET: 0x007588E0
void CWorldView::Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect) {
    this->m_flags &= ~0x4u;
    this->m_exteriorPortalView = CPortalView();
    this->m_frustumPortalView = CPortalView();
    this->m_exteriorPortalViews.SetCount(0);
    this->m_portalViews.SetCount(0);
    this->m_hasLiquids = 0;
    this->m_liquidGroups.m_count = 0;

    this->m_camPos = camPos;
    this->m_camTarget = camTarget;
    this->m_camDir = camTarget - camPos;
    this->m_camDir *= 1.0f / sqrt(this->m_camDir.x * this->m_camDir.x + this->m_camDir.y * this->m_camDir.y + this->m_camDir.z * this->m_camDir.z);
    this->m_nearClip = nearClip;
    this->m_farClip = farClip;
    this->m_fov = fov;
    this->m_aspect = aspect;
    this->m_camPlane.n = this->m_camDir;
    this->m_camPlane.d = -(this->m_camDir.z * camPos.z + this->m_camDir.y * camPos.y + camPos.x * this->m_camDir.x);

    C3Vector horiz(this->m_camDir.x, this->m_camDir.y, 0.0f);
    float horizLenSqr = horiz.y * horiz.y + horiz.x * horiz.x;

    if (horizLenSqr > 0.000099999997f) {
        float scale = 1.0f / sqrt(horizLenSqr);
        horiz.x *= scale;
        horiz.y *= scale;
        horiz.z = scale * 0.0f;
    }

    this->m_horizPlane.n = horiz;
    this->m_horizPlane.d = -(camPos.z * horiz.z + horiz.y * camPos.y + camPos.x * horiz.x);

    if (this->m_sortTable) {
        this->m_sortTable->Setup(this->m_farClip, horiz, camPos);
    }

    this->m_viewMat = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    this->m_projMat = g_theGxDevicePtr->m_projection;
    GxXformViewport(this->m_viewportMinX, this->m_viewportMaxX, this->m_viewportMinY, this->m_viewportMaxY, this->m_viewportMinZ, this->m_viewportMaxZ);
    CWorldScene::GetFrustumCornersInWorldSpace(this->m_viewMat, this->m_projMat, this->m_camPos, this->m_frustumCorners);

    this->m_frustumBounds = CAaBox::Bounding(this->m_frustumCorners, 8);

    if (this->m_frustumBounds.b.x > this->m_camPos.x) {
        this->m_frustumBounds.b.x = this->m_camPos.x;
    }

    if (this->m_frustumBounds.b.y > this->m_camPos.y) {
        this->m_frustumBounds.b.y = this->m_camPos.y;
    }

    if (this->m_frustumBounds.b.z > this->m_camPos.z) {
        this->m_frustumBounds.b.z = this->m_camPos.z;
    }

    if (this->m_camPos.x > this->m_frustumBounds.t.x) {
        this->m_frustumBounds.t.x = this->m_camPos.x;
    }

    if (this->m_camPos.y > this->m_frustumBounds.t.y) {
        this->m_frustumBounds.t.y = this->m_camPos.y;
    }

    if (this->m_camPos.z > this->m_frustumBounds.t.z) {
        this->m_frustumBounds.t.z = this->m_camPos.z;
    }

    this->m_lowDetailFrustum.CalcPlanesFromCorners(this->m_frustumCorners);

    this->m_viewMat.Translate(C3Vector(-this->m_camPos.x, -this->m_camPos.y, -this->m_camPos.z));
    this->m_viewProjMat = this->m_viewMat * this->m_projMat;

    this->m_frustumIndex = 0;
    this->m_frustumStack[0].CalcPlanesFromCorners(this->m_frustumCorners);

    this->m_quadX = 0;

    if (this->m_camTarget.x > this->m_camPos.x) {
        this->m_quadX = 2;
    }

    if (this->m_camTarget.y > this->m_camPos.y) {
        this->m_quadX++;
    }

    this->m_quadY = 0;

    if (this->m_camPos.x > this->m_camTarget.x) {
        this->m_quadY = 2;
    }

    if (this->m_camPos.y > this->m_camTarget.y) {
        this->m_quadY++;
    }

    this->m_camPlane.n = this->m_camDir;
    this->m_camPlane.d = -(this->m_camDir.z * camPos.z + this->m_camDir.y * camPos.y + camPos.x * this->m_camDir.x);

    if (this->m_occlusion) {
        uint32_t enables = World::GetEnables();
        int32_t useHoriz = (enables >> 12) & 1;
        C44Matrix occlusionView;
        float dirZ = this->m_camDir.z;

        if (dirZ < -0.89999998f || dirZ > 0.89999998f || horizLenSqr < 0.000099999997f) {
            useHoriz = 0;
        } else {
            C44Matrix lookAt;
            GxuXformCreateLookAtSgCompat(C3Vector(0.0f, 0.0f, 0.0f), horiz, C3Vector(0.0f, 0.0f, 1.0f), lookAt);
            lookAt.Translate(C3Vector(-this->m_camPos.x, -this->m_camPos.y, -this->m_camPos.z));
            occlusionView = lookAt * this->m_projMat;
        }

        this->m_occlusion->Update(occlusionView, this->m_projMat, this->m_camDir, useHoriz);
    }

    this->m_cullMapObjDef = NULL;
    this->m_cullMapObjDefPrev = NULL;
    this->m_portalVerts.SetCount(0);
    this->m_viewStamp++;
}

// OFFSET: 0x007590C0
void CWorldView::AddOccluder(const C3Vector& start, const C3Vector& end) {
    float dirZ = this->m_camDir.z;

    if (dirZ < -0.89999998f || dirZ > 0.89999998f) {
        return;
    }

    float startDist = start.z * this->m_horizPlane.n.z + start.y * this->m_horizPlane.n.y + start.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    float endDist = end.z * this->m_horizPlane.n.z + end.y * this->m_horizPlane.n.y + end.x * this->m_horizPlane.n.x + this->m_horizPlane.d;

    if (startDist < 0.1f && endDist < 0.1f) {
        return;
    }

    int32_t numEntries = this->m_sortTable->m_lastEntry;
    int32_t startEntry = static_cast<int32_t>(startDist * 0.029999999f);
    int32_t endEntry = static_cast<int32_t>(endDist * 0.029999999f);

    if (startEntry >= numEntries && endEntry >= numEntries) {
        return;
    }

    CWorldOccluder* occluder = CWorldOccluder::Alloc();
    occluder->m_start = start;
    occluder->m_end = end;

    if (startEntry == endEntry) {
        this->m_sortTable->m_entries.m_data[startEntry].m_occluders.LinkToTail(occluder);
        return;
    }

    C3Vector near;
    C3Vector far;

    if (startEntry <= endEntry) {
        near = occluder->m_start;
        far = end;
    } else {
        near = end;
        far = occluder->m_start;
        int32_t swap = startEntry;
        startEntry = endEntry;
        endEntry = swap;
    }

    if (startEntry < 0) {
        startEntry = 0;
    }

    if (endEntry >= this->m_sortTable->m_lastEntry) {
        endEntry = this->m_sortTable->m_lastEntry - 1;
    }

    for (int32_t entry = startEntry; entry < endEntry; entry++) {
        const C4Plane& plane = this->m_sortTable->m_entries.m_data[entry + 1].m_plane;
        float farDist = far.z * plane.n.z + far.y * plane.n.y + far.x * plane.n.x + plane.d;
        float nearDist = near.z * plane.n.z + near.y * plane.n.y + near.x * plane.n.x + plane.d;
        C3Vector split;

        if (farDist - nearDist <= 0.0000099999997f) {
            split = far;
        } else {
            float t = farDist / (farDist - nearDist);
            split = far + (near - far) * t;
        }

        CWorldOccluder* piece = CWorldOccluder::Alloc();
        piece->m_start = near;
        piece->m_end = split;
        this->m_sortTable->m_entries.m_data[entry].m_occluders.LinkToTail(piece);
        near = split;
    }

    occluder->m_start = near;
    occluder->m_end = far;
    this->m_sortTable->m_entries.m_data[endEntry].m_occluders.LinkToTail(occluder);
}

// OFFSET: 0x00759420
void CWorldView::AddExteriorPortal(CPortalView& portalView) {
    this->m_exteriorPortalViews.Add(1, &portalView);
}

// OFFSET: 0x00759440
void CWorldView::AddMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* group) {
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!(mapObj->GetGroupFlags(group->m_groupNum) & 0x8)) {
        if (mapObj->GetGroupFlags(group->m_groupNum) & 0x10000) {
            this->m_moveableGroups.LinkToTail(group);
        }

        return;
    }

    if (mapObjDef->m_flags & 0x200) {
        this->m_moveableGroups.LinkToTail(group);
        return;
    }

    C3Vector point;
    this->GetCameraClosestPoint(group->m_bbox, point);
    float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    int32_t entry = dist > 0.0f ? static_cast<int32_t>(dist * 0.029999999f) : 0;

    if (entry < this->m_sortTable->m_lastEntry) {
        this->m_moveableGroups.LinkToTail(group);
    }
}

// OFFSET: 0x00759540
void CWorldView::AddMoveableMapObjDefGroups() {
    CMapObjDefGroup* group = this->m_moveableGroups.Head();

    while (group) {
        CMapObjDefGroup* next = this->m_moveableGroups.Next(group);
        group->m_moveableLink.Unlink();

        C3Vector point;
        point.x = this->m_camPos.x <= this->m_camTarget.x ? group->m_bbox.b.x : group->m_bbox.t.x;
        point.y = this->m_camPos.y <= this->m_camTarget.y ? group->m_bbox.b.y : group->m_bbox.t.y;
        point.z = this->m_camPos.z <= this->m_camTarget.z ? group->m_bbox.b.z : group->m_bbox.t.z;

        float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
        int32_t entry = dist > 0.0f ? static_cast<int32_t>(dist * 0.029999999f) : 0;

        if (entry < this->m_sortTable->m_lastEntry) {
            this->m_moveableGroups.LinkToTail(group);
        }

        group = next;
    }
}

// OFFSET: 0x00759690
void CWorldView::AddMapChunk(CMapChunk* chunk, int32_t a2) {
    if (this->m_sortTable) {
        C3Vector point;
        this->GetCameraClosestPoint(chunk->m_bbox, point);
        float dist = point.y * this->m_horizPlane.n.y + point.z * this->m_horizPlane.n.z + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
        int32_t entry = dist > 0.0f ? static_cast<int32_t>(dist * 0.029999999f) : 0;

        if (entry >= this->m_sortTable->m_lastEntry) {
            return;
        }

        CSortEntryChunk* sortChunk = this->m_sortTable->m_entries.m_data[entry].m_chunks.New();
        sortChunk->m_box = chunk->m_bbox;
        sortChunk->m_minZ = chunk->m_terrainBox.b.z;
        sortChunk->m_maxZ = chunk->m_terrainBox.t.z;
        sortChunk->m_chunk = chunk;
        sortChunk->m_flags = 0;

        if (chunk->m_holes) {
            sortChunk->m_flags = 0x20;
        }

        if (chunk->m_chunkFlags & 0x4) {
            sortChunk->m_flags |= 0x80;
        }

        return;
    }

    if (chunk->m_holes == 0xFFFF || this->CullModel(chunk->m_bbox)) {
        return;
    }

    float distSqr = CWorldView::DistSqrToBox(chunk->m_bbox, this->m_camPos);
    chunk->RenderPrep(this, distSqr);
    this->AddMapChunkToRenderList(chunk);
}

// OFFSET: 0x007597E0
void CWorldView::AddMapEntity(CMapEntity* entity) {
    uint32_t entityFlags = entity->m_entityFlags;

    if (((entityFlags & 0x80) && !entity->m_model->m_attachList) || (entity->m_staticFlags & 0x8)) {
        this->m_nonVisEntities.LinkToTail(entity);
        return;
    }

    C3Vector point;
    this->GetCameraClosestPoint(entity->m_bbox, point);
    float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    int32_t entry = 0;

    if (dist > 0.0f) {
        entry = static_cast<int32_t>(dist * 0.029999999f);

        if ((entityFlags & 0x100) && entry >= this->m_sortTable->m_lastEntry) {
            entry = this->m_sortTable->m_lastEntry - 1;
        }
    }

    if (entry < this->m_sortTable->m_lastEntry) {
        this->m_sortTable->m_entries.m_data[entry].m_entities.LinkToTail(entity);
    }
}

// OFFSET: 0x007598D0
void CWorldView::AddLowDetailMapArea(CMapAreaLow* area) {
    C3Vector point;
    this->GetCameraClosestPoint(area->m_bbox, point);
    float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    int32_t entry = 0;

    if (dist > 0.0f) {
        entry = static_cast<int32_t>(dist * 0.029999999f);

        if (entry >= this->m_sortTable->m_numEntries) {
            entry = this->m_sortTable->m_numEntries - 1;
        }
    }

    this->m_sortTable->m_entries.m_data[entry].m_lowDetailAreas.Add(1, &area);
}

// OFFSET: 0x007599B0
void CWorldView::AddLowDetailMapObjDef(CMapObjDef* mapObjDef) {
    C3Vector point;
    this->GetCameraClosestPoint(mapObjDef->m_bbox, point);
    float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    int32_t entry = 0;

    if (dist > 0.0f) {
        entry = static_cast<int32_t>(dist * 0.029999999f);

        if (entry >= this->m_sortTable->m_numEntries) {
            entry = this->m_sortTable->m_numEntries - 1;
        }
    }

    this->m_sortTable->m_entries.m_data[entry].m_lowDetailMapObjDefs.Add(1, &mapObjDef);
}

// OFFSET: 0x00759A90
void CWorldView::AddLowDetailMapAreaOcean(CMapAreaLow* area) {
    C3Vector point;
    this->GetCameraClosestPoint(area->m_bbox, point);
    float dist = point.z * this->m_horizPlane.n.z + point.y * this->m_horizPlane.n.y + point.x * this->m_horizPlane.n.x + this->m_horizPlane.d;
    int32_t entry = 0;

    if (dist > 0.0f) {
        entry = static_cast<int32_t>(dist * 0.029999999f);

        if (entry >= this->m_sortTable->m_numEntries) {
            entry = this->m_sortTable->m_numEntries - 1;
        }
    }

    this->m_sortTable->m_entries.m_data[entry].m_lowDetailOceans.Add(1, &area);
}

// OFFSET: 0x00759B70
static uint32_t AddViewerGroup(TSGrowableArray<uint32_t>& groups, uint32_t groupNum) {
    for (uint32_t i = 0; i < groups.Count(); i++) {
        if (groups.m_data[i] == groupNum) {
            return i;
        }
    }

    groups.Add(1, &groupNum);
    return groups.Count();
}

// OFFSET: 0x0075A310
void CWorldView::CullChunkLiquids(CMapChunk* chunk, int32_t cull) {
    if (!chunk->m_liquidList.Head()) {
        return;
    }

    if (cull && this->CullModel(chunk->m_liquidBox)) {
        return;
    }

    if (this->m_occlusion && (this->m_occlusion->QueryBuffer(chunk->m_liquidBox, 0xC000) | this->m_occlusion->QueryVolumes(chunk->m_liquidBox))) {
        return;
    }

    this->m_hasLiquids = 1;

    CChunkLiquid* liquid = chunk->m_liquidList.Head();

    while (liquid) {
        CChunkLiquid* next = chunk->m_liquidList.Next(liquid);
        liquid->m_frameStamp = World::GetFrameCnt();
        CMap::s_mapChunkLiquidUpdateList.LinkToTail(liquid);
        liquid->RenderPrep();

        if (liquid->m_instance) {
            Liquid::RendererDraw(liquid->m_instance);
        }

        liquid = next;
    }
}

// OFFSET: 0x0075A3F0
void CWorldView::GatherMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* group, CMapObj* mapObj, CMapObjGroup* mapObjGroup) {
    if (!(World::GetEnables() & 0x20)) {
        return;
    }

    CMapObjRender::GatherGroup(this, group);

    if ((mapObjGroup->m_groupFlags & 0x1000) && (this->m_flags & 0x200) && (World::GetEnables() & 0x200)) {
        this->GatherMapObjDefGroupLiquids(mapObjDef, group, mapObj, mapObjGroup);
    }

    if ((this->m_flags & 0x4) && this->m_cullFromInterior && (mapObjDef == this->m_viewerMapObjDef || mapObjDef == this->m_viewerMovedMapObjDef)) {
        this->m_alphaGroups.Add(1, &group);
    }
}

// OFFSET: 0x0075A480
void CWorldView::AddPortalViewExterior(CMapObj* mapObj, const SMOPortalInfo* portal, const SMOPortalRef* ref, uint16_t* ext, int32_t addViewer) {
    if (*ext & 0x4) {
        return;
    }

    *ext |= 0x4;

    C3Vector offset(portal->m_plane.n.x * 0.0099999998f, portal->m_plane.n.y * 0.0099999998f, portal->m_plane.n.z * 0.0099999998f);

    if (ref->m_side > 0) {
        offset = C3Vector(-offset.x, -offset.y, -offset.z);
    }

    C3Vector* verts = NULL;
    uint32_t count = 0;
    *ext |= this->TransformAndClipVerts((*ext >> 4) & 1, &mapObj->m_portalVertices[portal->m_startVertex], portal->m_count, offset, verts, count);

    if (count <= 2) {
        return;
    }

    CPortalView view;
    CWorldView::RectFromVerts(view.m_rect, verts, count);
    view.m_rect.minX = (view.m_rect.minX + 1.0f) * 0.5f;
    view.m_rect.maxX = (view.m_rect.maxX + 1.0f) * 0.5f;
    view.m_rect.minY = (view.m_rect.minY + 1.0f) * 0.5f;
    view.m_rect.maxY = (view.m_rect.maxY + 1.0f) * 0.5f;
    view.m_maxViewDepth = CMapObj::PortalViewDepth(portal, this->m_localCamPlane);
    this->m_exteriorPortalViews.Add(1, &view);
    this->m_frustumPortalView.Merge(view);

    if (addViewer) {
        CMapObj::PortalViewPos(portal, this->m_localToWorld, view.m_pos);
        this->m_exteriorPortalView.Merge(view);
    }
}

// OFFSET: 0x0075A690
void CWorldView::AddPortalViewInterior(CMapObj* mapObj, const SMOPortalInfo* portal, const SMOPortalRef* ref, uint16_t* ext, TSGrowableArray<CPortalView>* views) {
    if (*ext & 0xC) {
        return;
    }

    C3Vector offset(portal->m_plane.n.x * 0.0099999998f, portal->m_plane.n.y * 0.0099999998f, portal->m_plane.n.z * 0.0099999998f);

    if (ref->m_side > 0) {
        offset = C3Vector(-offset.x, -offset.y, -offset.z);
    }

    C3Vector* verts = NULL;
    uint32_t count = 0;
    *ext |= this->TransformAndClipVerts((*ext >> 4) & 1, &mapObj->m_portalVertices[portal->m_startVertex], portal->m_count, offset, verts, count);

    if (count > 2) {
        CPortalView view;
        CWorldView::RectFromVerts(view.m_rect, verts, count);

        uint32_t start = this->m_portalVerts.Count();
        view.m_rect.minX = (view.m_rect.minX + 1.0f) * 0.5f;
        view.m_rect.maxX = (view.m_rect.maxX + 1.0f) * 0.5f;
        view.m_rect.minY = (view.m_rect.minY + 1.0f) * 0.5f;
        view.m_rect.maxY = (view.m_rect.maxY + 1.0f) * 0.5f;

        this->m_portalVerts.SetCount(start + count);
        memcpy(&this->m_portalVerts.m_data[start], verts, sizeof(C3Vector) * count);
        view.m_vertStart = start;
        view.m_vertCount = count;
        views->Add(1, &view);
    }

    *ext |= 0x8;
}

// OFFSET: 0x0075A8B0
CWorldView::CWorldView(int32_t viewType) {
    this->m_viewType = viewType;
    this->m_portalVerts.m_chunk = 32;
    this->m_lowDetailAreas.m_chunk = 32;
    this->m_flags = 0;
    this->m_parentView = NULL;
    this->m_sortTable = NULL;
    this->m_occlusion = NULL;
    this->m_shadowQuery = NULL;
    this->m_shadowMode = 0;
    this->m_viewerMapObjDef = NULL;
    this->m_viewerMovedMapObjDef = NULL;
    this->m_viewStamp = 0;
    this->m_cullFromInterior = 0;
    this->m_lowDetailMapObjDefs.m_chunk = 16;

    for (uint32_t i = 0; i < 6; i++) {
        for (uint32_t j = 0; j < 6; j++) {
            this->m_renderChunks[i][j].m_chunk = 8;
        }
    }

    this->m_frustumIndex = 0;
    this->m_exteriorUnderwater = 0;
    this->m_underwater = 0;
    this->m_liquidGroups.m_chunk = 16;
    this->m_nearClip = 0.0f;
    this->m_farClip = 0.0f;
    this->m_fov = 0.0f;
    this->m_aspect = 0.0f;
    this->m_camLiquidStatus = 0;
    this->m_camDepth = 0.0f;
}

// OFFSET: 0x0075B320
CWorldView::~CWorldView() {
    if (this->m_sortTable) {
        this->m_sortTable->~CSortTable();
        SMemFree(this->m_sortTable, "delete", -1, 0);
        this->m_sortTable = NULL;
    }

    if (this->m_occlusion) {
        this->m_occlusion->~CWorldOcclusion();
        SMemFree(this->m_occlusion, "delete", -1, 0);
        this->m_occlusion = NULL;
    }

    if (this->m_shadowQuery) {
        this->m_shadowQuery->~CShadowQuery();
        SMemFree(this->m_shadowQuery, "delete", -1, 0);
        this->m_shadowQuery = NULL;
    }
}

// OFFSET: 0x0075B670
void CWorldView::UpdateViewer() {
    this->m_viewerMapObjDef = NULL;
    this->m_viewerMovedMapObjDef = NULL;
    this->m_viewerGroups.m_count = 0;
    this->m_viewerMovedGroups.m_count = 0;
    this->m_zoneName[0] = '\0';
    this->m_subzoneName[0] = '\0';
    this->m_viewerExteriorDepth = 0.0f;

    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    if (!(this->m_flags & 0x400000) && (this->m_flags & 0x2000)) {
        int32_t exterior;
        CMapChunk* chunk;
        int32_t interior;
        CMapObjDef* mapObjDefs[2];
        uint32_t groups[2][2];

        worldMap->LocateViewer(this->m_camPos, exterior, chunk, interior, mapObjDefs, groups);

        if (exterior) {
            const char* name = worldMap->m_maps[0] ? worldMap->m_maps[0]->m_mapName : NULL;
            SStrCopy(this->m_zoneName, name, sizeof(this->m_zoneName));
            SStrPrintf(this->m_subzoneName, sizeof(this->m_subzoneName), "%i, %i", chunk->m_chunkX / 16, chunk->m_chunkY / 16);
        } else if (interior) {
            CMapObjDef* mapObjDef = mapObjDefs[0];
            this->m_viewerMapObjDef = mapObjDef;

            if (mapObjDef) {
                CMapObj* mapObj = mapObjDef->m_mapObj;
                const char* groupName = mapObj->GetGroupName(groups[0][0]);

                SStrCopy(this->m_zoneName, mapObj->m_name, sizeof(this->m_zoneName));

                if (groupName) {
                    SStrCopy(this->m_subzoneName, groupName, sizeof(this->m_subzoneName));
                }

                AddViewerGroup(this->m_viewerGroups, groups[0][0]);

                if (groups[0][1] != 0xFFFF) {
                    AddViewerGroup(this->m_viewerGroups, groups[0][1]);
                }

                uint32_t groupFlags = 0;

                if (groups[0][0] != 0xFFFF) {
                    groupFlags = mapObj->GetGroupFlags(groups[0][0]);
                }

                if (groups[0][1] != 0xFFFF) {
                    groupFlags |= mapObj->GetGroupFlags(groups[0][1]);
                }

                if (groupFlags & 0x40140) {
                    CAaRect rect;
                    rect.minX = 0.0f;
                    rect.minY = 0.0f;
                    rect.maxX = 1.0f;
                    rect.maxY = 1.0f;

                    CPortalView view(rect, 0.0f);
                    this->m_exteriorPortalView.Merge(view);
                    this->AddExteriorPortal(view);
                }
            }

            this->m_viewerMovedMapObjDef = mapObjDefs[1];

            if (mapObjDefs[1]) {
                AddViewerGroup(this->m_viewerMovedGroups, groups[1][0]);

                if (groups[1][1] != 0xFFFF) {
                    AddViewerGroup(this->m_viewerMovedGroups, groups[1][1]);
                }
            }
        }
    }

    if (this->m_sortTable) {
        float height = 3.402823466e38f;

        if (worldMap->QueryFlightBoundsUpper(this->m_camPos, height) && height >= this->m_camPos.z && this->m_viewerExteriorDepth < 1.0f) {
            this->m_viewerExteriorDepth = 1.0f;
        }
    }
}

// OFFSET: 0x0075B930
void CWorldView::Render() {
    g_theGxDevicePtr->RsPush();
    GxXformPush(GxXform_View);
    CWorldScene::s_activeWorldView = this;
    DayNight::SetExteriorUnderwater(this->m_exteriorUnderwater);
    this->UpdateShadows();
    this->RenderSceneClear();
    CShaderEffect::UpdateProjMatrix();
    CWorldScene::SetupSceneLighting();
    CWorldScene::RenderSetFog();
    this->RenderOpaque();
    this->RenderAlpha();
    this->ClearLists();
    CWorldScene::s_activeWorldView = NULL;
    GxXformPop(GxXform_View);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0075B9C0
void CWorldView::AddDoodadDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& list, int32_t entry) {
    if (!(World::GetEnables() & 0x1) || !(this->m_flags & 0x800)) {
        return;
    }

    for (CMapBaseObjLink* link = list.Head(); link; link = list.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->m_cullStamp == this->m_viewStamp) {
            continue;
        }

        doodadDef->m_cullStamp = this->m_viewStamp;

        if (!(doodadDef->m_flags & 0x40) || (doodadDef->m_flags & 0x10)) {
            continue;
        }

        float dist = doodadDef->m_sphere.c.z * this->m_horizPlane.n.z + doodadDef->m_sphere.c.y * this->m_horizPlane.n.y + doodadDef->m_sphere.c.x * this->m_horizPlane.n.x + this->m_horizPlane.d - doodadDef->m_sphere.r;
        int32_t index = entry;

        if (dist > 0.0f) {
            index = static_cast<int32_t>(dist * 0.029999999f);

            if (index >= this->m_sortTable->m_lastEntry) {
                continue;
            }

            if (index < entry) {
                index = entry;
            }
        }

        this->m_sortTable->AddDoodadDef(doodadDef, index);
    }
}

// OFFSET: 0x0075BAC0
void CWorldView::AddMapObjDef(CMapObjDef* mapObjDef) {
    CMapObj* mapObj = mapObjDef->m_mapObj;

    for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(); group; group = mapObjDef->m_defGroupList.Next(group)) {
        if (!(group->m_flags & 0x40)) {
            continue;
        }

        if (!(mapObj->GetGroupFlags(group->m_groupNum) & 0x4010008)) {
            this->AddMapObjDefGroup(mapObjDef, group);
            continue;
        }

        CMapObjGroup* mapObjGroup = mapObj->GetGroup(group->m_groupNum, 1);

        if (!(mapObjGroup->m_flags & 0x40) || this->m_frustumBounds.t.x < group->m_bbox.b.x || this->m_frustumBounds.t.y < group->m_bbox.b.y || this->m_frustumBounds.t.z < group->m_bbox.b.z || group->m_bbox.t.x < this->m_frustumBounds.b.x || group->m_bbox.t.y < this->m_frustumBounds.b.y || group->m_bbox.t.z < this->m_frustumBounds.b.z) {
            continue;
        }

        if (!(mapObjGroup->m_groupFlags & 0x4000000)) {
            continue;
        }

        for (CMapObjOccluder* occluder = mapObjGroup->m_occluders.Head(); occluder; occluder = mapObjGroup->m_occluders.Next(occluder)) {
            C3Vector start = occluder->m_start * mapObjDef->m_mat;
            C3Vector end = occluder->m_end * mapObjDef->m_mat;
            this->AddOccluder(start, end);
        }
    }
}

// OFFSET: 0x0075BDA0
void CWorldView::CullChunks(CSortEntry* entry, int32_t index) {
    for (uint32_t i = 0; i < entry->m_chunks.Count(); i++) {
        if (this->m_flags & 0x8) {
            this->CullChunkReflection(entry->m_chunks.m_data[i]);
        } else {
            this->CullChunk(entry->m_chunks.m_data[i]);
        }
    }

    this->UpdateOcclusion(entry, index, 1);

    int32_t liquids = (this->m_flags & 0x200) && (World::GetEnables() & 0x200);

    for (uint32_t i = 0; i < entry->m_chunks.Count(); i++) {
        CSortEntryChunk& sortChunk = entry->m_chunks.m_data[i];
        uint16_t flags = sortChunk.m_flags;

        if (flags & 0x9) {
            continue;
        }

        CMapChunk* chunk = sortChunk.m_chunk;
        float x = chunk->m_bbox.b.x;

        if (x <= this->m_camPos.x) {
            x = chunk->m_bbox.t.x;

            if (this->m_camPos.x <= x) {
                x = this->m_camPos.x;
            }
        }

        float y = chunk->m_bbox.b.y;

        if (y <= this->m_camPos.y) {
            y = chunk->m_bbox.t.y;

            if (this->m_camPos.y <= y) {
                y = this->m_camPos.y;
            }
        }

        float z = chunk->m_bbox.b.z;

        if (z <= this->m_camPos.z) {
            z = chunk->m_bbox.t.z;

            if (this->m_camPos.z <= z) {
                z = this->m_camPos.z;
            }
        }

        float distSqr = (x - this->m_camPos.x) * (x - this->m_camPos.x) + (z - this->m_camPos.z) * (z - this->m_camPos.z) + (y - this->m_camPos.y) * (y - this->m_camPos.y);
        uint16_t minSize;

        if (distSqr < 0.0f || s_fadeOuterSqr[0] > distSqr) {
            minSize = 0;
        } else if (s_fadeOuterSqr[1] > distSqr) {
            minSize = 1;
        } else if (s_fadeOuterSqr[2] > distSqr) {
            minSize = 2;
        } else if (s_fadeOuterSqr[3] > distSqr) {
            minSize = 3;
        } else {
            minSize = 4;
        }

        int32_t cull = !(flags & 0x4);
        this->CullDoodadsExterior(chunk->m_doodadDefLinkList, minSize, chunk->m_map->m_header.m_flags & 0x8, cull);

        if (liquids) {
            this->CullChunkLiquids(chunk, cull);
        }

        if (!(sortChunk.m_flags & 0x12) && chunk->m_holes != 0xFFFF) {
            chunk->RenderPrep(this, distSqr);
            this->AddMapChunkToRenderList(chunk);
        }
    }
}

// OFFSET: 0x0075BFB0
void CWorldView::CullMapObjDefGroup(CMapObjDefGroup* group, int32_t cullContents) {
    if (!(group->m_flags & 0x40)) {
        return;
    }

    CMapObjDef* mapObjDef = group->m_mapObjDef;
    CMapObj* mapObj = mapObjDef->m_mapObj;
    CMapObjGroup* mapObjGroup = mapObj->GetGroup(group->m_groupNum, 0);

    if (group->m_cullStamp != this->m_viewStamp) {
        group->m_flags &= ~0x4u;
        group->m_cullStamp = this->m_viewStamp;
        this->GatherMapObjDefGroup(mapObjDef, group, mapObj, mapObjGroup);
    }

    if (mapObjGroup->m_groupFlags & 0x40) {
        this->m_renderExterior = 1;
    }

    if ((this->m_flags & 0x4) && (group->m_flags & 0x2)) {
        group->m_flags |= 0x4;
    }

    if (cullContents) {
        this->CullMapObjDefGroupContents(group);
    }
}

// OFFSET: 0x0075C040
void CWorldView::CullPortals(CMapObj* mapObj, uint32_t groupNum, uint32_t prevGroup, const CAaRect& rect, uint32_t depth, int32_t fromInterior) {
    if (depth > s_maxPortalDepth) {
        return;
    }

    CMapObjGroup* mapObjGroup = mapObj->GetGroup(groupNum, 0);

    if (!mapObjGroup) {
        return;
    }

    uint32_t groupFlags = mapObjGroup->m_groupFlags;

    if (groupFlags & 0x10000) {
        return;
    }

    if (this->m_cullFromInterior && (groupFlags & 0x40000)) {
        CWorldScene::renderSkyboxName = mapObj->m_skyboxName;
    }

    CMapObjDefGroup* group = this->m_cullMapObjDef->m_defGroups[groupNum];
    this->CullMapObjDefGroup(group, !(groupFlags & 0x8));

    if (!mapObjGroup->m_portalCount) {
        return;
    }

    if (!this->m_cullFromInterior && !depth) {
        this->m_cullPortalViews.SetCount(0);
        this->m_cullRects.SetCount(0);
    }

    const SMOPortalRef* ref = &mapObj->m_portalRefs[mapObjGroup->m_portalStart];

    for (uint32_t i = 0; i < mapObjGroup->m_portalCount; i++, ref++) {
        uint16_t targetGroup = ref->m_groupIndex;

        if (targetGroup == 0xFFFF || ref->m_portalIndex == this->m_cullMapObjDef->m_skipPortal) {
            continue;
        }

        const SMOPortalInfo* portal = &mapObj->m_portalInfos[ref->m_portalIndex];

        if (targetGroup == prevGroup) {
            continue;
        }

        uint32_t targetFlags = mapObj->GetGroupFlags(targetGroup);
        SPortalExt* ext = &this->m_portalExts.m_data[ref->m_portalIndex];

        if (ext->m_stamp != this->m_cullStamp) {
            ext->m_stamp = this->m_cullStamp;
            ext->m_flags = 0;

            if (!(targetFlags & 0x8) && !(groupFlags & 0x8)) {
                ext->m_flags = 0x10;
            }

            this->ClipPortalExt(mapObj, portal, ext);
        }

        float dist = portal->m_plane.Distance(this->m_localCamPos);

        if (ref->m_side < 0) {
            dist = -dist;
        }

        if (dist < 0.0f) {
            if (!this->m_cullFromInterior && !depth) {
                CAaRect empty;
                empty.minX = 0.0f;
                empty.minY = 0.0f;
                empty.maxX = 1.0f;
                empty.maxY = 1.0f;
                this->m_cullRects.Add(1, &empty);
            }

            continue;
        }

        if (!(ext->m_flags & 0x2) && (ext->m_flags & 0x1)) {
            continue;
        }

        if (ext->m_rect.minX > rect.maxY || rect.minX > ext->m_rect.maxY || ext->m_rect.minY > rect.maxX || rect.minY > ext->m_rect.maxX) {
            continue;
        }

        CAaRect clipped;
        clipped.minY = ext->m_rect.minY;
        clipped.minX = ext->m_rect.minX;
        clipped.maxY = ext->m_rect.maxX;
        clipped.maxX = ext->m_rect.maxY;

        if (rect.minX > clipped.minX) {
            clipped.minX = rect.minX;
        }

        if (clipped.maxX > rect.maxY) {
            clipped.maxX = rect.maxY;
        }

        if (rect.minY > clipped.minY) {
            clipped.minY = rect.minY;
        }

        if (clipped.maxX > rect.maxY) {
            clipped.maxX = rect.maxY;
        }

        if (fequalz(clipped.minX, clipped.maxX, 0.001f) || fequalz(clipped.minY, clipped.maxY, 0.001f)) {
            continue;
        }

        if (this->m_cullFromInterior) {
            if (targetFlags & 0x50148) {
                this->AddPortalViewExterior(mapObj, portal, ref, &ext->m_flags, targetFlags & 0x10008);

                if (targetFlags & 0x10008) {
                    continue;
                }
            }
        } else if (targetFlags & 0x10008) {
            continue;
        } else if (!depth && this->m_cullValid && !(targetFlags & 0x140)) {
            this->AddPortalViewInterior(mapObj, portal, ref, &ext->m_flags, &this->m_cullPortalViews);
        }

        CAaRect frustumRect;
        frustumRect.minY = (clipped.minY + 1.0f) * 0.5f;
        frustumRect.minX = (clipped.minX + 1.0f) * 0.5f;
        frustumRect.maxY = (clipped.maxY + 1.0f) * 0.5f;
        frustumRect.maxX = (clipped.maxX + 1.0f) * 0.5f;

        this->FrustumPush();
        CWorldView::FrustumSet(&this->m_frustumStack[this->m_frustumIndex], this->m_frustumCorners, frustumRect);
        this->CullPortals(mapObj, targetGroup, groupNum, clipped, depth + 1, fromInterior);
        this->m_frustumIndex--;
    }

    if (depth || !this->m_cullPortalViews.Count()) {
        return;
    }

    for (uint32_t i = 0; i < this->m_cullPortalViews.Count(); i++) {
        CPortalView& view = this->m_cullPortalViews.m_data[i];
        uint32_t j = 0;

        for (; j < this->m_cullRects.Count(); j++) {
            const CAaRect& r = this->m_cullRects.m_data[j];

            if (r.minY <= view.m_rect.maxY && r.minX <= view.m_rect.maxX && view.m_rect.minY <= r.maxY && view.m_rect.minX <= r.maxX) {
                break;
            }
        }

        if (j >= this->m_cullRects.Count()) {
            this->m_portalViews.Add(1, &view);
        }
    }

    this->m_cullPortalViews.SetCount(0);
    this->m_cullRects.SetCount(0);
}

// OFFSET: 0x0075C630
void CWorldView::CullMapObjDefGroupFromExterior(CMapObjDefGroup* group, const CAaRect& rect) {
    this->FrustumPush();
    CWorldView::FrustumSet(&this->m_frustumStack[this->m_frustumIndex], this->m_frustumCorners, rect);

    CMapObjDef* mapObjDef = group->m_mapObjDef;
    CMapObj* mapObj = mapObjDef->m_mapObj;
    uint32_t groupFlags = mapObj->GetGroupFlags(group->m_groupNum);
    this->m_flags &= ~0x4u;

    if (groupFlags & 0x10000) {
        this->CullMapObjDefGroup(group, 0);
        this->m_frustumIndex--;
        return;
    }

    if (groupFlags & 0x8) {
        this->SetupMapObjDefCull(mapObjDef);
        this->m_cullFromInterior = 0;

        CAaRect ndcRect;
        ndcRect.minX = rect.minX * 2.0f - 1.0f;
        ndcRect.minY = rect.minY * 2.0f - 1.0f;
        ndcRect.maxX = rect.maxX * 2.0f - 1.0f;
        ndcRect.maxY = rect.maxY * 2.0f - 1.0f;
        this->CullPortals(mapObj, group->m_groupNum, 0xFFFF, ndcRect, 0, 0);
    }

    this->m_frustumIndex--;
}

// OFFSET: 0x0075C750
void CWorldView::CullMapObjDefGroupFromInterior(CMapObjDefGroup* group, const CAaRect& rect) {
    this->FrustumPush();
    CWorldView::FrustumSet(&this->m_frustumStack[this->m_frustumIndex], this->m_frustumCorners, rect);
    this->m_flags &= ~0x4u;
    this->m_cullFromInterior = 1;

    CMapObjDef* mapObjDef = group->m_mapObjDef;
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!(mapObj->GetGroupFlags(group->m_groupNum) & 0x48)) {
        this->m_flags |= 0x4;
    }

    this->SetupMapObjDefCull(mapObjDef);

    CAaRect ndcRect;
    ndcRect.minX = rect.minX * 2.0f - 1.0f;
    ndcRect.minY = rect.minY * 2.0f - 1.0f;
    ndcRect.maxX = rect.maxX * 2.0f - 1.0f;
    ndcRect.maxY = rect.maxY * 2.0f - 1.0f;
    this->CullPortals(mapObj, group->m_groupNum, 0xFFFF, ndcRect, 0, 1);
    this->m_frustumIndex--;
}

// OFFSET: 0x0075C850
void CWorldView::CullMoveableMapObjDefGroups(const CAaRect& rect) {
    this->FrustumPush();
    this->FrustumSet(rect);

    CMapObjDefGroup* group = this->m_moveableGroups.Head();

    while (group) {
        CMapObjDefGroup* next = this->m_moveableGroups.Next(group);
        group->m_moveableLink.Unlink();

        if (group->m_mapObjDef->m_flags & 0x200) {
            if (!this->CullModel(group->m_bbox)) {
                this->m_renderExterior = 1;
                this->CullMapObjDefGroupFromExterior(group, rect);
                this->CullMapObjDefGroupContents(group);
            }
        } else if (!this->CullModel(group->m_bbox) && (group->m_flags & 0x40)) {
            CMapObjDef* mapObjDef = group->m_mapObjDef;
            CMapObj* mapObj = mapObjDef->m_mapObj;
            CMapObjGroup* mapObjGroup = mapObj->GetGroup(group->m_groupNum, 0);

            if (group->m_cullStamp != this->m_viewStamp) {
                group->m_flags &= ~0x4u;
                group->m_cullStamp = this->m_viewStamp;
                this->GatherMapObjDefGroup(mapObjDef, group, mapObj, mapObjGroup);
            }

            if (mapObjGroup->m_groupFlags & 0x40) {
                this->m_renderExterior = 1;
            }

            if ((this->m_flags & 0x4) && (group->m_flags & 0x2)) {
                group->m_flags |= 0x4;
            }
        }

        group = next;
    }

    this->m_frustumIndex--;
}

// OFFSET: 0x0075C9D0
void CWorldView::CullMapObjDefGroups(CSortEntry* entry, const CAaRect& rect, int32_t index) {
    CMapObjDefGroup* group = entry->m_mapObjDefGroups.Head();

    while (group) {
        CMapObjDefGroup* next = entry->m_mapObjDefGroups.Next(group);
        group->m_moveableLink.Unlink();

        if (!this->CullModel(group->m_bbox) && !(this->m_occlusion && (this->m_occlusion->QueryBuffer(group->m_bbox, 0xC000) | this->m_occlusion->QueryVolumes(group->m_bbox)))) {
            if (!this->CullModel(group->m_portalBox) && !this->OcclusionQueryBox(group->m_portalBox, 0xC000)) {
                this->CullMapObjDefGroupFromExterior(group, rect);
            }

            this->AddDoodadDefs(group->m_doodadLinks, index);
        }

        group = next;
    }
}

// OFFSET: 0x0075CAE0
void CWorldView::CullSortTable(const CAaRect& rect) {
    CWorldScene::CreateClipVolumes(this->m_occlusion, this->m_camPos, this->m_frustumCorners, 0, this->m_frustumPortalView.m_maxViewDepth);
    CWorldScene::AddWorldOccluders(this);

    CFrustum lowDetailFrustum;
    this->CalcLowDetailFrustum(lowDetailFrustum, rect);

    this->FrustumPush();
    this->FrustumSet(rect);

    for (int32_t i = 0; i < this->m_sortTable->m_numEntries; i++) {
        CSortEntry* entry = &this->m_sortTable->m_entries.m_data[i];
        this->CullChunks(entry, i);
        this->CullMapObjDefGroups(entry, rect, i);
        this->CullEntitys(entry);

        float dist = static_cast<float>(i) * 33.333332f;
        uint16_t minSize;

        if (dist < 0.0f || s_fadeOuter[0] > dist) {
            minSize = 0;
        } else if (s_fadeOuter[1] > dist) {
            minSize = 1;
        } else if (s_fadeOuter[2] > dist) {
            minSize = 2;
        } else if (s_fadeOuter[3] > dist) {
            minSize = 3;
        } else {
            minSize = 4;
        }

        this->CullDoodads(this->m_sortTable, i, minSize);
        this->CullLowDetail(entry, lowDetailFrustum);
        this->UpdateOcclusion(entry, i, 0);
    }

    this->m_frustumIndex--;
}

// OFFSET: 0x0075CC70
void CWorldView::CullReflection() {
    this->m_renderExterior = 0;

    if (this->m_flags & 0x2000) {
        CAaRect fullRect;
        fullRect.minX = 0.0f;
        fullRect.minY = 0.0f;
        fullRect.maxX = 1.0f;
        fullRect.maxY = 1.0f;

        for (uint32_t i = 0; i < this->m_viewerGroups.Count(); i++) {
            this->CullMapObjDefGroupFromInterior(this->m_viewerMapObjDef->m_defGroups[this->m_viewerGroups.m_data[i]], fullRect);
        }

        CWorldView* parent = this->m_parentView;

        for (uint32_t i = 0; i < parent->m_liquidGroups.Count(); i++) {
            CMapObjDefGroup* group = parent->m_liquidGroups.m_data[i];

            if (group->m_mapObjDef == parent->m_viewerMapObjDef) {
                this->m_flags |= 0x4;
            } else {
                this->m_flags &= ~0x4u;
            }

            this->FrustumPush();
            CAaRect rect;
            rect.minX = 0.0f;
            rect.minY = 0.0f;
            rect.maxX = 1.0f;
            rect.maxY = 1.0f;
            CWorldView::FrustumSet(&this->m_frustumStack[this->m_frustumIndex], this->m_frustumCorners, rect);

            if (group->m_flags & 0x40) {
                CMapObjDef* mapObjDef = group->m_mapObjDef;
                CMapObj* mapObj = mapObjDef->m_mapObj;
                CMapObjGroup* mapObjGroup = mapObj->GetGroup(group->m_groupNum, 0);

                if (group->m_cullStamp != this->m_viewStamp) {
                    group->m_flags &= ~0x4u;
                    group->m_cullStamp = this->m_viewStamp;
                    this->GatherMapObjDefGroup(mapObjDef, group, mapObj, mapObjGroup);
                }

                if (mapObjGroup->m_groupFlags & 0x40) {
                    this->m_renderExterior = 1;
                }

                if ((this->m_flags & 0x4) && (group->m_flags & 0x2)) {
                    group->m_flags |= 0x4;
                }

                this->CullMapObjDefGroupContents(group);
            }

            this->m_frustumIndex--;
        }

        this->CullMoveableMapObjDefGroups(fullRect);
    }

    this->m_renderExterior = 1;
    this->m_exteriorPortalView.m_rect.minX = 0.0f;
    this->m_exteriorPortalView.m_rect.minY = 0.0f;
    this->m_exteriorPortalView.m_rect.maxX = 1.0f;
    this->m_exteriorPortalView.m_rect.maxY = 1.0f;
    this->m_exteriorPortalView.m_pos = C3Vector();
    this->m_exteriorPortalView.m_maxViewDepth = 0.0f;
    this->m_exteriorPortalView.m_vertStart = 0;
    this->m_exteriorPortalView.m_vertCount = 0;
    this->m_frustumPortalView.m_rect.minX = 0.0f;
    this->m_frustumPortalView.m_rect.minY = 0.0f;
    this->m_frustumPortalView.m_rect.maxX = 1.0f;
    this->m_frustumPortalView.m_rect.maxY = 1.0f;
    this->m_frustumPortalView.m_pos = C3Vector();
    this->m_frustumPortalView.m_maxViewDepth = 0.0f;
    this->m_frustumPortalView.m_vertStart = 0;
    this->m_frustumPortalView.m_vertCount = 0;

    if (this->m_flags & 0x11000) {
        this->UpdateViewerUnderwater();
        this->AddMoveableMapObjDefGroups();

        if (this->m_sortTable) {
            this->CullSortTable(this->m_frustumPortalView.m_rect);
        }
    } else if (this->m_sortTable) {
        this->m_sortTable->Clear(this->m_nonVisEntities);
    }

    this->UpdateNonVisEntitys();

    if (this->m_sortTable) {
        this->m_sortTable->Finish();
    }

    this->m_moveableGroups.UnlinkAll();
}

// OFFSET: 0x0075D010
void CWorldView::Cull() {
    if (this->m_occlusion) {
        this->m_occlusion->Clear();
        this->m_occlusion->ClearVolumes();
    }

    if (this->m_flags & 0x8) {
        this->CullReflection();
        return;
    }

    int32_t cullExterior = 1;
    this->m_renderExterior = 0;

    if (this->m_viewerMapObjDef) {
        CAaRect fullRect;
        fullRect.minX = 0.0f;
        fullRect.minY = 0.0f;
        fullRect.maxX = 1.0f;
        fullRect.maxY = 1.0f;
        cullExterior = 0;
        int32_t interiorVisible = 1;

        if (this->m_viewerMovedMapObjDef) {
            CAaRect movedRect = fullRect;

            for (uint32_t i = 0; i < this->m_viewerMovedGroups.Count(); i++) {
                this->CullMapObjDefGroupFromInterior(this->m_viewerMovedMapObjDef->m_defGroups[this->m_viewerMovedGroups.m_data[i]], movedRect);
            }

            interiorVisible = this->m_frustumPortalView.m_maxViewDepth >= 0.0f;
            this->m_exteriorPortalView = CPortalView();
            this->m_frustumPortalView = CPortalView();
            this->m_exteriorPortalViews.SetCount(0);
            this->m_portalViews.SetCount(0);
        }

        if (interiorVisible) {
            for (uint32_t i = 0; i < this->m_viewerGroups.Count(); i++) {
                this->CullMapObjDefGroupFromInterior(this->m_viewerMapObjDef->m_defGroups[this->m_viewerGroups.m_data[i]], fullRect);
            }
        }

        this->CullMoveableMapObjDefGroups(fullRect);

        float depth = this->m_frustumPortalView.m_maxViewDepth;

        if (depth >= 0.0f) {
            if (depth > this->m_viewerExteriorDepth) {
                this->m_viewerExteriorDepth = depth;
            }

            this->UpdateViewerUnderwater();
            cullExterior = 1;
        }
    } else {
        this->AddMoveableMapObjDefGroups();
        this->m_renderExterior = 1;
        this->m_exteriorPortalView.Clear();
        this->m_frustumPortalView.Clear();
    }

    if (this->m_sortTable) {
        if (cullExterior) {
            this->CullSortTable(this->m_frustumPortalView.m_rect);
        } else {
            this->m_sortTable->Clear(this->m_nonVisEntities);
        }
    }

    this->UpdateNonVisEntitys();

    if (this->m_sortTable) {
        this->m_sortTable->Finish();
    }

    this->m_moveableGroups.UnlinkAll();
}

// OFFSET: 0x0075A070
void CWorldView::UpdateOcclusion(CSortEntry* entry, int32_t index, int32_t chunks) {
    uint32_t enabled = World::GetEnables() & 0x1000;

    if (this->m_viewerExteriorDepth > static_cast<float>(index) * 33.333332f) {
        enabled = 0;
    }

    if (!this->m_occlusion) {
        enabled = 0;
    }

    if (chunks) {
        if (!enabled) {
            return;
        }

        for (uint32_t i = 0; i < entry->m_chunks.Count(); i++) {
            CSortEntryChunk& sortChunk = entry->m_chunks.m_data[i];
            uint16_t flags = sortChunk.m_flags;

            if (!((this->m_flags & 0x8) && (flags & 0x80)) && (!(flags & 0x20) || (flags & 0x2))) {
                continue;
            }

            uint32_t occluderFlags = (flags & 0x40) ? 6 : 4;
            C3Vector segment[2];
            segment[0] = C3Vector(sortChunk.m_box.t.x, sortChunk.m_box.t.y, sortChunk.m_box.b.z);
            segment[1] = C3Vector(sortChunk.m_box.b.x, sortChunk.m_box.b.y, sortChunk.m_box.b.z);
            this->m_occlusion->UpdateBuffer(segment, occluderFlags);

            segment[0].y = sortChunk.m_box.b.y;
            segment[1].y = sortChunk.m_box.t.y;
            this->m_occlusion->UpdateBuffer(segment, occluderFlags);
        }

        return;
    }

    if (enabled) {
        int32_t keepOccluders = (this->m_flags & 0x100) && (World::GetEnables() & 0x800000);

        for (uint32_t i = 0; i < entry->m_chunks.Count(); i++) {
            CSortEntryChunk& sortChunk = entry->m_chunks.m_data[i];

            if (!(sortChunk.m_flags & 0x30)) {
                this->AddChunkOccluders(sortChunk, 0, sortChunk.m_flags & 0x40, keepOccluders);
            }
        }

        int32_t debugOccluders = (this->m_flags & 0x100) && (World::GetEnables() & 0x400000);
        int32_t exterior = ~(this->m_flags >> 3) & 1;
        CWorldOccluder* occluder = entry->m_occluders.Head();

        while (occluder) {
            CWorldOccluder* next = entry->m_occluders.Next(occluder);
            occluder->ClipBufferUpdate(this->m_occlusion, exterior);

            if (debugOccluders) {
                this->m_occluders.LinkToTail(occluder);
            } else {
                occluder->Free();
            }

            occluder = next;
        }
    }

    CWorldOccluder* occluder = entry->m_occluders.Head();

    while (occluder) {
        CWorldOccluder* next = entry->m_occluders.Next(occluder);
        occluder->Free();
        occluder = next;
    }

    entry->m_chunks.SetCount(0);
}

// OFFSET: 0x00757390
void CWorldView::AddChunkOccluders(const CSortEntryChunk& sortChunk, int32_t a2, int32_t a3, int32_t keep) {
    uint32_t flags = a2 ? 4 : 0;

    if (a3) {
        flags |= 2;
    }

    C3Vector segment[2];
    segment[0] = C3Vector(sortChunk.m_box.t.x, sortChunk.m_box.t.y, sortChunk.m_box.b.z);
    segment[1] = C3Vector(sortChunk.m_box.b.x, sortChunk.m_box.b.y, sortChunk.m_box.b.z);
    this->m_occlusion->UpdateBuffer(segment, flags);

    if (keep) {
        CWorldOccluder* occluder = CWorldOccluder::Alloc();
        occluder->m_start = segment[0];
        occluder->m_end = segment[1];
        this->m_occluders.LinkToTail(occluder);
    }

    segment[0].y = sortChunk.m_box.b.y;
    segment[1].y = sortChunk.m_box.t.y;
    this->m_occlusion->UpdateBuffer(segment, flags);

    if (keep) {
        CWorldOccluder* occluder = CWorldOccluder::Alloc();
        occluder->m_start = segment[0];
        occluder->m_end = segment[1];
        this->m_occluders.LinkToTail(occluder);
    }
}

// OFFSET: 0x00759C10
void CWorldView::CreateInverseExteriorPortalList() {
    CAaRect rects[2][128];
    int32_t counts[2] = { 1, 0 };

    rects[0][0].minX = 0.0f;
    rects[0][0].minY = 0.0f;
    rects[0][0].maxX = 1.0f;
    rects[0][0].maxY = 1.0f;

    int32_t pass = 0;
    int32_t viewCount = this->m_exteriorPortalViews.Count();

    for (; pass < viewCount; pass++) {
        int32_t src = pass & 1;
        int32_t dst = (pass - 1) & 1;
        counts[dst] = 0;

        if (counts[src] > 60) {
            break;
        }

        const CAaRect& cut = this->m_exteriorPortalViews.m_data[pass].m_rect;

        for (int32_t i = 0; i < counts[src]; i++) {
            CAaRect r = rects[src][i];

            if (cut.minY >= r.maxY || r.minY >= cut.maxY || cut.minX >= r.maxX || r.minX >= cut.maxX) {
                rects[dst][counts[dst]++] = r;
            } else {
                if (cut.minX > r.minX) {
                    CAaRect piece = r;
                    piece.maxX = cut.minX;
                    rects[dst][counts[dst]++] = piece;
                    r.minX = cut.minX;
                }

                if (r.maxX > cut.maxX) {
                    CAaRect piece = r;
                    piece.minX = cut.maxX;
                    rects[dst][counts[dst]++] = piece;
                    r.maxX = cut.maxX;
                }

                if (cut.minY > r.minY) {
                    CAaRect piece = r;
                    piece.maxY = cut.minY;
                    rects[dst][counts[dst]++] = piece;
                    r.minY = cut.minY;
                }

                if (r.maxY > cut.maxY) {
                    CAaRect piece = r;
                    piece.minY = cut.maxY;
                    rects[dst][counts[dst]++] = piece;
                }
            }

            if (counts[dst] > 60) {
                break;
            }
        }
    }

    int32_t result = pass & 1;
    int32_t count = counts[result];
    this->m_exteriorPortalViews.SetCount(count);

    for (int32_t i = 0; i < count; i++) {
        this->m_exteriorPortalViews.m_data[i].m_rect = rects[result][i];
        this->m_exteriorPortalViews.m_data[i].m_maxViewDepth = 0.0f;
    }
}
