#ifndef WOW_SOURCE_WORLDCLIENT_MAPLOWDETAIL_H
#define WOW_SOURCE_WORLDCLIENT_MAPLOWDETAIL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/RectArray.h"

class CMap;
class CMapObjDef;
class CWorldMap;
struct SMMapObjDef;

class CMapLowDetail {
    public:
    struct CMapAreaLowTableEntry {
        /* 0000 */ CMapAreaLow* m_area;

        CMapAreaLowTableEntry();
    };

    /* 0000 */ CMap* m_map;
    /* 0004 */ CWorldMap* m_worldMap;
    /* 0008 */ uint8_t* m_data;
    /* 000C */ const char* m_mapObjNames;
    /* 0010 */ const uint32_t* m_mapObjNameOffsets;
    /* 0014 */ const SMMapObjDef* m_mapObjDefs;
    /* 0018 */ uint32_t m_mapObjDefCount;
    /* 001C */ const uint32_t* m_areaOffsets;
    /* 0020 */ TSRectArray<CMapAreaLowTableEntry> m_areaTable;
    /* 0048 */ TSGrowableArray<CMapObjDef*> m_lowDetailMapObjDefs;
    /* 0058 */ TSExplicitList<CMapAreaLow, offsetof(CMapAreaLow, m_lowDetailLink)> m_areaList;

    CMapLowDetail();
    ~CMapLowDetail();
    CMapAreaLow* CreateOcean(int32_t x, int32_t y);
    void LinkMapObjDefToAreas(CMapObjDef* mapObjDef);
    int32_t Load(CMap* map, const char* mapPath, const char* mapName);
    void Purge();
    void Unload();
};

#include "WoW/Source/WorldClient/MapLowDetail.inl"


#endif
