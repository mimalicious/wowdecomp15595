#include "WoW/Source/WorldClient/Map.h"
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"

// STATIC: 0x00D00D98
TSExplicitList<CMapAreaLow, offsetof(CMapAreaLow, m_renderLink)> CMap::s_mapAreaLowUpdateList;

// STATIC: 0x00D00DA4
TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_link)> CMap::s_mapRenderChunkUpdateList;

// STATIC: 0x00D00DB0
TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_updateLink)> CMap::s_mapChunkLiquidUpdateList;

// STATIC: 0x00D00DC8
TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_updateLink)> CMap::s_doodadDefLoadingList;

// STATIC: 0x00D00DD4
TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_updateLink)> CMap::s_doodadDefLoadedList;

// STATIC: 0x00E74314
CGxShader* CMap::ds_terrain;

// STATIC: 0x00E74318
CGxShader* CMap::hs_terrain;

// STATIC: 0x00E7431C
CGxShader* CMap::vs_terrain;

// STATIC: 0x00E74320
CGxShader* CMap::ps_terrain;

// STATIC: 0x00E74324
int32_t CMap::enableChunkBatching;

// STATIC: 0x00E74328
int32_t CMap::enableTriangleStrips;

// STATIC: 0x00E7432C
int32_t CMap::enableTerrainShaderPixel;

// STATIC: 0x00E74330
int32_t CMap::enableTerrainShaderVertex;

// STATIC: 0x00E74334
int32_t CMap::s_globalUniqueId;

// STATIC: 0x00E74344
CTexture* CMap::s_blackTexture;

// STATIC: 0x00E74348
TSGrowableArray<CGxVertexPC> CMap::s_testQueryVertices;

// STATIC: 0x00E74358
TSGrowableArray<uint16_t> CMap::s_testQueryIndices;

// STATIC: 0x00E8938C
int32_t CMap::s_areasUnloaded;

// STATIC: 0x00E89390
uint32_t CMap::freeCounts[10];

// STATIC: 0x00E893B8
uint32_t CMap::counts[10];

struct CMapAreaSortEntry {
    CMapArea* m_area;
    float m_distSq;
};

// PROVEN: 30adeb3c IDENTICAL/NONE
// OFFSET: 0x007614F0
static int CompareAreaDist(const void* a, const void* b) {
    float distA = static_cast<const CMapAreaSortEntry*>(a)->m_distSq;
    float distB = static_cast<const CMapAreaSortEntry*>(b)->m_distSq;

    if (distA > distB) {
        return -1;
    }

    return distA < distB;
}

// OFFSET: 0x00735A80
uint32_t CMap::GetTerrainVSPerm(int32_t displacement, int32_t lighting, uint32_t layers, uint32_t format, uint32_t material, int32_t shadow, uint32_t clip) {
    uint32_t base = layers + 4 * lighting - 1;
    return clip + 2 * ((displacement != 0) + 2 * ((shadow != 0) + 2 * (material + 2 * (base + format + 2 * base))));
}

// OFFSET: 0x00735AC0/0x140411FC0
void CMap::Initialize() {
    CMapArea::Initialize();
    CMapAreaLow::Initialize();
    CMapChunk::Initialize();
    CMapObj::Initialize();
    CMapObjGroup::Initialize();
    CDetailDoodad::Initialize();
    CMapChunkBuf::Initialize();
    memset(CMap::counts, 0, sizeof(CMap::counts));
    memset(CMap::freeCounts, 0, sizeof(CMap::freeCounts));
    CMap::s_globalUniqueId = -2;
    CMap::vs_terrain = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Terrain");
    CMap::hs_terrain = g_theGxDevicePtr->ShaderCreate(GxSh_Hull, "Terrain");
    CMap::ds_terrain = g_theGxDevicePtr->ShaderCreate(GxSh_Domain, "Terrain");
    CMap::ps_terrain = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "Terrain");

    CImVector black;
    black.b = 0;
    black.g = 0;
    black.r = 0;
    black.a = 0xFF;
    CMap::s_blackTexture = TextureCreateSolid(black);

    CMap::MapMemInitialize();
    CMap::enableTriangleStrips = 0;
}

// OFFSET: 0x00735BF0
void CMap::Destroy() {
    CDetailDoodad::DestroyPools();
    CMapRenderChunk::DestroyPools();
    CMapObj::DestroyPools();
    CMapChunkBuf::Destroy();
    CMapObj::Destroy();
    CMapChunk::Destroy();
    CMapArea::Destroy();
    CDetailDoodad::Destroy();
    CMapAreaLow::Destroy();

    if (CMap::vs_terrain) {
        g_theGxDevicePtr->ShaderRelease(CMap::vs_terrain);
        CMap::vs_terrain = NULL;
    }

    if (CMap::hs_terrain) {
        g_theGxDevicePtr->ShaderRelease(CMap::hs_terrain);
        CMap::hs_terrain = NULL;
    }

    if (CMap::ds_terrain) {
        g_theGxDevicePtr->ShaderRelease(CMap::ds_terrain);
        CMap::ds_terrain = NULL;
    }

    if (CMap::ps_terrain) {
        g_theGxDevicePtr->ShaderRelease(CMap::ps_terrain);
        CMap::ps_terrain = NULL;
    }

    if (CMap::s_blackTexture) {
        TextureRelease(CMap::s_blackTexture);
        CMap::s_blackTexture = NULL;
    }

    CMap::MapMemDestroy();
}

// PROVEN: 792fbef9 IDENTICAL/NONE
// OFFSET: 0x00735CB0
uint32_t CMap::GetTerrainPSPerm(uint32_t materialType, uint32_t layers, uint32_t shadowMode) {
    return materialType + 6 * (layers + 4 * (CShaderEffect::s_usePcfFiltering + 2 * shadowMode) - 1);
}

// OFFSET: 0x00735CE0
void CMap::ClearChunkDetailDoodads() {
    for (CMapArea* area = this->m_areaList.Head(); area; area = this->m_areaList.Next(area)) {
        for (CMapChunk* chunk = area->m_chunkList.Head(); chunk; chunk = area->m_chunkList.Next(chunk)) {
            if (chunk->m_detailDoodadInst) {
                CDetailDoodad::FreeInst(chunk->m_detailDoodadInst);
                chunk->m_detailDoodadInst = NULL;
            }
        }
    }
}

// OFFSET: 0x00735D50
void CMap::ClearRenderChunks() {
    for (CMapArea* area = this->m_areaList.Head(); area; area = this->m_areaList.Next(area)) {
        for (CMapChunk* chunk = area->m_chunkList.Head(); chunk; chunk = area->m_chunkList.Next(chunk)) {
            if (chunk->m_renderChunk) {
                CMap::FreeRenderChunk(chunk->m_renderChunk);
                chunk->m_renderChunk = NULL;
            }
        }
    }
}

// OFFSET: 0x00735DC0/0x140412310
void CMap::ClearChunkBufs() {
    for (CMapArea* area = this->m_areaList.Head(); area; area = this->m_areaList.Next(area)) {
        for (CMapChunk* chunk = area->m_chunkList.Head(); chunk; chunk = area->m_chunkList.Next(chunk)) {
            if (chunk->m_renderChunk) {
                chunk->m_renderChunk->FreeBuf();
            }
        }
    }
}

// OFFSET: 0x00735E30
CMap::~CMap() {
}

// OFFSET: 0x00735F50
CMap::CMap() {
    this->m_mapFlags &= ~0xFFu;
    this->m_type |= 0x8;
    this->m_worldMap = NULL;
    this->m_globalMapObjDef = NULL;
    this->m_mapId = -1;
}

// OFFSET: 0x007616E0
void CMap::UpdateArea(CMapArea* area, const CiRect& rect, int32_t depth) {
    if (rect.minX > World::gbChunkRect.maxX || rect.minY > World::gbChunkRect.maxY || rect.maxX < World::gbChunkRect.minX || rect.maxY < World::gbChunkRect.minY) {
        area->PurgeChunks(rect);
        return;
    }

    if (depth == 2) {
        area->Update(rect);
        return;
    }

    CiRect quad;

    quad.minY = rect.minY;
    quad.minX = rect.minX;
    quad.maxY = rect.minY + ((rect.maxY - rect.minY) >> 1);
    quad.maxX = rect.minX + ((rect.maxX - rect.minX) >> 1);
    this->UpdateArea(area, quad, depth + 1);

    quad.minX = ((rect.maxX - rect.minX) >> 1) + rect.minX + 1;
    quad.maxX = rect.maxX;
    this->UpdateArea(area, quad, depth + 1);

    quad.minY = ((rect.maxY - rect.minY) >> 1) + rect.minY + 1;
    quad.maxY = rect.maxY;
    this->UpdateArea(area, quad, depth + 1);

    quad.minX = rect.minX;
    quad.maxX = rect.minX + ((rect.maxX - rect.minX) >> 1);
    this->UpdateArea(area, quad, depth + 1);
}

// OFFSET: 0x00761800
void CMap::UpdateLowDetailMapObjDefs() {
    for (uint32_t i = 0; i < this->m_lowDetail.m_lowDetailMapObjDefs.m_count; i++) {
        CMapObjDef* mapObjDef = this->m_lowDetail.m_lowDetailMapObjDefs.m_data[i];
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

        float distSq = (x - pos.x) * (x - pos.x) + (y - pos.y) * (y - pos.y) + (z - pos.z) * (z - pos.z);

        if (mapObj->m_flushTimer > distSq) {
            mapObj->m_flushTimer = distSq;
        }

        if (mapObj->m_flags & 0x40) {
            mapObjDef->m_flags |= 0x40;
            uint32_t groupCount = mapObj->m_groupCount;

            for (uint32_t groupNum = 0; groupNum < groupCount; groupNum++) {
                CMapObjGroup* group = mapObj->GetGroup(groupNum, 1);

                if (group->m_flags & 0x40000) {
                    continue;
                }

                if (!(group->m_flags & 0x40) && !group->m_asyncObject) {
                    mapObj->ReadGroup(groupNum);
                }

                group->m_frameStamp = World::GetFrameCnt();

                if (group->m_distSq > distSq) {
                    group->m_distSq = distSq;
                }
            }
        }
    }
}

// OFFSET: 0x00761AB0
void CMap::PreUpdateAreas() {
    int32_t minX = World::gbChunkRect.minX >> 4;
    int32_t maxX = World::gbChunkRect.maxX >> 4;
    int32_t maxY = World::gbChunkRect.maxY >> 4;
    int32_t minY = World::gbChunkRect.minY >> 4;
    int32_t count = 0;
    CMapAreaSortEntry* sorted = static_cast<CMapAreaSortEntry*>(alloca(0x2000));

    for (CMapArea* area = this->m_areaList.Head(), * next; area; area = next) {
        next = this->m_areaList.Next(area);

        if (area->m_indexX >= minX && area->m_indexX <= maxX && area->m_indexY >= minY && area->m_indexY <= maxY) {
            float posX = World::GetGuardBandPosition().x;
            const C3Vector& pos = World::GetGuardBandPosition();
            float posY = pos.y;

            float x = area->m_bbox.b.x;
            if (x <= posX) {
                x = area->m_bbox.t.x;

                if (posX <= x) {
                    x = posX;
                }
            }

            float y = area->m_bbox.b.y;
            if (y <= posY) {
                y = area->m_bbox.t.y;

                if (posY <= y) {
                    y = pos.y;
                }
            }

            area->m_distSq = (y - posY) * (y - posY) + (x - posX) * (x - posX);
            sorted[count].m_area = area;
            sorted[count].m_distSq = area->m_distSq;
            count++;
        } else {
            area->PurgeXXX();
            CMap::FreeArea(area);
        }
    }

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMapAreaTableEntry* entry = this->m_areaTable.Get(x, y);
            uint32_t flags = entry ? entry->m_flags : 0;

            if (!(flags & 0x1)) {
                continue;
            }

            entry = this->m_areaTable.Get(x, y);

            if (entry && entry->m_area) {
                continue;
            }

            CMapArea* area = this->PrepareArea(x, y);
            float posX = World::GetGuardBandPosition().x;
            const C3Vector& pos = World::GetGuardBandPosition();
            float posY = pos.y;

            float ax = area->m_bbox.b.x;
            if (ax <= posX) {
                ax = area->m_bbox.t.x;

                if (posX <= ax) {
                    ax = posX;
                }
            }

            float ay = area->m_bbox.b.y;
            if (ay <= posY) {
                ay = area->m_bbox.t.y;

                if (posY <= ay) {
                    ay = pos.y;
                }
            }

            area->m_distSq = (ax - posX) * (ax - posX) + (ay - posY) * (ay - posY);
            sorted[count].m_area = area;
            sorted[count].m_distSq = area->m_distSq;
            count++;
        }
    }

    if (count) {
        qsort(sorted, count, sizeof(CMapAreaSortEntry), CompareAreaDist);
    }

    for (int32_t i = 0; i < count; i++) {
        CMapArea* area = sorted[i].m_area;
        area->m_mapLink.Unlink();
        this->m_areaList.LinkToHead(area);
    }
}

// OFFSET: 0x00761F20
void CMap::ProcessAreaLowUpdateList() {
    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11 || g_theGxDevicePtr->DeviceApi() == GxApi_GLL) {
        return;
    }

    for (CMapAreaLow* area = CMap::s_mapAreaLowUpdateList.Head(), * next; area; area = next) {
        next = CMap::s_mapAreaLowUpdateList.Next(area);

        if (World::GetFrameCnt() - area->m_lastUsedFrame <= 120) {
            break;
        }

        area->FreeBuf();
    }
}

// OFFSET: 0x00761F80
void CMap::ProcessChunkLiquidUpdateList() {
    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11 || g_theGxDevicePtr->DeviceApi() == GxApi_GLL) {
        return;
    }

    for (CChunkLiquid* liquid = CMap::s_mapChunkLiquidUpdateList.Head(), * next; liquid; liquid = next) {
        next = CMap::s_mapChunkLiquidUpdateList.Next(liquid);

        if (World::GetFrameCnt() - liquid->m_frameStamp <= 120) {
            break;
        }

        liquid->UpdateResourcePurging();
        liquid->m_updateLink.Unlink();
    }
}

// OFFSET: 0x00762020
void CMap::ProcessRenderChunkUpdateList() {
    if (g_theGxDevicePtr->DeviceApi() == GxApi_D3d11 || g_theGxDevicePtr->DeviceApi() == GxApi_GLL) {
        return;
    }

    for (CMapRenderChunk* renderChunk = CMap::s_mapRenderChunkUpdateList.Head(), * next; renderChunk; renderChunk = next) {
        next = CMap::s_mapRenderChunkUpdateList.Next(renderChunk);

        if (World::GetFrameCnt() - renderChunk->m_lastUsedFrame <= 120) {
            break;
        }

        renderChunk->FreeBuf();
        renderChunk->m_link.Unlink();
    }
}

// OFFSET: 0x007620C0
void CMap::ProcessDoodadDefUpdateList() {
    int32_t streaming = AsyncFileReadIsStreaming();
    C3Vector guardBand = World::GetGuardBandPosition();

    for (CMapDoodadDef* doodadDef = CMap::s_doodadDefLoadedList.Head(), * next; doodadDef; doodadDef = next) {
        next = CMap::s_doodadDefLoadedList.Next(doodadDef);
        uint16_t fade = doodadDef->m_fade;

        if (fade >= 0xFF00) {
            doodadDef->m_updateLink.Unlink();
            continue;
        }

        uint32_t value = (FrameTime::GetTickTimeMs() << 7) + fade;

        if (value > 0xFF00) {
            value = 0xFF00;
        }

        doodadDef->m_fade = static_cast<uint16_t>(value);
    }

    CMapDoodadDef* best = NULL;
    int32_t bestPriority = 0x7FFFFFFF;

    for (CMapDoodadDef* doodadDef = CMap::s_doodadDefLoadingList.Head(), * next; doodadDef; doodadDef = next) {
        next = CMap::s_doodadDefLoadingList.Next(doodadDef);

        if (doodadDef->m_model->IsLoaded()) {
            doodadDef->UpdateLoaded();
            const CAaBox& bounds = World::mapObjDefBounds;

            if (doodadDef->m_bbox.t.x < bounds.b.x || doodadDef->m_bbox.t.y < bounds.b.y || doodadDef->m_bbox.t.z < bounds.b.z || bounds.t.x < doodadDef->m_bbox.b.x || bounds.t.y < doodadDef->m_bbox.b.y || bounds.t.z < doodadDef->m_bbox.b.z) {
                doodadDef->m_fade = 0xFF00;
                doodadDef->m_updateLink.Unlink();
            } else {
                doodadDef->m_fade = 0;
                CMap::s_doodadDefLoadedList.LinkToTail(doodadDef);
            }

            continue;
        }

        if (!streaming) {
            continue;
        }

        const CAaBox& box = doodadDef->m_modelBox;
        float distSq;

        if (box.t.x < box.b.x || box.t.y < box.b.y || box.t.z < box.b.z) {
            const C3Vector& pos = doodadDef->m_position;
            distSq = (pos.z - guardBand.z) * (pos.z - guardBand.z) + (pos.y - guardBand.y) * (pos.y - guardBand.y) + (pos.x - guardBand.x) * (pos.x - guardBand.x);
        } else {
            float x = box.b.x;

            if (x <= guardBand.x) {
                x = box.t.x;

                if (guardBand.x <= x) {
                    x = guardBand.x;
                }
            }

            float y = box.b.y;

            if (y <= guardBand.y) {
                y = box.t.y;

                if (guardBand.y <= y) {
                    y = guardBand.y;
                }
            }

            float z = box.b.z;

            if (z <= guardBand.z) {
                z = box.t.z;

                if (guardBand.z <= z) {
                    z = guardBand.z;
                }
            }

            distSq = (z - guardBand.z) * (z - guardBand.z) + (y - guardBand.y) * (y - guardBand.y) + (x - guardBand.x) * (x - guardBand.x);
        }

        if (doodadDef->m_entityFlags & 0x8000) {
            int32_t priority = AsyncFileReadComputePriority(0, 0, 1, distSq);
            AsyncFileReadAddStreamingObject(CMapDoodadDef::StreamingCallback, doodadDef, priority);
            doodadDef->m_entityFlags &= ~0x8000u;
        } else {
            int32_t category = 4;

            if (box.t.x >= box.b.x && box.t.y >= box.b.y && box.t.z >= box.b.z) {
                category = 2;
            }

            int32_t priority = AsyncFileReadComputePriority(category, 0, 1, distSq);

            if (priority < bestPriority) {
                best = doodadDef;
                bestPriority = priority;
            }
        }

        if (distSq >= 100.0f) {
            continue;
        }

        CAaBox barrier;
        barrier.b.x = box.b.x;
        barrier.b.y = box.b.y;
        barrier.b.z = box.b.z - 0.5f;
        barrier.t.x = box.t.x;
        barrier.t.y = box.t.y;
        barrier.t.z = box.t.z + 1.0f;
        CWorldScene::s_barrier.AddBarrier(barrier, 10.0f);
    }

    if (best) {
        AsyncFileReadAddStreamingObject(CMapDoodadDef::StreamingCallback, best, bestPriority);
    }
}

// PROVEN: 8e810a58 IDENTICAL/NONE
// OFFSET: 0x00762580
void CMap::PreUpdateMap() {
    this->PreUpdateAreas();
}

// OFFSET: 0x00761DE0
void CMap::UpdateAreas(int32_t noDistance, int32_t urgent) {
    CiRect rect;

    for (CMapArea* area = this->m_areaList.Head(), * next; area; area = next) {
        next = this->m_areaList.Next(area);

        if (area->m_areaFlags & 0x2) {
            this->LoadArea(area);
        }

        rect.minX = area->m_chunkOffsetX;
        rect.minY = area->m_chunkOffsetY;
        rect.maxX = area->m_chunkOffsetX + 15;
        rect.maxY = area->m_chunkOffsetY + 15;

        if ((rect.minX > World::chunkRect.maxX || rect.minY > World::chunkRect.maxY || rect.maxX < World::chunkRect.minX || rect.maxY < World::chunkRect.minY) && area->IsLoading()) {
            CMap::s_areasUnloaded = 1;
        }

        if (area->m_flags & 0x40) {
            this->UpdateArea(area, rect, 0);
        } else if (area->IsLoading()) {
            int32_t category = 2;
            float distSq;

            if (noDistance) {
                distSq = 10000.0f;
            } else {
                float dist = World::GetObjectAoiDist();
                distSq = dist * dist;
            }

            float areaDistSq = area->m_distSq;

            if (distSq > areaDistSq || urgent) {
                category = 0;
            }

            int32_t priority = AsyncFileReadComputePriority(category, 0, 1, areaDistSq);
            AsyncFileReadAddStreamingObject(CMapArea::AsyncPriorityCallback, area, priority);
        }
    }
}

// OFFSET: 0x00762590
void CMap::UpdateMap() {
    this->UpdateAreas(0, 0);

    for (int32_t y = World::detailDoodadChunkRectOld.minY; y <= World::detailDoodadChunkRectOld.maxY; y++) {
        for (int32_t x = World::detailDoodadChunkRectOld.minX; x <= World::detailDoodadChunkRectOld.maxX; x++) {
            if (x < World::detailDoodadChunkRect.minX || x > World::detailDoodadChunkRect.maxX || y < World::detailDoodadChunkRect.minY || y > World::detailDoodadChunkRect.maxY) {
                CMapAreaTableEntry* entry = this->m_areaTable.Get(x >> 4, y >> 4);

                if (entry && (entry->m_flags & 0x2) && entry->m_area) {
                    CMapChunk* chunk = entry->m_area->m_chunks[(x & 0xF) + 16 * (y & 0xF)];

                    if (chunk && chunk->m_detailDoodadInst) {
                        CDetailDoodad::FreeInst(chunk->m_detailDoodadInst);
                        chunk->m_detailDoodadInst = NULL;
                    }
                }
            }
        }
    }
}

// OFFSET: 0x0077DD80
void CMap::WaterReset() {
    for (CMapArea* area = this->m_areaList.Head(); area; area = this->m_areaList.Next(area)) {
        for (CMapChunk* chunk = area->m_chunkList.Head(); chunk; chunk = area->m_chunkList.Next(chunk)) {
            chunk->CreateLiquids(0);
        }
    }

    for (CMapAreaLow* areaLow = this->m_lowDetail.m_areaList.Head(); areaLow; areaLow = this->m_lowDetail.m_areaList.Next(areaLow)) {
        areaLow->LiquidReset();
    }
}

// OFFSET: 0x0077F820
void CMap::TestQueryReset() {
    CMap::s_testQueryVertices.SetCount(0);
    CMap::s_testQueryIndices.m_count = 0;
}

// OFFSET: 0x00786520/0x140475670
void CMap::UnloadXXX() {
    this->Purge();
    this->m_lowDetail.Unload();

    if (this->m_globalMapObjDef) {
        this->m_globalMapObjDef->m_refCount--;
        this->m_globalMapObjDef->Purge(1);
        this->m_globalMapObjDef = NULL;
    }

    this->m_mapFlags &= ~Flag_GlobalMapObj;

    char blobName[260];
    strcpy(blobName, this->m_wdtFileName);
    strcpy(strrchr(blobName, '.'), ".tex");
    TextureUnloadBlob(blobName);
}

// OFFSET: 0x007869D0/0x140475D00
void CMap::Purge() {
    for (CMapArea* area = this->m_areaList.Head(), * next; area; area = next) {
        next = this->m_areaList.Next(area);
        area->PurgeXXX();
        CMap::FreeArea(area);
    }

    this->m_lowDetail.Purge();
}

// OFFSET: 0x0078C9C0
void CMap::TestQueryAdd(const CFacet& facet, CImVector color, const C44Matrix* mat) {
    uint16_t base = static_cast<uint16_t>(CMap::s_testQueryVertices.Count());
    C44Matrix identity;

    if (!mat) {
        mat = &identity;
    }

    for (uint32_t i = 0; i < 3; i++) {
        CGxVertexPC* vertex = CMap::s_testQueryVertices.New();
        vertex->p = facet.v[i] * *mat;
        vertex->c = color;
    }

    *CMap::s_testQueryIndices.New() = base;
    *CMap::s_testQueryIndices.New() = base + 1;
    *CMap::s_testQueryIndices.New() = base + 2;
}

// OFFSET: 0x0079D470
CMapArea* CMap::PrepareArea(int32_t x, int32_t y) {
    CMapArea* area = CMap::AllocArea();
    this->m_areaList.LinkToTail(area);
    area->m_map = this;
    area->m_worldMap = this->m_worldMap;
    area->m_indexY = y;
    area->m_chunkOffsetY = 16 * y;
    area->m_indexX = x;
    area->m_chunkOffsetX = 16 * x;
    area->m_posX = static_cast<float>(16 * y) * -33.333332f + 17066.666f;
    area->m_posY = static_cast<float>(16 * x) * -33.333332f + 17066.666f;
    area->InitBounds();
    this->m_areaTable.Get(area->m_indexX, area->m_indexY)->m_area = area;

    if (!(this->m_mapFlags & Flag_Current)) {
        return area;
    }

    CWorldMap* worldMap = this->m_worldMap;
    CMap* map;

    if (worldMap->m_isGlobalMapObj) {
        map = worldMap->m_maps[0];
    } else {
        CWorldMap::CMapTableEntry* tableEntry = worldMap->m_mapTable.Get(x, y);

        if (!tableEntry || !(tableEntry->m_flags & 1)) {
            return area;
        }

        map = worldMap->m_maps[tableEntry->m_mapIndex];
    }

    if (map) {
        area->m_areaFlags |= 0x1;
    }

    return area;
}
