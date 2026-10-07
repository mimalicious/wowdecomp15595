#ifndef WOW_SOURCE_WORLDCLIENT_SORTTABLE_H
#define WOW_SOURCE_WORLDCLIENT_SORTTABLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/WorldOcclusion.h"

class CMapAreaLow;
class CMapChunk;
class CMapDoodadDef;
class CMapEntity;
class CMapObjDef;
class CMapObjDefGroup;
class CSortTable;
class CWorldOccluder;

class CSortEntryChunk {
    public:
    /* 0000 */ CAaBox m_box;
    /* 0018 */ float m_minZ;
    /* 001C */ float m_maxZ;
    /* 0020 */ uint16_t m_flags;
    /* 0024 */ CMapChunk* m_chunk;
};

class CSortEntryDoodadDef {
    public:
    /* 0000 */ CMapDoodadDef* m_doodadDef;
    /* 0004 */ int32_t m_next;
};

class CSortEntry {
    public:
    /* 0000 */ CSortTable* m_table;
    /* 0004 */ C4Plane m_plane;
    /* 0014 */ TSGrowableArray<CSortEntryChunk> m_chunks;
    /* 0024 */ TSGrowableArray<CMapAreaLow*> m_lowDetailAreas;
    /* 0034 */ TSGrowableArray<CMapObjDef*> m_lowDetailMapObjDefs;
    /* 0044 */ TSGrowableArray<CMapAreaLow*> m_lowDetailOceans;
    /* 0054 */ TSExplicitList<CMapObjDefGroup, offsetof(CMapObjDefGroup, m_moveableLink)> m_mapObjDefGroups;
    /* 0060 */ TSExplicitList<CMapEntity, offsetof(CMapEntity, m_viewLink)> m_entities;
    /* 006C */ int32_t m_doodadDefHead;
    /* 0070 */ TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> m_occluders;
    /* 007C */ TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> m_occluders2;

    CSortEntry();
    ~CSortEntry();
};

class CSortTable {
    public:
    /* 0000 */ TSGrowableArray<CSortEntry> m_entries;
    /* 0010 */ TSGrowableArray<CSortEntryDoodadDef> m_doodadDefs;
    /* 0020 */ int32_t m_numEntries;
    /* 0024 */ int32_t m_lastEntry;

    CSortTable();
    ~CSortTable();
    void AddDoodadDef(CMapDoodadDef* doodadDef, int32_t entry);
    void Clear(TSExplicitList<CMapEntity, offsetof(CMapEntity, m_viewLink)>& entities);
    void Finish();
    void SetPlanes(const C3Vector& normal, const C3Vector& pos);
    void Setup(float farClip, const C3Vector& normal, const C3Vector& pos);
};

#endif
