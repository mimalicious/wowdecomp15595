#ifndef WOW_SOURCE_WORLDCLIENT_WORLDSCENE_H
#define WOW_SOURCE_WORLDCLIENT_WORLDSCENE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/WorldClient/World.h"

class CGxFrameBuffer;
class CGxShader;
class C44Matrix;
class CM2Lighting;
class CM2Model;
class CM2Scene;
class CMapEntity;
class CTexture;
class CWorldOcclusion;
class CWorldView;
class CPortalView;

class CRenderCallback {
    public:
    /* 0000 */ WORLD_RENDER_CALLBACK m_callback;
    /* 0004 */ void* m_param;
    /* 0008 */ uint64_t m_param64;
    /* 0010 */ TSLink<CRenderCallback> m_link;
};

class SObjectFadeout {
    public:
    /* 0000 */ CMapEntity* m_entity;
    /* 0004 */ uint32_t m_startTime;
    /* 0008 */ float m_alpha;
    /* 000C */ C44Matrix m_matrix;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint64_t m_param;
    /* 0058 */ WORLD_FADE_CALLBACK m_callback;
    /* 005C */ TSLink<SObjectFadeout> m_link;
    /* 0064 */ uint32_t unk_0064;
};

class CWorldSceneLight {
    public:
    /* 0000 */ CM2Light* m_source;
    /* 0004 */ float unk_0004;
    /* 0008 */ float m_fade;
    /* 000C */ C3Vector m_color;
    /* 0018 */ uint32_t m_fadeOut : 1;
    /* 0018 */ uint32_t m_fadeIn : 1;
    /* 0018 */ uint32_t m_free : 1;
    /* 001C */ CM2Light m_light;

    CWorldSceneLight();
};

class CWorldScene {
    public:
    static CWorldView* s_defaultWorldView;
    static CWorldView* s_defaultReflectionView;
    static CWorldView* s_defaultReflectionView2;
    static CWorldView* s_activeWorldView;
    static CM2Scene* s_m2Scene;
    static CGxShader* s_vsWaterFogPoly;
    static CGxShader* s_psWaterFogPoly;
    static CTexture* s_defaultTexture;
    static CTexture* s_defaultBlendTexture;
    static uintptr_t camTargEntity;
    static const char* renderSkyboxName;
    static C3Vector s_playerPos;
    static CM2Lighting s_lighting;
    static CWorldSceneLight s_sceneLights[3];
    static CBarrier s_barrier;
    static int32_t s_debugFrustumEnabled;
    static CFrustum s_debugFrustum;
    static TSExplicitList<CRenderCallback, offsetof(CRenderCallback, m_link)> s_renderCallbackList[3];
    static TSExplicitList<SObjectFadeout, offsetof(SObjectFadeout, m_link)> s_fadeoutList;
    static TSExplicitList<SObjectFadeout, offsetof(SObjectFadeout, m_link)> s_fadeoutFreeList;

    static void AddRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param, uint64_t param64);
    static void RemoveRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param);
    static void FreeSceneLights();
    static void RenderPortalList(const CPortalView* portalViews, uint32_t count, const C3Vector* verts, int32_t interior);
    static int32_t FadeoutUpdateCallback(void* param, uintptr_t flags, uint64_t param64, uint32_t param32);

    static void CallRenderCallbacks(uint32_t pass);
    static void ClearBarriers();
    static void ClearObjectFadeoutList();
    static void CreateClipVolumes(CWorldOcclusion* occlusion, const C3Vector& camPos, const C3Vector* corners, int32_t a4, float maxViewDepth);
    static void AddWorldOccluders(CWorldView* view);
    static void Destroy();
    static void FadeoutObject(CMapEntity* entity, float time, C44Matrix* matrix, WORLD_FADE_CALLBACK callback, uint64_t param);
    static void GetFrustumCorners(C3Vector* corners);
    static void GetFrustumCornersInWorldSpace(const C44Matrix& view, const C44Matrix& proj, const C3Vector& camPos, C3Vector* corners);
    static void Initialize();
    static CM2Model* LoadModel(const char* name, const C44Matrix& mat, void (*eventCallback)(CM2Model*, uint32_t, uint32_t, uint32_t, const C3Vector&, uint32_t, uint64_t), uint64_t eventParam, void (*lightingCallback)(CM2Model*, CM2Lighting*, void*), void* lightingParam, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
    static void OnMapUnload();
    static void Render(const C3Vector& pos, float elapsedSec);
    static void RenderAlphaWater(CWorldView* view);
    static void RenderAlphaWaterFog(CWorldView* view);
    static void RenderBarriers(CWorldView* view);
    static void RenderChunks(CWorldView* view);
    static void RenderChunksLayered(CWorldView* view);
    static void RenderChunksSolid(CWorldView* view);
    static void RenderClipVolumes(CWorldView* view);
    static void RenderDetailDoodads(CWorldView* view);
    static void RenderEntitys(CWorldView* view);
    static void RenderFrustum(CWorldView* view);
    static void RenderLowDetail(CWorldView* view);
    static void RenderLowDetailAreas(CWorldView* view);
    static void RenderLowDetailMapObjDefs(CWorldView* view, const C44Matrix& world);
    static void RenderMapObjAlpha(CWorldView* view);
    static void RenderMapObjOpaque(CWorldView* view);
    static void RenderOccluders(CWorldView* view);
    static void RenderPortalLists(CWorldView* view);
    static void RenderSetFog();
    static void RenderWaterFogPoly(CWorldView* view);
    static void RenderReflection(CWorldView* view, CGxFrameBuffer* frameBuffer, uint32_t mode, float height);
    static void __fastcall TextureRecreationCallback();
    static void Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect);
    static void UpdateFadeOutObjects();
    static void UpdateLiquid();
    static void RenderWeather();
    static void SetupSceneLighting();
    static void UpdateViewer();
};

#endif
