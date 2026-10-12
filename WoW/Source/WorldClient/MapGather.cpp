#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x0079F340
void CMap::GatherMapObjDefs(CWorldView* view, CMapArea* area) {
    for (CMapBaseObjLink* link = area->m_linkList.Head(); link; link = area->m_linkList.Next(link)) {
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->m_gatherStamp == view->m_viewStamp) {
            continue;
        }

        mapObjDef->m_gatherStamp = view->m_viewStamp;

        if (!(mapObjDef->m_flags & 0x10) && (mapObjDef->m_flags & 0x40) && (mapObjDef->m_mapObj->m_flags & 0x40)) {
            if (!view->CullModel(mapObjDef->m_bbox)) {
                view->AddMapObjDef(mapObjDef);
            }
        }
    }
}

// OFFSET: 0x0079F3C0/0x140494870
void CMap::GatherLowDetail(CWorldView* view, int32_t x, int32_t y) {
    CMapLowDetail::CMapAreaLowTableEntry* entry = this->m_lowDetail.m_areaTable.Get(x, y);

    if (!entry) {
        return;
    }

    CMapAreaLow* area = entry->m_area;

    if (!area) {
        return;
    }

    if (!(this->m_mapFlags & 0x80)) {
        view->AddLowDetailMapArea(area);
    }

    for (CMapBaseObjLink* link = area->m_linkList.Head(); link; link = area->m_linkList.Next(link)) {
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

        if (mapObjDef->m_gatherStamp == view->m_viewStamp) {
            continue;
        }

        mapObjDef->m_gatherStamp = view->m_viewStamp;

        if ((mapObjDef->m_flags & 0x40) && (mapObjDef->m_mapObj->m_flags & 0x40)) {
            view->AddLowDetailMapObjDef(mapObjDef);
        }
    }
}

// PROVEN: 9ce49674 IDENTICAL/DIFFERENT
// OFFSET: 0x0079F450/0x140494990
void CMap::GatherInfiniteOcean(CWorldView* view, int32_t x, int32_t y) {
    CMapLowDetail::CMapAreaLowTableEntry* entry = this->m_lowDetail.m_areaTable.Get(x, y);

    if (entry && entry->m_area) {
        view->AddLowDetailMapAreaOcean(entry->m_area);
        return;
    }

    CMapAreaLow* area = this->m_lowDetail.CreateOcean(x, y);

    if (area) {
        view->AddLowDetailMapAreaOcean(area);
    }
}

// OFFSET: 0x0079F4A0
void CMap::Gather(CWorldView* view, const CiRect& rect) {
    int32_t areaY = (rect.minY >> 4) & 0x3F;
    int32_t areaX = (rect.minX >> 4) & 0x3F;
    CMapAreaTableEntry* entry = this->m_areaTable.Get(areaX, areaY);

    if (!entry || !(entry->m_flags & 0x2)) {
        return;
    }

    CMapArea* area = entry->m_area;

    if (!area) {
        return;
    }

    int32_t minY = rect.minY & 0xF;
    int32_t minX = rect.minX & 0xF;
    int32_t maxY = rect.maxY & 0xF;
    int32_t maxX = rect.maxX & 0xF;
    int32_t flag = area->m_map->m_header.m_flags & 0x20;

    CAaBox box;
    box.b.z = area->m_bbox.b.z;
    box.t.z = area->m_bbox.t.z;
    box.t.x = area->m_bbox.t.x - static_cast<float>(minY) * 33.333332f;
    box.b.x = box.t.x - 33.333332f;

    for (int32_t y = minY; y <= maxY; y++) {
        box.t.y = area->m_bbox.t.y - static_cast<float>(minX) * 33.333332f;
        box.b.y = box.t.y - 33.333332f;

        for (int32_t x = minX; x <= maxX; x++) {
            if (!view->CullModel(box, 0x3E)) {
                CMapChunk* chunk = area->m_chunks[16 * y + x];

                if (chunk) {
                    view->AddMapChunk(chunk, flag);
                }
            }

            box.t.y = box.t.y - 33.333332f;
            box.b.y = box.b.y - 33.333332f;
        }

        box.t.x = box.t.x - 33.333332f;
        box.b.x = box.b.x - 33.333332f;
    }

    this->GatherMapObjDefs(view, area);
}
