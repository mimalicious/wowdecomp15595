#include "WoW/Source/WorldClient/WorldMap.h"
#include <malloc.h>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D02A24
static const float s_preloadPassWeight[5] = { 0.5f, 0.1f, 0.2f, 0.1f, 0.1f };

// OFFSET: 0x00773550
CWorldMapPreloadData::CWorldMapPreloadData() {
    this->m_loadRect.minY = 0;
    this->m_loadRect.minX = 0;
    this->m_loadRect.maxY = 0;
    this->m_loadRect.maxX = 0;
    this->m_streamRect.minY = 0;
    this->m_streamRect.minX = 0;
    this->m_streamRect.maxY = 0;
    this->m_streamRect.maxX = 0;
    this->m_center = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_boxes[0].b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_boxes[0].t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_boxes[1].b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_boxes[1].t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_mapIndex = -1;

    for (uint32_t i = 0; i < 5; i++) {
        this->m_totals[i] = 0;
        this->m_loaded[i] = 0;
    }
}

// OFFSET: 0x007735F0
void CWorldMap::PreloadCountMapObjGroupTextures(const CAaBox& box, CMapObjDef* mapObjDef, CMapObjDefGroup* defGroup, uint32_t* totals, uint32_t* loaded) {
    float x = (box.b.x + box.t.x) * 0.5f;
    float y = (box.t.y + box.b.y) * 0.5f;
    float z = (box.t.z + box.b.z) * 0.5f;

    if (x <= defGroup->m_bbox.b.x || y <= defGroup->m_bbox.b.y || z <= defGroup->m_bbox.b.z || defGroup->m_bbox.t.x <= x || defGroup->m_bbox.t.y <= y || defGroup->m_bbox.t.z <= z) {
        return;
    }

    CMapObj* mapObj = mapObjDef->m_mapObj;
    CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 0);

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        CTexture** texture = mapObj->m_runtimeMaterials[group->m_batches[i].m_materialId].m_textures;

        for (uint32_t j = 0; j < 3; j++) {
            if (texture[j]) {
                totals[4]++;

                if (TextureIsLoaded(texture[j])) {
                    loaded[4]++;
                }
            }
        }
    }
}

// OFFSET: 0x00773760
void CWorldMap::PreloadDoodadDefs(const CAaBox& box) {
    for (CMapDoodadDef* doodadDef = CMap::s_doodadDefLoadingList.Head(), * next; doodadDef; doodadDef = next) {
        next = CMap::s_doodadDefLoadingList.Next(doodadDef);

        if (doodadDef->m_model->IsLoaded()) {
            doodadDef->UpdateLoaded();
            doodadDef->m_fade = 0xFF00;
            doodadDef->m_updateLink.Unlink();
            continue;
        }

        const CAaBox& modelBox = doodadDef->m_modelBox;

        if (modelBox.t.x < modelBox.b.x || modelBox.t.y < modelBox.b.y || modelBox.t.z < modelBox.b.z || modelBox.t.x < box.b.x || modelBox.t.y < box.b.y || modelBox.t.z < box.b.z || box.t.x < modelBox.b.x || box.t.y < modelBox.b.y || box.t.z < modelBox.b.z) {
            continue;
        }

        float distSq = CWorldView::DistSqrToBox(modelBox, World::GetGuardBandPosition());
        int32_t priority = AsyncFileReadComputePriority(2, 0, 1, distSq);
        AsyncFileReadAddStreamingObject(CMapDoodadDef::StreamingCallback, doodadDef, priority);
    }
}

// OFFSET: 0x007738D0
CMapChunk* CMap::GetChunk(const C3Vector& pos) {
    int32_t y = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.029999999f);
    int32_t x = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.029999999f);
    CMapAreaTableEntry* entry = this->m_areaTable.Get((y >> 4) & 0x3F, (x >> 4) & 0x3F);

    if (!entry || !entry->m_area) {
        return NULL;
    }

    return entry->m_area->m_chunks[(y & 0xF) + 16 * (x & 0xF)];
}

// OFFSET: 0x00773960
void CWorldMap::PreloadChunk(CMap* map, const CAaBox& box) {
    C3Vector center((box.t.x + box.b.x) * 0.5f, (box.t.y + box.b.y) * 0.5f, (box.t.z + box.b.z) * 0.5f);
    CMapChunk* chunk = map->GetChunk(center);

    if (!chunk) {
        return;
    }

    int32_t load = !chunk->AreTexturesLoaded();

    if (!chunk->LoadDetailDoodadModels()) {
        load = 1;
    }

    for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
        next = chunk->m_liquidList.Next(liquid);

        if (Liquid::IsAllLoaded(liquid->m_liquidType)) {
            AsyncFileReadAddStreamingObject(CMapChunk::LinkToAsyncHead, chunk, 0);
            return;
        }
    }

    if (load) {
        AsyncFileReadAddStreamingObject(CMapChunk::LinkToAsyncHead, chunk, 0);
    }
}

// OFFSET: 0x00773A40
void CWorldMap::PreloadMapObjDefGroups(CMapObj* mapObj, CMapObjDef* mapObjDef, const CAaBox& box) {
    for (CMapObjDefGroup* defGroup = mapObjDef->m_defGroupList.Head(), * next; defGroup; defGroup = next) {
        next = mapObjDef->m_defGroupList.Next(defGroup);
        CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 1);

        if (group->m_flags & 0x40000) {
            continue;
        }

        const C3Vector& pos = World::GetGuardBandPosition();
        float x = defGroup->m_bbox.b.x <= pos.x ? (pos.x <= defGroup->m_bbox.t.x ? pos.x : defGroup->m_bbox.t.x) : defGroup->m_bbox.b.x;
        float y = defGroup->m_bbox.b.y <= pos.y ? (pos.y <= defGroup->m_bbox.t.y ? pos.y : defGroup->m_bbox.t.y) : defGroup->m_bbox.b.y;
        float z = defGroup->m_bbox.b.z <= pos.z ? (pos.z <= defGroup->m_bbox.t.z ? pos.z : defGroup->m_bbox.t.z) : defGroup->m_bbox.b.z;
        float distSq = (x - pos.x) * (x - pos.x) + (y - pos.y) * (y - pos.y) + (z - pos.z) * (z - pos.z);

        if (group->m_distSq > distSq) {
            group->m_distSq = distSq;
        }

        if (defGroup->m_bbox.t.x < box.b.x || defGroup->m_bbox.t.y < box.b.y || defGroup->m_bbox.t.z < box.b.z || box.t.x < defGroup->m_bbox.b.x || box.t.y < defGroup->m_bbox.b.y || box.t.z < defGroup->m_bbox.b.z) {
            continue;
        }

        if (!(group->m_flags & 0x40) && !group->m_asyncObject) {
            mapObj->ReadGroup(defGroup->m_groupNum);
        }

        group->m_frameStamp = World::GetFrameCnt();
        defGroup->m_frameStamp = World::GetFrameCnt();

        if ((group->m_flags & 0x40) && !(defGroup->m_flags & 0x40)) {
            defGroup->CreateDoodadRefs();
            defGroup->CreateBatchMaterials();
            defGroup->m_flags |= 0x40;
        }
    }
}

// OFFSET: 0x00773BD0
void CWorldMap::PreloadCountDoodadDefs(const CAaBox& box, TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, uint32_t* totals, uint32_t* loaded, int32_t raisePriority) {
    if (!totals || !loaded) {
        return;
    }

    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
        next = links.Next(link);
        const CAaBox& modelBox = doodadDef->m_modelBox;

        if (modelBox.t.x < modelBox.b.x || modelBox.t.y < modelBox.b.y || modelBox.t.z < modelBox.b.z || modelBox.t.x < box.b.x || modelBox.t.y < box.b.y || modelBox.t.z < box.b.z || box.t.x < modelBox.b.x || box.t.y < modelBox.b.y || box.t.z < modelBox.b.z) {
            continue;
        }

        totals[3]++;

        if (!(doodadDef->m_flags & 0x40)) {
            continue;
        }

        CM2Model* model = doodadDef->m_model;
        doodadDef->m_entityFlags |= 0x8000;

        if (model->TexturesAllLoaded()) {
            loaded[3]++;
        } else if (raisePriority) {
            doodadDef->m_model->IncreaseTexturePriority();
        }
    }
}

// OFFSET: 0x00773D00
void CWorldMap::PreloadMapObjDefs(const CAaBox& box) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefHash.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefHash.Next(mapObjDef);
        CMapObj* mapObj = mapObjDef->m_mapObj;

        if (mapObj->m_flags & 0x40000) {
            continue;
        }

        const C3Vector& pos = World::GetGuardBandPosition();
        float x = mapObjDef->m_bbox.b.x <= pos.x ? (pos.x <= mapObjDef->m_bbox.t.x ? pos.x : mapObjDef->m_bbox.t.x) : mapObjDef->m_bbox.b.x;
        float y = mapObjDef->m_bbox.b.y <= pos.y ? (pos.y <= mapObjDef->m_bbox.t.y ? pos.y : mapObjDef->m_bbox.t.y) : mapObjDef->m_bbox.b.y;
        float z = mapObjDef->m_bbox.b.z <= pos.z ? (pos.z <= mapObjDef->m_bbox.t.z ? pos.z : mapObjDef->m_bbox.t.z) : mapObjDef->m_bbox.b.z;
        float distSq = (x - pos.x) * (x - pos.x) + (z - pos.z) * (z - pos.z) + (y - pos.y) * (y - pos.y);

        if (mapObj->m_distSq > distSq) {
            mapObj->m_distSq = distSq;
        }

        if (mapObjDef->m_bbox.t.x >= box.b.x && mapObjDef->m_bbox.t.y >= box.b.y && mapObjDef->m_bbox.t.z >= box.b.z && box.t.x >= mapObjDef->m_bbox.b.x && box.t.y >= mapObjDef->m_bbox.b.y && box.t.z >= mapObjDef->m_bbox.b.z && (mapObj->m_flags & 0x40) && !(mapObjDef->m_flags & 0x40)) {
            mapObjDef->UpdateLoaded();
        }

        CWorldMap::PreloadMapObjDefGroups(mapObj, mapObjDef, box);
    }
}

// OFFSET: 0x00773E60
void CWorldMap::PreloadCount(CMap* map, const CAaBox& box, uint32_t* totals, uint32_t* loaded, int32_t raisePriority) {
    C3Vector center((box.t.x + box.b.x) * 0.5f, (box.t.y + box.b.y) * 0.5f, (box.t.z + box.b.z) * 0.5f);
    CMapChunk* chunk = map->GetChunk(center);

    if (chunk) {
        totals[4]++;

        if (chunk->AreTexturesLoaded()) {
            loaded[4]++;
        }

        for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
            next = chunk->m_liquidList.Next(liquid);
            totals[4]++;

            if (Liquid::IsAllLoaded(liquid->m_liquidType)) {
                loaded[4]++;
            }
        }
    }

    for (CMapArea* area = map->m_areaList.Head(), * next; area; area = next) {
        next = map->m_areaList.Next(area);
        totals[0]++;

        if (!(area->m_flags & 0x40)) {
            continue;
        }

        loaded[0]++;

        for (CMapChunk* areaChunk = area->m_chunkList.Head(), * nextChunk; areaChunk; areaChunk = nextChunk) {
            nextChunk = area->m_chunkList.Next(areaChunk);
            CWorldMap::PreloadCountDoodadDefs(box, areaChunk->m_doodadDefLinkList, totals, loaded, raisePriority);
        }
    }

    for (CMapObjDef* mapObjDef = this->m_mapObjDefHash.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefHash.Next(mapObjDef);

        if (box.t.x < mapObjDef->m_bbox.b.x || box.t.y < mapObjDef->m_bbox.b.y || box.t.z < mapObjDef->m_bbox.b.z || mapObjDef->m_bbox.t.x < box.b.x || mapObjDef->m_bbox.t.y < box.b.y || mapObjDef->m_bbox.t.z < box.b.z) {
            continue;
        }

        totals[1]++;

        if (!(mapObjDef->m_flags & 0x40)) {
            continue;
        }

        loaded[1]++;

        for (CMapObjDefGroup* defGroup = mapObjDef->m_defGroupList.Head(), * nextGroup; defGroup; defGroup = nextGroup) {
            nextGroup = mapObjDef->m_defGroupList.Next(defGroup);

            if (box.t.x < defGroup->m_bbox.b.x || box.t.y < defGroup->m_bbox.b.y || box.t.z < defGroup->m_bbox.b.z || defGroup->m_bbox.t.x < box.b.x || defGroup->m_bbox.t.y < box.b.y || defGroup->m_bbox.t.z < box.b.z) {
                continue;
            }

            totals[2]++;

            if (defGroup->m_flags & 0x40) {
                loaded[2]++;
                CWorldMap::PreloadCountMapObjGroupTextures(box, mapObjDef, defGroup, totals, loaded);
                CWorldMap::PreloadCountDoodadDefs(box, defGroup->m_doodadLinks, totals, loaded, raisePriority);
            }
        }
    }

    totals[4]++;

    if (DayNight::IsAllLoaded()) {
        loaded[4]++;
    }
}

// OFFSET: 0x00774150
void CWorldMap::Preload(CMap* map, CWorldMapPreloadData& data, int32_t pass, int32_t phase, float progressStart, float progressRange) {
    int32_t streaming = phase == 0;

    if (!this->m_isGlobalMapObj && !pass) {
        map->PreloadUpdateMap(phase ? data.m_loadRect : data.m_streamRect);
    }

    uint32_t& total = data.m_totals[pass];
    uint32_t& loaded = data.m_loaded[pass];
    const CAaBox& box = data.m_boxes[phase];
    loaded = 0;
    total = 0;

    for (this->PreloadCount(map, box, data.m_totals, data.m_loaded, streaming); loaded < total; this->PreloadCount(map, box, data.m_totals, data.m_loaded, streaming)) {
        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(static_cast<float>(static_cast<int32_t>(loaded)) / static_cast<float>(static_cast<int32_t>(total)) * progressRange + progressStart, World::s_loadProgressParam);
        }

        int32_t urgent = (!pass && streaming) ? 1 : 0;
        AsyncFileReadClearStreamingObjectList();
        map->UpdateAreas(1, urgent);
        CWorldMap::PreloadChunk(map, box);
        this->PreloadMapObjDefs(box);
        CMapObj::Update();
        CWorldMap::PreloadDoodadDefs(box);
        DayNight::StreamingUpdate();
        AsyncFileReadUpdateStreamingObjectList();

        if (!streaming && AsyncFileReadIsStreaming() && !AsyncFileReadIsDiskReading()) {
            break;
        }

        AsyncFileReadPollHandler(NULL, NULL);
        OsSleep(10);

        if (World::GetLoadFailed()) {
            break;
        }

        total = 0;
        loaded = 0;
    }
}

// OFFSET: 0x007742E0/0x14045E210
void CWorldMap::Preload(int32_t purge) {
    if (World::GetScene()) {
        World::GetScene()->PurgeModelCache();
    }

    if (purge) {
        this->PurgeMaps();
    }

    float aoiDist = World::GetGuardBandAoiDist();
    float streamTerrain = 266.66666f;
    float streamObject = 66.666664f;
    float loadTerrain = 1333.3334f;
    float loadObject = 533.33331f;

    if (CWorldParam::cvar_preloadStreamingDistTerrain && CWorldParam::cvar_preloadStreamingDistObject && CWorldParam::cvar_preloadLoadingDistTerrain && CWorldParam::cvar_preloadLoadingDistObject) {
        streamTerrain = static_cast<float>(CWorldParam::cvar_preloadStreamingDistTerrain->m_intValue);
        streamObject = static_cast<float>(CWorldParam::cvar_preloadStreamingDistObject->m_intValue);
        loadTerrain = static_cast<float>(CWorldParam::cvar_preloadLoadingDistTerrain->m_intValue);
        loadObject = static_cast<float>(CWorldParam::cvar_preloadLoadingDistObject->m_intValue);

        if (streamTerrain < 32.0f) {
            streamTerrain = 32.0f;
        }

        if (streamObject < 16.0f) {
            streamObject = 16.0f;
        }

        if (streamTerrain > 512.0f) {
            streamTerrain = 512.0f;
        }

        if (streamObject > 256.0f) {
            streamObject = 256.0f;
        }

        if (loadTerrain < 64.0f) {
            loadTerrain = 64.0f;
        }

        if (loadObject < 32.0f) {
            loadObject = 32.0f;
        }

        if (loadTerrain > 1536.0f) {
            loadTerrain = 1536.0f;
        }

        if (loadObject > 1024.0f) {
            loadObject = 1024.0f;
        }
    }

    if (streamTerrain > aoiDist) {
        streamTerrain = aoiDist;
    }

    if (streamObject > aoiDist) {
        streamObject = aoiDist;
    }

    if (loadTerrain > aoiDist) {
        loadTerrain = aoiDist;
    }

    if (loadObject > aoiDist) {
        loadObject = aoiDist;
    }

    uint32_t mapCount = 0;
    TSStackArray<CWorldMapPreloadData> preloads(static_cast<CWorldMapPreloadData*>(alloca(sizeof(CWorldMapPreloadData) * 12)), 0, 0);

    for (uint32_t i = 0; i < 12; i++) {
        if (!this->m_maps[i]) {
            continue;
        }

        const C3Vector& pos = World::GetGuardBandPosition();
        float x = pos.x;
        float y = pos.y;
        CiRect loadRect;
        loadRect.minY = static_cast<int32_t>(-((x + loadTerrain) - 17066.666f) * 0.029999999f) >> 4;
        loadRect.minX = static_cast<int32_t>(-((y + loadTerrain) - 17066.666f) * 0.029999999f) >> 4;
        loadRect.maxY = static_cast<int32_t>(-((x - loadTerrain) - 17066.666f) * 0.029999999f) >> 4;
        loadRect.maxX = static_cast<int32_t>(-((y - loadTerrain) - 17066.666f) * 0.029999999f) >> 4;
        int32_t streamMaxY = static_cast<int32_t>(-((x - streamTerrain) - 17066.666f) * 0.029999999f) >> 4;
        int32_t streamMaxX = static_cast<int32_t>(-((y - streamTerrain) - 17066.666f) * 0.029999999f) >> 4;

        if (!this->m_maps[i]->PreloadAreasExist(loadRect) && !this->m_isGlobalMapObj) {
            continue;
        }

        mapCount++;
        preloads.SetCount(preloads.m_count + 1);
        CWorldMapPreloadData& data = preloads.m_data[preloads.m_count - 1];
        data.m_mapIndex = i;
        data.m_center = World::GetGuardBandPosition();
        data.m_loadRect = loadRect;
        data.m_streamRect.minY = static_cast<int32_t>(-((x + streamTerrain) - 17066.666f) * 0.029999999f) >> 4;
        data.m_streamRect.minX = static_cast<int32_t>(-((y + streamTerrain) - 17066.666f) * 0.029999999f) >> 4;
        data.m_streamRect.maxY = streamMaxY;
        data.m_streamRect.maxX = streamMaxX;

        CAaBox& streamBox = data.m_boxes[0];
        streamBox.b = data.m_center;
        streamBox.t = data.m_center;
        streamBox.b.x = streamBox.b.x - streamObject;
        streamBox.b.y = streamBox.b.y - streamObject;
        streamBox.b.z = streamBox.b.z - streamObject;
        streamBox.t.x = streamBox.t.x + streamObject;
        streamBox.t.y = streamObject + streamBox.t.y;
        streamBox.t.z = streamObject + streamBox.t.z;
        streamBox.b.z = streamBox.b.z - 1000.0f;
        streamBox.t.z = streamBox.t.z + 10.0f;

        CAaBox& loadBox = data.m_boxes[1];
        loadBox.b = data.m_center;
        loadBox.t = data.m_center;
        loadBox.b.x = loadBox.b.x - loadObject;
        loadBox.b.y = loadBox.b.y - loadObject;
        loadBox.b.z = loadBox.b.z - loadObject;
        loadBox.t.x = loadBox.t.x + loadObject;
        loadBox.t.y = loadBox.t.y + loadObject;
        loadBox.t.z = loadBox.t.z + loadObject;
        loadBox.b.z = loadBox.b.z - 2000.0f;
        loadBox.t.z = loadBox.t.z + 2000.0f;
    }

    if (!mapCount) {
        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(1.0f, World::s_loadProgressParam);
        }

        return;
    }

    LoadingScreenEnableFileBar(true);
    float progress = 0.0f;
    float mapScale = static_cast<float>(1.0 / static_cast<double>(preloads.m_count));

    for (int32_t pass = 0; pass < 5; pass++) {
        float weight = s_preloadPassWeight[pass] * mapScale;

        for (uint32_t i = 0; i < preloads.m_count; i++) {
            CWorldMapPreloadData& data = preloads.m_data[i];
            CMap* map = this->m_maps[data.m_mapIndex];
            this->Preload(map, data, pass, 0, progress, weight * 0.75f);
            this->Preload(map, data, pass, 1, weight * 0.75f + progress, weight * 0.25f);
            progress = weight + progress;

            if (progress >= 1.0f) {
                progress = 1.0f;
            }

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(progress, World::s_loadProgressParam);
            }
        }
    }

    LoadingScreenEnableFileBar(false);

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(1.0f, World::s_loadProgressParam);
    }

    this->Update(0);
    CWorldScene::ClearBarriers();
    AsyncFileReadPollHandler(NULL, NULL);
    OsSleep(10);
    this->m_facetQueryValid = 0;
}
