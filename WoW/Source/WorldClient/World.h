#ifndef WOW_SOURCE_WORLDCLIENT_WORLD_H
#define WOW_SOURCE_WORLDCLIENT_WORLD_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"

class C2Vector;
class C3Spline_CatmullRom;
class C4Plane;
class CAaSphere;
struct C3Segment;
class CFrustum;
class CM2Model;
class CM2Scene;
class CTexture;
class WMOAreaTableRec;
class WeatherRec;

typedef int32_t (*WORLD_LOAD_CALLBACK)(void* param, uintptr_t object, uint64_t param64, uint32_t param32);
typedef void (*WORLD_PROGRESS_CALLBACK)(float progress, void* param);
typedef void (*WORLD_MODEL_EVENT_CALLBACK)(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time, uint64_t param);
typedef void (*WORLD_SEQUENCE_CALLBACK)(CM2Model* model, uint32_t a2, uint32_t a3, int32_t a4, uint32_t a5, uint64_t param, int32_t a7);
typedef void (*WORLD_RENDER_CALLBACK)(void* param, uint32_t pass, uint64_t param64);
typedef void (*WORLD_FADE_CALLBACK)(C44Matrix& matrix, uint64_t param);
typedef void (*WORLD_FRAME_CALLBACK)();

class LiquidTypeRec;

class WMOAreaTableRec;

const LiquidTypeRec* GetLiquidTypeRecForArea(uint32_t areaId, uint32_t liquidType);
const WMOAreaTableRec* SDBWMOAreaTableLookup(uint32_t wmoId, uint32_t nameSet, uint32_t groupId);

namespace World {
    struct HitTestDebug;
    struct LiquidQueryResult;
    struct MinimapQuad;
    struct ObjectCollisionData;
    struct TriData;

    typedef int32_t (*COLLISION_CALLBACK)(uint64_t param64, uint32_t param32, CM2Model* model, int32_t blobShadow);

    struct MinimapQuad {
        /* 0000 */ uint32_t m_groupNum;
        /* 0004 */ int32_t m_x;
        /* 0008 */ int32_t m_y;
        /* 000C */ CAaBox m_box;

        MinimapQuad();
    };

    struct FacetData {
        /* 0000 */ TSGrowableArray<CFacet> facets;
        /* 0010 */ TSGrowableArray<uint64_t> facetIds;
        /* 0020 */ TSGrowableArray<uint8_t> facetFlags;
        /* 0030 */ uint32_t m_status;

        FacetData();
    };

    struct CLIPINFO {
        /* 0000 */ float m_dist[6];
        /* 0018 */ uint32_t m_flags;
        /* 001C */ uint32_t unk_001C;

        void Set(const C3Vector& pos);
    };
    typedef int32_t (*RENDER_OBJECT_CALLBACK)(uint64_t guid, uintptr_t object, CM2Model* model, int32_t a4);

    extern uint64_t s_camTargetGuid;
    extern C44Matrix s_camTargetMatrix;
    extern C44Matrix s_camTargetView;
    extern C3Vector* s_ndcClipVertsA[32];
    extern C3Vector* s_ndcClipVertsB[32];
    extern WORLD_PROGRESS_CALLBACK s_loadProgressCallback;
    extern void* s_loadProgressParam;
    extern WORLD_MODEL_EVENT_CALLBACK s_userModelEventCallback;
    extern COLLISION_CALLBACK s_entityCollisionCallback;
    extern RENDER_OBJECT_CALLBACK s_entityRenderCallback;
    extern WORLD_FRAME_CALLBACK s_frameCallback;
    extern float s_frameTimes[30];
    extern uint32_t s_frameTimeIndex;
    extern uint64_t s_lastCollisionGUID;
    extern uint64_t s_trackedMapObjGUID;
    extern uint32_t frameCnt;
    extern uint32_t s_enables;
    extern uint32_t s_enablesMask;
    extern uint32_t s_m2PassEnables;
    extern uint32_t terrainAlphaBitDepth;
    extern uint32_t enableLayerCnt;
    extern int32_t prepareAll;
    extern uint32_t worldPoolUsage;
    extern uint32_t alphaShadowMipLevel;
    extern C4Vector texVect[8];
    extern C44Matrix idMat;
    extern uint32_t shadowColor;
    extern CImVector shadowModColor[64];
    extern CTexture* shadowModTexture;
    extern uint32_t detailDoodadAlphaRef;
    extern float detailDoodadDist;
    extern float detailDoodadDistSqr;
    extern float detailDoodadDensityFade;
    extern uint32_t detailDoodadDensity;
    extern int32_t detailDoodadTest;
    extern float s_farClip;
    extern float prevFarClip;
    extern float s_nearClip;
    extern float s_farClipOverride;
    extern float s_horizonFarClipScale;
    extern float s_horizonNearClipScale;
    extern float fCharacterAmbMul;
    extern uint32_t s_texCacheOverride;
    extern int32_t lodMax;
    extern int32_t lodMin;
    extern C3Vector gbPosition;
    extern C3Vector gbPositionOld;
    extern float gbAoiDist;
    extern float objectAoiDist;
    extern int32_t gbAreaIndex[2];
    extern CiRect chunkRect;
    extern CiRect gbChunkRect;
    extern CiRect gbChunkRectOld;
    extern CiRect detailDoodadChunkRect;
    extern CiRect detailDoodadChunkRectOld;
    extern CAaBox mapObjDefBounds;
    extern CAaBox mapObjDefBoundsGuard;
    extern C3Vector s_weatherVelocity;
    extern C3Vector s_weatherPosition;
    extern float s_weatherFacing;
    extern int32_t s_weatherOnTaxi;
    extern const C3Spline_CatmullRom* s_weatherSpline;
    extern float s_weatherSplineElapsedTime;
    extern float s_weatherSplineDuration;
    extern int32_t s_loadFailed;
    extern char s_loadFailedName[256];

    void SetLoadProgressCallback(WORLD_PROGRESS_CALLBACK callback, void* param);
    void SetUserModelEventCallback(WORLD_MODEL_EVENT_CALLBACK callback);
    CM2Scene* GetScene();
    void SetActivePlayerPos(const C3Vector& pos);
    int32_t QueryMapId();
    void SetObjectCollisionCallback(COLLISION_CALLBACK callback);
    void SetObjectRenderCallback(RENDER_OBJECT_CALLBACK callback);
    void ObjectSetUpdateCallback(uintptr_t object, WORLD_LOAD_CALLBACK callback, void* param);
    void ObjectSetShadow(uintptr_t object, int32_t enable);
    int32_t ObjectIsInvisibleStalker(uintptr_t object);
    uint32_t GetFrameCnt();
    float GetFramerate();
    const C3Vector& GetCamPos();
    void SetHorizonFarClipScale(float scale);
    void SetHorizonNearClipScale(float scale);
    void ClearLastCollisionGUID();
    void SetLastCollisionGUID(uint64_t guid);
    uint64_t GetLastCollisionGUID();
    void ClearTrackedMapObjGUID();
    void SetTrackedMapObjGUID(uint64_t guid);
    uint64_t GetTrackedMapObjGUID();
    uint32_t GetEnables();
    void SetEnables(uint32_t enables, int32_t enable);
    uint32_t GetM2PassEnables();
    float GetFarClip();
    float GetNearClip();
    float GetHorizonFarClip();
    void SetFarClipOverride(float farClip);
    void SetFarClipOverrideEnabled(int32_t enable);
    void SetUnderwaterViewEnabled(int32_t enable);
    const C3Vector& GetGuardBandPosition();
    float GetGuardBandAoiDist();
    float GetObjectAoiDist();
    float GetCharacterAmbMul();
    uint32_t GetPoolUsage();
    WORLD_MODEL_EVENT_CALLBACK GetUserModelEventCallback();
    void SetObjectAmbientMultiplier(float mul);
    int32_t GetLoadFailed();
    void ClearLoadFailed();
    const C3Vector& GetWeatherPosition();
    const C3Vector& GetWeatherVelocity();
    float GetWeatherFacing();
    int32_t GetWeatherOnTaxi();
    const C3Spline_CatmullRom* GetWeatherSpline();
    float GetWeatherSplineElapsedTime();
    float GetWeatherSplineDuration();
    float GetMaxTeleport();
    float GetWaterEpsilon();
    void SetLoadFailed(int32_t failed, const char* name);
    void InitializeShaderEffects();
    void OnShaderModelChanged(void* param);
    int32_t ProjectPositionCallback(const C3Vector& pos, float& z, void* param);
    void ProjectTex2d(const CAaBox& box, const CImVector& color, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset);
    void ShadowRender(const C3Vector& position, float facing, float scale, const CAaBox& box, float alpha);
    void ProjectTextureCallback(const CAaBox& box, const CImVector& color, int32_t a3, void* param, int32_t a5);
    uint32_t ValidateFarClip(float farClip);
    void SetDetailDoodadDist(float dist);
    void SetDetailDoodadDensityFade(float fade);
    void SetDetailDoodadDensity(uint32_t density);
    void SetShadowColor(const CImVector& color);
    void GetFrustumCorners(C3Vector* corners);
    bool Intersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist, uint32_t flags, HitTestDebug* debug);
    void ObjectDestroy(uintptr_t object);
    void FadeoutObject(uintptr_t object, float time, C44Matrix* matrix, WORLD_FADE_CALLBACK callback, uint64_t param);
    int32_t QueryOutdoors(uintptr_t object);
    void WaterReset();
    void Destroy();
    int32_t QueryAreaId(uintptr_t object, uint32_t* areaIds);
    int32_t ObjectIsWMO(uintptr_t object);
    void ObjectEnablePortals(uintptr_t object, int32_t enable);
    void ObjectSetEventCallback(uintptr_t object, WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    void ObjectSetSequenceFinishedCallback(uintptr_t object, WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    void ObjectSetSequence(uintptr_t object, uint32_t sequence, int32_t a3, uint16_t doodadSet);
    void ObjectSetSequenceSpeed(uintptr_t object, float speed, int32_t a3, uint16_t doodadSet);
    void ObjectSetDoodadsEnabled(uintptr_t object, int32_t enable, uint16_t doodadSet);
    void ObjectSetDoodadEmittersIgnoresDistance(uintptr_t object, int32_t enable, uint16_t doodadSet);
    void ObjectSetLinkAll(uintptr_t object, int32_t enable);
    int32_t ObjectIsLoaded(uintptr_t object);
    int32_t ObjectIsDrawable(uintptr_t object);
    int32_t QueryGroundType(uintptr_t object, uint32_t& groundType);
    int32_t QueryObjectLiquidFlags(uintptr_t object, int32_t& a2, int32_t& a3);
    int32_t QueryObjectLiquid(uintptr_t object, uint32_t& liquidType, float& height, int32_t& a4);
    int32_t QueryMapObjMinimap(uintptr_t object, const CAaBox& box, TSStackArray<MinimapQuad>& quads, uint32_t maxQuads, bool a5, bool a6);
    int32_t QueryMapObjAreaTable(uintptr_t object, const WMOAreaTableRec*& groupRec, const WMOAreaTableRec*& rootRec, uint32_t& wmoGroupId, int32_t a5);
    int32_t QueryMapObjMatrix(uintptr_t object, C44Matrix* matrix, C44Matrix* invMatrix);
    int32_t QueryMapObjIDs(uintptr_t object, uint32_t& wmoId, uint32_t& nameSet, uint32_t& groupId);
    int32_t QueryMapObjFileName(uintptr_t object, const char*& fileName);
    int32_t QueryMapObjSubzoneName(uintptr_t object, const char*& name, uint32_t& areaId);
    int32_t QueryMapObjZoneName(uintptr_t object, const char*& name);
    int32_t QueryObjectVisible(uintptr_t object);
    int32_t QueryObjectInside(uintptr_t object, int32_t a2);
    uint64_t QueryObjectParentParam64(uintptr_t object);
    int32_t GetFlightBoundsLower(const C3Vector& pos, float& z);
    int32_t GetFlightBoundsUpper(const C3Vector& pos, float& z);
    void CreateWaterRipple(const C3Vector& pos, float a2, float a3, float a4, float a5, float a6, int32_t a7, int32_t a8);
    int32_t QueryLiquidSounds(uintptr_t object, float radius, TSGrowableArray<LiquidQueryResult>& results);
    int32_t QueryLiquidNearest(const C3Vector& pos, uint32_t& liquidType, float& height);
    int32_t QueryLiquid(const C3Vector& pos, uint32_t& liquidType, float& height);
    int32_t GetIntersectingDoodadModels(CM2Model** models, uint32_t maxModels, const CAaBox& box, uint32_t flags);
    void GetTris(const CAaBox& box, TriData& tris, uint32_t flags);
    int32_t GetFacets(const CFrustum& frustum, FacetData& facets, uint32_t flags);
    int32_t IsFacetQueryValid();
    uintptr_t ObjectCreate(CM2Model* model, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a9);
    uintptr_t ObjectCreate(const char* name, C3Vector& pos, float facing, int32_t a4, int32_t snap, uint64_t param64, uint32_t param32, int32_t a8, uint16_t* a9, float* a10, int32_t a11);
    uintptr_t ObjectCreate(CM2Model* model, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a8);
    void SetWeather(int32_t type, float intensity, const WeatherRec* rec, bool a4, float a5);
    void NewFootprint(uint32_t a1, const C2Vector& a2, const C3Vector& a3, float a4, int32_t a5, uint32_t a6, int32_t a7);
    int32_t IsInfiniteOcean();
    bool MapIsDungeon();
    int32_t GetActiveMapLogical(int32_t a1, int32_t a2);
    int32_t SetMapActive(int32_t mapId, int32_t a2);
    int32_t AddMapById(int32_t mapId, int32_t a2);
    int32_t RemoveMapById(int32_t mapId);
    void ObjectSetMoveable(uintptr_t object, int32_t enable);
    void ObjectSetAlwaysExterior(uintptr_t object, int32_t enable);
    void ObjectSetAcceptProjTex(uintptr_t object, int32_t enable);
    void ObjectSetHidden(uintptr_t object, int32_t enable);
    uint32_t ValidateDetailDoodadDist(uint32_t dist);
    uint32_t ValidateDetailDoodadDensity(uint32_t density);
    float GetFarClipForMap(float farClip, int32_t mapId);
    void SetFarClip(float farClip);
    float GetCamDepth();
    const C3Vector* GetCamFrustumCorners();
    const C3Vector& GetCamTarget();
    void SetNearClip(float nearClip);
    void Initialize();
    void ObjectSetModel(uintptr_t object, CM2Model* model);
    void ObjectSetRenderEnabled(uintptr_t object, int32_t enable);
    int32_t ObjectMoveDoodads(uintptr_t object, uint16_t doodadSet, uintptr_t target, uint16_t targetDoodadSet);
    int32_t FrustumCull(const CAaBox& box);
    void UnloadMap();
    int32_t ObjectTestConvexVolume(uintptr_t object, const C3Vector& pos, float radius);
    void ObjectGetExtents(uintptr_t object, CAaBox& box);
    void Render(const C3Vector& pos, float elapsedSec);
    void UpdateDayNight();
    void TriDataToFacetData(const TriData& tris, FacetData& facets, uint64_t guid, uint8_t flags);
    void PrepareAreaOfInterest(const C3Vector& pos, float nearClip, float farClip, float fov, float aspect);
    void PrepareAreaOfInterest(const C3Vector& pos);
    void UpdateTextureCacheBudget();
    void Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect, const C3Vector& camUp, const C3Vector& weatherVelocity, const C3Vector& weatherPosition, float weatherFacing, int32_t weatherOnTaxi, const C3Spline_CatmullRom* weatherSpline, float splineElapsedTime, float splineDuration);
    void ObjectEnableCollision(uintptr_t object, int32_t enable);
    void ObjectUpdate(uintptr_t object, const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& pos, int32_t a6, uint32_t a7);
    void ObjectUpdate(uintptr_t object, const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& pos, int32_t a6);
    void ObjectUpdate(uintptr_t object, const C44Matrix& matrix);
    void ObjectUpdate(uintptr_t object, C3Vector& pos, float facing, float a4, float a5, int32_t a6);
    int32_t IsValidPosition(const C3Vector& pos, float a2);
    bool DNIntersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist);
    int32_t GetFacets(const CAaBox& box, const CAaBox& box2, FacetData& facets, uint32_t flags);
    int32_t GetFacets(const CAaBox& box, FacetData& facets, uint32_t flags);
    void Preload(const C3Vector& pos, int32_t loadingScreen);
    int32_t LoadMap(int32_t mapId, C3Vector& pos);
    void AddAaBoxFacets(const CAaBox& box, uint64_t guid, FacetData& facets, uint8_t flags);
    void InitializeMaps();
    void AddRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param, uint64_t param64);
    void RemoveRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param);
    void ObjectSetModelLighting(uintptr_t object, CM2Model* model);
    void SetCameraTarget(uintptr_t entity, uint64_t guid, const C44Matrix& matrix);
    uint32_t SceneCamLiquidStatus(float* depth);
    float GetSmoothCamDepth();
    void GetChunkCoords(const C3Vector& pos, int32_t& areaX, int32_t& areaY, int32_t& chunkX, int32_t& chunkY);
    int32_t ObjectHasParent(uintptr_t object);
    void UpdateAreaOfInterest(const C3Vector& pos, int32_t waitLoad);
    uintptr_t ObjectFind(uint32_t uniqueId);
    int32_t NDCClip(C3Vector* verts, uint32_t count, C3Vector**& outVerts, uint32_t& outCount);
    int32_t NDCXform(const CFrustum& frustum, C44Matrix& matrix, bool translate);
    bool IntersectSegment(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags);
}

#include "WoW/Source/WorldClient/World.inl"

#endif
