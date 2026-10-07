#include "WoW/Source/WorldClient/WorldMap.h"
#include <cfloat>
#include <malloc.h>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/WorldClient/HMMgr.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Particulate.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x007749E0/0x14045E950
void CWorldMap::UpdateAllEnvironments() {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap && CWorldScene::s_defaultWorldView) {
            if ((*worldMap)->m_particulate && CWorldScene::s_defaultWorldView->m_camLiquidStatus) {
                (*worldMap)->m_particulate->Update();
            }

            if ((*worldMap)->m_weather) {
                (*worldMap)->m_weather->Update();
            }
        }
    }
}

// PROVEN: b4827ba5 IDENTICAL/NONE
// OFFSET: 0x00774950
void CWorldMap::UpdateLowDetailMapObjDefs() {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i] && (this->m_maps[i]->m_mapFlags & CMap::Flag_Current)) {
            this->m_maps[i]->UpdateLowDetailMapObjDefs();
        }
    }
}

// OFFSET: 0x00774980
void CWorldMap::UpdateMaps(int32_t preload) {
    for (uint32_t i = 0; i < 12; i++) {
        CMap* map = this->m_maps[i];

        if (map) {
            int32_t wasPreload = static_cast<int32_t>(map->m_mapFlags << 30) >> 31;

            if (!preload) {
                map->m_mapFlags &= ~CMap::Flag_Preload;
            }

            map->UpdateMap();
            map->m_mapFlags ^= (map->m_mapFlags ^ (2 * (wasPreload != 0))) & CMap::Flag_Preload;
        }
    }
}

// OFFSET: 0x00774A30
void CWorldMap::UpdateAreas() {
    if (!this->m_maps[0]) {
        return;
    }

    CiRect rect;
    CWorldMap::GetLowDetailAreaRect(World::gbPosition, rect);

    int32_t gbMinX = World::gbChunkRect.minX >> 4;
    int32_t gbMinY = World::gbChunkRect.minY >> 4;
    int32_t gbMaxX = World::gbChunkRect.maxX >> 4;
    int32_t gbMaxY = World::gbChunkRect.maxY >> 4;

    CMapArea** disable = static_cast<CMapArea**>(alloca(1024));
    CMapArea** enable = static_cast<CMapArea**>(alloca(1024));
    uint32_t disableCount = 0;
    uint32_t enableCount = 0;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            uint8_t index = this->GetCurrentMapIndex(x, y);

            if (index == 0xFF) {
                continue;
            }

            if (x >= gbMinX && x <= gbMaxX && y >= gbMinY && y <= gbMaxY) {
                for (uint32_t i = 0; i < 12; i++) {
                    CMap* map = this->m_maps[i];

                    if (i == index) {
                        CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get(x, y);

                        if (entry && entry->m_area) {
                            map->m_mapFlags |= CMap::Flag_Preload;

                            if (!(entry->m_area->m_areaFlags & 0x1)) {
                                enable[enableCount++] = entry->m_area;
                            }
                        }
                    } else if (map) {
                        CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get(x, y);

                        if (entry && entry->m_area && (entry->m_area->m_areaFlags & 0x1)) {
                            disable[disableCount++] = entry->m_area;
                        }
                    }
                }
            }

            this->m_mapTable.Get(x, y)->m_mapIndex = index;
        }
    }

    for (uint32_t i = 0; i < disableCount; i++) {
        CMapArea* area = disable[i];
        area->Enable(0);
        area->m_areaFlags &= ~0x1;
    }

    for (uint32_t i = 0; i < enableCount; i++) {
        CMapArea* area = enable[i];
        area->Enable(1);
        area->m_areaFlags |= 0x1;
    }

    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i] && (this->m_maps[i]->m_mapFlags & CMap::Flag_Remove)) {
            this->m_maps[i]->UnloadXXX();

            if (this->m_maps[i]) {
                delete this->m_maps[i];
            }

            this->m_maps[i] = NULL;
        }
    }

    if (disableCount || enableCount) {
        this->unk_010C = 1;
        CShadowCache::s_fullClear = 1;
        CShadowCache::s_fullUpdate = 0;

        if (this->m_weather && this->m_weather->m_hmMgr) {
            this->m_weather->m_hmMgr->Invalidate();
        }
    }
}

// OFFSET: 0x00774CD0/0x14045EE20
void CWorldMap::UpdateMapObjDefGroups(CMapObj* mapObj, CMapObjDef* mapObjDef, int32_t waitLoad) {
    for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(), * next; group; group = next) {
        next = mapObjDef->m_defGroupList.Next(group);
        CMapObjGroup* objGroup = mapObj->GetGroup(group->m_groupNum, 1);

        if (objGroup->m_flags & 0x40000) {
            continue;
        }

        const C3Vector& pos = World::GetGuardBandPosition();

        float x = group->m_bbox.b.x;
        if (x <= pos.x) {
            x = group->m_bbox.t.x;

            if (pos.x <= x) {
                x = pos.x;
            }
        }

        float y = group->m_bbox.b.y;
        if (y <= pos.y) {
            y = group->m_bbox.t.y;

            if (pos.y <= y) {
                y = pos.y;
            }
        }

        float z = group->m_bbox.b.z;
        if (z <= pos.z) {
            z = group->m_bbox.t.z;

            if (pos.z <= z) {
                z = pos.z;
            }
        }

        float distSq = (y - pos.y) * (y - pos.y) + (x - pos.x) * (x - pos.x) + (z - pos.z) * (z - pos.z);

        if (objGroup->m_distSq > distSq) {
            objGroup->m_distSq = distSq;
        }

        const CAaBox& guard = World::mapObjDefBoundsGuard;

        if (group->m_bbox.t.x < guard.b.x || group->m_bbox.t.y < guard.b.y || group->m_bbox.t.z < guard.b.z || guard.t.x < group->m_bbox.b.x || guard.t.y < group->m_bbox.b.y || guard.t.z < group->m_bbox.b.z) {
            if (World::GetFrameCnt() - group->m_frameStamp >= 120 && (group->m_flags & 0x40)) {
                group->ClearRefs();
                group->m_flags &= ~0x40u;
            }

            continue;
        }

        if (!(objGroup->m_flags & 0x40)) {
            if (!objGroup->m_asyncObject) {
                mapObj->ReadGroup(group->m_groupNum);
            }

            const CAaBox& bounds = World::mapObjDefBounds;

            if (waitLoad && group->m_bbox.t.x >= bounds.b.x && group->m_bbox.t.y >= bounds.b.y && group->m_bbox.t.z >= bounds.b.z && bounds.t.x >= group->m_bbox.b.x && bounds.t.y >= group->m_bbox.b.y && bounds.t.z >= group->m_bbox.b.z) {
                mapObj->WaitLoadGroup(group->m_groupNum);
            }
        }

        objGroup->m_frameStamp = World::GetFrameCnt();
        group->m_frameStamp = World::GetFrameCnt();

        if (objGroup->m_flags & 0x40) {
            if (!(group->m_flags & 0x40)) {
                group->CreateDoodadRefs();
                this->UpdateEntityLinks(group->m_bbox);
                group->CreateBatchMaterials();
                group->m_flags |= 0x40;
            }

            if (objGroup->m_flags & 0x40) {
                continue;
            }
        }

        CAaBox box;
        box.b.x = group->m_bbox.b.x;
        box.b.y = group->m_bbox.b.y;
        box.b.z = group->m_bbox.b.z - 0.5f;
        box.t.x = group->m_bbox.t.x;
        box.t.y = group->m_bbox.t.y;
        box.t.z = group->m_bbox.t.z + 1.0f;
        CWorldScene::s_barrier.AddBarrier(box, 50.0f);
    }
}

// OFFSET: 0x00774FB0
void CWorldMap::UpdateEntitys() {
    for (CMapEntity* entity = this->m_entityList.Head(), * next; entity; entity = next) {
        next = this->m_entityList.Next(entity);
        entity->Update();
    }

    this->unk_010C = 0;
}

// OFFSET: 0x00774FF0
void CWorldMap::UpdateBarriers() {
    int32_t barrierMode = CWorldScene::s_barrier.m_enabled;

    if (!barrierMode) {
        return;
    }

    float posX = World::GetGuardBandPosition().x;
    float posY = World::GetGuardBandPosition().y;
    const C3Vector& pos = World::GetGuardBandPosition();

    int32_t maxY = static_cast<int32_t>(-((posX - 41.0f) - 17066.666f) * 0.029999999f) >> 4;
    int32_t minY = static_cast<int32_t>(-((pos.x + 41.0f) - 17066.666f) * 0.029999999f) >> 4;
    int32_t minX = static_cast<int32_t>(-((pos.y + 41.0f) - 17066.666f) * 0.029999999f) >> 4;

    for (int32_t y = minY; y <= maxY; y++) {
        int32_t maxX = static_cast<int32_t>(-((posY - 41.0f) - 17066.666f) * 0.029999999f) >> 4;

        for (int32_t x = minX; x <= maxX; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* tableEntry = this->m_mapTable.Get(x, y);

                if (!tableEntry || !(tableEntry->m_flags & 1)) {
                    continue;
                }

                map = this->m_maps[tableEntry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get(x, y);

            if (!entry || !(entry->m_flags & 1)) {
                continue;
            }

            CMapArea* area = entry->m_area;

            if (!area) {
                continue;
            }

            for (int32_t side = 0; side < 4; side++) {
                int32_t nx = x + (side == 0 ? -1 : side == 2 ? 1 : 0);
                int32_t ny = y + (side == 1 ? -1 : side == 3 ? 1 : 0);
                CMap* neighborMap;

                if (this->m_isGlobalMapObj) {
                    neighborMap = this->m_maps[0];
                } else {
                    CMapTableEntry* tableEntry = this->m_mapTable.Get(nx, ny);
                    neighborMap = tableEntry && (tableEntry->m_flags & 1) ? this->m_maps[tableEntry->m_mapIndex] : NULL;
                }

                if (neighborMap) {
                    CMap::CMapAreaTableEntry* neighbor = neighborMap->m_areaTable.Get(nx, ny);

                    if (neighbor && neighbor->m_area && !neighbor->m_area->IsLoading() && barrierMode != 2) {
                        continue;
                    }
                }

                C2Vector a;
                C2Vector b;

                if (side == 0) {
                    a.x = area->m_bbox.b.x;
                    a.y = area->m_bbox.t.y;
                    b.x = area->m_bbox.t.x;
                    b.y = area->m_bbox.t.y;
                } else if (side == 1) {
                    a.x = area->m_bbox.t.x;
                    a.y = area->m_bbox.t.y;
                    b.x = area->m_bbox.t.x;
                    b.y = area->m_bbox.b.y;
                } else if (side == 2) {
                    a.x = area->m_bbox.t.x;
                    a.y = area->m_bbox.b.y;
                    b.x = area->m_bbox.b.x;
                    b.y = area->m_bbox.b.y;
                } else {
                    a.x = area->m_bbox.b.x;
                    a.y = area->m_bbox.b.y;
                    b.x = area->m_bbox.b.x;
                    b.y = area->m_bbox.t.y;
                }

                CWorldScene::s_barrier.AddAxisAlignedWall(a, b, 40.0f);
            }
        }
    }
}

// OFFSET: 0x007754E0/0x14045F9F0
void CWorldMap::UpdateMapObjDefs(int32_t waitLoad) {
    if (waitLoad) {
        for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
            next = this->m_mapObjDefList.Next(mapObjDef);
            CMapObj* mapObj = mapObjDef->m_mapObj;
            const CAaBox& bounds = World::mapObjDefBounds;

            if (mapObjDef->m_bbox.t.x >= bounds.b.x && mapObjDef->m_bbox.t.y >= bounds.b.y && mapObjDef->m_bbox.t.z >= bounds.b.z && bounds.t.x >= mapObjDef->m_bbox.b.x && bounds.t.y >= mapObjDef->m_bbox.b.y && bounds.t.z >= mapObjDef->m_bbox.b.z && !(mapObj->m_flags & 0x40)) {
                mapObj->WaitLoad();
            }
        }
    }

    for (CMapObjDef* mapObjDef = this->m_mapObjDefHash.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefHash.Next(mapObjDef);
        CMapObj* mapObj = mapObjDef->m_mapObj;

        if (mapObj->m_flags & 0x40000) {
            continue;
        }

        const C3Vector& pos = World::GetGuardBandPosition();

        float x = mapObjDef->m_bbox.b.x;
        if (x <= pos.x) {
            x = mapObjDef->m_bbox.t.x;

            if (pos.x <= x) {
                x = pos.x;
            }
        }

        float y = mapObjDef->m_bbox.b.y;
        if (y <= pos.y) {
            y = mapObjDef->m_bbox.t.y;

            if (pos.y <= y) {
                y = pos.y;
            }
        }

        float z = mapObjDef->m_bbox.b.z;
        if (z <= pos.z) {
            z = mapObjDef->m_bbox.t.z;

            if (pos.z <= z) {
                z = pos.z;
            }
        }

        float distSq = (z - pos.z) * (z - pos.z) + (y - pos.y) * (y - pos.y) + (x - pos.x) * (x - pos.x);

        if (mapObj->m_flushTimer > distSq) {
            mapObj->m_flushTimer = distSq;
        }

        const CAaBox& guard = World::mapObjDefBoundsGuard;

        if (mapObjDef->m_bbox.t.x >= guard.b.x && mapObjDef->m_bbox.t.y >= guard.b.y && mapObjDef->m_bbox.t.z >= guard.b.z && guard.t.x >= mapObjDef->m_bbox.b.x && guard.t.y >= mapObjDef->m_bbox.b.y && guard.t.z >= mapObjDef->m_bbox.b.z) {
            const CAaBox& bounds = World::mapObjDefBounds;

            if (mapObjDef->m_bbox.t.x >= bounds.b.x && mapObjDef->m_bbox.t.y >= bounds.b.y && mapObjDef->m_bbox.t.z >= bounds.b.z && bounds.t.x >= mapObjDef->m_bbox.b.x && bounds.t.y >= mapObjDef->m_bbox.b.y && bounds.t.z >= mapObjDef->m_bbox.b.z && !(mapObj->m_flags & 0x40)) {
                mapObj->WaitLoad();
            }

            if ((mapObj->m_flags & 0x40) && !(mapObjDef->m_flags & 0x40)) {
                mapObjDef->UpdateLoaded();
            }
        }

        this->UpdateMapObjDefGroups(mapObj, mapObjDef, waitLoad);

        if (!(mapObjDef->m_flags & 0x40)) {
            CAaBox box;
            box.b.x = mapObjDef->m_bbox.b.x;
            box.b.y = mapObjDef->m_bbox.b.y;
            box.b.z = mapObjDef->m_bbox.b.z - 0.5f;
            box.t.x = mapObjDef->m_bbox.t.x;
            box.t.y = mapObjDef->m_bbox.t.y;
            box.t.z = mapObjDef->m_bbox.t.z + 1.0f;
            CWorldScene::s_barrier.AddBarrier(box, 50.0f);
        }
    }
}

// OFFSET: 0x007757F0/0x14045FDF0
void CWorldMap::Update(int32_t waitLoad) {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i]) {
            this->m_maps[i]->PreUpdateMap();
        }
    }

    this->UpdateAreas();
    this->UpdateLowDetailMapObjDefs();
    this->UpdateMaps(1);
    this->UpdateMapObjDefs(waitLoad);
    this->UpdateEntitys();
    this->UpdateStreaming();
    this->UpdateBarriers();
    this->m_facetQueryValid = 1;
}

// OFFSET: 0x00775860/0x14045FF20
void CWorldMap::UpdateAllMaps(int32_t waitLoad) {
    CMapChunkBuf::Update();
    CMapRenderChunk::UpdateBufs();
    CMap::UpdateDynamicTextures();

    if (World::prepareAll) {
        CShadowCache::s_fullClear = 1;
        CShadowCache::s_fullUpdate = 1;
        DayNight::ForceFullUpdate();
    }

    CMap::s_areasUnloaded = 0;
    AsyncFileReadClearStreamingObjectList();

    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap) {
            if (World::prepareAll) {
                (*worldMap)->PurgeMaps();
            }

            (*worldMap)->Update(waitLoad);
        }
    }

    CMapObj::Update();
    CMap::ProcessAreaLowUpdateList();
    CMap::ProcessRenderChunkUpdateList();
    CMap::ProcessChunkLiquidUpdateList();
    CMap::ProcessDoodadDefUpdateList();
    CMap::ProcessDetailDoodadUpdateList();
    AsyncFileReadUpdateStreamingObjectList();
}

// OFFSET: 0x0079A570/0x14048EAD0
void CWorldMap::GetLowDetailAreaRect(const C3Vector& pos, CiRect& rect) {
    int32_t chunkX = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.029999999f);
    int32_t chunkY = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.029999999f);
    int32_t range = static_cast<int32_t>(static_cast<int64_t>(World::GetHorizonFarClip() * 0.030000001f));

    int32_t minX = ((chunkX - range) >> 4) - 1;
    int32_t minY = ((chunkY - range) >> 4) - 1;
    int32_t maxX = ((range + chunkX) >> 4) + 1;
    int32_t maxY = ((chunkY + range) >> 4) + 1;

    rect.minX = minX;
    rect.minY = minY;
    rect.maxX = maxX;
    rect.maxY = maxY;

    if (minY < 0) {
        rect.minY = 0;
    }

    if (minX < 0) {
        rect.minX = 0;
    }

    if (maxX >= 64) {
        rect.maxX = 63;
    }

    if (maxY >= 64) {
        rect.maxY = 63;
    }
}

// OFFSET: 0x0079CB70
void CWorldMap::LocateViewerMapObjDef(CMapObjDef* mapObjDef, const C3Vector& start, const C3Vector& end, float* dists, CMapObjDef** mapObjDefs, uint32_t (*groups)[2]) {
    C3Vector localStart = start * mapObjDef->m_invMat;
    C3Vector localEnd = end * mapObjDef->m_invMat;
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!mapObj->TestBounds(localStart, localEnd)) {
        return;
    }

    uint32_t slot = (mapObjDef->m_flags & 0x200) != 0;
    int32_t exterior = 0;

    for (CMapObjDefGroup* defGroup = mapObjDef->m_defGroupList.Head(); defGroup; defGroup = mapObjDef->m_defGroupList.Next(defGroup)) {
        uint32_t groupNum = defGroup->m_groupNum;
        CMapObjGroup* group = mapObj->GetGroup(groupNum, 0);

        if (!group || (group->m_groupFlags & 0x410080) || !mapObj->TestGroupBounds(localStart, localEnd, groupNum)) {
            continue;
        }

        C3Segment seg;
        seg.b = localStart;
        seg.t = localEnd;
        World::TriData::s_statusFlags = 0;
        World::TriData::s_nBatches = 0;
        World::TriData::s_faceIndexCursor = 0;
        World::TriData::s_indexCursor = 0;
        World::TriData::s_nMatrices = 0;
        World::TriData triData;

        if (group->GetTris(seg, &dists[slot], 0, 0, &triData, mapObjDef)) {
            exterior = (group->m_groupFlags >> 3) & 0x1;
            mapObjDefs[slot] = mapObjDef;
            groups[slot][0] = groupNum;
            groups[slot][1] = 0xFFFF;

            if (!exterior) {
                float dist = 0.33329999f;
                uint32_t portalGroup = 0;

                if (mapObj->ClosestPortal(groupNum, localStart, &dist, &portalGroup) && !(mapObj->GetGroupFlags(portalGroup) & 0x8)) {
                    groups[slot][1] = portalGroup;
                }
            }
        }

        seg.b = localStart;
        seg.t = localEnd;
        float t = 1.05f;
        uint32_t portalGroups[2];

        if (mapObj->VectorIntersectPortals(groupNum, seg, t, portalGroups) && t - dists[slot] < 0.000099999997f) {
            exterior = (mapObj->GetGroupInfo(portalGroups[0])->m_flags >> 3) & 0x1;
            dists[slot] = t;
            mapObjDefs[slot] = mapObjDef;
            groups[slot][0] = portalGroups[0];

            if (mapObj->GetGroupInfo(portalGroups[1])->m_flags & 0x8) {
                groups[slot][1] = 0xFFFF;
            } else {
                groups[slot][1] = portalGroups[1];
            }
        }
    }

    if (exterior) {
        mapObjDefs[slot] = NULL;
    }
}

// OFFSET: 0x0079CF70
int32_t CWorldMap::LocateViewerMapObjDefs(CMapChunk* chunk, const C3Vector& start, const C3Vector& end, float t, CMapObjDef** mapObjDefs, uint32_t (*groups)[2]) {
    float dists[2];
    dists[0] = t;
    dists[1] = t;

    if (chunk) {
        for (CMapBaseObjLink* link = chunk->m_mapObjDefLinkList.Head(); link; link = chunk->m_mapObjDefLinkList.Next(link)) {
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

            if (mapObjDef->TestAABox(start, end)) {
                CWorldMap::LocateViewerMapObjDef(mapObjDef, start, end, dists, mapObjDefs, groups);
            }
        }
    }

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(); mapObjDef; mapObjDef = this->m_mapObjDefList.Next(mapObjDef)) {
        if (!(mapObjDef->m_flags & 0x10) && mapObjDef->TestAABox(start, end)) {
            CWorldMap::LocateViewerMapObjDef(mapObjDef, start, end, dists, mapObjDefs, groups);
        }
    }

    if (mapObjDefs[0]) {
        return 1;
    }

    if (!mapObjDefs[1]) {
        return 0;
    }

    mapObjDefs[0] = mapObjDefs[1];
    groups[0][0] = groups[1][0];
    groups[0][1] = groups[1][1];
    mapObjDefs[1] = NULL;
    groups[1][0] = 0xFFFF;
    groups[1][1] = 0xFFFF;
    return 1;
}

// OFFSET: 0x0079F160/0x140494580
void CWorldMap::UpdateStreaming() {
    CCharacterComponent::StreamingUpdate();
    DayNight::StreamingUpdate();
    SE3::StreamingUpdate();

    int32_t chunkX = static_cast<int32_t>(-(World::GetGuardBandPosition().y - 17066.666f) * 0.029999999f);
    float chunkYf = -(World::GetGuardBandPosition().x - 17066.666f) * 0.029999999f;
    int32_t minX = chunkX - 3;
    int32_t maxX = chunkX + 3;
    int32_t minY = static_cast<int32_t>(chunkYf) - 3;
    int32_t maxY = static_cast<int32_t>(chunkYf) + 3;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            int32_t areaX = x >> 4;
            int32_t areaY = y >> 4;
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* tableEntry = this->m_mapTable.Get(areaX, areaY);

                if (!tableEntry || !(tableEntry->m_flags & 1)) {
                    continue;
                }

                map = this->m_maps[tableEntry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get(areaX, areaY);

            if (entry && (entry->m_flags & 0x2) && entry->m_area) {
                CMapChunk* chunk = entry->m_area->m_chunks[(x & 0xF) + 16 * (y & 0xF)];

                if (chunk) {
                    this->UpdateStreaming(chunk);
                }
            }
        }
    }

    CWorldMap::s_queryTag++;
}

// OFFSET: 0x0079D0A0/0x140491D30
void CWorldMap::LocateViewer(const C3Vector& pos, int32_t& exterior, CMapChunk*& chunk, int32_t& interior, CMapObjDef** mapObjDefs, uint32_t (*groups)[2]) {
    C3Vector start = pos;
    C3Vector end;
    end.x = pos.x;
    end.y = pos.y;
    end.z = pos.z - 10000.0f;
    exterior = 0;
    chunk = NULL;
    float t = 1.0f;

    int32_t subX = static_cast<int32_t>(-(start.y - 17066.666f) * 0.23999999f);
    float subYf = -(start.x - 17066.666f) * 0.23999999f;
    int32_t chunkX = (subX >> 3) & 0xF;
    int32_t areaX = (subX >> 7) & 0x3F;
    int32_t subY = static_cast<int32_t>(subYf);
    int32_t areaY = (subY >> 7) & 0x3F;
    CMapChunk* viewerChunk = NULL;
    CMap* map = NULL;

    if (this->m_isGlobalMapObj) {
        map = this->m_maps[0];
    } else {
        CMapTableEntry* tableEntry = this->m_mapTable.Get(areaX, areaY);

        if (tableEntry && (tableEntry->m_flags & 1)) {
            map = this->m_maps[tableEntry->m_mapIndex];
        }
    }

    if (map) {
        CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get(areaX, areaY);

        if (entry && (entry->m_flags & 0x2) && entry->m_area) {
            float height = -FLT_MAX;
            viewerChunk = entry->m_area->m_chunks[chunkX + 16 * ((subY >> 3) & 0xF)];

            if (viewerChunk && CMap::GetHeightTerrain(viewerChunk, start, subX & 7, subY & 7, 1, height)) {
                float ratio = (start.z - height) / (start.z - end.z);

                if (ratio >= 0.0f && ratio <= 1.0f) {
                    chunk = viewerChunk;
                    t = ratio;
                    exterior = 1;
                }
            }
        }
    }

    mapObjDefs[0] = NULL;
    mapObjDefs[1] = NULL;
    groups[0][0] = 0xFFFF;
    groups[0][1] = 0xFFFF;
    groups[1][0] = 0xFFFF;
    groups[1][1] = 0xFFFF;
    interior = this->LocateViewerMapObjDefs(viewerChunk, start, end, t, mapObjDefs, groups);

    if (interior) {
        exterior = 0;
        chunk = NULL;
    }
}
