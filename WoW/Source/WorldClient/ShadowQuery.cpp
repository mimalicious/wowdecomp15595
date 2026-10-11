#include "WoW/Source/WorldClient/ShadowQuery.h"
#include <cmath>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00E89008
CShadowQuery* CShadowQuery::s_activeShadowQuery;

// STATIC: 0x00E8900C
CVar* CShadowQuery::s_shadowCull;

// STATIC: 0x00E89014
CVar* CShadowQuery::s_shadowScissor;

// STATIC: 0x00E89018
CVar* CShadowQuery::s_shadowInstancing;

// PROVEN: d847b370 IDENTICAL/NONE
// OFFSET: 0x00768B10
void CShadowQuery::Destroy() {
    CShadowCache::Destroy();
}

// OFFSET: 0x00768B20
void CShadowQuery::GetSize(CShadowCacheQueryData& data, int32_t band, float& minRadius, float& maxRadius) {
    maxRadius = 10000.0f;

    if (band == 0 && data.m_queryFlags[0] == 22) {
        maxRadius = 25.0f;
        minRadius = 0.25f;
    } else if (band < 1) {
        minRadius = 0.25f;
    } else if (band < 2) {
        minRadius = 2.0f;
    } else {
        minRadius = 10.0f;
    }
}

// PROVEN: ae9db892 IDENTICAL/NONE
// OFFSET: 0x00768BA0
void CShadowQuery::AllocBuffers() {
    for (uint32_t i = 0; i < 3; i++) {
        this->m_queries[i].AllocMapObjGroupList(2048);
        this->m_queries[i].AllocModelBatchList(0x2000);
    }
}

// OFFSET: 0x00768CE0
void CShadowQuery::GetMatrix(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band) {
    if (!(radius > 0.0f)) {
        return;
    }

    const C3Vector& camPos = this->m_worldView->m_camPos;
    C44Matrix proj;
    C44Matrix lookAt;
    C3Vector eye = (center - CShadowCache::s_sunDir * 2000.0f) - camPos;
    C3Vector at = center - camPos;
    C44Matrix view;
    C44Matrix invView;
    GxXformView(view);
    C44Matrix temp = view;
    invView = temp.Inverse(temp.Determinant());
    float flip = g_theGxDevicePtr->m_caps.m_texelCenterOnEdge ? -1.0f : 1.0f;
    GxuXformCreateOrtho(-radius, radius, -(flip * radius), flip * radius, 1.0f, 4000.0f, proj);
    GxuXformCreateLookAtXXX(eye, at, direction, lookAt);
    matrix = invView * lookAt * proj;
    C44Matrix depth = invView * lookAt;
    matrix.a2 = depth.a2;
    matrix.b2 = depth.b2;
    matrix.c2 = depth.c2;
    matrix.d2 = depth.d2;
    float pcfBias = CShaderEffect::s_usePcfFiltering ? 0.5f : 0.0f;
    float bandBias = 0.5f;

    if (band == 0) {
        bandBias = 2.0f;
    } else if (band == 1) {
        bandBias = 4.0f;
    } else if (band == 2) {
        bandBias = 8.0f;
    }

    matrix.d2 = depth.d2 - (bandBias * 0.2f + pcfBias);
}

// OFFSET: 0x00769190
void CShadowQuery::GetFrustum(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band) {
    const float* bounds = data.m_bounds[band];
    const C3Vector& pos = data.m_pos[band];
    C3Vector eye = pos - CShadowCache::s_sunDir * 2000.0f;
    GxuXformCreateOrtho(bounds[0], bounds[1], bounds[2], bounds[3], 1.0f, 4000.0f, matrix);
    C44Matrix lookAt;
    C3Vector corners[8];
    GxuXformCreateLookAtXXX(eye, pos, direction, lookAt);
    GxuXformCalcFrustumCorners(lookAt, matrix, corners);
    frustum.CalcPlanesFromCorners(corners);
    frustum.InvertPlanes();
    frustum.SwapCorners();
    float* cullRect = data.m_cullRects[band];
    cullRect[0] = -1.0f;
    cullRect[1] = -1.0f;
    cullRect[2] = 1.0f;
    cullRect[3] = 1.0f;
    CFrustum& queryFrustum = data.unk_04EC[band];
    queryFrustum = frustum;

    if (!(data.m_queryFlags[band] & 0x14) || !CShadowQuery::s_shadowCull->m_intValue) {
        return;
    }

    CFrustum viewFrustum = this->m_worldView->m_lowDetailFrustum;
    C44Matrix viewProj = lookAt * matrix;
    viewFrustum.Transform(viewProj);
    CAaBox box = CAaBox::Bounding(viewFrustum.corners, 8);
    box.b.x = box.b.x <= -1.0f ? -1.0f : box.b.x;
    box.b.y = box.b.y <= -1.0f ? -1.0f : box.b.y;
    box.b.z = box.b.z <= -1.0f ? -1.0f : box.b.z;
    box.b.x = box.b.x >= 1.0f ? 1.0f : box.b.x;
    box.b.y = box.b.y >= 1.0f ? 1.0f : box.b.y;
    box.b.z = box.b.z >= 1.0f ? 1.0f : box.b.z;
    box.t.x = box.t.x <= -1.0f ? -1.0f : box.t.x;
    box.t.y = box.t.y <= -1.0f ? -1.0f : box.t.y;
    box.t.z = box.t.z <= -1.0f ? -1.0f : box.t.z;
    box.t.x = box.t.x >= 1.0f ? 1.0f : box.t.x;
    box.t.y = box.t.y >= 1.0f ? 1.0f : box.t.y;
    box.t.z = box.t.z >= 1.0f ? 1.0f : box.t.z;
    float minX = -(bounds[0] * box.b.x);
    float maxX = bounds[1] * box.t.x;
    float minY = -(bounds[2] * box.b.y);
    float maxY = box.t.y * bounds[3];

    if (maxX > minX && maxY > minY && box.t.z > box.b.z) {
        C44Matrix ortho;
        GxuXformCreateOrtho(minX, maxX, minY, maxY, 1.0f, 4000.0f, ortho);
        GxuXformCalcFrustumCorners(lookAt, ortho, corners);
        queryFrustum.CalcPlanesFromCorners(corners);
        queryFrustum.InvertPlanes();
        queryFrustum.SwapCorners();
        cullRect[1] = box.b.x;
        cullRect[3] = box.t.x;
        cullRect[0] = -box.t.y;
        cullRect[2] = -box.b.y;
    }
}

// OFFSET: 0x00769740
int32_t CShadowQuery::Query(CShadowCacheQueryData& data, uint32_t frame) {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return 0;
    }

    this->AllocBuffers();

    if (this->m_frame++ == 0xFF) {
        this->m_frame = 1;
    }

    uint32_t flags = 0;

    for (int32_t i = 0; i <= data.m_lastBand; i++) {
        flags |= data.m_queryFlags[i];
    }

    if (flags & 0x6) {
        worldMap->ShadowQueryEntitys(*this, data, 0, data.m_lastBand);
    }

    if (data.m_useFrustum) {
        if (flags & 0x18) {
            const CFrustum& frustum = data.m_frustums[data.m_lastBand];
            C3Vector center = frustum.corners[0];
            center += frustum.corners[3];
            center += frustum.corners[1];
            center += frustum.corners[2];
            center *= 0.25f;
            CWorldScene::CreateClipVolumes(this->m_occlusion, center, frustum.corners, 0, 0.0f);
            worldMap->ShadowQueryMapObjDefs(*this, data, 0, data.m_lastBand);
            worldMap->ShadowQueryTerrain(*this, data, 0, data.m_lastBand);
        }
    } else if (flags & 0x18) {
        for (int32_t band = 0; band <= data.m_lastBand; band++) {
            this->m_occlusion->ClearVolumes();
            worldMap->ShadowQueryMapObjDefs(*this, data, band);
            worldMap->ShadowQueryTerrain(*this, data, band);
        }
    }

    return 1;
}

// PROVEN: 2b7c86b7 IDENTICAL/NONE
// OFFSET: 0x00769960
void CShadowQuery::MatrixCallback(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band) {
    if (CShadowQuery::s_activeShadowQuery) {
        CShadowQuery::s_activeShadowQuery->GetMatrix(center, radius, matrix, direction, band);
    }
}

// PROVEN: e863ffd6 IDENTICAL/NONE
// OFFSET: 0x00769990
void CShadowQuery::FrustumCallback(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band) {
    if (CShadowQuery::s_activeShadowQuery) {
        CShadowQuery::s_activeShadowQuery->GetFrustum(frustum, matrix, direction, data, band);
    }
}

// PROVEN: 3f0b4aaa IDENTICAL/NONE
// OFFSET: 0x007699C0
int32_t CShadowQuery::QueryCallback(CShadowCacheQueryData& data, uint32_t frame) {
    if (CShadowQuery::s_activeShadowQuery) {
        return CShadowQuery::s_activeShadowQuery->Query(data, frame);
    }

    return 0;
}

// OFFSET: 0x007699E0
void CShadowQuery::SetEntityCutoff(const C3Vector& pos, const C3Vector& camPos) {
    C44Matrix view;
    C44Matrix proj;
    GxXformView(view);
    GxXformProjection(proj);
    C44Matrix viewProj = view * proj;
    C3Vector dir = TransformDirection(C3Vector(0.0f, 0.0f, 1.0f), view);
    float scale = 1.0f / sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
    dir.y = dir.y * scale;
    C3Vector point = (pos - camPos) * viewProj;
    float x = dir.x * scale;
    float z = dir.z * scale;
    CShadowCache::s_cutoffPlane.x = x;
    CShadowCache::s_cutoffPlane.y = dir.y;
    CShadowCache::s_cutoffPlane.z = z;
    CShadowCache::s_cutoffPlane.w = -(point.z * z + point.y * dir.y + point.x * x);
    this->m_entityCutoff = pos.z + 2.0f;
}

// OFFSET: 0x00769CB0
void CShadowQuery::Update() {
    C3Vector sunDir = CWorldMap::s_sunLight.m_dir;
    CWorldView* view = this->m_worldView;
    int32_t useCullFrustum = view->m_frustumPortalView.m_maxViewDepth >= 0.0f || view->m_renderExterior;
    sunDir.z = sunDir.z * 5.0f;

    if (sunDir.z < -1.2f) {
        sunDir.z = -1.2f;
    }

    float scale = 1.0f / sqrt(sunDir.x * sunDir.x + sunDir.y * sunDir.y + sunDir.z * sunDir.z);
    sunDir.x = sunDir.x * scale;
    sunDir.y = sunDir.y * scale;
    sunDir.z = scale * sunDir.z;
    CShadowCache::PreUpdate(sunDir);
    C3Vector target = World::GetCamTarget();
    CShadowCache::SetDebugParams(1.0f, 0);
    CShadowCache::Update(target, useCullFrustum);
    CShadowCache::PostUpdate();
}

// OFFSET: 0x00769DD0
void CShadowQuery::Clear(CGxFrameBuffer* frameBuffer, const float* rect) {
    CGxFrameBuffer* oldFrameBuffer = g_theGxDevicePtr->FrameBufferGet();
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);
    GxXformSetViewport(rect[0], rect[1], rect[2], rect[3], 0.0f, 1.0f);

    if (CShadowCache::GetShadowMode() == SHADOW_MEDIUM) {
        GxSceneClearToViewport(31, CImVector(0xFFFFFFFF));
    } else {
        GxSceneClear(31, CImVector(0xFFFFFFFF));
    }

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(oldFrameBuffer);
}

// PROVEN: 2a220642 IDENTICAL/NONE
// OFFSET: 0x00769F50/0x1404517D0
CShadowQuery::CShadowQuery() {
    void* mem = SMemAlloc(sizeof(CWorldOcclusion), __FILE__, __LINE__, 0);
    this->m_occlusion = mem ? new (mem) CWorldOcclusion() : NULL;
    this->m_worldView = NULL;
    this->m_frame = 0;
    this->m_entityCutoff = 0.0f;
}

// OFFSET: 0x00769FC0/0x140451860
CShadowQuery::~CShadowQuery() {
    CWorldOcclusion* occlusion = this->m_occlusion;

    if (occlusion) {
        occlusion->~CWorldOcclusion();
        SMemFree(occlusion, "delete", -1, 0);
    }

    for (uint32_t i = 0; i < 3; i++) {
        this->m_queries[i].FreeMapObjGroupList();
        this->m_queries[i].FreeModelList();
        this->m_queries[i].FreeModelBatchList();
    }
}

// OFFSET: 0x0076A030
int32_t CShadowQuery::Render(CShadowCacheQueryData& data, int32_t band, CGxFrameBuffer* frameBuffer, C3Vector& direction) {
    CWorldQuery& query = this->m_queries[band];

    if (!query.m_mapObjGroupCount && !query.m_untexturedBatches.m_count && !query.m_texturedBatches.m_count) {
        CShadowQuery::Clear(frameBuffer, data.m_texBounds[band]);
        return 1;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Culling, 0);
    CShaderEffect::SetTexMtx_Identity(0);
    C44Matrix savedProj;
    C3Vector eye;
    C3Vector at;
    C44Matrix lookAt;
    C44Matrix savedView;
    C44Matrix invView;
    CGxFrameBuffer* oldFrameBuffer = g_theGxDevicePtr->FrameBufferGet();
    GxRsSet(GxRs_Fog, 0);
    const C3Vector& camPos = this->m_worldView->m_camPos;
    const C3Vector& pos = data.m_pos[band];
    eye = (pos - CShadowCache::s_sunDir * 2000.0f) - camPos;
    at = pos - camPos;
    C44Matrix world;
    world.d0 = -camPos.x;
    world.d1 = -camPos.y;
    world.d2 = -camPos.z;
    GxXformProjection(savedProj);
    const float* texBounds = data.m_texBounds[band];
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(texBounds[0], texBounds[1], texBounds[2], texBounds[3], 0.0f, 1.0f);
    GxXformView(savedView);
    C44Matrix temp = savedView;
    invView = temp.Inverse(temp.Determinant());
    C4Vector params(0.0f, 0.0f, 0.0f, 0.00025000001f);
    CShadowCache::SetShadowScale(0.00025000001f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, &params.x, 1);
    g_theGxDevicePtr->XformSetProjection(data.m_matrices[band]);
    CShaderEffect::UpdateProjMatrix();
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(frameBuffer);

    if (CShadowCache::GetShadowMode() == SHADOW_MEDIUM) {
        GxSceneClearToViewport(31, CImVector(0xFFFFFFFF));
    } else {
        GxSceneClear(31, CImVector(0xFFFFFFFF));
    }

    if (CShadowQuery::s_shadowScissor->m_intValue) {
        GxRsSet(GxRs_ScissorTest, 1);
        const float* cullRect = data.m_cullRects[band];
        CRect scissor;
        scissor.minY = (cullRect[0] + 1.0f) * 0.5f;
        scissor.minX = (cullRect[1] + 1.0f) * 0.5f;
        scissor.maxY = (cullRect[2] + 1.0f) * 0.5f;
        scissor.maxX = (cullRect[3] + 1.0f) * 0.5f;
        g_theGxDevicePtr->SetScissor(scissor);
    }

    GxuXformCreateLookAtXXX(eye, at, direction, lookAt);
    g_theGxDevicePtr->XformSetView(lookAt);
    C44Matrix texMatrix;
    texMatrix = (invView * lookAt).Transpose();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 14, &texMatrix.a0, 3);

    if (query.m_mapObjGroupCount) {
        CMapObj::RenderMapObjGroupsShadowMap(query.m_mapObjGroups, query.m_mapObjGroupCount, world, data.m_frustums[band]);
    }

    if (query.m_untexturedBatches.m_count || query.m_texturedBatches.m_count) {
        if (CShadowQuery::s_shadowInstancing->m_intValue) {
            query.m_untexturedBatches.Optimize();
            query.m_texturedBatches.Optimize();
        }

        CM2Model::RenderModelBatchesShadowMap(query.m_untexturedBatches, query.m_texturedBatches);
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    g_theGxDevicePtr->XformSetView(savedView);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->FrameBufferSet(oldFrameBuffer);
    g_theGxDevicePtr->XformSetProjection(savedProj);
    CShaderEffect::UpdateProjMatrix();
    return 1;
}

// PROVEN: dadba1ff IDENTICAL/NONE
// OFFSET: 0x0076A9B0
int32_t CShadowQuery::RenderCallback(CShadowCacheQueryData& data, int32_t band, CGxFrameBuffer* frameBuffer, C3Vector& direction) {
    if (CShadowQuery::s_activeShadowQuery) {
        return CShadowQuery::s_activeShadowQuery->Render(data, band, frameBuffer, direction);
    }

    return 0;
}

// OFFSET: 0x0076A9E0/0x140452730
int32_t CShadowQuery::Initialize() {
    CShadowCache::SetMatrixCallback(CShadowQuery::MatrixCallback);
    CShadowCache::SetFrustumCallback(CShadowQuery::FrustumCallback);
    CShadowCache::SetQueryCallback(CShadowQuery::QueryCallback);
    CShadowCache::SetRenderCallback(CShadowQuery::RenderCallback);
    CShadowQuery::s_shadowCull = CVar::Register("shadowCull", "enable shadow frustum culling", 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CShadowQuery::s_shadowScissor = CVar::Register("shadowScissor", "enable scissoring when rendering shadowmaps", 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CShadowQuery::s_shadowInstancing = CVar::Register("shadowInstancing", "enable instancing when rendering shadowmaps", 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CShadowCache::Create();
    return 1;
}
