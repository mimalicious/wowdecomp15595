#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// OFFSET: 0x00798000/0x14048BA10
void CMapLowDetail::LinkMapObjDefToAreas(CMapObjDef* mapObjDef) {
    int32_t minY = static_cast<int32_t>(-(mapObjDef->m_bbox.t.x - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxY = static_cast<int32_t>(-(mapObjDef->m_bbox.b.x - 17066.666f) * 0.029999999f) >> 4;
    int32_t minX = static_cast<int32_t>(-(mapObjDef->m_bbox.t.y - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxX = static_cast<int32_t>(-(mapObjDef->m_bbox.b.y - 17066.666f) * 0.029999999f) >> 4;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMapAreaLowTableEntry* entry = this->m_areaTable.Get(x, y);

            if (entry && entry->m_area) {
                CMapAreaLow* areaLow = entry->m_area;
                CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDef, areaLow);
                mapObjDef->m_parentLinkList.LinkToTail(link);
                areaLow->m_linkList.LinkToTail(link);
            }
        }
    }
}

// PROVEN: a54c4b04 IDENTICAL/DIFFERENT
// OFFSET: 0x00797F40/0x14048B890
CMapAreaLow* CMapLowDetail::CreateOcean(int32_t x, int32_t y) {
    CMapAreaLow* area = NULL;
    CMapAreaLowTableEntry* entry = this->m_areaTable.Get(x, y);

    if (entry) {
        area = CMap::AllocAreaLow();
        entry->m_area = area;
        area->Create(x, y, NULL, NULL);
        this->m_areaList.LinkToTail(area);
    }

    return area;
}

// PROVEN: 218a44ed IDENTICAL/DIFFERENT
// OFFSET: 0x00797F90/0x14048B980
void CMapLowDetail::Purge() {
    for (int32_t y = 0; y < 64; y++) {
        for (int32_t x = 0; x < 64; x++) {
            CMapAreaLowTableEntry* entry = this->m_areaTable.Get(x, y);

            if (entry && entry->m_area) {
                entry->m_area->FreeBuf();
            }
        }
    }
}

// OFFSET: 0x00798250/0x14048BC80
int32_t CMapLowDetail::Load(CMap* map, const char* mapPath, const char* mapName) {
    this->m_map = map;
    this->m_worldMap = map->m_worldMap;

    SFileImpl* file = NULL;
    char fileName[256];
    SStrPrintf(fileName, sizeof(fileName), "%s\\%s.wdl", mapPath, mapName);

    if (!CMap::SafeOpen(fileName, &file, 0)) {
        World::SetLoadFailed(1, fileName);
        return 0;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

    CAsyncObject* object;
    AsyncFileReadCreateObject(file, 3, 1, &object, this->m_data, NULL, NULL, NULL, 0, 0, 0);

    while (!object->m_isProcessed) {
        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.0f, World::s_loadProgressParam);
        }

        AsyncFileReadPollHandler(NULL, NULL);
        OsSleep(10);
    }

    int32_t succeeded = object->m_readSucceeded;
    int32_t cancelled = object->m_cancelled;
    AsyncFileReadDestroyObject(object);

    if (!succeeded) {
        if (!cancelled) {
            AsyncFileReadSetStreamingError();
        }

        World::SetLoadFailed(1, fileName);
        return 0;
    }

    uint32_t* chunk = reinterpret_cast<uint32_t*>(&this->m_data[reinterpret_cast<uint32_t*>(this->m_data)[1] + 8]);
    uint8_t* chunkData = reinterpret_cast<uint8_t*>(&chunk[2]);

    if (chunk[0] == 'MWMO') {
        this->m_mapObjNames = reinterpret_cast<const char*>(chunkData);
        uint32_t* mwid = reinterpret_cast<uint32_t*>(&chunkData[chunk[1]]);
        this->m_mapObjNameOffsets = &mwid[2];
        uint32_t* modf = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(&mwid[2]) + mwid[1]);
        this->m_mapObjDefs = reinterpret_cast<const SMMapObjDef*>(&modf[2]);
        this->m_mapObjDefCount = modf[1] >> 6;
        chunkData = reinterpret_cast<uint8_t*>(&modf[2]) + modf[1] + 8;
    }

    this->m_areaOffsets = reinterpret_cast<const uint32_t*>(chunkData);

    CiRect rect;

    if (map->m_mapFlags & 0x20) {
        rect.minX = 0;
        rect.maxX = 63;
        rect.minY = 0;
        rect.maxY = 63;
        this->m_areaTable.SetRect(rect);
    } else {
        int32_t minX = 0xFFFF;
        int32_t maxX = 0;
        int32_t minY = 0xFFFF;
        int32_t maxY = 0;
        const uint32_t* offset = this->m_areaOffsets;

        for (int32_t y = 0; y < 64; y++) {
            for (int32_t x = 0; x < 64; x++) {
                if (*offset) {
                    if (x < minX) {
                        minX = x;
                    }

                    if (x > maxX) {
                        maxX = x;
                    }

                    if (y < minY) {
                        minY = y;
                    }

                    if (y > maxY) {
                        maxY = y;
                    }
                }

                offset++;
            }
        }

        rect.maxY = maxY;
        rect.minX = minX;
        rect.minY = minY;
        rect.maxX = maxX;
        this->m_areaTable.SetRect(rect);
    }

    uint32_t index = 0;

    for (int32_t y = 0; y < 64; y++) {
        for (int32_t x = 0; x < 64; x++, index++) {
            if (!this->m_areaOffsets[index]) {
                continue;
            }

            uint32_t* mare = reinterpret_cast<uint32_t*>(&this->m_data[this->m_areaOffsets[index]]);
            const int16_t* heights = reinterpret_cast<const int16_t*>(&mare[2]);
            const uint16_t* holes = NULL;
            uint32_t* next = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(&mare[2]) + mare[1]);

            if (next[0] == 'MAHO') {
                holes = reinterpret_cast<const uint16_t*>(&next[2]);
            }

            CMapAreaLow* areaLow = CMap::AllocAreaLow();
            this->m_areaTable.Get(x, y)->m_area = areaLow;
            areaLow->Create(x, y, heights, holes);
            this->m_areaList.LinkToTail(areaLow);
        }
    }

    C3Vector offset(17066.666f, 17066.666f, 0.0f);
    this->m_lowDetailMapObjDefs.SetCount(this->m_mapObjDefCount);

    for (uint32_t i = 0; i < this->m_mapObjDefCount; i++) {
        SMMapObjDef& def = const_cast<SMMapObjDef&>(this->m_mapObjDefs[i]);
        CMapObjDef* mapObjDef = this->m_worldMap->CreateMapObjDef(&this->m_mapObjNames[this->m_mapObjNameOffsets[def.m_nameId]], def, offset, 0);
        mapObjDef->m_refCount++;
        this->m_lowDetailMapObjDefs.m_data[i] = mapObjDef;
        this->LinkMapObjDefToAreas(mapObjDef);
    }

    return 1;
}

// OFFSET: 0x00798710/0x14048C240
void CMapLowDetail::Unload() {
    for (int32_t y = 0; y < 64; y++) {
        for (int32_t x = 0; x < 64; x++) {
            CMapAreaLowTableEntry* entry = this->m_areaTable.Get(x, y);

            if (entry && entry->m_area) {
                CMap::FreeAreaLow(entry->m_area);
                entry = this->m_areaTable.Get(x, y);

                if (entry) {
                    entry->m_area = NULL;
                }
            }
        }
    }

    this->m_areaTable.m_rect.minY = 0;
    this->m_areaTable.m_rect.minX = 0;
    this->m_areaTable.m_height = 0;
    this->m_areaTable.m_rect.maxY = 0;
    this->m_areaTable.m_rect.maxX = 0;
    this->m_areaTable.m_width = 0;
    this->m_areaTable.m_data.SetCount(0);

    for (uint32_t i = 0; i < this->m_lowDetailMapObjDefs.m_count; i++) {
        CMapObjDef* mapObjDef = this->m_lowDetailMapObjDefs.m_data[i];
        mapObjDef->m_refCount--;
        mapObjDef->Purge(0);
    }

    this->m_lowDetailMapObjDefs.m_count = 0;
    this->m_mapObjNames = NULL;
    this->m_mapObjNameOffsets = NULL;
    this->m_mapObjDefs = NULL;
    this->m_mapObjDefCount = 0;
    this->m_areaOffsets = NULL;

    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
        this->m_data = NULL;
    }
}

// PROVEN: 78dafdd9 IDENTICAL/DIFFERENT
// OFFSET: 0x00798840/0x14048C3E0
CMapLowDetail::~CMapLowDetail() {
}

// OFFSET: 0x007988D0/0x14048C4B0
CMapLowDetail::CMapLowDetail() {
    this->m_map = NULL;
    this->m_worldMap = NULL;
    this->m_data = NULL;
    this->m_mapObjNames = NULL;
    this->m_mapObjNameOffsets = NULL;
    this->m_mapObjDefs = NULL;
    this->m_mapObjDefCount = 0;
    this->m_areaOffsets = NULL;
}
