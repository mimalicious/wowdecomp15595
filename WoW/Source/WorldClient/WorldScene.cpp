#include "WorldScene.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Particulate.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00E70C48
CWorldView* CWorldScene::s_defaultWorldView;

// STATIC: 0x00E70C44
CWorldView* CWorldScene::s_defaultReflectionView;

// STATIC: 0x00E70C40
CWorldView* CWorldScene::s_defaultReflectionView2;

// STATIC: 0x00E70C34
CWorldView* CWorldScene::s_activeWorldView;

// STATIC: 0x00E70C4C
CM2Scene* CWorldScene::s_m2Scene;

// STATIC: 0x00E70C24
CGxShader* CWorldScene::s_vsWaterFogPoly;

// STATIC: 0x00E70C20
CGxShader* CWorldScene::s_psWaterFogPoly;

// STATIC: 0x00E70C2C
CTexture* CWorldScene::s_defaultTexture;

// STATIC: 0x00E70C28
CTexture* CWorldScene::s_defaultBlendTexture;

// STATIC: 0x00E70C50
uintptr_t CWorldScene::camTargEntity;

// STATIC: 0x00E70C30
const char* CWorldScene::renderSkyboxName;

// STATIC: 0x00E70C58
C3Vector CWorldScene::s_playerPos;

// STATIC: 0x00E70C88
CBarrier CWorldScene::s_barrier;

// STATIC: 0x00E70C64
TSExplicitList<CWorldRenderCallback, offsetof(CWorldRenderCallback, m_link)> CWorldScene::s_renderCallbackList[3];

// STATIC: 0x00D00B28
TSExplicitList<CWorldFadeEntry, offsetof(CWorldFadeEntry, m_link)> CWorldScene::s_fadeoutList;

// STATIC: 0x00D00B34
TSExplicitList<CWorldFadeEntry, offsetof(CWorldFadeEntry, m_freeLink)> CWorldScene::s_fadeoutFreeList;

// STATIC: 0x00E70CD8
CM2Lighting CWorldScene::s_lighting;

// STATIC: 0x00E70DA8
CWorldSceneLight CWorldScene::s_sceneLights[3];

CWorldSceneLight::CWorldSceneLight() {
    this->m_color = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_source = NULL;
    this->unk_0004 = 0.0f;
    this->m_fade = 0.0f;
    this->m_fadeOut = 0;
    this->m_fadeIn = 0;
    this->m_free = 1;
}

// OFFSET: 0x00726BF0
void CWorldScene::ClearBarriers() {
    CWorldScene::s_barrier.m_vertices.SetCount(0);
    CWorldScene::s_barrier.m_contacts.SetCount(0);
    CWorldScene::s_barrier.m_chunks.m_count = 0;
    CWorldScene::s_barrier.m_mapObjDefGroups.m_count = 0;
}

// OFFSET: 0x00726F00
CM2Model* CWorldScene::LoadModel(const char* name, const C44Matrix& mat, void (*eventCallback)(CM2Model*, uint32_t, uint32_t, uint32_t, const C3Vector&, uint32_t, uint64_t), uint64_t eventParam, void (*lightingCallback)(CM2Model*, CM2Lighting*, void*), void* lightingParam, int32_t a7, int32_t a8, int32_t a9, int32_t a10) {
    uint32_t flags = 0;

    if (a8) {
        flags = 0x10;
    }

    if (a9) {
        flags |= 0x40;
    }

    if (a10) {
        flags |= 0x80;
    }

    CM2Model* model = CWorldScene::s_m2Scene->CreateModel(name, flags, 3);

    if (!model) {
        return NULL;
    }

    model->m_fFlags |= 0x4000;
    model->m_worldTransform = mat;
    model->SetEventCallback(reinterpret_cast<void*>(eventCallback), eventParam);
    model->m_lightingCallback = lightingCallback;
    model->m_lightingArg = lightingParam;
    model->SetBoneSequence(-1, 0, -1, 0, 1.0f, 1, 1, 0, 0, 0);

    if (a7) {
        model->WaitUntilLoaded();
    }

    return model;
}

// OFFSET: 0x00726FB0
void CWorldScene::GetFrustumCornersInWorldSpace(const C44Matrix& view, const C44Matrix& proj, const C3Vector& camPos, C3Vector* corners) {
    GxuXformCalcFrustumCorners(view, proj, corners);

    for (uint32_t i = 0; i < 8; i++) {
        corners[i].x = camPos.x + corners[i].x;
        corners[i].y = corners[i].y + camPos.y;
        corners[i].z = corners[i].z + camPos.z;
    }
}

// OFFSET: 0x00727130
void CWorldScene::GetFrustumCorners(C3Vector* corners) {
    memcpy(corners, CWorldScene::s_defaultWorldView->m_frustumCorners, sizeof(CWorldScene::s_defaultWorldView->m_frustumCorners));
}

// OFFSET: 0x00727150
void CWorldScene::UpdateViewer() {
    CWorldScene::s_defaultWorldView->m_flags &= ~0x400000u;
    CWorldScene::s_defaultWorldView->UpdateViewer();
    CWorldScene::s_defaultWorldView->UpdateViewerLiquid();

    uint32_t submerged = CWorldScene::s_defaultWorldView->m_camLiquidStatus;
    Liquid::SetCameraSubmerged(submerged);
    Liquid::SetCamPos(World::GetCamPos());
    ViewSettings::SetSubmerged(submerged);

    if (!CWorldScene::s_defaultWorldView->m_viewerMapObjDef && !CWorldScene::s_defaultWorldView->m_viewerMovedMapObjDef) {
        CWorldScene::s_defaultWorldView->m_flags |= 0x400000;
    }
}

// OFFSET: 0x007271C0
void CWorldScene::UpdateLiquid() {
    Liquid::Update();

    if (!Liquid::GetWaterDetail()) {
        CWorldScene::s_m2Scene->m_liquidTypeId = 0;
        return;
    }

    CWorldScene::s_m2Scene->m_liquidTypeId = 1;

    const DayNight::FogInfo* fog = DayNight::GetFogInfoUW();
    Liquid::SetUnderWaterFog(fog->m_start, fog->m_end, fog->m_density, fog->m_color);
    ViewSettings::SetFog(3, fog->m_start, fog->m_end, fog->m_density, fog->m_color);

    const DayNight::FogInfo* intFog = DayNight::GetIntFogInfoUW();
    ViewSettings::SetFog(2, intFog->m_start, intFog->m_end, intFog->m_density, intFog->m_color);

    CImVector color = DayNight::GetFogInfoAW()->m_color;
    Liquid::SetAboveWaterFog(-10.0f, 200.0f, 5.0f, color);
}

// OFFSET: 0x007272C0
void CWorldScene::CallRenderCallbacks(uint32_t pass) {
    for (CWorldRenderCallback* callback = CWorldScene::s_renderCallbackList[pass].Head(); callback; callback = CWorldScene::s_renderCallbackList[pass].Next(callback)) {
        callback->m_callback(callback->m_param, pass, callback->m_param64);
    }
}

// OFFSET: 0x00727310
void CWorldScene::SetupSceneLighting() {
    const DayNight::FogInfo* fogInfo = DayNight::GetFogInfo();
    const C3Vector& guardBand = World::GetGuardBandPosition();
    CAaSphere sphere;
    sphere.c = guardBand;
    sphere.r = 32.0f;
    CWorldScene::s_lighting.Initialize(NULL, sphere);
    CWorldScene::s_lighting.AddLight(&CWorldMap::s_sunLight);

    C3Vector fogColor;
    fogColor.x = fogInfo->m_color.r * 0.0039215689f;
    fogColor.y = fogInfo->m_color.g * 0.0039215689f;
    fogColor.z = fogInfo->m_color.b * 0.0039215689f;
    CWorldScene::s_lighting.SetFog(fogColor, fogInfo->m_start, fogInfo->m_end, fogInfo->m_density);

    CM2Lighting selected(sphere);
    CWorldScene::s_m2Scene->SelectLights(&selected);
    uint32_t lightCount = selected.m_lightCount;
    float step = FrameTime::GetTickTimeSec() * 3.0f;
    int32_t freeSlot = -1;

    for (uint32_t i = 0; i < 3; i++) {
        CWorldSceneLight& slot = CWorldScene::s_sceneLights[i];

        if (slot.m_free) {
            freeSlot = i;
            continue;
        }

        uint32_t j = 0;

        for (; j < lightCount; j++) {
            if (selected.m_lights[j] == slot.m_source) {
                break;
            }
        }

        if (j < lightCount) {
            slot.m_light.Copy(*selected.m_lights[j]);
            slot.m_color = slot.m_light.m_dirColor;
            selected.m_lights[j] = NULL;
            slot.m_fadeOut = 0;
            slot.m_fadeIn = 1;
        } else {
            slot.m_fadeIn = 0;
            slot.m_fadeOut = 1;
        }

        if (slot.m_fadeOut) {
            slot.m_fade -= step;

            if (slot.m_fade < 0.0f) {
                slot.m_fade = 0.0f;
                slot.m_fadeOut = 0;
                slot.m_free = 1;
                freeSlot = i;
            }
        } else if (slot.m_fadeIn) {
            slot.m_fade += step;

            if (slot.m_fade > 1.0f) {
                slot.m_fade = 1.0f;
                slot.m_fadeIn = 0;
            }
        }
    }

    if (freeSlot != -1) {
        for (uint32_t j = 0; j < lightCount; j++) {
            CM2Light* light = selected.m_lights[j];

            if (!light) {
                continue;
            }

            CWorldSceneLight& slot = CWorldScene::s_sceneLights[freeSlot];
            slot.m_free = 0;
            slot.m_fadeIn = 1;
            slot.m_source = light;
            slot.m_fade = 0.0f;
            slot.m_light.Copy(*light);
            slot.m_color = slot.m_light.m_dirColor;
            break;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        CWorldSceneLight& slot = CWorldScene::s_sceneLights[i];

        if (slot.m_free) {
            continue;
        }

        slot.m_light.m_dirColor = C3Vector(slot.m_color.x * slot.m_fade, slot.m_color.y * slot.m_fade, slot.m_fade * slot.m_color.z);
        CWorldScene::s_lighting.AddLight(&slot.m_light);
    }
}

// OFFSET: 0x00727620
void CWorldScene::RenderLowDetailAreas(CWorldView* view) {
    if (!view->m_lowDetailAreas.Count()) {
        return;
    }

    DefaultShaders::SetDefaultShaders(GxVBF_P, -1);

    for (uint32_t i = 0; i < view->m_lowDetailAreas.Count(); i++) {
        view->m_lowDetailAreas.m_data[i]->Render();
    }
}

// OFFSET: 0x00727670
void CWorldScene::RenderLowDetailMapObjDefs(CWorldView* view, const C44Matrix& world) {
    CWorldQueryElement groups[256];
    uint32_t count = 0;

    for (uint32_t i = 0; i < view->m_lowDetailMapObjDefs.Count(); i++) {
        CMapObjDef* mapObjDef = view->m_lowDetailMapObjDefs.m_data[i];
        CMapObj* mapObj = mapObjDef->m_mapObj;
        uint32_t groupCount = mapObj->m_groupCount;

        for (uint32_t j = 0; j < groupCount; j++) {
            if (!mapObj->IsGroupLoaded(j)) {
                continue;
            }

            CMapObjGroup* group = mapObj->GetGroup(j, 0);

            if ((group->m_groupFlags & 0x8) && count < 256) {
                groups[count].m_group = group;
                groups[count].m_matrix = &mapObjDef->m_mat;
                count++;
            }
        }
    }

    if (count) {
        CImVector color(0);
        DefaultShaders::GetFogColor(color);
        CMapObj::RenderMapObjGroupsColor(groups, count, world, color);
    }
}

// OFFSET: 0x00727C80
void CWorldScene::RenderReflection(CWorldView* view, CGxFrameBuffer* frameBuffer, uint32_t mode, float height) {
    if (!Liquid::GetWaterDetail()) {
        return;
    }

    view->m_flags &= ~0x400000u;

    if (CWorldScene::s_defaultWorldView->m_flags & 0x400000) {
        view->m_flags |= 0x400000;
    }

    if (!CWorldScene::s_defaultWorldView->m_hasLiquids) {
        return;
    }

    GxFrameBufferSet(frameBuffer);

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.94f);

    if (mode > 1) {
        view->m_flags |= 0x11000;
    } else {
        view->m_flags &= ~0x11000u;
    }

    if (mode > 2) {
        view->m_flags |= 0x2000;
    } else {
        view->m_flags &= ~0x2000u;
    }

    C4Plane plane;
    plane.n.x = 0.0f;
    plane.n.y = 0.0f;
    plane.n.z = 1.0f;
    plane.d = -height;

    if (Liquid::CameraSubmerged()) {
        plane.n.z = -1.0f;
    }

    CWorldView* parent = CWorldScene::s_defaultWorldView;
    C3Vector camPos = parent->m_camPos;
    C3Vector camTarget = parent->m_camTarget;
    C3Vector camDir = parent->m_camDir;
    plane.Reflect(parent->m_camPos, camPos);
    plane.Reflect(parent->m_camTarget, camTarget);

    view->m_cullPlane = plane;
    camDir.z = -camDir.z;

    C44Matrix viewMatrix;
    C44Matrix projNative;
    C44Matrix projection;
    CGxDevice* device = g_theGxDevicePtr;
    viewMatrix = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    projNative = device->m_projNative;

    if (!device->DeviceApi()) {
        projNative.c0 = projNative.c0 * -1.0f;
        projNative.c1 = projNative.c1 * -1.0f;
        projNative.c2 = projNative.c2 * -1.0f;
        projNative.c3 = projNative.c3 * -1.0f;
    }

    projection = g_theGxDevicePtr->m_projection;

    C44Matrix reflectedView = viewMatrix;
    float scale = -1.0f / sqrt(viewMatrix.c2 * viewMatrix.c2 + viewMatrix.c1 * viewMatrix.c1 + viewMatrix.c0 * viewMatrix.c0);
    reflectedView.c0 = viewMatrix.c0 * scale;
    reflectedView.c1 = reflectedView.c1 * scale;
    reflectedView.c2 = reflectedView.c2 * scale;

    C44Matrix inverse = projNative.Inverse(projNative.Determinant());
    C44Matrix transposed = inverse.Transpose();

    plane.d = -fabs(height - camPos.z);
    plane.Transform(reflectedView);

    C4Plane clipPlane;
    clipPlane.n.x = transposed.a0 * plane.n.x + transposed.b0 * plane.n.y + transposed.c0 * plane.n.z + transposed.d0 * plane.d;
    clipPlane.n.y = transposed.a1 * plane.n.x + transposed.b1 * plane.n.y + transposed.c1 * plane.n.z + transposed.d1 * plane.d;
    clipPlane.n.z = transposed.a2 * plane.n.x + transposed.b2 * plane.n.y + transposed.c2 * plane.n.z + transposed.d2 * plane.d;
    clipPlane.d = transposed.a3 * plane.n.x + transposed.b3 * plane.n.y + transposed.c3 * plane.n.z + transposed.d3 * plane.d;
    plane = clipPlane;

    C44Matrix lookAt;
    C3Vector up(0.0f, 0.0f, 1.0f);
    C3Vector eye(0.0f, 0.0f, 0.0f);
    GxuXformCreateLookAtSgCompat(eye, camDir, up, lookAt);
    g_theGxDevicePtr->XformSetView(lookAt);

    C3Vector corners[8];
    CWorldScene::GetFrustumCornersInWorldSpace(lookAt, projection, camPos, corners);

    view->Update(camPos, camTarget, parent->m_nearClip, parent->m_farClip, parent->m_fov, parent->m_aspect);
    view->UpdateViewer();
    view->UpdateViewerLiquid();
    view->Gather();
    view->Cull();

    g_theGxDevicePtr->XformSetView(reflectedView);
    view->UpdateViewMat();
    CShaderEffect::SetClipPlane(plane);
    CShaderEffect::SetClipEnabled(1);
    view->Render();
    CShaderEffect::SetClipEnabled(0);
    g_theGxDevicePtr->XformSetView(viewMatrix);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x00728480
void CWorldScene::RenderSetFog() {
    const DayNight::FogInfo* fog = DayNight::GetFogInfo();

    if (!g_theGxDevicePtr->m_caps.m_shaderTargets[0]) {
        GxRsSet(GxRs_FogStart, fog->m_start);
        GxRsSet(GxRs_FogEnd, fog->m_end);
    }

    GxRsSet(GxRs_FogColor, *reinterpret_cast<const int32_t*>(&fog->m_color));
    CShaderEffect::SetFogParams(fog->m_start, fog->m_end, fog->m_density, fog->m_color, 0.0f);
}

// OFFSET: 0x00728810
void CWorldScene::ClearObjectFadeoutList() {
    CWorldFadeEntry* entry = CWorldScene::s_fadeoutList.Head();

    while (entry) {
        CWorldFadeEntry* next = CWorldScene::s_fadeoutList.Next(entry);
        World::ObjectDestroy(reinterpret_cast<uintptr_t>(entry->m_entity));
        entry->m_entity = NULL;
        entry->m_entityLink.Unlink();
        CWorldScene::s_fadeoutList.UnlinkNode(entry);
        CWorldScene::s_fadeoutFreeList.LinkToHead(entry);
        entry = next;
    }
}

// OFFSET: 0x007288F0
void __fastcall CWorldScene::TextureRecreationCallback() {
    CWorldScene::ClearObjectFadeoutList();
}

// OFFSET: 0x00728900
void CWorldScene::Initialize() {
    CBarrier::Initialize();
    CWorldOccluder::Initialize();

    void* mem = SMemAlloc(sizeof(CWorldView), __FILE__, __LINE__, 0);
    CWorldScene::s_defaultWorldView = mem ? new (mem) CWorldView(0) : NULL;
    CWorldScene::s_defaultReflectionView = NULL;
    CWorldScene::s_defaultReflectionView2 = NULL;
    CWorldScene::s_defaultWorldView->EnableSortTable(1);
    CWorldScene::s_defaultWorldView->EnableOcclusion(1);
    CWorldScene::s_defaultWorldView->m_flags |= 0x1;
    CWorldScene::s_activeWorldView = NULL;
    CWorldScene::camTargEntity = 0;

    Liquid::RendererInit();
    CWorldScene::s_vsWaterFogPoly = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "WaterFogPoly");
    CWorldScene::s_psWaterFogPoly = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "WaterFogPoly");

    CImVector gray;
    *reinterpret_cast<uint32_t*>(&gray) = 0xFF808080;
    CWorldScene::s_defaultTexture = TextureCreateSolid(gray);

    CImVector black;
    *reinterpret_cast<uint32_t*>(&black) = 0xFF000000;
    CWorldScene::s_defaultBlendTexture = TextureCreateSolid(black);

    CWorldScene::s_m2Scene = M2CreateScene();
    CWorldScene::s_m2Scene->SetProjectTextureCallback(World::ProjectTextureCallback, NULL);
    CWorldScene::s_m2Scene->SetProjectPositionCallback(World::ProjectPositionCallback, NULL);
    g_theGxDevicePtr->AddTextureRecreationCallback(CWorldScene::TextureRecreationCallback);
}

// OFFSET: 0x00728A20
void CWorldScene::Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect) {
    Liquid::RendererValidate();
    CWorldScene::renderSkyboxName = NULL;

    CWorldView* view = CWorldScene::s_defaultWorldView;
    view->EnableSortTable(1);
    view->EnableOcclusion(1);
    view->m_shadowMode = CShadowCache::GetShadowMode();
    view->m_flags |= 0x10;
    view->m_flags |= 0x40;
    view->m_flags |= 0x20;
    view->m_flags |= 0x80;
    view->m_flags |= 0x1FFF00;
    view->EnableShadow(CShadowCache::GetShadowMode() > 0);
    view->Update(camPos, camTarget, nearClip, farClip, fov, aspect);

    CWorldScene::UpdateFadeOutObjects();
}

// OFFSET: 0x00728AE0
void CWorldScene::Render(const C3Vector& pos, float elapsedSec) {
    const CBoundingBox& viewport = g_theGxDevicePtr->m_viewport;
    GxXformSetViewport(viewport.m_x.m_l, viewport.m_x.m_h, viewport.m_y.m_l, viewport.m_y.m_h, viewport.m_z.m_l, 0.94f);

    CAaRect extents;
    extents.minX = viewport.m_y.m_l;
    extents.minY = viewport.m_x.m_l;
    extents.maxX = viewport.m_y.m_h;
    extents.maxY = viewport.m_x.m_h;
    ViewSettings::SetViewExtents(extents);

    CMapRenderChunk::UpdatePools();
    CMapObj::UpdatePools();
    CDetailDoodad::UpdatePools();
    CShadowCache::CreateResources();
    CWorldScene::s_defaultWorldView->Gather();
    CWorldScene::s_defaultWorldView->Cull();
    Liquid::UpdateProceduralTextures();

    C2Vector displSize(128.0f, 128.0f);
    Liquid::SetDisplMapInfo(CWorldScene::s_playerPos, displSize);
    Liquid::UpdateDisplMap(FrameTime::GetCurTimeMs());

    CM2Scene* scene = CWorldScene::s_m2Scene;

    if (scene) {
        scene->m_flags |= 0x1;
        scene->m_shadowValue = CWorldScene::s_defaultWorldView->m_shadowMode;
        scene->AdvanceTime(static_cast<uint32_t>(elapsedSec * 1000.0f));
        scene->Animate(pos);
        scene->BeginDraw();
        scene->m_flags &= ~0x1u;
        scene->m_passMask = World::GetM2PassEnables();
    }

    CWorldScene::CallRenderCallbacks(0);

    if (!CWorldScene::s_defaultReflectionView) {
        void* mem = SMemAlloc(sizeof(CWorldView), __FILE__, __LINE__, 0);
        CWorldView* view = mem ? new (mem) CWorldView(1) : NULL;
        CWorldScene::s_defaultReflectionView = view;
        view->m_flags |= 0x8000;
        view->m_flags |= 0x8;
        view->m_flags |= 0x200000;
        view->m_parentView = CWorldScene::s_defaultWorldView;
        view->EnableSortTable(1);
        view->EnableOcclusion(1);
        view->m_viewStamp = CWorldScene::s_defaultWorldView->m_viewStamp + 1;
    }

    if (!CWorldScene::s_defaultReflectionView2) {
        void* mem = SMemAlloc(sizeof(CWorldView), __FILE__, __LINE__, 0);
        CWorldView* view = mem ? new (mem) CWorldView(2) : NULL;
        CWorldScene::s_defaultReflectionView2 = view;
        view->m_flags |= 0x8000;
        view->m_flags |= 0x8;
        view->m_flags |= 0x200000;
        view->m_parentView = CWorldScene::s_defaultWorldView;
        view->EnableSortTable(1);
        view->EnableOcclusion(1);
        view->m_viewStamp = CWorldScene::s_defaultWorldView->m_viewStamp + 2;
    }

    if (Liquid::HasRealReflection()) {
        float height = Liquid::GetClosestReflectionHeight();
        uint32_t mode = Liquid::GetReflectionMode();
        CWorldScene::RenderReflection(CWorldScene::s_defaultReflectionView, Liquid::GetRealReflectionFB(), mode, height);
    }

    if (Liquid::HasFakeReflection()) {
        CWorldScene::RenderReflection(CWorldScene::s_defaultReflectionView2, Liquid::GetFakeReflectionFB(), 0, 0.0f);
    }

    CGxFrameBuffer* backBuffer = g_theGxDevicePtr->BackBuffer();

    GxRsSet(GxRs_ScissorTest, 0);

    g_theGxDevicePtr->FrameBufferSet(backBuffer);
    CWorldScene::s_defaultWorldView->Render();

    CWorldView* view = CWorldScene::s_defaultWorldView;

    if ((World::GetEnables() & 0x400) && view->m_camLiquidStatus) {
        CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

        if (worldMap && worldMap->m_particulate) {
            worldMap->m_particulate->Render(view);
        }
    }

    DayNight::RenderGlares();

    if (World::GetEnables() & 0x4000000) {
        CMap::TestQueryRender();
        CMap::TestQueryReset();
    }

    if ((World::GetEnables() & 0x20000000) && CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex]) {
        CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex]->RenderFlightBounds(pos);
    }

    if (DepthCache::DepthRangeDumpEnabled()) {
        DepthCache::DrawDepthRangeDump();
    }

    if (scene) {
        scene->GarbageCollectModels();
        scene->EndDraw();
    }
}

// OFFSET: 0x007291E0
void CWorldScene::RenderChunksLayered(CWorldView* view) {
    int32_t vertexShader = CMap::enableTerrainShaderVertex != 0;

    for (uint32_t shaderType = 0; shaderType < 6; shaderType++) {
        CMapRenderChunk::SetShaders(shaderType, view->m_shadowMode);

        for (uint32_t i = 0; i < 4; i++) {
            int32_t pixelShader = CMapRenderChunk::s_pixelShaderIds[i];

            if (pixelShader >= 0) {
                g_theGxDevicePtr->RsSet(GxRs_PixelShader, &CMap::ps_terrain->m_permutes[pixelShader]);
            }

            TSGrowableArray<CWorldView::CVisRenderChunk>& list = view->m_renderChunks[i + 1][shaderType];

            for (uint32_t j = 0; j < list.Count(); j++) {
                CMapRenderChunk* renderChunk = list.m_data[j].m_renderChunk;
                CMapRenderChunkBatch* batch = &renderChunk->m_batches[list.m_data[j].m_batch];
                renderChunk->RenderSetup(batch, vertexShader);

                if (World::GetEnables() & 0x2) {
                    if (CMapRenderChunk::s_pixelShaderIds[i] >= 0) {
                        if (vertexShader) {
                            CMapRenderChunk::RenderSinglePassVertexPixelShader(view, batch);
                        } else {
                            CMapRenderChunk::RenderSinglePassPixelShader(view, batch);
                        }
                    } else {
                        CMapRenderChunk::s_renderMultiPass(view, renderChunk, batch);
                    }
                }

                renderChunk->m_link.Unlink();
                renderChunk->m_lastUsedFrame = World::GetFrameCnt();
                CMap::s_mapRenderChunkUpdateList.LinkToTail(renderChunk);
            }
        }
    }

    GxRsSet(GxRs_VertexShader, static_cast<void*>(NULL));
    GxRsSet(GxRs_PixelShader, static_cast<void*>(NULL));
}

// OFFSET: 0x00729400
void CWorldScene::RenderChunksSolid(CWorldView* view) {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    CMap* map = worldMap->m_maps[0];
    uint32_t shaderType = map && ((map->m_header.m_flags >> 2) & 1) ? 2 : 0;
    CMapRenderChunk::SetShaders(shaderType, view->m_shadowMode);

    if (CMapRenderChunk::s_pixelShaderIds[0] >= 0) {
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &CMap::ps_terrain->m_permutes[CMapRenderChunk::s_pixelShaderIds[0]]);
    }

    int32_t vertexShader = CMap::enableTerrainShaderVertex != 0;
    TSGrowableArray<CWorldView::CVisRenderChunk>& list = view->m_renderChunks[0][shaderType];

    for (uint32_t j = 0; j < list.Count(); j++) {
        CMapRenderChunk* renderChunk = list.m_data[j].m_renderChunk;
        CMapRenderChunkBatch* batch = &renderChunk->m_batches[list.m_data[j].m_batch];
        renderChunk->RenderSetup(batch, vertexShader);

        if (World::GetEnables() & 0x2) {
            if (CMapRenderChunk::s_pixelShaderIds[0] >= 0) {
                if (vertexShader) {
                    CMapRenderChunk::RenderSolidVertexPixelShader(view, batch);
                } else {
                    CMapRenderChunk::RenderSolidPixelShader(batch);
                }
            } else {
                CMapRenderChunk::RenderSolid(batch);
            }
        }

        renderChunk->m_link.Unlink();
        renderChunk->m_lastUsedFrame = World::GetFrameCnt();
        CMap::s_mapRenderChunkUpdateList.LinkToTail(renderChunk);
    }
}

// OFFSET: 0x00729700
void CWorldScene::Destroy() {
    if (CWorldScene::s_m2Scene) {
        CWorldScene::s_m2Scene->GarbageCollectModels();
        CWorldScene::s_m2Scene->Release();
        CWorldScene::s_m2Scene = NULL;
    }

    if (CWorldScene::s_defaultTexture) {
        TextureRelease(CWorldScene::s_defaultTexture);
        CWorldScene::s_defaultTexture = NULL;
    }

    if (CWorldScene::s_defaultBlendTexture) {
        TextureRelease(CWorldScene::s_defaultBlendTexture);
        CWorldScene::s_defaultBlendTexture = NULL;
    }

    if (CWorldScene::s_vsWaterFogPoly) {
        g_theGxDevicePtr->ShaderRelease(CWorldScene::s_vsWaterFogPoly);
        CWorldScene::s_vsWaterFogPoly = NULL;
    }

    if (CWorldScene::s_psWaterFogPoly) {
        g_theGxDevicePtr->ShaderRelease(CWorldScene::s_psWaterFogPoly);
        CWorldScene::s_psWaterFogPoly = NULL;
    }

    Liquid::RendererDestroy();
    CWorldOccluder::Destroy();
    CBarrier::Destroy();

    for (uint32_t i = 0; i < 3; i++) {
        CWorldScene::s_renderCallbackList[i].Clear();
    }

    CWorldScene::s_fadeoutFreeList.Clear();

    if (CWorldScene::s_defaultWorldView) {
        CWorldScene::s_defaultWorldView->~CWorldView();
        SMemFree(CWorldScene::s_defaultWorldView, "delete", -1, 0);
        CWorldScene::s_defaultWorldView = NULL;
    }

    if (CWorldScene::s_defaultReflectionView) {
        CWorldScene::s_defaultReflectionView->~CWorldView();
        SMemFree(CWorldScene::s_defaultReflectionView, "delete", -1, 0);
        CWorldScene::s_defaultReflectionView = NULL;
    }

    if (CWorldScene::s_defaultReflectionView2) {
        CWorldScene::s_defaultReflectionView2->~CWorldView();
        SMemFree(CWorldScene::s_defaultReflectionView2, "delete", -1, 0);
        CWorldScene::s_defaultReflectionView2 = NULL;
    }

    g_theGxDevicePtr->RemoveTextureRecreationCallback(CWorldScene::TextureRecreationCallback);
}

// OFFSET: 0x007298A0
void CWorldScene::RenderAlphaWater(CWorldView* view) {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    CWorldScene::RenderSetFog();

    if (DepthCache::IsValid()) {
        g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(DepthCache::GetDepthSR()));
    }

    Liquid::RendererFlush(FrameTime::GetCurTimeMs(), view->m_camPos, 1);
    worldMap->RenderWaterRipples(view);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0072A0E0
void CWorldScene::RenderLowDetail(CWorldView* view) {
    if (view->m_frustumPortalView.m_maxViewDepth < 0.0f || !(World::GetEnables() & 0x100)) {
        view->m_lowDetailAreas.m_count = 0;
        view->m_lowDetailMapObjDefs.m_count = 0;
        return;
    }

    float minX = 0.0f;
    float maxX = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, 0.99804688f, 0.99902344f);

    C44Matrix projection;
    view->CalcProjection(projection);
    C44Matrix savedProjection = g_theGxDevicePtr->m_projection;
    g_theGxDevicePtr->XformSetProjection(projection);

    C4Plane savedClip;
    savedClip.n = C3Vector(0.0f, 0.0f, 1.0f);
    savedClip.d = 0.0f;

    if (view->m_flags & 0x200000) {
        C44Matrix viewMat = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
        C44Matrix nativeProj;
        GxXformProjNative(nativeProj);
        CShaderEffect::GetClipPlane(savedClip);
        C44Matrix inverse = nativeProj.Inverse(nativeProj.Determinant());
        C44Matrix transpose = inverse.Transpose();
        C4Plane plane;
        plane.n = view->m_cullPlane.n;
        plane.d = -fabsf(view->m_camPos.z);
        plane.Transform(viewMat);
        C4Plane clip;
        clip.n.x = transpose.a0 * plane.n.x + transpose.b0 * plane.n.y + transpose.c0 * plane.n.z + transpose.d0 * plane.d;
        clip.n.y = transpose.a1 * plane.n.x + transpose.b1 * plane.n.y + transpose.c1 * plane.n.z + transpose.d1 * plane.d;
        clip.n.z = transpose.a2 * plane.n.x + transpose.b2 * plane.n.y + transpose.c2 * plane.n.z + transpose.d2 * plane.d;
        clip.d = transpose.a3 * plane.n.x + transpose.b3 * plane.n.y + transpose.c3 * plane.n.z + transpose.d3 * plane.d;
        CShaderEffect::SetClipPlane(clip);
    }

    C44Matrix world;
    world.Translate(C3Vector(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_DepthFunc, 3);
    DefaultShaders::SetFogParams(-1000.0f, 1.0f, 1.0f, DayNight::GetFogInfo()->m_color);
    DefaultShaders::SetFogEnabled(1);
    GxRsSet(GxRs_Lighting, 0);
    CWorldScene::RenderLowDetailAreas(view);
    CWorldScene::RenderLowDetailMapObjDefs(view, world);
    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_World);

    if (view->m_flags & 0x200000) {
        CShaderEffect::SetClipPlane(savedClip);
    }

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    g_theGxDevicePtr->XformSetProjection(savedProjection);
}

// OFFSET: 0x0072A7D0
void CWorldScene::RenderChunks(CWorldView* view) {
    g_theGxDevicePtr->RsPush();
    CMapRenderChunk::s_savedFogColor = g_theGxDevicePtr->m_appRenderStates[GxRs_FogColor].m_value.m_data.i[0];

    if (view->m_flags & 0x8) {
        GxRsSet(GxRs_Culling, 2);
    }

    if (CMap::enableTerrainShaderVertex && CMap::enableTerrainShaderPixel) {
        CMapRenderChunk::InitializeVertexShaderConstants(view);
        CMapRenderChunk::RenderSetupPixelShader();
        CShadowCache::SetShadowMapTerrain();
    } else if (!CMap::enableTerrainShaderVertex && CMap::enableTerrainShaderPixel) {
        CMapRenderChunk::RenderSetupFixedFunction(view);
        CMapRenderChunk::RenderSetupPixelShader();
        CShadowCache::SetShadowMapTerrain();
    } else {
        CMapRenderChunk::RenderSetupFixedFunction(view);
        CMapRenderChunk::RenderSetupCombiners();
    }

    CWorldScene::RenderChunksLayered(view);
    CWorldScene::RenderChunksSolid(view);

    if (!CMap::enableTerrainShaderVertex) {
        for (uint32_t i = GxXform_Tex0; i <= GxXform_Tex4; i++) {
            g_theGxDevicePtr->m_xforms[i].Identity();
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0072AC10
void CWorldScene::RenderMapObjOpaque(CWorldView* view) {
    g_theGxDevicePtr->RsPush();
    view->FrustumPush();
    CShadowCache::SetShadowMapGenericGlobal();
    view->m_mapObjRender.BuildFrameMaterials(view);
    view->m_mapObjRender.RenderPass(view, 0);
    view->m_mapObjRender.RenderPass(view, 1);
    view->m_mapObjRender.RenderPass(view, 2);
    view->m_mapObjRender.RenderPass(view, 3);
    view->FrustumPop();
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0072AC80
void CWorldScene::RenderMapObjAlpha(CWorldView* view) {
    g_theGxDevicePtr->RsPush();
    view->FrustumPush();
    CShadowCache::SetShadowMapGenericGlobal();
    view->m_mapObjRender.RenderPass(view, 4);
    view->FrustumPop();
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0072C6B0
void CWorldScene::RenderAlphaWaterFog(CWorldView* view) {
    if (!CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex]) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    CWorldScene::RenderSetFog();

    if (Liquid::GetWaterDetail() && Liquid::CameraSubmerged()) {
        CWorldScene::RenderWaterFogPoly(view);
    }

    if (DepthCache::IsValid()) {
        g_theGxDevicePtr->RsSet(GxRs_Texture6, TextureGetGxTex(DepthCache::GetDepthSR()));
    }

    Liquid::RendererFlush(FrameTime::GetCurTimeMs(), view->m_camPos, 2);
    g_theGxDevicePtr->RsPop();
}
