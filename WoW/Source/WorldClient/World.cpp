#include "World.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include <typeinfo>
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
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/DepthCache.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"
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
#include "WoW/Source/WorldClient/HMMgr.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/ShadowQuery.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "WoW/Source/WorldClient/shadow.h"

// STATIC: 0x00E74040/0x140D5DD60
WORLD_PROGRESS_CALLBACK World::s_loadProgressCallback;

// STATIC: 0x00E7403C/0x140D5DD58
void* World::s_loadProgressParam;

// STATIC: 0x00E74044/0x140D5DD68
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

// STATIC: 0x00E6FED8/0x140D595B0
uint64_t World::s_lastCollisionGUID;

// STATIC: 0x00E6FEE0/0x140D595B8
uint64_t World::s_trackedMapObjGUID;

// STATIC: 0x00E74198/0x140D5DEC8
uint32_t World::frameCnt;

// STATIC: 0x00E741A8/0x140D5DED8
uint32_t World::s_enables;

// STATIC: 0x00E741AC/0x140D5DEDC
uint32_t World::s_enablesMask;

// STATIC: 0x00E741B0
uint32_t World::s_m2PassEnables;

// STATIC: 0x00E74168
uint32_t World::terrainAlphaBitDepth;

// STATIC: 0x00E7416C
uint32_t World::enableLayerCnt;

// STATIC: 0x00E74180
int32_t World::prepareAll;

// STATIC: 0x00E74064/0x140D5DD8C
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

// STATIC: 0x00E741A4/0x140D5DED4
float World::s_farClip;

// STATIC: 0x00E741A0
float World::prevFarClip;

// STATIC: 0x00D00C94/0x140BB05D4
float World::s_nearClip = 0.1f;

// STATIC: 0x00D00C98/0x140BB05D8
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

// STATIC: 0x00E741C0/0x140D5DEF0
C3Vector World::gbPosition;

// STATIC: 0x00E741CC
C3Vector World::gbPositionOld;

// STATIC: 0x00E74188
float World::gbAoiDist;

// STATIC: 0x00E74184/0x140D5DEB4
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

// STATIC: 0x00E74208/0x140D5DF40
C3Vector World::s_weatherPosition;

// STATIC: 0x00E74214/0x140D5DF50
C3Vector World::s_weatherVelocity;

// STATIC: 0x00E74058/0x140D5DD84
float World::s_weatherFacing;

// STATIC: 0x00E74054/0x140D5DD80
int32_t World::s_weatherOnTaxi;

// STATIC: 0x00E74050/0x140D5DD78
const C3Spline_CatmullRom* World::s_weatherSpline;

// STATIC: 0x00E7404C/0x140D5DD74
float World::s_weatherSplineElapsedTime;

// STATIC: 0x00E74048/0x140D5DD70
float World::s_weatherSplineDuration;

// STATIC: 0x00E741BC/0x140D5DEEC
int32_t World::s_loadFailed;

// STATIC: 0x00E70C38
uint64_t World::s_camTargetGuid;

// STATIC: 0x00D00AA8
C44Matrix World::s_camTargetMatrix;

// STATIC: 0x00D00AE8
C44Matrix World::s_camTargetView;

// STATIC: 0x00E71080
C3Vector* World::s_ndcClipVertsA[32];

// STATIC: 0x00E71000
C3Vector* World::s_ndcClipVertsB[32];

// STATIC: 0x00E73F38
char World::s_loadFailedName[256];

static const float s_texVectSpeed[16] = { -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, -1.0f, -1.0f };

static CWorldMap* ActiveWorldMap() {
    return CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];
}

// PROVEN: 2f95aeb8 IDENTICAL/NONE
// OFFSET: 0x00721CB0
void World::InitializeMaps() {
    CWorldMap::Initialize();
}

// PROVEN: 66b8f85f IDENTICAL/IDENTICAL
// OFFSET: 0x00721CC0/0x1403F82C0
void World::SetLoadProgressCallback(WORLD_PROGRESS_CALLBACK callback, void* param) {
    World::s_loadProgressCallback = callback;
    World::s_loadProgressParam = param;
}

// PROVEN: 9b265c1d IDENTICAL/IDENTICAL
// OFFSET: 0x00721CE0/0x1403F82D0
void World::SetUserModelEventCallback(WORLD_MODEL_EVENT_CALLBACK callback) {
    World::s_userModelEventCallback = callback;
}

// PROVEN: a51c5e34 IDENTICAL/NONE
// OFFSET: 0x00721CF0
void World::AddRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param, uint64_t param64) {
    CWorldScene::AddRenderCallback(pass, callback, param, param64);
}

// PROVEN: 56d0c355 IDENTICAL/NONE
// OFFSET: 0x00721D20
void World::RemoveRenderCallback(uint32_t pass, WORLD_RENDER_CALLBACK callback, void* param) {
    CWorldScene::RemoveRenderCallback(pass, callback, param);
}

// PROVEN: 6fb929dd IDENTICAL/NONE
// OFFSET: 0x00721D30/0x1403F8300
int32_t World::QueryAreaId(uintptr_t object, uint32_t* areaIds) {
    if (!object) {
        return 0;
    }

    return CWorldMap::QueryAreaIds(reinterpret_cast<CMapEntity*>(object), areaIds);
}

// PROVEN: 81e27fde IDENTICAL/NONE
// OFFSET: 0x00721D70
int32_t World::QueryObjectVisible(uintptr_t object) {
    if (!object) {
        return 0;
    }

    return (reinterpret_cast<CMapEntity*>(object)->m_entityFlags & 0x60000) == 0x20000;
}

// PROVEN: 35567ff0 IDENTICAL/NONE
// OFFSET: 0x00721D50/0x1403F8310
int32_t World::QueryObjectInside(uintptr_t object, int32_t a2) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryInterior(a2);
}

// PROVEN: 42504752 IDENTICAL/NONE
// OFFSET: 0x00721DA0
int32_t World::QueryMapObjZoneName(uintptr_t object, const char*& name) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjZoneName(name);
}

// PROVEN: 5b9af47e IDENTICAL/NONE
// OFFSET: 0x00721DC0
int32_t World::QueryMapObjSubzoneName(uintptr_t object, const char*& name, uint32_t& areaId) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjSubzoneName(name, areaId);
}

// PROVEN: d032d634 IDENTICAL/NONE
// OFFSET: 0x00721DE0/0x1403F8360
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

// OFFSET: 0x00721E80/0x1403F83C0
int32_t World::QueryMapObjAreaTable(uintptr_t object, const WMOAreaTableRec*& groupRec, const WMOAreaTableRec*& rootRec, uint32_t& wmoGroupId, int32_t a5) {
    if (!object) {
        return 0;
    }

    return reinterpret_cast<CMapEntity*>(object)->QueryMapObjAreaTable(groupRec, rootRec, wmoGroupId, a5);
}

// PROVEN: 3e5a0b50 IDENTICAL/NONE
// OFFSET: 0x00721EB0/0x1403F83F0
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

// PROVEN: 25342da4 IDENTICAL/NONE
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

// PROVEN: d1db0bde IDENTICAL/NONE
// OFFSET: 0x00721F30/0x1403F8450
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

// PROVEN: fff7bdea IDENTICAL/NONE
// OFFSET: 0x00721F60/0x1403F8470
int32_t World::QueryOutdoors(uintptr_t object) {
    if (!object) {
        return 1;
    }

    return CWorldMap::QueryOutdoors(reinterpret_cast<CMapEntity*>(object));
}

// PROVEN: daa3f05a IDENTICAL/NONE
// OFFSET: 0x00721F80
void World::SetObjectRenderCallback(RENDER_OBJECT_CALLBACK callback) {
    World::s_entityRenderCallback = callback;
}

// PROVEN: bef01d33 IDENTICAL/NONE
// OFFSET: 0x00721F90
void World::SetObjectCollisionCallback(COLLISION_CALLBACK callback) {
    World::s_entityCollisionCallback = callback;
}

// PROVEN: 6e009fb8 IDENTICAL/NONE
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

// PROVEN: 81c642d6 IDENTICAL/NONE
// OFFSET: 0x00721FE0
int32_t World::ObjectIsInvisibleStalker(uintptr_t object) {
    return (reinterpret_cast<CMapEntity*>(object)->m_entityFlags >> 7) & 1;
}

// PROVEN: 0a3205f3 IDENTICAL/UNPROVEN
// OFFSET: 0x00722000/0x1403F84E0
void World::GetFrustumCorners(C3Vector* corners) {
    CWorldScene::GetFrustumCorners(corners);
}

// PROVEN: 09921b73 IDENTICAL/IDENTICAL
// OFFSET: 0x00722010/0x1403F84F0
uint32_t World::GetFrameCnt() {
    return World::frameCnt;
}

// OFFSET: 0x00722020/0x1403F8500
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

// PROVEN: dde64a81 IDENTICAL/NONE
// OFFSET: 0x00722080
void World::SetDetailDoodadDensity(uint32_t density) {
    if (World::detailDoodadDensity != density) {
        World::detailDoodadDensity = density;
        CDetailDoodad::DirtyPools();
    }
}

// PROVEN: 76b6d8be IDENTICAL/NONE
// OFFSET: 0x007220A0
void World::SetDetailDoodadDensityFade(float fade) {
    if (World::detailDoodadDensityFade != fade) {
        World::detailDoodadDensityFade = fade;
        CDetailDoodad::DirtyPools();
    }
}

// PROVEN: 51fa7dc6 IDENTICAL/NONE
// OFFSET: 0x007220D0
void World::SetDetailDoodadDist(float dist) {
    if (World::detailDoodadDist != dist) {
        World::detailDoodadDist = dist;
        World::detailDoodadDistSqr = dist * dist;
        CDetailDoodad::DirtyPools();
    }
}

// OFFSET: 0x00722110/0x1403F85E0
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

// PROVEN: e43dabe8 IDENTICAL/NONE
// OFFSET: 0x00722160
void World::SetNearClip(float nearClip) {
    World::s_nearClip = 0.2f;
}

// PROVEN: fc615406 IDENTICAL/NONE
// OFFSET: 0x00722180
void World::SetHorizonFarClipScale(float scale) {
    World::s_horizonFarClipScale = scale;
}

// PROVEN: f4ef0a23 IDENTICAL/NONE
// OFFSET: 0x007221A0
void World::SetHorizonNearClipScale(float scale) {
    World::s_horizonNearClipScale = scale;
}

// OFFSET: 0x007221C0/0x1403F8670
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

// PROVEN: 83b05375 IDENTICAL/IDENTICAL
// OFFSET: 0x00722200/0x1403F86A0
void World::ClearLastCollisionGUID() {
    World::s_lastCollisionGUID = 0;
}

// PROVEN: ef4bff95 IDENTICAL/IDENTICAL
// OFFSET: 0x00722210/0x1403F86B0
void World::SetLastCollisionGUID(uint64_t guid) {
    World::s_lastCollisionGUID = guid;
}

// PROVEN: 281e775e IDENTICAL/IDENTICAL
// OFFSET: 0x00722230/0x1403F86C0
uint64_t World::GetLastCollisionGUID() {
    return World::s_lastCollisionGUID;
}

// PROVEN: a652528d IDENTICAL/IDENTICAL
// OFFSET: 0x00722240/0x1403F86D0
void World::ClearTrackedMapObjGUID() {
    World::s_trackedMapObjGUID = 0;
}

// PROVEN: 2fb78304 IDENTICAL/IDENTICAL
// OFFSET: 0x00722250/0x1403F86E0
void World::SetTrackedMapObjGUID(uint64_t guid) {
    World::s_trackedMapObjGUID = guid;
}

// PROVEN: 6ad48992 IDENTICAL/IDENTICAL
// OFFSET: 0x00722270/0x1403F86F0
uint64_t World::GetTrackedMapObjGUID() {
    return World::s_trackedMapObjGUID;
}

// PROVEN: 0cd6a15d IDENTICAL/IDENTICAL
// OFFSET: 0x00722280/0x1403F8700
uint32_t World::GetEnables() {
    return World::s_enables & World::s_enablesMask;
}

// PROVEN: cf1618f8 IDENTICAL/IDENTICAL
// OFFSET: 0x00722290/0x1403F8710
void World::SetEnables(uint32_t enables, int32_t enable) {
    if (enable) {
        World::s_enables |= enables;
    } else {
        World::s_enables &= ~enables;
    }
}

// PROVEN: 42d86e21 IDENTICAL/NONE
// OFFSET: 0x007222C0
uint32_t World::GetM2PassEnables() {
    return World::s_m2PassEnables;
}

// PROVEN: 628a1f5d DIFFERENT/IDENTICAL
// OFFSET: 0x007222D0/0x1403F8740
float World::GetFarClip() {
    if (World::s_farClip <= World::s_farClipOverride) {
        return World::s_farClipOverride;
    }

    return World::s_farClip;
}

// PROVEN: 6c36254e IDENTICAL/IDENTICAL
// OFFSET: 0x00722310/0x1403F8760
float World::GetNearClip() {
    return World::s_nearClip;
}

// OFFSET: 0x00722320/0x1403F8770
float World::GetHorizonFarClip() {
    if (World::s_farClip <= World::s_farClipOverride) {
        return World::s_farClipOverride * World::s_horizonFarClipScale;
    }

    return World::s_farClip * World::s_horizonFarClipScale;
}

// PROVEN: 21f767d6 DIFFERENT/IDENTICAL
// OFFSET: 0x00722360/0x1403F87A0
void World::SetFarClipOverride(float farClip) {
    float value = farClip;

    if (farClip < 183.33333f) {
        value = 183.33333f;
    } else if (farClip > 2500.0f) {
        value = 2500.0f;
    }

    World::s_farClipOverride = value;
}

// PROVEN: 15a80962 IDENTICAL/IDENTICAL
// OFFSET: 0x00722390/0x1403F87D0
void World::SetFarClipOverrideEnabled(int32_t enable) {
    if (enable) {
        World::s_enablesMask |= 0x100;
    } else {
        World::s_enablesMask &= ~0x100u;
    }
}

// OFFSET: 0x007223C0/0x1403F87F0
void World::SetUnderwaterViewEnabled(int32_t enable) {
    if (enable) {
        World::s_enablesMask |= 0x2;
        World::s_enablesMask |= 0x80;
    } else {
        World::s_enablesMask &= ~0x2u;
        World::s_enablesMask &= ~0x80u;
    }
}

// PROVEN: f3c04bc6 IDENTICAL/IDENTICAL
// OFFSET: 0x007223F0/0x1403F8820
const C3Vector& World::GetGuardBandPosition() {
    return World::gbPosition;
}

// PROVEN: c4c208b0 IDENTICAL/NONE
// OFFSET: 0x00722400
float World::GetGuardBandAoiDist() {
    return World::gbAoiDist;
}

// PROVEN: 8c9c87f9 IDENTICAL/IDENTICAL
// OFFSET: 0x00722410/0x1403F8840
float World::GetObjectAoiDist() {
    return World::objectAoiDist;
}

// PROVEN: 8508fdbd IDENTICAL/NONE
// OFFSET: 0x00722420
float World::GetCharacterAmbMul() {
    return World::fCharacterAmbMul;
}

// PROVEN: 745418d0 IDENTICAL/IDENTICAL
// OFFSET: 0x00722430/0x1403F8860
uint32_t World::GetPoolUsage() {
    return World::worldPoolUsage;
}

// OFFSET: 0x00722440/0x1403F8870
WORLD_MODEL_EVENT_CALLBACK World::GetUserModelEventCallback() {
    return World::s_userModelEventCallback;
}

// PROVEN: 06532170 IDENTICAL/NONE
// OFFSET: 0x00722450
void World::SetObjectAmbientMultiplier(float mul) {
    World::fCharacterAmbMul = mul;
}

// PROVEN: 05b9a825 IDENTICAL/IDENTICAL
// OFFSET: 0x00722470/0x1403F8890
int32_t World::GetLoadFailed() {
    return World::s_loadFailed;
}

// PROVEN: 84f28b98 IDENTICAL/IDENTICAL
// OFFSET: 0x00722480/0x1403F88A0
void World::ClearLoadFailed() {
    World::s_loadFailed = 0;
}

// PROVEN: 904420f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00722490/0x1403F88B0
const C3Vector& World::GetWeatherPosition() {
    return World::s_weatherPosition;
}

// PROVEN: 11201097 IDENTICAL/IDENTICAL
// OFFSET: 0x007224A0/0x1403F88C0
const C3Vector& World::GetWeatherVelocity() {
    return World::s_weatherVelocity;
}

// PROVEN: 400a5cb7 IDENTICAL/IDENTICAL
// OFFSET: 0x007224B0/0x1403F88D0
float World::GetWeatherFacing() {
    return World::s_weatherFacing;
}

// PROVEN: ea2102bd IDENTICAL/IDENTICAL
// OFFSET: 0x007224C0/0x1403F88E0
int32_t World::GetWeatherOnTaxi() {
    return World::s_weatherOnTaxi;
}

// PROVEN: 6fcd3f0e IDENTICAL/IDENTICAL
// OFFSET: 0x007224D0/0x1403F88F0
const C3Spline_CatmullRom* World::GetWeatherSpline() {
    return World::s_weatherSpline;
}

// PROVEN: 816f8342 IDENTICAL/IDENTICAL
// OFFSET: 0x007224E0/0x1403F8900
float World::GetWeatherSplineElapsedTime() {
    return World::s_weatherSplineElapsedTime;
}

// PROVEN: bba71aa1 IDENTICAL/IDENTICAL
// OFFSET: 0x007224F0/0x1403F8910
float World::GetWeatherSplineDuration() {
    return World::s_weatherSplineDuration;
}

// PROVEN: cd319b68 IDENTICAL/IDENTICAL
// OFFSET: 0x00722500/0x1403F8920
float World::GetMaxTeleport() {
    return 150.0f;
}

// PROVEN: 67cdb0a3 IDENTICAL/IDENTICAL
// OFFSET: 0x00722510/0x1403F8930
float World::GetWaterEpsilon() {
    return 0.01f;
}

// OFFSET: 0x00722520/0x1403F8940
void World::ProjectTex2d(const CAaBox& box, const CImVector& color, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset) {
    ::ProjectTex2d(box, color, rotation, fade, triFlags, flags, polygonOffset);
}

// PROVEN: 52172725 IDENTICAL/UNPROVEN
// OFFSET: 0x00722550/0x1403F8970
void World::ShadowRender(const C3Vector& position, float facing, float scale, const CAaBox& box, float alpha) {
    ::ShadowRender(position, facing, scale, box, alpha, World::gbPosition);
}

// PROVEN: 0dd755a3 IDENTICAL/DIFFERENT
// OFFSET: 0x007225F0/0x1403F8B40
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

// OFFSET: 0x007226C0/0x1403F8C20
int32_t World::RemoveMapById(int32_t mapId) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return 0;
    }

    worldMap->RemoveMap(mapId);
    return 1;
}

// OFFSET: 0x007226F0/0x1403F8C60
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

// PROVEN: 4bcdc566 IDENTICAL/NONE
// OFFSET: 0x00722770
void World::WaterReset() {
    for (uint32_t i = 0; i < 2; i++) {
        if (CWorldMap::s_worldMaps[i]) {
            CWorldMap::s_worldMaps[i]->WaterReset();
        }
    }
}

// PROVEN: 82b45be9 IDENTICAL/IDENTICAL
// OFFSET: 0x00722790/0x1403F8D40
CM2Scene* World::GetScene() {
    return CWorldScene::s_m2Scene;
}

// OFFSET: 0x007227A0/0x1403F8D50
void World::Render(const C3Vector& pos, float elapsedSec) {
    if (World::GetEnables() & 0x8000000) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 0);
    }

    CWorldScene::Render(pos, elapsedSec);

    if (World::GetEnables() & 0x8000000) {
        g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_PolygonFill, 1);
    }
}

// PROVEN: a359b16f IDENTICAL/NONE
// OFFSET: 0x00722810
void World::SetActivePlayerPos(const C3Vector& pos) {
    CWorldScene::s_playerPos = pos;
    CWorldScene::s_barrier.m_playerPos = pos;
}

// OFFSET: 0x00722850
int32_t World::FrustumCull(const CAaBox& box) {
    return CWorldScene::s_activeWorldView->FrustumCull(box);
}

// PROVEN: f5ab264f IDENTICAL/NONE
// OFFSET: 0x00722870
void World::NewFootprint(uint32_t a1, const C2Vector& a2, const C3Vector& a3, float a4, int32_t a5, uint32_t a6, int32_t a7) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        worldMap->NewFootprint(a1, a2, a3, a4, a5, a6, a7);
    }
}

// PROVEN: d236498c IDENTICAL/NONE
// OFFSET: 0x007228B0
void World::SetWeather(int32_t type, float intensity, const WeatherRec* rec, bool a4, float a5) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap && worldMap->m_weather) {
        worldMap->m_weather->SetType(type, intensity, rec, a4, 1.0f);
    }
}

// OFFSET: 0x007228F0/0x1403F8ED0
int32_t World::QueryMapId() {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        return worldMap->m_mapId;
    }

    return -1;
}

// OFFSET: 0x00722910/0x1403F8EF0
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

// OFFSET: 0x00722AE0/0x1403F91A0
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

// PROVEN: d7858bd4 IDENTICAL/NONE
// OFFSET: 0x00722B30
void World::ObjectSetAcceptProjTex(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x800;
    } else {
        baseObj->m_flags &= ~0x800u;
    }
}

// PROVEN: 9832f532 IDENTICAL/NONE
// OFFSET: 0x00722B50/0x1403F9210
void World::ObjectSetAlwaysExterior(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x1000;
    } else {
        baseObj->m_flags &= ~0x1000u;
    }
}

// PROVEN: 8ebcbfd0 IDENTICAL/NONE
// OFFSET: 0x00722B70/0x1403F9230
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

// PROVEN: 4b5a5891 IDENTICAL/NONE
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

// PROVEN: b21b9416 IDENTICAL/NONE
// OFFSET: 0x00722C10
void World::ObjectSetDoodadEmittersIgnoresDistance(uintptr_t object, int32_t enable, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetDoodadEmittersIgnoresDistance(enable, doodadSet);
    }
}

// PROVEN: 8116fa3f IDENTICAL/NONE
// OFFSET: 0x00722C30
void World::ObjectSetDoodadsEnabled(uintptr_t object, int32_t enable, uint16_t doodadSet) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->SetDoodadsEnabled(enable, doodadSet);
    }
}

// PROVEN: e79bdd6a IDENTICAL/NONE
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

// PROVEN: 147ff2a6 IDENTICAL/NONE
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

// PROVEN: 9554950b IDENTICAL/NONE
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

// PROVEN: 1156f339 IDENTICAL/NONE
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

// OFFSET: 0x00722D90/0x1403F9400
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

// PROVEN: 38e845cb IDENTICAL/NONE
// OFFSET: 0x00722F60/0x1403F9740
void World::ObjectEnablePortals(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if ((baseObj->m_type & MapBaseObj_Type_Entity) && (static_cast<CMapEntity*>(baseObj)->m_entityFlags & 0x2000)) {
        static_cast<CMapEntity*>(baseObj)->EnablePortals(enable);
    }
}

// PROVEN: a2b9e36d IDENTICAL/NONE
// OFFSET: 0x00722F80/0x1403F9760
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

// PROVEN: afd45f83 IDENTICAL/NONE
// OFFSET: 0x00722FE0
int32_t World::ObjectIsWMO(uintptr_t object) {
    return reinterpret_cast<CMapBaseObj*>(object)->m_type & MapBaseObj_Type_MapObjDef;
}

// OFFSET: 0x00723000/0x1403F9800
uintptr_t World::ObjectCreate(CM2Model* model, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uintptr_t param32, uint32_t flags, const char* name, int32_t a8) {
    C44Matrix matrix;
    return ActiveWorldMap()->AddEntity(model, matrix, callback, param, param64, param32, flags, name, a8);
}

// OFFSET: 0x007230A0/0x1403F98E0
uintptr_t World::ObjectCreate(CM2Model* model, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uintptr_t param32, uint32_t flags, const char* name, int32_t a9) {
    return ActiveWorldMap()->AddEntity(model, matrix, callback, param, param64, param32, flags, name, a9);
}

// OFFSET: 0x007230E0/0x1403F9950
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

// PROVEN: 9ae66e33 IDENTICAL/NONE
// OFFSET: 0x00723170
void World::ObjectSetModelLighting(uintptr_t object, CM2Model* model) {
    if (model) {
        model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        model->m_lightingArg = reinterpret_cast<void*>(object);
    }
}

// OFFSET: 0x00723190/0x1403F9A20
void World::ObjectUpdate(uintptr_t object, const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& pos, int32_t a6, uint32_t a7) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    C3Vector worldPos = pos * matrix;
    C3Vector translation(matrix.d0, matrix.d1, matrix.d2);

    CAaSphere worldSphere;
    worldSphere.c = translation;
    worldSphere.r = 0.0f;

    if (sphere.r > 0.0001f) {
        worldSphere.r = sqrt(matrix.a2 * matrix.a2 + matrix.a1 * matrix.a1 + matrix.a0 * matrix.a0) * sphere.r;
        worldSphere.c = sphere.c * matrix;
    }

    CAaBox worldBox(translation, translation);

    if (box.t.x > box.b.x && box.t.y > box.b.y && box.t.z > box.b.z) {
        worldBox = CAaBox::Transform(box, matrix);
    }

    C3Vector dPos = entity->m_position - translation;
    C3Vector dVec = entity->m_vec2 - worldPos;
    C3Vector dMin = entity->m_bbox.b - worldBox.b;
    C3Vector dMax = entity->m_bbox.t - worldBox.t;
    C3Vector dCenter = entity->m_sphere.c - worldSphere.c;
    float dRadius = entity->m_sphere.r - worldSphere.r;

    entity->m_position = translation;
    entity->m_bbox = worldBox;
    entity->m_vec2 = worldPos;
    entity->m_sphere = worldSphere;
    entity->unk_00BC = a7;

    if (a6) {
        return;
    }

    if (dPos.z * dPos.z + dPos.y * dPos.y + dPos.x * dPos.x > 0.000001f
        || dVec.z * dVec.z + dVec.y * dVec.y + dVec.x * dVec.x > 0.0001f
        || dMin.z * dMin.z + dMin.y * dMin.y + dMin.x * dMin.x > 0.0001f
        || dMax.z * dMax.z + dMax.y * dMax.y + dMax.x * dMax.x > 0.0001f
        || dCenter.z * dCenter.z + dCenter.y * dCenter.y + dCenter.x * dCenter.x > 0.0001f
        || dRadius > 0.0001f) {
        entity->m_worldMap->UpdateEntity(entity);
    }
}

// OFFSET: 0x00723640/0x1403F9F70
void World::ObjectUpdate(uintptr_t object, const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& pos, int32_t a6) {
    World::ObjectUpdate(object, matrix, box, sphere, pos, a6, -1);
}

// PROVEN: aae795c2 IDENTICAL/NONE
// OFFSET: 0x00723670
void World::ObjectSetHidden(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags |= 0x10000;
    } else {
        baseObj->m_flags &= ~0x10000u;
    }
}

// PROVEN: 54c2a4b6 IDENTICAL/NONE
// OFFSET: 0x00723690
void World::SetCameraTarget(uintptr_t entity, uint64_t guid, const C44Matrix& matrix) {
    CWorldScene::camTargEntity = entity;

    if (guid) {
        if (World::s_camTargetGuid) {
            C44Matrix inverse = matrix.AffineInverse();
            C44Matrix view = inverse * World::s_camTargetMatrix * CWorldScene::s_m2Scene->m_view;
            World::s_camTargetView = view;
        } else {
            World::s_camTargetView = CWorldScene::s_m2Scene->m_view;
        }

        World::s_camTargetMatrix = matrix;
    } else {
        World::s_camTargetView = CWorldScene::s_m2Scene->m_view;
    }

    World::s_camTargetGuid = guid;
}

// OFFSET: 0x00723770/0x1403FA0A0
bool World::Intersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist, uint32_t flags, HitTestDebug* debug) {
    C3Vector delta(end.x - start.x, end.y - start.y, end.z - start.z);

    if (fabs(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x) < 0.0000010000001 || !ActiveWorldMap()) {
        return false;
    }

    return ActiveWorldMap()->VectorIntersect(start, end, hitPoint, dist, flags, debug);
}

// PROVEN: b708c242 IDENTICAL/NONE
// OFFSET: 0x007237E0
bool World::DNIntersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist) {
    return World::Intersect(start, end, hitPoint, dist, 0x200112, NULL);
}

// PROVEN: d4a79de4 IDENTICAL/NONE
// OFFSET: 0x00723810
bool World::IntersectSegment(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags) {
    if (!ActiveWorldMap()) {
        return false;
    }

    return ActiveWorldMap()->Intersect(seg, t, plane, flags);
}

// OFFSET: 0x00723840
int32_t World::IsFacetQueryValid() {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        return worldMap->m_facetQueryValid;
    }

    return 0;
}

// OFFSET: 0x00723860
int32_t World::GetFacets(const CFrustum& frustum, FacetData& facets, uint32_t flags) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryFacets(frustum, facets, flags);
}

// PROVEN: 6d5ea70a IDENTICAL/NONE
// OFFSET: 0x00723890
void World::GetTris(const CAaBox& box, TriData& tris, uint32_t flags) {
    if (ActiveWorldMap()) {
        ActiveWorldMap()->QueryTris(box, tris, flags);
    }
}

// PROVEN: 132d9641 IDENTICAL/NONE
// OFFSET: 0x007238C0
int32_t World::GetIntersectingDoodadModels(CM2Model** models, uint32_t maxModels, const CAaBox& box, uint32_t flags) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryIntersectingModels(models, maxModels, box, flags);
}

// PROVEN: b59cda9b IDENTICAL/UNPROVEN
// OFFSET: 0x007238F0/0x1403FA250
int32_t World::QueryLiquid(const C3Vector& pos, uint32_t& liquidType, float& height) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryLiquid(pos, 0, liquidType, height) && height + 0.0099999998f > pos.z;
}

// PROVEN: b556ee82 IDENTICAL/NONE
// OFFSET: 0x00723950
int32_t World::QueryLiquidNearest(const C3Vector& pos, uint32_t& liquidType, float& height) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryLiquid(pos, 1, liquidType, height) && height + 0.0099999998f > pos.z;
}

// OFFSET: 0x007239B0/0x1403FA330
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

// PROVEN: 7fe193ba IDENTICAL/IDENTICAL
// OFFSET: 0x00723A40/0x1403FA3D0
uint32_t World::SceneCamLiquidStatus(float* depth) {
    CWorldView* view = CWorldScene::s_defaultWorldView;

    if (depth) {
        *depth = view->m_camDepth;
    }

    return view->m_camLiquidStatus;
}

// PROVEN: ff81fd99 IDENTICAL/NONE
// OFFSET: 0x00723A60
void World::CreateWaterRipple(const C3Vector& pos, float a2, float a3, float a4, float a5, float a6, int32_t a7, int32_t a8) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (worldMap) {
        worldMap->CreateWaterRipple(pos, a2, a3, a4, a5, a6, a7, a8);
    }
}

// PROVEN: 21bfafd2 IDENTICAL/IDENTICAL
// OFFSET: 0x00723AB0/0x1403FA460
const C3Vector& World::GetCamPos() {
    return CWorldScene::s_defaultWorldView->m_camPos;
}

// PROVEN: 4f20401c IDENTICAL/NONE
// OFFSET: 0x00723AC0
const C3Vector& World::GetCamTarget() {
    return CWorldScene::s_defaultWorldView->m_camTarget;
}

// PROVEN: 2a0c31f8 IDENTICAL/NONE
// OFFSET: 0x00723AD0
const C3Vector* World::GetCamFrustumCorners() {
    return CWorldScene::s_defaultWorldView->m_frustumCorners;
}

// PROVEN: 5721fb33 IDENTICAL/NONE
// OFFSET: 0x00723AE0
float World::GetCamDepth() {
    return CWorldScene::s_defaultWorldView->m_camDepth;
}

// PROVEN: e1350b32 IDENTICAL/NONE
// OFFSET: 0x00723AF0
float World::GetSmoothCamDepth() {
    return CWorldScene::s_defaultWorldView->m_smoothCamDepth;
}

// OFFSET: 0x00723B00/0x1403FA4B0
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

// OFFSET: 0x00723BB0/0x1403FA5C0
uint32_t World::ValidateDetailDoodadDensity(uint32_t density) {
    if (density < 16 || density > 256) {
        return 2;
    }

    if (density <= 64 || g_theGxDevicePtr->m_caps.unk_0304) {
        return 0;
    }

    return 128;
}

// OFFSET: 0x00723BF0/0x1403FA600
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

// OFFSET: 0x00723C50/0x1403FA650
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

// PROVEN: d3865ba8 IDENTICAL/NONE
// OFFSET: 0x007242F0
void World::ProjectTextureCallback(const CAaBox& box, const CImVector& color, int32_t a3, void* param, int32_t a5) {
    if (a5 || (World::GetEnables() & 0x800)) {
        ProjectTex2dDrawShaderEffect(box, color, 0x200122, a3 != 0 ? 4 : 0, GxPolygonOffset_Default);
    }
}

// OFFSET: 0x00724330/0x1403FAF50
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

// PROVEN: db2ba549 IDENTICAL/UNPROVEN
// OFFSET: 0x007243C0/0x1403FB040
int32_t World::GetFlightBoundsUpper(const C3Vector& pos, float& z) {
    if (!ActiveWorldMap()) {
        return 0;
    }

    return ActiveWorldMap()->QueryFlightBounds(pos, z, 0);
}

// PROVEN: 4fdce308 IDENTICAL/UNPROVEN
// OFFSET: 0x007243F0/0x1403FB070
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

// OFFSET: 0x007244A0/0x1403FB140
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

// OFFSET: 0x00724510/0x1403FB1C0
int32_t World::IsValidPosition(const C3Vector& pos, float margin) {
    if (!_finite(pos.x) || !_finite(pos.y) || !_finite(pos.z)) {
        return 0;
    }

    float ty = -(pos.y - 17066.666f);
    float tx = -(pos.x - 17066.666f);

    return ty >= margin && 34133.332f - margin > ty && tx >= margin && 34133.332f - margin > tx;
}

// OFFSET: 0x007245E0/0x1403FB2A0
void World::GetChunkCoords(const C3Vector& pos, int32_t& areaX, int32_t& areaY, int32_t& chunkX, int32_t& chunkY) {
    float x = pos.x;
    chunkX = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.029999999f);
    chunkY = static_cast<int32_t>(-(x - 17066.666f) * 0.029999999f);
    areaX = chunkX >> 4;
    areaY = chunkY >> 4;
    areaX &= 0x3F;
    areaY &= 0x3F;
    chunkX &= 0xF;
    chunkY &= 0xF;
}

// OFFSET: 0x00724660/0x1403FB310
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

// PROVEN: 81775072 IDENTICAL/NONE
// OFFSET: 0x00724B70/0x1403FB800
uint64_t World::QueryObjectParentParam64(uintptr_t object) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);
    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    if (link && link->m_ref && (link->m_ref->m_type & MapBaseObj_Type_MapObjDefGroup)) {
        return static_cast<CMapObjDefGroup*>(link->m_ref)->m_mapObjDef->m_param64;
    }

    return 0;
}

// OFFSET: 0x00724BB0
int32_t World::ObjectHasParent(uintptr_t object) {
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(object);

    if (!(entity->m_type & MapBaseObj_Type_Entity)) {
        return 0;
    }

    return entity->m_parentLinkList.Head() != NULL;
}

// OFFSET: 0x00724BE0
void World::ObjectUpdate(uintptr_t object, C3Vector& pos, float facing, float a4, float a5, int32_t a6) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (a6) {
        CMap::SnapBaseObjToSubChunk(baseObj, pos, facing);
    }

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->UpdatePos(pos, facing, a4, a5);
        return;
    }

    if (!(baseObj->m_type & MapBaseObj_Type_Entity)) {
        return;
    }

    C44Matrix matrix;
    matrix.d0 = pos.x;
    matrix.d1 = pos.y;
    matrix.d2 = pos.z;
    matrix.RotateAroundZ(facing);
    matrix.RotateAroundY(a4);
    matrix.RotateAroundX(a5);
    World::ObjectUpdate(object, matrix);
}

// OFFSET: 0x00724EB0/0x1403FBDF0
void World::ObjectUpdate(uintptr_t object, const C44Matrix& matrix) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        static_cast<CMapObjDef*>(baseObj)->UpdateMatrix(matrix);
        return;
    }

    if (!(baseObj->m_type & MapBaseObj_Type_Entity)) {
        return;
    }

    CMapEntity* entity = static_cast<CMapEntity*>(baseObj);
    C3Vector center(0.0f, 0.0f, 0.0f);
    CAaBox box(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
    CAaSphere sphere;
    sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    sphere.r = 0.0f;
    CM2Model* model = entity->m_model;

    if (model) {
        if (model->IsLoaded()) {
            box = model->GetBoundingBox();
            sphere = model->GetBoundingSphere();
            CM2Model* m = entity->m_model;

            if (!(m->m_shared->m_flags & 0x1)) {
                m->WaitForLoad(NULL);
            }

            CAaBox bounds = m->m_shared->m_data->m_collisionBounds.m_extent;
            box |= bounds;
            center.x = (bounds.t.x + bounds.b.x) * 0.5f;
            center.y = (bounds.t.y + bounds.b.y) * 0.5f;
            center.z = (bounds.t.z + bounds.b.z) * 0.5f;
        }

        entity->m_model->m_fFlags |= 0x4000;
        entity->m_model->m_worldTransform = matrix;
    }

    World::ObjectUpdate(object, matrix, box, sphere, center, 0, -1);
}

// OFFSET: 0x00725090/0x1403FC190
void World::PrepareAreaOfInterest(const C3Vector& pos) {
    float farClip = World::s_farClip;
    float nearClip = World::s_nearClip;
    CRect rect;
    g_theGxDevicePtr->CapsWindowSize(rect);
    World::PrepareAreaOfInterest(pos, nearClip, farClip, 1.5707964f, (rect.maxY - rect.minY) / (rect.maxX - rect.minX));
}

// OFFSET: 0x007253A0/0x1403FC490
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

// OFFSET: 0x007254D0/0x1403FC630
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

// OFFSET: 0x00725610/0x1403FC780
void World::UpdateAreaOfInterest(const C3Vector& pos, int32_t waitLoad) {
    World::gbChunkRectOld = World::gbChunkRect;
    World::gbPositionOld = World::gbPosition;
    World::detailDoodadChunkRectOld = World::detailDoodadChunkRect;
    World::PrepareAreaOfInterest(pos);

    if (CWorldParam::cvar_terrainMipLevel->m_intValue != World::alphaShadowMipLevel) {
        CWorldMap::ClearAllRenderChunks();
        World::alphaShadowMipLevel = CWorldParam::cvar_terrainMipLevel->m_intValue;
    }

    CWorldMap::UpdateAllMaps(waitLoad);
}

// OFFSET: 0x007256D0/0x1403FC850
void World::ObjectSetRenderEnabled(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags &= ~0x10;
    } else {
        baseObj->m_flags |= 0x10;
    }

    if (!(baseObj->m_type & MapBaseObj_Type_MapObjDef)) {
        return;
    }

    CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(baseObj);

    for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(), * nextGroup; group; group = nextGroup) {
        nextGroup = mapObjDef->m_defGroupList.Next(group);

        for (CMapBaseObjLink* link = group->m_entityLinks.Head(), * next; link; link = next) {
            next = group->m_entityLinks.Next(link);
            CMap::FreeBaseObjLink(link);
        }
    }
}

// OFFSET: 0x00725760/0x1403FC900
void World::ObjectEnableCollision(uintptr_t object, int32_t enable) {
    CMapBaseObj* baseObj = reinterpret_cast<CMapBaseObj*>(object);

    if (enable) {
        baseObj->m_flags &= ~0x80;
    } else {
        baseObj->m_flags |= 0x80;
    }

    if (baseObj->m_type & MapBaseObj_Type_Entity) {
        CMapEntity* entity = static_cast<CMapEntity*>(baseObj);
        uint32_t linkType = (entity->m_flags & 0x80) ? 2 : 1;

        for (CMapBaseObjLink* link = entity->m_parentLinkList.Head(), * next; link; link = next) {
            next = entity->m_parentLinkList.Next(link);
            CMapBaseObj* ref = link->m_ref;
            link->m_refLink.Unlink();

            if (ref->m_type & 0x4) {
                static_cast<CMapChunk*>(ref)->m_entityLinkList.LinkNode(link, linkType, NULL);
            } else if (ref->m_type & MapBaseObj_Type_MapObjDefGroup) {
                static_cast<CMapObjDefGroup*>(ref)->m_entityLinks.LinkNode(link, linkType, NULL);
            }
        }
    } else if (baseObj->m_type & MapBaseObj_Type_MapObjDef) {
        CWorldMap* worldMap = ActiveWorldMap();

        if (worldMap && worldMap->m_weather && worldMap->m_weather->m_hmMgr) {
            worldMap->m_weather->m_hmMgr->Invalidate();
        }
    }
}

// OFFSET: 0x00725890/0x1403FCA60
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

// OFFSET: 0x00725940/0x1403FCB40
void World::TriDataToFacetData(const TriData& tris, FacetData& facets, uint64_t guid, uint8_t flags) {
    uint32_t oldCount = facets.facets.Count();

    for (uint32_t b = 0; b < TriData::s_nBatches; b++) {
        TriData::Batch& batch = TriData::s_batches[b];
        const uint16_t* indices = batch.m_indices;

        for (uint32_t i = 0; i < batch.m_faceCount; i++) {
            CFacet* facet = facets.facets.New();
            const C44Matrix& m = *batch.m_matrix;
            C3Vector v0 = batch.m_vertexList[indices[0]];
            C3Vector v1 = batch.m_vertexList[indices[1]];
            C3Vector v2 = batch.m_vertexList[indices[2]];

            float nx = (v1.y - v0.y) * (v2.z - v0.z) - (v1.z - v0.z) * (v2.y - v0.y);
            float ny = (v1.z - v0.z) * (v2.x - v0.x) - (v2.z - v0.z) * (v1.x - v0.x);
            float nz = (v2.y - v0.y) * (v1.x - v0.x) - (v1.y - v0.y) * (v2.x - v0.x);
            facet->plane.n.x = m.c0 * nz + m.b0 * ny + m.a0 * nx;
            facet->plane.n.y = m.c1 * nz + m.b1 * ny + m.a1 * nx;
            facet->plane.n.z = m.c2 * nz + m.b2 * ny + m.a2 * nx;

            facet->v[0] = v0 * m;
            facet->v[1] = v1 * m;
            facet->v[2] = v2 * m;

            float len = facet->plane.n.x * facet->plane.n.x + facet->plane.n.y * facet->plane.n.y + facet->plane.n.z * facet->plane.n.z;

            if (len == 0.0f) {
                SysMsgPrintf(STATUS_ERROR, 2, "Found degenerate triangle -- data needs to be fixed\n");
                facet->plane.n.x = 0.0f;
                facet->plane.n.y = 0.0f;
                facet->plane.n.z = 1.0f;
            } else {
                float inv = 1.0f / sqrt(len);
                facet->plane.n.x *= inv;
                facet->plane.n.y *= inv;
                facet->plane.n.z *= inv;
            }

            indices += 3;
            facet->plane.d = -(facet->v[0].z * facet->plane.n.z + facet->v[0].y * facet->plane.n.y + facet->plane.n.x * facet->v[0].x);
        }
    }

    uint32_t count = facets.facets.Count();

    if (count == oldCount) {
        return;
    }

    facets.facetIds.SetCount(count);
    facets.facetFlags.SetCount(count);

    for (uint32_t i = oldCount; i < count; i++) {
        facets.facetIds.m_data[i] = guid;
        facets.facetFlags.m_data[i] = flags;
    }
}

// OFFSET: 0x00725ED0
void World::AddAaBoxFacets(const CAaBox& box, uint64_t guid, FacetData& facets, uint8_t flags) {
    if (!(box.t.x > box.b.x && box.t.y > box.b.y && box.t.z > box.b.z)) {
        return;
    }

    C3Vector corners[8];
    corners[0] = C3Vector(box.b.x, box.t.y, box.b.z);
    corners[1] = C3Vector(box.t.x, box.t.y, box.b.z);
    corners[2] = C3Vector(box.t.x, box.b.y, box.b.z);
    corners[3] = C3Vector(box.b.x, box.b.y, box.b.z);
    corners[4] = C3Vector(box.b.x, box.t.y, box.t.z);
    corners[5] = C3Vector(box.t.x, box.t.y, box.t.z);
    corners[6] = C3Vector(box.t.x, box.b.y, box.t.z);
    corners[7] = C3Vector(box.b.x, box.b.y, box.t.z);

    uint32_t oldCount = facets.facets.Count();

    static const uint32_t s_indices[36] = {
        0, 2, 3, 0, 1, 2,
        1, 6, 2, 1, 5, 6,
        4, 6, 5, 4, 7, 6,
        0, 7, 4, 0, 3, 7,
        0, 5, 1, 0, 4, 5,
        2, 7, 3, 2, 6, 7,
    };

    C4Plane planes[6];
    planes[0].From3Pos(corners[0], corners[2], corners[3]);
    planes[1].From3Pos(corners[1], corners[6], corners[2]);
    planes[2].From3Pos(corners[4], corners[6], corners[5]);
    planes[3].From3Pos(corners[0], corners[7], corners[4]);
    planes[4].From3Pos(corners[0], corners[5], corners[1]);
    planes[5].From3Pos(corners[2], corners[7], corners[3]);

    CFacet facet(0.0f);

    for (uint32_t i = 0; i < 36; i += 3) {
        facet.plane = planes[i / 6];
        facet.v[0] = corners[s_indices[i]];
        facet.v[1] = corners[s_indices[i + 1]];
        facet.v[2] = corners[s_indices[i + 2]];
        facets.facets.Add(1, &facet);
    }

    uint32_t count = facets.facets.Count();
    facets.facetIds.SetCount(count);
    facets.facetFlags.SetCount(count);

    for (uint32_t i = oldCount; i < count; i++) {
        facets.facetIds.m_data[i] = guid;
        facets.facetFlags.m_data[i] = flags;
    }
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

// OFFSET: 0x00726460/0x1403FD7F0
void World::UpdateDayNight() {
    CWorldView* worldView = CWorldScene::s_defaultWorldView;
    CWorldMap* worldMap = ActiveWorldMap();

    DayNight::SetCamera(worldView->m_camPos, worldView->m_camDir, worldView->m_camTarget, worldView->m_camLiquidStatus, worldView->m_camDepth, World::GetFarClip());
    DayNight::SetStormPercentage(worldMap->m_weather ? worldMap->m_weather->m_fogIntensity : 0.0f);

    SMOFog fog;
    uint8_t found;
    float distance = 3.402823466e38f;
    bool inside = CMapEntity::QueryCameraFog(&fog, found, distance) == 1;
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

// OFFSET: 0x007266C0
uintptr_t World::ObjectFind(uint32_t uniqueId) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return 0;
    }

    CMapObjDef* mapObjDef = worldMap->m_mapObjDefHash.Ptr(uniqueId, CWorldMap::s_mapObjDefKey);

    if (mapObjDef) {
        return reinterpret_cast<uintptr_t>(mapObjDef);
    }

    HASHKEY_DWORD key;
    key.m_key = 0;
    return reinterpret_cast<uintptr_t>(worldMap->m_doodadDefHash.Ptr(uniqueId, key));
}

// PROVEN: 94d71607 IDENTICAL/NONE
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

// OFFSET: 0x00726760/0x1403FDB30
void World::Update(const C3Vector& camPos, const C3Vector& camTarget, float nearClip, float farClip, float fov, float aspect, const C3Vector& camUp, const C3Vector& weatherPosition, const C3Vector& weatherVelocity, float weatherFacing, int32_t weatherOnTaxi, const C3Spline_CatmullRom* weatherSpline, float splineElapsedTime, float splineDuration) {
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

    World::s_weatherPosition = weatherPosition;
    World::s_weatherVelocity = weatherVelocity;
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

// OFFSET: 0x00724490/0x1403FB0A0
void World::OnShaderModelChanged(void* param) {
    CShadowQuery::Destroy();
    CShaderEffectManager::DestroyAllEffects();
    World::InitializeShaderEffects();
}

// OFFSET: 0x00726A40/0x1403FDED0
int32_t World::GetFacets(const CAaBox& box, const CAaBox& box2, FacetData& facets, uint32_t flags) {
    CWorldMap* worldMap = ActiveWorldMap();

    if (!worldMap) {
        return 0;
    }

    if (!(flags & 0x4000)) {
        return worldMap->QueryFacets(box, box2, facets, flags);
    }

    static FacetData s_facets;
    s_facets.facets.SetCount(0);
    s_facets.facetIds.SetCount(0);
    s_facets.facetFlags.SetCount(0);

    bool result = worldMap->QueryFacets(box, box2, s_facets, flags);

    for (uint32_t i = 0; i < s_facets.facets.Count(); i++) {
        if (s_facets.facets.m_data[i].plane.n.z >= 0.64278764f) {
            facets.facets.New(s_facets.facets.m_data[i]);
            facets.facetIds.New(s_facets.facetIds.m_data[i]);
            facets.facetFlags.New(s_facets.facetFlags.m_data[i]);
        }
    }

    return result;
}

// OFFSET: 0x00726BB0/0x1403FE090
int32_t World::GetFacets(const CAaBox& box, FacetData& facets, uint32_t flags) {
    return World::GetFacets(box, box, facets, flags);
}

// PROVEN: ac26dfe1 IDENTICAL/NONE
// OFFSET: 0x00726C00
void World::CLIPINFO::Set(const C3Vector& pos) {
    this->m_dist[0] = pos.x;
    this->m_dist[1] = 1.0f - pos.x;
    this->m_dist[2] = pos.y;
    this->m_dist[3] = 1.0f - pos.y;
    this->m_dist[4] = pos.z;
    this->m_dist[5] = 1.0f - pos.z;

    const uint32_t* bits = reinterpret_cast<const uint32_t*>(this->m_dist);
    this->m_flags = (bits[0] & 0x80000000)
        + ((bits[2] >> 2) & 0x20000000)
        + ((bits[5] >> 5) & 0x4000000)
        + ((bits[3] >> 3) & 0x10000000)
        + ((bits[4] >> 4) & 0x8000000)
        + ((bits[1] >> 1) & 0x40000000);
}

// OFFSET: 0x00727740
int32_t World::NDCClip(C3Vector* verts, uint32_t count, C3Vector**& outVerts, uint32_t& outCount) {
    static C3Vector s_clipVerts[32];

    CLIPINFO infoA[32];
    CLIPINFO* infoPtrA[32];
    CLIPINFO infoB[32];
    CLIPINFO* infoPtrB[32];

    if (!count) {
        return 0;
    }

    uint32_t andFlags = 0xFFFFFFFF;
    uint32_t orFlags = 0;

    for (uint32_t i = 0; i < count; i++) {
        infoA[i].Set(verts[i]);
        infoPtrA[i] = &infoA[i];
        andFlags &= infoA[i].m_flags;
        orFlags |= infoA[i].m_flags;
        World::s_ndcClipVertsA[i] = &verts[i];
    }

    if (andFlags) {
        return 0;
    }

    if (!orFlags) {
        outVerts = World::s_ndcClipVertsA;
        outCount = count;
        return 1;
    }

    C3Vector** srcVerts = World::s_ndcClipVertsA;
    CLIPINFO** srcInfos = infoPtrA;
    uint32_t srcCount = count;
    C3Vector** dstVerts = World::s_ndcClipVertsB;
    CLIPINFO** dstInfos = infoPtrB;
    uint32_t dstCount = 0;

    C3Vector* newVert = s_clipVerts;
    CLIPINFO* newInfo = infoB;
    uint32_t bit = 0x80000000;

    for (uint32_t plane = 0; plane < 6; plane++, bit >>= 1) {
        if (!(bit & orFlags)) {
            continue;
        }

        uint32_t prev = srcCount - 1;
        uint32_t prevOut = bit & srcInfos[prev]->m_flags;
        dstCount = 0;

        for (uint32_t i = 0; i < srcCount; i++) {
            CLIPINFO* info = srcInfos[i];
            uint32_t out = bit & info->m_flags;

            if (prevOut != out) {
                float denom = srcInfos[prev]->m_dist[plane] - info->m_dist[plane];

                if (denom == 0.0f) {
                    denom = 0.0001f;
                }

                const C3Vector* p = srcVerts[prev];
                const C3Vector* c = srcVerts[i];
                float t = srcInfos[prev]->m_dist[plane] / denom;
                newVert->x = (c->x - p->x) * t + p->x;
                newVert->y = p->y + (c->y - p->y) * t;
                newVert->z = p->z + (c->z - p->z) * t;
                newInfo->Set(*newVert);
                dstVerts[dstCount] = newVert;
                dstInfos[dstCount] = newInfo;
                newVert++;
                dstCount++;
                newInfo++;
            }

            if (!out) {
                dstVerts[dstCount] = srcVerts[i];
                dstInfos[dstCount] = srcInfos[i];
                dstCount++;
            }

            prev = i;
            prevOut = out;
        }

        if (!dstCount) {
            return 0;
        }

        C3Vector** tmpVerts = srcVerts;
        CLIPINFO** tmpInfos = srcInfos;
        srcVerts = dstVerts;
        srcInfos = dstInfos;
        srcCount = dstCount;
        dstVerts = tmpVerts;
        dstInfos = tmpInfos;
    }

    outVerts = srcVerts;
    outCount = srcCount;
    return 1;
}

// OFFSET: 0x00727A20
int32_t World::NDCXform(const CFrustum& frustum, C44Matrix& matrix, bool translate) {
    const C3Vector* corners = frustum.corners;
    matrix.a0 = corners[3].x - corners[0].x;
    matrix.a1 = corners[3].y - corners[0].y;
    matrix.a2 = corners[3].z - corners[0].z;
    matrix.b0 = corners[1].x - corners[0].x;
    matrix.b1 = corners[1].y - corners[0].y;
    matrix.b2 = corners[1].z - corners[0].z;
    matrix.c0 = corners[4].x - corners[0].x;
    matrix.c1 = corners[4].y - corners[0].y;
    matrix.c2 = corners[4].z - corners[0].z;
    matrix.d3 = 1.0f;
    matrix.d0 = 0.0f;
    matrix.d1 = 0.0f;
    matrix.d2 = 0.0f;
    matrix.c3 = 0.0f;
    matrix.b3 = 0.0f;
    matrix.a3 = 0.0f;

    if (translate) {
        matrix.d0 = corners[0].x;
        matrix.d1 = corners[0].y;
        matrix.d2 = corners[0].z;
    }

    float det = matrix.Determinant();

    if (fabs(det) < 0.00000023841858f) {
        matrix = C44Matrix();
        return 0;
    }

    matrix = matrix.Inverse(det);
    return 1;
}

// INLINED
World::FacetData::FacetData() {
    this->facets.m_chunk = 256;
    this->facetIds.m_chunk = 256;
    this->facetFlags.m_chunk = 256;
    this->m_status = 0;
}
