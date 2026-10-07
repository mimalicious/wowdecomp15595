#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x0079A640
void CWorldMap::GetChunkRect(const CAaBox& box, CiRect& rect) {
    float maxY = -(box.t.y - 17066.666f);
    float maxX = -(box.t.x - 17066.666f);
    float minX = -(box.b.x - 17066.666f);
    float minY = -(box.b.y - 17066.666f);

    if (maxY < 0.0f) {
        maxY = 0.0f;
    }

    if (maxX < 0.0f) {
        maxX = 0.0f;
    }

    if (minY >= 34133.332f) {
        minY = 34132.332f;
    }

    if (minX >= 34133.332f) {
        minX = 34132.332f;
    }

    rect.maxX = static_cast<int32_t>(minY * 0.23999999f) >> 3;
    rect.minY = static_cast<int32_t>(maxX * 0.23999999f) >> 3;
    rect.minX = static_cast<int32_t>(maxY * 0.23999999f) >> 3;
    rect.maxY = static_cast<int32_t>(minX * 0.23999999f) >> 3;
}

// OFFSET: 0x0079A700
void CWorldMap::GatherMapObjDefs(CWorldView* view) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(); mapObjDef; mapObjDef = this->m_mapObjDefList.Next(mapObjDef)) {
        if (!(mapObjDef->m_flags & 0x10) && (mapObjDef->m_flags & 0x40) && (mapObjDef->m_mapObj->m_flags & 0x40)) {
            view->AddMapObjDef(mapObjDef);
        }
    }
}

// OFFSET: 0x0079A760
void CWorldMap::GatherEntitys(CWorldView* view) {
    for (CMapEntity* entity = this->m_entityList.Head(); entity; entity = this->m_entityList.Next(entity)) {
        if (!(entity->m_flags & 0x10010)) {
            view->AddMapEntity(entity);
        }
    }
}

// OFFSET: 0x0079A7B0
void CWorldMap::GatherAreas(CWorldView* view) {
    CiRect rect(0, 0, 0, 0);
    CWorldMap::GetChunkRect(view->m_frustumBounds, rect);

    int32_t areaMinY = rect.minY >> 4;
    int32_t areaMinX = rect.minX >> 4;
    int32_t areaMaxY = rect.maxY >> 4;
    int32_t areaMaxX = rect.maxX >> 4;
    CiRect chunkRect(0, 0, 0, 0);

    for (int32_t y = areaMinY; y <= areaMaxY; y++) {
        chunkRect.minY = 16 * y <= rect.minY ? rect.minY : 16 * y;
        chunkRect.maxY = 16 * y + 15 >= rect.maxY ? rect.maxY : 16 * y + 15;

        for (int32_t x = areaMinX; x <= areaMaxX; x++) {
            CMap* map = NULL;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (map) {
                chunkRect.minX = 16 * x <= rect.minX ? rect.minX : 16 * x;
                chunkRect.maxX = 16 * x + 15 >= rect.maxX ? rect.maxX : 16 * x + 15;
                map->Gather(view, chunkRect);
            }
        }
    }
}

// OFFSET: 0x0079A900
void CWorldMap::GatherLowDetail(CWorldView* view) {
    CiRect rect(0, 0, 0, 0);
    CWorldMap::GetLowDetailAreaRect(view->m_camPos, rect);

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            CMap* map = NULL;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (entry && (entry->m_flags & 0x1)) {
                    map = this->m_maps[entry->m_mapIndex];
                }
            }

            if (map) {
                map->GatherLowDetail(view, x, y);
            } else if (this->m_maps[0] && (this->m_maps[0]->m_mapFlags & 0x20)) {
                this->m_maps[0]->GatherInfiniteOcean(view, x, y);
            }
        }
    }
}

// OFFSET: 0x0079A9F0
void CWorldMap::Gather(CWorldView* view) {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        return;
    }

    if (view->m_flags & 0x20000) {
        worldMap->GatherEntitys(view);
    }

    if (view->m_flags & 0x2000) {
        worldMap->GatherMapObjDefs(view);
    }

    if (view->m_flags & 0x1000) {
        worldMap->GatherAreas(view);
    }

    if (view->m_flags & 0x10000) {
        worldMap->GatherLowDetail(view);
    }
}
