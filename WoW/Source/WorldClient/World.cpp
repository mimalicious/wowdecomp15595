#include "World.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Common/AaBsp.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/LiquidEnvironment.h"
#include "WoW/Source/WorldClient/LiquidGeomFactories.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/ShadowQuery.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "WoW/Source/WorldClient/shadow.h"

// STATIC: 0x00E74040
WORLD_PROGRESS_CALLBACK World::s_loadProgressCallback;

// STATIC: 0x00E7403C
void* World::s_loadProgressParam;

// STATIC: 0x00E74044
WORLD_MODEL_EVENT_CALLBACK World::s_userModelEventCallback;

// STATIC: 0x00E73F34
World::COLLISION_CALLBACK World::s_entityCollisionCallback;

// STATIC: 0x00E74038
World::RENDER_OBJECT_CALLBACK World::s_entityRenderCallback;

// STATIC: 0x00E6FED4
WORLD_FRAME_CALLBACK World::s_frameCallback;

// STATIC: 0x00E6FE50
float World::s_frameTimes[30];

// STATIC: 0x00E6FEC8
uint32_t World::s_frameTimeIndex;

// STATIC: 0x00E6FED8
uint64_t World::s_lastCollisionGUID;

// STATIC: 0x00E6FEE0
uint64_t World::s_trackedMapObjGUID;

// STATIC: 0x00E74198
uint32_t World::frameCnt;

// STATIC: 0x00E741A8
uint32_t World::s_enables;

// STATIC: 0x00E741AC
uint32_t World::s_enablesMask;

// STATIC: 0x00E741B0
uint32_t World::s_m2PassEnables;

// STATIC: 0x00E74168
uint32_t World::terrainAlphaBitDepth;

// STATIC: 0x00E7416C
uint32_t World::enableLayerCnt;

// STATIC: 0x00E74180
int32_t World::prepareAll;

// STATIC: 0x00E74064
uint32_t World::worldPoolUsage;

// STATIC: 0x00E7405C
uint32_t World::alphaShadowMipLevel;

// STATIC: 0x00E74278
C4Vector World::texVect[8];

// STATIC: 0x00D00CA8
C44Matrix World::idMat;

// STATIC: 0x00E742F8
uint32_t World::shadowColor;

// STATIC: 0x00E74068
CImVector World::shadowModColor[64];

// STATIC: 0x00E74060
CTexture* World::shadowModTexture;

// STATIC: 0x00E74170
uint32_t World::detailDoodadAlphaRef;

// STATIC: 0x00D00C80
float World::detailDoodadDist = 70.0f;

// STATIC: 0x00E7417C
float World::detailDoodadDistSqr;

// STATIC: 0x00E74178
float World::detailDoodadDensityFade;

// STATIC: 0x00E7419C
uint32_t World::detailDoodadDensity;

// STATIC: 0x00E74174
int32_t World::detailDoodadTest;

// STATIC: 0x00E741A4
float World::s_farClip;

// STATIC: 0x00E741A0
float World::prevFarClip;

// STATIC: 0x00D00C94
float World::s_nearClip = 0.1f;

// STATIC: 0x00D00C98
float World::s_farClipOverride = 183.33333f;

// STATIC: 0x00D00C9C
float World::s_horizonFarClipScale = 1.0f;

// STATIC: 0x00D00CA0
float World::s_horizonNearClipScale = 0.7f;

// STATIC: 0x00D00C84
float World::fCharacterAmbMul = 1.0f;

// STATIC: 0x00E741B8
uint32_t World::s_texCacheOverride;

// STATIC: 0x00D00C8C
int32_t World::lodMax = 3;

// STATIC: 0x00D00C90
int32_t World::lodMin = 2;

// STATIC: 0x00E741C0
C3Vector World::gbPosition;

// STATIC: 0x00E741CC
C3Vector World::gbPositionOld;

// STATIC: 0x00E74188
float World::gbAoiDist;

// STATIC: 0x00E74184
float World::objectAoiDist;

// STATIC: 0x00E74240
int32_t World::gbAreaIndex[2];

// STATIC: 0x00E74220
CiRect World::chunkRect;

// STATIC: 0x00E74230
CiRect World::gbChunkRect;

// STATIC: 0x00E74268
CiRect World::gbChunkRectOld;

// STATIC: 0x00E74248
CiRect World::detailDoodadChunkRect;

// STATIC: 0x00E74258
CiRect World::detailDoodadChunkRectOld;

// STATIC: 0x00E741D8
CAaBox World::mapObjDefBounds;

// STATIC: 0x00E741F0
CAaBox World::mapObjDefBoundsGuard;

// STATIC: 0x00E74208
C3Vector World::s_weatherVelocity;

// STATIC: 0x00E74214
C3Vector World::s_weatherPosition;

// STATIC: 0x00E74058
float World::s_weatherFacing;

// STATIC: 0x00E74054
int32_t World::s_weatherOnTaxi;

// STATIC: 0x00E74050
const C3Spline_CatmullRom* World::s_weatherSpline;

// STATIC: 0x00E7404C
float World::s_weatherSplineElapsedTime;

// STATIC: 0x00E74048
float World::s_weatherSplineDuration;

// STATIC: 0x00E741BC
int32_t World::s_loadFailed;

// STATIC: 0x00E73F38
char World::s_loadFailedName[256];

static const float s_texVectSpeed[16] = { -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static CWorldMap* ActiveWorldMap() {
    return CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];
}

// OFFSET: 0x00721CC0
void World::SetLoadProgressCallback(WORLD_PROGRESS_CALLBACK callback, void* param) {
    World::s_loadProgressCallback = callback;
    World::s_loadProgressParam = param;
}

// OFFSET: 0x00721CE0
void World::SetUserModelEventCallback(WORLD_MODEL_EVENT_CALLBACK callback) {
    World::s_userModelEventCallback = callback;
}

// OFFSET: 0x00721D30
int32_t World::QueryAreaId(uintptr_t object, uint32_t& areaId) {
    if (!object) {
        return 0;
    }

    return ActiveWorldMap()->QueryAreaId(reinterpret_cast<CMapEntity*>(object), areaId);
}

// OFFSET: 0x00721D70
int32_t World::QueryObjectVisible(uintptr_t object) {
    if (!object) {
        return 0;
    }

    return (reinterpret_cast<CMapEntity*>(object)->m_entityFlags & 0x60000) == 0x20000;
}

// OFFSET: 0x00721D50
int32_t World::QueryObjectInside(uintptr_t object, int32_t a2) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryInterior(a2);
}

// OFFSET: 0x00721DC0
int32_t World::QueryMapObjSubzoneName(uintptr_t object, const char*& name, uint32_t& areaId) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjSubzoneName(name, areaId);
}

// OFFSET: 0x00721DE0
int32_t World::QueryMapObjFileName(uintptr_t object, const char*& fileName) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjFileName(fileName);
}

// OFFSET: 0x00721E00
int32_t World::QueryMapObjMinimap(uintptr_t object, const CAaBox& box, TSStackArray<MinimapQuad>& quads, uint32_t maxQuads, bool a5, bool a6) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjMinimap(box, quads, maxQuads, a5, a6);
}

// OFFSET: 0x00721E30
int32_t World::QueryMapObjIDs(uintptr_t object, uint32_t& wmoId, uint32_t& nameSet, uint32_t& groupId) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjIDs(wmoId, nameSet, groupId);
}

// OFFSET: 0x00721E60
int32_t World::QueryMapObjMatrix(uintptr_t object, C44Matrix* matrix, C44Matrix* invMatrix) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjMatrix(matrix, invMatrix);
}

// OFFSET: 0x00721E80
int32_t World::QueryMapObjAreaTable(uintptr_t object, const WMOAreaTableRec*& groupRec, const WMOAreaTableRec*& rootRec, uint32_t& wmoGroupId, int32_t a5) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjAreaTable(groupRec, rootRec, wmoGroupId, a5);
}

// OFFSET: 0x00721EB0
int32_t World::QueryObjectLiquid(uintptr_t object, uint32_t& liquidType, float& height, int32_t& a4) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (!entity || !(entity->m_entityFlags & 0x1)) {
        return 0;
    }

    height = entity->m_liquidHeight;
    liquidType = entity->m_liquidType;
    a4 = 0;
    return 1;
}

// OFFSET: 0x00721EF0
int32_t World::QueryObjectLiquidFlags(uintptr_t object, int32_t& a2, int32_t& a3) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (!entity || !(entity->m_entityFlags & 0x1)) {
        return 0;
    }

    a2 = (entity->m_entityFlags >> 5) & 1;
    a3 = (entity->m_entityFlags >> 6) & 1;
    return 1;
}

// OFFSET: 0x00721F30
int32_t World::QueryGroundType(uintptr_t object, uint32_t& groundType) {
    if (!object) {
        return 0;
    }

    int32_t type = reinterpret_cast<CMapEntity*>(object)->m_groundType;

    if (type == -1) {
        return 0;
    }

    groundType = type;
    return 1;
}

// OFFSET: 0x00721F60
int32_t World::QueryOutdoors(uintptr_t object) {
    if (!object) {
        return 1;
    }

    return ActiveWorldMap()->QueryOutdoors(reinterpret_cast<CMapEntity*>(object));
}

// OFFSET: 0x00721F80
void World::SetObjectRenderCallback(RENDER_OBJECT_CALLBACK callback) {
    World::s_entityRenderCallback = callback;
}

// OFFSET: 0x00721F90
void World::SetObjectCollisionCallback(COLLISION_CALLBACK callback) {
    World::s_entityCollisionCallback = callback;
}

// OFFSET: 0x00721FA0
void World::ObjectSetUpdateCallback(uintptr_t object, WORLD_LOAD_CALLBACK callback, void* param) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);
    entity->m_updateCallback = callback;
    entity->m_updateParam = param;
}

// OFFSET: 0x00721FC0
void World::ObjectSetShadow(uintptr_t object, int32_t enable) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);
    entity->m_entityFlags ^= (entity->m_entityFlags ^ ((enable != 0) << 9)) & 0x200;
}

// OFFSET: 0x00721FE0
int32_t World::ObjectIsInvisibleStalker(uintptr_t object) {
    return (reinterpret_cast<CMapEntity*>(object)->m_entityFlags >> 7) & 1;
}

// OFFSET: 0x00722000
void World::GetFrustumCorners(C3Vector* corners) {
    CWorldScene::GetFrustumCorners(corners);
}

// OFFSET: 0x00722010
uint32_t World::GetFrameCnt() {
    return World::frameCnt;
}

// OFFSET: 0x00722020
float World::GetFramerate() {
    float total = 0.0f;
    uint32_t index = World::s_frameTimeIndex;

    for (int32_t i = 30; i; i--) {
        total += World::s_frameTimes[index++];

        if (index == 30) {
            index = 0;
        }
    }

    if (total * 0.033333335f >= 0.001f) {
        return 1.0f / (total * 0.033333335f);
    }

    return 1000.0f;
}

// OFFSET: 0x00722080
void World::SetDetailDoodadDensity(uint32_t density) {
    if (World::detailDoodadDensity != density) {
        World::detailDoodadDensity = density;
        CDetailDoodad::DirtyPools();
    }
}

// OFFSET: 0x007220A0
void World::SetDetailDoodadDensityFade(float fade) {
    if (World::detailDoodadDensityFade != fade) {
        World::detailDoodadDensityFade = fade;
        CDetailDoodad::DirtyPools();
    }
}

// OFFSET: 0x007220D0
void World::SetDetailDoodadDist(float dist) {
    if (World::detailDoodadDist != dist) {
        World::detailDoodadDist = dist;
        World::detailDoodadDistSqr = dist * dist;
        CDetailDoodad::DirtyPools();
    }
}

// OFFSET: 0x00722110
uint32_t World::ValidateFarClip(float farClip) {
    if (farClip < 183.33333f || farClip > 1250.0f) {
        return 2;
    }

    if (farClip <= 727.0f) {
        return 0;
    }

    if (OsGetPhysicalMemory() > 0x40000000) {
        return 0;
    }

    return 32;
}

// OFFSET: 0x00722160
void World::SetNearClip(float nearClip) {
    World::s_nearClip = 0.2f;
}

// OFFSET: 0x00722180
void World::SetHorizonFarClipScale(float scale) {
    World::s_horizonFarClipScale = scale;
}

// OFFSET: 0x007221A0
void World::SetHorizonNearClipScale(float scale) {
    World::s_horizonNearClipScale = scale;
}

// OFFSET: 0x007221C0
static void ShadowModTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    switch (command) {
        case GxTex_Latch:
            texelStrideInBytes = 4 * width;
            texels = userArg;
            break;

        default:
            break;
    }
}

// OFFSET: 0x00722200
void World::ClearLastCollisionGUID() {
    World::s_lastCollisionGUID = 0;
}

// OFFSET: 0x00722210
void World::SetLastCollisionGUID(uint64_t guid) {
    World::s_lastCollisionGUID = guid;
}

// OFFSET: 0x00722230
uint64_t World::GetLastCollisionGUID() {
    return World::s_lastCollisionGUID;
}

// OFFSET: 0x00722240
void World::ClearTrackedMapObjGUID() {
    World::s_trackedMapObjGUID = 0;
}

// OFFSET: 0x00722250
void World::SetTrackedMapObjGUID(uint64_t guid) {
    World::s_trackedMapObjGUID = guid;
}

// OFFSET: 0x00722270
uint64_t World::GetTrackedMapObjGUID() {
    return World::s_trackedMapObjGUID;
}

// OFFSET: 0x00722280
uint32_t World::GetEnables() {
    return World::s_enables & World::s_enablesMask;
}

// OFFSET: 0x00722290
void World::SetEnables(uint32_t enables, int32_t enable) {
    if (enable) {
        World::s_enables |= enables;
    } else {
        World::s_enables &= ~enables;
    }
}

// OFFSET: 0x007222C0
uint32_t World::GetM2PassEnables() {
    return World::s_m2PassEnables;
}

// OFFSET: 0x007222D0
float World::GetFarClip() {
    if (World::s_farClip <= World::s_farClipOverride) {
        return World::s_farClipOverride;
    }

    return World::s_farClip;
}

// OFFSET: 0x00722310
float World::GetNearClip() {
    return World::s_nearClip;
}

// OFFSET: 0x00722320
float World::GetHorizonFarClip() {
    if (World::s_farClip <= World::s_farClipOverride) {
        return World::s_farClipOverride * World::s_horizonFarClipScale;
    }

    return World::s_farClip * World::s_horizonFarClipScale;
}

// OFFSET: 0x00722360
void World::SetFarClipOverride(float farClip) {
    float value = farClip;

    if (farClip < 183.33333f) {
        value = 183.33333f;
    } else if (farClip > 2500.0f) {
        value = 2500.0f;
    }

    World::s_farClipOverride = value;
}

// OFFSET: 0x00722390
void World::SetFarClipOverrideEnabled(int32_t enable) {
    if (enable) {
        World::s_enablesMask |= 0x100;
    } else {
        World::s_enablesMask &= ~0x100u;
    }
}

// OFFSET: 0x007223C0
void World::SetUnderwaterViewEnabled(int32_t enable) {
    if (enable) {
        World::s_enablesMask |= 0x2;
        World::s_enablesMask |= 0x80;
    } else {
        World::s_enablesMask &= ~0x2u;
        World::s_enablesMask &= ~0x80u;
    }
}

// OFFSET: 0x007223F0
const C3Vector& World::GetGuardBandPosition() {
    return World::gbPosition;
}

// OFFSET: 0x00722400
float World::GetGuardBandAoiDist() {
    return World::gbAoiDist;
}

// OFFSET: 0x00722410
float World::GetObjectAoiDist() {
    return World::objectAoiDist;
}

// OFFSET: 0x00722420
float World::GetCharacterAmbMul() {
    return World::fCharacterAmbMul;
}

// OFFSET: 0x00722430
uint32_t World::GetPoolUsage() {
    return World::worldPoolUsage;
}

// OFFSET: 0x00722440
WORLD_MODEL_EVENT_CALLBACK World::GetUserModelEventCallback() {
    return World::s_userModelEventCallback;
}

// OFFSET: 0x00722450
void World::SetObjectAmbientMultiplier(float mul) {
    World::fCharacterAmbMul = mul;
}

// OFFSET: 0x00722470
int32_t World::GetLoadFailed() {
    return World::s_loadFailed;
}

// OFFSET: 0x00722480
void World::ClearLoadFailed() {
    World::s_loadFailed = 0;
}

// OFFSET: 0x00722490
const C3Vector& World::GetWeatherVelocity() {
    return World::s_weatherVelocity;
}

// OFFSET: 0x007224A0
const C3Vector& World::GetWeatherPosition() {
    return World::s_weatherPosition;
}

// OFFSET: 0x007224B0
float World::GetWeatherFacing() {
    return World::s_weatherFacing;
}

// OFFSET: 0x007224C0
int32_t World::GetWeatherOnTaxi() {
    return World::s_weatherOnTaxi;
}

// OFFSET: 0x007224D0
const C3Spline_CatmullRom* World::GetWeatherSpline() {
    return World::s_weatherSpline;
}

// OFFSET: 0x007224E0
float World::GetWeatherSplineElapsedTime() {
    return World::s_weatherSplineElapsedTime;
}

// OFFSET: 0x007224F0
float World::GetWeatherSplineDuration() {
    return World::s_weatherSplineDuration;
}

// OFFSET: 0x00722500
float World::GetMaxTeleport() {
    return 150.0f;
}

// OFFSET: 0x00722510
float World::GetWaterEpsilon() {
    return 0.01f;
}

// OFFSET: 0x007225F0
void World::SetLoadFailed(int32_t failed, const char* name) {
    World::s_loadFailed = failed;

    if (name) {
        SStrCopy(World::s_loadFailedName, name, sizeof(World::s_loadFailedName));
    } else {
        World::s_loadFailedName[0] = '\0';
    }
}

// OFFSET: 0x00722640
int32_t World::AddMapById(int32_t mapId, int32_t a2) {
    if (!CWorldMap::s_worldMaps[0] && !CWorldMap::s_worldMaps[1]) {
        return 0;
    }

    MapRec* rec = g_mapDB.GetRecord(mapId);

    if (!rec) {
        return 0;
    }

    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap || rec->unk_004C != worldMap->m_mapId) {
        return 0;
    }

    return worldMap->AddMap(rec->m_directory, mapId, a2);
}

// OFFSET: 0x007226C0
int32_t World::RemoveMapById(int32_t mapId) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return 0;
    }

    worldMap->RemoveMap(mapId);
    return 1;
}

// OFFSET: 0x007226F0
int32_t World::SetMapActive(int32_t mapId, int32_t a2) {
    if (mapId < 0 || !ActiveWorldMap()) {
        return 0;
    }

    ActiveWorldMap()->SetCurrent(mapId, a2);
    return 1;
}

// OFFSET: 0x00722720
int32_t World::GetActiveMapLogical(int32_t a1, int32_t a2) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return -1;
    }

    return worldMap->GetActiveMapRecId(a1, a2);
}

// OFFSET: #0x0077EC80
bool World::MapIsDungeon() {
    CWorldMap* worldMap = ActiveWorldMap();
    return worldMap && worldMap->m_isGlobalMapObj;
}

// OFFSET: 0x00722750
int32_t World::IsInfiniteOcean() {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return 0;
    }

    return static_cast<int32_t>(worldMap->m_maps[0]->m_mapFlags << 26) >> 31;
}

// OFFSET: 0x00722770
void World::WaterReset() {
    for (uint32_t i = 0; i < 2; i++) {
        if (CWorldMap::s_worldMaps[i]) {
            CWorldMap::s_worldMaps[i]->WaterReset();
        }
    }
}

// OFFSET: 0x00722790
CM2Scene* World::GetScene() {
    return CWorldScene::s_m2Scene;
}

// OFFSET: 0x007227A0
void World::Render(const C3Vector& pos, float time) {
    if (World::GetEnables() & 0x8000000) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 0);
    }

    CWorldScene::Render(pos, time);

    if (World::GetEnables() & 0x8000000) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 1);
    }
}

// OFFSET: 0x00722810
void World::SetActivePlayerPos(const C3Vector& pos) {
    CWorldScene::s_playerPos = pos;
    CWorldScene::s_barrier.m_playerPos = pos;
}

// OFFSET: 0x00722850
int32_t World::FrustumCull(const CAaBox& box) {
    return CWorldScene::s_activeWorldView->FrustumCull(box);
}

// OFFSET: 0x00722870
void World::NewFootprint(uint32_t a1, const C2Vector& a2, const C3Vector& a3, float a4, int32_t a5, uint32_t a6, int32_t a7) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        worldMap->NewFootprint(a1, a2, a3, a4, a5, a6, a7);
    }
}

// OFFSET: 0x007228B0
void World::SetWeather(int32_t type, float intensity, const WeatherRec* rec, bool a4, float a5) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap && worldMap->m_weather) {
        worldMap->m_weather->SetType(type, intensity, rec, a4, 1.0f);
    }
}

// OFFSET: 0x007228F0
int32_t World::QueryMapId() {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        return worldMap->m_mapId;
    }

    return -1;
}

// OFFSET: 0x00722910
uintptr_t World::ObjectCreate(const char* name, C3Vector& pos, float facing, int32_t a4, int32_t snap, uint64_t param64, uint32_t param32, int32_t a8, uint16_t* a9, float* a10, int32_t a11) {
    const char* ext = strrchr(name, '.');

    if (ext && !SStrCmpI(ext, ".wmo", STORM_MAX_STR)) {
        CMapObjDef* mapObjDef = ActiveWorldMap()->CreateMapObjDef(name, pos, facing, a4, a8, a9, a10, a11);
        mapObjDef->m_refCount++;
        mapObjDef->m_param64 = param64;

        if (snap) {
            CMap::SnapBaseObjToSubChunk(mapObjDef, pos, facing);
            mapObjDef->UpdatePos(pos, facing, 0.0f, 0.0f);
        }

        return reinterpret_cast<uintptr_t>(mapObjDef);
    }

    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    rotation.FromAngleAxis(facing, C3Vector(0.0f, 0.0f, 1.0f));

    C44Matrix matrix;
    matrix.Translate(pos);
    matrix.Rotate(rotation);

    return ActiveWorldMap()->AddEntity(name, matrix, NULL, NULL, param64, param32, 11, NULL, 0);
}

// OFFSET: 0x00722AA0
int32_t World::ObjectIsDrawable(uintptr_t object) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        return static_cast<CMapObjDef*>(baseObj)->IsAllDrawable();
    }

    if (!(baseObj->m_type & MapBaseObj_Type_Entity)) {
        return 0;
    }

    CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

    if (entity->m_model) {
        return entity->m_model->IsDrawable();
    }

    return 1;
}

// OFFSET: 0x00722AE0
int32_t World::ObjectIsLoaded(uintptr_t object) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (!(baseObj->m_type & MapBaseObj_Type_MapObjDef)) {
        if (baseObj->m_type & MapBaseObj_Type_Entity) {
            CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

            if (entity->m_model) {
                return entity->m_model->IsLoaded();
            }

            return 1;
        }

        return 0;
    }

    CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(baseObj);

    if (!mapObjDef->IsAllLoaded()) {
        return 0;
    }

    return mapObjDef->m_mapObj->IsAllLoaded();
}

// OFFSET: 0x00722B30
void World::ObjectSetAcceptProjTex(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x800;
    } else {
        baseObj->m_flags &= ~0x800u;
    }
}

// OFFSET: 0x00722B50
void World::ObjectSetAlwaysExterior(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x1000;
    } else {
        baseObj->m_flags &= ~0x1000u;
    }
}

// OFFSET: 0x00722B70
void World::ObjectSetMoveable(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        if (enable) {
            baseObj->m_flags |= 0x200;
        } else {
            baseObj->m_flags &= ~0x200u;
        }
    }
}

// OFFSET: 0x00722BA0
void World::ObjectSetLinkAll(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (!(baseObj->m_type & MapBaseObj_Type_Entity)) {
        return;
    }

    CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

    if (enable && (entity->m_entityFlags & 0x1000)) {
        entity->m_entityFlags |= 0x2000;
    } else {
        entity->m_entityFlags &= ~0x2000u;
    }
}

// OFFSET: 0x00722BD0
int32_t World::ObjectMoveDoodads(uintptr_t object, uint16_t doodadSet, uintptr_t target, uint16_t targetDoodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);
    CMapBaseObj* targetObj = reinterpret_cast<CMapBaseObj*>(target);

    if ((baseObj->m_type & MapBaseObj_Type_MapObjDef) && (baseObj->m_flags & 0x40) && (targetObj->m_type & MapBaseObj_Type_MapObjDef) && (targetObj->m_flags & 0x40)) {
        return static_cast<CMapObjDef*>(baseObj)->MoveDoodadDefs(doodadSet, static_cast<CMapObjDef*>(targetObj), targetDoodadSet);
    }

    return 0;
}

// OFFSET: 0x00722C10
void World::ObjectSetDoodadEmittersIgnoresDistance(uintptr_t object, int32_t enable, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetDoodadEmittersIgnoresDistance(enable, doodadSet);
    }
}

// OFFSET: 0x00722C30
void World::ObjectSetDoodadsEnabled(uintptr_t object, int32_t enable, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetDoodadsEnabled(enable, doodadSet);
    }
}

// OFFSET: 0x00722C50
void World::ObjectSetSequence(uintptr_t object, uint32_t sequence, int32_t a3, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetSequence(sequence, a3, doodadSet);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

        if (entity->m_model) {
            entity->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        }
    }
}

// OFFSET: 0x00722CA0
void World::ObjectSetSequenceSpeed(uintptr_t object, float speed, int32_t a3, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetSequenceSpeed(speed, a3, doodadSet);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_DoodadDef) {
        static_cast<CMapDoodadDef*>(baseObj)->SetSequenceSpeed(speed);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

        if (entity->m_model) {
            entity->m_model->SetAnimationSpeed(speed);
        }
    }
}

// OFFSET: 0x00722CF0
void World::ObjectSetSequenceFinishedCallback(uintptr_t object, WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetSequenceFinishedCallback(callback, param, doodadSet);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

        if (entity->m_model) {
            entity->m_model->SetSequenceCallback(reinterpret_cast<void*>(callback), param);
        }
    }
}

// OFFSET: 0x00722D40
void World::ObjectSetEventCallback(uintptr_t object, WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetEventCallback(callback, param, doodadSet);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

        if (entity->m_model) {
            entity->m_model->SetEventCallback(reinterpret_cast<void*>(callback), param);
        }
    }
}

// OFFSET: 0x00722D90
int32_t World::ObjectTestConvexVolume(uintptr_t object, const C3Vector& pos, float radius) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        return static_cast<CMapObjDef*>(baseObj)->m_mapObj->TestConvexVolume(pos);
    }

    if (!(baseObj->m_type & MapBaseObj_Type_Entity)) {
        return 1;
    }

    CMapEntity* entity = static_cast<CMapEntity*>(baseObj);

    if (!entity->m_model || !entity->m_model->IsLoaded()) {
        return 0;
    }

    CM2Model* model = entity->m_model;

    if (!(model->m_shared->m_flags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    const CAaBox& box = model->m_shared->m_data->m_bounds.m_extent;

    C4Plane planes[6];
    planes[0].n = C3Vector(1.0f, 0.0f, 0.0f);
    planes[0].d = -box.t.x;
    planes[1].n = C3Vector(0.0f, 1.0f, 0.0f);
    planes[1].d = -box.t.y;
    planes[2].n = C3Vector(0.0f, 0.0f, 1.0f);
    planes[2].d = -box.t.z - radius - 0.013888889f;
    planes[3].n = C3Vector(-1.0f, 0.0f, 0.0f);
    planes[3].d = box.b.x;
    planes[4].n = C3Vector(0.0f, -1.0f, 0.0f);
    planes[4].d = box.b.y;
    planes[5].n = C3Vector(0.0f, 0.0f, -1.0f);
    planes[5].d = box.b.z;

    for (uint32_t i = 0; i < 6; i++) {
        if (planes[i].n.x * pos.x + planes[i].n.z * pos.z + planes[i].n.y * pos.y + planes[i].d > 0.0f) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00722F60
void World::ObjectEnablePortals(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if ((baseObj->m_type & MapBaseObj_Type_Entity) && (static_cast<CMapEntity*>(baseObj)->m_entityFlags & 0x2000)) {
        static_cast<CMapEntity*>(baseObj)->EnablePortals(enable);
    }
}

// OFFSET: 0x00722F80
void World::ObjectGetExtents(uintptr_t object, CAaBox& box) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->m_mapObj->GetBounds(box);
        return;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        box = static_cast<CMapEntity*>(baseObj)->m_model->GetBoundingBox();
    }
}

// OFFSET: 0x00722FE0
int32_t World::ObjectIsWMO(uintptr_t object) {
    return reinterpret_cast<CMapBaseObj*>(object)->m_type & MapBaseObj_Type_MapObjDef;
}

// OFFSET: 0x00723000
uintptr_t World::ObjectCreate(CM2Model* model, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a8) {
    C44Matrix matrix;
    return ActiveWorldMap()->AddEntity(model, matrix, callback, param, param64, param32, flags, name, a8);
}

// OFFSET: 0x007230A0
uintptr_t World::ObjectCreate(CM2Model* model, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a9) {
    return ActiveWorldMap()->AddEntity(model, matrix, callback, param, param64, param32, flags, name, a9);
}

// OFFSET: 0x007230E0
void World::ObjectSetModel(uintptr_t object, CM2Model* model) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (entity->m_model) {
        entity->m_model->m_lightingCallback = NULL;
        entity->m_model->m_lightingArg = NULL;
        entity->m_model->Release();
    }

    entity->m_flags &= ~0x40u;
    entity->m_entityFlags &= ~0x80u;
    entity->m_model = model;

    if (!model) {
        return;
    }

    if (!SStrCmpI("InvisibleStalker", model->m_shared->m_fileNameWithoutPath, 16)) {
        entity->m_entityFlags |= 0x80;
        entity->m_model->m_fFlags |= 0x2000000;
    }

    entity->m_model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
    entity->m_model->m_lightingArg = entity;
    entity->m_model->m_refCount++;
}

// OFFSET: 0x00723670
void World::ObjectSetHidden(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x10000;
    } else {
        baseObj->m_flags &= ~0x10000u;
    }
}

// OFFSET: 0x00723770
bool World::Intersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist, uint32_t flags, HitTestDebug* debug) {
    C3Vector delta(end.x - start.x, end.y - start.y, end.z - start.z);

    if (fabs(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x) < 0.0000010000001 || !ActiveWorldMap()) {
        return false;
    }

    return ActiveWorldMap()->VectorIntersect(start, end, hitPoint, dist, flags, debug);
}

// OFFSET: 0x007237E0
bool World::DNIntersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist) {
    return World::Intersect(start, end, hitPoint, dist, 0x200112, NULL);
}

// OFFSET: 0x00723840
int32_t World::IsFacetQueryValid() {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        return worldMap->m_facetQueryValid;
    }

    return 0;
}

// OFFSET: 0x007238F0
int32_t World::QueryLiquid(const C3Vector& pos, uint32_t& liquidType, float& height) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryLiquid(pos, 0, liquidType, height) && height + 0.0099999998f > pos.z;
}

// OFFSET: 0x00723950
int32_t World::QueryLiquidNearest(const C3Vector& pos, uint32_t& liquidType, float& height) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryLiquid(pos, 1, liquidType, height) && height + 0.0099999998f > pos.z;
}

// OFFSET: 0x007239B0
int32_t World::QueryLiquidSounds(uintptr_t object, float radius, TSGrowableArray<LiquidQueryResult>& results) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (entity->m_staticFlags & 0x8) {
        uint32_t distance = 9999;
        entity->QueryLiquidSounds(results, &distance);

        if (distance < 2) {
            ActiveWorldMap()->QueryLiquidSounds(entity->m_position, radius, results);
        }
    } else {
        ActiveWorldMap()->QueryLiquidSounds(entity->m_position, radius, results);
    }

    return 1;
}

// OFFSET: 0x00723A60
void World::CreateWaterRipple(const C3Vector& pos, float a2, float a3, float a4, float a5, float a6, int32_t a7, int32_t a8) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        worldMap->CreateWaterRipple(pos, a2, a3, a4, a5, a6, a7, a8);
    }
}

// OFFSET: 0x00723AB0
const C3Vector& World::GetCamPos() {
    return CWorldScene::s_defaultWorldView->m_camPos;
}

// OFFSET: 0x00723AC0
const C3Vector& World::GetCamTarget() {
    return CWorldScene::s_defaultWorldView->m_camTarget;
}

// OFFSET: 0x00723AD0
const C3Vector* World::GetCamFrustumCorners() {
    return CWorldScene::s_defaultWorldView->m_frustumCorners;
}

// OFFSET: 0x00723AE0
float World::GetCamDepth() {
    return CWorldScene::s_defaultWorldView->m_camDepth;
}

// OFFSET: 0x00723B00
void World::SetShadowColor(const CImVector& color) {
    if (World::shadowColor == *reinterpret_cast<const uint32_t*>(&color)) {
        World::shadowColor = *reinterpret_cast<const uint32_t*>(&color);
        return;
    }

    for (uint32_t i = 0; i < 64; i++) {
        World::shadowModColor[i] = color;
    }

    if (!World::shadowModTexture) {
        CGxTexFlags flags;
        flags.m_bit20 = 1;
        World::shadowModTexture = TextureCreate(GxTex_2d, 8, 8, 0, GxTex_Argb4444, GxTex_Argb8888, flags, World::shadowModColor, ShadowModTextureCallback, "World::shadowMod", 0);
    }

    GxTexUpdate(TextureGetGxTex(World::shadowModTexture), 0, 0, 8, 8, 1);
    World::shadowColor = *reinterpret_cast<const uint32_t*>(&color);
}

// OFFSET: 0x00723BB0
uint32_t World::ValidateDetailDoodadDensity(uint32_t density) {
    if (density < 16 || density > 256) {
        return 2;
    }

    if (density <= 64 || g_theGxDevicePtr->m_caps.unk_0304) {
        return 0;
    }

    return 128;
}

// OFFSET: 0x00723BF0
uint32_t World::ValidateDetailDoodadDist(uint32_t dist) {
    float value = static_cast<float>(dist);

    if (value < 0.0f || value > 500.0f) {
        return 2;
    }

    if (value <= 140.0f || g_theGxDevicePtr->m_caps.unk_0304) {
        return 0;
    }

    return 128;
}

// OFFSET: 0x00723C50
float World::GetFarClipForMap(float farClip, int32_t mapId) {
    float maxFarClip = OsGetPhysicalMemory() > 0x40000000 ? 1250.0f : 727.0f;

    if (mapId >= 0) {
        MapRec* rec = g_mapDB.GetRecord(mapId);

        if (rec && (rec->m_flags & 0x10000)) {
            maxFarClip = 727.0f;
        }
    }

    if (farClip > maxFarClip) {
        farClip = maxFarClip;
    }

    if (farClip < 183.33333f) {
        return 183.33333f;
    }

    return farClip;
}

// OFFSET: 0x00723CF0
void World::SetFarClip(float farClip) {
    CWorldMap* worldMap = ActiveWorldMap();
    float value = World::GetFarClipForMap(farClip, worldMap ? worldMap->m_mapId : -1);

    if (World::s_farClip != value) {
        World::prevFarClip = World::s_farClip;
        World::s_farClip = value;
        CMapObj::bPoolsDirty = 1;
        World::s_nearClip = 0.2f;
    }
}

// OFFSET: 0x00723D70
void World::PrepareAreaOfInterest(const C3Vector& pos, float nearClip, float farClip, float fov, float aspect) {
    float aoiFov = fov * 0.60000002f;
    World::gbPosition = pos;

    C44Matrix projection;
    GxuXformCreateProjection_Exact(aoiFov, aspect, nearClip, farClip, projection);
    ViewSettings::SetZNearFar(nearClip, farClip);
    ViewSettings::SetViewportFarClip(0.94f);

    C44Matrix view = g_theGxDevicePtr->m_xforms[GxXform_View].m_mtx[g_theGxDevicePtr->m_xforms[GxXform_View].m_level];
    C3Vector corners[8];
    GxuXformCreateProjection_Exact(aoiFov, aspect, World::s_nearClip, World::s_farClip, projection);
    GxuXformCalcFrustumCorners(view, projection, corners);

    World::gbAoiDist = sqrt(corners[7].z * corners[7].z + corners[7].y * corners[7].y + corners[7].x * corners[7].x);
    World::objectAoiDist = World::s_farClip >= 380.25f ? 380.25f : World::s_farClip;

    if (World::s_farClip > World::gbAoiDist) {
        World::gbAoiDist = World::s_farClip * 1.33f;
    } else if (World::gbAoiDist > World::s_farClip * 2.0f) {
        World::gbAoiDist = World::s_farClip * 2.0f;
    }

    if (World::gbAoiDist < 266.66666f) {
        World::gbAoiDist = 266.66666f;
    }

    int32_t radius = static_cast<int32_t>(ceil(World::gbAoiDist * 0.030000001f));
    int32_t chunkX = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.029999999f);
    int32_t chunkY = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.029999999f);

    World::gbAreaIndex[0] = (chunkX >> 4) & 0x3F;
    World::gbAreaIndex[1] = (chunkY >> 4) & 0x3F;

    World::chunkRect.minX = (chunkX - radius) & ~1;
    int32_t minY = (chunkY - radius) & ~1;
    World::gbChunkRect.minY = minY - 2;
    World::chunkRect.minY = minY;
    int32_t maxX = ((radius + chunkX) & ~1) + 1;
    World::gbChunkRect.maxX = maxX + 2;
    World::chunkRect.maxX = maxX;
    int32_t maxY = ((chunkY + radius) & ~1) + 1;
    World::gbChunkRect.maxY = maxY + 2;
    World::chunkRect.maxY = maxY;
    World::gbChunkRect.minX = World::chunkRect.minX - 2;

    World::mapObjDefBoundsGuard.b = C3Vector(pos.x - World::gbAoiDist, pos.y - World::gbAoiDist, pos.z - World::gbAoiDist);
    World::mapObjDefBoundsGuard.t = C3Vector(pos.x + World::gbAoiDist, pos.y + World::gbAoiDist, pos.z + World::gbAoiDist);
    World::mapObjDefBounds.b = C3Vector(pos.x - World::objectAoiDist, pos.y - World::objectAoiDist, pos.z - World::objectAoiDist);
    World::mapObjDefBounds.t = C3Vector(pos.x + World::objectAoiDist, pos.y + World::objectAoiDist, pos.z + World::objectAoiDist);

    if (World::chunkRect.minX < 0) {
        World::chunkRect.minX = 0;
    }

    if (maxX >= 1024) {
        World::chunkRect.maxX = 1023;
    }

    if (minY < 0) {
        World::chunkRect.minY = 0;
    }

    if (maxY >= 1024) {
        World::chunkRect.maxY = 1023;
    }

    if (World::gbChunkRect.minX < 0) {
        World::gbChunkRect.minX = 0;
    }

    if (World::gbChunkRect.maxX >= 1024) {
        World::gbChunkRect.maxX = 1023;
    }

    if (World::gbChunkRect.minY < 0) {
        World::gbChunkRect.minY = 0;
    }

    if (World::gbChunkRect.maxY >= 1024) {
        World::gbChunkRect.maxY = 1023;
    }

    int32_t detailRadius = 1 - static_cast<int32_t>(World::detailDoodadDist * -0.030000001f);
    World::detailDoodadChunkRect.minX = static_cast<int32_t>(-(World::gbPosition.y - 17066.666f) * 0.029999999f) - detailRadius;
    float gbChunkY = -(World::gbPosition.x - 17066.666f) * 0.029999999f;
    World::detailDoodadChunkRect.minY = static_cast<int32_t>(gbChunkY) - detailRadius;
    World::detailDoodadChunkRect.maxX = detailRadius + static_cast<int32_t>(-(World::gbPosition.y - 17066.666f) * 0.029999999f);
    World::detailDoodadChunkRect.maxY = detailRadius + static_cast<int32_t>(gbChunkY);
}

// OFFSET: 0x007242F0
void World::ProjectTextureCallback(const CAaBox& box, const CImVector& color, int32_t a3, void* param, int32_t a5) {
    if (a5 || (World::GetEnables() & 0x800)) {
        ProjectTex2dDrawShaderEffect(box, color, 0x200122, a3 != 0 ? 4 : 0, 1.0f);
    }
}

// OFFSET: 0x00724330
int32_t World::ProjectPositionCallback(const C3Vector& pos, float& z, void* param) {
    C3Vector end(pos.x, pos.y, pos.z - 20.0f);
    C3Vector hitPoint(0.0f, 0.0f, 0.0f);
    float dist = 1.0f;

    if (!World::Intersect(pos, end, &hitPoint, &dist, 0x100111, NULL)) {
        return 0;
    }

    z = hitPoint.z;
    return 1;
}

// OFFSET: 0x007243C0
int32_t World::GetFlightBoundsUpper(const C3Vector& pos, float& z) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryFlightBounds(pos, z, 0);
}

// OFFSET: 0x007243F0
int32_t World::GetFlightBoundsLower(const C3Vector& pos, float& z) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryFlightBounds(pos, z, 1);
}

// INLINED
void World::InitializeShaderEffects() {
    CShaderEffect::InitShaderSystem(M2GetCacheFlags() & 0x8, World::GetEnables() & 0x10000);
    CShaderEffectManager::AddEffectFile("MapObj.wfx");
    CShaderEffectManager::AddEffectFile("MapObjU.wfx");
    CShaderEffectManager::AddEffectFile("Model2.wfx");
    CShaderEffectManager::AddEffectFile("Particle.wfx");
    CShaderEffectManager::AddEffectFile("ShadowMap.wfx");
    CShaderEffectManager::AddEffectFile("LitSphere.wfx");
    CShadowQuery::Initialize();
}

// OFFSET: 0x007244A0
void World::UpdateTextureCacheBudget() {
    if (World::s_texCacheOverride) {
        return;
    }

    int32_t megabytes;

    if (g_theGxDevicePtr->DeviceBaseMipLevel()) {
        megabytes = World::s_farClip < 250.0f ? 16 : 32;
    } else if (World::s_farClip < 1000.0f) {
        megabytes = World::s_farClip < 700.0f ? 64 : 96;
    } else {
        megabytes = 128;
    }

    g_theGxDevicePtr->TexSetCacheSize(megabytes << 20);
}

// OFFSET: 0x00724510
int32_t World::IsValidPosition(const C3Vector& pos, float margin) {
    if (!_finite(pos.x) || !_finite(pos.y) || !_finite(pos.z)) {
        return 0;
    }

    float ty = -(pos.y - 17066.666f);
    float tx = -(pos.x - 17066.666f);

    return ty >= margin && 34133.332f - margin > ty && tx >= margin && 34133.332f - margin > tx;
}

// OFFSET: 0x00724660
void World::Initialize() {
    CWorldParam::Initialize();

    World::s_enables |= 0xBFFF;
    World::s_userModelEventCallback = NULL;
    World::s_loadProgressCallback = NULL;
    World::s_loadProgressParam = NULL;
    World::s_entityRenderCallback = NULL;
    World::s_entityCollisionCallback = NULL;
    World::frameCnt = 0;
    World::s_farClipOverride = 183.33333f;
    World::s_enablesMask = 0x3FF1FFFF;
    World::s_m2PassEnables = 0xFFFFFFFF;
    World::terrainAlphaBitDepth = CWorldParam::cvar_terrainAlphaBitDepth->m_intValue;

    if (!g_theGxDevicePtr->m_caps.m_shaderModel) {
        World::terrainAlphaBitDepth = 4;
    }

    for (uint32_t i = 0; i < 8; i++) {
        World::texVect[i].x = 0.0f;
        World::texVect[i].y = 0.0f;
        World::texVect[i].z = 0.0f;
        World::texVect[i].w = 1.0f;
    }

    World::idMat = C44Matrix();
    World::enableLayerCnt = 4;
    World::prepareAll = 0;
    World::worldPoolUsage = 0;
    World::shadowColor = 0xFFFFFFFF;
    World::detailDoodadAlphaRef = 128;
    World::detailDoodadDistSqr = World::detailDoodadDist * World::detailDoodadDist;
    World::detailDoodadTest = 0;

    Liquid::AddConfigChangedCallback(World::WaterReset);
    World::InitializeShaderEffects();
    CWorldScene::Initialize();
    DayNight::Initialize(true);
    DayNight::SetIntersectCallback(World::DNIntersect);

    World::chunkRect = CiRect();
    World::gbChunkRect = CiRect();
    World::gbChunkRectOld = CiRect();
    World::mapObjDefBounds = CAaBox();
    World::mapObjDefBoundsGuard = CAaBox();
    World::lodMax = 3;
    World::lodMin = 2;

    Liquid::Initialize(1, 1.0f);
    Weather::Initialize();
    ProjectTexInitialize();
    ShadowInit();

    World::s_weatherFacing = 0.0f;
    World::s_weatherOnTaxi = 0;
    World::s_weatherSpline = NULL;
    World::s_weatherSplineElapsedTime = 0.0f;
    World::s_weatherSplineDuration = 0.0f;

    g_theGxDevicePtr->AddShaderModelChangeCallback(World::OnShaderModelChanged, NULL);
    World::s_loadFailed = 0;
}

// OFFSET: 0x00724AA0
void World::Destroy() {
    g_theGxDevicePtr->RemoveShaderModelChangeCallback(World::OnShaderModelChanged, NULL);
    CMap::ClearDynamicTextures();
    ShadowDestroy();
    ProjectTexDestroy();
    Weather::Destroy();
    CMeshGeomFactory::Destroy();
    CChunkGeomFactory::Destroy();
    CClientLiquidEnvironment::Destroy();
    Liquid::Destroy();
    DepthCache::SetDepthRangeDump(-1.0f, -1.0f);
    SunShafts::SetMode(0);
    CWorldScene::ClearObjectFadeoutList();
    DayNight::Destroy();
    CWorldParam::Destroy();
    CWorldMap::Destroy();
    Liquid::RemoveConfigChangedCallback(World::WaterReset);

    if (World::shadowModTexture) {
        TextureRelease(World::shadowModTexture);
    }

    World::shadowModTexture = NULL;

    if (g_BspDigestCache) {
        g_BspDigestCache->~CAaBspDigestCache();
        SMemFree(g_BspDigestCache, "delete", -1, 0);
    }

    g_BspDigestCache = NULL;
    CWorldScene::Destroy();
    CShadowQuery::Destroy();
}

// OFFSET: 0x00725090
void World::PrepareAreaOfInterest(const C3Vector& pos) {
    float farClip = World::s_farClip;
    float nearClip = World::s_nearClip;
    CRect rect;
    g_theGxDevicePtr->CapsWindowSize(rect);
    World::PrepareAreaOfInterest(pos, nearClip, farClip, 1.5707964f, (rect.maxY - rect.minY) / (rect.maxX - rect.minX));
}

// OFFSET: 0x007253A0
int32_t World::LoadMap(int32_t mapId, C3Vector& pos) {
    int32_t firstMap = 0;
    World::s_loadFailed = 0;
    MapRec* rec = g_mapDB.GetRecord(mapId);

    if (!CWorldMap::s_worldMaps[0] && !CWorldMap::s_worldMaps[1]) {
        World::s_farClip = World::GetFarClipForMap(CWorldParam::cvar_farClip->m_floatValue, mapId);
        World::prevFarClip = World::s_farClip;
        World::s_nearClip = 0.2f;
        firstMap = 1;
    }

    World::PrepareAreaOfInterest(pos);
    World::gbChunkRectOld = World::gbChunkRect;
    World::gbPositionOld = World::gbPosition;
    World::detailDoodadChunkRectOld = World::detailDoodadChunkRect;

    if (firstMap) {
        Liquid::PurgeMaterialBank();
    }

    return CWorldMap::LoadWorldMap(rec->m_directory, mapId, 1);
}

// OFFSET: 0x007254D0
void World::Preload(const C3Vector& pos, int32_t loadingScreen) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return;
    }

    World::PrepareAreaOfInterest(pos);
    World::gbChunkRectOld = World::gbChunkRect;
    World::gbPositionOld = World::gbPosition;
    World::detailDoodadChunkRectOld = World::detailDoodadChunkRect;

    if (loadingScreen) {
        World::s_loadProgressCallback = LoadingScreenWorldCallback;
        World::s_loadProgressParam = NULL;
        AsyncFileReadProgressCallback(LoadingScreenAsyncCallback, NULL);

        if (LoadingScreenIsEnabled(-1)) {
            LoadingScreenSetMap(worldMap->m_mapId);
        } else {
            LoadingScreenEnable(worldMap->m_mapId, 1);
        }

        CWorldScene::s_m2Scene->PurgeModelCache();
    }

    worldMap->Preload(1);

    if (loadingScreen) {
        World::s_loadProgressCallback = NULL;
        World::s_loadProgressParam = NULL;
        AsyncFileReadProgressCallback(NULL, NULL);
        LoadingScreenDisable();
    }
}

// OFFSET: 0x00725890
void World::ObjectDestroy(uintptr_t object) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        baseObj->m_refCount--;
        static_cast<CMapObjDef*>(baseObj)->Purge(1);
        return;
    }

    CMapEntity* entity = static_cast<CMapEntity*>(baseObj);
    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    while (link) {
        CMapBaseObjLink* next = entity->m_parentLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    if (entity->m_model && entity->m_model->m_attachParent) {
        entity->m_model->DetachFromParent();
    }

    entity->m_sortLink.Unlink();
    CMap::FreeEntity(entity);
}

// OFFSET: 0x00726400
void World::FadeoutObject(uintptr_t object, float time, C44Matrix* matrix, WORLD_FADE_CALLBACK callback, uint64_t param) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (entity->m_model && entity->m_model->IsDrawable() && time >= 0.0099999998f) {
        CWorldScene::FadeoutObject(entity, time, matrix, callback, param);
    } else {
        World::ObjectDestroy(object);
    }
}

// OFFSET: 0x00726460
void World::UpdateDayNight() {
    CWorldView* worldView = CWorldScene::s_defaultWorldView;
    CWorldMap* worldMap = ActiveWorldMap();

    DayNight::SetCamera(worldView->m_camPos, worldView->m_camDir, worldView->m_camTarget, worldView->m_camLiquidStatus, worldView->m_camDepth, World::GetFarClip());
    DayNight::SetStormPercentage(worldMap->m_weather ? worldMap->m_weather->m_intensity : 0.0f);

    SMOFog fog;
    int32_t found;
    float distance = 3.402823466e38f;
    bool inside = CMapEntity::QueryCameraFog(&fog, &found, &distance) == 1;
    DayNight::SetInteriorFog(inside, fog, found, distance);

    DayNight::Update(FrameTime::GetCurTimeMs(), FrameTime::GetTickTimeSec(), FrameTime::GetCurTimeSec());

    const DayNight::LightInfo* lightInfo = DayNight::GetLightInfo();
    CImVector shadowColor;
    World::SetShadowColor(*DayNight::GetShadowColor(&shadowColor));

    CWorldMap::s_sunLight.SetDirection(lightInfo->m_direction);
    CWorldMap::s_sunLight.m_ambColor = C3Vector(lightInfo->m_ambientColor.r * 0.0039215689f, lightInfo->m_ambientColor.g * 0.0039215689f, lightInfo->m_ambientColor.b * 0.0039215689f);
    CWorldMap::s_sunLight.m_dirColor = C3Vector(lightInfo->m_directColor.r * 0.0039215689f, lightInfo->m_directColor.g * 0.0039215689f, lightInfo->m_directColor.b * 0.0039215689f);

    CImVector specular;
    DayNight::GetSpecularColor(&specular);
    CWorldMap::s_sunLight.m_specColor = C3Vector(specular.r * 0.0039215689f, specular.g * 0.0039215689f, specular.b * 0.0039215689f);
}

// OFFSET: 0x00726710
void World::UnloadMap() {
    CWorldScene::ClearObjectFadeoutList();

    CWorldView* worldView = CWorldScene::s_defaultWorldView;

    if (worldView) {
        worldView->m_viewerMapObjDef = NULL;
        worldView->m_viewerMovedMapObjDef = NULL;
        worldView->m_viewerGroups.m_count = 0;
        worldView->m_viewerMovedGroups.m_count = 0;
    }

    CWorldMap::UnloadAllWorldMaps();
    CWorldScene::s_m2Scene->GarbageCollectModels();
    World::s_farClipOverride = 183.33333f;
    World::s_enablesMask = 0x3FF1FFFF;
}

// OFFSET: 0x00726760
void World::Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect, const C3Vector& camUp, const C3Vector& weatherVelocity, const C3Vector& weatherPosition, float weatherFacing, int32_t weatherOnTaxi, const C3Spline_CatmullRom* weatherSpline, float splineElapsedTime, float splineDuration) {
    World::UpdateTextureCacheBudget();

    uint32_t index = World::s_frameTimeIndex;
    World::s_frameTimes[index] = FrameTime::GetTickTimeSec();
    World::s_frameTimeIndex = index + 1;

    if (index == 29) {
        World::s_frameTimeIndex = 0;
    }

    World::gbChunkRectOld = World::gbChunkRect;
    World::gbPositionOld = World::gbPosition;
    World::detailDoodadChunkRectOld = World::detailDoodadChunkRect;

    World::PrepareAreaOfInterest(camUp, nearClip, farClip, fov, aspect);

    if (CWorldParam::cvar_terrainMipLevel->m_intValue != World::alphaShadowMipLevel) {
        CWorldMap::ClearAllRenderChunks();
        World::alphaShadowMipLevel = CWorldParam::cvar_terrainMipLevel->m_intValue;
    }

    int32_t maxY = World::gbChunkRectOld.maxY >= World::gbChunkRect.maxY ? World::gbChunkRect.maxY : World::gbChunkRectOld.maxY;
    int32_t maxX = World::gbChunkRectOld.maxX >= World::gbChunkRect.maxX ? World::gbChunkRect.maxX : World::gbChunkRectOld.maxX;
    int32_t minY = World::gbChunkRectOld.minY <= World::gbChunkRect.minY ? World::gbChunkRect.minY : World::gbChunkRectOld.minY;
    int32_t minX = World::gbChunkRectOld.minX <= World::gbChunkRect.minX ? World::gbChunkRect.minX : World::gbChunkRectOld.minX;
    World::prepareAll = minX >= maxX || minY >= maxY;

    World::frameCnt++;

    if (World::s_frameCallback) {
        World::s_frameCallback();
    }

    for (uint32_t i = 0; i < 8; i++) {
        C4Vector& vect = World::texVect[i];
        vect.x = FrameTime::GetTickTimeSec() * s_texVectSpeed[2 * i] + vect.x;
        vect.y = FrameTime::GetTickTimeSec() * s_texVectSpeed[2 * i + 1] + vect.y;

        if (vect.x >= 64.0f) {
            vect.x = 0.0f;
        }

        if (vect.y >= 64.0f) {
            vect.y = 0.0f;
        }
    }

    CWorldScene::Update(camPos, camTarget, nearClip, farClip, fov, aspect);
    World::prevFarClip = World::s_farClip;
    CWorldMap::UpdateAllMaps(0);
    CWorldScene::UpdateViewer();
    World::UpdateDayNight();

    World::s_weatherVelocity = weatherVelocity;
    World::s_weatherPosition = weatherPosition;
    World::s_weatherFacing = weatherFacing;
    World::s_weatherOnTaxi = weatherOnTaxi;
    World::s_weatherSpline = weatherSpline;
    World::s_weatherSplineElapsedTime = splineElapsedTime;
    World::s_weatherSplineDuration = splineDuration;

    CWorldMap::UpdateAllEnvironments();

    World::s_weatherSpline = NULL;
    World::s_weatherSplineElapsedTime = 0.0f;
    World::s_weatherSplineDuration = 0.0f;

    CWorldScene::UpdateLiquid();

    if (ActiveWorldMap()) {
        ActiveWorldMap()->UpdateRipples(weatherVelocity);
    }
}

// OFFSET: 0x00724490
void World::OnShaderModelChanged(void* param) {
    CShadowQuery::Destroy();
    CShaderEffectManager::DestroyAllEffects();
    World::InitializeShaderEffects();
}
