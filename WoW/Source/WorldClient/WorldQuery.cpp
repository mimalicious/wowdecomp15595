#include "WoW/Source/WorldClient/WorldQuery.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/ShadowQuery.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x0079D5F0/0x1404923F0
void CWorldQuery::FreeMapObjGroupList() {
    if (this->m_mapObjGroups) {
        SMemFree(this->m_mapObjGroups, __FILE__, __LINE__, 0);
        this->m_mapObjGroups = NULL;
    }

    this->m_mapObjGroupCount = 0;
    this->m_mapObjGroupMax = 0;
}

// OFFSET: 0x0079D620/0x140492440
void CWorldQuery::FreeModelList() {
    if (this->m_models) {
        SMemFree(this->m_models, __FILE__, __LINE__, 0);
        this->m_models = NULL;
    }

    this->m_modelCount = 0;
    this->m_modelMax = 0;
}

// OFFSET: 0x0079D650/0x140492490
void CWorldQuery::AllocMapObjGroupList(uint32_t count) {
    if (!this->m_mapObjGroups) {
        CWorldQueryElement* groups = static_cast<CWorldQueryElement*>(SMemAlloc(sizeof(CWorldQueryElement) * count, __FILE__, __LINE__, 0));
        this->m_mapObjGroupMax = count;
        this->m_mapObjGroups = groups;
    }

    this->m_mapObjGroupCount = 0;
}

// OFFSET: 0x0079D690/0x1404924F0
CWorldQuery::CWorldQuery() {
    this->m_untexturedBatches.m_batches = NULL;
    this->m_untexturedBatches.m_count = 0;
    this->m_untexturedBatches.m_maxCount = 0;
    this->m_untexturedBatches.m_optimizedCount = 0;
    this->m_texturedBatches.m_batches = NULL;
    this->m_texturedBatches.m_count = 0;
    this->m_texturedBatches.m_maxCount = 0;
    this->m_texturedBatches.m_optimizedCount = 0;
    this->m_mapObjGroups = NULL;
    this->m_mapObjGroupCount = 0;
    this->m_mapObjGroupMax = 0;
    this->m_models = NULL;
    this->m_modelCount = 0;
    this->m_modelMax = 0;
}

// OFFSET: 0x0079D6D0/0x140492530
void CWorldQuery::FreeModelBatchList() {
    if (this->m_untexturedBatches.m_batches) {
        SMemFree(this->m_untexturedBatches.m_batches, "delete[]", -1, 0);
        this->m_untexturedBatches.m_batches = NULL;
    }

    this->m_untexturedBatches.m_count = 0;
    this->m_untexturedBatches.m_maxCount = 0;

    if (this->m_texturedBatches.m_batches) {
        SMemFree(this->m_texturedBatches.m_batches, "delete[]", -1, 0);
        this->m_texturedBatches.m_batches = NULL;
    }

    this->m_texturedBatches.m_count = 0;
    this->m_texturedBatches.m_maxCount = 0;
}

// OFFSET: 0x0079D720/0x1404925A0
void CWorldQuery::AllocModelBatchList(uint32_t count) {
    if (!this->m_untexturedBatches.m_batches) {
        this->m_untexturedBatches.m_batches = static_cast<CModelBatch*>(SMemAlloc((static_cast<uint64_t>(count) * sizeof(CModelBatch)) >> 32 ? 0xFFFFFFFF : count * sizeof(CModelBatch), __FILE__, __LINE__, 0));
        this->m_untexturedBatches.m_maxCount = count;
    }

    this->m_untexturedBatches.m_count = 0;
    this->m_untexturedBatches.m_optimizedCount = 0;

    if (!this->m_texturedBatches.m_batches) {
        this->m_texturedBatches.m_batches = static_cast<CModelBatch*>(SMemAlloc((static_cast<uint64_t>(count) * sizeof(CModelBatch)) >> 32 ? 0xFFFFFFFF : count * sizeof(CModelBatch), __FILE__, __LINE__, 0));
        this->m_texturedBatches.m_maxCount = count;
    }

    this->m_texturedBatches.m_count = 0;
    this->m_texturedBatches.m_optimizedCount = 0;
}

// OFFSET: 0x0079D7B0/0x140492640
CWorldQuery::~CWorldQuery() {
    this->FreeMapObjGroupList();
    this->FreeModelList();
    this->FreeModelBatchList();
}

// OFFSET: 0x0079D830
void CWorldQuery::AddMapObjGroup(CMapObjGroup* group, const C44Matrix* matrix) {
    if (this->m_mapObjGroupCount < this->m_mapObjGroupMax) {
        this->m_mapObjGroups[this->m_mapObjGroupCount].m_group = group;
        this->m_mapObjGroups[this->m_mapObjGroupCount].m_matrix = matrix;
        this->m_mapObjGroupCount++;
    }
}

// OFFSET: 0x0079D860/0x1404926B0
int32_t CWorldView::IsBeyondFadeInner(const C3Vector& pos, uint32_t sizeCategory) {
    float dx = pos.x - this->m_camPos.x;
    float dy = pos.y - this->m_camPos.y;
    return dx * dx + dy * dy + (pos.z - this->m_camPos.z) * (pos.z - this->m_camPos.z) > CWorldView::s_fadeInnerSqr[sizeCategory];
}

// OFFSET: 0x0079D8D0/0x140492710
void CWorldMap::ShadowQueryEntitys(CShadowQuery& query, CShadowCacheQueryData& data, int32_t startBand, int32_t endBand) {
    float minRadius[3];
    float maxRadius[3];

    for (int32_t i = 0; i < 3; i++) {
        CShadowQuery::GetSize(data, i, minRadius[i], maxRadius[i]);
    }

    for (CMapEntity* entity = this->m_entityList.Head(), * next; entity; entity = next) {
        next = this->m_entityList.Next(entity);

        if ((entity->m_flags & 0x10010) || !entity->m_model) {
            continue;
        }

        for (int32_t band = startBand; band <= endBand; band++) {
            uint32_t queryFlags = data.m_queryFlags[band];

            if (((queryFlags & 0x6) == 0x4 || (queryFlags & 0x1)) && (entity->m_entityFlags & 0x1000)) {
                continue;
            }

            if (queryFlags & 0x1) {
                if ((entity->m_flags & 0x1) || !(entity->m_position.z <= query.m_entityCutoff)) {
                    continue;
                }
            } else if (entity->m_flags & 0x2) {
                if (!(entity->m_entityFlags & 0x400) || !(entity->m_position.z <= query.m_entityCutoff)) {
                    continue;
                }
            }

            CM2Model* model = entity->m_model;
            uint32_t wanted = model->IsLoaded() && model->m_shared->m_billboardBoneCount ? queryFlags & 0x4 : queryFlags & 0x2;

            if (!wanted || !data.m_extents[band].Intersects(entity->m_bbox)) {
                continue;
            }

            float radius = entity->m_sphere.r;

            if (minRadius[band] <= radius && ((entity->m_flags & 0x8000) || radius <= maxRadius[band]) && data.unk_04EC[band].Cull(entity->m_bbox) && (!data.m_useCullFrustum[band] || data.m_cullFrustums[band].CullInv(entity->m_bbox) != 3)) {
                model->GatherModelBatchesShadowMap(query.m_queries[band].m_untexturedBatches, query.m_queries[band].m_texturedBatches);
            }
        }
    }
}

// OFFSET: 0x0079DB40/0x1404929C0
void CWorldMap::ShadowQueryMapObjDef(CMapObjDef* mapObjDef, CShadowQuery& query, CShadowCacheQueryData& data, int32_t startBand, int32_t endBand) {
    CWorldView* view = query.m_worldView;

    if (!data.m_extents[endBand].Intersects(mapObjDef->m_bbox) || !data.unk_04EC[endBand].Cull(mapObjDef->m_bbox)) {
        return;
    }

    uint32_t count = mapObjDef->m_defGroups.m_count == 0xFFFFFFFF ? mapObjDef->m_defGroups.Dynamic().Count() : mapObjDef->m_defGroups.m_count;

    for (uint32_t i = 0; i < count; i++) {
        CMapObjDefGroup* defGroup = mapObjDef->m_defGroups[i];
        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(i, 0);

        if (!group || !(group->m_groupFlags & 0x48)) {
            continue;
        }

        if (!data.m_extents[endBand].Intersects(defGroup->m_bbox) || !data.unk_04EC[endBand].Cull(defGroup->m_bbox)) {
            continue;
        }

        for (int32_t band = startBand; band <= endBand; band++) {
            if (!data.m_extents[band].Intersects(defGroup->m_bbox) || !data.unk_04EC[band].Cull(defGroup->m_bbox) || (data.m_useCullFrustum[band] && data.m_cullFrustums[band].CullInv(defGroup->m_bbox) == 3)) {
                continue;
            }

            if (query.m_queries[band].m_mapObjGroupCount >= 0x800) {
                break;
            }

            query.m_queries[band].AddMapObjGroup(group, &mapObjDef->m_mat);
        }

        for (CMapBaseObjLink* link = defGroup->m_doodadLinks.Head(), * next; link; link = next) {
            next = defGroup->m_doodadLinks.Next(link);
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
            uint32_t flags = doodadDef->m_flags;

            if ((flags & 0x10) || !doodadDef->m_model || !(flags & 0x40)) {
                continue;
            }

            for (int32_t band = startBand; band <= endBand; band++) {
                if (doodadDef->m_shadowFrame[band] == query.m_frame) {
                    continue;
                }

                doodadDef->m_shadowFrame[band] = query.m_frame;

                if ((doodadDef->m_sphere.c.z - view->m_camPos.z) * (doodadDef->m_sphere.c.z - view->m_camPos.z) + (doodadDef->m_sphere.c.y - view->m_camPos.y) * (doodadDef->m_sphere.c.y - view->m_camPos.y) + (doodadDef->m_sphere.c.x - view->m_camPos.x) * (doodadDef->m_sphere.c.x - view->m_camPos.x) <= CWorldView::s_fadeInnerSqr[doodadDef->m_sizeCategory] && data.m_extents[band].Intersects(doodadDef->m_bbox) && data.unk_04EC[band].Cull(doodadDef->m_bbox) && (!data.m_useCullFrustum[band] || data.m_cullFrustums[band].CullInv(doodadDef->m_bbox) != 3)) {
                    doodadDef->m_model->GatherModelBatchesShadowMap(query.m_queries[band].m_untexturedBatches, query.m_queries[band].m_texturedBatches);
                }
            }
        }
    }
}

// OFFSET: 0x0079DF90/0x140492F30
void CWorldMap::ShadowQueryMapObjDef(CMapObjDef* mapObjDef, CShadowQuery& query, CShadowCacheQueryData& data, int32_t band, float maxRadius) {
    int32_t global = 0;
    int32_t textured = 0;
    int32_t untextured = 0;

    if (mapObjDef->m_flags & 0x200) {
        if (!(data.m_queryFlags[band] & 0x10)) {
            return;
        }

        textured = 1;
        untextured = 1;
        global = 1;
        maxRadius = 100000.0f;
    } else {
        uint32_t queryFlags = data.m_queryFlags[band];

        if (queryFlags & 0x8) {
            untextured = 1;
        }

        if (queryFlags & 0x10) {
            textured = 1;
        }
    }

    if (!data.m_extents[band].Intersects(mapObjDef->m_bbox) || !data.unk_04EC[band].Cull(mapObjDef->m_bbox) || (data.m_useCullFrustum[band] && data.m_cullFrustums[band].CullInv(mapObjDef->m_bbox) == 3)) {
        return;
    }

    CWorldView* view = query.m_worldView;
    uint32_t count = mapObjDef->m_defGroups.m_count == 0xFFFFFFFF ? mapObjDef->m_defGroups.Dynamic().Count() : mapObjDef->m_defGroups.m_count;

    for (uint32_t i = 0; i < count; i++) {
        CMapObjDefGroup* defGroup = mapObjDef->m_defGroups[i];
        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(i, 0);

        if (!group || !(group->m_groupFlags & 0x48)) {
            continue;
        }

        if (!data.m_extents[band].Intersects(defGroup->m_bbox) || !data.unk_04EC[band].Cull(defGroup->m_bbox) || (data.m_useCullFrustum[band] && data.m_cullFrustums[band].CullInv(defGroup->m_bbox) == 3)) {
            continue;
        }

        if (global ? (data.m_queryFlags[band] & 0x10) : (data.m_queryFlags[band] & 0x8)) {
            if (query.m_queries[band].m_mapObjGroupCount >= 0x800) {
                return;
            }

            query.m_queries[band].AddMapObjGroup(group, &mapObjDef->m_mat);
        }

        for (CMapBaseObjLink* link = defGroup->m_doodadLinks.Head(), * next; link; link = next) {
            next = defGroup->m_doodadLinks.Next(link);
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

            if (doodadDef->m_shadowFrame[band] == query.m_frame) {
                continue;
            }

            doodadDef->m_shadowFrame[band] = query.m_frame;
            CM2Model* model = doodadDef->m_model;

            if ((doodadDef->m_flags & 0x10) || !model || !(doodadDef->m_flags & 0x40)) {
                continue;
            }

            if (view->IsBeyondFadeInner(doodadDef->m_sphere.c, doodadDef->m_sizeCategory) || doodadDef->m_sphere.r > maxRadius) {
                continue;
            }

            int32_t wanted = model->IsLoaded() && model->m_shared->m_billboardBoneCount ? textured : untextured;

            if (wanted && data.m_extents[band].Intersects(doodadDef->m_bbox) && data.unk_04EC[band].Cull(doodadDef->m_bbox) && (!data.m_useCullFrustum[band] || data.m_cullFrustums[band].CullInv(doodadDef->m_bbox) != 3)) {
                model->GatherModelBatchesShadowMap(query.m_queries[band].m_untexturedBatches, query.m_queries[band].m_texturedBatches);
            }
        }
    }
}

// OFFSET: 0x0079E340/0x140493390
void CWorldMap::ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, int32_t startBand, int32_t endBand) {
    CWorldMap::s_queryTag++;
    CiRect rect(0, 0, 0, 0);
    CWorldMap::GetChunkRect(data.m_extents[data.m_lastBand], rect);
    CiRect sub(0, 0, 0, 0);

    for (int32_t y = rect.minY >> 4; y <= rect.maxY >> 4; y++) {
        sub.minY = 16 * y <= rect.minY ? rect.minY : 16 * y;
        sub.maxY = 16 * y + 15 >= rect.maxY ? rect.maxY : 16 * y + 15;

        for (int32_t x = rect.minX >> 4; x <= rect.maxX >> 4; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            sub.minX = 16 * x <= rect.minX ? rect.minX : 16 * x;
            sub.maxX = 16 * x + 15 >= rect.maxX ? rect.maxX : 16 * x + 15;
            map->ShadowQueryTerrain(query, data, sub, startBand, endBand);
        }
    }
}

// OFFSET: 0x0079E4B0/0x1404935A0
void CWorldMap::ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, int32_t band) {
    CWorldMap::s_queryTag++;
    CiRect rect(0, 0, 0, 0);
    CWorldMap::GetChunkRect(data.m_extents[band], rect);
    CiRect sub(0, 0, 0, 0);

    for (int32_t y = rect.minY >> 4; y <= rect.maxY >> 4; y++) {
        sub.minY = 16 * y <= rect.minY ? rect.minY : 16 * y;
        sub.maxY = 16 * y + 15 >= rect.maxY ? rect.maxY : 16 * y + 15;

        for (int32_t x = rect.minX >> 4; x <= rect.maxX >> 4; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            sub.minX = 16 * x <= rect.minX ? rect.minX : 16 * x;
            sub.maxX = 16 * x + 15 >= rect.maxX ? rect.maxX : 16 * x + 15;
            map->ShadowQueryTerrain(query, data, sub, band);
        }
    }
}

// OFFSET: 0x0079E610/0x140493780
void CWorldMap::ShadowQueryMapObjDefs(CShadowQuery& query, CShadowCacheQueryData& data, int32_t startBand, int32_t endBand) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if (!(mapObjDef->m_flags & 0x10)) {
            CWorldMap::ShadowQueryMapObjDef(mapObjDef, query, data, startBand, endBand);
        }
    }
}

// OFFSET: 0x0079E670/0x140493810
void CWorldMap::ShadowQueryMapObjDefs(CShadowQuery& query, CShadowCacheQueryData& data, int32_t band) {
    float minRadius;
    float maxRadius;
    CShadowQuery::GetSize(data, band, minRadius, maxRadius);

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if (!(mapObjDef->m_flags & 0x10)) {
            CWorldMap::ShadowQueryMapObjDef(mapObjDef, query, data, band, maxRadius);
        }
    }
}

// OFFSET: 0x0079F670/0x140494D90
void CMap::ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, CiRect& rect, int32_t startBand, int32_t endBand) {
    CMapAreaTableEntry* entry = this->m_areaTable.Get((rect.minX >> 4) & 0x3F, (rect.minY >> 4) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2)) {
        return;
    }

    CMapArea* area = entry->m_area;

    if (!area) {
        return;
    }

    for (CMapBaseObjLink* link = area->m_linkList.Head(), * next; link; link = next) {
        next = area->m_linkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->unk_004C != CWorldMap::s_queryTag) {
            mapObjDef->unk_004C = CWorldMap::s_queryTag;
            CWorldMap::ShadowQueryMapObjDef(mapObjDef, query, data, startBand, endBand);
        }
    }

    if (!data.unk_04EC[endBand].Cull(area->m_bbox)) {
        return;
    }

    const C3Vector& camPos = query.m_worldView->m_camPos;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            uint32_t index = 16 * (y & 0xF) + (x & 0xF);
            CMapChunk* chunk = area->m_chunks[index];

            if (!chunk) {
                continue;
            }

            const CAaBox& box = area->m_chunkBounds[index];

            if (!data.m_extents[endBand].Intersects(box) || !data.unk_04EC[endBand].Cull(box) || query.m_occlusion->QueryVolumes(box)) {
                continue;
            }

            uint32_t sorted = this->m_header.m_flags & 0x8;
            float px = box.b.x;

            if (box.b.x <= camPos.x) {
                px = box.t.x;

                if (camPos.x <= box.t.x) {
                    px = camPos.x;
                }
            }

            float py = box.b.y;

            if (py <= camPos.y) {
                py = box.t.y;

                if (camPos.y <= py) {
                    py = camPos.y;
                }
            }

            float pz = box.b.z;

            if (pz <= camPos.z) {
                pz = box.t.z;

                if (camPos.z <= pz) {
                    pz = camPos.z;
                }
            }

            uint32_t minSizeCat = CWorldView::CalcMinSizeCatFromDistSqr((pz - camPos.z) * (pz - camPos.z) + (py - camPos.y) * (py - camPos.y) + (px - camPos.x) * (px - camPos.x));

            for (CMapBaseObjLink* link = chunk->m_doodadDefLinkList.Head(), * next; link; link = next) {
                next = chunk->m_doodadDefLinkList.Next(link);
                CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

                if (!(doodadDef->m_flags & 0x40)) {
                    continue;
                }

                if (sorted && doodadDef->m_sizeCategory < minSizeCat) {
                    break;
                }

                if (!((doodadDef->m_sphere.c.z - camPos.z) * (doodadDef->m_sphere.c.z - camPos.z) + (doodadDef->m_sphere.c.y - camPos.y) * (doodadDef->m_sphere.c.y - camPos.y) + (doodadDef->m_sphere.c.x - camPos.x) * (doodadDef->m_sphere.c.x - camPos.x) <= CWorldView::s_fadeInnerSqr[doodadDef->m_sizeCategory])) {
                    continue;
                }

                for (int32_t band = startBand; band <= endBand; band++) {
                    if (doodadDef->m_shadowFrame[band] == query.m_frame) {
                        continue;
                    }

                    doodadDef->m_shadowFrame[band] = query.m_frame;

                    if (data.m_extents[band].Intersects(doodadDef->m_bbox) && data.unk_04EC[band].Cull(doodadDef->m_bbox) && (!data.m_useCullFrustum[band] || data.m_cullFrustums[band].CullInv(doodadDef->m_bbox) != 3)) {
                        doodadDef->m_model->GatherModelBatchesShadowMap(query.m_queries[band].m_untexturedBatches, query.m_queries[band].m_texturedBatches);
                    }
                }
            }
        }
    }
}

// OFFSET: 0x0079FAD0/0x140495420
void CMap::ShadowQueryTerrain(CShadowQuery& query, CShadowCacheQueryData& data, CiRect& rect, int32_t band) {
    float minRadius;
    float maxRadius;
    CShadowQuery::GetSize(data, band, minRadius, maxRadius);
    CMapAreaTableEntry* entry = this->m_areaTable.Get((rect.minX >> 4) & 0x3F, (rect.minY >> 4) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2)) {
        return;
    }

    CMapArea* area = entry->m_area;

    if (!area) {
        return;
    }

    for (CMapBaseObjLink* link = area->m_linkList.Head(), * next; link; link = next) {
        next = area->m_linkList.Next(link);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->unk_004C != CWorldMap::s_queryTag) {
            mapObjDef->unk_004C = CWorldMap::s_queryTag;
            CWorldMap::ShadowQueryMapObjDef(mapObjDef, query, data, band, maxRadius);
        }
    }

    const C3Vector& camPos = query.m_worldView->m_camPos;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            uint32_t index = 16 * (y & 0xF) + (x & 0xF);
            CMapChunk* chunk = area->m_chunks[index];

            if (!chunk) {
                continue;
            }

            const CAaBox& box = area->m_chunkBounds[index];

            if (!data.m_extents[band].Intersects(box) || !data.unk_04EC[band].Cull(box) || (data.m_useCullFrustum[band] && data.m_cullFrustums[band].CullInv(box) == 3)) {
                continue;
            }

            for (CMapBaseObjLink* link = chunk->m_doodadDefLinkList.Head(), * next; link; link = next) {
                next = chunk->m_doodadDefLinkList.Next(link);
                CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

                if (doodadDef->m_shadowFrame[band] == query.m_frame) {
                    continue;
                }

                doodadDef->m_shadowFrame[band] = query.m_frame;

                if (!(doodadDef->m_flags & 0x40)) {
                    continue;
                }

                if (!((doodadDef->m_sphere.c.z - camPos.z) * (doodadDef->m_sphere.c.z - camPos.z) + (doodadDef->m_sphere.c.y - camPos.y) * (doodadDef->m_sphere.c.y - camPos.y) + (doodadDef->m_sphere.c.x - camPos.x) * (doodadDef->m_sphere.c.x - camPos.x) <= CWorldView::s_fadeInnerSqr[doodadDef->m_sizeCategory]) && !(doodadDef->m_sphere.r > maxRadius)) {
                    continue;
                }

                uint32_t wanted = (doodadDef->m_staticFlags & 0x4) ? data.m_queryFlags[band] & 0x10 : data.m_queryFlags[band] & 0x8;

                if (wanted && data.m_extents[band].Intersects(doodadDef->m_bbox) && data.unk_04EC[band].Cull(doodadDef->m_bbox) && (!data.m_useCullFrustum[band] || data.m_cullFrustums[band].CullInv(doodadDef->m_bbox) != 3)) {
                    doodadDef->m_model->GatherModelBatchesShadowMap(query.m_queries[band].m_untexturedBatches, query.m_queries[band].m_texturedBatches);
                }
            }
        }
    }
}
