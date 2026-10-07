#ifndef WOW_SOURCE_WORLDCLIENT_WORLDMAP_H
#define WOW_SOURCE_WORLDCLIENT_WORLDMAP_H

#include <cstdint>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/WorldClient/MapChunkBuf.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/RectArray.h"
#include "WoW/Source/WorldClient/World.h"

class C2Vector;
class C3Ray;
class C44Matrix;
class CFrustum;
class CM2Model;
class CMap;
class CMapBaseObj;
class CMapChunk;
class CMapChunkBuf;
class CMapDoodadDef;
class CMapEntity;
class CMapObj;
class CMapObjDef;
class CMapObjDefGroup;
class CMapStaticEntity;
struct CShadowCacheQueryData;
class CShadowQuery;
class CWorldView;
class Particulate;
class Weather;
struct MapObjIntersectData;
struct SMDoodadDef;
struct SMMapObjDef;
struct SMODoodadDef;
namespace Liquid { class CInstance; }

class CWorldMap {
    public:
    struct CMapTableEntry {
        /* 0000 */ uint8_t m_flags;
        /* 0001 */ uint8_t m_mapIndex;

        CMapTableEntry();
    };

    static HASHKEY_NONE s_mapObjDefKey;
    static CWorldMap* s_worldMaps[2];
    static uint32_t s_activeWorldMapIndex;
    static uint32_t s_queryTag;
    static CM2Light s_sunLight;
    static float s_oceanDepthScale;
    static float s_oceanDepthCoordTable[256];
    static float s_riverDepthCoordTable[256];
    static float s_emptyDepthCoordTable[256];
    static float* s_depthCoordTables[2];

    /* 0000 */ int32_t m_mapId;
    /* 0004 */ int32_t m_isGlobalMapObj;
    /* 0008 */ int32_t m_facetQueryValid;
    /* 000C */ CMap* m_maps[12];
    /* 003C */ TSRectArray<CMapTableEntry> m_mapTable;
    /* 0064 */ TSHashTable<CMapDoodadDef, HASHKEY_DWORD> m_doodadDefHash;
    /* 0090 */ TSHashTable<CMapObjDef, HASHKEY_NONE> m_mapObjDefHash;
    /* 00BC */ TSExplicitList<CMapEntity, offsetof(CMapEntity, m_sortLink)> m_entityList;
    /* 00C8 */ TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, m_link)> m_list00C8;
    /* 00D4 */ TSExplicitList<CMapBaseObj, 0x10> m_list00D4;
    /* 00E0 */ TSExplicitList<CMapObjDef, offsetof(CMapObjDef, m_link30)> m_mapObjDefList;
    /* 00EC */ TSExplicitList<CMapObjDef, offsetof(CMapObjDef, m_link30)> m_list00EC;
    /* 00F8 */ TSExplicitList<CMapBaseObj, 0x18> m_list00F8;
    /* 0104 */ Particulate* m_particulate;
    /* 0108 */ Weather* m_weather;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ int32_t m_exteriorUnderwater;

    static void ClearAllChunkBufs();
    static const float* GetDepthCoordTable(uint32_t liquidType);
    static void ClearAllRenderChunks();
    static void ClearAllDetailDoodads();
    static void ClearAllFootprints();
    static void Destroy();
    static void FootprintDestroy();
    static void FootprintInitialize();
    static void Gather(CWorldView* view);
    static void Initialize();
    static void GetChunkRect(const CAaBox& box, CiRect& rect);
    static void GetLowDetailAreaRect(const C3Vector& pos, CiRect& rect);
    static void InitializeLiquid();
    static int32_t LoadWorldMap(const char* directory, int32_t mapId, int32_t preload);

    static void UnloadAllWorldMaps();
    static void UpdateAllEnvironments();
    static void UpdateAllMaps(int32_t waitLoad);

    CWorldMap();
    ~CWorldMap();
    void AddChildMaps(int32_t mapId);
    int32_t AddMap(const char* directory, int32_t mapId, int32_t preload);
    uintptr_t AddEntity(CM2Model* model, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a9);
    uintptr_t AddEntity(const char* name, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* a8, int32_t a9);
    CMapObjDef* CreateMapObjDef(const char* name, C3Vector& pos, float facing, int32_t a4, int32_t a5, uint16_t* a6, float* a7, int32_t a8);
    CMapDoodadDef* CreateDoodadDef(const char* name, SMDoodadDef& def, C3Vector& offset);
    CMapObjDef* CreateMapObjDef(const char* name, SMMapObjDef& def, C3Vector& offset, int32_t a4);
    void CreateWaterRipple(const C3Vector& pos, float a2, float a3, float a4, float a5, float a6, int32_t a7, int32_t a8);
    void DestroyLiquidPerturbators();
    void GatherAreas(CWorldView* view);
    void GatherEntitys(CWorldView* view);
    void GatherLowDetail(CWorldView* view);
    void GatherMapObjDefs(CWorldView* view);
    int32_t GetActiveMapRecId(int32_t x, int32_t y);
    uint8_t GetCurrentMapIndex(int32_t x, int32_t y);
    int32_t Load(const char* directory, int32_t mapId, int32_t preload);
    int32_t LocateViewerMapObjDefs(CMapChunk* chunk, const C3Vector& start, const C3Vector& end, float t, CMapObjDef** mapObjDefs, uint32_t (*groups)[2]);
    void LocateViewer(const C3Vector& pos, int32_t& exterior, CMapChunk*& chunk, int32_t& interior, CMapObjDef** mapObjDefs, uint32_t (*groups)[2]);
    void NewFootprint(uint32_t a1, const C2Vector& a2, const C3Vector& a3, float a4, int32_t a5, uint32_t a6, int32_t a7);
    void Preload(int32_t a1);
    void PurgeMaps();
    int32_t QueryAreaId(CMapEntity* entity, uint32_t& areaId);
    int32_t QueryFlightBounds(const C3Vector& pos, float& z, int32_t lower);
    int32_t QueryIntersectingModels(CM2Model** models, uint32_t maxModels, const CAaBox& box, uint32_t flags);
    bool QueryFacets(const CFrustum& frustum, World::FacetData& facets, uint32_t flags);
    bool QueryLiquid(const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height);
    int32_t QueryFlightBoundsUpper(const C3Vector& pos, float& height);
    int32_t QueryLiquidTerrain(const C3Vector& pos, uint32_t& liquidType, float& height);
    void QueryLiquidSounds(const C3Vector& pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results);
    int32_t QueryOutdoors(CMapEntity* entity);
    void QueryTris(const CAaBox& box, World::TriData& tris, uint32_t flags);
    void RemoveMap(int32_t mapId);
    void RemoveOtherMaps();
    void RenderFlightBounds(const C3Vector& pos);
    void RenderFootprints(CWorldView* view);
    void RenderWaterRipples(CWorldView* view);
    void SetCurrent(int32_t mapId, int32_t current);
    void Unload();
    void Update(int32_t waitLoad);
    void UpdateAreas();
    void UpdateBarriers();
    void UpdateEntityLinks(const CAaBox& box);
    void UpdateEntitys();
    void UpdateLowDetailMapObjDefs();
    void UpdateMapObjDefGroups(CMapObj* mapObj, CMapObjDef* mapObjDef, int32_t waitLoad);
    void UpdateMapObjDefs(int32_t waitLoad);
    void UpdateMaps(int32_t preload);
    void UpdateStreaming();
    void UpdateStreaming(CMapChunk* chunk);
    void UpdateRipples(const C3Vector& pos);
    bool VectorIntersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist, uint32_t flags, World::HitTestDebug* debug);
    void WaterReset();
};

class CWaterRipple {
    public:
    static void Destroy();
    static void Initialize();
};

#include "WoW/Source/WorldClient/WorldMap.inl"

#endif
