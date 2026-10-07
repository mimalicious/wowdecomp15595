#include "WorldScene.h"
#include <cmath>
#include <cstring>
#include <typeinfo>
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
#include "WoW/Source/WorldClient/shadow.h"

// STATIC: 0x00E70C48/0x140D5A768
CWorldView* CWorldScene::s_defaultWorldView;

// STATIC: 0x00E70C44
CWorldView* CWorldScene::s_defaultReflectionView;

// STATIC: 0x00E70C40
CWorldView* CWorldScene::s_defaultReflectionView2;

// STATIC: 0x00E70C34
CWorldView* CWorldScene::s_activeWorldView;

// STATIC: 0x00E70C4C/0x140D5A770
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

// STATIC: 0x00E70C54
int32_t CWorldScene::s_debugFrustumEnabled;

// STATIC: 0x00E70F40
CFrustum CWorldScene::s_debugFrustum;

// STATIC: 0x00E70C88
CBarrier CWorldScene::s_barrier;

// STATIC: 0x00E70C64
TSExplicitList<CRenderCallback, offsetof(CRenderCallback, m_link)> CWorldScene::s_renderCallbackList[3];

// STATIC: 0x00D00B28
TSExplicitList<SObjectFadeout, offsetof(SObjectFadeout, m_link)> CWorldScene::s_fadeoutList;

// STATIC: 0x00D00B34
TSExplicitList<SObjectFadeout, offsetof(SObjectFadeout, m_link)> CWorldScene::s_fadeoutFreeList;

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

// PROVEN: b9e544c9 IDENTICAL/NONE
// OFFSET: 0x00726BD0
void CWorldScene::FreeSceneLights() {
    CWorldScene::s_sceneLights[0].m_free = 1;
    CWorldScene::s_sceneLights[1].m_free = 1;
    CWorldScene::s_sceneLights[2].m_free = 1;
}

// OFFSET: 0x00726BF0
void CWorldScene::ClearBarriers() {
    CWorldScene::s_barrier.m_vertices.SetCount(0);
    CWorldScene::s_barrier.m_contacts.SetCount(0);
    CWorldScene::s_barrier.m_chunks.m_count = 0;
    CWorldScene::s_barrier.m_mapObjDefGroups.m_count = 0;
}

// PROVEN: 5b22a4ce IDENTICAL/NONE
// OFFSET: 0x00726F00/0x1403FE3C0
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

// PROVEN: e652f252 IDENTICAL/NONE
// OFFSET: 0x00727130/0x1403FE700
void CWorldScene::GetFrustumCorners(C3Vector* corners) {
    memcpy(corners, CWorldScene::s_defaultWorldView->m_frustumCorners, sizeof(CWorldScene::s_defaultWorldView->m_frustumCorners));
}

// PROVEN: 8835a74c IDENTICAL/NONE
// OFFSET: 0x00727150/0x1403FE720
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

// OFFSET: 0x007271C0/0x1403FE7A0
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
    for (CRenderCallback* callback = CWorldScene::s_renderCallbackList[pass].Head(); callback; callback = CWorldScene::s_renderCallbackList[pass].Next(callback)) {
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

// OFFSET: 0x007275D0
void CWorldScene::RenderWeather() {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (worldMap && worldMap->m_weather) {
        worldMap->m_weather->Render();
    }
}

// OFFSET: 0x007275F0/0x1403FEC80
void CWorldScene::RenderBarriers(CWorldView* view) {
    CWorldScene::s_barrier.Render(view->m_camPos, FrameTime::GetTickTimeSec());
}

// OFFSET: 0x00727620/0x1403FECB0
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

// OFFSET: 0x00727BE0
int32_t CWorldScene::FadeoutUpdateCallback(void* param, uintptr_t flags, uint64_t param64, uint32_t param32) {
    SObjectFadeout* fadeout = static_cast<SObjectFadeout*>(param);
    CM2Model* model = fadeout->m_entity->m_model;

    if (flags & 0x1) {
        model->SetAnimating(1);

        if (model->m_attachParent) {
            model->m_fFlags |= 0x40;
        } else {
            model->m_fFlags |= 0x8;
        }

        if (model->m_attachParent) {
            model->m_fFlags |= 0x10000;
        } else {
            model->m_fFlags |= 0x8000;
        }

        return 1;
    }

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x40;
    } else {
        model->m_fFlags &= ~0x8;
    }

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x10000;
    } else {
        model->m_fFlags &= ~0x8000;
    }

    model->SetAnimating(0);

    if (model->m_attachParent) {
        model->m_fFlags &= ~0x10000;
    } else {
        model->m_fFlags &= ~0x8000;
    }

    return 1;
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

// OFFSET: 0x00728600/0x140400730
void CWorldScene::UpdateFadeOutObjects() {
    uint32_t now = FrameTime::GetCurTimeMs();

    for (SObjectFadeout* fadeout = CWorldScene::s_fadeoutList.Head(), * next; fadeout; fadeout = next) {
        uint32_t elapsed = now - fadeout->m_startTime;
        CM2Model* model = fadeout->m_entity->m_model;
        next = CWorldScene::s_fadeoutList.Next(fadeout);

        if (elapsed <= 2000 && model->CanUpdateNonReadyTextures()) {
            if (fadeout->m_callback) {
                C44Matrix matrix;
                fadeout->m_callback(matrix, fadeout->m_param);
                C44Matrix transform = fadeout->m_matrix * matrix;
                model->m_fFlags |= 0x4000;
                model->m_worldTransform = transform;
            }

            float alpha = (1.0f - static_cast<float>(elapsed) * 0.00050000002f) * fadeout->m_alpha;

            if (alpha >= 0.0f) {
                float value = 1.0f;

                if (alpha <= 1.0f) {
                    value = (3.0f - alpha * 2.0f) * alpha * alpha;
                }

                model->m_fadeAlpha = value;
            } else {
                model->m_fadeAlpha = 0.0f;
            }
        } else {
            World::ObjectDestroy(reinterpret_cast<uintptr_t>(fadeout->m_entity));
            fadeout->m_entity = NULL;
            fadeout->m_link.Unlink();
            CWorldScene::s_fadeoutFreeList.LinkToTail(fadeout);
        }
    }
}

// OFFSET: 0x00728810
void CWorldScene::ClearObjectFadeoutList() {
    SObjectFadeout* entry = CWorldScene::s_fadeoutList.Head();

    while (entry) {
        SObjectFadeout* next = CWorldScene::s_fadeoutList.Next(entry);
        World::ObjectDestroy(reinterpret_cast<uintptr_t>(entry->m_entity));
        entry->m_entity = NULL;
        entry->m_link.Unlink();
        CWorldScene::s_fadeoutFreeList.LinkToTail(entry);
        entry = next;
    }
}

// PROVEN: 6a1869f8 IDENTICAL/NONE
// OFFSET: 0x007288F0
void __fastcall CWorldScene::TextureRecreationCallback() {
    CWorldScene::ClearObjectFadeoutList();
}

// OFFSET: 0x00728900/0x140400BD0
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

// OFFSET: 0x00728A20/0x140400D30
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

// OFFSET: 0x00728AE0/0x140400E40
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

// OFFSET: 0x00728ED0
void CWorldScene::RenderEntitys(CWorldView* view) {
    int32_t blobShadows = ShadowBlobsEnabled();

    for (CMapEntity* entity = view->m_list2004.Head(), * next; entity; entity = next) {
        next = view->m_list2004.Next(entity);
        entity->m_viewLink.Unlink();
        CM2Model* model = entity->m_model;

        if (!model) {
            continue;
        }

        if (model->IsLoadedRecursive()) {
            view->CullModelTree(entity->m_model, entity->unk_00BC);
        }

        if (entity->m_model->IsLoaded() && World::s_entityCollisionCallback) {
            World::s_entityCollisionCallback(entity->m_param64, entity->m_param32, entity->m_model, blobShadows & (entity->m_entityFlags >> 9) & 1);
        }
    }
}

// OFFSET: 0x00728FE0
void CWorldScene::FadeoutObject(CMapEntity* entity, float time, C44Matrix* matrix, WORLD_FADE_CALLBACK callback, uint64_t param) {
    SObjectFadeout* fadeout = CWorldScene::s_fadeoutFreeList.Head();

    if (!fadeout) {
        fadeout = new (SMemAlloc(sizeof(SObjectFadeout), typeid(SObjectFadeout).raw_name(), SLOG_OBJECT, SMEM_FLAG_ZEROMEMORY)) SObjectFadeout;
    }

    fadeout->m_entity = entity;
    fadeout->m_startTime = FrameTime::GetCurTimeMs();
    fadeout->m_callback = callback;
    fadeout->m_alpha = time;
    fadeout->m_param = param;

    if (matrix) {
        fadeout->m_matrix = entity->m_model->m_worldTransform * *matrix;
    }

    entity->m_updateCallback = CWorldScene::FadeoutUpdateCallback;
    entity->m_updateParam = fadeout;
    CWorldScene::s_fadeoutList.LinkToTail(fadeout);
}

// OFFSET: 0x00729100
void CWorldScene::AddRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param, uint64_t param64) {
    TSExplicitList<CRenderCallback, offsetof(CRenderCallback, m_link)>& list = CWorldScene::s_renderCallbackList[pass];

    for (CRenderCallback* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_callback == callback && node->m_param == param) {
            return;
        }
    }

    CRenderCallback* node = list.NewNode(2, 0, 0);
    node->m_callback = callback;
    node->m_param = param;
    node->m_param64 = param64;
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

// OFFSET: 0x00729850
void CWorldScene::RemoveRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param) {
    TSExplicitList<CRenderCallback, offsetof(CRenderCallback, m_link)>& list = CWorldScene::s_renderCallbackList[pass];

    for (CRenderCallback* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_callback == callback && node->m_param == param) {
            list.DeleteNode(node);
            return;
        }
    }
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

// OFFSET: 0x00729920
void CWorldScene::RenderWaterFogPoly(CWorldView* view) {
    C3Vector camPos = view->m_camPos;
    const C3Vector* corners = view->m_lowDetailFrustum.corners;

    if (corners[4].z > 0.0f && corners[7].z > 0.0f) {
        return;
    }

    C3Vector c5 = corners[5];
    C3Vector d = camPos - c5;
    float inv = 1.0f / sqrt(d.y * d.y + d.z * d.z + d.x * d.x);
    c5.z = d.z * inv + c5.z;
    c5.y = d.y * inv + c5.y;
    c5.x = d.x * inv + c5.x;

    C3Vector c6 = corners[6];
    inv = 1.0f / sqrt((camPos.z - c6.z) * (camPos.z - c6.z) + (camPos.y - c6.y) * (camPos.y - c6.y) + (camPos.x - c6.x) * (camPos.x - c6.x));
    c6.z = (camPos.z - c6.z) * inv + c6.z;
    c6.y = (camPos.y - c6.y) * inv + c6.y;
    c6.x = (camPos.x - c6.x) * inv + c6.x;

    C3Vector c4 = corners[4];
    inv = 1.0f / sqrt((camPos.z - c4.z) * (camPos.z - c4.z) + (camPos.y - c4.y) * (camPos.y - c4.y) + (camPos.x - c4.x) * (camPos.x - c4.x));
    c4.z = (camPos.z - c4.z) * inv + c4.z;
    c4.y = (camPos.y - c4.y) * inv + c4.y;
    c4.x = (camPos.x - c4.x) * inv + c4.x;

    C3Vector c7 = corners[7];
    d = camPos - c7;
    inv = 1.0f / sqrt(d.z * d.z + d.y * d.y + d.x * d.x);
    c7.x = d.x * inv + c7.x;
    c7.y = d.y * inv + c7.y;
    c7.z = d.z * inv + c7.z;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_ScissorTest, 0);
    GxRsSet(GxRs_BlendingMode, 0);
    GxRsSet(GxRs_DepthWrite, 0);

    C44Matrix identity;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, identity);

    if (CWorldScene::s_vsWaterFogPoly) {
        GxRsSet(GxRs_VertexShader, CWorldScene::s_vsWaterFogPoly, 0);
        GxRsSet(GxRs_PixelShader, CWorldScene::s_psWaterFogPoly, 0);

        C44Matrix viewMatrix = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
        C44Matrix proj;
        GxXformProjNative(proj);
        C44Matrix transform = viewMatrix * proj;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    }

    float fogStart;
    float fogEnd;
    float fogDensity;
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0;
    Liquid::GetUnderWaterFog(fogStart, fogEnd, fogDensity, color);

    C3Vector verts[4];
    verts[0] = c5 - camPos;
    verts[1] = c4 - camPos;
    verts[2] = c6 - camPos;
    verts[3] = c7 - camPos;
    GxPrimVertexPtr(4, verts, sizeof(C3Vector), NULL, 0, &color, 0, NULL, 0, NULL, 0);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = 4;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 3;
    g_theGxDevicePtr->DrawNonIndexed(batch);

    GxXformPop(GxXform_World);
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

// PROVEN: be9df0ad IDENTICAL/NONE
// OFFSET: 0x0072AC80
void CWorldScene::RenderMapObjAlpha(CWorldView* view) {
    g_theGxDevicePtr->RsPush();
    view->FrustumPush();
    CShadowCache::SetShadowMapGenericGlobal();
    view->m_mapObjRender.RenderPass(view, 4);
    view->FrustumPop();
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0072ACC0/0x140403C90
void CWorldScene::RenderPortalList(const CPortalView* portalViews, uint32_t count, const C3Vector* verts, int32_t interior) {
    if (!count) {
        return;
    }

    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix savedProjection;
    C44Matrix savedView;
    C44Matrix savedWorld;
    savedProjection = device->m_projection;
    savedView = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
    savedWorld = device->m_xforms[GxXform_World].m_mtx[device->m_xforms[GxXform_World].m_level];

    C44Matrix identity;
    device->XformSetView(identity);
    g_theGxDevicePtr->XformSetProjection(identity);
    GxXformSet(GxXform_World, identity);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthWrite, 1);
    GxRsSet(GxRs_DepthTest, 1);

    const CImVector* fogColor = interior ? &DayNight::GetIntFogInfo()->m_color : &DayNight::GetFogInfo()->m_color;
    CImVector color;

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        color.b = fogColor->r;
        color.g = fogColor->g;
        color.r = fogColor->b;
        color.a = fogColor->a;
    } else {
        color = *fogColor;
    }

    GxRsSet(GxRs_BlendingMode, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    DefaultShaders::SetFogEnabled(0);

    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint16_t base = 0;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t viewVerts = portalViews[i].m_vertCount;

        if (viewVerts) {
            vertexCount += viewVerts;
            indexCount = viewVerts + indexCount - 2 + 2 * viewVerts - 4;
        } else {
            vertexCount += 4;
            indexCount += 6;
        }
    }

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 16, vertexCount);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));

    if (vertices && indices) {
        for (uint32_t i = 0; i < count; i++) {
            const CPortalView& view = portalViews[i];

            if (view.m_vertCount) {
                const C3Vector* vert = &verts[view.m_vertStart];

                for (uint32_t k = 0; k < view.m_vertCount; k++) {
                    vertices[0] = vert->x;
                    vertices[1] = vert->y;
                    vertices[2] = 1.0f;
                    *reinterpret_cast<CImVector*>(&vertices[3]) = color;
                    vertices += 4;
                    vert++;
                }

                for (uint32_t k = 0; k < view.m_vertCount - 2; k++) {
                    indices[1] = static_cast<uint16_t>(base + k + 1);
                    indices[2] = static_cast<uint16_t>(base + k + 2);
                    indices[0] = base;
                    indices += 3;
                }

                base = static_cast<uint16_t>(base + view.m_vertCount);
            } else {
                float x0 = view.m_rect.minY * 2.0f - 1.0f;
                float y0 = view.m_rect.minX * 2.0f - 1.0f;
                float y1 = view.m_rect.maxX * 2.0f - 1.0f;
                float x1 = view.m_rect.maxY * 2.0f - 1.0f;

                vertices[0] = x0;
                vertices[1] = y0;
                vertices[2] = 1.0f;
                *reinterpret_cast<CImVector*>(&vertices[3]) = color;
                vertices[4] = x0;
                vertices[5] = y1;
                vertices[6] = 1.0f;
                *reinterpret_cast<CImVector*>(&vertices[7]) = color;
                vertices[8] = x1;
                vertices[9] = y1;
                vertices[10] = 1.0f;
                *reinterpret_cast<CImVector*>(&vertices[11]) = color;
                vertices[12] = x1;
                vertices[13] = y0;
                vertices[14] = 1.0f;
                *reinterpret_cast<CImVector*>(&vertices[15]) = color;

                indices[1] = static_cast<uint16_t>(base + 1);
                indices[2] = static_cast<uint16_t>(base + 2);
                indices[4] = static_cast<uint16_t>(base + 2);
                indices[0] = base;
                indices[3] = base;
                indices[5] = static_cast<uint16_t>(base + 3);
                vertices += 16;
                indices += 6;
                base = static_cast<uint16_t>(base + 4);
            }
        }
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PC);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, static_cast<uint32_t>(-1));

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount - 1);
    g_theGxDevicePtr->Draw(batch);
    g_theGxDevicePtr->RsPop();

    GxXformSet(GxXform_World, savedWorld);
    g_theGxDevicePtr->XformSetProjection(savedProjection);
    g_theGxDevicePtr->XformSetView(savedView);
}

// OFFSET: 0x0072B4D0
void CWorldScene::RenderOccluders(CWorldView* view) {
    CWorldOccluder* occluder = view->m_occluders.Head();

    if (!occluder) {
        return;
    }

    int32_t count = 0;

    for (CWorldOccluder* o = occluder; o; o = view->m_occluders.Next(o)) {
        count++;
    }

    C44Matrix world;
    world.Translate(C3Vector(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);

    uint32_t vertexCount = 4 * count;
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 16, vertexCount);
    uint32_t* vertices = reinterpret_cast<uint32_t*>(g_theGxDevicePtr->BufLock(vertexBuf));

    do {
        CWorldOccluder* next = view->m_occluders.Next(occluder);
        float* v = reinterpret_cast<float*>(vertices);
        v[0] = occluder->m_start.x;
        v[1] = occluder->m_start.y;
        v[2] = occluder->m_start.z;
        vertices[3] = 0x90002020;
        v[4] = occluder->m_end.x;
        v[5] = occluder->m_end.y;
        v[6] = occluder->m_end.z;
        vertices[7] = 0x90002020;
        v[8] = occluder->m_start.x;
        v[9] = occluder->m_start.y;
        v[10] = occluder->m_start.z - 100.0f;
        vertices[11] = 0x90002020;
        v[12] = occluder->m_end.x;
        v[13] = occluder->m_end.y;
        v[14] = occluder->m_end.z - 100.0f;
        vertices[15] = 0x90002020;
        vertices += 16;
        occluder->Free();
        occluder = next;
    } while (occluder);

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PC);

    uint32_t indexCount = 6 * count;
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));

    for (int32_t i = 0; i < count; i++) {
        uint16_t b = static_cast<uint16_t>(4 * i);
        indices[0] = b;
        indices[1] = b + 1;
        indices[2] = b + 2;
        indices[3] = b + 1;
        indices[4] = b + 3;
        indices[5] = b + 2;
        indices += 6;
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, static_cast<uint32_t>(-1));

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = static_cast<uint16_t>(vertexCount);
    g_theGxDevicePtr->Draw(batch);
    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_World);
}

// OFFSET: 0x0072B8F0
void CWorldScene::RenderFrustum(CWorldView* view) {
    if (!CWorldScene::s_debugFrustumEnabled) {
        return;
    }

    static const uint16_t s_faceCorners[24] = {
        1, 2, 5, 6,
        0, 3, 4, 7,
        0, 1, 4, 5,
        2, 3, 6, 7,
        5, 6, 4, 7,
        1, 2, 0, 3,
    };

    CImVector faceColors[6];
    faceColors[0] = CImVector(0x00, 0x00, 0xFF, 0x7F);
    faceColors[1] = CImVector(0x00, 0xFF, 0x00, 0x7F);
    faceColors[2] = CImVector(0xFF, 0x00, 0x00, 0x7F);
    faceColors[3] = CImVector(0xFF, 0x00, 0xFF, 0x7F);
    faceColors[4] = CImVector(0x00, 0xFF, 0xFF, 0x7F);
    faceColors[5] = CImVector(0xFF, 0xFF, 0x00, 0x7F);

    C44Matrix world;
    world.Translate(C3Vector(-view->m_camPos.x, -view->m_camPos.y, -view->m_camPos.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_MatDiffuse, static_cast<int32_t>(0x90002020));
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    DefaultShaders::SetFogEnabled(0);

    const C3Vector* corners = CWorldScene::s_debugFrustum.corners;
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 16, 24);
    float* vertices = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(vertexBuf));

    for (uint32_t face = 0; face < 6; face++) {
        const uint16_t* indices = &s_faceCorners[face * 4];

        for (uint32_t k = 0; k < 4; k++) {
            const C3Vector& corner = corners[indices[k]];
            vertices[0] = corner.x;
            vertices[1] = corner.y;
            vertices[2] = corner.z;
            *reinterpret_cast<CImVector*>(&vertices[3]) = faceColors[face];
            vertices += 4;
        }
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PC);

    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 36);
    uint16_t* faceIndices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));

    for (uint32_t i = 0; i < 6; i++) {
        faceIndices[1] = static_cast<uint16_t>(4 * i + 1);
        faceIndices[2] = static_cast<uint16_t>(4 * i + 2);
        faceIndices[3] = static_cast<uint16_t>(4 * i + 1);
        faceIndices[0] = static_cast<uint16_t>(4 * i);
        faceIndices[4] = static_cast<uint16_t>(4 * i + 2);
        faceIndices[5] = static_cast<uint16_t>(4 * i + 3);
        faceIndices += 6;
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, static_cast<uint32_t>(-1));

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = 36;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 24;
    g_theGxDevicePtr->Draw(batch);

    CGxBuf* lineBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, 16, 12);
    float* lines = reinterpret_cast<float*>(g_theGxDevicePtr->BufLock(lineBuf));

    for (uint32_t face = 0; face < 6; face++) {
        const uint16_t* indices = &s_faceCorners[face * 4];
        const C3Vector& n = CWorldScene::s_debugFrustum.planes[face].n;
        C3Vector center;
        center.x = (corners[indices[0]].x + corners[indices[1]].x + corners[indices[2]].x + corners[indices[3]].x) * 0.25f;
        center.y = (corners[indices[0]].y + corners[indices[1]].y + corners[indices[2]].y + corners[indices[3]].y) * 0.25f;
        center.z = (corners[indices[0]].z + corners[indices[1]].z + corners[indices[2]].z + corners[indices[3]].z) * 0.25f;

        lines[0] = center.x;
        lines[1] = center.y;
        lines[2] = center.z;
        reinterpret_cast<uint32_t*>(lines)[3] = 0xFFFFFFFF;
        lines[4] = n.x * 10.0f + center.x;
        lines[5] = n.y * 10.0f + center.y;
        lines[6] = n.z * 10.0f + center.z;
        reinterpret_cast<uint32_t*>(lines)[7] = 0xFFFFFFFF;
        lines += 8;
    }

    g_theGxDevicePtr->BufUnlock(lineBuf, 0);
    lineBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(lineBuf, GxVBF_PC);

    CGxBuf* lineIndexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 12);
    uint16_t* lineIndices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(lineIndexBuf));

    for (uint32_t i = 0; i < 6; i++) {
        lineIndices[0] = static_cast<uint16_t>(2 * i);
        lineIndices[1] = static_cast<uint16_t>(2 * i + 1);
        lineIndices += 2;
    }

    g_theGxDevicePtr->BufUnlock(lineIndexBuf, 0);
    lineIndexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(lineIndexBuf);
    DefaultShaders::SetDefaultShaders(GxVertexBufferFormats_Last, static_cast<uint32_t>(-1));

    batch.m_primType = GxPrim_Lines;
    batch.m_start = 0;
    batch.m_count = 12;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 12;
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_World);
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

// PROVEN: 1affac43 IDENTICAL/NONE
// OFFSET: 0x0072C740/0x140405890
void CWorldScene::RenderPortalLists(CWorldView* view) {
    if (view->m_exteriorPortalViews.Count()) {
        if (Liquid::GetWaterDetail() <= 1 && view->m_underwater) {
            CWorldScene::RenderPortalList(view->m_exteriorPortalViews.m_data, view->m_exteriorPortalViews.Count(), view->m_portalVerts.m_data, 0);
        }

        view->CreateInverseExteriorPortalList();
        CWorldScene::RenderPortalList(view->m_exteriorPortalViews.m_data, view->m_exteriorPortalViews.Count(), view->m_portalVerts.m_data, 1);
    }

    if (view->m_portalViews.Count()) {
        CWorldScene::RenderPortalList(view->m_portalViews.m_data, view->m_portalViews.Count(), view->m_portalVerts.m_data, 0);
    }
}
